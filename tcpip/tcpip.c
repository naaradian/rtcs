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

#define CLEAR_ETH_COUNTER

extern "C" void wd_reset(void);	
extern "C" unsigned long GetTestMode(void);


extern unsigned long tcpipmsgcnt;

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


 void my_int_enable(void); //my
 void my_int_disable(void); //my

extern "C" void my_int_disable_irq0(void);
extern "C" void my_int_enable_irq0(void);
extern unsigned long my_IP_OK;
extern "C" void MaskIrq0();
extern "C" void UnMaskIrq0();
unsigned long tcp_ip_debug_cnt;



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
//____________________________________________________________my
/* RTCS Protocol Table */ // hier locally
uint_32 (_CODE_PTR_ RTCS_protocol_table[])(void) = 
   {
   RTCSPROT_IGMP,
   RTCSPROT_UDP,
   RTCSPROT_TCP,
   NULL
   };

tcp_ip_debug_cnt = 0;

//____________________________________________________________my





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
   RDN_init();
   BOOT_init();

   RTCS_task_resume_creator(creator, RTCS_OK);

// while (1) { //t090327
//LedOff();
//LedOn();
//}

   while (1) {
 //MaskIrq0();
tcp_ip_debug_cnt++;
// wd_reset(); //my 121129
#ifdef CLEAR_ETH_COUNTER
my_IP_OK = 0;
#endif //CLEAR_ETH_COUNTER
  /*
   if(GetTestMode() == 0x8e){
     SetTestMode(1l);
   _task_block();
} 
  */ 

//  _int_enable(); //090325
   
 // LedOff();
  //LedOn();
     //my_int_disable(); //my
//  my_int_disable_irq0();
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
//  _int_enable(); //090325
      tcpip_msg = (TCPIP_MESSAGE_PTR)RTCS_msgq_receive(tcpip_qid, timeout,
         RTCS_data_ptr->TCPIP_msg_pool);
 // ok LedOn(); //t090327
      /* SPR P122-0344-01 */
      /* Update TCP's time */
      TCP_tick();

// ok LedOn(); //t
      /* End SPR P122-0344-01 */

      timeafter = RTCS_get_milliseconds();
      timedelta = timeafter - timebefore;
      timebefore = timeafter;
      TCPIP_Event_time(timedelta);


 // _int_enable(); //090325
      if (tcpip_msg) {
tcpipmsgcnt++;
 //ok LedOn(); //t
// _int_enable(); //090330
        tcpip_msg->COMMAND(tcpip_msg->DATA);
//LedOn(); //t
         RTCS_msg_free(tcpip_msg);
//bad LedOn(); //t
      } /* Endif */
//bad LedOn(); //t090327
    //my_int_enable(); //my
//	_time_delay_ticks((_mqx_uint)1); //090325
 //  _int_enable(); //090326
 //   LedOn(); //090326
// my_int_enable_irq0();
 //UnMaskIrq0();
   } /* Endwhile */

} /* Endbody */

//________________________________________________________________
/*TASK*------------------------------------------------------------------
*
* Task Name      :   TCPIP_task2
* Comments       :
*     This task services requests from the socket layer and the
*     application, and services incoming packets from the link layer(s).
*
*END*------------------------------------------------------------------*/

//unsigned long tcp_ip_debug_cnt;

void TCPIP_task2
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
//____________________________________________________________my
/* RTCS Protocol Table */ // hier locally
uint_32 (_CODE_PTR_ RTCS_protocol_table[])(void) = 
   {
   RTCSPROT_IGMP,
   RTCSPROT_UDP,
   RTCSPROT_TCP,
   NULL
   };



//____________________________________________________________my

  
  
   tcp_ip_debug_cnt = 0;

   RTCS_data_ptr = RTCS_get_data();
   RTCS_setcfg(TCPIP, &TCPIP_cfg);

   TCPIP_cfg.status = RTCS_OK;

   tcpip_qid = RTCS_msgq_open(TCPIP_QUEUE, 0); // it is cleared before by msg_cleanup()
   if (tcpip_qid == 0) {
      RTCS_task_exit(creator, RTCSERR_OPEN_QUEUE_FAILED);
   } /* Endif */

   RTCS_data_ptr->TCPIP_TASKID = RTCS_task_getid();

   /*
   ** Initialize the Time Service
   */
   TCP_tick = TCPIP_fake_tick;
   TCPIP_Event_init(); //do not loss memory
   timebefore = RTCS_get_milliseconds();

   /*
   ** Allocate a block of PCBs
   */
   status = RTCSPCB_init(); //?????????????????loss memory???
   if (status != RTCS_OK) {
      RTCS_task_exit(creator, status);
   } /* Endif */

   /*
   ** Initialize the protocols
   */

  //101220  status = IP_init();
 status = IP_init1();
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
   RDN_init();
   BOOT_init();

   RTCS_task_resume_creator(creator, RTCS_OK);

