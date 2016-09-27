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
*** File: pppglob.c
***
*** Comments:  This file contains the various global PPP structures
***
************************************************************************
*END*******************************************************************/

#include <ppp.h>
#include "ppp_prv.h"


/*
** PPP task priority and extra stack size
*/

uint_32  _PPPTASK_priority   = 6;
uint_32  _PPPTASK_stacksize  = 0;

/*
** Variables used by the retransmission timer
*/

uint_32 _PPP_MIN_XMIT_TIMEOUT = 3000;        /* RFC 1661 4.6 */
uint_32 _PPP_MAX_XMIT_TIMEOUT = 10000l;
uint_32 _PPP_MAX_CONF_RETRIES = 10;          /* RFC 1661 4.6 */
uint_32 _PPP_MAX_TERM_RETRIES = 2;           /* RFC 1661 4.6 */
uint_32 _PPP_MAX_CONF_NAKS    = 5;           /* RFC 1661 4.6 */


/*
** The default PPP link options
*/

PPP_OPT PPP_DEFAULT_OPTIONS = {
   /* ACCM     */   0xFFFFFFFF,
   /* MRU      */   DEFAULT_MRU,
   /* AP       */   0,
   /* QP       */   0,
   /* AP_Start */   NULL,
   /* MAGIC    */   0,
   /* PFC      */   FALSE,
   /* ACFC     */   FALSE,
   /* CP       */   NULL
};

PPP_OPT PPP_DEFAULT_OPTIONS_PPPOE = {
   /* ACCM     */   0xFFFFFFFF,
   /* MRU      */   1492,
   /* AP       */   0,
   /* QP       */   0,
   /* AP_Start */   NULL,
   /* MAGIC    */   0,
   /* PFC      */   FALSE,
   /* ACFC     */   FALSE,
   /* CP       */   NULL
};


/*
** The default PPP call table
*/

PPP_CALL PPP_DEFAULT_CALL = {
   /* PARAM          */    0,
   /* PPP_Link_up    */    NULL,
   /* PPP_Link_down  */    NULL,
   /* PPP_Echo_reply */    NULL,
   /* PPP_Auth_fail  */    NULL
};



/*
** The minimal ACCM
*/

uint_32 _PPP_ACCM = 0xFFFFFFFFl;


/*
** The secrets.  These are the secrets used by PAP and CHAP.
** an xSECRET is a pointer to a single secret; an xSECRETS
** points to an array of secrets.
**
** LSECRET(S) points to the secret(s) to use when authenticating
** ourself to the peer (local secret(s)); RSECRET(S) points to
** the secret(s) to use when authenticating the peer (remote
** secret(s)).
**
** If an LSECRET(S) pointer is NULL, LCP will not allow the
** peer to request that authentication protocol.  If all LSECRET(S)
** are NULL, LCP will reject negotiation of the authentication
** protocol.
**
** If any RSECRET(S) are non-NULL, LCP will require that the peer
** authenticate itself.  If the peer rejects negotiation of the
** authentication protocol, LCP will terminate the link immediately
** upon reaching the OPENED state.
*/

PPP_SECRET_PTR _PPP_PAP_LSECRET = NULL;
PPP_SECRET_PTR _PPP_PAP_RSECRETS = NULL;
char_ptr       _PPP_CHAP_LNAME = "";
PPP_SECRET_PTR _PPP_CHAP_LSECRETS = NULL;
PPP_SECRET_PTR _PPP_CHAP_RSECRETS = NULL;


/* EOF */
