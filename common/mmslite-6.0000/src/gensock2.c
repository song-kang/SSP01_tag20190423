/************************************************************************/
/* SOFTWARE MODULE HEADER ***********************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2002 - 2011, All Rights Reserved                            */
/*                                                                      */
/* MODULE NAME : gensock2.c                                             */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*   general socket routines                                            */
/*                                                                      */
/*                                                                      */
/* MODIFICATION LOG :                                                   */ 
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/21/11  EJV	   sockCreateWakeupSockets: chg accept wait from*/
/*			   100ms to 25,000ms for slow starting service.	*/
/* 02/17/11  EJV	   sockStart(Ex): corr - don't free sockCtx.	*/
/* 08/02/10  JRB	   sockStart(Ex): chg arg to (GEN_SOCK_CTXT *).	*/
/*			   sockEnd: do not free (GEN_SOCK_CTXT *).	*/
/* 06/03/10  JRB	   Delete invalid log msg.			*/
/* 04/27/10  JRB	   Use new uSockRxBufFree funct ptr.		*/
/* 02/25/10  JRB	   _sockServiceDiscardSock: chk serviceCtrl.	*/
/*			   Del serviceCtrl chk before sockServiceWake.	*/
/* 01/20/10  JRB    67     Fix log messages.				*/
/* 03/10/09  EJV    66     Added two GS_DISCONNECT_CERT_* errors.	*/
/* 01/14/09  EJV    65     GEN_SOCK_CTXT: added gs_wakeup_port_range.	*/
/*			   sockStart: sockCtx can be passed in.		*/
/*			   Set default gs_wakeup_port_range 90.		*/
/* 01/14/09  EJV    64     sockInitCalling, sockSelectResultProcess:	*/
/*			   corr handling when secConnectProc ret FAILURE*/
/*			   to avoid calling sockFree twice; also	*/
/*			   added missing UNLOCK calls.			*/
/*			   sockEnd: chk for NULL sockCtx.		*/
/* 10/15/08  EJV    63     sockCheckForIdleConns CORR: don't close	*/
/*			     listening socket.				*/
/* 10/08/08  MDE    62     Allow variable len header, sockEventQueueFlush*/
/* 10/06/08  JRB    61     Close socket on ANY _sockCheckRxData error	*/
/*			   & remove sockClose calls in _sockRecv.	*/
/*			   Del _sockCheckRxData call right after connect*/
/*			   completes (almost never gets data).		*/
/* 07/24/08  EJV    60     sockSelectResultProcess: check sockErrorDetected */
/*			   Fix on 11/26/07 (Rev 1.59.1.0 branch).	*/
/* 04/07/08  EJV    59     sockFree CORR: call sockTxQueueDestroy to	*/
/*			     prevent memory leaks.			*/
/* 02/27/08  EJV    58     sockEventGet: check before decrementing.	*/ 
/* 01/29/08  EJV    57     Use S_FMT_PTR macro to log pointers.		*/
/*			   sockInitCalling: added 2 return(SD_FAILURE).	*/
/* 12/04/07  EJV    56     sockUsrFun: check if con secured.		*/
/*			   Add sockCheckForIdleConns (SISCO KeepAlive).	*/
/* 11/28/07  EJV    55     _WIN32: changed to Winsock version 2.	*/
/*			   Added disconnectReason in few places.	*/
/*			   Added sockDisconnReasonStr for slogging.	*/
/*			   sockFree: reversed 'if' for more redability.	*/
/* 02/21/08  EJV    54     sockGetRemAddrInfo: corr portOut casting for	*/
/*			     0x8000-0xFFFF ports.			*/
/* 11/28/07  EJV    53     sockSelectResultProcess corr: added continue.*/
/*			   sockClose: add pSock->hSock=INVALID_SOCKET;	*/
/* 10/04/07  MDE    52     Tweaked LOGCFG_VALUE_GROUP/LOGCFGX_VALUE_MAP	*/
/* 09/10/07  MDE    51     Added sockStartEx, log_disable		*/
/* 02/28/07  MDE    50     Handle 'send' returning 0 (full pipe)	*/
/* 02/08/07  JRB    49     Add sockEventPut, sockEventGet.		*/
/*			   Repl uSockConnect with 2 separate pointers	*/
/*			   uSockConnectInd, uSockConnectConf.		*/
/* 02/06/07  JRB    48     Call _sockServiceDiscardSock ONLY from sockClose.*/
/*			   Replace other ..DiscardSock calls w/ sockClose.*/
/* 01/30/07  JRB    47     Fix crash in sockFree by simplifying threads	*/
/*			   (eliminate Free thread & sockServiceFreeList).*/
/*			   Call _sockServiceDiscardSock from sockClose.	*/
/*			   Lock all access to sockList.			*/
/* 01/15/07  JRB    46     Del gs_poll_mode flag & poll_mode arg to 	*/
/*			   sockStart (no longer supported).		*/
/* 01/03/07 EJV     45	   Rpl gensock2 mutexes with S_LOCK_UTIL_RESOURCES */
/*			     to avoid potential deadlock in slogipc.c;	*/
/*			   Del create/destroy calls for gensock2 mutexes*/
/* 12/11/06 EJV     44	   sockCreateWakeupSockets, _sockAllocSock:	*/
/*			     chg nonblock_on from ST_LONG to int.	*/
/*			     Needed on 64-bit UNIX, Linux systems.	*/
/* 11/17/06 EJV     43	   socket funs: added (int) cast, the HP-UX	*/
/*			     ssize_t is long.				*/
/* 11/06/06  EJV    42     Rem static from sockCtxList (extern in H).	*/
/* 06/13/06  RKR    41     Incorporated porting changes                 */
/* 04/14/06  JRB    40     Close xCallingSock, xCalledSock in sockEnd.	*/
/* 02/17/06  EJV    39     __VMS: defined SOCK_OPTLEN and SOCK_ADDRLEN	*/
/*			     in gensock2.h.				*/
/*			   sockGetRemAddrInfo: chg len from ST_INT type	*/
/*			     to SOCK_ADDRLEN.				*/
/* 01/30/06  GLB    38     Integrated porting changes for VMS           */
/* 12/19/05  EJV    37     sockUsrFun: chg args.			*/
/* 10/09/05  MDE    36     Changed connect errors NERR log to FLOW      */
/* 09/28/05  EJV    35     Implemented GEN_SOCK_CTXT & other changes:	*/
/*			   Moved filio.h for sun to sysincs.h.		*/
/*			   Moved MUTEX macros from gensock2.h & renamed.*/
/*			   sockStart, sockEnd, _sockAddServiceThread:	*/
/*			     add cleanup code.				*/
/*			   _sockAddSock: chg return to ST_RET.		*/
/*			   Added handling when _sockAddSock fails.	*/
/*			   Protect all numActive operations.		*/
/*			   sockInitFds: mv init code out, for non-thread*/
/* 07/18/05  EJV    34     sockSelectResultProcess: corr UNIX,LINUX,QNX	*/
/*			     check if socket connection successful.	*/
/* 07/11/05  EJV    33     Added convertIPAddr.				*/
/* 07/05/05  EJV    32     sockInitCalling, sockInitListen, _sockAccept:*/
/*			     close hSock if hSock >= FD_SETSIZE		*/
/* 06/17/05  EJV    31     sockTx: clearer log.				*/
/* 06/10/05  JRB    30     "_sockClose" deleted. Use "sockClose".	*/
/*			   Don't close listen socket on accept error.	*/
/* 05/23/05  EJV    29     Add sockLogMaskMapCtrl for parsing logcfg.xml*/
/*			   Moved sock_debug_sel from slog.c.		*/
/* 05/12/05  EJV    28     MMSEASE_MOSI:listenSocket=NULL GS_ROLE_CALLED*/
/* 05/10/05  EJV           Exposed _sockAllocSock, _sockAddSock protos.	*/
/*			   sockServiceWake: chg NERR to FLOW log.	*/
/* 05/04/05  MDE    27     Changed recv NERR log to FLOW        	*/
/* 05/04/05  EJV    26     Corr sockGetRemAddrInfo (chg peer to pp)	*/
/* 05/03/05  MDE    25     OK to get EINPROGRESS & EINTR in select	*/
/* 04/15/05  ASK    24     Signed/unsigned warning cleanup. Comment chg.*/
/* 03/23/05  EJV    23     Exposed sockCreateWakeupSockets.		*/
/* 03/16/05  JRB    22     Disconnect socket if uSockHunt returns	*/
/*			   GENSOCK_HUNT_DISCONNECT.			*/
/* 02/14/05  MDE    21     _sockRecv now looks for SOCK_INPROGRESS too	*/
/* 02/10/05  MDE    20     Added sockGetRemAddrInfo			*/
/* 02/02/05  MDE    37     Allow header only receive (empty body)	*/
/* 12/20/04  ASK    36     Change gs_wakeup_port to 55050.		*/
/*			   _sockCreateWakeupSockets: try up to 100 ports*/
/*			   in range. Change XSocket logs to ERR.	*/
/* 09/22/04  JRB    35     Change "..Connect Failed.." logs to NERR.	*/
/*			   Change "expected" select err log to FLOW.	*/
/* 08/12/04  EJV    34     sockInitFds corr: do not use FD_SET directly!*/
/* 07/22/04  EJV    33     Added filio.h for sun.			*/
/*			   Moved _gUsrCount up (out of define).		*/
/* 06/21/04  EJV    32     sockInitListen: chg to return errno.		*/
/* 05/21/04  MDE    31     Added user count _gUsrCount			*/
/* 05/20/04  EJV    30     Log port number when unable to bind.		*/
/* 05/17/04  MDE    29     Removed send assert forcing data buffer to 	*/
/* 			   be allocated as part of GEN_SOCK_DATA 	*/
/* 05/03/04  ASK    28     Log errno when unable to send 		*/
/* 04/09/04  ASK    27     sockSelectResultProcess:log SOCK_INTR as NERR*/
/* 04/22/04  MDE    26     Minor logging cleanup			*/
/* 03/19/04  MDE    25     Added broken pipe handler for QNX		*/
/* 03/04/04  ASK    24     Switched code in _sockClose.			*/
/* 02/26/04  EJV    23     Switched code in sockClose.			*/
/*			   Use SOCK_OPTLEN and SOCK_ADDRLEN.		*/
/* 01/30/04  EJV    22     sockSelectResultProcess: fix previous change.*/
/* 01/27/04  EJV    21     sockSelectResultProcess: chg log to NERR.	*/
/* 01/23/04  EJV    20     sockLogState: added locking semaphore.	*/
/*			   sockTxQueueAdd: free the sockData if error.	*/
/* 01/22/04  EJV    19     Replaced time() calls with sGetMsTime().	*/
/* 01/12/04  ASK    18     Don't use socket if closed in _sockAccept.	*/
/*                         Add log msg if select() fails.		*/
/*                         Sleep for a second if select() fails.	*/
/*	     EJV	   Moved sock_debug_sel to slog.c		*/
/*			   Add sockTxQueuedGroupCnt update.		*/
/*		           Add sockTxQueueGroupCntGet func.		*/
/* 11/04/03  JRB    17     Replace gs_sleep calls with sMsSleep.	*/
/* 07/07/03  JRB    16     Fix some log macros.				*/
/* 06/25/03  JRB    15     Chg log macros. Chg FLOW log to ERR.		*/
/* 06/23/03  EJV    14     Added 'first' param to sockTxQueueAdd.	*/
/* 06/23/03  ASK    13 	   Set rc to SD_SUCCESS in sockTxQueueProc	*/
/* 06/20/03  EJV    12     Added sockTxQueueDestroy. Log conn errors.	*/
/* 06/19/03  EJV    11     Chg some logs. Chg gSock to pSock in funs.	*/
/*			   Moved pSock->sockStats.numSend.		*/
/* 06/19/03  JRB    10     Make Rx/Tx names more consistent.		*/
/* 06/19/03  JRB    09     Fix ioctlsocket calls chging to non-blocking	*/
/*			   mode (last arg must point to non-zero ulong).*/
/* 06/18/03  JRB    08     Chk FD_SETSIZE if !(_WIN32).			*/
/*			   Add sockInitAllFds.				*/
/*			   Add sockTxMsg, sockTxQueue*: these		*/
/*			     use new txMutex & uSockTxBufFree func ptr.	*/
/*			   Set REUSEADDR option only on listen socket.	*/
/*			   Init totalfds,selectnfds=0 in sockInitFds.	*/
/*			   Chg some logging. Fix some ";".		*/
/*			   Chk for SOCK_INPROGRESS too.			*/
/* 06/03/03  MDE    07     More user poll featuresAdded sockGetFds	*/
/* 06/02/03  ASK    06     _sockRecv(): Log err before calling sockClose*/
/* 05/28/03  JRB    05     Fix setting of timeout for select call.	*/
/*			   Del unused code.				*/
/* 05/14/03  MDE    04     Added sockGetFds				*/
/* 05/09/03  JRB    03     Don't use SOCKET_ERROR (only defined on WIN32)*/
/* 04/07/03  EJV    02     Preset  sock_debug_sel to SOCK_LOG_ERR	*/
/* 02/17/03  MDE    01     Created                                      */
/************************************************************************/

#include "gensock2.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Global logging variables */

#define GENSOCK_MAX_HEADER_SIZE 	500

ST_UINT sock_debug_sel;		/* init in sockStart()		*/

