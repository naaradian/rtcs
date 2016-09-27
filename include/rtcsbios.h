#ifndef __rtcsbios_h__
#define __rtcsbios_h__
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
*** File: rtcsbios.h
***
*** Comments:
***   This is the mqx type compatablity file for  TI DSP BIOSII  RTOS
***
************************************************************************
*END*******************************************************************/

#include <std.h>
#include <stdio.h>
#include <tsk.h>
#include <sem.h>
#include <mem.h>
#include <clk.h>
#include <mbx.h>
#undef __MQX__

/* Define the processor Specific  types file */
#ifndef __TYPES__
#define __TYPES__
#if (MQX_CPU == 3206711) || (MQX_CPU == 3206211) || (MQX_CPU == 3206701) || (MQX_CPU == 3206201)

/*--------------------------------------------------------------------------*/
/*
**                            STANDARD TYPES
*/

/*
**  The following typedefs allow us to minimize portability problems
**  due to the various C compilers (even for the same processor) not
**  agreeing on the sizes of "int"s and "short int"s and "longs".
*/

#define _PTR_      *
#define _CODE_PTR_ *

typedef char _PTR_                    char_ptr;    /* signed character       */
typedef unsigned char  uchar, _PTR_   uchar_ptr;   /* unsigned character     */

typedef signed   char   int_8, _PTR_   int_8_ptr;   /* 8-bit signed integer   */
typedef unsigned char  uint_8, _PTR_  uint_8_ptr;  /* 8-bit signed integer   */

typedef          short int_16, _PTR_  int_16_ptr;  /* 16-bit signed integer  */
typedef unsigned short uint_16, _PTR_ uint_16_ptr; /* 16-bit unsigned integer*/

typedef          int    int_32, _PTR_  int_32_ptr;  /* 32-bit signed integer  */
typedef unsigned int   uint_32, _PTR_ uint_32_ptr; /* 32-bit unsigned integer*/

typedef unsigned int   boolean;  /* Machine representation of a boolean */

typedef void _PTR_     pointer;  /* Machine representation of a pointer */

/* IEEE single precision floating point number (32 bits, 8 exponent bits) */
typedef float          ieee_single;

/* IEEE double precision floating point number (64 bits, 11 exponent bits) */
typedef double         ieee_double;

/* Type for the CPU's natural size */
typedef          int _mqx_int,  _PTR_ _mqx_int_ptr;  /* Pointer to an _mqx_int */
typedef unsigned int _mqx_uint, _PTR_ _mqx_uint_ptr; /* Pointer to an _mqx_uint */

/*--------------------------------------------------------------------------*/
/*
**                         DATATYPE VALUE RANGES
*/

#define MAX_CHAR      (0x7F)
#define MAX_UCHAR     (0xFF)
#define MAX_INT_8     (0x7F)
#define MAX_UINT_8    (0xFF)
#define MAX_INT_16    (0x7FFF)
#define MAX_UINT_16   (0xFFFF)
#define MAX_INT_32    (0x7FFFFFFFL)
#define MAX_UINT_32   (0xFFFFFFFFUL)

#define MIN_FLOAT     (1.175494E-38)
#define MAX_FLOAT     (3.40282346E+38)

#define MIN_DOUBLE    (2.22507385E-308)
#define MAX_DOUBLE    (1.79769313E+308)


#else
#if (MQX_CPU == 320542) || (MQX_CPU == 320549)

/* For the C54 family of processors the minimun type size is 16 bits */

#define _PTR_      *
#define _CODE_PTR_ *

typedef char _PTR_                    char_ptr;    /* signed character       */
typedef unsigned char  uchar, _PTR_   uchar_ptr;   /* _mqx_uint character     */

typedef signed   char   int_8, _PTR_  int_8_ptr;   /* 8-bit signed integer   */
typedef unsigned char  uint_8, _PTR_  uint_8_ptr;  /* 8-bit signed integer   */

typedef          short int_16,  _PTR_ int_16_ptr;  /* 16-bit signed integer  */
typedef unsigned short uint_16, _PTR_ uint_16_ptr; /* 16-bit _mqx_uint integer*/

