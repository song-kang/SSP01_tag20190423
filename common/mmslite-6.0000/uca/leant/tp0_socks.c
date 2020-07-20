/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2003-2010, All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_socks.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions based on "gensock2" interface (gensock2.c).	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 10/17/11  JRB	   Save (GEN_SOCK *) in np_connect_req so we can*/
/*			   always close socket in np_disconnect_req.	*/
/*			   Eliminate SOCK_STATE_CONNECT_CANCELLED state.*/
/* 03/29/11  JRB	   np_init: clear sockCtx so restart works.	*/
/* 08/02/10  JRB	   Chg sockCtx from (GEN_SOCK_CTXT *) to	*/
/*			   (GEN_SOCK_CTXT) so don't need to alloc/free.	*/
/*			   Fix eventType range check.			*/
/* 06/22/10  JRB           Move hTcpEvent to lean_var.c so always avail	*/
/*			   Del obsolete pipe_to_main.			*/
/* 04/27/10  JRB	   Set new uSockRxBufFree funct ptr.		*/
/* 03/01/10  JRB    41     Delete all security code,			*/
/*			   now use "tp0_sock_snap.c" for security.	*/
/* 02/03/10  NAV    40     Open VMS port				*/
/* 11/17/09  JRB    39     Del MMSEASE_MOSI code.			*/
/*			   Moved TP0_SOCK_CTX define to header file.	*/
/*			   Put ifdef S_SEC_ENABLED around security code.*/
/*			   _setSecInfo: chg sSecGet.. to sslGetCipherSuite*/
/* 10/10/08  JRB    38     _uSockHunt: make sure len is legal.		*/
/*			   Ignore empty msg (len=RFC1006_HEAD_LEN).	*/
/* 02/21/08  EJV    37     _uSockHunt: corr lenOut for 0x8000-0xFFFF len*/
/* 09/10/07  MDE    36     Updated to use new SOCK_LOG_ macros		*/
/* 02/16/07  JRB    35     Use new sockEventPut, sockEventGet functions	*/
/*			   and new "uSockConnectInd/Conf" funct ptrs.	*/
/* 01/30/07  JRB    34     readyToFree flag was deleted so don't use it.*/
/* 01/15/07  JRB    33     Del poll_mode arg to sockStart.		*/
/* 01/15/07  EJV    32	   Rpl TP0_SOCKS_MUTEX_* with S_LOCK_UTIL_RESOURCES */
/*			     due to mutex changes in gensock2.c.	*/
/*			   TP0_SOCK_CTX: rem listsMutex create/destroy.	*/
/* 12/11/06 EJV     31	   rfc1006_listener_connect: chg nonblock_on	*/
/*			     from ST_LONG to int.			*/
/*			     Needed on 64-bit UNIX, Linux systems.	*/
/* 11/29/06  EJV    30	   recvwait_fd: chg __hpux to use the SCM_RIGHTS*/
/* 11/17/06 EJV     29	   socket funs: added (int) cast, the HP-UX	*/
/*			     ssize_t is long.				*/
/*			   recvwait_fd: impl timeout for recvmsg loop	*/
/* 11/07/06  EJV    28     MMSEASE_MOSI:init tp0Ctx.tcpEventWakeupPort=0*/
/* 07/10/06  EJV    27     MMSEASE_MOSI: finished GEN_SOCK_CTXT work.	*/
/*			   Use sysincs.h for system includes.		*/
/*			     Changed sun to s_un (err on Sun Solaris)	*/
/* 01/30/06  GLB    26     Integrated porting changes for VMS           */
/* 12/15/05  EJV    24     _processDisconnectInd: call list_find_node	*/
/* 12/07/05  EJV    23     _processDisconnectInd: unlink if on the list	*/
/*			   _processConnectDone: moved secEnable code to	*/
/*			     case for SOCK_STATE_CONNECTED state.	*/
/* 10/04/05  EJV    22     Implemented GEN_SOCK_CTXT and other changes:	*/
/*			   Renamed orginal SOCK_CTX to TP0_SOCK_CTX,	*/
/*			     sockCtx to tp0Ctx.				*/
/*			   _uSockDisconnectInd: mk errptr more readable	*/
/*			   _addSockInd: chg to ST_RET from ST_VOID	*/
/*			   _sockClose: moved code to _sockCloseAllListen*/
/*			   np_end: added new call to _sockCloseAllListen*/
/*			   Rpl GSOCK_MUTEX_* with TP0_SOCKS_MUTEX_*.	*/
/*			   Folded some glb vars into the TP0_SOCK_CTX.	*/
/*			   Reversed Rev 21 change, not needed anymore.	*/
/* 08/03/05  EJV    20     Reworked rekeyTime to be per connection.	*/
/*			   Use sockCtx glb (instead saving it in usr1).	*/
/* 07/19/05  EJV    19     _processConnectInd: set usr2 back to NULL.	*/
/*			   _processConnectDone: if secEnable don't free	*/
/*			   sock_info. Check state.			*/
/* 			   np_disconnect_req:check other states, add log*/
/* 07/01/05  EJV    18     _uSockWritable CORR: removed state chg.	*/
/* 06/24/05  EJV    17     Added code to prevent queues buildup.	*/
/* 05/02/05  JRB    16     Add RFC1006_LISTENER task code (MMSEASE_MOSI)*/
/*           EJV           Added _sockSetDefaults.			*/
/* 05/05/05  EJV    15     Use rfc1006_listen_port if configured	*/
/* 04/21/05  JRB    14     Fix logging of sin_port (use ntohs).		*/
/*			   Del unused vars.				*/
/* 04/13/05  EJV    13     Corr EADDRINUSE to SOCK_EADDRINUSE		*/
/* 03/22/05  EJV    12     LINUX (MMSEASE_MOSI): implemented events.	*/
/*			   _sockInitListen: allow to SO_REUSEADDR.	*/
/*			   Added indTypeStr. Added/chg logging.		*/
/* 03/16/05  JRB    11     _uSockHunt: allow any val for 2nd byte &	*/
/*			   ret ..HUNT_DISCONNECT on err to cause disconn*/
/*			   For speed, set ptr to strings instead of strcpy.*/
/*			   Use RFC1006 defines from tp0_sock.h.		*/
/* 03/07/05  EJV    10     Fixed queuing bug where connectInd sometimes	*/
/*			   processed AFTER disconnectInd by moving	*/
/*			   sec cleanup from _uSockDisconnectInd to	*/
/*			   _processDisconnectInd.			*/
/*			   Moved sec code from _uSockConnectDone to 	*/
/*			   _processConnectDone,for consistency w/con ind*/
/*			   Use secEnable (in place of other sec fields)	*/
/* 02/19/04  EJV    09     np_end: chg sleep from 1000 to 100 ms	*/
/* 02/18/04  JRB    08     _uSockDisconnectInd: DON'T chk state, only	*/
/*			   safe from main thread (_processDisconnectInd)*/
/*			   np_end: allow disconnects to finish.		*/
/* 02/06/04  JRB    07     np_disconnect_req:Chg assert to log & ret err*/
/* 01/28/04  EJV    06     _handleRekeying:chk if connected before rekey*/
/* 01/23/04  EJV    05     np_data_req: sockTxMsg will free sockData.	*/
/* 01/21/04  JRB    04     Do most disconnect processing in callback	*/
/*			   _uSockDisconnectInd.				*/
/*	     EJV           Change test for rekey to '>='		*/
/* 01/14/04  EJV    03     np_data_req: added eot param.		*/
/*			     Free buff if sockTxMsg fails.		*/
/*			   Added np_get_tx_queue_cnt func.		*/
/* 10/16/03  JRB    02     Port to LINUX. Chg SOCKET to (GEN_SOCK *).	*/
/*			   Compare (GEN_SOCK *) to NULL, not INVALID_...*/
/* 07/29/03  EJV    01     New. Replacement for tp0_sock.c using	*/
/*			   gensock2 to interface to sockets.		*/
/*			   Used tp0_sock2.c and snapmain.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mem_chk.h"
#include "ssec.h"
#include "ssec_int.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4.h"		/* Internal definitions for tp4	*/
#include "tp4_log.h"
#include "tp0_sock.h"
#include "sock_log.h"

