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
*** File: loadtb.c
***
*** Comments:  This file contains the interface functions to RTCS_load()
***            for binary files using TFTP.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load_TFTP_BIN
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_load_TFTP_BIN
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename,
         /* [IN] file to retrieve */
      uchar_ptr            dl_addr
         /* [IN] download start address */
   )
{ /* Body */
   TFTP_DATA_STRUCT     tftp_data;
   BINFILE_DATA_STRUCT  binfile_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "octet";

   binfile_data.DOWNLOAD_ADDR = dl_addr;

   return RTCS_load(FT_TFTP, &tftp_data, FF_BIN, &binfile_data, 0);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_exec_TFTP_BIN
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_exec_TFTP_BIN
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename,
         /* [IN] file to retrieve */
      uchar_ptr            dl_addr,
         /* [IN] download start address */
      uchar_ptr            exec_addr
         /* [IN] execution start address */
   )
{ /* Body */
   TFTP_DATA_STRUCT     tftp_data;
   BINFILE_DATA_STRUCT  binfile_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "octet";

   binfile_data.DOWNLOAD_ADDR = dl_addr;
   binfile_data.EXEC_ADDR     = exec_addr;

   return RTCS_load(FT_TFTP, &tftp_data, FF_BIN, &binfile_data, RTCSLOADOPT_EXEC);

} /* Endbody */


/* EOF */
