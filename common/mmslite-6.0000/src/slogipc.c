/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		2004 - 2012, All Rights Reserved 			*/
/*									*/
/* MODULE NAME : slogipc.c						*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements code to slog through sockets.	*/
/*		The gensock2 is used to access socket functionality.	*/
/*		The socket logging subsystem accepts connections from	*/
/*		number of clients and sends any slog messages to them	*/
/*		once available.						*/
/*		No connections are made from this module.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/12/12  EJV           slogSetHdr: added slogHdrMaxSize parameter.	*/ 
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 07/14/11  MDE           Added check to slogIpcSendData 		*/
/* 03/23/11  NAV           add cast to slogIpcInit			*/
/* 11/18/10  MDE           Added heartbeat				*/
/* 10/21/10  MDE           Added SEV_SUPPORT				*/
/* 09/17/10  EJV           slogIpcInit:free sockCtx if sockStartEx fails*/
/* 08/02/10  JRB	   Alloc sockCtx & pass to sockStartEx.		*/
/*			   Free sockCtx in slogIpcStop.			*/
/* 07/19/10  MDE           Fixed possible crash in 'slogIpcSendQueued'	*/
/* 04/27/10  JRB	   Set new uSockRxBufFree funct ptr.		*/
/* 02/24/10  JRB	   Check list_find_last return.			*/
/* 06/01/09  MDE    53     Added SlogClient features			*/
/* 01/01/09  EJV    52     slogIpcInit: init if calling/listening config*/
/* 01/24/09  MDE    51     Don't set or check seal on MISSED messages	*/
/* 01/18/09  MDE    50     Added calling backoff for seal failure	*/
/* 11/06/08  MDE    49     Fixed for no SEAL compliation		*/
/* 10/08/08  MDE    48     Added SEAL					*/
/* 08/04/08  MDE    47     Added slogIpcEventEx				*/
/* 07/25/08  EJV    46     !_WIN32 _slogIpcInitSockCfg: set reuseAddr=1	*/
/* 07/25/08  MDE    45     Moved slogIpcCtx into LOG_CTRL		*/
/* 07/07/08  MDE    44     Added slogIpcActiveCount			*/
/* 07/07/08  MDE    43     Fixed 'clone' handling			*/
/* 06/13/08  MDE    42     Context state tweaks				*/
/* 05/12/08  RKR    41     modified slogIpc_uSockRxInd                  */
/* 05/02/08  MDE    40     Handle case of connection to log source	*/
/* 04/30/08  MDE    39     Fixed leak for too many commands		*/
/* 04/30/08  MDE    38     Disallow calling if not enabled in LogCfg 	*/
/* 04/24/08  MDE    37     Fixed terminate crash w/calling connections  */
/* 04/07/08  EJV    36     slogIpc_uSockRxInd CORR: chk sockData->result*/
/* 03/01/08  EJV    35     Set disconnect error codes.			*/
/* 12/14/07  DSF    34     Added sNonStandardLogMode to support .NET	*/
/*			   Logger class					*/
/* 10/29/07  JRB    33     strncat_safe obsolete, use strncat_maxstrlen.*/
/* 10/04/07  MDE    32     Minor function name fixup			*/
/* 09/10/07  MDE    31     Worked on called connection support		*/
/* 08/14/07  MDE    30     Don't crash if can't listen  or calling only */
/* 03/12/07  JRB    29     Add cast.					*/
/* 03/01/07  MDE    28     Added SLOGIPC options, missed message 	*/
/* 02/21/07  RKR    27     Added a cast to slog_ipc_std_cmd_fun         */
/* 01/22/07  MDE    26     Cleaned up command handling			*/
/* 01/15/07  JRB    25     Del poll_mode arg to sockStart.		*/
/* 01/15/07  EJV    24     Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 01/08/07  EJV    23     Chg SV* globals to local var in functions	*/
/*			     (see new struct SLOGIPC_SV_SLOG).		*/
/*			   slogIpcSaveLoggingParams: chg args.		*/
/*			   slogIpcStop: added lc param.			*/
/* 10/05/05  EJV    22     Implemented GEN_SOCK_CTXT.			*/
/*			   slogIpc: ! application MUST call slogIpcInit	*/
/*			   slogIpc: do not log if not in ACTIVE state.	*/
/*			   slogIpcInit: use memset (not secCfg={0});	*/
/*			     cleanup if sockInitListen fails.		*/
/*			   slogIpcStop: return rtn from sockEnd.	*/
/*			   slogIpc_uSockConnectActive: tx CRNLs with appId.*/
/*			   Added slogIpcEvent for non-threaded mode.	*/
/*			   Chg connection state names to SLOGIPC_CON*.	*/
/* 08/10/05  MDE    21     Retry listen periodically			*/
/* 08/02/05  MDE    20     slogIpcInit: set reuseAddr to SD_FALSE	*/
/* 07/06/05  EJV    19     slogIpcInit: set reuseAddr to SD_TRUE.	*/
/* 04/06/05  EJV    18     Elim Linux warnings (added typecasting)	*/
/* 02/23/05  JRB    17     slogIpc: add SD_CONST to some args.		*/
/*           EJV           slogIpc: init vars on stack to avoid warnings*/
/*           EJV	   Del unused vars.				*/
/* 02/02/05  MDE    16     Smart mode work				*/
/* 01/24/05  EJV    15     slogIpcInit: limited SLOGALWAYS		*/
/* 12/15/04  ASK    14     Fix buffering when sending Ipc Cmd Responses	*/
/* 12/08/04  ASK    13     Added 'smart mode' for Ipc Cmd clients.	*/
/*			   Use strncat_safe when filling IPC Buffer.	*/
/* 11/22/04  EJV    12     Move buf size computation to slogIpc, do not	*/
/*			     use sl_max_msg_size, may change dynamically*/
/* 09/22/04  EJV    11     Eliminated more sun warnings.		*/
/* 09/13/04  EJV    10     Eliminated sun warnings.			*/
/* 07/22/04  EJV    09     slogIpc_uSockWritable:don't slog in this fun	*/
/* 07/09/04  EJV    08     slogIpcSendQueued: init ret.			*/
/* 07/02/04  DSF    07     Always link in ws2_32.lib			*/
/* 06/24/04  EJV    06     slogIpcInit: set portUsed.			*/
/* 06/23/04  EJV    05     Init range of ports for multiple instances.	*/
/*			   Send Application ID after conn if configured.*/
/* 05/27/04 ASK/EJV 04     Fix S_LOCK_RESOURCES deadlock in slogIpcStop.*/
/*			   Rework slogIpc to prevent gensock logs.	*/
/* 05/20/04  EJV    03     Limited log message queue.			*/
/* 05/18/04  MDE    02     Removed LOG_IPC_SUPPORT #ifdef's		*/
/* 05/11/04  EJV    01     Initial Release				*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "stime.h"
#include "slog.h"
#include "gensock2.h"
#include "str_util.h"
#ifdef SEV_SUPPORT
#include "sev.h"
#endif
#if defined (_WIN32)
#pragma comment(linker, "/defaultlib:ws2_32.lib")
#endif

/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/


/* struct used to save slog log masks and dynamic slogging function pointers	*/
typedef struct slogipc_sv_slog_tag
  {
  ST_UINT32 logCtrl;

  ST_VOID (*slog_dyn_log_fun)  (LOG_CTRL *lc, 
			        SD_CONST ST_INT logType, 
                                SD_CONST ST_CHAR *SD_CONST sourceFile, 
			        SD_CONST ST_INT lineNum,
                                SD_CONST ST_INT bufLen, 
			        SD_CONST ST_CHAR *buf);

  ST_VOID (*_slog_dyn_log_fun) (LOG_CTRL *lc, 
			        SD_CONST ST_CHAR *timeStr, 
			        SD_CONST ST_INT logType, 
			        SD_CONST ST_CHAR *SD_CONST logTypeStr,
                                SD_CONST ST_CHAR *SD_CONST sourceFile, 
			        SD_CONST ST_INT lineNum,
                                SD_CONST ST_INT bufLen, 
			        SD_CONST ST_CHAR *buf);

  ST_VOID (*slog_remote_fun)   (ST_UINT32 slog_remote_flags,
			        LOG_CTRL *lc, 
			        SD_CONST ST_INT logType, 
                                SD_CONST ST_CHAR *SD_CONST sourceFile, 
			        SD_CONST ST_INT lineNum,
                                SD_CONST ST_INT bufLen, 
			        SD_CONST ST_CHAR *buf);

  ST_VOID (*_slog_remote_fun)  (ST_UINT32 slog_remote_flags,
			        LOG_CTRL *lc, 
			        SD_CONST ST_INT logType, 
                                SD_CONST ST_CHAR *SD_CONST logTypeStr, 
                                SD_CONST ST_CHAR *SD_CONST sourceFile, 
			        SD_CONST ST_INT lineNum,
                                SD_CONST ST_INT bufLen, 
			        SD_CONST ST_CHAR *buf);
  } SLOGIPC_SV_SLOG;


