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

#define RTCS_PART_ALIGN(n) ((n) + (-(n) & 3))

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_grow
* Returned Values : RTCS_OK if successful, or RTCS_ERROR if a
*                   constructor failed
* Comments        :
*     Add blocks to a partition.
*
*END*-----------------------------------------------------------------*/

int_32 RTCS_part_grow
   (
      _rtcs_part  part,          /* [IN] partition to grow */
      pointer     blocks,        /* [IN] array of blocks to add to part */
      uint_32     num_blocks     /* [IN] number of blocks to add to part */
   )
{ /* Body */
   uint_32  i;
   pointer  elem;

   /* Construct all the blocks */
   if (part->CONSTR) {
      elem = blocks;
      elem = (pointer _PTR_)elem + 1;
      for (i = 0; i < num_blocks; i++) {
         if (part->CONSTR(elem)) {
            if (part->DESTR) {
               while (i > 0) {
                  elem = (char_ptr)elem - part->BLOCK_SIZE;
                  part->DESTR(elem);
                  i--;
               } /* Endwhile */
            } /* Endif */
            return RTCS_ERROR;
         } /* Endif */
         elem = (char_ptr)elem + part->BLOCK_SIZE;
      } /* Endfor */
   } /* Endif */

   /* Add the blocks to the free list */
   part->NUM_BLOCKS += num_blocks;
   elem = blocks;
   for (i = 0; i < num_blocks; i++) {
      *(pointer _PTR_)elem = part;
      RTCS_part_free((pointer _PTR_)elem+1);
      elem = (char_ptr)elem + part->BLOCK_SIZE;
   } /* Endfor */

   return RTCS_OK;

} /* Endbody */


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
   _rtcs_part  part;

   /* Allocate partition and initial blocks */
   size = RTCS_PART_ALIGN(sizeof(pointer) + size);
   part = _mem_alloc_system(sizeof(*part) + size * init);
   if (!part) {
      return NULL;
   } /* Endif */

   /* Set up partition information */
   part->BLOCK_SIZE  = size;
   part->NUM_BLOCKS  = 0;
   part->GROW_BLOCKS = grow;
   part->MAX_BLOCKS  = max;
   part->GROW        = NULL;
   part->FREE        = NULL;
   part->CONSTR      = cons;
   part->DESTR       = dest;

   /* Construct all the blocks and add them to the free list */
   if (RTCS_part_grow(part, part+1, init) == RTCS_ERROR) {
      _mem_free(part);
      return NULL;
   } /* Endif */

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
   uint_32        new_blocks;
   pointer        grow;
   pointer _PTR_  elem;

   if (!part->FREE && part->GROW_BLOCKS) {
      _int_disable();
      if (!part->MAX_BLOCKS || (part->NUM_BLOCKS < part->MAX_BLOCKS)) {
         new_blocks = part->GROW_BLOCKS;
         if (part->MAX_BLOCKS && (new_blocks > part->MAX_BLOCKS-part->NUM_BLOCKS)) {
            /* Don't grow beyond maximum */
            new_blocks = part->MAX_BLOCKS - part->NUM_BLOCKS;
         } /* Endif */
         _int_enable();

         grow = _mem_alloc_system(sizeof(pointer) + part->BLOCK_SIZE * new_blocks);
         if (grow) {

            _int_disable();
            if (!part->MAX_BLOCKS || (part->NUM_BLOCKS+new_blocks <= part->MAX_BLOCKS)) {

               /* Construct all the blocks and add them to the free list */
               elem = grow;
               if (RTCS_part_grow(part, elem+1, new_blocks) == RTCS_ERROR) {
                  _int_enable();
                  _mem_free(grow);
               } else {
                  *elem = part->GROW;
                  part->GROW = elem;
                  _int_enable();
               } /* Endif */

            } else {
               _int_enable();
               _mem_free(grow);
            } /* Endif */

         } /* Endif */
      } else {
         _int_enable();
      } /* Endif */
   } /* Endif */

   /* Start CR 1139 */
   _int_disable();
   if (!part->FREE) {
      _int_enable();
      return NULL;
   } /* Endif */

   /* Dequeue a block */
   /* End CR */
   elem = part->FREE;
   part->FREE = *elem;
   _int_enable();

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
      _mem_zero(elem, part->BLOCK_SIZE - sizeof(pointer));
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
   pointer _PTR_  nextelem;

   /* Destroy all the elements */
   if (part->DESTR) {
      for (elem = part->FREE; elem; elem = *elem) {
         part->DESTR(elem+1);
      } /* Endwhile */
   } /* Endif */

   /* Free all the grown blocks */
   for (elem = part->GROW; elem; elem = nextelem) {
      nextelem = *elem;
      _mem_free(elem);
   } /* Endfor */

   /* Free the partition and the initial blocks */
   _mem_free(part);

} /* Endbody */


/* EOF */
