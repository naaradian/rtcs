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
*** File: ifdns.c
***
*** Comments:  This file contains the Domain Name System user interface
***            functions for RTCS.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "dns.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_gethostbyname()
* Returned Value  : DNS_RESOURCE_RECORD_STRUCT_PTR
* Comments        : Maps a host name to an IP address
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT _PTR_ DNS_gethostbyname
   (
   char _PTR_  name_ptr
   )
{   /* Body */

   HOSTENT_STRUCT _PTR_   host_ptr;

   /*
   ** First check to see if the information exists in the RTCS_host.c
   ** file.  This check is included for backwards compatibility
   ** with previous versions of RTCS
   */

   host_ptr = DNS_check_local_host_file((uchar _PTR_)name_ptr);

   if ( host_ptr == NULL ) {
      host_ptr = DNS_query_resolver_task((uchar _PTR_)name_ptr, DNS_A );
   }/* Endif */

   return( host_ptr );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_gethostbyaddr()
* Returned Value  : DNS_RESOURCE_RECORD_STRUCT_PTR
* Comments        : Maps a host name to an IP address
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT _PTR_ DNS_gethostbyaddr
   (
   char _PTR_ addr_ptr,
   uint_32    len,
   uint_32    type
   )
{   /* Body */
   HOSTENT_STRUCT _PTR_   host_ptr;

   /*
   ** This is only included to conform to how UNIX handles gethostbyaddr,
   ** internally, RTCS DNS can handle other types.
   */
   if ((type != AF_INET) || (len != sizeof(_ip_address))) {
      return NULL;
   } /* Endif */

   host_ptr = DNS_query_resolver_task((uchar _PTR_)addr_ptr, DNS_PTR );

   return( host_ptr );

} /* Endbody */


/* EOF */
