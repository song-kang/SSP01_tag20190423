#if defined(ADLC_MASTER)
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_msv.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* 	This module contains functions for transmitting and receiving 	*/
/* data for master only							*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    34     Cleaned up general PClint errors             */
/* 09/23/02  KCR    33     Removed extra include files                  */
/* 09/23/02  KCR    32     Moved thisFileName before includes           */
/* 01/19/00  KCR    31     Added ADLC_S_DISCONNECT_PHASE into timeout_err() */
/* 09/13/99  MDE    30     Added SD_CONST modifiers         		*/
/* 08/13/98  JRB    29     Lint cleanup.				*/
/* 03/05/98  IKE    28     Added Critical Sections to protect Adlc Data	*/
/*			   Renamed ipc call back funcs to handlexxx	*/
/* 02/10/98  IKE    27     Changed to ipcPostCommonEvent and 		*/
/*			   ipcAddCommonTimer to use new IPC which can 	*/
/*			   handle more than 64 events			*/
/* 01/15/97  IKE    26     Changed the polling algorithm (transmit 	*/
/*			   subsystem) especially between INFO and RR	*/
/* 10/07/97  IKE    25     Added support for UI frames for UCA_SMP	*/
/* 07/16/97  IKE    24     Added device statistics and device filt log	*/
/*			   changed algorithm of rrtimeout		*/
/* 06/12/97  IKE    23     Replaced dptr by cptr in stopWaitingForResp 	*/
/*			   to make ADLC protocol transaction oriented	*/
/* 05/12/97  IKE    22     Changed to MMSEASE 7.0 Data Types		*/
/* 04/16/97  IKE    21     Call ipcPostEventSem after end of tx state   */
/*                         machine in txStateService                    */
/* 02/24/96  IKE    20	   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/* 02/03/97  IKE    19     adlcCfg per port basis			*/
/* 12/03/96  IKE    18     WIN32 Support, header files cleanup		*/ 
/*			   Changed rxService for link manager to keep  	*/
/*			   calling getframe until sm has no more frames */
/* 11/06/96  IKE    17     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/28/96  KCR    16     Converted to mulitple thread safe alloc funs */
/*			   and put cptr->state = ADLC_S_DISCONNECT to	*/
/*			   avoid two connect_node_done to reduced stack	*/
/* 10/28/96  IKE    15     Replaced cptr by dptr in stopWaitingForResp 	*/
/* 10/04/96  IKE    14     Replaced the glb var name from pstPollGroups	*/
/*			   to pollGroupList to make it consistent with	*/
/*			   other vars, Modified pollingService for DOS	*/	
/*			   Added pollInit				*/
/* 09/27/96  IKE    13     Replaced set_rts by set_signals and clear_rts*/
/*			   by clear_rts_set_dtr and enabled port 	*/
/*			   turnaround time				*/
/* 09/17/96  IKE    12     Fixed some bugs in txStateService and 	*/
/*			   stopWaitingForResp				*/
/* 			   Added automatic switching algorithm between  */
/*			   polled mode and contention mode		*/
/* 08/22/96  IKE    11     Support for contention avoidance		*/
/* 08/16/96  IKE    10     Changed the name from _ui_frame to mult_frame*/
/* 			   added turnaroundTime in stopWaitingForResp	*/
/* 07/17/96  IKE    09     Removed some lint and C6.00 warnings 	*/
/* 06/21/96  IKE    08     Changed txService for ADLC Master and put 	*/
/*			   SNRM pacing time as desribed in the design	*/
/* 			   document where it would allow RR and INFO 	*/
/*			   between SNRMS. Put #if defined(ADLC_MASTER)	*/
/*			   around some time related functions		*/
/* 06/18/96  IKE    07     Changed time related macros calls to funcs	*/
/* 06/11/96  IKE    06     Added CNP related code. Chaned the way the   */
/*			   M frames were transmitted for ADLC Slave	*/
/* 05/22/96  IKE    05     Header file cleanup. Enhanced Logging	*/
/*			   Put some more comments			*/
/*			   Changed the priorities of RR and INFO frames */
/*			   Got rid of gAdlcEventCb, verifyAdlcFrame	*/
/*			   Added resetRxBuf, actionForTxUFrame. Modified*/
/*			   txSFrame and txIFrame to fix some bugs. Put	*/
/*			   giveIndicationToStack at some additional 	*/
/*			   places					*/
/* 04/05/96  IKE    04     Change Logging from device osi terminology	*/
/* 			   to local remote terminology			*/
/* 04/05/96  rkr    02     Header file and SAVE_CALLING cleanup 	*/
/* 04/04/96  rkr    01     Cleanup					*/
/* 01/11/96  IKE   1.00    Initial release                             	*/
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

#if defined(CNP)
#include "cnp_log.h"
#include "cnp_usr.h"
#include "cnp.h"
#endif

#define SNRM_QUE	1
#define UI_QUE		2
#define MULT_QUE	3
#define INFO_QUE	4
#define RR_QUE		5
#define REDR_QUE	6
#define MAX_NO_OF_QUE	6

/************************************************************************/
/*      Static Function Declarations                           		*/
/************************************************************************/
static ST_RET txUFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_RET txSFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_RET txRFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_RET txMFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_RET txIFrames(ADLC_PORT_INFO *pptr,ADLC_CON_INFO *theCptr,
			ST_BOOLEAN justCheck);
static ST_RET txUIFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_RET txService2(ADLC_PORT_INFO *pptr);
static ST_VOID resetRxBuf(ADLC_PORT_INFO *pptr);
static ST_VOID actionForTxUFrame(ADLC_FRAME *fptr);
static ST_VOID pollingService(ST_VOID);
static ST_VOID serial_tx_error(ADLC_PORT_INFO *pptr);
static ST_VOID checkModeSwitchingAlgorithm(ADLC_PORT_INFO *pptr);
static ST_VOID changeToContentionMode (ADLC_PORT_INFO *pptr);
static ST_VOID changeToPolledMode (ADLC_PORT_INFO *pptr);
static ST_VOID changeStateToWaitForCtsLow(ADLC_PORT_INFO *pptr);
static ST_VOID updateStatistics(ST_RET ret, ST_ULONG *frame1, ST_ULONG *frame2);
static ST_VOID fillPriorityArray(ADLC_PORT_INFO *pptr);
#if defined(ADLC_LM)
static ST_RET handleTxStateService(ST_ULONG userID, ST_ULONG postCount);
#endif
#if !defined(ADLC_LM)
/************************************************************************/

/************************************************************************/
ST_VOID adlcService(ST_VOID) 
  {
ADLC_PORT_INFO *pptr;

  pollingService();
  pptr = portList;		/* get a port pointer			*/
  while (pptr)
    {
    if (pptr->runMode == CONTENTION_MODE)
      txStateService(pptr);
    else
      txService(pptr);
    rxService(pptr);
    timeoutService(pptr);
    pptr = (ADLC_PORT_INFO *) list_get_next(portList,pptr);
    }
#if defined(CNP)
  cnpService();
#endif
  }
