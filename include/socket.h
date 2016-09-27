#ifndef __socket_h__
#define __socket_h__
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
*** File: socket.h
***
*** Comments:  Constants and prototype declarations which are internal
***            to the SOCKET module.
***
************************************************************************
*END*******************************************************************/


/*
** Connectionless-mode socket states
*/
#define SOCKSTATE_DGRAM_GROUND      0
#define SOCKSTATE_DGRAM_BOUND       1
#define SOCKSTATE_DGRAM_OPEN        2

/*
** Connection-mode socket states
*/
#define SOCKSTATE_STREAM_GROUND     0
#define SOCKSTATE_STREAM_BOUND      1
#define SOCKSTATE_STREAM_LISTENING  2
#define SOCKSTATE_STREAM_CONNECTED  3


/*
**  socket-specific structures
*/
struct sockselect_parm;
typedef struct socket_config {

   boolean                       INITIALIZED;
   uint_32                       CURRENT_SOCKETS;
   pointer                       SOCKET_HEAD;
   pointer                       SOCKET_TAIL;
   struct sockselect_parm _PTR_  SELECT_HEAD;
   pointer                       SELECT_TIME_HEAD;    /* not used */
   _rtcs_mutex                   SOCK_MUTEX;

} SOCKET_CONFIG_STRUCT, _PTR_ SOCKET_CONFIG_STRUCT_PTR;


/****************************************************
** extern statements for socket procedures          *
*****************************************************/

extern SOCKET_STRUCT_PTR   SOCK_Get_sock_struct
(
   RTCS_SOCKET_CALL_STRUCT_PTR   type,
   _rtcs_taskid                  owner
);
extern void SOCK_Free_sock_struct
(
   SOCKET_STRUCT_PTR          socket_ptr
);

extern boolean SOCK_Remove_owner( SOCKET_STRUCT_PTR, pointer );
extern boolean SOCK_Is_owner( SOCKET_STRUCT_PTR, pointer );
extern boolean SOCK_Add_owner( SOCKET_STRUCT_PTR, pointer );


#endif
/* EOF */
