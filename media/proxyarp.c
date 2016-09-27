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
*** File: proxyarp.c
***
*** Comments:  This file contains a hook into ARP to allow an
***            application to do ProxyARP.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "arp.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_do_proxy
*  Returned Value : TRUE or FALSE
*  Comments       :
*        Decides whether or not to send ARP replies on behalf
*        of another IP host.  Default is to never do ProxyARP,
*        but this function can be replaced in an application.
*
*END*-----------------------------------------------------------------*/

boolean ARP_do_proxy
   (
      IP_IF_PTR      iflocal,
            /* [IN] the interface that the ARP request arrived on */
      _ip_address    iplocal
            /* [IN] the IP address to test */
   )
{ /* Body */

   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RDN_do_proxy
*  Returned Value : TRUE or FALSE
*  Comments       :
*        Decides whether or not to send RDN replies on behalf
*        of another IP host.  Default is to never do ProxyARP,
*        but this function can be replaced in an application.
*
*END*-----------------------------------------------------------------*/

boolean RDN_do_proxy
   (
      IP_IF_PTR      iflocal,
            /* [IN] the interface that the ARP request arrived on */
      _ip_address    iplocal
            /* [IN] the IP address to test */
   )
{ /* Body */

   return FALSE;

} /* Endbody */
/* EOF */