/************************************************************************/
/*			       pollingService				*/
/************************************************************************/
static ST_VOID pollingService(ST_VOID)
  {
POLL_GROUP	*pstPollGrp;
POLLED_DEV	*pstPollDev;

  pstPollGrp = pollGroupList;
  while (pstPollGrp)
    {
    if (checkPollTimer(pstPollGrp))
      {
      updatePollTimer(pstPollGrp);	
      pstPollDev = pstPollGrp->pollDevList;
      while (pstPollDev)
        {
        if (!pstPollDev->bSuspendPolling && pstPollDev->dptr->pptr->runMode != CONTENTION_MODE)
          pollDevice(pstPollDev->dptr);
        pstPollDev = (POLLED_DEV *) list_get_next(pstPollGrp->pollDevList, pstPollDev);
        }
      }	
    pstPollGrp = (POLL_GROUP *) list_get_next(pollGroupList,pstPollGrp);
    }	
  }
#endif /* ADLC_LM */

/************************************************************************/
/*	                   txStateService				*/
/* Sends one frame at a time						*/
/* only for contention avoidance					*/
/************************************************************************/
ST_VOID txStateService(ADLC_PORT_INFO *pptr)
  {
#if defined(ADLC_LM)
ST_INT retCode;
SM_REQ stReq;

  ADLC_LOG_DEBUG2("txStateService for COM%d Signal State = %s",GET_COM_NO(pptr->port),
  		  getSignalState(pptr->signalState));
#endif /* ADLC_LM */

  switch (pptr->signalState)
    {	
    case MA_TX_STATE_IDLE :		/* nothing to transmit		*/
      if (checkTxService(pptr))
	{
	if (sm_set_signals(pptr->port) != SD_SUCCESS)
	  {
	  ADLC_LOG_ERR1("ADLC ERROR Could not set RTS high on COM%d",GET_COM_NO(pptr->port));
	  break;
	  }
        startSignalTimeout(pptr);
	pptr->signalState = MA_TX_STATE_WAIT_FOR_CTS_HIGH_DCD_LOW;
	ADLC_LOG_DEBUG2("Asserting RTS, Waiting for CTS for COM%d signalState = %s",
		       GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
#if defined(ADLC_LM)
        if ((retCode = ipcAddCommonTimer((ST_ULONG)pptr,
                          handleTxStateService,
			  &pptr->hTxStateTimer,
	    	     	  11L,
			  IPC_TIMER_REPEATED)) != SD_SUCCESS)
          logIpcError(retCode,"Unable to add timer event",thisFileName,__LINE__);
#endif /* ADLC_LM */
	}
    break;

    case MA_TX_STATE_WAIT_FOR_CTS_HIGH_DCD_LOW :	
#if defined(ADLC_LM)
      sm_get_status(pptr->port,&stReq);
      if (stReq.o.getStatReq.dcd == LOW && stReq.o.getStatReq.cts == HIGH)
#endif /* ADLC_LM */
#if !defined(ADLC_LM)
      if (sm_get_dcd(pptr->port) == LOW && sm_get_cts(pptr->port) == HIGH)
#endif
	{
	stopSignalTimeout(pptr);
	pptr->signalState = MA_TX_STATE_OK_TO_TX;
	ADLC_LOG_DEBUG2("Got DCD low, CTS high for COM%d signalState = %s",
		       GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
#if defined(ADLC_LM)
        ipcSleep(5,TIME_UNIT_MSEC); /* give sm time to get the frame 	*/
#endif        
	}
      else if (checkSignalTimeout(pptr))
	{
	serial_tx_error(pptr);
	}
    break;	

    case MA_TX_STATE_OK_TO_TX :
      if (checkTxService(pptr))
 	txService(pptr);
      else
        {
	if (isRespTimeoutEnabled(pptr))	/* PF given, will keep rts high */
	  {
#if defined(ADLC_LM)
          ipcDeleteCommonTimer (pptr->hTxStateTimer);
#endif
    	  pptr->signalState = MA_TX_STATE_IDLE;
	  }
	else		/* pf not given we have to lower rts so will go through whole cycle */
	  {
          pptr->signalState = MA_TX_STATE_WAIT_FOR_TX_BUF_EMPTY;
	  }
	}
    break;

    case MA_TX_STATE_WAIT_FOR_TX_BUF_EMPTY :
      if (sm_tx_cnt(pptr->port) == 0)	/* last byte is transmitted	*/
	{
  	if (ADLC_RADIO_DEKEY_DELAY(pptr)) 
	  { 	/* give radio some dkey before pulling rts low			*/
	  START_TIMEOUT(pptr->signalTimeout,ADLC_RADIO_DEKEY_DELAY(pptr));
	  pptr->signalState = MA_TX_STATE_DEKEY_DELAY;
	  }
	else
	  {
    	  changeStateToWaitForCtsLow(pptr);
	  }
	}
    break;	

    case MA_TX_STATE_DEKEY_DELAY :
      if (checkSignalTimeout(pptr))
	changeStateToWaitForCtsLow(pptr);
    break;
	
    case MA_TX_STATE_WAIT_FOR_CTS_LOW :
      if (sm_get_cts(pptr->port) == LOW)
	{
#if defined(ADLC_LM)
        ipcDeleteCommonTimer (pptr->hTxStateTimer);
#endif /* ADLC_LM */
  	stopSignalTimeout(pptr);
	pptr->signalState = MA_TX_STATE_IDLE;
	ADLC_LOG_DEBUG2("Got CTS low for COM%d signal State = %s",
   		        GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
	}
      else if (checkSignalTimeout(pptr))
        {
	serial_tx_error(pptr);
	}
#if defined(ADLC_LM)
      if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
        logIpcError(retCode,"FrameInList Semaphore Post Failed",thisFileName,__LINE__);
#endif    
    break;

    default :
      adlc_except(thisFileName,__LINE__);	 /* should never happen		*/
    break;		
    }
  }
/************************************************************************/

/************************************************************************/
static ST_VOID changeStateToWaitForCtsLow(ADLC_PORT_INFO *pptr)
  {
  sm_clear_rts_set_dtr(pptr->port);
  startSignalTimeout(pptr);
  pptr->signalState = MA_TX_STATE_WAIT_FOR_CTS_LOW;
  ADLC_LOG_DEBUG2("Clearing RTS, Waiting for CTS low for COM%d signalState = %s",
	       GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN checkTxService(ADLC_PORT_INFO *pptr)
  {
  if (txUFrames(pptr,SD_TRUE) == SD_SUCCESS)
    return(SD_TRUE);
  
  if (txSFrames(pptr,SD_TRUE) == SD_SUCCESS)
    return(SD_TRUE);
  
  if (txRFrames(pptr,SD_TRUE) == SD_SUCCESS)
    return(SD_TRUE);

  if (txMFrames(pptr,SD_TRUE) == SD_SUCCESS)
    return(SD_TRUE);

  if (txIFrames(pptr,NULL,SD_TRUE) == SD_SUCCESS)
    return(SD_TRUE);

  return (SD_FALSE);
  }
/************************************************************************/
/*	                         txService				*/
/* Sends one frame at a time						*/
/************************************************************************/
ST_VOID txService(ADLC_PORT_INFO *pptr)
  {
static ST_CHAR *QueNames[] =
  {
  "",
  "SNRM",
  "UI",
  "MULT",
  "INFO",
  "RR",
  "REDR",
  };
ST_CHAR buf[256];
ST_CHAR *pBuf;
ST_INT i;

#if defined(ADLC_LM)
  ADLC_LOG_FLOW1("txService called for COM%d",GET_COM_NO(pptr->port));
#endif

  pBuf = buf;
  if (txService2(pptr) == SD_SUCCESS) /* we did transmit a frame */
    {
    for (i=0; i < MAX_NO_OF_QUE; i++)
      pBuf += sprintf(pBuf,"%s ",QueNames[pptr->prQueArray[i]]);
    ADLC_LOG_CIO1("Priority order was %s",buf);	
    }
  }

/************************************************************************/
/*	                         txService2				*/
/* Sends one frame at a time						*/
/* This function will first determine the priority order of the queues	*/
/* and then try to send depending on the order				*/
/************************************************************************/
static ST_RET txService2(ADLC_PORT_INFO *pptr)
  {  
ST_INT i;
  
  /* fill the priority array */
  fillPriorityArray(pptr);

  for (i=0; i < MAX_NO_OF_QUE; i++)
    {
    switch (pptr->prQueArray[i])
      {
      case SNRM_QUE :
  	if (txUFrames(pptr,SD_FALSE) == SD_SUCCESS)
    	  {
    	  pptr->lastSnrmTime = sGetMsTime ();
    	  return SD_SUCCESS;
    	  }
      break;

#if defined(UCA_SMP)
      case UI_QUE :
        if (txUIFrames(pptr,SD_FALSE) == SD_SUCCESS)
          return SD_SUCCESS;
      break;
#endif /* UCA_SMP */

      case MULT_QUE :
        if (txMFrames(pptr,SD_FALSE) == SD_SUCCESS)
          return SD_SUCCESS;
      break;
      case INFO_QUE :
        if (txIFrames(pptr,NULL,SD_FALSE) == SD_SUCCESS)
          return SD_SUCCESS;
      break;
      case RR_QUE :
        if (txSFrames(pptr,SD_FALSE) == SD_SUCCESS)
          return SD_SUCCESS;
      break;
      case REDR_QUE :
        if (txRFrames(pptr,SD_FALSE) == SD_SUCCESS)
          return SD_SUCCESS;
      break;
      case 0 :
        return SD_FAILURE; /* did not transmit any frame */
      break;
      default :
        adlc_except(thisFileName,__LINE__);	 /* should never happen	*/
      break;
      }
    }

  return SD_FAILURE; /* did not transmit any frame */
  }

#if !defined(ADLC_LM)
/************************************************************************/
/*	                         rxService				*/
/************************************************************************/
ST_VOID rxService(ADLC_PORT_INFO *pptr)
  {
ST_INT ret_value;
#if defined(CNP)
ST_INT i; 
#endif
  ret_value = getFrame(pptr);

  if (ret_value == SD_SUCCESS) /* frame arrival		*/
    {
    ADLC_LOG_IO1 ("rxService: Frame received on COM%d",GET_COM_NO(pptr->port));

#if defined(CNP) /* put to all ied ports	*/
    CNP_LOG_CFLOW1("Radio (COM%d) to all IED ports transmission",GET_COM_NO(radio.port));
    for (i=0; i < ied.numOfPorts; i++)
      sm_put(ied.portInfo[i].port,pptr->rxLen,pptr->rxBuf);
#endif 

    ADLC_LOG_IO_HEX (pptr->rxLen,pptr->rxBuf);
    /* extract and process information		*/
    if (processAdlcFrame(pptr) == SD_FAILURE) 
      {
      ADLC_LOG_ERR0 ("Processing of Frame failed");
      ADLC_LOG_ERR_HEX(pptr->rxLen,pptr->rxBuf);
      }
    resetRxBuf(pptr);
    }
  else if (ret_value == E_ADLC_SERIAL_RX)
    {
    adlcStats.badFrames++;
    ADLC_LOG_CERR0("ADLC ERROR receiving frame");
    ADLC_LOG_ERR_HEX (pptr->rxLen,pptr->rxBuf);
    resetRxBuf(pptr);
    }
  }
#endif /* !ADLC_LM */

#if defined(ADLC_LM)
/************************************************************************/
/*	                         rxService				*/
/* For link manager we will keep requesting the frame unless there are	*/
/* no frames in serial manager						*/
/************************************************************************/
ST_VOID rxService(ADLC_PORT_INFO *pptr)
  {
ST_INT ret_value;
ST_INT done = SD_FALSE;

  while (done == SD_FALSE)
    {
    ret_value = getFrame(pptr);

    if (ret_value == SD_SUCCESS) /* frame arrival		*/
      {
      ADLC_LOG_IO1 ("rxService: Frame received on COM%d",GET_COM_NO(pptr->port));
      ADLC_LOG_IO_HEX (pptr->rxLen,pptr->rxBuf);
      ADLC_LOG_DEV_FILT1 (DEV_ADDR_GET(pptr->rxBuf),"rxService: Frame received on COM%d",GET_COM_NO(pptr->port));
      ADLC_LOG_DEV_FILT_HEX (DEV_ADDR_GET(pptr->rxBuf),pptr->rxLen,pptr->rxBuf);
      /* extract and process information		*/
      if (processAdlcFrame(pptr) == SD_FAILURE) 
        {
        ADLC_LOG_ERR0 ("Processing of Frame failed");
        ADLC_LOG_ERR_HEX(pptr->rxLen,pptr->rxBuf);
        }
      resetRxBuf(pptr);
      }
    else if (ret_value == E_ADLC_SERIAL_RX)	/* error receiving frame only for os2 */
      {					/* because we check 2nd crc only for os2 */
      adlcStats.badFrames++;
      ADLC_LOG_CERR0("ADLC ERROR receiving frame");
      ADLC_LOG_ERR_HEX (pptr->rxLen,pptr->rxBuf);
      resetRxBuf(pptr);
      }
    else if (ret_value == SD_FAILURE)	/* no more frames		*/
      {	
      done = SD_TRUE;	
      }	
    }
  }
#endif /* ADLC_LM */
/************************************************************************/
/*		                  txUFrames				*/
/* All the Unnumbered frames have poll final bit set so if the port has	*/
/* already one outstanding poll final bit set outstanding, just return	*/
/************************************************************************/
static ST_RET txUFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_CON_INFO *cptr;
ADLC_FRAME *fptr;
ST_INT ret = SD_FAILURE;

/* make sure that we have no outstanding connection and port turnaround	*/
/* time has elapsed (CTS of every device has gone low)			*/
  if (isRespTimeoutEnabled(pptr) || pptr->txRxMode == RX_MODE)
    return(ret);

  if (justCheck && pptr->txUFramesList)
    return (SD_SUCCESS);

  while (pptr->txUFramesList && ret == SD_FAILURE)
    {
    fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &pptr->txUFramesList);
    cptr = fptr->cptr;

    if (send_unum_frame(fptr) == SD_SUCCESS)
      {	
#if defined(ADLC_LM)
      cptr->dptr->devStats.txFrames ++;
#endif
      if (PF_GET(fptr->cmd))
        waitForResponse(cptr);
      ret = SD_SUCCESS;
      }
    else
      {	
      ADLC_LOG_ERR1("ADLC ERROR Serial Trasmit Error to COM%d",GET_COM_NO(cptr->pptr->port));
      ret = E_ADLC_SERIAL_TX;
      }			
    actionForTxUFrame(fptr);
    ipcChkFree(fptr);
    updateStatistics(ret, &adlcStats.txUFrames, NULL);
    }
  return (ret);
  }
/************************************************************************/
/*				txSFrames				*/
/* Because supervisory frames can have P/F bit set or not so we will 	*/
/* transmit the P/F  frame only if there is no other outstanding P/F	*/
/* frame is pending for the link (port). But we can transmit the frame	*/
/* without P/F bit							*/
/************************************************************************/
static ST_RET txSFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_FRAME *fptr;
ADLC_FRAME *nextFptr;
ADLC_CON_INFO *cptr;
ST_INT ret = SD_FAILURE;

/* make sure that we have no outstanding connection and port turnaround	*/
/* time has elapsed (CTS of every device has gone low)			*/
  if (isRespTimeoutEnabled(pptr) || pptr->txRxMode == RX_MODE)
    return(ret);

  fptr = pptr->txSFramesList;
  while (fptr && ret == SD_FAILURE)
    {
    nextFptr = (ADLC_FRAME *) list_get_next (pptr->txSFramesList,fptr);
    cptr = fptr->cptr;
    if (isOkToSendSupr(cptr,justCheck) == SD_FALSE)
      {				
      ADLC_LOG_FLOW2("Can not transmit S Frame for Connection 0x%x->0x%x",
		       cptr->masterAddr,cptr->dptr->address);
      ADLC_LOG_CFLOW2("Mode = %s state = %s",getTxRxMode(cptr->dptr->txRxMode),getConState(cptr->state));		
      fptr = nextFptr;	/* Go to the next request for another connection	*/
      continue;
      }

    if (justCheck)
      return (SD_SUCCESS);
/* use the substitution algorithm. So just before transmitting, check if we	*/
/* could send INFO for the same connection. If we could send INFO instead of	*/
/* RR frame and delete RR frame but increment RR counter			*/

    if (txIFrames(pptr,cptr,SD_TRUE) == SD_SUCCESS) /* yes we could send INFO */
      {
      ret = txIFrames(pptr,cptr,SD_FALSE); /* go ahead send it */
      }	
    else
      {
      if (send_supr_frame(fptr) == SD_SUCCESS)
        {
#if defined(ADLC_LM)
        cptr->dptr->devStats.txFrames ++;
#endif
        if (PF_GET(fptr->cmd))
	  waitForResponse(cptr);
        ret = SD_SUCCESS;
        }
      else
        {
        ADLC_LOG_ERR1("ADLC ERROR Serial Trasmit Error to COM%d",GET_COM_NO(cptr->pptr->port));
        ret = E_ADLC_SERIAL_TX;
        }	
      }
      
    deleteFrame(fptr,&pptr->txSFramesList);
    fptr = nextFptr;
    updateStatistics(ret, &adlcStats.txSFrames, &pptr->txRr);
    }
  return (ret);
  }

/************************************************************************/
/*		              	txRFrames				*/
/* Caution : No cptr can be retrieved					*/
/************************************************************************/
static ST_RET txRFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_FRAME *fptr;
ST_INT ret = SD_FAILURE;

  if (justCheck && pptr->txRFramesList)
    return (SD_SUCCESS);

  while (pptr->txRFramesList && ret == SD_FAILURE)
    {
    fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &pptr->txRFramesList);
    if (send_redr_frame(fptr,pptr) == SD_SUCCESS)
      ret = SD_SUCCESS;
    else
      {
      ADLC_LOG_ERR1("ADLC ERROR Serial Trasmit Error to COM%d",GET_COM_NO(pptr->port));
      ret = E_ADLC_SERIAL_TX;
      }
    ipcChkFree(fptr->u.redrBuf);	
    ipcChkFree(fptr);
    updateStatistics(ret, &adlcStats.txRFrames, NULL);
    }
  return (ret);
  }
