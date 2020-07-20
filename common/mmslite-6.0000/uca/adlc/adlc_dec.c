/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_dec.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* It has function dealing with decoding the data coming on the port	*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    29     Cleaned up general PClint errors             */
/* 09/23/02  KCR    28     Removed extra include files                  */
/* 09/23/02  KCR    27     Moved thisFileName before includes           */
/* 09/13/99  MDE    26     Added SD_CONST modifiers         		*/
/* 09/29/98  IKE    25     Moved u_smp_record_time back in this module	*/
/*			   for non ADLC_LM applications			*/
/* 04/20/98  IKE    24     Moved u_smp_record_time in Serial Manager	*/
/*			   Added WRITE_PACKET_DONE Event		*/
/* 09/23/97  IKE    23     Added adlcExtWritePacket for define UCA_SMP	*/
/* 09/18/97  IKE    22     Added telephone support			*/
/* 07/16/97  IKE    21     Added device statistics and device filt log	*/
/* 06/12/97  IKE    20     Support for COGW				*/
/*			   Changed the stats to show discardedBytes	*/
/* 05/12/97  IKE    19     Changed to MMSEASE 7.0 Data Types		*/
/* 02/24/97  IKE    18	   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/* 02/03/97  IKE    17     adlcCfg per port basis			*/
/* 12/03/96  IKE    16     WIN32 Support, header files cleanup		*/ 
/*			   Removed the error logging in getFrame for os2*/
/* 11/06/96  IKE    15     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/28/96  KCR    14     Converted to mulitple thread safe alloc funs */
/* 10/04/96  IKE    13     Added check before sending to multicast gw	*/
/*			   that if it has issued openInterface ADLC	*/
/* 09/27/96  IKE    12     Added support for multicast gateway 		*/
/* 08/22/96  IKE    11     Support for contention avoidance		*/
/* 08/16/96  IKE    10     Changed Multicast Frames from UI to INFO so	*/
/*			   actionForInfoFrame and actionForMulticastFra	*/
/*			   follow the same flow.			*/
/* 07/17/96  IKE    09     Removed some lint and C6.00 warnings 	*/
/* 06/21/96  IKE    08     Changed some ADLC_LOG_IO to ADLC_LOG_USER	*/
/* 06/18/96  IKE    07     Changed time related macros calls to funcs	*/
/*			   Moved getTaskId in adlc_utl.c		*/
/* 06/11/96  IKE    06     Added frame timeout				*/
/* 05/22/96  IKE    05     Header file cleanup. Enhanced Logging	*/
/*			   Put some more comments			*/
/*			   Modified actionForInfoFrame and 		*/
/* 			   actionForSFrame to fix some bugs 		*/
/*			   Added giveDataToStack, giveIndicationToStack,*/
/* 			   getTaskId. Changed the algorithm of getFrame */
/*			   to check the crc when receiving		*/
/*			   frame. Got rid of verifyAdlcFrame. 		*/
/*			   Changed so for SNRM to already established 	*/
/*			   con. pass DISCONNECT	and then CONNECT. 	*/
/*			   Put giveIndicationToStack at some additional */
/*			   places. Changed the logic so after any S 	*/
/*			   Frame received con will go normal		*/
/* 04/05/96  IKE    04     Change Logging from device osi terminology	*/
/* 			   to local remote terminology			*/
/* 04/05/96  rkr    02     Header file and SAVE_CALLING cleanup 	*/
/* 04/04/96  rkr    01	   Cleanup					*/
/* 10/25/95  IKE   1.00    Initial release                              */
/************************************************************************/
static char *thisFileName = __FILE__;

/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

/* Some other SISCO files						*/
#include "cfg_util.h"
#include "memlog.h"

/* ADLC files								*/
#include "adlc.h"

#ifdef ADLC_TESTING
#include "fkeydefs.h"
#endif


/************************************************************************/
/*      Static Function Declarations                           		*/
/************************************************************************/
static ST_RET actionForInfoFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
static ST_RET actionForRrFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
static ST_RET actionForRnrFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
static ST_RET actionForRejFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
static ST_RET actionForUiFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
#if defined(ADLC_MASTER)
static ST_RET actionForUaFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
static ST_RET actionForRdFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
static ST_RET actionForDmFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
static ST_RET actionForFrmrFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
#endif
#if defined(ADLC_SLAVE)
static ST_RET actionForSnrmFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
static ST_RET actionForDiscFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte);
#endif
static ST_RET actionForMulticastFrame(ADLC_PORT_INFO *pptr,ST_UCHAR ctrlByte);
static ST_BOOLEAN isNrValid(ADLC_CON_INFO *cptr,ST_UCHAR nR);
static ST_BOOLEAN isNsValid(ST_UCHAR vR,ST_UCHAR nS);
static ST_VOID processIncomingData(ADLC_CON_INFO *cptr,ADLC_PORT_INFO *pptr,ST_UCHAR cmd);
static ST_VOID giveDataToStack (ADLC_CON_INFO *cptr,ADLC_PORT_INFO *pptr,ST_UCHAR cmd);
static ST_VOID copyDataIntoMemForStack(ST_CHAR *pData,ADLC_FRAME **rxFramesList);

static ST_VOID sendWritePacketToStack(ST_UINT16 localAddr,ST_UINT16 remoteAddr,
			   ST_INT dataSize,
			   ADLC_CON_INFO *cptr, ADLC_PORT_INFO *pptr);
#if defined(UCA_SMP)
static ST_VOID sendExtWritePacketToStack(ST_UINT16 localAddr,ST_UINT16 remoteAddr,
			   ST_INT dataSize,
			   ADLC_CON_INFO *cptr, ADLC_PORT_INFO *pptr);
#endif

