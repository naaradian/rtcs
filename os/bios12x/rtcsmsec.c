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
*** File: rtcsmsec.c
***
*** Comments:  This file contains the interface to the RTOS
***            time services.
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_time_get
* Returned Value  : milliseconds elapsed since boot
* Comments        : Called to get number of milliseconds since
*                   bootup.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_time_get
   (
      void
   )
{ /* Body */
   TIME_STRUCT    time;

   _time_get_elapsed(&time);
   return (time.SECONDS*1000 + time.MILLISECONDS);

} /* Endbody */


/* EOF */
