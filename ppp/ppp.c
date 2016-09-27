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
*** File: ppp.c
***
*** Comments:  This file contains the implementation of the PPP
***            initialization routines.
***
************************************************************************
*END*******************************************************************/

#include <ppp.h>
#include "ppp_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_initialize
* Returned Value  :  error code
* Comments        :
*     Initializes PPP for a handle.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_initialize
   (
      _iopcb_handle       device,
            /* [IN] - I/O stream to use */
      _ppp_handle _PTR_   handle
            /* [OUT] - the PPP state structure */
   )
{ /* Body */
   PPP_CFG_PTR          ppp_ptr;
   uint_32              i, error;

   /* Allocate the state structure */
   ppp_ptr = PPP_memalloc(sizeof(PPP_CFG));
   if (!ppp_ptr) {
      return PPPERR_ALLOC_FAILED;
   } /* Endif */

   /* Initialize it */
   PPP_memzero(ppp_ptr, sizeof(PPP_CFG));
   ppp_ptr->LINK_STATE = FALSE;
   ppp_ptr->DEVICE = device;
   for (i = 0; i < PPP_CALL_MAX; i++) {
      ppp_ptr->LCP_CALL[i].CALLBACK = NULL;
      ppp_ptr->LCP_CALL[i].PARAM    = NULL;
   } /* Endfor */
   ppp_ptr->PROT_CALLS = NULL;
   ppp_ptr->RECV_OPTIONS = &PPP_DEFAULT_OPTIONS;
   ppp_ptr->SEND_OPTIONS = &PPP_DEFAULT_OPTIONS;

   /* Initialize the lwsem */
   if (PPP_mutex_init(&ppp_ptr->MUTEX)) {
      return PPPERR_INIT_MUTEX_FAILED;
   } /* Endif */

   /* Initialize LCP */
   error = LCP_init(ppp_ptr);
   if (error) {
      PPP_mutex_destroy(&ppp_ptr->MUTEX);
      return error;
   } /* Endif */

   /* Initialize CCP */
   error = CCP_init(ppp_ptr);
   if (error) {
      LCP_destroy(ppp_ptr);
      PPP_mutex_destroy(&ppp_ptr->MUTEX);
      return error;
   } /* Endif */
   CCP_open(ppp_ptr);

   /* Create a pool of message buffers */
   ppp_ptr->MSG_POOL = RTCS_msgpool_create(sizeof(PPP_MESSAGE),
      PPP_MESSAGE_INITCOUNT, PPP_MESSAGE_GROWTH, PPP_MESSAGE_LIMIT);
   if (ppp_ptr->MSG_POOL == MSGPOOL_NULL_POOL_ID) {
      CCP_destroy(ppp_ptr);
      LCP_destroy(ppp_ptr);
      PPP_mutex_destroy(&ppp_ptr->MUTEX);
      return PPPERR_CREATE_PKT_POOL_FAILED;
   } /* Endif */

   /* Create the Tx Task */
   error = RTCS_task_create("PPP tx", _PPPTASK_priority,
      _PPPTASK_stacksize + 1000,
      PPP_tx_task, ppp_ptr);
   if (error) {
      RTCS_msgpool_destroy(ppp_ptr->MSG_POOL);
      CCP_destroy(ppp_ptr);
      LCP_destroy(ppp_ptr);
      PPP_mutex_destroy(&ppp_ptr->MUTEX);
      return error;
   } /* Endif */

   /* Create the Rx Task */
   error = RTCS_task_create("PPP rx", _PPPTASK_priority,
      _PPPTASK_stacksize + 1000,
      PPP_rx_task, ppp_ptr);
   if (error) {
      RTCS_msgpool_destroy(ppp_ptr->MSG_POOL);
      CCP_destroy(ppp_ptr);
      LCP_destroy(ppp_ptr);
      PPP_mutex_destroy(&ppp_ptr->MUTEX);
      return error;
   } /* Endif */

   /* Return the handle */
   ppp_ptr->VALID = PPP_VALID;
   *handle = ppp_ptr;
   return PPP_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_shutdown
* Returned Value  :  error code
* Comments        :
*     Destroys a PPP handle and frees all resources.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_shutdown
   (
      _ppp_handle  handle
       /* [IN] - the PPP state structure */
   )
{ /* Body */
   PPP_CFG_PTR    ppp_ptr = handle;
   _rtcs_sem      sem;

   PPP_lowerdown(ppp_ptr);
   /* Kill Rx task */
   RTCS_task_destroy(ppp_ptr->RX_TASKID);
   /* Kill Tx task */
   RTCS_sem_init(&sem);
   PPP_send_shutdown(ppp_ptr, &sem);
   RTCS_sem_wait(&sem);
   RTCS_sem_destroy(&sem);
   RTCS_msgpool_destroy(ppp_ptr->MSG_POOL);
   CCP_destroy(ppp_ptr);
   LCP_destroy(ppp_ptr);
   PPP_mutex_destroy(&ppp_ptr->MUTEX);
   PPP_memfree(handle);
   return PPP_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_pcballoc
* Returned Value  :  pointer to allocated PCB
* Comments        :
*     Called by PPPFSM_send* and {P,CH}AP_send.  Dynamically
*     allocates a PCB.
*
*END*-----------------------------------------------------------------*/

PCB_PTR PPP_pcballoc
   (
      uint_16 protocol,
            /* [IN] - PPP protocol */
      uint_32 size
            /* [IN] - max size of packet, excluding protocol */
   )
{ /* Body */
   PCB_FRAGMENT _PTR_ pcb_frag_ptr;
   PCB_PTR packet;

   packet = _mem_alloc_system(sizeof(PCB) + sizeof(PCB_FRAGMENT) + 2 + size);
   if (packet) {
      packet->FREE = (void (_CODE_PTR_)(PCB_PTR))_mem_free;
      /* Start SPR P122-0266-22 remove all warnings from RTCS code. */
      /*  packet->FRAG[0].LENGTH   = size + 2; */
      /*  packet->FRAG[0].FRAGMENT = (uchar_ptr)packet + sizeof(PCB) + sizeof(PCB_FRAGMENT); */
      /*  packet->FRAG[1].LENGTH   = 0; */
      /*  packet->FRAG[1].FRAGMENT = NULL; */
      /*  htons(packet->FRAG[0].FRAGMENT, protocol); */
      pcb_frag_ptr = packet->FRAG;
      pcb_frag_ptr->LENGTH   = size + 2;
      pcb_frag_ptr->FRAGMENT = (uchar_ptr)packet + sizeof(PCB) +
         sizeof(PCB_FRAGMENT);
      htons(pcb_frag_ptr->FRAGMENT, protocol);
      pcb_frag_ptr++;
      pcb_frag_ptr->LENGTH   = 0;
      pcb_frag_ptr->FRAGMENT = NULL;
      /* End SPR P122-0266-22 */
   } /* Endif */

   return packet;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_getcall
* Returned Value  :  error code
* Comments        :
*     Retrieves an application callback function.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_getcall
   (
      _ppp_handle             handle,
            /* [IN] - the PPP state structure */
      uint_32                 callnum,
            /* [IN] - PPP callback */
      void (_CODE_PTR_ _PTR_  callback)(),
            /* [OUT] - callback function */
      pointer          _PTR_  callparam
            /* [OUT] - callback parameter */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = handle;
   PPP_CALL_INTERNAL_PTR call_ptr;

      /* Do some error checking */
  if (ppp_ptr->VALID != PPP_VALID) {
      return PPPERR_INVALID_HANDLE;
   } else if (callnum >= PPP_CALL_MAX) {
      return PPPERR_INVALID_CALLBACK;
   } /* Endif */

   call_ptr   = &ppp_ptr->LCP_CALL[callnum];
   *callback  = call_ptr->CALLBACK;
   *callparam = call_ptr->PARAM;
   return PPP_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_setcall
* Returned Value  :  error code
* Comments        :
*     Sets an application callback function.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_setcall
   (
      _ppp_handle             handle,
            /* [IN] - the PPP state structure */
      uint_32                 callnum,
            /* [IN] - PPP callback */
      void (_CODE_PTR_ _PTR_  callback)(),
            /* [IN/OUT] - callback function */
      pointer          _PTR_  callparam
            /* [IN/OUT] - callback parameter */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = handle;
   PPP_CALL_INTERNAL_PTR call_ptr;
   void (_CODE_PTR_ oldfn)();
   pointer oldparam;

      /* Do some error checking */
   if (ppp_ptr->VALID != PPP_VALID) {
      return PPPERR_INVALID_HANDLE;
   } else if (callnum >= PPP_CALL_MAX) {
      return PPPERR_INVALID_CALLBACK;
   } /* Endif */

   call_ptr = &ppp_ptr->LCP_CALL[callnum];
   oldfn    = call_ptr->CALLBACK;
   oldparam = call_ptr->PARAM;
   call_ptr->CALLBACK = *callback;
   call_ptr->PARAM    = *callparam;
   *callback  = oldfn;
   *callparam = oldparam;
   return PPP_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_getmtu
* Returned Value  :  MTU
* Comments        :
*     Retrieves the negotiated MTU (i.e. the peer's MRU)
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_getmtu
   (
      _ppp_handle       handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = handle;
   return ppp_ptr->SEND_OPTIONS->MRU;
} /* Endbody */


/* EOF */
