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
*** File: telnetio.c
***
*** Comments:  This file contains the implementation of an MQX
***            telnet I/O device.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "telnet.h"


/*
** Telnet Response strings
*/
#define TELNET_go_ahead(fd)   fputc(TELCMD_IAC,  (fd)); \
                              fputc(TELCMD_GA,   (fd))

#define TELNET_will(fd,c)     fputc(TELCMD_IAC,  (fd)); \
                              fputc(TELCMD_WILL, (fd)); \
                              fputc((c),         (fd))

#define TELNET_wont(fd,c)     fputc(TELCMD_IAC,  (fd)); \
                              fputc(TELCMD_WONT, (fd)); \
                              fputc((c),         (fd))

#define TELNET_do(fd,c)       fputc(TELCMD_IAC,  (fd)); \
                              fputc(TELCMD_DO,   (fd)); \
                              fputc((c),         (fd))

#define TELNET_dont(fd,c)     fputc(TELCMD_IAC,  (fd)); \
                              fputc(TELCMD_DONT, (fd)); \
                              fputc((c),         (fd))

typedef struct {
    FILE_PTR      NETWORK_FD;
    FILE_PTR      CONSOLE_FD;
    uchar         REMOTE_OPTIONS[TELNET_MAX_OPTS];
    uchar         LOCAL_OPTIONS[TELNET_MAX_OPTS];
    uint_32       FLAGS;
    int_32        STATE;
} IO_TELNET, _PTR_ IO_TELNET_PTR;


_mqx_int _io_telnet_open  (FILE_PTR, char _PTR_,  char _PTR_);
_mqx_int _io_telnet_close (FILE_PTR);
_mqx_int _io_telnet_read  (FILE_PTR, char _PTR_, _mqx_int);
_mqx_int _io_telnet_write (FILE_PTR, char _PTR_, _mqx_int);
_mqx_int _io_telnet_ioctl (FILE_PTR, _mqx_uint, pointer);

boolean _io_telnet_process_char (char _PTR_, FILE_PTR);

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_install
* Returned Value : int_32 a task error code or MQX_OK
* Comments       : Install the telnet device
*
*END*-----------------------------------------------------------------*/

