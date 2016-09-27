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
*** File: arp.c
***
*** Comments:  This file contains the implementation of the
***            ARP server.
***
*** Limitations:  Although ARP can be used over any multiple-access
***               medium, this implementation will work only over an
***               Ethernet link layer.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "arp.h"
#include "arp_prv.h"
#include "ip-e.h"

/* Functions for accessing the ARP cache */
static ARP_ENTRY_PTR ARP_find   (IP_IF_PTR, _ip_address);
static ARP_ENTRY_PTR ARP_insert (IP_IF_PTR, _ip_address);
static void          ARP_delete (ARP_ENTRY_PTR);

/* The various timer events */
static boolean ARP_send   (TCPIP_EVENT_PTR);
static boolean ARP_age    (TCPIP_EVENT_PTR);
static boolean ARP_expire (TCPIP_EVENT_PTR);


/* Functions for accessing the RDN cache */
static RDN_ENTRY_PTR RDN_find   (IP_IF_PTR, _ip_address);
static RDN_ENTRY_PTR RDN_insert (IP_IF_PTR, _ip_address);
static void          RDN_delete (RDN_ENTRY_PTR);

/* The various timer events */
static boolean RDN_send   (TCPIP_EVENT_PTR);
static boolean RDN_age    (TCPIP_EVENT_PTR);
static boolean RDN_expire (TCPIP_EVENT_PTR);

/* Macros */

#define htone(p,x)   ((p)[0] = (x)[0], \
                      (p)[1] = (x)[1], \
                      (p)[2] = (x)[2], \
                      (p)[3] = (x)[3], \
                      (p)[4] = (x)[4], \
                      (p)[5] = (x)[5])

#define ntohe(p,x)   ((x)[0] = (p)[0] & 0xFF, \
                      (x)[1] = (p)[1] & 0xFF, \
                      (x)[2] = (p)[2] & 0xFF, \
                      (x)[3] = (p)[3] & 0xFF, \
                      (x)[4] = (p)[4] & 0xFF, \
                      (x)[5] = (p)[5] & 0xFF)

#define ARP_REFRESH(a)                                         \
         (a)->RESEND_INTERVAL = ARPTIME_RESEND_MIN;            \
         (a)->RESEND_TIMER.TIME = 0;                           \
         (a)->EXPIRE_TIMER.TIME = ARPTIME_EXPIRE_INCOMPLETE;   \
         TCPIP_Event_add(&(a)->RESEND_TIMER);                  \
         TCPIP_Event_add(&(a)->EXPIRE_TIMER)

#define RDN_REFRESH(a)                                         \
         (a)->RESEND_INTERVAL = RDNTIME_RESEND_MIN;            \
         (a)->RESEND_TIMER.TIME = 0;                           \
         (a)->EXPIRE_TIMER.TIME = RDNTIME_EXPIRE_INCOMPLETE;   \
         TCPIP_Event_add(&(a)->RESEND_TIMER);                  \
         TCPIP_Event_add(&(a)->EXPIRE_TIMER)

#define ARP_COMPLETE(a,l)                                      \
         _mem_copy(l, (a)->LADDR, sizeof((a)->LADDR));         \
         (a)->COMPLETE = TRUE;                                 \
         (a)->AGED     = FALSE;                                \
         (a)->HIT      = FALSE;                                \
         (a)->AGE_TIMER.TIME = ARPTIME_EXPIRE_COMPLETE         \
                             - ARPTIME_EXPIRE_INCOMPLETE;      \
         (a)->EXPIRE_TIMER.TIME = ARPTIME_EXPIRE_COMPLETE;     \
         TCPIP_Event_add(&(a)->AGE_TIMER);                     \
         TCPIP_Event_add(&(a)->EXPIRE_TIMER)

#define RDN_COMPLETE(a,l)                                      \
         _mem_copy(l, (a)->LADDR, sizeof((a)->LADDR));         \
         (a)->COMPLETE = TRUE;                                 \
         (a)->AGED     = FALSE;                                \
         (a)->HIT      = FALSE;                                \
         (a)->AGE_TIMER.TIME = RDNTIME_EXPIRE_COMPLETE         \
                             - ARPTIME_EXPIRE_INCOMPLETE;      \
         (a)->EXPIRE_TIMER.TIME = RDNTIME_EXPIRE_COMPLETE;     \
         TCPIP_Event_add(&(a)->AGE_TIMER);                     \
         TCPIP_Event_add(&(a)->EXPIRE_TIMER)

/*
** Each ARP cache entry basically has four states:
** complete, aged/quiet, aged/active, incomplete.
**
** For every entry, there exists three timers.
**
**    The resend timer periodically causes an ARP request
**    to be sent.
**
**    The age timer causes complete->aged/quiet and
**    aged->incomplete transitions.
**
**    The expire timer deletes an entry.
**
** For each state, the following timers are active.
**
**    complete       age, expire
**    aged/quiet     age, expire
**    aged/active    resend, age, expire
**    incomplete     resend, expire
**
** Additionally, you can determine the state of an entry from the
** following fields:
**
**                   COMPLETE    AGED     HIT
**    complete       TRUE        FALSE    xx
**    aged/quiet     TRUE        TRUE     FALSE
**    aged/active    TRUE        TRUE     TRUE
**    incomplete     FALSE       xx       xx
**
**
** Here's how it works:
**
** Each entry starts as either complete or incomplete, depending
** on who initiated communication (us or the peer).
**
** Complete entries become aged/quiet after 17 minutes, and
** expire after 20 minutes.
** Aged entries become incomplete after 3 minutes.
**
** When an entry becomes aged, we try to refresh it before it expires.
** However, we don't want to keep entries for unused hosts.
**
** Once the entry ages, if it is not used in the next 3 minutes, it
** will be deleted when the expire timer fires.
**
** If the entry is used in the next 3 minutes, it becomes aged/active.
** We start the resend timer and reset the expire timer to 3 minutes
** after the first hit to give the peer a reasonable chance to reply.
** The age timer remains unchanged -- it will still fire at the same
** time, and make the entry incomplete.
**
** Finally, incomplete entries periodically send ARP requests until
** they expire.
*/


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_open
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the ARP Server on an interface.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_open
   (
      IP_IF_PTR   if_ptr
         /* [IN] IP interface structure */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg;
   ARP_PACKET_PTR packet;

   /* Allocate an ARP cache and a statistics structure */
   arp_cfg = _mem_alloc_zero(sizeof(ARP_CFG));
   if (!arp_cfg) {
      return RTCSERR_ARP_CFG;
   } /* Endif */

   /* Get the interface's Ethernet address */
   ENET_get_address(if_ptr->HANDLE, arp_cfg->LADDR);

   /* Prebuild an ARP request */
   packet = &arp_cfg->ARP_REQUEST;
   htons(packet->LINKTYPE, ARPLINK_ETHERNET);
   htons(packet->PROTTYPE, ARPPROT_IP);
   htonc(packet->LINKLEN,  6);
   htonc(packet->PROTLEN,  4);
   htons(packet->OPCODE,   ARPOP_REQUEST);
   htone(packet->LINKSRC,  arp_cfg->LADDR);
   _mem_zero(packet->LINKDEST, 6);

   if_ptr->ARP = arp_cfg;
   return RTCS_OK;

} /* Endbody */





