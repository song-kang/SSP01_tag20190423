/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_tm.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* It has function dealing with different timeouts			*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    19     Cleaned up general PClint errors             */
/* 09/23/02  KCR    18     Removed extra include files                  */
/* 09/23/02  KCR    17     Moved thisFileName before includes           */
/* 09/13/99  MDE    16     Added SD_CONST modifiers         		*/
/* 03/05/98  IKE    15     Added Critical Sections to protect Adlc Data	*/
/*			   Renamed ipc call back funcs to handlexxx	*/
/* 02/10/98  IKE    14     Changed to ipcAddCommonTimer to use new IPC  */
/*			   which can handle more than 64 events		*/
/* 10/07/97  IKE    13	   Clean up of header files for non IPC version */
/*			   of WIN32 MMS lite				*/
/* 05/12/97  IKE    12     Changed to MMSEASE 7.0 Data Types		*/
/* 02/24/96  IKE    11	   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/* 02/03/97  IKE    10     adlcCfg per port basis			*/
/* 01/23/97  IKE    09     Added txRxModeCounter for proper behaviour 	*/
/*			   of turnaround timers				*/
/* 12/03/96  IKE    08     WIN32 Support, header files cleanup		*/ 
/* 11/06/96  IKE    07     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/09/96  IKE    06     Added startPollTimer				*/
/* 09/17/96  IKE    05     Added portTurnaroundTime func for master 	*/
/*			   Modified deviceturnaroundtime func and 	*/
/*			   time_delay funcs				*/
/* 08/22/96  IKE    04     Changed > to >= in checkTimeout functions	*/
/* 08/16/96  IKE    03     Added deviceTurnaroundTime func for master	*/
/*			   and repostTxServiceSem for lm		*/ 
/* 08/07/96  IKE    02     Added RadioDelayTime functions for slave	*/
/* 06/11/96  IKE    01     Initial Release				*/
/************************************************************************/
static char *thisFileName = __FILE__;
/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

/* Some other SISCO files						*/
#include "cfg_util.h"
#include "memlog.h"
#include "stime.h"

/* ADLC files								*/
#include "adlc.h"
#if defined(ADLC_LM)
#include "adlc_eve.h"
#endif

/************************************************************************/
/* Static variables							*/
/************************************************************************/
#if defined(ADLC_MASTER) && defined(ADLC_LM)
static ST_INT handleChangeDeviceIntoTxMode(ST_ULONG userID, ST_ULONG postCount);
static ST_INT handleChangePortIntoTxMode(ST_ULONG userID, ST_ULONG postCount);
#endif

/************************************************************************/
/* Caller : waitForResponse						*/
/* We start the response timeout when we send the P/F bit to the master	*/
/************************************************************************/
ST_VOID startRespTimeout(ADLC_PORT_INFO *pptr)
  {
  START_TIMEOUT(pptr->responseTimeout,ADLC_RESPONSE_TIMEOUT(pptr));
  }
/************************************************************************/

/************************************************************************/
ST_VOID updateRespTimeout(ADLC_PORT_INFO *pptr)
  {
  UPDATE_TIMEOUT(pptr->responseTimeout, ADLC_RESPONSE_TIMEOUT(pptr));
  }
/************************************************************************/
/* Caller : stopWaitingForResp						*/
/* We stop the response timeout when we either get the response back	*/
/* from the connection or we get timeout error				*/
/************************************************************************/
ST_VOID stopRespTimeout(ADLC_PORT_INFO *pptr)
  {
  STOP_TIMEOUT(pptr->responseTimeout);
  }
