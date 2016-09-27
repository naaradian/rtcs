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
*** File: rtcslog1.c
***
*** Comments:  This file contains the RTCS logging control functions.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSLOG_enable
* Returned Value  : void
* Comments  :  Enables logging of certain RTCS events
*
*END*-----------------------------------------------------------------*/

void RTCSLOG_enable
   (
      uint_32     logtype
         /* [IN] the type of log entry to enable */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();

   rtcs->RTCS_LOG_CONTROL |= logtype;
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_IFTYPE (IPIFTYPE_ETHERNET) );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_IFTYPE (IPIFTYPE_PPP)      );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_IFTYPE (IPIFTYPE_LOOPBACK) );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_ARP    (IPIFTYPE_ETHERNET) );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_ICMP)      );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_IGMP)      );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_IP)        );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_TCP)       );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_UDP)       );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_OSPF)      );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PORT   (IPPORT_BOOTPC)     );
   RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PORT   (IPPORT_RIP)        );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSLOG_disable
* Returned Value  : void
* Comments  :  Disables logging of certain RTCS events
*
*END*-----------------------------------------------------------------*/

void RTCSLOG_disable
   (
      uint_32     logtype
         /* [IN] the type of log entry to disable */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();
   uint_32       i;

   rtcs->RTCS_LOG_CONTROL &= ~logtype;

   if (logtype & RTCS_LOGCTRL_PCB) {
      for (i=0; i<RTCSLOG_PROT_MAX; i++) {
         rtcs->RTCS_LOG_PCB[i] = 0;
      } /* Endfor */
   } /* Endif */

   if (logtype & RTCS_LOGCTRL_TIMER) {
      for (i=0; i<RTCSLOG_PROT_MAX; i++) {
         rtcs->RTCS_LOG_TIMER[i] = 0;
      } /* Endfor */
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSLOG_enable_prot
* Returned Value  : void
* Comments  :  Enables logging of certain RTCS events for a specific
*              protocol
*
*END*-----------------------------------------------------------------*/

void RTCSLOG_enable_prot
   (
      uint_32     logtype,
         /* [IN] the type of log entry to enable */
      uint_32     protocol
         /* [IN] the protocol to enable */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();
   uint_32       i, j;

   rtcs->RTCS_LOG_CONTROL |= logtype;

   if (logtype & RTCS_LOGCTRL_PCB) {
      j = RTCSLOG_PROT_MAX;
      for (i = RTCSLOG_PROT_MAX; i > 0; ) {
         i--;
         if (rtcs->RTCS_LOG_PCB[i] == protocol) {
            j = i;
            break;
         } else if (rtcs->RTCS_LOG_PCB[i] == 0) {
            j = i;
         } /* Endif */
      } /* Endfor */
      if (j < RTCSLOG_PROT_MAX) {
         rtcs->RTCS_LOG_PCB[j] = protocol;
      } /* Endif */
   } /* Endif */

   if (logtype & RTCS_LOGCTRL_TIMER) {
      j = RTCSLOG_PROT_MAX;
      for (i = RTCSLOG_PROT_MAX; i > 0; ) {
         i--;
         if (rtcs->RTCS_LOG_TIMER[i] == protocol) {
            j = i;
            break;
         } else if (rtcs->RTCS_LOG_TIMER[i] == 0) {
            j = i;
         } /* Endif */
      } /* Endfor */
      if (j < RTCSLOG_PROT_MAX) {
         rtcs->RTCS_LOG_TIMER[j] = protocol;
      } /* Endif */
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSLOG_disable_prot
* Returned Value  : void
* Comments  :  Disables logging of certain RTCS events for a specific
*              protocol
*
*END*-----------------------------------------------------------------*/

void RTCSLOG_disable_prot
   (
      uint_32     logtype,
         /* [IN] the type of log entry to disable */
      uint_32     protocol
         /* [IN] the protocol to disable */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();
   uint_32       i, j;

   rtcs->RTCS_LOG_CONTROL &= ~logtype | (RTCS_LOGCTRL_PCB | RTCS_LOGCTRL_TIMER);

   if (logtype & RTCS_LOGCTRL_PCB) {
      j = RTCSLOG_PROT_MAX;
      for (i = 0; i < RTCSLOG_PROT_MAX; i++) {
         if (rtcs->RTCS_LOG_PCB[i] == protocol) {
            rtcs->RTCS_LOG_PCB[i] = 0;
         } else if (rtcs->RTCS_LOG_PCB[i] != 0) {
            j = i;
         } /* Endif */
      } /* Endfor */
      if (j < RTCSLOG_PROT_MAX) {
         rtcs->RTCS_LOG_CONTROL &= ~RTCS_LOGCTRL_PCB;
      } /* Endif */
   } /* Endif */

   if (logtype & RTCS_LOGCTRL_TIMER) {
      j = RTCSLOG_PROT_MAX;
      for (i = 0; i < RTCSLOG_PROT_MAX; i++) {
         if (rtcs->RTCS_LOG_TIMER[i] == protocol) {
            rtcs->RTCS_LOG_TIMER[i] = 0;
         } else if (rtcs->RTCS_LOG_TIMER[i] != 0) {
            j = i;
         } /* Endif */
      } /* Endfor */
      if (j < RTCSLOG_PROT_MAX) {
         rtcs->RTCS_LOG_CONTROL &= ~RTCS_LOGCTRL_TIMER;
      } /* Endif */
   } /* Endif */

} /* Endbody */


/* EOF */
