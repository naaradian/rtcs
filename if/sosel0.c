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
*** File: sosel0.c
***
*** Comments:  This file contains the RTCS select() support functions.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "socket.h"
#include "tcpip.h"
#include "tcp_prv.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_activity1
* Returned Value  : TRUE or FALSE
* Comments        : Determine whether there is any activity on a socket.
*
*END*-----------------------------------------------------------------*/

boolean SOCK_select_activity1
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      int flag                          // direction flag - 0 recv, non zero send
   )
{ /* Body */
   TCB_STRUCT_PTR tcb_ptr;
   boolean        activity = FALSE;

   /* Check closing and connected stream sockets for data */
   if (socket_ptr->TCB_PTR) {
      tcb_ptr = socket_ptr->TCB_PTR;

#ifdef HTTP_PRINT
  printfpd("\n\r %X", (unsigned long)socket_ptr );
#endif
   
      if (!flag) {
         /*
         ** Check that there is data in the receive ring or
         ** that the socket has been closed by the peer
         */
         if ((tcb_ptr->conn_pending) || (tcb_ptr->state == CLOSED) ||
            /* START CR 2349 */
            //(tcb_ptr->rcvnxt > tcb_ptr->rcvbufseq)) {
            (GT32(tcb_ptr->rcvnxt, tcb_ptr->rcvbufseq))) {
            /* END CR 2349 */
            activity = TRUE;

#ifdef HTTP_PRINT
  printfp("recv activ" );
#endif
         }
      }
      else {
         if (!tcb_ptr->sndlen) {    // TODO - remake for partialy empty send buffers
            activity = TRUE;
#ifdef HTTP_PRINT
  printfp("trnsm activ" );
#endif
         }
      }

   /* Check datagram sockets for data */
   } else if (socket_ptr->UCB_PTR) {
      /*
      ** Check that there is queued data
      */
      if (socket_ptr->UCB_PTR->PHEAD) {
         activity = TRUE;
      } /* Endif */

   /* TCB=UCB=NULL is a TCP connection reset by peer */
   } else {

      activity = TRUE;

   } /* Endif */

   return activity;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_activity
* Returned Value  : TRUE or FALSE
* Comments        : Determine whether there is any activity on a socket.
*
*END*-----------------------------------------------------------------*/

boolean SOCK_select_activity
   (
      SOCKET_STRUCT_PTR    socket_ptr
   )
{ /* Body */
   TCB_STRUCT_PTR tcb_ptr;
   boolean        activity = FALSE;

   /* Check closing and connected stream sockets for data */
   if (socket_ptr->TCB_PTR) {
      tcb_ptr = socket_ptr->TCB_PTR;
      /*
      ** Check that there is data in the receive ring or
      ** that the socket has been closed by the peer
      */
      if ((tcb_ptr->conn_pending) ||
          (tcb_ptr->state == CLOSED) ||
          (tcb_ptr->rcvnxt > tcb_ptr->rcvbufseq)) {
         activity = TRUE;
      } /* Endif */

   /* Check datagram sockets for data */
   } else if (socket_ptr->UCB_PTR) {
      /*
      ** Check that there is queued data
      */
      if (socket_ptr->UCB_PTR->PHEAD) {
         activity = TRUE;
      } /* Endif */

   /* TCB=UCB=NULL is a TCP connection reset by peer */
   } else {

      activity = TRUE;

   } /* Endif */

   return activity;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_block
* Returned Values :
* Comments  :
*     Enqueues a select() call on the select queue.
*
*END*-----------------------------------------------------------------*/

void SOCK_select_block
   (
      SOCKSELECT_PARM_PTR  parms
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);

   parms->NEXT = socket_cfg_ptr->SELECT_HEAD;
   if (parms->NEXT) {
      parms->NEXT->PREV = &parms->NEXT;
   } /* Endif */
   socket_cfg_ptr->SELECT_HEAD = parms;
   parms->PREV = &socket_cfg_ptr->SELECT_HEAD;

   if (parms->timeout) {
      parms->EXPIRE.TIME    = parms->timeout;
      parms->EXPIRE.EVENT   = SOCK_select_expire;
      parms->EXPIRE.PRIVATE = parms;
      TCPIP_Event_add(&parms->EXPIRE);
   } /* Endif */
//printfpd("\n\rb> %X", (unsigned)socket_cfg_ptr->SELECT_HEAD); ///

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_expire
* Returned Values :
* Comments  :
*     Called by the Timer.  Expire a select call.
*
*END*-----------------------------------------------------------------*/

boolean SOCK_select_expire
   (
      TCPIP_EVENT_PTR   event
   )
{ /* Body */
   SOCKSELECT_PARM_PTR  parms = event->PRIVATE;

   if (parms->NEXT) {
      parms->NEXT->PREV = parms->PREV;
   } /* Endif */
   *parms->PREV = parms->NEXT;

   parms->sock = 0;
   RTCSCMD_complete(parms, RTCS_OK);

   return FALSE;
} /* Endbody */


/* EOF */
