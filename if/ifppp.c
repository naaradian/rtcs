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
*** File: ifppp.c
***
*** Comments:  This file contains the interface functions for the
***            IPCP client.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ipcp.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind_IPCP
* Returned Values : uint_32 (error code)
* Comments        :
*     Initialize a PPP interface.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_bind_IPCP
   (
      _rtcs_if_handle         handle,
         /* [IN] the RTCS interface state structure */
      IPCP_DATA_STRUCT_PTR    data_ptr
         /* [IN/OUT] IPCP parameters */
   )
{ /* Body */
   TCPIP_PARM_IPCP         parms;

   parms.handle = handle;
   parms.data   = data_ptr;

   return RTCSCMD_issue(parms, IPCP_bind);

} /* Endbody */


/* EOF */
