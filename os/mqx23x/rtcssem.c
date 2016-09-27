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
* Function Name    : RTCS_sem_create
* Returned Value   : RTCS_OK
*   queue
* Comments         :
*   This function initializes an rtcs semaphore.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_sem_create
   (
      RTCS_LWSEM_PTR sem_ptr, /* [IN] address of the semaphore to initialize */
      int_32 initial_number   /* [IN] inital number of semaphores available  */
   )
{ /* Body */

   INIT_QUEUE(&sem_ptr->QUEUE, 0);
   sem_ptr->VALUE = initial_number;

   return(RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_sem_destroy
* Returned Value   : error code
* Comments         :
*   This function deletes a light weight semaphore.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_sem_destroy
   (
      RTCS_LWSEM_PTR sem_ptr     /* [IN] the semaphore handle */
   )
{ /* Body */
   RTCS_TD_Q_PTR          td_q_ptr;

   _Disable();

   while (QUEUE_SIZE(&sem_ptr->QUEUE)) {
      td_q_ptr = (RTCS_TD_Q_PTR) sem_ptr->QUEUE.NEXT;
      DEQUEUE(&sem_ptr->QUEUE, td_q_ptr);

      _Add_ready(td_q_ptr->TD);
   } /* Endwhile */

   _Enable();

   _Reschedule(); /* Allow higher priority tasks to run */

   return(MQX_OK);

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_sem_wait
* Returned Value   : an error code
*   queue
* Comments         :
*   This function obtains a semaphore from the lwsem.  If one is not
* available, the task waits (in a FIFO order)
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_sem_wait
   (
      RTCS_LWSEM_PTR sem_ptr     /* [IN] the semaphore address */
   )
{ /* Body */
   RTCS_TD_Q            td_q;

   _Disable();
   if (sem_ptr->VALUE <= 0) {
      td_q.TD = _Convert_to_td(_My_id());

      ENQUEUE(&sem_ptr->QUEUE, sem_ptr->QUEUE.PREV, &td_q);

      _Block(); /* Let the other tasks run */

      /* Another task has posted a semaphore, and it has been tranfered to this
      ** task.
      */
   } else {
      --sem_ptr->VALUE;
   } /* Endif */

   _Enable();

   return(MQX_OK);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_sem_trywait
* Returned Value   : RTCS_OK if the semaphore is obtained,
*                    otherwise RTCS_ERROR if not available
* Comments         :
*   This function obtains a semaphore from the _rtcs_sem if one is
* available returning RTCS_OK, otherwise it returns RTCS_ERROR.  This call does
* not block.
*
*END*-----------------------------------------------------------------*/

extern uint_32 RTCS_sem_trywait
   (
      RTCS_LWSEM_PTR sem_ptr     /* [IN] the semaphore address */
   )
{ /* Body */
   uint_32 result;

   _Disable();
   if (sem_ptr->VALUE <= 0) {
      result = RTCS_ERROR;
   } else {
      --sem_ptr->VALUE;
      result = RTCS_OK;
   } /* Endif */
   _Enable();

   return result;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_sem_post
* Returned Value   : an error code
*   queue
* Comments         :
*   This function posts a semaphore to the lwsem.  If any tasks are waiting
* for semaphores, the first available task is made ready.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_sem_post
   (
      RTCS_LWSEM_PTR sem_ptr     /* [IN] the semaphore address */
   )
{ /* Body */
   RTCS_TD_Q_PTR        td_q_ptr;

   _Disable();
   if ((sem_ptr->VALUE >= 0) && QUEUE_SIZE(&sem_ptr->QUEUE)) {
      td_q_ptr = (RTCS_TD_Q_PTR) sem_ptr->QUEUE.NEXT;
      DEQUEUE(&sem_ptr->QUEUE, td_q_ptr);

      _Add_ready(td_q_ptr->TD);
   } else {
      ++sem_ptr->VALUE;
   } /* Endif */
   _Enable();

   return(MQX_OK);

} /* Endbody */


/* EOF */
