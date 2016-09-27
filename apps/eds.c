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
*** File: eds.c
***
*** Comments:  Source for the Embedded Debug Server.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


#define EDS_PORT          5002
#define EDS_TASK          10
#define EDS_BIG_ENDIAN    0L
#define EDS_LITTLE_ENDIAN 0xFFFFFFFF

/* server operations */
#define EDS_IDENTIFY 1L
#define EDS_READ     2L
#define EDS_WRITE    3L
#define EDS_LITTLE_ENDIAN_IDENTIFY 0x01000000

/* error codes */
#define EDS_INVALID_OP   1L
#define EDS_INVALID_SIZE 2L
#define EDS_OK           0L

/* structure declarations */
typedef struct op_struct {
   uchar OPERATION[4];   /* server operation          */
   uchar ADDRESS[4];     /* read/write memory address */
   uchar ADDRESS2[4];    /* extra address field       */
   uchar SIZE[4];        /* size of buffer            */
   uchar PROCESSOR[4];   /* processor type            */
   uchar ENDIAN[4];      /* endian of processor       */
   uchar EDS_ERROR[4];   /* error code                */
} EDS_OP_STRUCT, _PTR_ EDS_OP_STRUCT_PTR;

/* macros */
#define EDS_OP_STRUCT_SIZE (sizeof(EDS_OP_STRUCT))
#define EDS_BUFFER_SIZE    (1472 - EDS_OP_STRUCT_SIZE)

#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 8)

#define READ_BYTES(dest,src,size)   _mem_copy(dest,src,size)
#define WRITE_BYTES(dest,src,size)  _mem_copy(dest,src,size)

#elif (PSP_ENDIAN == MQX_BIG_ENDIAN)

#define READ_BYTES(src,dest,size) \
                              {\
                                 uint_32 i, j;\
                                 uint_32 _PTR_ src_lp;\
                                 uchar   _PTR_ dest_lp;\
                                 src_lp = (src);\
                                 dest_lp = (dest);\
                                 for(i=0;i<(size);i+=4) {\
                                    j = *src_lp++;\
                                    *dest_lp++ = (j >> 24) & 0xFF;\
                                    *dest_lp++ = (j >> 16) & 0xFF;\
                                    *dest_lp++ = (j >>  8) & 0xFF;\
                                    *dest_lp++ = (j)       & 0xFF;\
                                 }\
                              }

#define WRITE_BYTES(src,dest,size) \
                              {\
                                 uint_32 i;\
                                 uchar   _PTR_ src_lp;\
                                 uint_32 _PTR_ dest_lp;\
                                 src_lp = (src);\
                                 dest_lp = (dest);\
                                 for(i=0;i<(size);i+=4) {\
                                    *dest_lp++ = (((src_lp[0] & 0xFF) << 24) |\
                                                  ((src_lp[1] & 0xFF) << 16) |\
                                                  ((src_lp[2] & 0xFF) <<  8) |\
                                                  ((src_lp[3] & 0xFF)));\
                                    src_lp+=4;\
                                 }\
                              }

#else /* PSP_ENDIAN == MQX_LITTLE_ENDIAN */

#define READ_BYTES(src,dest,size) \
                              {\
                                 uint_32 i, j;\
                                 uint_32 _PTR_ src_lp;\
                                 uchar   _PTR_ dest_lp;\
                                 src_lp = (src);\
                                 dest_lp = (dest);\
                                 for(i=0;i<(size);i+=4) {\
                                    j = *src_lp++;\
                                    *dest_lp++ = (j)       & 0xFF;\
                                    *dest_lp++ = (j >>  8) & 0xFF;\
                                    *dest_lp++ = (j >> 16) & 0xFF;\
                                    *dest_lp++ = (j >> 24) & 0xFF;\
                                 }\
                              }
#define WRITE_BYTES(src,dest,size) \
                              {\
                                 uint_32 i;\
                                 uchar   _PTR_ src_lp;\
                                 uint_32 _PTR_ dest_lp;\
                                 src_lp = (src);\
                                 dest_lp = (dest);\
                                 for(i=0;i<(size);i+=4) {\
                                    *dest_lp++ = ((src_lp[0] & 0xFF)        |\
                                                 ((src_lp[1] & 0xFF) <<  8) |\
                                                 ((src_lp[2] & 0xFF) << 16) |\
                                                 ((src_lp[3] & 0xFF) << 24));\
                                    src_lp+=4;\
                                 }\
                              }

