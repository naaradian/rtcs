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
*** File: snmp.c
***
*** Comments:  This file contains an implementation of an
***            SNMP{v1,v2c} agent.
***
************************************************************************
*END*******************************************************************/

#include <string.h>
#include <rtcs.h>
#include "rtcs_prv.h"
#include "snmpcfg.h"
#include "asn1.h"
#include "snmp.h"

/* CR 887 */
/* typedef struct {} SNMP_PARSE; moved to snmp.h */
/* End CR 887 */

static uchar inbuf[SNMPCFG_BUFFER_SIZE];
static uchar outbuf[SNMPCFG_BUFFER_SIZE];

void SNMP_task (pointer, pointer);

/*
** This agent generates GetResponse PDUs to GetRequests,
** GetNextRequests and GetBulkRequests.  All GetResponses
** have the same format, specifically:
**
**    30 <len>
**       02 01 <00-01>
**       04 <len> <community>
**       A2 <len>
**          02 <len> <id>
**          02 01 00
**          02 01 00
**          30 <len>
**
**             30 <len>
**                06 <len> <object-id>
**                <tag> <len> <value>
**             30 <len>
**                06 <len> <object-id>
**                <tag> <len> <value>
**             30 <len>
**                06 <len> <object-id>
**                <tag> <len> <value>
**             ...
**
** Building this packet is a bit problematic.
**
** If we start from the top, we can't fill in the <len>
** fields, because
**    1) we don't yet know the length of the remainder
**       of the packet, and
**    2) since we don't know the length, we don't know
**       how many bytes the length field will occupy
**       (each <len> is 1 byte for lengths 00-7F,
**       2 bytes for lengths 80-FF, or 3 bytes for
**       lengths 0100-FFFF).
**
** We'd have to assume the worst case for each <len>
** and reserve 3 bytes for each, and make a second
** pass to fill them.
**
** If we start from the bottom, then building responses
** to GetBulkRequests becomes hard.
**
** So what do we do?  We start from the middle. :)
**
** We reserve space for the maximum header (the first eight
** lines above).  To do this, we make an assumption:  that
** no <len> ever gets a value larger than FFFF.  This is
** pretty safe, since SNMP packets have to fit inside IP
** datagrams, and IP datagrams only have 16-bit length
** fields.
**
** Then we make repeated MIB calls to fill in the
** VarBindList.
**
** Once the VarBindList is written, we write the header
** backwards.
**
*/


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_request
* Returned Value   :
* Comments  :  Parse a VarBindList in a PDU.
*
*END*-----------------------------------------------------------------*/

