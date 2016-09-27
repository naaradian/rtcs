#ifndef __rtcs_h__
#define __rtcs_h__
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
*** File: rtcs.h
***
*** Comments:  This file contains the defines, externs and data
***            structure definitions required by application
***            programs in order to use the RTCS Communication Library.
***
************************************************************************
*END*******************************************************************/
 
#include <rtcsrtos.h>
#include <rtcsvers.h>
#include <rtcscfg.h>
#include <rtcslog.h>
#include <rtcs_in.h>
/*
//#define  PROG_ACT155
//#define PROG_TUTS
//#define PROG_DTVM
//#define PROG_KTVM
//#define PROG_BMD155
//#define PROG_BMD34
//#define PROG_BUKC
//#define PROG_PRM_PCH

#ifdef PROG_PRM_PCH
#define PROG_BMD155
#endif // PROG_PRM_PCH

#ifdef 	PROG_ACT155
#define PROG_BMD155	
extern unsigned char wd_reset_enabled;
extern "C" void wd_reset(void);
#endif
*/
/*
** Constants
*/

/*
** Protocols for the RTCS_protocol_table[]
*/
#define RTCSPROT_IGMP   IGMP_init
#define RTCSPROT_UDP    UDP_init
#define RTCSPROT_TCP    TCP_Init
#define RTCSPROT_RIP    RIP_init
#define RTCSPROT_OSPF   OSPF_init
#define RTCSPROT_IPIP   IPIP_init

/*
** Protocol and Address families
*/
#define PF_INET         1
#define AF_UNSPEC       0
#define AF_INET         1

/*
** Socket types
*/
#define SOCK_DGRAM      ((uint_32)&SOCK_DGRAM_CALL)
#define SOCK_STREAM     ((uint_32)&SOCK_STREAM_CALL)

/*
** Validity check for socket structures
*/
#define SOCKET_VALID       0x52544353  /* "rtcs" */

/*
** Default number of tasks that can simultaneously own a socket
*/
#define SOCKET_NUMOWNERS   8

/*
** protocol level at which option resides
*/
#define SOL_SOCKET      ((uint_32)&SOL_SOCKET_CALL)
#define SOL_IP          ((uint_32)&SOL_IP_CALL)
#define SOL_UDP         ((uint_32)&SOL_UDP_CALL)
#define SOL_TCP         ((uint_32)&SOL_TCP_CALL)
#define SOL_IGMP        ((uint_32)&SOL_IGMP_CALL)
#define SOL_LINK        ((uint_32)&SOL_LINK_CALL)


/*
**   socket option ids
*/
#define OPT_CONNECT_TIMEOUT            (0x01)
#define OPT_RETRANSMISSION_TIMEOUT     (0x02)
#define OPT_SEND_TIMEOUT               (0x03)
#define OPT_RECEIVE_TIMEOUT            (0x04)
#define OPT_RECEIVE_PUSH               (0x05)
#define OPT_SEND_NOWAIT                (0x06)
#define OPT_SEND_WAIT_OBSOLETE         (0x07) /* obsolete */
#define OPT_SEND_PUSH                  (0x08)
#define OPT_RECEIVE_NOWAIT             (0x09)
#define OPT_CHECKSUM_BYPASS            (0x0A)
#define OPT_ACTIVE                     (0x0B)
#define OPT_TBSIZE                     (0x0C)
#define OPT_RBSIZE                     (0x0D)
#define OPT_MAXRTO                     (0x0E)
#define OPT_MAXRCV_WND                 (0x0F)
#define OPT_KEEPALIVE                  (0x10)
#define OPT_NOWAIT                     (0x11)
#define OPT_NO_NAGLE_ALGORITHM         (0x12)
#define OPT_NOSWRBUF                   (0x13)
#define OPT_CALL_BACK_OBSOLETE         (0x14) /* obsolete */
#define OPT_SOCKET_TYPE                (0x15)
#define OPT_CALLBACK                   (0x16)
#define OPT_TIMEWAIT_TIMEOUT           (0x17)
#define OPT_SOCKET_ERROR               (0x18)

/*
**   default option values
*/
#define DEFAULT_CONNECT_TIMEOUT        (480000L)   /* 8 mins */
#define DEFAULT_RETRANSMISSION_TIMEOUT (3000)      /* 3 sec  */
#define DEFAULT_SEND_TIMEOUT           (DEFAULT_CONNECT_TIMEOUT/2)
#define DEFAULT_RECEIVE_TIMEOUT        (0)         /* no timeout */
#define DEFAULT_NOWAIT                 FALSE       /* wait */
#define DEFAULT_WAIT                   FALSE       /* don't wait for ack */
#define DEFAULT_PUSH                   TRUE        /* push */
#define DEFAULT_CHECKSUM_BYPASS        FALSE       /* perform checksum */
#define DEFAULT_ACTIVE                 TRUE
#define DEFAULT_TBSIZE                 (-1)
#define DEFAULT_RBSIZE                 (-1)
#define DEFAULT_MAXRTO                 (0)
#define DEFAULT_MAXRCV_WND             (0)
#define DEFAULT_KEEPALIVE              (0)
#define DEFAULT_NO_NAGLE_ALGORITHM     FALSE
#define DEFAULT_NOSWRBUF               FALSE
#define DEFAULT_CALL_BACK              NULL
#define DEFAULT_CALLBACK               NULL
#define DEFAULT_TIMEWAIT_TIMEOUT       (0)


/*
** Socket options
*/
#define RTCS_SO_TYPE                   OPT_SOCKET_TYPE
#define RTCS_SO_ERROR                  OPT_SOCKET_ERROR

#define RTCS_SO_IGMP_ADD_MEMBERSHIP    0
#define RTCS_SO_IGMP_DROP_MEMBERSHIP   1
#define RTCS_SO_IGMP_GET_MEMBERSHIP    2

#define RTCS_SO_IP_RX_DEST             0
#define RTCS_SO_IP_RX_TTL              1
#define RTCS_SO_IP_RX_TOS              2

#define RTCS_SO_LINK_TX_8023           0
#define RTCS_SO_LINK_TX_8021Q_PRIO     1
#define RTCS_SO_LINK_RX_8023           2
#define RTCS_SO_LINK_RX_8021Q_PRIO     3

#define RTCS_SO_UDP_NONBLOCK           OPT_SEND_NOWAIT
/* Start CR 1145 */
#define RTCS_SO_UDP_NONBLOCK_TX        OPT_SEND_NOWAIT
#define RTCS_SO_UDP_NONBLOCK_RX        OPT_RECEIVE_NOWAIT
/* End CR */
#define RTCS_SO_UDP_NOCHKSUM           OPT_CHECKSUM_BYPASS


/*
** Flags for send[to]/recv[from]
*/
#define RTCS_MSG_O_NONBLOCK   0x0001
#define RTCS_MSG_S_NONBLOCK   0x0002
#define RTCS_MSG_BLOCK        (RTCS_MSG_O_NONBLOCK)
#define RTCS_MSG_NONBLOCK     (RTCS_MSG_O_NONBLOCK | RTCS_MSG_S_NONBLOCK)
#define RTCS_MSG_O_NOCHKSUM   0x0004
#define RTCS_MSG_S_NOCHKSUM   0x0008
#define RTCS_MSG_CHKSUM       (RTCS_MSG_O_NOCHKSUM)
#define RTCS_MSG_NOCHKSUM     (RTCS_MSG_O_NOCHKSUM | RTCS_MSG_S_NOCHKSUM)
#define RTCS_MSG_PEEK         0x0010
#define RTCS_MSG_NOLOOP       0x0020

/*
** Flags for logging
*/
#define RTCS_LOGCTRL_FUNCTION    0x00000001
#define RTCS_LOGCTRL_PCB         0x00000002
#define RTCS_LOGCTRL_TIMER       0x00000004
#define RTCS_LOGCTRL_ALL         0x00000007

#define RTCS_LOGCTRL_IFTYPE(p)   ((p) & 0x3FF)
#define RTCS_LOGCTRL_ARP(p)      ((p) & 0x3FF | 0x4000)
#define RTCS_LOGCTRL_RDN(p)      ((p) & 0x3FF | 0xD000)	//my
#define RTCS_LOGCTRL_PROTO(p)    ((p) & 0x3FF | 0x8000)
#define RTCS_LOGCTRL_PORT(p)     ((p) & 0x3FF | 0xC000)

/*
** Close methods for shutdown()
*/
#define FLAG_ABORT_CONNECTION       (0x0010)
#define FLAG_CLOSE_TX               (0x0001)


/*
** Options for RTCS_load()
*/
#define RTCSLOADOPT_EXEC      0x01


/*
** Modes for the FTP client
*/
#define FTPMODE_DEFAULT 1
#define FTPMODE_PORT    2
#define FTPMODE_PASV    3
#define FTPDIR_RECV     0
#define FTPDIR_SEND     4


