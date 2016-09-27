/*HEADER****************************************************************
************************************************************************
***
*** Copyright (c) 1992-2004 ARC International
*** All rights reserved
***
*** This software embodies materials and concepts which are
*** confidential to ARC International and is made available solely
*** pursuant to the terms of a written license agreement with
*** ARC International.
***
*** File: ftpsrv.c
***
*** Comments:  This file contains an implementation of an
***            FTP server.
***
************************************************************************
*END*******************************************************************/

#include <ctype.h>
#include <string.h>
#include <rtcs.h>
#include "ftp.h"

#define DEBUGM(x)

/* Response messages */
static char banner[]    = "220 %s FTP Ready\r\n";
static char badcmd[]    = "500 Unknown command\r\n";
static char logged[]    = "230 Logged in\r\n";
static char typeok[]    = "200 Type OK\r\n";
static char badtype[]   = "501 Unknown type \"%s\"\r\n";
static char badport[]   = "501 Bad port syntax\r\n";
static char unimp[]     = "502 Command not yet implemented\r\n";
static char bye[]       = "221 Goodbye!\r\n";
static char cantopen[]  = "550 Can't open file \"%s\"\r\n";
static char sending[]   = "150 Opening data connection for %s (%s)\r\n";
static char portok[]    = "200 Port command okay\r\n";
static char rxok[]      = "226 File received OK - %ld bytes in %ld.%03ld sec\r\n";
static char txok[]      = "226 File sent OK - %ld bytes in %ld.%03ld sec\r\n";

static char noconn[]    = "425 Data connection refused\r\n";
static char writefail[] = "426 write error\r\n";
static char writeeof[]  = "426 device full\r\n";

static char myname[BUF_SIZE];
static char message[BUF_SIZE];
static char message2[BUF_SIZE];

static char command[LINELEN];
static char username[LINELEN];

static char filename[LINELEN];
static FILE_PTR file;

static char transbuffer[BUF_SIZE+1];
static char transbuffer1;

static uint_32     command_index;
sockaddr_in ftp_port;
static sockaddr_in remote_addr;
static uint_16     remote_addr_len;
static uint_32     bytes_moved;

/* Start CR 1888 */
/*
** This variable is set by the function FTPSRV_callback().
*/
/* extern uint_32	   buffer_size;*/
/* End CR */

/* Command table */
static char _PTR_ commands[] = {
   "user",
   "acct",
   "pass",
   "type",
   "name",
   "quit",
   "port",
   "retr",
   "stor",
   "help",
   "abor",
   NULL
};

void FTPSRV_task(pointer, pointer);


#undef FTPSRV_DO_CALLBACK  /* read note below */

#ifdef FTPSRV_DO_CALLBACK
/* Start CR 1888 */
/*
** This variable is set by the function FTPSRV_callback().
*/
extern uint_32	   buffer_size;
/* End CR */
extern boolean FTPSRV_callback(uint_32, char _PTR_); 
#define FTPSRV_CALLBACK( connection, command) FTPSRV_callback( connection, command )
#else
#define FTPSRV_CALLBACK( connection, command)   FALSE
#endif


/*********************************
***  N O T E:
***    about FTPSRV_CALLBACK( connection, command)):
***
***  If you want the FTP server to handle more commands or to handle currently
***  handled commands differently, then you can overide the RTCS FTP server 
***  command handling by providing 
***  boolean FTPSRV_callback(uint_32 connection, char _PTR_ command);
*** 
***  If you define FTPSRV_DO_CALLBACK above, and provide FTPSRV_callback() in
***  your application, then the RTCS FTP server will call your FTPSRV_callback
***  routine for FTP command handling. You should return a TRUE if you handled
***  the command, and a FALSE if you did not handle the command.
***
***  Here is a template function for you to copy and paste:
*************************************************************/


#if 0 /* this is only a templeate made available for copy and paste.
       *  LEAVE the #if 0 in this file
       */