typedef          long  int_32, _PTR_  int_32_ptr;  /* 32-bit signed integer  */
typedef unsigned long  uint_32, _PTR_ uint_32_ptr; /* 32-bit _mqx_uint integer*/

typedef void _PTR_     pointer;  /* Machine representation of a pointer */

/* Type for the CPU's natural size */
typedef          int _mqx_int,  _PTR_ _mqx_int_ptr;  /* Pointer to an _mqx_int */
typedef unsigned int _mqx_uint, _PTR_ _mqx_uint_ptr; /* Pointer to an _mqx_uint */

/* How big a data pointer is on this processor */
typedef uint_16  _psp_data_addr, _PTR_ _psp_data_addr_ptr;

#ifdef _FAR_MODE
typedef uint_32  _mqx_max_type, _PTR_ _mqx_max_type_ptr;
typedef uint_32  _psp_code_addr, _PTR_ _psp_code_addr_ptr;
#else
typedef uint_16  _mqx_max_type, _PTR_ _mqx_max_type_ptr;
typedef uint_16  _psp_code_addr, _PTR_ _psp_code_addr_ptr;
#endif

/* _mem_size is equated to the a type that can hold the maximum data address */
typedef uint_16 _mem_size, _PTR_ _mem_size_ptr;

/* Used for file sizes. */
typedef uint_32       _file_size;
typedef int_32        _file_offset;

typedef _mqx_uint     boolean;  /* Machine representation of a boolean */

/* IEEE single precision floating point number (32 bits, 8 exponent bits) */
typedef float         ieee_single;

/*--------------------------------------------------------------------------*/
/*
**                         DATATYPE VALUE RANGES
*/

#define MAX_CHAR             (0x7F)
#define MAX_UCHAR            (0xFF)
#define MAX_INT_8            (0x7F)
#define MAX_UINT_8           (0xFF)
#define MAX_INT_16           (0x7FFF)
#define MAX_UINT_16          (0xFFFF)
#define MAX_INT_32           (0x7FFFFFFFL)
#define MAX_UINT_32          (0xFFFFFFFFUL)

#define MAX_MQX_UINT         (MAX_UINT_16)
#define MAX_MQX_INT          (MAX_INT_16)
#define MAX_FILE_SIZE        (MAX_UINT_16)
#define MAX_MEM_SIZE         (MAX_UINT_16)
#ifdef _FAR_MODE
#define MAX_MQX_MAX_TYPE     (MAX_UINT_32)
#else
#define MAX_MQX_MAX_TYPE     (MAX_UINT_16)
#endif
#define MQX_INT_SIZE_IN_BITS (16)


#define MIN_FLOAT     (1.175494E-38)
#define MAX_FLOAT     (3.40282346E+38)

/* Doesn't support ieee doubles */
#define MIN_DOUBLE    MIN_FLOAT
#define MAX_DOUBLE    MAX_FLOAT

#else
#error "Unknown processor type"
#endif
#endif

#endif   /* End of types */

/*--------------------------------------------------------------------------*/
/*                        DATATYPE DEFINITIONS                          */
typedef TSK_Handle _task_id;           /* what a task_id looks like  */
typedef pointer _pool_id;           /* what a pool_id looks like */
typedef uint_16 _msg_size;          /* what a message size looks like */
typedef uint_16 _queue_number;      /* what a queue number is         */
typedef uint_16 _queue_id;          /* What a queue_id looks like */
typedef uint_16 _processor_number;  /* processor number type */
typedef FILE *   FILE_PTR ;

/*--------------------------------------------------------------------------*/
/*
**                          CONSTANT DECLARATIONS
*/


extern _mqx_int _MEM_ALLOC_segid;
extern _mqx_int _STACK_ALLOC_segid;
extern _mqx_int _MESSAGE_ALLOC_segid;
extern _mqx_int _PARTITION_ALLOC_segid;

/*--------------------------------------------------------------------------*/
/*
**                          STANDARD CONSTANTS
**
**  Note that if standard 'C' library files are included after mqx.h,
**  the defines of TRUE, FALSE and NULL may sometimes conflict, as most
**  standard library files do not check for previous definitions.
*/

#ifdef  FALSE
   #undef  FALSE
#endif
#define FALSE ((boolean)0)

#ifdef  TRUE
   #undef  TRUE
