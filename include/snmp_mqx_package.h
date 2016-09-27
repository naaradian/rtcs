#ifndef __snmp_h__
#define __snmp_h__
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
*** File: snmp.h
***
*** Comments:  This file contains definitions for using the SNMP
***            agent.
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Constants
**
*/

/*
** SNMP error status values
*/

#define SNMP_ERROR_noError              0
#define SNMP_ERROR_tooBig               1
#define SNMP_ERROR_noSuchName           2
#define SNMP_ERROR_badValue             3
#define SNMP_ERROR_readOnly             4
#define SNMP_ERROR_genErr               5

#define SNMP_ERROR_noAccess             6
#define SNMP_ERROR_wrongType            7
#define SNMP_ERROR_wrongLength          8
#define SNMP_ERROR_wrongEncoding        9
#define SNMP_ERROR_wrongValue          10
#define SNMP_ERROR_noCreation          11
#define SNMP_ERROR_inconsistentValue   12
#define SNMP_ERROR_resourceUnavailable 13
#define SNMP_ERROR_commitFailed        14
#define SNMP_ERROR_undoFailed          15
#define SNMP_ERROR_authorizationError  16
#define SNMP_ERROR_notWritable         17
#define SNMP_ERROR_inconsistentName    18

/* decide whether an error-status uses the error-index */
#define SNMP_ERROR_USEINDEX(n)         ((n) > 1)

/* not real error codes -- used internally */
#define SNMP_ERROR_PARSE               0x7F
#define SNMP_ERROR_noSuchObject        ASN1_TYPE_ERR_NOOBJECT
#define SNMP_ERROR_noSuchInstance      ASN1_TYPE_ERR_NOINST
#define SNMP_ERROR_endOfMibView        ASN1_TYPE_ERR_ENDOFMIB

/*
** MIB access types
*/
#define RTCSMIB_ACCESS_READ   0x01
#define RTCSMIB_ACCESS_WRITE  0x02

/*
** MIB node types
*/
#define RTCSMIB_NODETYPE_INT_CONST     0
#define RTCSMIB_NODETYPE_INT_PTR       1
#define RTCSMIB_NODETYPE_INT_FN        2
/* CR 850 */
#define RTCSMIB_NODETYPE_UINT_CONST    3
#define RTCSMIB_NODETYPE_UINT_PTR      4
#define RTCSMIB_NODETYPE_UINT_FN       5
#define RTCSMIB_NODETYPE_DISPSTR_PTR   6
#define RTCSMIB_NODETYPE_DISPSTR_FN    7
#define RTCSMIB_NODETYPE_OCTSTR_FN     8
#define RTCSMIB_NODETYPE_OID_PTR       9
#define RTCSMIB_NODETYPE_OID_FN        10
/* End CR 850 */
/* CR 916 */
#if SNMPCFG_HAVE_INT64
#define RTCSMIB_NODETYPE_INT64_CONST   11
#define RTCSMIB_NODETYPE_INT64_PTR     12
#define RTCSMIB_NODETYPE_INT64_FN      13
#define RTCSMIB_NODETYPE_UINT64_CONST  14
#define RTCSMIB_NODETYPE_UINT64_PTR    15
#define RTCSMIB_NODETYPE_UINT64_FN     16
#endif
/* End CR 916 */

/*
** Used internally by the MIB instance parser
*/
#define RTCSMIB_OP_GET              0
#define RTCSMIB_OP_GETNEXT          1
#define RTCSMIB_OP_GETFIRST         2
#define RTCSMIB_OP_FINDNEXT         3
#define RTCSMIB_OP_SET              4

/***************************************
**
** Type definitions
**
*/

/*
** SNMP statistics for the SNMP MIB
*/

typedef struct {

   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   /* These are all included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_PARSE;
   uint_32  ST_RX_BAD_VERSION;
   uint_32  ST_RX_BAD_COMMUNITY;
   uint_32  ST_RX_BAD_PDU;

   /* Breakdown of all packets received and sent */
   uint_32  ST_RX_VAR_GETS;
   uint_32  ST_RX_VAR_SETS;

   uint_32  ST_RX_GETREQ;
   uint_32  ST_RX_GETNEXTREQ;
   uint_32  ST_RX_RESPONSE;
   uint_32  ST_RX_SETREQ;
   uint_32  ST_RX_TRAP;
   uint_32  ST_RX_GETBULKREQ;
   uint_32  ST_RX_INFORM;
   uint_32  ST_RX_V2TRAP;
   uint_32  ST_RX_REPORT;
   uint_32  ST_RX_OTHER;

   uint_32  ST_TX_GETREQ;
   uint_32  ST_TX_GETNEXTREQ;
   uint_32  ST_TX_RESPONSE;
   uint_32  ST_TX_SETREQ;
   uint_32  ST_TX_TRAP;
   uint_32  ST_TX_GETBULKREQ;
   uint_32  ST_TX_INFORM;
   uint_32  ST_TX_V2TRAP;
   uint_32  ST_TX_REPORT;
   uint_32  ST_TX_OTHER;

   uint_32  ST_TX_TOOBIG;
   uint_32  ST_TX_NOSUCHNAME;
   uint_32  ST_TX_BADVALUE;
   uint_32  ST_TX_READONLY;
   uint_32  ST_TX_GENERR;
   uint_32  ST_TX_OTHERERR;

} SNMP_STATS, _PTR_ SNMP_STATS_PTR;

