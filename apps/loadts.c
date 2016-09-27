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
*** File: loadts.c
***
*** Comments:  This file contains the interface functions to RTCS_load()
***            for S-Record files using TFTP.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load_TFTP_SREC
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_load_TFTP_SREC
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename
         /* [IN] file to retrieve */
   )
{ /* Body */
   TFTP_DATA_STRUCT  tftp_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "netascii";

 #if (PSP_MEMORY_ADDRESSING_CAPABILITY == 32)
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC32, NULL, 0);
 #else
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC,   NULL, 0);
 #endif

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_exec_TFTP_SREC
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_exec_TFTP_SREC
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename
         /* [IN] file to retrieve */
   )
{ /* Body */
   TFTP_DATA_STRUCT  tftp_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "netascii";

#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 32)
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC32, NULL, RTCSLOADOPT_EXEC);
#else
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC,   NULL, RTCSLOADOPT_EXEC);
#endif

} /* Endbody */


/* EOF */
