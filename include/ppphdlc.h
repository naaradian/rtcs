#ifndef __ppphdlc_h__
#define __ppphdlc_h__
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
*** File: ppphdlc.h
***
*** Comments:  This file contains the defines, externs and data
***            structure definitions required by application
***            programs in order to use the PPPHDLC device.
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>


/***************************************
**
** MQX compatibility
*/

#define _ppphdlc_mutex             LWSEM_STRUCT
#define PPPHDLC_mutex_init(p)      _lwsem_create(p,1)
#define PPPHDLC_mutex_destroy(p)   _lwsem_destroy(p)
#define PPPHDLC_mutex_lock(p)      _lwsem_wait(p)
#define PPPHDLC_mutex_unlock(p)    _lwsem_post(p)

#define PPPHDLC_memalloc      _mem_alloc_zero
#define PPPHDLC_memfree       _mem_free
#define PPPHDLC_memcopy       _mem_copy

#define _ppphdlc_partid       _rtcs_part
#define PPPHDLC_partcreate    RTCS_part_create
#define PPPHDLC_partdestroy   RTCS_part_destroy
#define PPPHDLC_partalloc     RTCS_part_alloc
#define PPPHDLC_partfree      RTCS_part_free

#define PPPHDLC_delay         _time_delay


/***************************************
**
** Constants
*/

#define PPPHDLC_FRAMESIZE_MAXDATA 1502    /* protocol(2) + data(1500) */
#define PPPHDLC_FRAMESIZE_FCS        2

/*
** Special PPPHDLC characters
*/

#define PPPHDLC_ESC     0x7D
#define PPPHDLC_FLAG    0x7E

#define PPPHDLC_ADDR    0xFF
#define PPPHDLC_CTRL    0x03

#define PPPHDLC_OK      0
#define PPPHDLC_END     1
#define PPPHDLC_ABORT   2

/*
** Size of the Async Control Character Map
*/

#define PPPHDLC_BITS_IN_UINT32  32
#define PPPHDLC_BITS_IN_ACCM    256


typedef struct {
   uint_32           ACCM[PPPHDLC_BITS_IN_ACCM / PPPHDLC_BITS_IN_UINT32];
   boolean           PFC;
   boolean           ACFC;
} PPPHDLC_OPT, _PTR_ PPPHDLC_OPT_PTR;

extern PPPHDLC_OPT PPPHDLC_DEFAULT_OPTIONS;

typedef struct {
   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   uint_32  ST_RX_ABORTED;
   uint_32  ST_RX_RUNT;
   uint_32  ST_RX_GIANT;
   uint_32  ST_RX_BAD_ADDR;
   uint_32  ST_RX_BAD_CTRL;
   uint_32  ST_RX_BAD_FCS;

} PPPHDLC_STATS, _PTR_ PPPHDLC_STATS_PTR;


typedef struct {
   struct _iopcb_table PCB_TABLE;
   PPPHDLC_STATS       STATS;
   _ppphdlc_partid     PART_ID;
   _ppphdlc_mutex      MUTEX;
   FILE_PTR            DEVICE;
   PPPHDLC_OPT         SEND_OPT;
   PPPHDLC_OPT         RECV_OPT;
   uint_16             FCS_SEND;
   uint_16             FCS_RECV;
   void (_CODE_PTR_    UP)(pointer);
   void (_CODE_PTR_    DOWN)(pointer);
   pointer             PARAM;

} PPPHDLC_STRUCT, _PTR_ PPPHDLC_STRUCT_PTR;


extern _iopcb_handle _iopcb_ppphdlc_init(FILE_PTR);


#endif
/* EOF */
