/*HEADER****************************************************************
************************************************************************
***
*** Copyright (c) 1992-2003 ARC International
*** All rights reserved
***
*** This software embodies materials and concepts which are
*** confidential to ARC International and is made available solely
*** pursuant to the terms of a written license agreement with
*** ARC International.
***
*** File: ip-e.c
***
*** Comments:  This file contains the interface between IP and
***            the Ethernet packet driver.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "arp.h"
#include "ip-e.h"

//#define 	TEST_RDN

#ifdef 	TEST_RDN
extern  unsigned long crdn, carp, cip;
#endif //TEST_RDN




#define ENET_FRAMESIZE_MAXDATA   1500

#define IPE_ADDR_BC(addr)     \
            (addr)[0] = 0xFF, \
            (addr)[1] = 0xFF, \
            (addr)[2] = 0xFF, \
            (addr)[3] = 0xFF, \
            (addr)[4] = 0xFF, \
            (addr)[5] = 0xFF

#define IPE_ADDR_MC(addr,ip)  \
            (addr)[0] = 0x01, \
            (addr)[1] = 0x00, \
            (addr)[2] = 0x5E, \
            (addr)[3] = ((ip) >> 16) & 0x7F, \
            (addr)[4] = ((ip) >>  8) & 0xFF, \
            (addr)[5] =  (ip)        & 0xFF


/*
** Ethernet packet header
*/
typedef struct {
   uchar    DEST[6];
   uchar    SOURCE[6];
   uchar    TYPE[2];
} ENET_HEADER, _PTR_ ENET_HEADER_PTR;

typedef struct {
   uchar    TAG[2];
   uchar    TYPE[2];
} ENET_8021QTAG_HEADER, _PTR_ ENET_8021QTAG_HEADER_PTR;

typedef struct {
   uchar    DSAP[1];
   uchar    SSAP[1];
   uchar    COMMAND[1];
   uchar    OUI[3];
   uchar    TYPE[2];
} ENET_8022_HEADER, _PTR_ ENET_8022_HEADER_PTR;


/*
** The structure used by the application to bind an IP address
** to an Ethernet packet driver.
*/

//static RTCS_IF_STRUCT rtcs_enet = {
//   IPE_open,
//   IPE_close,
//   IPE_send_IP,
//   IPE_join,
//   IPE_leave
//};