/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_close
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Shuts down the ARP Server on an interface.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_close
   (
      IP_IF_PTR   if_ptr
         /* [IN] IP interface structure */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg;
   uint_32        hash;
   ARP_ENTRY_PTR  entry, nextentry;

   arp_cfg = if_ptr->ARP;
   for (hash = 0; hash < ARPCACHE_SIZE; hash++) {
      entry = arp_cfg->CACHE[hash];
      while (entry) {

         /*
         ** Note: this could be replaced with
         **    TCPIP_Event_cancel(&entry->EXPIRE_TIMER);
         **    ARP_expire(&entry->EXPIRE_TIMER);
         ** but this is faster
         */

         /* Cancel all timers, free all packets */
         TCPIP_Event_cancel(&entry->RESEND_TIMER);
         TCPIP_Event_cancel(&entry->AGE_TIMER);
         TCPIP_Event_cancel(&entry->EXPIRE_TIMER);
         if (entry->WAITING) {
            RTCSLOG_PCB_FREE(entry->WAITING, RTCSERR_ARP_CANT_RESOLVE);
            RTCSPCB_free(entry->WAITING);
         } /* Endif */

         /* Enqueue the entry onto the free list */
         nextentry = entry->NEXT;
         entry->NEXT = ARP_freelist;
         ARP_freelist = entry;
         entry = nextentry;
      } /* Endwhile */
   } /* Endfor */

   _mem_free(arp_cfg);
   if_ptr->ARP = NULL;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_find
*  Returned Value : the ARP entry if found, NULL otherwise
*  Comments       :
*        Searches the ARP cache for an entry.
*
*END*-----------------------------------------------------------------*/

static ARP_ENTRY_PTR ARP_find
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    paddr
         /* [IN] protocol address */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = if_ptr->ARP;
   ARP_ENTRY_PTR  arp_ptr;
   uint_32        hash;

   /* Locate the IP address in the cache */
   hash = ARPCACHE_HASH(paddr);
   for (arp_ptr = arp_cfg->CACHE[hash];
        arp_ptr;
        arp_ptr = arp_ptr->NEXT) {
      if (arp_ptr->PADDR == paddr) {
         break;
      } /* Endif */
   } /* Endfor */

   return arp_ptr;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_find
*  Returned Value : the RDN entry if found, NULL otherwise
*  Comments       :
*        Searches the RDN cache for an entry.
*
*END*-----------------------------------------------------------------*/

static RDN_ENTRY_PTR RDN_find
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    paddr
         /* [IN] protocol address */
   )
{ /* Body */
   RDN_CFG_PTR    rdn_cfg = if_ptr->RDN;
   RDN_ENTRY_PTR  rdn_ptr;
   uint_32        hash;

   /* Locate the IP address in the cache */
   hash = RDNCACHE_HASH(paddr);
   for (rdn_ptr = rdn_cfg->CACHE[hash];
        rdn_ptr;
        rdn_ptr = rdn_ptr->NEXT) {
      if (rdn_ptr->PADDR == paddr) {
         break;
      } /* Endif */
   } /* Endfor */

   return rdn_ptr;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_is_complete
*  Returned Value : TRUE if the ARP entry was found, FALSE otherwise
*  Comments       :
*        Searches the ARP cache for an entry.
*
*END*-----------------------------------------------------------------*/

boolean  ARP_is_complete
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    addr
         /* [IN] the IP address to test */
   )
{ /* Body */

	ARP_ENTRY_PTR  arp_ptr;
  
  arp_ptr = ARP_find (if_ptr, addr);
  
  if (arp_ptr) { 	
  	return (arp_ptr->COMPLETE);
  }
  return FALSE;
 
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_kill_entry
*  Returned Value : Always returns FALSE (inherited from ARP_expire)
*  Comments       :
*        Searches the ARP cache for an entry. Removes it (and its PCB)
*        if found.
*
*END*-----------------------------------------------------------------*/

boolean  ARP_kill_entry
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    addr
         /* [IN] the IP address to test */
   )
{ /* Body */

	ARP_ENTRY_PTR  	arp_ptr;
	TCPIP_EVENT		dummy_event;
  
  arp_ptr = ARP_find (if_ptr, addr);
  
  if (arp_ptr) { 	  
     dummy_event.TIME 		= 0;
     dummy_event.EVENT 		= NULL;
     dummy_event.PRIVATE 	= arp_ptr;
     dummy_event.NEXT		= NULL;
     
     return (ARP_expire(&dummy_event));
  }  /* Endif */
  return FALSE;
 
} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_find_waiting
*  Returned Value : a pointer to the PCB waiting to be sent for the ARP 
*                 : table entry related to if_ptr and paddr.
*                 
*  Comments       :
*        Searches the ARP cache for an entry. Returns a pointer to the 
*        related waiting, outbound packet or NULL if none exixts
*
*        It is important that the calling application consider this
*        PCB read only. The arp system has the responsibility to 
*        expire and release the PCB. If you want to free the waiting 
*        PCB, you should use ARP_kill_entry()
*
END*-----------------------------------------------------------------*/

RTCSPCB_PTR  ARP_find_waiting
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    addr
         /* [IN] the IP address to test */
   )
{ /* Body */

	ARP_ENTRY_PTR  	arp_ptr;
	TCPIP_EVENT		dummy_event;
  
  arp_ptr = ARP_find (if_ptr, addr);

  if (arp_ptr) {
     return (RTCSPCB_PTR)arp_ptr->WAITING;
  }
  
  return NULL;
} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_insert
*  Returned Value : the new ARP entry
*  Comments       :
*        Create an ARP entry.  Note: it is the caller's responsibility
*        to initialize the ARP_ENTRY's fields.
*
*END*-----------------------------------------------------------------*/

static ARP_ENTRY_PTR ARP_insert
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    paddr
         /* [IN] protocol address */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = if_ptr->ARP;
   ARP_ENTRY_PTR  arp_ptr;
   uint_32        i, hash;

   /* Allocate a block of entries if none are left */
   if (!ARP_freelist) {
      arp_ptr = _mem_alloc(ARPALLOC_SIZE * sizeof(ARP_ENTRY));
      if (arp_ptr) {
         for (i = 0; i < ARPALLOC_SIZE; i++) {
            arp_ptr->NEXT = ARP_freelist;
            ARP_freelist = arp_ptr;
            arp_ptr++;
         } /* Endfor */
      } /* Endif */
   } /* Endif */

   /* Dequeue a free entry */
   arp_ptr = ARP_freelist;
   if (!arp_ptr) {
      arp_cfg->STATS.ST_ALLOCS_FAILED++;
      return NULL;
   } /* Endif */
   ARP_freelist = arp_ptr->NEXT;

   arp_ptr->PADDR                = paddr;
   arp_ptr->RESEND_TIMER.EVENT   = ARP_send;
   arp_ptr->RESEND_TIMER.PRIVATE = arp_ptr;
   arp_ptr->AGE_TIMER.EVENT      = ARP_age;
   arp_ptr->AGE_TIMER.PRIVATE    = arp_ptr;
   arp_ptr->EXPIRE_TIMER.EVENT   = ARP_expire;
   arp_ptr->EXPIRE_TIMER.PRIVATE = arp_ptr;

   /* Enqueue the new entry in the two-way list */
   hash = ARPCACHE_HASH(paddr);
   arp_ptr->HEAD = if_ptr;
   arp_ptr->PREV = NULL;
   arp_ptr->NEXT = arp_cfg->CACHE[hash];
   if (arp_ptr->NEXT) {
      arp_ptr->NEXT->PREV = arp_ptr;
   } /* Endif */
   arp_cfg->CACHE[hash] = arp_ptr;

   return arp_ptr;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_insert
