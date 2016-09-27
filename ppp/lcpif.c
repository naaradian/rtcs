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
*** File: lcpif.c
***
*** Comments:  This file contains the implementation of the LCP
***            interface routines.
***
************************************************************************
*END*******************************************************************/

#include <ppp.h>
#include "ppp_prv.h"


static uint_32 LCP_send(_ppp_handle, uchar, uchar, PCB_PTR);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_echo
* Returned Value  :  error code
* Comments        :
*     Sends an LCP Echo-Request packet.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_echo
   (
      _ppp_handle       handle,
            /* [IN] - the PPP state structure */
      uchar             id,
            /* [IN] - packet ID */
      PCB_PTR           packet
            /* [IN] - the packet to send */
   )
{ /* Body */
   return LCP_send(handle, LCP_CODE_ECHO_REQ, id, packet);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_discard
* Returned Value  :  error code
* Comments        :
*     Sends an LCP Discard-Request packet.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_discard
   (
      _ppp_handle       handle,
            /* [IN] - the PPP state structure */
      uchar             id,
            /* [IN] - packet ID */
      PCB_PTR           packet
            /* [IN] - the packet to send */
   )
{ /* Body */
   return LCP_send(handle, LCP_CODE_DISC_REQ, id, packet);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_send
* Returned Value  :  error code
* Comments        :
*     Sends an LCP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 LCP_send
   (
      _ppp_handle       handle,
            /* [IN] - the PPP state structure */
      uchar             code,
            /* [IN] - LCP code */
      uchar             id,
            /* [IN] - packet ID */
      PCB_PTR           pcb
            /* [IN] - the packet to send */
   )
{ /* Body */
   PCB_FRAGMENT_PTR  frag_ptr;
   uchar_ptr         outp;
   uint_32           size;

   /*
   ** Make sure the first fragment is long enough for the PPP protocol
   ** field and the LCP header.  This isn't strictly necessary, but
   ** it's impractical to split a 10 byte field over multiple fragments.
   */
   if (pcb->FRAG[0].LENGTH < 10) {
      PCB_free(pcb);
      return PPPERR_PACKET_TOO_SHORT;
   } /* Endif */

   size = 0;
   for (frag_ptr = pcb->FRAG; frag_ptr->LENGTH; frag_ptr++) {
      size += frag_ptr->LENGTH;
   } /* Endfor */
   size -= 2;

   outp = pcb->FRAG[0].FRAGMENT + 2;
   htonc(outp, code); outp++;
   htonc(outp, id);   outp++;
   htons(outp, size); outp += 2;
   htonl(outp, 0);    outp += 4;

   return PPP_send(handle, PPP_PROT_LCP, pcb);

} /* Endbody */


/* EOF */