/* CR 887 */
/*
** Internal state for the SNMP agent
*/

typedef struct {
   SNMP_STATS  STATS;

   uint_32     trapsock;
   char_ptr    community;
   uint_32     communitylen;

   uchar_ptr   inbuf;
   uint_32     inlen;
   uchar_ptr   outbuf;
   uint_32     outlen;

   uint_32     errstat;
   uint_32     errindex;
   uchar_ptr   errstatp;
   uchar_ptr   errindexp;

   uint_32     version;
   uint_32     pdutype;
   uint_32     nonrep;
   uint_32     reps;
} SNMP_PARSE, _PTR_ SNMP_PARSE_PTR;
/* End CR 887 */

/*
** Internal state for the MIB walker
*/

typedef struct {
   TCPIP_PARM  COMMON;

   uint_8      pdutype;

   uchar_ptr   inbuf;
   uint_32     inlen;
   uchar_ptr   outbuf;
   uint_32     outlen;

   uint_32     errstat;

   uchar_ptr   oidp;
   uint_32     oidlen;
   uint_32     totlen;

} RTCSMIB_WALK, _PTR_ RTCSMIB_WALK_PTR;

/*
** Structure of a single MIB node
*/

typedef struct rtcsmib_node {
   uint_32                    ID;

   struct rtcsmib_node _PTR_  NEXT;
   struct rtcsmib_node _PTR_  CHILD;
   struct rtcsmib_node _PTR_  PARENT;

   uint_32                    ACCESS;
   boolean (_CODE_PTR_        FIND)(uint_32, pointer, pointer _PTR_);
   boolean (_CODE_PTR_        PARSE)(RTCSMIB_WALK_PTR, uint_32, boolean (_CODE_PTR_)(uint_32, pointer, pointer _PTR_), boolean _PTR_, pointer _PTR_);
   uint_32                    ASN1_TYPE;

   uint_32                    TYPE;    /* ignored in nonleaf nodes */
   union {
      /* CR 850 */
      int_32                                INT_CONST;
      int_32_ptr                            INT_PTR;
      int_32                    (_CODE_PTR_ INT_FN)(pointer);
      uint_32                               UINT_CONST;
      uint_32_ptr                           UINT_PTR;
      uint_32                   (_CODE_PTR_ UINT_FN)(pointer);
      /* End CR 850 */
      uchar_ptr                             DISPSTR_PTR;
      uchar_ptr                 (_CODE_PTR_ DISPSTR_FN)(pointer);
      uchar_ptr                 (_CODE_PTR_ OCTSTR_FN)(pointer, uint_32 _PTR_);
      struct rtcsmib_node _PTR_             OID_PTR;
      struct rtcsmib_node _PTR_ (_CODE_PTR_ OID_FN)(pointer);
/* CR 916 */
#if SNMPCFG_HAVE_INT64
      int_64                                INT64_CONST;
      int_64_ptr                            INT64_PTR;
      int_64                    (_CODE_PTR_ INT64_FN)(pointer);
      uint_64                               UINT64_CONST;
      uint_64_ptr                           UINT64_PTR;
      uint_64                   (_CODE_PTR_ UINT64_FN)(pointer);
#endif
/* End CR 916 */
   } GET;
   uint_32 (_CODE_PTR_        SET)(pointer, uchar_ptr, uint_32);

} RTCSMIB_NODE, _PTR_ RTCSMIB_NODE_PTR;


/***************************************
**
** Prototypes
**
*/

/*
** Used by SNMP to make MIB requests:
*/

extern uint_32 RTCSMIB_request
(
   uint_8         pdutype,
   uchar_ptr      invarp,
   uint_32        invarlen,
   uchar_ptr      outvarp,
   uint_32        outvarlen,
   uint_32 _PTR_  writelen
);

/*
** Utilities for MIB implementations:
*/

extern void RTCSMIB_mib_add
(
   RTCSMIB_NODE_PTR  mib
);

/* CR 850 */
extern int_32 RTCSMIB_int_read    (uchar_ptr, uint_32);
/* End CR 850 */
extern boolean RTCSMIB_id_read8   (RTCSMIB_WALK_PTR, uchar _PTR_);
extern boolean RTCSMIB_id_read32  (RTCSMIB_WALK_PTR, uint_32 _PTR_);
extern boolean RTCSMIB_id_write8  (RTCSMIB_WALK_PTR, uchar);
extern boolean RTCSMIB_id_write32 (RTCSMIB_WALK_PTR, uint_32);

extern boolean MIB_instance_zero
(
   RTCSMIB_WALK_PTR     mib,
   uint_32              op,
   boolean (_CODE_PTR_  find)(uint_32, pointer, pointer _PTR_),
   boolean _PTR_        found,
   pointer _PTR_        instance
);

#endif
/* EOF */
