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
*** File: rtcspart.c
***
*** Comments:  This file contains the interface to the RTCS
***            partitions.
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_create
* Returned Values : An _rtcs_part if successful, or 0 if creation fails.
* Comments        :
*     Create and initialize a partition.
*
*END*-----------------------------------------------------------------*/

_rtcs_part RTCS_part_create
   (
      uint_32 size,                       /* [IN] size of one block */
      uint_32 init,                       /* [IN] initial number of blocks */
      uint_32 grow,                       /* [IN] number of blocks to grow by */
      uint_32 max,                        /* [IN] maximum number of blocks */
      int_32 (_CODE_PTR_ cons)(pointer),  /* [IN] block constructor */
      int_32 (_CODE_PTR_ dest)(pointer)   /* [IN] block destroyer */
   )
{ /* Body */
   _rtcs_part     part;
   pointer _PTR_  elem;
   uint_32        i;

   /* Allocate partition */
   /* CR 929 */
   part = _mem_alloc_system(sizeof(*part));
   if (!part) {
      return NULL;
   } /* Endif */
   part->PART = _partition_create(size + sizeof(pointer), init, grow, max);
   if (part->PART == PARTITION_NULL_ID) {
      _mem_free(part);
      return NULL;
   } /* Endif */

   /* Set up partition information */
   part->SIZE   = size;
   /* End CR 929 */
   part->FREE   = NULL;
   part->CONSTR = cons;
   part->DESTR  = dest;

   /* Pre-construct some blocks and enqueue them */
   for (i = 0; i < init; i++) {
      elem = _partition_alloc_system(part->PART);
      if (!elem) {
         break;
      } /* Endif */
      if (part->CONSTR) {
         if (part->CONSTR(elem+1)) {
            _partition_free(elem);
            break;
         } /* Endif */
      } /* Endif */
      *elem = part;
      RTCS_part_free(elem+1);
   } /* Endfor */

   return part;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_alloc
* Returned Values : A pointer to a partition block if successful, or
*                   NULL if allocation fails.
* Comments        :
*     Allocate a partition block.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_part_alloc
   (
      _rtcs_part part            /* [IN] partition to allocate from */
   )
{ /* Body */
   pointer _PTR_  elem;

   if (!part->FREE) {

      /* Allocate a new block and construct it */
      elem = _partition_alloc_system(part->PART);
      if (!elem) {
         return NULL;
      } /* Endif */
      if (part->CONSTR) {
         if (part->CONSTR(elem+1)) {
            _partition_free(elem);
            return NULL;
         } /* Endif */
      } /* Endif */

   } else {

      /* Dequeue a block */
     _int_disable();
      elem = part->FREE;
      part->FREE = *elem;
     _int_enable();

   } /* Endif */

   /* Point the block back to the partition (for RTCS_part_free()) */
   *elem = part;
   return elem + 1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_alloc_zero
* Returned Values : A pointer to a zeroed partition block if successful,
*                   or NULL if allocation fails.
* Comments        :
*     Allocate a partition block and zero it.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_part_alloc_zero
   (
      _rtcs_part part            /* [IN] partition to allocate from */
   )
{ /* Body */
   pointer elem;

   elem = RTCS_part_alloc(part);
   if (elem) {
      _mem_zero(elem, part->SIZE);
   } /* Endif */

   return elem;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_free
* Returned Values : void
* Comments        :
*     Free a partition block.
*
*END*-----------------------------------------------------------------*/

void RTCS_part_free
   (
      pointer block              /* [IN] block to free */
   )
{ /* Body */
   pointer _PTR_  elem = block;
   _rtcs_part     part;

   part = *--elem;

  _int_disable();
   *elem = part->FREE;
   part->FREE = elem;
   _int_enable();

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_destroy
* Returned Values : void
* Comments        :
*     Destroy a partition
*
*END*-----------------------------------------------------------------*/

void RTCS_part_destroy
   (
      _rtcs_part part            /* [IN] partition to destroy */
   )
{ /* Body */
   pointer _PTR_  elem;

   /* Destroy all the elements */
   if (part->DESTR) {
      while (part->FREE) {
         elem = part->FREE;
         part->FREE = *elem;
         part->DESTR(elem+1);
         _partition_free(elem);
      } /* Endwhile */
   } /* Endif */

   /* Free the partition */
   _partition_destroy(part->PART);
   _mem_free(part);

} /* Endbody */


/* EOF */
