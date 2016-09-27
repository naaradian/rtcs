#ifndef __dhcpsrv_h__
#define __dhcpsrv_h__
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
*** File: dhcpsrv.h
***
*** Comments:  Dynamic Host Configuration Protocol definitions.
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Constants
**
*/

#define DHCPTIME_OFFER  10       /* 10 seconds */


/***************************************
**
** Type definitions
**
*/

typedef struct {

   uint_32        COUNT;

   _ip_address    SERVERID;
   uint_32        LEASE;
   uint_32        MASK;

   uchar_ptr      OPTION_PTR;
   uint_32        OPTION_LEN;

   _ip_address    SADDR;
   uchar          SNAME[64];
   uchar          FILE[128];

} DHCPSRV_OPTIONS_STRUCT, _PTR_ DHCPSRV_OPTIONS_STRUCT_PTR;

typedef struct dhcpsrv_addr {

   struct dhcpsrv_addr  _PTR_ NEXT;
   uint_32                    EXPIRE;
   uint_32                    OFFER;
   uint_32                    CLIENTID_LEN;
   uchar_ptr                  CLIENTID_PTR;

   _ip_address                IP_ADDR;
   DHCPSRV_OPTIONS_STRUCT_PTR OPTIONS;

} DHCPSRV_ADDR_STRUCT, _PTR_ DHCPSRV_ADDR_STRUCT_PTR;

typedef struct {

   uchar                   SND_BUFFER[DHCP_MSGSIZE_MIN];
   uchar                   RCV_BUFFER[DHCP_MSGSIZE_MIN];
   uint_32                 SND_BUFFER_LEN;
   uint_32                 RCV_BUFFER_LEN;
   uint_32                 SOCKET;
   uint_32                 TIME;
   _rtcs_mutex             IPLIST_SEM;
   uint_32                 FLAGS;
   DHCPSRV_ADDR_STRUCT_PTR IP_AVAIL;
   DHCPSRV_ADDR_STRUCT_PTR IP_OFFERED;
   DHCPSRV_ADDR_STRUCT_PTR IP_LEASED;
   DHCPSRV_ADDR_STRUCT_PTR IP_TAKEN;

} DHCPSRV_STATE_STRUCT, _PTR_ DHCPSRV_STATE_STRUCT_PTR;

#define DHCPSVR_FLAG_DO_PROBE	0x00000001

#endif
/* EOF */