/************************************************************************/
/*		                 txMFrames				*/
/* Caution : No cptr can be retrieved					*/
/************************************************************************/
static ST_RET txMFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_FRAME *fptr;
ST_INT ret = SD_FAILURE;
 
/* make sure that we have no outstanding connection and port turnaround	*/
/* time has elapsed (CTS of every device has gone low)			*/
  if (isRespTimeoutEnabled(pptr) || pptr->txRxMode == RX_MODE)
    return(ret);        

  if (isOkToSendMult(pptr,justCheck) == SD_FALSE)
    return(ret);

  if (justCheck && pptr->txMFramesList)
    return (SD_SUCCESS);

  while (pptr->txMFramesList && ret == SD_FAILURE)
    {
    fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &pptr->txMFramesList);
    if (send_mult_frame(fptr,pptr) == SD_SUCCESS)
      ret = SD_SUCCESS;
    else
      {
      ADLC_LOG_ERR1("ADLC ERROR Serial Trasmit Error to COM%d",GET_COM_NO(pptr->port));
      ret = E_ADLC_SERIAL_TX;
      }
    ipcChkFree(fptr->u.infoBuf);	
    ipcChkFree(fptr);
    updateStatistics(ret, &adlcStats.txMFrames, NULL);
    }
  return (ret);
  }

