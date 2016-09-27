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
*** File: mib.c
***
*** Comments:  This file contains an implementation of a MIB
***            walker for the SNMP agent.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "snmpcfg.h"
#include "asn1.h"
#include "snmp.h"

static boolean RTCSMIB_output (RTCSMIB_WALK_PTR, RTCSMIB_NODE_PTR, pointer);

/***********************************************************
**
** BEGIN RFC1155-SMI, SNMPv2-SMI definitions
**
*/

extern RTCSMIB_NODE MIBNODE_iso_org;
extern RTCSMIB_NODE MIBNODE_dod;
extern RTCSMIB_NODE MIBNODE_internet;
extern RTCSMIB_NODE MIBNODE_directory;
extern RTCSMIB_NODE MIBNODE_mgmt;
extern RTCSMIB_NODE MIBNODE_experimental;
extern RTCSMIB_NODE MIBNODE_private;
extern RTCSMIB_NODE MIBNODE_enterprises;
extern RTCSMIB_NODE MIBNODE_security;
extern RTCSMIB_NODE MIBNODE_snmpV2;
extern RTCSMIB_NODE MIBNODE_snmpDomains;
extern RTCSMIB_NODE MIBNODE_snmpProxys;
extern RTCSMIB_NODE MIBNODE_snmpModules;

