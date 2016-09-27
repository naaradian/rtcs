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
*** File: rtcssem.c
***
*** Comments:  This file contains the interface to the RTOS
***            semaphore services.
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_sem_trywait
* Returned Value   : RTCS_OK if the semaphore is obtained,
*                    otherwise RTCS_ERROR if not available
* Comments         :
*     This function obtains a semaphore from the _rtcs_sem if one is
*     available returning RTCS_OK, otherwise it returns RTCS_ERROR.
*     This call does not block.
*
*END*-----------------------------------------------------------------*/

extern uint_32 RTCS_sem_trywait
   (
      _rtcs_sem _PTR_ sem_ptr
   )
{ /* Body */
   uint_32 result;

   _int_disable();
   if (sem_ptr->VALUE <= 0) {
      result = RTCS_ERROR;
   } else {
      --sem_ptr->VALUE;
      result = RTCS_OK;
   } /* Endif */
   _int_enable();

   return result;
} /* Endbody */


/* EOF */
