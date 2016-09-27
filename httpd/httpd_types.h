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
* $FileName: httpd_types.h$
* $Version : 3.6.15.0$
* $Date    : Aug-16-2010$
*
* Comments:
*
*   HTTPD type definitions.
*
*END************************************************************************/

#ifndef HTTPD_TYPES_H_
#define HTTPD_TYPES_H_

//120328 #include "user_config.h"

#include "httpd_cnfg.h"
#include "httpd_wrapper.h"
#include "httpd_mqx.h"

#ifndef HTTPD_DBG_LEV
#define HTTPD_DBG_LEV                       0       // http debug info level ( 0 - 4 )
#endif

/** http debug macro
 */
/* 120228
#if HTTPD_DBG_LEV > 0

#define HTTPD_DEBUG(lev, ...)   {           \
    if (lev <= HTTPD_DBG_LEV)   {           \
        HTTPD_TIME_STRUCT time;             \
        HTTPD_GET_TIME(&time);              \
        printf("HTTP (%u) - %4d.%3d: ", lev, time.SECONDS, time.MILLISECONDS);          \
        printf(__VA_ARGS__);                \
    }                                       \
}

#define HTTPD_ASSERT(cond) {                \
    if (!(cond)) while(1) {};                   \
}

#else
#define HTTPD_DEBUG(lev, ...)           {}
#define HTTPD_ASSERT(cond)              {}
#endif
*/



#define HTTPD_SESSION_VALID             1
#define HTTPD_SESSION_INVALID           0

/** http state machine status
 */

/*
typedef enum httpd_ses_state {
    HTTPD_SES_WAIT_REQ,
    HTTPD_SES_PROCESS_REQ,
    HTTPD_SES_END_REQ,
    HTTPD_SES_CLOSE,
    HTTPD_SES_RESP
} HTTPD_SES_STATE;
*/

#define     HTTPD_SES_WAIT_REQ 		(0)
#define     HTTPD_SES_PROCESS_REQ 	(1)
#define     HTTPD_SES_END_REQ 		(2)
#define     HTTPD_SES_CLOSE 		(3)
#define     HTTPD_SES_RESP 		(4)


/** http request method type
 */

/*
typedef enum httpd_req_method {
    HTTPD_REQ_UNKNOWN,
    HTTPD_REQ_GET,
    HTTPD_REQ_POST
} HTTPD_REQ_METHOD;
*/

#define   HTTPD_REQ_UNKNOWN 	(0)
#define    HTTPD_REQ_GET	(1)
#define    HTTPD_REQ_POST	(2)


/** http content type
 */
/*
typedef enum httpd_content_type {
    CONTENT_TYPE_PLAIN,
    CONTENT_TYPE_HTML,
    CONTENT_TYPE_CSS,
    CONTENT_TYPE_GIF,
    CONTENT_TYPE_JPG,
    CONTENT_TYPE_PNG,
    CONTENT_TYPE_OCTETSTREAM
} HTTPD_CONTENT_TYPE;
*/

#define      CONTENT_TYPE_PLAIN 	(0)
#define      CONTENT_TYPE_HTML		(1)
#define      CONTENT_TYPE_CSS		(2)
#define      CONTENT_TYPE_GIF		(3)
#define      CONTENT_TYPE_JPG		(4)
#define      CONTENT_TYPE_PNG		(5)
#define      CONTENT_TYPE_OCTETSTREAM 	(6)


/** http request parameters
 */
typedef struct httpd_req_struct {
 //120406   HTTPD_REQ_METHOD method;
	unsigned char   method;
    char *path;
    char *urldata;

    int content_len;

    char *auth;
    char *authPasswd;
} HTTPD_REQ_STRUCT;

/** http response parameters
 */
typedef struct httpd_res_struct {
    HTTPD_FILE *file;             ///< file node for send
    char hdrsent;               ///< 1 - header sended
    int statuscode;             ///< status code - http_sendhdr transform it to text
 //120406   HTTPD_CONTENT_TYPE contenttype; ///< content type
  unsigned char contenttype; ///< content type

    char header[HTTPDCFG_MAX_HEADER_LEN + 1];

    char script_token;
    char *data;
    int len;
} HTTPD_RES_STRUCT;


/** http session structure
 */
typedef struct httpd_session_struct {
//120406    HTTPD_SES_STATE state;      ///< http session state
    unsigned char state;      ///< http session state
    int valid;                  ///< non zero (1) = session is valid - data in this entry is valid
    unsigned int keep_alive;    ///< connection Persistance
    int sock;               ///< used socket

    HTTPD_REQ_STRUCT request;
    HTTPD_RES_STRUCT response;
    int header;
    int req_lines;
    int remain;
    HTTPD_TIME_STRUCT time; ///< state start time in ticks.

    char recv_buf[HTTPDCFG_RECV_BUF_LEN + 1];
    char *recv_rd;
    int recv_used;

#if HTTPDCFG_POLL_MODE    
    char *line;
    int line_used;
#endif    
} HTTPD_SESSION_STRUCT;

typedef struct httpd_root_dir_struct {
    char *alias;
    char *path;
} HTTPD_ROOT_DIR_STRUCT;

/** CGI callback function - script can evaluate content data.
 * Function is called until remain content data or until script return error (<0)
 * return < 0 if error or evaluated data length
 */
typedef int(*CGI_CALLBACK)(HTTPD_SESSION_STRUCT*);

/** CGI function link - script associate name with function.
 * Request for script is always scriptname.cgi, but name in table is without extension.
 */
typedef struct httpd_cgi_link_struct {
    char cgi_name[HTTPDCFG_MAX_SCRIPT_LN + 1];
    CGI_CALLBACK callback;
} HTTPD_CGI_LINK_STRUCT;

/**
 */
typedef void(*FN_CALLBACK)(HTTPD_SESSION_STRUCT*);

/** call function link - dynamic web.
 */
typedef struct httpd_fn_link_struct {
    char fn_name[HTTPDCFG_MAX_SCRIPT_LN + 1];
    FN_CALLBACK callback;
} HTTPD_FN_LINK_STRUCT;

/** Authentification callback function.
 * Callback return 0 if authorization is correct.
 */
typedef int(*HTTPD_AUTH_CALLBACK)(HTTPD_SESSION_STRUCT*);

/** http server parameters
 */
typedef struct httpd_params_struct {
    unsigned short port;            ///< listening port
	_ip_address  address;
    unsigned int max_uri;           ///< maximal URI length
    unsigned int max_auth;          ///< maximal auth length
    unsigned int max_ses;           ///< sessions count

#if HTTPDCFG_POLL_MODE  
    unsigned int max_line;          ///< maximal line length
#endif

    HTTPD_ROOT_DIR_STRUCT *root_dir;        ///< pointer root dir structure
    char *index_page;               ///< index page full path and name

    // callback functions
    HTTPD_CGI_LINK_STRUCT *cgi_lnk_tbl; ///< cgi callback table
    HTTPD_FN_LINK_STRUCT *fn_lnk_tbl;   ///< function callback table (dynamic web pages)
    HTTPD_AUTH_CALLBACK auth_fn;    ///< callback for authentification

    char *page401;
    char *page403;
    char *page404;
} HTTPD_PARAMS_STRUCT;

/** http server main structure.
 */
typedef struct httpd_struct {
    HTTPD_PARAMS_STRUCT *params;            ///< pointer to server parameters

    // runtime data
    int sock;                               ///< listening socket
    HTTPD_SESSION_STRUCT **session;         ///< pointers array to session
} HTTPD_STRUCT;


#endif  // HTTPD_TYPES_H_
