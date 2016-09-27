#ifndef __ccp_h__
#define __ccp_h__
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
*** File: ccp.h
***
*** Comments:  This file contains the defines and data structure
***            definitions required by CCP.
***
************************************************************************
*END*******************************************************************/

/*
** CCP-specific packet types
*/

#define CCP_CODE_RESET_REQ 14      /* Reset Request */
#define CCP_CODE_RESET_ACK 15      /* Reset Ack */


/*
** CCP Configuration Options
*/

#define CCP_CI_PRED1    1     /* Predictor type 1 */
#define CCP_CI_LZS      17    /* Stac LZS */


struct ccp_opt;
struct ppp_opt;
struct ppp_cfg;

/*
** The Compression Protocol Data Union.  This union contains the private
** data required by each supported compression method.
*/

typedef union {
   uint_32     DUMMY;
} CP_DATA, _PTR_ CP_DATA_PTR;

/*
** The Compression Procotol Call Table structure.  This structure
** contains pointers to functions that compress and decompress packets.
*/

typedef struct {
   void    (_CODE_PTR_ CP_init)  (CP_DATA_PTR, struct ccp_opt _PTR_);
   PCB_PTR (_CODE_PTR_ CP_comp)  (CP_DATA_PTR, PCB_PTR,
                                  struct ppp_cfg _PTR_, struct ppp_opt _PTR_);
   PCB_PTR (_CODE_PTR_ CP_decomp)(CP_DATA_PTR, PCB_PTR,
                                  struct ppp_cfg _PTR_, struct ppp_opt _PTR_);

   void    (_CODE_PTR_ CP_resetreq) (CP_DATA_PTR, PCB_PTR);
   void    (_CODE_PTR_ CP_resetack) (CP_DATA_PTR, PCB_PTR);
} CP_CALL, _PTR_ CP_CALL_PTR;

/*
** The CCP Options structure.  This structure contains all the
** negotiated options for an open CCP link.
*/

typedef struct ccp_opt {
   CP_CALL_PTR    CP;
   union {
      uint_32     DUMMY;
#ifdef PPP_CP_LZS
      struct {
         uint_16  HIST;
         uchar    CHECK;
      }           LZS;
#endif
   }              DATA;
} CCP_OPT, _PTR_ CCP_OPT_PTR;

/*
** The CCP Options state used during negotiation
*/

typedef struct {
   unsigned NEG_PRED1 : 1;    /* Not used */
   unsigned NEG_LZS   : 1;    /* Not used */
   unsigned           : 0;
#ifdef PPP_CP_LZS
   uint_16  LZS_HIST;
   uchar    LZS_CHECK;
#endif
} CCP_NEG, _PTR_ CCP_NEG_PTR;

/*
** The CCP Configuration structure
*/

typedef struct {

   /* Statistics counters */
   uint_32  ST_CCP_RX_RESET;
   uint_32  ST_CCP_RX_ACK;

   CCP_OPT  SEND_OPT;
   CCP_OPT  RECV_OPT;
   CCP_NEG  RECV_NEG;
   CP_DATA  SEND_DATA;
   CP_DATA  RECV_DATA;
} CCP_CFG, _PTR_ CCP_CFG_PTR;


/*
** Prototypes
*/

extern uint_32 CCP_init    (_ppp_handle);
extern uint_32 CCP_destroy (_ppp_handle);

#define CCP_lowerup(handle)   PPPFSM_lowerup(&((PPP_CFG_PTR)(handle))->CCP_FSM)
#define CCP_lowerdown(handle) PPPFSM_lowerdown(&((PPP_CFG_PTR)(handle))->CCP_FSM)
#define CCP_open(handle)      PPPFSM_open(&((PPP_CFG_PTR)(handle))->CCP_FSM, 0)
#define CCP_close(handle)     PPPFSM_close(&((PPP_CFG_PTR)(handle))->CCP_FSM)
#define CCP_input             PPPFSM_input

#endif
/* EOF */
