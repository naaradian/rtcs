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
*** File: ipif_add.c
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
#include "route.h"


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_add
*  Parameters     :
*
*     pointer              mhandle     [IN] the packet driver handle
*     _rtcs_if_handle      ihandle     [OUT] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      [IN] the call table
*     _ip_address          address     not used
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     not used
*
*  Comments       :
*        Registers a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

void IPIF_add
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   IP_IF_PTR         ipif;
   uint_32           i, error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   /* Allocate a block of entries if none are left */
   if (!IP_cfg_ptr->IF_FREE) {
      ipif = _mem_alloc_zero(IPIFALLOC_SIZE * sizeof(IP_IF));
      if (ipif) {
         for (i = 0; i < IPIFALLOC_SIZE; i++) {
            ipif->HANDLE = IP_cfg_ptr->IF_FREE;
            IP_cfg_ptr->IF_FREE = ipif;
            ipif++;
         } /* Endfor */
      } /* Endif */
   } /* Endif */

   /* Dequeue a free entry */
   ipif = IP_cfg_ptr->IF_FREE;
   if (!ipif) {
      RTCSCMD_complete(parms, RTCSERR_IP_IF_ALLOC);
      return;
   } /* Endif */

   /* Initialize the interface */
   IP_cfg_ptr->IF_FREE = ipif->HANDLE;
   ipif->HANDLE = parms->mhandle;
   ipif->DEVICE = *parms->if_ptr;
   ipif->ARP    = NULL;
   ipif->BOOTFN = NULL;
   ipif->MTU_FN = NULL;
   error = ipif->DEVICE.OPEN(ipif);
   if (error) {
      ipif->HANDLE = IP_cfg_ptr->IF_FREE;
      IP_cfg_ptr->IF_FREE = ipif;
      RTCSCMD_complete(parms, error);
      return;
   } /* Endif */

   /* If IGMP_ipif_add fails, it isn't sufficient reason to abort IPIF_add */
   IGMP_ipif_add(ipif);

   parms->ihandle = (_rtcs_if_handle)ipif;
   RTCSCMD_complete(parms, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_bind
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Binds an IP address and network to a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_bind
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   _ip_address       address, netmask;
   uint_32           error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   address = parms->address;
   netmask = parms->netmask;

   /*
   ** Make sure the netmask is valid.  We use the fact that
   ** (x & x+1) == 0  <=>  x = 2^n-1.
   */
   if (~netmask & (~netmask + 1)) {
      RTCSCMD_complete(parms, RTCSERR_IP_BIND_MASK);
      return;
   } /* Endif */

   /*
   ** Make sure the address is valid.
   */
   if (((address & ~netmask) == 0)
    || ((address & ~netmask) == ~netmask)) {
      RTCSCMD_complete(parms, RTCSERR_IP_BIND_ADDR);
      return;
   } /* Endif */

   /* Create the route entry for the directly connected network */
   error = IP_route_add_direct(address, netmask, (IP_IF_PTR)parms->ihandle,
      (IP_IF_PTR)parms->ihandle);

   /* Add the IP and the LOCALHOST interface into the routing table */
   if (!error) {
      error = IP_route_add_direct(address, 0xFFFFFFFF, (IP_IF_PTR)parms->ihandle,
         IP_cfg_ptr->IF_LOCALHOST);
   } /* Endif */

   if (error) {
      RTCSCMD_complete(parms, error);
      return;
   } /* Endif */

   /* IGMP_ipif_bind failure isn't sufficient reason to abort IPIF_bind */
   IGMP_ipif_bind((IP_IF_PTR)parms->ihandle, address);

   RTCSCMD_complete(parms, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_bind_ppp
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
*        Binds an IP address and network to a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_bind_ppp
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   _ip_address       address, peer;
   uint_32           error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   address = parms->address;
   peer    = parms->network;
   
   /* Create the route entry for the directly connected peer */
   error = IP_route_add_virtual(peer, 0xFFFFFFFF, address,
      INADDR_ANY, INADDR_ANY, parms->ihandle, NULL);

   /* Add the IP and the LOCALHOST interface into the routing table */
   if (!error) {
      error = IP_route_add_direct(address, 0xFFFFFFFF, parms->ihandle,
         IP_cfg_ptr->IF_LOCALHOST);
   } /* Endif */

   if (error) {
      RTCSCMD_complete(parms, error);
      return;
   } /* Endif */

   /* IGMP_ipif_bind failure isn't sufficient reason to abort IPIF_bind */
   IGMP_ipif_bind((IP_IF_PTR)parms->ihandle, address);

   RTCSCMD_complete(parms, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_gate_add
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     not used
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the gateway address
*     _ip_address          locmask     [IN] the gateway metric
*     _ip_address          network     [IN] the network address
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Adds a gateway to the routing table.
*
*END*-----------------------------------------------------------------*/

void IPIF_gate_add
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */
   _ip_address          netmask = parms->netmask;
   uint_32              error;

   /*
   ** Make sure the netmask is valid.  We use the fact that
   ** (x & x+1) == 0  <=>  x = 2^n-1.
   */
   if (~netmask & (~netmask + 1)) {
      RTCSCMD_complete(parms, RTCSERR_IP_BIND_MASK);
      return;
   } /* Endif */

   /* Start CR 1133 */
   error = IP_route_add_indirect(parms->address, netmask, parms->network, RTF_STATIC, parms->locmask);
   /* End CR */

   RTCSCMD_complete(parms, error);

} /* Endbody */


/* EOF */
