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
*** File: arpinit.c
***
*** Comments:  This file contains the implementation of the
***            ARP server initialization.
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

pointer ARP_freelist;
pointer RDN_freelist;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_init
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the ARP Server.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_init
   (
      void
   )
{ /* Body */

   ARP_freelist = NULL;
   return RTCS_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_init
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the RDN Server.
*
*END*-----------------------------------------------------------------*/

uint_32 RDN_init
   (
      void
   )
{ /* Body */

   RDN_freelist = NULL;
   return RTCS_OK;

} /* Endbody */


/* EOF */