typedef struct
  {
  ST_BOOLEAN logMsg;
  } SLOGIPC_MSG_INFO;

/************************************************************************/
#if defined(GENSOCK_THREAD_SUPPORT)
#define	SLOGIPC_WAIT(ctx, ms)	sMsSleep (ms);
#else
#define	SLOGIPC_WAIT(ctx, ms)	sockServiceAll (ctx, ms);
#endif


/************************************************************************/

static ST_VOID slogIpcAddSendQue (LOG_CTRL *lc, SLOGIPC_CTX *ctx, 
				  SLOGIPC_CON_CTRL *conCtrl,
				  GEN_SOCK_DATA    *sockData);

static ST_VOID _slogIpcInitSockCfg (LOG_CTRL *lc, GEN_SOCK_CONFIG *sockCfg);
static ST_RET slogIpcInitCalling (LOG_CTRL *lc);

#ifdef SLOGIPC_SEAL

/* Default table of keys */

static SJ_KEY _slogIpcKeys[] =
  {
    {0x5b,0x4c,0xd2,0x69,0xd6,0x30,0xd3,0x55,0xd9,0x0e},
    {0xdb,0xe7,0x7f,0x73,0x5a,0x3e,0xdb,0x9a,0x87,0xbf},
    {0x62,0x7b,0x43,0xbc,0x8a,0xb4,0xe2,0x5b,0x1c,0x77},
    {0x7c,0x93,0xd2,0xe0,0xf9,0x32,0xe4,0x4e,0x9a,0xb1},
    {0x7a,0x29,0x24,0x6e,0xb2,0x44,0xe6,0xf1,0xc7,0xa4},
    {0xdb,0x5e,0x61,0xf1,0xfe,0x3b,0x37,0xd5,0xab,0x30},
    {0x79,0x06,0x12,0x63,0x44,0x54,0x27,0x87,0x77,0xdc},
    {0x93,0xe6,0x3f,0xce,0xf6,0xd8,0xae,0xdf,0x1e,0xf9},
    {0xaf,0x53,0x73,0x9f,0x32,0xbf,0x48,0x55,0x51,0x6d},
    {0x49,0xd9,0xde,0xf5,0x21,0xe9,0x16,0x8f,0xbe,0x5b},
  };
#define SJ_KEY_COUNT 10

SJ_KEY *slogIpcKeyTable = _slogIpcKeys;
ST_INT slogIpcNumKeys = SJ_KEY_COUNT;

static ST_UINT32 _slogIpcCalcHeaderCrc (SLOGIPC_SMART_MSG_HDRS *sHdrS);
#endif

/************************************************************************/

	/*----------------------------------------------*/	
	/*	gensock2 callback functions		*/
	/*----------------------------------------------*/

/************************************************************************/
/*			slogIpc_DataBufAlloc				*/
/************************************************************************/

/* Used for both transmit and receive buffers */

static ST_VOID slogIpc_DataBufAlloc (GEN_SOCK *pSock, ST_INT dataLen,
                                      GEN_SOCK_DATA **sockDataOut)
  {
GEN_SOCK_DATA    *sockData;

  sockData = (GEN_SOCK_DATA *) chk_malloc (sizeof (GEN_SOCK_DATA) + dataLen + sizeof (SLOGIPC_MSG_INFO));
  sockData->data = (ST_UCHAR *)(sockData + 1);
  sockData->dataLen = 0;			/* curr length to send			*/
  sockData->usr1 = (sockData->data + dataLen);

/* These parameters are used for Transmit only */
  sockData->usrBufBase = sockData->data;	/* ptr to buffer base		*/
  sockData->usrBufLen = dataLen;		/* len of alloc buf, could be more than actual len */

  *sockDataOut = sockData;
  }

/************************************************************************/
/*			slogIpc_uDataBufFree				*/
/************************************************************************/

static ST_VOID slogIpc_uDataBufFree (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData)
  {
  chk_free (sockData);						        
  }

/************************************************************************/
/*			slogIpc_uSockHunt				*/
/*----------------------------------------------------------------------*/
/* This function will get the length of received message on a socket.	*/
/************************************************************************/

static ST_VOID slogIpc_uSockHunt (GEN_SOCK *pSock, ST_INT *huntStateIo,
                                  ST_CHAR *buf, ST_INT bufCount, ST_INT *lenOut)
  {
SLOGIPC_CTX *ctx;
SLOGIPC_CON_CTRL *conCtrl;
SLOGIPC_SMART_MSG_HDRS *sHdrs;
SLOGIPC_SMART_MSG_HDR *sHdr;
ST_UINT32 magic;
#ifdef SLOGIPC_SEAL
ST_RET rc;
#endif

  ctx     = (SLOGIPC_CTX *)      pSock->sockCfg.usr1;
  conCtrl = (SLOGIPC_CON_CTRL *) pSock->sockCfg.usr2;
  
  /* Make sure we have our header bytes */
  if (ctx->smartMode == SD_TRUE)
    {

  /* If we are discovering whether we are sealed or not ... */
    if (conCtrl->magicNumber == 0)
      {
      sHdr = (SLOGIPC_SMART_MSG_HDR *) buf;
      magic = ntohl (sHdr->magicNumber);
      switch (magic)
        {
        case SLOGIPC_MAGIC_NUMBER_SEAL:
          conCtrl->magicNumber = SLOGIPC_MAGIC_NUMBER_SEAL;
          conCtrl->sealMode = SSEAL_MODE_ALL;
          conCtrl->hdrSize = sizeof (SLOGIPC_SMART_MSG_HDRS);

          pSock->sockCfg.hdrSize = sizeof (SLOGIPC_SMART_MSG_HDRS);

        /* OK, need to get the rest of the header */
          pSock->recvDoneCount = pSock->sockCfg.hdrSize;
          *huntStateIo = GENSOCK_HUNT_UNDERWAY;
          return;
	break;

	case SLOGIPC_MAGIC_NUMBER_NOSEAL:
          conCtrl->magicNumber = SLOGIPC_MAGIC_NUMBER_NOSEAL;
          conCtrl->sealMode = SSEAL_MODE_NONE;
          conCtrl->hdrSize = sizeof (SLOGIPC_SMART_MSG_HDR);
          pSock->sockCfg.hdrSize = sizeof (SLOGIPC_SMART_MSG_HDR);
	break;

        default:
          *huntStateIo = GENSOCK_HUNT_RESET;
          pSock->sockCfg.hdrSize = sizeof (SLOGIPC_SMART_MSG_HDR);
          return;
        break;
        }
      }

  /* OK, we have a complete header to work with, and know what our seal mode is */
    if (conCtrl->sealMode == SSEAL_MODE_NONE)
      {
      sHdr = (SLOGIPC_SMART_MSG_HDR *) buf;
      magic = ntohl (sHdr->magicNumber);
     *lenOut = ntohl (sHdr->messageLen);
      }
    else
      {
      sHdrs = (SLOGIPC_SMART_MSG_HDRS *) buf;
      magic = ntohl (sHdrs->magicNumber);
     *lenOut = ntohl (sHdrs->messageLen);
      }

  /* Verify the magic number and that we have the right number of bytes */
    if (magic != conCtrl->magicNumber)
      {
      *huntStateIo = GENSOCK_HUNT_RESET;
      return;
      }
    if (bufCount != conCtrl->hdrSize)
      {
      *huntStateIo = GENSOCK_HUNT_RESET;
      return;
      }

  /* If sealed, check the seal */
    if (conCtrl->sealMode == SSEAL_MODE_ALL)
      {
    /* OK, check the seal ... */
      rc = _slogIpcCheckSeal (ctx->lc, sHdrs);
      if (rc != SD_SUCCESS)
        {
        *huntStateIo = GENSOCK_HUNT_DISCONNECT;

      /* If this was a calling socket, set a backoff connect time */
        if (pSock->role == GS_ROLE_CALLING)
          ctx->nextCallingConnectTime = sGetMsTime () + (ST_DOUBLE) ctx->lc->ipc.callingBackoff;

        return;
        }
      }

  /* All looks OK, do one final check to make sure size is within reason */
    if (*lenOut < _SLOGIPC_MAX_DATA_SIZE)
      *huntStateIo = GENSOCK_HUNT_DONE;
    else
      *huntStateIo = GENSOCK_HUNT_RESET;

    }
  else	/* Just discard data if not in smart mode */
    *huntStateIo = GENSOCK_HUNT_RESET;
  }

