/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2005, All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_sock.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions for dealing with "sockets" interface.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 10/09/07  JRB    32     Del sockLogMaskMapCtrl (if needed, must link	*/
/*			   to gensock2.c).				*/
/* 05/23/05  EJV    31     Add sockLogMaskMapCtrl for parsing logcfg.xml*/
/* 05/09/05  EJV    30     Use rem_port if configured			*/
/* 05/05/05  EJV    29     Use rfc1006_listen_port if configured	*/
/* 01/24/05  JRB    28     Move "*_logstr" to lean_var.c		*/
/* 01/14/04  EJV    27     np_data_req: added eot param.		*/
/*			   Added np_get_tx_queue_cnt func.		*/
/* 10/23/03  JRB    26     Move clnp_debug_sel to slog.c, slogl.c.	*/
/* 07/30/03  EJV    25     Added encrypt_ctrl to np_connect_req.	*/
/* 06/19/03  JRB    24     Del unused "*_logstr".			*/
/* 07/16/02  JRB    23     Del unused local vars.			*/
/* 06/20/02  JRB    22     Pass ip addr as ulong to np_connect_req.	*/
/* 05/06/02  JRB    21     np_connect_req: pass ip_addr to Read task or	*/
/*			   thread and call "blocking" connect from there.*/
/* 02/25/02  MDE    20     Now get max PDU size from mvl_cfg_info	*/
/* 06/18/01  JRB    19     Set RCVTIMEO socket option to "infinite".	*/
/* 03/15/01  JRB    18     Use new SMEM allocation functions.		*/
/* 08/07/00  MDE    17     Changed 'connect' return error check	again	*/
/* 08/01/00  MDE    16     Changed 'connect' return error check		*/
/* 07/25/00  JRB    15     Close socket ONLY from sock_info_free.	*/
/* 05/18/00  JRB    14     Only post "listen" if called conns supported.*/
/*			   Fix np_connect_req to closesocket on error.	*/
/*			   Do NOT kill read tasks: they will exit.	*/
/*			   Use new ioctlsocket macro. Lint cleanup.	*/
/* 09/13/99  MDE    13     Added SD_CONST modifiers			*/
/* 01/26/99  JRB    12     Init clnp_debug_sel in np_init.		*/
/* 10/22/98  JRB    11     Chg LOG_ALWAYS macros to LOG_REQ.		*/
/* 10/08/98  MDE    10     Minor logging cleanup			*/
/* 08/13/98  JRB    09     Lint cleanup.				*/
/* 03/11/98  JRB    08     setsockopt NODELAY always.			*/
/*			   setsockopt KEEPALIVE if tp0_cfg.keepalive.	*/
/* 02/16/98  JRB    07     Fix gethostbyname #if. Cast setsockopt arg.	*/
/* 12/08/97  JRB    06     Added __OS2__ support.			*/
/* 12/08/97  JRB    05     Skip FD_SETSIZE check. Can't happen.		*/
/* 11/24/97  JRB    04     Set socket option REUSEADDR.			*/
/*			   Call shutdown before closing socket.		*/
/* 11/24/97  JRB    03     Added VXWORKS support.			*/
/* 09/23/97  JRB    02     np_init call sockets_end on error.		*/
/* 08/01/97  JRB    01     Created (compatible with MMS-EASE 7.0).	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4.h"		/* Internal definitions for tp4	*/
#include "tp4_log.h" 
#include "clnp_log.h"

#ifdef MMS_LITE
#include "mvl_acse.h"		/* Need "mvl_num_called".	*/
#endif

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
SOCKET	hSockListen;			/* listen socket handle		*/

/************************************************************************/
/*			np_init						*/
/* 1. Perform platform-specific sockets init using "sockets_init".	*/
/* 2. Create listening socket in "blocking" mode.			*/
/* 3. Spawn listening task/thread.					*/
/************************************************************************/
ST_RET np_init (ST_INT max_num_conns)
  {
SOCKADDR_IN LocalAddr;		/* local TCP port and IP addr		*/
int sockopt_on = 1;	/* value for turning on boolean socket option	*/
ST_BOOLEAN do_listen;		/* Flag to tell if we need a "listen".	*/
#if defined (MMS_LITE)
  if (mvl_cfg_info->num_called)
    do_listen = SD_TRUE;	/* Called conns supported so must do listen.*/
  else
    do_listen = SD_FALSE;
#else
  /* DEBUG: is there a way to know if listen needed when (!MMS_LITE)?	*/
  do_listen = SD_TRUE;		/* Assume listen is necessary.	*/
#endif

#if defined(DEBUG_SISCO)
  clnp_debug_sel |= CLNP_LOG_ERR;
#endif

  /* Perform platform-specific sockets initialization.	*/
  if (sockets_init (max_num_conns))
    goto NP_INIT_ERROR;

  /* Create "listen" socket & task (only if called connections supported).*/
  if (do_listen)
    {
    /* Create a TCP socket		*/
    hSockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSockListen == INVALID_SOCKET)
      {
      CLNP_LOG_ERR0 ("Can't create socket");
      goto NP_INIT_ERROR;
      }

    if (setsockopt (hSockListen, IPPROTO_TCP, TCP_NODELAY,
                    (ST_CHAR *) &sockopt_on, sizeof (int) ) )
      {
      CLNP_LOG_ERR1 ("Error %d setting NODELAY socket option", WSAGetLastError());
      }

    if (tp0_cfg.keepalive  && 
        setsockopt (hSockListen, SOL_SOCKET, SO_KEEPALIVE,
                    (ST_CHAR *) &sockopt_on, sizeof (int) ) )
      {
      CLNP_LOG_ERR1 ("Error %d setting KEEPALIVE socket option", WSAGetLastError());
      }

    if (setsockopt (hSockListen, SOL_SOCKET, SO_REUSEADDR, (ST_CHAR *) &sockopt_on,
                    sizeof(int) ) )
      {
      CLNP_LOG_ERR1 ("Error %d setting REUSEADDR socket option", WSAGetLastError());
      goto NP_INIT_ERROR;
      }

    /* Bind socket to the RFC1006 port	*/
    LocalAddr.sin_family = AF_INET;
    if (tp0_cfg.rfc1006_listen_port)
      LocalAddr.sin_port = htons(tp0_cfg.rfc1006_listen_port);
    else
      LocalAddr.sin_port = htons(IPPORT_RFC1006);
    /* Use local address configured in Operating System.	*/
    LocalAddr.sin_addr.s_addr = htonl (INADDR_ANY);

    if (bind(hSockListen, (PSOCKADDR) &LocalAddr, sizeof(LocalAddr)) < 0)
      {
      CLNP_LOG_ERR1 ("Error %d binding to RFC1006 port", WSAGetLastError());
      goto NP_INIT_ERROR;
      }

    /* Listen for incoming connections.	*/
    if (listen(hSockListen, SOMAXCONN) < 0)
      {
      CLNP_LOG_ERR1 ("Error %d listening on RFC1006 port", WSAGetLastError());
      goto NP_INIT_ERROR;
      }

    /* Spawn Task/Thread to "listen" for incoming socket connections.	*/
    if (spawn_listen (hSockListen) != SD_SUCCESS)
      {
      CLNP_LOG_ERR0 ("ERROR spawning Socket Listen Task/Thread");
      goto NP_INIT_ERROR;
      }
    }	/* end "if (tp0_cfg.num_called)"	*/

  CLNP_LOG_REQ0 ("np_init complete");
  return (SD_SUCCESS);

NP_INIT_ERROR:		/* Init error. Do cleanup before return.	*/
  sockets_end ();
  return (SD_FAILURE);
  }