#ifdef DEBUG_SISCO
LOGCFGX_VALUE_MAP sockLogMaskMaps[] =
  {
    {"SOCK_LOG_ERR",	SOCK_LOG_ERR,	&sock_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"SOCK_LOG_NERR",	SOCK_LOG_NERR,	&sock_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"SOCK_LOG_FLOW",	SOCK_LOG_FLOW,	&sock_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Flow"},
    {"SOCK_LOG_RX",	SOCK_LOG_RX,	&sock_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Receive"},
    {"SOCK_LOG_TX",	SOCK_LOG_TX,	&sock_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Transmit"}
  };

LOGCFG_VALUE_GROUP sockLogMaskMapCtrl =
  {
  {NULL,NULL},
  "SocketLogMasks",	/* Parent Tag */
  sizeof(sockLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  sockLogMaskMaps
  };
#endif /* DEBUG_SISCO */

SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_FLOW_TEXT = "SOCK_LOG_FLOW";
SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_NERR_TEXT = "SOCK_LOG_NERR";
SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_ERR_TEXT  = "SOCK_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_RX_TEXT   = "SOCK_LOG_RX";
SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_TX_TEXT   = "SOCK_LOG_TX";


/************************************************************************/
/* variables */
GEN_SOCK_CTXT	*sockCtxList;

/* disconnectReason strings for logging */
ST_CHAR *sockDisconnReasonStr[] =
    {
    "DISCONNECT REASON: UNKNOWN",		/*  0 */
    "DISCONNECT REASON: CONNECT_FAILED",	/*  1 */
    "DISCONNECT REASON: USR_REFUSED",		/*  2 */
    "DISCONNECT REASON: SEND_FAILURE",		/*  3 */
    "DISCONNECT REASON: RECV_FAILED",		/*  4 */
    "DISCONNECT REASON: ACCEPT_FAILED",		/*  5 */
    "DISCONNECT REASON: SOCKET CLOSED",		/*  6 */
    "DISCONNECT REASON: TERMINATING",		/*  7 */
    "DISCONNECT REASON: RESOURCES_ERROR",	/*  8 */
    "DISCONNECT REASON: INTERNAL_ERROR",	/*  9 */
    "DISCONNECT REASON: NORMAL_DISCONNECT",	/* 10 */
    /* disconnectReason for SSL connection	*/ 
    "DISCONNECT REASON: SSLE_LICENSE",		/* 11 */
    "DISCONNECT REASON: SSLE_INT_ERR",		/* 12 */
    "DISCONNECT REASON: REKEY_FAILED",		/* 13 */
    "DISCONNECT REASON: CERT_VERIFY_ERR",	/* 14 */
    "DISCONNECT REASON: CERT_REVOKED",		/* 15 */
    "DISCONNECT REASON: SEC_PARAM_ERR",		/* 16 */
    "DISCONNECT REASON: CERT_NOT_YET_VALID",	/* 17 */
    "DISCONNECT REASON: CERT_EXPIRED",		/* 18 */
    };

ST_INT sockDisconnReasonStrCnt = sizeof (sockDisconnReasonStr)/sizeof (ST_CHAR *);

/************************************************************************/
/* Static functions and macros */

#if defined(GENSOCK_THREAD_SUPPORT)
  static ST_THREAD_RET ST_THREAD_CALL_CONV _sockServiceThread (ST_THREAD_ARG pArg);
  static ST_RET _sockAddServiceThread (GEN_SOCK_CTRL *serviceCtrl);
#endif


static GEN_SOCK_CTRL *_sockAddServiceCtrl (GEN_SOCK_CTXT *sockCtx);
static ST_RET _sockAccept (GEN_SOCK *pListenSock);
static ST_INT _sockCheckRxData (GEN_SOCK *pSock);
static ST_VOID _sockServiceDiscardSock (GEN_SOCK_CTRL *serviceCtrl, GEN_SOCK *pSock);


/************************************************************************/
/*                      sockStart                                       */
/* This function initializes the gensock2 for use in new context.	*/
/* The "sockCtx" arg must point to a structure that is valid		*/
/* until after "sockEnd" is called. The structure may be allocated or	*/
/* a global.								*/
/* The caller may initialize these elements of "sockCtx":		*/
/*    gs_wakeup_port							*/
/*    gs_wakeup_port_range						*/
/* Returns:								*/
/*	SD_SUCCESS (0)		context initialized			*/
/*	SD_FAILURE		otherwise				*/
/************************************************************************/

ST_RET sockStart (ST_CHAR *ctxName, GEN_SOCK_CTXT *sockCtx)
  {
ST_RET rc;

  rc = sockStartEx (ctxName, sockCtx, SD_FALSE);
  return (rc);
  }


ST_RET sockStartEx (ST_CHAR *ctxName, GEN_SOCK_CTXT *sockCtx, ST_BOOLEAN log_disable)
  {
static ST_UINT ctxId = 0;
GEN_SOCK_CTRL *serviceCtrl = NULL;
ST_RET         rc = SD_SUCCESS;

  if (sockCtx == NULL)
    {
    SLOGALWAYS0 ("sockStart ERROR: sockCtx is NULL");
    return (SD_FAILURE);
    }

  if (ctxId == 0)
    {
    /* do not put any logging above */
    #if DEBUG_SISCO
      sock_debug_sel |= SOCK_LOG_ERR;
    #endif

    #ifdef _WIN32
      {
      ST_INT  wRet;
      WSADATA wsaData;        /* WinSock data */
      /* init WinSock interface       */
      if (wRet = WSAStartup(0x0202, &wsaData))
        {
        SOCK_LOG_ERR2 (sockCtx, "Can't initialize WinSock interface to use version 2.2 (error=%d) for context '%s'",
                       wRet, (ctxName ? ctxName : ""));
        return (SD_FAILURE);
        }
      }
    #else /* UNIX,... */
      signal (SIGPIPE, SIG_IGN);
    #endif

    ++ctxId;
    }  /* if firstTime */

    /* these can be pre-initialized by caller:			*/
    /*   gs_wakeup_port      					*/
    /*   gs_wakeup_port_range					*/
  sockCtx->log_disable = log_disable;
  sockCtx->ctxId = ctxId++;
  sprintf(sockCtx->ctxName, "%s(ctxId=%u)", (ctxName ? ctxName : ""), sockCtx->ctxId);
  #if defined(GENSOCK_THREAD_SUPPORT)
  if (sockCtx->gs_wakeup_port == 0)
    sockCtx->gs_wakeup_port = GEN_SOCK_WAKEUP_PORT_BASE; /* up to 90 ports will be tried starting with this one */
  if (sockCtx->gs_wakeup_port_range == 0)
    sockCtx->gs_wakeup_port_range = GEN_SOCK_WAKEUP_PORT_RANGE;
  sockCtx->gs_select_timeout = 100000;		/* 10 seconds */
  /* already false
  sockCtx->gTerminateService = SD_FALSE;
  sockCtx->gTerminateFree = SD_FALSE;
  */
  #endif /* defined(GENSOCK_THREAD_SUPPORT) */

  SOCK_LOG_FLOW1 (sockCtx, "%s: sockStart() initialization pending...", sockCtx->ctxName);

  /* Start with one service control */
  serviceCtrl = _sockAddServiceCtrl (sockCtx);

  #if defined(GENSOCK_THREAD_SUPPORT)
  /* Start the service control thread */
  rc = _sockAddServiceThread (serviceCtrl);
  if (rc != SD_SUCCESS)
    {
    S_LOCK_UTIL_RESOURCES ();
    list_unlink (&sockCtx->sockServiceList, serviceCtrl);
    S_UNLOCK_UTIL_RESOURCES ();
    chk_free (serviceCtrl);
    return (rc);
    }
  #endif

  S_LOCK_UTIL_RESOURCES ();
  list_add_last (&sockCtxList, sockCtx);
  S_UNLOCK_UTIL_RESOURCES ();

  SOCK_LOG_FLOW1 (sockCtx, "%s: sockStart() successful", sockCtx->ctxName);

  return (rc);
  }

/************************************************************************/
/*                      sockEnd                                         */
/* This function  cleans up all resources aquired by the sockCtx and	*/
/* removes the sockCtx from the list.					*/
/* All context specific threads need to terminate before this function	*/
/* will return.								*/
/* Returns:								*/
/*	SD_SUCCESS (0)		context released			*/
/*	SD_FAILURE		otherwise				*/
/************************************************************************/

#if defined(_WIN32)
  #define MAX_THREAD_END_WAIT		INFINITE
#else  /* UNIX, Linux,... */
  #define MAX_THREAD_END_WAIT		-1	/* gs_thread_wait ignores this param (waits indefinitely)  */
#endif


ST_RET sockEnd (GEN_SOCK_CTXT *sockCtx)
  {
GEN_SOCK_CTRL *serviceCtrl;
#if !defined(GENSOCK_THREAD_SUPPORT)
GEN_SOCK *pSock;
#endif
#if defined(GENSOCK_THREAD_SUPPORT)
ST_RET rc;
#endif

  if (!sockCtx)
    return (SD_SUCCESS);
    
  SOCK_LOG_FLOW1 (sockCtx, "%s: sockEnd() pending...", sockCtx->ctxName);

  sockCtx->ctxStatus = GEN_SOCK_CTX_TERMINATING;	/* prevent adding new sockets to serviceCtrl(s) */ 

/* First let's terminate terminate all service threads and the free thread */
#if defined(GENSOCK_THREAD_SUPPORT)
  /* Set flag for service threads to terminate, then wake them */
  sockCtx->gTerminateService = SD_TRUE;
  sockServiceWakeAll (sockCtx);

  /* Wait for all service threads to terminate */
  serviceCtrl = sockCtx->sockServiceList;
  while (serviceCtrl != NULL)
    {
    if (serviceCtrl->tIdService)
      {
      rc = gs_wait_thread (serviceCtrl->thService, serviceCtrl->tIdService, MAX_THREAD_END_WAIT);
      rc = gs_close_thread (serviceCtrl->thService);
	serviceCtrl->tIdService = 0;
      }
    S_LOCK_UTIL_RESOURCES ();
    list_unlink (&sockCtx->sockServiceList, serviceCtrl);
    S_UNLOCK_UTIL_RESOURCES ();
    if (serviceCtrl->serviceEvent)
      gs_free_event_sem (serviceCtrl->serviceEvent);
    CLOSE_SOCKET (serviceCtrl->xCallingSock);
    CLOSE_SOCKET (serviceCtrl->xCalledSock);
    chk_free (serviceCtrl);
    serviceCtrl = sockCtx->sockServiceList;	/* remove from front */
    }

#else	/* !defined(GENSOCK_THREAD_SUPPORT)	*/
  /* Polled mode */
  /* Clean up the service controls */
  serviceCtrl = sockCtx->sockServiceList;
  while (serviceCtrl != NULL)
    {
    /* Close all sockets associated with the service control */
    while (serviceCtrl->sockList)
      {
      pSock = serviceCtrl->sockList;
      pSock->disconnectReason =  GS_DISCONNECT_TERMINATING;
      sockClose (pSock);
      }

    list_unlink (&sockCtx->sockServiceList, serviceCtrl);
    chk_free (serviceCtrl);
    serviceCtrl = sockCtx->sockServiceList;	/* remove from front */
    }
#endif	/* !defined(GENSOCK_THREAD_SUPPORT)	*/

  SOCK_LOG_FLOW1 (sockCtx, "%s: sockEnd() complete", sockCtx->ctxName);

  /* delete the context */
  S_LOCK_UTIL_RESOURCES ();
  list_unlink (&sockCtxList, sockCtx);
  S_UNLOCK_UTIL_RESOURCES ();

  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*                      sockInitCalling                                 */
/************************************************************************/

ST_RET sockInitCalling (GEN_SOCK_CTXT *sockCtx, GEN_SOCK_CONFIG *sockCfg,
                        ST_UINT16 portNo, ST_CHAR *ipAddr,
                        GEN_SOCK **pSockOut)
  {
GEN_SOCK *pSock;
SOCKET hSock;
struct hostent *pHost;
int sRet;
ST_RET rc;
int err;

  SOCK_LOG_FLOW2 (sockCtx, "in sockInitCalling() to ipAddr=%s portNo=%u",
                  ipAddr, (ST_UINT) portNo);

  if (sockCtx->ctxStatus == GEN_SOCK_CTX_TERMINATING)
    {
    /* prevent adding new sockets to serviceCtrl */ 
    SOCK_LOG_NERR1 (sockCtx, "sockInitCalling failed in context '%s' is terminating", sockCtx->ctxName);
    return (SD_FAILURE);
    }

  hSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (hSock == INVALID_SOCKET)
    {
    err = SOCKET_ERRORNO;
    SOCK_LOG_ERR1 (sockCtx, "socket() returned INVALID_SOCKET, errno=%d.", err);
    return (err);
    }
#if !defined(_WIN32)
  if (hSock >= FD_SETSIZE)
    {	/* Can't use this socket because illegal to use in "select" call*/
    SOCK_LOG_ERR1 (sockCtx, "Socket: socket num %d >= FD_SETSIZE (can not be used in select),"
                            " closing socket.", hSock);
    CLOSE_SOCKET (hSock);
    return (SD_FAILURE);
    }
#endif

  pSock = _sockAllocSock (sockCtx, GS_ROLE_CALLING, GS_STATE_CONNECTING, hSock, sockCfg);
  SOCK_LOG_FLOW1 (sockCtx, "%s: socket initialized", pSock->sockIdStr);
  *pSockOut = pSock;

/* Set up the socket address */
  if (strchr(ipAddr, '.') != NULL)      /* dotted address format */
    {
    pSock->sockAddrIn.sin_family = AF_INET;
    pSock->sockAddrIn.sin_addr.s_addr = inet_addr (ipAddr);
    }
  else          /* host address format, get dotted address */
    { 
    pHost = gethostbyname (ipAddr);
    if (pHost == NULL)
      {
      err = SOCKET_ERRORNO;
      SOCK_LOG_NERR2 (sockCtx, "%s: gethostbyname() failed for host='%s'", pSock->sockIdStr, ipAddr);
      CLOSE_SOCKET (pSock->hSock);
      chk_free (pSock);
      return (err);
      }
    pSock->sockAddrIn.sin_family = pHost->h_addrtype;
    pSock->sockAddrIn.sin_addr.s_addr = ((struct in_addr *)(pHost->h_addr))->s_addr;
    }
  pSock->sockAddrIn.sin_port = htons (portNo);

/* Do non-blocking connect */
  sRet = connect (pSock->hSock, (struct sockaddr *)&pSock->sockAddrIn, 
                  sizeof(pSock->sockAddrIn));
  if (sRet >= 0)
    {
    /* DEBUG: This code probably never executes because socket is non-blocking.	*/
    SOCK_LOG_FLOW1 (sockCtx, "%s: socket connected", pSock->sockIdStr);
    pSock->sockState = GS_STATE_CONNECTED;
    pSock->sockStats.activeTime = time (NULL);

    /* add new socket to service list and then call uSockConnect fun,		*/
    /* the sockAddSock may start new thread that will receive data before	*/
    /* the uSockConnect is called, LOCK is needed to prevent this scenario	*/
    S_LOCK_UTIL_RESOURCES ();
    rc = _sockAddSock (pSock);       		/* Add to a service list	*/
    if (rc == SD_SUCCESS)
      {
      if (pSock->sockCfg.secConnectProc == NULL)
        /* non-secure socket connected */
        rc = (*pSock->sockCfg.uSockConnectConf)(pSock);
      else
        /* for socket connection to be secured the SSL Engine function needs to be	*/
        /* called to start negotiating TLS session					*/
        rc = (*pSock->sockCfg.secConnectProc)(pSock);

      if (rc != SD_SUCCESS)
        {
        /* user does not want this connection (or SSLE fun failure or license expired) */
        pSock->usrCloseCalled = SD_TRUE;	/* don't call uSockDisconnect */
        sockClose (pSock);
        sockFree (pSock);
        /* DEBUG EJV: added this return, otherwise SD_SUCCESS was returned	*/
        S_UNLOCK_UTIL_RESOURCES ();
        return (SD_FAILURE);
        }
      }
    else
      {
      SOCK_LOG_FLOW1 (sockCtx, "%s: closing socket connections", pSock->sockIdStr);
      CLOSE_SOCKET (pSock->hSock);
      chk_free (pSock);	
      /* DEBUG EJV: added this return, otherwise SD_SUCCESS was returned	*/
      S_UNLOCK_UTIL_RESOURCES ();
      return (SD_FAILURE);
      }
    S_UNLOCK_UTIL_RESOURCES ();
    }
  else
    {
    err = SOCKET_ERRORNO;
    if (err == SOCK_WOULDBLOCK || err == SOCK_INPROGRESS)
      {
      rc = _sockAddSock (pSock);     /* Add to a service list */
      if (rc != SD_SUCCESS)
        {
	/* since socket is not connected we can use the system socket close	*/
        CLOSE_SOCKET (pSock->hSock);
        chk_free (pSock);
        return (SD_FAILURE);
	}
      }
    else
      {
      SOCK_LOG_NERR4 (sockCtx, "%s: connect() to host=%s failed, port=%d, errno=%d",
                      pSock->sockIdStr, ipAddr, portNo, err);
      CLOSE_SOCKET (pSock->hSock);
      chk_free (pSock);
      return (SD_FAILURE);
      }
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*                      sockInitListen                                  */
/* Returns:								*/
/*	SD_SUCCESS (0)		listen socket initialized		*/
/*	SD_FAILURE or errno	otherwise				*/
/************************************************************************/

ST_RET sockInitListen (GEN_SOCK_CTXT *sockCtx, GEN_SOCK_CONFIG *sockCfg,
                       ST_UINT16 portNo, ST_INT maxActive,
                       GEN_SOCK **pListenSockOut)
  {
ST_RET rc;
GEN_SOCK *pListenSock;
ST_INT    listenBacklog;
int sRet;
SOCKET hSock;
struct sockaddr_in localAddr;
int err;


  if (sockCtx->ctxStatus == GEN_SOCK_CTX_TERMINATING)
    {
    /* prevent adding new sockets to serviceCtrl */ 
    SOCK_LOG_NERR1 (sockCtx, "%s: sockInitListen() failed, context is terminating", sockCtx->ctxName);
    return (SD_FAILURE);
    }

  hSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (hSock == INVALID_SOCKET)
    {
    err = SOCKET_ERRORNO;
    SOCK_LOG_ERR2 (sockCtx, "%s: socket() failed for listen socket, errno=%d", sockCtx->ctxName, err);
    return (err);
    }
#if !defined(_WIN32)
  if (hSock >= FD_SETSIZE)
    {	/* Can't use this socket because illegal to use in "select" call*/
    SOCK_LOG_ERR2 (sockCtx, "%s: socket() returned socket num %d >= FD_SETSIZE "
                            "(can not be used in select), closing socket.", sockCtx->ctxName, hSock);
    CLOSE_SOCKET (hSock);
    return (SD_FAILURE);
    }
#endif

  pListenSock = _sockAllocSock (sockCtx, GS_ROLE_LISTENING, GS_STATE_LISTENING, 
                                hSock, sockCfg);
  SOCK_LOG_FLOW1 (sockCtx, "%s: initializing listen socket", pListenSock->sockIdStr);
  *pListenSockOut = pListenSock;
  pListenSock->maxActive = maxActive;

  memset((char *)(&localAddr), 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(portNo);
  //localAddr.sin_addr.s_addr = htonl (INADDR_ANY);
  //SK+20191201
  if (!strlen(sk_bind_address))
	  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  else	//SK+20191201
	  localAddr.sin_addr.s_addr = inet_addr(sk_bind_address);
  sRet = bind (pListenSock->hSock, (struct sockaddr *)&localAddr, sizeof(localAddr));
  if (sRet != 0)
    {
    err = SOCKET_ERRORNO;
    SOCK_LOG_NERR4 (sockCtx, "%s: listen socket bind() error on port=%u, errno=%d %s", 
                    pListenSock->sockIdStr, (ST_UINT) portNo, err,
                    (err == SOCK_EADDRINUSE) ? "(port in use)" : " ");
    CLOSE_SOCKET (pListenSock->hSock);
    chk_free (pListenSock);
    return (err);
    }
  SOCK_LOG_FLOW1 (sockCtx, "%s: listen socket bind() OK", pListenSock->sockIdStr);

  if (pListenSock->sockCfg.listenBacklog == 0)
    listenBacklog = SOMAXCONN;
  else
    listenBacklog = pListenSock->sockCfg.listenBacklog;

  sRet = listen (pListenSock->hSock, listenBacklog);
  if (sRet != 0)
    {
    err = SOCKET_ERRORNO;
    SOCK_LOG_NERR2 (sockCtx, "%s: listen() failed, errno=%d", pListenSock->sockIdStr, err);
    CLOSE_SOCKET (pListenSock->hSock);
    chk_free (pListenSock);
    return (err);
    }

  /* Add to a service list */
  rc = _sockAddSock (pListenSock);
  if (rc != SD_SUCCESS)
    {
    CLOSE_SOCKET (pListenSock->hSock);
    chk_free (pListenSock);
    }
  else
    SOCK_LOG_FLOW2 (sockCtx, "%s: listen started on port=%d", pListenSock->sockIdStr, portNo);

  return (rc);
  }

/************************************************************************/
/*			sockTxMsg					*/
/* Send complete message. Some data may be queued for later delivery.	*/
/* "sockTxQueueProc" must be called periodically to send any data that	*/
/* has been queued up.							*/
/************************************************************************/
ST_RET sockTxMsg (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData)
  {
ST_RET retVal;

  retVal = sockTxQueueAdd (pSock, sockData, SD_FALSE);	/* queue data	*/
  if (retVal==SD_SUCCESS)
    retVal = sockTxQueueProc (pSock);	/* send what we can from queue	*/
  return (retVal);
  }

/************************************************************************/
/*			sockTxQueueAdd					*/
/* Queue data for later delivery.					*/
/* "sockTxQueueProc" must be called periodically to send any data that	*/
/* has been queued up.							*/
/************************************************************************/
ST_RET sockTxQueueAdd (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData, ST_BOOLEAN first)
  {
GEN_SOCK_CTXT *sockCtx;
ST_RET retVal;

  sockCtx = pSock->sockCtx;

/* Put on our pending send list */

  S_LOCK_UTIL_RESOURCES ();
  if (first)
    retVal = list_add_first (&pSock->sockTxPend, sockData);
  else
    retVal = list_add_last (&pSock->sockTxPend, sockData);
  if (retVal == SD_SUCCESS)
    {
    if (sockData->eot)
      ++pSock->sockTxQueueGroupCnt;
    }
  else
    /* user expects that sockTxMsg will free the sockData buff in any case */
    (*pSock->sockCfg.uSockTxBufFree) (pSock, sockData);
  S_UNLOCK_UTIL_RESOURCES ();

  return (retVal);
  }

/************************************************************************/
/*			sockTxQueueDestroy				*/
/* Free all queued messages.						*/
/************************************************************************/
ST_RET sockTxQueueDestroy (GEN_SOCK *pSock)
  {
GEN_SOCK_CTXT *sockCtx;
GEN_SOCK_DATA *sockData;

  sockCtx = pSock->sockCtx;

/* Free all queued messages from list */
  S_LOCK_UTIL_RESOURCES ();
  while (pSock->sockTxPend != NULL)
    {
    sockData = pSock->sockTxPend;
    list_unlink (&pSock->sockTxPend, sockData);
    (*pSock->sockCfg.uSockTxBufFree) (pSock, sockData);
    }
  pSock->sockTxQueueGroupCnt = 0;
  S_UNLOCK_UTIL_RESOURCES ();

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sockTxQueueProc					*/
/* Must be called periodically to send any data queued up by calls to	*/
/* "sockTxQueueAdd".							*/
/************************************************************************/
ST_RET sockTxQueueProc (GEN_SOCK *pSock)
  {
GEN_SOCK_CTXT *sockCtx;
GEN_SOCK_DATA *sockData;
ST_INT numSent;
ST_RET rc = SD_SUCCESS;

  sockCtx = pSock->sockCtx;

  S_LOCK_UTIL_RESOURCES ();

/* Send as much data as possible */
  while (pSock->sockTxPend != NULL)
    {
    sockData = pSock->sockTxPend;
    rc = sockTx (pSock, sockData, &numSent);
    if (rc == SD_SUCCESS)
      {
      if (numSent == sockData->dataLen)
        {
      /* OK, we were able to send the rest of the buffer */
        ++pSock->sockStats.numSend;		/* num of RFC1006 msgs sent	*/
        list_unlink (&pSock->sockTxPend, sockData);
        if (sockData->eot)
	  {
          if (pSock->sockTxQueueGroupCnt > 0 )
            --pSock->sockTxQueueGroupCnt;	/* tx last msg from a group	*/
	  else
            SOCK_LOG_ERR1 (sockCtx, "%s: tx queue count invalid.", pSock->sockIdStr);
	  }
        (*pSock->sockCfg.uSockTxBufFree) (pSock, sockData);
        }
      else
        {
      /* Prepare for next send attempt */
        sockData->data += numSent;
        sockData->dataLen -= numSent;

      /* Stop trying to send */
        break;
        }
      }
    else	/* Send error */
      {
      SOCK_LOG_NERRC1 (sockCtx, "%s: sockTx() failed, emptying send queue.", pSock->sockIdStr);
    /* Free all queued send data ... */
      sockTxQueueDestroy (pSock);
      break;
      }
    }
  S_UNLOCK_UTIL_RESOURCES ();
  return (rc);
  }

/************************************************************************/
/*			sockTxQueueGroupCntGet				*/
/************************************************************************/
ST_UINT sockTxQueueGroupCntGet (GEN_SOCK *pSock)
  {
GEN_SOCK_CTXT *sockCtx;
ST_UINT queCnt;

  sockCtx = pSock->sockCtx;

  S_LOCK_UTIL_RESOURCES ();
  queCnt = pSock->sockTxQueueGroupCnt;
  S_UNLOCK_UTIL_RESOURCES ();

  return (queCnt);
  }

/************************************************************************/
/************************************************************************/
/*			sockTx						*/
/************************************************************************/

ST_RET sockTx (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData, ST_INT *numSentOut)
  {
GEN_SOCK_CTXT *sockCtx;
ST_RET ret;

  sockCtx = pSock->sockCtx;

  if (pSock->sockState != GS_STATE_CONNECTED)
    {
    SOCK_LOG_NERR2 (sockCtx, "%s: sockTx() failed, socket not connected sockState=%d",
                     pSock->sockIdStr, pSock->sockState);
    return (SD_FAILURE);
    }

  if (pSock->sockCfg.secTxProc == NULL)
    ret = _sockTx (pSock, sockData, numSentOut);
  else
    ret = (*pSock->sockCfg.secTxProc) (pSock, sockData, numSentOut);

  return (ret);
  }

/************************************************************************/
/*			_sockTx						*/
/************************************************************************/

ST_RET _sockTx (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData, ST_INT *numSentOut)
  {
GEN_SOCK_CTXT *sockCtx;
ST_INT numSent;
ST_INT err;

  sockCtx = pSock->sockCtx;

#ifdef SOCK_FLOW_PRINT	/* DEBUG: enable this to see data flow.	*/
  printf ("|");
#endif
  SOCK_LOG_FLOW2 (sockCtx, "%s: _sockTx() request to send %d bytes",
                  pSock->sockIdStr, sockData->dataLen);
  numSent = (ST_INT) send (pSock->hSock, sockData->data, sockData->dataLen, 0);
  if (numSent >= 0)  /* Success ? */
    {
    SOCK_LOG_FLOWC3 (sockCtx, "%s: _sockTx() sent %d vs. %d requested",
                     pSock->sockIdStr, numSent, sockData->dataLen);
    if (numSent < sockData->dataLen)
      {
      pSock->sockCfg.chkWritable = SD_TRUE;
      sockServiceWake (pSock->serviceCtrl);
      }
    *numSentOut = numSent;
    /* save the time of last successful data transfer activity on the socket */
    if (numSent > 0 && sockCtx->maxIdleTime > 0)
      pSock->sockStats.lastActivityTime = sGetMsTime();
    return (SD_SUCCESS);
    }
  else
    {
    err = SOCKET_ERRORNO;
    switch (err)
      {
      case SOCK_WOULDBLOCK:
      case SOCK_NOBUFS:
      case SOCK_INTR:
      case SOCK_INPROGRESS:
      case SOCK_TIMEDOUT:
        SOCK_LOG_FLOWC2 (sockCtx, "%s: _sockTx sent 0, error (%d), no send buffers?",
                         pSock->sockIdStr, err);
        pSock->sockCfg.chkWritable = SD_TRUE;
        sockServiceWake (pSock->serviceCtrl);
        *numSentOut = 0;
        return (SD_SUCCESS);
      break;
  
      default:
        SOCK_LOG_NERR2 (sockCtx, "%s: socket send() error (%d)", pSock->sockIdStr, err);
      break;
      }
    }

  S_LOCK_UTIL_RESOURCES ();
  if (pSock->sockState == GS_STATE_CONNECTED)
    {
    pSock->disconnectReason = GS_DISCONNECT_SEND_FAILURE;
    sockClose (pSock);  /* Close and wake the service thread */
    }
  S_UNLOCK_UTIL_RESOURCES ();
  return (SD_FAILURE);
  }

/************************************************************************/
/*                      sockClose                                       */
/************************************************************************/

ST_RET sockClose (GEN_SOCK *pSock)
  {
GEN_SOCK_CTXT *sockCtx;


  sockCtx = pSock->sockCtx;
  SOCK_LOG_FLOW1 (sockCtx, "%s: in sockClose()", pSock->sockIdStr);

  S_LOCK_UTIL_RESOURCES ();
  if (pSock->sockState != GS_STATE_CLOSED)
    {
    pSock->sockState = GS_STATE_CLOSED;
  
  /* On AIX the close() will not return until select() returns.	*/
  /* Wake the service thread (or terminate the select) */
  /* NOTE: serviceCtrl may be NULL. Functions called here must check it.*/
    sockServiceWake (pSock->serviceCtrl);

    CLOSE_SOCKET (pSock->hSock);
    pSock->hSock = INVALID_SOCKET; /* make sure handle not used anymore */

    /* Discard all events for this socket */
    sockEventQueueFlush (sockCtx, pSock);

    /* Done with this socket. Remove it from service list.	*/
    _sockServiceDiscardSock (pSock->serviceCtrl, pSock);
    }
  S_UNLOCK_UTIL_RESOURCES ();
  return SD_SUCCESS;
  }

/************************************************************************/
/*                      sockFree                                        */
/************************************************************************/
/* Must be called after (or from) disconnect callback */

ST_RET sockFree (GEN_SOCK *pSock)
  {
GEN_SOCK_CTXT *sockCtx;

  sockCtx = pSock->sockCtx;
  SOCK_LOG_FLOW1 (sockCtx, "%s: in sockFree()", pSock->sockIdStr);

  if (pSock->sockState != GS_STATE_CLOSED)
    {
    SOCK_LOG_NERR1 (sockCtx, "%s: socket state not closed, can't free", pSock->sockIdStr);
    return (SD_FAILURE);
    }

  if (pSock->sockCfg.secSockFree != NULL)
    (*pSock->sockCfg.secSockFree) (pSock);

  /* We can free this socket if:                                      */
  /*  1. It is not a listen socket, or                                */
  /*  2. It is a listen socket with no called socket references       */

  /* protect numActive	*/
  S_LOCK_UTIL_RESOURCES ();
  /* release any queued data buffers */
  sockTxQueueDestroy (pSock);
  if (pSock->role == GS_ROLE_LISTENING && pSock->numActive != 0)
    {
    /* can  happen when exiting */
    SOCK_LOG_NERR2 (sockCtx, "%s: Can't free listening GEN_SOCK " S_FMT_PTR " (pointer still in use)",
                    pSock->sockIdStr, pSock);
    }
  else
    {
    SOCK_LOG_FLOW2 (sockCtx, "%s: freeing GEN_SOCK " S_FMT_PTR ".", pSock->sockIdStr, pSock);
    chk_free (pSock);
    }
  S_UNLOCK_UTIL_RESOURCES ();
  return SD_SUCCESS;
  }

/************************************************************************/
/*                      _sockAddServiceCtrl                              */
/************************************************************************/

static GEN_SOCK_CTRL *_sockAddServiceCtrl (GEN_SOCK_CTXT *sockCtx)
  {
GEN_SOCK_CTRL *serviceCtrl;

  serviceCtrl = (GEN_SOCK_CTRL *) chk_calloc (1, sizeof (GEN_SOCK_CTRL));
  serviceCtrl->sockCtx = sockCtx;
  S_LOCK_UTIL_RESOURCES ();
  list_add_last (&sockCtx->sockServiceList, serviceCtrl);
  S_UNLOCK_UTIL_RESOURCES ();
  return (serviceCtrl);
  }


/************************************************************************/
/*			sockServiceWakeAll				*/
/************************************************************************/

ST_VOID sockServiceWakeAll (GEN_SOCK_CTXT *sockCtx)
  {
#if defined(GENSOCK_THREAD_SUPPORT)
GEN_SOCK_CTRL *serviceCtrl;

  serviceCtrl = sockCtx->sockServiceList;
  while (serviceCtrl != NULL)
    {
    sockServiceWake (serviceCtrl);
    serviceCtrl = (GEN_SOCK_CTRL *) list_get_next (sockCtx->sockServiceList, serviceCtrl);  
    }
#endif
  }

/************************************************************************/
/*                      sockServiceWake                          */
/************************************************************************/

ST_VOID sockServiceWake (GEN_SOCK_CTRL *serviceCtrl)
  {
#if defined(GENSOCK_THREAD_SUPPORT)
GEN_SOCK_CTXT *sockCtx;
ST_INT numSent;
static ST_UCHAR wakeupData = 1;

  if (!serviceCtrl)
    return;
  sockCtx = serviceCtrl->sockCtx;

/* Be sure it was not set NULL */
  if (serviceCtrl == NULL)
    return;

  if (serviceCtrl->serviceEvent)
    gs_signal_event_sem (serviceCtrl->serviceEvent);
  if (serviceCtrl->xCalledSock == 0)
    return;

/* OK, there is a wakeup port for this sock service */
  numSent = (ST_INT) send (serviceCtrl->xCalledSock, &wakeupData, sizeof(wakeupData), 0);
  if (numSent == sizeof (wakeupData))
    {
    SOCK_LOG_FLOW2 (sockCtx, "%s: XSocket sent %d wakeup bytes", sockCtx->ctxName, numSent);
    }
  else
    {
    /* when a lot of packets are received in short time, EWOULDBLOCK and EAGAIN	*/
    /* errors could happen often with this 1-byte packets			*/
    SOCK_LOG_FLOW3 (sockCtx, "%s: XSocket wakeup data send() error, bytes sent=%d, errno=%d",
                    sockCtx->ctxName, numSent, SOCKET_ERRORNO);
    }
  ++wakeupData;
#endif
  }


/************************************************************************/
/*			sockCheckForIdleConns				*/
/*----------------------------------------------------------------------*/
/* Check for idle connections if the tcpMaxIdleTime >0.			*/
/* Calling/called connections are closed if there is no activity tx/rx	*/
/* for this period of time. The checking will be performed with the	*/
/* frequency of 30 seconds.						*/
/************************************************************************/
#define GS_IDLE_CON_CHECK_FREQENCY	((ST_DOUBLE)(30*1000))   /* ms	*/
static ST_RET sockCheckForIdleConns (GEN_SOCK_CTRL *serviceCtrl)
  {
ST_DOUBLE    currTime;
ST_UINT      maxIdleTime;
GEN_SOCK    *pSock;
GEN_SOCK    *pSockNext;
GEN_SOCK_CTXT *sockCtx;

  if (!serviceCtrl || !serviceCtrl->sockCtx)
    return (SD_FAILURE);

  sockCtx = serviceCtrl->sockCtx;
  maxIdleTime = sockCtx->maxIdleTime;
  if (maxIdleTime == 0)
    /* user do not want to check for idle TCP connections	*/
    return (SD_SUCCESS);

  if (serviceCtrl->lastCheckIdleTime == 0)
    {
    /* starting, initialize the lastCheckedTime */
    serviceCtrl->lastCheckIdleTime = sGetMsTime ();
    return (SD_SUCCESS);
    }
  currTime = sGetMsTime ();
  if ((currTime - serviceCtrl->lastCheckIdleTime) < GS_IDLE_CON_CHECK_FREQENCY)
    return (SD_SUCCESS);

  /* it is time to check the list of sockets for idle connections */
  S_LOCK_UTIL_RESOURCES ();	/* CRITICAL: lock all access to sockList*/
  pSock = serviceCtrl->sockList;
  while (pSock)
    {
    pSockNext = (GEN_SOCK *) list_get_next (serviceCtrl->sockList, pSock);
    if (pSock->role != GS_ROLE_LISTENING)
      {
      if ((ST_UINT) (currTime - pSock->sockStats.lastActivityTime) >=  maxIdleTime)
        {
        SOCK_LOG_NERR2 (sockCtx, "%s: closing idle socket, maxIdleTime=%u seconds reached.",
                        pSock->sockIdStr, maxIdleTime/1000);
        sockClose (pSock);
        }
      }
    pSock = pSockNext;
    }
  S_UNLOCK_UTIL_RESOURCES ();

  serviceCtrl->lastCheckIdleTime = currTime;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sockCtrlService					*/
/************************************************************************/

ST_INT sockCtrlService (GEN_SOCK_CTRL *serviceCtrl, ST_LONG timeOut)
  {
GENSOCK_FD_SET sockFds;
struct timeval selectTimeout;
int nfds;
ST_INT retVal;

  /* CRITICAL: start with clean sockFds.	*/
  sockFds.totalfds = 0;
  
  sockFds.selectnfds = 0;
  sockFds.numReadfds = 0;
  sockFds.numWritefds = 0;
  sockFds.numExceptfds = 0;
  FD_ZERO (&sockFds.readfds);
  FD_ZERO (&sockFds.writefds);
  FD_ZERO (&sockFds.exceptfds);

/* Build the set of sockets to listen to */
  sockInitFds (serviceCtrl, &sockFds);
  if (sockFds.totalfds == 0)
    return (GS_SELECT_NO_ACTIVE_SOCK);

/* Now wait for some activity */
  selectTimeout.tv_sec  = timeOut / 1000;
  selectTimeout.tv_usec = (timeOut % 1000) * 1000;
  nfds = select (sockFds.selectnfds, &sockFds.readfds, &sockFds.writefds, &sockFds.exceptfds, &selectTimeout);

#if (defined(GENSOCK_THREAD_SUPPORT) && defined(__VMS))
  if (nfds > 0)
    {
    sys$cantim(10000, 0); /* that is sys$cantim(MICS_AST_WAITANYEVENT, 0); */
    sys$setef (101);      /* that is sys$setef (MICS_FLAG_ACTIVITY)        */
    }
#endif

#if defined(GENSOCK_THREAD_SUPPORT)
  {
  GEN_SOCK_CTXT *sockCtx = serviceCtrl->sockCtx;
/* Make sure we are not supposed to terminate .. */
  if (sockCtx->gTerminateService == SD_TRUE)
    return (GS_SELECT_TERMINATED);
  }
#endif

  retVal = sockSelectResultProcess (serviceCtrl, nfds, &sockFds);

  /* check if there are idle sockets before we return */
  sockCheckForIdleConns (serviceCtrl);

  return (retVal);
  }


#if !defined(GENSOCK_THREAD_SUPPORT)
/************************************************************************/
/*			sockInitAllFds					*/
/* Initialize the GENSOCK_FD_SET struct for "all" sockets, so that	*/
/* "select" call can include all sockets.				*/
/* Assume there is only one entry on the linked list "sockCtx->sockServiceList"	*/
/* and just use it.							*/
/************************************************************************/
ST_VOID sockInitAllFds (GENSOCK_FD_SET *sockFds)
  {
GEN_SOCK_CTXT *sockCtx;


  /* CRITICAL: start with clean sockFds.	*/
  sockFds->totalfds = 0;
  sockFds->selectnfds = 0;
  sockFds->numReadfds = 0;
  sockFds->numWritefds = 0;
  sockFds->numExceptfds = 0;
  FD_ZERO (&sockFds->readfds);
  FD_ZERO (&sockFds->writefds);
  FD_ZERO (&sockFds->exceptfds);

  sockCtx = sockCtxList;
  while (sockCtx)
    {
    assert (sockCtx->sockServiceList->l.next == (DBL_LNK *) sockCtx->sockServiceList);	/* make sure only one on list*/
    sockInitFds (sockCtx->sockServiceList, sockFds);

    sockCtx = (GEN_SOCK_CTXT *)list_get_next (sockCtxList, sockCtx);
    }
  }
#endif /* !defined(GENSOCK_THREAD_SUPPORT) */

/************************************************************************/
/*			sockInitFds					*/
/************************************************************************/

ST_VOID sockInitFds (GEN_SOCK_CTRL *serviceCtrl, GENSOCK_FD_SET *sockFds)
  {
GEN_SOCK *pSock;
GEN_SOCK *pNextSock;						       
GEN_SOCK_CONFIG *sockCfg;
GEN_SOCK_CTXT *sockCtx;

  sockCtx = serviceCtrl->sockCtx;

#if defined(GENSOCK_THREAD_SUPPORT)
  if (serviceCtrl->xCallingSock)
    sockAddReadFds (sockFds, serviceCtrl->xCallingSock);
#endif

  S_LOCK_UTIL_RESOURCES ();
  pSock = serviceCtrl->sockList;
  while (pSock)
    {
    pNextSock = (GEN_SOCK *) list_get_next (serviceCtrl->sockList, pSock);  

    if (pSock->sockState == GS_STATE_CONNECTED)
      {
      sockCfg = &pSock->sockCfg;
      if (sockCfg->pauseRecv == SD_FALSE)
        sockAddReadFds (sockFds, pSock->hSock);
      if (sockCfg->chkWritable)
        sockAddWriteFds (sockFds, pSock->hSock);
      }
    else if (pSock->sockState == GS_STATE_CONNECTING)
      {
      sockAddWriteFds (sockFds, pSock->hSock);
      sockAddExceptFds (sockFds, pSock->hSock);
      }
    else if (pSock->sockState == GS_STATE_LISTENING)
      sockAddReadFds (sockFds, pSock->hSock);

    pSock = pNextSock;
    }
  S_UNLOCK_UTIL_RESOURCES ();
  }


/************************************************************************/
/*			sockAddReadFds					*/
/************************************************************************/

ST_VOID sockAddReadFds (GENSOCK_FD_SET *sockFds, SOCKET hSock)
  {
#if !defined(_WIN32)
  assert (hSock < FD_SETSIZE);	/* FD_SET assumes this is true	*/
#endif
  FD_SET (hSock, &sockFds->readfds);
#if !defined(_WIN32) /* Ignored for Winsock */
  if ((hSock+1) > (sockFds->selectnfds))
    sockFds->selectnfds = (hSock+1);
#endif

  ++sockFds->numReadfds;
  ++sockFds->totalfds;
  }

/************************************************************************/
/*			sockAddWriteFds					*/
/************************************************************************/

ST_VOID sockAddWriteFds (GENSOCK_FD_SET *sockFds, SOCKET hSock)
  {
#if !defined(_WIN32)
  assert (hSock < FD_SETSIZE);	/* FD_SET assumes this is true	*/
#endif
  FD_SET (hSock, &sockFds->writefds);
#if !defined(_WIN32) /* Ignored for Winsock */
  if ((hSock+1) > (sockFds->selectnfds))
    sockFds->selectnfds = (hSock+1);
#endif

  ++sockFds->numWritefds;
  ++sockFds->totalfds;
  }

/************************************************************************/
/*			sockAddExceptFds				*/
/************************************************************************/

ST_VOID sockAddExceptFds (GENSOCK_FD_SET *sockFds, SOCKET hSock)
  {
#if !defined(_WIN32)
  assert (hSock < FD_SETSIZE);	/* FD_SET assumes this is true	*/
#endif
  FD_SET (hSock, &sockFds->exceptfds);
#if !defined(_WIN32) /* Ignored for Winsock */
  if ((hSock+1) > (sockFds->selectnfds))
    sockFds->selectnfds = (hSock+1);
#endif

  ++sockFds->numExceptfds;
  ++sockFds->totalfds;
  }

/************************************************************************/
/*			sockSelectResultProcess				*/
/* NOTE: this function closes the socket on any error.			*/
/************************************************************************/

ST_INT sockSelectResultProcess (GEN_SOCK_CTRL *serviceCtrl, 
				int nfds, GENSOCK_FD_SET *sockFds)
  {
GEN_SOCK_CTXT *sockCtx;
GEN_SOCK *pSock;
GEN_SOCK *pNextSock;
GEN_SOCK_CONFIG *sockCfg;
ST_RET rc;
ST_INT startState;
ST_INT err;
ST_RET sockErrorDetected;
#if defined(GENSOCK_THREAD_SUPPORT)
ST_CHAR bitBucket[100];
ST_INT recvRet;
#endif

  sockCtx = serviceCtrl->sockCtx;

/* Check for timeout							*/
  if (nfds == 0)
    return (GS_SELECT_TIMEOUT);

/* Check for error (may happen when sockets are closed while in select)	*/
  if (nfds < 0)	
    {
    /* error	*/
    err = SOCKET_ERRORNO;
    if (err == SOCK_INPROGRESS || err == SOCK_INTR)
      return (GS_SELECT_ACTIVE);

#if defined(GENSOCK_THREAD_SUPPORT)

    if ((err == SOCK_NOTSOCK || err == SOCK_INTR)) /* ? SOCK_INTR is above */
      /* If one or more sockets were closed by another thread, this error is expected.*/
      SOCK_LOG_FLOW2 (sockCtx, "%s: select() call failed."
                      " Expected if one or more sockets closed by another thread, errno=%d",
                      sockCtx->ctxName, err);
    else
      SOCK_LOG_ERR2 (sockCtx, "%s: select() call failed, errno=%d", sockCtx->ctxName, err);
#else
    SOCK_LOG_ERR2 (sockCtx, "%s: select() call failed, errno=%d", sockCtx->ctxName, err);
#endif
    return (GS_SELECT_ERROR);
    }

  /* OK now we have got some data to read from at least one of the sockets */
  S_LOCK_UTIL_RESOURCES ();	/* CRITICAL: lock all access to sockList*/
  pSock = serviceCtrl->sockList;
  while (pSock && nfds > 0)
    {
    pNextSock = (GEN_SOCK *) list_get_next (serviceCtrl->sockList, pSock);  

    sockErrorDetected = SD_FALSE;
    sockCfg = &pSock->sockCfg;

    if (pSock->sockState == GS_STATE_CONNECTED)
      {
      /* Check for data or disconnect on this socket ...  */
      if (FD_ISSET (pSock->hSock, &sockFds->readfds))
        {       /* We have data or a disconnect */
        --nfds;

        startState = pSock->recvState;
        if (_sockCheckRxData (pSock) != SD_SUCCESS)
          sockErrorDetected = SD_TRUE;
        }

      /* See if we are checking for writability too */
      if (!sockErrorDetected && sockCfg->chkWritable && FD_ISSET (pSock->hSock, &sockFds->writefds))
	{
        SOCK_LOG_FLOW1 (sockCtx, "%s: socket is Writable", pSock->sockIdStr);
        sockCfg->chkWritable = SD_FALSE;	/* one-shot */
        if (sockCfg->secEnable == SD_FALSE)
          (*sockCfg->uSockWritable)(pSock);
	else
          (*sockCfg->secWritable)(pSock);
	}
      }
    else if (pSock->sockState == GS_STATE_CONNECTING)
      {  /* Check for activity on this socket ...  */
      /* NOTE: On UNIX, LINUX, QNX the writefds is used to indicate that the	*/
      /*       connection completed or failed. Must call the getsocketopt()	*/
      /*       to check the connection status!					*/
      /*       On Windows writefds set indicates successful connection		*/
      /*       (exceptfds is used to indicate failed connection).		*/
      if (FD_ISSET (pSock->hSock, &sockFds->writefds))
        {
#if !defined(_WIN32)
        SOCK_OPTLEN optlen;
        ST_INT sRet;
        ST_INT connError = 0;

        optlen = sizeof(connError);
        sRet = getsockopt (pSock->hSock, SOL_SOCKET, SO_ERROR, (ST_CHAR *) &connError, &optlen);
        if (sRet == 0)
          {
          if (connError != 0)
            {
            SOCK_LOG_FLOW2 (sockCtx, "%s: socket Connect failed. Error = %d", pSock->sockIdStr, connError);
            pSock->disconnectReason = GS_DISCONNECT_CONNECT_FAILED;
            sockErrorDetected = SD_TRUE;
            }
          }
        else
          {
          SOCK_LOG_ERR2 (sockCtx, "%s: getsockopt(...,SOL_SOCKET, SO_ERROR,...) failed, errno=%d.",
                         pSock->sockIdStr, SOCKET_ERRORNO);
          pSock->disconnectReason = GS_DISCONNECT_CONNECT_FAILED;
          sockErrorDetected = SD_TRUE;
          }
#endif /* !defined(_WIN32) */

        --nfds;

        if (!sockErrorDetected)
          {
          SOCK_LOG_FLOW1 (sockCtx, "%s: socket connected", pSock->sockIdStr);
          pSock->sockState = GS_STATE_CONNECTED;
          pSock->sockStats.activeTime = time (NULL);

          if (sockCfg->secConnectProc == NULL)
            /* non-secure socket connected */
            rc = (*sockCfg->uSockConnectConf)(pSock);
          else
	    /* for socket connection to be secured the SSL Engine function needs to be	*/
	    /* called to start negotiating TLS session					*/
            rc = (*sockCfg->secConnectProc)(pSock);

          if (rc == SD_SUCCESS)
            {
            /* Do nothing. Receive data on next call.	*/
            }
          else    /* User does not want */
            {
            if (!pSock->disconnectReason)
              pSock->disconnectReason = GS_DISCONNECT_USR_REFUSED;
            sockClose (pSock);   
            /* NOTE: for secure connection do NOT call sockFree() here because the	*/
            /* secDisconnectProc and uSockDisconnectProc must be processed first before	*/
            /* the pSock is deallocated							*/
            if (sockCfg->secConnectProc == NULL)
              sockFree (pSock);     /* Allow it to be free'd                */
	    }
          }
        }
      else if (FD_ISSET (pSock->hSock, &sockFds->exceptfds))
        {       /* Connect failed (WNIDOWS ONLY) */
        SOCK_OPTLEN optlen;
        ST_INT sRet;
        ST_INT connError;

        optlen = sizeof(connError);
        sRet = getsockopt (pSock->hSock, SOL_SOCKET, SO_ERROR, (ST_CHAR *) &connError, &optlen);
        if (sRet == 0)
          SOCK_LOG_FLOW2 (sockCtx, "%s: socket Connect Failed. Error = %d", pSock->sockIdStr, connError);
        else
          SOCK_LOG_ERR2 (sockCtx, "%s: getsockopt(...,SOL_SOCKET, SO_ERROR,...) failed, errno=%d.",
                         pSock->sockIdStr, SOCKET_ERRORNO);
        --nfds;
        pSock->disconnectReason = GS_DISCONNECT_CONNECT_FAILED;
        sockErrorDetected = SD_TRUE;
        }
      }
    else if (pSock->sockState == GS_STATE_LISTENING)
      {  /* Check for activity on this socket ...  */
      if (FD_ISSET (pSock->hSock, &sockFds->readfds))
        {
        --nfds;
        _sockAccept (pSock);
        }
      }
    else  /* Not really interesting             */
      { 
      if (FD_ISSET (pSock->hSock, &sockFds->readfds))
        --nfds;
      }

    if (sockErrorDetected == SD_TRUE)
      sockClose (pSock);	/* error on this socket, so close it	*/

    pSock = pNextSock;
    } /* While sockets and nfds */
  S_UNLOCK_UTIL_RESOURCES ();	/* CRITICAL: lock all access to sockList*/

#if defined(GENSOCK_THREAD_SUPPORT)
  /* See if someone woke us by sending us data */
  if (nfds > 0)
    {
    if (FD_ISSET (serviceCtrl->xCallingSock, &sockFds->readfds))
      {
    /* Non-blocking receive, dump data */
      recvRet = (ST_INT) recv (serviceCtrl->xCallingSock, bitBucket, sizeof (bitBucket), 0);
      /* log only if this is not an SLOGIPC xCallingSock (otherwise looping occurs)	*/
      if (memcmp (sockCtx->ctxName, SLOGIPC_NAME, strlen(SLOGIPC_NAME)) != 0)
        {
        if (recvRet > 0)
          {
          SOCK_LOG_FLOW2 (sockCtx, "%s: XSocket received %d wakeup bytes", sockCtx->ctxName, recvRet);
          }
        else if (recvRet == 0)
          {
          SOCK_LOG_NERR1 (sockCtx, "%s: XSocket disconnected detected by recv()", sockCtx->ctxName);
          }
        }
      }
    }
#endif

  return (GS_SELECT_ACTIVE);
  }

/************************************************************************/
/*                      _sockServiceDiscardSock                         */
/* NOTE: this must be called ONLY from sockClose.			*/
/************************************************************************/
/* Remove a pSock from a service control and allow it to be free'd      */

static ST_VOID _sockServiceDiscardSock (GEN_SOCK_CTRL *serviceCtrl, 
                                        GEN_SOCK *pSock)
  {
GEN_SOCK_CTXT *sockCtx;

  if (serviceCtrl == NULL)
    return;	/* serviceCtrl already freed. Can't do anything.	*/

  sockCtx = pSock->sockCtx;
  SOCK_LOG_FLOW2 (sockCtx, "%s: preparing to free GEN_SOCK " S_FMT_PTR ".", pSock->sockIdStr, pSock);

/* The service control is done with this socket, clean it up            */
/* See if we need to return a receive buffer to the user */
  if (pSock->recvState == RECV_STATE_DATA)
    { 
    pSock->sockData->result = SD_FAILURE;
    (*pSock->sockCfg.uSockRx)(pSock, pSock->sockData);
    }
  
  /* If the socket has not been closed, time to do so */
  /* protect numActive, numSock, sockList	*/
  S_LOCK_UTIL_RESOURCES ();

  /* Dec the active socket count for a called socket. Note that the     */
  /* listen socket will not be free'd until numActive = 0               */
  if (pSock->role == GS_ROLE_CALLED && pSock->listenSocket)
    --pSock->listenSocket->numActive;
  
  list_unlink (&serviceCtrl->sockList, pSock);
  pSock->serviceCtrl = NULL;
  --serviceCtrl->numSock;

  /* See if the user needs to be told that the socket is closed */
  if (pSock->usrCloseCalled == SD_FALSE)
    {
    pSock->usrCloseCalled = SD_TRUE;
    if (pSock->sockCfg.secDisconnectProc == NULL)
      {
      SOCK_LOG_FLOW1 (sockCtx, "%s: calling uSockDisconnect()",pSock->sockIdStr);
      (*pSock->sockCfg.uSockDisconnect)(pSock);
      }
    else
      {
      SOCK_LOG_FLOW1 (sockCtx, "%s: calling secDisconnectProc()", pSock->sockIdStr);
      (*pSock->sockCfg.secDisconnectProc)(pSock);
      }
    }
  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*                      _sockAccept                                    */
/************************************************************************/

static ST_RET _sockAccept (GEN_SOCK *pListenSock)
  {
GEN_SOCK_CTXT *sockCtx = pListenSock->sockCtx;
GEN_SOCK *pSock;
SOCKET hNewSock;
ST_BOOLEAN okToAccept;
ST_INT rc = SD_SUCCESS;
SOCK_ADDRLEN callingAddrLen;
SOCKADDR_IN callingAddr;
int err;

/* This accept should succeed immediately */
  callingAddrLen = sizeof (callingAddr);
  hNewSock = accept (pListenSock->hSock, 
                     (SOCKADDR *) &callingAddr, &callingAddrLen);
  if (hNewSock == INVALID_SOCKET)
    {
    err = SOCKET_ERRORNO;
    SOCK_LOG_NERR2 (sockCtx, "%s: socket accept() error (errno=%d)", pListenSock->sockIdStr, err);
    if (pListenSock->sockState == GS_STATE_LISTENING)
      {
      pListenSock->disconnectReason = GS_DISCONNECT_ACCEPT_FAILED;
      }
    return (SD_FAILURE);
    }

#if !defined(_WIN32)
  if (hNewSock >= FD_SETSIZE)
    {	/* Can't use this socket because illegal to use in "select" call*/
    SOCK_LOG_ERR2 (sockCtx, "%s: accept returned socket num %d >= FD_SETSIZE "
                            "(can not be used in select), closing socket.", pListenSock->sockIdStr, hNewSock);
    CLOSE_SOCKET (hNewSock);
    return (SD_FAILURE);
    }
#endif

  if (sockCtx->ctxStatus == GEN_SOCK_CTX_TERMINATING)
    {
    /* prevent adding new sockets to serviceCtrl */ 
    SOCK_LOG_FLOW1 (sockCtx, "%s: can't accept socket connection, context is terminating", pListenSock->sockIdStr);
    CLOSE_SOCKET (hNewSock);
    return (SD_FAILURE);
    }

  /* Got a connection, see if we are still listening ... */
  SOCK_LOG_FLOW4 (sockCtx, "%s: accepted connection (socket=%d) from IP Address=%s Port=%u",
                  pListenSock->sockIdStr, hNewSock,
                  inet_ntoa (callingAddr.sin_addr), (ST_UINT) ntohs (callingAddr.sin_port));
  /* protect numActive */
  S_LOCK_UTIL_RESOURCES ();
  if (pListenSock->maxActive != 0 && (pListenSock->numActive >= pListenSock->maxActive))
    okToAccept = SD_FALSE;
  else
    okToAccept = SD_TRUE;
  S_UNLOCK_UTIL_RESOURCES ();
  if (okToAccept == SD_FALSE)
    {
    SOCK_LOG_FLOW2 (sockCtx, "%s: cannot process new socket connection, reached maxActive=%d socket connections",
                    pListenSock->sockIdStr, pListenSock->maxActive);
    CLOSE_SOCKET (hNewSock);
    return (SD_FAILURE);
    }

/* OK, we have a go ... */
  pSock = _sockAllocSock (sockCtx, GS_ROLE_CALLED, GS_STATE_CONNECTED, 
                                hNewSock, &pListenSock->sockCfg);

  pSock->listenSocket = pListenSock;
  pSock->callingAddrLen = callingAddrLen;
  pSock->callingAddr = callingAddr;

  /* add new socket to service list and then call uSockConnect fun,		*/
  /* the sockAddSock may start new thread that will receive data before		*/
  /* the uSockConnect is called, LOCK is needed to prevent this scenario	*/
  S_LOCK_UTIL_RESOURCES ();
  rc = _sockAddSock (pSock);       		/* Add to a service list	*/
  if (rc == SD_SUCCESS)
    {
    if (pSock->sockCfg.secConnectProc == NULL)
      rc = (*pSock->sockCfg.uSockConnectInd)(pSock);
    else
      rc = (*pSock->sockCfg.secConnectProc)(pSock);
    if (rc != SD_SUCCESS)
      {
      /* user does not want this connection */
      pSock->usrCloseCalled = SD_TRUE;	/* don't call uSockDisconnect */
      sockClose (pSock);
      sockFree (pSock);
      }
    }
  else
    {
    SOCK_LOG_FLOW1 (sockCtx, "%s: closing socket connections", pSock->sockIdStr);
    CLOSE_SOCKET (hNewSock);
    chk_free (pSock);	
    }
  S_UNLOCK_UTIL_RESOURCES ();

  return (SD_SUCCESS);
  }

/************************************************************************/
/*                      _sockCheckRxData                                */
/* Receive data from the socket and check its validity.			*/
/* NOTE: caller should close the socket if this function returns error.	*/
/************************************************************************/

static ST_INT _sockCheckRxData (GEN_SOCK *pSock)
  {
ST_INT numRxd;
ST_INT totalSize;
ST_INT bodySize;
ST_INT huntState;
ST_RET rc;
ST_CHAR *rxDest;
ST_INT rxBufSize;
ST_BOOLEAN secDataLeft;
ST_BOOLEAN foundHeader;
GEN_SOCK_CTXT *sockCtx;

  sockCtx = pSock->sockCtx;
  secDataLeft = SD_FALSE;
  do
    {
    foundHeader = SD_FALSE;
    rxDest = pSock->recvBuf+pSock->recvCurrCount; 
    rxBufSize =  pSock->recvDoneCount - pSock->recvCurrCount; 
  
  /* Use a non-blocking recv to get what's there */
  /* See if we need to pass it to a receive processing subsystem */
    if (pSock->sockCfg.secRxProc == NULL)
      rc = _sockRecv (pSock, rxDest, rxBufSize, 0, &numRxd);
    else
      rc = (*pSock->sockCfg.secRxProc) (pSock, rxDest, rxBufSize, 0, 
      					&numRxd, &secDataLeft);

    if (rc == SD_SUCCESS)
      {
      if (numRxd)
        {
#ifdef SOCK_FLOW_PRINT	/* DEBUG: enable this to see data flow.	*/
        printf (".");
#endif
        SOCK_LOG_FLOW2 (sockCtx, "%s: received %d bytes", pSock->sockIdStr, numRxd);
        pSock->recvCurrCount += numRxd;
        if (pSock->recvDoneCount == pSock->recvCurrCount)
          { 
        /* We got all the data we wanted, see if it is the header or data */
          if (pSock->recvState == RECV_STATE_HUNT)
            {
            huntState = GENSOCK_HUNT_UNDERWAY;
            (*pSock->sockCfg.uSockHunt) (pSock, &huntState, 
                                         pSock->recvBuf, pSock->recvCurrCount, &bodySize);
            if (huntState == GENSOCK_HUNT_DONE)
              {
            /* OK, the hunt is over and we have the length of the buffer to read  */
              totalSize = pSock->sockCfg.hdrSize + bodySize;
              SOCK_LOG_FLOWC2 (sockCtx, "%s: hunt complete, expecting %d more bytes", pSock->sockIdStr, bodySize);
              (*pSock->sockCfg.uSockRxBufAlloc) (pSock, totalSize, &pSock->sockData);
              pSock->sockData->dataLen = totalSize;
              memcpy (pSock->sockData->data, pSock->recvBuf, pSock->sockCfg.hdrSize);

              if (bodySize)
                {        
                pSock->recvState = RECV_STATE_DATA;
                pSock->recvBuf = (ST_CHAR *) pSock->sockData->data + pSock->sockCfg.hdrSize;
                pSock->recvCurrCount = 0;
                pSock->recvDoneCount = bodySize;
	        foundHeader = SD_TRUE;
                }
	      else /* No body, just header */
	        {
                ++pSock->sockStats.numRecv;
  
              /* Give the data to the user... */
                pSock->sockData->result = SD_SUCCESS;
                (*pSock->sockCfg.uSockRx)(pSock, pSock->sockData);
        
              /* Now hunt for the next */
                pSock->recvState = RECV_STATE_HUNT;
                pSock->recvDoneCount = pSock->sockCfg.hdrSize;
                pSock->recvCurrCount = 0;
                pSock->recvBuf = pSock->hdrBuf;
		}
              }
            else if (huntState == GENSOCK_HUNT_RESET)
              {
              huntState = GENSOCK_HUNT_UNDERWAY;
              pSock->recvCurrCount = 0;
              }
            else if (huntState == GENSOCK_HUNT_DISCONNECT)
              {		/* beginning of packet is invalid	*/
              pSock->recvCurrCount = 0;
              rc = SD_FAILURE; 	/* this should cause an immediate disconnect*/
              }
            }
          else /* Not hunting, state is RECV_STATE_DATA */
            {
            SOCK_LOG_FLOWC1 (sockCtx, "%s: got body", pSock->sockIdStr);
            ++pSock->sockStats.numRecv;
  
          /* Give the data to the user... */
            pSock->sockData->result = SD_SUCCESS;
            (*pSock->sockCfg.uSockRx)(pSock, pSock->sockData);
        
          /* Now hunt for the next */
            pSock->recvState = RECV_STATE_HUNT;
            pSock->recvDoneCount = pSock->sockCfg.hdrSize;
            pSock->recvCurrCount = 0;
            pSock->recvBuf = pSock->hdrBuf;
            }
          } /* Got all expected data */
        } /* Got some data */
      }
    } while (rc == SD_SUCCESS && (secDataLeft == SD_TRUE || foundHeader == SD_TRUE));
  return (rc);
  }

/************************************************************************/
/*                      _sockRecv                                       */
/* Receive up to "maxRx" bytes from the socket.				*/
/* NOTE: caller should close the socket if this function returns error.	*/
/************************************************************************/

ST_RET _sockRecv (GEN_SOCK *pSock, ST_CHAR *dest, 
                         ST_INT maxRx, ST_INT flags, ST_INT *rxCountOut)
  {
ST_INT recvRet;
ST_RET retVal;
int err;
GEN_SOCK_CTXT *sockCtx;

  sockCtx = pSock->sockCtx;

/* All socketa are non-blocking, so recv will NOT block.	*/
  recvRet = (ST_INT) recv (pSock->hSock, dest, maxRx, flags);
  if (recvRet > 0)
    {
    *rxCountOut = recvRet;
    /* save the time of last successful data transfer activity on the socket */
    if (sockCtx->maxIdleTime > 0)
      pSock->sockStats.lastActivityTime = sGetMsTime();
    return (SD_SUCCESS);
    }

/* Not a clean receive, see if we have a disconnect */
  if (recvRet == 0)
    {
    pSock->disconnectReason = GS_DISCONNECT_CLOSED;
    SOCK_LOG_FLOW1 (sockCtx, "%s: Socket disconnect detected by recv()", pSock->sockIdStr);
    return (SD_FAILURE);
    }

/* Some kind of error */
  err = SOCKET_ERRORNO;
  switch (err)			/* See if recoverable */
    {	       
    case SOCK_WOULDBLOCK:	
    case SOCK_INTR:
    case SOCK_TIMEDOUT:
    case SOCK_INPROGRESS:	/* Seen for Solaris 8 */
      *rxCountOut = 0;
      retVal = SD_SUCCESS;
    break;

    default:
      SOCK_LOG_FLOW2 (sockCtx, "%s: Socket error detected by recv() errno=%d", pSock->sockIdStr, err);
      pSock->disconnectReason = GS_DISCONNECT_RECV_FAILED;
      retVal = SD_FAILURE;
    break;
    }

  return (retVal);
  }

/************************************************************************/
/*                      _sockAllocSock                                  */
/************************************************************************/

GEN_SOCK *_sockAllocSock (GEN_SOCK_CTXT *sockCtx,
                         ST_INT role, ST_INT sockState, 
                         SOCKET hSock, GEN_SOCK_CONFIG *sockCfg)
  {
static ST_UINT  sockId = 0;		/* give diff id to each socket	*/
GEN_SOCK *pSock;
#if defined(__hpux) || defined(_AIX) || defined(sun) || defined(linux)
int nonblock_on=1;	/* CRITICAL: must be non-zero to enable non-blocking*/
#else
/* _WIN32 */
ST_ULONG nonblock_on=1;	/* CRITICAL: must be non-zero to enable non-blocking*/
#endif
int sRet;

#if !defined(_WIN32)
  assert (hSock < FD_SETSIZE);	/* this should have already been checked*/
#endif


/* Allocate enough room for the header too */
/* 0 is OK for backward compatability */
  if (sockCfg->hdrAllocSize < sockCfg->hdrSize) 
    { 
    SOCK_LOG_FLOW1 (sockCtx, "Setting hdrAllocSize = %d", sockCfg->hdrSize);
    sockCfg->hdrAllocSize = sockCfg->hdrSize;
    }
/* Sanity checks */
  if ((sockCfg->hdrAllocSize > GENSOCK_MAX_HEADER_SIZE) || 
      (sockCfg->hdrAllocSize < 0) ||
      (sockCfg->hdrSize      < 0))
    {
    assert (0);	/* these must be set correctly before now.	*/
    }

  pSock = (GEN_SOCK *) chk_calloc (1, sizeof (GEN_SOCK) + sockCfg->hdrAllocSize); 
  pSock->hdrBuf = (ST_CHAR *) (pSock+1);

  pSock->sockId = ++sockId;	/* assign different gensock2 Id number to each socket	*/
  if (pSock->sockId == 0)
    ++sockId;
  sprintf (pSock->sockIdStr, "%s sockId=%5.5u%s", sockCtx->ctxName, pSock->sockId, (role == GS_ROLE_LISTENING ? "L" : ""));
  pSock->sockCtx = sockCtx;
  pSock->hSock = hSock;
  pSock->role = role;
  pSock->sockCfg = *sockCfg; /* Copy socket config */
  if (sockCfg->setSockOpts)
    {
    if (role == GS_ROLE_LISTENING)
      {		/* set REUSEADDR option ONLY if Listen socket	*/
      sRet = setsockopt (hSock, SOL_SOCKET, SO_REUSEADDR, 
          (ST_CHAR *) &sockCfg->reuseAddr, sizeof(sockCfg->reuseAddr));
      }
    else
      {		/* set all other options ONLY if NOT Listen socket	*/
      sRet = setsockopt (hSock, IPPROTO_TCP, TCP_NODELAY, 
        (ST_CHAR *) &sockCfg->noDelay, sizeof(sockCfg->noDelay));

      sRet = setsockopt (hSock, SOL_SOCKET, SO_KEEPALIVE, 
        (ST_CHAR *) &sockCfg->keepAlive, sizeof(sockCfg->keepAlive));

      if (sockCfg->rcvBufSize > 0)
        {
        sRet = setsockopt (hSock, SOL_SOCKET, SO_RCVBUF, 
          (ST_CHAR *) &sockCfg->rcvBufSize, sizeof(sockCfg->rcvBufSize));
        }
      if (sockCfg->sndBufSize > 0)
        {
        sRet = setsockopt (hSock, SOL_SOCKET, SO_SNDBUF, 
          (ST_CHAR *) &sockCfg->sndBufSize, sizeof(sockCfg->sndBufSize));
        }
      }
    }

  pSock->sockState = sockState;

/* Make the socket non-blocking */
  sRet = ioctlsocket (pSock->hSock, FIONBIO, &nonblock_on);

  pSock->sockStats.createdTime = time (NULL);
  pSock->sockStats.lastActivityTime = sGetMsTime();

  SOCK_LOG_FLOW3 (sockCtx, "%s: allocated GEN_SOCK " S_FMT_PTR ", socket=%d.",
                  pSock->sockIdStr, pSock, pSock->hSock);
  return (pSock);
  }

/************************************************************************/
/*                      _sockAddSock                                    */
/************************************************************************/

ST_RET _sockAddSock (GEN_SOCK *pSock)
  {
GEN_SOCK_CTXT *sockCtx = pSock->sockCtx;
GEN_SOCK_CTRL *serviceCtrl;
ST_RET rc = SD_SUCCESS;

  /* protect numActive, sockServiceList, numSock, sockList */
  S_LOCK_UTIL_RESOURCES ();

/* Set receive mode to start hunting ... not needed for listen socks    */
  pSock->recvState = RECV_STATE_HUNT;
  pSock->recvDoneCount = pSock->sockCfg.hdrSize;
  pSock->recvCurrCount = 0;
  pSock->recvBuf = pSock->hdrBuf;

/* Find a service thread with capacity */
  serviceCtrl = sockCtx->sockServiceList;
  while (serviceCtrl != NULL)
    {
    if (serviceCtrl->numSock < GS_MAX_SOCK_PER_SERVICE)
      break;

    serviceCtrl = (GEN_SOCK_CTRL *) list_get_next (sockCtx->sockServiceList, serviceCtrl);  
    }
#if defined(GENSOCK_THREAD_SUPPORT)
  if (serviceCtrl == NULL)
    {
    /* UNIX/Linux note: theoretically we should never need to add new	*/
    /* thread when GS_MAX_SOCK_PER_SERVICE is (FD_SETSIZE - 1)		*/

    /* add new service control (it will exist until the sockEnd() is called) */
    serviceCtrl = _sockAddServiceCtrl (sockCtx);
    rc = _sockAddServiceThread (serviceCtrl);
    if (rc != SD_SUCCESS)
      {
      list_unlink (&sockCtx->sockServiceList, serviceCtrl);
      chk_free (serviceCtrl);
      pSock->disconnectReason = GS_DISCONNECT_INTERNAL_ERROR;
      }
    }
#else  /* !defined(GENSOCK_THREAD_SUPPORT) */
  if (serviceCtrl == NULL)
    {
    /* list empty or we reached max numSock */
    SOCK_LOG_NERR2 (sockCtx, "%s: Can't add GEN_SOCK to list (limit reached %d sockets)", 
                    pSock->sockIdStr, GS_MAX_SOCK_PER_SERVICE);
    rc = SD_FAILURE;
    pSock->disconnectReason = GS_DISCONNECT_RESOURCES_ERROR;
    }
#endif

  if (rc == SD_SUCCESS)
    {
    if (pSock->role == GS_ROLE_CALLED && pSock->listenSocket)
      ++pSock->listenSocket->numActive;

    /* Add to the tracking list */
    list_add_last (&serviceCtrl->sockList, pSock);
    pSock->serviceCtrl = serviceCtrl;
    ++serviceCtrl->numSock;
    sockServiceWake (serviceCtrl);
    SOCK_LOG_FLOW1 (sockCtx, "%s: Added GEN_SOCK to service list", pSock->sockIdStr);
    }
  else
    SOCK_LOG_FLOW2 (sockCtx, "%s: Add GEN_SOCK to service list failed (error=%d)", pSock->sockIdStr, rc);

  S_UNLOCK_UTIL_RESOURCES ();

  return (rc);
  }

/************************************************************************/
/************************************************************************/
/* POLLING MODEL SPECIFIC 						*/
/************************************************************************/
/*                      sockServiceAll                                  */
/************************************************************************/

#if !defined(GENSOCK_THREAD_SUPPORT)
/* For use in polled environment */
ST_VOID sockServiceAll (GEN_SOCK_CTXT *sockCtx, ST_LONG timeOut)
  {
GEN_SOCK_CTRL *serviceCtrl;

  serviceCtrl = sockCtx->sockServiceList;
  while (serviceCtrl != NULL)
    {
    sockCtrlService (serviceCtrl, timeOut);
    serviceCtrl = (GEN_SOCK_CTRL *) list_get_next (sockCtx->sockServiceList, serviceCtrl);  
    }
  }
#endif	/* !defined(GENSOCK_THREAD_SUPPORT)	*/

/************************************************************************/
/* End of polling specific functions 					*/
/************************************************************************/


	/****************************************/
	/*	THREAD MODEL SPECIFIC		*/
	/****************************************/

#if defined(GENSOCK_THREAD_SUPPORT)
/************************************************************************/
/*                      _sockAddServiceThread                            */
/************************************************************************/

static ST_RET _sockAddServiceThread (GEN_SOCK_CTRL *serviceCtrl)
  {
ST_RET rc = SD_SUCCESS;
GEN_SOCK_CTXT *sockCtx = serviceCtrl->sockCtx;
 
  SOCK_LOG_FLOW1 (sockCtx, "%s: in sockAddServiceThread()", sockCtx->ctxName);

  serviceCtrl->serviceEvent = gs_get_event_sem (SD_FALSE);
  if (!serviceCtrl->serviceEvent)
    {
    SOCK_LOG_ERR1 (sockCtx, "%s: could not get service event", sockCtx->ctxName);
    return (SD_FAILURE);
    }

  /* Create a set of wakeup sockets for this service control */
  rc = sockCreateWakeupSockets (sockCtx, sockCtx->gs_wakeup_port, sockCtx->gs_wakeup_port_range,
				&serviceCtrl->wakeupPort,
				&serviceCtrl->xCallingSock, &serviceCtrl->xCalledSock);
  if (rc != SD_SUCCESS)
    {
    gs_free_event_sem (serviceCtrl->serviceEvent);
    serviceCtrl->serviceEvent = NULL;
    return (rc);
    }

  /* start Service thread */
  rc = gs_start_thread (_sockServiceThread, serviceCtrl, 
                          &serviceCtrl->thService, 
                          &serviceCtrl->tIdService);
  if (rc != SD_SUCCESS)
    {
    SOCK_LOG_ERR2 (sockCtx, "%s: could not start Service thread error=%d", sockCtx->ctxName, rc);
    gs_free_event_sem (serviceCtrl->serviceEvent);
    serviceCtrl->serviceEvent = NULL;
    if (serviceCtrl->xCallingSock)
      {
      CLOSE_SOCKET (serviceCtrl->xCallingSock);
      serviceCtrl->xCallingSock = 0;
      }
    if (serviceCtrl->xCalledSock)
      {
      CLOSE_SOCKET (serviceCtrl->xCalledSock);
      serviceCtrl->xCalledSock = 0;
      }
    rc = SD_FAILURE;
    }

  return (rc);
  }

/************************************************************************/
/*                      _sockServiceThread                              */
/************************************************************************/

static ST_THREAD_RET ST_THREAD_CALL_CONV _sockServiceThread (ST_THREAD_ARG pArg)
  {
GEN_SOCK_CTRL *serviceCtrl = (GEN_SOCK_CTRL *) pArg;
GEN_SOCK_CTXT *sockCtx = serviceCtrl->sockCtx;
GEN_SOCK *pSock;
ST_INT ret;

  SOCK_LOG_FLOW1 (sockCtx, "%s: sockServiceThread started", sockCtx->ctxName);

  while (sockCtx->gTerminateService == SD_FALSE)
    {
    ret = sockCtrlService (serviceCtrl, sockCtx->gs_select_timeout);
    switch (ret)
      {
      case GS_SELECT_NO_ACTIVE_SOCK:    /* Nothing on our list  */
        gs_wait_event_sem (serviceCtrl->serviceEvent, 10000);
      break;

      case GS_SELECT_ACTIVE:            /* Action!              */
      break;

      case GS_SELECT_TIMEOUT:           /* No action            */
      break;

      case GS_SELECT_ERROR:             /* Error                */
        gs_wait_event_sem (serviceCtrl->serviceEvent, 1000);
      break;

      case GS_SELECT_TERMINATED:        /* Terminating          */
      break;

      default:
      break;
      }
    } /* While not terminate */


/* Close all sockets associated with the service control */
  S_LOCK_UTIL_RESOURCES ();	/* CRITICAL: lock all access to sockList*/
  while (serviceCtrl->sockList)
    {
    pSock = serviceCtrl->sockList;
    pSock->disconnectReason = GS_DISCONNECT_TERMINATING;
    sockClose (pSock);
    }
  S_UNLOCK_UTIL_RESOURCES ();	/* CRITICAL: lock all access to sockList*/

  SOCK_LOG_FLOW1 (sockCtx, "%s: sockServiceThread ended", sockCtx->ctxName);
  return (ST_THREAD_RET_VAL); 
  }

/************************************************************************/
/************************************************************************/
/*                      sockCreateWakeupSockets                         */
/* This function will create pair of sockets that can be used for	*/
/* signaling an event.							*/
/************************************************************************/

ST_RET sockCreateWakeupSockets (GEN_SOCK_CTXT *sockCtx, 
                                ST_UINT16 basePort, ST_UINT portRange,
				ST_UINT16 *usedPort,
				SOCKET *callingSock, SOCKET *calledSock)
  {
ST_UINT16 wakeupPort = basePort;
SOCKET xCallingSock, xCalledSock;
SOCKET hListenSock;
SOCK_ADDRLEN callingAddrLen;
SOCKADDR_IN callingAddr;
#if defined(__hpux) || defined(_AIX) || defined(sun) || defined(linux)
int nonblock_on=1;	/* CRITICAL: must be non-zero to enable non-blocking*/
#else
/* _WIN32 */
ST_ULONG nonblock_on=1;	/* CRITICAL: must be non-zero to enable non-blocking*/
#endif
struct sockaddr_in localAddr;
ST_INT noDelay;
ST_INT keepAlive;
ST_INT reuseAddr;
ST_UINT i;
int sRet;
int err;

  SOCK_LOG_FLOW1 (sockCtx, "%s: in sockCreateWakeupSockets()", sockCtx->ctxName);

/* Step 1: Make a non-blocking listen socket */
  hListenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (hListenSock == INVALID_SOCKET)
    {
    SOCK_LOG_ERR1 (sockCtx, "%s: XSocket socket() failed", sockCtx->ctxName);
    return (SD_FAILURE);
    }

  sRet = ioctlsocket (hListenSock, FIONBIO, &nonblock_on);
  reuseAddr = 0;
  sRet = setsockopt (hListenSock, SOL_SOCKET, SO_REUSEADDR, 
                    (ST_CHAR *) &reuseAddr, sizeof(reuseAddr));

  memset((char *)(&localAddr), 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons (wakeupPort);
  localAddr.sin_addr.s_addr = htonl (INADDR_ANY);

  /* Try up to the next <portRange> ports in this range */
  for (i = 0; i < portRange; ++i)
    {
    sRet = bind (hListenSock, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (sRet == 0)
      {
      SOCK_LOG_FLOW2 (sockCtx, "%s: XSocket bind on port=%d", sockCtx->ctxName, (ST_UINT) wakeupPort);
      break;
      }

    err = SOCKET_ERRORNO;
    SOCK_LOG_FLOW4 (sockCtx, "%s: XSocket bind() failure on port=%d, errno=%d %s",
                    sockCtx->ctxName, (ST_UINT) wakeupPort, err,
                    (err == SOCK_EADDRINUSE) ? "(port in use)" : " ");
    wakeupPort++;
    localAddr.sin_port = htons(wakeupPort);
    
    sMsSleep (2);
    } 

  /* App may still function if unable to create Wakeup Socket, but let user know with ERR log */
  if (sRet != 0)
    {
    SOCK_LOG_ERR2 (sockCtx, "%s: XSocket bind() failed, errno=%d", sockCtx->ctxName, SOCKET_ERRORNO);
    CLOSE_SOCKET (hListenSock);
    return (SD_FAILURE);
    }

  sRet = listen (hListenSock, SOMAXCONN);
  if (sRet != 0)
    {
    SOCK_LOG_ERR2 (sockCtx, "%s: XSocket listen() failed, errno=%d", sockCtx->ctxName, SOCKET_ERRORNO);
    CLOSE_SOCKET (hListenSock);
    return (SD_FAILURE);
    }


/* Step 2: Do a non-blocking connect */
  xCallingSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (xCallingSock == INVALID_SOCKET)
    {
    SOCK_LOG_ERR1 (sockCtx, "%s: XSocket socket() failed", sockCtx->ctxName);
    CLOSE_SOCKET (hListenSock);
    return (SD_FAILURE);
    }

  noDelay = 1;
  keepAlive = 0;
  sRet = setsockopt (xCallingSock, IPPROTO_TCP, TCP_NODELAY, 
                    (ST_CHAR *) &noDelay, sizeof(noDelay));
  sRet = setsockopt (xCallingSock, SOL_SOCKET, SO_KEEPALIVE, 
                    (ST_CHAR *) &keepAlive, sizeof(keepAlive));

/* Set up the socket address */
  memset((char *)(&localAddr), 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
  localAddr.sin_port = htons (wakeupPort);

/* Make the calling socket non-blocking */
  sRet = ioctlsocket (xCallingSock, FIONBIO, &nonblock_on);

/* Do non-blocking connect */
  sRet = connect (xCallingSock, (struct sockaddr *)&localAddr, sizeof(localAddr));
  if (sRet >= 0)
    {
    SOCK_LOG_FLOW2 (sockCtx, "%s: XSocket connected on port=%d", sockCtx->ctxName, (ST_UINT) wakeupPort);
    }
  else
    {
    sRet = SOCKET_ERRORNO;
    if (sRet == SOCK_WOULDBLOCK || sRet == SOCK_INPROGRESS)
      {
      SOCK_LOG_FLOW1 (sockCtx, "%s: XSocket connection pending", sockCtx->ctxName);
      }
    else
      {
      CLOSE_SOCKET (hListenSock);
      CLOSE_SOCKET (xCallingSock);
      SOCK_LOG_ERR2 (sockCtx, "%s: XSocket connect() failed, errno=%d", sockCtx->ctxName, sRet);
      return (SD_FAILURE);
      }
    }

/* Step 3: Do non-blocking accept */
  callingAddrLen = sizeof (callingAddr);

  for (i = 0; i < 1250; ++i)	/* wait up to 25,000 ms	for service that starts slowly	*/
    {
    callingAddrLen = sizeof (callingAddr);	/* set for each accept call	*/ 
    xCalledSock = accept (hListenSock, (SOCKADDR *) &callingAddr, &callingAddrLen);
    if (xCalledSock != INVALID_SOCKET)
      break;
    sMsSleep (20);
    } 

  if (xCalledSock == INVALID_SOCKET)
    {
    sRet = SOCKET_ERRORNO;
    CLOSE_SOCKET (xCallingSock);
    CLOSE_SOCKET (hListenSock);
    SOCK_LOG_ERR2 (sockCtx, "%s: XSocket accept() failed, errno=%d", sockCtx->ctxName, sRet);
    return (SD_FAILURE);
    }

  SOCK_LOG_FLOW2 (sockCtx, "%s: XSocket accept() successful on port=%d", sockCtx->ctxName, wakeupPort);
  noDelay = 1;
  keepAlive = 0;
  sRet = setsockopt (xCalledSock, IPPROTO_TCP, TCP_NODELAY, 
                    (ST_CHAR *) &noDelay, sizeof(noDelay));
  sRet = setsockopt (xCalledSock, SOL_SOCKET, SO_KEEPALIVE, 
                    (ST_CHAR *) &keepAlive, sizeof(keepAlive));

/* Make the called socket non-blocking */
  sRet = ioctlsocket (xCalledSock, FIONBIO, &nonblock_on);

  CLOSE_SOCKET (hListenSock);

  /* wakeup sockets pair connected */
  *usedPort = wakeupPort;
  *callingSock = xCallingSock;
  *calledSock = xCalledSock;
  return (SD_SUCCESS);
  }

/************************************************************************/
/* End of thread specific functions 					*/
#endif /* #if defined(GENSOCK_THREAD_SUPPORT */
/************************************************************************/

/************************************************************************/
/************************************************************************/
/*                      sockLogState                                    */
/************************************************************************/

ST_VOID sockLogState (GEN_SOCK_CTXT *sockCtx)
  {
GEN_SOCK_CTRL *serviceCtrl;
GEN_SOCK *pSock;
ST_INT serviceCount;
ST_INT sockCount;

  S_LOCK_UTIL_RESOURCES ();

  SLOGALWAYS1 ("GEN_SOCK STATE for context '%s':", sockCtx->ctxName);
  SLOGCALWAYS1 ("  Service Control List (%d)", list_get_sizeof (sockCtx->sockServiceList));
  serviceCtrl = sockCtx->sockServiceList;

  serviceCount = 1;
  while (serviceCtrl != NULL)
    {
    SLOGCALWAYS1 ("    %d) Service Control", serviceCount++);
    SLOGCALWAYS1 ("        %d sockets", serviceCtrl->numSock);
#if defined(GENSOCK_THREAD_SUPPORT)
    SLOGCALWAYS3 ("        Wakeup Port: %d, xCalledSock: %d xCallingSock: %d", 
                                        serviceCtrl->wakeupPort,
                                        serviceCtrl->xCalledSock,
                                        serviceCtrl->xCallingSock);
#endif
    sockCount = 1;
    pSock = serviceCtrl->sockList;
    while (pSock != NULL)
      {
      SLOGCALWAYS1 ("        %d) Socket Control", sockCount++);
      sockLogSockState (pSock,"              ");
      pSock = (GEN_SOCK *) list_get_next (serviceCtrl->sockList, pSock);  
      }
    serviceCtrl = (GEN_SOCK_CTRL *) list_get_next (sockCtx->sockServiceList, serviceCtrl);  
    }

  S_UNLOCK_UTIL_RESOURCES ();
  }


/************************************************************************/
/*                      sockLogSockState                                */
/************************************************************************/

ST_VOID sockLogSockState (GEN_SOCK *pSock, ST_CHAR *prefix)
  {
ST_CHAR *str;

  if (prefix == NULL)
    prefix = "";

  switch (pSock->role)
    {
    case GS_ROLE_CALLED:
      str = "Called";
    break;
    case GS_ROLE_CALLING:
      str = "Calling";
    break;
    case GS_ROLE_LISTENING:     /* Not valid */
      str = "Lisening";
    break;
    default:
      str = "Invalid";
    break;
    }
  SLOGCALWAYS2 ("%sRole : %s", prefix, str);


  switch (pSock->sockState)
    {
    case GS_STATE_LISTENING  :
      str = "LISTENING";
    break;

    case GS_STATE_FAILED_LISTEN  :
      str = "FAILED LISTEN";
    break;

    case GS_STATE_STOPPING_LISTEN :
      str = "STOPPING_LISTEN";

    case GS_STATE_CONNECTING :
      str = "CONNECTING";
    break;

    case GS_STATE_CONNECTED  :
      str = "CONNECTED";
    break;

    case GS_STATE_CLOSED  :
      str = "CLOSED";
    break;

    default:
      str = "Invalid";
    break;
    }

  SLOGCALWAYS2 ("%sState: %s", prefix, str); 
  str = ctime (&pSock->sockStats.createdTime); 
  str[24] = 0;
  SLOGCALWAYS2 ("%sTime Created: %s", prefix, str);
  if (pSock->sockStats.activeTime) 
    {
    str = ctime (&pSock->sockStats.activeTime); 
    str[24] = 0;
    SLOGCALWAYS2 ("%sTime Active: %s", prefix, str);
    }
  SLOGCALWAYS2 ("%sNum Group Msgs Queued: %d", prefix, pSock->sockTxQueueGroupCnt); 
  SLOGCALWAYS2 ("%sNum Msgs (RFC1006) Sent: %ld", prefix, pSock->sockStats.numSend); 
  SLOGCALWAYS2 ("%sNum Msgs (RFC1006) Recv: %ld", prefix, pSock->sockStats.numRecv); 
  }


/************************************************************************/
/*			sockGetRemAddrInfo				*/
/************************************************************************/

ST_RET sockGetRemAddrInfo (GEN_SOCK *pSock, SOCKADDR_IN *remSockAddrDest, 
			  ST_CHAR **remAddrTxtOut, ST_INT *portOut)
  {
SOCKADDR_IN peer;
SOCKADDR_IN *pp;
SOCK_ADDRLEN len;
ST_RET ret;

  if (remSockAddrDest)
    pp = remSockAddrDest;
  else
    pp = &peer;

  len = sizeof (SOCKADDR_IN);
  ret = getpeername (pSock->hSock, (struct sockaddr *) pp, &len);
  if (ret == SD_SUCCESS)
    {
    if (remAddrTxtOut != NULL)
      *remAddrTxtOut = inet_ntoa (pp->sin_addr);

    if (portOut != NULL)
      /* first cast to ST_UINT to prevent sign extension for 0x8000-0xFFFF ports */
      *portOut =  (ST_INT) ((ST_UINT) ntohs (pp->sin_port));
    }
  return (ret);
  }


		/* --------------------------------------------	*/
		/*	User misc				*/
		/* --------------------------------------------	*/

/************************************************************************/
/*			sockUsrFun					*/
/*----------------------------------------------------------------------*/
/* This function will call user function for all connections and close	*/
/* the connection if the user function does not return SD_SUCCESS.	*/
/* The secCtrl parameter is passed to the user function.		*/
/************************************************************************/
ST_VOID sockUsrFun (ST_RET (*usrFun)(ST_VOID *secCtrl), ST_CHAR *errMsg)
{
GEN_SOCK_CTXT   *sockCtx;
GEN_SOCK_CTRL   *serviceCtrl;
GEN_SOCK        *pSock;
ST_RET           ret;

  if (!usrFun)
    return;

  sockCtx = sockCtxList;
  while (sockCtx)
    {
    SOCK_LOG_FLOW0 (sockCtx, "sockUsrFun");
    S_LOCK_UTIL_RESOURCES ();

    serviceCtrl = sockCtx->sockServiceList;
    while (serviceCtrl != NULL)
      {
      /* Check all sockets associated with the service control */
      pSock = serviceCtrl->sockList;
      while (pSock)
        {
	/* applies only to secured connections */
	if (pSock->sockCfg.secCtrl)
	  {
          ret = (*usrFun) (pSock->sockCfg.secCtrl);
          if (ret != SD_SUCCESS)
            {
            SOCK_LOG_ERR2 (sockCtx, "%s: %s, closing socket.", pSock->sockIdStr, errMsg);
            sockClose(pSock);
	    }
          }
        pSock = (GEN_SOCK *)list_get_next (serviceCtrl->sockList, pSock);
        }
      serviceCtrl = (GEN_SOCK_CTRL *)list_get_next (sockCtx->sockServiceList, serviceCtrl);
      }

    S_UNLOCK_UTIL_RESOURCES ();

    sockCtx = (GEN_SOCK_CTXT *)list_get_next (sockCtxList, sockCtx);
    }
}

		/* --------------------------------------------	*/
		/*	Misc socket related functions		*/
		/* --------------------------------------------	*/

/************************************************************************/
/*			convertIpAddr					*/
/*----------------------------------------------------------------------*/
/* This function will take pointer to IP address (host string or dotted	*/
/* notation string) and convert it to unsigned long value.		*/
/* If the useGetHostByName is SD_TRUE the gethostbyname() function will	*/
/* be called when the inet_addr(ipAddrStr) produced INADDR_NONE return.	*/
/* (host name was used or dotted notation was invalid).			*/
/* RETURN:								*/
/*	ULONG != 0	converted IP Addr to ST_ULONG value or		*/
/*	0		if function failed				*/
/*			Note: 0.0.0.0 would be invalid because we	*/
/*                            return 0 for conversion error.		*/
/************************************************************************/
ST_ULONG convertIPAddr (ST_CHAR *ipAddrStr, ST_BOOLEAN useGetHostByName)
{
ST_ULONG   ipAddr = 0;
ST_CHAR   *dotPtr;

#ifdef _WIN32
/* init WinSock interface	*/
static ST_BOOLEAN bWSAStarted = SD_FALSE;
WSADATA	wsaData;	
ST_INT  wRet;

  /* make sure sockets initialized before we call any socket functions	*/
  if (!bWSAStarted)
    {
    if (wRet = WSAStartup(0x0202, &wsaData))
      {
      SLOGALWAYS1 ("convertIPAddr: unable to initialize WinSock interface to use version 2.2 (error=%d)", wRet);
      return (ipAddr);
      }
    bWSAStarted = SD_TRUE;
    }
#endif /* _WIN32 */

  if (ipAddrStr == NULL || strlen(ipAddrStr) == 0)
    {
    SLOGALWAYS0 ("convertIPAddr: conversion failed, ipAddrStr=NULL or ipAddrStr is empty");
    return (ipAddr);
    }

  /* Since inet_addr() considers following addresses valid:
     a.b.c.d  (Internet addr)
     a.b.c    (Class B addr)
     a.b      (Class A addr)
     a        stored directly
     we will pass to  inet_addr only valid ipAddrStr in form of Internet Addr	*/

  /* call inet_addr if we find 3 '.' in the ipAddrStr (and do not use strtok)	*/
  if ((dotPtr = strstr(ipAddrStr, ".")) != NULL)
    if ((dotPtr+1 < ipAddrStr+strlen(ipAddrStr)) && (dotPtr = strstr(dotPtr+1, ".")) != NULL)
      if ((dotPtr+1 < ipAddrStr+strlen(ipAddrStr)) && (dotPtr = strstr(dotPtr+1, ".")) != NULL)
        {
        ipAddr = (ST_ULONG) inet_addr (ipAddrStr);
        /* NOTE: some systems return from the inet_addr() unsigned int (LINUX)	*/
        /*       and some return unsigned long (Windows).			*/
	}

  if (ipAddr == 0 || ipAddr == htonl(INADDR_NONE))
    {
#if !defined (VXWORKS)		/* gethostbyname not supported	*/
    if (useGetHostByName)
      {
      struct hostent *pHostEnt = NULL;	/* host database entry for remote host	*/
      /* UNIX IEEE: the behavior of gethostbyname() when passed a numeric	*/
      /*            address string is unspecified 				*/
      /*            DEBUG: should we check for it?				*/
      pHostEnt = gethostbyname(ipAddrStr);
      if (pHostEnt != NULL)
	ipAddr = (ST_ULONG) (*(ST_UINT32 *)(pHostEnt->h_addr));
      else
	{
	ipAddr = 0;
        SLOGALWAYS2 ("convertIPAddr : gethostbyname IPAddr='%s'conversion errno=%d",
                     ipAddrStr, SOCKET_ERRORNO);
        }
      }
    else
#endif /* !defined (VXWORKS) */
      {
      ipAddr = 0;
      SLOGALWAYS1 ("convertIPAddr : IPAddr='%s'conversion error", ipAddrStr);
      }
    }

  return (ipAddr);
}

/************************************************************************/
/*			sockEventPut					*/
/* Put socket event on list. This should be called from callback	*/
/* functions to save events on the list and return immediately. Other	*/
/* threads can get events from the list later and process them.		*/
/************************************************************************/
ST_VOID sockEventPut (GEN_SOCK_CTXT *sockCtx, GEN_SOCK_EVENT *sockEvent)
  {
  S_LOCK_UTIL_RESOURCES ();
  list_add_last (&sockCtx->sockEventList, sockEvent);
  sockCtx->sockEventCount++;
  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*			sockEventGet					*/
/* Get next socket event from list (to process the event).		*/
/************************************************************************/
GEN_SOCK_EVENT *sockEventGet (GEN_SOCK_CTXT *sockCtx)
  {
GEN_SOCK_EVENT *sockEvent;
  S_LOCK_UTIL_RESOURCES ();
  sockEvent = (GEN_SOCK_EVENT *)list_get_first (&sockCtx->sockEventList);
  if (sockEvent)
    sockCtx->sockEventCount--;
  S_UNLOCK_UTIL_RESOURCES ();
  return (sockEvent);
  }


/************************************************************************/
/*			sockEventQueueFlush				*/
/************************************************************************/

ST_VOID sockEventQueueFlush (GEN_SOCK_CTXT *sockCtx, GEN_SOCK *pSock)
  {
GEN_SOCK_EVENT *sockEvent;
GEN_SOCK_EVENT *nextSockEvent;

  S_LOCK_UTIL_RESOURCES ();
  sockEvent = (GEN_SOCK_EVENT *) sockCtx->sockEventList;

  while (sockEvent)
    {
    nextSockEvent = (GEN_SOCK_EVENT *) list_get_next (sockCtx->sockEventList, sockEvent);
    if (sockEvent->pSock == pSock)
      {
      list_unlink (&sockCtx->sockEventList, sockEvent);
      sockCtx->sockEventCount--;
      /* If data was allocated, it must be freed here by user function.	*/
      if (sockEvent->sockData)
        {
        if (pSock->sockCfg.uSockRxBufFree)
          (*pSock->sockCfg.uSockRxBufFree) (pSock, sockEvent->sockData);
        else
          SOCK_LOG_ERR1 (sockCtx, "Function pointer 'uSockRxBufFree' not set. Data buffer " S_FMT_PTR " can't be freed.",
             sockEvent->sockData);
        }
      chk_free (sockEvent);
      }
    sockEvent = nextSockEvent;
    }
  S_UNLOCK_UTIL_RESOURCES ();
  }

//SK+20191201
ST_VOID sk_set_bind_address(char * address)
{
	memcpy(sk_bind_address, address, strlen(address));
}
