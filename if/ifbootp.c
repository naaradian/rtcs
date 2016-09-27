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
*** File: ifbootp.c
***
*** Comments:  This file contains the interface functions for the
***            BOOTP client.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "bootp.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind_BOOTP
* Returned Values : uint_32 (error code)
* Comments        :
*     Initialize a hardware interface using BOOTP.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_bind_BOOTP
   (
      _rtcs_if_handle         handle,
         /* [IN] the RTCS interface state structure */
      BOOTP_DATA_STRUCT_PTR   data_ptr
         /* [IN/OUT] BOOTP parameters */
   )
{ /* Body */
   TCPIP_PARM_BOOTP        parms;

   parms.handle = handle;
   parms.data   = data_ptr;

   return RTCSCMD_issue(parms, BOOTP_open);

} /* Endbody */


/* EOF */
