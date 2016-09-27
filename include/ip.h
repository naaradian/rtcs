#ifndef __ip_h__
#define __ip_h__
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
*** File: ip.h
***
*** Comments:
***    Definitions for the Internet protocol layer.
***
************************************************************************
*END*******************************************************************/

#include "rtcstime.h"

/***************************************
**
** Constants
**
*/

/*
** IP datagram sizes
*/

#define IP_HEADSIZE        20       /* sizeof(IP_HEADER)                 */

#define IP_DEFAULT_MTU     576      /* minimum IP datagram size which    */
                                    /* must be supported by all IP hosts */
#define IP_MAX_MTU         0xFFFF   /* size of largest IP datagram       */

/*
** default IP parameters for outgoing packets
*/
#define IP_VERSION         4        /* Version 4               */
#define IP_VERSION_4       4        /* Version 4               */
#define IP_VERSION_6       6        /* Version 6               */
#define IPTTL_DEFAULT      60       /* Time to Live (60 hops)  */
//#define IPTTL_DEFAULT      2       /* Time to Live (60 hops)  */
#define IPTOS_DEFAULT      0        /* Type of Service         */

#define IP_HEADER_LENGTH_MASK  0x0F

#define IPTTL(ttl)         ((uint_32)(ttl) <<  8)
#define IPTOS(tos)         ((uint_32)(tos) << 16)
#define IPDF(df)           ((uint_32)(df)  << 24)

/*
**
** Ports < IPPORT_RESERVED are reserved for
**  privileged processes (e.g. root).
**
** Ports > IPPORT_USERRESERVED are reserved
**  for servers, not necessarily privileged.
*/
#define IPPORT_RESERVED          1024
#define IPPORT_USERRESERVED      5000


/***************************************
**
** Type definitions
**
*/

/*
** Multicast group members
*/

typedef struct MCB {
   ip_mreq           IGRP;       /* the group description */
   uint_32           UCOUNT;     /* usage counter */
   struct MCB _PTR_  NEXT;       /* pointer to the next in the list */

   /* used only by the interface part */
   TCPIP_EVENT TIMER;
   boolean     RUNNING_TIMER; /* true if the timer is running */
   boolean     LAST_REPORTER; /* true if we are the last host to report on this group */
   uint_32     UNSOLICITED_REPORT_COUNTER;   /* the number of unsolicited reports remaining to send */

   /*
   ** it's unnecessary to store a state (i.e. non-member, delaying-member, idle-member)
   ** because non-members don't have a MC_MEMBER, delaying-member is when
   ** the running_timer is true, and idle-member when running_timer is true
   */

} MC_MEMBER, _PTR_ MC_MEMBER_PTR;

/*
** Function for servicing packets
*/

typedef void (_CODE_PTR_ IP_SERVICE_FN)(RTCSPCB_PTR, pointer);

/*
** Internet Channel Block
*/
typedef struct ICB {

   struct ICB _PTR_  NEXT;          /* next ICB in the chain      */
   uint_32           PROTOCOL;      /* protocol for connection    */

   IP_SERVICE_FN     SERVICE;       /* Owner's service function    */
   pointer           PRIVATE;       /* Owner's configuration block */

} ICB_STRUCT, _PTR_ ICB_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/

extern uint_32 IP_init
(
   void
);

extern uint_32 IP_init1
(
   void
);

extern ICB_STRUCT_PTR IP_open
(
   uchar             ,     /* [IN]  Protocol to use */
   IP_SERVICE_FN     ,     /* [IN]  Packet receive function */
   pointer           ,     /* [IN]  Owner's config block */
   uint_32_ptr             /* [OUT] return code */
);

extern uint_32 IP_send
(
   RTCSPCB_PTR       ,     /* [IN] the packet to send */
   uint_32           ,     /* [IN] transport protocol, TTL and TOS */
   _ip_address       ,     /* [IN] the destination interface (0 = any) */
   _ip_address       ,     /* [IN] the ultimate destination */
   uint_32                 /* [IN] optional flags */
);

extern uint_32 IP_send_IF
(
   RTCSPCB_PTR       ,     /* [IN] the packet to send */
   uint_32           ,     /* [IN] transport protocol, TTL and TOS */
   pointer                 /* [IN] the destination interface */
);

extern void IP_service
(
   RTCSPCB_PTR             /* [IN] received packet */
);

extern _ip_address IP_source
(
   RTCSPCB_PTR             /* [IN] packet to find source of */
);

extern _ip_address IP_dest
(
   RTCSPCB_PTR             /* [IN] packet to find destination of */
);




#endif
/* EOF */