/************************************************************************/
/* 				 txIFrames				*/
/* This function will transmit one I frame at a time 			*/
/* This function goes through the entire list of I Frames of every 	*/
/* device and as soon as it finds one I frame ready to transmit it 	*/
/* transmits that frame and returns SD_SUCCESS				*/
/* For ADLC Master what we are trying to do in the master code is 	*/
/* enhanced polling							*/
/* so if the master is waiting for an ack from one device it could	*/
/* still send info to other devices without the p/f bit and later	*/
/* on it can send RR with P/F set once it gets the response from first	*/
/* connection								*/
/************************************************************************/
static ST_RET txIFrames(ADLC_PORT_INFO *pptr,ADLC_CON_INFO *theCptr,ST_BOOLEAN justCheck)
  {
ADLC_CON_INFO *cptr;
ADLC_REQ *rptr;
ADLC_REQ *nextRptr;
ADLC_FRAME *fptr;
ADLC_FRAME *nextFptr;
ST_BOOLEAN eosBit;
ST_BOOLEAN pfBit;
ST_BOOLEAN tmpPfBit;
ST_INT ret = SD_FAILURE;

  if (pptr->runMode != ENHANCED_POLLED_MODE)  /* contention or polled mode */
    { 
    /* make sure that we have no outstanding connection and port turnaround	*/
    /* time has elapsed (CTS of every device has gone low)			*/
    if (isRespTimeoutEnabled(pptr) || pptr->txRxMode == RX_MODE)
      return(ret);      
    }
  else	/* we waiting for port turnaroundtime to be elapsed */
    {
    if (isRespTimeoutDisabled(pptr) && pptr->txRxMode == RX_MODE)
      return(ret);      
    }

  rptr = pptr->txAdlcReqList;	/* Is there any write req ?		*/
  while (rptr && ret == SD_FAILURE)
    {
    nextRptr = (ADLC_REQ *) list_get_next (pptr->txAdlcReqList,rptr);
    cptr = rptr->cptr;		/* Get the device for that write req	*/	
    /* check if we are interested in frames for particular connection	*/
    if (theCptr && theCptr != cptr)
      {	
      rptr = nextRptr;	/* Go to the next request for another connection	*/
      continue;
      }
    if (isOkToSendInfo(cptr,justCheck) == SD_FALSE)
      {				
      ADLC_LOG_FLOW2("Can not transmit I Frame for Connection 0x%x->0x%x",
		       cptr->masterAddr,cptr->dptr->address);
      ADLC_LOG_CFLOW2("Mode = %s state = %s",getTxRxMode(cptr->dptr->txRxMode),getConState(cptr->state));		
      rptr = nextRptr;	/* Go to the next request for another connection	*/
      continue;
      }

    fptr = rptr->txIFramesList;	/* Is there any frame for that write req*/
    while (fptr && cptr->sendCount < (ST_UCHAR) (ADLC_MODULUS(pptr) - 1)  && ret == SD_FAILURE)
      {
      nextFptr = (ADLC_FRAME *) list_get_next (rptr->txIFramesList,fptr);
      if (fptr->status == STAT_STANDBY || fptr->status == STAT_NACKED)
        {
  	if (justCheck)
    	  return (SD_SUCCESS);

        cptr->sendCount ++;
        getPfAndEosBit(nextFptr,cptr->sendCount,&pfBit,&eosBit,pptr);	
	tmpPfBit = pfBit;
        if (pfBit && isRespTimeoutEnabled(pptr))
	  {
 	  tmpPfBit = SD_FALSE;		/* we will not send pf in info frame */
          if (isOkToPoll(cptr) == SD_TRUE)
            pollConnection(cptr);	
	  }
        if (send_info_frame(fptr,tmpPfBit,eosBit) == SD_SUCCESS)
	  {
#if defined(ADLC_LM)
          cptr->dptr->devStats.txFrames ++;
#endif
          fptr->status = STAT_PROGRESS;	
          cptr->vS = INC_MODULO_N(cptr->vS,1,ADLC_MODULUS(pptr));
/* ---------------------------------------------------------------------*	
 * check if we have sent seven outstanding reqs, and no other connection*
 * has outstanding reqs so make this connection as outstanding connection
 * ---------------------------------------------------------------------*/
	  if (pfBit == SD_TRUE && isRespTimeoutDisabled(pptr))
	    waitForResponse(cptr); 
	  ret = SD_SUCCESS;		/* we did send one frame	*/
	  }
	else
	  {
          ADLC_LOG_ERR1("ADLC ERROR Serial Trasmit Error to COM%d",GET_COM_NO(cptr->pptr->port));
	  ret = E_ADLC_SERIAL_TX;
	  }
        }
      fptr = nextFptr;
      }
    rptr = nextRptr;
    /* if this was sent because of substitution mechanism of RR	*/
    /* do not increment txInfo */
    if (theCptr) 
      updateStatistics(ret, &adlcStats.txIFrames, NULL);
    else	
      updateStatistics(ret, &adlcStats.txIFrames, &pptr->txInfo);
    }
  return (ret);
  }
