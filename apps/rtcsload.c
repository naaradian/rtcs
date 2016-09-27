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
*** File: rtcsload.c
***
*** Comments:  This file contains the interface functions to the
***            Exec function of the RTCS Communication Library.
***
************************************************************************
*END*******************************************************************/

#include <rtcs.h>
#define MAX_PACKETS_RECEIVE (1000)

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_load
   (
      FT_CALL_STRUCT_PTR   ft_ptr,
         /* [IN] file transfer protocol to use */
      pointer              ft_data,
         /* [IN] file transfer data */
      FF_CALL_STRUCT_PTR   ff_ptr,
         /* [IN] file format to decode */
      pointer              ff_data,
         /* [IN] file format data */
      uint_32              flags
         /* [IN] optional flags */
   )
{ /* Body */
   uint_32   tmpcnt;
   uint_32   error;  /* also used as packet size */
   uchar_ptr data;
 // return RTCS_OK;	//___________________________________________________t
   /*
   ** Initialize
   */
   error = (*ft_ptr->OPEN)(ft_data);
   if (error) {
      return error;
   } /* Endif */
//ok  return RTCS_OK;	//___________________________________________________t

   error = (*ff_ptr->START)(ff_data);
   if (error) {
      (*ft_ptr->CLOSE)();
      return error;
   } /* Endif */
 //ok  return RTCS_OK;	//___________________________________________________t

   /*
   ** Repeat until end of file
   */
  while (!(*ft_ptr->EOFT)()) {	//this "while" not stopped!!!!! Temporary comment  
   //	for(tmpcnt = 0; tmpcnt < MAX_PACKETS_RECEIVE; tmpcnt++) {

    //	if((*ft_ptr->EOFT)()) break;
      /*
      ** Retrieve some data...
      */
      data = (*ft_ptr->READ)(&error);
      if (!data) {
         (*ff_ptr->EOFD)();
         return error;
      } /* Endif */

      /*
      ** ...and decode it
      **
      ** Note:  If data is non-NULL, then error contains the
      **        number of bytes read.
      */
    //t  error = ((error >> 2) << 2) + 4; //make value divided by 4	//my!!!!!!!!!!!!!!!!!!!!!!!!!!!
      error = (*ff_ptr->DECODE)(error, data);

     if (error) {
         (*ft_ptr->CLOSE)();
         return error;
      } /* Endif */

  } /* Endwhile */

   /*
   ** Check if the decoding succeeded
   */
   error = (*ff_ptr->EOFD)();
   if (error) {
      (*ft_ptr->CLOSE)();
      return error;
   } /* Endif */

   /*
   ** End the file transfer session
   */
   error = (*ft_ptr->CLOSE)();
   if (error) {
      return error;
   } /* Endif */

   /*
   ** If we get here, the file was retrieved and decoded successfully.
   ** Run the image.
   */
   if (flags & RTCSLOADOPT_EXEC) {
    (*ff_ptr->EXEC)(); //______________________________not olweis need!!!!!
   } /* Endif */

   return RTCS_OK;

} /* Endbody */


/* EOF */
