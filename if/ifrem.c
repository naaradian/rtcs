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
*** File: ifrem.c
***
*** Comments:  This file contains the interface functions to the
***            packet driver interface.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_remove
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Unregister a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_remove
   (
      _rtcs_if_handle   handle
         /* [IN] the RTCS interface state structure */
   )
{ /* Body */
   IPIF_PARM   parms;

   parms.ihandle = handle;

   return RTCSCMD_issue(parms, IPIF_remove);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_unbind
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Unbind an IP address from a registered hardware interface.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_unbind
   (
      _rtcs_if_handle   handle,
         /* [IN] the RTCS interface state structure */
      _ip_address       address
         /* [IN] the IP address for the interface */
   )
{ /* Body */
   IPIF_PARM   parms;

   parms.ihandle = handle;
   parms.address = address;

   return RTCSCMD_issue(parms, IPIF_unbind);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gate_remove
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a gateway with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_gate_remove
   (
      _ip_address       gateway,
         /* [IN] the IP address of the gateway */
      _ip_address       network,
         /* [IN] the IP (sub)network to route */
      _ip_address       netmask
         /* [IN] the IP (sub)network mask to route */
   )
{ /* Body */
   IPIF_PARM   parms;

   parms.address = gateway;
   parms.network = network;
   parms.netmask = netmask;
   /* Start CR 1133 */
   parms.locmask = 0;
   /* End CR */

   return RTCSCMD_issue(parms, IPIF_gate_remove);

} /* Endbody */


/* Start CR 1133 */
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gate_remove_metric
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a gateway with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_gate_remove_metric
   (
      _ip_address       gateway,
         /* [IN] the IP address of the gateway */
      _ip_address       network,
         /* [IN] the IP (sub)network to route */
      _ip_address       netmask,
         /* [IN] the IP (sub)network mask to route */
      uint_16           metric
         /* [IN] the metric of the gateway */
   )
{ /* Body */
   IPIF_PARM   parms;

   parms.address = gateway;
   parms.network = network;
   parms.netmask = netmask;
   parms.locmask = metric;

   return RTCSCMD_issue(parms, IPIF_gate_remove);

} /* Endbody */
/* End CR */

/* EOF */