//const RTCS_IF_STRUCT_PTR RTCS_IF_ENET = &rtcs_enet;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_open
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Registers IP and ARP with an Ethernet packet driver.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_open
   (
      IP_IF_PTR   if_ptr
         /* [IN] the IP interface structure */
   )
{ /* Body */
   uint_32  error;

   error = ARP_open(if_ptr);
   if (error) {
      return error;
   } /* Endif */

  error = RDN_open(if_ptr);
   if (error) {
      return error;
   } /* Endif */

   error = ENET_open(if_ptr->HANDLE, ENETPROT_IP, IPE_recv_IP, if_ptr);
   if (error) {
      ARP_close(if_ptr);
      return error;
   } /* Endif */

//______________> 
   error = ENET_open(if_ptr->HANDLE, ENETPROT_ARP, IPE_recv_ARP, if_ptr);
   if (error) {
      ENET_close(if_ptr->HANDLE, ENETPROT_IP);
      ARP_close(if_ptr);
      return error;
   } 



  error = ENET_open(if_ptr->HANDLE, ENETPROT_RDN, IPE_recv_RDN, if_ptr);
   if (error) {
      ENET_close(if_ptr->HANDLE, ENETPROT_IP);
      ARP_close(if_ptr); //t
      return error;
   } 
//_______________________________<
   if_ptr->MTU = ENET_FRAMESIZE_MAXDATA;

   if_ptr->DEV_TYPE    = 1;
   if_ptr->DEV_ADDRLEN = 6;
   ENET_get_address(if_ptr->HANDLE, if_ptr->DEV_ADDR);
   switch (ENET_get_speed(if_ptr->HANDLE)) {
   case 100:
      if_ptr->SNMP_IF_TYPE = IPIFTYPE_FASTETHERNET;
      break;
   case 1000:
      if_ptr->SNMP_IF_TYPE = IPIFTYPE_GIGABITETHERNET;
      break;
   default:
      if_ptr->SNMP_IF_TYPE = IPIFTYPE_ETHERNET;
      break;
   } /* Endswitch */

   /* Add some uniqueness to the seed */
   RTCS_rand_seed(if_ptr->DEV_ADDR[0]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[1]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[2]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[3]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[4]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[5]);

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_close
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Registers IP and ARP with an Ethernet packet driver.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_close
   (
      IP_IF_PTR   if_ptr
         /* [IN] the IP interface structure */
   )
{ /* Body */
   uint_32  firsterror, error;

   firsterror = RTCS_OK;

   error = ARP_close(if_ptr);
   if (error && !firsterror) {
      firsterror = error;
   } /* Endif */

   error = ENET_close(if_ptr->HANDLE, ENETPROT_IP);
   if (error && !firsterror) {
      firsterror = error;
   } /* Endif */

   error = ENET_close(if_ptr->HANDLE, ENETPROT_ARP);
   if (error && !firsterror) {
      firsterror = error;
   } /* Endif */

   return firsterror;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_recv1
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Services an Ethernet packet.
*
*END*-----------------------------------------------------------------*/

void IPE_recv1
   (
      PCB_PTR           pcb,
         /* [IN] the received packet */
      pointer           handle,
         /* [IN] the IP interface structure */
      void (_CODE_PTR_  service)(RTCSPCB_PTR)
         /* [IN] IP_service or ARP_service */
   )
{ /* Body */
   IP_IF_PTR            if_ptr = handle;
   RTCSPCB_PTR          packet;
   ENET_HEADER_PTR      enetpkt;
/* Start SPR P122-0266-37 remove all warnings from RTCS code. */
#if RTCSCFG_LINKOPT_8021Q_PRIO || RTCSCFG_LINKOPT_8023
   uchar_ptr            type_ptr;
#endif
   uint_32              hdrlen, error;
 // printf("\n\r IPE_recv");
   if_ptr->STATS.ST_RX_TOTAL++;
   if_ptr->STATS.ST_RX_OCTETS += pcb->FRAG[0].LENGTH;

  // ok return; //t 090330	

//ok 091210 return; //for test 091210

   packet = RTCSPCB_alloc_recv(pcb);

//ok  return; //t 090330	

    if (packet == NULL) 
//bad  if (1) //t090330 
    {
      if_ptr->STATS.ST_RX_MISSED++;
      PCB_free(pcb);
      return;
   } /* Endif */

  //after hier point do not work usart olwais    RTCSPCB_free(packet); return;   //t 090330

   RTCSLOG_PCB_ALLOC(packet);

//ok!  return; //t 090330

   enetpkt = (ENET_HEADER_PTR)RTCSPCB_DATA(packet);
   if (enetpkt->DEST[0] & 1) {
      if ((enetpkt->DEST[0] == 0xFF)
       && (enetpkt->DEST[1] == 0xFF)
       && (enetpkt->DEST[2] == 0xFF)
       && (enetpkt->DEST[3] == 0xFF)
       && (enetpkt->DEST[4] == 0xFF)
       && (enetpkt->DEST[5] == 0xFF)) {
         if_ptr->STATS.ST_RX_BROADCAST++;
         packet->TYPE = RTCSPCB_TYPE_BROADCAST;
      } else {
         if_ptr->STATS.ST_RX_MULTICAST++;
         packet->TYPE = RTCSPCB_TYPE_MULTICAST;
      } /* Endif */
   } else {
      if_ptr->STATS.ST_RX_UNICAST++;
      packet->TYPE = RTCSPCB_TYPE_UNICAST;
   } /* Endif */

#if RTCSCFG_LINKOPT_8021Q_PRIO || RTCSCFG_LINKOPT_8023
   type_ptr = enetpkt->TYPE;
#endif
   /* End SPR P122-0266-37. */
   hdrlen = sizeof(ENET_HEADER);

#if RTCSCFG_LINKOPT_8021Q_PRIO
   if (ntohs(type_ptr) == 0x8100) {
      packet->LINK_OPTIONS.RX.OPT_PRIO = 1;
      packet->LINK_OPTIONS.RX.PRIO = (ntohc(type_ptr+2) >> 5) & 0x7;
      hdrlen += sizeof(ENET_8021QTAG_HEADER);
      type_ptr += sizeof(ENET_8021QTAG_HEADER);
   } /* Endif */
#endif

#if RTCSCFG_LINKOPT_8023
   if (ntohs(type_ptr) < ENET_FRAMESIZE_MAXDATA) {
      packet->LINK_OPTIONS.RX.OPT_8023 = 1;
      hdrlen += sizeof(ENET_8022_HEADER);
      type_ptr += sizeof(ENET_8022_HEADER);
   } /* Endif */
#endif


   error = RTCSPCB_next(packet, hdrlen);
   if (error) {
      if_ptr->STATS.ST_RX_ERRORS++;
      RTCS_seterror(&if_ptr->STATS.ERR_RX, error, (uint_32)packet);
      RTCSLOG_PCB_FREE(packet, error);
      RTCSPCB_free(packet);
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(packet, RTCS_LOGCTRL_IFTYPE(IPIFTYPE_ETHERNET), 0);

   packet->LINK_OPTIONS.TX = packet->LINK_OPTIONS.RX;
   packet->IFSRC = handle;

 //printf("\n\r start RTCSCMD_service");

// ok return; //t 090330

//ok 091210    return; //t091210

  if (!RTCSCMD_service(packet, service)) {
// if (!RTCSCMD_service(packet, IP_service)) {  //t091211


//    return; //t 090331

      if_ptr->STATS.ST_RX_MISSED++;
      RTCSLOG_PCB_FREE(packet, RTCS_OK);
      RTCSPCB_free(packet);
   } /* Endif */

} /* Endbody */

  extern "C" void printfpd(char*, unsigned long);


unsigned long ipe_test_counter;

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_recv
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Services an Ethernet packet.
*
*END*-----------------------------------------------------------------*/

void IPE_recv
   (
      PCB_PTR           pcb,
         /* [IN] the received packet */
      pointer           handle,
         /* [IN] the IP interface structure */
      void (_CODE_PTR_  service)(RTCSPCB_PTR)
         /* [IN] IP_service or ARP_service */
   )
{ /* Body */
   IP_IF_PTR            if_ptr = handle;
   RTCSPCB_PTR          packet;
   ENET_HEADER_PTR      enetpkt;
/* Start SPR P122-0266-37 remove all warnings from RTCS code. */
//t 091214#if RTCSCFG_LINKOPT_8021Q_PRIO || RTCSCFG_LINKOPT_8023
   uchar_ptr            type_ptr;
//091214 #endif
   uint_32              hdrlen, error;
uint_16              type; //091214

ipe_test_counter = 0;

//  printfpd("\n\rIP %d", 1);

 // printf("\n\r IPE_recv");
   if_ptr->STATS.ST_RX_TOTAL++;
   if_ptr->STATS.ST_RX_OCTETS += pcb->FRAG[0].LENGTH;

  // ok return; //t 090330	


   packet = RTCSPCB_alloc_recv(pcb);

//ok  return; //t 090330	

    if (packet == NULL) 
//bad  if (1) //t090330 
    {
      if_ptr->STATS.ST_RX_MISSED++;
      PCB_free(pcb);
ipe_test_counter = 1;
      return;
   } /* Endif */

  //after hier point do not work usart olwais    RTCSPCB_free(packet); return;   //t 090330

   RTCSLOG_PCB_ALLOC(packet);

//ok!  return; //t 090330

   enetpkt = (ENET_HEADER_PTR)RTCSPCB_DATA(packet);
   if (enetpkt->DEST[0] & 1) {
      if ((enetpkt->DEST[0] == 0xFF)
       && (enetpkt->DEST[1] == 0xFF)
       && (enetpkt->DEST[2] == 0xFF)
       && (enetpkt->DEST[3] == 0xFF)
       && (enetpkt->DEST[4] == 0xFF)
       && (enetpkt->DEST[5] == 0xFF)) {
         if_ptr->STATS.ST_RX_BROADCAST++;
         packet->TYPE = RTCSPCB_TYPE_BROADCAST;
      } else {
         if_ptr->STATS.ST_RX_MULTICAST++;
         packet->TYPE = RTCSPCB_TYPE_MULTICAST;
      } /* Endif */
   } else {
      if_ptr->STATS.ST_RX_UNICAST++;
      packet->TYPE = RTCSPCB_TYPE_UNICAST;
   } /* Endif */

//091214 #if RTCSCFG_LINKOPT_8021Q_PRIO || RTCSCFG_LINKOPT_8023
   type_ptr = enetpkt->TYPE;
//091214 #endif
   /* End SPR P122-0266-37. */
   hdrlen = sizeof(ENET_HEADER);

#if RTCSCFG_LINKOPT_8021Q_PRIO
   if (ntohs(type_ptr) == 0x8100) {
      packet->LINK_OPTIONS.RX.OPT_PRIO = 1;
      packet->LINK_OPTIONS.RX.PRIO = (ntohc(type_ptr+2) >> 5) & 0x7;
      hdrlen += sizeof(ENET_8021QTAG_HEADER);
      type_ptr += sizeof(ENET_8021QTAG_HEADER);
   } /* Endif */
#endif

#if RTCSCFG_LINKOPT_8023
   if (ntohs(type_ptr) < ENET_FRAMESIZE_MAXDATA) {
      packet->LINK_OPTIONS.RX.OPT_8023 = 1;
      hdrlen += sizeof(ENET_8022_HEADER);
      type_ptr += sizeof(ENET_8022_HEADER);
   } /* Endif */
#endif


   error = RTCSPCB_next(packet, hdrlen);
   if (error) {
      if_ptr->STATS.ST_RX_ERRORS++;
      RTCS_seterror(&if_ptr->STATS.ERR_RX, error, (uint_32)packet);
      RTCSLOG_PCB_FREE(packet, error);
      RTCSPCB_free(packet);
ipe_test_counter = 2;
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(packet, RTCS_LOGCTRL_IFTYPE(IPIFTYPE_ETHERNET), 0);

   packet->LINK_OPTIONS.TX = packet->LINK_OPTIONS.RX;
   packet->IFSRC = handle;

 //printf("\n\r start RTCSCMD_service");

// ok return; //t 090330


//hier can use type of protokol

//_________________________________________________________________________________________091215
/*
 type = ntohs(type_ptr);

if(type == ENETPROT_IP)
{
 //IP_service
ipe_test_counter = 3;
if (!RTCSCMD_service(packet, IP_service))
  {
      if_ptr->STATS.ST_RX_MISSED++;
      RTCSLOG_PCB_FREE(packet, RTCS_OK);
      RTCSPCB_free(packet);
ipe_test_counter = 4;
   }
ipe_test_counter = 5;
}
else if(type == ENETPROT_RDN)
{
// RDN_service
ipe_test_counter = 6;
if (!RTCSCMD_service(packet, RDN_service))
  {
      if_ptr->STATS.ST_RX_MISSED++;
      RTCSLOG_PCB_FREE(packet, RTCS_OK);
      RTCSPCB_free(packet);
ipe_test_counter = 7;
   }
ipe_test_counter = 8;
}
else if(type == ENETPROT_ARP)
{
//ARP_service
// printfpd("\n\rIP %d", 2);
ipe_test_counter = 9;
if (!RTCSCMD_service(packet, ARP_service))
  {
// printfpd("\n\rIP %d", 3);
      if_ptr->STATS.ST_RX_MISSED++;
      RTCSLOG_PCB_FREE(packet, RTCS_OK);
      RTCSPCB_free(packet);
ipe_test_counter = 10;
   } 
ipe_test_counter = 11;
}
*/
//_________________________________________________________________________________________091215


 ipe_test_counter += 100;

 if (!RTCSCMD_service(packet, service))
//if (!RTCSCMD_service(packet, IP_service))
 {

//    return; //t 090331
// printfpd("\n\rIP %d", 4);
      if_ptr->STATS.ST_RX_MISSED++;
      RTCSLOG_PCB_FREE(packet, RTCS_OK);
      RTCSPCB_free(packet);
ipe_test_counter += 1000;
   } 

// printfpd("\n\rIP %d", 5);

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_recv_IP
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Services an IP packet.
*
*END*-----------------------------------------------------------------*/

void IPE_recv_IP
   (
      PCB_PTR  pcb,
         /* [IN] the received packet */
      pointer  handle
         /* [IN] the IP interface structure */
   )
{ /* Body */
#ifdef 	TEST_RDN
 cip++;
#endif
  IPE_recv(pcb, handle, IP_service);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_recv_ARP
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Services an ARP packet.
*
*END*-----------------------------------------------------------------*/

void IPE_recv_ARP
   (
      PCB_PTR  pcb,
         /* [IN] the received packet */
      pointer  handle
         /* [IN] the IP interface structure */
   )
{ /* Body */
#ifdef 	TEST_RDN
 carp++;
#endif

 //printfpd("rcv  %d", 2);

    IPE_recv(pcb, handle, ARP_service);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_recv_RDN
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Services an RDN packet.
*
*END*-----------------------------------------------------------------*/

void IPE_recv_RDN
   (
      PCB_PTR  pcb,
         /* [IN] the received packet */
      pointer  handle
         /* [IN] the IP interface structure */
   )
{ /* Body */
#ifdef 	TEST_RDN
crdn++;
#endif
    IPE_recv(pcb, handle, RDN_service);   //hier need change ARP_service to RDN_service
 //  IPE_recv1(pcb, handle, RDN_service);   //hier need change ARP_service to RDN_service
 //together get errors!!!!! IPE_recv1(pcb, handle, IP_service);   //091210 for test rdn_service changed to ip_service

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a packet.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_send
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    pcb,
         /* [IN] the packet to send */
      uint_16        protocol,
         /* [IN] ENETPROT_IP or ENETPROT_ARP */
      uchar          dest[6]
         /* [IN] the destination Ethernet address */
   )
{ /* Body */
   uint_32        hdrlen, flags, error;

   hdrlen = sizeof(ENET_HEADER);
   flags = 0;
//    printf("\n\r>>IPE_Send 0");
#if RTCSCFG_LINKOPT_8021Q_PRIO
   if (pcb->LINK_OPTIONS.TX.OPT_PRIO) {
      hdrlen += sizeof(ENET_8021QTAG_HEADER);
      flags |= ENET_SETOPT_8021QPRIO(pcb->LINK_OPTIONS.TX.PRIO);
   } /* Endif */
#endif

#if RTCSCFG_LINKOPT_8023
   if (pcb->LINK_OPTIONS.TX.OPT_8023) {
      hdrlen += sizeof(ENET_8022_HEADER);
      flags |= ENET_OPT_8023;
   } /* Endif */
#endif

   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_IFTYPE(IPIFTYPE_ETHERNET), 0);
  //printf("\n\r insert start ");
   error = RTCSPCB_insert(pcb, hdrlen);
 // printf("\n\r insert end");
   if_ptr->STATS.ST_TX_OCTETS += RTCSPCB_SIZE(pcb);
   if (error) {
      if_ptr->STATS.ST_TX_ERRORS++;
      RTCS_seterror(&if_ptr->STATS.ERR_TX, error, (uint_32)pcb);
      RTCSLOG_PCB_FREE(pcb, error);
      RTCSPCB_free(pcb);
      return error;
   } /* Endif */

   pcb->PCB_PTR->FRAG[0].LENGTH   = pcb->FRAGUSED;
   pcb->PCB_PTR->FRAG[0].FRAGMENT = RTCSPCB_DATA(pcb);
  //printf("\n\r IPE_Send ");
   error = ENET_send(if_ptr->HANDLE, pcb->PCB_PTR, protocol, dest, flags);
   RTCSLOG_PCB_FREE(pcb, error);
   if (error) {
   //   printf("error: %X", error);
      if_ptr->STATS.ST_TX_ERRORS++;
      RTCS_seterror(&if_ptr->STATS.ERR_TX, error, (uint_32)pcb);
   } /* Endif */
   return error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_IP
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends an IP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_send_IP
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    pcb,
         /* [IN] the packet to send */
      _ip_address    src,
         /* [IN] the next-hop source address */
      _ip_address    dest,
         /* [IN] the next-hop destination address */
      pointer        data
         /* [IN] unused */
   )
{ /* Body */
   uchar    enetdest[6];

   if_ptr->STATS.ST_TX_TOTAL++;

   if (dest == INADDR_BROADCAST) {
      if_ptr->STATS.ST_TX_BROADCAST++;
      IPE_ADDR_BC(enetdest);

   } else if (IN_MULTICAST(dest)) {
      if_ptr->STATS.ST_TX_MULTICAST++;
      IPE_ADDR_MC(enetdest, dest);

   } else {
      /* ARP_resolve() will record an error if one occurs */
      if_ptr->STATS.ST_TX_UNICAST++;
      return ARP_resolve(if_ptr, pcb, src, dest);

   } /* Endif */

   return IPE_send(if_ptr, pcb, ENETPROT_IP, enetdest);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_IP_ucast
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Called from ARP.  Sends a resolved IP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_send_IP_ucast
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    pcb,
         /* [IN] the packet to send */
      uchar          dest[6]
         /* [IN] the destination Ethernet address */
   )
{ /* Body */

   return IPE_send(if_ptr, pcb, ENETPROT_IP, dest);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_ARP_bcast
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a broadcast ARP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_send_ARP_bcast
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    pcb
         /* [IN] the packet to send */
   )
{ /* Body */
   uchar    enetdest[6];

   if_ptr->STATS.ST_TX_TOTAL++;
   if_ptr->STATS.ST_TX_BROADCAST++;
   IPE_ADDR_BC(enetdest);
   return IPE_send(if_ptr, pcb, ENETPROT_ARP, enetdest);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_RDN_bcast
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a broadcast RDN packet.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_send_RDN_bcast
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    pcb
         /* [IN] the packet to send */
   )
{ /* Body */
  uchar    enetdest[6];
  //   uchar enetdest[] = (0xff, 0xff, 0xff,0xff,0xff,0xff); 
/*
    enetdest[0] = 1;
    enetdest[1] = 2;
    enetdest[2] = 3;
    enetdest[3] = 4;
    enetdest[4] = 5;
    enetdest[5] = 6;
*/
//   printf("\n\r  IPE_send_RDN_bcast start ");
   if_ptr->STATS.ST_TX_TOTAL++;
   if_ptr->STATS.ST_TX_BROADCAST++;
   IPE_ADDR_BC(enetdest);

//   printf("\n\r  IPE_send_RDN_bcast  end ");
   return IPE_send(if_ptr, pcb, ENETPROT_RDN, enetdest);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_ARP_ucast
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a unicast ARP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_send_ARP_ucast
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    pcb,
         /* [IN] the packet to send */
      uchar          dest[6]
         /* [IN] the next-hop destination address */
   )
{ /* Body */

   if_ptr->STATS.ST_TX_TOTAL++;
   if_ptr->STATS.ST_TX_UNICAST++;
   return IPE_send(if_ptr, pcb, ENETPROT_ARP, dest);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_RDN_ucast
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a unicast ARP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_send_RDN_ucast
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    pcb,
         /* [IN] the packet to send */
      uchar          dest[6]
         /* [IN] the next-hop destination address */
   )
{ /* Body */

   if_ptr->STATS.ST_TX_TOTAL++;
   if_ptr->STATS.ST_TX_UNICAST++;
   return IPE_send(if_ptr, pcb, ENETPROT_RDN, dest);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_join
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Joins a multicast group.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_join
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      _ip_address    group
         /* [IN] the multicast group */
   )
{ /* Body */
   uchar    enetgroup[6];

   IPE_ADDR_MC(enetgroup, group);
   return ENET_join(if_ptr->HANDLE, ENETPROT_IP, enetgroup);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_leave
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Leaves a multicast group.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_leave
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      _ip_address    group
         /* [IN] the multicast group */
   )
{ /* Body */
   uchar    enetgroup[6];

   IPE_ADDR_MC(enetgroup, group);
   return ENET_leave(if_ptr->HANDLE, ENETPROT_IP, enetgroup);

} /* Endbody */


/* EOF */
