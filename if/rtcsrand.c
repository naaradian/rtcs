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
*** File: rtcsrand.c
***
*** Comments:  This file contains the implementation of a random
***            number generator.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>

#define P 0x6C77
#define Q 0x12E1B

static uint_32 seed   = 0;
static uint_32 reseed = 0;
static uint_32 state  = 4;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_rand_seed
* Returned Values : void
* Comments        :
*     Add randomness to the seed.
*
*END*-----------------------------------------------------------------*/

void RTCS_rand_seed
   (
      uint_32 newseed
   )
{ /* Body */
   uint_32 realseed = seed;

   realseed = realseed + newseed;
   realseed = (realseed << 15) | (realseed >> 17);
   realseed = realseed & 0xFFFFFFFFUL;

   seed = realseed;
   reseed = 1;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_rand
* Returned Values : uint_32
* Comments        :
*     Output 32 random bits.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_rand
   (
      void
   )
{ /* Body */
   uint_32 i,j;
   uint_32 pq = P*Q;
   uint_32 square;
   uint_32 randnum;

   if (reseed) {
      uint_32 realseed = seed ^ state;
      uint_32 resp, resq;
      while (realseed >= pq) {
         realseed -= pq;
      } /* Endwhile */
      for (;;) {
         resp = realseed % P;
         resq = realseed % Q;
         if ((resp != 0) && (resp != 1) && (resp != P-1)
          && (resq != 0) && (resq != 1) && (resq != Q-1)) {
            break;
         } /* Endif */
         realseed += 3;
         if (realseed >= pq) realseed -= pq;
      } /* Endfor */
      state = realseed;
      reseed = 0;
   } /* Endif */

   randnum = 0;
   for (i=0; i<32; i++) {
      /* square = state*state mod pq */
      square = 0;
      for (j=0x40000000UL; j; j>>=1) {
         square <<= 1;
         if (square >= pq) square -= pq;
         if (state & j)    square += state;
         if (square >= pq) square -= pq;
      } /* Endfor */
      state = square;
      randnum = (randnum << 1) | (state & 1);
   } /* Endfor */

   return randnum;
} /* Endbody */


/* EOF */
