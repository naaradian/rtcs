#ifndef __udp_prv_h__
#define __udp_prv_h__
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
*** File: udp_prv.h
***
*** Comments:  This file contains the private User Datagram Protocol
***            definitions.  For more details, refer to RFC768.
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Type definitions
**
*/

typedef struct {
   uchar    SRC_PORT[2];
   uchar    DEST_PORT[2];
   uchar    LENGTH[2];
   uchar    CHECKSUM[2];
} UDP_HEADER, _PTR_ UDP_HEADER_PTR;

/*
** UDP Configuration.  This information is persistent for the UDP layer.
*/
typedef struct {

   UDP_STATS      STATS;

   UCB_STRUCT_PTR BOUND_UCB_HEAD;   /* The head of the bound UCB chain      */
   uint_16        LAST_PORT;        /* Last used UDP port                   */
   uint_16        RESERVED0;

   UCB_STRUCT_PTR GROUND_UCB_HEAD;  /* The head of the ground UCB chain     */
   UCB_STRUCT_PTR LBOUND_UCB_HEAD;  /* The head of local IP bound UCB chain */
   UCB_STRUCT_PTR OPEN_UCB_HEAD;    /* The head of the open UCB chain       */

} UDP_CFG_STRUCT, _PTR_ UDP_CFG_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/

uint_16 UDP_source
(
   RTCSPCB_PTR                /* [IN] packet to find source of */
);


#endif
/* EOF */
