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
*** File: ppp273.c
***
*** Comments:  This file contains the 2.73 function calls with their
***            2.74 equivalents.
***
************************************************************************
*END*******************************************************************/

#include <ppp.h>
#include "ppp_prv.h"

_ppp_handle _PPP273_handle;

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_Initialize
* Returned Value  :  error code
* Comments        :
*     Initializes PPP for a channel.
*
*END*-----------------------------------------------------------------*/

int_32 PPP_Initialize
   (
      _ppp_channel   channel,
            /* [IN] - PPP channel number */
      FILE_PTR       device,
            /* [IN] - I/O stream to use */
      PPP_CALL_PTR   call
            /* [IN] - call table */
   )
{ /* Body */
   _iopcb_handle     ioppp;
   void (_CODE_PTR_  callfn)();
   pointer           callparam;
   int_32            error;

   ioppp = _iopcb_ppphdlc_init(device);
   error = PPP_initialize(ioppp, &_PPP273_handle);
   if (!error) {
      callfn    = call->PPP_Link_up;
      callparam = (pointer)call->PARAM;
      PPP_setcall(_PPP273_handle, PPP_CALL_LINK_UP, &callfn, &callparam);
      callfn    = call->PPP_Link_down;
      callparam = (pointer)call->PARAM;
      PPP_setcall(_PPP273_handle, PPP_CALL_LINK_DOWN, &callfn, &callparam);
      callfn    = (void(_CODE_PTR_)())call->PPP_Echo_reply;
      callparam = (pointer)call->PARAM;
      PPP_setcall(_PPP273_handle, PPP_CALL_ECHO_REPLY, &callfn, &callparam);
      callfn    = call->PPP_Auth_fail;
      callparam = (pointer)call->PARAM;
      PPP_setcall(_PPP273_handle, PPP_CALL_AUTH_FAIL, &callfn, &callparam);
      _iopcb_open(ioppp, PPP_lowerup, PPP_lowerdown, _PPP273_handle);
   } /* Endif */
   return error;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_Setcall
* Returned Value  :  error code
* Comments        :
*     Sets an application callback function.
*
*END*-----------------------------------------------------------------*/

int_32 PPP_Setcall
   (
      _ppp_channel        channel,
            /* [IN] - the PPP state structure */
      uint_32             callnum,
            /* [IN] - PPP callback */
      void (_CODE_PTR_    callback)(),
            /* [IN] - callback function */
      uint_32             callparam
            /* [IN] - callback parameter */
   )
{ /* Body */
   return PPP_setcall(_PPP273_handle, callnum, &callback, (pointer _PTR_)&callparam);
} /* Endbody */


/* EOF */