/************************************************************************/

/************************************************************************/
static ST_VOID resetRxBuf(ADLC_PORT_INFO *pptr)
  {
  pptr->rxState = STATE_WAIT_SOF;
  pptr->rxLen = 0;
  memset(pptr->rxBuf,0x00,(ST_UINT) ADLC_FRAME_LEN(pptr));
#if !defined(ADLC_LM)
  stopFrameTimeout(pptr);
#endif
  }
/************************************************************************/
/*		               timeoutService				*/
/* check the timeout for every link (port)				*/
/************************************************************************/
ST_VOID timeoutService(ADLC_PORT_INFO *pptr)
  {
ADLC_DEV_INFO *dptr;
ADLC_CON_INFO *cptr;
static ST_BOOLEAN checkModeFlag = SD_FALSE;

  if (pptr->runMode == CONTENTION_MODE || checkModeFlag == SD_TRUE)
    {
    checkModeSwitchingAlgorithm(pptr);
    checkModeFlag = SD_TRUE;
    }

  if (checkRespTimeout(pptr))
    {	
    if (pptr->outstandingCptr)	
      timeout_error (pptr->outstandingCptr);
    }	
#if !defined(ADLC_LM)
    if (checkPortTurnaroundTimeout(pptr))
      {
      pptr->txRxMode = TX_MODE;
      stopPortTurnaroundTimeout(pptr);
      }
#endif		
  dptr = pptr->devList;
  while (dptr)
    {
#if !defined(ADLC_LM)
    if (checkDeviceTurnaroundTimeout(dptr))
      {
      dptr->txRxMode = TX_MODE;
      stopDeviceTurnaroundTimeout(dptr);
      }
#endif		
    cptr = dptr->conList;
    while (cptr)
      {
      if (checkConRetryTimeout(cptr))
        {
        /* send the connect request */
        addToUFramesList(cptr,SNRM+PF_BIT);
	stopConRetryTimeout(cptr);
	}
      cptr = (ADLC_CON_INFO *) list_get_next(dptr->conList,cptr);
      }
    dptr = (ADLC_DEV_INFO *) list_get_next(pptr->devList,dptr);
    }
  }