#include<ctype.h>
#include<string.h>
#include<rtcs.h>
#include<mqx.h>
#include<ftp.h>
/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_callback
* Returned Value   : TRUE= I handled this command
*                  : FALSE= I did not handle this command
* Comments         :
*    This function parses and processes the command.
*
*    The RTCS FPT server calls this function for command handling before
*    its default command handling. For this to work be sure that 
*    #define FTPSRV_DO_CALLBACK is set in rtcs/source/apps/ftpsrv.c
*
*END*-----------------------------------------------------------------*/
boolean FTPSRV_callback(uint_32 connection, char _PTR_ command)
{
   switch (command)
   {
   /* these commands are already handeled by RTCS FTP server. We don't need to 
    * re-do the handling here 
    */
      case USER_CMD:
      case ACCT_CMD:
      case PASS_CMD:
      case TYPE_CMD:
      case NAME_CMD:
      case QUIT_CMD:
      case PORT_CMD:
      case RETR_CMD:
      case STOR_CMD:
      case HELP_CMD:
      case ABORT_CMD:
        return (FALSE);  

      /* these commands are not handled by RTCS, We need to parse
       * them and handle them here. 
       */
      case CWD_CMD:
      case CDUP_CMD:
      case LIST_CMD:
      case DELE_CMD:
      case MKD_CMD:
      case XMKD_CMD:
      case PWD_CMD:
      case XPWD_CMD:
      case RMD_CMD:
      case XRMD_CMD:
         return (TRUE);     

      default :
      return(FALSE);
   }
   return (FALSE);

}
#endif /* end of copy and paste function template for FTPSRV_callback */

/**********************************/






/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_init
* Returned Value   : error code
* Comments  :  Start the FTP server.
*
*END*-----------------------------------------------------------------*/

uint_32 FTPSRV_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, FTPSRV_task, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : FTPSRV_task
* Returned Value   : none
* Comments  :  FTP server.
*
*END*-----------------------------------------------------------------*/

void FTPSRV_task
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   sockaddr_in    laddr;
   uint_32        sock, listensock;
   uint_32        error;

   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_FTP;
   laddr.sin_addr.s_addr = INADDR_ANY;

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

   message[0] = '\0';
   message2[0] = '\0';

   sprintf(myname, "Precise/RTCS");

   for (;;) {

      command_index = 0;
      /* Connection requested; accept it */
      remote_addr_len = sizeof(remote_addr);
      sock = accept(listensock, &remote_addr, &remote_addr_len);

      sprintf(message2, "%ld.%ld.%ld.%ld",
              (remote_addr.sin_addr.s_addr >> 24) & 0xFF,
              (remote_addr.sin_addr.s_addr >> 16) & 0xFF,
              (remote_addr.sin_addr.s_addr >>  8) & 0xFF,
               remote_addr.sin_addr.s_addr        & 0xFF);
      sprintf(message,"%s,%hd", message2, remote_addr.sin_port);

      /* set default data port */
      ftp_port.sin_family      = remote_addr.sin_family;
      ftp_port.sin_port        = remote_addr.sin_port;
      ftp_port.sin_addr.s_addr = remote_addr.sin_addr.s_addr;

      FTPSRV_process_connection(listensock, sock);

      shutdown(sock, FLAG_CLOSE_TX);

   } /* Endfor */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_process_connection
* Returned Value   : none
* Comments         :
*    This function processes the new connection
*
*END*-----------------------------------------------------------------*/

