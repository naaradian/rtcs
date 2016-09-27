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
*** File: rtcscmd.c
***
*** Comments:  This file contains the interface functions to the
***            RTCS Communication Library.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "socket.h"
#include "ip_prv.h"
#include "ip-e.h" //my
#include "ipcp.h" //my

//remove to rtcs.h #define  fopen      _io_fopen1	//my!!!!!!!!!!!!!!!!!

extern unsigned long keeprtcs;
extern unsigned long keeppool;
extern unsigned long keepsocket;
extern unsigned long keeppart;
extern unsigned long tcpip_id;

RTCS_IF_STRUCT rtcs_enet;
RTCS_IF_STRUCT_PTR RTCS_IF_ENET;

RTCS_IF_STRUCT rtcs_local;
RTCS_IF_STRUCT_PTR RTCS_IF_LOCALHOST;

RTCS_IF_STRUCT rtcs_ppp;
RTCS_IF_STRUCT_PTR RTCS_IF_PPP;

//_____________________________________________________________________________________-
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_create2
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Initialize RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_create2
   (
      void
   )
{ /* Body */
   RTCS_DATA_PTR              RTCS_data_ptr;
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr;
  // io_toggle(0xffffffffl); //my w
   uint_32 		tmp; //my
//_____________________________________________________________my_add
//init global variables

extern uint_32 _SVCTCPTASK_priority;
extern uint_32 _SVCTCPTASK_stacksize;
//______________________________________________________test n5

//was used keeppool keepsocket keeprtcs!!!!

//RTCS_part_destroy(RTCS_data_ptr->RTCS_PCB_partition);

//RTCS_part_destroy(RTCS_data_ptr->RTCS_socket_partition);

//RTCS_msgpool_destroy  (RTCS_data_ptr->TCPIP_msg_pool );
//_______________________________________________________testn5


  _RTCSQUEUE_base = 0x10;

/*
** Number of RTCSPCBs to allocate
*/
  _RTCSPCB_max = 32;

/*
** TCP/IP task priority and extra stack size
*/
 _RTCSTASK_priority   = 6; //???
// _RTCSTASK_priority   = 2; //try 090527

//  _RTCSTASK_stacksize  = 0; //t090331

   _RTCSTASK_stacksize  = 10000l; //t 1020

  _RTCS_initialized    = FALSE;

/*
** TCP-based RPC task priority and extra stack size
*/
   _SVCTCPTASK_priority    = 9;
//  _SVCTCPTASK_priority    = 1; //rpc not use now! - make  priority
  _SVCTCPTASK_stacksize   = 0;

/*
** Indicates whether or not IP should forward packets between interfaces
*/
//  _IP_forward = FALSE;
 _IP_forward = TRUE;

/*
** Indicates whether or not to bypass checksum checking
*/
  _TCP_bypass_rx = FALSE;
  _TCP_bypass_tx = FALSE;

/*
** The TCP retransmission timeout minimum
*/
 // _TCP_rto_min;     /* initialized by TCP_Init() */

/*
** Maximum number of packets that will be enqueued
*/
  _UDP_max_queue_size  = 100;

rtcs_enet.OPEN  = IPE_open; 
rtcs_enet.CLOSE = IPE_close;
rtcs_enet.SEND  = IPE_send_IP;
rtcs_enet.JOIN  = IPE_join;
rtcs_enet.LEAVE = IPE_leave;
 
RTCS_IF_ENET = &rtcs_enet;

rtcs_local.OPEN  = IPLOCAL_open; 
rtcs_local.CLOSE = IPLOCAL_close;
rtcs_local.SEND  = IPLOCAL_send;
rtcs_local.JOIN  = NULL;
rtcs_local.LEAVE = NULL;

RTCS_IF_LOCALHOST = &rtcs_local;

rtcs_ppp.OPEN  = IPCP_init; 
rtcs_ppp.CLOSE = IPCP_destroy;
rtcs_ppp.SEND  = IPCP_send;
rtcs_ppp.JOIN  = NULL;
rtcs_ppp.LEAVE = NULL;

RTCS_IF_PPP = &rtcs_ppp;
//_____________________________________________________________my_add

   /*
   ** Check and see if this is the first time we have initialized,
   */
   if (RTCS_get_data() != NULL) {
//	return 0x555555l;
 //101123     return RTCSERR_INITIALIZED;
  } /* Endif */

//   io_toggle(0xffffffffl);

//   RTCS_data_ptr = _mem_alloc_system_zero(sizeof(RTCS_DATA));
  RTCS_data_ptr =	(RTCS_DATA_PTR) keeprtcs; //test n12071450
   RTCS_set_data(RTCS_data_ptr);
// io_toggle(0xffffffffl); //nw

   /*
   ** Initialize socket state
   */
//101208    socket_cfg_ptr = _mem_alloc_system_zero(sizeof(SOCKET_CONFIG_STRUCT));
   socket_cfg_ptr = (SOCKET_CONFIG_STRUCT_PTR)keepsocket;
   socket_cfg_ptr->INITIALIZED = TRUE;
   socket_cfg_ptr->SOCKET_HEAD = NULL;
   socket_cfg_ptr->SOCKET_TAIL = NULL;
   RTCS_mutex_init(&socket_cfg_ptr->SOCK_MUTEX);
   RTCS_setcfg(SOCKET, socket_cfg_ptr);

   /*
   ** Initialize global data
   */
   _TCP_bypass_rx = FALSE;
   _TCP_bypass_tx = FALSE;

   RTCS_data_ptr->VERSION            = RTCS_VERSION;
   RTCS_data_ptr->RTCS_LOG_PROTCOUNT = RTCSLOG_PROT_MAX;
   RTCSLOG_disable(RTCS_LOGCTRL_ALL);

   RTCS_data_ptr->RTCSQUEUE_BASE_COPY     = _RTCSQUEUE_base;
   RTCS_data_ptr->RTCSPCB_MAX_COPY        = _RTCSPCB_max;
   RTCS_data_ptr->RTCSTASK_PRIORITY_COPY  = _RTCSTASK_priority;
   RTCS_data_ptr->RTCSTASK_STACKSIZE_COPY = _RTCSTASK_stacksize;
   RTCS_data_ptr->RTCS_INITIALIZED_COPY   = _RTCS_initialized;
   RTCS_data_ptr->IP_FORWARD_COPY         = _IP_forward;
   RTCS_data_ptr->TCP_BYPASS_RX_COPY      = _TCP_bypass_rx;
   RTCS_data_ptr->TCP_BYPASS_TX_COPY      = _TCP_bypass_tx;
   RTCS_data_ptr->TCP_RTO_MIN_COPY        = _TCP_rto_min;
   RTCS_data_ptr->UDP_MAX_QUEUE_SIZE_COPY = _UDP_max_queue_size;

   RTCS_data_ptr->TCPIP_qid = RTCS_msgq_get_id(0,TCPIP_QUEUE);

   /*
   ** Create a pool of buffers for use in communicating to the TCP/IP task.
   */

/* n101208
   RTCS_data_ptr->TCPIP_msg_pool = RTCS_msgpool_create(sizeof(TCPIP_MESSAGE),
      TCPIP_MSGPOOL_INIT, TCPIP_MSGPOOL_GROW, TCPIP_MSGPOOL_LIMIT);
*/
 RTCS_data_ptr->TCPIP_msg_pool = (_pool_id)keeppool;


   if (RTCS_data_ptr->TCPIP_msg_pool == MSGPOOL_NULL_POOL_ID) {
      RTCS_log_error( ERROR_TCPIP, RTCSERR_CREATE_POOL_FAILED, 0, 0, 0);
      return RTCSERR_CREATE_POOL_FAILED;
// return 0x555555l;
   } /* Endif */


   /*
   ** Create the socket partition
   */
/* 101208
   RTCS_data_ptr->RTCS_socket_partition =
      RTCS_part_create(sizeof(SOCKET_STRUCT),
         SOCKET_INITIAL_COUNT, SOCKET_GROW_COUNT, 0, NULL, NULL);
*/
  RTCS_data_ptr->RTCS_socket_partition = (_rtcs_part)keeppart;



   if (RTCS_data_ptr->RTCS_socket_partition == 0) {
      RTCS_log_error(ERROR_RTCS, RTCSERR_CREATE_PARTITION_FAILED, 0, 0, 0);
      return RTCSERR_CREATE_PARTITION_FAILED;
 //return 0x555555l;
   } /* Endif */


//io_toggle(0xffffffffl); //nw
   /*
   ** Create TCPIP task
   */

//hangt RTCS_task_destroy  ((_rtcs_taskid)_task_get_id_from_name("TCP/IP")); //test n6


 
   return RTCS_task_create("TCP/IP",
                           _RTCSTASK_priority,
                         _RTCSTASK_stacksize + (1000 * sizeof(uint_32)),
//101220                          TCPIP_task, NULL);
                          TCPIP_task2, NULL); //101220



// _task_restart(tcpip_id, NULL, FALSE);

//return 1; //test n5


 //  tmp = RTCS_task_create("TCP/IP",
 //                          _RTCSTASK_priority,
  //                         _RTCSTASK_stacksize + (1000 * sizeof(uint_32)),
  //                         TCPIP_task, NULL);

 //tmp = RTCS_task_create("TCP/IP",
 //                          6,
 //                          0x4000,
 //                          TCPIP_task, NULL);


  //tmp = 0x555555l;
 // return tmp;





 // return 0x555555l; //t

} /* Endbody */