#ifdef MMS_LITE
#include "mvl_acse.h"		/* Need "mvl_num_called".	*/
#endif

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* This many messages can accumulate in the receiving ind queue 	*/
/* for given socket before we stop receiving, start receiving after	*/
/* we reach low mark.							*/
#define SOCK_RX_QUE_HIGH_MARK	10		/* >= stop  receiving	*/
#define SOCK_RX_QUE_LOW_MARK	 1		/* <= start receiving	*/

ST_CHAR *eventTypeStr[5] =
  {
  "UNKNOWN",
  "CONNECT_IND",
  "CONNECT_CONF",
  "DISCONNECT",
  "RXDATA"
  };
#define NUM_EVENT_TYPES		(sizeof(eventTypeStr)/sizeof(ST_CHAR *))

/* Similar to sock_info_alloc but uses GEN_SOCK.	*/
SOCK_INFO *sock_info_alloc2 (GEN_SOCK *genSock, ST_INT state, ST_VOID *user_conn_id);

/* Functions to process indications/events.	*/
static ST_VOID _processConnectInd (GEN_SOCK *pSock);
static ST_VOID _processConnectDone (GEN_SOCK *pSock);
static ST_VOID _processDisconnectInd (GEN_SOCK *pSock);
static ST_VOID _processRxInd (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData);

/* gensock2 callback functions	*/
static ST_RET  _uSockConnectDone (GEN_SOCK *pSock);
static ST_RET  _uSockConnectInd (GEN_SOCK *pSock);
static ST_VOID _uSockDisconnectInd (GEN_SOCK *pSock);
static ST_VOID _uSockRxInd (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData);
static ST_VOID _uSockWritable (GEN_SOCK *pSock);
static ST_VOID _uSockHunt (GEN_SOCK *pSock, ST_INT *huntStateIo,
                     ST_CHAR *buf, ST_INT bufCount, ST_INT *lenOut);
static ST_VOID _uSockDataAlloc (GEN_SOCK *pSock, ST_INT dataLen,
			  GEN_SOCK_DATA **sockDataOut);
static ST_VOID _uSockDataFree (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData);

/* misc functions */
static ST_RET  _addSockInd (GEN_SOCK *pSock, ST_INT indType,
			    GEN_SOCK_DATA *sockData);

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
/* listening socket types */
#define	SOCK_LISTEN_NON_SSL	1
#define	SOCK_LISTEN_SSL		2

/* Ctx state */
#define TP0_SOCK_CTX_STATE_IDLE			0
#define TP0_SOCK_CTX_STATE_ACTIVE		1
#define TP0_SOCK_CTX_STATE_TERMINATING		2

TP0_SOCK_CTX	tp0Ctx;


static ST_RET _sockSetDefaults (GEN_SOCK_CONFIG *sockCfg);
static ST_RET _sockInitListen (ST_UINT type, ST_UINT idx, ST_INT max_num_conns);
static ST_RET _sockCloseAllListen (ST_VOID);
static ST_RET _sockClean (ST_VOID);

	/*----------------------------------------------*/
	/*	Initialization / Cleanup		*/
	/*----------------------------------------------*/

/************************************************************************/
/*			np_init						*/
/* NOTE: max_num_conns arg used only by listen sockets.			*/
/************************************************************************/
ST_RET np_init (ST_INT max_num_conns)
  {
ST_RET rc;

  /* Start with a clean context structure. Critical to avoid old data	*/
  /* when restarting.							*/
  memset (&tp0Ctx.sockCtx, 0, sizeof (GEN_SOCK_CTXT));

  if (tp0Ctx.state != TP0_SOCK_CTX_STATE_IDLE)
    {
    SOCK_LOG_NERR0 (&tp0Ctx.sockCtx, "np_init: TP0 Context State not IDLE.");
    return (SD_FAILURE);
    }

  SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "np_init: initializing Network layer ... TCP max_num_conns = %d.", max_num_conns);

  /* Create "TCP" Event Semaphore.	*/
#if defined(_WIN32)
  if (!(hTcpEvent = gs_get_event_sem (SD_FALSE)))
    {
    SOCK_LOG_ERR0 (&tp0Ctx.sockCtx, "np_init: error creating TCP Event Semaphore.");
    return (SD_FAILURE);
    }
#endif

  /* initialize gensock2 before calling any of the socket functions */
  rc = sockStart ("TP0_SOCKS", &tp0Ctx.sockCtx);
  if (rc != SD_SUCCESS)
    {
    SOCK_LOG_ERR1 (&tp0Ctx.sockCtx, "np_init: sockStart() failed, rc=%d", rc);
#if defined(_WIN32)
    gs_free_event_sem (hTcpEvent);
#endif
    return (rc);
    }

  if (mvl_cfg_info->num_called)
    {
    /* start listening for non-secured connections from remotes		*/
      {
      rc = _sockInitListen (SOCK_LISTEN_NON_SSL, 0, max_num_conns);
      if (rc != SD_SUCCESS)
        {
        _sockClean ();
        return (rc);
        }
      }
    }

  if (rc == SD_SUCCESS)
    {
    tp0Ctx.state = TP0_SOCK_CTX_STATE_ACTIVE;
    SOCK_LOG_FLOW0 (&tp0Ctx.sockCtx, "np_init: initialization successful.");
    }

  return (rc);
  }

