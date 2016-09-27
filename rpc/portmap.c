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
*** Comments:  This file contains the implementation of the portmapper.
***
************************************************************************
*END*******************************************************************/


#include <rtcsrpc.h>

static PMAPLIST_PTR  pml_head = NULL;

static void pmap_service(SVCREQ_PTR, SVCXPRT_PTR);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : pmap_init
* Returned Value   : none
* Comments  :  Initializes the portmapper but doesn't start it.
*     Assumes RTCS is already initialized.
*
*END*-----------------------------------------------------------------*/

bool_t pmap_init
   (
      void
   )
{ /* Body */
   int_32                  sock;
   sockaddr_in             laddr;
   SVCXPRT_PTR             xprt;
   PMAPLIST_PTR            pml;

   /* create and bind a UDP socket */
   sock = socket(PF_INET, SOCK_DGRAM, 0);
   if (sock == (int_32)RTCS_SOCKET_ERROR) {
      fprintf(stderr, "portmap: error creating UDP socket\n");
      return FALSE;
   } /* Endif */

   _mem_zero(&laddr, sizeof(laddr));
   laddr.sin_family = AF_INET;
   laddr.sin_port = PMAPPORT;
   laddr.sin_addr.s_addr = 0;
   if (bind(sock, &laddr, sizeof(laddr)) != RTCS_OK) {
      fprintf(stderr, "portmap: error binding UDP socket, rec=%08X\n",
         RTCS_geterror(sock));
      return FALSE;
   } /* Endif */

   /* create a UDP transport*/
   xprt = svcudp_create(sock);
   if (xprt == NULL) {
      fprintf(stderr, "portmap: error creating UDP transport, rec=%08X\n",
         RTCS_geterror(sock));
      return FALSE;
   } /* Endif */

   /* create an entry for the UDP transport */
   pml = (PMAPLIST_PTR)_mem_alloc_zero(sizeof(PMAPLIST));
   if (pml == NULL) {
      fprintf(stderr, "portmap: error creating UDP entry, can't alloc memory\n");
      return FALSE;
   } /* Endif */
   pml->pml_map.pm_prog = PMAPPROG;
   pml->pml_map.pm_vers = PMAPVERS;
   pml->pml_map.pm_prot = IPPROTO_UDP;
   pml->pml_map.pm_port = PMAPPORT;
   pml->pml_next = pml_head;
   pml_head = pml;

   /* create, bind and connect a TCP socket */
   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == (int_32)RTCS_SOCKET_ERROR) {
      fprintf(stderr, "portmap: error creating TCP socket\n");
      return FALSE;
   } /* Endif */

   _mem_zero(&laddr, sizeof(laddr));
   laddr.sin_family = AF_INET;
   laddr.sin_port = PMAPPORT;
   laddr.sin_addr.s_addr = 0;

   if (bind(sock, &laddr, sizeof(laddr)) != RTCS_OK) {
      fprintf(stderr, "portmap: error binding TCP socket, rec=%08X\n",
         RTCS_geterror(sock));
      return FALSE;
   } /* Endif */

   if (listen(sock, 0) != RTCS_OK) {
      fprintf(stderr, "portmap: error on socket listen, rec=%08X\n",
         RTCS_geterror(sock));
      return FALSE;
   } /* Endif */

   /* create a TCP transport*/
   xprt = svctcp_create(sock, RPCSMALLMSGSIZE, RPCSMALLMSGSIZE);
   if (xprt == NULL) {
      fprintf(stderr, "portmap: error creating TCP transport, rec=%08X\n",
         RTCS_geterror(sock));
      return FALSE;
   } /* Endif */

   /* create an entry for the TCP transport */
   pml = (PMAPLIST_PTR)_mem_alloc_zero(sizeof(PMAPLIST));
   if (pml == NULL) {
      fprintf(stderr, "portmap: error creating TCP entry, task error=%08X\n",
         _task_get_error());
      return FALSE;
   } /* Endif */
   pml->pml_map.pm_prog = PMAPPROG;
   pml->pml_map.pm_vers = PMAPVERS;
   pml->pml_map.pm_prot = IPPROTO_TCP;
   pml->pml_map.pm_port = PMAPPORT;
   pml->pml_next = pml_head;
   pml_head = pml;

   /* register program */
   if (!svc_register(NULL, PMAPPROG, PMAPVERS, pmap_service, 0)) {
      fprintf(stderr, "portmap: error registering service\n");
      return FALSE;
   } /* Endif */

   return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : pmap_service
