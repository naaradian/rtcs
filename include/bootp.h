#ifndef __bootp_h__
#define __bootp_h__
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
*** File: bootp.h
***
*** Comments:  Bootstrap Protocol definitions.
***
************************************************************************
*END*******************************************************************/

#include "ip_prv.h"


/***************************************
**
** Constants
**
*/

#define BOOTP_TIMEOUT_MIN     4000     /* 4 sec */
#define BOOTP_TIMEOUT_MAX     64000    /* 64 sec */

#define BOOTPOP_BOOTREQUEST   1
#define BOOTPOP_BOOTREPLY     2

#define BOOTPFLAG_BROADCAST   0x8000

#define BOOTP_MAGIC           0x63825363L

/*
** BOOTP options
*/
#define BOOTPOPT_PAD    0
#define BOOTPOPT_END    255
#define BOOTPOPT_MASK   1


/***************************************
**
** Type definitions
**
*/

/*
** Contents of a BOOTP packet
*/
typedef struct
{
   uchar    OP[1];
   uchar    HTYPE[1];
   uchar    HLEN[1];
   uchar    HOPS[1];
   uchar    XID[4];
   uchar    SECS[2];
   uchar    FLAGS[2];
   uchar    CIADDR[4];
   uchar    YIADDR[4];
   uchar    SIADDR[4];
   uchar    GIADDR[4];
   uchar    CHADDR[16];
} BOOTP_HEADER, _PTR_ BOOTP_HEADER_PTR;

typedef struct
{
   uchar    SNAME[64];
   uchar    FILE[128];
   uchar    VEND[64];
} BOOTP_DATA, _PTR_ BOOTP_DATA_PTR;

typedef struct
{
   BOOTP_HEADER   HEAD;
   BOOTP_DATA     DATA;
} BOOTP_PACKET, _PTR_ BOOTP_PACKET_PTR;

/*
** BOOTP Configuration
*/
typedef struct
{
   uint_32        XID;
   uint_32        TIMEOUT;
   uint_32        SECS;
   TCPIP_EVENT    RESEND;
   BOOTP_HEADER   PACKET;
} BOOTP_CFG, _PTR_ BOOTP_CFG_PTR;


/***************************************
**
** Function-specific type definitions
**
*/

/* BOOTP_open() */
typedef struct {
   TCPIP_PARM              COMMON;
   _rtcs_if_handle         handle;  /* [IN] the RTCS interface state structure */
   BOOTP_DATA_STRUCT_PTR   data;    /* [IN/OUT] BOOTP parameters */
   BOOTP_CFG               config;
} TCPIP_PARM_BOOTP;


/***************************************
**
** Prototypes
**
*/

void BOOTP_open
(
   TCPIP_PARM_BOOTP _PTR_ parms
);

uint_32 BOOTP_close
(
   IP_IF_PTR                     /* [IN]     IP interface structure */
);

boolean BOOTP_send
(
   TCPIP_EVENT_PTR               /* [IN/OUT] the resend event       */
);

void BOOTP_service
(
   RTCSPCB_PTR                   /* [IN]     BOOTREPLY packet       */
);


#endif
/* EOF */
