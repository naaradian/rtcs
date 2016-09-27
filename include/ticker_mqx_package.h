#ifndef __ticker_h__
#define __ticker_h__
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
*** File: ticker.h
***
*** Comments:  Timer interface definitions for the TCP layer.
***
************************************************************************
*END*******************************************************************/


/************************************************************************/
/* Constants                                                            */
/************************************************************************/

#ifdef _MSDOS_
#define TICK_LENGTH   55   /* in milliseconds */
#else
#define TICK_LENGTH   5    /* in milliseconds */
#endif

/************************************************************************/
/* Macros                                                               */
/************************************************************************/

#define TimerInit(timeq_ptr)  (_Zeromem((void _PTR_)(timeq_ptr), sizeof(TimeQ)))

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/*  The following structure is used for delays (where client blocks),
 *   oneshot timers, and cyclic timers, i.e. all items scheduled by
 *   the TickerServer.
 */
typedef struct TimeQ {

    struct TimeQ  _PTR_ next;
    struct TimeQ  _PTR_ prev;

    struct TimeQ  _PTR_ _PTR_ donelist;
                              /* where to put completed oneshots */

    struct TimeQ  _PTR_ _PTR_ curlist;
                              /* the list which currently holds the
                              ** timer (usually &qhead or donelist),
                              ** 0 if none */

    uint_16       RESERVED;
    uint_16       count;      /* nb of timeouts (ignored for delay) */
    int_32        delay;      /* time delta before this node done */
    uint_32       abs_to;     /* time at which this node should timeout */
    int_32        reload;     /* <0==delay, 0==oneshot, >0==cyclic
                              ** timer's repeated delay */

/*  task_id       client; */  /* task to Signal() (timers) or
                                 Reply() to (delays); cleared when
                                 oneshot or delay has expired;
                                 if set, this means this timer is
                                 active, and in the schedule list
                                 (`qhead', see ticker.c) */

} TimeQ;

/************************************************************************/
/* Functions                                                            */
/************************************************************************/

void TCP_Timer_schedule
(
   TimeQ _PTR_       ,        /* IN/OUT - The timer to schedule */
   TimeQ _PTR_ _PTR_          /* IN/OUT - The head of the timer queue */
);

void  TCP_Timer_expire
(
   TimeQ _PTR_                /* IN/OUT - The expired timer */
);

void  TCP_Timer_remove
(
   TimeQ _PTR_                /* IN/OUT - The timer to remove */
);

int_32 TCP_Timer_stop
(
   TimeQ _PTR_       ,        /* The timer to stop */
   TimeQ _PTR_ _PTR_          /* The timer queue head */
);

int_32 TCP_Timer_start
(
   TimeQ _PTR_       ,        /* IN/OUT - The timer */
   int_32            ,        /* IN     - No. msec until timeout */
   int_32            ,        /* IN     - No. msec to dly after 1st to */
   TimeQ _PTR_ _PTR_          /* IN/OUT - Head of timer queue */
);

int_32 TCP_Timer_advance
(
   TimeQ _PTR_       ,        /* IN/OUT - timer to reschedule */
   int_32            ,        /* IN     - no. msec to increment timeout */
   TimeQ _PTR_ _PTR_          /* IN/OUT - timer queue head */
);

int_32 TCP_Timer_oneshot_max
(
   TimeQ _PTR_       ,        /* IN/OUT - timer to start */
   int_32            ,        /* IN     - timeout */
   TimeQ _PTR_ _PTR_          /* IN/OUT - timer queue head */
);

uint_32 TCP_Tick_server
(
   void
);

#endif
/* EOF */