void FTPSRV_process_connection
   (
      uint_32 socket,
      uint_32 connection
   )
{ /* Body */

   command_index = 0;
   sprintf(message, banner, myname);

   FTPSRV_tputs(connection, message);
   while ( FTPSRV_r_ftp(connection) ) { /* Connection not dropped */
   } /* Endwhile */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_r_ftp
* Returned Value   : boolean connection NOT dropped
* Comments         :
*    This function assembles a command string, then calls the
* command interpretter
*
*END*-----------------------------------------------------------------*/

boolean FTPSRV_r_ftp
   (
      uint_32 connection
   )
{ /* Body */
   int_32  length;
   uchar   cmd_buffer[LINELEN];
   int_32  index;
   uchar   c;

   _mem_zero(cmd_buffer, LINELEN);

   /* Assemble an input line in the session buffer. Return if incomplete */
   DEBUGM(printf("RX Wait:\n");)
   length = recv(connection, cmd_buffer, LINELEN, 0);
   if ( length == RTCS_ERROR  ) {
      return FALSE;
   } /* Endif */
   DEBUGM(printf("Got message length %ld msg: %s\n", length, cmd_buffer);)

   index = 0;
   while ( length-- ) {
      c = cmd_buffer[index++];
      switch ( c ) {
         case '\r':   /* Strip cr's */
         continue;

         case '\n':   /* Complete line; process it */
            command[command_index] = '\0';
            DEBUGM(printf("Rx:%s\n", cmd_buffer);)
            if (!FTPSRV_CALLBACK(connection,command)) {
               if ( ! FTPSRV_docommand( connection, command) ) {
                  return FALSE;
               } /* Endif */
            } /* Endif */
            command_index = 0;
         break;

         default:   /* Assemble line */
            if ( command_index != (LINELEN-1) ) {
               command[command_index++] = c;
            } /* Endif */
         break;

      } /* End Switch */
   } /* Endwhile */
   return TRUE; /* Keep going */

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_docommand
* Returned Value   : boolean all ok
* Comments         :
*    This function parses and processes the command
*
*END*-----------------------------------------------------------------*/

boolean FTPSRV_docommand
   (
      uint_32     connection,
      char _PTR_  command
   )
{ /* Body */
   sockaddr_in              temp_addr;
   uint_32                  start_time;
   uint_32                  end_time;
   char               _PTR_ arg;
   char               _PTR_ cp;
   char         _PTR_ _PTR_ cmdp;
   char_ptr                 mode, cmdstr, donestr;
   uint_32                  data_connection;
   boolean                  error;
   uint_32                  error_code;

   DEBUGM(printf("Executing FTP command <%s>\n",command);)

   if ( command == 0 ) {
      /* Can't be a legal FTP command */
      FTPSRV_tputs(connection, badcmd);
      return TRUE;   /* continue despite invalid command */
   } /* Endif */

   /* Translate entire buffer to lower case NO!! JUST COMMAND */

   for ( cp = command; *cp != '\0'; cp++ ) {
      if ( *cp == ' ' ) {
         break;
      } /* Endif */
      if ( (*cp >= 'A') && (*cp <= 'Z') ) {
         *cp = tolower(*cp);
      } /* Endif */
   } /* Endfor */

   DEBUGM(printf("Looking for <%s>\n", command);)
   /* Find command in table; if not present, return syntax error */
   for ( cmdp = commands; *cmdp != NULL; cmdp++ ) {
      if ( strncmp(*cmdp,command,strlen(*cmdp)) == 0 ) {
         break;
      } /* Endif */
   } /* Endfor */

   if ( *cmdp == NULL ) {
      FTPSRV_tputs(connection, badcmd);
      return TRUE; /* continue despite invalid command */
   } /* Endif */

   arg = command;
   while ( *arg && (*arg != ' ') ) {
      arg++;
   } /* Endwhile */
   while ( *arg && (*arg == ' ') ) {
      arg++;
   } /* Endwhile */

   /* Execute specific command */
   switch ( cmdp-commands ) {

      case USER_CMD:
         strcpy(username,arg);
         FTPSRV_tputs(connection, logged);
         break;

      case ACCT_CMD:
         FTPSRV_tputs(connection, unimp);
         break;

      case PASS_CMD:
         FTPSRV_tputs(connection, logged);
         break;

      case TYPE_CMD:
         switch ( arg[0] ) {
            case 'a':   /* Ascii */
            case 'A':   /* Ascii */
               FTPSRV_tputs(connection, typeok);
            break;
            case 'b':   /* Binary */
            case 'B':   /* Binary */
            case 'i':   /* Image */
            case 'I':   /* Image */
               FTPSRV_tputs(connection, typeok);
            break;
            default:   /* Invalid */
               sprintf(message, badtype, arg);
               FTPSRV_tputs(connection, message);
            break;
         } /* End Switch */
         break;

      case QUIT_CMD:
         FTPSRV_tputs(connection, bye);
         return FALSE; /* Quit */

      case PORT_CMD:
         if ( FTPSRV_pport(&ftp_port, arg) == -1 ) {
            FTPSRV_tputs(connection, badport);
         } else {
            FTPSRV_tputs(connection, portok);
         } /* Endif */
         break;

      case RETR_CMD:
      case STOR_CMD:
         if (cmdp-commands == RETR_CMD) {
            mode = "r";
            cmdstr = "RETR";
            donestr = txok;
         } else {
            mode = "w";
            cmdstr = "STOR";
            donestr = rxok;
         } /* Endif */

         /*
         ** Changed By the ASM Tech(Sumanth)
         ** filename[0] = '\0';
         ** if ( *arg ) {
         **    sscanf(arg, "%s", filename);
         ** }
         */
         strcpy(filename, arg);
         file = RTCS_io_open(filename, mode, NULL);
         if (!file) {
            sprintf(message, cantopen, filename);
            FTPSRV_tputs(connection, message);
            break;
         } /* Endif */

         sprintf(message, "%ld.%ld.%ld.%ld",
                 (ftp_port.sin_addr.s_addr >> 24) & 0xFF,
                 (ftp_port.sin_addr.s_addr >> 16) & 0xFF,
                 (ftp_port.sin_addr.s_addr >>  8) & 0xFF,
                  ftp_port.sin_addr.s_addr        & 0xFF);
         sprintf(message2, "%s,%hd", message, ftp_port.sin_port);
         sprintf(message, sending, cmdstr, message2);
         FTPSRV_tputs(connection, message);

         error = FALSE;
         temp_addr.sin_family      = AF_INET;
         temp_addr.sin_port        = IPPORT_FTPDATA;
         temp_addr.sin_addr.s_addr = INADDR_ANY;

         data_connection = socket(PF_INET, SOCK_STREAM, 0);
         if (data_connection == RTCS_SOCKET_ERROR) {
            error = TRUE;
            FTPSRV_tputs(connection,noconn);
         } /* Endif */

         if (! error) {
            error_code = bind(data_connection, &temp_addr, sizeof(temp_addr));
            if (error_code != RTCS_OK) {
               error = TRUE;
               FTPSRV_tputs(connection, noconn);
            } /* Endif */
         } /* Endif */

         if (! error) {
            error_code = connect(data_connection, &ftp_port, sizeof(ftp_port));
            if (error_code != RTCS_OK) {
               shutdown(data_connection, FLAG_ABORT_CONNECTION);
               error = TRUE;
               FTPSRV_tputs(connection, noconn);
            } /* Endif */
         } /* Endif */

         if (! error) {
            bytes_moved = 0;

            start_time = RTCS_time_get();
            if (cmdp-commands == RETR_CMD) {
               while (FTPSRV_s_ftpd(data_connection));
            } else {
               while (FTPSRV_r_ftpd(data_connection));
            } /* Endif */
            end_time = RTCS_time_get();

            /* set default data port */
            ftp_port.sin_family      = remote_addr.sin_family;
            ftp_port.sin_port        = remote_addr.sin_port;
            ftp_port.sin_addr.s_addr = remote_addr.sin_addr.s_addr;

            shutdown(data_connection, FLAG_CLOSE_TX);

            sprintf(message, donestr, bytes_moved,
               (end_time - start_time) / 1000,
               (end_time - start_time) % 1000);
            FTPSRV_tputs(connection, message);
         } /* Endif */

         fclose(file);
         break;

      case HELP_CMD:
         FTPSRV_print_help(connection);
         break;

      case ABORT_CMD:
         break;

      default:
         FTPSRV_tputs(connection, unimp);
         break;

   } /* End Switch */
   return 1;   /* Keep going! */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_pport
* Returned Value   : int_32 successful if 0, -1 on failure
* Comments         :
*    This function converts a string to an IP/PORT address
*
*END*-----------------------------------------------------------------*/

int_32 FTPSRV_pport
   (
      sockaddr_in _PTR_ sock,
      char        _PTR_ arg
   )
{ /* Body */
   int_32 n;
   int_32 atoi(),i;

   sock->sin_family = AF_INET;
   n = 0;
   for ( i=0; i<4; i++ ) {
      n = atoi(arg) + (n << 8);
      if ( (arg = strpbrk(arg, ",")) == NULL ) {
         return -1;
      } /* Endif */
      arg++;
   } /* Endfor */
   sock->sin_addr.s_addr = n;
   n = atoi(arg);
   if ( (arg = strpbrk(arg, ",")) == NULL ) {
      return -1;
   } /* Endif */
   arg++;
   n = atoi(arg) + (n << 8);
   sock->sin_port = n;
   return 0;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_r_ftpd
* Returned Value   : boolean operation successful or not
* Comments         :
*    This function is the
* FTP data channel receive upcall handler, converting hex data
* to a memory location, and writing it there.
*
*END*-----------------------------------------------------------------*/

boolean FTPSRV_r_ftpd
   (
      uint_32 socket
   )
{ /* Body */
   int_32     count;
   int_32     result;

   count = recv(socket, transbuffer, BUF_SIZE, 0);
   DEBUGM(printf("recv in f_ftpd got %ld bytes\n", count);)
   if ( count == RTCS_ERROR ) {
      /* looks like connection dropped */
      return 0;   /* Connection dropping */
   } /* Endif */

   if ( count == 0 ) {  /* All done */
      return 0;   /* Connection dropping */
   } /* Endif */

   bytes_moved += count;
   result = write(file, transbuffer, count);
   if ( result < count ) {
      if (result == IO_ERROR ) {
         sprintf(message, writefail);
      } else {
         sprintf(message, writeeof);
      } /* Endif */
      FTPSRV_tputs(socket, message);
      DEBUGM(printf("Error: %s\n", message);)
      return 0;
   } /* Endif */

   return 1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_s_ftpd
* Returned Value   : boolean operation successful or not
* Comments         :
*    This function is the
* FTP data channel send upcall handler, converting hex data
* to a memory location, and writing it there.
*
*END*-----------------------------------------------------------------*/

boolean FTPSRV_s_ftpd
   (
      uint_32 socket
   )
{ /* Body */
   int_32     count;
   int_32     result;

   result = read(file, transbuffer, BUF_SIZE);

   if (result == IO_ERROR) {  /* EOF reached */
      return 0;
   } /* EndIf */

   count = send(socket, transbuffer, result, 0);
   DEBUGM(printf("send in f_ftpd put %ld bytes\n", result);)
   if (count == RTCS_ERROR) {
      /* looks like connection dropped */
      return 0;   /* Connection dropping */
   } /* Endif */

   bytes_moved += count;

   if (count == 0) {  /* All done */
      return 0;
   } /* Endif */

   return 1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_print_help
* Returned Value   : none
* Comments         :
*    This function sends the help menu on the connection
*
*END*-----------------------------------------------------------------*/

void FTPSRV_print_help
   (
      uint_32 connection
   )
{ /* Body */
   char _PTR_ _PTR_ cmd_ptr;
   register uint_32 i;

   sprintf(message, "214- The following commands are recognized\r\n");
   FTPSRV_tputs(connection, message);

   cmd_ptr = &commands[0];
   while ( *cmd_ptr ) {
      message[0] = 0;
      for ( i = 0; i < 4; i++ ) {
         if ( ! *cmd_ptr ) {
            break;
         } /* Endif */
         sprintf(message2,"%-12s", *cmd_ptr++);
         strcat(message, message2);
      } /* Endfor */
      if ( strlen(message) ) {
         strcat(message, "\r\n");
         FTPSRV_tputs(connection, message);
      } /* Endif */
   } /* Endwhile */
   sprintf(message, "214 - Direct comments to ARC International\r\n");
   FTPSRV_tputs(connection, message);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPSRV_tputs
* Returned Value   : none
* Comments         :
*    This function sends a string to the specified connection
*
*END*-----------------------------------------------------------------*/

void FTPSRV_tputs
   (
      uint_32 connection,
      char   _PTR_ message
   )
{ /* Body */

   DEBUGM(printf("Tx:%s\n", message);)
   send(connection, message, strlen(message), 0);

} /* Endbody */

/*FUNCTION*--------------------------------------------------------------
*
*Function Name	:FTPSRV_openDataConnection
*Returned Value	:Boolean Operation Successful or not
*Comments		:
*		This function opens the DataConnection to send the list Information
*
*END*---------------------------------------------------------------------*/

/* Start CR 1888 */
#ifdef FTPSRV_DO_CALLBACK
void FTPSRV_openDataConnection(uint_32 connection,char _PTR_ addr)
{
	uint_32 		error,data_connection;
	uint_32			error_code;	
    sockaddr_in     temp_addr;
	uint_32			start_time;
	uint_32			end_time;
	char_ptr		donestr;
	uint_32			count;
    
     donestr = txok;	 
     error = FALSE;
     temp_addr.sin_family      = AF_INET;
     temp_addr.sin_port        = IPPORT_FTPDATA;
     temp_addr.sin_addr.s_addr = INADDR_ANY;

     data_connection = socket(PF_INET, SOCK_STREAM, 0);
     if (data_connection == RTCS_SOCKET_ERROR) 
     {
        error = TRUE;
        FTPSRV_tputs(connection,noconn);
     } /* Endif */

     if (! error)
     {
     	/*Bind/reserve the server port 20*/
        error_code = bind(data_connection, &temp_addr, sizeof(temp_addr));
        if (error_code != RTCS_OK) 
        {
           error = TRUE;
           FTPSRV_tputs(connection, noconn);
        } /* Endif */
     } /* Endif */
     
     if (! error) 
     {
     	/*Now establish the connection with the port given by client*/
        error_code = connect(data_connection, &ftp_port, sizeof(ftp_port));
        if (error_code != RTCS_OK) 
        {
           shutdown(data_connection, FLAG_ABORT_CONNECTION);
           error = TRUE;
           FTPSRV_tputs(connection, noconn);
        } /* Endif */
     } /* Endif */
     
     if (! error) 
     {
        start_time = RTCS_time_get();
        
	   	count = send(data_connection, addr, buffer_size, 0);
	   	buffer_size=0;

        end_time = RTCS_time_get();

        shutdown(data_connection, FLAG_CLOSE_TX);
		
        sprintf(message, donestr, count,
           (end_time - start_time) / 1000,
           (end_time - start_time) % 1000);
           
        FTPSRV_tputs(connection, message);
     } /* Endif */
} /* Endbody */
#endif
/* End CR */

/* EOF */
