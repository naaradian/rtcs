#ifndef __rtcs_prv_h__
#define __rtcs_prv_h__
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
*** File: rtcs_prv.h
***
*** Comments:  This file contains the include files for the
***            RTCS Communication Library.
***
************************************************************************
*END*******************************************************************/

#include "rtcspcb.h"


//#define _int_disable                ; //my_int_disable_eth //my!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#define _int_enable                my_int_enable_eth //my!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*
** RTCS queue numbers
*/
#define TCPIP_QUEUE     _RTCSQUEUE_base

/*
** Number of blocks allocated for sockets
*/
#define SOCKET_INITIAL_COUNT     20
#define SOCKET_GROW_COUNT        20

/*
** Number of buffers allocated for the network-TCP/IP interface.
*/
#define TCPIP_MSGPOOL_INIT       32
#define TCPIP_MSGPOOL_GROW       16
#define TCPIP_MSGPOOL_LIMIT      160

/*
** Maximum number of protocols that can be logged
*/
#define RTCSLOG_PROT_MAX   16


/*
** Macro definitions for manipulating common data structures
*/

   /*
   ** RTCS_QUEUE: a singly-linked FIFO list
   **
   ** - requires a head and a tail
   ** - each element requires a field called NEXT
   */

#define RTCS_QUEUE_INIT(head,tail)              \
      (head) = NULL

#define RTCS_QUEUE_PEEK(head,tail,elem)         \
      (elem) = (head)

#define RTCS_QUEUE_INS(head,tail,elem)          \
      (elem)->NEXT = NULL;                      \
      if (head) {                               \
         (tail)->NEXT = (elem);                 \
         (tail) = (elem);                       \
      } else {                                  \
         (head) = (elem);                       \
         (tail) = (elem);                       \
      }

#define RTCS_QUEUE_DEL(head,tail)               \
      if (head) {                               \
         (head) = (head)->NEXT;                 \
      }

#define RTCS_QUEUE_DEL_NONEMPTY(head,tail)      \
      (head) = (head)->NEXT

#define RTCS_QUEUE_DELALL(head,tail,elem,temp)  \
      for ((elem)=(head);                       \
           (elem)?(temp)=(elem)->NEXT:0,        \
           (elem);                              \
           (elem)=(temp))

   /*
   ** RTCS_LIST: a singly-linked list
   **
   ** - requires a head
   ** - each element requires a field called NEXT
   */

#define RTCS_LIST_INIT(head)                    \
      (head) = NULL

#define RTCS_LIST_INS(head,elem)                \
      (elem)->NEXT = (head);                    \
      (head) = (elem)

#define RTCS_LIST_INS_END(head,elem,tempp)      \
      for ((tempp)=&(head);                     \
            *(tempp);                           \
            (tempp)=&(*(tempp))->NEXT);         \
      *(tempp) = (elem);                        \
      (elem)->NEXT = NULL

#define RTCS_LIST_PEEK(head,elem)               \
      (elem) = (head)

#define RTCS_LIST_SEARCH(head,elem)             \
      for ((elem)=(head);                       \
           (elem);                              \
           (elem)=(elem)->NEXT)

#define RTCS_LIST_DEL(head,elemp)               \
      *(elemp) = (*(elemp))->NEXT

#define RTCS_LIST_SEARCHMOD(head,elemp)         \
      for ((elemp)=&(head);                     \
           *(elemp);                            \
           (elemp)=&(*(elemp))->NEXT)

#define RTCS_LIST_SEARCHMOD_NOHEAD(head,elemp)  \
      for ((elemp)=&(head)->NEXT;               \
           *(elemp);                            \
           (elemp)=&(*(elemp))->NEXT)

   /*
   ** RTCS_DLIST: a doubly-linked list
   **
   ** - requires a head
   ** - each element requires fields called NEXT and PREV
   */

#define RTCS_DLIST_INIT(head)                   \
      (head) = NULL

#define RTCS_DLIST_INS(head,elem)               \
      (elem)->NEXT = (head);                    \
      (elem)->PREV = &(head);                   \
      if (head) {                               \
         (head)->PREV = &(elem)->NEXT;          \
      }                                         \
      (head) = (elem)

#define RTCS_DLIST_PEEK(head,elem)              \
      (elem) = (head)

#define RTCS_DLIST_SEARCH(head,elem)            \
      for ((elem)=(head);                       \
           (elem);                              \
           (elem)=(elem)->NEXT)

