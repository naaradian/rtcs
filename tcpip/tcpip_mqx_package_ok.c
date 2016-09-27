/*HEADER****************************************************************
************************************************************************
***
*** Copyright (c) Precise Software Technologies Incorporated
*** All rights reserved
***
*** This software embodies materials and concepts which are
*** confidential to Precise Software Technologies Inc. and is made
*** available solely pursuant to the terms of a written license
*** agreement with Precise Software Technologies Inc.
***
*** File: tcpip.c
***
*** Comments:  This is the main task for the TCP/IP protocol suite.
***
*** Revision History:
***   Date        Version  Changes
***   ----        -------  -------
***   Oct  6/92   1.0      Initial version
***   Nov  3/93   2.10     Use named buffers
***   Mar 10/97   2.40     Support for BOOTP
***   Mar 10/97   2.60     Medium Independance and support for tcpGETNAME
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>

#include "rtcs.h"
#include "rtcs_prv.h"
#include "ticker.h"
#include "tcpip.h"
#include "arp.h"


uint_32 (_CODE_PTR_ TCP_tick)(void);

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCPIP_fake_tick
* Returned Value  : 0
* Comments        :
*
*  The fake TCP_Tick_server(), used when TCP is not installed.
*
*END*-----------------------------------------------------------------*/

uint_32 TCPIP_fake_tick
   (
      void
   )
{ /* Body */
   return 0;
} /* Endbody */

/*TASK*------------------------------------------------------------------
*
* Task Name      :   TCPIP_task
* Comments       :
*     This task services requests from the socket layer and the
*     application, and services incoming packets from the link layer(s).
*
*END*------------------------------------------------------------------*/

void TCPIP_task
   (
      pointer  dummy,
      pointer  creator
   )
{ /* Body */
   TCPIP_CFG_STRUCT           TCPIP_cfg;
   RTCS_DATA_PTR              RTCS_data_ptr;
   uint_32  (_CODE_PTR_ _PTR_ prot_init)(void);
   TCPIP_MESSAGE_PTR          tcpip_msg;
   uint_32                    timeout, timebefore, timeafter, timedelta;
   uint_32                    status;           /* Return status */
   _queue_id                  tcpip_qid;

   RTCS_data_ptr = RTCS_get_data();
   RTCS_setcfg(TCPIP, &TCPIP_cfg);

   TCPIP_cfg.status = RTCS_OK;

   tcpip_qid = RTCS_msgq_open(TCPIP_QUEUE, 0);
   if (tcpip_qid == 0) {
      RTCS_task_exit(creator, RTCSERR_OPEN_QUEUE_FAILED);
   } /* Endif */

   RTCS_data_ptr->TCPIP_TASKID = RTCS_task_getid();

   /*
   ** Initialize the Time Service
   */
   TCP_tick = TCPIP_fake_tick;
   TCPIP_Event_init();
   timebefore = RTCS_get_milliseconds();

   /*
   ** Allocate a block of PCBs
   */
   status = RTCSPCB_init();
   if (status != RTCS_OK) {
      RTCS_task_exit(creator, status);
   } /* Endif */

   /*
   ** Initialize the protocols
   */

   status = IP_init();
   if (status) {
      RTCS_task_exit(creator, status);
   } /* Endif */

   status = ICMP_init();
   if (status) {
      RTCS_task_exit(creator, status);
   } /* Endif */

   for (prot_init = RTCS_protocol_table; *prot_init; prot_init++) {
      status = (*prot_init)();
      if (status) {
         RTCS_task_exit(creator, status);
      } /* Endif */
   } /* Endfor */

   ARP_init();
   BOOT_init();

   RTCS_task_resume_creator(creator, RTCS_OK);

   while (1) {
      /*
      ** Update any option that can be changed
      */
      RTCS_data_ptr->IP_FORWARD_COPY =    _IP_forward;
      RTCS_data_ptr->TCP_BYPASS_RX_COPY = _TCP_bypass_rx;
      RTCS_data_ptr->TCP_BYPASS_TX_COPY = _TCP_bypass_tx;
      RTCS_data_ptr->TCP_RTO_MIN_COPY =   _TCP_rto_min;

      /*
      ** Process TCP events
      */
      timeout = TCP_tick();

      /*
      ** Set up the timeout to hit at exactly the time of the next timeout.
      */
      timeafter = RTCS_get_milliseconds();
      timedelta = timeafter - timebefore;
      timebefore = timeafter;
      timedelta = TCPIP_Event_time(timedelta);

      if (timedelta != 0) {
         if ((timedelta < timeout) || (timeout == 0)) {
            timeout = timedelta;
         } /* Endif */
      } /* Endif */

      tcpip_msg = (TCPIP_MESSAGE_PTR)RTCS_msgq_receive(tcpip_qid, timeout,
         RTCS_data_ptr->TCPIP_msg_pool);

      /* SPR P122-0344-01 */
      /* Update TCP's time */
      TCP_tick();
      /* End SPR P122-0344-01 */

      timeafter = RTCS_get_milliseconds();
      timedelta = timeafter - timebefore;
      timebefore = timeafter;
      TCPIP_Event_time(timedelta);

      if (tcpip_msg) {
         tcpip_msg->COMMAND(tcpip_msg->DATA);
         RTCS_msg_free(tcpip_msg);
      } /* Endif */

   } /* Endwhile */

} /* Endbody */


/* EOF */