/************************************************************************/
/*			slogIpc_uSockConnectActive			*/
/*----------------------------------------------------------------------*/
/* This is gensock2 callback function when a socket connection has been	*/
/* accepted on listen socket.						*/
/************************************************************************/
static ST_RET slogIpc_uSockConnectActive (struct tag_GEN_SOCK *pSock)
  {
SLOGIPC_CTX      *ctx;
SLOGIPC_CON_CTRL *conCtrl;
SLOGIPC_CONNECT_IND slogIpcConnectInd;
ST_CHAR          *appId;
LOG_CTRL         *lc;
ST_CHAR          *p;
GEN_SOCK_DATA    *sockData;
ST_INT            appIdLen;
ST_INT            optionsLen;
ST_UINT32         options;


  S_LOCK_UTIL_RESOURCES ();

  ctx  = (SLOGIPC_CTX *) pSock->sockCfg.usr1;
  lc = ctx->lc;

/* Do not accept connections if state other than active (terminating)	*/
  if (ctx->state != SLOGIPC_CTX_ACTIVE)
    {
    pSock->disconnectReason = GS_DISCONNECT_USR_REFUSED;
    S_UNLOCK_UTIL_RESOURCES ();
    return (SD_FAILURE);
    }

  conCtrl = (SLOGIPC_CON_CTRL *) chk_calloc (1, sizeof(SLOGIPC_CON_CTRL));
  conCtrl->conSock  = pSock;
  conCtrl->conState = SLOGIPC_CONNECTED;

  lc = ctx->lc;
  if (lc->ipc.sealMode == SSEAL_MODE_NONE)
    {
    conCtrl->magicNumber   = SLOGIPC_MAGIC_NUMBER_NOSEAL;
    conCtrl->hdrSize       = sizeof (SLOGIPC_SMART_MSG_HDR);
    conCtrl->sealMode	   = SSEAL_MODE_NONE;
    }
  else if (lc->ipc.sealMode == SSEAL_MODE_ALL)
    {
    conCtrl->magicNumber   = SLOGIPC_MAGIC_NUMBER_SEAL;
    conCtrl->hdrSize       = sizeof (SLOGIPC_SMART_MSG_HDRS);
    conCtrl->sealMode	   = SSEAL_MODE_ALL;
    }
  else
    {
    conCtrl->magicNumber   = 0;
    conCtrl->hdrSize       = 0;
    }

/* Don't need because of calloc 
  if (ctx->smartMode == SD_TRUE)
    conCtrl->txEnable = SD_FALSE;
*/

  /* save the our ptr (used in other callback functions) */
  pSock->sockCfg.usr2 = (ST_VOID *) conCtrl;

  /* add to list of connections */
  list_add_last (&ctx->conCtrlList, conCtrl);


/* If we are not the client, send the AppId and options */
  if (lc->ipc.client == SD_FALSE)
    {
    /* queue the application identification to send to the connecting client	*/
    appId = ctx->lc->ipc.appId;
    if (appId)
      {
      appIdLen = (int) strlen (appId);
      if (ctx->smartMode != SD_TRUE)
        {	
        #define APPID_PAD_STR		"\r\n\r\n"
        #define APPID_PAD_STR_LEN		(2 * strlen (APPID_PAD_STR))
        slogIpc_DataBufAlloc (pSock, appIdLen + (int) APPID_PAD_STR_LEN + 1, &sockData);
        /* transmit CR NL to separate AppIdfrom other messages on the screen */
        sprintf ((ST_CHAR *)sockData->data, "%s%s%s", APPID_PAD_STR, (ST_CHAR *)appId, APPID_PAD_STR);
        sockData->dataLen = appIdLen + (int) APPID_PAD_STR_LEN;
        slogIpcAddSendQue (NULL, ctx, conCtrl, sockData);
        }
      else /* Smart mode */
        { 
        slogIpc_DataBufAlloc (pSock, 
                               conCtrl->hdrSize + (appIdLen + 1), 
                               &sockData);
  
       /* Add the data following the header */
        p = (ST_CHAR *) (sockData->data + conCtrl->hdrSize);
        strcpy (p, appId);
  
        _slogIpcInitHdr (pSock, sockData, SLOGIPC_MSG_TYPE_APPID, appIdLen);
        slogIpcAddSendQue (NULL, ctx, conCtrl, sockData);
  
        /* See if we need to send options */
        options = ctx->lc->ipc.options;
        if (options != 0)
          {
          slogIpc_DataBufAlloc (pSock, 
                                conCtrl->hdrSize + _SLOGIPC_MAX_OPTIONS_LEN + 1, 
                                &sockData);
      
        /* Go ahead and create the options string */
          p = (ST_CHAR *) (sockData->data + conCtrl->hdrSize);
          p[0] = 0;
          if (options & LOG_IPC_EDIT_LOGCFG)
            strncat_maxstrlen (p, "|EDIT_LOGCFG|", _SLOGIPC_MAX_OPTIONS_LEN);
#ifdef SEV_SUPPORT
          if (_sevCfg.sevEnable)
	    strncat_maxstrlen (p, "SEV|", _SLOGIPC_MAX_OPTIONS_LEN);
#endif
        /* OK, finish the len and send it */
          optionsLen = (int) strlen (p);
  
          _slogIpcInitHdr (pSock, sockData, SLOGIPC_MSG_TYPE_OPTIONS, optionsLen);
          slogIpcAddSendQue (NULL, ctx, conCtrl, sockData);
          }
        } /* Smart mode */
  
      /* can't send from here because socket is not ready for send until we ret	*/
      /* from this function, but to drive the sending we can set the writable	*/
      /* flag and gensock2 service thread should soon call slogIpc_uSockWritable*/
      pSock->sockCfg.chkWritable = SD_TRUE;
      }
    }

/* If the user wants to know, let them */
  if (ctx->lc->ipc.slog_ipc_connect_ind != NULL)
    {
    slogIpcConnectInd.conCtrl = conCtrl;
    slogIpcConnectInd.ctx     = ctx;
    slogIpcConnectInd.pSock   = pSock;
    (*ctx->lc->ipc.slog_ipc_connect_ind) (&slogIpcConnectInd);
    }

  S_UNLOCK_UTIL_RESOURCES ();

  /* connection accepted */
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			slogIpc_uSockDisconnectInd			*/
/*----------------------------------------------------------------------*/
/* This is gensock2 callback function when socket gets disconnected or	*/
/* we call sockClose.							*/
/************************************************************************/
static ST_VOID slogIpc_uSockDisconnectInd (struct tag_GEN_SOCK *pSock)
  {
SLOGIPC_CTX      *ctx;
SLOGIPC_CON_CTRL *conCtrl;
GEN_SOCK_DATA    *sockData;
SLOGIPC_DISCONNECT_IND slogIpcDisconnectInd;

  ctx = (SLOGIPC_CTX *) pSock->sockCfg.usr1;
  conCtrl = (SLOGIPC_CON_CTRL *) pSock->sockCfg.usr2;

/* If the user wants to know, let them */
  if (ctx->lc->ipc.slog_ipc_disconnect_ind != NULL && conCtrl != NULL)
    {
    slogIpcDisconnectInd.conCtrl = conCtrl;
    slogIpcDisconnectInd.ctx 	 = ctx;
    slogIpcDisconnectInd.pSock   = pSock;
    (*ctx->lc->ipc.slog_ipc_disconnect_ind) (&slogIpcDisconnectInd);
    }

  if (pSock->role == GS_ROLE_LISTENING)
    {
    ctx->listenSock = NULL;
    sockFree (pSock);
    return;
    }

  S_LOCK_UTIL_RESOURCES ();

/* If this was a calling socket, we need to try to reconnect later */
  if (pSock->role == GS_ROLE_CALLING)
    {
    ctx->callingSock = NULL;
    ctx->nextCallingConnectTime = sGetMsTime () + (ST_DOUBLE) ctx->lc->ipc.callingBackoff;
    }

  sockFree (pSock);

  if (conCtrl != NULL)
    {
    /* free buffers in send que */
    while (conCtrl->sendQue != NULL)
      {
      sockData = conCtrl->sendQue;
      list_unlink (&conCtrl->sendQue, sockData);
      slogIpc_uDataBufFree (NULL, sockData);
      }
  
    /* remove the connection control from list */
    list_unlink (&ctx->conCtrlList, conCtrl);
    chk_free (conCtrl);
    }
  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*			slogIpc_uSockRxInd				*/
/*----------------------------------------------------------------------*/
/* This is gensock2 callback function when socket receives a msg.	*/
/* NOTE that we do not expect to receive anything on that socket, any	*/
/* bytes received will be ignored if not in smartMode.			*/
/************************************************************************/


static ST_VOID slogIpc_uSockRxInd (struct tag_GEN_SOCK *pSock, GEN_SOCK_DATA *sockData)
{
SLOGIPC_CON_CTRL *conCtrl;
SLOGIPC_CTX 	 *ctx;
SLOGIPC_SMART_MSG_HDR  *sHdr;
ST_CHAR 	 *msgData;
ST_UINT32  	  msgType;
ST_UINT32  	  msgDataLen;
SLOGIPC_RX_IND	  slogIpcRxInd;

  if (sockData->result != SD_SUCCESS)
    {
    /* socket is disconnecting, any received data buffers need to be free */
    slogIpc_uDataBufFree (NULL, sockData);
    return;
    }

  ctx = (SLOGIPC_CTX *) pSock->sockCfg.usr1;
  conCtrl =  (SLOGIPC_CON_CTRL *) pSock->sockCfg.usr2;

  if (ctx->smartMode == SD_TRUE)
    {
    sHdr = (SLOGIPC_SMART_MSG_HDR *) (sockData->data);	/* point to magic number */
    msgType    = ntohl(sHdr->messageType);
    msgDataLen = ntohl(sHdr->messageLen);
    msgData    = (ST_CHAR *) sHdr;
    msgData   += conCtrl->hdrSize;

    slogIpcRxInd.conCtrl 	= conCtrl;
    slogIpcRxInd.ctx		= ctx;
    slogIpcRxInd.pSock		= pSock;
    slogIpcRxInd.sockData	= sockData;
    slogIpcRxInd.msgData	= msgData;
    slogIpcRxInd.msgType	= msgType;
    slogIpcRxInd.msgDataLen	= msgDataLen;
    if (ctx->lc->ipc.slog_ipc_rx_ind != NULL)
      {
      (*ctx->lc->ipc.slog_ipc_rx_ind) (&slogIpcRxInd);
      return;
      }

    switch (msgType)
      {
      case SLOGIPC_MSG_TYPE_START:
        conCtrl->txEnable = SD_TRUE;
        slogIpc_uDataBufFree (NULL, sockData);
      break;

      case SLOGIPC_MSG_TYPE_STOP:
        conCtrl->txEnable = SD_FALSE;
        slogIpc_uDataBufFree (NULL, sockData);
      break;

      case SLOGIPC_MSG_TYPE_LOG:	/* Should not be getting any of these 	*/
      case SLOGIPC_MSG_TYPE_LOG_MISSED: /* (Probably connected to a slogIpc log	*/
      case SLOGIPC_MSG_TYPE_APPID:	/* source by mistake)			*/
      case SLOGIPC_MSG_TYPE_OPTIONS:
      case SLOGIPC_MSG_TYPE_RESERVED:
        conCtrl->txEnable = SD_FALSE;
        slogIpc_uDataBufFree (NULL, sockData);
      break;


    /* All others can get passed to the application */
      case SLOGIPC_MSG_TYPE_RESP_SUCCESS:
      case SLOGIPC_MSG_TYPE_RESP_FAILURE:
      default:
        if (ctx->lc->ipc.slog_ipc_cmd_fun != NULL)
          {
          (*ctx->lc->ipc.slog_ipc_cmd_fun) (&slogIpcRxInd);
          }
	else /* No command handler, just ignore. Otherwise, can get */
	  {  /*	in loop sending FAILURES back & forth 		    */
          slogIpc_uDataBufFree (NULL, sockData);
	  }
      break;
      }
    }
  }

/************************************************************************/
/*			slogIpcSaveLoggingParams   			*/
/************************************************************************/

static ST_VOID slogIpcSaveLoggingParams(LOG_CTRL *lc, SLOGIPC_SV_SLOG *svSlog)
  {
  /* prevent any logging from code handling IPC slogging */
  /* NOTE: we may still have problems if program changes some of the	*/
  /* slog params that we saved and reset them while we exec this fun!	*/
  svSlog->logCtrl = lc->logCtrl;

  svSlog->slog_dyn_log_fun  = slog_dyn_log_fun;
  svSlog->_slog_dyn_log_fun = _slog_dyn_log_fun;
  svSlog->slog_remote_fun   =  slog_remote_fun;
  svSlog->_slog_remote_fun  = _slog_remote_fun;
  slog_dyn_log_fun  = NULL;
  _slog_dyn_log_fun = NULL;
  slog_remote_fun   = NULL;
  _slog_remote_fun  = NULL;

  lc->logCtrl &= ~LOG_FILE_EN;
  lc->logCtrl &= ~LOG_MEM_EN;
  lc->logCtrl &= ~LOG_IPC_EN;
  }

/************************************************************************/
/*			slogIpcRestoreLoggingParams   			*/
/************************************************************************/

static ST_VOID slogIpcRestoreLoggingParams(LOG_CTRL *lc, SLOGIPC_SV_SLOG *svSlog)
  {
  lc->logCtrl = svSlog->logCtrl;	/* restore the slogging control		*/
  slog_dyn_log_fun  = svSlog->slog_dyn_log_fun;
  _slog_dyn_log_fun = svSlog->_slog_dyn_log_fun;
  slog_remote_fun   = svSlog->slog_remote_fun;
  _slog_remote_fun  = svSlog->_slog_remote_fun;
  }

/************************************************************************/
/*			slogIpcSendQueued				*/
/*----------------------------------------------------------------------*/
/* Send all queued log messages on a socket.				*/
/************************************************************************/

static ST_RET slogIpcSendQueued (struct tag_GEN_SOCK *pSock)
{
ST_RET            rtn = SD_SUCCESS;
SLOGIPC_CON_CTRL *conCtrl;
GEN_SOCK_DATA    *sockData;
ST_INT            numSent;

  conCtrl = (SLOGIPC_CON_CTRL *) pSock->sockCfg.usr2;

  S_LOCK_UTIL_RESOURCES ();


  /* send as many queued messages as possible */
  while (conCtrl->sendQue != NULL)
    {
    sockData = conCtrl->sendQue;
    rtn = sockTx (pSock, sockData, &numSent);

    if (rtn == SD_SUCCESS)
      {
      if (numSent == sockData->dataLen)
        {
        /* the whole message has been sent, unlink the buffer */
        ++pSock->sockStats.numSend;		/* num of msgs sent	*/

        if ( conCtrl->sendQueCnt > 0)
	  --conCtrl->sendQueCnt;
        list_unlink (&conCtrl->sendQue, sockData);
        slogIpc_uDataBufFree (NULL, sockData);
        }
      else
        {
        /* data sent partially, adjust for next send attempt */
        sockData->data += numSent;
        sockData->dataLen -= numSent;
        break;	/* Stop trying to send */
        }
      }
    else
      {
      /* SendQueue and conCtrl are free'd on sockTx failure */
      break;
      }
    }

  S_UNLOCK_UTIL_RESOURCES ();

  return (rtn);
}

/************************************************************************/
/*			slogIpc_uSockWritable				*/
/*----------------------------------------------------------------------*/
/* This function is called by gensock2 when the socket becomes writable.*/
/************************************************************************/
static ST_VOID slogIpc_uSockWritable (struct tag_GEN_SOCK *pSock)
{
SLOGIPC_CTX      *ctx;
SLOGIPC_CON_CTRL *conCtrl;
LOG_CTRL         *lc;
SLOGIPC_SV_SLOG   svSlog;

  ctx = (SLOGIPC_CTX *) pSock->sockCfg.usr1;
  conCtrl = (SLOGIPC_CON_CTRL *) pSock->sockCfg.usr2;
  lc = ctx->lc;

  S_LOCK_UTIL_RESOURCES ();
  if (conCtrl->conState != SLOGIPC_CONNECTED || ctx->state != SLOGIPC_CTX_ACTIVE)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

  slogIpcSaveLoggingParams(lc, &svSlog);
  slogIpcSendQueued (pSock);
  slogIpcRestoreLoggingParams(lc, &svSlog);

  S_UNLOCK_UTIL_RESOURCES ();

}

/************************************************************************/
/*                              slogIpcInit				*/
/*----------------------------------------------------------------------*/
/* Initialize the socket logging subsystem.				*/
/* Parameters:								*/
/*	lc		Pointer to LOG_CTRL that is used for logging.	*/
/* Return values:							*/
/*	SD_SUCCESS	if functions successful				*/
/*	SD_FAILURE	or other error otherwise.			*/
/************************************************************************/
/************************************************************************/

ST_RET slogIpcInit (LOG_CTRL *lc)
{
ST_RET          rtn;
SLOGIPC_CTX    *ctx;
GEN_SOCK_CONFIG sockCfg;
ST_UINT16	portNum;


  if (sNonStandardLogMode)
    return (SD_SUCCESS);

  /* Init only if Calling and/or Listening option is configured */
  if ((lc->logCtrl & LOG_IPC_LISTEN_EN) == 0  &&
      (lc->logCtrl & LOG_IPC_CALL_EN) == 0)
    return (SD_SUCCESS);

  S_LOCK_UTIL_RESOURCES ();

  ctx = &lc->slogIpcCtx;
  rtn = SD_SUCCESS;

/* If we are already active, we are done */
  if (ctx->state == SLOGIPC_CTX_ACTIVE)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return (SD_SUCCESS);
    }

/* Must be terminating, can't do it ... */
  if (ctx->state != SLOGIPC_CTX_IDLE)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return (SD_FAILURE);
    }

  ctx->lc = lc;

  /* are we running in smart mode? */
  if(lc->logCtrl & LOG_IPC_SMART)
    ctx->smartMode = SD_TRUE;
  else
    ctx->smartMode = SD_FALSE;

   /* make sure port and maxConns are set to some good values */
  if (!lc->ipc.maxQueCnt)
    lc->ipc.maxQueCnt = IPC_LOG_MAX_QUEUE_CNT;
  
  /* initialize gensock2 before calling any of the socket functions	*/
  /* NOTE: slogipc uses default GEN_SOCK_WAKEUP_PORT_BASE and		*/
  /*       GEN_SOCK_WAKEUP_PORT_RANGE for init of wakeup socket.	*/
  lc->slogIpcCtx.sockCtx = (GEN_SOCK_CTXT *) chk_calloc (1, sizeof(GEN_SOCK_CTXT));
  rtn = sockStartEx (SLOGIPC_NAME, lc->slogIpcCtx.sockCtx, SD_TRUE);
  if (rtn != SD_SUCCESS)
    {
    ctx->state = SLOGIPC_CTX_IDLE;
    chk_free (lc->slogIpcCtx.sockCtx);
    lc->slogIpcCtx.sockCtx = NULL;
    S_UNLOCK_UTIL_RESOURCES ();
    return (rtn);
    }

  /* OK, we are acive */
  ctx->state = SLOGIPC_CTX_ACTIVE;
  lc->slogIpcCtx.sockCtx->log_disable = SD_TRUE;


  /* If LISTEN enabled, start listening */
  if (lc->logCtrl & LOG_IPC_LISTEN_EN)
    {
  /* make sure port and maxConns are set to some good values */
    if (!lc->ipc.port)
      lc->ipc.port = IPC_LOG_BASE_PORT;
    if (!lc->ipc.portCnt)
      lc->ipc.portCnt = 1;
    if (!lc->ipc.maxConns)
      lc->ipc.maxConns = IPC_LOG_MAX_CONNECTIONS;


  /* Start listening */
    _slogIpcInitSockCfg (lc, &sockCfg);
  
    /* init the listening socket */
    /* for multiple instances of an application try range of ports 	*/
    portNum = lc->ipc.port;
    while (portNum < lc->ipc.port + lc->ipc.portCnt)
      {
      rtn = sockInitListen (lc->slogIpcCtx.sockCtx, &sockCfg, portNum, lc->ipc.maxConns, &ctx->listenSock);
      if (rtn == SD_SUCCESS)
        {
        lc->ipc.portUsed = portNum;
        break;
        }
      else
        {
        ctx->listenSock = NULL;	       	/* need to be set if bind fails	*/
        if (rtn == SOCK_EADDRINUSE)
          ++portNum;			/* try next port		*/
        else
          break;			/* other error			*/
        }
      }
    }

/* If CALLING enabled, initiate a connection */
  if (ctx->callingEnabled)
    {
  /* Make retry timeout is reasonable */
    if (ctx->lc->ipc.callingBackoff == 0)
      ctx->lc->ipc.callingBackoff = 1000;

    ctx->callingSock = NULL;
    ctx->nextCallingConnectTime = 0.0;
    slogIpcInitCalling (lc);
    }

  S_UNLOCK_UTIL_RESOURCES ();

  return (rtn);
}

