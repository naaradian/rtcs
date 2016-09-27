#ifndef __ipcp_h__
#define __ipcp_h__
/*HEADER****************************************************************
************************************************************************
***
*** Copyright (c) 1992-2003 ARC International
*** All rights reserved
***
*** Portions of this software were written by Carnegie Mellon University
*** Copyright (c) 1989 Carnegie Mellon University.
***
*** This software embodies materials and concepts which are
*** confidential to ARC International and is made available solely
*** pursuant to the terms of a written license agreement with
*** ARC International.
***
*** File: ipcp.h
***
*** Comments:  This file contains the defines and data structure
***            definitions required by IPCP.
***
************************************************************************
*END*******************************************************************/

/*
** ipcp.h - IP Control Protocol definitions.
**
** Copyright (c) 1989 Carnegie Mellon University.
** All rights reserved.
**
** Redistribution and use in source and binary forms are permitted
** provided that the above copyright notice and this paragraph are
** duplicated in all such forms and that any documentation,
** advertising materials, and other materials related to such
** distribution and use acknowledge that the software was developed
** by Carnegie Mellon University.  The name of the
** University may not be used to endorse or promote products derived
** from this software without specific prior written permission.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
** WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <ppp.h>
#include "pppfsm.h"

#include "ip_prv.h"


/***************************************
**
** Constants
**
*/

/*
** PPP Protocol Numbers
*/

#define PPP_PROT_IP     0x0021
#define PPP_PROT_IPCP   0x8021


/*
** IPCP Configuration Options
*/

#define IPCP_CI_ADDR    3     /* IP Address */
#define IPCP_CI_DNS     129   /* Primary DNS server */


/***************************************
**
** Macros
**
*/

/*
** PPP error code -> RTCS error code conversion
*/

#define PPP_TO_RTCS_ERROR(x) ((x)?((x)|0x2000):(x))


/***************************************
**
** Type definitions
**
*/

/*
** The IPCP Options state used during negotiation
*/

typedef struct {
   unsigned    NEG_ADDRS : 1;    /* Not used */
   unsigned    NEG_CP    : 1;    /* Not used */
   unsigned    NEG_ADDR  : 1;
   unsigned    NEG_DNS   : 1;
   unsigned              : 0;
   _ip_address ADDR;
   _ip_address DNS;
} IPCP_NEG, _PTR_ IPCP_NEG_PTR;

typedef struct {

   unsigned    NEG_LOCAL_DNS      : 1;
   unsigned    NEG_REMOTE_DNS     : 1;

   unsigned    ACCEPT_LOCAL_ADDR  : 1;
   unsigned    ACCEPT_REMOTE_ADDR : 1;
   unsigned    ACCEPT_LOCAL_DNS   : 1;
   unsigned    ACCEPT_REMOTE_DNS  : 1;

   unsigned    DEFAULT_NETMASK    : 1;
   unsigned    DEFAULT_ROUTE      : 1;

   unsigned                       : 0;

   _ip_address LOCAL_ADDR;
   _ip_address REMOTE_ADDR;
   _ip_address NETMASK;
   _ip_address LOCAL_DNS;
   _ip_address REMOTE_DNS;

} IPCP_INIT, _PTR_ IPCP_INIT_PTR;

typedef struct {
   _ip_address ADDR;    /* Negotiated IP address */
   _ip_address DNS;     /* Negotiated DNS server address */
} IPCP_OPT, _PTR_ IPCP_OPT_PTR;

/*
** The IPCP Configuration structure
*/

typedef struct {
   _ppp_handle       HANDLE;        /* PPP channel */
   void (_CODE_PTR_  CALLUP)(pointer);    /* previous PPP_LINK_UP callback */
   pointer           PARAMUP;             /* previous PPP_LINK_UP parameter */
   void (_CODE_PTR_  CALLDOWN)(pointer);  /* previous PPP_LINK_DOWN callback */
   pointer           PARAMDOWN;           /* previous PPP_LINK_DOWN parameter */
   void (_CODE_PTR_  IP_UP)   (pointer);  /* application UP callback */
   void (_CODE_PTR_  IP_DOWN) (pointer);  /* application DOWN callback */
   pointer           IP_PARAM;            /* parameter to IP_UP and IP_DOWN */
   IPCP_INIT         INIT;          /* Initial values provided by application */
   IPCP_NEG          NEG;           /* Options under negotiation */
   IPCP_OPT          LOPT;          /* Negotiated local options */
   IPCP_OPT          POPT;          /* Negotiated peer options */
   IPIF_PARM         BIND_PARMS;    /* Parameters to IPIF_[un]bind */
   IPIF_PARM         GATE_PARMS;    /* Parameters to IPIF_gate_{add,remove} */
   PPPFSM_CFG        FSM;           /* IPCP automaton state */
} IPCP_CFG_STRUCT, _PTR_ IPCP_CFG_STRUCT_PTR;


/***************************************
**
** Function-specific type definitions
**
*/

/* IPCP_bind() */
typedef struct {
   TCPIP_PARM              COMMON;
   _rtcs_if_handle         handle;  /* [IN] the RTCS interface state structure */
   IPCP_DATA_STRUCT_PTR    data;    /* [IN] IPCP parameters */
} TCPIP_PARM_IPCP;


/***************************************
**
** Global variables
**
*/

extern PPPFSM_CALL IPCP_FSM_CALL;


/***************************************
**
** Prototypes
**
*/

uint_32 IPCP_init    (IP_IF_PTR);
uint_32 IPCP_destroy (IP_IF_PTR);
uint_32 IPCP_send    (IP_IF_PTR, RTCSPCB_PTR, _ip_address, _ip_address, pointer);
void IPCP_recv(PCB_PTR, pointer);

void IPCP_bind(TCPIP_PARM_IPCP _PTR_);


#endif
/* EOF */
