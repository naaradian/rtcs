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
#include <mem_prv.h>
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
   ((TD_STRUCT_PTR)_task_get_td(0))->TASK_TEMPLATE_PTR->TASK_ADDRESS = (void(_CODE_PTR_)(uint_32))task->start;

   task->start(task->arg, task);

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
	
//	for(;;){} //t

//	return (uint_32)0x555555l;  //t



   TASK_TEMPLATE_STRUCT    task_template;
   struct rtcs_task_state  task;

   RTCS_sem_init(&task.sem);
   task.start = start;
   task.arg   = arg;
   task.error = RTCS_OK;
 

   _mem_zero((uchar_ptr)&task_template, sizeof(task_template));

   task_template.TASK_NAME          = name;
   task_template.TASK_PRIORITY      = priority;
   task_template.TASK_STACKSIZE     = stacksize;
   task_template.TASK_ADDRESS       = RTCS_task;

//   task_template.TASK_NAME          = "tcpip";
//   task_template.TASK_PRIORITY      = 7;
//   task_template.TASK_STACKSIZE     = 0x4000l;
 //  task_template.TASK_ADDRESS       = RTCS_task;
//   task_template.CREATION_PARAMETER = (uint_32)&task;




   task_template.CREATION_PARAMETER = (uint_32)&task;

 //  task.error = 0; //

   if (_task_create1(0, 0, (uint_32)&task_template) == MQX_NULL_TASK_ID) {    //hier do not work
 //  if (_task_create(0, 0, (uint_32)&task_template) == MQX_NULL_TASK_ID) {    //hier do not work
      RTCS_sem_destroy(&task.sem);
 //  return 0x555555l; //t
      return RTCSERR_CREATE_FAILED;
   } /* Endif */
 //  task.error = 0; //ok
 // return 0;//___________________________________my

   RTCS_sem_wait1(&task.sem); // hier hangt!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 //return 0; //______________________________________my
   RTCS_sem_destroy(&task.sem);
  // return 0x555555l;  //t
// task.error = 0; //t ok
   return task.error;
//return 0;
//   return 0;//___________________________________my
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_create1
* Returned Values : uint_32 (error code)
* Comments        :
*     Create a task and wait for it to complete initialization.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_task_create1
   (
      char_ptr          name,
      uint_32           priority,
      uint_32           stacksize,
      void (_CODE_PTR_  start)(pointer, pointer),
      pointer           arg
   )
{ /* Body */
	
//	for(;;){} //t

//	return (uint_32)0x555555l;  //t
// printf("\n\r rtcs task_create1-0");


   TASK_TEMPLATE_STRUCT    task_template;
   struct rtcs_task_state  task;

   RTCS_sem_init(&task.sem);
   task.start = start;
   task.arg   = arg;
   task.error = RTCS_OK;
 

   _mem_zero((uchar_ptr)&task_template, sizeof(task_template));

   task_template.TASK_NAME          = name;
   task_template.TASK_PRIORITY      = priority;
   task_template.TASK_STACKSIZE     = stacksize;
   task_template.TASK_ADDRESS       = RTCS_task;

//   task_template.TASK_NAME          = "tcpip";
//   task_template.TASK_PRIORITY      = 7;
//   task_template.TASK_STACKSIZE     = 0x4000l;
 //  task_template.TASK_ADDRESS       = RTCS_task;
//   task_template.CREATION_PARAMETER = (uint_32)&task;




   task_template.CREATION_PARAMETER = (uint_32)&task;

 //  task.error = 0; //
// printf("\n\r rtcs task_create1-1");
   if (_task_create2(0, 0, (uint_32)&task_template) == MQX_NULL_TASK_ID) {    //hier do not work
 //  if (_task_create(0, 0, (uint_32)&task_template) == MQX_NULL_TASK_ID) {    //hier do not work
      RTCS_sem_destroy(&task.sem);
 //  return 0x555555l; //t
      return RTCSERR_CREATE_FAILED;
   } /* Endif */
 //  task.error = 0; //ok
 // return 0;//___________________________________my

   RTCS_sem_wait1(&task.sem); // hier hangt!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 // RTCS_sem_wait(&task.sem); // hier hangt!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 //return 0; //______________________________________my
   RTCS_sem_destroy(&task.sem);
  // return 0x555555l;  //t
// task.error = 0; //t ok
// printf("\n\r rtcs task_create1-2");
   return task.error;
//return 0;
//   return 0;//___________________________________my
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
   _task_set_error(error);
   _task_block();

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_destroy
* Returned Values : void
* Comments        :
*     Destroy a task.
*
*END*-----------------------------------------------------------------*/

void RTCS_task_destroy
   (
      _rtcs_taskid   taskid
   )
{ /* Body */

   _task_destroy(((TD_STRUCT_PTR)taskid)->TASK_ID);

} /* Endbody */


/* EOF */
