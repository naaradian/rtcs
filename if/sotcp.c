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
*** File: sotcp.c
***
*** Comments:  This file contains the implementation of getsockopt()
***            and setsockopt() at the SOL_TCP level.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_TCP_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_TCP_ ## f, a); \
                        return a


uint_32 SOL_TCP_getsockopt (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
uint_32 SOL_TCP_setsockopt (uint_32, uint_32, uint_32, pointer, uint_32);

const RTCS_SOCKOPT_CALL_STRUCT SOL_TCP_CALL = {
   SOL_TCP_getsockopt,
   SOL_TCP_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_TCP_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_TCP_getsockopt
   (
      uint_32        sock,
         /* [IN] socket handle */
      uint_32        level,
         /* [IN] protocol level for the option */
      uint_32        optname,
         /* [IN] name of the option */
      pointer        optval,
         /* [IN] buffer for the current value of the option */
      uint_32  _PTR_ optlen
         /* [IN/OUT] length of the option value, in bytes */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   *(uint_32_ptr)optval = ((uint_32_ptr)&socket_ptr->CONNECT_TIMEOUT)[optname-1];
   *optlen = sizeof(uint_32);

   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_TCP_setsockopt
* Returned Value  : error code
* Comments  :  Routine is used to modify the current socket options.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_TCP_setsockopt
   (
      uint_32        sock,
         /* [IN] socket handle */
      uint_32        level,
         /* [IN] protocol level for the option */
      uint_32        optname,
         /* [IN] name of the option */
      pointer        optval,
         /* [IN] new value for the option */
      uint_32        optlen
         /* [IN] length of the option value, in bytes */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   ((uint_32_ptr)&socket_ptr->CONNECT_TIMEOUT)[optname-1] = *(uint_32_ptr)optval;

   RTCS_EXIT(SETSOCKOPT, RTCS_OK);

} /* Endbody */


/* EOF */