boolean SNMP_request
   (
      SNMP_PARSE_PTR snmp,
      uchar_ptr      invarp,
      uint_32        invarlen,
      uchar_ptr      outvarp,
      uint_32        outvarlen,
      uint_32 _PTR_  writelen
   )
{ /* Body */

   snmp->STATS.ST_RX_VAR_GETS++;
   snmp->errstat = RTCSMIB_request(snmp->pdutype, invarp, invarlen, outvarp, outvarlen, writelen);

   if (snmp->errstat == SNMP_ERROR_noError) {
      return TRUE;
   } /* Endif */

   if (snmp->errstat == SNMP_ERROR_PARSE) {
      snmp->errstat = SNMP_ERROR_noError;
      return FALSE;
   } /* Endif */

   if (snmp->version == 0) {
      switch (snmp->errstat) {
      case SNMP_ERROR_noSuchObject:
      case SNMP_ERROR_noSuchInstance:
      case SNMP_ERROR_endOfMibView:
      case SNMP_ERROR_notWritable:
         snmp->errstat = SNMP_ERROR_noSuchName;
         break;
      case SNMP_ERROR_noAccess:
      case SNMP_ERROR_wrongType:
      case SNMP_ERROR_wrongLength:
      case SNMP_ERROR_wrongEncoding:
      case SNMP_ERROR_wrongValue:
      case SNMP_ERROR_noCreation:
      case SNMP_ERROR_inconsistentValue:
      case SNMP_ERROR_resourceUnavailable:
         snmp->errstat = SNMP_ERROR_badValue;
         break;
      } /* Endswitch */
      return FALSE;
   } /* Endif */

   if ((snmp->version == 1) && (snmp->errstat >= 0x80)) {
      uint_32 oidlen, varlen, totlen;

      invarp++;
      if (*invarp < 0x80) {
         invarp++;
      } else {
         invarp += (*invarp & 0x7F) + 1;
      } /* Endif */

      invarp++;
      if (*invarp < 0x80) {
         oidlen = *invarp++;
      } else {
         uint_32 lenlen = *invarp++ & 0x7F;
         oidlen = 0;
         /* CR 891 */
         while (lenlen--) {
         /* End CR 891 */
            oidlen <<= 8;
            oidlen += *invarp++;
         } /* Endwhile */
      } /* Endif */

      varlen = oidlen;
      if (varlen < 0x80) {
         varlen += 2;
      } else if (varlen < 0x100) {
         varlen += 3;
      } else {
         varlen += 4;
      } /* Endif */
      varlen += 2;

      totlen = varlen;
      if (totlen < 0x80) {
         totlen += 2;
      } else if (totlen < 0x100) {
         totlen += 3;
      } else {
         totlen += 4;
      } /* Endif */

      if (totlen > outvarlen) {
         snmp->errstat = SNMP_ERROR_tooBig;
         return FALSE;
      } /* Endif */

      *outvarp++ = ASN1_TYPE_SEQUENCE;
      if (varlen < 0x80) {
         *outvarp++ = varlen;
      } else if (varlen < 0x100) {
         *outvarp++ = 0x81;
         *outvarp++ = varlen;
      } else {
         *outvarp++ = 0x82;
         htons(outvarp, varlen);
         outvarp += 2;
      } /* Endif */

      *outvarp++ = ASN1_TYPE_OBJECT;
      if (oidlen < 0x80) {
         *outvarp++ = oidlen;
      } else if (oidlen < 0x100) {
         *outvarp++ = 0x81;
         *outvarp++ = oidlen;
      } else {
         *outvarp++ = 0x82;
         htons(outvarp, oidlen);
         outvarp += 2;
      } /* Endif */
      _mem_copy(invarp, outvarp, oidlen);
      outvarp += oidlen;

      *outvarp++ = snmp->errstat;
      *outvarp++ = 0;

      *writelen = totlen;
      return TRUE;
   } /* Endif */

   return FALSE;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_parse_varbindlist
* Returned Value   :
* Comments  :  Parse a VarBindList in a PDU.
*
*END*-----------------------------------------------------------------*/

boolean SNMP_parse_varbindlist
   (
      SNMP_PARSE_PTR snmp,
      uint_32_ptr    writelen
   )
{ /* Body */
   uchar_ptr   outp, varp;
   uint_32     outlen, varlen, varcount, index;

   outp = snmp->outbuf;
   outlen = snmp->outlen;
   index = 0;
   *writelen = 0;

   while (snmp->nonrep-- && snmp->inlen) {
      varp = snmp->inbuf;
      varlen = snmp->inlen;
      index++;
      if (!SNMP_request(snmp, varp, varlen, outp, outlen, &varlen)) {
         if (SNMP_ERROR_USEINDEX(snmp->errstat)) {
            snmp->errindex = index;
         } /* Endif */
         return FALSE;
      } /* Endif */
      outp += varlen;
      outlen -= varlen;
      *writelen += varlen;
      ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, varlen);
      ASN1_READ_IGNORE(snmp, varlen);
   } /* Endwhile */

   while (snmp->inlen) {
      varp = snmp->inbuf;
      varlen = snmp->inlen;
      index++;
      for (varcount = 0; varcount < snmp->reps; varcount++) {
         if (!SNMP_request(snmp, varp, varlen, outp, outlen, &varlen)) {
            if (SNMP_ERROR_USEINDEX(snmp->errstat)) {
               snmp->errindex = index;
            } /* Endif */
            return FALSE;
         } /* Endif */
         varp = outp;
         outp += varlen;
         outlen -= varlen;
         *writelen += varlen;
      } /* Endfor */
      ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, varlen);
      ASN1_READ_IGNORE(snmp, varlen);
   } /* Endwhile */

   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_parse_pduv1
* Returned Value   :
* Comments  :  Parse a {Get,GetNext,Set}Request PDU.
*
*END*-----------------------------------------------------------------*/

boolean SNMP_parse_pduv1
   (
      SNMP_PARSE_PTR snmp
   )
{ /* Body */
   uint_32     typelen;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   if (typelen == 0) return FALSE;
   snmp->errstatp = snmp->inbuf + (typelen - 1);
   ASN1_READZERO_IGNORE(snmp, typelen);

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   if (typelen == 0) return FALSE;
   snmp->errindexp = snmp->inbuf + (typelen - 1);
   ASN1_READZERO_IGNORE(snmp, typelen);

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, typelen);
   snmp->inlen = typelen;

   /*
   ** We want nonrep to be (at least) the number of VarBinds in
   ** the VarBindList.  We pick typelen, because each VarBind is
   ** guaranteed to be at least one byte (in fact, they're guaranteed
   ** to be at least five bytes), so typelen is a guaranteed upper
   ** bound.  We pick reps = 1, because if ever nonrep weren't big
   ** enough (which will never happen), the remaining VarBinds will
   ** still be treated as non-repeaters.
   */
   snmp->nonrep = typelen;
   snmp->reps = 1;

   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_parse_awesome_pdubulk
