#ifndef __route_h__
#define __route_h__
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
*** File: route.h
***
*** Comments:
***    definition for the route core functions
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Constants
**
*/


/***************************************
**
** Type definitions
**
*/


/***************************************
**
** Prototypes
**
*/
void ROUTE_register(
   IP_ROUTE_FN_PTR   routefn  /* [IN] list of routing functions */
);

uint_32 ROUTE_new_bindif(
   IP_ROUTE_DIRECT_PTR   bindif   /* [IN] the outgoing binded interface */
);

IP_ROUTE_INDIRECT_PTR ROUTE_get(
   _ip_address    netaddr, /* [IN] the network address */
   _ip_address    netmask  /* [IN] the network mask */
);

uint_32 ROUTE_insert(
   IP_ROUTE_INDIRECT_PTR    gate,    /* [IN] the route entry to insert */
   _ip_address              netaddr, /* [IN] the network address */
   _ip_address              netmask  /* [IN] the network mask */

);

uint_32 ROUTE_remove(
   IP_ROUTE_INDIRECT_PTR    gate     /* [IN] the route entry to insert */
);

#endif
/* EOF */
