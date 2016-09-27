#ifndef __checksum_h__
#define __checksum_h__
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
*** File: checksum.h
***
*** Comments:  This file contains the IP checksum function prototypes.
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Code macros
**
*/

#define IP_Sum_invert(s)   (((s) == 0xFFFF) ? (s) : ~(s) & 0xFFFF)


/***************************************
**
** Prototypes
**
*/

uint_16 IP_Sum_immediate
(
   uint_16,             /* [IN] initial sum           */
   uint_16              /* [IN] number to add to sum  */
);

uint_16 IP_Sum_PCB
(
   uint_16,             /* [IN] initial sum           */
   RTCSPCB_PTR          /* [IN] the PCB               */
);

uint_16 IP_Sum_pseudo
(
   uint_16,             /* [IN] initial sum           */
   RTCSPCB_PTR,         /* [IN] the PCB               */
   int_32               /* [IN] IP layer              */
);

#endif
/* EOF */