/************************************************************************/
/* Valid Frame Format for supervisory and unnumbered is following	*/
/* SOF(1) TYPE+LEN(2) DST_ADDR(2) SRC_ADDR(2) CTRL(1) CRC(2) EOF(1)	*/
/* Valid Frame Format for Info frame is following			*/
/* SOF(1) TYPE+LEN(2) DST_ADDR(2) SRC_ADDR(2) CTRL(1) CRC(2) INFO(?)	*/
/* CRC(2) EOF(1)							*/
/************************************************************************/
ST_RET getFrame(ADLC_PORT_INFO *pptr)
  {
#if !defined(ADLC_LM)

ST_INT port = pptr->port;
ST_INT ret_type = SD_FAILURE;
ST_INT val;
ST_INT num_discarded, num_bytes_rxd;
ST_INT enc_len;
ST_UCHAR discarded_data[MAX_ADLC_FRAME_LEN];
ST_UCHAR *frame = pptr->rxBuf;
ST_UINT *len = &pptr->rxLen;

  if (checkFrameTimeout(pptr))
    {
    ADLC_LOG_ERR0("ADLC ERROR Frame Timeout"); 
    return (E_ADLC_SERIAL_RX);
    }

  num_discarded = num_bytes_rxd = 0;
  val = RX_BYTE(port);
  while ( (val != -1) && (ret_type == SD_FAILURE) &&
          (num_bytes_rxd ++ < ADLC_FRAME_LEN(pptr)) )
    {
    switch (pptr->rxState)
      {
      case STATE_WAIT_SOF :	/* to get first byte equal to SOF 		*/
	frame[*len] = (ST_UCHAR) val;
        if (frame[0] == ADLC_SOF(pptr))
	  {
	  startFrameTimeout(pptr);
	  pptr->rxState = STATE_WAIT_LEN;
	  *len +=1;
	  }
	else
	  discarded_data[num_discarded++] = frame[0];
      break;

      case STATE_WAIT_LEN :   	/* to get three bytes so we can extract length	*/
	frame[(*len)++] = (ST_UCHAR) val;
	if (*len >= DST_ADDR_FIELD_OFFSET)
	  {
	  enc_len = FRAME_LEN_GET(frame);
	  if (enc_len < MIN_ADLC_FRAME_LEN - 1 || enc_len > ADLC_FRAME_LEN(pptr))
	    {
	    ADLC_LOG_NERR1("ADLC ERROR Encoded Length  = %d in frame is invalid",enc_len);
	    ret_type = E_ADLC_SERIAL_RX;
	    }
	  else
	    pptr->rxState = STATE_WAIT_CRC;
	  }
      break;
	
      case STATE_WAIT_CRC :
	frame[(*len)++] = (ST_UCHAR) val;
	if (*len >= INFO_FIELD_OFFSET)
	  {
          if (verifyCrc(frame,*len) == SD_FAILURE) 
	    {
            ADLC_LOG_ERR0("ADLC ERROR CRC not valid");
	    ret_type = E_ADLC_SERIAL_RX;
	    }
	  else 
	    {
	    if ((ST_INT) *len == FRAME_LEN_GET(frame))
	      pptr->rxState = STATE_WAIT_EOF;
	    else 
	      pptr->rxState = STATE_WAIT_DATA;
	    }
	  }
      break;		

      case STATE_WAIT_DATA : 	/* get the frame until length encoded in frame	*/
	frame[(*len)++] = (ST_UCHAR) val;
	if ((ST_INT) *len >= FRAME_LEN_GET(frame))
	  {
          if (verifyCrc(frame,*len) == SD_FAILURE) 
	    {
            ADLC_LOG_ERR0("ADLC ERROR CRC not valid");
	    ret_type = E_ADLC_SERIAL_RX;
	    }
	  else 
	    {
	    pptr->rxState = STATE_WAIT_EOF;
	    }
	  }
      break;

      case STATE_WAIT_EOF :	/* get the last byte equal to EOF		*/
	frame[*len] = (ST_UCHAR) val;
        if (frame[*len] == ADLC_EOF(pptr))
	  {
          adlcStats.goodFrames++;
	  adlcStats.rxFrames++;
	  stopFrameTimeout(pptr);
	  ret_type = SD_SUCCESS;
	  }
	else
	  {
	  ADLC_LOG_NERR1("ADLC ERROR Last received byte of the frame = 0x%x is invalid",frame[*len]);
	  ret_type = E_ADLC_SERIAL_RX;
	  }
        *len += 1;
      break;

      default :
        adlc_except(thisFileName,__LINE__);	 /* should never happen		*/
      break;
      }  

    if (ret_type == SD_FAILURE)
      val = RX_BYTE(port);
    }
  if (num_discarded)
    {
    if (num_discarded > 1 || discarded_data[0] != 0xFF)
    /* To prevent logging of radio noise (ff char) */
      {
      adlcStats.discardedBytes += num_discarded;
      ADLC_LOG_NERR1 ("ADLC - Get Response Discard Data: on COM%d",GET_COM_NO(port));
      ADLC_LOG_NERR_HEX(num_discarded,discarded_data);
      }
    }
  return (ret_type);
#endif	/* !ADLC_LM */

#if defined(ADLC_LM)
SM_REQ stReq;
ST_INT ret_type = SD_SUCCESS;

/* Make the serial Manager request			*/
  stReq.opCode = SM_GET_FRAME_REQ;
  stReq.port = pptr->port;
  if (_SmRequest(&stReq) != SD_SUCCESS)
    {
    return(SD_FAILURE);
    }
  /* copy the data */
  memcpy(pptr->rxBuf,stReq.o.frameArrReq.data,stReq.o.frameArrReq.len);
#if defined(UCA_SMP)
  /* copy the time stamp */
  memcpy(pptr->recTime,stReq.o.frameArrReq.recTime,SMP_SIZE_OF_REC_TIME);
#endif
  pptr->rxLen = stReq.o.frameArrReq.len; 
  ipcChkFree(stReq.o.frameArrReq.data);	/* free the buffer allocated by SM	*/

#if defined(__OS2__)
/* We will check the 2nd CRC in INFO frame because OS/2 Serial Manager does not */
  if (eventTypeOf(CTRL_BYTE_GET(pptr->rxBuf)) == INFO)
    {  		/* In INFO Packet there are two CRC	*/
    if (verifyCrc(pptr->rxBuf,pptr->rxLen - EOF_FIELD_LEN) == SD_FAILURE) 
      {
      ADLC_LOG_ERR0("ADLC ERROR CRC not valid");
      ret_type = E_ADLC_SERIAL_RX;
      }
    }
#endif /* only if OS2 defined */

  return (ret_type);
#endif /* ADLC_LM */
  }
/************************************************************************/
/*			processAdlcFrame				*/
/* extract and process information					*/
/* extract type of frame, source and destination address		*/
/* This function is based upon a state matrix of state versus events	*/
/* states as rows, events as columns and matrix elements as actions	*/
/* First check the event then check the state of the connection and	*/
/* then take action							*/
/*  ---------> State							*/
/* |									*/
/* |  Actions								*/
/* |									*/
/* Events								*/
/************************************************************************/
ST_RET processAdlcFrame(ADLC_PORT_INFO *pptr)
  {
ADLC_DEV_INFO *dptr;
ADLC_CON_INFO *cptr;
ST_UCHAR frameType;
ST_UINT16 srcAddr;
ST_UINT16 dstAddr;
ST_UCHAR ctrlByte;
ST_RET ret = SD_SUCCESS;

#if defined(ADLC_SLAVE)
ST_BOOLEAN bNewCptr = SD_FALSE;
#endif

/* Extract some fields from the received frame	*/
  frameType = FRAME_TYPE_GET(pptr->rxBuf);
  srcAddr = SRC_ADDR_GET(pptr->rxBuf);
  dstAddr = DST_ADDR_GET(pptr->rxBuf);
  ctrlByte = CTRL_BYTE_GET(pptr->rxBuf);

  ADLC_LOG_CIO4("Received %s Frame from Remote = 0x%x for Local Addr = 0x%x P/F Bit = %x",
  		getCommandName(ctrlByte),srcAddr,dstAddr, PF_GET(ctrlByte) ? 1: 0);
  ADLC_LOG_CIO1("Frame Type = 0x%x",frameType);
#if defined(ADLC_LM)
  ADLC_LOG_CDEV_FILT4(DEV_ADDR_GET(pptr->rxBuf),"Received %s Frame from Remote = 0x%x for Local Addr = 0x%x P/F Bit = %x",
  		getCommandName(ctrlByte),srcAddr,dstAddr, PF_GET(ctrlByte) ? 1: 0);
  ADLC_LOG_CDEV_FILT1(DEV_ADDR_GET(pptr->rxBuf),"Frame Type = 0x%x",frameType);
#endif
  if (frameType != FRAME_FORMAT_TYPE3 && frameType != FRAME_FORMAT_TYPE4)
    {
    ADLC_LOG_ERR1("ADLC ERROR Could not match Frame Type 0x%x in received frame",
    		   frameType);
    return (SD_FAILURE);
    }

  if (IS_MULTICAST_ADDR(dstAddr))	/* the MSB is set	*/
    {
    ret = actionForMulticastFrame(pptr,ctrlByte);
    adlcStats.mCastFrames ++;
    return (ret);
    } 

#if defined(ADLC_MASTER)
  if ((dptr = getDevPtrFromAddress(srcAddr)) == NULL)
    {
    ADLC_LOG_ERR1("ADLC ERROR Could not match Remote Addr = 0x%x in the received frame",
    srcAddr);
    return (SD_FAILURE);
    }

#if defined(ADLC_LM)
  dptr->devStats.rxFrames ++;
#endif
  if (IS_REMOTE_ADDR(dstAddr))	/* High three bits are not set 	*/
    {				/* pass to redirector		*/
    addToRFramesList(pptr);
    return (SD_SUCCESS);
    }

/* if none of the above two cases then we should have a connection (cptr)	*/
  if ((cptr = getConPtrFromDptr(dptr,dstAddr)) == NULL)
    {
    ADLC_LOG_ERR2("ADLC ERROR No 0x%x->0x%x Connection",dstAddr,srcAddr);
    return (SD_FAILURE);
    }
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
  if ((dptr = getDevPtrFromAddress(dstAddr)) == NULL)	/* not for this slave */
    return (SD_SUCCESS);

#if defined(ADLC_LM)
  dptr->devStats.rxFrames ++;
#endif
  if ((cptr = getConPtrFromDptr(dptr,srcAddr)) == NULL)
    {
    if (eventTypeOf(ctrlByte) == SNRM) /* new connection request	*/
      {
      bNewCptr = SD_TRUE;		
      cptr = addToConList(dptr,srcAddr);
      }	
    else
      {	
      ADLC_LOG_ERR2("ADLC ERROR No 0x%x->0x%x Connection",srcAddr,dstAddr);
      return (SD_FAILURE);
      }	
    }

  stopWaitingForResp(cptr,SD_TRUE);
  if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE3)
    goInTxMode(cptr);

