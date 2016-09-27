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
*** File: bootcomm.c
***
*** Comments:  This file contains the implementation of the
***            functions common to BOOTP and DHCP clients.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"


extern uint_32          BOOT_count;
extern UCB_STRUCT_PTR   BOOT_port;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_open
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Opens the BOOTP/DHCP client port.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOT_open
   (
      void (_CODE_PTR_ service)(RTCSPCB_PTR, UCB_STRUCT_PTR)
   )
{ /* Body */
   uint_32 error;

   if (BOOT_count == 0) {
      error = UDP_openbind_internal(IPPORT_BOOTPC, service, &BOOT_port);
      if (error) {
         return error;
      } /* Endif */
   } /* Endif */

   BOOT_count++;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_close
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Closes the BOOTP/DHCP client port.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOT_close
   (
      void
   )
{ /* Body */
   uint_32 error = RTCS_OK;

   BOOT_count--;

   if (BOOT_count == 0) {
      error = UDP_close_internal(BOOT_port);
      BOOT_port = NULL;
   } /* Endif */

   return error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_send
*  Returned Value : void
*  Comments       :
*        Sends a BOOTP/DHCP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOT_send
   (
      RTCSPCB_PTR    pcb_ptr,          /* [IN]     outgoing packet  */
      pointer        if_ptr            /* [IN]     target interface */
   )
{ /* Body */

   return UDP_send_IF(BOOT_port, if_ptr, IPPORT_BOOTPS, pcb_ptr);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_service
*  Returned Value : void
*  Comments       :
*        Services a BOOTP/DHCP packet.
*
*END*-----------------------------------------------------------------*/

void BOOT_service
   (
      RTCSPCB_PTR    pcb_ptr,          /* [IN/OUT] incoming packet  */
      UCB_STRUCT_PTR ucb_ptr           /* [IN]     target UCB       */
   )
{ /* Body */
   IP_IF_PTR if_ptr = (IP_IF_PTR)pcb_ptr->IFSRC;

   if (if_ptr->BOOTFN) {
      (*if_ptr->BOOTFN)(pcb_ptr);
   } else {
      RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
      RTCSPCB_free(pcb_ptr);
   } /* Endif */

} /* Endbody */


/* EOF */
