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
*** File: timer.c
***
*** Comments:  TCP/IP internal Time Service implementation
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcstime.h"

TCPIP_EVENT_PTR TCPIP_Event_head;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : TCPIP_Event_init
*  Returned Value : void
*  Comments       :
*        Initializes the timer queue.
*
*END*-----------------------------------------------------------------*/

void TCPIP_Event_init
   (
      void
   )
{ /* Body */

   TCPIP_Event_head = NULL;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : TCPIP_Event_add
*  Returned Value : void
*  Comments       :
*        Adds an event to the timer queue.
*
*END*-----------------------------------------------------------------*/

void TCPIP_Event_add
   (
      TCPIP_EVENT_PTR   event
         /* [IN] the event */
   )
{ /* Body */
   TCPIP_EVENT_PTR _PTR_   search_ptr;

   for (search_ptr = &TCPIP_Event_head;;
        search_ptr = &(*search_ptr)->NEXT) {

      /* Append to tail of list */
      if (*search_ptr == NULL) {
         event->NEXT = NULL;
         *search_ptr = event;
         break;
      } /* Endif */

      /* Insert in middle (or head) of list */
      if ((*search_ptr)->TIME > event->TIME) {
         (*search_ptr)->TIME -= event->TIME;
         event->NEXT = *search_ptr;
         *search_ptr = event;
         break;
      } /* Endif */

      event->TIME -= (*search_ptr)->TIME;

   } /* Endfor */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : TCPIP_Event_cancel
*  Returned Value : void
*  Comments       :
*        Removes an event from the timer queue.
*
*END*-----------------------------------------------------------------*/

void TCPIP_Event_cancel
   (
      TCPIP_EVENT_PTR   event
         /* [IN] the event */
   )
{ /* Body */
   TCPIP_EVENT_PTR _PTR_   search_ptr;

   for (search_ptr = &TCPIP_Event_head;
       *search_ptr;
        search_ptr = &(*search_ptr)->NEXT) {

      /* Found it */
      if (*search_ptr == event) {
         if (event->NEXT) {
            event->NEXT->TIME += event->TIME;
         } /* Endif */
         *search_ptr = event->NEXT;
         break;
      } /* Endif */

   } /* Endfor */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : TCPIP_Event_expire
*  Returned Value : time until event expires, 0 if event is not started
*  Comments       :
*        Calculates the time until an event expires.
*
*END*-----------------------------------------------------------------*/

uint_32 TCPIP_Event_expire
   (
      TCPIP_EVENT_PTR   event
         /* [IN] the event */
   )
{ /* Body */
   TCPIP_EVENT_PTR   search;
   uint_32           time = 0;

   for (search = TCPIP_Event_head;
        search;
        search = search->NEXT) {

      time += search->TIME;

      /* Found it */
      if (search == event) {
         return time;
      } /* Endif */

   } /* Endfor */

   /* Event is not in list */
   return 0;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : TCPIP_Event_time
*  Returned Value : time until next event (0 if none)
*  Comments       :
*        Notifies the time server of elapsed time.
*
*END*-----------------------------------------------------------------*/

uint_32 TCPIP_Event_time
   (
      uint_32  time
         /* [IN] time elapsed */
   )
{ /* Body */
   TCPIP_EVENT_PTR   event;

   while (TCPIP_Event_head) {

      /* Check if the head of the timer queue had expired */
      if (TCPIP_Event_head->TIME > time) {
         TCPIP_Event_head->TIME -= time;
         return TCPIP_Event_head->TIME;
      } /* Endif */

      /* The head has expired -- dequeue it */
      event = TCPIP_Event_head;
      TCPIP_Event_head = event->NEXT;
      time -= event->TIME;

      /* Now trigger it */
      if ((*event->EVENT)(event)) {
         event->TIME += time;
         TCPIP_Event_add(event);
      } /* Endif */

   } /* Endwhile */

   /* Timer queue is empty */
   return 0;

} /* Endbody */


/* EOF */
