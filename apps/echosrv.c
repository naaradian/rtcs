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
*** File: echosrv.c
***
*** Comments:  This file contains an implementation of an
***            RFC 862 Echo server.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>

//#define BUFFER_SIZE  1460
#define BUFFER_SIZE  1500
#define BUFFER_DATA_SIZE  1460
//#define BUFFER_SIZE  900 //091218
static char buffer[BUFFER_SIZE];
static char buffer_rad[BUFFER_SIZE];

char buffer_rad_T[BUFFER_SIZE];
unsigned long  buffer_rad_T_len;

//_______________________________________________________101125
char buffer_rad_TU[BUFFER_SIZE];
unsigned long  buffer_rad_T_lenU;
extern "C" void ToBufferIPU(char *, unsigned long);
//_______________________________________________________101125

void ECHOSRV_task(pointer, pointer);
void RDNSRV_task(pointer, pointer);
void RDNSRVT_task(pointer, pointer);

extern "C" void ToBufferIP(char *, unsigned long);


//extern unsigned long shutdown_cnt;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : ECHOSRV_init
* Returned Value   : error code
* Comments  :  Start the port 7 echo server.
*
*END*-----------------------------------------------------------------*/

uint_32 ECHOSRV_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, ECHOSRV_task, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : ECHO_task
* Returned Value   : none
* Comments  :  Simple Echo server.
*
*END*-----------------------------------------------------------------*/