/************************************************************************/
/*                              slogIpcStop				*/
/*----------------------------------------------------------------------*/
/* Call this function to close all sockets and release resources used 	*/
/* socket logging subsystem.						*/
/* Parameters:								*/
/*	none								*/
/* Return values:							*/
/*	SD_SUCCESS	if functions successful				*/
/*	SD_FAILURE	or other error otherwise.			*/
/************************************************************************/
#define _SLOGIPC_MAX_STOP_TIME	5000
ST_RET slogIpcStop (LOG_CTRL *lc)
{
ST_RET            rtn = SD_SUCCESS;
SLOGIPC_CTX      *ctx;
SLOGIPC_CON_CTRL *conCtrl;
SLOGIPC_CON_CTRL *nextConCtrl;
ST_DOUBLE	  startTime;
ST_DOUBLE	  currTime;

  /* Note: the S_LOCK_UTIL_RESOURCES must lock only portions of the code*/
  /*       since we will wait on work performed by gensock2 threads.	*/

  S_LOCK_UTIL_RESOURCES ();

/* If already idle, we are done */
  ctx = &lc->slogIpcCtx;
  if (ctx->state == SLOGIPC_CTX_IDLE)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return (SD_SUCCESS);
    }

  ctx->state = SLOGIPC_CTX_TERMINATING;
  S_UNLOCK_UTIL_RESOURCES ();

  startTime = sGetMsTime ();

  /* close listen socket */
  if (ctx->listenSock)
    {
    ctx->listenSock->disconnectReason =  GS_DISCONNECT_NORMAL_DISCONNECT;
    sockClose (ctx->listenSock);
    while (ctx->listenSock != NULL)
      {
      SLOGIPC_WAIT(ctx->sockCtx, 20);		/* wait until listen socket closes	*/
      currTime = sGetMsTime ();
      if (currTime - startTime > _SLOGIPC_MAX_STOP_TIME)
	break;
      }
    }

  /* disconnect all connected sockets */
  S_LOCK_UTIL_RESOURCES ();
  conCtrl = ctx->conCtrlList;
  while (conCtrl)
    {
    nextConCtrl = (SLOGIPC_CON_CTRL *) list_get_next (ctx->conCtrlList, conCtrl);
    conCtrl->conState = SLOGIPC_CON_CLOSING;
    conCtrl->conSock->disconnectReason =  GS_DISCONNECT_NORMAL_DISCONNECT;
    sockClose (conCtrl->conSock); /* Will likely remove conCtrl from the conCtrlList & free it */
    conCtrl = nextConCtrl;
    }
  S_UNLOCK_UTIL_RESOURCES ();
  
  /* wait until all socket connections terminate */
  while (ctx->conCtrlList)
    {
    SLOGIPC_WAIT(ctx->sockCtx, 20);		/* wait until listen socket closes	*/
    currTime = sGetMsTime ();
    if (currTime - startTime > _SLOGIPC_MAX_STOP_TIME)
      break;
    }
  
  /* cleanup gensock2 resources, terminate threads,... */
  rtn = sockEnd (lc->slogIpcCtx.sockCtx);

  S_LOCK_UTIL_RESOURCES ();
  if (rtn == SD_SUCCESS)
    {
    chk_free (lc->slogIpcCtx.sockCtx);
    lc->slogIpcCtx.sockCtx = NULL;
    }
  ctx->state = SLOGIPC_CTX_IDLE;
  S_UNLOCK_UTIL_RESOURCES ();

  return (rtn);
}

