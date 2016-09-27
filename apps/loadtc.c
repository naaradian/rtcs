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
*** File: loadtc.c
***
*** Comments:  This file contains the interface functions to RTCS_load()
***            for COFF files using TFTP.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load_TFTP_COFF
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_load_TFTP_COFF
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename
         /* [IN] file to retrieve */
   )
{ /* Body */
   TFTP_DATA_STRUCT     tftp_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "octet";

   return RTCS_load(FT_TFTP, &tftp_data, FF_COFF, NULL, 0);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_exec_TFTP_COFF
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_exec_TFTP_COFF
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename
         /* [IN] file to retrieve */
   )
{ /* Body */
   TFTP_DATA_STRUCT     tftp_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "octet";

   return RTCS_load(FT_TFTP, &tftp_data, FF_COFF, NULL, RTCSLOADOPT_EXEC);

} /* Endbody */


/* EOF */
