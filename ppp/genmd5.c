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
*** File: genmd5.c
***
*** Comments:  This file generates the lookup table for the MD5 hash.
***
************************************************************************
*END*******************************************************************/

#include <stdio.h>
#include <math.h>
#define SIZE 64

int main (void)
{ /* Body */
   int count = 0;

   printf("/*\n** The MD5 Lookup Table\n*/\n\n"
          "static uint_32 mdtab[] = {");

   for (;;) {
      if (!(count%4)) printf("\n  ");
      printf(" 0x%08lXl", (long)ldexp(fabs(sin(++count)),32));
      if (count == SIZE) break;
      printf(",");
   } /* Endfor */
   printf("\n};\n");

   return 0;
} /* Endbody */

/* EOF */