/************************************************************************/
/*                              slogIpcEvent				*/
/*----------------------------------------------------------------------*/
/* Call this function to process slogIpc events. This function needs	*/
/* to be called only if the gensock2 is compiled without		*/
/* GENSOCK_THREAD_SUPPORT defined.					*/
/* Parameters:								*/
/*	none								*/
/* Return values:							*/
/*	SD_SUCCESS	if functions successful				*/
/*	SD_FAILURE	or other error otherwise.			*/
/************************************************************************/

ST_RET slogIpcEvent ()
  {
  return (slogIpcEventEx (sLogCtrl));
  }


ST_RET slogIpcEventEx (LOG_CTRL *lc)
{
#if !defined(GENSOCK_THREAD_SUPPORT)
SLOGIPC_CTX *ctx;

  ctx = &lc->slogIpcCtx;

  if (ctx->state == SLOGIPC_CTX_ACTIVE)
    sockServiceAll (ctx->sockCtx, 0);	/* arg is timeout in milliseconds*/
#endif /* !defined(GENSOCK_THREAD_SUPPORT) */

  return (SD_SUCCESS);
}

/************************************************************************/
/*                              slogIpc					*/
/*----------------------------------------------------------------------*/
/* This function will send the log message to all connected clients.	*/
/* If this is first log message this function will initialize the	*/
/* socket slogging subsystem.						*/
/* Parameters:								*/
/*	see slog() parameters						*/
/* Return values:							*/
/*	none								*/
/************************************************************************/