*  Returned Value : the new RDN entry
*  Comments       :
*        Create an ARP entry.  Note: it is the caller's responsibility
*        to initialize the ARP_ENTRY's fields.
*
*END*-----------------------------------------------------------------*/

static RDN_ENTRY_PTR RDN_insert
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    paddr
         /* [IN] protocol address */
   )
{ /* Body */
   RDN_CFG_PTR    rdn_cfg = if_ptr->RDN;
   RDN_ENTRY_PTR  rdn_ptr;
   uint_32        i, hash;

   /* Allocate a block of entries if none are left */
   if (!RDN_freelist) {
      rdn_ptr = _mem_alloc(RDNALLOC_SIZE * sizeof(RDN_ENTRY));
      if (rdn_ptr) {
         for (i = 0; i < RDNALLOC_SIZE; i++) {
            rdn_ptr->NEXT = RDN_freelist;
            RDN_freelist = rdn_ptr;
            rdn_ptr++;
         } /* Endfor */
      } /* Endif */
   } /* Endif */

   /* Dequeue a free entry */
   rdn_ptr = RDN_freelist;
   if (!rdn_ptr) {
      rdn_cfg->STATS.ST_ALLOCS_FAILED++;
      return NULL;
   } /* Endif */
   RDN_freelist = rdn_ptr->NEXT;

   rdn_ptr->PADDR                = paddr;
   rdn_ptr->RESEND_TIMER.EVENT   = RDN_send;
   rdn_ptr->RESEND_TIMER.PRIVATE = rdn_ptr;
   rdn_ptr->AGE_TIMER.EVENT      = RDN_age;
   rdn_ptr->AGE_TIMER.PRIVATE    = rdn_ptr;
   rdn_ptr->EXPIRE_TIMER.EVENT   = RDN_expire;
   rdn_ptr->EXPIRE_TIMER.PRIVATE = rdn_ptr;

   /* Enqueue the new entry in the two-way list */
   hash = RDNCACHE_HASH(paddr);
   rdn_ptr->HEAD = if_ptr;
   rdn_ptr->PREV = NULL;
   rdn_ptr->NEXT = rdn_cfg->CACHE[hash];
   if (rdn_ptr->NEXT) {
      rdn_ptr->NEXT->PREV = rdn_ptr;
   } /* Endif */
   rdn_cfg->CACHE[hash] = rdn_ptr;

   return rdn_ptr;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_delete
*  Returned Value : void
*  Comments       :
*        Delete an ARP entry.
*
*END*-----------------------------------------------------------------*/

static void ARP_delete
   (
      ARP_ENTRY_PTR  arp_ptr
         /* [IN] ARP entry */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = arp_ptr->HEAD->ARP;
   uint_32        hash;

   /* Dequeue the entry from the two-way list */
   hash = ARPCACHE_HASH(arp_ptr->PADDR);
   if (arp_ptr->NEXT) {
      arp_ptr->NEXT->PREV = arp_ptr->PREV;
   } /* Endif */
   if (arp_ptr->PREV) {
      arp_ptr->PREV->NEXT = arp_ptr->NEXT;
   } /* Endif */
   if (arp_cfg->CACHE[hash] == arp_ptr) {
      arp_cfg->CACHE[hash] = arp_ptr->NEXT;
   } /* Endif */

   /* Enqueue the entry onto the free list */
   arp_ptr->NEXT = ARP_freelist;
   ARP_freelist = arp_ptr;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_delete
*  Returned Value : void
*  Comments       :
*        Delete an RDN entry.
*
*END*-----------------------------------------------------------------*/

static void RDN_delete
   (
      RDN_ENTRY_PTR  rdn_ptr
         /* [IN] RDN entry */
   )
{ /* Body */
   RDN_CFG_PTR    rdn_cfg = rdn_ptr->HEAD->RDN;
   uint_32        hash;

   /* Dequeue the entry from the two-way list */
   hash = RDNCACHE_HASH(rdn_ptr->PADDR);
   if (rdn_ptr->NEXT) {
      rdn_ptr->NEXT->PREV = rdn_ptr->PREV;
   } /* Endif */
   if (rdn_ptr->PREV) {
      rdn_ptr->PREV->NEXT = rdn_ptr->NEXT;
   } /* Endif */
   if (rdn_cfg->CACHE[hash] == rdn_ptr) {
      rdn_cfg->CACHE[hash] = rdn_ptr->NEXT;
   } /* Endif */

   /* Enqueue the entry onto the free list */
   rdn_ptr->NEXT = RDN_freelist;
   RDN_freelist = rdn_ptr;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_send
*  Returned Value :
*  Comments       :
*        Called by the Timer.  Send an ARP request
*
*END*-----------------------------------------------------------------*/

static boolean ARP_send
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the refresh event */
   )
{ /* Body */
   ARP_ENTRY_PTR  arp_ptr = event->PRIVATE;
   ARP_CFG_PTR    arp_cfg = arp_ptr->HEAD->ARP;
   RTCSPCB_PTR    pcb_ptr;
   ARP_PACKET_PTR packet;
   uint_32        error;

   arp_cfg->STATS.ST_TX_TOTAL++;
   arp_cfg->STATS.ST_TX_REQUESTS++;

   /*
   ** Obviously, the gotos below aren't necessary; they're there to
   ** prevent the indentation level from getting ridiculously deep.
   */

   pcb_ptr = RTCSPCB_alloc_send();
   if (!pcb_ptr) {
      arp_cfg->STATS.ST_TX_MISSED++;
      goto arpsend_fail;
   } /* Endif */

   RTCSLOG_PCB_ALLOC(pcb_ptr);

   error = RTCSPCB_insert(pcb_ptr, sizeof(ARP_PACKET));
   if (error) {
      arp_cfg->STATS.ST_TX_ERRORS++;
      RTCS_seterror(&arp_cfg->STATS.ERR_TX, error, (uint_32)pcb_ptr);
      RTCSLOG_PCB_FREE(pcb_ptr, error);
      RTCSPCB_free(pcb_ptr);
      goto arpsend_fail;
   } /* Endif */

   RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_ARP(IPIFTYPE_ETHERNET), 0);

   packet = (ARP_PACKET_PTR)RTCSPCB_DATA(pcb_ptr);
   _mem_copy(&arp_cfg->ARP_REQUEST, packet, sizeof(*packet));
   htonl(packet->PROTSRC,  arp_ptr->WAITADDR);
   htonl(packet->PROTDEST, arp_ptr->PADDR);

   pcb_ptr->LINK_OPTIONS.TX = arp_ptr->WAITOPT;
   error = IPE_send_ARP_bcast(arp_ptr->HEAD, pcb_ptr);
   if (error) {
      arp_cfg->STATS.ST_TX_ERRORS++;
      RTCS_seterror(&arp_cfg->STATS.ERR_TX, error, (uint_32)pcb_ptr);
      goto arpsend_fail;
   } /* Endif */

arpsend_fail:
   arp_ptr->RESEND_INTERVAL *= 2;
   if (arp_ptr->RESEND_INTERVAL > ARPTIME_RESEND_MAX) {
      arp_ptr->RESEND_INTERVAL = ARPTIME_RESEND_MAX;
   } /* Endif */
   event->TIME = arp_ptr->RESEND_INTERVAL;
   return TRUE;
} /* Endbody */

//_______________________________________________


boolean  RDN_send//( void)  //   RTCSPCB_PTR    pcb_ptr   
(
     TCPIP_EVENT_PTR   event
    //   [IN/OUT] the refresh event
 )
{

   RDN_ENTRY_PTR  rdn_ptr = event->PRIVATE;
   RDN_CFG_PTR    rdn_cfg = rdn_ptr->HEAD->RDN;
   RTCSPCB_PTR    pcb_ptr;
   RDN_PACKET_PTR packet;
   uint_32        error;

   rdn_cfg->STATS.ST_TX_TOTAL++;
   rdn_cfg->STATS.ST_TX_REQUESTS++;

   /*
   ** Obviously, the gotos below aren't necessary; they're there to
   ** prevent the indentation level from getting ridiculously deep.
   */

   pcb_ptr = RTCSPCB_alloc_send();
   if (!pcb_ptr) {
      rdn_cfg->STATS.ST_TX_MISSED++;
      goto rdnsend_fail;
   } /* Endif */

   RTCSLOG_PCB_ALLOC(pcb_ptr);

   error = RTCSPCB_insert(pcb_ptr, sizeof(RDN_PACKET));
   if (error) {
      rdn_cfg->STATS.ST_TX_ERRORS++;
      RTCS_seterror(&rdn_cfg->STATS.ERR_TX, error, (uint_32)pcb_ptr);
      RTCSLOG_PCB_FREE(pcb_ptr, error);
      RTCSPCB_free(pcb_ptr);
      goto rdnsend_fail;
   } /* Endif */

   RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_RDN(IPIFTYPE_ETHERNET), 0);

   packet = (RDN_PACKET_PTR)RTCSPCB_DATA(pcb_ptr);
   _mem_copy(&rdn_cfg->RDN_REQUEST, packet, sizeof(*packet));
   htonl(packet->PROTSRC,  rdn_ptr->WAITADDR);
   htonl(packet->PROTDEST, rdn_ptr->PADDR);

   pcb_ptr->LINK_OPTIONS.TX = rdn_ptr->WAITOPT;
   error = IPE_send_RDN_bcast(rdn_ptr->HEAD, pcb_ptr);
   if (error) {
      rdn_cfg->STATS.ST_TX_ERRORS++;
      RTCS_seterror(&rdn_cfg->STATS.ERR_TX, error, (uint_32)pcb_ptr);
      goto rdnsend_fail;
   } /* Endif */

   rdnsend_fail:
   rdn_ptr->RESEND_INTERVAL *= 2;
   if (rdn_ptr->RESEND_INTERVAL > RDNTIME_RESEND_MAX) {
      rdn_ptr->RESEND_INTERVAL = RDNTIME_RESEND_MAX;
   } /* Endif */
   event->TIME = rdn_ptr->RESEND_INTERVAL;
   return TRUE;

//_____________________________________________________________________________
}

