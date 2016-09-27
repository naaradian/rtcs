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
*** File: sockio.c
***
*** Comments:  This file contains the implementation of an MQX
***            socket I/O device.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


typedef struct {
   uint_32  SOCKET;
} IO_SOCKET, _PTR_ IO_SOCKET_PTR;

_mqx_int _io_socket_open  (FILE_PTR, char _PTR_,  char _PTR_);
_mqx_int _io_socket_close (FILE_PTR);
_mqx_int _io_socket_read  (FILE_PTR, char _PTR_, _mqx_int);
_mqx_int _io_socket_write (FILE_PTR, char _PTR_, _mqx_int);
_mqx_int _io_socket_ioctl (FILE_PTR, _mqx_uint, pointer);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_install
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

int_32 _io_socket_install
   (
      char_ptr identifier
   )
{ /* Body */

   return _io_dev_install(identifier,
                          _io_socket_open,
                          _io_socket_close,
                          _io_socket_read,
                          _io_socket_write,
#if MQX_VERSION < 250
                          (int_32(_CODE_PTR_)(FILE_PTR, uint_32, uint_32_ptr))
#endif
                          _io_socket_ioctl,
                          NULL );
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_open
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_open
   (
      FILE_PTR    fd_ptr,
      char _PTR_  open_name_ptr,
      char _PTR_  flags_ptr
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = _mem_alloc_zero(sizeof(*io_ptr));
   if (io_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
   fd_ptr->DEV_DATA_PTR = io_ptr;

   io_ptr->SOCKET = (uint_32)flags_ptr;

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_close
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_close
   (
      FILE_PTR   fd_ptr
   )
{ /* Body */

   if (fd_ptr->DEV_DATA_PTR != NULL) {
      _mem_free(fd_ptr->DEV_DATA_PTR);
      fd_ptr->DEV_DATA_PTR = NULL;
   } /* Endif */

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_read
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_read
   (
      FILE_PTR    fd_ptr,
      char _PTR_  data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if (io_ptr != NULL) {
     return (_mqx_int)recv(io_ptr->SOCKET, data_ptr, num, 0);
   } /* Endif */

   return -1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_write
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_write
   (
      FILE_PTR    fd_ptr,
      char _PTR_  data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if (io_ptr != NULL) {
      return (_mqx_int)send(io_ptr->SOCKET, data_ptr, num, 0);
   } /* Endif */

   return 0;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_ioctl
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_ioctl
   (
      FILE_PTR    fd_ptr,
      _mqx_uint   cmd,
      pointer     param_ptr
   )
{ /* Body */
   IO_SOCKET_PTR  io_ptr;
   _mqx_int       result = IO_ERROR;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if ((io_ptr != NULL) && (cmd == IO_IOCTL_CHAR_AVAIL)) {
      result = MQX_OK;
      if (RTCS_selectset(&io_ptr->SOCKET, 1, (uint_32)-1)) {
         *(boolean _PTR_)param_ptr = TRUE;
      } else {
         *(boolean _PTR_)param_ptr = FALSE;
      } /* Endif */
   } /* Endif */

   return result;

} /* Endbody */


/* EOF */