RTCSMIB_NODE MIBNODE_iso_org = {
   43,

   NULL,
   &MIBNODE_dod,
   NULL,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_dod = {
   6,

   NULL,
   &MIBNODE_internet,
   &MIBNODE_iso_org,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_internet = {
   1,

   NULL,
   &MIBNODE_directory,
   &MIBNODE_dod,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_directory = {
   1,

   &MIBNODE_mgmt,
   NULL,
   &MIBNODE_internet,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_mgmt = {
   2,

   &MIBNODE_experimental,
   NULL,
   &MIBNODE_internet,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_experimental = {
   3,

   &MIBNODE_private,
   NULL,
   &MIBNODE_internet,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_private = {
   4,

   &MIBNODE_security,
   &MIBNODE_enterprises,
   &MIBNODE_internet,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_enterprises = {
   1,

   NULL,
   NULL,
   &MIBNODE_private,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_security = {
   5,

   &MIBNODE_snmpV2,
   NULL,
   &MIBNODE_internet,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_snmpV2 = {
   6,

   NULL,
   &MIBNODE_snmpDomains,
   &MIBNODE_internet,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_snmpDomains = {
   1,

   &MIBNODE_snmpProxys,
   NULL,
   &MIBNODE_snmpV2,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_snmpProxys = {
   2,

   &MIBNODE_snmpModules,
   NULL,
   &MIBNODE_snmpV2,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE MIBNODE_snmpModules = {
   3,

   NULL,
   NULL,
   &MIBNODE_snmpV2,

   0, NULL, NULL, 0, 0, {0}, NULL
};

RTCSMIB_NODE_PTR RTCSMIB_root = &MIBNODE_iso_org;

/*
**
** END RFC1155-SMI, SNMPv2-SMI definitions
**
***********************************************************/


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_int_read
* Returned Value  : an ASN.1 INTEGER
* Comments  :  Read an INTEGER at varptr, varlen bytes long.
*
*END*-----------------------------------------------------------------*/

/* CR 850 */
int_32 RTCSMIB_int_read
   (
      uchar_ptr   varptr,
      uint_32     varlen
   )
{ /* Body */
   int_32 varval = 0;
   if (varlen) {
      varval = *(signed char _PTR_)varptr;
      varlen--;
   } /* Endif */
   while (varlen--) {
      varval <<= 8;
      varval += *varptr++;
   } /* Endwhile */
   return varval;
} /* Endbody */
/* End CR 850 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_id_read8
* Returned Value  : TRUE if successful
* Comments  :  Read the next ID from an object ID into a uchar.
*
*END*-----------------------------------------------------------------*/

boolean RTCSMIB_id_read8
   (
      RTCSMIB_WALK_PTR  mib,
      uchar _PTR_       index
   )
{ /* Body */
   ASN1_READ_ID(mib,*index);
   return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_id_read32
* Returned Value  : TRUE if successful
* Comments  :  Read the next ID from an object ID into a uint_32.
*
*END*-----------------------------------------------------------------*/

boolean RTCSMIB_id_read32
   (
      RTCSMIB_WALK_PTR  mib,
      uint_32 _PTR_     index
   )
{ /* Body */
   ASN1_READ_ID(mib,*index);
   return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_id_write8
* Returned Value  : TRUE if successful
* Comments  :  Appends an ID (up to 8 bits) to an object ID.
*
*END*-----------------------------------------------------------------*/

boolean RTCSMIB_id_write8
   (
      RTCSMIB_WALK_PTR  mib,
      uchar             index
   )
{ /* Body */
   ASN1_WRITE_ID_OCTET(mib,index);
   return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_id_write32
* Returned Value  : TRUE if successful
* Comments  :  Appends an ID (up to 32 bits) to an object ID.
*
*END*-----------------------------------------------------------------*/

boolean RTCSMIB_id_write32
   (
      RTCSMIB_WALK_PTR  mib,
      uint_32           index
   )
{ /* Body */
   ASN1_WRITE_ID_INT(mib,index);
   return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_mib_add
* Returned Value  : void
* Comments  :  Dynamically adds a MIB to the tree.
*
*END*-----------------------------------------------------------------*/

void RTCSMIB_mib_add
   (
      RTCSMIB_NODE_PTR  mib
   )
{ /* Body */
   RTCSMIB_NODE_PTR _PTR_ node;

   for (node = &mib->PARENT->CHILD;
        *node && (*node)->ID < mib->ID;
        node = &(*node)->NEXT);

   mib->NEXT = *node;
   *node = mib;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : MIB_instance_zero
* Returned Value  : TRUE on successful parse, FALSE on parse error
* Comments  :  The default instance parser -- assume there is a
*              single instance with index 0.
*
*END*-----------------------------------------------------------------*/

boolean MIB_instance_zero
   (
      RTCSMIB_WALK_PTR     mib,
      uint_32              op,
      boolean (_CODE_PTR_  find)(uint_32, pointer, pointer _PTR_),
      boolean _PTR_        found,
      pointer _PTR_        instance
   )
{ /* Body */
   uint_32 index;

   *found = FALSE;
   *instance = NULL;

   switch (op) {

   case RTCSMIB_OP_GET:
   case RTCSMIB_OP_SET:
      if (mib->inlen == 0) {
         break;
      } /* Endif */
      ASN1_READ_ID(mib,index);
      if ((mib->inlen == 0) && (index == 0)) {
         *found = TRUE;
      } /* Endif */
      break;

   case RTCSMIB_OP_GETNEXT:
      if (mib->inlen == 0) {
         *found = TRUE;
      } /* Endif */
      break;

   case RTCSMIB_OP_GETFIRST:
      *found = TRUE;
      break;

   } /* Endswitch */

   if (*found && (op != RTCSMIB_OP_SET)) {
      ASN1_WRITE_ID(mib, 0);
   } /* Endif */

   return TRUE;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_get
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments  :  Finds a VarBind in the MIB.
*
*END*-----------------------------------------------------------------*/

boolean RTCSMIB_get
   (
      RTCSMIB_WALK_PTR  mib
   )
{ /* Body */
   RTCSMIB_NODE_PTR     node = RTCSMIB_root;
   boolean (_CODE_PTR_  find)(uint_32, pointer, pointer _PTR_) = NULL;
   uint_32              varlen;
   uint_32              id;
   pointer              instance;
   boolean              found;

   ASN1_READ_TYPELEN_EXPECT(mib, ASN1_TYPE_SEQUENCE, varlen);
   mib->inlen = varlen;
   ASN1_READ_TYPELEN_EXPECT(mib, ASN1_TYPE_OBJECT, varlen);
   mib->inlen = varlen;

   mib->oidlen = 0;
   ASN1_READ_ID(mib,id);

   for (;;) {

      /*
      ** First, search the MIB for the requested leaf node.
      */

      if ((node->ID < id) && node->NEXT) {

         node = node->NEXT;
         continue;

      } else if (node->ID == id) {

         mib->oidlen += (id > 0x7F) ? 2 : 1;

         if (node->CHILD) {
            if (mib->inlen == 0) {
               mib->errstat = SNMP_ERROR_noSuchObject;
               return FALSE;
            } /* Endif */
            ASN1_READ_ID(mib,id);
            if (node->FIND) find = node->FIND;
            node = node->CHILD;
            continue;
         } /* Endif */

      } else {
         mib->errstat = SNMP_ERROR_noSuchObject;
         return FALSE;

      } /* Endif */

      /*
      ** We've found our leaf node.  Parse the instance identifier.
      */

      if (!(node->ACCESS & RTCSMIB_ACCESS_READ) || !node->PARSE) {
         mib->errstat = SNMP_ERROR_noSuchObject;
         return FALSE;
      } /* Endif */

      if (mib->oidlen + 4 > mib->outlen) {
         mib->errstat = SNMP_ERROR_tooBig;
         return FALSE;
      } /* Endif */
      mib->outbuf += mib->oidlen + 4;
      mib->outlen -= mib->oidlen + 4;
      mib->oidp = mib->outbuf;

      if (!node->PARSE(mib, RTCSMIB_OP_GET, find, &found, &instance)) return FALSE;

      if (!found) {
         mib->errstat = SNMP_ERROR_noSuchInstance;
         return FALSE;
      } /* Endif */

      /*
      ** The instance exists -- output the value.
      */

      return RTCSMIB_output(mib, node, instance);

   } /* Endfor */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_powerful_getnext
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments  :  Finds the VarBind in the MIB lexicographically greater
*              than the one provided.
*
*END*-----------------------------------------------------------------*/

boolean RTCSMIB_powerful_getnext
   (
      RTCSMIB_WALK_PTR  mib
   )
{ /* Body */
   RTCSMIB_NODE_PTR     node = RTCSMIB_root;
   boolean (_CODE_PTR_  find)(uint_32, pointer, pointer _PTR_) = NULL;
   uint_32              varlen;
   uint_32              op, id;
   pointer              instance;
   boolean              found;

   ASN1_READ_TYPELEN_EXPECT(mib, ASN1_TYPE_SEQUENCE, varlen);
   mib->inlen = varlen;
   ASN1_READ_TYPELEN_EXPECT(mib, ASN1_TYPE_OBJECT, varlen);
   mib->inlen = varlen;

   mib->oidlen = (node->ID > 0x7F) ? 2 : 1;
   ASN1_READ_ID(mib,id);

   op = RTCSMIB_OP_GETNEXT;

   for (;;) {

      /*
      ** First, search the MIB for the requested leaf node.
      */

      switch (op) {
      case RTCSMIB_OP_GETNEXT:

         if (node->ID < id) {

            if (node->NEXT) {
               if ((node->ID <= 0x7F) && (node->NEXT->ID > 0x7F)) mib->oidlen++;
               node = node->NEXT;
            } else {
               op = RTCSMIB_OP_FINDNEXT;
            } /* Endif */

            continue;

         } else if (node->ID == id) {

            if (node->CHILD) {
               if (mib->inlen == 0) {
                  op = RTCSMIB_OP_GETFIRST;
                  continue;
               } /* Endif */
               ASN1_READ_ID(mib,id);
               if (node->FIND) find = node->FIND;
               node = node->CHILD;
               mib->oidlen += (node->ID > 0x7F) ? 2 : 1;
               continue;
            } /* Endif */

            /* We might have found our leaf node -- break out of the switch */
            break;

         } else {
            op = RTCSMIB_OP_GETFIRST;
            continue;

         } /* Endif */

      case RTCSMIB_OP_GETFIRST:

         if (node->CHILD) {
            if (node->FIND) find = node->FIND;
            node = node->CHILD;
            mib->oidlen += (node->ID > 0x7F) ? 2 : 1;
            continue;
         } /* Endif */

         /* We might have found our leaf node -- break out of the switch */
         break;

      case RTCSMIB_OP_FINDNEXT:

         if (node->NEXT) {
            if ((node->ID <= 0x7F) && (node->NEXT->ID > 0x7F)) mib->oidlen++;
            node = node->NEXT;
            op = RTCSMIB_OP_GETFIRST;
            continue;
         } /* Endif */

         mib->oidlen -= (node->ID > 0x7F) ? 2 : 1;
         node = node->PARENT;

         if (!node) {
            mib->errstat = SNMP_ERROR_endOfMibView;
            return FALSE;
         } /* Endif */

         continue;

      } /* Endswitch */

      /*
      ** We may have found our leaf node.  Parse the instance identifier.
      */

      if (!(node->ACCESS & RTCSMIB_ACCESS_READ) || !node->PARSE) {
         op = RTCSMIB_OP_FINDNEXT;
         continue;
      } /* Endif */

      if (mib->oidlen + 4 > mib->outlen) {
         mib->errstat = SNMP_ERROR_tooBig;
         return FALSE;
      } /* Endif */
      mib->outbuf += mib->oidlen + 4;
      mib->outlen -= mib->oidlen + 4;
      mib->oidp = mib->outbuf;

      if (!node->PARSE(mib, op, find, &found, &instance)) return FALSE;

      if (!found) {
         mib->outbuf -= mib->oidlen + 4;
         mib->outlen += mib->oidlen + 4;
         op = RTCSMIB_OP_FINDNEXT;
         continue;
      } /* Endif */

      /*
      ** The instance exists -- output the value.
      */

      return RTCSMIB_output(mib, node, instance);

   } /* Endfor */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_set
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments  :  Writes a VarBind in the MIB.
*
*END*-----------------------------------------------------------------*/

boolean RTCSMIB_set
   (
      RTCSMIB_WALK_PTR  mib
   )
{ /* Body */
   RTCSMIB_NODE_PTR     node = RTCSMIB_root;
   uchar_ptr            oidptr, valptr;
   uint_32              oidlen, vallen, valtype;
   boolean (_CODE_PTR_  find)(uint_32, pointer, pointer _PTR_) = NULL;
   uint_32              varlen;
   uint_32              id;
   pointer              instance;
   boolean              found;

   ASN1_READ_TYPELEN_EXPECT(mib, ASN1_TYPE_SEQUENCE, varlen);
   mib->inlen = varlen;
   ASN1_READ_TYPELEN_EXPECT(mib, ASN1_TYPE_OBJECT, oidlen);
   oidptr = mib->inbuf;

   /* Read the variable's type and length */
   mib->inbuf += oidlen;
   mib->inlen -= oidlen;
   ASN1_READ_TYPELEN(mib, valtype, vallen);
   valptr = mib->inbuf;

   mib->inbuf = oidptr;
   mib->inlen = oidlen;

   ASN1_READ_ID(mib,id);

   for (;;) {

      /*
      ** First, search the MIB for the requested leaf node.
      */

      if ((node->ID < id) && node->NEXT) {

         node = node->NEXT;
         continue;

      } else if (node->ID == id) {

         if (node->CHILD) {
            if (mib->inlen == 0) {
               mib->errstat = SNMP_ERROR_notWritable;
               return FALSE;
            } /* Endif */
            ASN1_READ_ID(mib,id);
            if (node->FIND) find = node->FIND;
            node = node->CHILD;
            continue;
         } /* Endif */

      } else {
         mib->errstat = SNMP_ERROR_notWritable;
         return FALSE;

      } /* Endif */

      /*
      ** We've found our leaf node.  Parse the instance identifier.
      */

      if (!(node->ACCESS & RTCSMIB_ACCESS_WRITE) || !node->PARSE) {
         mib->errstat = SNMP_ERROR_notWritable;
         return FALSE;
      } /* Endif */

      if (node->ASN1_TYPE != valtype) {
         mib->errstat = SNMP_ERROR_wrongType;
         return FALSE;
      } /* Endif */

      if (!node->PARSE(mib, RTCSMIB_OP_SET, find, &found, &instance)) return FALSE;

      if (!found) {
         mib->errstat = SNMP_ERROR_noCreation;
         return FALSE;
      } /* Endif */

      /*
      ** The instance exists -- set the value.
      */

      mib->totlen = 0;
      mib->errstat = node->SET(instance, valptr, vallen);
      return (mib->errstat == SNMP_ERROR_noError);

   } /* Endfor */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_output
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments  :  Writes a VarBind to outbuf.
*
*END*-----------------------------------------------------------------*/

boolean RTCSMIB_output
   (
      RTCSMIB_WALK_PTR  mib,
      RTCSMIB_NODE_PTR  node,
      pointer           instance
   )
{ /* Body */
   uint_32  oidlen, varlen, varheadlen, totlen, shiftlen;

   /* CR 850 */
   int_32            sinteger;
   uint_32           uinteger, octlen;
   /* End CR 850 */
/* CR 916 */
#if SNMPCFG_HAVE_INT64
   int_64            sinteger64;
   uint_64           uinteger64;
#endif
/* End CR 916 */
   uchar_ptr         octstr;
   RTCSMIB_NODE_PTR  oidptr, tempoidptr;

   switch (node->TYPE) {
   /* CR 850 */
   case RTCSMIB_NODETYPE_INT_CONST:    sinteger = node->GET.INT_CONST;                   break;
   case RTCSMIB_NODETYPE_INT_PTR:      sinteger = *node->GET.INT_PTR;                    break;
   case RTCSMIB_NODETYPE_INT_FN:       sinteger = node->GET.INT_FN(instance);            break;
   case RTCSMIB_NODETYPE_UINT_CONST:   uinteger = node->GET.UINT_CONST;                  break;
   case RTCSMIB_NODETYPE_UINT_PTR:     uinteger = *node->GET.UINT_PTR;                   break;
   case RTCSMIB_NODETYPE_UINT_FN:      uinteger = node->GET.UINT_FN(instance);           break;
   /* End CR 850 */
   case RTCSMIB_NODETYPE_DISPSTR_PTR:  octstr  = node->GET.DISPSTR_PTR;                  break;
   case RTCSMIB_NODETYPE_DISPSTR_FN:   octstr  = node->GET.DISPSTR_FN(instance);         break;
   case RTCSMIB_NODETYPE_OCTSTR_FN:    octstr  = node->GET.OCTSTR_FN(instance, &octlen); break;
   case RTCSMIB_NODETYPE_OID_PTR:      oidptr  = node->GET.OID_PTR;                      break;
   case RTCSMIB_NODETYPE_OID_FN:       oidptr  = node->GET.OID_FN(instance);             break;
/* CR 916*/
#if SNMPCFG_HAVE_INT64
   case RTCSMIB_NODETYPE_INT64_CONST:  sinteger64 = node->GET.INT64_CONST;               break;
   case RTCSMIB_NODETYPE_INT64_PTR:    sinteger64 = *node->GET.INT64_PTR;                break;
   case RTCSMIB_NODETYPE_INT64_FN:     sinteger64 = node->GET.INT64_FN(instance);        break;
   case RTCSMIB_NODETYPE_UINT64_CONST: uinteger64 = node->GET.UINT64_CONST;              break;
   case RTCSMIB_NODETYPE_UINT64_PTR:   uinteger64 = *node->GET.UINT64_PTR;               break;
   case RTCSMIB_NODETYPE_UINT64_FN:    uinteger64 = node->GET.UINT64_FN(instance);       break;
#endif
/* End CR 916 */
   } /* Endswitch */

   switch (node->TYPE) {
   /* CR 850 */
   case RTCSMIB_NODETYPE_INT_CONST:
   case RTCSMIB_NODETYPE_INT_PTR:
   case RTCSMIB_NODETYPE_INT_FN:
      if ((sinteger >= -0x80) && (sinteger < 0x80)) {
         varlen = 1;
      } else if ((sinteger >= -0x8000) && (sinteger < 0x8000)) {
         varlen = 2;
      } else if ((sinteger >= -0x800000) && (sinteger < 0x800000)) {
         varlen = 3;
      } else {
         varlen = 4;
      } /* Endif */

      break;

   case RTCSMIB_NODETYPE_UINT_CONST:
   case RTCSMIB_NODETYPE_UINT_PTR:
   case RTCSMIB_NODETYPE_UINT_FN:
      if (uinteger < 0x80) {
         varlen = 1;
      } else if (uinteger < 0x8000) {
         varlen = 2;
      } else if (uinteger < 0x800000) {
         varlen = 3;
      } else if (uinteger < 0x80000000) {
         varlen = 4;
      } else {
         varlen = 5;
      } /* Endif */

      break;
   /* End CR 850 */

   case RTCSMIB_NODETYPE_DISPSTR_PTR:
   case RTCSMIB_NODETYPE_DISPSTR_FN:
      octlen = 0;
      if (octstr) {
         while (octstr[octlen] != '\0') {
            octlen++;
            if (octlen > mib->outlen) {
               mib->errstat = SNMP_ERROR_tooBig;
               return FALSE;
            } /* Endif */
         } /* Endwhile */
      } /* Endif */

      /* Fall through */
   case RTCSMIB_NODETYPE_OCTSTR_FN:
      varlen = octlen;
      break;

   case RTCSMIB_NODETYPE_OID_PTR:
   case RTCSMIB_NODETYPE_OID_FN:
      if (!oidptr) {
         varlen = 1;
      } else {
         tempoidptr = oidptr;
         varlen = 0;
         while (tempoidptr) {
            varlen += (tempoidptr->ID > 0x7F) ? 2 : 1;
            tempoidptr = tempoidptr->PARENT;
         } /* Endwhile */
      } /* Endif */
      break;

/* CR 916 */
#if SNMPCFG_HAVE_INT64
   case RTCSMIB_NODETYPE_INT64_CONST:
   case RTCSMIB_NODETYPE_INT64_PTR:
   case RTCSMIB_NODETYPE_INT64_FN:
      if ((sinteger64 >= -0x80LL) && (sinteger64 < 0x80LL)) {
         varlen = 1;
      } else if ((sinteger64 >= -0x8000LL) && (sinteger64 < 0x8000LL)) {
         varlen = 2;
      } else if ((sinteger64 >= -0x800000LL) && (sinteger64 < 0x800000LL)) {
         varlen = 3;
      } else if ((sinteger64 >= -0x80000000LL) && (sinteger64 < 0x80000000LL)) {
         varlen = 4;
      } else if ((sinteger64 >= -0x8000000000LL) && (sinteger64 < 0x8000000000LL)) {
         varlen = 5;
      } else if ((sinteger64 >= -0x800000000000LL) && (sinteger64 < 0x800000000000LL)) {
         varlen = 6;
      } else if ((sinteger64 >= -0x80000000000000LL) && (sinteger64 < 0x80000000000000LL)) {
         varlen = 7;
      } else {
         varlen = 8;
      } /* Endif */

      break;

   case RTCSMIB_NODETYPE_UINT64_CONST:
   case RTCSMIB_NODETYPE_UINT64_PTR:
   case RTCSMIB_NODETYPE_UINT64_FN:
      if (uinteger64 < 0x80ULL) {
         varlen = 1;
      } else if (uinteger64 < 0x8000ULL) {
         varlen = 2;
      } else if (uinteger64 < 0x800000ULL) {
         varlen = 3;
      } else if (uinteger64 < 0x80000000ULL) {
         varlen = 4;
      } else if (uinteger64 < 0x8000000000ULL) {
         varlen = 5;
      } else if (uinteger64 < 0x800000000000ULL) {
         varlen = 6;
      } else if (uinteger64 < 0x80000000000000ULL) {
         varlen = 7;
      } else if (uinteger64 < 0x8000000000000000ULL) {
         varlen = 8;
      } else {
         varlen = 9;
      } /* Endif */

      break;
#endif
/* End CR 916 */

   } /* Endswitch */

   /*
   ** The caller assumed that the entire VarBind would occupy at most
   ** 127 bytes.  This is usually (but not always) the case.  Now,
   ** node->PARSE() has already written the instance part of the
   ** object ID into outbuf, with the 127-byte assumption.
   **
   ** If it turns out that the VarBind will exceed 127 bytes, the
   ** instance number will have to be shifted to make room for the
   ** larger headers.
   */

   oidlen = mib->outbuf - mib->oidp;   /* oidlen = length of instance in OID */
   totlen = 4 + oidlen + varlen;
   shiftlen = 0;

   if (mib->oidlen + oidlen > 0x7F) {
      if (mib->oidlen + oidlen > 0xFF) {
         totlen++;
         shiftlen++;
      } /* Endif */
      totlen++;
      shiftlen++;
   } /* Endif */

   varheadlen = 2;
   if (varlen > 0x7F) {
      if (varlen > 0xFF) {
         varheadlen++;
         totlen++;
      } /* Endif */
      varheadlen++;
      totlen++;
   } /* Endif */

   if (totlen > 0x7F) {
      if (totlen > 0xFF) {
         shiftlen++;
      } /* Endif */
      shiftlen++;
   } /* Endif */

   if (shiftlen + varheadlen + varlen > mib->outlen) {
      mib->errstat = SNMP_ERROR_tooBig;
      return FALSE;
   } /* Endif */

   if (shiftlen) {
      uchar_ptr shiftp;

      for (shiftp = mib->outbuf; shiftp > mib->oidp;) {
         shiftp--;
         shiftp[shiftlen] = shiftp[0];
      } /* Endfor */

   } /* Endif */

   mib->outbuf += shiftlen + varheadlen + varlen;
   mib->oidp += shiftlen;

   /*
   ** Everything is in order.  Write out the response.
   */

   varlen = 0;
   switch (node->TYPE) {
   /* CR 850 */
   case RTCSMIB_NODETYPE_INT_CONST:
   case RTCSMIB_NODETYPE_INT_PTR:
   case RTCSMIB_NODETYPE_INT_FN:
      ASN1_BKWRITE_TYPELEN_BIGINT(mib, varlen, node->ASN1_TYPE, sinteger);
      break;
   case RTCSMIB_NODETYPE_UINT_CONST:
   case RTCSMIB_NODETYPE_UINT_PTR:
   case RTCSMIB_NODETYPE_UINT_FN:
      ASN1_BKWRITE_TYPELEN_BIGUINT(mib, varlen, node->ASN1_TYPE, uinteger);
      break;
   /* End CR 850 */
   case RTCSMIB_NODETYPE_DISPSTR_PTR:
   case RTCSMIB_NODETYPE_DISPSTR_FN:
   case RTCSMIB_NODETYPE_OCTSTR_FN:
      ASN1_BKWRITE_TYPELEN_BYTES(mib, varlen, node->ASN1_TYPE, octstr, octlen);
      break;
   case RTCSMIB_NODETYPE_OID_PTR:
   case RTCSMIB_NODETYPE_OID_FN:
      if (!oidptr) {
         ASN1_BKWRITE_ID(mib, varlen, 0);
      } else {
         while (oidptr) {
            ASN1_BKWRITE_ID(mib, varlen, oidptr->ID);
            oidptr = oidptr->PARENT;
         } /* Endwhile */
      } /* Endif */
      ASN1_BKWRITE_TYPELEN(mib, varlen, ASN1_TYPE_OBJECT, varlen);
      break;
/* CR 916 */
#if SNMPCFG_HAVE_INT64
   case RTCSMIB_NODETYPE_INT64_CONST:
   case RTCSMIB_NODETYPE_INT64_PTR:
   case RTCSMIB_NODETYPE_INT64_FN:
      ASN1_BKWRITE_TYPELEN_BIGINT(mib, varlen, node->ASN1_TYPE, sinteger64);
      break;
   case RTCSMIB_NODETYPE_UINT64_CONST:
   case RTCSMIB_NODETYPE_UINT64_PTR:
   case RTCSMIB_NODETYPE_UINT64_FN:
      ASN1_BKWRITE_TYPELEN_BIGUINT(mib, varlen, node->ASN1_TYPE, uinteger64);
      break;
#endif
/* End CR 916 */
   } /* Endswitch */

   mib->outbuf = mib->oidp;
   while (node) {
      ASN1_BKWRITE_ID(mib, oidlen, node->ID);
      node = node->PARENT;
   } /* Endwhile */
   ASN1_BKWRITE_TYPELEN(mib, oidlen, ASN1_TYPE_OBJECT, oidlen);

   totlen = oidlen + varlen;
   ASN1_BKWRITE_TYPELEN(mib, totlen, ASN1_TYPE_SEQUENCE, totlen);

   mib->totlen = totlen;
   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_request_internal
* Parameters      :
*
*     uint_8            request     [IN] identification for echo message.
*     uchar_ptr         inbuf       [IN] pointer to requested VarBind
*     uint_32           inlen       [IN] length of requested VarBind
*     uchar_ptr         outbuf      [IN] where to write response
*     uint_32           outlen      [IN] available space for response
*     boolean           errstat     [OUT] whether or not an error occurred
*     uchar_ptr         oidp        not used
*     uint_32           oidlen      not used
*     uint_32           totlen      [OUT] actual length of response
*
* Comments        :
*     Service a single Get request.
*
*END*-----------------------------------------------------------------*/

void RTCSMIB_request_internal
   (
      RTCSMIB_WALK_PTR  mib
   )
{ /* Body */
   boolean  ok = FALSE;

   mib->errstat = SNMP_ERROR_noError;

   switch (mib->pdutype) {
   case ASN1_TYPE_PDU_GET:
      ok = RTCSMIB_get(mib);
      break;
   case ASN1_TYPE_PDU_GETNEXT:
      ok = RTCSMIB_powerful_getnext(mib);
      break;
   case ASN1_TYPE_PDU_SET:
      ok = RTCSMIB_set(mib);
      break;
   } /* Endswitch */

   if (!ok && !mib->errstat) {
      mib->errstat = SNMP_ERROR_PARSE;
   } /* Endif */

   RTCSCMD_complete(mib, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSMIB_request
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments  :
*     Issue a MIB request to the RTCS task.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSMIB_request
   (
      uint_8         pdutype,
      uchar_ptr      invarp,
      uint_32        invarlen,
      uchar_ptr      outvarp,
      uint_32        outvarlen,
      uint_32 _PTR_  writelen
   )
{ /* Body */
   RTCSMIB_WALK   mib;

   mib.pdutype = pdutype;
   mib.inbuf   = invarp;
   mib.inlen   = invarlen;
   mib.outbuf  = outvarp;
   mib.outlen  = outvarlen;

   if (RTCSCMD_issue(mib, RTCSMIB_request_internal)) {
      return SNMP_ERROR_genErr;
   } /* Endif */

   if (!mib.errstat) {
      *writelen = mib.totlen;
   } /* Endif */

   return mib.errstat;

} /* Endbody */

/* EOF */
