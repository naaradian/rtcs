#ifndef __rip_prv_h__
#define __rip_prv_h__
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
*** File: rip_prv.h
***
*** Comments:
***    Private definitions for the RIP protocol layer.
***
************************************************************************
*END*******************************************************************/

typedef struct {
   TCPIP_EVENT TIMEOUT; /* used for timeout _AND_ garbage collecting */
   uint_32     METRIC;
   uint_32     RT_TAG;
   boolean     CHANGED_F;
   IP_IF_PTR   IPIFSRC; /* the incoming interface of this route */
}  RIP_INFO, _PTR_ RIP_INFO_PTR;

#endif   /* __rip_prv_h__ */
/* EOF */
