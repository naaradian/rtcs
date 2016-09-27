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
*** File: ifadd.c
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

extern unsigned long keephandle;

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_add1
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_add1
   (
      pointer                 mhandle,
         /* [IN] the packet driver handle */
      RTCS_IF_STRUCT_PTR      if_ptr,
         /* [IN] call table for the interface */
      _rtcs_if_handle _PTR_   handle
         /* [OUT] the RTCS interface state structure */
   )
{ /* Body */
   IPIF_PARM   parms;
   uint_32     error;

   parms.mhandle = mhandle;
   parms.if_ptr  = if_ptr;

// error = RTCSCMD_issue1(parms, IPIF_add);
   error = RTCSCMD_issue(parms, IPIF_bdd1);
   if (!error) {
      *handle = parms.ihandle;
   } /* Endif */
   return error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_add
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_add
   (
      pointer                 mhandle,
         /* [IN] the packet driver handle */
      RTCS_IF_STRUCT_PTR      if_ptr,
         /* [IN] call table for the interface */
      _rtcs_if_handle _PTR_   handle
         /* [OUT] the RTCS interface state structure */
   )
{ /* Body */
   IPIF_PARM   parms;
   uint_32     error;

   parms.mhandle = mhandle;
   parms.if_ptr  = if_ptr;

   error = RTCSCMD_issue(parms, IPIF_add);

   if (!error) {
      *handle = parms.ihandle;
 //      keephandle = (unsigned long)(parms.ihandle); //101215		
   } /* Endif */
   return error;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind1
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Bind an IP address to a registered hardware interface.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_bind1
   (
      _rtcs_if_handle   handle,
         /* [IN] the RTCS interface state structure */
      _ip_address       address,
         /* [IN] the IP address for the interface */
      _ip_address       netmask
         /* [IN] the IP (sub)network mask for the interface */
   )
{ /* Body */
   IPIF_PARM   parms;

   parms.ihandle = handle;
   parms.address = address;
   parms.locmask = 0xFFFFFFFFL;
   parms.netmask = netmask;

  //t return RTCSCMD_issue(parms, IPIF_bind);
 return 555;//t

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Bind an IP address to a registered hardware interface.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_bind
   (
      _rtcs_if_handle   handle,
         /* [IN] the RTCS interface state structure */
      _ip_address       address,
         /* [IN] the IP address for the interface */
      _ip_address       netmask
         /* [IN] the IP (sub)network mask for the interface */
   )
{ /* Body */
   IPIF_PARM   parms;

   parms.ihandle = handle;
   parms.address = address;
   parms.locmask = 0xFFFFFFFFL;
   parms.netmask = netmask;

   return RTCSCMD_issue(parms, IPIF_bind);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gate_add
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a gateway with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_gate_add
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

   return RTCSCMD_issue(parms, IPIF_gate_add);

} /* Endbody */


/* Start CR 1133 */
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gate_add_metric
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a gateway with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_gate_add_metric
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

   return RTCSCMD_issue(parms, IPIF_gate_add);

} /* Endbody */
/* End CR */


/* EOF */