#endif
#define TRUE ((boolean)1)

#ifdef  NULL
   #undef  NULL
#endif
#ifdef __cplusplus
   #define NULL (0)
#else
   #define NULL ((pointer)0)
#endif


/* The IO component indexes, used to index into the component
** arrays to access component specific data
*/
#define IO_RTCS_COMPONENT             (1L)
#define IO_PPP_COMPONENT              (8L)
#define IO_SNMP_COMPONENT             (9L)

#define MQX_EACCES                    (0x0802L)
#define MQX_ENOENT                    (0x0819L)
#define MQX_EEXIST                    (0x080bL)

#define RTCS_EACCES                   MQX_EACCES
#define RTCS_ENOENT                   MQX_ENOENT
#define RTCS_EEXIST                   MQX_EEXIST
#define MQX_NULL_TASK_ID              ((_task_id)0)
#define MSGPOOL_NULL_POOL_ID          ((_pool_id)0)
#define IO_STDIN                      (0L)
#define IO_STDOUT                     (1L)
#define IO_STDERR                     (2L)
#define IO_SERIAL_RAW_IO              (0)
#define IO_SERIAL_XON_XOFF            (1)
#define IO_SERIAL_TRANSLATION         (2)
#define IO_SERIAL_ECHO                (4)
#define IO_IOCTL_CHAR_AVAIL           (0x0006L)
/* Serial I/O IOCTL commands */
#define IO_IOCTL_SERIAL_GET_FLAGS     (0x0101)
#define IO_IOCTL_SERIAL_SET_FLAGS     (0x0102)
#define IO_IOCTL_SERIAL_GET_BAUD      (0x0103)
#define IO_IOCTL_SERIAL_SET_BAUD      (0x0104)
#define IO_IOCTL_SERIAL_GET_STATS     (0x0105)
#define IO_IOCTL_SERIAL_CLEAR_STATS   (0x0106)

#define SEM_INVALID_SEMAPHORE      (0x100CL)
#define SEM_CREATE_FAILED          (0x100DL)
#define TASK_CREATE_FAILED         (0x100EL)
#define SIZE_OF_TCPIP_MESSAGE_STRUCT 64
#define TCPIP_QUEUE     _RTCSQUEUE_base

#define RTCSLOG_TYPE_FNENTRY  (uint_32)1L
#define RTCSLOG_TYPE_FNEXIT   (uint_32)2L
#define RTCSLOG_TYPE_PCB      6
#define RTCSLOG_TYPE_TIMER    7
#define RTCSLOG_FNBASE        0x00200000
#define RTCS_log_internal(type,p1,p2)

/*
**  Function defines
*/

#define RTCS_get_data()  _mqx_get_io_component_handle(IO_RTCS_COMPONENT)
#define RTCS_set_data(p) _mqx_set_io_component_handle(IO_RTCS_COMPONENT,p)

#define SNMP_get_data()  _mqx_get_io_component_handle(IO_SNMP_COMPONENT)
#define SNMP_set_data(p) _mqx_set_io_component_handle(IO_SNMP_COMPONENT,p)


/***************************************
**
** Synchronization
*/

#define _rtcs_sem              LWSEM_STRUCT

#define RTCS_sem_init(p)      _lwsem_create(p, 0)
#define RTCS_sem_destroy(p)   _lwsem_destroy(p)
#define RTCS_sem_post(p)      _lwsem_post(p)
#define RTCS_sem_wait(p)      _lwsem_wait(p)

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
** Task management
*/

typedef TSK_Handle   _rtcs_taskid;

#define RTCS_task_getid()  TSK_Self()

extern uint_32 RTCS_task_create
(
   char_ptr          name,
   uint_32           priority,
   uint_32           stacksize,
   void (_CODE_PTR_  start)(pointer, pointer),
   pointer           arg
);

extern uint_32 RTCS_task_create1
(
   char_ptr          name,
   uint_32           priority,
   uint_32           stacksize,
   void (_CODE_PTR_  start)(pointer, pointer),
   pointer           arg
);

extern uint_32 RTCS_task_create2
(
   char_ptr          name,
   uint_32           priority,
   uint_32           stacksize,
   void (_CODE_PTR_  start)(pointer, pointer),
   pointer           arg
);

