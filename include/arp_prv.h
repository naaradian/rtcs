#ifndef __arp_prv_h__
#define __arp_prv_h__
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
*** File: arp_prv.h
***
*** Comments:
***      Address Resolution Protocol definitions.
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Constants
**
*/

/*
** ARP operation codes
*/
#define ARPOP_REQUEST   1
#define ARPOP_REPLY     2

/*
** RDN operation codes
*/
#define RDNOP_REQUEST   1
#define RDNOP_REPLY     2

/*
** ARP protocol types
*/
#define ARPPROT_IP      0x800
#define RDNPROT_IP      0x888

/*
** ARP cache hash table size
*/
#define ARPCACHE_SIZE      16
#define RDNCACHE_SIZE      16
/*
** Warning: ARPCACHE_HASH() should really be ((a) % ARPCACHE_SIZE),
** but this is faster when ARPCACHE_SIZE is a power of two.
** Note however, that if ARPCACHE_SIZE must be increased, a power
** of two is NOT a good choice -- an odd number (i.e. anything
** coprime to the size of the subnet) would be better.
*/
#define ARPCACHE_HASH(a)   ((a) & (ARPCACHE_SIZE - 1))
#define RDNCACHE_HASH(a)   ((a) & (RDNCACHE_SIZE - 1))
/*
** Allocation blocks for ARP cache entries
*/
#define ARPALLOC_SIZE   16
#define RDNALLOC_SIZE   16

/*
** ARP cache entry longevity (in milliseconds)
*/
//#define ARPTIME_RESEND_MIN             5000     /*  5 sec */
//#define ARPTIME_RESEND_MAX            30000     /* 30 sec */

#define ARPTIME_RESEND_MIN             1L     /*  1 ms */
#define ARPTIME_RESEND_MAX             2L     /*  2 ms */
//#define ARPTIME_EXPIRE_INCOMPLETE    180000L    /*  3 min */
#define ARPTIME_EXPIRE_INCOMPLETE      2L    /*  2ms */

#define ARPTIME_EXPIRE_COMPLETE     1200000L    /* 20 min */


#define RDNTIME_RESEND_MIN             5000     /*  5 sec */
#define RDNTIME_RESEND_MAX            30000     /* 30 sec */
#define RDNTIME_EXPIRE_INCOMPLETE    180000L    /*  3 min */
#define RDNTIME_EXPIRE_COMPLETE     1200000L    /* 20 min */





/***************************************
**
** Type definitions
**
*/

/*
** Contents of an ARP packet
*/
typedef struct
{
   uchar    LINKTYPE[2];
   uchar    PROTTYPE[2];
   uchar    LINKLEN[1];
   uchar    PROTLEN[1];
   uchar    OPCODE[2];
   uchar    LINKSRC[6];
   uchar    PROTSRC[4];
   uchar    LINKDEST[6];
   uchar    PROTDEST[4];
} ARP_PACKET, _PTR_ ARP_PACKET_PTR;

/*
** Contents of an RDN packet
*/
typedef struct
{
   uchar    LINKTYPE[2];
   uchar    PROTTYPE[2];
   uchar    LINKLEN[1];
   uchar    PROTLEN[1];
   uchar    OPCODE[2];
   uchar    LINKSRC[6];
   uchar    PROTSRC[4];
   uchar    LINKDEST[6];
   uchar    PROTDEST[4];
   //ok uchar	DATA[40]; //my
   //ok uchar	 DATA[100];
   //uchar	 DATA[1000];
   uchar	 DATA[1450];
//uchar	 DATA[20]; //temporary 20+28 to debug mode
} RDN_PACKET, _PTR_ RDN_PACKET_PTR;

typedef struct arp_entry {

   _ip_address             PADDR;
   uchar                   LADDR[6];   /* ignored when COMPLETE == FALSE */
   uchar                   RESERVED[2];

   IP_IF_PTR               HEAD;
   struct arp_entry _PTR_  PREV;
   struct arp_entry _PTR_  NEXT;

   boolean                 COMPLETE;
   boolean                 AGED;       /* ignored when COMPLETE == FALSE */
   boolean                 HIT;        /* ignored when COMPLETE == FALSE */

   /* There is at most one queued PCB on any incomplete entry */
   RTCSPCB_PTR             WAITING;
   _ip_address             WAITADDR;   /* ignored when RESEND_TIMER inactive */
   RTCS_LINKOPT_STRUCT     WAITOPT;    /* ignored when RESEND_TIMER inactive */

   uint_32                 RESEND_INTERVAL;
   TCPIP_EVENT             RESEND_TIMER;
   TCPIP_EVENT             AGE_TIMER;
   TCPIP_EVENT             EXPIRE_TIMER;

} ARP_ENTRY, _PTR_ ARP_ENTRY_PTR;


typedef struct rdn_entry {

   _ip_address             PADDR;
   uchar                   LADDR[6];   /* ignored when COMPLETE == FALSE */
   uchar                   RESERVED[2];

   IP_IF_PTR               HEAD;
   struct rdn_entry _PTR_  PREV;
   struct rdn_entry _PTR_  NEXT;

   boolean                 COMPLETE;
   boolean                 AGED;       /* ignored when COMPLETE == FALSE */
   boolean                 HIT;        /* ignored when COMPLETE == FALSE */

   /* There is at most one queued PCB on any incomplete entry */
   RTCSPCB_PTR             WAITING;
   _ip_address             WAITADDR;   /* ignored when RESEND_TIMER inactive */
   RTCS_LINKOPT_STRUCT     WAITOPT;    /* ignored when RESEND_TIMER inactive */

   uint_32                 RESEND_INTERVAL;
   TCPIP_EVENT             RESEND_TIMER;
   TCPIP_EVENT             AGE_TIMER;
   TCPIP_EVENT             EXPIRE_TIMER;

} RDN_ENTRY, _PTR_ RDN_ENTRY_PTR;




/*
** ARP Configuration
**    This information is persistent for the ARP layer.
*/
typedef struct {

   ARP_STATS      STATS;

   ARP_ENTRY_PTR  CACHE[ARPCACHE_SIZE];

   uchar          LADDR[6];
   uchar          RESERVED[2];

   ARP_PACKET     ARP_REQUEST;

} ARP_CFG, _PTR_ ARP_CFG_PTR;

/*
** RDN Configuration
**    This information is persistent for theRDN layer.
*/
typedef struct {

   RDN_STATS      STATS;

   RDN_ENTRY_PTR  CACHE[RDNCACHE_SIZE];

   uchar          LADDR[6];
   uchar          RESERVED[2];

   RDN_PACKET     RDN_REQUEST;

} RDN_CFG, _PTR_ RDN_CFG_PTR;



/***************************************
**
** Global variables
**
*/
extern pointer ARP_freelist;
extern pointer RDN_freelist;

#endif
/* EOF */