* Returned Value   :
* Comments  :  Parse a GetBulkRequest PDU.
*
*END*-----------------------------------------------------------------*/

boolean SNMP_parse_awesome_pdubulk
   (
      SNMP_PARSE_PTR snmp
   )
{ /* Body */
   uint_32     typelen;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   if (typelen == 0) return FALSE;
   snmp->errstatp = snmp->inbuf + (typelen - 1);
   ASN1_READZERO_INT(snmp, typelen, snmp->nonrep);

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   if (typelen == 0) return FALSE;
   snmp->errindexp = snmp->inbuf + (typelen - 1);
   ASN1_READZERO_INT(snmp, typelen, snmp->reps);

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, typelen);
   snmp->inlen = typelen;

   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_parse
* Returned Value   :
* Comments  :  Parse an SNMP packet.
*
*END*-----------------------------------------------------------------*/

boolean SNMP_parse
   (
      SNMP_PARSE_PTR snmp,
      uint_32_ptr    writelen
   )
{ /* Body */
   uint_32     typelen, maxheadlen;
   uint_32     ctylen, idlen;
   uchar_ptr   ctyp,   idp,  pdup;

   maxheadlen = 0;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, typelen);
   maxheadlen += 4;
   snmp->inlen = typelen;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   ASN1_READ_INT(snmp, typelen, snmp->version);
   maxheadlen += 3;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_OCTET, ctylen);
   ctyp = snmp->inbuf;

   if ((ctylen != snmp->communitylen)
    || memcmp(ctyp, snmp->community, ctylen)) 
  {
      snmp->STATS.ST_RX_BAD_COMMUNITY++;
      snmp->STATS.ST_RX_BAD_PARSE--; /* because SNMP_task will increment it */
      return FALSE;
   } 
  
   ASN1_READ_IGNORE(snmp, ctylen);
   maxheadlen += ctylen + 4;

   pdup = snmp->inbuf;
   ASN1_READ_TYPELEN(snmp, snmp->pdutype, typelen);
   *pdup = ASN1_TYPE_PDU_RESPONSE;
   maxheadlen += 4;
   snmp->inlen = typelen;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, idlen);
   idp = snmp->inbuf;
   ASN1_READ_IGNORE(snmp, idlen);
   maxheadlen += idlen + 4;

   maxheadlen += 10; /* two INTEGERs and a SEQUENCE */
   //maxheadlen += 20;//t /* two INTEGERs and a SEQUENCE */

   snmp->outbuf += maxheadlen;
   snmp->outlen -= maxheadlen;

   switch (snmp->pdutype) {
   case ASN1_TYPE_PDU_GET:       snmp->STATS.ST_RX_GETREQ++;     break;
   case ASN1_TYPE_PDU_GETNEXT:   snmp->STATS.ST_RX_GETNEXTREQ++; break;
   case ASN1_TYPE_PDU_RESPONSE:  snmp->STATS.ST_RX_RESPONSE++;   break;
   case ASN1_TYPE_PDU_SET:       snmp->STATS.ST_RX_SETREQ++;     break;
   case ASN1_TYPE_PDU_TRAP:      snmp->STATS.ST_RX_TRAP++;       break;
   case ASN1_TYPE_PDU_GETBULK:   snmp->STATS.ST_RX_GETBULKREQ++; break;
   case ASN1_TYPE_PDU_INFORM:    snmp->STATS.ST_RX_INFORM++;     break;
   case ASN1_TYPE_PDU_TRAPV2:    snmp->STATS.ST_RX_V2TRAP++;     break;
   case ASN1_TYPE_PDU_REPORT:    snmp->STATS.ST_RX_REPORT++;     break;
   default:                      snmp->STATS.ST_RX_OTHER++;      break;
   } /* Endswitch */

   switch (snmp->pdutype) {
   case ASN1_TYPE_PDU_GET:
   case ASN1_TYPE_PDU_GETNEXT:
   case ASN1_TYPE_PDU_SET:
      if (snmp->version > 1) {
         snmp->STATS.ST_RX_BAD_VERSION++;
         snmp->STATS.ST_RX_BAD_PARSE--; /* because SNMP_task will increment it */
         return FALSE;
      } /* Endif */
      if (!SNMP_parse_pduv1(snmp)) return FALSE;
      break;
   case ASN1_TYPE_PDU_GETBULK:
      if (snmp->version != 1) {
         snmp->STATS.ST_RX_BAD_VERSION++;
         snmp->STATS.ST_RX_BAD_PARSE--; /* because SNMP_task will increment it */
         return FALSE;
      } /* Endif */
      if (!SNMP_parse_awesome_pdubulk(snmp)) return FALSE;
      snmp->pdutype = ASN1_TYPE_PDU_GETNEXT;
      break;
   default:
      snmp->STATS.ST_RX_BAD_PDU++;
      snmp->STATS.ST_RX_BAD_PARSE--; /* because SNMP_task will increment it */
      return FALSE;
   } /* Endswitch */

   if (!SNMP_parse_varbindlist(snmp, &typelen)) return FALSE;

   ASN1_BKWRITE_TYPELEN       (snmp, typelen, ASN1_TYPE_SEQUENCE, typelen);
   ASN1_BKWRITE_TYPELEN_INT   (snmp, typelen, 0);
   ASN1_BKWRITE_TYPELEN_INT   (snmp, typelen, 0);
   ASN1_BKWRITE_TYPELEN_BYTES (snmp, typelen, ASN1_TYPE_INTEGER, idp, idlen);
   ASN1_BKWRITE_TYPELEN       (snmp, typelen, ASN1_TYPE_PDU_RESPONSE, typelen);
   ASN1_BKWRITE_TYPELEN_BYTES (snmp, typelen, ASN1_TYPE_OCTET, ctyp, ctylen);
   ASN1_BKWRITE_TYPELEN_INT   (snmp, typelen, snmp->version);
   ASN1_BKWRITE_TYPELEN       (snmp, typelen, ASN1_TYPE_SEQUENCE, typelen);

   *writelen = typelen;
   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_init
