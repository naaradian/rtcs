#ifndef __ipe_h__
#define __ipe_h__
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
*** File: ip-e.h
***
*** Comments:  This file contains the private defines, externs and
***            data structure definitions required by the IP-E layer.
***
************************************************************************
*END*******************************************************************/

#include "ip_prv.h"


/***************************************
**
** Ethernet protocols
**
*/

#define ENETPROT_IP     0x800
#define ENETPROT_ARP 0x806
#define ENETPROT_RDN 0x888

/***************************************
**
** Prototypes
**
*/

uint_32 IPE_open  (IP_IF_PTR);
uint_32 IPE_close (IP_IF_PTR);

uint_32 IPE_send_IP (IP_IF_PTR, RTCSPCB_PTR, _ip_address, _ip_address, pointer);
uint_32 IPE_send_IP_ucast  (IP_IF_PTR, RTCSPCB_PTR, uchar[6]);
uint_32 IPE_send_ARP_bcast (IP_IF_PTR, RTCSPCB_PTR);
uint_32 IPE_send_ARP_ucast (IP_IF_PTR, RTCSPCB_PTR, uchar[6]);

uint_32 IPE_send_RDN_bcast (IP_IF_PTR, RTCSPCB_PTR);
uint_32 IPE_send_RDN_ucast (IP_IF_PTR, RTCSPCB_PTR, uchar[6]);

void IPE_recv_IP  (PCB_PTR, pointer);
void IPE_recv_ARP (PCB_PTR, pointer);
void IPE_recv_RDN (PCB_PTR, pointer);

uint_32 IPE_join  (IP_IF_PTR, _ip_address);
uint_32 IPE_leave (IP_IF_PTR, _ip_address);


#endif
/* EOF */