extern void RTCS_task_resume_creator (pointer, uint_32);
extern void RTCS_task_exit           (pointer, uint_32);


/*
** Error messages
*/

#define MQX_OK                              (0L)
#define MQX_INVALID_POINTER                 (0x0001L)
#define MQX_OUT_OF_MEMORY                   (0x0004L)
#define MQX_INVALID_CHECKSUM                (0x0008L)
#define MQX_INVALID_MEMORY_BLOCK            (0x000AL)
#define MQX_INVALID_HANDLE                  (0x0065L)
#define IO_ERROR_INVALID_IOCTL_CMD          (0x20007L)
#define IO_ERROR                            (-1)
#define IO_EOF                              (-1)

/*
**  The following structure defnitions are necessary for
**  the PCB handling of RTCS and Enet drivers
*/

#ifndef __PCB__
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

#endif     /* End of PCB types */

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/


/*--------------------------------------------------------------------------*/
/*
** TASK TEMPLATE STRUCTURE
**
*/

typedef struct   task_template_struct
{
   /* The local unique number identifying this task template. */
   uint_32             TASK_TEMPLATE_INDEX;

   /*
   ** The start address of the function which represents this task.
   ** This function will be called when a task is created with the
   ** task template index above. The task is deleted when this function
   ** returns.
   */
   void                (_CODE_PTR_ TASK_ADDRESS)(uint_32);

   /* The amount of stack space required by this task. */
   uint_32             TASK_STACKSIZE;

   /*
   ** The software priority level of this task.
   ** These priorities start at 0, which is the highest priority.
   */
   uint_32             TASK_PRIORITY;

   /* pointer to the string name of the task. */
   char               _PTR_ TASK_NAME;

   /*
   ** possible attributes for the task.
   ** Possible bit values are:
   ** MQX_AUTO_START_TASK     - create 1 instance of task at initialization time
   ** MQX_FLOATING_POINT_TASK - task uses the floating point co-processor
   ** MQX_TIME_SLICE_TASK     - task uses the time-slice scheduler
   */
   uint_32             TASK_ATTRIBUTES;

   /*
   ** The value stored in this field is the default value passed as the
   ** parameter to a Task when it is created.
   */
   uint_32             CREATION_PARAMETER;

   /*
   ** This field is the default time slice to use for the task
   ** if the attributes field contains the TIME_SLICE_TASK attribute bit
   */
   uint_32             DEFAULT_TIME_SLICE;

} TASK_TEMPLATE_STRUCT, _PTR_ TASK_TEMPLATE_STRUCT_PTR;



/*--------------------------------------------------------------------------*/
/*
** MESSAGE HEADER STRUCT
**
** This structure defines the first fields of any message.
** Any pool of messsages must be at least this size.
*/

typedef struct message_header_struct
{
   /* The size of the DATA field in bytes */
   _msg_size       SIZE;

   /* The queue_id to put the queue on */
   _queue_id       TARGET_QID;

   /* The queue_id of the source (for reply) */
   _queue_id       SOURCE_QID;

   /* Control bits, indicating ENDIANness of message
   ** Message Priority, and Message Urgency.
   */
   uchar           CONTROL;

   uchar           RESERVED;

} MESSAGE_HEADER_STRUCT, _PTR_ MESSAGE_HEADER_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
** TIME STRUCTURE
**
** This structure defines how time is maintained in the system
*/

typedef struct time_struct
{

   /* The number of seconds in the time.  */
   uint_32     SECONDS;

   /* The number of milliseconds in the time. */
   uint_32     MILLISECONDS;

} TIME_STRUCT, _PTR_ TIME_STRUCT_PTR;



/*--------------------------------------------------------------------------*/
/*
**                         LWSEM STRUCTURE
**
*/
typedef struct lwsem_struct
{

   /* The next two fields are used to maintain a list of all LWSEMS */

   /* The next lwsem */
   struct lwsem_struct _PTR_  NEXT;

   /* The previous lwsem */
   struct lwsem_struct _PTR_  PREV;

   /* The queue of tasks waiting for this lwsem */
   /* QUEUE_STRUCT               TD_QUEUE; */

   /* A validation stamp */
   uint_32                    VALID;

   /* the semaphore value */
   uint_32_ptr                VALUE;

} LWSEM_STRUCT, _PTR_ LWSEM_STRUCT_PTR;