/************************************************************************/
/*			np_end						*/
/* This function will terminate the operation of the Network layer	*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if termination successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET np_end (ST_VOID)
  {
  sockets_end ();
  CLNP_LOG_REQ0 ("np_end complete");
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			handle_accepted_conn				*/
/************************************************************************/
ST_VOID handle_accepted_conn (SOCK_INFO *sock_info)
  {
  /* User only cares about T-CONNECT.ind.  Just log this.		*/
  CLNP_LOG_IND1 ("N-CONNECT.ind:  sock_info = 0x%X", sock_info);
  sock_info->state = SOCK_STATE_CONNECTED;
  }
/************************************************************************/
/*			handle_connect_success				*/
/************************************************************************/
ST_VOID handle_connect_success (SOCK_INFO *sock_info)
  {
TP_LOG_REQ0 ("connect async success");

  /* Pass up N-CONNECT.cnf+ to user.	*/          
  CLNP_LOG_IND1 ("N-CONNECT.cnf+ (pos): user_conn=%d", sock_info->user_conn_id);
  np_connect_cnf_pos (sock_info, sock_info->user_conn_id);
  sock_info->state = SOCK_STATE_CONNECTED;
  }
/************************************************************************/
/*			handle_connect_fail				*/
/************************************************************************/
ST_VOID handle_connect_fail (SOCK_INFO *sock_info)
  {
TP_LOG_REQ0 ("connect async fail");

  /* Pass up N-DISCONNECT.ind to user.	*/          
  CLNP_LOG_IND1 ("N-DISCONNECT.ind: user_conn=%d", sock_info->user_conn_id);
  np_disconnect_ind (sock_info);
  sock_info_free (sock_info);
  }
