#ifndef __auth_h__
#define __auth_h__
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
*** Comments:  This file contains the declarations of the client
***            side authenticator for the RPC Library.
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


#define MAX_AUTH_BYTES  XDRUNIT_BYTES

/*
** Status returned from authentication check
*/
enum auth_stat {
   AUTH_OK=0,
   /*
   ** failed at remote end
   */
   AUTH_BADCRED=1,         /* bogus credentials (seal broken) */
   AUTH_REJECTEDCRED=2,    /* client should begin new session */
   AUTH_BADVERF=3,         /* bogus verifier (seal broken) */
   AUTH_REJECTEDVERF=4,    /* verifier expired or was replayed */
   AUTH_TOOWEAK=5,         /* rejected due to security reasons */
   /*
   ** failed locally
   */
   AUTH_INVALIDRESP=6,     /* bogus response verifier */
   AUTH_FAILED=7           /* some unknown reason */
};


/*
** Authentication info.  Opaque to client.
*/
typedef struct opaque_auth {
   enum_t   oa_flavor;     /* flavor of authentication */
   caddr_t  oa_base;       /* address of more auth stuff */
   uint_32  oa_length;     /* not to exceed MAX_AUTH_BYTES */
} opaque_auth, _PTR_ opaque_auth_ptr;


/*
** Auth handle, interface to client side authenticators.
*/
typedef struct {
   opaque_auth       ah_cred;       /* credentials */
   opaque_auth       ah_verf;       /* verifier */
   struct auth_ops {
      /*
      ** Added arguments to each function pointer as per the actual function
      */
      /* generate verifier */
      void     (_CODE_PTR_ ah_nextverf)(void _PTR_);
      /* nextverf & serialize */
      bool_t   (_CODE_PTR_ ah_marshal)(void _PTR_, XDR _PTR_);
      /* validate verifier */
      bool_t   (_CODE_PTR_ ah_validate)(void _PTR_, opaque_auth _PTR_);
      /* refresh credentials */
      bool_t   (_CODE_PTR_ ah_refresh)(void _PTR_);
      /* destroy this structure */
      void     (_CODE_PTR_ ah_destroy)(void _PTR_);
   } _PTR_ ah_ops;
   pointer           ah_private;
} AUTH, _PTR_ AUTH_PTR;


/*
** Authentication ops.
** The ops and the auth handle provide the interface to the authenticators.
**
** AUTH  *auth;
** XDR   *xdrs;
** struct opaque_auth verf;
*/
#define auth_nextverf(auth)         \
      ((*((auth)->ah_ops->ah_nextverf))(auth))

#define auth_marshall(auth, xdrs)   \
      ((*((auth)->ah_ops->ah_marshal))(auth, xdrs))

#define auth_validate(auth, verfp)  \
      ((*((auth)->ah_ops->ah_validate))(auth, verfp))

#define auth_refresh(auth)          \
      ((*((auth)->ah_ops->ah_refresh))(auth))

#define auth_destroy(auth)          \
      ((*((auth)->ah_ops->ah_destroy))(auth))


extern const opaque_auth rpc_null_auth;


/*
** XDR routine to handle an opaque authentication block.
** xdr_opaque_auth(xdrs, auth)
**    XDR *xdrs;
**    opaque_auth *auth;
*/
extern bool_t xdr_opaque_auth (XDR_PTR, opaque_auth_ptr);


/*
** These are the various implementations of client side authenticators.
**
** Only null authentication is implemented.
*/

extern AUTH_PTR authnone_create(void);

#define AUTH_NONE  0    /* no authentication */
#define AUTH_NULL  0    /* backward compatibility */
#define AUTH_UNIX  1    /* unix style (uid, gids) */
#define AUTH_SHORT 2    /* short hand unix style */
#define AUTH_DES   3    /* DES style (encrypted timestamps) */

#endif
/* EOF */
