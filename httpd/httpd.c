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
* $FileName: httpd.c$
* $Version : 3.6.25.0$
* $Date    : Aug-16-2010$
*
* Comments:
*
*   This file contains the HTTPD implementation.
*
*END************************************************************************/

#include "httpd.h"
#include "httpd_supp.h"

#if HTTPDCFG_POLL_MODE
#include <select.h>
#endif

#include <string.h>
#include <stdlib.h>

//extern int strcasecmp(const char *str1, const char *str2);

HTTPD_PARAMS_STRUCT* httpd_default_params(HTTPD_PARAMS_STRUCT *params) {

         HTTPD_PARAMS_STRUCT *ptr = NULL;
    //ok    return ptr; //t

  //  if (params) {      //hier clling with  NULL do not work!!!
  //      ptr = params;
  //  }
  //  else {
    //   ptr = HTTPD_MALLOC(sizeof(HTTPD_PARAMS_STRUCT));
  //    ptr = (HTTPD_PARAMS_STRUCT*)_lwmem_alloc(sizeof(HTTPD_PARAMS_STRUCT));
     ptr = HTTPD_MALLOC(sizeof(HTTPD_SESSION_STRUCT)); //120411

//120411   ptr = _mem_alloc_system_zero(sizeof(HTTPD_PARAMS_STRUCT));
//120402       HTTPD_MEMTYPE(ptr,MEM_TYPE_HTTPD_PARAMS);
  //  }
    //    return ptr;//t
    if (ptr) {
    
        HTTPD_MEMZERO(ptr, sizeof(HTTPD_PARAMS_STRUCT));
 //t120409		HTTPD_MEMZERO(ptr, sizeof(HTTPD_PARAMS_STRUCT)); //try 120409 ok
    //   return ptr; //t
        ptr->port = HTTPDCFG_DEF_PORT;
        ptr->address = HTTPDCFG_DEF_ADDR;
        ptr->max_uri = HTTPDCFG_DEF_URL_LEN;
        ptr->max_auth = HTTPDCFG_DEF_AUTH_LEN;

        ptr->cgi_lnk_tbl = 0;
        ptr->fn_lnk_tbl = 0;
         
        ptr->auth_fn = 0;
        
        ptr->root_dir = NULL;
        ptr->index_page = HTTPDCFG_DEF_INDEX_PAGE;

        ptr->max_ses = HTTPDCFG_DEF_SES_CNT;
        
#if HTTPDCFG_POLL_MODE
        ptr->max_line = HTTPD_MAX_LEN;
#endif      

    }
       
    return ptr;
}

int httpd_set_index_page(HTTPD_STRUCT *server, char *index_page) {
 //   HTTPD_ASSERT(server && server->params && index_page);
    if (server && server->params && index_page) {
        server->params->index_page = index_page;
        return 0;
    }
    
    return 1;
}

HTTPD_STRUCT* httpd_init(HTTPD_PARAMS_STRUCT *params) {
    struct sockaddr_in sin;
    HTTPD_STRUCT *server = NULL;
    int option, error, si;

  //  HTTPD_ASSERT(params);

    if (params == NULL)
        return NULL;

    if ((server = HTTPD_MALLOC(sizeof(HTTPD_STRUCT))) == NULL) {
     //   HTTPD_DEBUG(1, "server: main structure allocation error\n");            
        return NULL;
    }
    
  //  HTTPD_MEMTYPE(server,MEM_TYPE_HTTPD_STRUCT);
    server->params = params;

#if HTTPDCFG_POLL_MODE
    // session table allocation
    if ((server->session = HTTPD_MALLOC(sizeof(HTTPD_SESSION_STRUCT*) * server->params->max_ses)) == NULL) {
    //    HTTPD_DEBUG(1, "server: session table allocation error\n");
#ifdef HTTP_PRINT
 printfp(" server: session table allocation error\n");
#endif
        goto ERROR_ALLOC;
    }
    
  //  HTTPD_MEMTYPE(server->session, MEM_TYPE_HTTPD_SESSION_STRUCT);
     
    // sessions allocation and sessions table initialization 
    for (si = 0; si < server->params->max_ses; si++) {
        server->session[si] = httpd_ses_alloc(server);
        
        if (server->session[si] == NULL) {
            while (--si) {
                HTTPD_FREE(server->session[si]);
            }

      //      HTTPD_DEBUG(1, "server: session allocation error\n");           
            goto ERROR_ALLOC;
        }
    }
#endif

    // init socket
    if ((server->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //    HTTPD_DEBUG(1, "socket: creation error\n");
#ifdef HTTP_PRINT
 printfp(" socket: creation error\n");
#endif
        goto ERROR_ALLOC;
    }

#ifdef __MQX__
    option = 500;
    error = setsockopt(server->sock, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));

    option = 500;
    error = setsockopt(server->sock, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));

    option = 5000;
    error = setsockopt(server->sock, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));

    option = 1000;
    //error = setsockopt(server->sock, SOL_TCP, OPT_RETRANSMISSION_TIMEOUT, &option, sizeof(option));
    
    option = 1000;
    //error = setsockopt(server->sock, SOL_TCP, OPT_MAXRTO, &option, sizeof(option));

    option = HTTPCFG_TIMEWAIT_TIMEOUT;
    error = setsockopt(server->sock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));

    option = TRUE;
    error = setsockopt(server->sock, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));

    option = TRUE;
    //error = setsockopt(server->sock, SOL_TCP, OPT_SEND_PUSH, &option, sizeof(option));
    
    option = TRUE;
    //error = setsockopt(server->sock, SOL_TCP, OPT_RECEIVE_PUSH, &option, sizeof(option));

    option = TRUE;
    error = setsockopt(server->sock, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));

    option = HTTPCFG_TX_WINDOW_SIZE;
    error = setsockopt(server->sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));

    option = HTTPCFG_RX_WINDOW_SIZE;
    error = setsockopt(server->sock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));