/*
** RTCS Error Code Definitions
*/

/*
** Initialization failures
*/
#define RTCSERR_INITIALIZED      0x300    /* RTCS already initialized */

/*
** PCB module errors
*/
#define RTCSERR_PCB_ALLOC        0x310    /* couldn't allocate PCBs         */
#define RTCSERR_PCB_FORK         0x311    /* PCB already forked             */
#define RTCSERR_PCB_LAYER        0x312    /* too few PCB layers             */
#define RTCSERR_PCB_BUFFER       0x313    /* PCB buffer too small           */
#define RTCSERR_PCB_FRAG         0x314    /* too few PCB fragments          */
#define RTCSERR_PCB_NOFRAG       0x315    /* can't add fragment (must fork) */
#define RTCSERR_PCB_DEPEND       0x316    /* can't create dependency        */

/*
** ARP errors
*/
#define RTCSERR_ARP_CFG          0x400    /* couldn't allocate state       */
#define RTCSERR_ARP_CACHE        0x401    /* couldn't allocate cache entry */
#define RTCSERR_ARP_CANT_RESOLVE 0x402    /* couldn't resolve address      */
#define RTCSERR_ARP_BAD_HEADER   0x403    /* error in header               */

/*
** RDN errors
*/
#define RTCSERR_RDN_CFG          0x2400    /* couldn't allocate state       */
#define RTCSERR_RDN_CACHE        0x2401    /* couldn't allocate cache entry */
#define RTCSERR_RDN_CANT_RESOLVE 0x2402    /* couldn't resolve address      */
#define RTCSERR_RDN_BAD_HEADER   0x2403    /* error in header               */



/*
** IPCP errors
*/
#define RTCSERR_IPCP_CFG         0x410    /* couldn't allocate state */

/*
** IP errors
*/
#define RTCSERR_IP_ICB_ALLOC     0x500    /* couldn't allocate ICB       */
#define RTCSERR_IP_PROT_OPEN     0x501    /* protocol already open       */
#define RTCSERR_IP_IF_ALLOC      0x502    /* couldn't allocate interface */
#define RTCSERR_IP_ROUTE_ALLOC   0x503    /* couldn't allocate route     */
#define RTCSERR_IP_GATE_ALLOC    0x504    /* couldn't allocate gateway   */
#define RTCSERR_IP_BIND_ADDR     0x505    /* invalid local address       */
#define RTCSERR_IP_BIND_MASK     0x506    /* invalid network mask        */
#define RTCSERR_IP_SEC_ALLOC     0x507    /* couldn't allocate SPD node   */
#define RTCSERR_IP_VIRTUAL_ALLOC 0x508    /* couldn't alloc virtual route */

/*
** IP datagram processing failures
*/
#define RTCSERR_IP_UNREACH       0x510    /* no route to host            */
#define RTCSERR_IP_TTL           0x511    /* TTL expired                 */
#define RTCSERR_IP_SMALLMTU      0x512    /* packet exceeds MTU          */
#define RTCSERR_IP_CANTFRAG      0x513    /* need to fragment but DF set */
#define RTCSERR_IP_BAD_HEADER    0x514    /* error in header             */
#define RTCSERR_IP_BAD_ADDRESS   0x515    /* illegal source or dest      */
#define RTCSERR_IP_BAD_CHECKSUM  0x516    /* invalid checksum            */

/*
** IPIP errors
*/
#define RTCSERR_IPIP_NOT_INITIALIZED   0x550   /* IPIP not initialized      */
#define RTCSERR_IPIP_LOOP              0x551   /* received a packet we sent */

/*
** ICMP errors
*/
#define RTCSERR_ICMP_ECHO_TIMEOUT   0x580 /* timed out waiting for echo reply */
#define RTCSERR_ICMP_BAD_HEADER     0x581 /* error in header                  */
#define RTCSERR_ICMP_BAD_CHECKSUM   0x582 /* invalid checksum                 */

/*
** IGMP errors
*/
#define RTCSERR_IGMP_CFG            0x5C0 /* couldn't allocate state        */
#define RTCSERR_IGMP_GROUP_ALLOC    0x5C1 /* couldn't allocate MCB          */
#define RTCSERR_IGMP_GROUP_FREE     0x5C2 /* couldn't free MCB              */
#define RTCSERR_IGMP_INVALID_IP     0x5C3 /* can't join group (nonlocal IP) */
#define RTCSERR_IGMP_NOT_JOINED     0x5C4 /* can't leave group (not joined) */
#define RTCSERR_IGMP_BAD_HEADER     0x5C5 /* error in header                */
#define RTCSERR_IGMP_BAD_CHECKSUM   0x5C6 /* invalid checksum               */

/*
** UDP errors
*/
#define RTCSERR_UDP_UCB_ALLOC    0x660    /* couldn't allocate UCB */
#define RTCSERR_UDP_UCB_FREE     0x661    /* couldn't free UCB     */
#define RTCSERR_UDP_UCB_CLOSE    0x662    /* UCB not open          */
#define RTCSERR_UDP_PORT_OPEN    0x663    /* port already open     */
#define RTCSERR_UDP_PORT_ALLOC   0x664    /* no more ports         */
#define RTCSERR_UDP_PORT_ZERO    0x665    /* can't send to port 0  */
#define RTCSERR_UDP_BAD_HEADER   0x666    /* error in header       */
#define RTCSERR_UDP_BAD_CHECKSUM 0x667    /* invalid checksum      */

/*
** Socket error codes
*/
#define RTCSERR_SOCK_INVALID        0x704
#define RTCSERR_SOCK_INVALID_AF     0x702
#define RTCSERR_SOCK_SHORT_ADDRESS  0x706
#define RTCSERR_SOCK_NOT_SUPPORTED  0x719
#define RTCSERR_SOCK_INVALID_OPTION 0x71A
#define RTCSERR_SOCK_SHORT_OPTION   0x71B
#define RTCSERR_SOCK_NOT_BOUND      0x709
#define RTCSERR_SOCK_IS_BOUND       0x705
#define RTCSERR_SOCK_NOT_LISTENING  0x70C
#define RTCSERR_SOCK_IS_LISTENING   0x70B
#define RTCSERR_SOCK_NOT_CONNECTED  0x70D
#define RTCSERR_SOCK_IS_CONNECTED   0x70A
#define RTCSERR_SOCK_NOT_OWNER      0x70E

/*
** RTCS_load() TFTP errors
*/
#define RTCSERR_TFTP_SOCKET         0x800
#define RTCSERR_TFTP_BIND           0x801
#define RTCSERR_TFTP_RRQ_ALLOC      0x802
#define RTCSERR_TFTP_RRQ_SEND       0x803
#define RTCSERR_TFTP_TIMEOUT        0x804
#define RTCSERR_TFTP_ERROR          0x810

/*
** RTCS_load() decoder errors
*/

#define RTCSERR_SREC_CHAR           0x880
#define RTCSERR_SREC_RECORD         0x881
#define RTCSERR_SREC_SHORT          0x882
#define RTCSERR_SREC_CHECKSUM       0x883
#define RTCSERR_SREC_START          0x884
#define RTCSERR_SREC_END            0x885
#define RTCSERR_SREC_EOF            0x886
#define RTCSERR_SREC_ERROR          0x887

#define RTCSERR_COFF_BAD_MAGIC      0x890
#define RTCSERR_COFF_ALLOC_FAILED   0x891
#define RTCSERR_COFF_TRUNCATED      0x892
#define RTCSERR_COFF_ERROR          0x893

/*
** Protocol layer definitions.  These are used by RTCS_[non]fatal_error().
*/
#define ERROR_RTCS                  (0x1)
#define ERROR_SOCKET                (0x2)
#define ERROR_TCPIP                 (0x3)
#define ERROR_TCP                   (0x4)
#define ERROR_UDP                   (0x5)
#define ERROR_RPC                   (0x6)
#define ERROR_ECHO                  (0x7)
#define ERROR_EDS                   (0x8)
#define ERROR_TELNET                (0x9)
#define ERROR_DHCPSRV               (0xA)
#define ERROR_DNS                   (0xB)
#define ERROR_IGMP                  (0xC)

/*
** Successful return code.  No errors.
*/
#define RTCS_OK                     0
#define RTCS_ERROR                  (-1)
#define RTCS_HANDLE_ERROR           (0xffffffffL)
#define RTCS_SOCKET_ERROR           (0xffffffffL)

#define MEM_TYPE_COMPONENT_SHIFT          12


