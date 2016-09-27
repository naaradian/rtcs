#ifndef __icmp_prv_h__
#define __icmp_prv_h__
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
*** File: icmp_prv.h
***
*** Comments:
***    Private definitions for the ICMP protocol layer.
***
************************************************************************
*END*******************************************************************/

#include "ip_prv.h"     /* For IP_HEADER */


/***************************************
**
** Code macros
**
*/

#define ICMPTYPE_ISQUERY(t)   (((t) == ICMPTYPE_ECHO_REPLY) \
                            || ((t) == ICMPTYPE_ECHO_REQ)   \
                            || (((t) >= ICMPTYPE_TIME_REQ) && ((t) <= ICMPTYPE_AM_REPLY)))


/***************************************
**
** Type definitions
**
*/

/*
** Generic ICMP packet header
*/
typedef struct
{
   uchar    TYPE[1];
   uchar    CODE[1];
   uchar    CHECKSUM[2];
} ICMP_HEADER, _PTR_ ICMP_HEADER_PTR;

/*
** ICMP Redirect packet
*/
typedef struct
{
   ICMP_HEADER HEAD;
   uchar       DATA[4];
   IP_HEADER   IP;
} ICMP_ERR_HEADER, _PTR_ ICMP_ERR_HEADER_PTR;

/*
** ICMP Echo packet
*/
typedef struct
{
   ICMP_HEADER HEAD;
   uchar       ID[2];
   uchar       SEQ[2];
} ICMP_ECHO_HEADER, _PTR_ ICMP_ECHO_HEADER_PTR;

/*
** ICMP Configuration.  This information is persistent for the ICMP layer.
*/
typedef struct {

   ICMP_STATS     STATS;

   ICMP_PARM_PTR  ECHO_PARM_HEAD;
   uint_16        ECHO_SEQ;
   uint_16        RESERVED;

} ICMP_CFG_STRUCT, _PTR_ ICMP_CFG_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/

extern void ICMP_send_error_internal
(
   uint_8         type,    /* [IN] the type to send */
   uint_8         code,    /* [IN] the code to send */
   uint_32        param,   /* [IN] the icmp parameter */
   IP_HEADER_PTR  iph,     /* [IN] the IP header */
   RTCSPCB_PTR    origpcb, /* [IN] the bad packet pcb */
   uint_32        maxlen   /* [IN[ the max data len to send, 0 = default */
);


#endif
/* EOF */
