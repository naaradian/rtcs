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
*** File: dnshosts.c
***
*** Comments:  This file contains the default hosts file structures
***            which contains the current list of root name servers.
***            It also contains the default local network name (for
***            appending to non-authoritative names (don't end in a '.')),
***            the default local server name struct, the default local
***            server list, and the initial Zone of Authority list.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "dns.h"

char DNS_Local_network_name[] = ".";
uint_32 DNS_CACHE_NAME_LIMIT = 0;

/***********************************************************************

 This section is for the Root Servers List.
 This should only be assigned to the DNS config_ptr->ROOT_SLIST_PTR

***********************************************************************/

DNS_SLIST_STRUCT DNS_M_Root_server[] =
      {{(uchar _PTR_)"M.ROOT-SERVERS.NET.", NULL,
        0xCA0C1B21, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_L_Root_server[] =
      {{(uchar _PTR_)"L.ROOT-SERVERS.NET.", DNS_M_Root_server,
        0xC620400C, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_K_Root_server[] =
      {{(uchar _PTR_)"K.ROOT-SERVERS.NET.", DNS_L_Root_server,
        0xC1000E81, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_J_Root_server[] =
      {{(uchar _PTR_)"J.ROOT-SERVERS.NET.", DNS_K_Root_server,
        0xC629000A, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_I_Root_server[] =
      {{(uchar _PTR_)"I.ROOT-SERVERS.NET.", DNS_J_Root_server,
        0xC0249411, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_H_Root_server[] =
      {{(uchar _PTR_)"H.ROOT-SERVERS.NET.", DNS_I_Root_server,
        0x803F0235, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_G_Root_server[] =
      {{(uchar _PTR_)"G.ROOT-SERVERS.NET.", DNS_H_Root_server,
        0xC0702404, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_F_Root_server[] =
      {{(uchar _PTR_)"F.ROOT-SERVERS.NET.", DNS_G_Root_server,
        0xC00505F1, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_E_Root_server[] =
      {{(uchar _PTR_)"E.ROOT-SERVERS.NET.", DNS_F_Root_server,
        0xC0CBE60A, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_D_Root_server[] =
      {{(uchar _PTR_)"D.ROOT-SERVERS.NET.", DNS_E_Root_server,
        0x80080A5A, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_C_Root_server[] =
      {{(uchar _PTR_)"C.ROOT-SERVERS.NET.", DNS_D_Root_server,
        0xC021040C, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_B_Root_server[] =
      {{(uchar _PTR_)"B.ROOT-SERVERS.NET.", DNS_C_Root_server,
        0x8009006B, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_A_Root_server[] =
      {{(uchar _PTR_)"A.ROOT-SERVERS.NET.", DNS_B_Root_server,
        0xC6290004, 0, 0, 0, 0, DNS_A, DNS_IN }};


/***********************************************************************
 End of Root List Section
***********************************************************************/


/***********************************************************************

 This section is for the Local Servers List.
 This should only be assigned to the DNS config_ptr->LOCAL_SLIST_PTR

***********************************************************************/


char DNS_Local_server_name[] = "ns.arc.com.";

DNS_SLIST_STRUCT DNS_First_Local_server[] =
   {{(uchar _PTR_)DNS_Local_server_name, 0, INADDR_LOOPBACK, 0, 0, 0, 0,
     DNS_A, DNS_IN }};

/***********************************************************************
 End of Local List Section
***********************************************************************/


/***********************************************************************

 This section is for the Name Server initial Zone of Authority List.
 This should only be used by the Name Server and only assigned to
 the config_ptr->NS_ZONE_AUTH_LIST;

***********************************************************************/


char DNS_Zone_Node_Name_4[] = {"ns4.arc.com"};

     DNS_ADDR_RECORD_STRUCT DNS_zn4_rr1[] =
     {{{ 0, 0, (uchar _PTR_)DNS_Zone_Node_Name_4, 172800, DNS_IN, DNS_A },
      (_ip_address)0xC0A80401 }};

     DNS_RR_LIST_STRUCT DNS_Zone_Name_List_4[] =
     {{(uchar _PTR_)DNS_Zone_Node_Name_4,
       (DNS_RR_HEADER_STRUCT _PTR_)((void _PTR_)DNS_zn4_rr1), 0 }};

char DNS_Zone_Node_Name_3[] = {"ns3.arc.com"};

     DNS_ADDR_RECORD_STRUCT DNS_zn3_rr1[] =
     {{{ 0, 0, (uchar _PTR_)DNS_Zone_Node_Name_3, 172800, DNS_IN, DNS_A },
      (_ip_address)0xC0A80301 }};

     DNS_RR_LIST_STRUCT DNS_Zone_Name_List_3[] =
     {{ (uchar _PTR_)DNS_Zone_Node_Name_3,
        (DNS_RR_HEADER_STRUCT _PTR_)((void _PTR_)DNS_zn3_rr1),
        DNS_Zone_Name_List_4 }};

char DNS_Zone_Node_Name_2[] = {"ns2.arc.com"};

     DNS_ADDR_RECORD_STRUCT DNS_zn2_rr1[] =
     {{{ 0, 0, (uchar _PTR_)DNS_Zone_Node_Name_2, 172800, DNS_IN, DNS_A },
      (_ip_address)0xC0A80201 }};

     DNS_RR_LIST_STRUCT DNS_Zone_Name_List_2[] =
     {{ (uchar _PTR_)DNS_Zone_Node_Name_2,
        (DNS_RR_HEADER_STRUCT _PTR_)((void _PTR_)DNS_zn2_rr1),
        DNS_Zone_Name_List_3 }};


char DNS_Zone_Node_Name_1[] = {"ns1.arc.com"};

     DNS_ADDR_RECORD_STRUCT DNS_zn1_rr1[] =
     {{{ 0, 0, (uchar _PTR_)DNS_Zone_Node_Name_1, 172800, DNS_IN, DNS_A },
      (_ip_address)0xC0A80101 }};

     DNS_RR_LIST_STRUCT DNS_Zone_of_Authority_list[] =
     {{ (uchar _PTR_)DNS_Zone_Node_Name_1,
        (DNS_RR_HEADER_STRUCT _PTR_)((void _PTR_)DNS_zn1_rr1),
        DNS_Zone_Name_List_2 }};


/***********************************************************************
 End of Zone of Authority List Section
***********************************************************************/

/* EOF */