/************************************************************************/
/*			_sockSetDefaults				*/
/*----------------------------------------------------------------------*/
/* Initialize socket cfg params (used for secure/non-secure connections)*/
/************************************************************************/
static ST_RET _sockSetDefaults (GEN_SOCK_CONFIG *sockCfg)

{
ST_RET rc = SD_SUCCESS;

  SOCK_LOG_FLOW0 (&tp0Ctx.sockCtx, "in _sockSetDefaults()");

  memset (sockCfg, 0, sizeof (GEN_SOCK_CONFIG));	/* start clean	*/

  /* set socket parameters */

  sockCfg->hdrSize     = RFC1006_HEAD_LEN;	/* RFC1006 header len	*/

  /* set sockopt parameters */
  sockCfg->setSockOpts = SD_TRUE;
  sockCfg->noDelay     = SD_TRUE;
  sockCfg->keepAlive   = SD_TRUE;
  sockCfg->reuseAddr   = SD_TRUE;  	/* SO_REUSEADDR for quick restart */
  sockCfg->rcvBufSize  = 0;		/* use default socket buffer size */
  sockCfg->sndBufSize  = 0;		/* use default socket buffer size */

  sockCfg->pauseRecv   = SD_FALSE;	/* used to apply back pressure	  */
  sockCfg->listenBacklog = 0;		/* if 0,  SOMAXCONN used	  */

  /* set callback funcs for non-secured connections from remotes	*/

  sockCfg->uSockConnectInd = &_uSockConnectInd;
  sockCfg->uSockConnectConf= &_uSockConnectDone;
  sockCfg->uSockDisconnect = &_uSockDisconnectInd;
  sockCfg->uSockRx         = &_uSockRxInd;
  sockCfg->uSockWritable   = &_uSockWritable;
  sockCfg->uSockHunt       = &_uSockHunt;
  sockCfg->uSockRxBufAlloc = &_uSockDataAlloc;
  sockCfg->uSockRxBufFree  = &_uSockDataFree;	/* same funct for Rx,Tx	*/
  sockCfg->uSockTxBufFree  = &_uSockDataFree;

  /*  sockCfg->recvEvent    = not used				*/
  /*  sockCfg->usr1         = not used				*/

  return (rc);
}

/************************************************************************/
/*			_sockInitListen					*/
/*----------------------------------------------------------------------*/
/* Initialize listen socket. 						*/
/************************************************************************/
static ST_RET _sockInitListen (ST_UINT    type,
			       ST_UINT    idx,
			       ST_INT     max_num_conns)

{
ST_RET          rc;
GEN_SOCK_CONFIG sockCfg;
ST_UINT16       listenPort;
ST_UINT         maxActive;
GEN_SOCK      **pListenSock;

  SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: in _sockInitListen()", tp0Ctx.sockCtx.ctxName);

  /* set socket parameters */

  rc = _sockSetDefaults (&sockCfg);

    {
    /* SOCK_LISTEN_NON_SSL */
    if (tp0_cfg.rfc1006_listen_port)
      listenPort  =  tp0_cfg.rfc1006_listen_port;
    else
      listenPort  =  IPPORT_RFC1006;		/* use the default port */
    tp0Ctx.rfc1006Port = listenPort;	/* save the port used	*/
    maxActive   =  max_num_conns;
    pListenSock = &tp0Ctx.remListenSock;
    }

  /* init the listening socket */
  rc = sockInitListen (&tp0Ctx.sockCtx, &sockCfg, listenPort, maxActive, pListenSock);
  if (rc != SD_SUCCESS)
    {
    SOCK_LOG_ERR4 (&tp0Ctx.sockCtx, "%s: sockInitListen() failed for port=%u, rc=%d %s",
                   tp0Ctx.sockCtx.ctxName, (ST_UINT) listenPort, rc,
                   (rc== SOCK_EADDRINUSE) ? "(port already used)" : "");
    *pListenSock = NULL;	/* need to be set if bind fails	*/
    return (SD_FAILURE);
    }

  SOCK_LOG_FLOW2 (&tp0Ctx.sockCtx, "%s: listening on port=%u", (*pListenSock)->sockIdStr, (ST_UINT) listenPort);

  return (SD_SUCCESS);
}

/************************************************************************/
/*			_sockCloseAllListen				*/
/* Closes all listen sockets. Currently the only return is SD_SUCCESS.	*/
/************************************************************************/
static ST_RET _sockCloseAllListen (ST_VOID)
{
ST_INT i;

  /* Close all listen sockets to avoid handling incoming connections */
  if (tp0Ctx.secCfg)
    for (i=0; i<tp0Ctx.secCfg->numSslListenPorts; ++i)
      if (tp0Ctx.remListenSockSSL[i])
        {
        SOCK_LOG_FLOW2 (&tp0Ctx.sockCtx, "%s: closing secure listening port=%d",
          tp0Ctx.remListenSockSSL[i]->sockIdStr, tp0Ctx.secCfg->sslListenPorts[i]);
        sockClose (tp0Ctx.remListenSockSSL[i]);
	}

  if (tp0Ctx.remListenSock)
    {
    SOCK_LOG_FLOW2 (&tp0Ctx.sockCtx, "%s: closing listening port=%d",
                    tp0Ctx.remListenSock->sockIdStr, tp0Ctx.rfc1006Port);
    sockClose (tp0Ctx.remListenSock);
    }

  return SD_SUCCESS;
}

