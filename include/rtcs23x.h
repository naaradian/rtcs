#ifndef __rtcs23x_h__
#define __rtcs23x_h__
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
*** File: rtcs23x.h
***
*** Comments:  This file contains the definitions required to
***            use RTCS with MQX2.3x
***
************************************************************************
*END*******************************************************************/

#include <mqx.h>
#include <kernel.h>

#include <klog.h>


#define MQX_VERSION  233


/****************************************
**
** Data type definitions
*/
typedef unsigned char  uint_8, _PTR_  uint_8_ptr;  /* 8-bit unsigned integer */

typedef int_32       _mqx_int;
typedef uint_32      _mqx_uint;
typedef uint_32_ptr  _mqx_uint_ptr;

/* Changes to mqx error codes */
#define MQX_OK                   OK
#define MQX_NULL_TASK_ID         NULL_TASK_ID
#define MSGPOOL_NULL_POOL_ID     NULL_POOL_ID

#define IO_SERIAL_RAW_IO              (0)
#define IO_SERIAL_XON_XOFF            (1)
#define IO_SERIAL_TRANSLATION         (2)
#define IO_SERIAL_ECHO                (4)

#define IO_STDIN                 STDIN
#define IO_STDOUT                STDOUT



/* Changes to MQX functions */
#define _mem_copy                   _Copy
#define _mem_zero                   _Zeromem
#define _time_get                   _Get_time
#define _io_set_handle              _Set_stdio
#define _time_delay                 _Timeout
#define _task_get_td(p)             _Convert_to_td(((p)==0)?_My_id():(p))
#define RTCS_msgq_get_id            _Convert_to_qid
#define RTCS_msgpool_create         _Create_named_pool
#define RTCS_msgpool_destroy        _Destroy_named_pool
#define RTCS_msg_alloc              _Get_named_buffer
#define RTCS_msgq_send_blocked(a,b) _Send_message_blocked(a)
#define _task_get_id                _My_id
#define _task_ready                 _Add_ready
#define RTCS_msgq_send(a,b)         _Send_message(a)
#define _task_get_error             _Task_error_code
#define _time_set                   _Set_time
#define _time_get_elapsed           _Get_kernel_time
#define _mem_transfer               _Transfer_memory_block
#define _mqx_get_kernel_data        _Get_kernel_data
#define RTCS_msgq_open              _Open_queue
#define RTCS_msgq_receive(a,b,c)    _Receive_message(a,b)
#define RTCS_msg_free               _Free_buffer
#define RTCS_msgq_close             _Close_queue
#define _int_disable                _Disable
#define _time_get_microseconds      _MQX_Get_microseconds


#define RTCS_get_data()  _mqx_get_io_component_handle(IO_RTCS_COMPONENT)
#define RTCS_set_data(p) _mqx_set_io_component_handle(IO_RTCS_COMPONENT,p)

#define SNMP_get_data()  _mqx_get_io_component_handle(IO_SNMP_COMPONENT)
#define SNMP_set_data(p) _mqx_set_io_component_handle(IO_SNMP_COMPONENT,p)

/* Obtain the processor number from the task ID */
#define PROC_NUMBER_FROM_TASKID(taskid) ((uint_16)((taskid) >> 16))

/* Obtain the task number from the task ID */
#define TASK_NUMBER_FROM_TASKID(taskid) ((uint_32)((taskid) & 0xFFFF))


/***************************************
**
** PPP Memory management
*/

#define PPP_memzero(ptr, bsize)        _Zeromem(ptr, bsize)
#define PPP_memcopy(src, dest, bsize)  _Copy(src, dest, bsize)
#define PPP_memalloc(bsize)            _Getvec(bsize)
#define PPP_memfree(ptr)               _Freevec(ptr)


/***************************************
**
** PPP Mutual exclusion
*/

#define _ppp_mutex            LWSEM_STRUCT

#define PPP_mutex_init(p)     _lwsem_create(p, 1)
#define PPP_mutex_destroy(p)  _lwsem_destroy(p)
#define PPP_mutex_lock(p)     _lwsem_wait(p)
#define PPP_mutex_unlock(p)   _lwsem_post(p)



/***************************************
**
** Time
*/

