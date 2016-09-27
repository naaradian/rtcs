#ifndef __rpctypes_h__
#define __rpctypes_h__
/*HEADER****************************************************************
************************************************************************
***
*** Copyright (c) 1992-2003 ARC International
*** All rights reserved
***
*** Portions of this software were written by Sun Microsystems, Inc.
*** Copyright (C) 1984, Sun Microsystems, Inc.
***
*** This software embodies materials and concepts which are
*** confidential to ARC International and is made available solely
*** pursuant to the terms of a written license agreement with
*** ARC International.
***
*** Comments:  This file contains the declaration of the types
***            required for the XDR Library functions.
***
************************************************************************
*END*******************************************************************/

/*
** Sun RPC is a product of Sun Microsystems, Inc. and is provided for
** unrestricted use provided that this legend is included on all tape
** media and as a part of the software program in whole or part.  Users
** may copy or modify Sun RPC without charge, but are not authorized
** to license or distribute it to anyone else except as part of a product or
** program developed by the user.
**
** SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
** WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
** PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
**
** Sun RPC is provided with no support and without any obligation on the
** part of Sun Microsystems, Inc. to assist in its use, correction,
** modification or enhancement.
**
** SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
** INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
** OR ANY PART THEREOF.
**
** In no event will Sun Microsystems, Inc. be liable for any lost revenue
** or profits or other special, indirect and consequential damages, even if
** Sun has been advised of the possibility of such damages.
**
** Sun Microsystems, Inc.
** 2550 Garcia Avenue
** Mountain View, California  94043
*/

#include <rtcsrtos.h>

/*
** RPC additions to <sys/types.h>
*/

/*
** !!NPA!! u_int and enum_t may change on other targets/compilers
*/

#define  u_char   uchar
#define  u_short  uint_16
#define  u_int    uint_32
#define  u_long   uint_32

#define  bool_t   boolean
/*
** enum_t is compiler dependent
*/
#if defined(__BORLANDC__) || defined(PSP_TI32054)
#define  enum_t   int_16
#else
#define  enum_t   int_32
#endif
#define  caddr_t  char_ptr


struct timeval {
   int_32 tv_sec;       /* seconds */
   int_32 tv_usec;      /* and microseconds */
};


#define  __dontcare__   -1


/*
** Some functions to promote portability
*/

#define  rpc_memcopy(src, dest, bsize) _mem_copy(src, dest, bsize)
#define  rpc_memzero(ptr, bsize)       _mem_zero(ptr, bsize)
#define  rpc_memalloc(bsize)           _mem_alloc_zero(bsize)
#define  rpc_memfree(ptr, bsize)       _mem_free(ptr)

#endif
/* EOF */
