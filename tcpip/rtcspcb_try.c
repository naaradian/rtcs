/*HEADER****************************************************************
************************************************************************
***
*** Copyright (c) Precise Software Technologies Incorporated
*** All rights reserved
***
*** This software embodies materials and concepts which are
*** confidential to Precise Software Technologies Inc. and is made
*** available solely pursuant to the terms of a written license
*** agreement with Precise Software Technologies Inc.
***
*** File: pcb.c
***
*** Comments:  This file contains the PCB (Packet Control Block)
***            library.
***
*** Revision History:
***   Date        Version  Changes
***   ----        -------  -------
***   Oct  6/92   1.0      Initial Version
***   Dec  1/93   2.10     Added PCB_Free and optimized PCB_Alloc
***   Jan 19/94   2.20     Cleaned up PCB_Init
***   Mar 10/97   2.60     Restructured for multihoming support
***
************************************************************************
*END*******************************************************************/

#include <rtcsrtos.h>

#include "rtcs.h"
#include "rtcs_prv.h"

#define PCBLOG(x) /*printf x*/


/*
** Note:  The following discussion deals with two different types of data
** structures.  A 'PCB' is a very simple structure consisting of a 'free'
** function and a sequence of fragments.  An 'RTCSPCB' is a larger
** structure used by RTCS to hold extra information for 'PCB's.
**
** For PCBs generated on the local host, RTCS calls RTCSPCB_alloc_send(),
** and PCB_PTR points to the PCB field.  The PCB is initialized with only
** one fragment, which is stored in the BUFFER array.
**
** For PCBs arriving from the link layer, RTCS calls RTCSPCB_alloc_recv(),
** and PCB_PTR points the the received PCB.  Here, the PCB, FRAG and BUFFER
** fields of the RTCSPCB are unused.
**
** In both cases, PCB_PTR points to the "actual" PCB.
**
** Some terminology:  RTCSPCBs where PCB_PTR points to the PCB field will
** be called S-PCBs (since they're allocated with RTCSPCB_alloc_send()),
** and RTCSPCBs where PCB_PTR points to an external PCB will be called
** R-PCBs (since they're allocated with RTCSPCB_alloc_recv()).
**
** Now, there exist situations (e.g. IP forwarding) where we want to
** convert an R-PCB to an S-PCB.  In order to do this, we cut the R-PCB's
** first fragment at the current layer, and prepend a fragment for the
** new headers.  This is done by RTCSPCB_fork().  Note that we must prepend
** a fragment:  although there was room in the R-PCB's first fragment for
** headers, we can't use it in case the new headers turn out to be larger
** than the old ones.
**
** Additionally, there are situations where we want to duplicate an RTCSPCB
** (e.g. broadcasting and multicasting).  RTCSPCB_alloc_dup() will allocate
** an S-PCB containing the same data as the original.
**
*/