/************************************************************************/

/************************************************************************/
ST_RET pollInit(ST_VOID)
  {
ST_RET retCode = SD_SUCCESS;
POLL_GROUP	*pstPollGrp;

  pstPollGrp = pollGroupList;
  while (pstPollGrp && retCode == SD_SUCCESS)
    {
    retCode = startPollTimer(pstPollGrp);
    pstPollGrp = (POLL_GROUP *) list_get_next(pollGroupList, pstPollGrp);
    }
  return (retCode);
  }

/************************************************************************/
/*		                  timeout_error				*/
/************************************************************************/
ST_VOID timeout_error (ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_DEV_INFO *dptr = cptr->dptr;
#if defined(ADLC_LM)
ST_INT retCode;
#endif


  ADLC_LOG_ERR2 ("ADLC ERROR Connection 0x%x->0x%x Timeout",
			cptr->masterAddr,cptr->dptr->address);
  adlcStats.timeout ++;
#if defined(ADLC_LM)
  dptr->devStats.timeout ++;
#endif
  cptr->retries ++;			/* indicate a retry		*/
/*  triggerLogicAnalyzer(pptr->port);     				*/
  stopWaitingForResp(cptr,SD_FALSE); /* SD_FALSE => do not reset retries	*/
  if (cptr->retries < ADLC_RETRIES(pptr))
    {
    ADLC_LOG_ERR2 ("ADLC ERROR will retry to Connection 0x%x->0x%x",
		    cptr->masterAddr,cptr->dptr->address);
    switch(cptr->state)
      {
      case ADLC_S_LINK_SETUP :
        addToUFramesList(cptr,SNRM+PF_BIT);
        cptr->state = ADLC_S_DISCONNECT_PHASE;
      break;

      case ADLC_S_LINK_DISCONNECT :
        addToUFramesList(cptr,DISC+PF_BIT);
      break;

      case ADLC_S_SEND_REJECT :
	addToSFramesList(cptr,REJ+PF_BIT);
      break;


      case ADLC_S_NORMAL :
      case ADLC_S_REMOTE_BUSY :
/*        nackAllIFramesFromTxList(cptr);				*/
        addToSFramesList(cptr,RR+PF_BIT);
      break;

      default :
      break;
      }	
    }
  else  
    {
    switch(cptr->state)
      {
      case ADLC_S_LINK_SETUP :
        giveIndicationToStack(cptr,ADLC_CMD_CONNECT_NODE_DONE,E_ADLC_RESPONSE_TIMEOUT, 0L);
      break;

      case ADLC_S_LINK_DISCONNECT :
        giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE_DONE,SD_SUCCESS, 0L);
      break;

      case ADLC_S_REMOTE_BUSY :
      case ADLC_S_SEND_REJECT :
      case ADLC_S_NORMAL :
      case ADLC_S_DISCONNECT_PHASE :
        giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE,SD_SUCCESS, 0L);
      break;
	
      default :
      break;
      }	
    ADLC_LOG_ERR2 ("ADLC ERROR Resetting Connection 0x%x->0x%x",
		    cptr->masterAddr,cptr->dptr->address);
    resetConnection(cptr,ADLC_DELETE_CON);
#if defined(ADLC_LM)
    if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
      logIpcError(retCode,"FrameInList Semaphore Post Failed",thisFileName,__LINE__);
#endif    
    }
  }
/************************************************************************/
/*                              waitForResponse				*/
/* The equivalent function for slave is goInRxMode			*/
/* The counter part of this function is stopWaitingForResp		*/
/************************************************************************/
ST_VOID waitForResponse(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_DEV_INFO *dptr = cptr->dptr;

  startRespTimeout(pptr);
  pptr->outstandingCptr = cptr;	/* can not send any more	*/
  cptr->txRxMode = RX_MODE;
  dptr->txRxMode = RX_MODE;
  pptr->txRxMode = RX_MODE;
  ADLC_LOG_DEBUG4("Connection 0x%x->0x%x, Device (0x%x) and COM%d into RX_MODE",
		   getLocalAddr(cptr),getRemoteAddr(cptr),dptr->address,GET_COM_NO(pptr->port));
  }

/************************************************************************/
/*			     stopWaitingForResp				*/
/* we were waiting for the response and we got the response		*/
/* The equivalent function for slave is goInTxMode			*/
/* The counter part of this function is waitForResponse			*/
/************************************************************************/
ST_VOID stopWaitingForResp(ADLC_CON_INFO *cptr, ST_BOOLEAN bResetRetries)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_DEV_INFO *dptr = cptr->dptr;
ADLC_CON_INFO *outstandingCptr = pptr->outstandingCptr;

  /* is the responding connection same as the one we are expecting	*/
  if (outstandingCptr && outstandingCptr == cptr) 
    {
    stopRespTimeout(pptr);
    pptr->outstandingCptr = NULL;
    cptr->txRxMode = TX_MODE;
    /* we will lower rts only if in contention and were waiting for resp  
       and nothing else to transmit					*/

    if (pptr->runMode == CONTENTION_MODE && checkTxService(pptr) == SD_FALSE)
        sm_clear_rts_set_dtr(pptr->port);
    }

  /* is the responding device is same but connection is different than	*/
  /* what we are expecting? if yes then it is an error log it		*/

  if (outstandingCptr && outstandingCptr != cptr) 
    { 
    if (outstandingCptr->dptr == cptr->dptr)  
      ADLC_LOG_ERR5("ADLC ERROR Device(0x%x) non conformant\n" 
		    "            Responding Connection = 0x%x->0x%x\n"	
		    "            Outstanding Connection = 0x%x->0x%x",dptr->address,
		    getLocalAddr(cptr),getRemoteAddr(cptr),
		    getLocalAddr(outstandingCptr),getRemoteAddr(outstandingCptr));
 		
    }			
  if (bResetRetries == SD_TRUE) /* we got the response from the rxService */
    {
    cptr->retries = 0;
    startDeviceTurnaroundTimeout(dptr); /* will put dev into TX_MODE after a while */
    startPortTurnaroundTimeout(pptr);   /* will put port into TX_MODE after a while */
/*    pptr->txRxMode = TX_MODE;						*/
    }
  else	/* we timedout waiting for the response				*/
    {
    ADLC_LOG_DEBUG4("Connection 0x%x->0x%x, Device (0x%x) and COM%d into TX_MODE",
		   getLocalAddr(cptr),getRemoteAddr(cptr),dptr->address,GET_COM_NO(pptr->port));
    cptr->txRxMode = TX_MODE;
    dptr->txRxMode = TX_MODE;
    pptr->txRxMode = TX_MODE;
    }
  }