#endif /* ADLC_SLAVE */

  switch (eventTypeOf(ctrlByte))
    {
    case INFO :
      ret = actionForInfoFrame(cptr,ctrlByte);
      adlcStats.rxInfo ++;
#if defined(ADLC_MASTER)
      pptr->rxInfo ++;			/* used for switch mode algorithm */	
#endif	
    break;	
							/* S-Frame */
    case RR :
      ret = actionForRrFrame(cptr,ctrlByte);
      adlcStats.rxRr ++;
#if defined(ADLC_MASTER)
      pptr->rxRr ++;			/* used for switch mode algorithm */
#endif	
    break;

    case RNR :
      ret = actionForRnrFrame(cptr,ctrlByte);
      adlcStats.rxRnr ++;
    break;

    case REJ :
      ret = actionForRejFrame(cptr,ctrlByte);
      adlcStats.rxRej ++;
    break;

    case UI :
      ret = actionForUiFrame(cptr,ctrlByte);
      adlcStats.rxUi ++;
    break;	    

#if defined(ADLC_MASTER)
    case UA :
      ret = actionForUaFrame(cptr,ctrlByte);
      adlcStats.rxUa ++;
    break;	

    case RD :
      ret = actionForRdFrame(cptr,ctrlByte);
      adlcStats.rxRd ++;
    break;	

    case DM :
      ret = actionForDmFrame(cptr,ctrlByte);
      adlcStats.rxDm ++;
    break;	

    case FRMR :
      ret = actionForFrmrFrame(cptr,ctrlByte);
      adlcStats.rxFrmr ++;
    break;
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
    case SNRM :
      ret = actionForSnrmFrame(cptr,ctrlByte);
      if (ret == SD_SUCCESS)
	{
	if (bNewCptr)
	  giveIndicationToStack(cptr,ADLC_CMD_CONNECT_NODE,SD_SUCCESS, 0L);
	else
	  {
	  giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE,SD_SUCCESS, 0L);
	  giveIndicationToStack(cptr,ADLC_CMD_CONNECT_NODE,SD_SUCCESS, 0L);
	  }
	}
      adlcStats.rxSnrm ++;
    break;	

    case DISC :
      ret = actionForDiscFrame(cptr,ctrlByte);
      adlcStats.rxDisc ++;
    break;	
#endif /* ADLC_SLAVE */

    default :
      ADLC_LOG_ERR2 ("ADLC ERROR Unknown response for Connection 0x%x->0x%x",
		      cptr->masterAddr,cptr->dptr->address);
      ret = SD_FAILURE;	
    break;		
    }	 
  return (ret);
  }

/************************************************************************/
/*			    actionForMulticastFrame			*/
/************************************************************************/
static ST_RET actionForMulticastFrame(ADLC_PORT_INFO *pptr,ST_UCHAR ctrlByte)
  {
ST_UCHAR nR;
ST_UCHAR nS;
ST_INT infoBufLen;

/* initialize local variables						*/
  nR = INFO_NR_GET(ctrlByte);
  nS = INFO_NS_GET(ctrlByte);
  infoBufLen = FRAME_LEN_GET(pptr->rxBuf) + EOF_FIELD_LEN - MIN_INFO_FRAME_LEN;
  ADLC_LOG_CIO3("Ns = %d Nr = %d EOS Bit = %x",nS,nR,EOS_GET(pptr->rxBuf) ? 1 : 0);

#if defined(ADLC_MASTER)
/* First take care of timeout					*/
  updateRespTimeout(pptr);	
#endif

/* check for ns 	    */
  if (isNsValid(pptr->vR,nS) == SD_FALSE)
    {
    ADLC_LOG_ERR2("ADLC ERROR Received Ns = %d wrong Expected = %d",nS,pptr->vR);
    pptr->vR = 0;
    deleteMFramesFromRxList(pptr);
    return (SD_FAILURE);
    }	

  if (EOS_GET(pptr->rxBuf) == 0)	/* End of segment bit is zero	*/
    pptr->vR = 0;
  else
    pptr->vR = INC_MODULO_N(pptr->vR,1,ADLC_MODULUS(pptr));			

  if (infoBufLen > 0) 
    processIncomingData(NULL,pptr,INFO);

  return(SD_SUCCESS);
  }

/************************************************************************/
/*			     actionForInfoFrame				*/
/************************************************************************/
static ST_RET actionForInfoFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ST_RET ret = SD_SUCCESS;
ST_UCHAR nR;
ST_UCHAR nS;
ST_INT infoBufLen;
ST_UCHAR frameType;

  nR = INFO_NR_GET(ctrlByte);
  nS = INFO_NS_GET(ctrlByte);
  infoBufLen = FRAME_LEN_GET(pptr->rxBuf) + EOF_FIELD_LEN - MIN_INFO_FRAME_LEN;
  frameType = FRAME_TYPE_GET(pptr->rxBuf);
  ADLC_LOG_CIO3("Ns = %d Nr = %d EOS Bit = %x",nS,nR,EOS_GET(pptr->rxBuf) ? 1 : 0);
#if defined(ADLC_LM)
  ADLC_LOG_CDEV_FILT3(cptr->dptr->address,"Ns = %d Nr = %d EOS Bit = %x",nS,nR,EOS_GET(pptr->rxBuf) ? 1 : 0);
#endif
  switch (cptr->state)
    {
    case ADLC_S_DISCONNECT_PHASE :
      if (PF_GET(ctrlByte))
#if defined(ADLC_MASTER)
	{
	if (isUFrameQueuedUp(cptr) == SD_FALSE)
          addToUFramesList(cptr,DISC+PF_BIT);
	}
#endif
#if defined(ADLC_SLAVE)
 	{
	if (isUFrameQueuedUp(cptr) == SD_FALSE)
          addToUFramesList(cptr,DM+PF_BIT);
	}
#endif
    break;

    case ADLC_S_LINK_DISCONNECT :
    case ADLC_S_LINK_SETUP :
    case ADLC_S_RESET :
      ADLC_LOG_DEBUG3("Connection 0x%x->0x%x in invalid state (%s)  to receive INFO frame ignoring",
			cptr->masterAddr,cptr->dptr->address,getConState(cptr->state));
    break;

#if defined(ADLC_SLAVE)
    case ADLC_S_FRAME_REJECT : 	
/*      stopWaitingForResp(cptr);					*/
      if (PF_GET(ctrlByte))
        addToUFramesList(cptr,FRMR+PF_BIT);
    break;
#endif

    case ADLC_S_REMOTE_BUSY :
    case ADLC_S_SEND_REJECT :
    case ADLC_S_NORMAL :
#if defined(ADLC_MASTER)
/* First take care of timeout					*/
      if (PF_GET(ctrlByte))
	stopWaitingForResp(cptr,SD_TRUE);
      else
	updateRespTimeout(pptr);	
/* Check NR							*/
      if (isNrValid(cptr,nR) == SD_FALSE) /* Invalid Nr */
        {
	ADLC_LOG_ERR2("ADLC ERROR Received wrong Nr = %d Expected = %d will send DISC",nR,cptr->vS);
        giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE,SD_SUCCESS, 0L);
        addToUFramesList(cptr,DISC + PF_BIT);
	cptr->state = ADLC_S_LINK_DISCONNECT;
	break; /* No further processing	*/
	}
      deleteAckedIFramesFromTxList(cptr,nR); /* will synchronize our vS  */
