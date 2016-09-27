/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved                       
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: httpd_cnfg.h$
* $Version : 3.6.19.0$
* $Date    : Aug-16-2010$
*
* Comments:
*
*   HTTPD default configuration.
*
*END************************************************************************/

#ifndef HTTPD_CNFG_H_
#define HTTPD_CNFG_H_

//120405 #define HTTPDCFG_POLL_MODE     1

#define HTTPDCFG_POLL_MODE     0

/* server without rtc */
#define HTTPD_CLOCKLESS                 1

#ifndef HTTPDCFG_DEF_ADDR
#define HTTPDCFG_DEF_ADDR               INADDR_ANY     // default listen address
#endif

#ifndef HTTPDCFG_DEF_PORT
#define HTTPDCFG_DEF_PORT               80      // default listen port
#endif

#ifndef HTTPDCFG_DEF_INDEX_PAGE
#define HTTPDCFG_DEF_INDEX_PAGE         "index.htm"
#endif

#ifndef HTTPDCFG_DEF_SES_CNT
//#define HTTPDCFG_DEF_SES_CNT            2       // default sessions count
#define HTTPDCFG_DEF_SES_CNT            1       // 120409 default sessions count
#endif

#ifndef HTTPDCFG_DEF_URL_LEN
//150320#define HTTPDCFG_DEF_URL_LEN            128     // maximal URL length
#define HTTPDCFG_DEF_URL_LEN            512     // maximal URL length
#endif

#ifndef HTTPDCFG_DEF_AUTH_LEN
//150320#define HTTPDCFG_DEF_AUTH_LEN           16      // maximal length for auth data
#define HTTPDCFG_DEF_AUTH_LEN           512      // maximal length for auth data
#endif

#ifndef HTTPDCFG_MAX_BYTES_TO_SEND
#define HTTPDCFG_MAX_BYTES_TO_SEND      (512)  // maximal send data size in one step
#endif

#ifndef HTTPDCFG_MAX_SCRIPT_LN
//150320#define HTTPDCFG_MAX_SCRIPT_LN          16      // maximal length for script line
#define HTTPDCFG_MAX_SCRIPT_LN          128      // maximal length for script line
#endif

#ifndef HTTPDCFG_RECV_BUF_LEN
//150320#define HTTPDCFG_RECV_BUF_LEN           32
#define HTTPDCFG_RECV_BUF_LEN           128
#endif

//#define HTTP_KEEP_ALIVE_TIME          100     // 10 seconds

/* Default buffer configuration */
#ifndef HTTPD_MAX_LEN
//141219#define HTTPD_MAX_LEN                   128
//150320#define HTTPD_MAX_LEN                   128
#define HTTPD_MAX_LEN                   512
#endif

#ifndef HTTPDCFG_MAX_HEADER_LEN
#define HTTPDCFG_MAX_HEADER_LEN         256  // maximal length for http header
//#define HTTPDCFG_MAX_HEADER_LEN         512  // maximal length for http header
#endif

#ifndef HTTPDCFG_SES_TO
//120504 #define HTTPDCFG_SES_TO                 (20000) // session timeout in ms
#define HTTPDCFG_SES_TO                 (8000) //120504  session timeout in ms
#endif


#ifndef HTTPD_TIMEOUT_REQ_MS
//141219#define HTTPD_TIMEOUT_REQ_MS            (4000) /*ms. Request Timeout */
#define HTTPD_TIMEOUT_REQ_MS            (1000) /*ms. Request Timeout */
#endif

#ifndef HTTPD_TIMEOUT_SEND_MS
//141219#define HTTPD_TIMEOUT_SEND_MS           (8000) /*ms. Send Timeout */
#define HTTPD_TIMEOUT_SEND_MS           (1000) /*ms. Send Timeout */
#endif

/* socket settings */

#ifndef HTTPCFG_TX_WINDOW_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define HTTPCFG_TX_WINDOW_SIZE (1460) 
   #else
      #define HTTPCFG_TX_WINDOW_SIZE (3*1460) 
   #endif
#endif

#ifndef HTTPCFG_RX_WINDOW_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define HTTPCFG_RX_WINDOW_SIZE (1460) 
   #else
      #define HTTPCFG_RX_WINDOW_SIZE (3*1460) 
   #endif
#endif

#ifndef HTTPCFG_TIMEWAIT_TIMEOUT
   #define HTTPCFG_TIMEWAIT_TIMEOUT 1000 
#endif


#endif // HTTPD_CNFG_H_