/************************************************************************/
/*			_sockClean					*/
/************************************************************************/
static ST_RET _sockClean (ST_VOID)
{
ST_RET rc;

  SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: in _sockClean()", tp0Ctx.sockCtx.ctxName);

  /* CRITICAL: release resources in reverse order.	*/

  /* close all listen sockets to avoid handling incoming connections */
  _sockCloseAllListen ();

  /* cleanup gensock2 resources, terminate threads,... */
  rc = sockEnd (&tp0Ctx.sockCtx);

  /* free events and mutexes */
#if defined(_WIN32)
  gs_free_event_sem (hTcpEvent);
#endif

  return (SD_SUCCESS);
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
ST_RET rc;
GEN_SOCK_EVENT *sdi;

  SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: in np_end()", tp0Ctx.sockCtx.ctxName);

  tp0Ctx.state = TP0_SOCK_CTX_STATE_TERMINATING;

  /* close all listen sockets to avoid handling incoming connections */
  _sockCloseAllListen ();

  /* Finish processing of disconnect events. Ignore all other events.	*/
  sMsSleep (100);	/* let other threads finish disconnects		*/

  /* Get all events off list and process them.	*/
  while ((sdi = sockEventGet (&tp0Ctx.sockCtx)) != NULL)
    {
    if (sdi->eventType == GS_EVENT_DISCONNECT)
      {
      SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: np_end: processing Event Indication IndType=DISCONNECT.",
                      sdi->pSock->sockIdStr);
      _processDisconnectInd (sdi->pSock);
      }
    else if (sdi->eventType == GS_EVENT_CONNECT_IND)
      {
      SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: np_end: IGNORING Event Indication IndType=CONNECT (closing socket).",
                      sdi->pSock->sockIdStr);
      sockClose (sdi->pSock);
      }
    else if (sdi->eventType == GS_EVENT_DATA_IND)
      {
      SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: np_end: IGNORING Event Indication IndType=RXDATA (closing socket).",
                      sdi->pSock->sockIdStr);
      _uSockDataFree (sdi->pSock, sdi->sockData);
       }
    else
      SOCK_LOG_FLOW3 (&tp0Ctx.sockCtx, "%s: np_end: IGNORING Event Indication IndType=%d (%s).",
                      sdi->pSock->sockIdStr, sdi->eventType,
      (sdi->eventType < NUM_EVENT_TYPES) ? eventTypeStr[sdi->eventType] : "UNKNOWN");

    chk_free (sdi);
    }	/* end loop processing all events	*/

  rc = _sockClean ();

  tp0Ctx.state = TP0_SOCK_CTX_STATE_IDLE;
  return (rc);
  }

	/*----------------------------------------------*/
	/*	Event/Indication Handling		*/
	/*----------------------------------------------*/

/************************************************************************/
/*			np_event					*/
/* This function is called to process network events.			*/
/************************************************************************/
ST_BOOLEAN np_event (ST_VOID)
  {
GEN_SOCK_EVENT *sdi;
ST_BOOLEAN activityFlag = SD_FALSE;	/* assume no events processed	*/

#if !defined(GENSOCK_THREAD_SUPPORT)
  /* Service all sockets.						*/
  /* NOTE: this may add "many" entries to the linked list.		*/
  /* User should have already waited for event, so use timeout=0 here.	*/
  sockServiceAll (&tp0Ctx.sockCtx, 0);	/* arg is timeout in milliseconds*/
#endif	/* !GENSOCK_THREAD_SUPPORT	*/

  /* CRITICAL: Get one and only one entry off linked list. If we get more,*/
  /* MVL queue fills up, and user can't process other events, including TP4.*/
  if ((sdi = sockEventGet (&tp0Ctx.sockCtx)) != NULL)
    {
    activityFlag = SD_TRUE;		/* 1 event processed	*/
    S_LOCK_UTIL_RESOURCES ();
    /* check if we need to enable reception of data (we have flow control to	*/
    /* prevent excessive memory allocations when data messages are received	*/
    /* faster than the application can process)					*/
    if (sdi->eventType == GS_EVENT_DATA_IND)
      {
      SOCK_INFO *sock_info = (SOCK_INFO *) sdi->pSock->sockCfg.usr2; /* get (SOCK_INFO *) from GEN_SOCK	*/
      if (sock_info->recvCnt > 0)
        --sock_info->recvCnt;
      if (sock_info->recvCnt <= SOCK_RX_QUE_LOW_MARK && sdi->pSock->sockCfg.pauseRecv == SD_TRUE)
        {
        sdi->pSock->sockCfg.pauseRecv = SD_FALSE;
        SOCK_LOG_FLOW2 (&tp0Ctx.sockCtx, "%s: recvCnt=%u setting pauseRecv=SD_FALSE", sdi->pSock->sockIdStr, sock_info->recvCnt);
        sockServiceWakeAll (&tp0Ctx.sockCtx);
	}
      }
    S_UNLOCK_UTIL_RESOURCES ();
    SOCK_LOG_FLOW3 (&tp0Ctx.sockCtx, "%s: processing Event Indication IndType=%d (%s).",
                    sdi->pSock->sockIdStr, sdi->eventType,
                    (sdi->eventType < NUM_EVENT_TYPES) ? eventTypeStr[sdi->eventType] : "UNKNOWN");

    switch (sdi->eventType)
      {
      case GS_EVENT_CONNECT_IND:
        _processConnectInd (sdi->pSock);
      break;

      case GS_EVENT_CONNECT_CONF:
        _processConnectDone (sdi->pSock);
      break;

      case GS_EVENT_DISCONNECT:
        _processDisconnectInd (sdi->pSock);
      break;

      case GS_EVENT_DATA_IND:
        _processRxInd (sdi->pSock, sdi->sockData);
      break;

      default:
        assert (0);	/* Unknown eventType	*/
      }
    chk_free (sdi);
    }

  return (activityFlag);
  }

/************************************************************************/
/*			_processConnectInd				*/
/* Accepted new socket connection, let user know.			*/
/************************************************************************/
static ST_VOID _processConnectInd (GEN_SOCK *pSock)
  {
SOCK_INFO          *sock_info;

  /* For a listening socket we will attach the GEN_SOCK to our user ctrl*/
  /* Pass (GEN_SOCK *) instead of SOCKET as first arg.			*/
  /* This saves (GEN_SOCK *) in SOCK_INFO.				*/
  sock_info = sock_info_alloc2 (pSock, SOCK_STATE_ACCEPTED,
                               INVALID_CONN_ID);
  sock_info->ip_addr = pSock->callingAddr.sin_addr.s_addr; /* Save remote IP addr	*/
  pSock->sockCfg.usr2 = sock_info;			   /* Save (SOCK_INFO *) in GEN_SOCK*/

  handle_accepted_conn (sock_info);
  }

/************************************************************************/
/*			handle_accepted_conn				*/
/************************************************************************/
ST_VOID handle_accepted_conn (SOCK_INFO *sock_info)
  {
  /* User only cares about T-CONNECT.ind.  Just log this.		*/
  SOCK_LOG_RX1 (&tp0Ctx.sockCtx, "N-CONNECT.ind:  sock_info =" S_FMT_PTR, sock_info);
  sock_info->state = SOCK_STATE_CONNECTED;
  }