#define MEM_TYPE_RTCS_BASE                   ( (IO_RTCS_COMPONENT) << (MEM_TYPE_COMPONENT_SHIFT))
#define MEM_TYPE_RTCS_DATA                      (MEM_TYPE_RTCS_BASE+1)
#define MEM_TYPE_ARP_CFG                        (MEM_TYPE_RTCS_BASE+2)
#define MEM_TYPE_ARP_ENTRY                      (MEM_TYPE_RTCS_BASE+3)
#define MEM_TYPE_DHCP_CLNT_STRUCT               (MEM_TYPE_RTCS_BASE+4)
#define MEM_TYPE_DHCPSRV_OPTIONS_STRUCT         (MEM_TYPE_RTCS_BASE+5) 
#define MEM_TYPE_FTPd_SESSION_PTR               (MEM_TYPE_RTCS_BASE+6) 
#define MEM_TYPE_FTPd_DATA_BUFFER               (MEM_TYPE_RTCS_BASE+7) 
#define MEM_TYPE_ICMP_CFG_STRUCT                (MEM_TYPE_RTCS_BASE+8) 
#define MEM_TYPE_ICMP_DATA                      (MEM_TYPE_RTCS_BASE+9) 
#define MEM_TYPE_IP_CFG_STRUCT                  (MEM_TYPE_RTCS_BASE+10) 
#define MEM_TYPE_IP_DATA                        (MEM_TYPE_RTCS_BASE+11) 
#define MEM_TYPE_ICB                            (MEM_TYPE_RTCS_BASE+12) 
#define MEM_TYPE_IP_IF                          (MEM_TYPE_RTCS_BASE+13) 
#define MEM_TYPE_SOCKET_CONFIG_STRUCT           (MEM_TYPE_RTCS_BASE+14) 
#define MEM_TYPE_RTCS_PARTITION                 (MEM_TYPE_RTCS_BASE+15) 
#define MEM_TYPE_IO_SOCKET                      (MEM_TYPE_RTCS_BASE+16) 
#define MEM_TYPE_SOCKET_OWNER_STRUCT            (MEM_TYPE_RTCS_BASE+17)
#define MEM_TYPE_TCP_CFG_STRUCT                 (MEM_TYPE_RTCS_BASE+18)
#define MEM_TYPE_TCB                            (MEM_TYPE_RTCS_BASE+19)
#define MEM_TYPE_TCP_TX_WINDOW                  (MEM_TYPE_RTCS_BASE+20)
#define MEM_TYPE_TCP_RX_WINDOW                  (MEM_TYPE_RTCS_BASE+21)
#define MEM_TYPE_TCP_SEND_CLOCK                 (MEM_TYPE_RTCS_BASE+22)
#define MEM_TYPE_UDP_CFG_STRUCT                 (MEM_TYPE_RTCS_BASE+23)
#define MEM_TYPE_UCB_STRUCT                     (MEM_TYPE_RTCS_BASE+24)
#define MEM_TYPE_UDP_BUFFER                     (MEM_TYPE_RTCS_BASE+25)
#define MEM_TYPE_IO_TELNET                      (MEM_TYPE_RTCS_BASE+26)
#define MEM_TYPE_SbufNode                       (MEM_TYPE_RTCS_BASE+27)
#define MEM_TYPE_Rchunk                         (MEM_TYPE_RTCS_BASE+28)
#define MEM_TYPE_FTPc_CONTEXT_STRUCT            (MEM_TYPE_RTCS_BASE+29)
#define MEM_TYPE_DHCPSRV_ADDR_STRUCT            (MEM_TYPE_RTCS_BASE+30)
#define MEM_TYPE_DHCPSRV_CID                    (MEM_TYPE_RTCS_BASE+31)
#define MEM_TYPE_DNS_CONTROL_STRUCT             (MEM_TYPE_RTCS_BASE+32)
#define MEM_TYPE_DNS_UDP_MESSAGE                (MEM_TYPE_RTCS_BASE+33)
#define MEM_TYPE_FTPc_RX_BUFFER                 (MEM_TYPE_RTCS_BASE+34)
#define MEM_TYPE_FTPc_COMMAND_BUFFER            (MEM_TYPE_RTCS_BASE+35)
#define MEM_TYPE_HOSTENT_STRUCT                 (MEM_TYPE_RTCS_BASE+36)
#define MEM_TYPE_TFTP_PACKET                    (MEM_TYPE_RTCS_BASE+37)
#define MEM_TYPE_TFTP_TRANS_STRUCT              (MEM_TYPE_RTCS_BASE+38)

#define MEM_TYPE_HTTPD_STRUCT                   (MEM_TYPE_RTCS_BASE+40)
#define MEM_TYPE_HTTPD_PARAMS                   (MEM_TYPE_RTCS_BASE+41)
#define MEM_TYPE_HTTPD_SESSION_STRUCT           (MEM_TYPE_RTCS_BASE+42)
#define MEM_TYPE_HTTPD_URI                      (MEM_TYPE_RTCS_BASE+43)
#define MEM_TYPE_HTTPD_AUTH                     (MEM_TYPE_RTCS_BASE+44)
#define MEM_TYPE_HTTPD_LINE                     (MEM_TYPE_RTCS_BASE+45)

/*
** Generic Error codes
*/
#define RTCSERR_SEND_FAILED             (0x1102)
#define RTCSERR_CREATE_POOL_FAILED      (0x1105)
#define RTCSERR_OPEN_QUEUE_FAILED       (0x1106)
#define RTCSERR_CREATE_FAILED           (0x1107)
#define RTCSERR_RECEIVE_FAILED          (0x1108)
#define RTCSERR_DEALLOC_FAILED          (0x1182)
#define RTCSERR_OUT_OF_MEMORY           (0x102)
#define RTCSERR_TIMEOUT                 (0x103)
#define RTCSERR_INVALID_ADDRESS         (0x104)
#define RTCSERR_INVALID_PARAMETER       (0x105)
#define RTCSERR_READ_ABORTED            (0x11f)
#define RTCSERR_OUT_OF_BUFFERS          (0x120)
#define RTCSERR_CREATE_PARTITION_FAILED (0x121)
#define RTCSERR_OUT_OF_SOCKETS          (0x122)
#define RTCSERR_FOPEN_FAILED            (0x123)

/*
** TCPIP error codes
*/
#define RTCSERR_TCPIP_INVALID_ARGUMENT  (0x610)  /* Invalid argument            */
#define RTCSERR_TCPIP_DESTADDR_REQUIRED (0x611)  /* Destination address         */
                                                 /*    required                 */
#define RTCSERR_TCPIP_NO_BUFFS          (0x612)  /* No buffer space available   */
#define RTCSERR_TCPIP_DELAY_REQUESTED   (0x613)  /* TCP_Delay() called, but not */
                                                 /*    supported                */
#define RTCSERR_TCPIP_TIMER_CORRUPT     (0x614)  /* Corrupt timer pointers      */

/*
** TCP error codes
*/
#define RTCSERR_TCP_OPEN_FAILED         (0x630)  /* TcpOpen failed              */
#define RTCSERR_TCP_INVALID_OPTION      (0x631)  /* Option was invalid          */
#define RTCSERR_TCP_IN_PROGRESS         (0x632)  /* Operation already in        */
                                                 /*    progress                 */
#define RTCSERR_TCP_ADDR_IN_USE         (0x633)  /* Address already in use      */
#define RTCSERR_TCP_ADDR_NA             (0x634)  /* Can't assign requested      */
                                                 /*    address                  */
#define RTCSERR_TCP_CONN_ABORTED        (0x635)  /* Software caused connection  */
                                                 /*    abort                    */
#define RTCSERR_TCP_CONN_RESET          (0x636)  /* Connection reset by peer    */
#define RTCSERR_TCP_HOST_DOWN           (0x637)  /* Host is down                */
#define RTCSERR_TCP_CONN_CLOSING        (0x638)  /* Connection closing          */
#define RTCSERR_TCP_CONN_RLSD           (0x639)  /* Connection/TCB released     */
#define RTCSERR_TCP_MISSING_OPEN        (0x63A)  /* TCB exists in LISTEN state  */
                                                 /*    with no associated Listn */
                                                 /*    request from upper layer */
#define RTCSERR_TCP_CTR_ZERO_RSIZE      (0x63B)  /* TCP_Copy_to_ring got 0      */
                                                 /*    ring buf size            */
#define RTCSERR_TCP_SP_BAD_SEND_STATE   (0x63C)  /* Attempted to TCP_Send_packet*/
                                                 /*    in invalid state         */
#define RTCSERR_TCP_SP_OUT_OF_PCBS      (0x63D)  /* Could not get a PCB in      */
                                                 /*    TCP_Send_packet          */
#define RTCSERR_TCP_SRR_OUT_OF_PCBS     (0x63E)  /* Could not get a PCB in      */
                                                 /*    TCP_Send_reply_reset     */
#define RTCSERR_TCP_SR_OUT_OF_PCBS      (0x63F)  /* Could not get a PCB in      */
                                                 /*    TCP_Send_reset           */