/************************************************************************/
/* Caller : timeoutService						*/
/* We check the timeout for every link (port) because there is one 	*/
/* response timeout per port 						*/
/************************************************************************/
ST_BOOLEAN checkRespTimeout(ADLC_PORT_INFO *pptr)
  {
  return (CHECK_TIMEOUT(pptr->responseTimeout));
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN isRespTimeoutEnabled(ADLC_PORT_INFO *pptr)
  {
  if (IS_TIMEOUT_ENABLED(pptr->responseTimeout))
    return (SD_TRUE);
  return (SD_FALSE);
  }  
/************************************************************************/

/************************************************************************/
ST_BOOLEAN isRespTimeoutDisabled(ADLC_PORT_INFO *pptr) 
  {
  return (IS_TIMEOUT_DISABLED(pptr->responseTimeout));
  }
#if defined(ADLC_MASTER)
/************************************************************************/

/************************************************************************/
ST_VOID startConRetryTimeout(ADLC_CON_INFO *cptr)
  {
/*ST_DOUBLE currentMs = sGetMsTime ();					*/
ST_DOUBLE delta;
ADLC_PORT_INFO *pptr = cptr->pptr;

  if (cptr->retries == 0)
    delta = ADLC_RESPONSE_TIMEOUT(pptr);
  else
    delta = min(ADLC_MAX_CONNECT_RETRY_TIMEOUT(pptr), ADLC_RESPONSE_TIMEOUT(pptr) * cptr->retries * cptr->retries);
  START_TIMEOUT(cptr->connectRetryTimeout,delta);
/*  ADLC_LOG_FLOW3("startConRetryTimeout currentMs = % .3f delta = % .3f retryTimeout = % .3f", */
/*		currentMs,delta,cptr->connectRetryTimeout); */
  }
/************************************************************************/

/************************************************************************/
ST_VOID stopConRetryTimeout(ADLC_CON_INFO *cptr)
  {
  STOP_TIMEOUT(cptr->connectRetryTimeout);
  }

/************************************************************************/

/************************************************************************/
ST_BOOLEAN checkConRetryTimeout(ADLC_CON_INFO *cptr)
  {
  return (CHECK_TIMEOUT(cptr->connectRetryTimeout));
  }
/************************************************************************/

/************************************************************************/
ST_VOID startDeviceTurnaroundTimeout(ADLC_DEV_INFO *dptr)
  {
ADLC_PORT_INFO *pptr = dptr->pptr;
#if !defined(ADLC_LM)
  if (ADLC_DEVICE_TURNAROUND_TIME(pptr))
    {
    dptr->txRxMode = RX_MODE;    /* we just got some from that device dont send for a while */
    START_TIMEOUT(dptr->turnaroundTime, ADLC_DEVICE_TURNAROUND_TIME(pptr));
    }
  else
    {
    dptr->txRxMode = TX_MODE;
    }
#endif
#if defined(ADLC_LM)
ST_INT retCode;
ST_CHAR errBuf [ERR_STR_LEN];

  if (ADLC_DEVICE_TURNAROUND_TIME(pptr))
    {
    /* add a timer so a function will be called when turnaround time will be elapsed  to change 	*/
    /* device into TX_MODE										*/
    retCode = ipcAddCommonTimer((ST_ULONG)dptr,
                        handleChangeDeviceIntoTxMode,
	  		NULL,
			(ST_ULONG) ADLC_DEVICE_TURNAROUND_TIME(pptr),
			(ST_ULONG) IPC_TIMER_SINGLE);  
    if (retCode != SD_SUCCESS )
      {
      ipcGetErrString (retCode, errBuf, sizeof (errBuf));
      ADLC_LOG_ERR2 ("ADLC ERROR: Unable to add timer event (retCode=0x%4.4X %s).",
                      retCode, errBuf);
      dptr->txRxMode = TX_MODE;
      }
    else
      {	
      ADLC_LOG_DEBUG1("Device (0x%x) into RX_MODE",dptr->address);
      dptr->txRxMode = RX_MODE;    /* we just got some from that device dont send for a while */
      dptr->txRxModeCounter++;
      }		
    }	
  else
    {
    dptr->txRxMode = TX_MODE;
    }
#endif
  }
/************************************************************************/

/************************************************************************/
ST_VOID updateDeviceTurnaroundTimeout(ADLC_DEV_INFO *dptr)
  {
  UPDATE_TIMEOUT(dptr->turnaroundTime, ADLC_DEVICE_TURNAROUND_TIME(dptr->pptr));
  }
/************************************************************************/

/************************************************************************/
ST_VOID stopDeviceTurnaroundTimeout(ADLC_DEV_INFO *dptr)
  {
  STOP_TIMEOUT(dptr->turnaroundTime);
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN checkDeviceTurnaroundTimeout(ADLC_DEV_INFO *dptr)
  {
  return (CHECK_TIMEOUT(dptr->turnaroundTime));
  }
/************************************************************************/

/************************************************************************/
ST_VOID startPortTurnaroundTimeout(ADLC_PORT_INFO *pptr)
  {
#if !defined(ADLC_LM)
  if (ADLC_PORT_TURNAROUND_TIME(pptr))
    {
    pptr->txRxMode = RX_MODE;    /* we just got some from that port dont send for a while */
    START_TIMEOUT(pptr->turnaroundTime,ADLC_PORT_TURNAROUND_TIME(pptr));
    }
  else
    {
    pptr->txRxMode = TX_MODE;
    }
#endif

#if defined(ADLC_LM)
ST_INT retCode;
ST_CHAR errBuf [ERR_STR_LEN];

  if (ADLC_PORT_TURNAROUND_TIME(pptr))
    {
    /* add a timer so a function will be called when turnaround time will be elapsed to   */
    /* change port into TX_MODE							      */
    retCode = ipcAddCommonTimer((ST_ULONG)pptr,
                        handleChangePortIntoTxMode,
	  		NULL,
			(ST_ULONG) ADLC_PORT_TURNAROUND_TIME(pptr),
			(ST_ULONG) IPC_TIMER_SINGLE);  
    if (retCode != SD_SUCCESS )
      {
      ipcGetErrString (retCode, errBuf, sizeof (errBuf));
      ADLC_LOG_ERR2 ("ADLC ERROR: Unable to add timer event (retCode=0x%4.4X %s).",
                  retCode, errBuf);
      pptr->txRxMode = TX_MODE;
      }
    else
      {
      ADLC_LOG_DEBUG1("COM%d into RX_MODE",GET_COM_NO(pptr->port));
      pptr->txRxMode = RX_MODE;    /* we just got some from that port dont send for a while */
      pptr->txRxModeCounter++;
      }	
    }	
  else
    {
    pptr->txRxMode = TX_MODE;
    }
#endif
  }
/************************************************************************/

/************************************************************************/
ST_VOID updatePortTurnaroundTimeout(ADLC_PORT_INFO *pptr)
  {
  UPDATE_TIMEOUT(pptr->turnaroundTime,ADLC_PORT_TURNAROUND_TIME(pptr));
  }
/************************************************************************/

/************************************************************************/
ST_VOID stopPortTurnaroundTimeout(ADLC_PORT_INFO *pptr)
  {
  STOP_TIMEOUT(pptr->turnaroundTime);
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN checkPortTurnaroundTimeout(ADLC_PORT_INFO *pptr)
  {
  return (CHECK_TIMEOUT(pptr->turnaroundTime));
  }
/************************************************************************/

/************************************************************************/
ST_RET startPollTimer(POLL_GROUP *pstPollGrp)
  {
ST_INT retCode = SD_SUCCESS;

#if !defined(ADLC_LM)
  START_TIMEOUT(pstPollGrp->pollTime,pstPollGrp->pollRate);
#endif /* !ADLC_LM */

#if defined(ADLC_LM)
ST_CHAR errBuf [ERR_STR_LEN];

  if (pstPollGrp->pollRate > 0.0 )
    {
    retCode = ipcAddTimer((ST_ULONG)pstPollGrp->pollGrpID,
                          handlePollEvent,
		    	  &pstPollGrp->hTimer,
		    	  (ST_ULONG) pstPollGrp->pollRate,
		    	  IPC_TIMER_REPEATED);
    if (retCode != SD_SUCCESS )
      {
      ipcGetErrString (retCode, errBuf, sizeof (errBuf));
      ADLC_LOG_ERR2 ("PollService:  Unable to add timer event (retCode=0x%4.4X %s).",
                      retCode, errBuf);
      }
    }
#endif /* ADLC_LM */
  return (retCode);
  }

/************************************************************************/

/************************************************************************/
ST_RET updatePollTimer(POLL_GROUP *pstPollGrp)
  {
ST_RET retCode = SD_SUCCESS;

#if !defined(ADLC_LM)
  UPDATE_TIMEOUT(pstPollGrp->pollTime,pstPollGrp->pollRate);
#endif /* ADLC_LM */
  return (retCode);
  }
/************************************************************************/

/************************************************************************/
ST_RET stopPollTimer(POLL_GROUP *pstPollGrp)
  {
ST_RET retCode = SD_SUCCESS;

#if !defined(ADLC_LM)
  STOP_TIMEOUT(pstPollGrp->pollTime);
#endif /* ADLC_LM */

#if defined(ADLC_LM)
ST_CHAR errBuf [ERR_STR_LEN];

  retCode = ipcDeleteTimer (pstPollGrp->hTimer);
  if (retCode != SD_SUCCESS )
    {
    ipcGetErrString (retCode, errBuf, sizeof (errBuf));
    ADLC_LOG_ERR2 ("PollService:  Unable to add timer event (retCode=0x%4.4X %s).",
                    retCode, errBuf);
    }
  else
    {
    pstPollGrp->hTimer = 0L;
    }
#endif /* ADLC_LM */
  return (retCode);
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN checkPollTimer(POLL_GROUP *pstPollGrp)
  {
#if !defined(ADLC_LM)
  return CHECK_TIMEOUT(pstPollGrp->pollTime);
#endif /* !ADLC_LM */
#if defined(ADLC_LM)
  return (SD_FALSE);
#endif
  }
#endif /* ADLC_MASTER */
#if !defined(ADLC_LM)
/************************************************************************/

/************************************************************************/
ST_VOID startFrameTimeout(ADLC_PORT_INFO *pptr) 
  {
  START_TIMEOUT(pptr->frameTimeout,ADLC_FRAME_TIMEOUT(pptr));
  }
/************************************************************************/

/************************************************************************/
ST_VOID stopFrameTimeout(ADLC_PORT_INFO *pptr) 
  {
  STOP_TIMEOUT(pptr->frameTimeout);
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN checkFrameTimeout(ADLC_PORT_INFO *pptr) 
  {
  return (CHECK_TIMEOUT(pptr->frameTimeout));
  }
#endif
/************************************************************************/

/************************************************************************/
ST_VOID startSignalTimeout(ADLC_PORT_INFO *pptr) 
  {
  START_TIMEOUT(pptr->signalTimeout,ADLC_SIGNAL_TIMEOUT(pptr));
  }
/************************************************************************/

/************************************************************************/
ST_VOID updateSignalTimeout(ADLC_PORT_INFO *pptr) 
  {
  UPDATE_TIMEOUT(pptr->signalTimeout,ADLC_SIGNAL_TIMEOUT(pptr));
  }
/************************************************************************/

/************************************************************************/
ST_VOID stopSignalTimeout(ADLC_PORT_INFO *pptr) 
  {
  STOP_TIMEOUT(pptr->signalTimeout);
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN checkSignalTimeout(ADLC_PORT_INFO *pptr) 
  {
  return (CHECK_TIMEOUT(pptr->signalTimeout));
  }

/************************************************************************/
/*			time_delay					*/
/************************************************************************/
ST_VOID time_delay (ST_DOUBLE tme)
  {
ST_DOUBLE start;

  if (!tme)
    return;

  start = sGetMsTime ();
  while (sGetMsTime() - start < tme)
    ;
  }
#if defined(ADLC_MASTER) && defined(ADLC_LM)

/************************************************************************/

/************************************************************************/
static ST_INT handleChangeDeviceIntoTxMode(ST_ULONG userID, ST_ULONG postCount)
  {
ADLC_DEV_INFO *dptr;
ST_INT retCode;

  /* wait for Adlc Data to be available, then guard it */
  EnterCriticalSection(&protectAdlcData);

  dptr = (ADLC_DEV_INFO *) userID;
  if (isDevPtrValid(dptr))	/* we checking because device could been deleted */
    {
    dptr->txRxModeCounter--;
    ADLC_LOG_FLOW2("handleChangeDeviceIntoTxMode for Device = 0x%x txRxCounter = %d",
		    dptr->address,dptr->txRxModeCounter);
    if (dptr->txRxModeCounter <= 0)
      {
      ADLC_LOG_DEBUG1("Device (0x%x) into TX_MODE",dptr->address);
      dptr->txRxMode = TX_MODE;
      if ((retCode = ipcPostCommonEvent(dptr->pptr->frameInListSem)) != SD_SUCCESS)
        logIpcError(retCode,"FrameInList Semaphore Post Failed",thisFileName,__LINE__);
      }	
    }
  /* unlock the adlc global data for other thread like main thread 	*/
  LeaveCriticalSection(&protectAdlcData);

  return (SD_SUCCESS);
  }

/************************************************************************/

/************************************************************************/
static ST_INT handleChangePortIntoTxMode(ST_ULONG userID, ST_ULONG postCount)
  {
ADLC_PORT_INFO *pptr;
ST_INT retCode;

  /* wait for Adlc Data to be available, then guard it */
  EnterCriticalSection(&protectAdlcData);

  pptr = (ADLC_PORT_INFO *) userID;
  pptr->txRxModeCounter--;
  ADLC_LOG_FLOW2("handleChangePortIntoTxMode for COM%d txRxCounter = %d",
		  GET_COM_NO(pptr->port),pptr->txRxModeCounter);
  if (pptr->txRxModeCounter <= 0)
    {
    ADLC_LOG_DEBUG1("COM%d into TX_MODE",GET_COM_NO(pptr->port));
    pptr->txRxMode = TX_MODE;
    if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
      logIpcError(retCode,"FrameInList Semaphore Post Failed",thisFileName,__LINE__);
    }

  /* unlock the adlc global data for other thread like main thread 	*/
  LeaveCriticalSection(&protectAdlcData);

  return (SD_SUCCESS);
  }
#endif /* ADLC_LM && ADLC_MASTER */
