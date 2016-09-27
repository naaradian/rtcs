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
*** File: rtcsmem.c
***
*** Comments:  This file contains the stub function calls using MQX2.3X
***            operating system.
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>

pointer _RTCS_io_component_handle = NULL;
pointer _SNMP_io_component_handle = NULL;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mqx_get_io_component_handle
* Returned Value   : pointer handle
* Comments         :
*    This function returns the I/O Component handle for the specified
*    I/O component
*
*END*-----------------------------------------------------------------*/

pointer _mqx_get_io_component_handle
   (
      uint_32 component       /* [IN] the component number */
   )
{ /* Body */
   if (component == IO_RTCS_COMPONENT) {
      return _RTCS_io_component_handle;
   }else if (component == IO_SNMP_COMPONENT) {
      return _SNMP_io_component_handle;
   }else{
      return NULL;
   } /* Endif */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mqx_set_io_component_handle
* Returned Value   : pointer handle
* Comments         :
*    This function sets the I/O Component handle for the specified
*
*END*-----------------------------------------------------------------*/

pointer _mqx_set_io_component_handle
   (
      uint_32 component,   /* [IN] the component number */
      pointer handle       /* [IN] the new handle */
   )
{ /* Body */
   pointer _PTR_ handle_ptr;
   pointer       old_handle;

   if (component == IO_RTCS_COMPONENT) {
       handle_ptr = &_RTCS_io_component_handle;
   } else if (component == IO_SNMP_COMPONENT) {
       handle_ptr = &_SNMP_io_component_handle;
   } else {
      return NULL;
   } /* Endif */

   _Disable();
   old_handle = *handle_ptr;
   if (old_handle == NULL) {
      *handle_ptr = handle;
   } else {
      if (handle == NULL) {
         *handle_ptr = handle;
      } /* Endif */
   } /* Endif */
   _Enable();

   return old_handle;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : _mem_sum_ip
* Returned Value  : one's complement checksum
* Comments        :
*     Returns a one's complement checksum over a block of memory, as
*     used for packets in Internet protocols (see RFC791 for definition).
*
*     Note:  This function returns 0 iff all summands are 0.
*
*     Note:  It is impossible to overflow the 32-bit accumulator,
*            because we're summing length+1 words.  length is a 16-bit
*            value, and each word is also 16 bits.  Hence, maximum sum
*            is (2^16)(2^16-1) < 2^32.
*
*END*-----------------------------------------------------------------*/

uint_32 _mem_sum_ip
   (
               uint_32     sum,
      register uint_32     length,
               pointer     buffer
   )
{ /* Body */
   register uint_32        total = sum & 0xffff;
   register uint_32        temp;
   register uchar _PTR_    buf = (char _PTR_) buffer;
   if (length & 1) {
      length--;
      total += ntohc(buf+length) << 8;
   } /* Endif */

   length >>= 1;
   while (length--) {
      temp   = (uint_16)ntohc(buf);
      total += temp<<8 | ntohc(buf+1);
      buf   += 2;
   } /* Endwhile */

   sum = ((total >> 16) & 0xFFFF);
   if (sum) {
      total = (total & 0xFFFF) + sum;
      sum = ((total >> 16) & 0xFFFF);
      if (sum) {
         total = (total & 0xFFFF) + sum;
      } /* Endif */
   } /* Endif */
   return total;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_alloc_internal
* Returned Value   : pointer to a memory block, or NULL
*   queue
* Comments         :
*   This function calls a specific memory allocation function depending upon
*   the parameters that are passed in. It also saves information allowing
*   _mem_free to call the correct memory freeing function.
*
*END*-----------------------------------------------------------------*/

pointer _mem_alloc_internal
   (
      uint_32 size,        /* [IN] size of the memory block */
      boolean zero,        /* [IN] should it be zeroed? */
      boolean system       /* [IN] should it be system memory ? */
   )
{ /* Body */
   pointer (_CODE_PTR_  get_mem)(uint_32);
   boolean _PTR_        block;

   if (system) {
      get_mem = _KRNL_Ltm_getzerovec;
   } else if (zero) {
      get_mem = _Getzerovec;
   } else {
      get_mem = _Getvec;
   } /* Endif */

   block = get_mem(size + sizeof(boolean));

   if (!block) {
      return NULL;
   } /* Endif */

   *block = system;
   return (pointer) (block + 1);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _mem_free
* Returned Value   : error code or RTCS_OK
* Comments         :
*   This function frees a memory block.
*
*END*-----------------------------------------------------------------*/

uint_32 _mem_free
   (
      pointer  block       /* [IN] memory block to free */
   )
{ /* Body */
   boolean _PTR_  free_block = (boolean _PTR_)block - 1;
   uint_32        error;

   if (*free_block) {
      error = _KRNL_Ltm_freevec(free_block);
   } else {
      error = _Freevec(free_block);
   } /* Endif */

   return error;

} /* Endbody */


/* EOF */
