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
*** File: tftputil.c
***
*** Comments:  This file contains the TFTP utility implementation for
***            the RTCS Communication Library.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "tftp.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTP_timeout_init
* Returned Value  : time to expiry in milliseconds
* Comments        : This function initializes the adaptive timeout.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_timeout_init
   (
      TFTP_TO_STRUCT_PTR   to
   )
{ /* Body */

   to->TS     = RTCS_time_get();
   to->UPDATE = TRUE;
   to->TO     = TFTP_TIMEOUT_MIN;
   to->M      = TFTP_TIMEOUT_MIN;
   to->D      = 0;

   return to->TO;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTP_timeout_restart
* Returned Value  : time to expiry in milliseconds
* Comments        : This function causes the next call to
*                   TFTP_timeout_update to not update the adaptive
*                   timeout.  Used when retransmitting before expiry.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_timeout_restart
   (
      TFTP_TO_STRUCT_PTR   to
   )
{ /* Body */

   to->TS     = RTCS_time_get();
   to->UPDATE = FALSE;

   return to->TO;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTP_timeout_update
* Returned Value  : time to expiry in milliseconds
* Comments        : This function updates the timeout when a response
*                   is received without retransmission.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_timeout_update
   (
      TFTP_TO_STRUCT_PTR   to
   )
{ /* Body */
   uint_32 time, time_elapsed;
   int_32 error;
//________________________________________________my
   return to->TO; //t to work on 10MBit/s
//________________________________________________my
   time = RTCS_time_get();
   if (to->UPDATE) {
      time_elapsed = time - to->TS;
      error = time_elapsed - to->M;
      to->M += error >> 3;
      if (!to->M) to->M++;
      if (error < 0) error = -error;
      error -= to->D;
      to->D += error >> 2;
      if (!to->D) to->D++;
      to->TO = to->M + (to->D << 2);
   } /* Endif */

   to->TS     = time;
   to->UPDATE = TRUE;

   return to->TO;

} /* Endbody */


extern void my_printf(unsigned char);
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTP_timeout_left
* Returned Value  : time to expiry in milliseconds
* Comments        : This function determines the amout of time until
*                   the timeout expires
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_timeout_left
   (
      TFTP_TO_STRUCT_PTR   to,
      boolean _PTR_        expire
   )
{ /* Body */
   uint_32 time;
//   uint_32 tmp;	//t
   
   time = RTCS_time_get();

//	tmp = time - to->TS;

   if ((time - to->TS) >= to->TO)
 //  if (((time - to->TS) > to->TO) && (tmp > 0))//t
    {
//	my_printf((unsigned char)(time >> 24)) ;
//	my_printf((unsigned char)(time >> 16)) ;
//	my_printf((unsigned char)(time >> 8)) ;
//	my_printf((unsigned char)time) ;
//	my_printf((unsigned char)0x55) ;


//	my_printf((unsigned char)(to->TS >> 24)) ;
//	my_printf((unsigned char)(to->TS >> 16)) ;
//	my_printf((unsigned char)(to->TS >> 8)) ;
//	my_printf((unsigned char)to->TS) ;
//	my_printf((unsigned char)0x77) ;


//	my_printf((unsigned char)(to->TO >> 24)) ;
//	my_printf((unsigned char)(to->TO >> 16)) ;
//	my_printf((unsigned char)(to->TO >> 8)) ;
//	my_printf((unsigned char)to->TO) ;
//	my_printf((unsigned char)0x99) ;

//	my_printf((unsigned char)(tmp >> 24)) ;
//	my_printf((unsigned char)(tmp >> 16)) ;
//	my_printf((unsigned char)(tmp >> 8)) ;
//	my_printf((unsigned char)tmp) ;


      to->TS     = time;
      to->UPDATE = FALSE;
      to->TO <<= 1;

  //	to->TO *= 2; // << not used for unsigned long!!!
      if (to->TO > TFTP_TIMEOUT_MAX) {
         to->TO = TFTP_TIMEOUT_MAX;
      } /* Endif */
      if (expire) *expire = TRUE;
    //	if (expire) *expire = FALSE; //t
      return to->TO;
   } /* Endif */

   if (expire) *expire = FALSE;
   return to->TO - (time - to->TS);

} /* Endbody */

#define TFTP_ERROR_PKT(err,str)  {{0, TFTPOP_ERROR}, {0, err}, str}

/* The Illegal TID error packet */
TFTP_ERROR_PACKET _tftp_error_tid    = TFTP_ERROR_PKT(TFTPERR_ILLEGAL_TID, "Invalid TID");

/* The Illegal Operation error packet */
TFTP_ERROR_PACKET _tftp_error_op     = TFTP_ERROR_PKT(TFTPERR_ILLEGAL_OP, "Illegal Operation");

/* The Access Violation error packet */
TFTP_ERROR_PACKET _tftp_error_accvio = TFTP_ERROR_PKT(TFTPERR_ACCESS_VIOLATION, "Access Violation");

/* The Server Failure error packet */
TFTP_ERROR_PACKET _tftp_error_srv    = TFTP_ERROR_PKT(TFTPERR_UNKNOWN, "TFTP Server Failure");

/* The Server Busy error packet */
TFTP_ERROR_PACKET _tftp_error_busy   = TFTP_ERROR_PKT(TFTPERR_UNKNOWN, "TFTP Server Busy");

/* The Excessive Retrasnmissions error packet */
TFTP_ERROR_PACKET _tftp_error_to     = TFTP_ERROR_PKT(TFTPERR_UNKNOWN, "Excessive Retransmissions");

/* The File Not Found error packet */
TFTP_ERROR_PACKET _tftp_error_nofile = TFTP_ERROR_PKT(TFTPERR_FILE_NOT_FOUND, "File Not Found");

/* The File Already Exists error packet */
TFTP_ERROR_PACKET _tftp_error_exists = TFTP_ERROR_PKT(TFTPERR_FILE_EXISTS, "File Already Exists");


/* EOF */
