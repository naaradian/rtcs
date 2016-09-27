#ifndef __arp_h__
#define __arp_h__
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
*** File: arp.h
***
*** Comments:
***      Address Resolution Protocol definitions.
***
************************************************************************
*END*******************************************************************/

#include "ip_prv.h"


/***************************************
**
** Prototypes
**
*/
//extern boolean  RDN_send   
//(
  //   TCPIP_EVENT_PTR   
 // )



extern uint_32 ARP_init
(
   void
); 

extern  void RDN_receive
(
 char*
//, char*
);

extern uint_32 RDN_init
(
   void
);

extern uint_32 ARP_open
(
   IP_IF_PTR      if_ptr      /* [IN] IP interface structure */
);

extern uint_32 RDN_open
(
   IP_IF_PTR      if_ptr      /* [IN] IP interface structure */
);

extern uint_32 ARP_close
(
   IP_IF_PTR      if_ptr      /* [IN] IP interface structure */
);

extern void ARP_service
(
   RTCSPCB_PTR    pcb_ptr     /* [IN] received packet */
);

extern void RDN_service
(
   RTCSPCB_PTR    pcb_ptr     /* [IN] received packet */
);

extern boolean RDN_transmit //my
(
 IP_IF_PTR  handle,
 char* pBuff,
 char * penetdest	      
);


extern uint_32 ARP_resolve
(
   IP_IF_PTR      if_ptr,     /* [IN] IP interface structure */
   RTCSPCB_PTR    pcb_ptr,    /* [IN] packet to send */
   _ip_address    isrc,       /* [IN] source address */
   _ip_address    idest       /* [IN] destination address */
);

extern boolean ARP_do_proxy
(
   IP_IF_PTR      iflocal,    /* [IN] the local interface */
   _ip_address    iplocal     /* [IN] the IP address to test */
);


extern boolean RDN_do_proxy
(
   IP_IF_PTR      iflocal,    /* [IN] the local interface */
   _ip_address    iplocal     /* [IN] the IP address to test */
);

extern boolean ARP_is_compete
(
  IP_IF_PTR      if_ptr,      /* [IN] IP interface structure */
  _ip_address    addr         /* [IN] the IP address to search for */
);

boolean  ARP_kill_entry
(
  IP_IF_PTR      if_ptr,       /* [IN] IP interface structure */
  _ip_address    addr          /* [IN] the IP address to remove from arp Q */
        
);

extern  RTCSPCB_PTR ARP_find_waiting
(
  IP_IF_PTR      if_ptr,       /* [IN] IP interface structure */
  _ip_address    addr          /* [IN] the IP address to remove from */
);
extern unsigned long Stephany16;
#endif
/* EOF */
