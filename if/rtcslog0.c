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
*** File: rtcslog0.c
***
*** Comments:  This file contains the RTCS logging functions.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_log
* Returned Value  : void
* Comments  :  Logs an RTCS event
*
*END*-----------------------------------------------------------------*/

void RTCS_log
   (
      uint_32     logtype,
         /* [IN] specifies the type of log entry */
      uint_32     p1,
         /* [IN] first parameter */
      uint_32     p2
         /* [IN] second parameter */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();
   uint_32       i;

   switch (logtype) {
   case RTCSLOG_TYPE_FNENTRY:
   case RTCSLOG_TYPE_FNEXIT:
      if (!(rtcs->RTCS_LOG_CONTROL & RTCS_LOGCTRL_FUNCTION)) {
         return;
      } /* Endif */
      break;

   case RTCSLOG_TYPE_PCB:
      if (!(rtcs->RTCS_LOG_CONTROL & RTCS_LOGCTRL_PCB)) {
         return;
      } /* Endif */
      for (i = 0; i < RTCSLOG_PROT_MAX; i++) {
         if (rtcs->RTCS_LOG_PCB[i] == (p1 & 0xFFFF)) {
            break;
         } /* Endif */
      } /* Endfor */
      if (i == RTCSLOG_PROT_MAX) {
         return;
      } /* Endif */
      break;

   case RTCSLOG_TYPE_TIMER:
      if (!(rtcs->RTCS_LOG_CONTROL & RTCS_LOGCTRL_TIMER)) {
         return;
      } /* Endif */
      for (i = 0; i < RTCSLOG_PROT_MAX; i++) {
         if (rtcs->RTCS_LOG_TIMER[i] == (p1 & 0xFFFF)) {
            break;
         } /* Endif */
      } /* Endfor */
      if (i == RTCSLOG_PROT_MAX) {
         return;
      } /* Endif */
      break;

   default:
      return;
   } /* Endswitch */

   RTCS_log_internal(logtype, p1, p2);

} /* Endbody */


/* EOF */