#endif

#ifndef __MQX__
    sin.sin_port = htons(server->params->port);
	sin.sin_addr.s_addr = htonl(server->params->address);
#else
    sin.sin_port = server->params->port;
	sin.sin_addr.s_addr = server->params->address;
#endif

    sin.sin_family = AF_INET;

    // bind address
    if ((int_32)bind(server->sock, &sin, sizeof(sin)) < 0) {
    //    HTTPD_DEBUG(1, "bind error\n");
#ifdef HTTP_PRINT
 printfp(" bind error\n");
#endif
        goto ERROR;
    }

    // listen
    if ((int_32)listen(server->sock, 5) < 0) {
    //    HTTPD_DEBUG(1, "socket: listen error\n");
#ifdef HTTP_PRINT
 printfp("socket: listen error\n");
#endif

        goto ERROR;
    }

  //  HTTPD_DEBUG(1, "------------------------------------------\n");
  //  HTTPD_DEBUG(1, " HTTP 1.1 server has been started.\n");
//  HTTPD_DEBUG(1, " Use: http://%s.\n",ip_str);
 //   HTTPD_DEBUG(1, "------------------------------------------\n\n");

    return server;

    ERROR:
 //   printf("error %d\n", errno);

#ifdef HTTP_PRINT
 printfpd("error %d\n", errno);
#endif

#ifdef __MQX__
    shutdown(server->sock, FLAG_ABORT_CONNECTION);
#else
    if (close(server->sock) < 0) {
        //return;
    }
#endif

    ERROR_ALLOC:
    
    HTTPD_FREE(server);

    return NULL;
}

int httpd_release(HTTPD_STRUCT *server) {
    int res = 1;

#ifdef HTTP_PRINT
 printfp(" httpd_release");
#endif
    
    if (server) {
#ifdef __MQX__
        shutdown(server->sock, FLAG_ABORT_CONNECTION);
#else
        if (close(server->sock) < 0) {
            //return;
        }
#endif  
        //server->socket = SOCKET_INVALID;

        memset(server->params, 0, sizeof(HTTPD_PARAMS_STRUCT));
    }

    return res;
}

/** Close session
 * \param session < pointer to session
 */
static void httpd_ses_close(HTTPD_SESSION_STRUCT *session) {
//#ifdef HTTP_PRINT
//do not print this printfp("... ses_close ");
//#endif

//ok return;

  //  HTTPD_ASSERT(session);
  //  HTTPD_DEBUG(1, "session closed (%p)\n", session);

    if (session->response.file) {
#ifdef HTTP_PRINT
//______________________________
printfp(" fclose_18");
// OperateBuffers_usart0t();
 //____________________________
#endif

        fclose(session->response.file);
    }

    session->valid = HTTPD_SESSION_INVALID;     //0
//#ifdef HTTP_PRINT
// printfp(" before ");
//#endif

// shutdown(session->sock, FLAG_CLOSE_TX);

#ifdef __MQX__

#ifdef HTTP_PRINT
 printfp("\n\rses_close shutdown_18 \n\r");
#endif

//____________________________

// printfp(" start shutdown ");
//OperateBuffers_usart0t();
 //____________________________

    shutdown(session->sock, FLAG_CLOSE_TX);
 //   shutdown(session->sock, FLAG_ABORT_CONNECTION);
    
    
//#ifdef HTTP_PRINT
// printfp(" after");
//#endif
//not used&&return;

#else
    close(session->sock);

#ifdef HTTP_PRINT
 printfp(" close_18 ");
#endif
//return;
#endif

}

/** Read http request.
 * \param session < pointer to session structure
 * \return
 */
