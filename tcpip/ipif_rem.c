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
*** File: ipif_rem.c
***
*** Comments:  This file contains the implementation of the
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
*  Function Name  : IPIF_remove
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     not used
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unregisters a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

void IPIF_remove
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   IP_IF_PTR         ipif;
   uint_32           error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   ipif = (IP_IF_PTR)parms->ihandle;
   error = ipif->DEVICE.CLOSE(ipif);

   /* Enqueue onto the free list */
   ipif->HANDLE = IP_cfg_ptr->IF_FREE;
   IP_cfg_ptr->IF_FREE = ipif;

   RTCSCMD_complete(parms, error);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_unbind
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unbinds an IP address and network from a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_unbind
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */
   _ip_address    mask;

   if (!IP_get_netmask(parms->ihandle, parms->address, &mask)) {
      RTCSCMD_complete(parms, RTCSERR_IP_BIND_ADDR);
      return;
   } /* Endif */

   IP_route_remove_direct(parms->address, 0xFFFFFFFF, parms->ihandle);
   if (parms->ihandle) {
      ((IP_IF_PTR)(parms->ihandle))->IGMP_UNBIND((IP_IF_PTR)parms->ihandle,
         parms->address);
   } /* Endif */
   IP_route_remove_direct(parms->address, mask, parms->ihandle);

   RTCSCMD_complete(parms, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_unbind_ppp
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     [IN] the peer address
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unbinds an IP address and network from a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_unbind_ppp
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

   IP_route_remove_virtual(parms->network, 0xFFFFFFFF, parms->address,
      INADDR_ANY, INADDR_ANY, parms->ihandle);
   if (parms->ihandle) {
      ((IP_IF_PTR)(parms->ihandle))->IGMP_UNBIND((IP_IF_PTR)parms->ihandle,
         parms->address);
   } /* Endif */
   IP_route_remove_direct(parms->address, 0xFFFFFFFF, parms->ihandle);

   RTCSCMD_complete(parms, RTCS_OK);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_gate_remove
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     not used
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the gateway address
*     _ip_address          locmask     not used
*     _ip_address          network     [IN] the network address
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Removes a gateway from the routing table.
*
*END*-----------------------------------------------------------------*/

void IPIF_gate_remove
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

   /* Start CR 1133 */
   IP_route_remove_indirect(parms->address, parms->netmask, parms->network, RTF_STATIC, parms->locmask);
   /* End CR */

   RTCSCMD_complete(parms, RTCS_OK);

} /* Endbody */


/* EOF */
