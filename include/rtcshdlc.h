#ifndef __rtcshdlc_h__
#define __rtcshdlc_h__
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
*** File: rtcshdlc.h
***
*** Comments:  This file contains the defines, externs and data
***            structure definitions required by application
***            programs in order to use the HDLC device.
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>


/***************************************
**
** MQX compatibility
*/
#define _RTCS_poolid            _pool_id
#define _RTCS_queueid           _queue_id
#define _RTCS_queue_number      _queue_number
#define RTCS_memcopy            _mem_copy
#define RTCS_memalloc           _mem_alloc_zero
#define RTCS_memfree            _mem_free

/***************************************
**
** Constants
*/
#define HDLCIO_OK              0
/* Start CR 1134 */
#define RTCS_HDLC_MESSAGE_SIZE 1502
/* End CR */

/***************************************
**
** Structures
*/

typedef struct
{
   MESSAGE_HEADER_STRUCT         HEADER;
   uint_32                       PACKET_SIZE;
} RTCS_HDLC_HEADER_STRUCT, _PTR_ RTCS_HDLC_HEADER_PTR;

typedef struct rtcs_hdlc_message
{
   RTCS_HDLC_HEADER_STRUCT HDLC_HEADER;
   uchar       DATA[RTCS_HDLC_MESSAGE_SIZE];
} RTCS_HDLC_MESSAGE_STRUCT, _PTR_ RTCS_HDLC_MESSAGE_STRUCT_PTR;



typedef struct {
   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_ERRORS;

   uint_32  ST_RX_RUNT;
   uint_32  ST_RX_GIANT;

} HDLCIO_STATS, _PTR_ HDLCIO_STATS_PTR;


typedef struct {
   struct _iopcb_table PCB_TABLE;
   HDLCIO_STATS        STATS;
   _RTCS_poolid        POOL_ID;
   _RTCS_queueid       QID;
   uint_16             CHANNEL_Q;
   _ppp_mutex _PTR_    MUTEX_PTR;
   _rtcs_part          PART_ID;
   void (_CODE_PTR_    UP)(pointer);
   void (_CODE_PTR_    DOWN)(pointer);
   pointer             PARAM;

} HDLCIO_STRUCT, _PTR_ HDLCIO_STRUCT_PTR;

extern _iopcb_handle _iopcb_hdlc_init(uint_16, _RTCS_poolid, _RTCS_queueid, _ppp_mutex _PTR_);
extern void          _iopcb_hdlc_rcv_unblock(pointer);
/* Start CR 1130 */
extern _iopcb_handle _iopcb_hdlcio_init(FILE_PTR);
/* End CR */

#endif
/* EOF */