* Returned Value   : none
* Comments  :  Services an RPC request to the portmapper.
*
*END*-----------------------------------------------------------------*/

static void pmap_service
   (
      SVCREQ_PTR  req,
         /* [IN] - RPC request */
      SVCXPRT_PTR xprt
         /* [IN] - transport handle */
   )
{ /* Body */
   PMAP                 arg;
   uint_32              res;
   PMAPLIST_PTR         pml;
   PMAPLIST_PTR _PTR_   p;

   switch (req->rq_proc) {

   case PMAPPROC_NULL:
      svc_sendreply(xprt, xdr_void, NULL);
      break;

   case PMAPPROC_SET:
      if (!svc_getargs(xprt, (xdrproc_t)xdr_pmap, (caddr_t)&arg)) {
         svcerr_decode(xprt);
      } else {

         /*
         ** Scan list for existing (prog,vers,prot) entry.
         */
         res = FALSE;
         for (p = &pml_head; *p != NULL; p = &(*p)->pml_next) {
            if (((*p)->pml_map.pm_prog == arg.pm_prog)
             && ((*p)->pml_map.pm_vers == arg.pm_vers)
             && ((*p)->pml_map.pm_prot == arg.pm_prot)) {
               /*
               ** Found -- if the port matches, there's no work to be done.
               ** If the port doesn't match, we can't satisfy the request.
               ** Either way, we exit the loop and *p is non-NULL.
               */
               if ((*p)->pml_map.pm_port == arg.pm_port) {
                  res = TRUE;
               } /* Endif */
               break;
            } /* Endif */
         } /* Endfor */

         /*
         ** If there was no entry for (prog,vers,prot), create one
         ** at end of list.
         */
         if (*p == NULL) {
            pml = (PMAPLIST_PTR)_mem_alloc_zero(sizeof(PMAPLIST));
            if (pml != NULL) {
               pml->pml_map = arg;
               pml->pml_next = NULL;
               *p = pml;
               res = TRUE;
            } /* Endif */
         } /* Endif */

         /*
         ** Reply to client
         */
         svc_sendreply(xprt, (xdrproc_t)xdr_uint_32, (pointer)&res);
      } /* Endif */

      break;

   case PMAPPROC_UNSET:
      if (!svc_getargs(xprt, (xdrproc_t)xdr_pmap, (caddr_t)&arg)) {
         svcerr_decode(xprt);
      } else {

         /*
         ** Scan list for all (prog,vers) entries.
         */
         res = FALSE;
         for (p = &pml_head; *p != NULL;) {
            if (((*p)->pml_map.pm_prog == arg.pm_prog)
             && ((*p)->pml_map.pm_vers == arg.pm_vers)) {
               /*
               ** Found -- unlink and free it.
               */
               res = TRUE;
               pml = *p;
               *p = pml->pml_next;
               _mem_free(pml);
            } else {
               p = &(*p)->pml_next;
            } /* Endif */
         } /* Endfor */

         /*
         ** Reply to client
         */
         svc_sendreply(xprt, (xdrproc_t)xdr_uint_32, (pointer)&res);
      } /* Endif */

      break;

   case PMAPPROC_GETPORT:
      if (!svc_getargs(xprt, (xdrproc_t)xdr_pmap, (caddr_t)&arg)) {
         svcerr_decode(xprt);
      } else {

         /*
         ** Scan list for (prog,prot) entries.
         */
         res = 0;
         for (pml = pml_head; pml != NULL; pml = pml->pml_next) {
            if ((pml->pml_map.pm_prog == arg.pm_prog)
             && (pml->pml_map.pm_prot == arg.pm_prot)) {
               /*
               ** Found -- get the port, even if the version doesn't match.
               */
               res = pml->pml_map.pm_port;
               if (pml->pml_map.pm_vers == arg.pm_vers) {
                  break;
               } /* Endif */
            } /* Endif */
         } /* Endfor */

         /*
         ** Reply to client
         */
         svc_sendreply(xprt, (xdrproc_t)xdr_uint_32, (pointer)&res);
      } /* Endif */

      break;

   case PMAPPROC_DUMP:
      if (!svc_getargs(xprt, xdr_void, (caddr_t)NULL)) {
         svcerr_decode(xprt);
      } else {
         svc_sendreply(xprt, (xdrproc_t)xdr_pmaplist, (pointer)&pml_head);
      } /* Endif */
      break;

   default:
      svcerr_noproc(xprt);
      break;
   } /* Endswitch */

} /* Endbody */


/* EOF */
