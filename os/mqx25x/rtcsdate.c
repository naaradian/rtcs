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
*** File: rtcsdate.c
***
*** Comments:  This file contains the interface to the RTOS
***            date services.
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>


/* Factor to convert milliseconds into fraction of a second */
#define SECOND_FRACTION_TO_MILLISECONDS     (0xFFFFFFFF / 1000)

/*
** Time, in secondes, between Jan 1 0h 1900, and Jan 1 0h 1970:
**
** 17 leap years: 17 days + 365 days * 70 years = 25567 days
** 25567 days * 24 hours/day * 60 minutes/hour * 60 seconds/minute
** = 2208988800 seconds
*/
#define SECONDS_1900_1970  2208988800


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_date_get
* Returned Value   : void
* Comments  :  Gets the current date in seconds and fraction of seconds
*     since 0h Jan 1 1900
*
*END*-----------------------------------------------------------------*/

void RTCS_date_get
   (
      uint_32 _PTR_ seconds,
      /* [OUT] Elapsed seconds since 0h Jan 1 1900 */
      uint_32 _PTR_ fraction
      /* [OUT] Second fraction in 2^-32 second units */
   )
{ /* Body */
   TIME_STRUCT time;

   _time_get(&time);
   *seconds = time.SECONDS + SECONDS_1900_1970;
   *fraction = time.MILLISECONDS * SECOND_FRACTION_TO_MILLISECONDS;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_date_set
* Returned Value   : void
* Comments  :  Sets the current date in seconds and fraction of seconds
*     since 0h Jan 1 1900
*
*END*-----------------------------------------------------------------*/

void RTCS_date_set
   (
      uint_32  seconds,
      /* [IN] Elapsed seconds since 0h Jan 1 1900 */
      uint_32  fraction
      /* [IN] Fraction of a second in 2^-32 second units */
   )
{ /* Body */
   TIME_STRUCT time;

   time.SECONDS = seconds - SECONDS_1900_1970;
   time.MILLISECONDS = fraction / SECOND_FRACTION_TO_MILLISECONDS;
   _time_set(&time);
} /* Endbody */


/* EOF */