//_____________________________________________________________________________________



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_create
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Initialize RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_create
   (
      void
   )
{ /* Body */
   RTCS_DATA_PTR              RTCS_data_ptr;
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr;
  // io_toggle(0xffffffffl); //my w
   uint_32 		tmp; //my
//_____________________________________________________________my_add
//init global variables

extern uint_32 _SVCTCPTASK_priority;
extern uint_32 _SVCTCPTASK_stacksize;



  _RTCSQUEUE_base = 0x10;

/*
** Number of RTCSPCBs to allocate
*/
  _RTCSPCB_max = 32;

/*
** TCP/IP task priority and extra stack size
*/
  _RTCSTASK_priority   = 6; //???
//_RTCSTASK_priority   = 2; //try 090527

//  _RTCSTASK_stacksize  = 0; //t090331

   _RTCSTASK_stacksize  = 10000l; //t 1020

  _RTCS_initialized    = FALSE;

/*
** TCP-based RPC task priority and extra stack size
*/
   _SVCTCPTASK_priority    = 9;
//  _SVCTCPTASK_priority    = 1; //rpc not use now! - make  priority
  _SVCTCPTASK_stacksize   = 0;

/*
** Indicates whether or not IP should forward packets between interfaces
*/
//  _IP_forward = FALSE;
_IP_forward = TRUE;


/*
** Indicates whether or not to bypass checksum checking
*/
  _TCP_bypass_rx = FALSE;
  _TCP_bypass_tx = FALSE;

/*
** The TCP retransmission timeout minimum
*/
 // _TCP_rto_min;     /* initialized by TCP_Init() */

/*
** Maximum number of packets that will be enqueued
*/
  _UDP_max_queue_size  = 100;

rtcs_enet.OPEN  = IPE_open; 
rtcs_enet.CLOSE = IPE_close;
rtcs_enet.SEND  = IPE_send_IP;
rtcs_enet.JOIN  = IPE_join;
rtcs_enet.LEAVE = IPE_leave;
 
RTCS_IF_ENET = &rtcs_enet;

rtcs_local.OPEN  = IPLOCAL_open; 
rtcs_local.CLOSE = IPLOCAL_close;
rtcs_local.SEND  = IPLOCAL_send;
rtcs_local.JOIN  = NULL;
rtcs_local.LEAVE = NULL;

RTCS_IF_LOCALHOST = &rtcs_local;

rtcs_ppp.OPEN  = IPCP_init; 
rtcs_ppp.CLOSE = IPCP_destroy;
rtcs_ppp.SEND  = IPCP_send;
rtcs_ppp.JOIN  = NULL;
rtcs_ppp.LEAVE = NULL;

RTCS_IF_PPP = &rtcs_ppp;
//_____________________________________________________________my_add

   /*
   ** Check and see if this is the first time we have initialized,
   */
   if (RTCS_get_data() != NULL) {
//	return 0x555555l;
      return RTCSERR_INITIALIZED;
  } /* Endif */

//   io_toggle(0xffffffffl);

   RTCS_data_ptr = _mem_alloc_system_zero(sizeof(RTCS_DATA));
   keeprtcs = (unsigned long)RTCS_data_ptr;
   RTCS_set_data(RTCS_data_ptr);
// io_toggle(0xffffffffl); //nw

   /*
   ** Initialize socket state
   */
   socket_cfg_ptr = _mem_alloc_system_zero(sizeof(SOCKET_CONFIG_STRUCT));
keepsocket = (unsigned long)socket_cfg_ptr;
   socket_cfg_ptr->INITIALIZED = TRUE;
   socket_cfg_ptr->SOCKET_HEAD = NULL;
   socket_cfg_ptr->SOCKET_TAIL = NULL;
   RTCS_mutex_init(&socket_cfg_ptr->SOCK_MUTEX);
   RTCS_setcfg(SOCKET, socket_cfg_ptr);

   /*
   ** Initialize global data
   */
 
 //  _TCP_bypass_rx = FALSE;
 //  _TCP_bypass_tx = FALSE;

   RTCS_data_ptr->VERSION            = RTCS_VERSION;
   RTCS_data_ptr->RTCS_LOG_PROTCOUNT = RTCSLOG_PROT_MAX;
   RTCSLOG_disable(RTCS_LOGCTRL_ALL);

   RTCS_data_ptr->RTCSQUEUE_BASE_COPY     = _RTCSQUEUE_base;
   RTCS_data_ptr->RTCSPCB_MAX_COPY        = _RTCSPCB_max;
   RTCS_data_ptr->RTCSTASK_PRIORITY_COPY  = _RTCSTASK_priority;
   RTCS_data_ptr->RTCSTASK_STACKSIZE_COPY = _RTCSTASK_stacksize;
   RTCS_data_ptr->RTCS_INITIALIZED_COPY   = _RTCS_initialized;
   RTCS_data_ptr->IP_FORWARD_COPY         = _IP_forward;
   RTCS_data_ptr->TCP_BYPASS_RX_COPY      = _TCP_bypass_rx;
   RTCS_data_ptr->TCP_BYPASS_TX_COPY      = _TCP_bypass_tx;
   RTCS_data_ptr->TCP_RTO_MIN_COPY        = _TCP_rto_min;
   RTCS_data_ptr->UDP_MAX_QUEUE_SIZE_COPY = _UDP_max_queue_size;

   RTCS_data_ptr->TCPIP_qid = RTCS_msgq_get_id(0,TCPIP_QUEUE);

   /*
   ** Create a pool of buffers for use in communicating to the TCP/IP task.
   */
   RTCS_data_ptr->TCPIP_msg_pool = RTCS_msgpool_create(sizeof(TCPIP_MESSAGE),
      TCPIP_MSGPOOL_INIT, TCPIP_MSGPOOL_GROW, TCPIP_MSGPOOL_LIMIT);

  keeppool = (unsigned long)RTCS_data_ptr->TCPIP_msg_pool;

   if (RTCS_data_ptr->TCPIP_msg_pool == MSGPOOL_NULL_POOL_ID) {
      RTCS_log_error( ERROR_TCPIP, RTCSERR_CREATE_POOL_FAILED, 0, 0, 0);
      return RTCSERR_CREATE_POOL_FAILED;
// return 0x555555l;
   } /* Endif */


   /*
   ** Create the socket partition
   */
   RTCS_data_ptr->RTCS_socket_partition =
      RTCS_part_create(sizeof(SOCKET_STRUCT),
         SOCKET_INITIAL_COUNT, SOCKET_GROW_COUNT, 0, NULL, NULL);
   if (RTCS_data_ptr->RTCS_socket_partition == 0) {
      RTCS_log_error(ERROR_RTCS, RTCSERR_CREATE_PARTITION_FAILED, 0, 0, 0);
      return RTCSERR_CREATE_PARTITION_FAILED;
 //return 0x555555l;
   } /* Endif */
keeppart = (unsigned long)RTCS_data_ptr->RTCS_socket_partition;


//io_toggle(0xffffffffl); //nw
   /*
   ** Create TCPIP task
   */

 
   return RTCS_task_create("TCP/IP",
                           _RTCSTASK_priority,
                         _RTCSTASK_stacksize + (1000 * sizeof(uint_32)),
                          TCPIP_task, NULL);
 //  tmp = RTCS_task_create("TCP/IP",
 //                          _RTCSTASK_priority,
  //                         _RTCSTASK_stacksize + (1000 * sizeof(uint_32)),
  //                         TCPIP_task, NULL);

 //tmp = RTCS_task_create("TCP/IP",
 //                          6,
 //                          0x4000,
 //                          TCPIP_task, NULL);


  //tmp = 0x555555l;
 // return tmp;





 // return 0x555555l; //t

} /* Endbody */