/* Check Ns							*/
      if (isNsValid(cptr->vR,nS) == SD_FALSE)
	{
	ADLC_LOG_ERR2("ADLC ERROR Received Ns = %d wrong Expected = %d will send REJ",nS,cptr->vR);
        if (PF_GET(ctrlByte))   /* we will send REJ only when other waiting for resp */
	  {
          addToSFramesList(cptr,REJ+PF_BIT);
	  cptr->state = ADLC_S_SEND_REJECT;
	  }
	break; /* No further processing	*/
	}

      cptr->state = ADLC_S_NORMAL;
      cptr->vR = INC_MODULO_N(cptr->vR,1,ADLC_MODULUS(pptr));

      if (infoBufLen > 0) 
        processIncomingData(cptr,cptr->pptr,INFO);

      if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE4)
        {
        if (isOkToPoll(cptr) == SD_TRUE)
          addToSFramesList(cptr,RR);
	}
      else if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE3)
	deleteSFramesListWithoutPf(cptr);
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
/*      stopWaitingForResp(cptr);					*/
      if (isNrValid(cptr,nR) == SD_FALSE) /* Invalid Nr */
        {
	ADLC_LOG_ERR2("ADLC ERROR Received wrong Nr = %d Expected = %d will send FRMR",nR,cptr->vS);
        if (PF_GET(ctrlByte))   /* we will send REJ only when other waiting for resp */
	  {
          addToUFramesList(cptr,FRMR+PF_BIT);
          cptr->state = ADLC_S_FRAME_REJECT;
	  }
	break; /* No further processing	*/
	}
      deleteAckedIFramesFromTxList(cptr,nR); /* will synchronize our vS  */

      if (isNsValid(cptr->vR,nS) == SD_FALSE)
	{
	ADLC_LOG_ERR2("ADLC ERROR Received Ns = %d wrong Expected = %d",nS,cptr->vR);
        if (PF_GET(ctrlByte))   /* we will send REJ only when other waiting for resp */
	  {
          addToSFramesList(cptr,REJ+PF_BIT);
      	  cptr->state = ADLC_S_SEND_REJECT;
	  }
	break; /* No further processing	*/
	}

      cptr->state = ADLC_S_NORMAL;
      cptr->vR = INC_MODULO_N(cptr->vR,1,ADLC_MODULUS(pptr));

      if (infoBufLen > 0) 
        processIncomingData(cptr,cptr->pptr,INFO);

      if (PF_GET(ctrlByte))
        {
        if (isOkToPoll(cptr) == SD_TRUE)
          addToSFramesList(cptr,RR+PF_BIT);
	}
#endif /* ADLC_SLAVE */
    break;

    default :
      adlc_except(thisFileName,__LINE__);	 /* should never happen		*/
    break;
    }
  return (ret);
  }
/************************************************************************/
/*			      actionForSFrame				*/
/************************************************************************/
static ST_RET actionForSFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;
ST_UCHAR nR;
ADLC_PORT_INFO *pptr = cptr->pptr;
ST_UCHAR frameType;

  nR = SUPR_NR_GET(ctrlByte);
  frameType = FRAME_TYPE_GET(pptr->rxBuf);

  ADLC_LOG_CIO1("Nr = %d ",nR);
#if defined(ADLC_LM)
  ADLC_LOG_CDEV_FILT1(cptr->dptr->address,"Nr = %d ",nR);
#endif
  switch (cptr->state)
    {
    case ADLC_S_DISCONNECT_PHASE :
      if (PF_GET(ctrlByte))
#if defined(ADLC_MASTER)
	{
	if (isUFrameQueuedUp(cptr) == SD_FALSE)
          addToUFramesList(cptr,DISC+PF_BIT);
	}
#endif	
#if defined(ADLC_SLAVE)
 	{
	if (isUFrameQueuedUp(cptr) == SD_FALSE)
          addToUFramesList(cptr,DM+PF_BIT);
	}
#endif
    break;  

    case ADLC_S_LINK_DISCONNECT :
    case ADLC_S_LINK_SETUP :
    case ADLC_S_RESET :	
      ADLC_LOG_DEBUG3("Connection 0x%x->0x%x in invalid state (%s) to receive INFO frame ignoring",
			cptr->masterAddr,cptr->dptr->address,getConState(cptr->state));
    break;

#if defined(ADLC_SLAVE)
    case ADLC_S_FRAME_REJECT : 	
/*      stopWaitingForResp(cptr);					*/
      if (PF_GET(ctrlByte))
        addToUFramesList(cptr,FRMR+PF_BIT);
    break;
#endif

    case ADLC_S_SEND_REJECT :
    case ADLC_S_REMOTE_BUSY :
    case ADLC_S_NORMAL :
#if defined(ADLC_MASTER)
/* First take care of timeout					*/
      stopWaitingForResp(cptr,SD_TRUE);		/* we are done waiting for the response	*/
/* check if nr is valid, if not send disc */
      if (isNrValid(cptr,nR)  == SD_FALSE)
        {
	ADLC_LOG_ERR2("ADLC ERROR Received wrong Nr = %d Expected = %d will send DISC",nR,cptr->vS);
        giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE,SD_SUCCESS, 0L);
        addToUFramesList(cptr,DISC + PF_BIT);
	cptr->state = ADLC_S_LINK_DISCONNECT;
	break; /* No further processing	*/
	}
      deleteAckedIFramesFromTxList(cptr,nR); /* will synchronize our vS  */

      if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE4)
        {
        if (isOkToPoll(cptr) == SD_TRUE)
          addToSFramesList(cptr,RR);
	}
      else if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE3)
	deleteSFramesListWithoutPf(cptr);
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
/* First take care of timeout					*/
/*      stopWaitingForResp(cptr);					*/
/* check if nr is valid, if not send frmr */
      if (isNrValid(cptr,nR)  == SD_FALSE)
        {
	ADLC_LOG_ERR2("ADLC ERROR Received Nr = %d wrong Expected = %d",nR,cptr->vS);
        if (PF_GET(ctrlByte))
	  {
          addToUFramesList(cptr,FRMR+PF_BIT);
	  cptr->state = ADLC_S_FRAME_REJECT;
	  }
	break; /* No further processing	*/
	}
      deleteAckedIFramesFromTxList(cptr,nR); /* will synchronize our vS  */

      if (PF_GET(ctrlByte))
        {
        if (isOkToPoll(cptr) == SD_TRUE)
          addToSFramesList(cptr,RR+PF_BIT);
	}
#endif /* ADLC_SLAVE */
    break;

    default :
      adlc_except(thisFileName,__LINE__);	 /* should never happen		*/
    break;
    }
  return (ret);
  }

/************************************************************************/
/*			      actionForRrFrame				*/
/************************************************************************/
static ST_RET actionForRrFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;

  ret = actionForSFrame(cptr,ctrlByte);
  if (cptr->state == ADLC_S_REMOTE_BUSY || cptr->state == ADLC_S_SEND_REJECT)
    cptr->state = ADLC_S_NORMAL;

  return(ret);
  }

/************************************************************************/
/*			      actionForRnrFrame				*/
/************************************************************************/
static ST_RET actionForRnrFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;

  ret = actionForSFrame(cptr,ctrlByte);
  if (cptr->state == ADLC_S_NORMAL || cptr->state == ADLC_S_SEND_REJECT)
    cptr->state = ADLC_S_REMOTE_BUSY;

  return(ret);
  }

/************************************************************************/
/*			       actionForRejFrame			*/
/************************************************************************/
static ST_RET actionForRejFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;

  ret = actionForSFrame(cptr,ctrlByte);
  if (cptr->state == ADLC_S_REMOTE_BUSY || cptr->state == ADLC_S_SEND_REJECT)
    cptr->state = ADLC_S_NORMAL;

  return(ret);
  }


/************************************************************************/
/*			      actionForUiFrame				*/
/************************************************************************/
static ST_RET actionForUiFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
#if !defined(UCA_SMP)
  /* ignore UI */
  return (SD_SUCCESS);
  }