ST_VOID slogIpc (LOG_CTRL *lc, ST_INT logType,
		 SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		 SD_CONST ST_CHAR *SD_CONST sourceFile,
		 ST_INT lineNum, ST_INT bufLen, 
                 ST_CHAR *buf)
  {
SLOGIPC_CTX      *ctx;
SLOGIPC_CON_CTRL *conCtrl;
GEN_SOCK_DATA    *sockData;
ST_UINT		  slogDataBufLen;
ST_CHAR          *p;
ST_CHAR          *logMsg;
ST_INT            logMsgLen;
ST_BOOLEAN        firstConn;
SLOGIPC_SV_SLOG   svSlog;

  S_LOCK_UTIL_RESOURCES ();
  ctx = &lc->slogIpcCtx;
  if (ctx->state != SLOGIPC_CTX_ACTIVE || ctx->lc != lc)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;	/* Application needs to call slogIpcInit function	*/
    }

  /* prevent any logging from code handling IPC slogging */
  /* NOTE: we may still have problems if program changes some of the	*/
  /* slog params that we saved and reset them while we exec this fun!	*/
  slogIpcSaveLoggingParams(lc, &svSlog);

  /* first queue message to all clients */
  conCtrl = ctx->conCtrlList;
  firstConn = SD_TRUE;
  while (conCtrl)
    {
    if (conCtrl->conState == SLOGIPC_CONNECTED)
      {
      /* setup the smart mode header */
      if (ctx->smartMode == SD_TRUE)
        {
        if (conCtrl->txEnable == SD_TRUE)
	  {
          if (firstConn == SD_TRUE)
	    {
            slogDataBufLen = conCtrl->hdrSize +		/* IPC CMD Headers	*/ 
        		       SLOG_MAX_HDR +		/* log header		*/
        		       2 +			/* CR NL		*/
        		       bufLen+1 + 		/* msg in buf, +1 for NULL */
        		       1;	  		/* CR			*/

            slogIpc_DataBufAlloc (conCtrl->conSock, slogDataBufLen, &sockData);
	  
            p = (ST_CHAR *) sockData->data + conCtrl->hdrSize;
            slogSetHdr (lc, logType, logTypeStr, sourceFile, lineNum,  p, SLOG_MAX_HDR, "\r\n");

            /* Now put the message to buffer */
            strncat_maxstrlen (p, (ST_CHAR *) buf, slogDataBufLen - conCtrl->hdrSize-1);
	    logMsgLen = (int) strlen (p);

            /* Save for next socket */
            logMsg = p;
            firstConn = SD_FALSE;
	    }
	  else /* Not first time, just send duplicate */
	    {
            slogIpc_DataBufAlloc (conCtrl->conSock, slogDataBufLen, &sockData);
            p = (ST_CHAR *) sockData->data + conCtrl->hdrSize;
	    *p = 0;
            strncat_maxstrlen (p, logMsg, slogDataBufLen - conCtrl->hdrSize-1);
	    }

          _slogIpcInitHdr (conCtrl->conSock, sockData, SLOGIPC_MSG_TYPE_LOG, logMsgLen);
          slogIpcAddSendQue (lc, ctx, conCtrl, sockData);
	  }
        }
      else /* not smart mode */
        {
        slogDataBufLen = SLOG_MAX_HDR +		/* log header		*/
        		   2 +				/* CR NL		*/
        		   bufLen+1 +			/* msg in buf, +1 for NULL */
        		   1;				/* CR			*/

        slogIpc_DataBufAlloc (conCtrl->conSock, slogDataBufLen , &sockData);

        slogSetHdr (lc, logType, logTypeStr, sourceFile, lineNum, (ST_CHAR *)sockData->data, SLOG_MAX_HDR, "\r\n");
        /* Now put the message to buffer	*/
        strncat_maxstrlen ((ST_CHAR *) sockData->data, (ST_CHAR *) buf, slogDataBufLen-1);
        sockData->dataLen = (int) strlen ((ST_CHAR *)sockData->data);
        slogIpcAddSendQue (lc, ctx, conCtrl, sockData);
	}
      }
    conCtrl = (SLOGIPC_CON_CTRL *) list_get_next (ctx->conCtrlList, conCtrl);
    }

  /* now try to send all the messages */
  conCtrl = ctx->conCtrlList;
  while (conCtrl)
    {
    if (conCtrl->conState == SLOGIPC_CONNECTED)
      {
      if (ctx->smartMode == SD_FALSE || conCtrl->txEnable == SD_TRUE)
        slogIpcSendQueued (conCtrl->conSock);
      }
    conCtrl = (SLOGIPC_CON_CTRL *) list_get_next (ctx->conCtrlList, conCtrl);
    }

  slogIpcRestoreLoggingParams(lc, &svSlog);

/* In case we need to establish a calling connection */
  if (ctx->callingEnabled)
    slogIpcInitCalling (lc);

  S_UNLOCK_UTIL_RESOURCES ();
  }


/************************************************************************/
/*                       slogIpcSendData	                        */
/************************************************************************/

ST_VOID slogIpcSendData (GEN_SOCK *pSock, ST_UINT32 msgType, ST_INT dataLen, ST_UCHAR *data)
  {
GEN_SOCK_DATA 	 *sockData;
SLOGIPC_CON_CTRL *conCtrl;
SLOGIPC_CTX      *ctx;
LOG_CTRL         *lc;
SLOGIPC_SV_SLOG   svSlog;

  S_LOCK_UTIL_RESOURCES();
  if (pSock->sockState != GS_STATE_CONNECTED)
    {
    S_UNLOCK_UTIL_RESOURCES();
    return;
    }

  ctx     = (SLOGIPC_CTX *)      pSock->sockCfg.usr1;
  lc      = ctx->lc;
  conCtrl = (SLOGIPC_CON_CTRL *) pSock->sockCfg.usr2;

  slogIpcSaveLoggingParams(lc, &svSlog);
  
  slogIpc_DataBufAlloc (pSock, (conCtrl->hdrSize + dataLen), &sockData);

  /* Add the data */
  memcpy (sockData->data+conCtrl->hdrSize, data, dataLen);

  /* Add to the queue */
  _slogIpcInitHdr (pSock, sockData, msgType, dataLen);
  slogIpcAddSendQue (NULL, ctx, conCtrl, sockData);

  /* Service the queue */
  slogIpcSendQueued (pSock);

  /* Restore log parameters */
  slogIpcRestoreLoggingParams(lc, &svSlog);
  S_UNLOCK_UTIL_RESOURCES();
  }


/************************************************************************/
/************************************************************************/
ST_VOID slogIpcSendRawData (LOG_CTRL *lc, ST_INT bufLen, ST_CHAR *buf, ST_BOOLEAN sendAlways)

  {
SLOGIPC_CTX      *ctx;
SLOGIPC_CON_CTRL *conCtrl;
GEN_SOCK_DATA    *sockData;
ST_BOOLEAN        firstConn;
SLOGIPC_SV_SLOG   svSlog;

  S_LOCK_UTIL_RESOURCES ();
  ctx = &lc->slogIpcCtx;
  if (ctx->state != SLOGIPC_CTX_ACTIVE || ctx->lc != lc)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;	/* Application needs to call slogIpcInit function	*/
    }

  /* prevent any logging from code handling IPC slogging */
  /* NOTE: we may still have problems if program changes some of the	*/
  /* slog params that we saved and reset them while we exec this fun!	*/
  slogIpcSaveLoggingParams(lc, &svSlog);

  /* first queue message to all clients */
  conCtrl = ctx->conCtrlList;
  firstConn = SD_TRUE;
  while (conCtrl)
    {
    if (conCtrl->conState == SLOGIPC_CONNECTED)
      {
      slogIpc_DataBufAlloc (conCtrl->conSock, bufLen , &sockData);

      /* Now put the message to buffer	*/
      memcpy (sockData->data, buf, bufLen);
      sockData->dataLen = bufLen;
      slogIpcAddSendQue (lc, ctx, conCtrl, sockData);
      }
    conCtrl = (SLOGIPC_CON_CTRL *) list_get_next (ctx->conCtrlList, conCtrl);
    }

  /* now try to send all the messages */
  conCtrl = ctx->conCtrlList;
  while (conCtrl)
    {
    if (conCtrl->conState == SLOGIPC_CONNECTED)
      {
      if (ctx->smartMode == SD_FALSE || conCtrl->txEnable == SD_TRUE || sendAlways == SD_TRUE)
        slogIpcSendQueued (conCtrl->conSock);
      }
    conCtrl = (SLOGIPC_CON_CTRL *) list_get_next (ctx->conCtrlList, conCtrl);
    }

  slogIpcRestoreLoggingParams(lc, &svSlog);
  S_UNLOCK_UTIL_RESOURCES ();
  }


/************************************************************************/
/*			slogIpcAddSendQue				*/
/************************************************************************/