/************************************************************************/
/*			handle_data					*/
/* Parameters:								*/
/* 	sock_info	ptr to socket tracking struct			*/
/*	tpkt_ptr	ptr to RFC1006 TPKT.				*/
/*	tpkt_len	len of RFC1006 TPKT.				*/
/************************************************************************/
ST_VOID handle_data (SOCK_INFO *sock_info, ST_UCHAR *tpkt_ptr, ST_UINT16 tpkt_len)
  {
  CLNP_LOG_IND3 ("N-DATA.ind:  sock_info = 0x%X, user_conn = %d tpkt_len = %d",
     sock_info, sock_info->user_conn_id, tpkt_len);
  CLNP_LOGH_IND (tpkt_len, tpkt_ptr);
  np_data_ind (sock_info, tpkt_ptr, tpkt_len);
  }
/************************************************************************/
/*			handle_disconnect				*/
/************************************************************************/
ST_VOID handle_disconnect (SOCK_INFO *sock_info)
  {
  CLNP_LOG_IND2 ("N-DISCONNECT.ind:  sock_info = 0x%X, user_conn = %d",
          sock_info, sock_info->user_conn_id);
  np_disconnect_ind (sock_info);
  sock_info_free (sock_info);
  }
/************************************************************************/
/*			np_connect_req					*/
/* RETURNS:								*/
/*	SOCK_INFO *	ptr to socket info for new socket,  OR		*/
/*			NULL if connect fails immediately.		*/
/************************************************************************/
SOCK_INFO *np_connect_req (ST_LONG user_conn_id, ST_ULONG ipAddr, ST_UINT16 rem_port,
                           S_SEC_ENCRYPT_CTRL *encrypt_ctrl)
  {
SOCKET hSock;
int sockopt_on = 1;	/* value for turning on boolean socket option	*/
SOCK_INFO *sock_info;		/* new tracking struct for this conn.*/
ST_INT status = SD_SUCCESS;	/* Change status if "anything" fails.	*/
struct timeval TimeOut;

  CLNP_LOG_REQ1 ("N-CONNECT.req:        user_conn=%d", user_conn_id);

/* create a TCP socket	*/
  hSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (hSock == INVALID_SOCKET)
    {
    CLNP_LOG_ERR0 ("N-CONNECT.req ERROR: Can't create socket");
    return (NULL);
    }

  /* Receive timeout should be infinite (i.e. set it to 0).		*/
  TimeOut.tv_sec = TimeOut.tv_usec = 0;
  if (setsockopt (hSock, SOL_SOCKET, SO_RCVTIMEO,
                  (ST_CHAR *) &TimeOut, sizeof (TimeOut) ) )
    {
    CLNP_LOG_ERR1 ("N-CONNECT.req ERROR %d setting RCVTIMEO socket option", WSAGetLastError());
    /* This may not be critical, so go ahead and continue.		*/
    }

  sock_info = sock_info_alloc (hSock, SOCK_STATE_CONNECTING, user_conn_id);

  /* Pass IP addr as long in "network byte order".	*/
  sock_info->ip_addr = ipAddr;
  sock_info->port = rem_port;	

  if (setsockopt (hSock, IPPROTO_TCP, TCP_NODELAY,
                  (ST_CHAR *) &sockopt_on, sizeof (int) ) )
    {
    CLNP_LOG_ERR1 ("N-CONNECT.req ERROR %d setting NODELAY socket option", WSAGetLastError());
    }

  if (tp0_cfg.keepalive  && 
      setsockopt (hSock, SOL_SOCKET, SO_KEEPALIVE,
                  (ST_CHAR *) &sockopt_on, sizeof (int) ) )
    {
    CLNP_LOG_ERR1 ("N-CONNECT.req ERROR %d setting KEEPALIVE socket option", WSAGetLastError());
    }

  /* Spawn Read Task/Thread. It will first do a blocking "connect" call.*/
  /* sock_info->state = SOCK_STATE_CONNECTING.				*/
  if (status == SD_SUCCESS)
    {
    if (spawn_read (sock_info) != SD_SUCCESS)
      {
      CLNP_LOG_ERR0 ("N-CONNECT.req ERROR: spawning Read");
      status = SD_FAILURE;
      }
    }	/* if (status == SD_SUCCESS)	*/

  if (status != SD_SUCCESS)
    {				/* Something failed. Cleanup.	*/
    sock_info_free (sock_info);
    sock_info = NULL;		/* return NULL to caller.	*/
    }

  return (sock_info);
  }