#else
ADLC_PORT_INFO *pptr = cptr->pptr;
ST_RET ret = SD_SUCCESS;
ST_INT infoBufLen;
ST_UCHAR frameType;

  infoBufLen = FRAME_LEN_GET(pptr->rxBuf) + EOF_FIELD_LEN - MIN_INFO_FRAME_LEN;
  frameType = FRAME_TYPE_GET(pptr->rxBuf);
  switch (cptr->state)
    {
    case ADLC_S_DISCONNECT_PHASE :
      if (PF_GET(ctrlByte))
#if defined(ADLC_MASTER)
	{
	if (isUFrameQueuedUp(cptr) == SD_FALSE)
          addToUFramesList(cptr,DISC+PF_BIT);
	}
#endif
#if defined(ADLC_SLAVE)
 	{
	if (isUFrameQueuedUp(cptr) == SD_FALSE)
          addToUFramesList(cptr,DM+PF_BIT);
	}
#endif
    break;

    case ADLC_S_LINK_DISCONNECT :
    case ADLC_S_LINK_SETUP :
    case ADLC_S_RESET :
      ADLC_LOG_DEBUG3("Connection 0x%x->0x%x in invalid state (%s)  to receive INFO frame ignoring",
			cptr->masterAddr,cptr->dptr->address,getConState(cptr->state));
    break;

#if defined(ADLC_SLAVE)
    case ADLC_S_FRAME_REJECT : 	
/*      stopWaitingForResp(cptr);					*/
      if (PF_GET(ctrlByte))
        addToUFramesList(cptr,FRMR+PF_BIT);
    break;
#endif

    case ADLC_S_REMOTE_BUSY :
    case ADLC_S_SEND_REJECT :
    case ADLC_S_NORMAL :
#if defined(ADLC_MASTER)
/* First take care of timeout					*/
      if (PF_GET(ctrlByte))
	stopWaitingForResp(cptr,SD_TRUE);
      else
	updateRespTimeout(pptr);	

      cptr->state = ADLC_S_NORMAL;

      if (infoBufLen > 0) 
        processIncomingData(cptr,cptr->pptr,UI);

      if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE4)
        {
        if (isOkToPoll(cptr) == SD_TRUE)
          addToSFramesList(cptr,RR);
	}
      else if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE3)
	deleteSFramesListWithoutPf(cptr);
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
/*      stopWaitingForResp(cptr);					*/

      cptr->state = ADLC_S_NORMAL;

      if (infoBufLen > 0) 
        processIncomingData(cptr,cptr->pptr,UI);

      if (PF_GET(ctrlByte))
        {
        if (isOkToPoll(cptr) == SD_TRUE)
          addToSFramesList(cptr,RR+PF_BIT);
	}
#endif /* ADLC_SLAVE */
    break;

    default :
      adlc_except(thisFileName,__LINE__);	 /* should never happen		*/
    break;
    }
  return (ret);
  }
#endif /* UCA_SMP */

#if defined(ADLC_MASTER)
/************************************************************************/
/*			        actionForUaFrame			*/
/************************************************************************/
static ST_RET actionForUaFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;
  switch (cptr->state)
    {
    case ADLC_S_LINK_SETUP : 
      stopWaitingForResp(cptr,SD_TRUE);
      cptr->state = ADLC_S_NORMAL;	
      giveIndicationToStack(cptr,ADLC_CMD_CONNECT_NODE_DONE,SD_SUCCESS, 0L);
    break;

    case ADLC_S_LINK_DISCONNECT : 
      stopWaitingForResp(cptr,SD_TRUE);
      giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE_DONE,SD_SUCCESS, 0L);
      resetConnection(cptr,ADLC_DELETE_CON);
    break;

    case ADLC_S_DISCONNECT_PHASE :
    case ADLC_S_NORMAL :
    case ADLC_S_REMOTE_BUSY :
    case ADLC_S_SEND_REJECT :
      ADLC_LOG_DEBUG2 ("Connection 0x%x->0x%x in invalid state to receive UA frame ignoring",
		       cptr->masterAddr,cptr->dptr->address);
    break;

    default : 
      adlc_except(thisFileName,__LINE__);	 /* should never happen		*/
    break;
    }
  return (ret);
  }
/************************************************************************/
/*                           actionForRdFrame				*/
/************************************************************************/
static ST_RET actionForRdFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;

  stopWaitingForResp(cptr,SD_TRUE);
  giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE,SD_SUCCESS, 0L);
  resetConnection(cptr,ADLC_DELETE_CON);
  return (ret);
  }
/************************************************************************/
/*			    actionForDmFrame				*/
/************************************************************************/
static ST_RET actionForDmFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;

  stopWaitingForResp(cptr,SD_TRUE);
  giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE,SD_SUCCESS, 0L);
  resetConnection(cptr,ADLC_DELETE_CON);
  return (ret);
  }
/************************************************************************/

/************************************************************************/
static ST_RET actionForFrmrFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;

  stopWaitingForResp(cptr,SD_TRUE);
  giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE,SD_SUCCESS, 0L);
  resetConnection(cptr,ADLC_DELETE_CON);
  return (ret);
  }
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
/************************************************************************/
/*			      actionForSnrmFrame			*/
/************************************************************************/
static ST_RET actionForSnrmFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;
ADLC_PORT_INFO *pptr = cptr->pptr;
ST_UCHAR frameType = FRAME_TYPE_GET(pptr->rxBuf);

  switch (cptr->state)
    {
    case ADLC_S_DISCONNECT_PHASE : 	/* will change state to Normal	*/
    case ADLC_S_FRAME_REJECT : 	
    case ADLC_S_NORMAL : 	
    case ADLC_S_REMOTE_BUSY : 	
    case ADLC_S_SEND_REJECT :	
      resetConnection(cptr,ADLC_RESET_ONLY);
      if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE3) /* PF should always be set */
        addToUFramesList(cptr,UA+PF_BIT);
      else if (PF_GET(ctrlByte) && frameType == FRAME_FORMAT_TYPE4) /* PF should always be set */
        addToUFramesList(cptr,UA);
      cptr->state = ADLC_S_NORMAL;
/*      giveIndicationToStack(cptr,ADLC_CMD_CONNECT_NODE, 0L);		*/
    break;

#if 0 /* LINK_DISCONNECT and LINK_SETUP are for master, slave goes directly
         from disconnect to normal and normal to disconnect		*/	
    case ADLC_S_LINK_DISCONNECT : 
      if (PF_GET(ctrlByte))	/* PF should always be set */
	{
        addToUFramesList(cptr,DM+PF_BIT);
        cptr->state = ADLC_S_DISCONNECT_PHASE;
	}
    break;

    case ADLC_S_LINK_SETUP :	  	/* will remain in the same state	*/
    case ADLC_S_RESET :
      if (PF_GET(ctrlByte))
        addToUFramesList(cptr,UA+PF_BIT);
    break;
#endif
    default : 
      adlc_except(thisFileName,__LINE__);	
      ret = SD_FAILURE;
    break;
    }
  return (ret);
  }

/************************************************************************/
/*			    actionForDiscFrame				*/
/************************************************************************/
static ST_RET actionForDiscFrame(ADLC_CON_INFO *cptr,ST_UCHAR ctrlByte)
  {
ST_RET ret = SD_SUCCESS;

  switch (cptr->state)
    {
    case ADLC_S_DISCONNECT_PHASE : 	
      if (PF_GET(ctrlByte))
        addToUFramesList(cptr,DM+PF_BIT);
    break;

    case ADLC_S_LINK_DISCONNECT :
/*      giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE, 0L);	*/
/*      resetConnection(cptr,ADLC_RESET_ONLY);				*/
      if (PF_GET(ctrlByte))
        addToUFramesList(cptr,UA+PF_BIT);
    break;

    case ADLC_S_LINK_SETUP :
    case ADLC_S_RESET :
/*      giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE, 0L);	*/
/*      resetConnection(cptr,ADLC_RESET_ONLY);				*/
      if (PF_GET(ctrlByte))
        addToUFramesList(cptr,DM+PF_BIT);
      cptr->state = ADLC_S_DISCONNECT_PHASE;
    break;

    case ADLC_S_FRAME_REJECT :
    case ADLC_S_NORMAL :
    case ADLC_S_REMOTE_BUSY :
    case ADLC_S_SEND_REJECT :
/*      giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE, 0L);	*/
/*      resetConnection(cptr,ADLC_RESET_ONLY);				*/
      if (PF_GET(ctrlByte))
        addToUFramesList(cptr,UA+PF_BIT);
      cptr->state = ADLC_S_DISCONNECT_PHASE;
    break;

    default : 
      adlc_except(thisFileName,__LINE__);	 /* should never happen	*/
      ret = SD_FAILURE;
    break;
    }
  return (ret);
  }
#endif /* ADLC_SLAVE */
/************************************************************************/
/*		                isNrValid				*/
/************************************************************************/
static ST_BOOLEAN isNrValid(ADLC_CON_INFO *cptr,ST_UCHAR nR)
  {
ST_UCHAR vS = cptr->vS;
ST_UCHAR sendCount = cptr->sendCount;

  if (DIFF_MODULO_N(vS,nR,ADLC_MODULUS(cptr->pptr)) <= sendCount)
    return (SD_TRUE);
  return(SD_FALSE);
  }

/************************************************************************/
/*				isNsValid				*/
/************************************************************************/
static ST_BOOLEAN isNsValid(ST_UCHAR vR,ST_UCHAR nS)
  {
  if (nS == vR)
    return (SD_TRUE);
  return (SD_FALSE);
  }

