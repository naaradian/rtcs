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
*** File: sockutil.c
***
*** Comments:  This file contains utility functions for managing
***            socket structures.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "tcp_prv.h"
#include "socket.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Get_sock_struct
* Returned Value  : none
* Comments        : Get a socket structure, initialize it and return
*                     address of socket.
*
*END*-----------------------------------------------------------------*/

SOCKET_STRUCT_PTR   SOCK_Get_sock_struct
   (
      RTCS_SOCKET_CALL_STRUCT_PTR   type,
      _rtcs_taskid                  owner

   )
{ /* Body */
   RTCS_DATA_PTR              RTCS_data_ptr;
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr;
   SOCKET_STRUCT_PTR          socket_ptr;

   RTCS_data_ptr = RTCS_get_data();
   socket_cfg_ptr = RTCS_data_ptr->SOCKET_CFG;

   socket_ptr = RTCS_part_alloc_zero(RTCS_data_ptr->RTCS_socket_partition);

   if ( socket_ptr != NULL ) {

      RTCS_mutex_lock(&socket_cfg_ptr->SOCK_MUTEX);
      socket_cfg_ptr->CURRENT_SOCKETS++;

      SOCK_Add_owner(socket_ptr, owner);

      /*
      ** link in this socket into the linked list of active sockets
      */
      if ( socket_cfg_ptr->SOCKET_HEAD != NULL ) {

         ((SOCKET_STRUCT_PTR)socket_cfg_ptr->SOCKET_TAIL)->NEXT =
            (pointer)socket_ptr;
         socket_ptr->PREV = (SOCKET_STRUCT_PTR)socket_cfg_ptr->SOCKET_TAIL;

      } else {
         socket_cfg_ptr->SOCKET_HEAD = (pointer)socket_ptr;
         socket_ptr->PREV = NULL;

      } /* Endif */

      socket_ptr->NEXT = NULL;
      socket_cfg_ptr->SOCKET_TAIL = (pointer)socket_ptr;
      RTCS_mutex_unlock(&socket_cfg_ptr->SOCK_MUTEX);

      socket_ptr->VALID = SOCKET_VALID;

      /*
      ** set the default socket options
      */
      socket_ptr->CONNECT_TIMEOUT  = DEFAULT_CONNECT_TIMEOUT;
      socket_ptr->RETRANSMISSION_TIMEOUT = DEFAULT_RETRANSMISSION_TIMEOUT;
      socket_ptr->SEND_TIMEOUT     = DEFAULT_SEND_TIMEOUT;
      socket_ptr->RECEIVE_TIMEOUT  = DEFAULT_RECEIVE_TIMEOUT;
      socket_ptr->RECEIVE_PUSH     = DEFAULT_PUSH;
      socket_ptr->SEND_NOWAIT      = DEFAULT_NOWAIT;
      socket_ptr->SEND_WAIT        = DEFAULT_WAIT;
      socket_ptr->SEND_PUSH        = DEFAULT_PUSH;
      socket_ptr->RECEIVE_NOWAIT   = DEFAULT_NOWAIT;
      socket_ptr->CHECKSUM_BYPASS  = DEFAULT_CHECKSUM_BYPASS;
      socket_ptr->ACTIVE           = DEFAULT_ACTIVE;
      socket_ptr->TBSIZE           = DEFAULT_TBSIZE;
      socket_ptr->RBSIZE           = DEFAULT_RBSIZE;
      socket_ptr->MAXRTO           = DEFAULT_MAXRTO;
      socket_ptr->MAXRCV_WND       = DEFAULT_MAXRCV_WND;
      socket_ptr->KEEPALIVE        = DEFAULT_KEEPALIVE;
      socket_ptr->NOWAIT           = DEFAULT_NOWAIT;
      socket_ptr->NO_NAGLE_ALGORITHM  = DEFAULT_NO_NAGLE_ALGORITHM;
      socket_ptr->NOSWRBUF         = DEFAULT_NOSWRBUF;
      socket_ptr->CALL_BACK        = DEFAULT_CALL_BACK;
      socket_ptr->TYPE_MIRROR      = (uint_32)type;
      socket_ptr->CALLBACK         = DEFAULT_CALLBACK;
      socket_ptr->TIMEWAIT_TIMEOUT = DEFAULT_TIMEWAIT_TIMEOUT;

      socket_ptr->PROTOCOL     = type;

   } /* Endif */

   return(socket_ptr);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Free_sock_struct
* Returned Value  : none
* Comments        : Delink and release a socket structure.
*
*END*-----------------------------------------------------------------*/

void SOCK_Free_sock_struct
   (
      SOCKET_STRUCT_PTR          socket_ptr
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   SOCKET_OWNER_STRUCT_PTR    owner_ptr, free_ptr;

   socket_ptr->VALID = 0;

   owner_ptr = socket_ptr->OWNERS.NEXT;
   while (owner_ptr != NULL) {
      free_ptr = owner_ptr;
      owner_ptr = owner_ptr->NEXT;
      _mem_free(free_ptr);
   } /* Endwhile */

   RTCS_mutex_lock(&socket_cfg_ptr->SOCK_MUTEX);

   /*
   ** Fix up the head/next pointer of our predecessor.
   */
   if ( socket_ptr->PREV == NULL ) {
      socket_cfg_ptr->SOCKET_HEAD = socket_ptr->NEXT;
   } else {
      (socket_ptr->PREV)->NEXT = socket_ptr->NEXT;
   } /* Endif */

   /*
   ** Fix up the tail/prev pointer of our successor.
   */
   if ( socket_ptr->NEXT == NULL ) {
      socket_cfg_ptr->SOCKET_TAIL = socket_ptr->PREV;
   } else {
      (socket_ptr->NEXT)->PREV = socket_ptr->PREV;
   } /* Endif */

   /*
   ** Release the socket structure memory.
   */
   RTCS_part_free(socket_ptr);

   socket_cfg_ptr->CURRENT_SOCKETS--;
   RTCS_mutex_unlock(&socket_cfg_ptr->SOCK_MUTEX);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Remove_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

boolean SOCK_Remove_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
   SOCKET_OWNER_STRUCT_PTR owner_ptr;
   uint_32                 i;

   owner_ptr = &socket_ptr->OWNERS;

   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            owner_ptr->TASK[i] = NULL;
            return TRUE;
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */
   return FALSE;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Is_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

boolean SOCK_Is_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
   SOCKET_OWNER_STRUCT_PTR owner_ptr;
   uint_32                 i;

   owner_ptr = &socket_ptr->OWNERS;


   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            /* already here, just return */
            return TRUE;
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */
   return FALSE;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Add_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

boolean SOCK_Add_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
   SOCKET_OWNER_STRUCT_PTR owner_ptr, new_owner_ptr;
   uint_32                 i;
   pointer _PTR_           saved_ptr = NULL;

   owner_ptr = &socket_ptr->OWNERS;

   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            /* already here, just return */
            return TRUE;
         } else if ((owner_ptr->TASK[i] == 0) && (saved_ptr == NULL)) {
            saved_ptr = &owner_ptr->TASK[i];
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */

   if (saved_ptr != NULL) {
      *saved_ptr = task_ptr;
   } else {
      new_owner_ptr = _mem_alloc_zero(sizeof(SOCKET_OWNER_STRUCT));
      if (new_owner_ptr == NULL) {
         return FALSE;
      } /* Endif */

      new_owner_ptr->TASK[0] = task_ptr;
      owner_ptr->NEXT = new_owner_ptr;
   } /* Endif */
   return TRUE;
} /* Endbody */


/* EOF */
