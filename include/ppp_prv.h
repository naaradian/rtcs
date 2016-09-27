#ifndef __ppp_prv_h__
#define __ppp_prv_h__
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
*** File: ppp_prv.h
***
*** Comments:  This file contains the private defines, externs and data
***            structure definitions required by the PPP Library.
***
************************************************************************
*END*******************************************************************/

#include "pppfsm.h"
#include "lcp.h"
#include "pap.h"
#include "chap.h"
#include "ccp.h"


/*
** Assorted PPP constants
*/

#define PPP_VALID       0x1A0A80CFl
#define PPP_PROT_CP     0x00FD
#define PPP_PROT_CCP    0x80FD
#define PPP_PROT_LCP    0xC021
#define PPP_PROT_PAP    0xC023
#define PPP_PROT_CHAP   0xC223

/*
** Control packet types
*/

#define PPPCMD_SEND     0
#define PPPCMD_RESTART  1
#define PPPCMD_STOP     2
#define PPPCMD_SHUTDOWN 3

/*
** Constants for the Message Pool
*/

#define PPP_MESSAGE_INITCOUNT    8
#define PPP_MESSAGE_GROWTH       4
#define PPP_MESSAGE_LIMIT        32


struct ppp_opt;
struct ppp_cfg;

typedef struct ppp_msg {
   MESSAGE_HEADER_STRUCT   HEADER;
   uint_16                 COMMAND;
   uint_16                 PROTOCOL;
   PCB_PTR                 PCB;
   void (_CODE_PTR_        ORIG_FREE)(PCB_PTR);
   uint_32                 TIMEOUT;
   uint_32                 RETRY;
   void (_CODE_PTR_        CALL)(pointer, PCB_PTR, boolean);
   pointer                 PARAM;
   int_32                  DELTA;
   struct ppp_msg _PTR_    NEXT;
} PPP_MESSAGE, _PTR_ PPP_MESSAGE_PTR;

/*
** The PPP Options structure.  This structure contains all the
** negotiated options for an open PPP link.
*/

typedef struct ppp_opt {
   uint_32           ACCM;
   uint_32           MRU;
   uint_16           AP;
   uint_16           QP;            /* Not implemented */
   void (_CODE_PTR_  AP_Start)(_ppp_handle);
/*
   union {}          QP_DATA;
*/
   uint_32           MAGIC;
   boolean           PFC;
   boolean           ACFC;
   CP_CALL_PTR       CP;
} PPP_OPT, _PTR_ PPP_OPT_PTR;

extern PPP_OPT PPP_DEFAULT_OPTIONS;
extern PPP_OPT PPP_DEFAULT_OPTIONS_PPPOE;


/*
** The LCP Options state used during negotiation
*/

typedef struct {
   unsigned NEG_MRU   : 1;
   unsigned NEG_ACCM  : 1;
   unsigned NEG_AP    : 1;
   unsigned NEG_QP    : 1;    /* Not used */
   unsigned NEG_MAGIC : 1;    /* Not used */
   unsigned NEG_PFC   : 1;
   unsigned NEG_ACFC  : 1;
   unsigned NEG_FCS   : 1;    /* Not used */
   unsigned           : 0;
   uint_16  MRU;
   uint_32  ACCM;       /* LCP only negotiates 32 bits of the ACCM */
   uint_16  AP;
} LCP_NEG, _PTR_ LCP_NEG_PTR;

/*
** The LCP Configuration structure
*/

typedef struct {

   /* Statistics counters */
   uint_32        ST_LCP_RX_REJECT;
   uint_32        ST_LCP_RX_ECHO;
   uint_32        ST_LCP_RX_REPLY;
   uint_32        ST_LCP_RX_DISCARD;
   uint_32        ST_LCP_TX_REJECT;
   uint_32        ST_LCP_TX_ECHO;
   uint_32        ST_LCP_TX_REPLY;
   uint_32        ST_LCP_TX_DISCARD;

   _iopcb_handle  DEVICE;
   PPP_OPT        SEND_OPT;
   PPP_OPT        RECV_OPT;
   LCP_NEG        RECV_NEG;

} LCP_CFG, _PTR_ LCP_CFG_PTR;