int_32 _io_telnet_install
   (
      char_ptr identifier
   )
{ /* Body */

   return _io_dev_install(identifier,
                          _io_telnet_open,
                          _io_telnet_close,
                          _io_telnet_read,
                          _io_telnet_write,
#if MQX_VERSION < 250
                          (int_32(_CODE_PTR_)(FILE_PTR, uint_32, uint_32_ptr))
#endif
                          _io_telnet_ioctl,
                          NULL );
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_open
* Returned Value : int_32 a task error code or MQX_OK
* Comments       : Open the telnet device
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_open
   (
      FILE_PTR   fd_ptr,
      char _PTR_ open_name_ptr,
      char _PTR_ flags_ptr
   )
{ /* Body */
   IO_TELNET_PTR io_ptr;

   io_ptr = _mem_alloc_zero(sizeof(*io_ptr));
   if (io_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
   fd_ptr->DEV_DATA_PTR = io_ptr;

   io_ptr->NETWORK_FD = (FILE_PTR)flags_ptr;
   io_ptr->CONSOLE_FD = NULL;
#ifdef __MQX__
   fd_ptr->FLAGS = IO_SERIAL_ECHO;  /* To work around a bug in fgetline() */
#endif

   /*
   ** Send WILL SGA and DO SGA options
   */
   TELNET_will (io_ptr->NETWORK_FD, TELOPT_SGA);
   TELNET_do   (io_ptr->NETWORK_FD, TELOPT_SGA);
   io_ptr->LOCAL_OPTIONS[TELOPT_SGA]  = TRUE;
   io_ptr->REMOTE_OPTIONS[TELOPT_SGA] = TRUE;

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  :  _io_telnet_close
* Returned Value :  int_32 Error Code
* Comments       :  Close the telnet device
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_close
   (
      FILE_PTR   fd_ptr
   )
{ /* Body */
   IO_TELNET_PTR  io_ptr;
   _mqx_uint      flags;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if (io_ptr != NULL) {

      /* Reenable local echo */
      if (io_ptr->CONSOLE_FD) {
         ioctl(io_ptr->CONSOLE_FD, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
         flags |= IO_SERIAL_ECHO;
         ioctl(io_ptr->CONSOLE_FD, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
      } /* Endif */

      _mem_free(io_ptr);
      fd_ptr->DEV_DATA_PTR = NULL;
   } /* Endif */

   return MQX_OK;

} /* Endbody */


/* CR 945 */
/*FUNCTION*-------------------------------------------------------------
*
* Function Name  :  _io_telnet_read
* Returned Value :  Number of characters read
* Comments       :  Read data from the Telnet device
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_read
   (
      FILE_PTR    fd_ptr,
      char _PTR_  data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_TELNET_PTR  io_ptr;
   _mqx_int       count, read_count, write_count;
   char_ptr       read_ptr, write_ptr;
   char           c;

   io_ptr = fd_ptr->DEV_DATA_PTR;

   write_ptr = data_ptr;
   write_count = 0;
   for (;;) {

      read_ptr = write_ptr;
      count = _io_read(io_ptr->NETWORK_FD, read_ptr, num-write_count);
      if (count == IO_EOF) {
         break;
      } /* Endif */

      for (read_count = 0; read_count < count; read_count++) {
         c = *read_ptr++;
         if (_io_telnet_process_char(&c, fd_ptr)) {
            *write_ptr++ = c;
            write_count++;
            if (io_ptr->FLAGS & IO_SERIAL_ECHO) {
               if (c == '\b') {
                  fputs("\b ", fd_ptr);
               } /* Endif */
               fputc(c, fd_ptr);
            } /* Endif */
         } /* Endif */
      } /* Endfor */

      if (write_count >= num) {
         break;
      } /* Endif */
      if (write_count && !_io_fstatus(io_ptr->NETWORK_FD)) {
         break;
      } /* Endif */

   } /* Endfor */

   return write_count;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_write
* Returned Value : Number of characters written
* Comments       : Writes the data to the Telnet device
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_write
   (
      FILE_PTR    fd_ptr,
      char _PTR_  data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_TELNET_PTR  io_ptr;
   _mqx_int       count;
   _mqx_int       towrite = 0;
   char_ptr       scan_ptr;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   for (count = 0, scan_ptr = data_ptr; count < num ; count++, scan_ptr++) {

      if (*scan_ptr == TELCMD_IAC) {
         towrite++;
         _io_write(io_ptr->NETWORK_FD, data_ptr, towrite);
         data_ptr += towrite;
         fputc(TELCMD_IAC, io_ptr->NETWORK_FD);
         towrite = 0;

      } else if (*scan_ptr == '\n') {
         if (towrite) {
            _io_write(io_ptr->NETWORK_FD, data_ptr, towrite);
            data_ptr += towrite;
         } /* Endif */
         fputc('\r', io_ptr->NETWORK_FD);
         towrite = 1;

      } else if (*scan_ptr == '\r') {
         towrite++;
         _io_write(io_ptr->NETWORK_FD, data_ptr, towrite);
         data_ptr += towrite;
         fputc('\0', io_ptr->NETWORK_FD);
         towrite = 0;

      } else {
         towrite++;
      } /* Endif */

   } /* Endfor */

   if (towrite) {
      _io_write(io_ptr->NETWORK_FD, data_ptr, towrite);
   } /* Endif */

   return count;

} /* Endbody */
/* End CR 945 */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_ioctl
* Returned Value : int_32
* Comments       : Returns result of ioctl operation.
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_ioctl
   (
      FILE_PTR    fd_ptr,
      _mqx_uint   cmd,
      pointer     param_ptr
   )
{ /* Body */
   IO_TELNET_PTR  io_ptr;
   int_32         result;
   _mqx_uint      flags;
   int_32         rawc;
   char           c;

   result = IO_ERROR;
   io_ptr = (IO_TELNET_PTR) fd_ptr->DEV_DATA_PTR;

   if (io_ptr == NULL) {
      return (_mqx_int)result;
   } /* Endif */

   switch (cmd) {

   case IO_IOCTL_CHAR_AVAIL:

      while (1) {
         *(boolean _PTR_)param_ptr = fstatus(io_ptr->NETWORK_FD);
         if (!*(boolean _PTR_)param_ptr) {
            break;
         } /* Endif */
         rawc = (int_32)fgetc(io_ptr->NETWORK_FD);
         if (rawc == IO_EOF) {
            break;
         } /* Endif */
         c = (char)rawc;
         if (_io_telnet_process_char(&c, fd_ptr)) {
            fungetc(c, fd_ptr);
            break;
         } /* Endif */
      } /* Endwhile */

      result = MQX_OK;
      break;

   case IO_IOCTL_SET_STREAM:

      io_ptr->CONSOLE_FD = (FILE_PTR)param_ptr;

      /* Clear or set echoing */
      ioctl(io_ptr->CONSOLE_FD, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
      if (io_ptr->REMOTE_OPTIONS[TELOPT_ECHO]) {
         flags &= ~IO_SERIAL_ECHO;
      } else {
         flags |= IO_SERIAL_ECHO;
      } /* Endif */
      ioctl(io_ptr->CONSOLE_FD, IO_IOCTL_SERIAL_SET_FLAGS, &flags);

      result = MQX_OK;
      break;

   case IO_IOCTL_SERIAL_GET_FLAGS:
      *(_mqx_uint_ptr)param_ptr = io_ptr->FLAGS;
      result = MQX_OK;
      break;

   case IO_IOCTL_SERIAL_SET_FLAGS:
      io_ptr->FLAGS = *(_mqx_uint_ptr)param_ptr;
      /* Handle Echo */
      if (!io_ptr->LOCAL_OPTIONS[TELOPT_ECHO]) {
         if (io_ptr->REMOTE_OPTIONS[TELOPT_ECHO]) {
            TELNET_dont(io_ptr->NETWORK_FD, TELOPT_ECHO);
            io_ptr->REMOTE_OPTIONS[TELOPT_ECHO] = FALSE;
         } /* Endif */
         TELNET_will(io_ptr->NETWORK_FD, TELOPT_ECHO);
         io_ptr->LOCAL_OPTIONS[TELOPT_ECHO] = TRUE;
      } /* Endif */
      result = MQX_OK;
      break;

   } /* Endswitch */

   return (_mqx_int)result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_process_char
* Returned Value : boolean
* Comments       : process the arrived characters, including negotiated options
*
*END*-----------------------------------------------------------------*/

boolean _io_telnet_process_char
   (
      char _PTR_  cp,
      FILE_PTR    fd_ptr
   )
{ /* Body */
   char           c = *cp;
   _mqx_uint      flags;
   IO_TELNET_PTR  io_ptr;
   FILE_PTR       console_stream;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   console_stream = io_ptr->CONSOLE_FD;

   switch (io_ptr->STATE) {

   case 0: /* Start state */
      switch (c) {

      case TELCMD_IAC:
         io_ptr->STATE = 1;
         return FALSE;

      case '\r':
         io_ptr->STATE = 6;
         return FALSE;

      default:
         return TRUE;
      } /* Endswitch */
      /* Start SPR P122-0266-33 remove all warnings from RTCS code. */
      /* break; */
      /* End SPR P122-0266-33. */

   case 1: /* IAC */
      switch (c) {

      case TELCMD_AYT:
         /*
         ** Request to send a string to prove we're alive.
         ** Send back comforting message.
         */
         fputs("\nPrecise/RTCS Telnet server online.\n", fd_ptr);
         io_ptr->STATE = 0;
         return FALSE;

      case TELCMD_EC:
         /*
         ** Request to erase the last character before this command.
         */
         *cp = '\b';
         io_ptr->STATE = 0;
         return TRUE;

      case TELCMD_WILL:
         io_ptr->STATE = 2;
         return FALSE;

      case TELCMD_DO:
         io_ptr->STATE = 3;
         return FALSE;

      case TELCMD_WONT:
         io_ptr->STATE = 4;
         return FALSE;

      case TELCMD_DONT:
         io_ptr->STATE = 5;
         return FALSE;

      case TELCMD_IAC:
         /* Second IAC is not interpreted as command, it's data */
         io_ptr->STATE = 0;
         return TRUE;

      default:
         /* Ignore all other telnet commands (e.g GA) */
         io_ptr->STATE = 0;
         return FALSE;
      } /* Endswitch */

   case 2: /* IAC WILL */
      switch (c) {

      case TELOPT_ECHO:
         if (!io_ptr->REMOTE_OPTIONS[c]) {
            if (io_ptr->LOCAL_OPTIONS[c]) {
               TELNET_dont(io_ptr->NETWORK_FD, c);
            } else {
               /* Clear the echo bit */
               ioctl(console_stream, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
               flags &= ~IO_SERIAL_ECHO;
               ioctl(console_stream, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
               io_ptr->REMOTE_OPTIONS[c] = TRUE;
               TELNET_do(io_ptr->NETWORK_FD, c);
            } /* Endif */
         } /* Endif */
         break;

      case TELOPT_SGA:
         /* ACK only */
         if (!io_ptr->REMOTE_OPTIONS[c]) {
            io_ptr->REMOTE_OPTIONS[c] = TRUE;
            TELNET_do(io_ptr->NETWORK_FD, c);
         } /* Endif */
         break;

      default:
         TELNET_dont(io_ptr->NETWORK_FD, c);
         break;
      }  /* Endswitch */

      io_ptr->STATE = 0;
      return FALSE;

   case 3: /* IAC DO */
      switch (c) {

      case TELOPT_ECHO:
         /* ACK only */
         if (!io_ptr->LOCAL_OPTIONS[c]) {
            if (io_ptr->REMOTE_OPTIONS[c]) {
               TELNET_wont(io_ptr->NETWORK_FD, c);
            } else {
               io_ptr->LOCAL_OPTIONS[c] = TRUE;
               TELNET_will(io_ptr->NETWORK_FD, c);
            } /* Endif */
         } /* Endif */
         break;

      case TELOPT_SGA:
         /* ACK only */
         if (!io_ptr->LOCAL_OPTIONS[c]) {
            io_ptr->LOCAL_OPTIONS[c] = TRUE;
            TELNET_will(io_ptr->NETWORK_FD, c);
         } /* Endif */
         break;

      default:
         TELNET_wont(io_ptr->NETWORK_FD, c);
         break;
      } /* Endswitch */

      io_ptr->STATE = 0;
      return FALSE;

   case 4: /* IAC WONT */
      switch (c) {

      case TELOPT_ECHO:
         if (io_ptr->REMOTE_OPTIONS[c]) {
            /* Set the echo bit */
            ioctl(console_stream, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
            flags |= IO_SERIAL_ECHO;
            ioctl(console_stream, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
            io_ptr->REMOTE_OPTIONS[c] = FALSE;
            TELNET_dont(io_ptr->NETWORK_FD, c);
         } /* Endif */
         break;

      default:
         if (c < TELNET_MAX_OPTS) {
            /* ACK only */
            if (io_ptr->REMOTE_OPTIONS[c]) {
               TELNET_dont(io_ptr->NETWORK_FD, c);
               io_ptr->REMOTE_OPTIONS[c] = FALSE;
            } /* Endif */
         } /* Endif */
         break;
      } /* Endswitch */

      io_ptr->STATE = 0;
      return FALSE;

   case 5: /* IAC DONT */

      if (c < TELNET_MAX_OPTS) {
         /* ACK only */
         if (io_ptr->LOCAL_OPTIONS[c]) {
            io_ptr->LOCAL_OPTIONS[c] = FALSE;
            TELNET_wont(io_ptr->NETWORK_FD, c);
         } /* Endif */
      } /* Endif */

      io_ptr->STATE = 0;
      return FALSE;

   case 6: /* CR */

      if (c == '\0') {
         *cp = '\r';
      } /* Endif */
      io_ptr->STATE = 0;
      return TRUE;

   } /* Endswitch */

   /* Start SPR P122-0266-33 remove all warnings from RTCS code. */
   /* Will never reach here, this is just for compiler warning. */
   return FALSE;
   /* End SPR P122-0266-33. */
} /* Endbody */


/* EOF */