#define RTCS_time_delay       _time_delay
#define RTCS_get_milliseconds RTCS_time_get

extern uint_32 RTCS_time_get     (void);
extern uint_32 RTCS_time_get_sec (void);



/***************************************
**
** Date
*/

extern void RTCS_date_get (uint_32_ptr, uint_32_ptr);
extern void RTCS_date_set (uint_32, uint_32);



/*--------------------------------------------------------------------------*/
/*
**                         _rtcs_sem STRUCTURE
**
** This structure defines a light weight semaphore.
** These sempahores implement a simple counting semaphore.
** Tasks wait on these semaphores in a FIFO manner.
** Priority inheritance is NOT implemented for these semaphores.
** The semaphores can be embedded into data structures similarly to
** mutexes.
*/

typedef struct rtcs_td_queue
{
   struct rtcs_td_queue _PTR_ NEXT;
   struct rtcs_td_queue _PTR_ PREV;
   TD_STRUCT_PTR              TD;
} RTCS_TD_Q, _PTR_ RTCS_TD_Q_PTR;

typedef struct rtcs_sem_struct
{

   /* The queue of blocked tasks */
   QUEUE_STRUCT               QUEUE;

   /* the semaphore value */
   int_32                     VALUE;

} RTCS_LWSEM, _PTR_ RTCS_LWSEM_PTR;


/***************************************
**
** Synchronization
*/
#define _rtcs_sem          RTCS_LWSEM
#define LWSEM_STRUCT       RTCS_LWSEM

extern uint_32 RTCS_sem_create(RTCS_LWSEM_PTR, int_32);
extern uint_32 RTCS_sem_destroy(RTCS_LWSEM_PTR);
extern uint_32 RTCS_sem_post(RTCS_LWSEM_PTR);
extern uint_32 RTCS_sem_wait(RTCS_LWSEM_PTR);
extern uint_32 RTCS_sem_trywait(RTCS_LWSEM_PTR);

#define RTCS_sem_init(p) RTCS_sem_create(p, 0)
#define _lwsem_create  RTCS_sem_create
#define _lwsem_destroy RTCS_sem_destroy
#define _lwsem_wait    RTCS_sem_wait
#define _lwsem_post    RTCS_sem_post

/***************************************
**
** Mutual exclusion
*/

#define _rtcs_mutex           _rtcs_sem

#define RTCS_mutex_init(p)    RTCS_sem_create(p, 1)
#define RTCS_mutex_destroy(p) RTCS_sem_destroy(p)
#define RTCS_mutex_lock(p)    RTCS_sem_wait(p)
#define RTCS_mutex_unlock(p)  RTCS_sem_post(p)


/***************************************
**
** Task management
*/

typedef pointer   _rtcs_taskid;

#define RTCS_task_getid()  _Convert_to_td(_My_id())

extern uint_32 RTCS_task_create
(
   char_ptr          name,
   uint_32           priority,
   uint_32           stacksize,
   void (_CODE_PTR_  start)(pointer, pointer),
   pointer           arg
);

extern void RTCS_task_resume_creator (pointer, uint_32);
extern void RTCS_task_exit           (pointer, uint_32);

#define RTCS_task_destroy(x)  _Destroy(((TD_STRUCT_PTR)(x))->TASK_ID)

/***************************************
**
** I/O
*/

#define IO_RTCS_COMPONENT             (1L)
#define IO_SNMP_COMPONENT             (9L)


#define RTCS_EACCES (0x0802L)
#define RTCS_ENOENT (0x0819L)
#define RTCS_EEXIST (0x080bL)

//my #define fopen                  _Fopen
#define  fopen      _io_fopen1 //my
//#define fclose                 _Freevec
#define  fclose      _io_fclose //my
extern int_32                  read(FILE_PTR, char_ptr, int_32);
extern int_32                  write(FILE_PTR, char_ptr, int_32);
#define ioctl(a,b,c)

extern FILE_PTR RTCS_io_open(char_ptr, char_ptr, uint_32 _PTR_);

#define RTCS_io_read(f,p,l)   ((int_32)read(f,(char_ptr)(p),l))
#define RTCS_io_write(f,p,l)  ((int_32)write(f,(char_ptr)(p),l))
#define RTCS_io_close(f)      fclose(f)

