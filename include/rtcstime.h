#ifndef __rtcstime_h__
#define __rtcstime_h__
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
*** File: rtcstime.h
***
*** Comments:
***      TCP/IP internal Time Server definitions
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Type definitions
**
*/

/*
** A timer event
*/
typedef struct tcpip_event {

   uint_32                    TIME;
   boolean (_CODE_PTR_        EVENT)(struct tcpip_event _PTR_);
   pointer                    PRIVATE;

   struct tcpip_event _PTR_   NEXT;

} TCPIP_EVENT, _PTR_ TCPIP_EVENT_PTR;


/***************************************
**
** Prototypes
**
*/

void TCPIP_Event_init(void);

void TCPIP_Event_add(TCPIP_EVENT_PTR);
void TCPIP_Event_cancel(TCPIP_EVENT_PTR);

uint_32 TCPIP_Event_expire(TCPIP_EVENT_PTR);

uint_32 TCPIP_Event_time(uint_32);

#define TCPIP_Event_launch(var, time, event, private) \
         {                             \
            (var)->TIME = time;        \
            (var)->EVENT = event;      \
            (var)->PRIVATE = private;  \
            TCPIP_Event_add(var);      \
         }

#endif
/* EOF */