/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif
/* task  utility function needed for RTCS */
extern _task_id         _task_create(_processor_number, uint_32, uint_32);
extern uint_32          _task_set_error(uint_32);
extern void             _task_block(void);
extern void             _task_ready(pointer);
extern uint_32          _task_destroy(_task_id);
extern _task_id         _task_get_id(void);
extern pointer          _task_get_td(_task_id);
extern uint_32          _task_get_error(void);

/*  memory component function neded for RTCS */
extern void  _mem_test(void);
extern void _PTR_       _mem_alloc_zero(uint_32);
extern pointer          _mem_alloc_system(uint_32);
extern pointer          _mem_alloc_system_zero(uint_32);
extern void _PTR_       _mem_alloc(uint_32);
extern uint_32          _mem_transfer(pointer, _task_id, _task_id);
extern uint_32          _mem_free(pointer);
extern void _PTR_       _mem_get_highwater(void);
extern void             _mem_zero(pointer, uint_32);
extern void             _mem_copy(pointer, pointer, uint_32);
extern uint_16          _mem_sum_ip(uint_16, uint_16, pointer);


/* Time managment function for RTCS */
extern void             _time_delay(uint_32);
extern void             _time_get(TIME_STRUCT_PTR);
extern void             _time_get_elapsed(TIME_STRUCT_PTR);
extern uint_16          _time_get_microseconds(void);


/*  message component function neded for RTCS */
extern _queue_id        RTCS_msgq_get_id(_processor_number, _queue_number);
extern void             RTCS_msg_free(pointer);
extern boolean          RTCS_msgq_send(pointer,_pool_id);
extern pointer          RTCS_msg_alloc(_pool_id);
extern pointer          RTCS_msgq_receive(_queue_id, uint_32,_pool_id);
extern _pool_id         RTCS_msgpool_create(uint_16, uint_16, uint_16, uint_16);
extern uint_32          RTCS_msgpool_destroy(_pool_id);
extern _queue_id        RTCS_msgq_open(_queue_number, uint_16);
extern boolean          RTCS_msgq_send_blocked(pointer, _pool_id);
/*
extern _pool_id         _msgpool_create_ppp(uint_16, uint_16, uint_16, uint_16);
extern boolean          _msgq_send_ppp(pointer);
extern pointer          _msgq_receive_ppp(_queue_id, uint_32);
*/


/* semaphore functions needed for RTCS/ENET */
extern uint_32          _lwsem_create(LWSEM_STRUCT_PTR, int_32);
extern uint_32          _lwsem_destroy(LWSEM_STRUCT_PTR);
extern uint_32          _lwsem_post(LWSEM_STRUCT_PTR);
extern uint_32          _lwsem_wait(LWSEM_STRUCT_PTR);
extern boolean          _lwsem_trwait(LWSEM_STRUCT_PTR);

extern pointer          _mqx_get_io_component_handle(uint_32);
extern pointer          _mqx_set_io_component_handle(uint_32, pointer);

/* RTCS function specfic to OS */
extern uint_32           RTCS_sem_trywait(_rtcs_sem _PTR_);

extern FILE_PTR          RTCS_io_open(char_ptr, char_ptr, uint_32 _PTR_);
extern int_32           _io_socket_install(char_ptr);
extern int_32           _io_telnet_install(char_ptr);

_mqx_int                _io_serial_int_write (char _PTR_,_mqx_int);
extern FILE_PTR         _io_fopen (void);
extern FILE_PTR         _io_fopen1 (void);

//my #define  _bsp_io_fopen()  _io_fopen()
#define  _bsp_io_fopen()  _io_fopen1() //my
#define  _bsp_io_fclose()   _io_fclose()
#define  fgetc          _io_fgetc
#define  fputc          _io_fputc
#define  write          _io_write
#define  getchar()      _io_fgetc()
#define  putchar(c)     _io_fputc((c))


/* RTCS encapsulation of MQX functions */
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


#ifdef __cplusplus
}
#endif
#endif
/* EOF */