#define RTCSERR_TCP_SHRINKER_HOST       (0x640)  /* TCP detected a 'shrinker'   */
                                                 /*    peer host                */
#define RTCSERR_TCP_PA_BUFF_CORRUPT     (0x642)  /* TCP send buffer corruption  */
                                                 /*    detected in TCP_Process_ack   */
#define RTCSERR_TCP_PS_FAILED_GET_SBUF  (0x643)  /* Could not get an SbufNode   */
                                                 /*    in TCP_Process_send      */
#define RTCSERR_TCP_PR_OUT_OF_MEM       (0x644)  /* Could not get a Rcvchunk    */
                                                 /*    buffer in Setup Receive  */
#define RTCSERR_TCP_PP_OUT_OF_MEM       (0x645)  /* Could not add a host IpList */
                                                 /*    node in TCP_Process_packet    */
#define RTCSERR_TCP_TIMER_FAILURE       (0x646)  /* Could not start a TCP timer */
#define RTCSERR_TCP_NOT_CONN            (0x647)  /* Socket is not connected     */
#define RTCSERR_TCP_SHUTDOWN            (0x648)  /* Can't send after socket     */
                                                 /*    shutdown                 */
#define RTCSERR_TCP_TIMED_OUT           (0x649)  /* Connection timed out        */
#define RTCSERR_TCP_CONN_REFUSED        (0x64A)  /* Connection refused          */
#define RTCSERR_TCP_NO_MORE_PORTS       (0x64B)  /* No more ports available     */
                                                 /*    for connections          */
#define RTCSERR_TCP_BAD_STATE_FOR_CLOSE (0x64C)  /* Attempted to call           */
                                                 /*    TCP_Process_effective_close or */
                                                 /*    TCP_Process_close in invalid  */
                                                 /*    state                    */
#define RTCSERR_TCP_BAD_STATE_FOR_REL   (0x64D)  /* Attempted to release a TCB  */
                                                 /*    in an invalid state      */
#define RTCSERR_TCP_REXMIT_PROBLEM      (0x64E)  /* A retransmission timer      */
                                                 /*    timed out while in IDLE  */
                                                 /*    mode (i.e. invalid)      */
#define RTCSERR_TCP_URGENT_DATA_PROB    (0x64F)  /* Urgent data pointer         */
                                                 /*    was corrupted in the tcb */
#define RTCSERR_TCP_DEALLOC_FAILED      (0x650)  /* A call to release memory failed   */
#define RTCSERR_TCP_HOST_UNREACH        (0x651)  /* No route to host            */
#define RTCSERR_TCP_BAD_HEADER          (0x652)  /* error in header             */
#define RTCSERR_TCP_BAD_CHECKSUM        (0x653)  /* invalid checksum            */

#define  fopen      _io_fopen1 //my

/*
** Configurable globals
*/
extern uint_32 _RTCSTASK_priority;     /* TCP/IP task priority  */
extern uint_32 _RTCSTASK_stacksize;    /* additional stack size */

extern uint_32 _RTCSQUEUE_base;        /* RTCS queue numbers to use  */
extern uint_32 _RTCSPCB_max;           /* maximum RTCSPCBs available */

extern boolean _IP_forward;            /* IP forwarding */

extern boolean _TCP_bypass_rx;         /* TCP checksum bypass (recv) */
extern boolean _TCP_bypass_tx;         /* TCP checksum bypass (send) */
extern uint_32 _TCP_rto_min;           /* minimum TCP resend timeout */
extern boolean _RTCS_initialized;      /* RTCS initialized */
extern uint_32 _UDP_max_queue_size;    /* maximum length of UDP queue */

/*
** Type definitions
*/

/*
** IP address definition
*/
typedef uint_32 _ip_address;

typedef struct in_addr {
   _ip_address s_addr;
} in_addr;

/*
** IP multicast group
*/
typedef struct ip_mreq {
   in_addr  imr_multiaddr;
   in_addr  imr_interface;
} ip_mreq;

/*
** Socket Address Structure
*/
typedef struct sockaddr_in {
   uint_16  sin_family;
   uint_16  sin_port;
   in_addr  sin_addr;
} sockaddr_in;

/*
** The protocol table
*/
extern uint_32 (_CODE_PTR_ RTCS_protocol_table[])(void);


/*
** Protocol statistics
*/

/*
** The error structure embedded in statistics structures
*/
typedef struct {
   uint_32     ERROR;
   uint_32     PARM;
   _task_id    TASK_ID;
   uint_32     TASKCODE;
   pointer     MEMPTR;
   boolean     STACK;
} RTCS_ERROR_STRUCT, _PTR_ RTCS_ERROR_STRUCT_PTR;

/*
** In the structures below, the following four fields are common:
**
** [RT]X_TOTAL      is the total number of packets sent/received
** [RT]X_MISSED     is the number of packets discarded due to lack of resources
** [RT]X_DISCARDED  is the number of packets discarded for all other reasons
** [RT]X_ERRORS     is the number of times an internal error occurred
**
** Internal errors are errors that should _never_ happen.  However,
** if one does occur, information is recorded in ERR_[RT]X.
*/

typedef struct {

   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_OCTETS;           /* total bytes received       */
   uint_32  ST_RX_UNICAST;          /* unicast packets received   */
   uint_32  ST_RX_MULTICAST;        /* multicast packets received */
   uint_32  ST_RX_BROADCAST;        /* broadcast packets received */

   uint_32  ST_TX_OCTETS;           /* total bytes sent           */
   uint_32  ST_TX_UNICAST;          /* unicast packets sent       */
   uint_32  ST_TX_MULTICAST;        /* multicast packets sent     */
   uint_32  ST_TX_BROADCAST;        /* broadcast packets sent     */

} IPIF_STATS, _PTR_ IPIF_STATS_PTR;

typedef struct {
   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_REQUESTS;         /* valid ARP requests received */
   uint_32  ST_RX_REPLIES;          /* valid ARP replies received  */

   uint_32  ST_TX_REQUESTS;         /* ARP requests sent           */
   uint_32  ST_TX_REPLIES;          /* ARP replies received        */

   uint_32  ST_ALLOCS_FAILED;       /* ARP_alloc returned NULL     */
   uint_32  ST_CACHE_HITS;          /* ARP cache hits              */
   uint_32  ST_CACHE_MISSES;        /* ARP cache misses            */
   uint_32  ST_PKT_DISCARDS;        /* data packets discarded due  */
                                    /*    to missing ARP entry     */

} ARP_STATS, _PTR_ ARP_STATS_PTR;

typedef struct {
   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_REQUESTS;         /* valid ARP requests received */
   uint_32  ST_RX_REPLIES;          /* valid ARP replies received  */

   uint_32  ST_TX_REQUESTS;         /* ARP requests sent           */
   uint_32  ST_TX_REPLIES;          /* ARP replies received        */

   uint_32  ST_ALLOCS_FAILED;       /* ARP_alloc returned NULL     */
   uint_32  ST_CACHE_HITS;          /* ARP cache hits              */
   uint_32  ST_CACHE_MISSES;        /* ARP cache misses            */
   uint_32  ST_PKT_DISCARDS;        /* data packets discarded due  */
                                    /*    to missing ARP entry     */

} RDN_STATS, _PTR_ RDN_STATS_PTR;



typedef struct {

   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_HDR_ERRORS;       /* Discarded -- error in IP header    */
   uint_32  ST_RX_ADDR_ERRORS;      /* Discarded -- illegal destination   */
   uint_32  ST_RX_NO_PROTO;         /* Discarded -- unrecognized protocol */
   uint_32  ST_RX_DELIVERED;        /* Datagrams delivered to upper layer */
   uint_32  ST_RX_FORWARDED;        /* Datagrams forwarded                */

   /* These are included in ST_RX_DISCARDED and ST_RX_HDR_ERRORS */
   uint_32  ST_RX_BAD_VERSION;      /* Datagrams with version != 4        */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum    */
   uint_32  ST_RX_BAD_SOURCE;       /* Datagrams with invalid src address */
   uint_32  ST_RX_SMALL_HDR;        /* Datagrams with header too small    */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header      */
   uint_32  ST_RX_SMALL_PKT;        /* Datagrams larger than frame        */
   uint_32  ST_RX_TTL_EXCEEDED;     /* Datagrams to route with TTL = 0    */

   uint_32  ST_RX_FRAG_RECVD;       /* Number of received IP fragments    */
   uint_32  ST_RX_FRAG_REASMD;      /* Number of reassembled datagrams    */
   uint_32  ST_RX_FRAG_DISCARDED;   /* Number of discarded fragments      */

   uint_32  ST_TX_FRAG_SENT;        /* Number of sent fragments           */
   uint_32  ST_TX_FRAG_FRAGD;       /* Number of fragmented datagrams     */
   uint_32  ST_TX_FRAG_DISCARDED;   /* Number of fragmentation failures   */

} IP_STATS, _PTR_ IP_STATS_PTR;