extern pointer _RTCS_io_component_handle;// = NULL;
extern pointer _SNMP_io_component_handle;// = NULL;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_create1
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Initialize RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_create1
   (
      void
   )
{ /* Body */
   RTCS_DATA_PTR              RTCS_data_ptr;
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr;
  // io_toggle(0xffffffffl); //my w
   uint_32 		tmp; //my
//_____________________________________________________________my_add
//init global variables

//RTCS_task_exit(TCPIP_task, NULL);

RTCS_data_ptr = RTCS_get_data();

//RTCS_msgpool_destroy  (RTCS_data_ptr->TCPIP_msg_pool );

RTCS_part_destroy(RTCS_data_ptr->RTCS_PCB_partition);

RTCS_part_destroy(RTCS_data_ptr->RTCS_socket_partition);

RTCS_msgpool_destroy  (RTCS_data_ptr->TCPIP_msg_pool );




//extern uint_32 _SVCTCPTASK_priority;
//extern uint_32 _SVCTCPTASK_stacksize;

//printf("\n\r RTCS_create1 - 0");

  _RTCSQUEUE_base = 0x10;

/*
** Number of RTCSPCBs to allocate
*/
  _RTCSPCB_max = 32;

/*
** TCP/IP task priority and extra stack size
*/
 _RTCSTASK_priority   = 6; //???
// _RTCSTASK_priority   = 2; //try 090527

//  _RTCSTASK_stacksize  = 0; //t090331

   _RTCSTASK_stacksize  = 10000l; //t 1020

  _RTCS_initialized    = FALSE;

/*
** TCP-based RPC task priority and extra stack size
*/
   _SVCTCPTASK_priority    = 9;
//  _SVCTCPTASK_priority    = 1; //rpc not use now! - make  priority
  _SVCTCPTASK_stacksize   = 0;

/*
** Indicates whether or not IP should forward packets between interfaces
*/
// _IP_forward = FALSE;
 _IP_forward = TRUE;

/*
** Indicates whether or not to bypass checksum checking
*/
  _TCP_bypass_rx = FALSE;
  _TCP_bypass_tx = FALSE;

/*
** The TCP retransmission timeout minimum
*/
 // _TCP_rto_min;     /* initialized by TCP_Init() */

/*
** Maximum number of packets that will be enqueued
*/
  _UDP_max_queue_size  = 100;

rtcs_enet.OPEN  = IPE_open; 
rtcs_enet.CLOSE = IPE_close;
rtcs_enet.SEND  = IPE_send_IP;
rtcs_enet.JOIN  = IPE_join;
rtcs_enet.LEAVE = IPE_leave;
 
RTCS_IF_ENET = &rtcs_enet;

rtcs_local.OPEN  = IPLOCAL_open; 
rtcs_local.CLOSE = IPLOCAL_close;
rtcs_local.SEND  = IPLOCAL_send;
rtcs_local.JOIN  = NULL;
rtcs_local.LEAVE = NULL;

RTCS_IF_LOCALHOST = &rtcs_local;

rtcs_ppp.OPEN  = IPCP_init; 
rtcs_ppp.CLOSE = IPCP_destroy;
rtcs_ppp.SEND  = IPCP_send;
rtcs_ppp.JOIN  = NULL;
rtcs_ppp.LEAVE = NULL;

RTCS_IF_PPP = &rtcs_ppp;
//_____________________________________________________________my_add

   /*
   ** Check and see if this is the first time we have initialized,
   */

// _RTCS_io_component_handle = NULL;
// _SNMP_io_component_handle = NULL;

RTCS_set_data(NULL);
SNMP_set_data(NULL);

//   if (RTCS_get_data() != NULL) {
//	return 0x555555l;
 //     return RTCSERR_INITIALIZED;
 // } /* Endif */

//printf("\n\r RTCS_create1 - 1");

//   io_toggle(0xffffffffl);

   RTCS_data_ptr = _mem_alloc_system_zero(sizeof(RTCS_DATA));
   RTCS_set_data(RTCS_data_ptr);
// io_toggle(0xffffffffl); //nw

   /*
   ** Initialize socket state
   */

   socket_cfg_ptr = _mem_alloc_system_zero(sizeof(SOCKET_CONFIG_STRUCT));
   socket_cfg_ptr->INITIALIZED = TRUE;
   socket_cfg_ptr->SOCKET_HEAD = NULL;
   socket_cfg_ptr->SOCKET_TAIL = NULL;
   RTCS_mutex_init(&socket_cfg_ptr->SOCK_MUTEX);
   RTCS_setcfg(SOCKET, socket_cfg_ptr);

   /*
   ** Initialize global data
   */
 
   _TCP_bypass_rx = FALSE;
   _TCP_bypass_tx = FALSE;

   RTCS_data_ptr->VERSION            = RTCS_VERSION;
   RTCS_data_ptr->RTCS_LOG_PROTCOUNT = RTCSLOG_PROT_MAX;
   RTCSLOG_disable(RTCS_LOGCTRL_ALL);

   RTCS_data_ptr->RTCSQUEUE_BASE_COPY     = _RTCSQUEUE_base;
   RTCS_data_ptr->RTCSPCB_MAX_COPY        = _RTCSPCB_max;
   RTCS_data_ptr->RTCSTASK_PRIORITY_COPY  = _RTCSTASK_priority;
   RTCS_data_ptr->RTCSTASK_STACKSIZE_COPY = _RTCSTASK_stacksize;
   RTCS_data_ptr->RTCS_INITIALIZED_COPY   = _RTCS_initialized;
   RTCS_data_ptr->IP_FORWARD_COPY         = _IP_forward;
   RTCS_data_ptr->TCP_BYPASS_RX_COPY      = _TCP_bypass_rx;
   RTCS_data_ptr->TCP_BYPASS_TX_COPY      = _TCP_bypass_tx;
   RTCS_data_ptr->TCP_RTO_MIN_COPY        = _TCP_rto_min;
   RTCS_data_ptr->UDP_MAX_QUEUE_SIZE_COPY = _UDP_max_queue_size;

   RTCS_data_ptr->TCPIP_qid = RTCS_msgq_get_id(0,TCPIP_QUEUE);

   /*
   ** Create a pool of buffers for use in communicating to the TCP/IP task.
   */

   RTCS_data_ptr->TCPIP_msg_pool = RTCS_msgpool_create(sizeof(TCPIP_MESSAGE),
      TCPIP_MSGPOOL_INIT, TCPIP_MSGPOOL_GROW, TCPIP_MSGPOOL_LIMIT);
   if (RTCS_data_ptr->TCPIP_msg_pool == MSGPOOL_NULL_POOL_ID) {
      RTCS_log_error( ERROR_TCPIP, RTCSERR_CREATE_POOL_FAILED, 0, 0, 0);
      return RTCSERR_CREATE_POOL_FAILED;
// return 0x555555l;
   } 



//printf("\n\r RTCS_create1 - 2");
   /*
   ** Create the socket partition
   */

   RTCS_data_ptr->RTCS_socket_partition =
      RTCS_part_create(sizeof(SOCKET_STRUCT),
         SOCKET_INITIAL_COUNT, SOCKET_GROW_COUNT, 0, NULL, NULL);
   if (RTCS_data_ptr->RTCS_socket_partition == 0) {
      RTCS_log_error(ERROR_RTCS, RTCSERR_CREATE_PARTITION_FAILED, 0, 0, 0);
      return RTCSERR_CREATE_PARTITION_FAILED;
 //return 0x555555l;
   } 

//printf("\n\r RTCS_create1 - 3");
//io_toggle(0xffffffffl); //nw
   /*
   ** Create TCPIP task
   */

 /*
   return RTCS_task_create1("TCP/IP",
                           _RTCSTASK_priority,
                         _RTCSTASK_stacksize + (1000 * sizeof(uint_32)),
                          TCPIP_task, NULL);
*/
 return RTCS_task_create1("TCP/IP",
                           _RTCSTASK_priority,
                         _RTCSTASK_stacksize + (1000 * sizeof(uint_32)),
                          TCPIP_task1, NULL);






 //  tmp = RTCS_task_create("TCP/IP",
 //                          _RTCSTASK_priority,
  //                         _RTCSTASK_stacksize + (1000 * sizeof(uint_32)),
  //                         TCPIP_task, NULL);

 //tmp = RTCS_task_create("TCP/IP",
 //                          6,
 //                          0x4000,
 //                          TCPIP_task, NULL);


  //tmp = 0x555555l;
 // return tmp;





 // return 0x555555l; //t
//printf("\n\r RTCS_create1 - 4");
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_issue1
* Returned Values : uint_32 (error code)
* Comments        :
*     Issue a command to the TCP/IP task
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_cmd_issue1
   (
      TCPIP_PARM_PTR    parm_ptr,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */
   RTCS_DATA_PTR        RTCS_data_ptr;
   TCPIP_MESSAGE_PTR    message;

   RTCS_data_ptr = RTCS_get_data();
 //ok  return (unsigned long)(RTCS_data_ptr);
   message = RTCS_msg_alloc(RTCS_data_ptr->TCPIP_msg_pool);
   if (message == NULL) {
      parm_ptr->ERROR = RTCSERR_OUT_OF_BUFFERS;
   } else {
      parm_ptr->ERROR = RTCS_OK;
      parm_ptr->SYNC  = _task_get_td(0);

      message->HEAD.TARGET_QID = RTCS_data_ptr->TCPIP_qid;
      message->HEAD.SOURCE_QID = 0;
      message->HEAD.SIZE = sizeof(*message);
      message->COMMAND = command;
      message->DATA = parm_ptr;
//ok return (unsigned long)(message);
	return (unsigned long)(parm_ptr->ERROR);//0
return (unsigned long)(parm_ptr);//0
      if (!RTCS_msgq_send_blocked(message, RTCS_data_ptr->TCPIP_msg_pool)) {
         parm_ptr->ERROR = RTCSERR_SEND_FAILED;
      } /* Endif */
//return (unsigned long)(message);
   } /* Endif */

   return parm_ptr->ERROR;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_issue
* Returned Values : uint_32 (error code)
* Comments        :
*     Issue a command to the TCP/IP task
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_cmd_issue
   (
      TCPIP_PARM_PTR    parm_ptr,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */
   RTCS_DATA_PTR        RTCS_data_ptr;
   TCPIP_MESSAGE_PTR    message;

   RTCS_data_ptr = RTCS_get_data();

   message = RTCS_msg_alloc(RTCS_data_ptr->TCPIP_msg_pool);
   if (message == NULL) {
      parm_ptr->ERROR = RTCSERR_OUT_OF_BUFFERS;
   } else {
      parm_ptr->ERROR = RTCS_OK;
      parm_ptr->SYNC  = _task_get_td(0);

      message->HEAD.TARGET_QID = RTCS_data_ptr->TCPIP_qid;
      message->HEAD.SOURCE_QID = 0;
      message->HEAD.SIZE = sizeof(*message);
      message->COMMAND = command;
      message->DATA = parm_ptr;

      if (!RTCS_msgq_send_blocked(message, RTCS_data_ptr->TCPIP_msg_pool)) {
         parm_ptr->ERROR = RTCSERR_SEND_FAILED;
      } /* Endif */

   } /* Endif */

   return parm_ptr->ERROR;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_internal
* Returned Values : uint_32 (error code)
* Comments        :
*     Issue a command from within the TCP/IP task
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_cmd_internal
   (
      TCPIP_PARM_PTR    parm_ptr,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */

   parm_ptr->ERROR = RTCS_OK;
   parm_ptr->SYNC  = NULL;

   command(parm_ptr);

   return parm_ptr->ERROR;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_smartissue
* Returned Values : uint_32 (error code)
* Comments        :
*     Issue a command from any context
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_cmd_smartissue
   (
      TCPIP_PARM_PTR    parm_ptr,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */
   RTCS_DATA_PTR        RTCS_data_ptr;

   RTCS_data_ptr = RTCS_get_data();

   if (RTCS_task_getid() == RTCS_data_ptr->TCPIP_TASKID) {
      return RTCS_cmd_internal(parm_ptr, command);
   } else {
      return RTCS_cmd_issue(parm_ptr, command);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_complete
* Returned Values : void
* Comments        :
*     Unblocks an application following successful completion of a command.
*
*END*-----------------------------------------------------------------*/

void RTCS_cmd_complete
   (
      TCPIP_PARM_PTR    parm_ptr,
      uint_32           error
   )
{ /* Body */

   if (error) {
      parm_ptr->ERROR = error;
   } /* Endif */

   if (parm_ptr->SYNC) {
      _task_ready(parm_ptr->SYNC);
   } /* Endif */

} /* Endbody */


  extern "C" void printfpd(char*, unsigned long);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_service
* Returned Values : TRUE on success
* Comments        :
*     Issue a nonblocking command to the TCP/IP task
*
*END*-----------------------------------------------------------------*/

boolean RTCS_cmd_service
   (
      RTCSPCB_PTR       pcb,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */
   RTCS_DATA_PTR        RTCS_data_ptr;
   TCPIP_MESSAGE_PTR    message;

//ok if make returnin called 	 return FALSE; //t 090330

   RTCS_data_ptr = RTCS_get_data();

//ok  return FALSE; //t 090330

   message = RTCS_msg_alloc(RTCS_data_ptr->TCPIP_msg_pool);
   if (message == NULL) {
      return FALSE;
   } /* Endif */

   message->HEAD.TARGET_QID = RTCS_data_ptr->TCPIP_qid;
   message->HEAD.SOURCE_QID = 0;
   message->HEAD.SIZE = sizeof(*message);
   message->COMMAND = command;
   message->DATA = pcb;

  // printfpd("\n\rcmd %X", command);
  // printfpd("\n\rs %X", (unsigned long)pcb->NETWORK_LAYER);
  // printfpd("\n\rd %X", (unsigned long )pcb->TRANSPORT_LAYER);


//ok!   return FALSE; //t 090330

   return RTCS_msgq_send(message, RTCS_data_ptr->TCPIP_msg_pool);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  RTCS_seterror
*  Returned Value:  void
*  Comments      :  Called to record an error condition in a
*                   provided error structure.
*
*END*-----------------------------------------------------------------*/

void RTCS_seterror
   (
      RTCS_ERROR_STRUCT_PTR   errptr,
         /* [IN/OUT] where to record the error */
      uint_32                 errcode,
         /* [IN] the error that occurred */
      uint_32                 errparm
         /* [IN] additional information */
   )
{ /* Body */

   /* Only record the first error that occurs */
   if (errptr->ERROR == RTCS_OK) {
      errptr->ERROR    = errcode;
      errptr->PARM     = errparm;
#ifdef __MQX__
      errptr->TASK_ID   = _task_get_id();
      errptr->TASKCODE = _task_get_error();
      errptr->MEMPTR   = _mem_get_highwater();
      errptr->STACK    = _task_check_stack();
#endif
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_log_error
* Returned Value   : none
* Comments  :  Routine to handle non-fatal errors.
*
*END*-----------------------------------------------------------------*/

void  RTCS_log_error
   (
      uint_32  module,
      uint_32  error,
      uint_32  p1,
      uint_32  p2,
      uint_32  p3
   )
{ /* Body */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_io_open
* Returned Value  : open file descriptor
* Comments        : RTOS-specific wrapper for fopen().
*
*END*-----------------------------------------------------------------*/

FILE_PTR RTCS_io_open
   (
      char_ptr       filename,
      char_ptr       filemode,
      uint_32 _PTR_  error_ptr
   )
{ /* Body */
   FILE_PTR file;
   uint_32  i;

   /* Scan for device name delimiter */
   for (i = 0; filename[i] && filename[i] != ':'; i++);
 //ok  for(;;){} //_________________________________________________________________t
   /* 
   ** If the next char is not nul then a file
   ** on the resident file system is probably
   ** being opened.
   */
   if (filename[i] == ':' && filename[i+1] != '\0') {
      /* Assume we are opening a file */
 //no	for(;;){} //_________________________________________________________________t
      file = fopen(filename, filemode);
  //  file = fopen1(filename, filemode);
   } else {
//ok  for(;;){} //_________________________________________________________________t
      file = fopen(filename, 0);
  //  file = fopen1(filename, 0);
 //no     for(;;){} //_________________________________________________________________t
   } /* Endif */
//no	for(;;){} //_________________________________________________________________t
     if (error_ptr) {
      if (file) {
         *error_ptr = RTCS_OK;
      } else {
         *error_ptr = _task_get_error();
         if (!*error_ptr) {
            *error_ptr = RTCS_ENOENT;
         } /* Endif */
      } /* Endif */
   } /* Endif */

   return file;

} /* Endbody */


/* EOF */
