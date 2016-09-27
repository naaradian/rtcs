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
*** Comments:  This file contains the implementation of the client
***            side interface to the portmapper functions.
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

static struct timeval timeout = {5, 0};
static struct timeval tottimeout = {60, 0};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : pmap_getport
* Returned Value   : port of remote service
* Comments  :  Find the mapped port for program,version.
*     Calls the pmap service remotely to do the lookup.
*     Returns 0 if no map exists.
*
*END*-----------------------------------------------------------------*/

uint_16 pmap_getport
   (
      sockaddr_in _PTR_  address,
      uint_32            program,
      uint_32            version,
      uint_32            protocol
   )
{ /* Body */
   uint_16     port = 0;
   int_32      socket = -1;
   CLIENT_PTR  client;
   PMAP        parms;

   address->sin_port = PMAPPORT;
   client = clntudp_bufcreate(address, PMAPPROG, PMAPVERS, timeout,
            &socket, RPCSMALLMSGSIZE, RPCSMALLMSGSIZE);
   if (client != NULL) {
      parms.pm_prog = program;
      parms.pm_vers = version;
      parms.pm_prot = protocol;
      parms.pm_port = 0;  /* not needed or used */
      if (clnt_call(client, PMAPPROC_GETPORT, (xdrproc_t)xdr_pmap,
         (caddr_t)&parms, (xdrproc_t)xdr_uint_16, (caddr_t)&port, tottimeout) != RPC_SUCCESS) {
         rpc_createerr.cf_stat = RPC_PMAPFAILURE;
         clnt_geterr(client, &rpc_createerr.cf_error);
      } else if (port == 0) {
         rpc_createerr.cf_stat = RPC_PROGNOTREGISTERED;
      } /* Endif */
      clnt_destroy(client);
   } /* Endif */
   shutdown(socket, 0);
   address->sin_port = 0;
   return (port);
} /* EndBody */


/* EOF */
