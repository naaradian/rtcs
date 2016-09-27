#ifndef __telnet_h__
#define __telnet_h__
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
*** File: telnet.h
***
*** Comments:  This file contains the definitions for the Telnet
***            client and server.
***
************************************************************************
*END*******************************************************************/

#define TELCMD_IP       '\xf4'   /* Interrupt process */
#define TELCMD_AO       '\xf5'   /* Abort Output */
#define TELCMD_AYT      '\xf6'   /* Are You There */
#define TELCMD_EC       '\xf7'   /* Erase Char */
#define TELCMD_EL       '\xf8'   /* Erase Line */
#define TELCMD_GA       '\xf9'   /* Process ready for input */
#define TELCMD_WILL     '\xfb'
#define TELCMD_WONT     '\xfc'
#define TELCMD_DO       '\xfd'
#define TELCMD_DONT     '\xfe'
#define TELCMD_IAC      '\xff'   /* TELNET command sequence header */

#define TELOPT_BINARY   0        /* Use 8 bit binary transmission */
#define TELOPT_ECHO     1        /* Echo data received */
#define TELOPT_SGA      3        /* Suppress Go-ahead signal */

#define TELNET_MAX_OPTS 4


/* set the proper stream for the device */
#define IO_IOCTL_SET_STREAM         0x0050L


#endif
/* EOF */