// while (1) { //t090327
//LedOff();
//LedOn();
//}

   while (1) {

tcp_ip_debug_cnt++;
#ifdef CLEAR_ETH_COUNTER
my_IP_OK = 0;
#endif //CLEAR_ETH_COUNTER
//  _int_enable(); //090325
 // LedOff();
  //LedOn();
     //my_int_disable(); //my
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
//  _int_enable(); //090325
      tcpip_msg = (TCPIP_MESSAGE_PTR)RTCS_msgq_receive(tcpip_qid, timeout,
         RTCS_data_ptr->TCPIP_msg_pool);
 // ok LedOn(); //t090327
      /* SPR P122-0344-01 */
      /* Update TCP's time */
      TCP_tick();

// ok LedOn(); //t
      /* End SPR P122-0344-01 */

      timeafter = RTCS_get_milliseconds();
      timedelta = timeafter - timebefore;
      timebefore = timeafter;
      TCPIP_Event_time(timedelta);


 // _int_enable(); //090325
      if (tcpip_msg) {
tcpipmsgcnt++;
 //ok LedOn(); //t
// _int_enable(); //090330
        tcpip_msg->COMMAND(tcpip_msg->DATA);
//LedOn(); //t
         RTCS_msg_free(tcpip_msg);
//bad LedOn(); //t
      } /* Endif */
//bad LedOn(); //t090327
    //my_int_enable(); //my
//	_time_delay_ticks((_mqx_uint)1); //090325
 //  _int_enable(); //090326
 //   LedOn(); //090326
   } /* Endwhile */

} /* Endbody */



//_________________________________________________________________



/*TASK*------------------------------------------------------------------
*
* Task Name      :   TCPIP_task1
* Comments       :
*     This task services requests from the socket layer and the
*     application, and services incoming packets from the link layer(s).
*
*END*------------------------------------------------------------------*/

void TCPIP_task1
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
//____________________________________________________________my
/* RTCS Protocol Table */ // hier locally
uint_32 (_CODE_PTR_ RTCS_protocol_table[])(void) = 
   {
   RTCSPROT_IGMP,
   RTCSPROT_UDP,
   RTCSPROT_TCP,
   NULL
   };



//____________________________________________________________my





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
   RDN_init();
   BOOT_init();

   RTCS_task_resume_creator(creator, RTCS_OK);

// while (1) { //t090327
//LedOff();
//LedOn();
//}

   while (1) {
#ifdef CLEAR_ETH_COUNTER
my_IP_OK = 0;
#endif //CLEAR_ETH_COUNTER
//  _int_enable(); //090325
 // LedOff();
  //LedOn();
     //my_int_disable(); //my
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
//  _int_enable(); //090325
      tcpip_msg = (TCPIP_MESSAGE_PTR)RTCS_msgq_receive(tcpip_qid, timeout,
         RTCS_data_ptr->TCPIP_msg_pool);
 // ok LedOn(); //t090327
      /* SPR P122-0344-01 */
      /* Update TCP's time */
      TCP_tick();

// ok LedOn(); //t
      /* End SPR P122-0344-01 */

      timeafter = RTCS_get_milliseconds();
      timedelta = timeafter - timebefore;
      timebefore = timeafter;
      TCPIP_Event_time(timedelta);


 // _int_enable(); //090325
      if (tcpip_msg) {
 //ok LedOn(); //t
// _int_enable(); //090330
        tcpip_msg->COMMAND(tcpip_msg->DATA);
//LedOn(); //t
         RTCS_msg_free(tcpip_msg);
//bad LedOn(); //t
      } /* Endif */
//bad LedOn(); //t090327
    //my_int_enable(); //my
//	_time_delay_ticks((_mqx_uint)1); //090325
 //  _int_enable(); //090326
 //   LedOn(); //090326
// _time_delay_ticks((_mqx_uint)1);

   } /* Endwhile */

} /* Endbody */


/* EOF */
