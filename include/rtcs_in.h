#ifndef __rtcs_in_h__
#define __rtcs_in_h__
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
*** File: rtcs_in.h
***
*** Comments:
***    Definitions for the Internet protocol layer.
***
************************************************************************
*END*******************************************************************/


/***************************************
**
** Constants
**
*/

/*
** IP protocol types
*/
#define IPPROTO_HOPOPTS    0        /* IPv6 hop-by-hop options            */
#define IPPROTO_ICMP       1        /* Internet Control Message Protocol  */
#define IPPROTO_IGMP       2        /* Internet Group Management Protocol */
#define IPPROTO_IP         4        /* IP-in-IP encapsulation             */
#define IPPROTO_IPIP       4        /* IP-in-IP encapsulation             */
#define IPPROTO_TCP        6        /* Transmission Control Protocol      */
#define IPPROTO_UDP        17       /* User Datagram Protocol             */
#define IPPROTO_IPV6       41       /* IPv6-in-IP encapsulation           */
#define IPPROTO_ROUTING    43       /* IPv6 routing header                */
#define IPPROTO_FRAGMENT   44       /* IPv6 fragmentation header          */
#define IPPROTO_ESP        50       /* Encapsulating Security Payload     */
#define IPPROTO_AH         51       /* Authentication Header              */
#define IPPROTO_ICMPV6     58       /* ICMPv6                             */
#define IPPROTO_NONE       59       /* IPv6 no next header                */
#define IPPROTO_DSTOPTS    60       /* IPv6 destination options           */
#define IPPROTO_OSPF       89       /* Open Shortest Path Protocol        */
#define IPPROTO_COMP       108      /* IP compression                     */

/*
** Standard port assignments
*/
#define IPPORT_ECHO        7
#define IPPORT_FTPDATA     20
#define IPPORT_FTP         21
#define IPPORT_TELNET      23
#define IPPORT_BOOTPS      67
#define IPPORT_BOOTPC      68
#define IPPORT_TFTP        69
#define IPPORT_NTP         123
#define IPPORT_RADIAN      155  //my
//101125#define IPPORT_RADIANT	156
#define IPPORT_RADIANT	10000
#define IPPORT_SNMP        161
/* CR 887 */
#define IPPORT_SNMPTRAP    162
/* End CR 887 */
#define IPPORT_ISAKMP      500
#define IPPORT_RIP         520
#define IPPORT_ESPUDP      4500

/*
** IANA ifTypes
*/
#define IPIFTYPE_OTHER           1
#define IPIFTYPE_ETHERNET        6
#define IPIFTYPE_PPP             23
#define IPIFTYPE_LOOPBACK        24
#define IPIFTYPE_RS232           33
#define IPIFTYPE_FASTETHERNET    62
#define IPIFTYPE_GIGABITETHERNET 117
#define IPIFTYPE_HDLC            118
#define IPIFTYPE_TUNNEL          131

/*
** IANA ARP hardware types
*/
#define ARPLINK_NONE       0
#define ARPLINK_ETHERNET   1
#define ARPLINK_SERIAL     20

/*
** ouer RDN hardware types
*/
#define RDNLINK_NONE       0
#define RDNLINK_ETHERNET   1
#define RDNLINK_SERIAL     20


/*
** Special IP addresses
*/
#define INADDR_ANY               0
#define INADDR_LOOPBACK          0x7F000001L
#define INADDR_ALLHOSTS_GROUP    0xE0000001L
#define INADDR_ALLROUTERS_GROUP  0xE0000002L
#define INADDR_RIP_GROUP         0xE0000009L
#define INADDR_NTP_GROUP         0xE0000101L
#define INADDR_BROADCAST         0xFFFFFFFFL
#define INADDR_MY         		0x9b9b9b72L  //155.155.155.114


/***************************************
**
** Code macros
**
*/

/*
** Macros to classify IP addresses:
*/
#define IN_ZERONET(a)         (((a) & 0xFF000000L) == 0x00000000L)
#define IN_LOOPBACK(a)        (((a) & 0xFF000000L) == 0x7F000000L)
#define IN_MULTICAST(a)       (((a) & 0xF0000000L) == 0xE0000000L)
#define IN_LOCAL_MULTICAST(a) (((a) & 0xFFFFFF00L) == 0xE0000000L)
#define IN_EXPERIMENTAL(a)    (((a) & 0xF0000000L) == 0xF0000000L)

#define IN_CLASSA(a)          (((a) & 0x80000000L) == 0x00000000L)
#define IN_CLASSA_NET         0xFF000000L
#define IN_CLASSB(a)          (((a) & 0xC0000000L) == 0x80000000L)
#define IN_CLASSB_NET         0xFFFF0000L
#define IN_CLASSC(a)          (((a) & 0xE0000000L) == 0xC0000000L)
#define IN_CLASSC_NET         0xFFFFFF00L

#define IN_DEFAULT_NET(a)  ((a) == INADDR_ANY ? INADDR_ANY : \
                            (IN_CLASSA(a) ? IN_CLASSA_NET :  \
                            (IN_CLASSB(a) ? IN_CLASSB_NET :  \
                            (               IN_CLASSC_NET))))


#endif
/* EOF */
