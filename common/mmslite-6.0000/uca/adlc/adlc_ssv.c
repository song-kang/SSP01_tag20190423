#if defined(ADLC_SLAVE)
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_ssv.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* 	This module contains functions for transmitting and receiving 	*/
/* data for the slave only						*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    30     Cleaned up general PClint errors             */
/* 09/23/02  KCR    29     Removed extra include files                  */
/* 09/23/02  KCR    28     Moved thisFileName before includes           */
/* 09/13/99  MDE    27     Added SD_CONST modifiers         		*/
/* 09/30/98  IKE    26     Updated for CNP				*/
/* 08/13/98  JRB    25     Lint cleanup.				*/
/* 02/10/98  IKE    24     Changed to ipcPostCommonEvent to use new IPC */
/*			   which can handle more than 64 events		*/
/* 10/07/97  IKE    23     Added support for UI frames for UCA_SMP	*/
/* 07/16/97  IKE    22     Added device statistics and device filt log	*/
/* 06/12/97  IKE    21     Made ADLC protocol transaction oriented	*/
/* 05/12/97  IKE    20     Changed to MMSEASE 7.0 Data Types		*/
/* 02/24/96  IKE    19	   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/* 02/03/97  IKE    18     adlcCfg per port basis			*/
/* 12/03/96  IKE    17     WIN32 Support, header files cleanup		*/ 
/*			   Changed rxService for os2 and win32 to keep  */
/*			   calling getframe until sm has no more frames */
/* 11/06/96  IKE    16     Converted to mulitple thread safe alloc funs */
/* 10/04/96  IKE    15     Added check to see the communication media	*/
/*			   and if we are using cable bypass all the code*/
/*			   which deals with specific behaviour of radios*/
/* 09/27/96  IKE    14     Re arranged the txStateService to introduce  */
/*  			   delay before transmission of every frame	*/
/* 09/17/96  IKE    13	   Added startOfTxDelay, reset the rcv buffer	*/
/*			   when the radio finishes transmitting keeping */
/* 			   in mind that radio is half duplex		*/
/* 08/22/96  IKE    12     Support for contention avoidance		*/
/* 08/16/96  IKE    11     Changed the name from _ui_frame to mult_frame*/
/* 08/07/96  IKE    10     Fixed the bug in slave transmit state mach.	*/
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
#include "slog.h"
#include "mem_chk.h"
#include "memlog.h"
#include "stime.h"

/* ADLC files								*/
#include "adlc.h"

#if defined(CNP)
#include "cnp_log.h"
#include "cnp_usr.h"
#include "cnp.h"
#endif

#ifdef ADLC_TESTING
#include "fkeydefs.h"
#endif

/************************************************************************/
/*      Static Function Declarations                           		*/
/************************************************************************/
static ST_INT txUFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_INT txSFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_INT txMFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_INT txIFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_INT txUIFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
static ST_VOID resetRxBuf(ADLC_PORT_INFO *pptr);
static ST_VOID actionForTxUFrame(ADLC_FRAME *fptr);
static ST_VOID serial_tx_error(ADLC_PORT_INFO *pptr);

#if !defined(ADLC_LM)
static ST_INT isTxComplete(ADLC_PORT_INFO *pptr);
static ST_VOID actionBeforeTx(ADLC_PORT_INFO *pptr);
static ST_VOID actionAfterTx(ADLC_PORT_INFO *pptr);
static ST_VOID changeStateToWaitForCtsLow(ADLC_PORT_INFO *pptr);
/************************************************************************/

