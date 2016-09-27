#ifndef __ftp_h__
#define __ftp_h__
/*HEADER****************************************************************
************************************************************************
***
*** Copyright (c) 1992-2004 ARC International
*** All rights reserved
***
*** This software embodies materials and concepts which are
*** confidential to ARC International and is made available solely
*** pursuant to the terms of a written license agreement with
*** ARC International.
***
*** File: ftp.h
***
*** Comments:  This file contains the FTP server definitions.
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Constants
**
*/

#define IMAGE_TYPE 0
#define ASCII_TYPE 1

/* Size configuration */
#define LINELEN      128   /* Length of command buffer */
#define BUF_SIZE     1460

/* Command table commands */
#define USER_CMD     0
#define ACCT_CMD     1
#define PASS_CMD     2
#define TYPE_CMD     3
#define NAME_CMD     4
#define QUIT_CMD     5
#define PORT_CMD     6
#define RETR_CMD     7
#define STOR_CMD     8
#define HELP_CMD     9
#define ABORT_CMD    10

#define CWD_CMD		0 
#define CDUP_CMD	1
#define LIST_CMD	2 
#define DELE_CMD	3
#define MKD_CMD		4
#define XMKD_CMD	5
#define PWD_CMD		6	
#define XPWD_CMD	7
#define RMD_CMD		8
#define XRMD_CMD	9

/***************************************
**
** Prototypes
**
*/

extern void    FTPSRV_process_connection( uint_32, uint_32 );
extern boolean FTPSRV_r_ftp( uint_32 );
extern boolean FTPSRV_docommand( uint_32, char _PTR_);

extern boolean FTPSRV_r_ftpd(uint_32);
extern boolean FTPSRV_s_ftpd(uint_32);
extern int_32  FTPSRV_pport(sockaddr_in _PTR_, char _PTR_);
extern void    FTPSRV_print_help(uint_32);
extern void    FTPSRV_tputs( uint_32, char _PTR_);
extern void FTPSRV_openDataConnection(uint_32, char _PTR_);

#endif
/* EOF */