static int httpd_readreq(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
#if HTTPDCFG_POLL_MODE
    char *buf;
#else
    char buf[HTTPD_MAX_LEN + 1];
#endif

    char *cp, *cp2;
    int len;
    int len_my;
    char * buf_my;
//#ifdef HTTP_PRINT
//many prints printfp(" httpd_readreq_18");
//#endif

 //   HTTPD_ASSERT(server && session);

    // standard response
  //  strcpy(session->response.header, "Server: MQX HTTP - Freescale Embedded Web Server\n");
    strcpy(session->response.header, "Server: MQX HTTP - Radian Embedded Web Server\n");
    session->response.contenttype = CONTENT_TYPE_HTML;
    session->response.statuscode = 200;
    session->response.hdrsent = 0;
    session->response.file = NULL;
    session->response.data = 0;
    session->response.script_token = 0;
    
  //  HTTPD_DEBUG(4, "request BEGIN\n");
    

#if HTTPDCFG_POLL_MODE
    buf = &session->line[session->line_used];
    len = server->params->max_line - session->line_used;
#else
    len = HTTPD_MAX_LEN;
#endif

//____________________________________________________________



//____________________________________________________________

// #ifdef HTTP_PRINT
      
//         len_my = len;
//         buf_my = buf;
//         printfp("\n\r>");
//         while(len_my--){ /* Body */
 //       printfpd("%c",*buf_my++);
         
 //    } /* Endbody */
   //      printfp("\n\r<");

//#endif      
    

    // read the request
    while (httpd_readln(session, buf, &len)) {
  //      HTTPD_DEBUG(4, "request: %s\n", buf);
// #ifdef HTTP_PRINT
//   printfp("\n\r session->header");
//   printfpd("%d", session->header);
//  printfp("\n\r session->req_lines");
//   printfpd("%d", session->req_lines);
   
// #endif 
        session->req_lines++;
        
#if HTTPDCFG_POLL_MODE
        buf = session->line;
        len = server->params->max_line;
#endif
//________________________________   
 #ifdef HTTP_PRINT
      
  //       len_my = len;
  //       buf_my = buf;
  //       printfp("\n\r");
  //       while(len_my--){ /* Body */
  //       printfpd("%c",*buf_my++);
         
   //  } /* Endbody */
   //      printfp("\n\r");

#endif         
//_________________________________      

        if (1 == session->req_lines) {  // first line = start-line
            if ((cp = strstr(buf, "GET"))!=0)
            {
#ifdef HTTP_PRINT
 printfp("\n\r HTTPD_REQ_GET");
#endif
            
                session->request.method = HTTPD_REQ_GET;
            }
            else if ((cp = strstr(buf, "POST"))!=0)
            {
                session->request.method = HTTPD_REQ_POST;
                
#ifdef HTTP_PRINT
 printfp("\n\r_____ HTTPD_REQ_POST!!!!!!!!");
 printfp( session->request.urldata);
 printfpd("\n\rh:%X", session->header);

#endif
               
                
            }
            else {  // unknown method
                session->request.method = HTTPD_REQ_UNKNOWN;
                
#ifdef HTTP_PRINT
 printfp("\n\r HTTPD_REQ_UNKNOWN");
#endif
                
                return -1;
            }

            // parse remain part of 1. line
            cp = strchr(cp, ' ') + 1;
            cp2 = strchr(cp, ' ');
            *cp2 = 0;
#ifdef HTTP_PRINT
 printfp("\n\rhttpd_sanitiseurl");
#endif
            

            strncpy(session->request.path, cp, server->params->max_uri);
            httpd_sanitiseurl(session->request.path);
            continue;
        }
        
 //______________________________________________________________my
     /*
       if (strncmp(buf, "Content-Length: ", 16) == 0) {
       
 #ifdef HTTP_PRINT
          printfp("\n\r..found length!!!");
#endif        
                cp = strchr(buf, ':') + 2;
                if (cp)
                {
                    session->request.content_len = _io_atoi(cp);
#ifdef HTTP_PRINT
          printfpd("\n\rc_l>...%d",session->request.content_len);
#endif         

            //        HTTPD_DEBUG(4, "Content-Length: %u\n", session->request.content_len);
                }
 
          }
          */
 
 //______________________________________________________________my       
        
        
        
        

        if (session->header) {
            if (*buf == 0) {        // header end - \r\n\r\n
                session->header = 0;
#ifdef HTTP_PRINT
          printfp("\n\r BREAK");
#endif                 
                len = 0;
                break;
            }
            else if (strncmp(buf, "Connection: ", 12) == 0) {
                cp = strchr(buf, ':') + 2;
                
                if (strncmp(cp, "keep-alive", 10) == 0) {
                    session->keep_alive = 0;
            //        HTTPD_DEBUG(4, "Connection: keep-alive\n");
#ifdef HTTP_PRINT
          printfp("\n\rConnection: keep-alive");
#endif                    
                }
            }
            else if (strncmp(buf, "Content-Length: ", 16) == 0) {
                cp = strchr(buf, ':') + 2;
                if (cp)
                {
                    session->request.content_len = _io_atoi(cp);
#ifdef HTTP_PRINT
          printfpd("\n\rc_l>...%d",session->request.content_len);
#endif         

            //        HTTPD_DEBUG(4, "Content-Length: %u\n", session->request.content_len);
                }
            }
            else if (strncmp(buf, "Authorization: ", 15) == 0) {
                cp = strchr(buf, ':') + 2;

                if (strncmp(cp, "Basic ", 6) == 0) {
                    cp = strchr(cp, ' ') + 1;

                    httpd_decode_base64(session->request.auth, cp, (int)server->params->max_auth);

                    cp = strchr(session->request.auth, ':');
                    if (cp)
                    {
                        *cp = 0;
                        session->request.authPasswd = cp + 1;
                    }
#ifdef HTTP_PRINT
          printfp("\n\rAuthorization");
#endif                       

             //       HTTPD_DEBUG(4, "Authorization - Basic: %s:%s\n", session->request.auth, session->request.authPasswd);
                }
                else
                {
                
#ifdef HTTP_PRINT
          printfp("\n\rAuthorization - unknown method");
#endif                     
                
             //       HTTPD_DEBUG(4, "Authorization - unknown method\n");
                }
            }

            continue;
        }
        else{ /* Body */
        
#ifdef HTTP_PRINT
          printfp("\n\r ELSE");
#endif               
      } /* Endbody */
        
    }
    
#if HTTPDCFG_POLL_MODE
    session->line_used += len;
#endif

    if (!session->header)
    {
    
#ifdef HTTP_PRINT
          printfp("\n\r NO HEADER\n\r");     
//           printfp(buf);
//              printfp("\n\r -----------\n\r");  
          
#endif       
//____________________________________________________________
 /*
if (strncmp(buf, "Content-Length: ", 16) == 0) {
                cp = strchr(buf, ':') + 2;
                if (cp)
                {
                    session->request.content_len = _io_atoi(cp);
#ifdef HTTP_PRINT
          printfpd("\n\rc_l>...%d",session->request.content_len);
#endif         

            //        HTTPD_DEBUG(4, "Content-Length: %u\n", session->request.content_len);
                }
            }

       */


//____________________________________________________________


       
        // process any URL data
        cp = strchr(session->request.path, '?');
        if (0 != cp)
        {
            *cp = 0;
            cp++;
            session->request.urldata = cp;

#ifdef HTTP_PRINT
// printfp(session->request.urldata);
#endif
        }
        
        
        
    }
//_________________________________
/*

  while (httpd_readln(session, buf, &len)) {
  //      HTTPD_DEBUG(4, "request: %s\n", buf);
 #ifdef HTTP_PRINT
   printfp("\n\rtry read some");
//   printfpd("%d", session->header);
//  printfp("\n\r session->req_lines");
//   printfpd("%d", session->req_lines);
   
 #endif 
  //      session->req_lines++;
        
//#if HTTPDCFG_POLL_MODE
//        buf = session->line;
//        len = server->params->max_line;
//#endif
 #ifdef HTTP_PRINT
      
         len_my = len;
         buf_my = buf;
         while(len_my--){ 
         printfpd("-%c",*buf_my++);
         
      } 
#endif         
}

*/
//__________________________________    
    
    
    
    
 //  HTTPD_DEBUG(4, "request END\n");

//#ifdef HTTP_PRINT
//many prints printfp("\n\r request END");
//#endif

    return 0;
}