/************************************************************************/
/*			       pollDevice				*/
/* poll to all connections associated with this device			*/
/************************************************************************/
ST_VOID pollDevice(ADLC_DEV_INFO *dptr)
  {
ADLC_CON_INFO *cptr;

  ADLC_LOG_FLOW1("pollDevice: Polling Event received for Device '%s'", dptr->name);

  cptr = dptr->conList;
  while (cptr)
    {
    if (isOkToPoll(cptr) == SD_TRUE)
      {
      ADLC_LOG_FLOW2("Will poll Connection 0x%x->0x%x",
			  cptr->masterAddr,cptr->dptr->address);
      pollConnection(cptr);	
      }
    cptr = (ADLC_CON_INFO *) list_get_next(dptr->conList,cptr);
    }
  }
/************************************************************************/
/*		                pollConnection				*/
/************************************************************************/
ST_VOID pollConnection(ADLC_CON_INFO *cptr)
  {
  addToSFramesList(cptr,RR+PF_BIT);
  }

/************************************************************************/

/************************************************************************/
static ST_VOID actionForTxUFrame(ADLC_FRAME *fptr)
  {
ADLC_CON_INFO *cptr;

  cptr = fptr->cptr;
  switch (eventTypeOf(fptr->cmd))
    {
    case SNRM :
      cptr->state = ADLC_S_LINK_SETUP;
    break;
    case DISC :
      cptr->state = ADLC_S_LINK_DISCONNECT;
    break;
    default :
    break;
    }	
  }	
/************************************************************************/

/************************************************************************/
static ST_VOID serial_tx_error(ADLC_PORT_INFO *pptr)
  {
#if defined(DEBUG_SISCO)
ST_CHAR errBuf[100];
  switch (pptr->signalState)
    {
    case MA_TX_STATE_WAIT_FOR_CTS_HIGH :
      strcpy(errBuf,"Could not get CTS high");
    break;
    case MA_TX_STATE_WAIT_FOR_CTS_LOW :
      strcpy(errBuf,"Could not get CTS low");
    break;
    case MA_TX_STATE_WAIT_FOR_CTS_HIGH_DCD_LOW :	
      strcpy(errBuf,"Could not get CTS high or dcd low");
    break;
    default :
      strcpy(errBuf,"Unknown error");
    break;
    }
#endif /* DEBUG_SISCO */

#if defined(ADLC_LM)
  ipcDeleteCommonTimer (pptr->hTxStateTimer);
#endif /* ADLC_LM */

#if defined(__OS2__) 
  DosBeep(1000,500);
#endif

  ADLC_LOG_ERR2("ADLC ERROR %s on COM%d",errBuf,GET_COM_NO(pptr->port));
  stopSignalTimeout(pptr);
  sm_clear_rts_set_dtr(pptr->port);
  pptr->signalState = MA_TX_STATE_IDLE;
  }

#if defined(ADLC_LM)
/************************************************************************/

/************************************************************************/
static ST_RET handleTxStateService(ST_ULONG userID, ST_ULONG postCount)
  {
ADLC_PORT_INFO *pptr;
#if defined(__OS2__)
register ST_ULONG i;
#endif

  /* wait for Adlc Data to be available, then guard it */
  EnterCriticalSection(&protectAdlcData);

  pptr = (ADLC_PORT_INFO *) userID;
  ADLC_LOG_FLOW1("handleTxStateService for COM%d",GET_COM_NO(pptr->port));
#if defined(__OS2__)
  for (i = 0; i < postCount; i++)
#endif
    txStateService(pptr);

  /* unlock the adlc global data for other thread like main thread 	*/
  LeaveCriticalSection(&protectAdlcData);

  return (SD_SUCCESS);
  }
#endif /* ADLC_LM */
/************************************************************************/

/************************************************************************/
static ST_VOID checkModeSwitchingAlgorithm(ADLC_PORT_INFO *pptr)
  {
static ST_INT dcdCounter = 0;
static ST_INT samplingCounter = 0;
ST_INT cfgDcdCounter;

  if (pptr->rxRr > 3000)
    {
    pptr->rxRr = 0;
    pptr->rxInfo = 0;
    }

  if (pptr->runMode == CONTENTION_MODE)
    {
    samplingCounter ++;
    if (sm_get_dcd(pptr->port) == HIGH)
      dcdCounter ++;
  
    if (samplingCounter > 20)	/* 10 secs have elapsed */
      {
      cfgDcdCounter = (ADLC_PORT_USAGE(pptr) * 20)/100;	
      if (dcdCounter > cfgDcdCounter)	/* dcd was high most of the time */
        {
        changeToPolledMode(pptr);
        pptr->rxRr = 0;
        pptr->rxInfo = 0;
	}
      dcdCounter = 0;
      samplingCounter = 0;
      }
    }
  else		/* in polled mode		*/
    {
    if (pptr->rxRr > 20 && pptr->rxRr > 2 * pptr->rxInfo)	/* we are getting more rr than info */
      {
      changeToContentionMode(pptr);
      dcdCounter = 0;
      samplingCounter = 0;
      }
    }
  }
/************************************************************************/

/************************************************************************/
static ST_VOID changeToContentionMode (ADLC_PORT_INFO *pptr)
  {
ST_INT port = pptr->port;

  ADLC_LOG_ALWAYS1("Changing to Contention Mode for COM%d",GET_COM_NO(port));
  pptr->runMode = CONTENTION_MODE;
  sm_clear_rts_set_dtr(port);
  }
/************************************************************************/

/************************************************************************/
static ST_VOID changeToPolledMode (ADLC_PORT_INFO *pptr)
  {
ST_INT port = pptr->port;

  ADLC_LOG_ALWAYS1("Changing to Enhanced poll Mode for COM%d",GET_COM_NO(port));
  pptr->runMode = ENHANCED_POLLED_MODE;
  sm_set_signals(port);
  }

/************************************************************************/

/************************************************************************/
ST_VOID triggerLogicAnalyzer(ST_INT port)
  {
#if defined(__OS2__)  
SM_REQ stReq;

  DosBeep(100L,250L);
  sm_set_status(port,&stReq);
  stReq.i.setStatReq.rts = sm_get_rts(port);
  stReq.i.setStatReq.dtr = SD_FALSE;
  _SmRequest(&stReq);
  ipcSleep(20,TIME_UNIT_MSEC);
  stReq.i.setStatReq.dtr = SD_TRUE;
  _SmRequest(&stReq);
/*  DosSleep(10000L);					*/
/*  ASSERT(0);						*/
#endif /* __OS2__ */  

#if !defined(ADLC_LM)
  sm_clear_dtr(port);
  time_delay(5.0);
  sm_set_dtr(port);
#endif /* !ADLC_LM */
  }

