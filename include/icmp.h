#ifndef __icmp_h__
#define __icmp_h__
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
*** File: icmp.h
***
*** Comments:
***    Definitions for the ICMP protocol layer.
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Constants
**
*/

/*
** ICMP message types
*/
#define ICMPTYPE_ECHO_REPLY   0
#define ICMPTYPE_DESTUNREACH  3
#define ICMPTYPE_SRCQUENCH    4
#define ICMPTYPE_REDIRECT     5
#define ICMPTYPE_ECHO_REQ     8
#define ICMPTYPE_TIMEEXCEED   11
#define ICMPTYPE_PARMPROB     12
#define ICMPTYPE_TIME_REQ     13
#define ICMPTYPE_TIME_REPLY   14
#define ICMPTYPE_INFO_REQ     15    /* obsolete */
#define ICMPTYPE_INFO_REPLY   16    /* obsolete */
#define ICMPTYPE_AM_REQ       17
#define ICMPTYPE_AM_REPLY     18

/*
** Destination Unreachable codes
*/
#define ICMPCODE_DU_NET_UNREACH     0
#define ICMPCODE_DU_HOST_UNREACH    1
#define ICMPCODE_DU_PROTO_UNREACH   2
#define ICMPCODE_DU_PORT_UNREACH    3
#define ICMPCODE_DU_NEEDFRAG        4
#define ICMPCODE_DU_SRCROUTE        5
#define ICMPCODE_DU_NET_UNKNOWN     6
#define ICMPCODE_DU_HOST_UNKNOWN    7
#define ICMPCODE_DU_HOST_ISOLATED   8
#define ICMPCODE_DU_NET_PROHIB      9
#define ICMPCODE_DU_HOST_PROHIB     10
#define ICMPCODE_DU_NET_TOS         11
#define ICMPCODE_DU_HOST_TOS        12

/*
** Redirect codes
*/
#define ICMPCODE_RD_NET             0
#define ICMPCODE_RD_HOST            1
#define ICMPCODE_RD_NET_TOS         2
#define ICMPCODE_RD_HOST_TOS        3

/*
** Time Exceeded codes
*/
#define ICMPCODE_TE_TTL             0
#define ICMPCODE_TE_REASM           1


/***************************************
**
** Type definitions
**
*/

/*
** ICMP echo parameters
*/

typedef struct icmp_parm {
   TCPIP_PARM                    COMMON;
   struct icmp_parm _PTR_        NEXT;
   struct icmp_parm _PTR_ _PTR_  PREV;
   TCPIP_EVENT                   EXPIRE;
   _ip_address                   ipaddress;
   uint_32                       timeout;
   uint_16                       id;
   uint_16                       seq;
} ICMP_PARM, _PTR_ ICMP_PARM_PTR;


/***************************************
**
** Prototypes
**
*/

extern uint_32 ICMP_init
(
   void
);

extern void ICMP_send_echo
(
   ICMP_PARM_PTR
);

extern void ICMP_send_error
(
   uint_8         type,    /* [IN] the type to send */
   uint_8         code,    /* [IN] the code to send */
   uint_32        param,   /* [IN] the icmp parameter */
   RTCSPCB_PTR    pcb,     /* [IN] the packet which caused the error */
   int_32         layer    /* [IN] IP layer, relative to current */
);

extern void ICMP_service
(
   RTCSPCB_PTR,
   pointer
);


#endif
/* EOF */