/************************************************************************/
/*			      processIncomingData			*/
/* This function puts the data into rxList and if it is end of segmet	*/
/* then it gives data to stack						*/
/* Note : If the cptr is NULL it means that this data is a multicast  	*/
/* Note : we already know that dataLen > 0				*/
/************************************************************************/
static ST_VOID processIncomingData(ADLC_CON_INFO *cptr,ADLC_PORT_INFO *pptr,ST_UCHAR cmd)
  {
ST_INT dataLen;

  /* get the length of the data (the actual INFO)			*/
  dataLen = FRAME_LEN_GET(pptr->rxBuf) + EOF_FIELD_LEN - MIN_INFO_FRAME_LEN;

  if (cptr && cmd == INFO) /* the data is connection data of INFO frames */
    addToIFramesInRxList(cptr,&pptr->rxBuf[INFO_FIELD_OFFSET],dataLen);
#if defined(UCA_SMP)
  else if (cptr && cmd == UI)
    addToUIFramesInRxList(cptr,&pptr->rxBuf[INFO_FIELD_OFFSET],dataLen);
#endif
  else
    addToMFramesInRxList(pptr,&pptr->rxBuf[INFO_FIELD_OFFSET],dataLen);

  if (EOS_GET(pptr->rxBuf) == 0)	/* End of segment bit is zero	*/
    giveDataToStack(cptr,pptr,cmd);
  }
/************************************************************************/
/*			        giveDataToStack				*/
/* give the data to the stack 						*/
/* Note : If the cptr is NULL it means that this data is a multicast  	*/
/* The pseucode will be as follows					*/
/* - get local address							*/
/* - get remote address							*/
/* - get dataSize							*/
/* - reset xxxptr->infoBufLen						*/
/* - call giveDataToStackForxxx to handle OS specific things		*/
/************************************************************************/
static ST_VOID giveDataToStack (ADLC_CON_INFO *cptr,ADLC_PORT_INFO *pptr,ST_UCHAR cmd)
  {
ST_UINT16 localAddr;
ST_UINT16 remoteAddr;
ST_INT dataSize;

  adlcStats.rxPackets++;
  localAddr =  DST_ADDR_GET(pptr->rxBuf);
  remoteAddr = SRC_ADDR_GET(pptr->rxBuf);
  if (cptr && cmd == INFO)
    {
    dataSize = cptr->infoBufLen;
    cptr->infoBufLen = 0;
    }	
#if defined(UCA_SMP)
  else if (cptr && cmd == UI)
    {
    /* allocate enough space for data from remote and time stamp when 	*/
    /* packet was received						*/
    dataSize = cptr->extInfoBufLen + SMP_SIZE_OF_REC_TIME;
    cptr->extInfoBufLen = 0;
    }	
#endif
  else
    {		
    dataSize = pptr->infoBufLen;
    pptr->infoBufLen = 0;
    }
  if (cmd == INFO)
    sendWritePacketToStack(localAddr,remoteAddr,dataSize,cptr,pptr);
#if defined(UCA_SMP)
  else
    sendExtWritePacketToStack(localAddr,remoteAddr,dataSize,cptr,pptr);
#endif
  }
/************************************************************************/
/*			     giveIndicationToStack			*/
/* For OS/2 the psudo code will be as follows				*/
/* - get local address							*/
/* - get remote address							*/
/* - get tptr from local address					*/
/* - get shared memory for cmd header					*/
/* - put a switch on the opcode and fill the adlc_cmd			*/
/* - Use ipcSendCmdMsg to write to the queue				*/
/* For DOS the psudo code will be as follows				*/
/* - get local address							*/
/* - get remote address							*/
/* - put a switch on the opcode and allocate the proper control block	*/
/* - fill the control block						*/
/* - Call addToEventList						*/
/************************************************************************/
ST_VOID giveIndicationToStack(ADLC_CON_INFO *cptr,ST_INT opcode,
		ST_RET retCode, ST_LONG usr1)
  {
ST_UINT16 localAddr;
ST_UINT16 remoteAddr;

#if !defined(ADLC_LM)
ADLC_CONNECT_NODE *pstConNode;  
ADLC_DISCONNECT_NODE *pstDiscNode;
ADLC_DISCONNECT_NODE_DONE *pstDiscNodeDone;
ADLC_CONNECT_NODE_DONE *pstConNodeDone;
ADLC_WRITE_PACKET_DONE *pstWritePacketDone;
ADLC_EVENT_INFO *eptr;
#endif

#if defined(ADLC_LM)
ADLC_CMD cmdMsg;
ST_INT stackTaskId;
#endif

  localAddr =  getLocalAddr(cptr);
  remoteAddr = getRemoteAddr(cptr);

#if defined(ADLC_LM)

  if (getTaskId(cptr,&stackTaskId) != SD_SUCCESS)
    {
/*    ADLC_LOG_ERR1("ADLC ERROR Do not know which queue to write to for local address = 0x%x",localAddr); */
    return;
    }

  memset (&cmdMsg, '\x0', sizeof (ADLC_CMD));
  cmdMsg.hdr.doneSem = 0;		/* Semaphore not used.	*/
  cmdMsg.hdr.cmdRslt = retCode;

  ADLC_LOG_USER5("Issuing %s to stack (Task Id = %d) for Connection 0x%x->0x%x cmdRslt = %s",
		getOpCode(opcode),stackTaskId, cptr->masterAddr,cptr->dptr->address,getRetCode(retCode));

#else /* ADLC_LM not defined*/

  ADLC_LOG_USER4("Issuing %s to stack for Connection 0x%x->0x%x cmdRslt = %s",
		getOpCode(opcode),cptr->masterAddr,cptr->dptr->address,getRetCode(retCode));
#endif /* ADLC_LM */

  switch (opcode)
    {
    case ADLC_CMD_CONNECT_NODE :
#if !defined(ADLC_LM)
      pstConNode = (ADLC_CONNECT_NODE *) ipcChkCalloc(1,sizeof(ADLC_CONNECT_NODE));	
      pstConNode->i.localAddr = localAddr;
      pstConNode->i.remoteAddr = remoteAddr;
      eptr = addToEventList(ADLC_CMD_CONNECT_NODE,pstConNode,retCode);
#endif
#if defined(ADLC_LM)
      cmdMsg.hdr.cmdCode = ADLC_CMD_CONNECT_NODE;
      cmdMsg.u.conNode.i.localAddr = localAddr;
      cmdMsg.u.conNode.i.remoteAddr = remoteAddr;
#endif
    break;

    case ADLC_CMD_DISCONNECT_NODE :
#if !defined(ADLC_LM)
      pstDiscNode = (ADLC_DISCONNECT_NODE *) ipcChkCalloc(1,sizeof(ADLC_DISCONNECT_NODE));	
      pstDiscNode->i.localAddr = localAddr;
      pstDiscNode->i.remoteAddr = remoteAddr;
      eptr = addToEventList(ADLC_CMD_DISCONNECT_NODE,pstDiscNode,retCode);
#endif
#if defined(ADLC_LM)
      cmdMsg.hdr.cmdCode = ADLC_CMD_DISCONNECT_NODE;
      cmdMsg.u.discNode.i.localAddr = localAddr;
      cmdMsg.u.discNode.i.remoteAddr = remoteAddr;
#endif
    break;

    case ADLC_CMD_CONNECT_NODE_DONE :
#if !defined(ADLC_LM)
      pstConNodeDone = (ADLC_CONNECT_NODE_DONE *) ipcChkCalloc(1,sizeof(ADLC_CONNECT_NODE));	
      pstConNodeDone->i.localAddr = localAddr;
      pstConNodeDone->i.remoteAddr = remoteAddr;
      eptr = addToEventList(ADLC_CMD_CONNECT_NODE_DONE,pstConNodeDone,retCode);
#endif
#if defined(ADLC_LM)
      cmdMsg.hdr.cmdCode = ADLC_CMD_CONNECT_NODE_DONE;
      cmdMsg.u.conNodeDone.i.localAddr = localAddr;
      cmdMsg.u.conNodeDone.i.remoteAddr = remoteAddr;
#endif
    break;

    case ADLC_CMD_DISCONNECT_NODE_DONE :
#if !defined(ADLC_LM)
      pstDiscNodeDone = (ADLC_DISCONNECT_NODE_DONE *) ipcChkCalloc(1,sizeof(ADLC_DISCONNECT_NODE));	
      pstDiscNodeDone->i.localAddr = localAddr;
      pstDiscNodeDone->i.remoteAddr = remoteAddr;
      eptr = addToEventList(ADLC_CMD_DISCONNECT_NODE_DONE,pstDiscNodeDone,retCode);
#endif
#if defined(ADLC_LM)
      cmdMsg.hdr.cmdCode = ADLC_CMD_DISCONNECT_NODE_DONE;
      cmdMsg.u.discNodeDone.i.localAddr = localAddr;
      cmdMsg.u.discNodeDone.i.remoteAddr = remoteAddr;
#endif
    break;

    case ADLC_CMD_WRITE_PACKET_DONE :
#if !defined(ADLC_LM)
      pstWritePacketDone = (ADLC_WRITE_PACKET_DONE *) ipcChkCalloc(1,sizeof(ADLC_WRITE_PACKET_DONE));	
      pstWritePacketDone->i.localAddr = localAddr;
      pstWritePacketDone->i.remoteAddr = remoteAddr;
      pstWritePacketDone->i.usr1 = usr1;
      eptr = addToEventList(ADLC_CMD_WRITE_PACKET_DONE,pstWritePacketDone,retCode);
#endif
#if defined(ADLC_LM)
      cmdMsg.hdr.cmdCode = ADLC_CMD_WRITE_PACKET_DONE;
      cmdMsg.u.writePacketDone.i.localAddr = localAddr;
      cmdMsg.u.writePacketDone.i.remoteAddr = remoteAddr;
      cmdMsg.u.writePacketDone.i.usr1 = usr1;
#endif
    break;

    default :
      adlc_except(thisFileName,__LINE__);	 /* should never happen		*/
    break;
    }

#if defined(ADLC_LM)
  sendDataMsgToStack(&cmdMsg,stackTaskId);
#endif /* ADLC_LM */  
  }