typedef struct {

   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   /* These are included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_CODE;         /* Datagrams with unrecognized code     */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum      */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header        */
   uint_32  ST_RX_RD_NOTGATE;       /* Redirects received from non-gateway  */

   /* Statistics on each ICMP type */
   uint_32  ST_RX_DESTUNREACH;      /* Received Destination Unreachables    */
   uint_32  ST_RX_TIMEEXCEED;       /* Received Time Exceededs              */
   uint_32  ST_RX_PARMPROB;         /* Received Parameter Problems          */
   uint_32  ST_RX_SRCQUENCH;        /* Received Source Quenches             */

   uint_32  ST_RX_REDIRECT;         /* Received Redirects                   */
   uint_32  ST_RX_ECHO_REQ;         /* Received Echo Requests               */
   uint_32  ST_RX_ECHO_REPLY;       /* Received Echo Replys                 */
   uint_32  ST_RX_TIME_REQ;         /* Received Timestamp Requests          */

   uint_32  ST_RX_TIME_REPLY;       /* Received Timestamp Replys            */
   uint_32  ST_RX_INFO_REQ;         /* Received Information Requests        */
   uint_32  ST_RX_INFO_REPLY;       /* Received Information Replys          */
   uint_32  ST_RX_OTHER;            /* All other types                      */

   uint_32  ST_TX_DESTUNREACH;      /* Transmitted Destination Unreachables */
   uint_32  ST_TX_TIMEEXCEED;       /* Transmitted Time Exceededs           */
   uint_32  ST_TX_PARMPROB;         /* Transmitted Parameter Problems       */
   uint_32  ST_TX_SRCQUENCH;        /* Transmitted Source Quenches          */

   uint_32  ST_TX_REDIRECT;         /* Transmitted Redirects                */
   uint_32  ST_TX_ECHO_REQ;         /* Transmitted Echo Requests            */
   uint_32  ST_TX_ECHO_REPLY;       /* Transmitted Echo Replys              */
   uint_32  ST_TX_TIME_REQ;         /* Transmitted Timestamp Requests       */

   uint_32  ST_TX_TIME_REPLY;       /* Transmitted Timestamp Replys         */
   uint_32  ST_TX_INFO_REQ;         /* Transmitted Information Requests     */
   uint_32  ST_TX_INFO_REPLY;       /* Transmitted Information Replys       */
   uint_32  ST_TX_OTHER;            /* All other types                      */

} ICMP_STATS, _PTR_ ICMP_STATS_PTR;

typedef struct {

   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_BAD_TYPE;         /* Datagrams with unrecognized code     */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum      */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header        */

   uint_32  ST_RX_QUERY;            /* Received Queries                     */
   uint_32  ST_RX_REPORT;           /* Received Reports                     */

   uint_32  ST_TX_QUERY;            /* Transmitted Queries                  */
   uint_32  ST_TX_REPORT;           /* Transmitted Reports                  */

} IGMP_STATS, _PTR_ IGMP_STATS_PTR;

typedef struct {

   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   /* These are all included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_PORT;         /* Datagrams with dest port zero   */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header   */
   uint_32  ST_RX_SMALL_PKT;        /* Datagrams larger than frame     */
   uint_32  ST_RX_NO_PORT;          /* Datagrams for a closed port     */

} UDP_STATS, _PTR_ UDP_STATS_PTR;

typedef struct {

   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   /* These are all included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_PORT;         /* Segments with dest port zero   */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Segments with invalid checksum */
   uint_32  ST_RX_BAD_OPTION;       /* Segments with invalid options  */
   uint_32  ST_RX_BAD_SOURCE;       /* Segments with invalid source   */
   uint_32  ST_RX_SMALL_HDR;        /* Segments with header too small */
   uint_32  ST_RX_SMALL_DGRAM;      /* Segments smaller than header   */
   uint_32  ST_RX_SMALL_PKT;        /* Segments larger than frame     */
   uint_32  ST_RX_BAD_ACK;          /* Received ack for unsent data   */
   uint_32  ST_RX_BAD_DATA;         /* Received data outside window   */
   uint_32  ST_RX_LATE_DATA;        /* Received data after close      */
   uint_32  ST_RX_OPT_MSS;          /* Segments with MSS option set   */
   uint_32  ST_RX_OPT_OTHER;        /* Segments with other options    */

   uint_32  ST_RX_DATA;             /* Data segments received         */
   uint_32  ST_RX_DATA_DUP;         /* Duplicate data received        */
   uint_32  ST_RX_ACK;              /* Acks received                  */
   uint_32  ST_RX_ACK_DUP;          /* Duplicate acks received        */
   uint_32  ST_RX_RESET;            /* RST segments received          */
   uint_32  ST_RX_PROBE;            /* Window probes received         */
   uint_32  ST_RX_WINDOW;           /* Window updates received        */

   uint_32  ST_RX_SYN_EXPECTED;     /* Expected SYN, not received     */
   uint_32  ST_RX_ACK_EXPECTED;     /* Expected ACK, not received     */
   uint_32  ST_RX_SYN_NOT_EXPECTED; /* Received SYN, not expected     */
   uint_32  ST_RX_MULTICASTS;       /* Multicast packets              */


   uint_32  ST_TX_DATA;             /* Data segments sent             */
   uint_32  ST_TX_DATA_DUP;         /* Data segments retransmitted    */
   uint_32  ST_TX_ACK;              /* Ack-only segments sent         */
   uint_32  ST_TX_ACK_DELAYED;      /* Delayed acks sent              */
   uint_32  ST_TX_RESET;            /* RST segments sent              */
   uint_32  ST_TX_PROBE;            /* Window probes sent             */
   uint_32  ST_TX_WINDOW;           /* Window updates sent            */

   uint_32  ST_CONN_ACTIVE;         /* Active opens                   */
   uint_32  ST_CONN_PASSIVE;        /* Passive opens                  */
   uint_32  ST_CONN_OPEN;           /* Established connections        */
   uint_32  ST_CONN_CLOSED;         /* Graceful shutdowns             */
   uint_32  ST_CONN_RESET;          /* Ungraceful shutdowns           */
   uint_32  ST_CONN_FAILED;         /* Failed opens                   */
   uint_32  ST_CONN_ABORTS;         /* Aborts                         */

} TCP_STATS, _PTR_ TCP_STATS_PTR;

typedef struct {

   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

} RIP_STATS, _PTR_ RIP_STATS_PTR;


/*
** Type definitions for RTCS_if_XXX()
*/

typedef pointer _rtcs_if_handle;
struct ip_if;
struct rtcspcb;

typedef struct {
   uint_32 (_CODE_PTR_  OPEN) (struct ip_if _PTR_);
   uint_32 (_CODE_PTR_  CLOSE)(struct ip_if _PTR_);
   uint_32 (_CODE_PTR_  SEND) (struct ip_if _PTR_, struct rtcspcb _PTR_, _ip_address, _ip_address, pointer);
   uint_32 (_CODE_PTR_  JOIN) (struct ip_if _PTR_, _ip_address);
   uint_32 (_CODE_PTR_  LEAVE)(struct ip_if _PTR_, _ip_address);
} RTCS_IF_STRUCT, _PTR_ RTCS_IF_STRUCT_PTR;

//extern const RTCS_IF_STRUCT_PTR  RTCS_IF_LOCALHOST;
extern RTCS_IF_STRUCT_PTR  RTCS_IF_LOCALHOST;
//extern const RTCS_IF_STRUCT_PTR  RTCS_IF_ENET;
extern RTCS_IF_STRUCT_PTR  RTCS_IF_ENET;
//extern const RTCS_IF_STRUCT_PTR  RTCS_IF_PPP;
extern RTCS_IF_STRUCT_PTR  RTCS_IF_PPP;

/*
** Required when booting with BOOTP
*/
typedef struct bootp_data_struct {
   _ip_address SADDR;
#if RTCSCFG_BOOTP_RETURN_YIADDR
   _ip_address CLIENTADDR;
#endif
   uchar       SNAME[64];
   uchar       BOOTFILE[128];
   uchar       OPTIONS[64];
} BOOTP_DATA_STRUCT, _PTR_ BOOTP_DATA_STRUCT_PTR;