#if defined(UCA_SMP)
/************************************************************************/
/* 				 txUIFrames				*/
/* This function will transmit one UI frame at a time 			*/
/* This function goes through the entire list of UI Frames of every 	*/
/* device and as soon as it finds one UI frame ready to transmit it 	*/
/* transmits that frame and returns SD_SUCCESS				*/
/************************************************************************/
static ST_RET txUIFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_CON_INFO *cptr;
ADLC_REQ *rptr;
ADLC_REQ *nextRptr;
ADLC_FRAME *fptr;
ADLC_FRAME *nextFptr;
ST_RET ret = SD_FAILURE;
ST_CHAR recTime[SMP_SIZE_OF_REC_TIME] = {0}; /* buffer to hold time stamp */

/* make sure that we have no outstanding connection and port turnaround	*/
/* time has elapsed (CTS of every device has gone low)			*/
  if (isRespTimeoutEnabled(pptr) || pptr->txRxMode == RX_MODE)
    return(ret);        

  rptr = pptr->txExtAdlcReqList;	/* Is there any write req ?		*/
  while (rptr && ret == SD_FAILURE)
    {
    nextRptr = (ADLC_REQ *) list_get_next (pptr->txExtAdlcReqList,rptr);
    cptr = rptr->cptr;		/* Get the device for that write req	*/	

    if (isOkToSendInfo(cptr,justCheck) == SD_FALSE)
      {				
      ADLC_LOG_FLOW2("Can not transmit UI Frame for Connection 0x%x->0x%x",
		       cptr->masterAddr,cptr->dptr->address);
      ADLC_LOG_CFLOW2("Mode = %s state = %s",getTxRxMode(cptr->dptr->txRxMode),getConState(cptr->state));		
      rptr = nextRptr;	/* Go to the next request for another connection	*/
      continue;
      }

    fptr = rptr->txUIFramesList;	/* Is there any frame for that write req*/
    while (fptr && ret == SD_FAILURE)
      {
      nextFptr = (ADLC_FRAME *) list_get_next (rptr->txUIFramesList,fptr);
      if (fptr->status == STAT_STANDBY || fptr->status == STAT_NACKED)
        {
  	if (justCheck)
    	  return (SD_SUCCESS);

        ret = send_ui_frame(fptr);
	if (ret == SD_SUCCESS)
	  {
#if defined(ADLC_LM)
          cptr->dptr->devStats.txFrames ++;
#endif
	  u_smp_record_time((ST_UCHAR *) recTime,sizeof(recTime));
          if (PF_GET(fptr->cmd))
            waitForResponse(cptr);
	  }
	else
	  {
          ADLC_LOG_ERR1("ADLC ERROR Serial Trasmit Error to COM%d",GET_COM_NO(cptr->pptr->port));
	  ret = E_ADLC_SERIAL_TX;
	  }
        ipcChkFree(fptr->u.infoBuf);	
        deleteFrame(fptr,&rptr->txUIFramesList);
	if (rptr->txUIFramesList == NULL)
	  {
          sendExtWriteDoneToStack(cptr,rptr->userId,recTime,sizeof(recTime),ret);
          deleteExtReqFrame(rptr);	/* all done with this rptr			*/
	  }
        }
      fptr = nextFptr;
      }
    rptr = nextRptr;
    updateStatistics(ret, &adlcStats.txUiFrames, NULL);
    }
  return (ret);
  }
#endif /* UCA_SMP */
/************************************************************************/

/************************************************************************/
static ST_VOID updateStatistics(ST_RET ret, ST_ULONG *frame1, ST_ULONG *frame2)
  {
  if (ret == SD_SUCCESS)
    {
    adlcStats.txFrames ++;
    if (frame1)
      (*frame1) ++;	
    if (frame2)
      (*frame2) ++;	
    }
  }

/************************************************************************/
/*			fillPriorityArray				*/
/* if it is a master it gives SNRM highest priority when some time has	*/
/* been elapsed between last SNRM and current time, other wise it gives	*/
/* SNRM the least priority over INFO and RR frames. So this way many 	*/
/* queued SNRMs could not block INFO and RR frames			*/
/* ADLC_LOG_IO2("currentMs = %.3f sec lastSnrmTime = %.3f sec",	      	*/
/* currentMs/1000,lastSnrmTime/1000);					*/
/* ADLC_LOG_CIO2("delta = (currentMs - lastSnrmTime) = %.3f sec > 	*/
/* snrmTimeout = %.3f sec try to send SNRM",delta/1000,snrmTimeout/1000)*/
/************************************************************************/
static ST_VOID fillPriorityArray(ADLC_PORT_INFO *pptr)
  {
ST_DOUBLE currentMs = sGetMsTime ();
ST_DOUBLE delta;
ST_DOUBLE snrmTimeout = ADLC_RESPONSE_TIMEOUT(pptr) + 2.0 * 1000;
ST_INT i = 0;

  /*---------------------------------------------------------------------
   * if sometime has elapsed between last snrm then snrm have highest priority 
   *--------------------------------------------------------------------*/
  delta = currentMs - pptr->lastSnrmTime;
  if (delta > snrmTimeout) /* then snrm have highest priority */
    pptr->prQueArray[i++] = SNRM_QUE;	

#if defined(UCA_SMP)
  pptr->prQueArray[i++] = UI_QUE;	
#endif /* UCA_SMP */

  pptr->prQueArray[i++] = MULT_QUE;	

  if (pptr->bTxRrFlag)	/* are we set to send RR ? */
    {
    if (pptr->txRr >= gRrFrames)  /* yes so see if we have crossed the limit */
      {
      pptr->bTxRrFlag = SD_FALSE; /* we sent max RR so send INFO */	
      pptr->txInfo = 0;		  /* reset counters when we change the flag */
      pptr->txRr = 0;
      }
    }
  else			/* so we are set to send INFO */
    {
    if (pptr->txInfo >= gInfoFrames)  /* see if we have crossed the limit */
      {
      pptr->bTxRrFlag = SD_TRUE; /* we have sent max INFO so send RR */	
      pptr->txInfo = 0;		/* reset counters when we change the flag */
      pptr->txRr = 0;
      }	
    }

  if (pptr->bTxRrFlag)
    {
    pptr->prQueArray[i++] = RR_QUE;	
    pptr->prQueArray[i++] = INFO_QUE;	
    }
  else
    {
    pptr->prQueArray[i++] = INFO_QUE;	
    pptr->prQueArray[i++] = RR_QUE;	
    }

  pptr->prQueArray[i++] = REDR_QUE;	

  if (pptr->prQueArray[0] != SNRM_QUE)	
    pptr->prQueArray[i++] = SNRM_QUE;	

  pptr->prQueArray[i++] = 0;  /* to tell caller not to go beyond this value */	
  }
#else
/* Intel compiler needs something to compile: */
static void dummy(void)
{
}
#endif /* ADLC_MASTER */