/************************************************************************/
/*			sendWritePacketToStack				*/
/* If the link manager is not defined it will use event list to 	*/
/* send the data, the algorithm will be as follows			*/
/* - allocate a control block						*/
/* - fill the control block						*/
/* - set the global event						*/
/*									*/
/* For the link manager, it will use shared memory to send the data	*/
/* the algorithm will be as follows					*/
/* - resolve the task Id						*/
/* - get shared memory for cmd header					*/
/* - get shared memory for data						*/
/* - Use ipcSendCmdMsg to write to the queue				*/
/* - Free shared memory for data					*/
/*									*/
/* Note1: If the cptr is NULL it means that this data is a multicast or */
/* broadcast. In that case copy data from pptr->rxMFramesList and pass  */
/* to all the stacks							*/
/* If cptr is not NULL then copy data from cptr->rxIFramesList and pass	*/
/* to appropriate stack							*/
/* Note2: If any function fails before copying the connection data we 	*/
/* need to deleteIFramesFromRxList(cptr)				*/
/************************************************************************/
static ST_VOID sendWritePacketToStack(ST_UINT16 localAddr,ST_UINT16 remoteAddr,
			   ST_INT dataSize,
			   ADLC_CON_INFO *cptr, ADLC_PORT_INFO *pptr)
#if !defined(ADLC_LM)
  {
ADLC_WRITE_PACKET *adlcPacket;
ST_CHAR *pData;
ADLC_EVENT_INFO *eptr;

  adlcPacket = (ADLC_WRITE_PACKET *) ipcChkCalloc(1,sizeof(ADLC_WRITE_PACKET));
  adlcPacket->i.dataSize = dataSize;
  adlcPacket->i.localAddr = localAddr;
  adlcPacket->i.remoteAddr = remoteAddr;
  adlcPacket->i.data = ipcChkCalloc(1,(ST_UINT) dataSize);

  pData = (ST_CHAR *) adlcPacket->i.data;
  if (cptr)
    copyDataIntoMemForStack(pData,&cptr->rxIFramesList);
  else
    copyDataIntoMemForStack(pData,&pptr->rxMFramesList);

  ADLC_LOG_USER3 ("giveDataToStack: Packet received Remote = 0x%x, Local = 0x%x Size = %d",
  		   remoteAddr,localAddr,dataSize);
  ADLC_LOG_USER_HEX (adlcPacket->i.dataSize,adlcPacket->i.data);
  eptr = addToEventList(ADLC_CMD_WRITE_PACKET,adlcPacket,SD_SUCCESS);
  }
#endif /* !ADLC_LM */

#if defined(ADLC_LM)
  {
ADLC_CMD *dstPstCmd;
ST_CHAR errBuf[ERR_STR_LEN];
ST_INT retCode;
ST_INT stackTaskId;
ST_ULONG msgSize = dataSize + sizeof(ADLC_CMD); /* for ipc api */

/*-----------------------------------------------------------------------
 * First resolve task Ids					
 *-----------------------------------------------------------------------*/
  if (getTaskId(cptr,&stackTaskId) != SD_SUCCESS)
    {
/*    ADLC_LOG_ERR1("ADLC ERROR Do not know which queue to write to for local address = 0x%x",localAddr); */
    if (cptr) /* the data was connection data			*/
      deleteIFramesFromRxList(cptr);
    else
      deleteMFramesFromRxList(pptr);
    return;
    }

/*-----------------------------------------------------------------------
 * Allocate shared memory for data and cmd header
 *-----------------------------------------------------------------------*/

  retCode = ipcAllocSMem (msgSize,(PVOID *) &dstPstCmd);
  if (retCode != SD_SUCCESS)
    {
    ipcGetErrString (retCode, errBuf, sizeof (errBuf));
    logIpcError(retCode,"Unable to allocate shared memory",thisFileName,__LINE__);
    if (cptr) /* the data was connection data			*/
      deleteIFramesFromRxList(cptr);
    else
      deleteMFramesFromRxList(pptr);
    return;
    }
 

/*-----------------------------------------------------------------------
 * Fill in dstPstCmd	
 *-----------------------------------------------------------------------*/

  dstPstCmd->hdr.cmdCode = ADLC_CMD_WRITE_PACKET;
  dstPstCmd->hdr.doneSem = 0;		/* Semaphore not used.	*/
  dstPstCmd->hdr.cmdRslt = SD_SUCCESS;

  dstPstCmd->u.writePacket.i.remoteAddr = remoteAddr;
  dstPstCmd->u.writePacket.i.localAddr = localAddr;
  dstPstCmd->u.writePacket.i.dataSize = dataSize;

 /* copy the data into shared memory 				*/
  if (cptr)
    copyDataIntoMemForStack(dstPstCmd->data,&cptr->rxIFramesList);
  else
    copyDataIntoMemForStack(dstPstCmd->data,&pptr->rxMFramesList);
  dstPstCmd->dataSize = dataSize;


  /*---------------------------------------------------------------------
   * everything is ok so send to the stack
   * If it is straight connection daa then send that data to only that task
   * but if this is a multicast or broadcast then send data to all intersting
   * tasks who have issued open interface to ADLC-LM
   ---------------------------------------------------------------------*/
  ADLC_LOG_USER3 ("giveDataToStack: Packet received Remote = 0x%x, Local = 0x%x Size = %d",
  		   remoteAddr,localAddr,dataSize);
  ADLC_LOG_USER_HEX (dstPstCmd->dataSize,dstPstCmd->data);

  if (cptr) /* this is straight connection data */
    {
    sendDataMsgToStackNoAlloc(dstPstCmd,stackTaskId);
    }
  else	  /* this is multicast data so send to all interesting tasks */
    {	
    if (connectedTask[RLI_TASK_ID_COGW] == SD_TRUE)
      sendDataMsgToStackNoAlloc(dstPstCmd,RLI_TASK_ID_COGW);
    if (connectedTask[RLI_TASK_ID_MCGW] == SD_TRUE)
      sendDataMsgToStackNoAlloc(dstPstCmd,RLI_TASK_ID_MCGW);
    if (connectedTask[RLI_TASK_ID_TRIM7] == SD_TRUE)
      sendDataMsgToStackNoAlloc(dstPstCmd,RLI_TASK_ID_TRIM7);
    }
  /*---------------------------------------------------------------------
   * we are done with the shared memory, free it		       
   ---------------------------------------------------------------------*/
  if ((retCode = ipcFreeSMem(dstPstCmd)) != SD_SUCCESS)
    logIpcError(retCode,"Unable to free shared memory",thisFileName,__LINE__);
  }
#endif /* ADLC_LM */

