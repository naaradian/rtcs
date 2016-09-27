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
*** File: ifstat.c
***
*** Comments:  This file has functions to retrieve statistics structures
***            for each protocol in RTCS.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "tcp.h"
#include "udp_prv.h"
#include "icmp_prv.h"
#include "ip_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IP_stats
* Returned Value  :  IP statistics
* Comments        :
*     Returns a pointer to the IP layer statistics.
*
*END*-----------------------------------------------------------------*/

IP_STATS_PTR IP_stats
   (
      void
   )
{ /* Body */
   IP_CFG_STRUCT_PTR   ip_cfg_ptr;

   ip_cfg_ptr = RTCS_getcfg(IP);

   if (ip_cfg_ptr != NULL) {
      return &ip_cfg_ptr->STATS;
   } /* Endif */

   return NULL;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  ICMP_stats
* Returned Value  :  ICMP statistics
* Comments        :
*     Returns a pointer to the ICMP layer statistics.
*
*END*-----------------------------------------------------------------*/

ICMP_STATS_PTR ICMP_stats
   (
      void
   )
{ /* Body */
   ICMP_CFG_STRUCT_PTR   icmp_cfg_ptr;

   icmp_cfg_ptr = RTCS_getcfg(ICMP);

   if (icmp_cfg_ptr != NULL) {
      return &icmp_cfg_ptr->STATS;
   } /* Endif */

   return NULL;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  UDP_stats
* Returned Value  :  UDP statistics
* Comments        :
*     Returns a pointer to the UDP layer statistics.
*
*END*-----------------------------------------------------------------*/

UDP_STATS_PTR UDP_stats
   (
      void
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   udp_cfg_ptr;

   udp_cfg_ptr = RTCS_getcfg(UDP);

   if (udp_cfg_ptr != NULL) {
      return &udp_cfg_ptr->STATS;
   } /* Endif */

   return NULL;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  TCP_stats
* Returned Value  :  TCP statistics
* Comments        :
*     Returns a pointer to the TCP layer statistics.
*
*END*-----------------------------------------------------------------*/

TCP_STATS_PTR TCP_stats
   (
      void
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg_ptr;

   tcp_cfg_ptr = RTCS_getcfg(TCP);

   if (tcp_cfg_ptr != NULL) {
      return &tcp_cfg_ptr->STATS;
   } /* Endif */

   return NULL;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPIF_stats
* Returned Value  :  IPIF statistics
* Comments        :
*     Returns a pointer to an interface's statistics.
*
*END*-----------------------------------------------------------------*/

IPIF_STATS_PTR IPIF_stats
   (
      _rtcs_if_handle   handle
         /* [IN] the RTCS interface state structure */
   )
{ /* Body */
   IP_IF_PTR if_ptr = (IP_IF_PTR)handle;
   return &if_ptr->STATS;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  ARP_stats
* Returned Value  :  ARP statistics
* Comments        :
*     Returns a pointer to an interface's ARP statistics.
*
*END*-----------------------------------------------------------------*/

ARP_STATS_PTR ARP_stats
   (
      _rtcs_if_handle   handle
         /* [IN] the RTCS interface state structure */
   )
{ /* Body */
   IP_IF_PTR if_ptr = (IP_IF_PTR)handle;
   return (ARP_STATS_PTR)if_ptr->ARP;
} /* Endbody */


/* EOF */
