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
*** File: rtcsinit.c
***
*** Comments:  This file contains the default protocol table for RTCS.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


uint_32 (_CODE_PTR_ RTCS_protocol_table[])(void) = {
   RTCSPROT_IGMP,
   RTCSPROT_UDP,
   RTCSPROT_TCP,
   NULL
};


/* EOF */
