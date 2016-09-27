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
*** File: echosrv.c
***
*** Comments:  This file contains an implementation of an
***            RFC 862 Echo server.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


#define BUFFER_SIZE  1500
static char buffer[BUFFER_SIZE];


void ECHOSRV_task(pointer, pointer);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : ECHOSRV_init
* Returned Value   : error code
* Comments  :  Start the port 7 echo server.
*
*END*-----------------------------------------------------------------*/

uint_32 ECHOSRV_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, ECHOSRV_task, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : ECHO_task
* Returned Value   : none
* Comments  :  Simple Echo server.
*
*END*-----------------------------------------------------------------*/

void ECHO_task
   (
      uint_32 dummy
   )
{ /* Body */
   ECHOSRV_task(NULL, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : ECHOSRV_task
* Returned Value   : none
* Comments  :  Simple Echo server.
*
*END*-----------------------------------------------------------------*/

void ECHOSRV_task
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   sockaddr_in    laddr, raddr;
   uint_32        sock, listensock;
   uint_32        error;
   uint_32        optval, optlen;
   int_32         length;
   uint_16        rlen;

   /* Echo server services port 7 */
   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_ECHO;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   sock = socket(PF_INET, SOCK_DGRAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Listen on TCP port */
   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   error = listen(sock, 0);
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   listensock = sock;

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {

      sock = RTCS_selectall(0);

      if (sock == listensock) {

         /* Connection requested; accept it */
         rlen = sizeof(raddr);
         accept(listensock, &raddr, &rlen);

      } else {

         /* Get the socket type */
         getsockopt(sock, SOL_SOCKET, RTCS_SO_TYPE, (char_ptr)&optval, &optlen);

         if (optval == SOCK_STREAM) {

            length = recv(sock, buffer, BUFFER_SIZE, 0);
            if (length == RTCS_ERROR) {
               shutdown(sock, FLAG_CLOSE_TX);
            } else {
               send(sock, buffer, length, 0);
            } /* Endif */

         } else { /* optval == SOCK_DGRAM */

            rlen = sizeof(raddr);
            length = recvfrom(sock, buffer, BUFFER_SIZE, 0, &raddr, &rlen);
            sendto(sock, buffer, length, 0, &raddr, rlen);

         } /* Endif */

      } /* Endif */

   } /* Endfor */

} /* Endbody */


/* EOF */