extern int_32 _io_socket_install(char_ptr);
extern int_32 _io_telnet_install(char_ptr);


/***************************************
**
** Stub functions
*/
extern pointer          _mqx_get_io_component_handle(uint_32);
extern pointer          _mqx_set_io_component_handle(uint_32, pointer);
extern uint_32          _mem_sum_ip(uint_32, uint_32, pointer);


/***************************************
**
** Memory
*/

extern pointer _mem_alloc_internal(uint_32 size, boolean zero, boolean system);
extern uint_32 _mem_free(pointer);

#define _mem_alloc(s)               _mem_alloc_internal(s, FALSE, FALSE)
#define _mem_alloc_zero(s)          _mem_alloc_internal(s, TRUE,  FALSE)
#define _mem_alloc_system(s)        _mem_alloc_internal(s, FALSE, TRUE)
#define _mem_alloc_system_zero(s)   _mem_alloc_internal(s, TRUE,  TRUE)


/***************************************
**
** Partitions
*/

typedef struct {
   uint_32              BLOCK_SIZE;
   uint_32              NUM_BLOCKS;
   uint_32              GROW_BLOCKS;
   uint_32              MAX_BLOCKS;
   pointer              GROW;
   pointer              FREE;
   int_32 (_CODE_PTR_   CONSTR)(pointer);
   int_32 (_CODE_PTR_   DESTR)(pointer);
} _PTR_ _rtcs_part;

extern _rtcs_part RTCS_part_create(
                     uint_32 size,
                     uint_32 init, uint_32 grow, uint_32 max,
                     int_32 (_CODE_PTR_ cons)(pointer),
                     int_32 (_CODE_PTR_ dest)(pointer)
                  );

extern void    RTCS_part_destroy    (_rtcs_part);
extern pointer RTCS_part_alloc      (_rtcs_part);
extern pointer RTCS_part_alloc_zero (_rtcs_part);
extern void    RTCS_part_free       (pointer);


/***************************************
**
** Logging
*/

#define RTCSLOG_TYPE_FNENTRY  KLOG_FUNCTION_ENTRY
#define RTCSLOG_TYPE_FNEXIT   KLOG_FUNCTION_EXIT
#define RTCSLOG_TYPE_PCB      6
#define RTCSLOG_TYPE_TIMER    7

#define RTCSLOG_FNBASE        0x00200000

#define RTCS_log_internal(type,p1,p2)  _KRNL_Log(type,2,p1,p2)



/***************************************
**
** PCB
*/

#define __pcb_h__

#define __PCB__

typedef struct {
   uint_32           LENGTH;
   uchar_ptr         FRAGMENT;
} PCB_FRAGMENT, _PTR_ PCB_FRAGMENT_PTR;

typedef struct pcb {
   void (_CODE_PTR_  FREE)(struct pcb _PTR_);
   pointer           PRIVATE;
   PCB_FRAGMENT      FRAG[1];
} PCB, _PTR_ PCB_PTR;

#define PCB_free(pcb_ptr)  (*(pcb_ptr)->FREE)(pcb_ptr)

/*
** This set of macros will always work on all processors.
*/

#define htonl(p,x) ((p)[0] = (x) >> 24 & 0xFF, \
                    (p)[1] = (x) >> 16 & 0xFF, \
                    (p)[2] = (x) >>  8 & 0xFF, \
                    (p)[3] = (x)       & 0xFF, \
                    (x))

#define htons(p,x) ((p)[0] = (x) >>  8 & 0xFF, \
                    (p)[1] = (x)       & 0xFF, \
                    (x))

#define htonc(p,x) ((p)[0] = (x)       & 0xFF, \
                    (x))

#define ntohl(p)   ((((uint_32)(p)[0] & 0xFF) << 24) | \
                    (((uint_32)(p)[1] & 0xFF) << 16) | \
                    (((uint_32)(p)[2] & 0xFF) <<  8) | \
                    (((uint_32)(p)[3] & 0xFF)      ))

#define ntohs(p)   ((((uint_16)(p)[0] & 0xFF) <<  8) | \
                    (((uint_16)(p)[1] & 0xFF)      ))

#define ntohc(p)   ((((  uchar)(p)[0] & 0xFF)      ))


#endif
/* EOF */
