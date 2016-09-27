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
*** File: rtcsglob.c
***
*** Comments:  Definitions of global variables required for RTCS.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


uint_32  _RTCSQUEUE_base = 0x10;

/*
** Number of RTCSPCBs to allocate
*/
uint_32  _RTCSPCB_max = 32;

/*
** TCP/IP task priority and extra stack size
*/
uint_32  _RTCSTASK_priority   = 6;
uint_32  _RTCSTASK_stacksize  = 0;
boolean  _RTCS_initialized    = FALSE;

/*
** TCP-based RPC task priority and extra stack size
*/
uint_32  _SVCTCPTASK_priority    = 9;
uint_32  _SVCTCPTASK_stacksize   = 0;

/*
** Indicates whether or not IP should forward packets between interfaces
*/
//boolean  _IP_forward = FALSE;
boolean  _IP_forward = TRUE; //work settin on debugger only

/*
** Indicates whether or not to bypass checksum checking
*/
boolean  _TCP_bypass_rx = FALSE;
boolean  _TCP_bypass_tx = FALSE;

/*
** The TCP retransmission timeout minimum
*/
uint_32  _TCP_rto_min;     /* initialized by TCP_Init() */

/*
** Maximum number of packets that will be enqueued
*/
uint_32  _UDP_max_queue_size  = 100;


/* EOF */