#if defined(UCA_SMP)
/************************************************************************/
/*			sendExtWritePacketToStack			*/
/* If the link manager is not defined it will use event list to 	*/
/* send the data, the algorithm will be as follows			*/
/* - allocate a control block						*/
/* - fill the control block						*/
/* - set the global event						*/
/*									*/
/* For the link manager, it will use shared memory to send the data	*/
/* the algorithm will be as follows					*/
/* - resolve the task Id						*/
/* - get shared memory for cmd header					*/
/* - get shared memory for data						*/
/* - Use ipcSendCmdMsg to write to the queue				*/
/* - Free shared memory for data					*/
/*									*/
/* Note1: If the cptr is NULL it means that this data is a multicast or */
/* broadcast. In that case copy data from pptr->rxMFramesList and pass  */
/* to all the stacks							*/
/* If cptr is not NULL then copy data from cptr->rxIFramesList and pass	*/
/* to appropriate stack							*/
/* Note2: If any function fails before copying the connection data we 	*/
/* need to deleteIFramesFromRxList(cptr)				*/
/************************************************************************/
static ST_VOID sendExtWritePacketToStack(ST_UINT16 localAddr,ST_UINT16 remoteAddr,
			   ST_INT dataSize,
			   ADLC_CON_INFO *cptr, ADLC_PORT_INFO *pptr)
#if !defined(ADLC_LM)
  {
ADLC_EXT_WRITE_PACKET *adlcPacket;
ST_CHAR *pData;
ADLC_EVENT_INFO *eptr;
ST_CHAR recTime[SMP_SIZE_OF_REC_TIME] = {0}; /* buffer to hold time stamp */

  u_smp_record_time(recTime,sizeof(recTime));

  adlcPacket = (ADLC_EXT_WRITE_PACKET *) ipcChkCalloc(1,sizeof(ADLC_EXT_WRITE_PACKET));
  adlcPacket->i.dataSize = dataSize;
  adlcPacket->i.localAddr = localAddr;
  adlcPacket->i.remoteAddr = remoteAddr;
  adlcPacket->i.data = ipcChkCalloc(1,(ST_UINT) dataSize);
  pData = (ST_CHAR *) adlcPacket->i.data;
  if (cptr)
    copyDataIntoMemForStack(pData,&cptr->rxUIFramesList);
  else
    copyDataIntoMemForStack(pData,&pptr->rxMFramesList);

  /* attach time stamp with data */
  memcpy(&pData[dataSize - SMP_SIZE_OF_REC_TIME],recTime,sizeof(recTime));

  /* also put time stamp in time stamp structure member */
  memcpy(adlcPacket->i.recTime,recTime,sizeof(recTime));

  ADLC_LOG_USER3 ("giveExtDataToStack: Packet received Remote = 0x%x, Local = 0x%x Size = %d",
  		   remoteAddr,localAddr,dataSize);
  ADLC_LOG_USER_HEX (adlcPacket->i.dataSize,adlcPacket->i.data);
  eptr = addToEventList(ADLC_CMD_EXT_WRITE_PACKET,adlcPacket,SD_SUCCESS);
  }
#endif /* !ADLC_LM */

#if defined(ADLC_LM)
  {
ADLC_CMD *dstPstCmd;
ST_CHAR errBuf[ERR_STR_LEN];
ST_INT retCode;
ST_INT stackTaskId;
ST_ULONG msgSize = dataSize + sizeof(ADLC_CMD); /* for ipc api */
  
/*-----------------------------------------------------------------------
 * First resolve task Ids					
 *-----------------------------------------------------------------------*/
  if (getTaskId(cptr,&stackTaskId) != SD_SUCCESS)
    {
/*    ADLC_LOG_ERR1("ADLC ERROR Do not know which queue to write to for local address = 0x%x",localAddr); */
    if (cptr) /* the data was connection data			*/
      deleteUIFramesFromRxList(cptr);
    else
      deleteMFramesFromRxList(pptr);
    return;
    }

/*-----------------------------------------------------------------------
 * Allocate shared memory for data and cmd header
 *-----------------------------------------------------------------------*/

  retCode = ipcAllocSMem (msgSize,(PVOID *) &dstPstCmd);
  if (retCode != SD_SUCCESS)
    {
    ipcGetErrString (retCode, errBuf, sizeof (errBuf));
    logIpcError(retCode,"Unable to allocate shared memory",thisFileName,__LINE__);
    if (cptr) /* the data was connection data			*/
      deleteUIFramesFromRxList(cptr);
    else
      deleteMFramesFromRxList(pptr);
    return;
    }

/*-----------------------------------------------------------------------
 * Fill in dstPstCmd	
 *-----------------------------------------------------------------------*/

  dstPstCmd->hdr.cmdCode = ADLC_CMD_EXT_WRITE_PACKET;
  dstPstCmd->hdr.doneSem = 0;		/* Semaphore not used.	*/
  dstPstCmd->hdr.cmdRslt = SD_SUCCESS;

  dstPstCmd->u.extWritePacket.i.remoteAddr = remoteAddr;
  dstPstCmd->u.extWritePacket.i.localAddr = localAddr;
  dstPstCmd->u.extWritePacket.i.dataSize = dataSize;

 /* copy the data into shared memory 				*/
  if (cptr)
    copyDataIntoMemForStack(dstPstCmd->data,&cptr->rxUIFramesList);
  else
    copyDataIntoMemForStack(dstPstCmd->data,&pptr->rxMFramesList);
  dstPstCmd->dataSize = dataSize;

  /* attach time stamp with data */
  memcpy(&dstPstCmd->data[dataSize - SMP_SIZE_OF_REC_TIME],pptr->recTime,SMP_SIZE_OF_REC_TIME);

  /* also put time stamp in time stamp structure member */
  memcpy(dstPstCmd->u.extWritePacket.i.recTime,pptr->recTime,SMP_SIZE_OF_REC_TIME);

  /*---------------------------------------------------------------------
   * everything is ok so send to the stack
   * If it is straight connection daa then send that data to only that task
   * but if this is a multicast or broadcast then send data to all intersting
   * tasks who have issued open interface to ADLC-LM
   ---------------------------------------------------------------------*/
  ADLC_LOG_USER3 ("giveExtDataToStack: Packet received Remote = 0x%x, Local = 0x%x Size = %d",
  		   remoteAddr,localAddr,dataSize);
  ADLC_LOG_USER_HEX (dstPstCmd->dataSize,dstPstCmd->data);

  if (cptr) /* this is straight connection data */
    {
    sendDataMsgToStackNoAlloc(dstPstCmd,stackTaskId);
    }
  else	  /* this is multicast data so send to all interesting tasks */
    {	
    if (connectedTask[RLI_TASK_ID_COGW] == SD_TRUE)
      sendDataMsgToStackNoAlloc(dstPstCmd,RLI_TASK_ID_COGW);
    if (connectedTask[RLI_TASK_ID_MCGW] == SD_TRUE)
      sendDataMsgToStackNoAlloc(dstPstCmd,RLI_TASK_ID_MCGW);
    if (connectedTask[RLI_TASK_ID_TRIM7] == SD_TRUE)
      sendDataMsgToStackNoAlloc(dstPstCmd,RLI_TASK_ID_TRIM7);
    }
  /*---------------------------------------------------------------------
   * we are done with the shared memory, free it		       
   ---------------------------------------------------------------------*/
  if ((retCode = ipcFreeSMem(dstPstCmd)) != SD_SUCCESS)
    logIpcError(retCode,"Unable to free shared memory",thisFileName,__LINE__);
  }
#endif /* ADLC_LM */
#endif /* UCA_SMP */
/************************************************************************/
/*			copyDataIntoMemForStack				*/
/* This function reassembles the data from smaller ADLC Info frames to	*/
/* large LSDU								*/
/************************************************************************/
static ST_VOID copyDataIntoMemForStack(ST_CHAR *pData,ADLC_FRAME **rxFramesList)
  {
ADLC_FRAME *fptr;
ADLC_FRAME *headOfList;

  headOfList = *rxFramesList;
  while (headOfList)
    {
    fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **)&headOfList);
    memcpy(pData,fptr->u.infoBuf,fptr->infoLen);
    pData = pData + fptr->infoLen;
    ipcChkFree (fptr->u.infoBuf);
    ipcChkFree (fptr);	
    }

  *rxFramesList = headOfList;
  }