extern "C" void httpd_set_device_params(char *, int*); 

/** Http request process.
 * \param server < pointer to server
 * \param session < pointer to session data structure
 */
static void httpd_processreq(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session)
{

//_____________________________________
char *mycp;
int mylen;

// char buf[HTTPD_MAX_LEN + 1];

//_____________________________________


    char buf[HTTPD_MAX_LEN + 1];
    char *cp, *cp1;
    int cgi = 0, res = 0, auth = 0; // authorization required flag
    HTTPD_ROOT_DIR_STRUCT *root_dir;


#ifdef HTTP_PRINT
 printfp("\n\r httpd_ses_processreq\n\r");
// return;
#endif

  //  HTTPD_ASSERT(server && session);

    // check request path
    cp = strrchr(session->request.path, '/');
    if (0 == cp)
    {
    //    HTTPD_DEBUG(1, "Invalid request path '%s'\n", session->request.path);
    
 #ifdef HTTP_PRINT
 printfp("\n\r Invalid request path");
 printfp("session->request.path");
// return;
 #endif
    
        return;
    }
    
//#ifdef HTTP_PRINT
 //printfp("\n\r httpd_ses_processreq1\n\r");
// printfpd("\n\r len:%d :", session->request.content_len); 
// mylen = HTTPDCFG_RECV_BUF_LEN; //session->request.content_len;
// mycp =  session->request.path;
// printfp(session->request.path);
 
// for(mylen = 0; mylen < HTTPDCFG_RECV_BUF_LEN; mylen++){ /* Body */
// printfpd(" %c", session->recv_buf[mylen]);
//} /* Endbody */
 
//recv_buf[HTTPDCFG_RECV_BUF_LEN + 1] 
// return;


//_________________________________

//  while (httpd_readln(session, buf, &len)) {
//  while (httpd_readln(session, buf, &mylen)) {
  //      HTTPD_DEBUG(4, "request: %s\n", buf);
  
//mylen = HTTPDCFG_RECV_BUF_LEN; //session->request.content_len;
mylen = session->request.content_len;
if(mylen >  HTTPDCFG_RECV_BUF_LEN) 
mylen = HTTPDCFG_RECV_BUF_LEN;  
 if(mylen){ 
  
 #ifdef HTTP_PRINT
   printfp("\n\rlen:");
   printfpd("%d ",mylen);
//  printfp("\n\r session->req_lines");
//   printfpd("%d", session->req_lines);
   
 #endif 
 
 httpd_readln(session, buf, &mylen);
  //      session->req_lines++;
        
//#if HTTPDCFG_POLL_MODE
//        buf = session->line;
//        len = server->params->max_line;
//#endif
// #ifdef HTTP_PRINT
      
//         mylen = session->request.content_len;
//         mycp = buf;
//         while(mylen--){ 
//         printfpd("-%c",*mycp++);
//      } 
//#endif  

 httpd_set_device_params(buf, &session->request.content_len);     
session->request.content_len = 0; //to work on back - forward  
}


//__________________________________    
//#endif
    
    
    if (session->request.path[0] == '/' && session->request.path[1] == 0)
        strcpy(session->request.path, server->params->index_page);
    
    // check if request is script request

    // if request contain data
//  if (session->request.content_len)
//  {
        res = -1;

        if ((cp1 = strrchr(cp, '.')) != 0)
        {
            if (0 == _io_strcasecmp(cp1, ".cgi"))   // file extension is cgi, maybe cgi script
            {
                cgi = 1;
#ifdef HTTP_PRINT
 printfp("\n\r search cgi script");
#endif

                // search cgi script
                *cp1 = 0;
                res = httpd_cgi(server, session, cp + 1);   // cp + 1 = cgi script name begin
                *cp1 = '.';
            }
        }

    if (cgi || session->request.content_len)
    {
    
    
        if (res < 0)
        {
            if (res != -2)
            	cgi = 0;
            
            // cgi script not found - read data from socket - discard it
            while (session->request.content_len)
            {
                res = httpd_read(session, buf, (int)((res > sizeof(buf))? sizeof(buf) : 			session->request.content_len));

                session->request.content_len -= res;

                // TODO add timeout - can be deathlock

                // temporary solution
                if (res == 0)
                    break;
            }

        }
        else {
            // cgi script found - decrease content len
            session->request.content_len -= res;

            if (session->request.content_len == 0)  // whole content procesed
                session->state = HTTPD_SES_END_REQ;
#ifdef HTTP_PRINT
 printfp("\n\rcontent_len == 0 => state = HTTPD_SES_END_REQ_18  ");
#endif
                
        }
    }

    // send regular file(document) - if is not cgi script
    if (!cgi) {
        // get correct root dir (by alias)
        
        if (server->params->root_dir[1].alias) {
            // more root dir pressents, find alias
            root_dir = server->params->root_dir;
            
            cp = strchr(session->request.path, '/');
            
            if (cp) {
                cp++;
                cp1 = strchr(cp, '/');
            
                if (cp1) {
                    while (root_dir->alias) {
                        if (strncmp(root_dir->alias, cp, cp1 - cp) == 0) {
                            strcpy(buf, root_dir->path);
                            strcat(buf, cp1);
                            break;
                        }
                
                        root_dir++;
                    }
                }
            }
            
            if (!cp || !cp1 || !root_dir->alias) {
                strcpy(buf, server->params->root_dir->path);
                strcat(buf, session->request.path);
            }
        }
        else {
            strcpy(buf, server->params->root_dir[0].path);
            strcat(buf, session->request.path);
        }
    
        // correct slash
        cp = buf;
        while (*cp) {
        
        
            if ('/' == *cp)
                *cp = '\\';

            cp++;
        }

#ifdef __MQX__
        session->response.file = fopen(buf, "rb");
#else
        session->response.file = open(buf, 0, "r");
#endif

        session->response.len = 0;

        if (session->response.file) {
        }
        else {
            session->response.statuscode = 404;
        }

        session->state = HTTPD_SES_RESP;
#ifdef HTTP_PRINT
 printfp("\n\r 1 => state = HTTPD_SES_RESP ");
#endif
        
    }

    if (auth && server->params->auth_fn)
    {
        if (*session->request.auth)
        {
            // check authorization
            if (server->params->auth_fn(session))
            {
                session->response.statuscode = 401;
                strncat(session->response.header, "WWW-Authenticate: Basic realm=\"test\"\n", HTTPDCFG_MAX_HEADER_LEN);
            }
        }
        else
        {
            session->response.statuscode = 401;
            strncat(session->response.header, "WWW-Authenticate: Basic realm=\"test\"\n", HTTPDCFG_MAX_HEADER_LEN);
        }
    }
}

