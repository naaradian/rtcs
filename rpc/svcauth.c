/*HEADER****************************************************************
************************************************************************
***
*** Copyright (c) 1992-2003 ARC International
*** All rights reserved
***
*** Portions of this software were written by Sun Microsystems, Inc.
*** Copyright (C) 1984, Sun Microsystems, Inc.
***
*** This software embodies materials and concepts which are
*** confidential to ARC International and is made available solely
*** pursuant to the terms of a written license agreement with
*** ARC International.
***
*** Comments:  This file contains the implementation of the server
***            side RPC Library authentication functions.
***
************************************************************************
*END*******************************************************************/

/*
** Sun RPC is a product of Sun Microsystems, Inc. and is provided for
** unrestricted use provided that this legend is included on all tape
** media and as a part of the software program in whole or part.  Users
** may copy or modify Sun RPC without charge, but are not authorized
** to license or distribute it to anyone else except as part of a product or
** program developed by the user.
**
** SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
** WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
** PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
**
** Sun RPC is provided with no support and without any obligation on the
** part of Sun Microsystems, Inc. to assist in its use, correction,
** modification or enhancement.
**
** SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
** INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
** OR ANY PART THEREOF.
**
** In no event will Sun Microsystems, Inc. be liable for any lost revenue
** or profits or other special, indirect and consequential damages, even if
** Sun has been advised of the possibility of such damages.
**
** Sun Microsystems, Inc.
** 2550 Garcia Avenue
** Mountain View, California  94043
*/


#include <rtcsrpc.h>

/*
** Server side authenticators are called from _authenticate() by
** using the client auth struct flavor field to index into svcauthsw[].
** The server auth flavors must implement a routine that looks like:
**
** enum auth_stat
** flavorx_auth(rqst, msg)
**    register struct svc_req *rqst;
**    register struct rpc_msg *msg;
**
*/

enum auth_stat _svcauth_null(struct svc_req _PTR_, RPC_MESSAGE_PTR);     /* no authentication */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _svcauth_null
* Returned Value   : status
* Comments  :  Authenticates an RPC request using "null" authentication
*
*END*-----------------------------------------------------------------*/

enum auth_stat _svcauth_null
   (
      register struct svc_req _PTR_ rqst,
            /* [OUT] - Service request structure */
      RPC_MESSAGE_PTR msg
            /* [IN] - RPC request */
   )
{ /* Body */
   return (AUTH_OK);
} /* Endbody */


static struct {
   enum auth_stat (_CODE_PTR_ authenticator)(struct svc_req _PTR_, RPC_MESSAGE_PTR);
} svcauthsw[] = {
   _svcauth_null        /* AUTH_NULL */
};

#define  AUTH_MAX 0     /* highest AUTH number -- only "null" is implemented */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : _authenticate
* Returned Value   : status
* Comments  :  Authenticates an RPC request
*
* The RPC request, msg, has been obtained from the wire.  The msg contains
* the raw form of credentials and verifiers.  _authenticate() returns AUTH_OK
* if the msg is successfully authenticated.  If AUTH_OK, then the routine also
* does the following things:
*
* sets rqst->rq_xprt->verf to the appropriate response verifier;
* sets rqst->rq_client_cred to the "cooked" form of the credentials.
*
* NB: rqst->rq_cxprt->verf must be pre-alloctaed;
* its length is set appropriately.
*
* The caller still owns and is responsible for msg->u.cmb.cred and
* msg->u.cmb.verf.  The authentication system retains ownership of
* rqst->rq_client_cred, the cooked credentials.
*
* There is an assumption that any flavour less than AUTH_NULL is
* invalid.
*
*END*-----------------------------------------------------------------*/

enum auth_stat _authenticate
   (
      register struct svc_req _PTR_ rqst,
            /* [OUT] - Service request structure */
      RPC_MESSAGE_PTR         msg
            /* [IN] - RPC request */
   )
{ /* Body */
   register uint_32 cred_flavor;

   rqst->rq_cred = msg->rm_call.cb_cred;
   rqst->rq_xprt->xp_verf.oa_flavor = rpc_null_auth.oa_flavor;
   rqst->rq_xprt->xp_verf.oa_length = 0;
   cred_flavor = rqst->rq_cred.oa_flavor;
   /* Start SPR P122-0266-13 remove all warnings from RTCS code. */
   /* Unsigned compare to 0 always true. */
   /* if ((cred_flavor <= AUTH_MAX) && (cred_flavor >= AUTH_NULL)) { */
   if (cred_flavor <= AUTH_MAX) {
   /* End SPR P122-0266-13 */
      return ((*(svcauthsw[cred_flavor].authenticator))(rqst, msg));
   } /* Endif */

   return (AUTH_REJECTEDCRED);
} /* Endbody */


/* EOF */
