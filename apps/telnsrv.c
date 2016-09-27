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
*** File: telnsrv.c
***
*** Comments:  This file contains the Telnet server implementation.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "telnet.h"


struct telnetsrv_init {
   RTCS_TASK_PTR  shell;
   uint_32        sock;
};


void TELNETSRV_task  (pointer, pointer);
void TELNETSRV_child (pointer, pointer);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TELNETSRV_init
* Returned Value   : error code
* Comments  :  Start the Telnet server.
*
*END*-----------------------------------------------------------------*/

uint_32 TELNETSRV_init
   (
      char_ptr       name,
      uint_32        priority,
      uint_32        stacksize,
      RTCS_TASK_PTR  shell
   )
{ /* Body */
   struct telnetsrv_init init;

   init.shell = shell;
   init.sock  = RTCS_SOCKET_ERROR;
   return RTCS_task_create(name, priority, stacksize, TELNETSRV_task, &init);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TELNETSRV_sockinit
* Returned Value   : error code
* Comments  :  Start the Telnet server with an existing socket.
*
*END*-----------------------------------------------------------------*/

uint_32 TELNETSRV_sockinit
   (
      char_ptr       name,
      uint_32        priority,
      uint_32        stacksize,
      RTCS_TASK_PTR  shell,
      uint_32        sock
   )
{ /* Body */
   struct telnetsrv_init init;

   init.shell = shell;
   init.sock  = sock;
   return RTCS_task_create(name, priority, stacksize, TELNETSRV_task, &init);

} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : TELNET_server
* Returned Value   : none
* Comments  :  A TCP/IP based Telnet Server
*
*END*-----------------------------------------------------------------*/

void TELNET_server
   (
      uint_32 template_index
   )
{ /* Body */
   struct telnetsrv_init      init;
   RTCS_TASK                  shell;
   TASK_TEMPLATE_STRUCT_PTR   template_ptr;

   template_ptr = _mqx_get_initialization()->TASK_TEMPLATE_LIST;
   while (template_ptr->TASK_TEMPLATE_INDEX &&
          (template_ptr->TASK_TEMPLATE_INDEX != template_index)) {
      template_ptr++;
   } /* Endwhile */

   shell.NAME      = template_ptr->TASK_NAME;
   shell.PRIORITY  = template_ptr->TASK_PRIORITY;
   shell.STACKSIZE = template_ptr->TASK_STACKSIZE;
   shell.START     = (void(_CODE_PTR_)(pointer))template_ptr->TASK_ADDRESS;
   shell.ARG       = (pointer)template_ptr->CREATION_PARAMETER;

   init.shell = &shell;
   init.sock  = RTCS_SOCKET_ERROR;

   TELNETSRV_task(&init, NULL);

} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : TELNETSRV_task
* Returned Value   : none
* Comments  :  A TCP/IP based Telnet Server
*
* If sock<0, *sockp is set to a newly created TCP socket, and it
* is connected to raddr.
*
*END*-----------------------------------------------------------------*/

void TELNETSRV_task
   (
      pointer  init_ptr,
      pointer  creator
   )
{ /* Body */
   struct telnetsrv_init init;
   RTCS_TASK             shell = *((struct telnetsrv_init _PTR_)init_ptr)->shell;
   uint_32               sock = ((struct telnetsrv_init _PTR_)init_ptr)->sock;
   sockaddr_in           addr;
   uint_32               error;

   /* Install device drivers for socket and telnet I/O */
   _io_socket_install("socket:");
   _io_telnet_install("telnet:");

   /* Allocate a socket if the caller didn't provide one */
   if (sock == RTCS_SOCKET_ERROR) {
      sock = socket(PF_INET, SOCK_STREAM, 0);
      if (sock == RTCS_SOCKET_ERROR) {
         RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
      } /* Endif */
   } /* Endif */

   /* Bind the socket to the Telnet port */
   addr.sin_family      = AF_INET;
   addr.sin_port        = IPPORT_TELNET;
   addr.sin_addr.s_addr = INADDR_ANY;

   error = bind(sock, &addr, sizeof(addr));
   if (error != RTCS_OK) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Put the socket into listening mode */
   error = listen(sock, 0);
   if (error != RTCS_OK) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   RTCS_task_resume_creator(creator, RTCS_OK);

   init.shell = &shell;
   while (TRUE) {

      /* Wait for a connection */
      init.sock = accept(sock, NULL, NULL);
      if (init.sock == RTCS_SOCKET_ERROR) {
         continue;
      } /*Endif*/

      /* Create a task to look after it */
      RTCS_detachsock(init.sock);
      RTCS_task_create(shell.NAME,
                       shell.PRIORITY,
                       shell.STACKSIZE,
                       TELNETSRV_child, &init);

   } /* Endwhile */

} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Task Name      : TELNETSRV_child
* Returned Value : void
* Comments       : Telnet child task.
*
*END*-----------------------------------------------------------------*/

void TELNETSRV_child
   (
      pointer  init_ptr,
      pointer  creator
   )
{ /* Body */
   struct telnetsrv_init _PTR_ init = init_ptr;
   uint_32                     sock = init->sock;
   _mqx_uint                   echoflag = IO_SERIAL_ECHO;
   void (_CODE_PTR_            start)(pointer) = init->shell->START;
   pointer                     arg = init->shell->ARG;

   FILE_PTR sockfd, telnetfd;

   sockfd = fopen("socket:", (char_ptr)sock);
   if (sockfd == NULL) {
      RTCS_task_exit(creator, RTCSERR_FOPEN_FAILED);
   } /* Endif */

   telnetfd = fopen("telnet:", (char_ptr)sockfd);
   if (telnetfd == NULL) {
      RTCS_task_exit(creator, RTCSERR_FOPEN_FAILED);
   } /* Endif */

   ioctl(telnetfd, IO_IOCTL_SERIAL_SET_FLAGS, &echoflag);

   _io_set_handle(IO_STDIN, telnetfd);
   _io_set_handle(IO_STDOUT, telnetfd);

   RTCS_task_resume_creator(creator, RTCS_OK);

   /*
   ** Send a greeting message to the user
   */
   printf("\nPrecise/RTCS v%d.%02d.%02d Telnet server\n",
          RTCS_VERSION_MAJOR, RTCS_VERSION_MINOR, RTCS_VERSION_REV);

   /*
   ** Call user application function
   */
   start(arg);

   printf("\nGoodbye\n");

   /*
   ** Allow some time for queued data to go out.
   */
   RTCS_time_delay(100);

   /*
   ** Child task is done
   */
   fclose(telnetfd);
   fclose(sockfd);
   shutdown(sock, FLAG_CLOSE_TX);
} /* Endbody */


/* EOF */
