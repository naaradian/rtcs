#ifndef __rtcsrtos_h__
#define __rtcsrtos_h__
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
*** File: rtcsrtos.h
***
*** Comments:  This file contains the definitions required to
***            use RTCS with MQX.
***
************************************************************************
*END*******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#if   defined(RTCS_OS_MQX23X)
#include <rtcs23x.h>

#elif defined(RTCS_OS_MQX25X)
#include <rtcs25x.h>

#elif defined(RTCS_OS_BIOS)
#include <rtcsbios.h>

#else
#include <rtcs24x.h>
#endif


/***************************************
**
** Random Number Generation
*/

void     RTCS_rand_seed (uint_32);
uint_32  RTCS_rand      (void);


/***************************************
**
** The _iopcb_handle
*/

#ifndef __IOPCB__
#define __IOPCB__

#define IOPCB_IOCTL_S_ACCM       4
#define IOPCB_IOCTL_R_ACCM       5
#define IOPCB_IOCTL_GET_IFTYPE   6
#define IOPCB_IOCTL_S_PFC        14
#define IOPCB_IOCTL_R_PFC        15
#define IOPCB_IOCTL_S_ACFC       16
#define IOPCB_IOCTL_R_ACFC       17

typedef struct _iopcb_table {
   uint_32 (_CODE_PTR_ OPEN)  (struct _iopcb_table _PTR_, void (_CODE_PTR_)(pointer), void (_CODE_PTR_)(pointer), pointer);
   uint_32 (_CODE_PTR_ CLOSE) (struct _iopcb_table _PTR_);
   PCB_PTR (_CODE_PTR_ READ)  (struct _iopcb_table _PTR_, uint_32);
   void    (_CODE_PTR_ WRITE) (struct _iopcb_table _PTR_, PCB_PTR, uint_32);
   uint_32 (_CODE_PTR_ IOCTL) (struct _iopcb_table _PTR_, uint_32, pointer);
} _PTR_ _iopcb_handle;

#define _iopcb_open(h,up,down,param)   ((h)->OPEN(h,up,down,param))
#define _iopcb_close(h)                ((h)->CLOSE(h))
#define _iopcb_read(h,flags)           ((h)->READ(h,flags))
#define _iopcb_write(h,pcb,flags)      ((h)->WRITE(h,pcb,flags))
#define _iopcb_ioctl(h,opt,val)        ((h)->IOCTL(h,opt,val))

#endif


/***************************************
**
** The Ethernet driver API
*/

uint_32 ENET_send  (pointer, PCB_PTR, uint_16, uchar[6], uint_32);
uint_32 ENET_open  (pointer, uint_16, void (_CODE_PTR_)(PCB_PTR, pointer), pointer);
uint_32 ENET_close (pointer, uint_16);
uint_32 ENET_join  (pointer, uint_16, uchar[6]);
uint_32 ENET_leave (pointer, uint_16, uchar[6]);

uint_32 ENET_get_address (pointer, uchar[6]);
uint_32 ENET_get_speed   (pointer);


#ifdef __cplusplus
}
#endif


#endif
/* EOF */
