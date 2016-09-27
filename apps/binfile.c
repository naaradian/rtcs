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
*** File: binfile.c
***
*** Comments:  This file contains the binary file decoder for the
***            Exec function of the RTCS Communication Library.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>


/* The binary file decoder information to pass to RTCS_load() */
uint_32 BIN_start  (pointer ff_data);
uint_32 BIN_decode (uint_32 size, uchar_ptr data);
uint_32 BIN_eod    (void);
void    BIN_exec   (void);

//my static FF_CALL_STRUCT ff_bin = {
 FF_CALL_STRUCT ff_bin = { //my
   BIN_start,
   BIN_decode,
   BIN_eod,
   BIN_exec
};

//my const FF_CALL_STRUCT_PTR FF_BIN = &ff_bin;
FF_CALL_STRUCT_PTR FF_BIN = &ff_bin;


/* Binary file decoder state information */
static uchar_ptr download_address;
static uchar_ptr execute_address;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_start
* Returned Value : error code
* Comments       : Routine to begin loading a binary file.
*
*END*-----------------------------------------------------------------*/

uint_32 BIN_start
   (
      pointer  ff_data
   )
{ /* Body */

   download_address = ((BINFILE_DATA_STRUCT_PTR)ff_data)->DOWNLOAD_ADDR;
   execute_address  = ((BINFILE_DATA_STRUCT_PTR)ff_data)->EXEC_ADDR;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_decode
* Returned Value : status
* Comments       : Copy the transferred bytes to the target.
*
*END*-----------------------------------------------------------------*/

uint_32 BIN_decode
   (
      uint_32   size,
         /* [IN] number of bytes to decode */
      uchar_ptr data
         /* [IN] bytes to decode */
   )
{ /* Body */
/*
 uint_32 cnt;
 uint_32 val;
 uint_32 tmp;
 uint_32 *  download_address_32; //this pointer is need for sdram!
	  //t    download_address_32 = (uint_32 *)(0x21000000) ;
 		download_address_32 = (uint_32 *)download_address;

//ok	tmp =0x12345678;
	tmp = size;	
 	for(cnt = 0; cnt < 32; cnt++)
//	for(cnt = 0; cnt < 128; cnt++)
//	for(cnt = 0; cnt < ((size & 0xff) >> 2); cnt++)
	{
	 //  val = *data++;
	 //  val <<= 8;
	 //  val += *data++;
	 //  val <<= 8;
	 //  val += *data++;
	 //  val <<= 8;
	 //  val += *data++;
	
	//  *download_address =  tmp; //t
	*download_address_32++ = tmp; //t
	//download_address_32 >>= 8; //step for address is 1 bytes!
	   tmp >>= 8; 	
	download_address++; 
	}

*/


   _mem_copy(data, download_address, size); //t
 //ok    _mem_copy(data, download_address, 4);//t
 //t   _mem_copy(data, download_address, 128); //256 - bad 132 128 - ok 40 -ok
//	data += 128;
// _mem_copy(data, download_address, 128); //256 - bad 132 128 - ok 40 -ok
//data += 128;
// _mem_copy(data, download_address, 128); //256 - bad 132 128 - ok 40 -ok
//data += 128;
// _mem_copy(data, download_address, 128); //256 - bad 132 128 - ok 40 -ok
// _mem_copy(download_address, download_address, size); //t
 //t  download_address += size;
//	download_address += 128;
 //download_address += 512;
  download_address += size; //t
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_eod
* Returned Value : error code
* Comments       : Routine to end decoding.
*
*END*-----------------------------------------------------------------*/

uint_32 BIN_eod
   (
      void
   )
{ /* Body */

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_exec
* Returned Value : none
* Comments       : Routine to execute a decoded binary file.
*
*END*-----------------------------------------------------------------*/

void BIN_exec
   (
      void
   )
{ /* Body */

   (*(void(_CODE_PTR_)(void))(uint_32)execute_address)();

} /* Endbody */


/* EOF */