/*
** Required when using PPP
*/
typedef struct ipcp_data_struct {
   void (_CODE_PTR_  IP_UP)   (pointer);
   void (_CODE_PTR_  IP_DOWN) (pointer);
   pointer           IP_PARAM;

   unsigned ACCEPT_LOCAL_ADDR  : 1;
   unsigned ACCEPT_REMOTE_ADDR : 1;
   unsigned DEFAULT_NETMASK    : 1; /* obsolete */
   unsigned DEFAULT_ROUTE      : 1;
   unsigned NEG_LOCAL_DNS      : 1;
   unsigned NEG_REMOTE_DNS     : 1;
   unsigned ACCEPT_LOCAL_DNS   : 1; /* ignored if NEG_LOCAL_DNS  == 0 */
   unsigned ACCEPT_REMOTE_DNS  : 1; /* ignored if NEG_REMOTE_DNS == 0 */
   unsigned                    : 0;

   _ip_address LOCAL_ADDR;
   _ip_address REMOTE_ADDR;
   _ip_address NETMASK;     /* obsolete */
   _ip_address LOCAL_DNS;   /* ignored if NEG_LOCAL_DNS   == 0 */
   _ip_address REMOTE_DNS;  /* ignored if NEG_REMOTE_DNS  == 0 */

} IPCP_DATA_STRUCT, _PTR_ IPCP_DATA_STRUCT_PTR;


/*
** Type definitions for socket()
*/

typedef struct {
   uint_32 (_CODE_PTR_ SOCK_SOCKET)      (uint_32);
   uint_32 (_CODE_PTR_ SOCK_BIND)        (uint_32, const sockaddr_in _PTR_, uint_16);
   uint_32 (_CODE_PTR_ SOCK_CONNECT)     (uint_32, const sockaddr_in _PTR_, uint_16);
   uint_32 (_CODE_PTR_ SOCK_LISTEN)      (uint_32, uint_32);
   uint_32 (_CODE_PTR_ SOCK_ACCEPT)      (uint_32, sockaddr_in _PTR_, uint_16 _PTR_);
   uint_32 (_CODE_PTR_ SOCK_GETSOCKNAME) (uint_32, sockaddr_in _PTR_, uint_16 _PTR_);
   uint_32 (_CODE_PTR_ SOCK_GETPEERNAME) (uint_32, sockaddr_in _PTR_, uint_16 _PTR_);
    int_32 (_CODE_PTR_ SOCK_RECV)        (uint_32, pointer, uint_32, uint_32);
    int_32 (_CODE_PTR_ SOCK_RECVFROM)    (uint_32, pointer, uint_32, uint_32, sockaddr_in _PTR_, uint_16 _PTR_);
    int_32 (_CODE_PTR_ SOCK_RECVMSG)     (uint_32, pointer, uint_32);
    int_32 (_CODE_PTR_ SOCK_SEND)        (uint_32, pointer, uint_32, uint_32);
    int_32 (_CODE_PTR_ SOCK_SENDTO)      (uint_32, pointer, uint_32, uint_32, sockaddr_in _PTR_, uint_16);
    int_32 (_CODE_PTR_ SOCK_SENDMSG)     (uint_32, pointer, uint_32);
   uint_32 (_CODE_PTR_ SOCK_SOCKATMARK)  (uint_32);
   uint_32 (_CODE_PTR_ SOCK_SHUTDOWN)    (uint_32, uint_32);
} RTCS_SOCKET_CALL_STRUCT, _PTR_ RTCS_SOCKET_CALL_STRUCT_PTR;

extern const RTCS_SOCKET_CALL_STRUCT  SOCK_DGRAM_CALL;
extern const RTCS_SOCKET_CALL_STRUCT  SOCK_STREAM_CALL;

/*
** Type definitions for [gs]etsockopt()
*/

