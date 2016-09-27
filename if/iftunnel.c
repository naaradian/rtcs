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
*** File: iftunnel.c
***
*** Comments:  This file contains the interface functions to the
***            RTCS tunnel device.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "iptunnel.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_tunnel_add
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Adds an IP over IP tunnel to RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_tunnel_add
   (
      /* [IN] Inner IP header source address to tunnel */
      _ip_address  inner_source_addr,
      /* [IN] Inner IP header source address network mask */
      _ip_address  inner_source_netmask,
      /* [IN] Inner IP header destination address to tunnel */
      _ip_address  inner_dest_addr,
      /* [IN] Inner IP header destination network mask */
      _ip_address  inner_dest_netmask,
      /* [IN] Outer IP header source address */
      _ip_address  outer_source_addr,
      /* [IN] Outer IP header destination address */
      _ip_address  outer_dest_addr,
      /* [IN] Flags to set tunnel behaviour */
      uint_32 flags
   )
{ /* Body */
   IPIP_PARM  parms;

   parms.TUNNEL.INNER_SOURCE         = inner_source_addr & inner_source_netmask;
   parms.TUNNEL.INNER_SOURCE_NETMASK = inner_source_netmask;
   parms.TUNNEL.INNER_DEST           = inner_dest_addr & inner_dest_netmask;
   parms.TUNNEL.INNER_DEST_NETMASK   = inner_dest_netmask;
   parms.TUNNEL.OUTER_DEST           = outer_dest_addr;
   parms.TUNNEL.OUTER_SOURCE         = outer_source_addr;
   parms.TUNNEL.FLAGS                = flags;

   return RTCSCMD_issue(parms, IPIP_insert);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_tunnel_remove
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Removes an IP over IP tunnel from RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_tunnel_remove
   (
      /* [IN] Inner IP header source address to tunnel */
      _ip_address  inner_source_addr,
      /* [IN] Inner IP header source address network mask */
      _ip_address  inner_source_netmask,
      /* [IN] Inner IP header destination address to tunnel */
      _ip_address  inner_dest_addr,
      /* [IN] Inner IP header destination network mask */
      _ip_address  inner_dest_netmask,
      /* [IN] Outer IP header source address */
      _ip_address  outer_source_addr,
      /* [IN] Outer IP header destination address */
      _ip_address  outer_dest_addr,
      /* [IN] Flags to set tunnel behaviour */
      uint_32 flags
   )
{ /* Body */
   IPIP_PARM  parms;

   parms.TUNNEL.INNER_SOURCE         = inner_source_addr & inner_source_netmask;
   parms.TUNNEL.INNER_SOURCE_NETMASK = inner_source_netmask;
   parms.TUNNEL.INNER_DEST           = inner_dest_addr & inner_dest_netmask;
   parms.TUNNEL.INNER_DEST_NETMASK   = inner_dest_netmask;
   parms.TUNNEL.OUTER_DEST           = outer_dest_addr;
   parms.TUNNEL.OUTER_SOURCE         = outer_source_addr;
   parms.TUNNEL.FLAGS                = flags;

   return RTCSCMD_issue(parms, IPIP_delete);

} /* Endbody */


/* EOF */
