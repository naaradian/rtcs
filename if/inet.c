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
*** File: inet.c
***
*** Comments:  This file contains some Internet address manipulation
***            functions.
***
************************************************************************
*END*******************************************************************/

#include <ctype.h>
#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : inet_aton
* Returned Value  : -1 on success, 0 on error
* Comments  :  Converts a dotted-decimal into a binary in_addr
*
*END*-----------------------------------------------------------------*/

int_32 inet_aton
   (
      const char _PTR_  name,
         /* [IN] dotted decimal IP address */
      in_addr _PTR_     ipaddr_ptr
         /* [OUT] binary IP address */
   )
{ /* Body */
   boolean     ipok = FALSE;
   uint_32     dots;
   uint_32     byte;
   _ip_address addr;

   addr = 0;
   dots = 0;
   for (;;) {

      if (!isdigit(*name)) break;

      byte = 0;
      while (isdigit(*name)) {
         byte *= 10;
         byte += *name - '0';
         if (byte > 255) break;
         name++;
      } /* Endwhile */
      if (byte > 255) break;
      addr <<= 8;
      addr += byte;

      if (*name == '.') {
         dots++;
         if (dots > 3) break;
         name++;
         continue;
      } /* Endif */

      if ((*name == '\0') && (dots == 3)) {
         ipok = TRUE;
      } /* Endif */

      break;

   } /* Endfor */

   if (!ipok) {
      return 0;
   } /* Endif */

   if (ipaddr_ptr) {
      ipaddr_ptr->s_addr = addr;
   } /* Endif */

   return -1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : inet_addr
* Returned Value  : a binary IP address, or -1 on error
* Comments  :  Converts a dotted-decimal into a binary in_addr
*
*END*-----------------------------------------------------------------*/

_ip_address inet_addr
   (
      const char _PTR_  name
         /* [IN] dotted decimal IP address */
   )
{ /* Body */
   in_addr  ipaddr;
   int_32   ipok;

   ipok = inet_aton(name, &ipaddr);
   if (!ipok) {
      return INADDR_BROADCAST;
   } /* Endif */
   return ipaddr.s_addr;

} /* Endbody */


/* EOF */