* Returned Value   : error code
* Comments  :  Start the SNMP agent.
*
*END*-----------------------------------------------------------------*/

uint_32 SNMP_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, SNMP_task, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : SNMP_task
* Returned Value   : none
* Comments  :  SNMP agent.
*
*END*-----------------------------------------------------------------*/

void SNMP_task
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   SNMP_PARSE     snmp;
   sockaddr_in    addr;
   uint_32        sock, error;
   int_32         inlen;
   uint_32        outlen;
   uint_16        addrlen;

   /* SNMP agent services port 161 */
   addr.sin_family      = AF_INET;
   addr.sin_port        = IPPORT_SNMP;
   addr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   sock = socket(PF_INET, SOCK_DGRAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(sock, &addr, sizeof(addr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* CR 887 */
   _mem_zero(&snmp.STATS, sizeof(snmp.STATS));
   snmp.trapsock     = sock;
   snmp.community    = SNMPCFG_COMMUNITY;
   snmp.communitylen = sizeof(SNMPCFG_COMMUNITY) - 1;

   SNMP_set_data(&snmp);
   /* End CR 887 */

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {

      addrlen = sizeof(addr);
      inlen = recvfrom(sock, inbuf, SNMPCFG_BUFFER_SIZE, 0, &addr, &addrlen);
      snmp.STATS.ST_RX_TOTAL++;

      snmp.inbuf = inbuf;
      snmp.inlen = inlen;
      snmp.outbuf = outbuf;
      snmp.outlen = SNMPCFG_BUFFER_SIZE;
      snmp.pdutype = 0; /* anything except ASN1_TYPE_PDU_SET */
      snmp.errstat = SNMP_ERROR_noError;
      snmp.errindex = 0;
      if (SNMP_parse(&snmp, &outlen)) {
         snmp.STATS.ST_TX_TOTAL++;
         snmp.STATS.ST_TX_RESPONSE++;
         sendto(sock, snmp.outbuf, outlen, 0, &addr, addrlen);
      } else if (snmp.errstat || snmp.pdutype == ASN1_TYPE_PDU_SET) {
         if (snmp.errindex >= 0xFF) {
            snmp.errstat = SNMP_ERROR_tooBig;
            snmp.errindex = 0;
         } /* Endif */
         *snmp.errstatp  = snmp.errstat;
         *snmp.errindexp = snmp.errindex;
         snmp.STATS.ST_TX_TOTAL++;
         snmp.STATS.ST_TX_RESPONSE++;
         switch (snmp.errstat) {
         case SNMP_ERROR_noError:    break;
         case SNMP_ERROR_tooBig:     snmp.STATS.ST_TX_TOOBIG++;     break;
         case SNMP_ERROR_noSuchName: snmp.STATS.ST_TX_NOSUCHNAME++; break;
         case SNMP_ERROR_badValue:   snmp.STATS.ST_TX_BADVALUE++;   break;
         case SNMP_ERROR_readOnly:   snmp.STATS.ST_TX_READONLY++;   break;
         case SNMP_ERROR_genErr:     snmp.STATS.ST_TX_GENERR++;     break;
         default:                    snmp.STATS.ST_TX_OTHER++;      break;
         } /* Endswitch */
         sendto(sock, inbuf, inlen, 0, &addr, addrlen);
      } else {
         snmp.STATS.ST_RX_BAD_PARSE++;
         snmp.STATS.ST_RX_DISCARDED++;
      } /* Endif */

   } /* Endfor */

} /* Endbody */


/* EOF */