/*
** The PPP Callback structure.  An array of these structures allows
** PPP to inform the application when certain events occur.
*/

typedef struct {
   void (_CODE_PTR_  CALLBACK)();
   pointer           PARAM;
} PPP_CALL_INTERNAL, _PTR_ PPP_CALL_INTERNAL_PTR;


/*
** The Protocol Callback structure.  A linked list of these structures allows
** PPP_rx_task to forward incoming packets to upper layer protocols.
*/

typedef struct prot_call {
   uint_16                 PROTOCOL;
   void (_CODE_PTR_        CALLBACK)(PCB_PTR, pointer);
   pointer                 PARAM;
   struct prot_call _PTR_  NEXT;
} PROT_CALL, _PTR_ PROT_CALL_PTR;


/*
** The PPP Configuration structure.
**
** MUTEX provides mutual exclusion on LINK_STATE, RECV_OPTIONS and
** SEND_OPTIONS, since they are read by both the Rx and Tx tasks,
** and written by the Rx task and any task that calls PPP_open or PPP_close.
**
** MUTEX is also used for mutual exclusion on the PROT_CALLS linked
** list.  We don't have to use the same mutex, but it saves space.
*/

typedef struct ppp_cfg {

   /* Statistics counters */
   uint_32                 ST_RX_RECEIVED;
   uint_32                 ST_RX_ABORTED;
   uint_32                 ST_RX_DISCARDED;
   uint_32                 ST_RX_RUNT;
   uint_32                 ST_RX_LONG;
   uint_32                 ST_RX_BAD_ADDR;
   uint_32                 ST_RX_BAD_CTRL;
   uint_32                 ST_RX_BAD_FCS;
   uint_32                 ST_TX_SENT;
   uint_32                 ST_TX_DISCARDED;
   uint_32                 ST_TX_BAD_FCS;

   uint_32                 VALID;
   boolean                 LINK_STATE;    /* TRUE if PPP link is up */
   boolean                 LINK_AUTH;     /* TRUE if the peer is authentic */
   _ppp_mutex              MUTEX;
   LCP_CFG                 LCP_STATE;
   PPPFSM_CFG              LCP_FSM;
   PPP_CALL_INTERNAL       LCP_CALL[PPP_CALL_MAX];

   _iopcb_handle           DEVICE;        /* for _iopcb_read/_iopcb_write */
   _queue_id               MSG_QUEUE;     /* for Tx task */
   _queue_id               RESERVED1;
   _pool_id                MSG_POOL;      /* for Tx task */

   PROT_CALL_PTR           PROT_CALLS;
   PPP_OPT volatile _PTR_  RECV_OPTIONS;
   PPP_OPT volatile _PTR_  SEND_OPTIONS;

   PAP_DATA                PAP_STATE;
   CHAP_DATA               CHAP_STATE;

   CCP_CFG                 CCP_STATE;
   PPPFSM_CFG              CCP_FSM;

   _rtcs_taskid            RX_TASKID;

} PPP_CFG, _PTR_ PPP_CFG_PTR;


extern PCB_PTR PPP_pcballoc (uint_16, uint_32);

extern uint_32 PPP_send_one      (PPP_CFG_PTR, uint_16, PCB_PTR);
extern uint_32 PPP_send_rexmit   (PPP_CFG_PTR, uint_16, PCB_PTR,
                                  uint_32, void (_CODE_PTR_)(pointer, PCB_PTR, boolean), pointer);
extern uint_32 PPP_send_restart  (PPP_CFG_PTR, uint_16);
extern uint_32 PPP_send_stop     (PPP_CFG_PTR, uint_16);
extern uint_32 PPP_send_shutdown (PPP_CFG_PTR, pointer);

extern void PPP_up(_ppp_handle);


#endif
/* EOF */