/************************************************************************/
/*			_processConnectDone				*/
/* Socket connection established, let user know.			*/
/************************************************************************/
static ST_VOID _processConnectDone (GEN_SOCK *pSock)
  {
SOCK_INFO          *sock_info = (SOCK_INFO *) pSock->sockCfg.usr2; /* get (SOCK_INFO *) from GEN_SOCK	*/

  assert (sock_info);
  sock_info->genSock = pSock;	/* CRITICAL: save pSock now	*/

  if (sock_info->state == SOCK_STATE_CONNECTING)
    {
    handle_connect_success (sock_info);
    }
  else
    SOCK_LOG_ERR2 (&tp0Ctx.sockCtx, "%s: _processConnectDone invalid state=%d", pSock->sockIdStr, sock_info->state);
  }

/************************************************************************/
/*			handle_connect_success				*/
/************************************************************************/
ST_VOID handle_connect_success (SOCK_INFO *sock_info)
  {
  /* Pass up N-CONNECT.cnf+ to user.	*/
  SOCK_LOG_RX1 (&tp0Ctx.sockCtx, "N-CONNECT.cnf+ (pos): user_conn_id =" S_FMT_PTR, sock_info->user_conn_id);
  np_connect_cnf_pos (sock_info, sock_info->user_conn_id);
  sock_info->state = SOCK_STATE_CONNECTED;
  }

/************************************************************************/
/*			_processDisconnectInd				*/
/************************************************************************/
static ST_VOID _processDisconnectInd (GEN_SOCK *pSock)
  {
SOCK_INFO *sock_info = (SOCK_INFO *) pSock->sockCfg.usr2;

  if (pSock->sockCfg.secEnable)
    {
    if (sock_info)
      {
      S_LOCK_UTIL_RESOURCES ();
      if (list_find_node (tp0Ctx.secureSockList, sock_info) == SD_SUCCESS)
        /* del sock_info from list of secured sockets */
        list_unlink (&tp0Ctx.secureSockList, sock_info);
      S_UNLOCK_UTIL_RESOURCES ();
      }
    else
      {
      /* in this case a remote node attempted to connect on a secured port	*/
      /* but for some SSL reason the socket connection need to be terminated	*/
      sockClose (pSock);
      sockFree (pSock);
      return;	/* can't do anything else	*/
      }
    }

  if (sock_info)
    {
    /* If we disconnected, don't call handle_disconnect.	*/
    if (sock_info->state != SOCK_STATE_DISCONNECTING)
      handle_disconnect (sock_info);

    sock_info_free (sock_info);
    }
  else
    SOCK_LOG_ERR1 (&tp0Ctx.sockCtx, "%s: _processDisconnectInd sock_info=NULL", pSock->sockIdStr);

  sockFree (pSock);	/* Totally done with this socket now.	*/
  }

/************************************************************************/
/*			handle_disconnect				*/
/************************************************************************/
ST_VOID handle_disconnect (SOCK_INFO *sock_info)
  {
  SOCK_LOG_RX2 (&tp0Ctx.sockCtx, "N-DISCONNECT.ind:  sock_info = " S_FMT_PTR ", user_conn_id =" S_FMT_PTR ".",
          sock_info, sock_info->user_conn_id);
  np_disconnect_ind (sock_info);
  }

/************************************************************************/
/*			_processRxInd					*/
/************************************************************************/
static ST_VOID _processRxInd (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData)
  {
  /* data is only valid if result==SD_SUCCESS	*/
  if (sockData->result == SD_SUCCESS)
    {
    /* rxData->data includes 4-byte RFC-1006 header. That's what handle_data wants.*/
    assert (sockData->dataLen <= 65535);	/* RFC-1006 limit	*/
    handle_data ((SOCK_INFO *)pSock->sockCfg.usr2, sockData->data,
                 (ST_UINT16)sockData->dataLen);
    }
  _uSockDataFree (pSock, sockData);
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
  SOCK_LOG_RX3 (&tp0Ctx.sockCtx, "N-DATA.ind:  sock_info =" S_FMT_PTR ", user_conn_id =" S_FMT_PTR " tpkt_len=%d",
     sock_info, sock_info->user_conn_id, tpkt_len);
  SOCK_LOG_RXH (&tp0Ctx.sockCtx, tpkt_len, tpkt_ptr);
  np_data_ind (sock_info, tpkt_ptr, tpkt_len);
  }

	/*----------------------------------------------*/
	/*	Connection Request / Indication		*/
	/*----------------------------------------------*/

/************************************************************************/
/*			np_connect_req					*/
/* RETURNS:								*/
/*	SOCK_INFO *	ptr to socket info for new socket,  OR		*/
/*			NULL if connect fails immediately.		*/
/************************************************************************/
SOCK_INFO *np_connect_req (ST_VOID *user_conn_id, ST_ULONG ipAddr, ST_UINT16 rem_port,
                           S_SEC_ENCRYPT_CTRL *encrypt_ctrl)
  {
ST_RET          rc;
SOCK_INFO      *sock_info;	/* new tracking struct for this conn.	*/
struct in_addr  sin_addr;	/* inet_ntoa needs this addr format	*/
ST_CHAR        *tmp_ptr;
ST_CHAR         addr_string [32];		/* local copy of addr	*/
GEN_SOCK_CONFIG sockCfg = {0};
ST_UINT16       port;
GEN_SOCK       *pGenSock;

  SOCK_LOG_TX1 (&tp0Ctx.sockCtx, "N-CONNECT.req:        user_conn=" S_FMT_PTR, user_conn_id);

  /* MMS Lite stores IP Addr as ULONG. Must convert back to string.	*/
  sin_addr.s_addr = ipAddr;
  tmp_ptr = inet_ntoa (sin_addr);
  assert (strlen (tmp_ptr) < sizeof (addr_string));
  strcpy (addr_string, tmp_ptr);

  /* Must do this first, because connect ind callback may be called
   * before returning from sockInitCalling!
   */
  sock_info = sock_info_alloc2 (NULL, SOCK_STATE_CONNECTING, user_conn_id);


  /* setup rem socket params and connect to the rem IP Addr, Port	*/
  _sockSetDefaults (&sockCfg);
  sockCfg.reuseAddr = SD_FALSE;	/* Override setting from _sockSetDefaults.*/

  /* save our control ptr (used in callback functions) */
  sockCfg.usr2 = (ST_VOID *) sock_info;

    {
    /* non-secured connection */
    if (rem_port)
      port = rem_port;			/* use configured remote port	*/
    else
      port = IPPORT_RFC1006;
    }

  /* connect to Remote */
  /* NOTE: The callback may be called before
   * sockInitCalling returns, so just save the (GEN_SOCK *) to "sock_info->genSock"
   * in the callback function (_uSockConnectInd).
   */
  SOCK_LOG_TX4 (&tp0Ctx.sockCtx, "N-CONNECT.req:        sock_info=" S_FMT_PTR ", connection pending to IP Address='%s' Port=%u %s",
                  sock_info, addr_string, port,
                  (encrypt_ctrl->encryptMode==S_SEC_ENCRYPT_SSL) ? "(SSL)" : "");

  rc = sockInitCalling (&tp0Ctx.sockCtx, &sockCfg, port, addr_string, &pGenSock);

  if (rc != SD_SUCCESS)
    {		/* failed, clean up	*/
    sock_info_free (sock_info);
    sock_info = NULL;
    }
  else
    {
    sock_info->genSock = pGenSock;	/* save (GEN_SOCK *)	*/
    }

  return (sock_info);
  }

