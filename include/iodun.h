#ifndef __iodun_h__
#define __iodun_h__
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
*** File: iodun.h
***
*** Comments: The file contains functions prototype, defines, structure
***           definitions specific for the DUN and RAS drivers
***
************************************************************************
*END*******************************************************************/

/***************************************
**
** MQX compatibility
*/

#define _iodun_mutex             LWSEM_STRUCT
#define IODUN_mutex_init(p)      _lwsem_create(p,1)
#define IODUN_mutex_destroy(p)   _lwsem_destroy(p)
#define IODUN_mutex_lock(p)      _lwsem_wait(p)
#define IODUN_mutex_unlock(p)    _lwsem_post(p)

/*--------------------------------------------------------------------*/
/*
**                    TYPE DEFINITIONS
*/

/*
** RAS DEVICE STRUCT
*/
typedef struct {
   char_ptr     RECV;
   char_ptr     SEND;
   _mqx_uint    RECV_SIZE;
   _mqx_uint    SEND_SIZE;
} IODUN_DEV_STRUCT, _PTR_ IODUN_DEV_STRUCT_PTR;

/*
** RAS FILE STRUCT
*/
typedef struct {
   FILE_PTR       F_PTR;
   _mqx_uint      STATE;
   _iodun_mutex   MUTEX;
   char_ptr       PARSE;
   char           C;
} IODUN_STRUCT, _PTR_ IODUN_STRUCT_PTR;


/*--------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif


uint_32 _io_ras_install(char_ptr);
uint_32 _io_dun_install(char_ptr);

extern _mqx_int _io_dun_open      (FILE_PTR, char_ptr, char_ptr);
extern _mqx_int _io_dun_close     (FILE_PTR);
extern _mqx_int _io_dun_write     (FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_dun_read      (FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_dun_ioctl     (FILE_PTR, _mqx_uint, pointer);
extern char     _io_dun_read_char (FILE_PTR);

#ifdef __cplusplus
}
#endif


#endif
/* EOF */