#endif


/* #define DEBUG  */   /* Uncomment to enable debug printfs */
#ifdef DEBUG
#define DEBUGM(x) x
#else
#define DEBUGM(x)
#endif


char _PTR_  buffer;

void EDS_Process_TCP_packet(uint_32);
void EDS_Process_UDP_packet(uint_32);
void EDS_Check_errors(EDS_OP_STRUCT_PTR);
void EDS_identify(EDS_OP_STRUCT_PTR);
void EDS_task_internal (pointer, pointer);

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : EDS_init
* Returned Value   : error code
* Comments  :  Start the EDS task.
*
*END*-----------------------------------------------------------------*/

uint_32 EDS_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, EDS_task_internal, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : EDS_task
* Returned Value   : none
* Comments  :  The Embedded Debug Server.
*
*END*-----------------------------------------------------------------*/

void EDS_task
   (
      uint_32 dummy
   )
{ /* Body */
   EDS_task_internal(NULL, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : EDS_task_internal
* Returned Value   : none
* Comments  :  The Embedded Debug Server.
*
*END*-----------------------------------------------------------------*/

void EDS_task_internal
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   sockaddr_in    laddr;
   uint_32        sock, listensock;
   uint_32        optval, optlen;
   uint_32        error;

   DEBUGM(printf("\nPrecise/RTCS Embedded Debug Server");)

   buffer = _mem_alloc_zero(EDS_BUFFER_SIZE);
   if (!buffer) {
      DEBUGM(printf("\n_mem_alloc failed ");)
      RTCS_task_exit(creator, RTCSERR_OUT_OF_MEMORY);
   } /* Endif */

   laddr.sin_family      = AF_INET;
   laddr.sin_port        = EDS_PORT;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   sock = socket(PF_INET, SOCK_DGRAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      DEBUGM(printf("\nCreate datagram socket failed");)
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error != RTCS_OK) {
      DEBUGM(printf("\nDatagram bind failed - 0x%lx", error);)
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Listen on TCP port */
   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      DEBUGM(printf("\nCreate stream socket failed");)
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &laddr, sizeof(laddr));
   if (error != RTCS_OK) {
      DEBUGM(printf("\nStream bind failed - 0x%lx", error);)
      RTCS_task_exit(creator, error);
   } /* Endif */
   error = listen(sock, 0);
   if (error != RTCS_OK) {
      DEBUGM(printf("\nListen failed - 0x%lx", error);)
      RTCS_task_exit(creator, error);
   } /* Endif */

   listensock = sock;

   RTCS_task_resume_creator(creator, RTCS_OK);

   DEBUGM(printf("\n\nEmbedded Debug Server active (port %d)\n", EDS_PORT);)

   while (TRUE) {

      sock = RTCS_selectall(0);

      if (sock == listensock) {
         /* Connect received; issue ACCEPT on listening stream socket */
         sock = accept(listensock, NULL, NULL);
         if (sock == RTCS_SOCKET_ERROR) {
            DEBUGM(printf("\nAccept failed, error 0x%lx",
               RTCS_geterror(listensock));)
         } /* Endif */
         DEBUGM(printf("\nAccepted connection");)
      } else {
         /* Get the socket type */
         optlen = sizeof(optval);
         getsockopt(sock, SOL_SOCKET, OPT_SOCKET_TYPE, &optval, &optlen);

         if (optval == SOCK_STREAM) {
            EDS_Process_TCP_packet(sock);
         } else {   /* optval == SOCK_DGRAM */
            EDS_Process_UDP_packet(sock);
         } /* Endif */

      } /* Endif */
   } /* Endwhile */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : EDS_Process_TCP_packet
* Returned Value : void
* Comments       : Receives a stream packet and processes it according to the
*                  client's request.
*
*END------------------------------------------------------------------*/

void EDS_Process_TCP_packet
(
   uint_32                   sock
)
{ /* Body */
   EDS_OP_STRUCT_PTR      op_ptr;
   sockaddr_in            raddr;
   int_32                 length;
   uint_16                rlen;

   _mem_zero((char_ptr)&raddr, sizeof(raddr));
   rlen = sizeof(raddr);

   getpeername(sock, &raddr, &rlen);
   length = recv(sock, buffer, EDS_OP_STRUCT_SIZE, 0);
   if (length == RTCS_ERROR) {
     shutdown(sock, FLAG_CLOSE_TX);
     DEBUGM(printf("\nClosed connection to %d.%d.%d.%d, port %d",
        (raddr.sin_addr.s_addr >> 24) & 0xFF,
        (raddr.sin_addr.s_addr >> 16) & 0xFF,
        (raddr.sin_addr.s_addr >>  8) & 0xFF,
         raddr.sin_addr.s_addr        & 0xFF,
         raddr.sin_port);)
      return;
   } else {
      DEBUGM(printf("\n%d bytes received from %d.%d.%d.%d, port %d",
         length,
         (raddr.sin_addr.s_addr >> 24) & 0xFF,
         (raddr.sin_addr.s_addr >> 16) & 0xFF,
         (raddr.sin_addr.s_addr >>  8) & 0xFF,
          raddr.sin_addr.s_addr        & 0xFF,
          raddr.sin_port);)
   } /* Endif */

   op_ptr = (EDS_OP_STRUCT_PTR)buffer;

   EDS_Check_errors(op_ptr);
   if ((ntohl(op_ptr->EDS_ERROR)) != MQX_OK) {
      DEBUGM(printf("\nOperation error %d", op_ptr->EDS_ERROR);)
      send(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE, 0);
   } else {
      /* Determine and handle client request */
      switch(ntohl(op_ptr->OPERATION)) {

         case EDS_IDENTIFY:
         case EDS_LITTLE_ENDIAN_IDENTIFY:
            EDS_identify(op_ptr);

            /* Send the identity */
            length = send(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE, 0);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         case EDS_READ:
            READ_BYTES((pointer)(ntohl(op_ptr->ADDRESS)),
            (pointer)&buffer[EDS_OP_STRUCT_SIZE], ntohl(op_ptr->SIZE));
            /* Send the result status and the data to read */
            length = send(sock, buffer, EDS_OP_STRUCT_SIZE + ntohl(op_ptr->SIZE), 0);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         case EDS_WRITE:
            /* Write data directly to memory */
            length = recv(sock, (char_ptr)(&buffer[EDS_OP_STRUCT_SIZE]), ntohl(op_ptr->SIZE), 0);
            WRITE_BYTES((pointer)&buffer[EDS_OP_STRUCT_SIZE],
               (pointer)(ntohl(op_ptr->ADDRESS)), ntohl(op_ptr->SIZE));
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n%d bytes received from %d.%d.%d.%d, port %d",
                  length,
                  (raddr.sin_addr.s_addr >> 24) & 0xFF,
                  (raddr.sin_addr.s_addr >> 16) & 0xFF,
                  (raddr.sin_addr.s_addr >>  8) & 0xFF,
                   raddr.sin_addr.s_addr        & 0xFF,
                   raddr.sin_port);)
            } /* Endif */

            /* Send the result status */
            length = send(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE,
               0);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         default:
            break;
      } /* Endswitch */
   }/* Endif */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : EDS_Process_UDP_packet
