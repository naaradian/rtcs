#ifndef __rtcscfg_h__
#define __rtcscfg_h__
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
*** File: rtcscfg.h
***
*** Comments:  This file contains the definitions for configuring
***            optional features in RTCS.
***
*** RTCS is delivered as follows:
***
***      RTCSCFG_CHECK_ERRORS                   1
***      RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS 1
***      RTCSCFG_CHECK_VALIDITY                 1
***      RTCSCFG_CHECK_ADDRSIZE                 1
***      RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST  0
***      RTCSCFG_BOOTP_RETURN_YIADDR            0
***      RTCSCFG_LOG_SOCKET_API                 1
***      RTCSCFG_LOG_PCB                        1
***      RTCSCFG_LINKOPT_8023                   0
***      RTCSCFG_LINKOPT_8021Q_PRIO             0
***
************************************************************************
*END*******************************************************************/

/***************************************
**
** Error checking options
*/

/*
** When RTCSCFG_CHECK_ERRORS is 1, RTCS API functions will perform
** error checking on its parameters.
*/
#ifndef RTCSCFG_CHECK_ERRORS
#define RTCSCFG_CHECK_ERRORS 1
#endif

/*
** When RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS is 1, RTCS API functions
** will perform error checking on memory allocation.
*/
#ifndef RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS
#define RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS 1
#endif

/*
** When RTCSCFG_CHECK_VALIDITY is 1, RTCS will check the VALID field
** in internal structures before accessing them.
*/
#ifndef RTCSCFG_CHECK_VALIDITY
#define RTCSCFG_CHECK_VALIDITY 1
#endif

/*
** When RTCSCFG_CHECK_ADDRSIZE is 1, RTCS will check whether addrlen
** is at least sizeof(sockaddr_in) in functions that take a struct
** sockaddr _PTR_ parameter.  If not, RTCS returns an error (for bind(),
** connect() and sendto()) or does a partial copy (for accept(),
** getsockname(), getpeername() and recvfrom()).
*/
#ifndef RTCSCFG_CHECK_ADDRSIZE
#define RTCSCFG_CHECK_ADDRSIZE 1
//#define RTCSCFG_CHECK_ADDRSIZE 0
#endif


/***************************************
**
** Protocol behaviour options
*/

/*
** RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST disables the reception and
** forwarding of directed broadcast datagrams.
*/
#ifndef RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST
#define RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST 0
#endif

/*
** When RTCSCFG_BOOTP_RETURN_YIADDR is 1, the BOOTP_DATA_STRUCT has
** an additional field, which will be filled in with the YIADDR field
** of the BOOTREPLY.
*/
#ifndef RTCSCFG_BOOTP_RETURN_YIADDR
#define RTCSCFG_BOOTP_RETURN_YIADDR 0
#endif


/***************************************
**
** Logging options
*/

/*
** When RTCSCFG_LOG_SOCKET_API is 1, RTCS will call RTCS_log() on
** every socket API entry and exit.
*/
#ifndef RTCSCFG_LOG_SOCKET_API
#define RTCSCFG_LOG_SOCKET_API 1
#endif

/*
** When RTCSCFG_LOG_PCB is 1, RTCS will call RTCS_log() every time
** a PCB is allocated, freed, or passed between two layers.
*/
#ifndef RTCSCFG_LOG_PCB
#define RTCSCFG_LOG_PCB 1
#endif


/***************************************
**
** Link layer options
*/

/*
** RTCSCFG_LINKOPT_8023 enables support for sending and receiving
** 802.3 frames.
*/
#ifndef RTCSCFG_LINKOPT_8023
#define RTCSCFG_LINKOPT_8023 0
#endif

/*
** RTCSCFG_LINKOPT_8021Q_PRIO enables support for sending and receiving
** 802.1Q priority tags.
*/
#ifndef RTCSCFG_LINKOPT_8021Q_PRIO
#define RTCSCFG_LINKOPT_8021Q_PRIO 0
#endif


#endif
/* EOF */
