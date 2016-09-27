#ifndef __snmpcfg_h__
#define __snmpcfg_h__
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
*** File: snmpcfg.h
***
*** Comments:  This file contains the definitions for configuring
***            optional features in SNMP.
***
*** SNMP is delivered as follows:
***
***      SNMPCFG_BUFFER_SIZE            512
***      SNMPCFG_COMMUNITY              "public"
***      SNMPCFG_SYSDESCR               "Precise/RTCS version <version>"
***      SNMPCFG_SYSSERVICES            8 (end-to-end)
***      SNMPCFG_HAVE_INT64             0
***
************************************************************************
*END*******************************************************************/

/*
** SNMP has two static buffers (one for receiving and one for generating
** responses), each of size SNMPCFG_BUFFER_SIZE.  RFCs 1157 and 1906 require
** this number to be at least 484.
*/
#ifndef SNMPCFG_BUFFER_SIZE
#define SNMPCFG_BUFFER_SIZE   512
#endif

/*
** The community string, used in both SNMPv1 and SNMPv2c.
*/
#ifndef SNMPCFG_COMMUNITY
//#define SNMPCFG_COMMUNITY     "public"
#define SNMPCFG_COMMUNITY     "private"
#endif

/*
** The value of the system.sysDescr variable.
*/
#ifndef SNMPCFG_SYSDESCR
#define SNMPCFG_NUMTOSTR(x)   #x
#define SNMPCFG_NUMTOSTR2(x)  SNMPCFG_NUMTOSTR(x)
#define SNMPCFG_SYSDESCR      "Precise/RTCS version " ## SNMPCFG_NUMTOSTR2(RTCS_VERSION_MAJOR) \
                              ## "." ## SNMPCFG_NUMTOSTR2(RTCS_VERSION_MINOR) \
                              ## "." ## SNMPCFG_NUMTOSTR2(RTCS_VERSION_REV)
#endif

/*
** The value of the system.sysServices variable.
*/
#ifndef SNMPCFG_SYSSERVICES
#define SNMPCFG_SYSSERVICES   8
#endif

/* CR 916 */
/*
** SNMPCFG_HAVE_INT64 enables support for 64-bit integers.
*/
#ifndef SNMPCFG_HAVE_INT64
#define SNMPCFG_HAVE_INT64    0
#endif
/* End CR 916 */

#endif
/* EOF */
