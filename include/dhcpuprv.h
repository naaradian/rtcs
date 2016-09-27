#ifndef __dhcpuprv_h__
#define __dhcpuprv_h__
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
*** File: dhcpuprv.h
***
*** Comments:  Dynamic Host Configuration Protocol utility prototypes.
***
************************************************************************
*END*******************************************************************/

/***************************************
**
** Prototypes
**
*/
static void DHCPCLNT_fill_header ( DHCP_CLNT_STRUCT  _PTR_ );
static void DHCPCLNT_fill_options (DHCP_CLNT_STRUCT  _PTR_,
                                   TCPIP_PARM_IF_DHCP _PTR_ );
static void DHCPCLNT_open ( DHCP_CLNT_STRUCT _PTR_ );
static void DHCPCLNT_modify_options ( DHCP_CLNT_STRUCT _PTR_ );
static void DHCPCLNT_send_one_shot ( DHCP_CLNT_STRUCT _PTR_ );
static boolean DHCPCLNT_rebind_lease ( TCPIP_EVENT _PTR_ );
static boolean DHCPCLNT_renew_lease ( TCPIP_EVENT _PTR_ );
static boolean DHCPCLNT_terminate_lease ( TCPIP_EVENT _PTR_ );
static boolean DHCPCLNT_bind_attempt_timeout( TCPIP_EVENT _PTR_ );
static void DHCPCLNT_decline ( DHCP_CLNT_STRUCT _PTR_, uint_32 );
static void DHCPCLNT_parse_offer ( DHCP_CLNT_STRUCT _PTR_, RTCSPCB _PTR_ );
static void DHCPCLNT_copy_binding ( DHCP_CLNT_STRUCT _PTR_ );
static boolean DHCPCLNT_verify_address ( DHCP_CLNT_STRUCT _PTR_ );
static uchar DHCPCLNT_verify_packet (DHCP_CLNT_STRUCT _PTR_, RTCSPCB_PTR, uchar _PTR_, uint_32 _PTR_, _ip_address _PTR_, _ip_address _PTR_, uint_32 _PTR_);
static void DHCPCLNT_service_offer (DHCP_CLNT_STRUCT _PTR_, RTCSPCB_PTR, uchar _PTR_, uint_32, _ip_address, _ip_address);
static void DHCPCLNT_service_ack (DHCP_CLNT_STRUCT _PTR_, RTCSPCB_PTR, uchar _PTR_, uint_32, IP_IF_PTR);
static void DHCPCLNT_service_nak (DHCP_CLNT_STRUCT _PTR_);
static void DHCPCLNT_set_timed_events(DHCP_CLNT_STRUCT _PTR_);

#endif
/* EOF */
