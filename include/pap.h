#ifndef __pap_h__
#define __pap_h__
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
*** File: pap.h
***
*** Comments:  This file contains the defines and data structure
***            definitions required by PAP.
***
************************************************************************
*END*******************************************************************/


/*
** PAP header = Code, id, length.
*/

#define PAP_HDR_LEN   (4)

/*
** PAP packet types
*/

#define PAP_CODE_AUTH_REQ  1     /* Authenticate Request */
#define PAP_CODE_AUTH_ACK  2     /* Authenticate Ack */
#define PAP_CODE_AUTH_NAK  3     /* Authenticate Nak */

/*
** Internal PAP states
*/

#define PAP_STATE_CLOSED   0
#define PAP_STATE_INITIAL  1
#define PAP_STATE_AUTH_ACK PAP_CODE_AUTH_ACK
#define PAP_STATE_AUTH_NAK PAP_CODE_AUTH_NAK

/*
** Private state required by the PAP server
*/

typedef struct {

   /* Statistics counters */
   uint_32  ST_PAP_SHORT;     /* packet too short */
   uint_32  ST_PAP_CODE;      /* invalid code */
   uint_32  ST_PAP_ID;        /* incorrect id */
   uint_32  ST_PAP_NOAUTH;    /* request received but PAP not negotiated */
   uint_32  ST_PAP_NOREQ;     /* reply received but no request made */

   uchar CLIENT_STATE;
   uchar SERVER_STATE;
   uchar CURID;
} PAP_DATA, _PTR_ PAP_DATA_PTR;


/*
** Prototypes
*/

extern void PAP_init  (_ppp_handle);
extern void PAP_send  (_ppp_handle);
extern void PAP_open  (_ppp_handle);

extern void PAP_input (PCB_PTR, _ppp_handle);


#endif
/* EOF */