/************************************************************************/
/*                      _uSockConnectDone                               */
/* This is gensock2 callback function called when socket connection	*/
/* to the remote has been established.					*/
/************************************************************************/

static ST_RET _uSockConnectDone (GEN_SOCK *pSock)
  {
ST_RET rtn;

  SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: in _uSockConnectDone()", pSock->sockIdStr);

  rtn = _addSockInd (pSock, GS_EVENT_CONNECT_CONF, NULL);
  return (rtn);
  }

/************************************************************************/
/*                      _uSockConnectInd                                */
/* This is gensock2 callback function when a socket connection has been	*/
/* accepted from remote.						*/
/************************************************************************/

static ST_RET _uSockConnectInd (GEN_SOCK *pSock)
  {
ST_RET rtn;
ST_CHAR *SSLstr;

  if (pSock->sockCfg.secEnable)
    SSLstr = "SSL";
  else
    SSLstr = "";
  SOCK_LOG_FLOW4 (&tp0Ctx.sockCtx, "%s: in _uSockConnectInd(), received %s connection from"
                  " IP Address=%s Port=%u", pSock->sockIdStr, SSLstr,
    inet_ntoa (pSock->callingAddr.sin_addr), (ST_UINT) ntohs (pSock->callingAddr.sin_port));

  rtn = _addSockInd (pSock, GS_EVENT_CONNECT_IND, NULL);
  return (rtn);
  }

	/*----------------------------------------------*/
	/*	Disconnection Request / Indication	*/
	/*----------------------------------------------*/

/************************************************************************/
/*			np_disconnect_req				*/
/* This function is called by the user to break connection		*/
/************************************************************************/

ST_RET np_disconnect_req (SOCK_INFO *sock_info)
  {
ST_RET retCode;
  SOCK_LOG_TX2 (&tp0Ctx.sockCtx, "N-DISCONNECT.req:  sock_info = " S_FMT_PTR ", user_conn_id =" S_FMT_PTR ".",
     sock_info, sock_info->user_conn_id);


  if (sock_info->genSock == NULL)
    {
    /* This should NEVER happen. Just log info to help with debugging.	*/
    SOCK_LOG_ERR3 (&tp0Ctx.sockCtx, "N-DISCONNECT.req ERROR:  socket unknown\n"
                    "                     sock_info =" S_FMT_PTR ", user_conn_id =" S_FMT_PTR ", state = %d",
        sock_info, sock_info->user_conn_id, sock_info->state);
    retCode = SD_FAILURE;
    }
  else
    {
    /* change the state to avoid calling any transport functions from this point on 	*/
    sock_info->state = SOCK_STATE_DISCONNECTING;
    retCode = sockClose (sock_info->genSock);
    }

  return (retCode);
  /* sock_info is not freed until _processDisconnectInd.	*/
  }

/************************************************************************/
/*                      _uSockDisconnectInd                             */
/* This is gensock2 callback function when socket gets disconnected or	*/
/* we call sockClose.							*/
/************************************************************************/
static ST_VOID _uSockDisconnectInd (GEN_SOCK *pSock)
  {
ST_RET     rtn;
ST_CHAR   *errptr;

  switch (pSock->disconnectReason)
    {
    case GS_DISCONNECT_CONNECT_FAILED:	errptr= "CONNECT_FAILED"; break;
    case GS_DISCONNECT_USR_REFUSED:	errptr= "USER_REFUSED"; break;
    case GS_DISCONNECT_SEND_FAILURE:	errptr= "SEND_FAILURE"; break;
    case GS_DISCONNECT_RECV_FAILED:	errptr= "RECEIVE_FAILED"; break;
    case GS_DISCONNECT_ACCEPT_FAILED:	errptr= "ACCEPT_FAILED"; break;
    case GS_DISCONNECT_CLOSED:		errptr= "SOCKET_CLOSED"; break;
    case GS_DISCONNECT_TERMINATING:	errptr= "TERMINATING"; break;
    case GS_DISCONNECT_RESOURCES_ERROR:	errptr= "RESOURCES_ERROR"; break;
    case GS_DISCONNECT_INTERNAL_ERROR:	errptr= "INTERNAL_ERROR"; break;

    case GS_DISCONNECT_UNKNOWN:
    default:				errptr= "UNKNOWN"; break;
    }
  SOCK_LOG_FLOW2 (&tp0Ctx.sockCtx, "%s: in _uSockDisconnectInd() reason=%s", pSock->sockIdStr, errptr);

  /* CRITICAL: deal with GS_ROLE_LISTENING now. This happens when server is
   * exiting. _processDisconnectInd may not be called again.
   */
  if (pSock->role == GS_ROLE_LISTENING)
    {
    if (pSock == tp0Ctx.remListenSock)
      {
      SOCK_LOG_FLOWC1 (&tp0Ctx.sockCtx, " for non-secured listening port=%d", tp0Ctx.rfc1006Port);
      tp0Ctx.remListenSock = NULL;
      }
    sockFree (pSock);
    return;
    }

  rtn = _addSockInd (pSock, GS_EVENT_DISCONNECT, NULL);
  if (rtn != SD_SUCCESS && tp0Ctx.state == TP0_SOCK_CTX_STATE_TERMINATING)
    {
    SOCK_INFO *sock_info = (SOCK_INFO *) pSock->sockCfg.usr2;
    /* since we will not pass the disconnect ind to the main thread clean the	*/
    /* sock_info here 								*/
    if (sock_info)
      sock_info_free (sock_info);

    sockFree (pSock);
    }
  }

	/*----------------------------------------------*/
	/*	Data Request / Indication		*/
	/*----------------------------------------------*/