//________________________________________________
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_age
*  Returned Value :
*  Comments       :
*        Called by the Timer.  Age an ARP cache entry.
*
*END*-----------------------------------------------------------------*/

static boolean ARP_age
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the aging event */
   )
{ /* Body */
   ARP_ENTRY_PTR  arp_ptr = event->PRIVATE;
   /* Complete entries become aged/quiet */
   if (!arp_ptr->AGED) {
      arp_ptr->AGED = TRUE;
      arp_ptr->HIT  = FALSE;
      event->TIME   = ARPTIME_EXPIRE_INCOMPLETE;
      return TRUE;

   /* Aged entries become incomplete */
   } else {
      arp_ptr->COMPLETE = FALSE;
      return FALSE;

   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_age
*  Returned Value :
*  Comments       :
*        Called by the Timer.  Age an RDN cache entry.
*
*END*-----------------------------------------------------------------*/

static boolean RDN_age
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the aging event */
   )
{ /* Body */
   RDN_ENTRY_PTR  rdn_ptr = event->PRIVATE;

   /* Complete entries become aged/quiet */
   if (!rdn_ptr->AGED) {
      rdn_ptr->AGED = TRUE;
      rdn_ptr->HIT  = FALSE;
      event->TIME   = ARPTIME_EXPIRE_INCOMPLETE;
      return TRUE;

   /* Aged entries become incomplete */
   } else {
      rdn_ptr->COMPLETE = FALSE;
      return FALSE;

   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_expire
*  Returned Value :
*  Comments       :
*        Called by the Timer.  Expire an ARP cache entry.
*
*END*-----------------------------------------------------------------*/

static boolean ARP_expire
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the expiration event */
   )
{ /* Body */
   ARP_ENTRY_PTR  arp_ptr = event->PRIVATE;
   ARP_CFG_PTR    arp_cfg = arp_ptr->HEAD->ARP;

   /* When an entry expires, free it and any packets queued on it */
   if (arp_ptr->WAITING) {
      arp_cfg->STATS.ST_PKT_DISCARDS++;
      arp_ptr->HEAD->STATS.ST_TX_DISCARDED++;
      RTCSLOG_PCB_FREE(arp_ptr->WAITING, RTCSERR_ARP_CANT_RESOLVE);
      RTCSPCB_free(arp_ptr->WAITING);
   } /* Endif */

   TCPIP_Event_cancel(&arp_ptr->RESEND_TIMER);
   TCPIP_Event_cancel(&arp_ptr->AGE_TIMER);
   
   /* NOTE: ARP_expire may be called either from the ARP_expire timer event
    or possibly from ARP_kill_entry(). If we are called from ARP_kill_entry()
    then we MUST cancel the ARP_expire timer event. If we are called from 
    the ARP_expire timer event, then canceling the ARP_expire timer event 
    here is useless but harmless.
    So leave the cancle(expire timer) here even though it may look odd.
    */
   TCPIP_Event_cancel(&arp_ptr->EXPIRE_TIMER);
   ARP_delete(arp_ptr);

   return FALSE;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_expire
*  Returned Value :
*  Comments       :
*        Called by the Timer.  Expire an RDN cache entry.
*
*END*-----------------------------------------------------------------*/

static boolean RDN_expire
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the expiration event */
   )
{ /* Body */
   RDN_ENTRY_PTR  rdn_ptr = event->PRIVATE;
   RDN_CFG_PTR    rdn_cfg = rdn_ptr->HEAD->RDN;

   /* When an entry expires, free it and any packets queued on it */
   if (rdn_ptr->WAITING) {
      rdn_cfg->STATS.ST_PKT_DISCARDS++;
      rdn_ptr->HEAD->STATS.ST_TX_DISCARDED++;
      RTCSLOG_PCB_FREE(rdn_ptr->WAITING, RTCSERR_RDN_CANT_RESOLVE);
      RTCSPCB_free(rdn_ptr->WAITING);
   } /* Endif */

   TCPIP_Event_cancel(&rdn_ptr->RESEND_TIMER);
   TCPIP_Event_cancel(&rdn_ptr->AGE_TIMER);
   
   /* NOTE: RDN_expire may be called either from the ARP_expire timer event
    or possibly from RDN_kill_entry(). If we are called from RDN_kill_entry()
    then we MUST cancel the ARP_expire timer event. If we are called from 
    the ARP_expire timer event, then canceling the ARP_expire timer event 
    here is useless but harmless.
    So leave the cancle(expire timer) here even though it may look odd.
    */
   TCPIP_Event_cancel(&rdn_ptr->EXPIRE_TIMER);
   RDN_delete(rdn_ptr);

   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_resolve
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Called by IP-E.  Translates an IP address to a physical address.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_resolve
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      RTCSPCB_PTR    pcb_ptr,
         /* [IN] packet to send */
      _ip_address    isrc,
         /* [IN] source address */
      _ip_address    idest
         /* [IN] destination address */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = if_ptr->ARP;
   ARP_ENTRY_PTR  arp_ptr;

   /* First check the ARP cache */
   arp_ptr = ARP_find(if_ptr, idest);

   /* If there's no entry, create an incomplete one */
   if (!arp_ptr) {

      arp_ptr = ARP_insert(if_ptr, idest);
      if (!arp_ptr) {
         if_ptr->STATS.ST_TX_MISSED++;
         arp_cfg->STATS.ST_PKT_DISCARDS++;
         RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_ARP_CANT_RESOLVE);
         RTCSPCB_free(pcb_ptr);

      } else {
         arp_ptr->WAITING  = pcb_ptr;
         arp_ptr->WAITADDR = isrc;
         arp_ptr->WAITOPT  = pcb_ptr->LINK_OPTIONS.TX;
         arp_ptr->COMPLETE = FALSE;
         ARP_REFRESH(arp_ptr);
      } /* Endif */

      arp_cfg->STATS.ST_CACHE_MISSES++;
      return RTCS_OK;

   /* If there's an incomplete one, queue the packet */
   } else if (!arp_ptr->COMPLETE) {

      if (arp_ptr->WAITING) {
         arp_cfg->STATS.ST_PKT_DISCARDS++;
         if_ptr->STATS.ST_TX_DISCARDED++;
         RTCSLOG_PCB_FREE(arp_ptr->WAITING, RTCSERR_ARP_CANT_RESOLVE);
         RTCSPCB_free(arp_ptr->WAITING);
      } /* Endif */
      arp_ptr->WAITING  = pcb_ptr;
      arp_ptr->WAITADDR = isrc;
      arp_ptr->WAITOPT  = pcb_ptr->LINK_OPTIONS.TX;
      arp_cfg->STATS.ST_CACHE_MISSES++;
      return RTCS_OK;

   /* If there's a complete one, send the packet */
   } else {

      /* If the entry is about to expire, try to refresh it */
      if (arp_ptr->AGED && !arp_ptr->HIT) {
         arp_ptr->HIT = TRUE;
         TCPIP_Event_cancel(&arp_ptr->EXPIRE_TIMER);
         arp_ptr->WAITADDR = isrc;
         arp_ptr->WAITOPT  = pcb_ptr->LINK_OPTIONS.TX;
         ARP_REFRESH(arp_ptr);
      } /* Endif */

      arp_cfg->STATS.ST_CACHE_HITS++;
      return IPE_send_IP_ucast(if_ptr, pcb_ptr, arp_ptr->LADDR);

   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_request
*  Returned Value : RTCS_OK or error code
*  Comments       :
*                 : send an arp request for a given IP.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_request
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    isrc,
         /* [IN] source address */
      _ip_address    idest
         /* [IN] destination address */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = if_ptr->ARP;
   ARP_ENTRY_PTR  arp_ptr;
 
   /* First check the ARP cache */
   arp_ptr = ARP_find(if_ptr, idest);

   /* If there's no entry, create an incomplete one */
   if (!arp_ptr) {
   
      arp_cfg->STATS.ST_CACHE_MISSES++;
      arp_ptr = ARP_insert(if_ptr, idest);
      if (!arp_ptr) {
         if_ptr->STATS.ST_TX_MISSED++;
      } else {
         arp_ptr->WAITING  = NULL;  
         arp_ptr->WAITADDR = isrc;     
         arp_ptr->COMPLETE = FALSE;         
         
         /* send request now (don't queue it for later) */                                  
         ARP_send(&(arp_ptr->RESEND_TIMER));
      } /* Endif */   
   } /* Endif */
   
   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_service
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Processes received ARP packets.
*
*END*-----------------------------------------------------------------*/

void ARP_service
   (
      RTCSPCB_PTR    pcb_ptr
         /* [IN] ARP packet */
   )
{ /* Body */
   IP_IF_PTR      if_ptr;
   ARP_CFG_PTR    arp_cfg;
   ARP_ENTRY_PTR  arp_ptr;
   ARP_PACKET_PTR packet;
   uint_16        opcode;
   uchar          rladdr[6];
   _ip_address    lpaddr, rpaddr;
   boolean        merge;
   uint_32        error;

   if_ptr = pcb_ptr->IFSRC;
   arp_cfg = if_ptr->ARP;
   arp_cfg->STATS.ST_RX_TOTAL++;
   packet = (ARP_PACKET_PTR)RTCSPCB_DATA(pcb_ptr);
   opcode = ntohs(packet->OPCODE);

   RTCSLOG_PCB_READ(pcb_ptr, RTCS_LOGCTRL_ARP(IPIFTYPE_ETHERNET), 0);

   /* Validate the packet */
   if ((ntohs(packet->LINKTYPE) != ARPLINK_ETHERNET)
    || (ntohs(packet->PROTTYPE) != ARPPROT_IP)
    || (ntohc(packet->LINKLEN)  != 6)
    || (ntohc(packet->PROTLEN)  != 4)
    || ((opcode != ARPOP_REQUEST)
     && (opcode != ARPOP_REPLY))) {
      arp_cfg->STATS.ST_RX_DISCARDED++;
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_ARP_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */

   ntohe(packet->LINKSRC, rladdr);
   rpaddr = ntohl(packet->PROTSRC);
   lpaddr = ntohl(packet->PROTDEST);

   /* If we have an entry, update it */
   arp_ptr = ARP_find(if_ptr, rpaddr);
   if (!arp_ptr) {
      merge = FALSE;
   } else {
      TCPIP_Event_cancel(&arp_ptr->RESEND_TIMER);
      TCPIP_Event_cancel(&arp_ptr->AGE_TIMER);
      TCPIP_Event_cancel(&arp_ptr->EXPIRE_TIMER);
      ARP_COMPLETE(arp_ptr, rladdr);

      if (arp_ptr->WAITING) {
         error = IPE_send_IP_ucast(if_ptr, arp_ptr->WAITING, rladdr);
         arp_ptr->WAITING = NULL;
      } /* Endif */

      merge = TRUE;
   } /* Endif */

   if (IP_is_local(if_ptr, lpaddr) || ARP_do_proxy(if_ptr, lpaddr)) {

      /* If we're the target and we don't have an entry, create one */
      if (!merge) {
         arp_ptr = ARP_insert(if_ptr, rpaddr);
         if (arp_ptr) {
            ARP_COMPLETE(arp_ptr, rladdr);
            arp_ptr->WAITING = NULL;
         } else {
            arp_cfg->STATS.ST_RX_MISSED++;
         } /* Endif */
      } /* Endif */

      /* If it's a request for us, reply */
      if (opcode == ARPOP_REQUEST) {
         arp_cfg->STATS.ST_RX_REQUESTS++;
         arp_cfg->STATS.ST_TX_TOTAL++;
         arp_cfg->STATS.ST_TX_REPLIES++;

         RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_ARP(IPIFTYPE_ETHERNET), 0);

         htons(packet->OPCODE, ARPOP_REPLY);
         htone(packet->LINKSRC,  arp_cfg->LADDR);
         htonl(packet->PROTSRC,  lpaddr);
         htone(packet->LINKDEST, rladdr);
         htonl(packet->PROTDEST, rpaddr);

         error = IPE_send_ARP_ucast(if_ptr, pcb_ptr, rladdr);
         if (error) {
            arp_cfg->STATS.ST_TX_ERRORS++;
            RTCS_seterror(&arp_cfg->STATS.ERR_TX, error, (uint_32)pcb_ptr);
            return;
         } /* Endif */

      } else {
         arp_cfg->STATS.ST_RX_REPLIES++;
         RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
         RTCSPCB_free(pcb_ptr);
      } /* Endif */

   } else {
      arp_cfg->STATS.ST_RX_DISCARDED++;
      RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
      RTCSPCB_free(pcb_ptr);
   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : TRAP_transmit
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Processes transmit TRAP packets.
*
*END*-----------------------------------------------------------------*/

boolean TRAP_transmit
   (
        IP_IF_PTR    handle,
        char * pBuff,			//now max size 1028 Bytes
        char *pt,
        long len	        	
   )
{ 
   RTCSPCB_PTR    my_pcb_ptr;
//   RDN_CFG_PTR    rdn_cfg;
   RDN_ENTRY_PTR  rdn_ptr;
   RDN_PACKET_PTR packet;
    uchar          enetdest[6];
   uint_32        error;
 //  uint_32        len;
   char * pBuff1;
   pBuff1 = pBuff;
	/*
   len = (*pBuff1++) & 0xff;
   len <<= 8;
   len += *pBuff1 & 0xff;
  // len = 100; //for test
   if(len > sizeof(RDN_PACKET))
  {
     len = sizeof(RDN_PACKET);
  }	
    */
   rdn_ptr =  RDN_insert( handle, 0x9b9b9b9d); //ip not need but set for compatibility
 
 //  rdn_cfg =  handle->RDN;

  // rdn_cfg->STATS.ST_RX_TOTAL++;

   my_pcb_ptr = RTCSPCB_alloc_send();

   if (!my_pcb_ptr) {
 //     rdn_cfg->STATS.ST_TX_MISSED++;
//	rdn_ptr->RESEND_INTERVAL *= 2;
//   if (rdn_ptr->RESEND_INTERVAL > RDNTIME_RESEND_MAX) {
//      rdn_ptr->RESEND_INTERVAL = RDNTIME_RESEND_MAX;
 //  } 
    RDN_delete(rdn_ptr);
    return FALSE;
      } /* Endif */

//    RTCSLOG_PCB_ALLOC(my_pcb_ptr);
//t11  error = RTCSPCB_insert(my_pcb_ptr, sizeof(RDN_PACKET));
  error = RTCSPCB_insert(my_pcb_ptr, len); //t

   if (error) {
  //    rdn_cfg->STATS.ST_TX_ERRORS++;
  //    RTCS_seterror(&rdn_cfg->STATS.ERR_TX, error, (uint_32)my_pcb_ptr);
//      RTCSLOG_PCB_FREE(my_pcb_ptr, error);
      RTCSPCB_free(my_pcb_ptr);
  //   rdn_ptr->RESEND_INTERVAL *= 2;
  //   if (rdn_ptr->RESEND_INTERVAL > RDNTIME_RESEND_MAX)
  // {
  //    rdn_ptr->RESEND_INTERVAL = RDNTIME_RESEND_MAX;
  // } 
    RDN_delete(rdn_ptr);
      return FALSE;
   } /* Endif */

  // RTCSLOG_PCB_WRITE(my_pcb_ptr, RTCS_LOGCTRL_RDN(IPIFTYPE_ETHERNET), 0);

   packet = (RDN_PACKET_PTR)RTCSPCB_DATA(my_pcb_ptr);

//t09  _mem_copy(&Buff[0], packet, sizeof(*packet));
//t11  _mem_copy(pBuff, packet, sizeof(*packet));
  _mem_copy(pBuff, packet,len); //t
 //t  htonl(packet->PROTSRC,  rdn_ptr->WAITADDR);

 //t  htonl(packet->PROTDEST, rdn_ptr->PADDR);

   my_pcb_ptr->LINK_OPTIONS.TX = rdn_ptr->WAITOPT; //??

    enetdest[0] = (uchar)*pt++;
    enetdest[1] = (uchar)*pt++;
    enetdest[2] = (uchar)*pt++;
    enetdest[3] = (uchar)*pt++;;
    enetdest[4] = (uchar)*pt++;
    enetdest[5] = (uchar)*pt;

//   handle->STATS.ST_TX_TOTAL++;    ARPPROT_IP
//    error =IPE_send(handle, my_pcb_ptr, ENETPROT_RDN, enetdest);
      error =IPE_send(handle, my_pcb_ptr, ARPPROT_IP, enetdest);


   if (error) {
    //  rdn_cfg->STATS.ST_TX_ERRORS++;
  //    RTCS_seterror(&rdn_cfg->STATS.ERR_TX, error, (uint_32)my_pcb_ptr);
//	rdn_ptr->RESEND_INTERVAL *= 2;
//   if (rdn_ptr->RESEND_INTERVAL > RDNTIME_RESEND_MAX) {
//      rdn_ptr->RESEND_INTERVAL = RDNTIME_RESEND_MAX;
//   } 
 //   RTCSLOG_PCB_FREE(pcb_ptr, error);
 //   RTCSPCB_free(my_pcb_ptr);
   RDN_delete(rdn_ptr);
     return FALSE;
   } 
// RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
//RTCSPCB_free(my_pcb_ptr); //t hangt!
 RDN_delete(rdn_ptr);
 return TRUE;
} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_transmit
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Processes transmit RDN packets.
*
*END*-----------------------------------------------------------------*/

boolean RDN_transmit
   (
        IP_IF_PTR    handle,
        char * pBuff,			//now max size 1028 Bytes
        char *pt	        	
   )
{ 
   RTCSPCB_PTR    my_pcb_ptr;
//   RDN_CFG_PTR    rdn_cfg;
   RDN_ENTRY_PTR  rdn_ptr;
   RDN_PACKET_PTR packet;
    uchar          enetdest[6];
   uint_32        error;
   uint_32        len;
   char * pBuff1;
   pBuff1 = pBuff;
	
   len = (*pBuff1++) & 0xff;
   len <<= 8;
   len += *pBuff1 & 0xff;

//	printfpd("\n\r________________len : %d \n\r)", len);


  // len = 100; //for test
   if(len > sizeof(RDN_PACKET))
  {
     len = sizeof(RDN_PACKET);
  }	

   rdn_ptr =  RDN_insert( handle, 0x9b9b9b9d); //ip not need but set for compatibility
 
 //  rdn_cfg =  handle->RDN;

  // rdn_cfg->STATS.ST_RX_TOTAL++;

   my_pcb_ptr = RTCSPCB_alloc_send();

   if (!my_pcb_ptr) {
 //     rdn_cfg->STATS.ST_TX_MISSED++;
//	rdn_ptr->RESEND_INTERVAL *= 2;
//   if (rdn_ptr->RESEND_INTERVAL > RDNTIME_RESEND_MAX) {
//      rdn_ptr->RESEND_INTERVAL = RDNTIME_RESEND_MAX;
 //  } 
    RDN_delete(rdn_ptr);
    return FALSE;
      } /* Endif */

//    RTCSLOG_PCB_ALLOC(my_pcb_ptr);
//t11  error = RTCSPCB_insert(my_pcb_ptr, sizeof(RDN_PACKET));
  error = RTCSPCB_insert(my_pcb_ptr, len); //t

   if (error) {
  //    rdn_cfg->STATS.ST_TX_ERRORS++;
  //    RTCS_seterror(&rdn_cfg->STATS.ERR_TX, error, (uint_32)my_pcb_ptr);
//      RTCSLOG_PCB_FREE(my_pcb_ptr, error);
      RTCSPCB_free(my_pcb_ptr);
  //   rdn_ptr->RESEND_INTERVAL *= 2;
  //   if (rdn_ptr->RESEND_INTERVAL > RDNTIME_RESEND_MAX)
  // {
  //    rdn_ptr->RESEND_INTERVAL = RDNTIME_RESEND_MAX;
  // } 
    RDN_delete(rdn_ptr);
      return FALSE;
   } /* Endif */

  // RTCSLOG_PCB_WRITE(my_pcb_ptr, RTCS_LOGCTRL_RDN(IPIFTYPE_ETHERNET), 0);

   packet = (RDN_PACKET_PTR)RTCSPCB_DATA(my_pcb_ptr);

//t09  _mem_copy(&Buff[0], packet, sizeof(*packet));
//t11  _mem_copy(pBuff, packet, sizeof(*packet));
  _mem_copy(pBuff, packet,len); //t
 //t  htonl(packet->PROTSRC,  rdn_ptr->WAITADDR);

 //t  htonl(packet->PROTDEST, rdn_ptr->PADDR);

   my_pcb_ptr->LINK_OPTIONS.TX = rdn_ptr->WAITOPT; //??

    enetdest[0] = (uchar)*pt++;
    enetdest[1] = (uchar)*pt++;
    enetdest[2] = (uchar)*pt++;
    enetdest[3] = (uchar)*pt++;;
    enetdest[4] = (uchar)*pt++;
    enetdest[5] = (uchar)*pt;

//   handle->STATS.ST_TX_TOTAL++;
    error =IPE_send(handle, my_pcb_ptr, ENETPROT_RDN, enetdest);

   if (error) {
    //  rdn_cfg->STATS.ST_TX_ERRORS++;
  //    RTCS_seterror(&rdn_cfg->STATS.ERR_TX, error, (uint_32)my_pcb_ptr);
//	rdn_ptr->RESEND_INTERVAL *= 2;
//   if (rdn_ptr->RESEND_INTERVAL > RDNTIME_RESEND_MAX) {
//      rdn_ptr->RESEND_INTERVAL = RDNTIME_RESEND_MAX;
//   } 
 //   RTCSLOG_PCB_FREE(pcb_ptr, error);
 //   RTCSPCB_free(my_pcb_ptr);
   RDN_delete(rdn_ptr);
     return FALSE;
   } 
// RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
//RTCSPCB_free(my_pcb_ptr); //t hangt!
 RDN_delete(rdn_ptr);
 return TRUE;
} 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_service
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Processes received RDN packets.
*
*END*-----------------------------------------------------------------*/

void RDN_service
   (
      RTCSPCB_PTR    pcb_ptr
         /* [IN] RDN packet */
   )
{ /* Body */
   IP_IF_PTR      if_ptr;
   RDN_CFG_PTR    rdn_cfg;
   RDN_ENTRY_PTR  rdn_ptr;
   RDN_PACKET_PTR packet;
  //PCB_PTR curpcb;
  //PCB_FRAGMENT_PTR fragment;
//  uint_16        opcode;
   //uchar          rladdr[6];
  // _ip_address    lpaddr, rpaddr;
   boolean        merge;
   uint_32        error;
//   char * fullpack;

 // return; //t

  //printf("\n\rRDN_service ");
  //thanks  Stephany16 ++;	//for test
   packet = (RDN_PACKET_PTR)RTCSPCB_DATA(pcb_ptr);

//______________________________________________________________091211

  /* Validate the packet */
/*
   if ((ntohs(packet->LINKTYPE) != ARPLINK_ETHERNET)
    || (ntohs(packet->PROTTYPE) != ARPPROT_IP)
    || (ntohc(packet->LINKLEN)  != 6)
    || (ntohc(packet->PROTLEN)  != 4)
    || ((opcode != ARPOP_REQUEST)
     && (opcode != ARPOP_REPLY))) {
      arp_cfg->STATS.ST_RX_DISCARDED++;
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_ARP_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } 

*/
/*
 opcode = ntohs(packet->OPCODE);

  if ((ntohs(packet->LINKTYPE) != RDNLINK_ETHERNET)
    || (ntohs(packet->PROTTYPE) != RDNPROT_IP)
    || (ntohc(packet->LINKLEN)  != 6)
    || (ntohc(packet->PROTLEN)  != 4)
    || ((opcode != RDNOP_REQUEST)
     && (opcode != RDNOP_REPLY))) {
      rdn_cfg->STATS.ST_RX_DISCARDED++;
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_RDN_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } 
*/

//______________________________________________________________091211






   RDN_receive((char*)packet); //comment not get some!
//_____________________________________________________________________________________
//   curpcb  =  (PCB_PTR)&(pcb_ptr->PCB_PTR);
  // fragment = (PCB_FRAGMENT_PTR)&(curpcb->FRAG[0]); //new first 4 bytes full length in pcp after FRAG[0]
 //fragment = (PCB_FRAGMENT_PTR)&(pcb_ptr->FRAG[0]); //new first 4 bytes full length in pcp after FRAG[0]
  // fullpack = (char*)fragment->FRAGMENT;
//fullpack = (char*)&pcb_ptr->PCB_ORIG->FRAG[0].FRAGMENT;
//fullpack = (char*)RTCSPCB_DATA_NETWORK(pcb_ptr);;
//fullpack = (char*)(packet-12);
//  RDN_receive( fullpack);
 //ntohe(packet->LINKSRC, rladdr);

//RDN_receive((char*)packet,(char*) rladdr);
//______________________________________________________________________________________


/*
   if_ptr = pcb_ptr->IFSRC;
   rdn_cfg = if_ptr->RDN;
   rdn_cfg->STATS.ST_RX_TOTAL++;
   packet = (RDN_PACKET_PTR)RTCSPCB_DATA(pcb_ptr);

   opcode = ntohs(packet->OPCODE);

   RTCSLOG_PCB_READ(pcb_ptr, RTCS_LOGCTRL_RDN(IPIFTYPE_ETHERNET), 0);

  
   if ((ntohs(packet->LINKTYPE) != RDNLINK_ETHERNET)
    || (ntohs(packet->PROTTYPE) != RDNPROT_IP)
    || (ntohc(packet->LINKLEN)  != 6)
    || (ntohc(packet->PROTLEN)  != 4)
    || ((opcode != RDNOP_REQUEST)
     && (opcode != RDNOP_REPLY))) {
      rdn_cfg->STATS.ST_RX_DISCARDED++;
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_RDN_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } 

   ntohe(packet->LINKSRC, rladdr);
   rpaddr = ntohl(packet->PROTSRC);
   lpaddr = ntohl(packet->PROTDEST);

   
   rdn_ptr = RDN_find(if_ptr, rpaddr);
   if (!rdn_ptr) {
      merge = FALSE;
   } else {
      TCPIP_Event_cancel(&rdn_ptr->RESEND_TIMER);
      TCPIP_Event_cancel(&rdn_ptr->AGE_TIMER);
      TCPIP_Event_cancel(&rdn_ptr->EXPIRE_TIMER);
      RDN_COMPLETE(rdn_ptr, rladdr);

      if (rdn_ptr->WAITING) {
         error = IPE_send_IP_ucast(if_ptr, rdn_ptr->WAITING, rladdr);
         rdn_ptr->WAITING = NULL;
      } 

      merge = TRUE;
   } 

   if (IP_is_local(if_ptr, lpaddr) || RDN_do_proxy(if_ptr, lpaddr)) {

 
      if (!merge) {
         rdn_ptr = RDN_insert(if_ptr, rpaddr);
         if (rdn_ptr) {
           RDN_COMPLETE(rdn_ptr, rladdr);
            rdn_ptr->WAITING = NULL;
         } else {
            rdn_cfg->STATS.ST_RX_MISSED++;
         }
      } 

  
      if (opcode == RDNOP_REQUEST) {
         rdn_cfg->STATS.ST_RX_REQUESTS++;
         rdn_cfg->STATS.ST_TX_TOTAL++;
         rdn_cfg->STATS.ST_TX_REPLIES++;

         RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_RDN(IPIFTYPE_ETHERNET), 0);

         htons(packet->OPCODE, RDNOP_REPLY);
         htone(packet->LINKSRC,  rdn_cfg->LADDR);
         htonl(packet->PROTSRC,  lpaddr);
         htone(packet->LINKDEST, rladdr);
         htonl(packet->PROTDEST, rpaddr);

         error = IPE_send_RDN_ucast(if_ptr, pcb_ptr, rladdr);
         if (error) {
            rdn_cfg->STATS.ST_TX_ERRORS++;
            RTCS_seterror(&rdn_cfg->STATS.ERR_TX, error, (uint_32)pcb_ptr);
            return;
         } 

      } else {
         rdn_cfg->STATS.ST_RX_REPLIES++;
         RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
         RTCSPCB_free(pcb_ptr);
      }

   } else {
      rdn_cfg->STATS.ST_RX_DISCARDED++;
      RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
      RTCSPCB_free(pcb_ptr);
   }
*/
 //  RDN_delete(rdn_ptr);  //my
   RTCSPCB_free(pcb_ptr);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_request
*  Returned Value : RTCS_OK or error code
*  Comments       :
*                 : send an rdn request for a given IP.
*
*END*-----------------------------------------------------------------*/

uint_32 RDN_request
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    isrc,
         /* [IN] source address */
      _ip_address    idest
         /* [IN] destination address */
   )
{ /* Body */
   RDN_CFG_PTR    rdn_cfg = if_ptr->RDN;
   RDN_ENTRY_PTR  rdn_ptr;
 
   /* First check the RDN cache */
   rdn_ptr = RDN_find(if_ptr, idest);

   /* If there's no entry, create an incomplete one */
   if (!rdn_ptr) {
   
      rdn_cfg->STATS.ST_CACHE_MISSES++;
      rdn_ptr = RDN_insert(if_ptr, idest);
      if (!rdn_ptr) {
         if_ptr->STATS.ST_TX_MISSED++;
      } else {
         rdn_ptr->WAITING  = NULL;  
         rdn_ptr->WAITADDR = isrc;     
         rdn_ptr->COMPLETE = FALSE;         
         
         /* send request now (don't queue it for later) */                                  
         RDN_send(&(rdn_ptr->RESEND_TIMER));
      } /* Endif */   
   } /* Endif */
   
   return RTCS_OK;
} /* Endbody */ 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_open
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the RDN Server on an interface.
*
*END*-----------------------------------------------------------------*/

uint_32 RDN_open
   (
      IP_IF_PTR   if_ptr
         /* [IN] IP interface structure */
   )
{ /* Body */
   RDN_CFG_PTR    rdn_cfg;
   RDN_PACKET_PTR packet;

   /* Allocate an RDN cache and a statistics structure */
   rdn_cfg = _mem_alloc_zero(sizeof(RDN_CFG));
   if (!rdn_cfg) {
      return RTCSERR_RDN_CFG;
   } /* Endif */

   /* Get the interface's Ethernet address */
   ENET_get_address(if_ptr->HANDLE, rdn_cfg->LADDR);

   /* Prebuild an RDN request */
   packet = &rdn_cfg->RDN_REQUEST;
   htons(packet->LINKTYPE, RDNLINK_ETHERNET);
   htons(packet->PROTTYPE, RDNPROT_IP);
   htonc(packet->LINKLEN,  6);
   htonc(packet->PROTLEN,  4);
   htons(packet->OPCODE,   RDNOP_REQUEST);
   htone(packet->LINKSRC,  rdn_cfg->LADDR);
   _mem_zero(packet->LINKDEST, 6);

   if_ptr->RDN = rdn_cfg;
   return RTCS_OK;

} /* Endbody */



/* EOF */
