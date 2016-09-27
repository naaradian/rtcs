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
*** File: bootinit.c
***
*** Comments:  This file contains the initialization of the
***            BOOTP and DHCP clients.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"


uint_32          BOOT_count;
UCB_STRUCT_PTR   BOOT_port;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_init
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the BOOTP and DHCP clients.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOT_init
   (
      void
   )
{ /* Body */

   BOOT_count = 0;
   BOOT_port = NULL;
   return RTCS_OK;

} /* Endbody */


/* EOF */