/************************************************************************/
/* np_data_req								*/
/* Parameters:								*/
/*  eot			SD_TRUE if last TPDU msg in a SPDU.		*/
/* RETURN CODES:							*/
/*	SD_SUCCESS or SD_FAILURE					*/
/************************************************************************/
ST_RET np_data_req (SOCK_INFO *sock_info, ST_INT tpkt_len, ST_UCHAR *tpkt_ptr,
                    ST_BOOLEAN eot)
  {
GEN_SOCK_DATA *sockData;
ST_RET rc;

  assert (sock_info->genSock != NULL);

  /* check if queueing limit has been reached to prevent the excessive	*/
  /* growth of the outbound queue in gensock2.c				*/
  if (sockTxQueueGroupCntGet (sock_info->genSock) >= tp0_cfg.max_spdu_outst)
    {
    SOCK_LOG_ERR2 (&tp0Ctx.sockCtx, "N-DATA.req:  sock_info =" S_FMT_PTR ", error sending (queue limit = %u reached)",
                   sock_info, tp0_cfg.max_spdu_outst);
    return (SD_FAILURE);
    }

  /* _uSockDataAlloc allocs "sockData" (plus space for data) and initializes it.*/
  _uSockDataAlloc (sock_info->genSock, tpkt_len, &sockData);
  memcpy (sockData->data, tpkt_ptr, tpkt_len);	/* copy the data	*/
  sockData->eot = eot;

  rc = sockTxMsg (sock_info->genSock, sockData);

  if (rc == SD_SUCCESS)
    {
    SOCK_LOG_TX3 (&tp0Ctx.sockCtx, "N-DATA.req:  sock_info =" S_FMT_PTR ", user_conn_id =" S_FMT_PTR " tpkt_len=%d",
       sock_info, sock_info->user_conn_id, tpkt_len);
    SOCK_LOG_TXH (&tp0Ctx.sockCtx, tpkt_len, tpkt_ptr);
    }
  else
    SOCK_LOG_ERR1 (&tp0Ctx.sockCtx, "N-DATA.req:  sock_info = "S_FMT_PTR ", error sending", sock_info);

  return (rc);
  }

/************************************************************************/
/*			np_get_tx_queue_cnt				*/
/* Returns number of queued SPDUs. 					*/
/************************************************************************/
ST_UINT np_get_tx_queue_cnt (SOCK_INFO *sock_info)
  {
  assert (sock_info->genSock != NULL);
  return (sockTxQueueGroupCntGet (sock_info->genSock));
  }

/************************************************************************/
/*                      _uSockWritable                                  */
/* This function is called by gensock2 when the socket becomes writable.*/
/************************************************************************/
static ST_VOID _uSockWritable (GEN_SOCK *pSock)
  {
ST_RET     rc;

  SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: in _uSockWritable()", pSock->sockIdStr);

  /* the socket is writable, so send something from the transmit queue.	*/
  rc = sockTxQueueProc (pSock);
  if (rc != SD_SUCCESS)
    {
    SOCK_LOG_ERR1 (&tp0Ctx.sockCtx, "%s: sockTxQueueProc failed, closing socket", pSock->sockIdStr);
    /* DEBUG: this change of state will cause problem,	*/
    /*        app will not be notified about disconnect */
    /*    sock_info->state = SOCK_STATE_DISCONNECTING; 	*/
    sockClose (pSock);
    }
  }

/************************************************************************/
/*                      _uSockRxInd                                     */
/************************************************************************/
static ST_VOID _uSockRxInd (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData)
  {
ST_RET rtn;

  if (pSock && sockData)
    SOCK_LOG_FLOW3 (&tp0Ctx.sockCtx, "%s: in _uSockRxInd() result=%s, bytes=%d", pSock->sockIdStr,
      (sockData->result == SD_SUCCESS) ? "SD_SUCCESS" : "SD_FAILURE", sockData->dataLen);
  else
    {
    SOCK_LOG_ERR2 (&tp0Ctx.sockCtx, "%s: in _uSockRxInd() sockData=" S_FMT_PTR, pSock->sockIdStr, sockData);
    if (sockData)
      _uSockDataFree (pSock, sockData);
    return;
    }

  if (sockData->result != SD_SUCCESS)
    {
    /* socket is disconnecting, any received data buffers need to be free */
    _uSockDataFree (pSock, sockData);
    return;
    }

  /* If no data (len=RFC1006_HEAD_LEN) ignore msg.	*/
  if (sockData->dataLen <= RFC1006_HEAD_LEN)
    {
    /* Should NEVER be '<' because _uSockHunt checks for illegal len, but just in case...*/
    if (sockData->dataLen < RFC1006_HEAD_LEN)
      SOCK_LOG_ERR1 (&tp0Ctx.sockCtx, "Illegal TPKT len = %d", sockData->dataLen);

    /* May be '='. NOT normal, but some bad apps could send just header.*/
    /* Either way, do NOT put on queue.					*/
    _uSockDataFree (pSock, sockData);
    return;
    }

  /* received data on the socket, queue indication for user */
  rtn = _addSockInd (pSock, GS_EVENT_DATA_IND, sockData);
  if (rtn != SD_SUCCESS)
    _uSockDataFree (pSock, sockData);
  }

/************************************************************************/
/*                      	uSockHunt				*/
/* This function will get the length of received message on a socket.	*/
/* We expect the size field to be RFC1006_HEAD_LEN bytes.		*/
/************************************************************************/

