#ifndef __iptunnel_h__
#define __iptunnel_h__
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
*** File: iptunnel.h
***
*** Comments:  This file contains the defines, externs and data
***            structure definitions required by IP over IP encapsulation.
***
**************************************************************************
*END*********************************************************************/

#include "ip_prv.h"


/*********************************************************
**
** IPIP CONSTANTS
*/

#define IPIP_INITIAL_COUNT  4
#define IPIP_GROW_COUNT     4


/*********************************************************
**
** IPIP Structures
*/

/*
** IPIP_TUNNEL
**    Contains information about an IP over IP tunnel
*/
typedef struct iptunnel_data {
   struct iptunnel_data _PTR_ NEXT;       /* Points to next node in list */
   _ip_address    INNER_SOURCE;           /* Embedded header source */
   _ip_address    INNER_SOURCE_NETMASK;   /* Embedded header source mask */
   _ip_address    INNER_DEST;             /* Embedded header destination */
   _ip_address    INNER_DEST_NETMASK;     /* Embedded header dest mask */
   _ip_address    OUTER_SOURCE;           /* Source for outer header */
   _ip_address    OUTER_DEST;             /* Destination for outer header */
   uint_32        FLAGS;                  /* Behaviour flags */
} IPIP_TUNNEL, _PTR_ IPIP_TUNNEL_PTR;


/*
** IPIP_PARM
**    Used to pass information from the application to RTCS
*/
typedef struct  {
   TCPIP_PARM     COMMON;                 /* Common to all parm structs */
   IPIP_TUNNEL    TUNNEL;
} IPIP_PARM, _PTR_ IPIP_PARM_PTR;


/*
** IPIP_CFG_STRUCT
**    Contains the configuration information for the IP Tunnel device
*/
typedef struct iptunnel_config {
   IPIP_TUNNEL_PTR   NEXT;             /* Linked list of tunnels */
   _rtcs_part        IPIP_PART;        /* Tunnel information part */
   IP_IF_PTR         IPIP_IF;          /* IPIP interface pointer */
   uint_32           TUNNELS;          /* Number of tunnels */
} IPIP_CFG_STRUCT,  _PTR_ IPIP_CFG_STRUCT_PTR;



#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 IPIP_init (void);

extern void IPIP_service (RTCSPCB_PTR pcb_ptr, pointer dummy);

extern void IPIP_insert (IPIP_PARM_PTR);
extern void IPIP_delete (IPIP_PARM_PTR);

extern uint_32 IPIP_open  (IP_IF_PTR);
extern uint_32 IPIP_close (IP_IF_PTR);

extern uint_32 IPIP_send (IP_IF_PTR, RTCSPCB_PTR, _ip_address, _ip_address, pointer);

extern void IPIP_send_internal (RTCSPCB_PTR);

extern uint_32 IPIP_MTU (pointer);

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