typedef struct {
   uint_32 (_CODE_PTR_ SOCK_GETSOCKOPT)  (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
   uint_32 (_CODE_PTR_ SOCK_SETSOCKOPT)  (uint_32, uint_32, uint_32, pointer, uint_32);
} RTCS_SOCKOPT_CALL_STRUCT, _PTR_ RTCS_SOCKOPT_CALL_STRUCT_PTR;

extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_SOCKET_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_IP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_UDP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_TCP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_IGMP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_LINK_CALL;

/*
** The socket state structure
*/

typedef struct {
   unsigned    OPT_8023 : 1;
   unsigned    OPT_PRIO : 1;
   unsigned             : 0;
   _ip_address DEST;
   uchar       TTL;
   uchar       TOS;
   uchar       RESERVED[2];
#if RTCSCFG_LINKOPT_8021Q_PRIO
   uint_32     PRIO;
#endif
} RTCS_LINKOPT_STRUCT, _PTR_ RTCS_LINKOPT_STRUCT_PTR;

typedef struct socket_owner_struct {
   struct socket_owner_struct  _PTR_   NEXT;
   _rtcs_taskid                        TASK[SOCKET_NUMOWNERS];
} SOCKET_OWNER_STRUCT, _PTR_ SOCKET_OWNER_STRUCT_PTR;

struct MCB;
struct UCB;
struct TCB;

typedef struct socket_struct {

   struct   socket_struct _PTR_  NEXT;
   struct   socket_struct _PTR_  PREV;

   uint_32                       VALID;

   uint_16                       LOCAL_PORT;          /* not used */
   uint_16                       REMOTE_PORT;         /* not used */
   _ip_address                   LOCAL_IPADDRESS;     /* not used */
   _ip_address                   REMOTE_IPADDRESS;    /* not used */

   uint_16                       STATE;
   uint_16                       AF;
   RTCS_SOCKET_CALL_STRUCT_PTR   PROTOCOL;

   /*
   ** Socket options.
   **
   **  The order of these options must match the order of the option ids
   **  defined above, and CONNECT_TIMEOUT must be the first one.
   */
   uint_32                       CONNECT_TIMEOUT;
   uint_32                       RETRANSMISSION_TIMEOUT;
   uint_32                       SEND_TIMEOUT;
   uint_32                       RECEIVE_TIMEOUT;
   uint_32                       RECEIVE_PUSH;
   uint_32                       SEND_NOWAIT;
   uint_32                       SEND_WAIT;
   uint_32                       SEND_PUSH;
   uint_32                       RECEIVE_NOWAIT;
   boolean                       CHECKSUM_BYPASS;
   boolean                       ACTIVE;
   int_32                        TBSIZE;
   int_32                        RBSIZE;
   int_32                        MAXRTO;
   uint_32                       MAXRCV_WND;
   int_32                        KEEPALIVE;
   boolean                       NOWAIT;
   boolean                       NO_NAGLE_ALGORITHM;
   boolean                       NOSWRBUF;
   void _CODE_PTR_               CALL_BACK; /* obsolete */
   uint_32                       TYPE_MIRROR;
   void (_CODE_PTR_              CALLBACK)(int_32);
   uint_32                       TIMEWAIT_TIMEOUT;  /* last option */

   pointer                       OUTSTANDING_ACCEPT;    /* not used */
   pointer                       OUTSTANDING_CONNECT;   /* not used */
   struct TCB _PTR_              TCB_PTR;
   struct UCB _PTR_              UCB_PTR;

   uint_32                       ERROR_CODE;

   SOCKET_OWNER_STRUCT           OWNERS;

   struct MCB _PTR_ _PTR_        MCB_PTR;
   uint_32 (_CODE_PTR_ _PTR_     IGMP_LEAVEALL)(struct MCB _PTR_ _PTR_);

   struct {
      RTCS_LINKOPT_STRUCT        RX;
      RTCS_LINKOPT_STRUCT        TX;
   }                             LINK_OPTIONS;

} SOCKET_STRUCT, _PTR_ SOCKET_STRUCT_PTR;


/*
** Type definitions for the Telnet server
*/

typedef struct {
   char_ptr          NAME;
   uint_32           PRIORITY;
   uint_32           STACKSIZE;
   void (_CODE_PTR_  START)(pointer);
   pointer           ARG;
} RTCS_TASK, _PTR_ RTCS_TASK_PTR;


/*
** Type definitions for RTCS_load()
*/

/*
** File Transfer information structure
*/
typedef struct {
   uint_32     (_CODE_PTR_ OPEN)    (pointer ft_data);
   uchar_ptr   (_CODE_PTR_ READ)    (uint_32_ptr size);
   boolean     (_CODE_PTR_ EOFT)    (void);
   uint_32     (_CODE_PTR_ CLOSE)   (void);
} FT_CALL_STRUCT, _PTR_ FT_CALL_STRUCT_PTR;

//extern const FT_CALL_STRUCT_PTR FT_TFTP;
extern FT_CALL_STRUCT_PTR FT_TFTP;
extern const FT_CALL_STRUCT_PTR FT_FTP;      /* not implemented */

/*
** TFTP data structure
*/
typedef struct tftp_data_struct {
   _ip_address SERVER;
   char_ptr    FILENAME;
   char_ptr    FILEMODE;
} TFTP_DATA_STRUCT, _PTR_ TFTP_DATA_STRUCT_PTR;

/*
** File Format information structure
*/
typedef struct
{
   uint_32     (_CODE_PTR_ START)   (pointer ff_data);
   uint_32     (_CODE_PTR_ DECODE)  (uint_32 size, uchar_ptr data);
   uint_32     (_CODE_PTR_ EOFD)    (void);
   void        (_CODE_PTR_ EXEC)    (void);
} FF_CALL_STRUCT, _PTR_ FF_CALL_STRUCT_PTR;

//my extern const FF_CALL_STRUCT_PTR FF_BIN;
extern FF_CALL_STRUCT ff_bin; //my 
extern FF_CALL_STRUCT_PTR FF_BIN; //my

extern const FF_CALL_STRUCT_PTR FF_BIN32;    /* not implemented */
extern const FF_CALL_STRUCT_PTR FF_SREC;
extern const FF_CALL_STRUCT_PTR FF_SREC32;
extern const FF_CALL_STRUCT_PTR FF_COFF;

/*
** Binary file data structure
*/
typedef struct binfile_data_struct {
   uchar_ptr   DOWNLOAD_ADDR;
   uchar_ptr   EXEC_ADDR;
} BINFILE_DATA_STRUCT, _PTR_ BINFILE_DATA_STRUCT_PTR;


/*
** Initialization for the DHCP server
*/
typedef struct dhcpsrv_data_struct {

   _ip_address SERVERID;
   uint_32     LEASE;
   _ip_address MASK;

   _ip_address SADDR;
   uchar       SNAME[64];
   uchar       FILE[128];

} DHCPSRV_DATA_STRUCT, _PTR_ DHCPSRV_DATA_STRUCT_PTR;


/*
** Type definitions for gethostbyname()
*/

/*
** Host name information structure
*/
#define MAX_HOST_ALIASES     16
#define MAX_HOST_ADDRESSES   16

typedef struct hostent
{
   char_ptr          h_name;
   char_ptr _PTR_    h_aliases;
   int_16            h_addrtype;
   int_16            h_length;
   char_ptr _PTR_    h_addr_list;

} HOSTENT_STRUCT, _PTR_ HOSTENT_STRUCT_PTR;

/* For 4.2BSD compatability */
#define h_addr h_addr_list[0]

/*
** Host entry structure - a ram version of the hosts file
*/
typedef struct host_entry_struct
{
   uint_32        ip_address;
   char_ptr       host_name;
   char_ptr       aliases[MAX_HOST_ALIASES+1];
} HOST_ENTRY_STRUCT;

/*
* extern struct to input host list into
*/
extern const HOST_ENTRY_STRUCT RTCS_Hosts_list[];

typedef struct internal_hostent_struct
{
   HOSTENT_STRUCT   HOSTENT;
   char_ptr         ALIASES[MAX_HOST_ALIASES+1];
   uint_32_ptr      ADDRESSES[MAX_HOST_ADDRESSES+1];
   uint_32          IP_address;
} INTERNAL_HOSTENT_STRUCT, _PTR_ INTERNAL_HOSTENT_STRUCT_PTR;


#ifdef __cplusplus
extern "C" {
#endif


/*
** Prototypes for the RTCS_protocol_table[]
*/

extern uint_32 IGMP_init  (void);
extern uint_32 UDP_init   (void);
extern uint_32 TCP_Init   (void);
extern uint_32 RIP_init   (void);
extern uint_32 OSPF_init  (void);
extern uint_32 IPIP_init  (void);


/*
** BSD prototypes
*/

extern uint_32  RTCS_socket
(
   uint_32              ,  /* [IN] protocol family */
   uint_32              ,  /* [IN] type of communication */
   uint_32                 /* [IN] select a specific protocol */
);
extern uint_32  RTCS_shutdown
(
   uint_32              ,  /* [IN] socket handle */
   uint_32                 /* [IN] shutdown method */
);

extern int_32        inet_aton (const char _PTR_, in_addr _PTR_);
extern _ip_address   inet_addr (const char _PTR_);

extern HOSTENT_STRUCT _PTR_ DNS_gethostbyname
(
   char _PTR_  name_ptr
);

extern HOSTENT_STRUCT _PTR_ DNS_gethostbyaddr
(
   char _PTR_ addr_ptr,
   uint_32    len,
   uint_32    type
);

extern HOSTENT_STRUCT_PTR RTCS_gethostbyname
(
   char _PTR_    hostname    /* [IN] name of host to search for */
);

extern void DNS_free_HOSTENT_STRUCT
(
   HOSTENT_STRUCT _PTR_ host_ptr
);


#if RTCSCFG_CHECK_ERRORS

   #define RTCS_CHECKSOCK(s,err) \
                       ((s) == 0) || ((uint_32)(s) == RTCS_SOCKET_ERROR) \
                     ? (err) \
                     :

   #define RTCS_CHECKAPI(f,s,err) \
                       ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f == NULL \
                     ? (err) \
                     :

   #define RTCS_CHECKLEVEL(v,err) \
                       (v) == 0 \
                     ? (err) \
                     :

#else

   #define RTCS_CHECKSOCK(s,err)
   #define RTCS_CHECKAPI(f,s,err)
   #define RTCS_CHECKLEVEL(v,err)

#endif

#if RTCSCFG_CHECK_VALIDITY

   #define RTCS_CHECKVALID(s,err) \
                       ((SOCKET_STRUCT_PTR)(s))->VALID != SOCKET_VALID \
                     ? (err) \
                     :

#else

   #define RTCS_CHECKVALID(s,err)

#endif

#define RTCS_API(f,s,p)             RTCS_CHECKSOCK(s,RTCSERR_SOCK_INVALID) \
                                    RTCS_CHECKVALID(s,RTCSERR_SOCK_INVALID) \
                                    RTCS_CHECKAPI(f,s,RTCSERR_SOCK_NOT_SUPPORTED) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SOCK(f,s,p)        RTCS_CHECKSOCK(s,RTCS_SOCKET_ERROR) \
                                    RTCS_CHECKVALID(s,RTCS_SOCKET_ERROR) \
                                    RTCS_CHECKAPI(f,s,RTCS_SOCKET_ERROR) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SENDRECV(f,s,p)    RTCS_CHECKSOCK(s,RTCS_ERROR) \
                                    RTCS_CHECKVALID(s,RTCS_ERROR) \
                                    RTCS_CHECKAPI(f,s,RTCS_ERROR) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SOCKOPT(f,s,v,p)   RTCS_CHECKLEVEL(v,RTCSERR_SOCK_INVALID_OPTION) \
                                    ((RTCS_SOCKOPT_CALL_STRUCT_PTR)(v))->SOCK_ ## f p

#define bind(s,ap,al)            (RTCS_API(BIND,        s, (s,ap,al)))
#define connect(s,ap,al)         (RTCS_API(CONNECT,     s, (s,ap,al)))
#define listen(s,f)              (RTCS_API(LISTEN,      s, (s,f)))
#define getsockname(s,ap,al)     (RTCS_API(GETSOCKNAME, s, (s,ap,al)))
#define getpeername(s,ap,al)     (RTCS_API(GETPEERNAME, s, (s,ap,al)))

#define accept(s,ap,al)          (RTCS_API_SOCK(ACCEPT, s, (s,ap,al)))

#define send(s,p,l,f)            (RTCS_API_SENDRECV(SEND,     s, (s,p,l,f)))
#define sendto(s,p,l,f,ap,al)    (RTCS_API_SENDRECV(SENDTO,   s, (s,p,l,f,ap,al)))
#define recv(s,p,l,f)            (RTCS_API_SENDRECV(RECV,     s, (s,p,l,f)))
#define recvfrom(s,p,l,f,ap,al)  (RTCS_API_SENDRECV(RECVFROM, s, (s,p,l,f,ap,al)))

#define getsockopt(s,v,on,op,ol) (RTCS_API_SOCKOPT(GETSOCKOPT, s, v, (s,v,on,op,ol)))
#define setsockopt(s,v,on,op,ol) (RTCS_API_SOCKOPT(SETSOCKOPT, s, v, (s,v,on,op,ol)))

#define socket          RTCS_socket
#define shutdown        RTCS_shutdown
#define gethostbyname   DNS_gethostbyname
#define gethostbyaddr   DNS_gethostbyaddr


/*
** RTCS (non-BSD) prototypes
*/

extern uint_32  RTCS_attachsock
(
   uint_32                       /* [IN] socket handle */
);
extern uint_32  RTCS_detachsock
(
   uint_32                       /* [IN] socket handle */
);

extern uint_32  RTCS_selectall
(
   uint_32                       /* [IN] time to wait for data, in milliseconds */
);

extern uint_32  RTCS_selectset
(
   pointer                    ,  /* [IN] set of sockets to block on */
   uint_32                    ,  /* [IN] size of socket set */
   uint_32                       /* [IN] time to wait for data, in milliseconds */
);

extern uint_32  RTCS_geterror
(
   uint_32                       /* [IN] socket handle */
);

extern uint_32 RTCS_load
(
   FT_CALL_STRUCT_PTR         ,  /* [IN] file transfer protocol to use */
   pointer                    ,  /* [IN] file transfer data */
   FF_CALL_STRUCT_PTR         ,  /* [IN] file format to decode */
   pointer                    ,  /* [IN] file format data */
   uint_32                       /* [IN] optional flags */
);

extern uint_32 RTCS_load_TFTP_BIN
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                   ,  /* [IN] file to retrieve */
   uchar_ptr                     /* [IN] download start address */
);

extern uint_32 RTCS_exec_TFTP_BIN
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                   ,  /* [IN] file to retrieve */
   uchar_ptr                  ,  /* [IN] download start address */
   uchar_ptr                     /* [IN] execution start address */
);

