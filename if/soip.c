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
*** File: soip.c
***
*** Comments:  This file contains the implementation of getsockopt()
***            and setsockopt() at the SOL_IP level.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_IP_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_IP_ ## f, a); \
                        return a


uint_32 SOL_IP_getsockopt  (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
uint_32 SOL_IP_setsockopt  (uint_32, uint_32, uint_32, pointer, uint_32);

const RTCS_SOCKOPT_CALL_STRUCT SOL_IP_CALL = {
   SOL_IP_getsockopt,
   SOL_IP_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_IP_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_IP_getsockopt
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
   uint_32                       error;

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

      case RTCS_SO_IP_RX_DEST:
         if (*optlen < sizeof(_ip_address)) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         *(_ip_address _PTR_)optval = socket_ptr->LINK_OPTIONS.RX.DEST;
         *optlen = sizeof(_ip_address);
         break;

      case RTCS_SO_IP_RX_TTL:
         if (!*optlen) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         *(uchar_ptr)optval = socket_ptr->LINK_OPTIONS.RX.TTL;
         *optlen = sizeof(uchar);
         break;

      case RTCS_SO_IP_RX_TOS:
         if (!*optlen) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         *(uchar_ptr)optval = socket_ptr->LINK_OPTIONS.RX.TOS;
         *optlen = sizeof(uchar);
         break;

       default:
         error = RTCSERR_SOCK_INVALID_OPTION;
      } /* Endswitch */

   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_IP_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_IP_setsockopt
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
   uint_32     error;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   error = RTCSERR_SOCK_INVALID_OPTION;

   RTCS_EXIT(SETSOCKOPT, error);
} /* Endbody */

/* EOF */