static ST_VOID slogIpcAddSendQue (LOG_CTRL *lc, SLOGIPC_CTX *ctx, 
				  SLOGIPC_CON_CTRL *conCtrl,
				  GEN_SOCK_DATA    *sockData)
  {
GEN_SOCK_DATA *sd;
GEN_SOCK_DATA *lastCtrlMsg;
SLOGIPC_SMART_MSG_HDR *sHdr;
SLOGIPC_MSG_INFO *ipcMsgInfo;
ST_UINT32 msgType;
ST_UINT32 *pUl;
ST_UINT32 missedMsgCount;

  ipcMsgInfo = (SLOGIPC_MSG_INFO *) sockData->usr1;

  if (lc)	/* If this is a log message */
    {
    ipcMsgInfo->logMsg = SD_TRUE;

  /* See if there is room at in the queue */
    if (conCtrl->sendQueCnt < lc->ipc.maxQueCnt)
      {
      ++conCtrl->sendQueCnt;
      list_add_last (&conCtrl->sendQue, sockData);
      }
    else
      { 
    /* Overflow .... This one is lost */
      slogIpc_uDataBufFree (NULL, sockData);

    /* Take a look at the last one, see if it is a 'missed' message */
      sockData = (GEN_SOCK_DATA *) list_find_last ((DBL_LNK *) conCtrl->sendQue);
      if (sockData)
        {
        sHdr = (SLOGIPC_SMART_MSG_HDR *) sockData->data;
        msgType = ntohl(sHdr->messageType);
        if (msgType == SLOGIPC_MSG_TYPE_LOG_MISSED)
          {
        /* Increment the missed count to 1 */
          pUl = (ST_UINT32 *) (sockData->data + conCtrl->hdrSize);
          missedMsgCount = ntohl(*pUl);
          ++missedMsgCount;
          *pUl = htonl (missedMsgCount);
          }
        else /* We need to queue up a 'LOG MISSED' message */
          {
          slogIpc_DataBufAlloc (conCtrl->conSock, (conCtrl->hdrSize + sizeof (ST_UINT32)), &sockData);

        /* Set the missed count to 1 */
          pUl = (ST_UINT32 *) (sockData->data + conCtrl->hdrSize);
          *pUl = htonl (1);

          _slogIpcInitHdr (conCtrl->conSock, sockData, SLOGIPC_MSG_TYPE_LOG_MISSED, sizeof (ST_UINT32));
          ++conCtrl->sendQueCnt;
          list_add_last (&conCtrl->sendQue, sockData);
          }
        }	/* end "if (sockData)"	*/
      }	/* end "overflow handling" block	*/
    }
  else 	/* Not a log message, put after the last control message */
    {
    ipcMsgInfo->logMsg = SD_FALSE;
    ++conCtrl->sendQueCnt;

    lastCtrlMsg = NULL;
    sd = conCtrl->sendQue;
    while (sd != NULL)
      {
      ipcMsgInfo = (SLOGIPC_MSG_INFO *) sd->usr1;
      if (ipcMsgInfo->logMsg == SD_FALSE)
        lastCtrlMsg = sd;
      else
        break;

      sd = (GEN_SOCK_DATA *) list_get_next (conCtrl->sendQue, sd);
      }

    if (lastCtrlMsg == NULL)
      list_add_first (&conCtrl->sendQue, sockData);
    else
      list_add_node_after (lastCtrlMsg, sockData);
    
    sd = conCtrl->sendQue;
    }
  }

/************************************************************************/
/************************************************************************/
/* Standard IPC Command Processing Framework				*/
/************************************************************************/

SLOGIPC_CMD *slogIpcCmdList;
ST_INT 	    slogIpcMaxCmdPend = 1;

/************************************************************************/
/*			slog_ipc_std_cmd_fun				*/
/************************************************************************/

ST_VOID slog_ipc_std_cmd_fun (SLOGIPC_RX_IND *slogIpcRxInd)
  {
SLOGIPC_CMD *slogIpcCmd;

/* If we have room on the que, alloc a ctrl struct for this command */
  if (list_get_sizeof (slogIpcCmdList) < slogIpcMaxCmdPend)
    { 
  /* We will want to save the message data */
    slogIpcCmd = (SLOGIPC_CMD *) chk_calloc (1, sizeof (SLOGIPC_CMD) + slogIpcRxInd->msgDataLen +1);
    slogIpcCmd->slogIpcRxInd = *slogIpcRxInd;

  /* Persist the received data */
    slogIpcCmd->msgData = (ST_CHAR *) (slogIpcCmd + 1);
    if (slogIpcRxInd->msgDataLen != 0)
      memcpy (slogIpcCmd->msgData, slogIpcRxInd->msgData, slogIpcRxInd->msgDataLen);

    S_LOCK_UTIL_RESOURCES ();
    list_add_last (&slogIpcCmdList, slogIpcCmd);
    S_UNLOCK_UTIL_RESOURCES ();
    }
  else	/* No room ... */
    {
    slogIpc_uDataBufFree (NULL, slogIpcRxInd->sockData);
    slogIpcSendData (slogIpcRxInd->pSock, SLOGIPC_MSG_TYPE_RESP_FAILURE, 0, NULL);
    }
  }

/************************************************************************/
/************************************************************************/
/*                       slogIpcCallingEnable   			*/
/*----------------------------------------------------------------------*/
/* An application has to enable calling before the connection request   */
/* will be made. If the enable is SD_FALSE, an active calling connection*/
/* will be terminated.                                                  */
/************************************************************************/

ST_VOID slogIpcCallingEnable (ST_BOOLEAN enable)
  {
  slogIpcCallingEnableEx (sLogCtrl, enable);
  }

