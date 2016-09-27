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
*** File: soselset.c
***
*** Comments:  This file contains the RTCS_selectset() implementation.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "socket.h"
#include "tcpip.h"

#include "select.h"

void SOCK_select
   (
      SOCKSELECT_PARM_PTR  parms
   );



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_select
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      in a specified set.
*
*END*-----------------------------------------------------------------*/

uint_32 _RTCS_select
   (
      pointer     readset,
         /* [IN] set of sockets to block on */
      uint_32     read_size,
         /* [IN] size of sockset */
         
      pointer     writeset,
         /* [IN] set of sockets to block on */
      uint_32     write_size,
         /* [IN] size of sockset */
         
      uint_32     timeout
         /* [IN] specifies the maximum amount of time to wait for data */
   )
{ /* Body */
   SOCKSELECT_PARM   parms;
   uint_32           error;

   parms.sock_ptr   = readset;
   parms.sock_count = read_size;

   parms.writesock_ptr   = writeset;
   parms.writesock_count = write_size;

   parms.timeout    = timeout;
   error = RTCSCMD_issue(parms, SOCK_select);
   if (error) return RTCS_SOCKET_ERROR;

   return parms.sock;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_selectset
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      in a specified set.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_selectset
   (
      pointer     sockset,
         /* [IN] set of sockets to block on */
      uint_32     size,
         /* [IN] size of sockset */
      uint_32     timeout
         /* [IN] specifies the maximum amount of time to wait for data */
   )
{ /* Body */
   SOCKSELECT_PARM   parms;
   uint_32           error;

   parms.sock_ptr   = sockset;
   parms.sock_count = size;
   parms.timeout    = timeout;
   error = RTCSCMD_issue(parms, SOCK_selectset);
   if (error) return RTCS_SOCKET_ERROR;

   return parms.sock;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_selectset
* Parameters      :
*
*     _rtcs_taskid      owner       not used
*     uint_32 _PTR_     sock_ptr    [IN] array of socket handles
*     uint_32           sock_count  [IN] size of socket array
*     uint_32           timeout     [IN] timeout, 0=forever, -1=none
*     uint_32           sock        [OUT] socket with activity
*
* Comments  :  Wait for data or connection requests on any socket
*      owned by this task.
*
*END*-----------------------------------------------------------------*/

void SOCK_selectset
   (
      SOCKSELECT_PARM_PTR  parms
   )
{ /* Body */
   uint_32 _PTR_  socket_array;
   uint_32        i;

   /* check every socket in the supplied array */
   socket_array = parms->sock_ptr;
   for (i=0; i<parms->sock_count; i++) {

 //     if (SOCK_select_activity((SOCKET_STRUCT_PTR)socket_array[i])) {
     if (SOCK_select_activity1((SOCKET_STRUCT_PTR)socket_array[i], 0)) {
         break;
      } /* Endif */

   } /* Endfor */

   if (i < parms->sock_count) {
      parms->sock = socket_array[i];
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   } /* Endif */

   if (parms->timeout == (uint_32)-1) {
      parms->sock = 0;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   } /* Endif */

   parms->owner = 0;
   SOCK_select_block(parms);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select


* Parameters      :
*
*     _rtcs_taskid      owner            not used
*     uint_32 _PTR_     sock_ptr         [IN] array of socket handles
*     uint_32           sock_count       [IN] size of socket array
*     uint_32 _PTR_     writesock_ptr    [IN] array of write socket handles
*     uint_32           writesock_count  [IN] size of write socket array
*     uint_32           timeout          [IN] timeout, 0=forever, -1=none
*     uint_32           sock             [OUT] socket with activity
*
* Comments  :  Wait for data or connection requests on any socket
*      owned by this task.
*
*END*-----------------------------------------------------------------*/

void SOCK_select
   (
      SOCKSELECT_PARM_PTR  parms
   )
{ /* Body */
   uint_32 _PTR_  socket_array;
   int i, i2;
   uint_32 fds[RTCS_SELECT_MAX_FDS];
   int active = FALSE;
   
   // param check
   if ((parms->sock_count != 0 && parms->sock_ptr == NULL) || (parms->writesock_count != 0 && parms->writesock_ptr == NULL)) {
      RTCSCMD_complete(parms, RTCSERR_INVALID_PARAMETER);
      return;   
   }

   // read fds - check every socket in the supplied array
   if (parms->sock_count) {
      _mem_zero(fds, parms->sock_count * sizeof(uint_32));   
      for (i = 0, i2 = 0; i < parms->sock_count; i++) {
         if (SOCK_select_activity1((SOCKET_STRUCT_PTR)parms->sock_ptr[i], 0)) {
            fds[i2++] = parms->sock_ptr[i];
         }
      }
   
      // update socket array - only active sockets
      if (i2) {
         _mem_zero(parms->sock_ptr, parms->sock_count * sizeof(uint_32));
         _mem_copy(fds, parms->sock_ptr, i2 * sizeof(uint_32));
         parms->sock_count = i2;
         parms->sock = fds[0];

         active = TRUE;
      }
   }
   
   // write fds - check every socket in the supplied array
   if (parms->writesock_count) {
      _mem_zero(fds, parms->writesock_count * sizeof(uint_32));
      for (i = 0, i2 = 0; i < parms->writesock_count; i++) {
         if (SOCK_select_activity1((SOCKET_STRUCT_PTR)parms->writesock_ptr[i], 1)) {
            fds[i2++] = parms->writesock_ptr[i];
         }
      }

      // update socket array - only active sockets
      if (i2) {
         _mem_zero(parms->writesock_ptr, parms->writesock_count * sizeof(uint_32));
         _mem_copy(fds, parms->writesock_ptr, i2 * sizeof(uint_32));
         parms->writesock_count = i2;
      
         if (active == FALSE) {
            // write fds active but read fds no - set read fds to zero
            _mem_zero(parms->sock_ptr, parms->sock_count * sizeof(uint_32));
            parms->sock_count = 0;
            parms->sock = fds[0];
         }
      
         active = TRUE;
      }
      else if (active == TRUE) {
         // read fds active but write fds no - set write fds to zero
         _mem_zero(parms->writesock_ptr, parms->writesock_count * sizeof(uint_32));
         parms->writesock_count = 0;
      }
   }

   if (active == TRUE) {
      // some fds active
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   }

   if (parms->timeout == (uint_32)-1) {
      parms->sock = 0;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   } /* Endif */

   parms->owner = 0;
   SOCK_select_block(parms);

} /* Endbody */







/* EOF */