/************************************************************************/
/* np_data_req								*/
/* RETURN CODES:							*/
/*	SD_SUCCESS or SD_FAILURE					*/
/************************************************************************/
ST_RET np_data_req (SOCK_INFO *sock_info, ST_INT tpkt_len, ST_UCHAR *tpkt_ptr,
                    ST_BOOLEAN eot)
  {
ST_INT nSend;

  CLNP_LOG_REQ3 ("N-DATA.req:  sock_info = 0x%X, user_conn = %d tpkt_len = %d",
     sock_info, sock_info->user_conn_id, tpkt_len);
  CLNP_LOGH_REQ (tpkt_len, tpkt_ptr);

  /* This is a "blocking" send.			*/
  nSend = send(sock_info->hSock, (ST_CHAR *) tpkt_ptr, tpkt_len, 0);
  if (nSend < 0)
    {
    CLNP_LOG_ERR1 ("Error %d sending data", WSAGetLastError());
    return (SD_FAILURE);
    }
  if (nSend != tpkt_len)
    {
    CLNP_LOG_ERR0 ("ERROR: Incomplete packet sent.");
    return (SD_FAILURE);
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			np_get_tx_queue_cnt				*/
/* Returns number of queued SPDUs. 					*/
/************************************************************************/
ST_UINT np_get_tx_queue_cnt (SOCK_INFO *sock_info)
  {
  return (0);	/* not queueing */
  }

/************************************************************************/
/*			np_disconnect_req				*/
/* This function is called by the user to break connection		*/
/************************************************************************/

ST_RET np_disconnect_req (SOCK_INFO *sock_info)
  {
  CLNP_LOG_REQ2 ("N-DISCONNECT.req:  sock_info = 0x%X, user_conn = %d",
     sock_info, sock_info->user_conn_id);
  sock_info->state = SOCK_STATE_DISCONNECTING;
  if (shutdown (sock_info->hSock, 2))
    {
    CLNP_LOG_ERR1 ("Error %d shutting down socket", WSAGetLastError());
    return (SD_FAILURE);
    }
  /* NOTE: do not free "sock_info" until signal from ThreadSockRead	*/
  /*       that disconnect is done.					*/
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			sock_info_alloc					*/
/* Allocate a SOCK_INFO struct and fill it in.				*/
/************************************************************************/
SOCK_INFO *sock_info_alloc (SOCKET hSock, ST_INT state, ST_LONG user_conn_id)
  {
SOCK_INFO *sock_info;

  /* Allocate SOCK_INFO struct.						*/
  sock_info = (SOCK_INFO *) M_MALLOC (MSMEM_SOCK_INFO, sizeof (SOCK_INFO));
  /* Fill in SOCK_INFO struct.						*/
  sock_info->hSock = hSock;
  sock_info->state = state;
  sock_info->user_conn_id = user_conn_id;
  return (sock_info);
  }
/************************************************************************/
/*			sock_info_free					*/
/* Free a SOCK_INFO struct.						*/
/************************************************************************/
ST_VOID sock_info_free (SOCK_INFO *sock_info)
  {
  /* Close socket here and ONLY here.					*/
  if (closesocket (sock_info->hSock) < 0)
    {
    CLNP_LOG_ERR1 ("Error %d closing socket", WSAGetLastError());
    }
  M_FREE (MSMEM_SOCK_INFO, sock_info);
  }
