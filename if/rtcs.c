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
*** File: rtcs.c
***
*** Comments:  This file contains the interface functions to the
***            RTCS Communication Library.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "socket.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_socket
* Returned Value  : socket handle
* Comments  :  Routine to open a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_socket
   (
      uint_32     pf,
         /*[IN] specifies the protocol family */
      uint_32     type,
         /*[IN] specifies the type of communication */
      uint_32     protocol
         /*[IN] select a specific protocol */
   )
{ /* Body */
   SOCKET_STRUCT_PTR socket_ptr;
   uint_32           error;

   RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_SOCKET, type);

   socket_ptr = SOCK_Get_sock_struct((RTCS_SOCKET_CALL_STRUCT_PTR)type,
                                     RTCS_task_getid());
   if (socket_ptr == NULL) {
      RTCSLOG_API(RTCSLOG_TYPE_FNEXIT, RTCSLOG_FN_SOCKET, RTCS_SOCKET_ERROR);
      return RTCS_SOCKET_ERROR;
   } /* Endif */

   if (socket_ptr->PROTOCOL->SOCK_SOCKET) {
      error = socket_ptr->PROTOCOL->SOCK_SOCKET((uint_32)socket_ptr);
      if (error) {
         SOCK_Free_sock_struct(socket_ptr);
         RTCSLOG_API(RTCSLOG_TYPE_FNEXIT, RTCSLOG_FN_SOCKET, RTCS_SOCKET_ERROR);
         return RTCS_SOCKET_ERROR;
      } /* Endif */
   } /* Endif */

   RTCSLOG_API(RTCSLOG_TYPE_FNEXIT, RTCSLOG_FN_SOCKET, (uint_32)socket_ptr);
   return (uint_32)socket_ptr;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_shutdown
* Returned Value  : OK or error code
* Comments  :  Routine to close a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_shutdown
   (
      uint_32     sock,
         /*[IN] socket handle for this socket */
      uint_32     how
         /*[IN] specifies whether or not connection should be immediate */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;
            uint_32           error;

   error = RTCS_OK;

#if RTCSCFG_CHECK_VALIDITY
   if (socket_ptr->VALID != SOCKET_VALID) {
      return RTCSERR_SOCK_INVALID;
   } /* Endif */
#endif

   if (socket_ptr->PROTOCOL->SOCK_SHUTDOWN) {
      error = socket_ptr->PROTOCOL->SOCK_SHUTDOWN(sock, how);
   } /* Endif */

   SOCK_Free_sock_struct(socket_ptr);
   return error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_attachsock
* Returned Value  : socket handle or RTCS_ERROR
* Comments  :  Routine to take ownership of a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_attachsock
   (
      uint_32     sock
         /*[IN] specifies the handle of the existing socket */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;

   if (!SOCK_Add_owner(socket_ptr, RTCS_task_getid())) {
      RTCS_setsockerror(sock, RTCSERR_OUT_OF_MEMORY);
      return RTCS_SOCKET_ERROR;
   } /* Endif */

   return sock;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_detachsock
* Returned Value  : error code
* Comments  :  Routine to relinquish ownership of a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_detachsock
   (
      uint_32     sock
         /*[IN] specifies the handle of the existing socket */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;

   if (!SOCK_Remove_owner(socket_ptr, RTCS_task_getid())) {
      return RTCSERR_SOCK_NOT_OWNER;
   } /* Endif */

   return RTCS_OK;

} /*EndBody*/


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_geterror
* Returned Value  : error code
* Comments  :  Routine is used to get the last error code associated
*              with a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_geterror
   (
      uint_32     sock
         /*[IN] socket handle for this socket */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;

   /*
   ** Verify the socket
   */
   if (socket_ptr->VALID != SOCKET_VALID) {
      return RTCSERR_SOCK_INVALID;
   } /* Endif */

   return socket_ptr->ERROR_CODE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_setsockerror
* Returned Value  : error code
* Comments  :  Routine is used to set the error code on a socket.
*              Used internally by RTCS.
*
*END*-----------------------------------------------------------------*/

void RTCS_setsockerror
   (
      uint_32     sock,
         /* [IN] socket handle for this socket */
      uint_32     error
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;

   socket_ptr->ERROR_CODE = error;

} /* Endbody */


/* EOF */