void ECHO_task
   (
      uint_32 dummy
   )
{ /* Body */
   ECHOSRV_task(NULL, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : ECHOSRV_task
* Returned Value   : none
* Comments  :  Simple Echo server.
*
*END*-----------------------------------------------------------------*/

void ECHOSRV_task
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   sockaddr_in    laddr, raddr;
   uint_32        sock, listensock;
   uint_32        error;
   uint_32        optval, optlen;
   int_32         length;
   uint_16        rlen;

   /* Echo server services port 7 */
   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_ECHO;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   sock = socket(PF_INET, SOCK_DGRAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Listen on TCP port */
   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   error = listen(sock, 0);
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   listensock = sock;

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {

      sock = RTCS_selectall(0);

      if (sock == listensock) {

         /* Connection requested; accept it */
         rlen = sizeof(raddr);
         accept(listensock, &raddr, &rlen);

      } else {

         /* Get the socket type */
         getsockopt(sock, SOL_SOCKET, RTCS_SO_TYPE, (char_ptr)&optval, &optlen);

         if (optval == SOCK_STREAM) {

//_int_disable();

            length = recv(sock, buffer, BUFFER_SIZE, 0);
            if (length == RTCS_ERROR) {
               shutdown(sock, FLAG_CLOSE_TX);
            } else {
               send(sock, buffer, length, 0);
            } /* Endif */
//_int_enable();

         } else { /* optval == SOCK_DGRAM */

            rlen = sizeof(raddr);
            length = recvfrom(sock, buffer, BUFFER_SIZE, 0, &raddr, &rlen);
            sendto(sock, buffer, length, 0, &raddr, rlen);

         } /* Endif */

      } /* Endif */

   } /* Endfor */

} /* Endbody */


//_____________________________added for radian

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RDNSRV_init
* Returned Value   : error code
* Comments  :  Start the port 155  server.
*
*END*-----------------------------------------------------------------*/

uint_32 RDNSRV_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, RDNSRV_task, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : RDN_task
* Returned Value   : none
* Comments  :  Simple RDN server.
*
*END*-----------------------------------------------------------------*/

void RDN_task
   (
      uint_32 dummy
   )
{ /* Body */
   RDNSRV_task(NULL, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : RDNSRV_task
* Returned Value   : none
* Comments  :  Simple RDN server.
*
*END*-----------------------------------------------------------------*/

void RDNSRV_task
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   sockaddr_in    laddr, raddr;
   uint_32        sock, listensock;
   uint_32        error;
   uint_32        optval, optlen;
   int_32         length;
   uint_16        rlen;
// char loc_buffer[BUFFER_SIZE];
//unsigned long loc_len;
//char *buffs;
//char* buffd;

   /* Echo server services port 7 */
   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_RADIAN;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   sock = socket(PF_INET, SOCK_DGRAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Listen on TCP port */
   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   error = listen(sock, 0);
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   listensock = sock;

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {
/*
#ifdef PROG_ACT155
wd_reset_enabled = 1; 
#endif //PROG_ACT155
*/

      sock = RTCS_selectall(0);

      if (sock == listensock) {

         /* Connection requested; accept it */
         rlen = sizeof(raddr);
         accept(listensock, &raddr, &rlen);

      } else {

         /* Get the socket type */
         getsockopt(sock, SOL_SOCKET, RTCS_SO_TYPE, (char_ptr)&optval, &optlen);

         if (optval == SOCK_STREAM) {

            length = recv(sock, buffer_rad, BUFFER_SIZE, 0);
            if (length == RTCS_ERROR) {
//shutdown_cnt++;
              shutdown(sock, FLAG_CLOSE_TX);      //100226

            } else {
        //echo make off       send(sock, buffer_rad, length, 0);
//_int_disable();
	ToBufferIP(buffer_rad, length); //t 090606

//________________________________________________________________________>send back data from internal channel
		if(buffer_rad_T_len)
		{
		 if(buffer_rad_T_len > BUFFER_DATA_SIZE)
		  {
		    buffer_rad_T_len = BUFFER_DATA_SIZE;
		  }

//		buffer_rad_T_len  = 1501; //t100525
		send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
		buffer_rad_T_len = 0;
		}
		else
		{
		 send(sock, "U", 1,0);
		}
//________________________________________________________________________<send back data from internal channel
//_int_enable();

//t090606	ToBufferIP(buffer_rad, length);
            } /* Endif */

         } else { /* optval == SOCK_DGRAM */

            rlen = sizeof(raddr);
            length = recvfrom(sock, buffer_rad, BUFFER_SIZE, 0, &raddr, &rlen);
//_______________________________________________________________________101125
	ToBufferIPU(buffer_rad, length); //t 090606
		if(buffer_rad_T_lenU)
		{
		 if(buffer_rad_T_lenU > BUFFER_DATA_SIZE)
		  {
		    buffer_rad_T_lenU = BUFFER_DATA_SIZE;
		  }
		 sendto(sock,  buffer_rad_TU, buffer_rad_T_lenU, 0, &raddr, rlen);
		buffer_rad_T_lenU = 0;
		}
		else
		{
	//	sendto(sock, buffer_rad, length, 0, &raddr, rlen);
		sendto(sock,"U",1, 0, &raddr, rlen);

		}
//______________________________________________________________________101125
//101125            sendto(sock, buffer_rad, length, 0, &raddr, rlen);

         } /* Endif */ //SOCK_DGRAM 

      } /* Endif */ // no listensock

   } /* Endfor */

} /* Endbody */

//_____________________________added for radian 1

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RDNSRVT_init
* Returned Value   : error code
* Comments  :  Start the port 156  server.
*
*END*-----------------------------------------------------------------*/

uint_32 RDNSRVT_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, RDNSRVT_task, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : RDNT_task
* Returned Value   : none
* Comments  :  Simple RDN server.
*
*END*-----------------------------------------------------------------*/

void RDNT_task
   (
      uint_32 dummy
   )
{ /* Body */
   RDNSRVT_task(NULL, NULL);
} /* Endbody */


//__________________________________________________
/*TASK*-----------------------------------------------------------------
*
* Function Name    : RDNSRVT_task
* Returned Value   : none
* Comments  :  Simple RDN server.
*
*END*-----------------------------------------------------------------*/

void RDNSRVT_task
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   sockaddr_in    laddr, raddr;
   uint_32        sock, listensock;
   uint_32        error;
   uint_32        optval, optlen;
   int_32         length;
   uint_16        rlen;
// char loc_buffer[BUFFER_SIZE];
//unsigned long loc_len;
//char *buffs;
//char* buffd;

   /* Echo server services port  */
   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_RADIANT;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   sock = socket(PF_INET, SOCK_DGRAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Listen on TCP port */
   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   error = listen(sock, 0);
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   listensock = sock;

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {
/*
#ifdef PROG_ACT155
wd_reset_enabled = 1; 
#endif //PROG_ACT155
*/

      sock = RTCS_selectall(0);

      if (sock == listensock) {

         /* Connection requested; accept it */
         rlen = sizeof(raddr);
         accept(listensock, &raddr, &rlen);

      } else {

         /* Get the socket type */
         getsockopt(sock, SOL_SOCKET, RTCS_SO_TYPE, (char_ptr)&optval, &optlen);

         if (optval == SOCK_STREAM) {

            length = recv(sock, buffer_rad, BUFFER_SIZE, 0);
            if (length == RTCS_ERROR) {
//shutdown_cnt++;
              shutdown(sock, FLAG_CLOSE_TX);      //100226

            } else {
        //echo make off       send(sock, buffer_rad, length, 0);

	ToBufferIP(buffer_rad, length); //t 090606

//________________________________________________________________________>send back data from internal channel
		if(buffer_rad_T_len)
		{
		 if(buffer_rad_T_len > BUFFER_DATA_SIZE)
		  {
		    buffer_rad_T_len = BUFFER_DATA_SIZE;
		  }

//		buffer_rad_T_len  = 1501; //t100525
		send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
		buffer_rad_T_len = 0;
		}
		else
		{
		 send(sock, "U", 1,0);
		}
//________________________________________________________________________<send back data from internal channel


//t090606	ToBufferIP(buffer_rad, length);
            } /* Endif */

         } else { /* optval == SOCK_DGRAM */

            rlen = sizeof(raddr);
            length = recvfrom(sock, buffer_rad, BUFFER_SIZE, 0, &raddr, &rlen);
//_______________________________________________________________________101125
	ToBufferIPU(buffer_rad, length); //t 090606
		if(buffer_rad_T_lenU)
		{
		 if(buffer_rad_T_lenU > BUFFER_DATA_SIZE)
		  {
		    buffer_rad_T_lenU = BUFFER_DATA_SIZE;
		  }
		 sendto(sock,  buffer_rad_TU, buffer_rad_T_lenU, 0, &raddr, rlen);
		buffer_rad_T_lenU = 0;
		}
		else
		{
	//	sendto(sock, buffer_rad, length, 0, &raddr, rlen);
		sendto(sock,"U",1, 0, &raddr, rlen);

		}
//______________________________________________________________________101125
//101125            sendto(sock, buffer_rad, length, 0, &raddr, rlen);

         } /* Endif */ //SOCK_DGRAM 

      } /* Endif */ // no listensock

   } /* Endfor */

} /* Endbody */


//__________________________________________________



/*TASK*-----------------------------------------------------------------
*
* Function Name    : RDNSRVT_task_old
* Returned Value   : none
* Comments  :  Simple RDN server.
*
*END*-----------------------------------------------------------------*/

void RDNSRVT_task_old
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   sockaddr_in    laddr, raddr;
   uint_32        sock, listensock;
   uint_32        error;
   uint_32        optval, optlen;
   int_32         length;
   uint_16        rlen;

   /* Echo server services port  */
  // laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_RADIANT;
 //laddr.sin_port        = IPPORT_RADIAN;
//101125   laddr.sin_addr.s_addr = INADDR_MY; // INADDR_ANY;
  laddr.sin_addr.s_addr =  INADDR_ANY;
   

   /* Listen on TCP port */
   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */


   error = bind(sock, &laddr, sizeof(laddr));
  if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   error = listen(sock, 0);
   if (error) {
      RTCS_task_exit(creator, error);
  } /* Endif */
   //listensock = sock;

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {
/*
	if( buffer_rad_T_len)
	//if( (buffer_rad_T_len) &&(buffer_rad_T_len != 100) )
	{

//	printf("Start!");
//	sock = listensock;
	error = connect(sock, &laddr, sizeof(laddr));
  	 	if (error != RTCS_OK) 
		{
	//	printf("Error!");
      		 shutdown(sock, FLAG_ABORT_CONNECTION);
     ;  	 	}
		else
		{
	//	printf("Send!");
		send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//		shutdown(sock, FLAG_CLOSE_TX);
	//	buffer_rad_T_len = 15;//t
		buffer_rad_T_len = 0;
		}
	}
	else
	{
	 //	printf(".");	 
	//	buffer_rad_T_len =100;
	}

   */
   sock = RTCS_selectall(0);

      if (sock == listensock) {

         /* Connection requested; accept it */
         rlen = sizeof(raddr);
         accept(listensock, &raddr, &rlen);

      } else {

         /* Get the socket type */
         getsockopt(sock, SOL_SOCKET, RTCS_SO_TYPE, (char_ptr)&optval, &optlen);

         if (optval == SOCK_STREAM) {

            length = recv(sock, buffer_rad, BUFFER_SIZE, 0);
            if (length == RTCS_ERROR) {
//shutdown_cnt++;
              shutdown(sock, FLAG_CLOSE_TX);      //100226

            } else {
        //echo make off       send(sock, buffer_rad, length, 0);

	ToBufferIP(buffer_rad, length); //t 090606

//________________________________________________________________________>send back data from internal channel
		if(buffer_rad_T_len)
		{
		 if(buffer_rad_T_len > BUFFER_DATA_SIZE)
		  {
		    buffer_rad_T_len = BUFFER_DATA_SIZE;
		  }

//		buffer_rad_T_len  = 1501; //t100525
		send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
//send(sock, buffer_rad_T, buffer_rad_T_len, 0);
		buffer_rad_T_len = 0;
		}
		else
		{
		 send(sock, "U", 1,0);
		}
//________________________________________________________________________<send back data from internal channel


//t090606	ToBufferIP(buffer_rad, length);
            } /* Endif */

         } else { /* optval == SOCK_DGRAM */

            rlen = sizeof(raddr);
            length = recvfrom(sock, buffer_rad, BUFFER_SIZE, 0, &raddr, &rlen);
//_______________________________________________________________________101125
	ToBufferIPU(buffer_rad, length); //t 090606
		if(buffer_rad_T_lenU)
		{
		 if(buffer_rad_T_lenU > BUFFER_DATA_SIZE)
		  {
		    buffer_rad_T_lenU = BUFFER_DATA_SIZE;
		  }
		 sendto(sock,  buffer_rad_TU, buffer_rad_T_lenU, 0, &raddr, rlen);
		buffer_rad_T_lenU = 0;
		}
		else
		{
	//	sendto(sock, buffer_rad, length, 0, &raddr, rlen);
		sendto(sock,"U",1, 0, &raddr, rlen);

		}
//______________________________________________________________________101125
//101125            sendto(sock, buffer_rad, length, 0, &raddr, rlen);

         } /* Endif */ //SOCK_DGRAM 

      } /* Endif */ // no listensock

   } /* Endfor */

} /* Endbody */


/* EOF */
