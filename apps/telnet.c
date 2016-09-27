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
*** File: telnet.c
***
*** Comments:  This file contains the Telnet client implementation.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "telnet.h"


/*TASK*-----------------------------------------------------------------
*
* Function Name  : TELNET_connect
* Returned Value : error code
* Comments       : A TCP/IP based Telnet Client
*
*END------------------------------------------------------------------*/

uint_32 TELNET_connect
   (
      _ip_address    ipaddress
   )
{ /* Body */
   FILE_PTR       sockfd, telnetfd;
   sockaddr_in    addr;
   uint_32        sock;
   uint_32        error;
   boolean        work;
   int_32         c;

   /*
   ** Install device driver for socket and telnet
   */
   _io_socket_install("socket:");
   _io_telnet_install("telnet:");

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      return RTCSERR_OUT_OF_SOCKETS;
   } /* Endif */

   addr.sin_family      = AF_INET;
   addr.sin_port        = 0;
   addr.sin_addr.s_addr = INADDR_ANY;
   error = bind(sock, &addr, sizeof(addr));
   if (error != RTCS_OK) {
      return error;
   } /* Endif */

   addr.sin_port        = IPPORT_TELNET;
   addr.sin_addr.s_addr = ipaddress;

   error = connect(sock, &addr, sizeof(addr));
   if (error != RTCS_OK) {
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return error;
   } /* Endif */

   sockfd = fopen("socket:", (char_ptr)sock);
   if (sockfd == NULL) {
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return RTCSERR_FOPEN_FAILED;
   } /* Endif */

   telnetfd = fopen("telnet:", (char_ptr)sockfd);
   if (telnetfd == NULL) {
      fclose(sockfd);
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return RTCSERR_FOPEN_FAILED;
   } /* Endif */

   /* Set the console stream to the client  */
   /* Start SPR P122-0266-09 remove all warnings from RTCS. */
   /* ioctl(telnetfd, IO_IOCTL_SET_STREAM, (uint_32_ptr)stdin); */
   ioctl(telnetfd, IO_IOCTL_SET_STREAM, (uint_32_ptr)((void _PTR_)stdin));
   /* End SPR P122-0266-09 */
   while (TRUE) {

      work = FALSE;
      if (fstatus(stdin)) {
         work = TRUE;
         c = (int_32)fgetc(stdin);
         fputc(c & 0x7F, telnetfd);
      } /* Endif */

      if (fstatus(telnetfd)) {
         work = TRUE;
         c = (int_32)fgetc(telnetfd);
         if (c == IO_EOF) {
            break;
         }/* Endif */
         fputc(c & 0x7F, stdout);
      } /* Endif */

      /* Let another task run if there is no I/O */
      if (!work) {
         RTCS_time_delay(1);
      } /* Endif */

   } /* Endwhile */

   fclose(telnetfd);
   fclose(sockfd);
   shutdown(sock, FLAG_CLOSE_TX);

   return RTCS_OK;

} /* Endbody */

/* EOF */