ST_VOID slogIpcCallingEnableEx (LOG_CTRL *lc, ST_BOOLEAN enable)
  {
SLOGIPC_CTX *ctx;
ST_UINT32 logCtrl;

  if (sNonStandardLogMode)
    return;

  S_LOCK_UTIL_RESOURCES ();
  ctx = &lc->slogIpcCtx;

/* Don't allow enable if the LogCtrl bit is not set */
  logCtrl = lc->logCtrl;
  if ((lc->logCtrl & LOG_IPC_CALL_EN) == 0)
    enable = SD_FALSE;
    
  ctx->callingEnabled = enable;

  if (!ctx->callingEnabled && ctx->callingSock)
    sockClose (ctx->callingSock);

  if (ctx->callingEnabled)
    {
    ctx->nextCallingConnectTime = 0.0;
    slogIpcInitCalling (ctx->lc);
    }

  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*                          slogIpcInitCalling   			*/
/* Initialize slogIpc calling connections.                              */
/************************************************************************/

static ST_RET slogIpcInitCalling (LOG_CTRL *lc)
  {
ST_RET    rtn = SD_SUCCESS;
GEN_SOCK_CONFIG sockCfg;
SLOGIPC_CTX *ctx;
SLOGIPC_SV_SLOG   svSlog;
ST_DOUBLE currTime;

/* See if we are enabled */
  ctx = &lc->slogIpcCtx;
  if (!ctx->callingEnabled)
    return (SD_SUCCESS);

/* If we already have a callng socket ...*/
  if (ctx->callingSock != NULL)
    return (SD_SUCCESS);

/* See if we are in backoff */
  currTime = sGetMsTime ();
  if (currTime < ctx->nextCallingConnectTime)
    return (SD_SUCCESS);

/* OK, time to connect */
  S_LOCK_UTIL_RESOURCES ();
  slogIpcSaveLoggingParams(lc, &svSlog);

  _slogIpcInitSockCfg (lc, &sockCfg);
  rtn = sockInitCalling (ctx->sockCtx, &sockCfg, 
  			 lc->ipc.callingPort, lc->ipc.callingIp, 
  			 &ctx->callingSock);
  
  /* sockInitCalling failed, cleanup */
  if (rtn != SD_SUCCESS)
    {
    ctx->nextCallingConnectTime = currTime + (ST_DOUBLE) ctx->lc->ipc.callingBackoff;
    ctx->callingSock = NULL;
    }
  slogIpcRestoreLoggingParams(lc, &svSlog);
  S_UNLOCK_UTIL_RESOURCES ();
  return (rtn);
  }


/************************************************************************/
/*		_slogIpcInitSockCfg					*/
/************************************************************************/

static ST_VOID _slogIpcInitSockCfg (LOG_CTRL *lc, GEN_SOCK_CONFIG *sockCfg)
  {
SLOGIPC_CTX *ctx;

  ctx = &lc->slogIpcCtx;
  memset (sockCfg, 0, sizeof (GEN_SOCK_CONFIG));

  /* set listen socket parameters */
  if (lc->slogIpcCtx.smartMode == SD_FALSE)
    {
    sockCfg->hdrSize = 1; 	 /* set to 1 to drain the socket rcv buf */
    sockCfg->hdrAllocSize = 1;
    }
  else	/* Smart Mode */
    {
    if (lc->ipc.sealMode == SSEAL_MODE_NONE)
      {
      sockCfg->hdrSize      = sizeof (SLOGIPC_SMART_MSG_HDR);
      sockCfg->hdrAllocSize = sizeof (SLOGIPC_SMART_MSG_HDR);
      }
    else if (lc->ipc.sealMode == SSEAL_MODE_ALL)
      {
      sockCfg->hdrSize      = sizeof (SLOGIPC_SMART_MSG_HDRS);
      sockCfg->hdrAllocSize = sizeof (SLOGIPC_SMART_MSG_HDRS);
      }
    else if (lc->ipc.sealMode == SSEAL_MODE_FALLBACK)
      {
      sockCfg->hdrSize      = sizeof (SLOGIPC_SMART_MSG_HDR);
      sockCfg->hdrAllocSize = sizeof (SLOGIPC_SMART_MSG_HDRS);
      }
    }

  /* set sockopt parameters */
  sockCfg->setSockOpts = SD_TRUE;
  sockCfg->noDelay     = SD_TRUE;
  sockCfg->keepAlive   = SD_TRUE;
#if defined(_WIN32)
  /* need to set this option to use multiple listen ports by application instances	*/
  sockCfg->reuseAddr   = SD_FALSE;
#else
  /* on UNIX, Linux we like to reuse the address (to avoid the TIME_WAIT state)		*/
  sockCfg->reuseAddr   = SD_TRUE;
#endif
  sockCfg->rcvBufSize  = 0;		/* use default socket buffer size	*/
  sockCfg->sndBufSize  = 0;		/* use default socket buffer size	*/
  sockCfg->listenBacklog = 0;		/* if 0,  SOMAXCONN used 		*/
  /* set user callback functions */
  sockCfg->uSockConnectInd   = &slogIpc_uSockConnectActive;
  sockCfg->uSockConnectConf  = &slogIpc_uSockConnectActive;
  sockCfg->uSockDisconnect = &slogIpc_uSockDisconnectInd;
  sockCfg->uSockRx         = &slogIpc_uSockRxInd;
  sockCfg->uSockWritable   = &slogIpc_uSockWritable;
  sockCfg->uSockHunt       = &slogIpc_uSockHunt;
  sockCfg->uSockRxBufAlloc = &slogIpc_DataBufAlloc;
  sockCfg->uSockRxBufFree  = &slogIpc_uDataBufFree;	/* same funct for Rx,Tx*/
  sockCfg->uSockTxBufFree  = &slogIpc_uDataBufFree;
  /*  sockCfg->recvEvent     = not used */
  sockCfg->usr1 = (ST_VOID *) &lc->slogIpcCtx;
  sockCfg->usr2 = NULL;	    /* conCtrl needs to be NULL until connection active */
  }



/************************************************************************/
/*			slogIpcActiveCount				*/
/************************************************************************/

ST_INT slogIpcActiveCount (LOG_CTRL *lc)
  {
SLOGIPC_CTX      *ctx;
SLOGIPC_CON_CTRL *conCtrl;
ST_INT            activeCount;

  S_LOCK_UTIL_RESOURCES ();
  activeCount = 0;

  ctx = &lc->slogIpcCtx;
  if (ctx->state == SLOGIPC_CTX_ACTIVE)
    {
    /* first queue message to all clients */
    conCtrl = ctx->conCtrlList;
    while (conCtrl)
      {
      if (conCtrl->conState == SLOGIPC_CONNECTED)
        ++activeCount;

      conCtrl = (SLOGIPC_CON_CTRL *) list_get_next (ctx->conCtrlList, conCtrl);
      }
    }

  S_UNLOCK_UTIL_RESOURCES ();
  return (activeCount);
  }


/************************************************************************/

ST_VOID slogIpcHeartbeatService (LOG_CTRL *lc)
  {
SLOGIPC_CTX      *ctx;
SLOGIPC_CON_CTRL *conCtrl;
static ST_DOUBLE nextHeartbeatTime = 0.0;
ST_DOUBLE currTime;

  if (lc->ipc.heartbeatTxPeriod < 1.0)
    return;

  currTime = sGetMsTime ();
  if (currTime < nextHeartbeatTime)
    return;

  nextHeartbeatTime = currTime + lc->ipc.heartbeatTxPeriod;

  S_LOCK_UTIL_RESOURCES ();
  ctx = &lc->slogIpcCtx;
  if (ctx->state == SLOGIPC_CTX_ACTIVE)
    {
    /* first queue message to all clients */
    conCtrl = ctx->conCtrlList;
    while (conCtrl)
      {
      if (conCtrl->conState == SLOGIPC_CONNECTED)
        {
        slogIpcSendData (conCtrl->conSock, SLOGIPC_MSG_TYPE_HEARTBEAT, 0, NULL);
        }
      conCtrl = (SLOGIPC_CON_CTRL *) list_get_next (ctx->conCtrlList, conCtrl);
      }
    }

  S_UNLOCK_UTIL_RESOURCES ();
  }



/************************************************************************/
/************************************************************************/

ST_VOID _slogIpcInitHdr (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData, ST_UINT32 msgType, ST_INT dataLen)
  {
SLOGIPC_CTX *ctx;
SLOGIPC_CON_CTRL *conCtrl;
SLOGIPC_SMART_MSG_HDR  *sHdr;
#ifdef SLOGIPC_SEAL
SLOGIPC_SMART_MSG_HDRS *sHdrS;
#endif

  ctx = (SLOGIPC_CTX *) pSock->sockCfg.usr1;
  conCtrl =  (SLOGIPC_CON_CTRL *) pSock->sockCfg.usr2;

  sockData->dataLen = conCtrl->hdrSize + dataLen;

#ifdef SLOGIPC_SEAL
  if (conCtrl->sealMode != SSEAL_MODE_NONE)
    {
    sHdrS = (SLOGIPC_SMART_MSG_HDRS *) sockData->data;
    sockData->data = (ST_UCHAR *) sHdrS;
    sHdrS->magicNumber = htonl (conCtrl->magicNumber);
    sHdrS->messageType = htonl (msgType);
    sHdrS->messageLen  = htonl ((ST_UINT32) dataLen);
   _slogIpcSetSeal (sHdrS);
   return;
   }
#endif

  sHdr = (SLOGIPC_SMART_MSG_HDR *) sockData->data;
  sockData->data = (ST_UCHAR *) sHdr;
  sHdr->magicNumber = htonl (conCtrl->magicNumber);
  sHdr->messageType = htonl (msgType);
  sHdr->messageLen  = htonl ((ST_UINT32) dataLen);
  }

/************************************************************************/
/************************************************************************/

#ifdef SLOGIPC_SEAL

ST_VOID _slogIpcSetSeal (SLOGIPC_SMART_MSG_HDRS *sHdrS)
  {
static ST_INT slogIpcKeySel = 0;
ST_UINT32 dataCrc;

/* We won't seal log messages */
  switch (sHdrS->messageType)
    {
    case SLOGIPC_MSG_TYPE_LOG:
    case SLOGIPC_MSG_TYPE_LOG_MISSED:
      break;

    default:
    /* Calculate the data CRC (header elements) */
      dataCrc = _slogIpcCalcHeaderCrc (sHdrS);
  
    /* OK, seal it up! */
      sMakeSeal  (slogIpcKeySel, slogIpcNumKeys, slogIpcKeyTable,  dataCrc, &sHdrS->seal);
      ++slogIpcKeySel;
      if (slogIpcKeySel >= slogIpcNumKeys)
        slogIpcKeySel = 0;
    break;
    }
  }

/************************************************************************/

ST_RET _slogIpcCheckSeal (LOG_CTRL *lc, SLOGIPC_SMART_MSG_HDRS *sHdrS)
  {
ST_UINT32 dataCrc;
ST_INT sealTimeWindow;
ST_INT rc;
    
  rc = SD_SUCCESS;

/* We won't seal log messages */
  switch (sHdrS->messageType)
    {
    case SLOGIPC_MSG_TYPE_LOG:
    case SLOGIPC_MSG_TYPE_LOG_MISSED:
      break;

    default:
    /* Calculate the data CRC (header elements) */
      dataCrc = _slogIpcCalcHeaderCrc (sHdrS);

    /* Verify seal - make sure seal time it set*/
      sealTimeWindow = lc->ipc.sealTimeWindow;
      if (sealTimeWindow < 1)
        sealTimeWindow = 1;
      rc = sCheckSeal (&sHdrS->seal, slogIpcNumKeys, slogIpcKeyTable, dataCrc, sealTimeWindow);
    break;
    }
  return (rc);
  }

/************************************************************************/

static ST_UINT32 _slogIpcCalcHeaderCrc (SLOGIPC_SMART_MSG_HDRS *sHdrS)
  {
ST_UINT32 crc32;

  crc32 = SSEAL_LIB_INITIAL_CRC32;
  sIntCrc32 (sHdrS->magicNumber, &crc32);
  sIntCrc32 (sHdrS->messageType, &crc32);
  sIntCrc32 (sHdrS->messageLen,  &crc32);
  return (crc32);
  }



#endif