/** http response
 * \param server < pointer to server data structure
 * \param session < pointer to session data structure
 */
static void httpd_response(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {

#ifdef HTTP_PRINT
 printfp("\n\r httpd_response");
 printfpd("\n\r %X", server->params->address);

#endif
 //   HTTPD_ASSERT(server && session);

    switch (session->response.statuscode) {
    case 200:
    #ifdef HTTP_PRINT
   printfp("\n\rstatus200,,,,, httpd_sendfile!\n\r");
   #endif

        httpd_sendfile(server, session);
//_______________________________________        
 //  session->state = HTTPD_SES_END_REQ; //my 120503 - may be better?
//________________________________________      
        
        break;
    case 401:
     #ifdef HTTP_PRINT
   printfp("\n\rstatus401,,,,, httpd_send401\n\r");
   #endif

        httpd_send401(server, session);
        session->state = HTTPD_SES_END_REQ;
        
        break;
    default:
       #ifdef HTTP_PRINT
   printfp("\n\rstatus  default,,,,, httpd_send404\n\r");
   #endif

        httpd_send404(server, session);
        session->state = HTTPD_SES_END_REQ;
        break;
    }
    
    #ifdef HTTP_PRINT
   printfp("\n\rfinish response : session->state");
   printfpd(" %d \n\r", session->state);
   #endif
 
}

/** HTTPD session state machine
 * \param server < pointer to server
 * \param session < session number
 */
void httpd_ses_process(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
    HTTPD_TIME_STRUCT time;

#ifdef HTTP_PRINT
// printfp("httpd_ses_process");
// printfp("h");
#endif
 //ok          session->valid = HTTPD_SESSION_INVALID;     // for test

//if one time - ok return; //t120406

   // HTTPD_ASSERT(server && session);
   
#ifdef HTTP_PRINT
//  printfpd(" state : %d",session->state );
// printfpd(" val : %d",session->valid );
  
// return;  //t
#endif
   

    if (HTTPD_SESSION_VALID == session->valid)
    {

#ifdef HTTP_PRINT
//  printfpd("\n\r state : %d",session->state );
// return;
#endif

        // check timeout
        HTTPD_GET_TIME(&time);

#ifdef HTTP_PRINT
 // printfpd("\n\r state : %d",session->state );
//ok return;
#endif
        
  //120921      if (HTTPD_DIFF_TIME_MS(session->time, time) > HTTPDCFG_SES_TO)
        
           if((HTTPD_DIFF_TIME_MS(session->time, time) > HTTPDCFG_SES_TO) &&
         ((time.SECONDS*1000l +  time.MILLISECONDS) > 
              (session->time.SECONDS*1000l +  session->time.MILLISECONDS)  ) )
             
       //     HTTPD_DEBUG(1, "session %p timeout\n", session);
       {       
   //      printfp("\n\r...........SESSION TIMEOUT..........");
   //      printfpd("sestime :%d ",session->time.SECONDS);
   //      printfpd(".:%d ",session->time.MILLISECONDS);

    //      printfpd("time :%d ",time.SECONDS);
    //        printfpd(".:%d ",time.MILLISECONDS);

    //          printfpd("result :%d ",HTTPD_DIFF_TIME_MS(session->time, time));

          
          
          
            session->state = HTTPD_SES_CLOSE;
        }
         
#ifdef HTTP_PRINT
if(session->state)
{
   printfp("\n\rhttpd_ses_process_state");
   printfpd(" : %d_",session->state );

}
 // session->valid = HTTPD_SESSION_INVALID; //t120409
// return;  //120409
#endif
        
        switch (session->state) {
        
   case HTTPD_SES_WAIT_REQ:     //hier start
        
//        #ifdef HTTP_PRINT
//  printfp("HTTPD_SES_WAIT_REQ" );
 // session->valid = HTTPD_SESSION_INVALID; //t120409
// return;  //120409
//#endif


            if (0 > httpd_readreq(server, session))
            {
                session->state = HTTPD_SES_END_REQ;
#ifdef HTTP_PRINT
 printfp("\n\r 0 > httpd_readreq => state = HTTPD_SES_END_REQ  ");
#endif
                
            }
            else \
            {
                if (session->header == 0)
                {
                    session->state = HTTPD_SES_PROCESS_REQ;
                    HTTPD_GET_TIME(&session->time);
             //       session->valid = HTTPD_SESSION_INVALID; //for test120409
                }
            }

            break;
        case HTTPD_SES_PROCESS_REQ:
 //_____________________________________________
 //  printfp("HTTPD_SES_PROCESS_REQ" );
//   OperateBuffers_usart0t();
//____________________________________________
        
//#ifdef HTTP_PRINT
//  printfp("start HTTPD_SES_PROCESS_REQ" );
 //ok session->valid = HTTPD_SESSION_INVALID; //t120409
//ok return;  //120409
//#endif
        
            httpd_processreq(server, session);
            
     //        session->valid = HTTPD_SESSION_INVALID; //for test120409

            break;

        case HTTPD_SES_RESP:
  //_____________________________________________
 //  printfp("HTTPD_SES_RESP" );
 //  OperateBuffers_usart0t();
//____________________________________________
       
        
//#ifdef HTTP_PRINT
 // printfp("start httpd_response" );
 //ok session->valid = HTTPD_SESSION_INVALID; //t120409
//ok return;  //120409
//#endif
        
       //    session->valid = HTTPD_SESSION_INVALID; //for test120409

            httpd_response(server, session);
            break;

 case HTTPD_SES_END_REQ:
  //_____________________________________________
 //  printfp("HTTPD_SES_END_REQ" );
 //  OperateBuffers_usart0t();
//____________________________________________

              #ifdef HTTP_PRINT
               printfp("HTTPD_SES_END_REQ" );
               // session->valid = HTTPD_SESSION_INVALID; //t120409
               // return;  //120409
               #endif

        
            if (!session->keep_alive){ 
             #ifdef HTTP_PRINT
               printfp("=>HTTPD_SES_CLOSE_18" );
               // session->valid = HTTPD_SESSION_INVALID; //t120409
               // return;  //120409
               #endif

            
                            session->state = HTTPD_SES_CLOSE;
                            }
            else {
            
                // init session
                session->state = HTTPD_SES_WAIT_REQ;
                   #ifdef HTTP_PRINT
               printfp("=>HTTPD_SES_WAIT_REQ_18" );
               // session->valid = HTTPD_SESSION_INVALID; //t120409
               // return;  //120409
               #endif

                
        //      printfp("\n\rHTTPD_SES_END_REQ change state to");
        //      printfpd(" : %d_",session->state );
                
                //session->sock = sock;
                //session->valid = HTTPD_SESSION_VALID;

                session->header = 1;
                session->req_lines = 0;
                session->remain = 0;
                session->recv_used = 0;
        
                HTTPD_GET_TIME(&session->time);         
            
                session->keep_alive--;
                break;
            }
            
 case HTTPD_SES_CLOSE:
 //_____________________________________________
//   printfp("httpd_ses_close" );
//   OperateBuffers_usart0t();
//____________________________________________
            httpd_ses_close(session);
            break;
            
        default:
            // This cannot be good
            session->valid = HTTPD_SESSION_INVALID;
            break;
        }
    }
}

/** Allocate session.
 * \param server < pointer to server
 * \return pointer to allocated session
 */
HTTPD_SESSION_STRUCT* httpd_ses_alloc(HTTPD_STRUCT *server) {
    HTTPD_SESSION_STRUCT *session = NULL;

#ifdef HTTP_PRINT
 printfp("httpd_ses_alloc");
#endif

 //  HTTPD_ASSERT(server);

    if (server) {
        session = HTTPD_MALLOC(sizeof(HTTPD_SESSION_STRUCT));
        if (session) {
       //     HTTPD_MEMTYPE(session,MEM_TYPE_HTTPD_SESSION_STRUCT);

           HTTPD_MEMZERO(session, sizeof(HTTPD_SESSION_STRUCT));  //t120409

            session->request.path = HTTPD_MALLOC(server->params->max_uri + 1);
            if (NULL == session->request.path)
                goto ERROR;
            
        //    HTTPD_MEMTYPE(session->request.path,MEM_TYPE_HTTPD_URI);

            session->request.auth = HTTPD_MALLOC(server->params->max_auth + 1);
            if (NULL == session->request.auth)
                goto ERROR;
            
        //    HTTPD_MEMTYPE(session->request.auth,MEM_TYPE_HTTPD_AUTH);

            session->recv_used = 0;

#if HTTPDCFG_POLL_MODE
            session->line = HTTPD_MALLOC(server->params->max_line + 1);;
      //      HTTPD_MEMTYPE(session->line, MEM_TYPE_HTTPD_LINE);
            session->line_used = 0;
#endif              
        }
    }
    
#ifdef HTTP_PRINT
 printfp(" ses_alloc ok ");
#endif
    

    return session;

ERROR:
#ifdef HTTP_PRINT
 printfp(" ses_alloc error ");
#endif


    if (session->request.path)
        HTTPD_FREE(session->request.path);

    HTTPD_FREE(session);

    return NULL;
}

void httpd_ses_free(HTTPD_SESSION_STRUCT *session) {

#ifdef HTTP_PRINT
 printfp("httpd_ses_free");
#endif
  //  HTTPD_ASSERT(session);

    if (session) {
        HTTPD_FREE(session->request.path);
        HTTPD_FREE(session->request.auth);
        HTTPD_FREE(session);
    }
}

unsigned long far_server;

void httpd_ses_init(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session, const int sock) {
  //  HTTPD_ASSERT(server && session && sock >= 0);
#ifdef HTTP_PRINT
 printfp("httpd_ses_init");
#endif
     //ok  return;
    if (server && session) {
        // init session
        session->state = HTTPD_SES_WAIT_REQ;
        session->sock = sock;
        session->keep_alive = 0;
        session->valid = HTTPD_SESSION_VALID;
 //ok       session->valid = 0;       //for test no valid

        session->header = 1;
        session->req_lines = 0;
        session->remain = 0;
        session->recv_used = 0;

#if HTTPDCFG_POLL_MODE      
        session->line_used = 0;
#endif
        
        HTTPD_GET_TIME(&session->time);
    }
}

void httpd_ses_poll(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
    int s;
    unsigned short len;
    struct sockaddr_in sin;
    
    static int check, check1;
      static int check2;

 //   check++;
 //   check1++;


#ifdef HTTP_PRINT
 printfp("httpd_ses_poll");
  printfpd("\n\r %X", server->params->address);
   printfpd("\n\r %X", server->params->address);
#endif

//ok  return; // ok 120409 //t120406

 //   HTTPD_ASSERT(server && session);

    do {
    // check++;

#ifdef HTTP_PRINT
// printfp("...st r");
 printfpd(" valid : %d ", session->valid);
 // printfpd("ses_waiting check : %d ", check++);

#endif
    
        if (HTTPD_SESSION_VALID != session->valid) {
            len = sizeof(sin);       

            s = accept(server->sock, &sin, &len);
            
//#ifdef HTTP_PRINT
// printfp("...st r");
// printfpd(" s : %d ", s);
// printfpd("\n\r ip addr: %X", sin.sin_addr.s_addr);
//#endif
       far_server =   sin.sin_addr.s_addr;    
            
            if (s && s != RTCS_SOCKET_ERROR) {
         //       HTTPD_DEBUG(1, "request from (%p): %d.%d.%d.%d; port: %d.\n", session, (sin.sin_addr.s_addr >> 24) & 0xff, (sin.sin_addr.s_addr >> 16) & 0xff, (sin.sin_addr.s_addr >> 8) & 0xff, sin.sin_addr.s_addr & 0xff, sin.sin_port);
//#ifdef HTTP_PRINT
// printfp("request from...");
//#endif
   #ifdef HTTP_PRINT
// printfp("...st r");
   printfpd(" valid : %d ", session->valid);
   printfpd(" ses_init check : %d ", check++);

    #endif


                httpd_ses_init(server, session, s);    //t
            }
        }
// return; //t120406

   //     while(0){     //t120409
   check1 = 0;
        while (HTTPD_SESSION_VALID == session->valid) 
    //   if(HTTPD_SESSION_VALID == session->valid)     //1
        {
        
        
//        #ifdef HTTP_PRINT
// printfp("...st r");
//many prints   printfpd("process_check1:%d ", check1++);

//     #endif

 //    if(check1 > 100)    //15sec
       if(check1 > 40)  
     { 
   
    //   httpd_ses_close(session);
   
        break;   
     }
//#ifdef HTTP_PRINT
// printfp("HTTPD_SESSION_VALID == ...");
//#endif
        
            httpd_ses_process(server, session);
//#ifdef HTTP_PRINT
// printfpd("\n\r %X", server->params->address);
//#endif

        //not    httpd_ses_close(session);   //my
            
      //   return; //t   
            if (HTTPDCFG_POLL_MODE)
            { 
               break;
            }
            
          _time_delay_ticks((_mqx_uint)1);    //t
      
                
      }
      //141223   _time_delay_ticks((_mqx_uint)1);    //t
 
    } while (!HTTPDCFG_POLL_MODE);
    
   // _time_delay_ticks((_mqx_uint)1);
    
}
