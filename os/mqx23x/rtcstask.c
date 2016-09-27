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
*** File: rtcstask.c
***
*** Comments:  This file contains the interface to the RTOS
***            task management functions.
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>

#include <mqx_prv.h>

struct rtcs_task_state {
   _rtcs_sem        sem;
   void (_CODE_PTR_ start)(pointer, pointer);
   pointer          arg;
   uint_32          error;
};

/*TASK*-----------------------------------------------------------------
*
* Function Name   : RTCS_task
* Returned Values :
* Comments        :
*     The parent of all RTCS tasks.
*
*END*-----------------------------------------------------------------*/

void RTCS_task
   (
      uint_32 task_parm
   )
{ /* Body */
   struct rtcs_task_state _PTR_ task = (struct rtcs_task_state _PTR_)task_parm;

   /* This is completely unnecessary -- it's done only for Task Aware Debugging */
   ((TD_STRUCT_PTR)_Convert_to_td(_My_id()))->TASK_TEMPLATE_PTR->TASK_ADDRESS = (void(_CODE_PTR_)(uint_32))task->start;

   task->start(task->arg, task);

   _Destroy(_My_id());

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_create
* Returned Values : uint_32 (error code)
* Comments        :
*     Create a task and wait for it to complete initialization.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_task_create
   (
      char_ptr          name,
      uint_32           priority,
      uint_32           stacksize,
      void (_CODE_PTR_  start)(pointer, pointer),
      pointer           arg
   )
{ /* Body */
   TASK_TEMPLATE_STRUCT    task_template;
   struct rtcs_task_state  task;

   RTCS_sem_init(&task.sem);
   task.start = start;
   task.arg   = arg;
   task.error = RTCS_OK;

   _Zeromem((uchar_ptr)&task_template, sizeof(task_template));
   task_template.TASK_NAME          = name;
   task_template.TASK_PRIORITY      = priority;
   task_template.TASK_STACKSIZE     = stacksize;
   task_template.TASK_ADDRESS       = RTCS_task;
   task_template.CREATION_PARAMETER = (uint_32)&task;
   if (_Create(0, 0, (uint_32)&task_template) == MQX_NULL_TASK_ID) {
      RTCS_sem_destroy(&task.sem);
      return RTCSERR_CREATE_FAILED;
   } /* Endif */

   RTCS_sem_wait(&task.sem);
   RTCS_sem_destroy(&task.sem);
   return task.error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_resume_creator
* Returned Values : void
* Comments        :
*     Return an error code to the caller's creator.
*
*END*-----------------------------------------------------------------*/

void RTCS_task_resume_creator
   (
      pointer creator,
      uint_32 error
   )
{ /* Body */
   struct rtcs_task_state _PTR_ task = creator;

   if (task) {
      task->error = error;
      RTCS_sem_post(&task->sem);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_exit
* Returned Values : void
* Comments        :
*     Return an error code to the caller's creator, and block.
*
*END*-----------------------------------------------------------------*/

void RTCS_task_exit
   (
      pointer creator,
      uint_32 error
   )
{ /* Body */

   RTCS_task_resume_creator(creator, error);
   _Set_task_error_code(error);
   _Block();

} /* Endbody */


/* EOF */