static void RTCSPCB_free_internal(PCB_PTR);


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_init
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the PCB module.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_init
   (
      void
   )
{ /* Body */
   RTCS_DATA_PTR    RTCS_data_ptr;

   RTCS_data_ptr = RTCS_get_data();

   RTCS_data_ptr->RTCS_PCB_partition = _partition_create(sizeof(RTCSPCB), _RTCSPCB_max, 0, 0);
   if (RTCS_data_ptr->RTCS_PCB_partition == 0) {
      return RTCSERR_PCB_ALLOC;
   } /* Endif */

   return RTCS_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_alloc
*  Returned Value : pcb_ptr or NULL
*  Comments       :
*         Allocate an RTCSPCB
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR RTCSPCB_alloc
   (
      void
   )
{ /* Body */
   RTCS_DATA_PTR     RTCS_data_ptr;
   RTCSPCB_PTR       pcb;

   RTCS_data_ptr = RTCS_get_data();

   pcb = _partition_alloc_system(RTCS_data_ptr->RTCS_PCB_partition);

   if (pcb != NULL) {
      pcb->PCB.FREE    = RTCSPCB_free_internal;
      pcb->PCB.PRIVATE = pcb;

      pcb->PCB_ORIG = NULL;

      pcb->DUPPTR   = NULL;
      RTCS_sem_init(&pcb->DUPCOUNT);

      pcb->IP_SUM_PTR = NULL;

      pcb->FREE_FRAG_BITFIELD = 0;

      pcb->UDP_REQUEST = NULL;

      _mem_zero(&pcb->LINK_OPTIONS, sizeof(pcb->LINK_OPTIONS));
   } /* Endif */

   return pcb;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_alloc_send
*  Returned Value : pointer to PCB
*  Comments       :
*        Allocates a PCB for sending.
*        Caller must still fill in FRAG[n] and SIZE.
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR RTCSPCB_alloc_send
   (
      void
   )
{ /* Body */
   RTCSPCB_PTR pcb;
   PCB_FRAGMENT _PTR_ pcb_frag_ptr;

   pcb = RTCSPCB_alloc();

   if (pcb != NULL) {

      pcb->PCB_PTR = &pcb->PCB;
      /* Start SPR P122-0266-24 remove all warnings from RTCS code. */
      /* pcb->PCB.FRAG[0].LENGTH   = 0; */
      /* pcb->PCB.FRAG[0].FRAGMENT = (uchar_ptr)&pcb->PCB; */ 
      /* pcb->PCB.FRAG[1].LENGTH   = 0; */
      /* pcb->PCB.FRAG[1].FRAGMENT = NULL; */

      pcb_frag_ptr = pcb->PCB.FRAG;
      pcb_frag_ptr->LENGTH   = 0;
      /* grows down into BUFFER */
      pcb_frag_ptr->FRAGMENT = (uchar_ptr)&pcb->PCB; 
      pcb_frag_ptr++;
      pcb_frag_ptr->LENGTH   = 0;
      pcb_frag_ptr->FRAGMENT = NULL;
      /* END SPR P122-0266-24. */
      
      pcb->CL       = RTCSPCB_LAYER_APPLICATION;
      pcb->FRAGUSED = 0;
      pcb->FRAGFREE = RTCSPCB_DATA_MAX;
      pcb->LASTFRAG = 1;
      RTCSPCB_SIZE(pcb) = 0;
      RTCSPCB_DATA(pcb) = pcb->PCB.FRAG[0].FRAGMENT;

      pcb->PCB_FREE = NULL;
   } /* Endif */

   PCBLOG(("\nPCB: Alloc_send: returning %p", pcb));
   return pcb;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_alloc_recv
*  Returned Value : pointer to PCB
*  Comments       :
*        Allocates a PCB for receiving.
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR RTCSPCB_alloc_recv
   (
      PCB_PTR  inpcb
         /* [IN] received packet */
   )
{ /* Body */
   RTCSPCB_PTR      pcb;
   PCB_FRAGMENT_PTR frag;

   pcb = RTCSPCB_alloc();

   if (pcb != NULL) {

      pcb->PCB_PTR  = inpcb;

      pcb->CL       = RTCSPCB_LAYER_LINK;
      pcb->FRAGUSED = inpcb->FRAG[0].LENGTH;
      pcb->FRAGFREE = 0;
      pcb->LASTFRAG = 0;
      RTCSPCB_SIZE(pcb) = 0;
      RTCSPCB_DATA(pcb) = inpcb->FRAG[0].FRAGMENT;

      for (frag = inpcb->FRAG; frag->LENGTH; frag++) {
         RTCSPCB_SIZE(pcb) += frag->LENGTH;
      } /* Endfor */

      pcb->PCB_FREE    = inpcb->FREE;
      pcb->PCB_PRIVATE = inpcb->PRIVATE;
      pcb->PCB_BUFFER  = inpcb->FRAG[0];
      inpcb->FREE      = RTCSPCB_free_internal;
      inpcb->PRIVATE   = pcb;
   } /* Endif */

   PCBLOG(("\nPCB: Alloc_recv: returning %p, PCB is %p", pcb, inpcb));
   return pcb;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_alloc_dup
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Duplicate a PCB.
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR RTCSPCB_alloc_dup
   (
      RTCSPCB_PTR    origpcb
         /* [IN] packet to duplicate */
   )
{ /* Body */
   RTCSPCB_PTR pcb;

   pcb = RTCSPCB_alloc();

   if (pcb != NULL) {
      _mem_copy((uchar_ptr)origpcb, (uchar_ptr)pcb, sizeof(RTCSPCB));
      pcb->PCB.PRIVATE = pcb;

      if (origpcb->PCB_PTR == &origpcb->PCB) {

         /* Packet originated on this host */

          pcb->PCB_PTR = &pcb->PCB;
          RTCSPCB_DATA(pcb) = pcb->BUFFER + (RTCSPCB_DATA(origpcb) - origpcb->BUFFER);

          if (pcb->IP_SUM_PTR) {
             pcb->IP_SUM_PTR = pcb->BUFFER + (origpcb->IP_SUM_PTR - origpcb->BUFFER);
          } /* Endif */
	  /* START SPR 122-319-01
	  ** A duplicate rtcspcb must not point to the same external
	  ** pcb as the original
	  */
	  } else {
         	_mem_copy(origpcb->PCB_PTR->FRAG, pcb->PCB.FRAG, sizeof(PCB_FRAGMENT)*2);
      } /* Endif */
	  /* END SPR 122-319-01*/
      pcb->PCB_FREE = NULL;
      pcb->PCB_ORIG = NULL;

      pcb->DUPPTR   = origpcb;
      RTCS_sem_init(&pcb->DUPCOUNT);
      RTCS_sem_post(&origpcb->DUPCOUNT);

      pcb->UDP_REQUEST = NULL;
      pcb->FREE_FRAG_BITFIELD = 0;

      PCBLOG(("\nPCB: Alloc_dup: returning %p, duplicate of %p", pcb, origpcb));
   } /* Endif */
   return pcb;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_fork
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Prepends a fragment to a received PCB to allow it to be sent.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_fork
   (
      RTCSPCB_PTR    pcb
         /* [IN] packet to fork */
   )
{ /* Body */
   PCB_FRAGMENT _PTR_ pcb_frag_ptr;

   if (pcb->LASTFRAG != 0) {
      PCBLOG(("\nPCB: Fork: can't fork %p", pcb));
      return RTCSERR_PCB_FORK;
   } /* Endif */

   if (!pcb->DUPPTR) {
      pcb->PCB_PTR->FREE    = pcb->PCB_FREE;
      pcb->PCB_PTR->PRIVATE = pcb->PCB_PRIVATE;
      pcb->PCB_PTR->FRAG[0] = pcb->PCB_BUFFER;
      pcb->PCB_ORIG = pcb->PCB_PTR;
   } /* Endif */
   pcb->PCB_FREE = NULL;

   pcb->PCB_PTR = &pcb->PCB;
   /* Start SPR P122-0266-24 remove all warnings from RTCS code. */
   /*pcb->PCB.FRAG[0].LENGTH   = 0; */
   /*pcb->PCB.FRAG[0].FRAGMENT = (uchar_ptr)&pcb->PCB; */
   /*pcb->PCB.FRAG[1].LENGTH   = pcb->FRAGUSED; */
   /*pcb->PCB.FRAG[1].FRAGMENT = RTCSPCB_DATA(pcb); */
   /*pcb->PCB.FRAG[2].LENGTH   = 0; */
   /*pcb->PCB.FRAG[2].FRAGMENT = NULL; */
   pcb_frag_ptr = pcb->PCB.FRAG;
   pcb_frag_ptr->LENGTH   = 0;
   pcb_frag_ptr->FRAGMENT = (uchar_ptr)&pcb->PCB; /* grows down into BUFFER */
   pcb_frag_ptr++;
   pcb_frag_ptr->LENGTH   = pcb->FRAGUSED;
   pcb_frag_ptr->FRAGMENT = RTCSPCB_DATA(pcb);
   pcb_frag_ptr++;
   pcb_frag_ptr->LENGTH   = 0;
   pcb_frag_ptr->FRAGMENT = NULL;
   /* End SPR P122-0266-24. */
   
   pcb->CL       = RTCSPCB_LAYER_APPLICATION;
   pcb->FRAGUSED = 0;
   pcb->FRAGFREE = RTCSPCB_DATA_MAX;
   pcb->LASTFRAG = 2;
   RTCSPCB_DATA(pcb) = pcb->PCB.FRAG[0].FRAGMENT;

   PCBLOG(("\nPCB: Fork: forked %p", pcb));
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_free_internal
*  Returned Value : void
*  Comments       :
*        Frees an allocated PCB.
*
*END*-----------------------------------------------------------------*/

void RTCSPCB_free_internal
   (
      PCB_PTR   inpcb
         /* [IN] packet to free */
   )
{ /* Body */
   RTCSPCB_PTR pcb = (RTCSPCB_PTR)inpcb->PRIVATE;

   /* Can't free this PCB until all duplicates are freed */
   if (RTCS_sem_trywait(&pcb->DUPCOUNT) == RTCS_OK) {
      PCBLOG(("\nPCB: Free_internal: delaying %p", pcb));
      return;
   } /* Endif */

   /* If this PCB is a duplicate, decrement the original's refcount */
   if (pcb->DUPPTR) {
      PCBLOG(("\nPCB: Free_internal: about to free %p", pcb->DUPPTR));
      RTCSPCB_free(pcb->DUPPTR);
   } /* Endif */

   /* If this PCB has been forked, don't forget the link layer's PCB */
   if (pcb->PCB_ORIG) {
      PCB_free(pcb->PCB_ORIG);
   } /* Endif */

   /* handle the FREE_FRAG_FIELD */
   if (pcb->FREE_FRAG_BITFIELD) {
      uint_32  i;
      for (i = 0; i < RTCSPCB_FRAG_MAX; i++) {
         if (pcb->FREE_FRAG_BITFIELD & (1 << i)) {
            _mem_free(pcb->PCB_PTR->FRAG[i].FRAGMENT);
         } /* Endif */
      } /* Endfor */
      pcb->FREE_FRAG_BITFIELD = 0;
   } /* Endif */

   if (pcb->PCB_FREE) {
      inpcb->FREE    = pcb->PCB_FREE;
      inpcb->PRIVATE = pcb->PCB_PRIVATE;
      inpcb->FRAG[0] = pcb->PCB_BUFFER;
      PCBLOG(("\nPCB: Free_internal: PCB is %p", inpcb));
      PCB_free(inpcb);
   } /* Endif */

   if (pcb->UDP_REQUEST) {
      RTCS_cmd_complete(pcb->UDP_REQUEST, RTCS_OK);
   } /* Endif */

   _partition_free(pcb);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_insert
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Prepend a layer header in a PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_insert
   (
      RTCSPCB_PTR    pcb,
         /* [IN] packet to change */
      uint_32        size
         /* [IN] size of new header */
   )
{ /* Body */
   uchar_ptr data_ptr;

   if (pcb->CL == 0) {
      PCBLOG(("\nPCB: Insert: out of layers in %p", pcb));
      return RTCSERR_PCB_LAYER;
   } /* Endif */

   if (size > pcb->FRAGFREE) {
      PCBLOG(("\nPCB: Insert: out of space in %p (%d/%d)", pcb, size, pcb->FRAGFREE));
      return RTCSERR_PCB_BUFFER;
   } /* Endif */

   pcb->FRAGFREE     -= size;
   pcb->FRAGUSED     += size;
   RTCSPCB_SIZE(pcb) += size;

   data_ptr = RTCSPCB_DATA(pcb);
   pcb->CL--;
   data_ptr -= size;
   RTCSPCB_DATA(pcb) = data_ptr;

   PCBLOG(("\nPCB: Insert: inserted %d bytes in %p", size, pcb));
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_next
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Advance a layer header in a PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_next
   (
      RTCSPCB_PTR    pcb,
         /* [IN] packet to change */
      uint_32        size
         /* [IN] size of last header */
   )
{ /* Body */
   uchar_ptr data_ptr;

   if (pcb->CL == RTCSPCB_LAYERS_MAX-1) {
      PCBLOG(("\nPCB: Next: out of layers in %p", pcb));
      return RTCSERR_PCB_LAYER;
   } /* Endif */

   if (size > pcb->FRAGUSED) {
      PCBLOG(("\nPCB: Next: out of space in %p (%d/%d)", pcb, size, pcb->FRAGUSED));
      return RTCSERR_PCB_BUFFER;
   } /* Endif */

   pcb->FRAGFREE     += size;
   pcb->FRAGUSED     -= size;
   RTCSPCB_SIZE(pcb) -= size;

   data_ptr = RTCSPCB_DATA(pcb);
   pcb->CL++;
   data_ptr += size;
   RTCSPCB_DATA(pcb) = data_ptr;

   PCBLOG(("\nPCB: Next: skipped %d bytes in %p", size, pcb));
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_shrink
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Shrinks the first fragment of a PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_shrink
   (
      RTCSPCB_PTR    pcb,
         /* [IN] packet to change */
      uint_32        size
         /* [IN] size to remove */
   )
{ /* Body */

   if (size > pcb->FRAGUSED) {
      PCBLOG(("\nPCB: Shrink: out of space in %p (%d/%d)", pcb, size, pcb->FRAGUSED));
      return RTCSERR_PCB_BUFFER;
   } /* Endif */

   pcb->FRAGUSED     -= size;
   RTCSPCB_SIZE(pcb) -= size;

   PCBLOG(("\nPCB: Shrink: shrunk %d bytes in %p", size, pcb));
   return RTCS_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_frag_wfree
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Adds a fragment to a PCB, and get RTCSPCB_free to _mem_free()
*        it when called.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_frag_wfree
   (
      RTCSPCB_PTR    pcb,
         /* [IN] packet to change */
      uint_32        size,
         /* [IN] size of new fragment */
      uchar_ptr      frag
   )
{ /* Body */
   uint_32 lastfrag = pcb->LASTFRAG;
   uint_32 error = RTCSPCB_frag(pcb, size, frag);

   if (error == RTCS_OK) {
      pcb->FREE_FRAG_BITFIELD |= 1 << lastfrag;
   } /* Endif */
   return error;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_frag
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Adds a fragment to a PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_frag
   (
      RTCSPCB_PTR    pcb,
         /* [IN] packet to change */
      uint_32        size,
         /* [IN] size of new fragment */
      uchar_ptr      frag
         /* [IN] pointer to new fragment */
   )
{ /* Body */

   if (pcb->LASTFRAG == 0) {
      PCBLOG(("\nPCB: Frag: must fork %p", pcb));
      return RTCSERR_PCB_NOFRAG;
   } /* Endif */

   if (pcb->LASTFRAG >= RTCSPCB_FRAG_MAX) {
      PCBLOG(("\nPCB: Frag: out of fragments in %p", pcb));
      return RTCSERR_PCB_FRAG;
   } /* Endif */

   pcb->PCB_PTR->FRAG[pcb->LASTFRAG].LENGTH   = size;
   pcb->PCB_PTR->FRAG[pcb->LASTFRAG].FRAGMENT = frag;
   RTCSPCB_SIZE(pcb) += size;
   pcb->LASTFRAG++;
   pcb->PCB_PTR->FRAG[pcb->LASTFRAG].LENGTH   = 0;
   pcb->PCB_PTR->FRAG[pcb->LASTFRAG].FRAGMENT = NULL;

   PCBLOG(("\nPCB: Frag: added %d bytes to %p", size, pcb));
   return RTCS_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_memcopy
*  Returned Value : void
*  Comments       :
*        Copies data out from a PCB.
*
*END*-----------------------------------------------------------------*/

void RTCSPCB_memcopy
   (
      RTCSPCB_PTR    pcb,
         /* [IN] packet to copy */
      pointer        dest,
         /* [IN] destination */
      uint_32        offset,
         /* [IN] offset into PCB */
      uint_32        dest_size
         /* [IN] number of bytes to copy */
   )
{ /* Body */
   PCB_FRAGMENT_PTR  frag_ptr = pcb->PCB_PTR->FRAG;
   uchar_ptr         src_ptr  = RTCSPCB_DATA(pcb);
   uchar_ptr         dest_ptr = dest;
   uint_32           src_size = pcb->FRAGUSED;

   while (src_size < offset) {

      offset -= src_size;
      frag_ptr++;
      src_ptr  = frag_ptr->FRAGMENT;
      src_size = frag_ptr->LENGTH;

   } /* Endwhile */

   src_ptr  += offset;
   src_size -= offset;

   while (src_size < dest_size) {

      _mem_copy(src_ptr, dest_ptr, src_size);
      dest_ptr  += src_size;
      dest_size -= src_size;
      frag_ptr++;
      src_ptr  = frag_ptr->FRAGMENT;
      src_size = frag_ptr->LENGTH;

   } /* Endwhile */

   _mem_copy(src_ptr, dest_ptr, dest_size);

} /* Endbody */

/* EOF */
