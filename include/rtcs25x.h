#ifndef __rtcs25x_h__
#define __rtcs25x_h__
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
*** File: rtcs25x.h
***
*** Comments:  This file contains the definitions required to
***            use RTCS with MQX2.5X.
***
************************************************************************
*END*******************************************************************/

#include <mqx.h>
#include <psp.h>
#include <message.h>
#include <fio.h>
#include <serial.h>
#include <part.h>
#include <mqx_ioc.h>
#include <log.h>
#include <klog.h>
#include <pcb.h>


#ifdef __MQX__

#define RTCS_get_data()  _mqx_get_io_component_handle(IO_RTCS_COMPONENT)
#define RTCS_set_data(p) _mqx_set_io_component_handle(IO_RTCS_COMPONENT,p)

#define SNMP_get_data()  _mqx_get_io_component_handle(IO_SNMP_COMPONENT)
#define SNMP_set_data(p) _mqx_set_io_component_handle(IO_SNMP_COMPONENT,p)


/***************************************
**
** Message passing
*/

extern boolean _msgq_send_blocked_internal(pointer);
#define RTCS_msgq_send_blocked(a,b)    _msgq_send_blocked_internal(a)
#define RTCS_msgq_send(a,b)            _msgq_send(a)
#define RTCS_msgq_get_id               _msgq_get_id
#define RTCS_msg_free                  _msg_free
#define RTCS_msg_alloc                 _msg_alloc
#define RTCS_msgq_receive(a,b,c)       _msgq_receive(a,b)
#define RTCS_msgpool_create            _msgpool_create
#define RTCS_msgpool_destroy           _msgpool_destroy
#define RTCS_msgq_open                 _msgq_open
#define RTCS_msgq_close                _msgq_close


/***************************************
**
** PPP Memory management
*/

#define PPP_memzero(ptr, bsize)        _mem_zero(ptr, bsize)
#define PPP_memcopy(src, dest, bsize)  _mem_copy(src, dest, bsize)
#define PPP_memalloc(bsize)            _mem_alloc_zero(bsize)
#define PPP_memfree(ptr)               _mem_free(ptr)


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
** Partitions
*/

typedef struct {
   _partition_id        PART;
   uint_32              SIZE;
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


/***************************************
**
** Synchronization
*/

#define _rtcs_sem             LWSEM_STRUCT

#define RTCS_sem_init(p)      _lwsem_create(p, 0)
#define RTCS_sem_destroy(p)   _lwsem_destroy(p)
#define RTCS_sem_post(p)      _lwsem_post(p)
#define RTCS_sem_wait(p)      _lwsem_wait(p)
#define RTCS_sem_wait1(p)      _lwsem_wait1(p)
#define RTCS_sem_trywait(p)   (_lwsem_poll(p) ? RTCS_OK : RTCS_ERROR)


/***************************************
**
** Mutual exclusion
*/

#define _rtcs_mutex           LWSEM_STRUCT

#define RTCS_mutex_init(p)    _lwsem_create(p, 1)
#define RTCS_mutex_destroy(p) _lwsem_destroy(p)
#define RTCS_mutex_lock(p)    _lwsem_wait(p)
#define RTCS_mutex_unlock(p)  _lwsem_post(p)


/***************************************
**
** Messaging
*/

#ifndef RTCSMQ_SIZE
#define RTCSMQ_SIZE  16
#endif

typedef struct {
   LWSEM_STRUCT      EMPTY;   /* Semaphore for signaling senders */
   LWSEM_STRUCT      FULL;    /* Semaphore for signaling the receiver */
   uint_32           HEAD;    /* Index of the oldest message in the queue */
   uint_32           TAIL;    /* Index of the first free slot in the queue */
   pointer           QUEUE[RTCSMQ_SIZE];  /* The message queue */
} _rtcs_msgqueue;

extern uint_32 RTCS_msgqueue_create  (_rtcs_msgqueue _PTR_);
extern void    RTCS_msgqueue_send    (_rtcs_msgqueue _PTR_, pointer);
extern int_32  RTCS_msgqueue_trysend (_rtcs_msgqueue _PTR_, pointer);
extern pointer RTCS_msgqueue_receive (_rtcs_msgqueue _PTR_, uint_32 timeout);
extern void    RTCS_msgqueue_close   (_rtcs_msgqueue _PTR_);


/***************************************
**
** Task management
*/

typedef pointer   _rtcs_taskid;

#define RTCS_task_getid()  _task_get_td(0)

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

extern void RTCS_task_destroy (_rtcs_taskid);


/***************************************
**
** I/O
*/

#define RTCS_EACCES MQX_EACCES
#define RTCS_ENOENT MQX_ENOENT
#define RTCS_EEXIST MQX_EEXIST

extern FILE_PTR RTCS_io_open(char_ptr, char_ptr, uint_32 _PTR_);

#define RTCS_io_read(f,p,l)   ((int_32)read(f,(char_ptr)(p),l))
#define RTCS_io_write(f,p,l)  ((int_32)write(f,(char_ptr)(p),l))
#define RTCS_io_close(f)      fclose(f)

extern int_32 _io_socket_install(char_ptr);
extern int_32 _io_telnet_install(char_ptr);


/***************************************
**
** Logging
*/

#define RTCSLOG_TYPE_FNENTRY  KLOG_FUNCTION_ENTRY
#define RTCSLOG_TYPE_FNEXIT   KLOG_FUNCTION_EXIT
#define RTCSLOG_TYPE_PCB      6
#define RTCSLOG_TYPE_TIMER    7

#define RTCSLOG_FNBASE        0x00200000

extern void _klog_log(uint_32, uint_32, uint_32, uint_32, uint_32, uint_32);

#define RTCS_log_internal(type,p1,p2)  _klog_log(type,p1,p2,0,0,0)


#endif /* __MQX__ */


#endif
/* EOF */
