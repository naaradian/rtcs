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
*** File: rtcshost.c
***
*** Comments:  This file contains the default hosts file structure.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


const HOST_ENTRY_STRUCT RTCS_Hosts_list[] =
{
 /* IP Address   Host Name     Aliases          */
  { 0x7F000001, "localhost",    0       },
  { 0xC0A80001, "www.arc.com", {"www"}  },
  { 0xFFFFFFFF, "broadcast",    0       },
  { 0, 0, 0 },
};


/* EOF */
