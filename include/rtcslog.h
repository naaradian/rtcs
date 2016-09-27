#ifndef __rtcslog_h__
#define __rtcslog_h__
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
*** File: rtcslog.h
***
*** Comments:  This file contains the definitions for RTCS logging.
***
************************************************************************
*END*******************************************************************/


/*
** Code macros
*/

#define RTCSLOG_NONE(t,a1,a2)

#define RTCSLOG_LOG(t,a1,a2) RTCS_log(t,a1,a2)

#if RTCSCFG_LOG_SOCKET_API
#define RTCSLOG_API     RTCSLOG_LOG
#else
#define RTCSLOG_API     RTCSLOG_NONE
#endif

#if RTCSCFG_LOG_PCB
#define RTCSLOG_PCB     RTCSLOG_LOG
#else
#define RTCSLOG_PCB     RTCSLOG_NONE
#endif

#define RTCSLOG_PCB_ALLOC(pcb)            RTCSLOG_PCB( RTCSLOG_TYPE_PCB, \
                                                       RTCSLOG_PCBCMD_ALLOC, \
                                                       (uint_32)pcb)

#define RTCSLOG_PCB_FREE(pcb,err)         RTCSLOG_PCB( RTCSLOG_TYPE_PCB, \
                                                       RTCSLOG_PCBCMD_FREE | (err), \
                                                       (uint_32)pcb)

#define RTCSLOG_PCB_READ(pcb,prot,vers)   RTCSLOG_PCB( RTCSLOG_TYPE_PCB, \
                                                       RTCSLOG_PCBCMD_READ | ((vers) << 16) | (prot), \
                                                       (uint_32)pcb)

#define RTCSLOG_PCB_WRITE(pcb,prot,vers)  RTCSLOG_PCB( RTCSLOG_TYPE_PCB, \
                                                       RTCSLOG_PCBCMD_WRITE | ((vers) << 16) | (prot), \
                                                       (uint_32)pcb)

#define RTCSLOG_TIMER   RTCSLOG_NONE

/*
** Obsolete macros
*/

#define DEBUGTCP(x)     /* x */

/*
** PCB actions
*/

#define RTCSLOG_PCBCMD_ALLOC     0x00000000
#define RTCSLOG_PCBCMD_FREE      0x40000000
#define RTCSLOG_PCBCMD_READ      0x80000000
#define RTCSLOG_PCBCMD_WRITE     0xC0000000


/*
** Function numbers
*/

#define RTCSLOG_FN_SOCKET              (RTCSLOG_FNBASE   )
#define RTCSLOG_FN_DGRAM_BIND          (RTCSLOG_FNBASE+ 1)
#define RTCSLOG_FN_DGRAM_CONNECT       (RTCSLOG_FNBASE+ 2)
#define RTCSLOG_FN_DGRAM_LISTEN        (RTCSLOG_FNBASE+ 3)
#define RTCSLOG_FN_DGRAM_ACCEPT        (RTCSLOG_FNBASE+ 4)
#define RTCSLOG_FN_DGRAM_GETSOCKNAME   (RTCSLOG_FNBASE+ 5)
#define RTCSLOG_FN_DGRAM_GETPEERNAME   (RTCSLOG_FNBASE+ 6)
#define RTCSLOG_FN_DGRAM_RECV          (RTCSLOG_FNBASE+ 7)
#define RTCSLOG_FN_DGRAM_RECVFROM      (RTCSLOG_FNBASE+ 8)
#define RTCSLOG_FN_DGRAM_RECVMSG       (RTCSLOG_FNBASE+ 9)
#define RTCSLOG_FN_DGRAM_SEND          (RTCSLOG_FNBASE+10)
#define RTCSLOG_FN_DGRAM_SENDTO        (RTCSLOG_FNBASE+11)
#define RTCSLOG_FN_DGRAM_SENDMSG       (RTCSLOG_FNBASE+12)
#define RTCSLOG_FN_DGRAM_SOCKATMARK    (RTCSLOG_FNBASE+13)
#define RTCSLOG_FN_DGRAM_SHUTDOWN      (RTCSLOG_FNBASE+14)
#define RTCSLOG_FN_STREAM_BIND         (RTCSLOG_FNBASE+15)
#define RTCSLOG_FN_STREAM_CONNECT      (RTCSLOG_FNBASE+16)
#define RTCSLOG_FN_STREAM_LISTEN       (RTCSLOG_FNBASE+17)
#define RTCSLOG_FN_STREAM_ACCEPT       (RTCSLOG_FNBASE+18)
#define RTCSLOG_FN_STREAM_GETSOCKNAME  (RTCSLOG_FNBASE+19)
#define RTCSLOG_FN_STREAM_GETPEERNAME  (RTCSLOG_FNBASE+20)
#define RTCSLOG_FN_STREAM_RECV         (RTCSLOG_FNBASE+21)
#define RTCSLOG_FN_STREAM_RECVFROM     (RTCSLOG_FNBASE+22)
#define RTCSLOG_FN_STREAM_RECVMSG      (RTCSLOG_FNBASE+23)
#define RTCSLOG_FN_STREAM_SEND         (RTCSLOG_FNBASE+24)
#define RTCSLOG_FN_STREAM_SENDTO       (RTCSLOG_FNBASE+25)
#define RTCSLOG_FN_STREAM_SENDMSG      (RTCSLOG_FNBASE+26)
#define RTCSLOG_FN_STREAM_SOCKATMARK   (RTCSLOG_FNBASE+27)
#define RTCSLOG_FN_STREAM_SHUTDOWN     (RTCSLOG_FNBASE+28)
#define RTCSLOG_FN_SOCKET_GETSOCKOPT   (RTCSLOG_FNBASE+29)
#define RTCSLOG_FN_SOCKET_SETSOCKOPT   (RTCSLOG_FNBASE+30)
#define RTCSLOG_FN_UDP_GETSOCKOPT      (RTCSLOG_FNBASE+31)
#define RTCSLOG_FN_UDP_SETSOCKOPT      (RTCSLOG_FNBASE+32)
#define RTCSLOG_FN_TCP_GETSOCKOPT      (RTCSLOG_FNBASE+33)
#define RTCSLOG_FN_TCP_SETSOCKOPT      (RTCSLOG_FNBASE+34)
#define RTCSLOG_FN_IP_GETSOCKOPT       (RTCSLOG_FNBASE+35)
#define RTCSLOG_FN_IP_SETSOCKOPT       (RTCSLOG_FNBASE+36)
#define RTCSLOG_FN_IGMP_GETSOCKOPT     (RTCSLOG_FNBASE+37)
#define RTCSLOG_FN_IGMP_SETSOCKOPT     (RTCSLOG_FNBASE+38)
#define RTCSLOG_FN_LINK_GETSOCKOPT     (RTCSLOG_FNBASE+39)
#define RTCSLOG_FN_LINK_SETSOCKOPT     (RTCSLOG_FNBASE+40)
#define RTCSLOG_FN_NAT_GETSOCKOPT      (RTCSLOG_FNBASE+41)
#define RTCSLOG_FN_NAT_SETSOCKOPT      (RTCSLOG_FNBASE+42)

/*
** Prototypes
*/

#ifdef __cplusplus
extern "C" {
#endif

void RTCS_log (uint_32 logtype, uint_32 p1, uint_32 p2);

void RTCSLOG_enable       (uint_32 logtype);
void RTCSLOG_disable      (uint_32 logtype);
void RTCSLOG_enable_prot  (uint_32 logtype, uint_32 protocol);
void RTCSLOG_disable_prot (uint_32 logtype, uint_32 protocol);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
