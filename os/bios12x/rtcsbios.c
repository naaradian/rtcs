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
*** File: rtcsbios.c
***
*** Comments:
***   This file contains the stub function calls using TI DSP/BIOS
***   operating system.
***
************************************************************************
*END*******************************************************************/

#include <rtcsbios.h>

extern "C" void printEthLongHex(unsigned long);

/*
**   Global variables
*/
extern uint_32  _RTCSQUEUE_base ;
uint_32  _PPPQUEUE_base = 0x11 ;

pointer _RTCS_io_component_handle = NULL;
pointer _SNMP_io_component_handle = NULL;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _task_create
* Returned Value   : _task_id -- the created task id or NULL_TASK_ID
* Comments         :
*    This function will create a new task of the type specified by the
*    task template number.
*
*END*-----------------------------------------------------------------*/

_task_id _task_create
   (
      /* [IN] the processor upon which to create the task */
      _processor_number processor_number,

      /* [IN] the task template index number for this task */
      uint_32           template_index,

      /* [IN] the parameter to pass to the newly created task */
      uint_32           parameter
   )
{ /* Body */
    TSK_Attrs attrs;
    SEM_Attrs sem_attrs;
    TASK_TEMPLATE_STRUCT_PTR task_template_ptr;
    TSK_Handle task_handle = MQX_NULL_TASK_ID;

    attrs = TSK_ATTRS;  /* initialize with default value */
    sem_attrs = SEM_ATTRS;

    if (template_index == 0) {
        /* Task template is pointed to by the parameter */
      task_template_ptr = (TASK_TEMPLATE_STRUCT_PTR)parameter;

      /* set the task attributes */
      attrs.priority = (Int)task_template_ptr->TASK_PRIORITY;
      attrs.stacksize = (Uns)task_template_ptr->TASK_STACKSIZE;	
      attrs.stackseg = _STACK_ALLOC_segid;	
      attrs.name = (String)task_template_ptr->TASK_NAME;		
      attrs.environ = (Ptr)SEM_create(0,&sem_attrs);	
      task_handle = TSK_create(task_template_ptr->TASK_ADDRESS,
         &attrs,(Arg)task_template_ptr->CREATION_PARAMETER);

    } /* Endif */
    return task_handle;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _task_set_error
* Returned Value   : uint_32
*                  :   The old value of the error code
* Comments         :
*
*END*-----------------------------------------------------------------*/

uint_32 _task_set_error
   (
      /* [IN] the new error code */
      uint_32 new_error_code
   )
{ /* Body */
   uint_32 old_error;
   TSK_Handle task;

   task = TSK_self(); /* return the handle of current task */
   old_error = (uint_32)TSK_geterr(task);
   TSK_seterr(task,(Int)new_error_code);
   return old_error;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _task_block
*
*END*-----------------------------------------------------------------*/

void _task_block(void)
{ /* Body */
   TSK_Handle task;
   pointer environ;

   task = TSK_self(); /* return the handle of current task */
   environ = TSK_getenv(task);
   SEM_pend((SEM_Handle)environ,SYS_FOREVER);
  // printEthLongHex(1);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _task_ready
* Returned Value   : void
* Comments         :
*
*END*-----------------------------------------------------------------*/

void _task_ready
   (
      /* [IN] the address of the task handle to add */
      pointer  task_handle
   )
{ /* Body */
   TSK_Handle task;
   Ptr environ;

   task = (TSK_Handle)task_handle;
   environ = TSK_getenv(task);
   SEM_post((SEM_Handle)environ);
// printEthLongHex(2);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _task_destroy
* Returned Value   : uint_32 error code
* Comments         :
*
*END*-----------------------------------------------------------------*/

uint_32 _task_destroy
   (
      /* [IN] the task id of the task to destroy */
      _task_id task_id
   )
{ /* Body */
   TSK_delete(task_id);
   return(MQX_OK);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _task_get_id
* Returned Value   : uint_32
* Comments         :
*    Returns the current task's task id
*
*END*-----------------------------------------------------------------*/

_task_id _task_get_id
   (
      void
   )
{ /* Body */
   return TSK_self() ;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _task_get_td
* Returned Value   : TD_STRUCT *
* Comments         :
*
*END*-----------------------------------------------------------------*/

pointer _task_get_td
   (
      /* [IN] the task id whose task descriptor address is required */
      _task_id task_id
   )
{ /* Body */
   /* RTCS library always wants the the self id */
   if (task_id == 0) {
      return (pointer)TSK_self() ;
   } /* Endif */

   return (pointer)task_id;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _task_get_error
* Returned Value   : uint_32
* Comments         :
*
*END*-----------------------------------------------------------------*/

uint_32 _task_get_error
   (
      void
   )
{ /* Body */
   TSK_Handle task;

   task = TSK_self(); /* return the handle of current task */
   return (uint_32)(TSK_geterr(task));
} /* Endbody */


/* memory functions */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_test
* Returned Value   : pointer
* Comments         :
*
*END*-----------------------------------------------------------------*/

void  _mem_test
   (
      void
   )
{ /* Body */
  MEM_Stat  statbuf;
  MEM_stat(_MEM_ALLOC_segid,&statbuf);
  printf("B: size= %0X, used= %0X, len=%0X\n",statbuf.size,statbuf.used,statbuf.length);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_alloc_zero
* Returned Value   : pointer
* Comments         :
*    Allocates zero filled memory.
*
*END*-----------------------------------------------------------------*/

pointer _mem_alloc_zero
   (
      /* [IN] the size of the memory block */
      uint_32 size
   )
{ /* Body */
   uint_32_ptr addr;
   Int newsize;

   newsize= (Uns)size+sizeof(uint_32);
   addr=(uint_32_ptr)MEM_calloc(_MEM_ALLOC_segid,newsize,sizeof(uint_32));
   if (addr == MEM_ILLEGAL) {
      return NULL;
   } /* Endif */
   *addr++=(uint_32)newsize;

   return (pointer)(addr);

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : _mem_alloc_system
* Returned Value  : None
* Comments        : allocates memory that is available system wide.
*
*END*-----------------------------------------------------------------*/

pointer _mem_alloc_system
   (
      /* [IN] the size of the memory block */
      uint_32 size
   )
{ /* Body */
   uint_32_ptr addr;
   Int newsize;

   newsize= (Uns)size+sizeof(uint_32);
   addr=(uint_32_ptr)MEM_alloc(_MEM_ALLOC_segid,newsize,sizeof(uint_32));
   if (addr == MEM_ILLEGAL) {
      return NULL;
   } /* Endif */
   *addr++=(uint_32)newsize;

   return (pointer)(addr);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : _mem_alloc_system_zero
* Returned Value  : None
* Comments        : allocates memory from the system, zeroed
*
*END*-----------------------------------------------------------------*/

pointer _mem_alloc_system_zero
   (
      /* [IN] the size of the memory block */
      uint_32 size
   )
{ /* Body */
   uint_32_ptr addr;
   Int newsize;

   newsize= (Uns)size+sizeof(uint_32);
   addr=(uint_32_ptr)MEM_calloc(_MEM_ALLOC_segid,newsize,sizeof(uint_32));
   if (addr == MEM_ILLEGAL) {
      return NULL;
   } /* Endif */
   *addr++=(uint_32)newsize;

   return (pointer)(addr);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_alloc
* Returned Value   : pointer
*    NULL is returned upon error.
* Comments         :
*    Allocates size bytes.
*
*END*-----------------------------------------------------------------*/

pointer _mem_alloc
   (
      /* [IN] the size of the memory block */
      uint_32 size
   )
{ /* Body */

   uint_32_ptr addr;
   Int newsize;

   newsize= (Uns)size+sizeof(uint_32);
   addr=(uint_32_ptr)MEM_alloc(_MEM_ALLOC_segid,newsize,sizeof(uint_32));
   if (addr == MEM_ILLEGAL) {
      return NULL;
   } /* Endif */
   *addr++=(uint_32)newsize;
   return (pointer)(addr);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_free
* Returned Value   : uint_32 MQX_OK or a MQX error code.
* Comments         :
*
*END*-----------------------------------------------------------------*/

uint_32 _mem_free
   (
      /* [IN] the address of the memory block to free */
      pointer mem_ptr
   )
{ /* Body */
   Uns size;
   uint_32_ptr free_ptr;

   free_ptr = (uint_32_ptr)mem_ptr;
   if (mem_ptr == NULL) {
      return  MQX_INVALID_POINTER;
   } /* Endif */
   size=(Uns)*--free_ptr;
   MEM_free(_MEM_ALLOC_segid, (Ptr)free_ptr, size);
   return MQX_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_get_highwater
* Returned Value   : pointer
* Comments         :
*   This function returns the highest memory address ever used in the
*   kernel memory area.
*
*END*-----------------------------------------------------------------*/

pointer _mem_get_highwater
   (
      void
   )
{ /* Body */
   return  NULL;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_zero
* Returned Value   :
* Comments         :
*
*END*-----------------------------------------------------------------*/

void _mem_zero(pointer temp, uint_32 i)
{ /* Body */
   memset(temp,0,(size_t)i);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_copy
* Returned Value   :
* Comments         :
*
*END*-----------------------------------------------------------------*/

void _mem_copy(pointer src_ptr, pointer  dest_ptr, uint_32 num_bytes)
{ /* Body */
   memcpy(dest_ptr,src_ptr,(size_t)num_bytes);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
 *
 * Function Name    : _mem_transfer
 * Returned Value   : uint_32 MQX_OK, or a mqx error code.
 * Comments         :
 *
*END*-----------------------------------------------------------------*/

uint_32 _mem_transfer
   (
      /* [IN] the memory block whose ownership is to be transferred */
      pointer  memory_ptr,

      /* [IN] the source (owner) task id */
      _task_id source_id,

      /* [IN] the target (new owner) task id */
      _task_id target_id
   )
{ /* Body */
   return MQX_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_sum_ip
* Returned Value   :
* Comments         :
*
*END*-----------------------------------------------------------------*/

uint_16 _mem_sum_ip
   (
      uint_16     sum,
         /* [IN] initial sum */
      uint_16     length,
         /* [IN] length of the buffer */
      pointer     buffer
         /* [IN] buffer to checksum */
   )
{ /* Body */
   register uchar_ptr buf = (uchar_ptr)buffer;
   register uint_32 total = (uint_32)(sum & 0xFFFF);
            boolean odd;

   odd = (length & 1);
   length >>= 1;

   while (length--) {
      total += (uint_32)ntohs(buf);
      buf += 2;
   } /* Endwhile */

   if (odd) {
      total += (uint_32)(ntohs(buf) & 0xFF00);
   } /* Endif */

   sum = (uint_16)((total >> 16) & 0xFFFF);
   total = (total & 0xFFFF) + (uint_32)sum;
   sum = (total >> 16) & 0xFFFF;
   total = (total & 0xFFFF) + (uint_32)sum;
   return ((uint_16)total & 0xFFFF);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
 *
 * Function Name    : _time_delay
 * Returned Value   : void
 * Comments         :
 *   This function puts a task on the timeout queue for the specified number
 * of milliseconds, or until removed by another task.
 *
*END*-----------------------------------------------------------------*/

void _time_delay
   (
      /* [IN] the number of milliseconds to delay */
      register uint_32 milliseconds
   )
{ /* Body */
   _mqx_uint ticks;
   ticks = ((_mqx_uint)milliseconds * CLK_countspms()) / CLK_getprd();
   TSK_sleep(ticks);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _time_get
* Returned Value   : void
* Comments         :
*   This function retrieves the number of seconds and milliseconds since
*   the processor started.  (without any time offset information)
*
*END*-----------------------------------------------------------------*/

void _time_get
   (
      /* [IN/OUT] the address where the time is to be written */
      register TIME_STRUCT_PTR time_ptr
   )
{ /* Body */
   uint_32  kernel_time;
   /* calculate the time in milli seconds */
   /* Note : CLk_getltime will wrap back approximately every 49.7 days */
   kernel_time = ((uint_32)CLK_getltime() * (uint_32)CLK_getprd()) / (uint_32)CLK_countspms();

   time_ptr->SECONDS = (uint_32)(kernel_time/1000);
   time_ptr->MILLISECONDS = (uint_32) (kernel_time%1000);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _time_get_elapsed
* Returned Value   : void
* Comments         :
*   This function retrieves the number of seconds and milliseconds since
*   the processor started.  (without any time offset information)
*
*END*-----------------------------------------------------------------*/

void _time_get_elapsed
   (
      /* [IN/OUT] the address where the time is to be put */
      TIME_STRUCT_PTR time_ptr
   )
{ /* Body */
   _time_get(time_ptr);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _time_get_microseconds
* Returned Value   : uint_16 microseconds
* Comments         :
*
*END**********************************************************************/

uint_16 _time_get_microseconds
   (
      void
   )
{ /* Body */
   /*
   ** High resolution time is the number of times the timer counter register
   ** has been incremented. When clock manager is enabled the timer counter will
   ** increment in every 4 clock cycles
   */
   /* In RTCS _time_get_microseconds is used for random number generation */
   return (uint_16)CLK_gethtime();

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  RTCS_msgpool_create
* Returned Value  :  _pool_id of the named pool, or MSGPOOL_NULL_POOL_ID on error
* Comments        :
*
*END*-----------------------------------------------------------------*/

_pool_id  RTCS_msgpool_create
   (
      /*  [IN]  size of the messages being created  */
      uint_16  message_size,

      /*  [IN]  initial number of messages in this pool  */
      uint_16  num_messages,

      /*  [IN]  number of messages to grow pool by if empty */
      uint_16  grow_number,

      /*  [IN]  maximum number of messages allowed in pool */
      uint_16  grow_limit
   )
{ /* Body */
   MBX_Attrs  attrs;
   MBX_Handle mbx; /* mailbox object handle */

   attrs = MBX_ATTRS;
   attrs.segid = _MESSAGE_ALLOC_segid;
   message_size +=sizeof(uint_32); /* allocate space to store the message length */
   mbx = MBX_create(message_size,grow_limit,&attrs);
   if(mbx == NULL){
      return MSGPOOL_NULL_POOL_ID;
   } /* Endif */
   return (_pool_id)mbx;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msgq_open
* Returned Value  : _queue_id - indicating successful queue creation,
*   a NULL indicates error
*
*END*-----------------------------------------------------------------*/

_queue_id RTCS_msgq_open
   (
      /* [IN] the queue being opened */
      _queue_number queue,

      /* [IN] the maximum number of entries allowed in this queue */
      uint_16       max_queue_size
   )
{ /* Body */
    /* PPP driver is try to allocate from the system queue */
   if (queue == 0) {
      return _PPPQUEUE_base;
   }else {
      return  queue;   /* this is normally TCPIPQUEUE */
   } /* Endif */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msgq_get_id
* Returned Value  : _queue_id - the constructed QUEUE ID
*
*END*-----------------------------------------------------------------*/

_queue_id RTCS_msgq_get_id
   (
      /* [IN] the processor on which the queue exists */
      _processor_number  processor_number,

      /* [IN] the queue number */
      _queue_number      queue_number
   )
{ /* Body */
   return ((_queue_id)queue_number);   /* this is normally TCPIPQUEUE */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  RTCS_msg_alloc
* Returned Value  :  pointer to a message
*
*END*-----------------------------------------------------------------*/

pointer RTCS_msg_alloc
   (
      /* [IN]  the pool from which to get the message */
      _pool_id      pool
   )
{ /* Body */

   MBX_Handle mbx;        /* mailbox object handle */
   uint_32_ptr addr;
   Uns newsize;

   mbx = (MBX_Handle)pool;
   newsize= mbx->size;
   addr=MEM_alloc(_MESSAGE_ALLOC_segid,(_mqx_uint)newsize,sizeof(uint_32));
   if (addr == MEM_ILLEGAL) {
      return NULL;
   } /* Endif */
   *addr++=(uint_32)newsize;
   return (pointer)(addr);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msg_free
* Returned Value  : None
* Comments        : mark the specified message as 'free'
*
*END*-----------------------------------------------------------------*/

void  RTCS_msg_free
   (
      /*  [IN]   pointer to a message struct which is to be released  */
      pointer msg_ptr
   )
{ /* Body */
    _mem_free(msg_ptr);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  RTCS_msgq_send
* Returned Value  :  boolean, indicating validity of queue_id
* Comments        :  Verify the input queue_id and try to send the
*                    message directly.
*
*   Messages can be sent by tasks and ISRs.
*
*END*-----------------------------------------------------------------*/

boolean  RTCS_msgq_send
   (
      /* [IN]  pointer to the  message being sent by application */
      pointer input_msg_ptr,
      /* [IN]  the pool to which message need to send */
      _pool_id      pool
   )
{ /* Body */
   MESSAGE_HEADER_STRUCT_PTR msg;
   boolean status = TRUE;

   msg = (MESSAGE_HEADER_STRUCT_PTR)input_msg_ptr;
   status = MBX_post((MBX_Handle)pool,msg,SYS_FOREVER);
   RTCS_msg_free((pointer)msg);
   return status;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msgq_receive
* Returned Value  : pointer to a message structure, or NULL if timeout occurred.
* Comments        : Dequeue the first item from
*
*END*-----------------------------------------------------------------*/

pointer RTCS_msgq_receive
   (
      /* [IN]  id of the queue from which a message is to be received */
     _queue_id queue_id,

      /* [IN]  indication of the number of millisecs which can expire before
      **       this request times out
      */
      uint_32        timeout,
      /* [IN]  the pool to which message need to send */
      _pool_id      pool

   )
{ /* Body */
   pointer  msg;
   MBX_Obj mbx;
   MBX_Handle mbx_pool = (MBX_Handle)pool;

   if(timeout==0){
      timeout=SYS_FOREVER;
   }else{
      timeout = (timeout * (uint_32)CLK_countspms()) / (uint_32)CLK_getprd();
   } /* Endif */

   mbx.size = mbx_pool->size;
   msg = RTCS_msg_alloc((_pool_id)&mbx);

   if(MBX_pend(mbx_pool,msg,(Uns)timeout)==FALSE){
      RTCS_msg_free(msg);
      return NULL;
   }
   return(msg);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  RTCS_msgpool_destroy
* Returned Value  :  uint_32 MQX_OK, or a MQX error code.
* Comments        :
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_msgpool_destroy
   (
      /*  [IN]  the pool to delete */
      _pool_id pool_id
   )
{ /* Body */
 MBX_delete((MBX_Handle)pool_id);
 return MQX_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  RTCS_msgq_send_blocked
* Returned Value  :  boolean, indicating validity of queue_id
* Comments        :  Verify the input queue_id and try to send the
*                    message directly.
*
*  This can only be used by tasks
*  The calling task is blocked after the send.
*
*END*-----------------------------------------------------------------*/

boolean  RTCS_msgq_send_blocked
   (
      /* [IN]  pointer to the  message being sent by application */
      pointer input_msg_ptr,
      /* [IN]  the pool to which message need to send */
      _pool_id      pool
   )
{ /* Body */
   MESSAGE_HEADER_STRUCT_PTR msg;
   boolean status = TRUE;

   msg = (MESSAGE_HEADER_STRUCT_PTR)input_msg_ptr;
   status = MBX_post((MBX_Handle)pool,msg,SYS_FOREVER);
   RTCS_msg_free((pointer)msg);
   _task_block();   /* block the calling task */
   return status;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _lwsem_create
* Returned Value   : a lwsem handle
*   queue
* Comments         :
*   This function initializes a light weight semaphore.
*
*END*-----------------------------------------------------------------*/

uint_32 _lwsem_create
   (
      /* [IN] the address of the semaphore to initialize */
      LWSEM_STRUCT_PTR sem_ptr,

      /* [IN] the inital number of semaphores available  */
      int_32 initial_number
   )
{ /* Body */
  SEM_Handle sem;
  SEM_Attrs sem_attrs;

  sem_attrs = SEM_ATTRS;
  sem = SEM_create((Int)initial_number,&sem_attrs);	
  if(sem == NULL){
   sem_ptr->VALID = FALSE;
   return SEM_CREATE_FAILED;
  }/* endif */
  sem_ptr->VALID = TRUE;
  sem_ptr->VALUE = (uint_32_ptr)sem;
  return(MQX_OK);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _lwsem_destroy
* Returned Value   : error code
* Comments         :
*   This function deletes a light weight semaphore.
*
*END*-----------------------------------------------------------------*/

uint_32 _lwsem_destroy
   (
      /* [IN] the semaphore handle */
      LWSEM_STRUCT_PTR sem_ptr
   )
{ /* Body */
  SEM_Handle sem;

  if(sem_ptr->VALID){
     sem = (SEM_Handle)sem_ptr->VALUE;
     SEM_delete(sem);
     sem_ptr->VALID = 0;
     return(MQX_OK);
  } else {
     return SEM_INVALID_SEMAPHORE; ;
  } /* endif */
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _lwsem_post
* Returned Value   : an error code
*   queue
* Comments         :
*   This function posts a semaphore to the lwsem.  If any tasks are waiting
* for semaphores, the first available task is made ready.
*
*END*-----------------------------------------------------------------*/

uint_32 _lwsem_post
   (
      /* [IN] the semaphore address */
      LWSEM_STRUCT_PTR  sem_ptr
   )
{ /* Body */
   SEM_Handle sem;

   if(sem_ptr->VALID){
      sem = (SEM_Handle)sem_ptr->VALUE;
      SEM_post(sem);
      return(MQX_OK);
   } else {
      return SEM_INVALID_SEMAPHORE; ;
   } /* endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _lwsem_wait
* Returned Value   : an error code
*   queue
* Comments         :
*
*END*-----------------------------------------------------------------*/

uint_32 _lwsem_wait
   (
      /* [IN] the semaphore address */
      LWSEM_STRUCT_PTR sem_ptr
   )
{ /* Body */
   SEM_Handle sem;

   if(sem_ptr->VALID){
      sem = (SEM_Handle)sem_ptr->VALUE;
      SEM_pend(sem,SYS_FOREVER);
      return(MQX_OK);
   } else {
      return SEM_INVALID_SEMAPHORE; ;
   } /* endif */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_sem_trywait
* Returned Value   : RTCS_OK if the semaphore is obtained,
*                    otherwise RTCS_ERROR if not available
* Comments         :
*   This function obtains a semaphore from the _rtcs_sem if one is
* available returning 0, otherwise it returns -1.  This call does
* not block.
*
*END*-----------------------------------------------------------------*/

extern int_32 RTCS_sem_trywait
   (
      _rtcs_sem _PTR_ sem_ptr
   )

{ /* Body */
   SEM_Handle sem;
   uint_32  result = -1;

   if(sem_ptr->VALID){
      sem = (SEM_Handle)sem_ptr->VALUE;
      if ((SEM_pend(sem,0))) {
         result = 0;
      } /* Endif */
   } /* Endif */
   return result;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mqx_get_io_component_handle
* Returned Value   : pointer handle
* Comments         :
*    This function returns the I/O Component handle for the specified
*    I/O component
*
*END*-----------------------------------------------------------------*/

pointer _mqx_get_io_component_handle
   (
      /* [IN] the component number */
      uint_32 component
   )
{ /* Body */
   if (component == IO_RTCS_COMPONENT) {
      return _RTCS_io_component_handle;
   }else if (component == IO_SNMP_COMPONENT) {
      return _SNMP_io_component_handle;
   }else{
      return NULL;
   } /* Endif */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mqx_set_io_component_handle
* Returned Value   : pointer handle
* Comments         :
*    This function sets the I/O Component handle for the specified
*
*END*-----------------------------------------------------------------*/

pointer _mqx_set_io_component_handle
   (
      /* [IN] the component number */
      uint_32 component,

      /* [IN] the new handle */
      pointer handle
   )
{ /* Body */
   if (component == IO_RTCS_COMPONENT) {
       _RTCS_io_component_handle = handle;
   }else if (component == IO_SNMP_COMPONENT) {
       _SNMP_io_component_handle = handle;
   }else{
      return 0;
   } /* Endif */
   return 0;
} /* Endbody */


/* EOF */

