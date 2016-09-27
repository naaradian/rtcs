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
*** File: gen16.c
***
*** Comments:  This file generates the lookup table for the CCITT-16
***            FCS.
***
************************************************************************
*END*******************************************************************/

#include <stdio.h>
#define SIZE 256
#define POLY 0x8408

int main (void)
{ /* Body */
   int count = 0, i;
   register int carry;
   unsigned short crc;

   printf("/*\n** The CCITT-16 Lookup Table\n*/\n\n"
          "static uint_16 fcstab[] = {");

   for (;;) {
      if (!(count%8)) printf("\n  ");
      crc = count;
      for (i=8;i--;) {
         carry = crc & 1;
         crc >>= 1;
         if (carry) crc ^= POLY;
      } /* Endfor */
      printf(" 0x%04X", crc);
      if (++count == SIZE) break;
      printf(",");
   } /* Endfor */
   printf("\n};\n");

   return 0;
} /* Endbody */

/* EOF */
