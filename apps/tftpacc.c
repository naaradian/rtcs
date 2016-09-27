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
*** File: tftpacc.c
***
*** Comments:  This file contains the TFTP access control function.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "tftp.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_service_request
* Returned Value  : TRUE if the requested transaction is to be
*                   allowed, FALSE if it is to be rejected
* Comments        : The default behaviour for this function is to allow
*                   read requests on all devices and reject all write
*                   requests.
*
*END*-----------------------------------------------------------------*/

boolean TFTPSRV_access
   (
      char_ptr    string_ptr,
         /* [IN] The requested device */
      uint_16     request_type
         /* [IN] The type of access requested */
   )
{ /* Body */
 //t  return (request_type == TFTPOP_RRQ);
      return TRUE;	
} /* Endbody */

/* EOF */
