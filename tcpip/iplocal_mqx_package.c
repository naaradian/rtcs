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
*** File: iplocal.c
***
*** Comments:  This file contains the interface between IP and
***            the local host.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#include "icmp_prv.h"


/*
** The structure used by the application to bind an IP address
** to an Ethernet packet driver.
*/

static RTCS_IF_STRUCT rtcs_local = {
   IPLOCAL_open,
   IPLOCAL_close,
   IPLOCAL_send,
   NULL,
   NULL
};

const RTCS_IF_STRUCT_PTR RTCS_IF_LOCALHOST = &rtcs_local;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_open
* Returned Value  : ICB_STRUCT_PTR
* Comments        :
*     Registers a transport protocol with IP.
*
*END*-----------------------------------------------------------------*/

ICB_STRUCT_PTR IP_open
   (
      uchar             protocol,   /* [IN]  Protocol to use */
      IP_SERVICE_FN     service,    /* [IN]  Packet receive function */
      pointer           private,    /* [IN]  Owner's config block */
      uint_32_ptr       status      /* [OUT] return code */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR    IP_cfg_ptr;
   ICB_STRUCT_PTR icb, search_ptr;

   IP_cfg_ptr = RTCS_getcfg(IP);

   /* Make sure the protocol isn't already is use */
   RTCS_LIST_SEARCH(IP_cfg_ptr->ICB_HEAD, search_ptr) {
      if (search_ptr->PROTOCOL == protocol) {
         *status = RTCSERR_IP_PROT_OPEN;
         return NULL;
      } /* Endif */
   } /* End SEARCH */

   icb = _mem_alloc(sizeof(ICB_STRUCT));
   if (!icb) {
      *status = RTCSERR_IP_ICB_ALLOC;
      return NULL;
   } /* Endif */

   icb->PROTOCOL = protocol & 0xFF;
   icb->SERVICE  = service;
   icb->PRIVATE  = private;

   /*
   ** Add the record into the chain of open ICBs
   */
   RTCS_LIST_INS(IP_cfg_ptr->ICB_HEAD, icb);

   /*
   ** Open new ICB succeeded
   */
   *status = RTCS_OK;
   return icb;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPLOCAL_open
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Registers the local host as an IP interface.
*
*END*-----------------------------------------------------------------*/

uint_32 IPLOCAL_open
   (
      IP_IF_PTR   if_ptr
         /* [IN] the IP interface structure */
   )
{ /* Body */

   if_ptr->MTU = IP_MAX_MTU;
   if_ptr->ARP = NULL;

   if_ptr->DEV_TYPE     = 0;
   if_ptr->DEV_ADDRLEN  = 0;
   if_ptr->SNMP_IF_TYPE = IPIFTYPE_LOOPBACK;

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPLOCAL_close
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Unregisters the local host as an IP interface.
*
*END*-----------------------------------------------------------------*/

uint_32 IPLOCAL_close
   (
      IP_IF_PTR   if_ptr
         /* [IN] the IP interface structure */
   )
{ /* Body */

   return RTCS_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPLOCAL_send
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a packet.
*
*END*-----------------------------------------------------------------*/

uint_32 IPLOCAL_send
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
   IP_CFG_STRUCT_PTR    IP_cfg_ptr = RTCS_getcfg(IP);
   IP_HEADER_PTR        iph = (IP_HEADER_PTR)RTCSPCB_DATA(pcb);

   /*
   ** ST_RX_TOTAL is incremented in IP_service(), which catches
   ** all packets except those which arrived through the loopback
   ** interface.  Those are caught here.
   **
   ** Additionally, those packets that arrive by loopback are
   ** enqueued back onto the TCPIP task's message queue to
   ** prevent the task's stack requirements from getting too
   ** large (this is mainly for TCP loopback).
   */
   if (pcb->IFSRC == IP_cfg_ptr->IF_LOCALHOST) {
      IP_cfg_ptr->STATS.ST_RX_TOTAL++;

      if (!RTCSCMD_service(pcb, IPLOCAL_service)) {
         IP_cfg_ptr->STATS.ST_RX_MISSED++;
         RTCSLOG_PCB_FREE(pcb, RTCSERR_OUT_OF_BUFFERS);
         RTCSPCB_free(pcb);
         return RTCSERR_OUT_OF_BUFFERS;
      } /* Endif */

      return RTCS_OK;

   } else {
      /* If it is a fragmented packet, reassemble it */
      if (ntohs(iph->FRAGMENT) & (IP_FRAG_MF | IP_FRAG_MASK)) {
         uint_32 error = IP_reasm(pcb, &pcb);
         IP_cfg_ptr->STATS.ST_RX_FRAG_RECVD++;
         if (pcb) {
            IP_cfg_ptr->STATS.ST_RX_FRAG_REASMD++;
            if (IP_cfg_ptr->SEC_SEND_FN) {
               error = IP_cfg_ptr->SEC_SERVICE_FN(IP_cfg_ptr->SEC_FN_ARG,
                  if_ptr, &pcb, src, dest);
               if (error) {
                  RTCSLOG_PCB_FREE(pcb, error);
                  RTCSPCB_free(pcb);
                  return error;
               } /* Endif */
            } /* Endif */
         } else {
            if (error) {
               IP_cfg_ptr->STATS.ST_RX_FRAG_DISCARDED++;
            } /* Endif */
            return error;
         } /* Endif */
      } /* Endif */

      /* If not, don't wait and forward it to the upper layer */
      return IPLOCAL_service(pcb);

   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPLOCAL_service
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Delivers a packet to the loopback interface.
*
*END*-----------------------------------------------------------------*/

uint_32 IPLOCAL_service
   (
      RTCSPCB_PTR    pcb
         /* [IN] the packet to deliver */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR    IP_cfg_ptr;
   ICB_STRUCT_PTR       search_ptr;
   uchar                hdrlen;
   uchar                protocol;
   uint_32              error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   /* Find an ICB for the received packet's protocol */
   protocol = RTCSPCB_TRANSPORT_PROTL(pcb);
   RTCS_LIST_SEARCH(IP_cfg_ptr->ICB_HEAD, search_ptr) {
      if (search_ptr->PROTOCOL == protocol) {
         break;
      } /* Endif */
   } /* End SEARCH */

   if (!search_ptr) {
      IP_cfg_ptr->STATS.ST_RX_DISCARDED++;
      IP_cfg_ptr->STATS.ST_RX_NO_PROTO++;
      ICMP_send_error(ICMPTYPE_DESTUNREACH, ICMPCODE_DU_PROTO_UNREACH, 0, pcb, 0);
      RTCSLOG_PCB_FREE(pcb, RTCS_OK);
      RTCSPCB_free(pcb);
      return RTCS_OK;
   } /* Endif */

   hdrlen = RTCSPCB_TRANSPORT_DELTA(pcb);
   error = RTCSPCB_next(pcb, hdrlen);
   if (error) {
      IP_cfg_ptr->STATS.ST_RX_ERRORS++;
      RTCS_seterror(&IP_cfg_ptr->STATS.ERR_RX, error, (uint_32)pcb);
      RTCSLOG_PCB_FREE(pcb, error);
      RTCSPCB_free(pcb);
      return error;
   } /* Endif */

   IP_cfg_ptr->STATS.ST_RX_DELIVERED++;
   search_ptr->SERVICE(pcb, search_ptr->PRIVATE);
   return RTCS_OK;

} /* Endbody */


/* EOF */
