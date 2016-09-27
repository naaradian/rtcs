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
*** File: dnsinit.c
***
*** Comments:  This file contains the Domain Name System resolver
***            initialization functions for RTCS.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "dns.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_init
* Returned Value  : DNS_OK or error code
* Comments        : Initializes the DNS task
*
*END*-----------------------------------------------------------------*/

uint_32 DNS_init
   (
      void
   )

{ /* Body */
   DNS_CONTROL_STRUCT _PTR_ control_ptr;
   uint_32                  temp_socket;
   int_32                   error;

   /*
   ** Please note, if this is modified to use a value other than
   ** DNS_MAX_UDP_MESSAGE_SIZE, the recvfrom() in the DNS_Resolver_task
   ** will need to be modified as well, as they rely on this constant.
   */
   control_ptr = _mem_alloc_system_zero(sizeof(DNS_CONTROL_STRUCT)+DNS_MAX_UDP_MESSAGE_SIZE);
   if (control_ptr == NULL) {
      return DNS_UNABLE_TO_ALLOCATE_MEMORY;
   } /* Endif */

   temp_socket = socket(PF_INET, SOCK_DGRAM, 0);
   if (temp_socket == RTCS_HANDLE_ERROR) {
      _mem_free(control_ptr);
      return DNS_UNABLE_TO_OPEN_SOCKET;
   } /* Endif */

   error = RTCS_detachsock(temp_socket);
   if (error != RTCS_OK) {
      _mem_free(control_ptr);
      return error;
   } /* Endif */

   control_ptr->INT_QUERY_PARTID  = RTCS_part_create(
      sizeof(DNS_INTERNAL_QUERY_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->INT_QUERY_PARTID == 0) {
      _mem_free(control_ptr);
      return DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->ADDR_RR_PARTID  = RTCS_part_create(
      sizeof(DNS_ADDR_RECORD_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->ADDR_RR_PARTID == 0) {
      RTCS_part_destroy(control_ptr->INT_QUERY_PARTID);
      _mem_free(control_ptr);
      return DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->NAME_RR_PARTID  = RTCS_part_create(
      sizeof(DNS_NAME_RECORD_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->NAME_RR_PARTID == 0) {
      RTCS_part_destroy(control_ptr->INT_QUERY_PARTID);
      RTCS_part_destroy(control_ptr->ADDR_RR_PARTID);
      _mem_free(control_ptr);
      return DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->RR_LIST_PARTID  = RTCS_part_create(
      sizeof(DNS_RR_LIST_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->RR_LIST_PARTID == 0) {
      RTCS_part_destroy(control_ptr->NAME_RR_PARTID);
      RTCS_part_destroy(control_ptr->INT_QUERY_PARTID);
      RTCS_part_destroy(control_ptr->ADDR_RR_PARTID);
      _mem_free(control_ptr);
      return DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->CNAME_PARTID  = RTCS_part_create(
      sizeof(DNS_CNAME_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->CNAME_PARTID == 0) {
      RTCS_part_destroy(control_ptr->RR_LIST_PARTID);
      RTCS_part_destroy(control_ptr->NAME_RR_PARTID);
      RTCS_part_destroy(control_ptr->INT_QUERY_PARTID);
      RTCS_part_destroy(control_ptr->ADDR_RR_PARTID);
      _mem_free(control_ptr);
      return DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->ROOT_SLIST_PTR  = DNS_A_Root_server;
   control_ptr->LOCAL_SLIST_PTR = DNS_First_Local_server;
   control_ptr->NS_BUFFER       = NULL;
   control_ptr->RES_BUFFER      = (uchar_ptr)(control_ptr + 1);
   control_ptr->RES_SOCKET      = temp_socket;
   control_ptr->UPDATING        = FALSE;

   error = RTCS_mutex_init(&control_ptr->CACHE_MUTEX);
   if (error != RTCS_OK) {
      _mem_free(control_ptr);
      return error;
   } /* Endif */

   error = RTCS_task_create("DNS Resolver", _RTCSTASK_priority,
      _RTCSTASK_stacksize + 500 * sizeof(uint_32),
      DNS_Resolver_task, control_ptr);
   if (error) {
      _mem_free(control_ptr);
      return error;
   } /* Endif */

   return RTCS_OK;

} /* Endbody */


/* EOF */
