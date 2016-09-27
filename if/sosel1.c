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
*** File: sosel1.c
***
*** Comments:  This file contains the RTCS select() support functions.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "socket.h"
#include "tcpip.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_unblock
* Returned Values :
* Comments  :
*     Dequeues a select() call from the select queue.
*
*END*-----------------------------------------------------------------*/

void SOCK_select_unblock
   (
      SOCKSELECT_PARM_PTR  parms
   )
{ /* Body */

   if (parms->NEXT) {
      parms->NEXT->PREV = parms->PREV;
   } /* Endif */
   *parms->PREV = parms->NEXT;

   if (parms->timeout) {
      TCPIP_Event_cancel(&parms->EXPIRE);
   } /* Endif */

   RTCSCMD_complete(parms, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_signal
* Returned Values :
* Comments  :
*     Unblocks all select() calls blocked on a specific socket.
*
*END*-----------------------------------------------------------------*/

void SOCK_select_signal
   (
      uint_32  sock
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   SOCKSELECT_PARM_PTR        parms, nextparms;
   uint_32                    i;

   if (!sock) {
      return;
   } /* Endif */

   for (parms=socket_cfg_ptr->SELECT_HEAD; parms; parms=nextparms) {

      nextparms = parms->NEXT;
      if (parms->owner) {
         if (SOCK_Is_owner((SOCKET_STRUCT_PTR)sock, parms->owner)) {
            parms->sock = sock;
            SOCK_select_unblock(parms);
         } /* Endif */
      } else {
         for (i=0; i<parms->sock_count; i++) {
            if (parms->sock_ptr[i] == sock) {
               parms->sock = sock;
               SOCK_select_unblock(parms);
               break;
            } /* Endif */
         } /* Endfor */
      } /* Endif */

   } /* Endfor */

   /*
   ** Finally, call the application callback, if one exists.
   */
   if (((SOCKET_STRUCT_PTR)sock)->CALLBACK) {
      ((SOCKET_STRUCT_PTR)sock)->CALLBACK(sock);
   } /* Endif */

} /* Endbody */


/* EOF */