#define RTCS_DLIST_SEARCH_REST(head,elem)       \
      for ((elem)=(elem)->NEXT;                 \
           (elem);                              \
           (elem)=(elem)->NEXT)

#define RTCS_DLIST_DEL(head,elem)               \
      if ((elem)->NEXT) {                       \
         (elem)->NEXT->PREV = (elem)->PREV;     \
      }                                         \
      *(elem)->PREV = (elem)->NEXT


/*
** Macro definitions for RTCS
*/

#define RTCSCMD_issue(parm,cmd)      RTCS_cmd_issue(&(parm).COMMON, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_issue1(parm,cmd)      RTCS_cmd_issue1(&(parm).COMMON, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_internal(parm,cmd)   RTCS_cmd_internal(&(parm).COMMON, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_smartissue(parm,cmd) RTCS_cmd_smartissue(&(parm).COMMON, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_service(pcb,cmd)     RTCS_cmd_service(pcb, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_complete(parm,err)   RTCS_cmd_complete(&(parm)->COMMON, err)

#define RTCS_getcfg(prot)        ((RTCS_DATA_PTR)RTCS_get_data())-> prot ## _CFG
#define RTCS_setcfg(prot,cfg)    ((RTCS_DATA_PTR)RTCS_get_data())-> prot ## _CFG = (cfg)


/*
** Internal structure to synchronize between applications and
** the TCP/IP task.
*/

typedef struct {
   pointer        SYNC;       /* for synchronization with application */
   uint_32        ERROR;      /* [OUT] error code */
} TCPIP_PARM, _PTR_ TCPIP_PARM_PTR;


typedef struct {
   uint_32        VERSION;

   pointer        TCPIP_CFG;
   pointer        SOCKET_CFG;
   pointer        IP_CFG;
   pointer        ICMP_CFG;
   pointer        UDP_CFG;
   pointer        TCP_CFG;

   _pool_id       TCPIP_msg_pool;
   _queue_id      TCPIP_qid;
   _queue_id      SOCKET_qid;             /* not used */
   _pool_id       RTCS_SOCK_pool_id;      /* not used */
   _pool_id       RTCS_ICCB_pool_id;      /* not used */
   _rtcs_part     RTCS_PCB_partition;
   _rtcs_part     RTCS_socket_partition;

   uint_32        RTCSSOCK_MAX_COPY;
   uint_32        RTCSQUEUE_BASE_COPY;
   uint_32        RTCSPCB_MAX_COPY;
   uint_32        RTCSTASK_PRIORITY_COPY;
   uint_32        RTCSTASK_STACKSIZE_COPY;
   boolean        RTCS_INITIALIZED_COPY;
   boolean        IP_FORWARD_COPY;
   boolean        TCP_BYPASS_RX_COPY;
   boolean        TCP_BYPASS_TX_COPY;
   uint_32        TCP_RTO_MIN_COPY;
   uint_32        UDP_MAX_QUEUE_SIZE_COPY;

   pointer        IGMP_CFG;
   pointer        RIP_CFG;
   pointer        OSPF_CFG;

   _rtcs_taskid   TCPIP_TASKID;
   uint_32        RTCS_LOG_CONTROL;
   uint_32        RTCS_LOG_PROTCOUNT;
   uint_16        RTCS_LOG_PCB[RTCSLOG_PROT_MAX];
   uint_16        RTCS_LOG_TIMER[RTCSLOG_PROT_MAX];

   pointer        NAT_CFG;
   pointer        IPSHIELD_CFG;
   pointer        IPIP_CFG;
} RTCS_DATA, _PTR_ RTCS_DATA_PTR;

/*
** externs for RTCS utility procedures
*/

extern void RTCS_seterror
(
   RTCS_ERROR_STRUCT_PTR   errptr,
   uint_32                 errcode,
   uint_32                 errparm
);

extern void RTCS_setsockerror
(
   uint_32     sock,
   uint_32     error
);

extern uint_32 RTCS_cmd_issue      (TCPIP_PARM_PTR, void(_CODE_PTR_)(pointer));
extern uint_32 RTCS_cmd_internal   (TCPIP_PARM_PTR, void(_CODE_PTR_)(pointer));
extern uint_32 RTCS_cmd_smartissue (TCPIP_PARM_PTR, void(_CODE_PTR_)(pointer));
extern boolean RTCS_cmd_service    (RTCSPCB_PTR,    void(_CODE_PTR_)(pointer));
extern void    RTCS_cmd_complete   (TCPIP_PARM_PTR, uint_32);

#endif
/* EOF */
