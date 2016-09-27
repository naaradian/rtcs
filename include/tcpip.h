#ifndef __tcpip_h__
#define __tcpip_h__
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
*** File: tcpip.h
***
*** Comments:
***    Definitions for the TCP/IP layer error codes.
***
************************************************************************
*END*******************************************************************/

#include "rtcstime.h"
#include "ipradix.h"
#include "checksum.h"
#include "ip.h"
#include "icmp.h"
#include "igmp.h"
#include "udp.h"
#include "tcp.h"


/***************************************
**
** Type definitions
**
*/

typedef struct {
   MESSAGE_HEADER_STRUCT   HEAD;
   void (_CODE_PTR_        COMMAND)(pointer);
   pointer                 DATA;
} TCPIP_MESSAGE, _PTR_ TCPIP_MESSAGE_PTR;

/*  TCP Configuration
**    This information is persistent for the TCP layer.
*/
typedef struct tcpip_cfg {

   uint_32     status;           /* task status - RTCS_OK => running */

} TCPIP_CFG_STRUCT, _PTR_ TCPIP_CFG_STRUCT_PTR;


/***************************************
**
** Function-specific type definitions
**
*/


/* IPIF_*() */
typedef struct {
   TCPIP_PARM           COMMON;
   pointer              mhandle;
   _rtcs_if_handle      ihandle;
   RTCS_IF_STRUCT_PTR   if_ptr;
   _ip_address          address;
   _ip_address          locmask;
   _ip_address          network;
   _ip_address          netmask;
} IPIF_PARM, _PTR_ IPIF_PARM_PTR;


/* SOCK_selectall() and SOCK_selectset() */

typedef struct sockselect_parm {
   TCPIP_PARM                          COMMON;
   struct sockselect_parm _PTR_        NEXT;
   struct sockselect_parm _PTR_ _PTR_  PREV;
   TCPIP_EVENT                         EXPIRE;
   _rtcs_taskid                        owner;
   uint_32 _PTR_                       sock_ptr;
   uint_32 _PTR_                       writesock_ptr;
   uint_32 _PTR_                       exceptsock_ptr;
   uint_32                             sock_count;
   uint_32                             writesock_count;
   uint_32                             timeout;
   uint_32                             sock;
} SOCKSELECT_PARM, _PTR_ SOCKSELECT_PARM_PTR;


/*
typedef struct sockselect_parm {
   TCPIP_PARM                          COMMON;
   struct sockselect_parm _PTR_        NEXT;
   struct sockselect_parm _PTR_ _PTR_  PREV;
   TCPIP_EVENT                         EXPIRE;
   _rtcs_taskid                        owner;
   uint_32 _PTR_                       sock_ptr;
   uint_32                             sock_count;
   uint_32                             timeout;
   uint_32                             sock;
} SOCKSELECT_PARM, _PTR_ SOCKSELECT_PARM_PTR;
*/

/***************************************
**
** Globals
**
*/

/* The TCP tick server */
extern uint_32 (_CODE_PTR_ TCP_tick)(void);


/***************************************
**
** Prototypes
**
*/

void TCPIP_task (pointer, pointer);
void TCPIP_task1 (pointer, pointer);
void TCPIP_task2 (pointer, pointer);

/* Select functions */
extern void    SOCK_selectall       (SOCKSELECT_PARM_PTR);
extern void    SOCK_selectset       (SOCKSELECT_PARM_PTR);

extern boolean SOCK_select_activity (SOCKET_STRUCT_PTR);
extern boolean SOCK_select_activity1 (SOCKET_STRUCT_PTR, int);

extern void    SOCK_select_block    (SOCKSELECT_PARM_PTR);
extern void    SOCK_select_unblock  (SOCKSELECT_PARM_PTR);
extern boolean SOCK_select_expire   (TCPIP_EVENT_PTR);
extern void    SOCK_select_signal   (uint_32);

/* BOOTP and DHCP common functions */

uint_32 BOOT_init
(
   void
);

uint_32 BOOT_open
(
   void (_CODE_PTR_)(RTCSPCB_PTR, UCB_STRUCT_PTR)
);

uint_32 BOOT_close
(
   void
);

uint_32 BOOT_send
(
   RTCSPCB_PTR       ,        /* [IN]     outgoing packet  */
   pointer                    /* [IN]     target interface */
);

void BOOT_service
(
   RTCSPCB_PTR       ,        /* [IN/OUT] incoming packet  */
   UCB_STRUCT_PTR             /* [IN]     target UCB       */
);


#endif
/* EOF */