/************************************************************************/
ST_VOID adlcService(ST_VOID) 
  {
ADLC_PORT_INFO *pptr;

  pptr = portList;		/* get a port pointer			*/
  while (pptr)
    {
    if (ADLC_COMMUNICATION_MEDIA(pptr) == RADIO_MEDIA)
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
#endif /* ADLC_LM */

/************************************************************************/
/*	                   txStateService				*/
/* see the state diagram to understand this function			*/
/* this takes care of transmitting through the half duplex radio both	*/
/* in polled mode and in contention mode				*/
/* Note : Slave could only switch mode between poll or contention if it	*/
/* is configured for contention. If it is configured for poll, it can	*/
/* never switch modes							*/
/* Sends one frame at a time						*/
/************************************************************************/
ST_VOID txStateService(ADLC_PORT_INFO *pptr)
  {
#if defined(ADLC_LM)
  txService(pptr);
  }
#endif
#if !defined(ADLC_LM)
  switch (pptr->signalState)
    {
    case SL_TX_STATE_IDLE :		/* nothing to transmit			*/
      if (pptr->txRxMode == TX_MODE)	
	{	/* P/F received from the master			*/
        pptr->signalState = SL_TX_STATE_PF_RCVD;
	ADLC_LOG_DEBUG2("PF received slave will transmit for COM%d signalState = %s",
  		         GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
	}
      else if (ADLC_RUN_MODE(pptr) == CONTENTION_MODE)
	{
        if (checkTxService(pptr))
	  {
          pptr->runMode = CONTENTION_MODE;
          pptr->signalState = SL_TX_STATE_UNSOL_TX;
	  ADLC_LOG_DEBUG2("Slave has something unsolicited to transmit for COM%d signalState = %s",
  		         GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
	  }
	}
    break;

    case SL_TX_STATE_PF_RCVD :		
      sm_clear_dtr(pptr->port);
      sm_set_rts(pptr->port);
      startSignalTimeout(pptr);
      pptr->signalState = SL_TX_STATE_WAIT_FOR_CTS_HIGH;
      ADLC_LOG_DEBUG2("Asserting RTS, Clearing DTR, Waiting for CTS for COM%d signalState = %s",
		       GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
    break;

    case SL_TX_STATE_WAIT_FOR_CTS_HIGH :
      if (sm_get_cts(pptr->port) == HIGH)
        {
	actionBeforeTx(pptr);
	ADLC_LOG_DEBUG2("Got CTS high, Slave will transmit for COM%d signalState = %s",
		       GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
	}
      else if (checkSignalTimeout(pptr))
        {
	serial_tx_error(pptr);
	}
    break;

    case SL_TX_STATE_START_OF_TX_DELAY :
      if (checkSignalTimeout(pptr))
	{
	stopSignalTimeout(pptr);
        pptr->signalState = SL_TX_STATE_OK_TO_TX;
	}
    break;
	
    case SL_TX_STATE_OK_TO_TX :
      txService(pptr);		/* transmit only one frame */
      pptr->signalState = SL_TX_STATE_WAIT_FOR_TX_BUF_EMPTY;
    break;

    case SL_TX_STATE_WAIT_FOR_TX_BUF_EMPTY :
      if (sm_tx_cnt(pptr->port) == 0)	/* last byte is transmitted	*/
	{
	if (isTxComplete(pptr))
	  actionAfterTx(pptr);
        else	
	  actionBeforeTx(pptr);
	}
      break;

    case SL_TX_STATE_DEKEY_DELAY :
      if (checkSignalTimeout(pptr))
	changeStateToWaitForCtsLow(pptr);
    break;	

    case SL_TX_STATE_WAIT_FOR_CTS_LOW :
      if (sm_get_cts(pptr->port) == LOW)
	{
  	stopSignalTimeout(pptr);
	sm_set_dtr(pptr->port);
	resetRxBuf(pptr);
	pptr->signalState = SL_TX_STATE_IDLE;
	ADLC_LOG_DEBUG2("Got CTS low, asserting DTR for COM%d, signal State = %s",
   		        GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
	}
      else if (checkSignalTimeout(pptr))
        {
	serial_tx_error(pptr);
	}
    break;
    
    case SL_TX_STATE_UNSOL_TX :		
      sm_set_signals(pptr->port);
      pptr->signalState = SL_TX_STATE_WAIT_FOR_LINK;
      ADLC_LOG_DEBUG2("Asserting RTS and DTR, Waiting for CTS for COM%d signalState = %s",
		       GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
    break;

    case SL_TX_STATE_WAIT_FOR_LINK :
      if (sm_get_cts(pptr->port) == HIGH)
        {
	actionBeforeTx(pptr);
        ADLC_LOG_DEBUG2("Got Link, Slave will unsol transmit for COM%d signalState = %s",
		       GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
	}
      else if (pptr->txRxMode == TX_MODE)	
        {     /* P/F received from the master during waiting for link */
        pptr->signalState = SL_TX_STATE_PF_RCVD;
	}
      break;	

    default :
      adlc_except(thisFileName,__LINE__);	 /* should never happen		*/
    break;		
    }
  }	
/************************************************************************/

/************************************************************************/
static ST_INT isTxComplete(ADLC_PORT_INFO *pptr)
  {
ST_INT ret = SD_FALSE;

  switch (pptr->runMode)
    {
    case CONTENTION_MODE :
      if (checkTxService(pptr) == SD_FALSE) /* nothing else to transmit */
	ret = SD_TRUE;
    break;

    default :
      if (pptr->txRxMode == RX_MODE) /* see if p/f given during transmission	*/
	ret = SD_TRUE;
    break;
    }    
  return (ret);  
  }
/************************************************************************/

/************************************************************************/
static ST_VOID actionBeforeTx(ADLC_PORT_INFO *pptr)
  {
  if (ADLC_RADIO_START_OF_TX_DELAY(pptr))
    {
    START_TIMEOUT(pptr->signalTimeout,ADLC_RADIO_START_OF_TX_DELAY(pptr));
    pptr->signalState = SL_TX_STATE_START_OF_TX_DELAY;
    }
  else
    {
    stopSignalTimeout(pptr);
    pptr->signalState = SL_TX_STATE_OK_TO_TX;
    }
  }
/************************************************************************/

/************************************************************************/
static ST_VOID actionAfterTx(ADLC_PORT_INFO *pptr)
  {
  if (ADLC_RADIO_DEKEY_DELAY(pptr))
    {
    START_TIMEOUT(pptr->signalTimeout,ADLC_RADIO_DEKEY_DELAY(pptr));
    pptr->signalState = SL_TX_STATE_DEKEY_DELAY;
    }
  else
    {
    changeStateToWaitForCtsLow(pptr);
    }
  }
/************************************************************************/

/************************************************************************/
static ST_VOID changeStateToWaitForCtsLow(ADLC_PORT_INFO *pptr)
  {
  sm_clear_rts(pptr->port);
  startSignalTimeout(pptr);
  pptr->signalState = SL_TX_STATE_WAIT_FOR_CTS_LOW;
  ADLC_LOG_DEBUG2("Clearing RTS, Waiting for CTS to go low for COM%d signalState = %s",
       GET_COM_NO(pptr->port),getSignalState(pptr->signalState));
  }
#endif /* !ADLC_LM */
/************************************************************************/
/* check all the conditions which should be satisfied before 		*/
/* transmitting any info frames						*/
/************************************************************************/
ST_BOOLEAN checkTxService(ADLC_PORT_INFO *pptr)
  {
  if (txMFrames(pptr,SD_TRUE) == SD_SUCCESS)
    return(SD_TRUE);

  if (txIFrames(pptr,SD_TRUE) == SD_SUCCESS)
    return(SD_TRUE);

  return (SD_FALSE);
  }	
  
/************************************************************************/
/*	                         txService				*/
/* Sends one frame at a time						*/
/************************************************************************/
ST_VOID txService(ADLC_PORT_INFO *pptr)
  {
  if (txUFrames(pptr,SD_FALSE) == SD_SUCCESS)
    {
#if defined(ADLC_LM)
    pptr->bPortBusy = SD_TRUE;
#endif
    adlcStats.txFrames ++;
    adlcStats.txUFrames ++;	
    return;
    }
#if defined(UCA_SMP)
  /*---------------------------------------------------------------------
   * try to send UI Frame
   *--------------------------------------------------------------------*/
  if (txUIFrames(pptr,SD_FALSE) == SD_SUCCESS)
    {
#if defined(ADLC_LM)
    pptr->bPortBusy = SD_TRUE;
#endif
    adlcStats.txFrames ++;
    return;
    }
#endif /* UCA_SMP */
  /*---------------------------------------------------------------------
   * try to send Multicast Frame
   *--------------------------------------------------------------------*/
  if (txMFrames(pptr,SD_FALSE) == SD_SUCCESS)
    {
#if defined(ADLC_LM)
    pptr->bPortBusy = SD_TRUE;
#endif
    adlcStats.txFrames ++;
    adlcStats.txMFrames ++;
    return;
    }
  /*---------------------------------------------------------------------
   * try to send Info Frame
   *--------------------------------------------------------------------*/
  if (txIFrames(pptr,SD_FALSE) == SD_SUCCESS)
    {
#if defined(ADLC_LM)
    pptr->bPortBusy = SD_TRUE;
#endif
    adlcStats.txFrames ++;
    adlcStats.txIFrames ++;
    return;
    }
  /*---------------------------------------------------------------------
   * try to send Supervisory Frame
   *--------------------------------------------------------------------*/
  if (txSFrames(pptr,SD_FALSE) == SD_SUCCESS)
    {
#if defined(ADLC_LM)
    pptr->bPortBusy = SD_TRUE;
#endif
    adlcStats.txFrames ++;
    adlcStats.txSFrames ++;	
    return;
    }
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
/* For Link Manager we will keep requesting the frame unless there are	*/
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
      {
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
static ST_INT txUFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_FRAME *fptr;
ADLC_FRAME *nextFptr;
ADLC_CON_INFO *cptr;
ST_INT ret = SD_FAILURE;

  if (isRespTimeoutEnabled(pptr))
    return(ret);

  fptr = pptr->txUFramesList;
  while (fptr && ret == SD_FAILURE)
    {
    nextFptr = (ADLC_FRAME *) list_get_next (pptr->txUFramesList,fptr);
    cptr = fptr->cptr;
    if (isOkToSendUnum(cptr,justCheck) == SD_FALSE)
      {				
      ADLC_LOG_FLOW2("Can not transmit U Frame for Connection 0x%x->0x%x",
		       cptr->masterAddr,cptr->dptr->address);
      ADLC_LOG_CFLOW3("Device Mode = %s Connection Mode = %s state = %s",
		       getTxRxMode(cptr->dptr->txRxMode),getTxRxMode(cptr->txRxMode),
		       getConState(cptr->state));		
      fptr = nextFptr;	/* Go to the next request for another connection	*/
      continue;
      }

    if (justCheck)
      return (SD_SUCCESS);

    if (send_unum_frame(fptr) == SD_SUCCESS)
      {	
#if defined(ADLC_LM)
      cptr->dptr->devStats.txFrames ++;
#endif
      goInRxMode(cptr);	    /* slave will go in receive mode */	
      if (FRAME_TYPE_GET(fptr->u.unumBuf) == FRAME_FORMAT_TYPE4)
        waitForResponse(cptr);
      ret = SD_SUCCESS;
      }
    else
      {	
      ADLC_LOG_ERR1("ADLC ERROR Serial Trasmit Error to COM%d",GET_COM_NO(cptr->pptr->port));
      ret = E_ADLC_SERIAL_TX;
      }			
    actionForTxUFrame(fptr);
    fptr = nextFptr;
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
static ST_INT txSFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_FRAME *fptr;
ADLC_FRAME *nextFptr;
ADLC_CON_INFO *cptr;
ST_INT ret = SD_FAILURE;

  if (isRespTimeoutEnabled(pptr))
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
      ADLC_LOG_CFLOW3("Device Mode = %s Connection Mode = %s state = %s",
		       getTxRxMode(cptr->dptr->txRxMode),getTxRxMode(cptr->txRxMode),
		       getConState(cptr->state));		
      fptr = nextFptr;	/* Go to the next request for another connection	*/
      continue;
      }

    if (justCheck)
      return (SD_SUCCESS);

    if (send_supr_frame(fptr) == SD_SUCCESS)
      {
#if defined(ADLC_LM)
      cptr->dptr->devStats.txFrames ++;
#endif
      goInRxMode(cptr);	    /* slave will go in receive mode */	
      if (FRAME_TYPE_GET(fptr->u.suprBuf) == FRAME_FORMAT_TYPE4)
        waitForResponse(cptr);
      ret = SD_SUCCESS;
      }
    else
      {
      ADLC_LOG_ERR1("ADLC ERROR Serial Trasmit Error to COM%d",GET_COM_NO(cptr->pptr->port));
      ret = E_ADLC_SERIAL_TX;
      }	
    deleteFrame(fptr,&pptr->txSFramesList);
    fptr = nextFptr;
    }
  return (ret);
  }

/************************************************************************/
/*		                 txMFrames				*/
/* Caution : No cptr can be retrieved					*/
/************************************************************************/
static ST_INT txMFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_FRAME *fptr;
ST_INT ret = SD_FAILURE;
 
  if (isRespTimeoutEnabled(pptr))
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
static ST_INT txIFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_CON_INFO *cptr;
ADLC_REQ *rptr;
ADLC_REQ *nextRptr;
ADLC_FRAME *fptr;
ADLC_FRAME *nextFptr;
ST_BOOLEAN eosBit;
ST_BOOLEAN pfBit;
ST_INT ret = SD_FAILURE;

  if (isRespTimeoutEnabled(pptr))
    return(ret);      

  rptr = pptr->txAdlcReqList;	/* Is there any write req ?		*/
  while (rptr && ret == SD_FAILURE)
    {
    nextRptr = (ADLC_REQ *) list_get_next (pptr->txAdlcReqList,rptr);
    cptr = rptr->cptr;		/* Get the device for that write req	*/	

    if (isOkToSendInfo(cptr,justCheck) == SD_FALSE)
      {				
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
        if (send_info_frame(fptr,pfBit,eosBit) == SD_SUCCESS)
	  {
#if defined(ADLC_LM)
          cptr->dptr->devStats.txFrames ++;
#endif
          fptr->status = STAT_PROGRESS;	
          cptr->vS = INC_MODULO_N(cptr->vS,1,ADLC_MODULUS(pptr));
          if (pfBit)
	    {
            goInRxMode(cptr);	    	/* slave will go in receive mode */	
            if (FRAME_TYPE_GET(fptr->u.infoBuf) == FRAME_FORMAT_TYPE4)
       	      waitForResponse(cptr);
	    }	
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
    }
  return (ret);
  }
/************************************************************************/

/************************************************************************/
static ST_VOID resetRxBuf(ADLC_PORT_INFO *pptr)
  {
  pptr->rxState = STATE_WAIT_SOF;
  pptr->rxLen = 0;
  memset(pptr->rxBuf,0x00,ADLC_FRAME_LEN(pptr));
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
  if (checkRespTimeout(pptr))
    {	
    if (pptr->outstandingCptr)	
      timeout_error (pptr->outstandingCptr);
    }
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
  stopWaitingForResp(cptr,SD_FALSE);

  if (cptr->retries < ADLC_RETRIES(pptr))
    {
    ADLC_LOG_ERR2 ("ADLC ERROR will retry to Connection 0x%x->0x%x",
		    cptr->masterAddr,cptr->dptr->address);
    switch(cptr->state)
      {
      case ADLC_S_SEND_REJECT :
      case ADLC_S_NORMAL :
      case ADLC_S_REMOTE_BUSY :
	nackAllIFramesFromTxList(cptr);	/* will let vR and vS to synchronize the last acked frames */
      break;

      default :
      break;
      }	
    }
  else  
    {
    switch(cptr->state)
      {
      case ADLC_S_REMOTE_BUSY :
      case ADLC_S_SEND_REJECT :
      case ADLC_S_NORMAL :
        /* unlink and free all the buffers associated with this connection	*/
        ADLC_LOG_ERR2 ("ADLC ERROR Flushing buffers for Connection 0x%x->0x%x",
		    cptr->masterAddr,cptr->dptr->address);
	nackAllIFramesFromTxList(cptr);	/* will let vR and vS to synchronize the last acked frames */
        deleteAllFrames(cptr);
        cptr->infoBufLen = 0;
	cptr->retries = 0;
	/* not sure to either reset the whole vars. or just infoBufLen	*/
        /*resetStateVarsForConnection(cptr);				*/
      break;
	
      default :
      break;
      }	
#if defined(ADLC_LM)
    if (retCode = ipcPostCommonEvent(pptr->frameInListSem))
      logIpcError(retCode,"FrameInList Semaphore Post Failed",thisFileName,__LINE__);
#endif    
    }
  }
/************************************************************************/
/*			actionForTxUFrame				*/
/* This function can reset the connection associated with frame just	*/
/* transmitted. If it does not resets the connection it will delete the */
/* frame (unlink and free)						*/
/************************************************************************/
static ST_VOID actionForTxUFrame(ADLC_FRAME *fptr)
  {
ADLC_CON_INFO *cptr;
ADLC_PORT_INFO *pptr;

  cptr = fptr->cptr;
  pptr = cptr->pptr;
  switch (eventTypeOf(fptr->cmd))
    {
    case UA :
    case DM :
      switch(cptr->state)
        {
	case ADLC_S_LINK_DISCONNECT :
	case ADLC_S_DISCONNECT_PHASE :
          giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE,SD_SUCCESS, 0L);
          resetConnection(cptr,ADLC_DELETE_CON);
	break;	

	default :
          deleteFrame(fptr,&pptr->txUFramesList);
	break;
	}
    break;
    default :
      deleteFrame(fptr,&pptr->txUFramesList);
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
    case SL_TX_STATE_WAIT_FOR_CTS_HIGH :
      strcpy(errBuf,"Could not get CTS high");
    break;
    case SL_TX_STATE_WAIT_FOR_CTS_LOW :
      strcpy(errBuf,"Could not get CTS low");
    break;
    default :
      strcpy(errBuf,"Unknown error");
    break;
    }
#endif /* DEBUG_SISCO */
  ADLC_LOG_ERR2("ADLC ERROR %s on COM%d",errBuf,GET_COM_NO(pptr->port));
  stopSignalTimeout(pptr);
#if !defined(ADLC_LM)
  sm_set_dtr(pptr->port);
  sm_clear_rts(pptr->port);
#endif /* !ADLC_LM */
  pptr->signalState = SL_TX_STATE_IDLE;
  }
/************************************************************************/
/*			goInTxMode					*/
/* slave got the frame format 3 from master with p/f bit set		*/
/* so it will go in polled mode						*/
/************************************************************************/
ST_VOID goInTxMode(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_DEV_INFO *dptr = cptr->dptr;

  cptr->txRxMode = TX_MODE;	/* we now can transmit for this connection */
  dptr->txRxMode = TX_MODE;	/* we now can transmit for this slave */
  pptr->txRxMode = TX_MODE;	/* we now can transmit for this port  */
  pptr->runMode = POLLED_MODE;
  }
/************************************************************************/
/* slave has transmitted p/f bit					*/
/************************************************************************/
ST_VOID goInRxMode(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_DEV_INFO *dptr = cptr->dptr;

  cptr->txRxMode = RX_MODE;	/* we can't transmit for this connection */
  dptr->txRxMode = RX_MODE;	/* we can't transmit for this slave */
  pptr->txRxMode = RX_MODE;	/* we can't transmit for this port  */
  }
/************************************************************************/
/* if bResetRetries == SD_TRUE we will reset retries			*/
/* When this function is called from adlc_dec, bResetRetries will be 	*/
/* SD_TRUE because we actually got the response from the remote, if this	*/
/* will be called from timeout_error we will not reset retries		*/
/************************************************************************/
ST_VOID stopWaitingForResp(ADLC_CON_INFO *cptr, ST_BOOLEAN bResetRetries)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_DEV_INFO *dptr = cptr->dptr;

  stopRespTimeout(pptr);
  pptr->outstandingCptr = NULL;
  if (bResetRetries == SD_TRUE)
    cptr->retries = 0;
  }
/************************************************************************/

/************************************************************************/
ST_VOID waitForResponse(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_DEV_INFO *dptr = cptr->dptr;

  startRespTimeout(pptr);
  pptr->outstandingCptr = cptr;	/* can not send any more	*/
  }

#if defined(UCA_SMP)
/************************************************************************/
/* 				 txUIFrames				*/
/* This function will transmit one UI frame at a time 			*/
/* This function goes through the entire list of UI Frames of every 	*/
/* device and as soon as it finds one UI frame ready to transmit it 	*/
/* transmits that frame and returns SD_SUCCESS				*/
/************************************************************************/
static ST_INT txUIFrames(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
ADLC_CON_INFO *cptr;
ADLC_REQ *rptr;
ADLC_REQ *nextRptr;
ADLC_FRAME *fptr;
ADLC_FRAME *nextFptr;
ST_RET ret = SD_FAILURE;
ST_CHAR recTime[SMP_SIZE_OF_REC_TIME]; /* buffer to hold time stamp */

  if (isRespTimeoutEnabled(pptr))
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
          goInRxMode(cptr);	    /* slave will go in receive mode */	
          if (FRAME_TYPE_GET(fptr->u.infoBuf) == FRAME_FORMAT_TYPE4)
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
    }
  return (ret);
  }
#endif /* UCA_SMP */
#endif /* ADLC_SLAVE */



