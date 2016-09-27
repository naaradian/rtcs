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
*** File: ip.c
***
*** Comments:  This file contains the implementation of the Internet
***            Protocol.  For more details, refer to RFC791 and RFC1122.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"

extern unsigned long keep_ip_cfg;




unsigned long* Set_ipDefaultTTL (unsigned long* pNew_TTL)
{ /* Body */
   static unsigned long Old_TTL;
   IP_CFG_STRUCT_PTR IP_cfg_ptr = RTCS_getcfg(IP);
   Old_TTL = IP_cfg_ptr->DEFAULT_TTL;
    IP_cfg_ptr->DEFAULT_TTL = *pNew_TTL;
   return (unsigned long*)&Old_TTL;
} /* Endbody */





/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_init
* Returned Values : RTCS_OK or error code
* Comments        :
*     Initialize the IP layer.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_init
   (
      void
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   IPIF_PARM         parms;
   uint_32           error;

   IP_cfg_ptr = _mem_alloc_zero(sizeof(IP_CFG_STRUCT));
   keep_ip_cfg = (unsigned long)(IP_cfg_ptr);

   RTCS_setcfg(IP, IP_cfg_ptr);

   RTCS_LIST_INIT(IP_cfg_ptr->ICB_HEAD);
   IP_cfg_ptr->NEXT_ID  = 0;
   IP_cfg_ptr->DEFAULT_TTL = IPTTL_DEFAULT;

   IP_cfg_ptr->IF_FREE       = NULL;
   IP_cfg_ptr->ROUTE_PARTID  = RTCS_part_create(sizeof(IP_ROUTE_DIRECT),
      IPROUTEALLOC_SIZE, IPROUTEALLOC_SIZE, 0, NULL, NULL);
   IP_cfg_ptr->GATE_PARTID   = RTCS_part_create(sizeof(IP_ROUTE_INDIRECT),
      IPGATEALLOC_SIZE,  IPGATEALLOC_SIZE,  0, NULL, NULL);
   IP_cfg_ptr->VIRTUAL_PARTID   = RTCS_part_create(sizeof(IP_ROUTE_VIRTUAL),
      IPROUTEALLOC_SIZE,  IPROUTEALLOC_SIZE,  0, NULL, NULL);
   IP_cfg_ptr->ROUTE_FN      = NULL;
   IP_cfg_ptr->MCB_PARTID    = RTCS_part_create(sizeof(MC_MEMBER),
      IPMCBALLOC_SIZE,   IPMCBALLOC_SIZE,   0, NULL, NULL);
   IP_cfg_ptr->SEC_SERVICE_FN = NULL;
   IP_cfg_ptr->SEC_SEND_FN    = NULL;
   IP_cfg_ptr->SEC_FN_ARG     = NULL;
   IP_cfg_ptr->RADIX_PARTID  = RTCS_part_create(sizeof(IP_ROUTE),
      RADIXALLOC_SIZE,   RADIXALLOC_SIZE,   0, NULL, NULL);

   IP_route_init(&IP_cfg_ptr->ROUTE_ROOT);

   /* Initialize the reassembler */
   IP_reasm_init();

   /* Add the localhost interface (127.0.0.1) */

   parms.mhandle = NULL;
   parms.if_ptr  = RTCS_IF_LOCALHOST;
   error = RTCSCMD_internal(parms, IPIF_add);
   if (error) {
      return error;
   } /* Endif */
   IP_cfg_ptr->IF_LOCALHOST = parms.ihandle;

/* parms.ihandle set by IPIF_add() */
   parms.address = INADDR_LOOPBACK;
 //  parms.locmask = IN_CLASSA_NET;
//   parms.locmask = IN_CLASSB_NET;
   parms.locmask = IN_CLASSC_NET;
//   parms.netmask = IN_CLASSA_NET;
 //  parms.netmask = IN_CLASSB_NET;
  parms.netmask = IN_CLASSC_NET;

   /* Create the localhost route entry */
 //  error = IP_route_add_direct(INADDR_LOOPBACK, IN_CLASSA_NET,
//   error = IP_route_add_direct(INADDR_LOOPBACK, IN_CLASSB_NET,
   error = IP_route_add_direct(INADDR_LOOPBACK, IN_CLASSC_NET,
      (IP_IF_PTR)parms.ihandle, (IP_IF_PTR)parms.ihandle);

   if (error) {
      return error;
   } /* Endif */

   /* IGMP_ipif_bind failure isn't sufficient reason to abort */
   IGMP_ipif_bind((IP_IF_PTR)parms.ihandle, INADDR_LOOPBACK);

   return RTCS_OK;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_init1
* Returned Values : RTCS_OK or error code
* Comments        :
*     Initialize the IP layer.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_init1
   (
      void
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   IPIF_PARM         parms;
   uint_32           error;

  IP_cfg_ptr = _mem_alloc_zero(sizeof(IP_CFG_STRUCT));
   //IP_cfg_ptr =(IP_CFG_STRUCT_PTR)keep_ip_cfg;
   RTCS_setcfg(IP, IP_cfg_ptr);

   RTCS_LIST_INIT(IP_cfg_ptr->ICB_HEAD);
   IP_cfg_ptr->NEXT_ID  = 0;
   IP_cfg_ptr->DEFAULT_TTL = IPTTL_DEFAULT;

   IP_cfg_ptr->IF_FREE       = NULL;
   IP_cfg_ptr->ROUTE_PARTID  = RTCS_part_create(sizeof(IP_ROUTE_DIRECT),
      IPROUTEALLOC_SIZE, IPROUTEALLOC_SIZE, 0, NULL, NULL);
   IP_cfg_ptr->GATE_PARTID   = RTCS_part_create(sizeof(IP_ROUTE_INDIRECT),
      IPGATEALLOC_SIZE,  IPGATEALLOC_SIZE,  0, NULL, NULL);
   IP_cfg_ptr->VIRTUAL_PARTID   = RTCS_part_create(sizeof(IP_ROUTE_VIRTUAL),
      IPROUTEALLOC_SIZE,  IPROUTEALLOC_SIZE,  0, NULL, NULL);
   IP_cfg_ptr->ROUTE_FN      = NULL;
   IP_cfg_ptr->MCB_PARTID    = RTCS_part_create(sizeof(MC_MEMBER),
      IPMCBALLOC_SIZE,   IPMCBALLOC_SIZE,   0, NULL, NULL);
   IP_cfg_ptr->SEC_SERVICE_FN = NULL;
   IP_cfg_ptr->SEC_SEND_FN    = NULL;
   IP_cfg_ptr->SEC_FN_ARG     = NULL;
   IP_cfg_ptr->RADIX_PARTID  = RTCS_part_create(sizeof(IP_ROUTE),
      RADIXALLOC_SIZE,   RADIXALLOC_SIZE,   0, NULL, NULL);

   IP_route_init(&IP_cfg_ptr->ROUTE_ROOT);

   /* Initialize the reassembler */
   IP_reasm_init();

   /* Add the localhost interface (127.0.0.1) */

   parms.mhandle = NULL;
   parms.if_ptr  = RTCS_IF_LOCALHOST;
 //101220   error = RTCSCMD_internal(parms, IPIF_add);
  error = RTCSCMD_internal(parms, IPIF_add2);
   if (error) {
      return error;
   } /* Endif */
   IP_cfg_ptr->IF_LOCALHOST = parms.ihandle;

/* parms.ihandle set by IPIF_add() */
   parms.address = INADDR_LOOPBACK;
//  parms.locmask = IN_CLASSA_NET;
//  parms.locmask = IN_CLASSB_NET;
  parms.locmask = IN_CLASSC_NET;
//   parms.netmask = IN_CLASSA_NET;
//   parms.netmask = IN_CLASSB_NET;
   parms.netmask = IN_CLASSC_NET;

   /* Create the localhost route entry */
 // error = IP_route_add_direct(INADDR_LOOPBACK, IN_CLASSA_NET,
//  error = IP_route_add_direct(INADDR_LOOPBACK, IN_CLASSB_NET,
  error = IP_route_add_direct(INADDR_LOOPBACK, IN_CLASSC_NET,
      (IP_IF_PTR)parms.ihandle, (IP_IF_PTR)parms.ihandle);

   if (error) {
      return error;
   } /* Endif */

   /* IGMP_ipif_bind failure isn't sufficient reason to abort */
   IGMP_ipif_bind((IP_IF_PTR)parms.ihandle, INADDR_LOOPBACK);

   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_send
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends an IP packet generated on the local host.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_send
   (
      RTCSPCB_PTR    pcb,
            /* [IN] the packet to send */
      uint_32        protocol,
            /* [IN] the transport layer protocol */
      _ip_address    ipsrc,
            /* [IN] the destination interface (0 = any) */
      _ip_address    ipdest,
            /* [IN] the ultimate destination */
      uint_32        flags
            /* [IN] optional flags */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR    IP_cfg_ptr;


   IP_cfg_ptr = RTCS_getcfg(IP);

   IP_cfg_ptr->STATS.ST_TX_TOTAL++;

   pcb->IP_COMPLETE = IP_complete_send;
   pcb->IFSRC       = IP_cfg_ptr->IF_LOCALHOST;

   /*
   ** Validate destination address
   */
   if (IN_ZERONET(ipdest)) {
      IP_cfg_ptr->STATS.ST_TX_DISCARDED++;
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_ADDRESS);
      RTCSPCB_free(pcb);
      return RTCSERR_IP_BAD_ADDRESS;
   } /* Endif */

   if ((ipdest == INADDR_BROADCAST)
    || IN_MULTICAST(ipdest)) {
      return IP_route_multi(pcb, protocol, ipsrc, ipdest, flags);
   } else {
      return IP_route(pcb, protocol, ipsrc, INADDR_ANY, ipdest, flags);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_send_IF
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends an IP packet through a specified interface.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_send_IF
   (
      RTCSPCB_PTR    pcb,
            /* [IN] the packet to send */
      uint_32        protocol,
            /* [IN] the transport layer protocol */
      pointer        interface
            /* [IN] the destination interface */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR    IP_cfg_ptr;
   IP_IF_PTR            ifdest = interface;

   IP_cfg_ptr = RTCS_getcfg(IP);

   IP_cfg_ptr->STATS.ST_TX_TOTAL++;


   pcb->IP_COMPLETE = IP_complete_send;
   pcb->IFSRC       = IP_cfg_ptr->IF_LOCALHOST;

   return IP_send_dgram(ifdest, pcb, INADDR_ANY, INADDR_BROADCAST,
      INADDR_BROADCAST, protocol, NULL);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_complete_send
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Generates an IP header for outgoing packets.  This function
*        must not consume pcb.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_complete_send
   (
      pointer           ifdest,
            /* [IN] the destination interface */
      RTCSPCB_PTR _PTR_ pcb_ptr_ptr,
            /* [IN] the packet to send */
      _ip_address       hopsrc,
            /* [IN] the destination interface */
       _ip_address      ipdest,
            /* [IN] the ultimate dest */
      uint_32           protocol
            /* [IN] the transport layer protocol */
   )
{ /* Body */
   RTCSPCB_PTR            pcb = *pcb_ptr_ptr;
   IP_CFG_STRUCT_PTR      IP_cfg_ptr;
   IP_HEADER_PTR          packet;
   uchar                  proto, ttl, tos;
   uint_32                error;
   uint_16                chksum;
   uchar                  dfrag;

   IP_cfg_ptr = RTCS_getcfg(IP);

   error = RTCSPCB_insert(pcb, sizeof(IP_HEADER));
   if (error) {
      IP_cfg_ptr->STATS.ST_TX_ERRORS++;
      RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb);
      /* The PCB will be freed for us. */
      return error;
   } /* Endif */

   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IP), IP_VERSION);
   packet = (IP_HEADER_PTR)RTCSPCB_DATA(pcb);
   RTCSPCB_DATA_NETWORK(pcb) = RTCSPCB_DATA(pcb);
   proto  = IPPROTO_GET(protocol);
   ttl    = IPTTL_GET(protocol);
   tos    = IPTOS_GET(protocol);
   dfrag  = IPDFRAG_GET(protocol);
   if (!ttl) ttl = IP_cfg_ptr->DEFAULT_TTL;

   htonc(packet->VERSLEN,  (IP_VERSION << 4) | (sizeof(IP_HEADER) >> 2));
   htonc(packet->TOS,      tos);
   htons(packet->LENGTH,   RTCSPCB_SIZE(pcb));
   htons(packet->ID,       IP_cfg_ptr->NEXT_ID);
   IP_cfg_ptr->NEXT_ID++;
   if (dfrag) {
      htons(packet->FRAGMENT, IP_FRAG_DF);
   } else {
      htons(packet->FRAGMENT, 0);
   } /* Endif */
   htonc(packet->TTL,      ttl);
   htonc(packet->PROTOCOL, proto);
   htonl(packet->DEST,     ipdest);
   htons(packet->CHECKSUM, 0);
   htonl(packet->SOURCE,   hopsrc);

   /* Set up the transport layer protocol and delta. */
   RTCSPCB_SET_TRANS_PROTL(pcb, proto);
   RTCSPCB_SET_TRANS_DELTA(pcb, sizeof(IP_HEADER));

   /*
   ** If the transport layer needs a pseudo header checksum,
   ** calculate it now.
   */
   if (pcb->IP_SUM_PTR) {
      chksum = IP_Sum_pseudo(pcb->IP_SUM, pcb, 0);
      chksum = IP_Sum_invert(chksum);
      htons(pcb->IP_SUM_PTR, chksum);
   } /* Endif */

   if (IP_cfg_ptr->SEC_SEND_FN) {
      error = IP_cfg_ptr->SEC_SEND_FN(IP_cfg_ptr->SEC_FN_ARG, ifdest,
         pcb_ptr_ptr, hopsrc, ipdest);
      if (error) {
         /* The calling function will discard the packet for us. */
         return error;
      } /* Endif */
   } /* Endif */

   pcb = *pcb_ptr_ptr;

   /* We need to refresh the header location, as it may have been shifted.*/
   packet = (IP_HEADER_PTR)RTCSPCB_DATA(pcb);
   chksum = _mem_sum_ip(0, sizeof(IP_HEADER), packet);
   chksum = IP_Sum_invert(chksum);
   htons(packet->CHECKSUM, chksum);

   /* Special case for loopback */
   if (ifdest == IP_cfg_ptr->IF_LOCALHOST) {
      RTCSPCB_PTR            new_pcb;
      uchar_ptr              data;
      PCB_FRAGMENT_PTR       frag;

      data = _mem_alloc_system(sizeof(PCB) + sizeof(PCB_FRAGMENT) +
         RTCSPCB_SIZE(pcb));

      if (!data)  {
         return RTCSERR_OUT_OF_MEMORY;
      } /* Endif */

      frag = ((PCB_PTR)data)->FRAG;
      ((PCB_PTR)data)->FREE = (void(_CODE_PTR_)(PCB_PTR))_mem_free;
      ((PCB_PTR)data)->PRIVATE = NULL;
      frag[0].LENGTH = RTCSPCB_SIZE(pcb);
      frag[0].FRAGMENT = data + sizeof(PCB) + sizeof(PCB_FRAGMENT);
      frag[1].LENGTH = 0;
      frag[1].FRAGMENT = NULL;

      new_pcb = RTCSPCB_alloc_recv((PCB_PTR)data);
      if (!new_pcb)  {
         _mem_free(data);
         return RTCSERR_OUT_OF_MEMORY;
      } /* Endif */

      new_pcb->TYPE = pcb->TYPE;
      new_pcb->IFSRC = pcb->IFSRC;
      new_pcb->LINK_OPTIONS = pcb->LINK_OPTIONS;
      new_pcb->IP_COMPLETE = IP_complete_recv;

      data += sizeof(PCB) + sizeof(PCB_FRAGMENT);
      RTCSPCB_memcopy(pcb, data, 0, RTCSPCB_SIZE(pcb));
      RTCSPCB_DATA_NETWORK(new_pcb) = RTCSPCB_DATA(new_pcb);
      RTCSPCB_free(pcb);

      *pcb_ptr_ptr = new_pcb;

      return IP_complete_recv(ifdest, pcb_ptr_ptr, hopsrc, ipdest, protocol);

   } /* Endif */

   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_service
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Parses a received IP packet.
*
*END*-----------------------------------------------------------------*/

void IP_service
   (
      RTCSPCB_PTR    pcb
            /* [IN] the packet to send */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR          IP_cfg_ptr;
   IP_HEADER_PTR              packet = (IP_HEADER_PTR)RTCSPCB_DATA(pcb);
   _ip_address                ipsrc, ipdest;
   uint_16                    hdrlen, pktlen;
   uint_32                    routeopt, error;
   uint_32 (_CODE_PTR_ _PTR_  nat_exec)(RTCSPCB_PTR _PTR_);

   IP_cfg_ptr = RTCS_getcfg(IP);

   IP_cfg_ptr->STATS.ST_RX_TOTAL++;
   hdrlen = (ntohc(packet->VERSLEN) & 0x0F) << 2;
   pktlen = ntohs(packet->LENGTH);
   RTCSPCB_DATA_NETWORK(pcb) = RTCSPCB_DATA(pcb);

   /*
   ** Make sure that
   **    sizeof(IP_HEADER) <= hdrlen <= pktlen <= RTCSPCB_SIZE(pcb)
   */
   if (hdrlen < sizeof(IP_HEADER)) {
      IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
      IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++;
      IP_cfg_ptr->STATS.ST_RX_SMALL_HDR++;
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_HEADER);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */
   if (pktlen < hdrlen) {
      IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
      IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++;
      IP_cfg_ptr->STATS.ST_RX_SMALL_DGRAM++;
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_HEADER);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */
   if (RTCSPCB_SIZE(pcb) < pktlen) {
      IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
      IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++;
      IP_cfg_ptr->STATS.ST_RX_SMALL_PKT++;
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_HEADER);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */

   /*
   ** IP version must be 4
   */
   if ((ntohc(packet->VERSLEN) >> 4) != IP_VERSION) {
      IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
      IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++;
      IP_cfg_ptr->STATS.ST_RX_BAD_VERSION++;
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_HEADER);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IP), IP_VERSION);

   /*
   ** Validate source address
   */
   ipsrc = ntohl(packet->SOURCE);
   if (IN_LOOPBACK(ipsrc)
    || IN_MULTICAST(ipsrc)
    || IN_EXPERIMENTAL(ipsrc)) {
      IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
      IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++;
      IP_cfg_ptr->STATS.ST_RX_BAD_SOURCE++;
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_ADDRESS);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */

   /*
   ** Validate destination address
   */
   ipdest = ntohl(packet->DEST);
   if (IN_LOOPBACK(ipdest)
    || IN_ZERONET(ipdest)
    || (IN_EXPERIMENTAL(ipdest) && ipdest != INADDR_BROADCAST)) {
      IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
      IP_cfg_ptr->STATS.ST_RX_ADDR_ERRORS++;
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_ADDRESS);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */

   /*
   ** Verify the checksum
   */
   if (_mem_sum_ip(0, hdrlen, packet) != 0xFFFF) {
      IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
      IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++;
      IP_cfg_ptr->STATS.ST_RX_BAD_CHECKSUM++;
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_CHECKSUM);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */

   /*
   ** In this function, the PCB always has only one fragment, so it's
   ** always safe to call RTCSPCB_shrink() here.
   */
   error = RTCSPCB_shrink(pcb, RTCSPCB_SIZE(pcb) - pktlen);
   if (error) {
      IP_cfg_ptr->STATS.ST_RX_ERRORS++;
      RTCS_seterror(&IP_cfg_ptr->STATS.ERR_RX, error, (uint_32)pcb);
      RTCSLOG_PCB_FREE(pcb, error);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */


   pcb->IP_COMPLETE = IP_complete_recv;
/* pcb->IFSRC has already been set by link layer */

   /*
   ** Now decide what to do with the packet.
   **
   ** Normally, we should do:
   **
   **    if (!FORWARD || !(TYPE & UNICAST))
   **       IP_route_local();
   **    else
   **       IP_route();
   **
   ** i.e. we only forward unicast packets, and only if forwarding is
   ** turned on.  Unfortunately, this isn't sufficient.
   **
   ** If forwarding is turned on, limited broadcasts from PPP links
   ** will be discarded, because on PPP links, (TYPE & UNICAST) is
   ** always true (PPP packets are both unicast and broadcast), and
   ** IP_route() discards limited broadcasts.
   **
   ** Hence, we have to explicitly make sure that limited broadcasts
   ** and multicasts go through IP_route_local().
   **
   ** Once multicast routing is implemented, we'll remove IN_MULTICAST()
   ** from the first condition.
   */

   if (!_IP_forward
    || !(pcb->TYPE & RTCSPCB_TYPE_UNICAST)
    || (ipdest == INADDR_BROADCAST)
    || IN_MULTICAST(ipdest)) {

      IP_route_local(pcb, ipdest);

   } else {

      if (IN_ZERONET(ipsrc)) {
         IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
         IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++;
         IP_cfg_ptr->STATS.ST_RX_BAD_SOURCE++;
         RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_BAD_ADDRESS);
         RTCSPCB_free(pcb);
         return;
      } /* Endif */

      /*
      ** If the packet was broadcast, don't send it out the receiving
      ** interface.  This protects mainly against bouncing PPP directed
      ** broadcasts.
      */

      if (pcb->TYPE & RTCSPCB_TYPE_BROADCAST) {
        routeopt = IPROUTEOPT_RECVIF;
    } else {
         routeopt = 0;
   } /* Endif */

      /* Apply NAT if it is installed */
      nat_exec = RTCS_getcfg(NAT);
      if (nat_exec && *nat_exec) {
         error = (*nat_exec)(&pcb);
         if (error) {
            IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
            RTCSLOG_PCB_FREE(pcb, error);
            RTCSPCB_free(pcb);
            return;
         } /* Endif */
         /* PCB pointer may have changed */
         packet = (IP_HEADER_PTR)RTCSPCB_DATA(pcb);
         ipdest = ntohl(packet->DEST);
      } /* Endif */

      IP_route(pcb, 0, INADDR_ANY, ipsrc, ipdest, routeopt);

   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_complete_recv
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Decrements the TTL on forwarding packets.  This function
*        must not consume pcb.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_complete_recv
   (
      pointer           ifdest,
            /* [IN] the destination interface */
      RTCSPCB_PTR _PTR_ pcb_ptr_ptr,
            /* [IN] the packet to send */
      _ip_address       hopsrc,
            /* [IN] the destination interface */
      _ip_address       ipdest,
      /* [IN] the ultimate dest */
      uint_32           protocol
      /* [IN] the transport layer protocol */
   )
{ /* Body */
   RTCSPCB_PTR          pcb = *pcb_ptr_ptr;
   IP_CFG_STRUCT_PTR    IP_cfg_ptr;
   IP_HEADER_PTR packet = (IP_HEADER_PTR)RTCSPCB_DATA(pcb);
   uint_16       hdrlen, pktlen;
   uint_16       chksum;
   uchar         ttl;
   uint_32       error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   /* Set up the transport layer protocol and delta. */
   RTCSPCB_SET_TRANS_PROTL(pcb, ntohc(packet->PROTOCOL));
   RTCSPCB_SET_TRANS_DELTA(pcb, ((ntohc(packet->VERSLEN) & 0x0F) << 2));

   if (IP_cfg_ptr->SEC_SEND_FN) {
      error = IP_cfg_ptr->SEC_SERVICE_FN(IP_cfg_ptr->SEC_FN_ARG, ifdest,
         pcb_ptr_ptr, hopsrc, ipdest);
      if (error) {
         /* The calling function will discard the packet for us. */
         return error;
      } /* Endif */
   } /* Endif */

   /* Either no security database exists, or the packet has been decoded, or */
   /* the packet has been let through unchanged (bypass). */
   pcb = *pcb_ptr_ptr;

   if ((IP_IF_PTR)ifdest != IP_cfg_ptr->IF_LOCALHOST) {
      /*
      ** Check the TTL
      */

      ttl = ntohc(packet->TTL);
      if (ttl <= 1) {
         IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
         IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++;
         IP_cfg_ptr->STATS.ST_RX_TTL_EXCEEDED++;
         ICMP_send_error(ICMPTYPE_TIMEEXCEED, ICMPCODE_TE_TTL, 0, pcb, 0);
         return RTCSERR_IP_TTL;
      } /* Endif */

      /*
      ** Fork the PCB and duplicate the IP header.  Also copy up to
      ** 8 bytes of data in case we need to generate an ICMP error.
      */

      hdrlen = (ntohc(packet->VERSLEN) & 0x0F) << 2;
      pktlen = ntohs(packet->LENGTH);
      if (pktlen > hdrlen + 8) {
         pktlen = hdrlen + 8;
      } /* Endif */

      error = RTCSPCB_next(pcb, pktlen);
      if (error) {
         IP_cfg_ptr->STATS.ST_TX_ERRORS++;
         RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb);
         return error;
      } /* Endif */

      error = RTCSPCB_fork(pcb);
      if (error) {
         IP_cfg_ptr->STATS.ST_TX_ERRORS++;
         RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb);
         return error;
      } /* Endif */

      RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IP), IP_VERSION);

      error = RTCSPCB_insert(pcb, pktlen);
      if (error) {
         IP_cfg_ptr->STATS.ST_TX_ERRORS++;
         RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb);
         return error;
      } /* Endif */

      _mem_copy((uchar_ptr)packet, RTCSPCB_DATA(pcb), pktlen);
      packet = (IP_HEADER_PTR)RTCSPCB_DATA(pcb);
      RTCSPCB_DATA_NETWORK(pcb) = RTCSPCB_DATA(pcb);

      /*
      ** Decrement the TTL and adjust the checksum
      */

      ttl--;
      htonc(packet->TTL, ttl);
      chksum = ntohs(packet->CHECKSUM);
      chksum = IP_Sum_immediate(chksum, 0x100);
      htons(packet->CHECKSUM, chksum);

      IP_cfg_ptr->STATS.ST_RX_FORWARDED++;

   } /* Endif */

   pcb->LINK_OPTIONS.RX.DEST  = ipdest;
   pcb->LINK_OPTIONS.RX.TOS   = ntohc(packet->TOS);
   pcb->LINK_OPTIONS.RX.TTL   = ntohc(packet->TTL);

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_send_dgram
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends an IP datagram out an interface, fragmenting it if
*        necessary.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_send_dgram
   (
      IP_IF_PTR      ifdest,     /* [IN] the outgoing interface */
      RTCSPCB_PTR    inpcb,      /* [IN] the packet to send */
      _ip_address    hopsrc,     /* [IN] the hop src */
      _ip_address    hopdest,    /* [IN] the hop dest */
      _ip_address    ipdest,     /* [IN] the ultimate dest */
      uint_32        protocol,   /* [IN] the transport layer protocol */
      pointer        data        /* [IN] routing entry data */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr = RTCS_getcfg(IP);
   IP_HEADER_PTR     srciph;
   uint_32           iphlen;
   uint_32           srcdatalen;
   uint_32           dmtumax, dmtu;
   uint_32           srclen, offset, error;
   uchar_ptr         srcdata;
   PCB_FRAGMENT_PTR  frag_ptr;

   error = inpcb->IP_COMPLETE(ifdest, &inpcb, hopsrc, ipdest, protocol);
   if (error) {
      RTCSLOG_PCB_FREE(inpcb, error);
      RTCSPCB_free(inpcb);
      return error;
   } /* Endif */

   /* If the packet doesn't exceed the MTU, send it directly */
   if (RTCSPCB_SIZE(inpcb) <= ifdest->MTU) {
      return ifdest->DEVICE.SEND(ifdest, inpcb, hopsrc, hopdest, data);
   } /* Endif */

   srciph = (IP_HEADER_PTR)RTCSPCB_DATA(inpcb);
   iphlen = (ntohc(srciph->VERSLEN) & 0x0F) << 2;
   srcdatalen = RTCSPCB_SIZE(inpcb) - iphlen;

   /* If the datagram exceeds the MTU, but DF is set, respond with ICMP */
   if (ntohs(srciph->FRAGMENT) & IP_FRAG_DF) {
      IP_cfg_ptr->STATS.ST_TX_FRAG_DISCARDED++;
      ICMP_send_error(ICMPTYPE_DESTUNREACH, ICMPCODE_DU_NEEDFRAG, ifdest->MTU, inpcb, 0);
      RTCSLOG_PCB_FREE(inpcb, RTCSERR_IP_CANTFRAG);
      RTCSPCB_free(inpcb);
      return RTCSERR_IP_CANTFRAG;
   } /* Endif */

   /* Sanity check.  Note: it's an error to bind an interface with an MTU < 68 */
   if (iphlen + IP_FRAG_MIN > ifdest->MTU) {
      IP_cfg_ptr->STATS.ST_TX_FRAG_DISCARDED++;
      RTCSLOG_PCB_FREE(inpcb, RTCSERR_IP_SMALLMTU);
      RTCSPCB_free(inpcb);
      return RTCSERR_IP_SMALLMTU;
   } /* Endif */

   IP_cfg_ptr->STATS.ST_TX_FRAG_FRAGD++;
   dmtumax = ifdest->MTU - iphlen;
   dmtu = dmtumax & (((uint_32)(-1))<<IP_FRAG_SHIFT);
   offset = (ntohs(srciph->FRAGMENT) & IP_FRAG_MASK) << IP_FRAG_SHIFT;
   srcdata = RTCSPCB_DATA(inpcb) + iphlen;
   srclen = inpcb->FRAGUSED - iphlen;
   frag_ptr = inpcb->PCB_PTR->FRAG;

   do {
      IP_HEADER_PTR  dstiph;
      uchar_ptr      dstdata;
      uint_32        dstlen = (srcdatalen > dmtumax) ? dmtu : srcdatalen;
      RTCSPCB_PTR    pcb;
      uint_16        chksum, fragment;

      /* Allocate the PCB */
      pcb = RTCSPCB_alloc_send();
      if (pcb == NULL) {
         IP_cfg_ptr->STATS.ST_TX_MISSED++;
         RTCSLOG_PCB_FREE(inpcb, RTCSERR_PCB_ALLOC);
         RTCSPCB_free(inpcb);
         return RTCSERR_PCB_ALLOC;
      } /* Endif */

      /* Allocate the memory for the IP data */
      dstdata = _mem_alloc_system(dstlen);
      if (dstdata == NULL) {
         IP_cfg_ptr->STATS.ST_TX_MISSED++;
         RTCSLOG_PCB_FREE(pcb, RTCSERR_OUT_OF_MEMORY);
         RTCSPCB_free(pcb);
         RTCSLOG_PCB_FREE(inpcb, RTCSERR_OUT_OF_MEMORY);
         RTCSPCB_free(inpcb);
         return RTCSERR_OUT_OF_MEMORY;
      } /* Endif */

      /* Append the data to the PCB */
      error = RTCSPCB_frag_wfree(pcb, dstlen, dstdata);
      if (error) {
         IP_cfg_ptr->STATS.ST_TX_ERRORS++;
         RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb);
         _mem_free(dstdata);
         RTCSLOG_PCB_FREE(pcb, error);
         RTCSPCB_free(pcb);
         RTCSLOG_PCB_FREE(inpcb, error);
         RTCSPCB_free(inpcb);
         return error;
      } /* Endif */

      /* Allocate the IP header */
      error = RTCSPCB_insert(pcb, iphlen);
      if (error) {
         IP_cfg_ptr->STATS.ST_TX_ERRORS++;
         RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb);
         /* don't _mem_free(dstdata) here because RTCSPCB_free(pcb) will do it */
         RTCSLOG_PCB_FREE(pcb, error);
         RTCSPCB_free(pcb);
         RTCSLOG_PCB_FREE(inpcb, error);
         RTCSPCB_free(inpcb);
         return error;
      } /* Endif */

      /* Copy the IP header */
      dstiph = (IP_HEADER_PTR)RTCSPCB_DATA(pcb);
      RTCSPCB_DATA_NETWORK(pcb) = RTCSPCB_DATA(pcb);
      _mem_copy(srciph, dstiph, iphlen);

      /* Update the LENGTH field */
      htons(dstiph->LENGTH, iphlen + dstlen);

      /* Build the FRAGMENT field */
      fragment = offset >> IP_FRAG_SHIFT;
      offset     += dstlen;
      srcdatalen -= dstlen;
      /* An MF bit must be added on all fragments except the last */
      if (srcdatalen > 0 || (ntohs(srciph->FRAGMENT) & IP_FRAG_MF)) {
         fragment |= IP_FRAG_MF;
      } /* Endif */
      htons(dstiph->FRAGMENT, fragment);

      /* Recompute the IP header checksum */
      htons(dstiph->CHECKSUM, 0);
      chksum = _mem_sum_ip(0, iphlen, dstiph);
      chksum = IP_Sum_invert(chksum);
      htons(dstiph->CHECKSUM, chksum);

      /* Copy the IP data */
      for (;;) {
         uint_32 copylen = (srclen > dstlen) ? dstlen : srclen;
         _mem_copy(srcdata, dstdata, copylen);
         srcdata += copylen;
         dstdata += copylen;
         srclen  -= copylen;
         dstlen  -= copylen;
         if (dstlen == 0) break;
         frag_ptr++;
         srcdata = frag_ptr->FRAGMENT;
         srclen = frag_ptr->LENGTH;
      } /* Endfor */

      /* Send the fragment through the interface */
      IP_cfg_ptr->STATS.ST_TX_FRAG_SENT++;
      error = ifdest->DEVICE.SEND(ifdest, pcb, hopsrc, hopdest, data);
      if (error) {
         RTCSLOG_PCB_FREE(inpcb, error);
         RTCSPCB_free(inpcb);
         return error;
      } /* Endif */

   } while (srcdatalen > 0);

   RTCSLOG_PCB_FREE(inpcb, RTCS_OK);
   RTCSPCB_free(inpcb);
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_source
* Returned Value  : source IP address
* Comments        :
*     Return source IP address of packet.
*
*END*-----------------------------------------------------------------*/

_ip_address IP_source
   (
      RTCSPCB_PTR    pcb      /* [IN] packet to find source of */
   )
{ /* Body */
   uchar_ptr srcptr = ((IP_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb))->SOURCE;

   return ntohl(srcptr);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_dest
* Returned Value  : destination IP address
* Comments        :
*     Return destination IP address of packet.
*
*END*-----------------------------------------------------------------*/

_ip_address IP_dest
   (
      RTCSPCB_PTR    pcb      /* [IN] packet to find dest of */
   )
{ /* Body */
   uchar_ptr dstptr = ((IP_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb))->DEST;

   return ntohl(dstptr);

} /* Endbody */



/* EOF */
