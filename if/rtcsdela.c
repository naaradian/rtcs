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
*** File: rtcsdela.c
***
*** Comments:  This file contains the interface to the RTCS
***            time delay services.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "rtcstime.h"


typedef struct {
   TCPIP_PARM     COMMON;
   TCPIP_EVENT    EXPIRE;
   uint_32        milliseconds;
} RTCSTIME_DELAY_PARM, _PTR_ RTCSTIME_DELAY_PARM_PTR;


static void RTCSTIME_delay(RTCSTIME_DELAY_PARM_PTR);
static boolean RTCSTIME_delay_expire(TCPIP_EVENT_PTR);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_delay
* Returned Value  : void
* Comments        :
*
*  Blocks the calling task until the time (specified in milliseconds)
*  expires.
*
*END*-----------------------------------------------------------------*/

void RTCS_delay
   (
      /*[IN] time length in milliseconds to block task */
      uint_32 milliseconds
   )
{ /* Body */
   RTCSTIME_DELAY_PARM   parms;

   if (milliseconds) {
      parms.milliseconds = milliseconds;
      RTCSCMD_issue(parms, RTCSTIME_delay);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSTIME_delay
* Returned Value  : void
* Comments        :
*
*  Adds an event to the TCPIP timer queue.
*
*END*-----------------------------------------------------------------*/

static void RTCSTIME_delay
   (
      RTCSTIME_DELAY_PARM_PTR     parms
   )
{ /* Body */

   parms->EXPIRE.TIME    = parms->milliseconds;
   parms->EXPIRE.EVENT   = RTCSTIME_delay_expire;
   parms->EXPIRE.PRIVATE = parms;
   TCPIP_Event_add(&parms->EXPIRE);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSTIME_delay_expire
* Returned Value  : FALSE
* Comments        :
*
*  Unblocks the task that called RTCS_time_delay
*
*END*-----------------------------------------------------------------*/

static boolean RTCSTIME_delay_expire
   (
      TCPIP_EVENT_PTR   event
   )
{ /* Body */
   RTCSTIME_DELAY_PARM_PTR  parms = event->PRIVATE;

   RTCSCMD_complete(parms, RTCS_OK);
   return FALSE;

} /* Endbody */


/* EOF */