extern uint_32 RTCS_load_TFTP_SREC
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                      /* [IN] file to retrieve */
);

extern uint_32 RTCS_exec_TFTP_SREC
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                      /* [IN] file to retrieve */
);

extern uint_32 RTCS_load_TFTP_COFF
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                      /* [IN] file to retrieve */
);

extern uint_32 RTCS_exec_TFTP_COFF
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                      /* [IN] file to retrieve */
);

extern IP_STATS_PTR   IP_stats   (void);
extern ICMP_STATS_PTR ICMP_stats (void);
extern UDP_STATS_PTR  UDP_stats  (void);
extern TCP_STATS_PTR  TCP_stats  (void);
extern IPIF_STATS_PTR IPIF_stats (_rtcs_if_handle);
extern ARP_STATS_PTR  ARP_stats  (_rtcs_if_handle);

extern uint_32 ECHOSRV_init       (char_ptr, uint_32, uint_32);
extern uint_32 FTPSRV_init        (char_ptr, uint_32, uint_32);
extern uint_32 TELNETSRV_init     (char_ptr, uint_32, uint_32, RTCS_TASK_PTR);
extern uint_32 TELNETSRV_sockinit (char_ptr, uint_32, uint_32, RTCS_TASK_PTR, uint_32);
extern uint_32 DHCPSRV_init       (char_ptr, uint_32, uint_32);
extern uint_32 TFTPSRV_init       (char_ptr, uint_32, uint_32);
extern uint_32 SNTP_init          (char_ptr, uint_32, uint_32, _ip_address, uint_32);
extern uint_32 SNMP_init          (char_ptr, uint_32, uint_32);
extern uint_32 EDS_init           (char_ptr, uint_32, uint_32);

extern boolean TFTPSRV_access (char_ptr, uint_16);

extern void    RTCS_delay (uint_32);
extern void    RTCS_shell (void);
extern uint_32 RTCS_ping  (_ip_address, uint_32_ptr timeout, uint_16 id);

extern int_32  FTP_open         (pointer _PTR_, _ip_address, FILE_PTR);
extern int_32  FTP_command      (pointer, char_ptr, FILE_PTR);
extern int_32  FTP_command_data (pointer, char_ptr, FILE_PTR, FILE_PTR, uint_32);
extern int_32  FTP_close        (pointer, FILE_PTR);

extern uint_32 TELNET_connect   (_ip_address);

extern uint_32 SNTP_oneshot     (_ip_address, uint_32);

extern void MIB1213_init (void);
extern void MIBMQX_init  (void);

/* Obsolete -- provided for compatility */
extern void RTCS_netstat_task (uint_32);
extern void ECHO_task         (uint_32);
extern void TELNET_server     (uint_32);
extern void EDS_task          (uint_32);
#define TELNET_client RTCS_netstat_task

/*
** RTCS initialization prototypes
*/

extern uint_32 RTCS_create
(
   void
);

extern uint_32 RTCS_create2
(
   void
);

extern uint_32 RTCS_if_add
(
   pointer                    ,  /* [IN] the packet driver handle */
   RTCS_IF_STRUCT_PTR         ,  /* [IN] call table for the interface */
   _rtcs_if_handle _PTR_         /* [OUT] the RTCS interface state structure */
);
extern uint_32 RTCS_if_remove
(
   _rtcs_if_handle               /* [IN] the RTCS interface state structure */
);
extern uint_32 RTCS_if_bind
(
   _rtcs_if_handle   handle   ,  /* [IN] the RTCS interface state structure */
   _ip_address       address  ,  /* [IN] the IP address for the interface */
   _ip_address       netmask     /* [IN] the IP (sub)network mask for the interface */
);
extern uint_32 RTCS_if_bind1
(
   _rtcs_if_handle   handle   ,  /* [IN] the RTCS interface state structure */
   _ip_address       address  ,  /* [IN] the IP address for the interface */
   _ip_address       netmask     /* [IN] the IP (sub)network mask for the interface */
);
extern uint_32 RTCS_if_bind_BOOTP
(
   _rtcs_if_handle            ,
   BOOTP_DATA_STRUCT_PTR
);
extern uint_32 RTCS_if_bind_IPCP
(
   _rtcs_if_handle            ,
   IPCP_DATA_STRUCT_PTR
);
extern uint_32 RTCS_if_unbind
(
   _rtcs_if_handle   handle   ,  /* [IN] the RTCS interface state structure */
   _ip_address       address     /* [IN] the IP address for the interface */
);
extern uint_32 RTCS_gate_add
(
   _ip_address       gateway  ,  /* [IN] the IP address of the gateway */
   _ip_address       network  ,  /* [IN] the IP (sub)network to route */
   _ip_address       netmask     /* [IN] the IP (sub)network mask to route */
);
/* Start CR 1133 */
extern uint_32 RTCS_gate_add_metric
(
   _ip_address       gateway  ,  /* [IN] the IP address of the gateway */
   _ip_address       network  ,  /* [IN] the IP (sub)network to route */
   _ip_address       netmask  ,  /* [IN] the IP (sub)network mask to route */
   uint_16           metric      /* [IN] the metric of the gateway */
);
/* End CR */
extern uint_32 RTCS_gate_remove
(
   _ip_address       gateway  ,  /* [IN] the IP address of the gateway */
   _ip_address       network  ,  /* [IN] the IP (sub)network to route */
   _ip_address       netmask     /* [IN] the IP (sub)network mask to route */
);
/* Start CR 1133 */
extern uint_32 RTCS_gate_remove_metric
(
   _ip_address       gateway  ,  /* [IN] the IP address of the gateway */
   _ip_address       network  ,  /* [IN] the IP (sub)network to route */
   _ip_address       netmask  ,  /* [IN] the IP (sub)network mask to route */
   uint_16           metric      /* [IN] the metric of the gateway */
);
/* End CR */

extern uint_32 DNS_init
(
   void
);

extern void  RTCS_log_error
(
   uint_32  module,
   uint_32  error,
   uint_32  p1,
   uint_32  p2,
   uint_32  p3
);


#include <dhcp.h>

/*
** The DHCP server
*/

#define DHCPSRV_option_int8     DHCP_option_int8
#define DHCPSRV_option_int16    DHCP_option_int16
#define DHCPSRV_option_int32    DHCP_option_int32
#define DHCPSRV_option_addr     DHCP_option_addr
#define DHCPSRV_option_addrlist DHCP_option_addrlist
#define DHCPSRV_option_string   DHCP_option_string

extern uint_32 DHCPSRV_ippool_add
(
   _ip_address             ipstart,
   uint_32                 ipnum,
   DHCPSRV_DATA_STRUCT_PTR params,
   uchar_ptr               optptr,
   uint_32                 optlen
);

/*
** The IPIP device
*/

extern uint_32 RTCS_tunnel_add
(
   _ip_address  inner_source_addr,     /* [IN] Inner IP header source        */
   _ip_address  inner_source_netmask,  /* [IN] Inner IP header source mask   */
   _ip_address  inner_dest_addr,       /* [IN] Inner IP header dest          */
   _ip_address  inner_dest_netmask,    /* [IN] Inner IP header dest mask     */
   _ip_address  outer_source_addr,     /* [IN] Outer IP header source        */
   _ip_address  outer_dest_addr,       /* [IN] Outer IP header dest          */
   uint_32 flags                       /* [IN] Flags to set tunnel behaviour */
);

extern uint_32 RTCS_tunnel_remove
(
   _ip_address  inner_source_addr,     /* [IN] Inner IP header source        */
   _ip_address  inner_source_netmask,  /* [IN] Inner IP header source mask   */
   _ip_address  inner_dest_addr,       /* [IN] Inner IP header dest          */
   _ip_address  inner_dest_netmask,    /* [IN] Inner IP header dest mask     */
   _ip_address  outer_source_addr,     /* [IN] Outer IP header source        */
   _ip_address  outer_dest_addr,       /* [IN] Outer IP header dest          */
   uint_32 flags                       /* [IN] Flags to set tunnel behaviour */
);

#ifdef __cplusplus
}
#endif


#endif

//#define HTTP_PRINT

extern "C" void printfp(char*);
extern "C" void printfpd(char* , unsigned long );

/* EOF */