static ST_VOID _uSockHunt (GEN_SOCK *pSock, ST_INT *huntStateIo,
                     ST_CHAR *buf, ST_INT bufCount, ST_INT *lenOut)
  {
ST_UINT16 u16len;

  /* Better get our 4 byte RFC-1006 header ... */
  if (bufCount != RFC1006_HEAD_LEN)
    {
    /* This should NEVER happen. gensock2 state machine must be corrupted.*/
    SOCK_LOG_ERR1 (&tp0Ctx.sockCtx, "%s: Socket Hunt for start of message failed.\n"
                   " Error could be unrecoverable.", pSock->sockIdStr);
    *huntStateIo = GENSOCK_HUNT_DISCONNECT;
    return;
    }

  if (buf[0] != RFC1006_VERSION)
    {
    SOCK_LOG_ERR1 (&tp0Ctx.sockCtx, "%s: Received RFC 1006 header with invalid version (first byte). Header:",
                    pSock->sockIdStr);
    SOCK_LOG_ERRH (&tp0Ctx.sockCtx, 4, buf);
    *huntStateIo = GENSOCK_HUNT_DISCONNECT;
    return;
    }

  /* Ignore 2nd byte (reserved).	*/

  /* len is in 3rd/4th bytes, in network byte order, and includes len of header.
   * Convert to host byte order, & subtract header len.
   * CRITICAL: cast to ST_UCHAR before ST_UINT16 to avoid sign extension.
   */
  u16len = (((ST_UINT16)(ST_UCHAR) buf[2]) << 8) | (ST_UINT16)(ST_UCHAR) buf[3];
  /* CRITICAL: make sure len is at least RFC1006_HEAD_LEN so subtracting*/
  /* does not create negative number.					*/
  if (u16len < RFC1006_HEAD_LEN)
    {
    SOCK_LOG_ERR2 (&tp0Ctx.sockCtx, "%s: Received RFC 1006 header with invalid length (%u). Header:",
                    pSock->sockIdStr, u16len);
    SOCK_LOG_ERRH (&tp0Ctx.sockCtx, RFC1006_HEAD_LEN, buf);
    *huntStateIo = GENSOCK_HUNT_DISCONNECT;
    return;
    }
  u16len -= RFC1006_HEAD_LEN;	/* subtract header len	*/
  /* NOTE: cast to ST_UINT before ST_INT to avoid sign extension.	*/
  *lenOut = (ST_INT)(ST_UINT)u16len;
  *huntStateIo = GENSOCK_HUNT_DONE;
  }

	/*----------------------------------------------*/
	/*		Alloc / Free			*/
	/*----------------------------------------------*/

/************************************************************************/
/*			sock_info_alloc2				*/
/* Allocate a SOCK_INFO struct and fill it in.				*/
/************************************************************************/
SOCK_INFO *sock_info_alloc2 (GEN_SOCK *genSock, ST_INT state, ST_VOID *user_conn_id)
  {
SOCK_INFO *sock_info;

  /* Allocate SOCK_INFO struct.						*/
  sock_info = (SOCK_INFO *) M_CALLOC (MSMEM_SOCK_INFO, 1, sizeof (SOCK_INFO));
  /* Fill in SOCK_INFO struct.						*/
  /* NOTE: sock_info->hSock is NOT used. sock_info->genSock is used instead.*/
  sock_info->genSock = genSock;
  sock_info->state = state;
  sock_info->user_conn_id = user_conn_id;
  sock_info->encrypt_ctrl.encryptMode = S_SEC_ENCRYPT_NONE;
  sock_info->recvCnt = 0;
  return (sock_info);
  }

/************************************************************************/
/*			sock_info_free					*/
/* Free a SOCK_INFO struct.						*/
/************************************************************************/
ST_VOID sock_info_free (SOCK_INFO *sock_info)
  {
  /* NOTE: gensock2 handles closing of the socket.	*/
  M_FREE (MSMEM_SOCK_INFO, sock_info);
  }

/************************************************************************/
/*			_uSockDataAlloc					*/
/************************************************************************/
static ST_VOID _uSockDataAlloc (GEN_SOCK *pSock, ST_INT dataLen, 
			         GEN_SOCK_DATA **sockDataOut)
  {
GEN_SOCK_DATA *sockData;

  sockData = (GEN_SOCK_DATA *) chk_malloc (sizeof (GEN_SOCK_DATA) + dataLen);
  sockData->data = (ST_UCHAR *) (sockData + 1);
  sockData->dataLen = dataLen;
  /* We don't use sockData->usrBufBase, sockData->usrBufLen */

  *sockDataOut = sockData;
  }
/************************************************************************/
/*			_uSockDataFree					*/
/************************************************************************/
static ST_VOID _uSockDataFree (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData)
  {
  /* Allocated in _uSockDataAlloc using "chk_malloc", so use "chk_free".*/
  chk_free (sockData);
  }

	/*----------------------------------------------*/
	/*			Misc			*/
	/*----------------------------------------------*/

/************************************************************************/
/*			_addSockInd					*/
/************************************************************************/
static ST_RET _addSockInd (GEN_SOCK *pSock, ST_INT eventType, GEN_SOCK_DATA *sockData)
  {
GEN_SOCK_EVENT *sdi;

  if (tp0Ctx.state == TP0_SOCK_CTX_STATE_TERMINATING)
    {
    SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: in _addSockInd failed - context terminating", pSock->sockIdStr);
    return (SD_FAILURE);
    }
  else
    SOCK_LOG_FLOW1 (&tp0Ctx.sockCtx, "%s: in _addSockInd", pSock->sockIdStr);

  sdi = (GEN_SOCK_EVENT *) chk_calloc (1, sizeof (GEN_SOCK_EVENT));
  sdi->eventType = eventType;
  sdi->pSock = pSock;

  sdi->sockData = sockData;
  sockEventPut (&tp0Ctx.sockCtx, sdi);	/* let gensock save on list*/
  S_LOCK_UTIL_RESOURCES ();
  /* if application can't process incoming data msgs as fast as they are*/
  /* received by the socket then the best way to prevent uncontrolled	*/
  /* growth of the receiving queue is to apply back pressure to the	*/
  /* sending socket.							*/
  if (eventType == GS_EVENT_DATA_IND)
    {
SOCK_INFO *sock_info = (SOCK_INFO *) pSock->sockCfg.usr2;
    /* this is a bit tricky, if we accept connection from rfc1006listener	*/
    /* then the first packet will be queued before the sock_info is allocated	*/
    /* so we must check here if the ptr is NULL					*/
    if (sock_info)
      {
      ++sock_info->recvCnt;		
      if (sock_info->recvCnt >= SOCK_RX_QUE_HIGH_MARK && pSock->sockCfg.pauseRecv == SD_FALSE)
        {
        pSock->sockCfg.pauseRecv = SD_TRUE;
        SOCK_LOG_FLOWC2 (&tp0Ctx.sockCtx, "%s: recvCnt=%u, setting pauseRecv=SD_TRUE",
                         pSock->sockIdStr, sock_info->recvCnt);
        }
      }
    }
  S_UNLOCK_UTIL_RESOURCES ();

  /* wake up main thread	*/
#if defined(_WIN32)
  gs_signal_event_sem (hTcpEvent);	/* wake up main thread	*/
  SOCK_LOG_FLOW2 (&tp0Ctx.sockCtx, "%s: in _addSockInd() signaled hTcpEvent=%lu", pSock->sockIdStr, (ST_ULONG) hTcpEvent);
#endif

  return (SD_SUCCESS);
  }

