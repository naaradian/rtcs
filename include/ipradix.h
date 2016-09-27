#ifndef __ipradix_h__
#define __ipradix_h__
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
*** File: ipradix.h
***
*** Comments:
***      IP Radix Tree definitions
***
************************************************************************
***********************************************************************/

/***************************************
**
** Type definitions
**
*/

typedef struct ipradix {
   pointer              DATA;    /* must be first field */
   _ip_address          IP;
   _ip_address          MASK;
   struct ipradix _PTR_ PARENT;
   struct ipradix _PTR_ CHILD[2];
} IPRADIX_NODE, _PTR_ IPRADIX_NODE_PTR;


/***************************************
**
** Prototypes
**
*/

void IPRADIX_init(IPRADIX_NODE_PTR);

void IPRADIX_findbest
(
   IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
   _ip_address          ip,      /* [IN] the IP address to search for */
   boolean (_CODE_PTR_  test)(pointer, pointer),
                                 /* [IN] application-specific test */
   pointer              data     /* [IN] data for test() */
);

void IPRADIX_insert
(
   IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
   _ip_address          ip,      /* [IN] the IP address to insert */
   _ip_address          mask,    /* [IN] the IP network mask */
   _rtcs_part           part,    /* [IN] the node partition */
   void (_CODE_PTR_     insert)(pointer _PTR_, pointer),
                                 /* [IN] application-specific insert */
   pointer              data     /* [IN] data for insert() */
);

void IPRADIX_walk
(
   IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
   boolean (_CODE_PTR_  test)(_ip_address, _ip_address, pointer, pointer),
                                 /* [IN] application-specific test */
   pointer              data     /* [IN] data for test() */
);

#endif
/* EOF */