* Returned Value : void
* Comments       : Receives a datagram packet and processes it according to the
*                  client's request.
*
*END------------------------------------------------------------------*/

void EDS_Process_UDP_packet
(
   uint_32                   sock
)
{ /* Body */
   EDS_OP_STRUCT_PTR      op_ptr;
   sockaddr_in            raddr;
   uint_16                rlen;
   int_32                 length;

   _mem_zero((char_ptr)&raddr, sizeof(raddr));
   rlen = sizeof(raddr);

   length = recvfrom(sock, buffer, EDS_BUFFER_SIZE, 0, &raddr, &rlen);
   if (length == RTCS_ERROR) {
      DEBUGM(printf("\nError %d receiving from %d.%d.%d.%d, port %d",
         RTCS_geterror(sock),
         (raddr.sin_addr.s_addr >> 24) & 0xFF,
         (raddr.sin_addr.s_addr >> 16) & 0xFF,
         (raddr.sin_addr.s_addr >>  8) & 0xFF,
          raddr.sin_addr.s_addr        & 0xFF,
          raddr.sin_port);)
   } else {
      /* Success on receive */
      DEBUGM(printf("\nDatagram (%d bytes) received from %d.%d.%d.%d, port %d",
         length,
         (raddr.sin_addr.s_addr >> 24) & 0xFF,
         (raddr.sin_addr.s_addr >> 16) & 0xFF,
         (raddr.sin_addr.s_addr >>  8) & 0xFF,
          raddr.sin_addr.s_addr        & 0xFF,
          raddr.sin_port);)
   } /* Endif */

   op_ptr = (EDS_OP_STRUCT_PTR)buffer;

   EDS_Check_errors(op_ptr);
   if ((ntohl(op_ptr->EDS_ERROR)) != MQX_OK) {
      DEBUGM(printf("\nOperation error %d", op_ptr->EDS_ERROR);)
      sendto(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE, 0, &raddr, rlen);
   } else {
      /* Determine and handle client request */
      switch(ntohl(op_ptr->OPERATION)) {

         case EDS_IDENTIFY:
         case EDS_LITTLE_ENDIAN_IDENTIFY:
            EDS_identify(op_ptr);

            length = sendto(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE,
                            0, &raddr, rlen);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         case EDS_READ:
            /* Copy data read into buffer */
            READ_BYTES((pointer)(ntohl(op_ptr->ADDRESS)),
            (pointer)&buffer[EDS_OP_STRUCT_SIZE], ntohl(op_ptr->SIZE));
            /* Send buffer */
            length = sendto(sock, buffer, EDS_OP_STRUCT_SIZE + ntohl(op_ptr->SIZE), 0,
                            &raddr, rlen);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         case EDS_WRITE:
            /* Write received data into memory */
            WRITE_BYTES((pointer)&buffer[EDS_OP_STRUCT_SIZE],
               (pointer)(ntohl(op_ptr->ADDRESS)), ntohl(op_ptr->SIZE));

            /* Send the result status */
            length = sendto(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE,
                            0, &raddr, rlen);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         default:
            break;
      } /* Endswitch */
   } /* Endif */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : EDS_Check_errors
* Returned Value : void
* Comments       : Determines if the client request is valid.
*
*END------------------------------------------------------------------*/

void EDS_Check_errors
(
   EDS_OP_STRUCT_PTR op_ptr
)
{ /* Body */
   /* Valid operation? */
   switch(ntohl(op_ptr->OPERATION)) {
      case EDS_IDENTIFY:
      case EDS_LITTLE_ENDIAN_IDENTIFY:
      case EDS_READ:
      case EDS_WRITE:
         htonl(op_ptr->EDS_ERROR,EDS_OK);
         break;
      default:
         htonl(op_ptr->EDS_ERROR,EDS_INVALID_OP);
         return;
   } /* Endswitch */

   /* Valid size? */
   if ((ntohl(op_ptr->OPERATION) == EDS_READ) || (ntohl(op_ptr->OPERATION) == EDS_WRITE)) {
      if (ntohl(op_ptr->SIZE) > (uint_32)EDS_BUFFER_SIZE) {
         htonl(op_ptr->EDS_ERROR,EDS_INVALID_SIZE);
      } else {
         htonl(op_ptr->EDS_ERROR,EDS_OK);
      } /* Endif */
   } /* Endif */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : EDS_identify
* Returned Value : void
* Comments       : Fills in an OP_STRUCT containing the processor type, endian,
*                  start and end of kernel memory of this server, and the result
*                  status.
*
*END------------------------------------------------------------------*/

void EDS_identify
(
   EDS_OP_STRUCT_PTR op_ptr
)
{ /* Body */
#ifdef __MQX__
   /* Only useful if MQX is present */

   htonl(op_ptr->PROCESSOR,_mqx_get_cpu_type());
   htonl(op_ptr->ENDIAN, MSG_HDR_ENDIAN);
   htonl(op_ptr->ADDRESS,(uint_32)_mqx_get_initialization()->START_OF_KERNEL_MEMORY);
   htonl(op_ptr->ADDRESS2,(uint_32)_mqx_get_initialization()->END_OF_KERNEL_MEMORY);
#endif
} /* Endbody */

/* EOF */
