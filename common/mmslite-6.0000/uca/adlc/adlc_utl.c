/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_utl.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* It has utility functions used by other functions			*/
/* It has three main category of functions which all basically work	*/
/* with linked list							*/
/* 1- getPortPtrFromxxx, getDevPtrFromxxx, getConPtrFromxxx		*/
/*   These type of functions return port, device or connection pointer	*/
/*   They basically search for some specific information xxx passed in 	*/
/*   the argument in the whole list. If found it returns the pointer 	*/
/*   for that control block else returns NULL				*/
/*                                                                      */
/* 2- addToxxxList 							*/
/*   These type of functions do the following three tasks		*/
/*   - Allocate memory for the control block				*/
/*   - Fill the control block with the info passed into arguments	*/
/*   - calls list_add_last						*/
/*   Note : The caller of these functions should check all the error	*/
/*   conditions (which will prohibit to add to the list). The caller 	*/
/*   functions could be like initPort,adlcAddDev, isOkToAddMFramesList 	*/
/*   etc.								*/
/*                                                                      */
/* 3- deletexxx								*/
/*   These type of functions free all the allocated memory associated	*/
/*   with the passed argument and unlink from the list. They do it by	*/
/*   going through all the nodes of the List of the passed argument 	*/
/*   unlink and free it							*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    38     Cleaned up general PClint errors             */
/* 09/23/02  KCR    37     Removed extra include files                  */
/* 09/23/02  KCR    36     Moved thisFileName before includes           */
/* 03/09/00  MDE    35     Added SD_CONST modifier to 'adlc_except'	*/
/* 09/13/99  MDE    34     Added SD_CONST modifiers         		*/
/* 08/13/98  JRB    33     Lint cleanup.				*/
/* 04/20/98  IKE    32     Added WRITE_PACKET_DONE Event		*/
/* 02/10/98  IKE    31     Changed to ipcPostCommonEvent to use new IPC */
/*			   which can handle more than 64 events		*/
/* 02/04/98  IKE    30     Added deleteUIFramesFromTxList to fix bug	*/
/* 10/20/97  IKE    29     Added addToListTime for Link Manager		*/
/*			   Returned error code instead of failure in	*/
/*			   addTaskToLocalAddr				*/
/* 09/23/97  IKE    28     Added adlcExtWritePacket for define UCA_SMP	*/
/* 			   Clean up of header files for non IPC version */
/*			   of WIN32 MMS lite				*/
/* 09/18/97  IKE    27     Added telephone support			*/
/* 07/16/97  IKE    26     Added device statistics and device filt log	*/
/* 06/12/97  IKE    25     Support for COGW				*/
/* 06/12/97  IKE    24     Made ADLC protocol transaction oriented	*/
/* 05/12/97  IKE    23     Changed to MMSEASE 7.0 Data Types		*/
/* 02/24/96  IKE    22	   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/*			   CFG_PORT also for DOS so multiple ports can	*/
/*			   be initialized in dos platform		*/
/* 02/17/97  IKE    21     Changed the function addTaskForLocalAddress  */
/*			   so it adds task to list even added previously*/
/* 02/03/97  IKE    20     adlcCfg per port basis			*/
/* 01/22/97  IKE    19     Changed txSFraxxx to txUFrxxx in Uframe check*/
/* 01/17/97  IKE    18     Added addToCfgPortList			*/
/*			   Changed pstPort to pstCfgPort		*/
/* 12/03/96  IKE    17     WIN32 Support, header files cleanup		*/ 
/*			   Added logIpcError				*/
/* 11/06/96  IKE    16     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/28/96  KCR    15     Converted to mulitple thread safe alloc funs */
/* 10/04/96  IKE    14     Added addDevToPollGrp, modified getStackId	*/
/* 09/17/96  IKE    13     Removed the free of slogCtrl for new lib 	*/
/* 08/22/96  IKE    12     Support for contention avoidance, moved 	*/
/*			   *getxxx functions into adlc_get.c and timeout*/
/*			   _error funct into adlc_xsv.c			*/
/* 08/16/96  IKE    11     Added check for device turnaround time in 	*/
/* 			   isOkToSendxxx, changed Multicast Frames from */
/*			   UI to INFO 					*/
/* 08/07/96  IKE    10     Added pptr->mode for slave			*/
/* 07/17/96  IKE    09     Removed some lint and C6.00 warnings 	*/
/* 06/21/96  IKE    08     Put #if defined(ADLC_MASTER) around some	*/
/*			   timing functions. Changed txService 		*/
/*			   to do some snrm pacing time as described in	*/
/*			   document					*/
/* 06/18/96  IKE    07     Bump cptr->retries whenever there is a time	*/
/*			   timeout_error. Put the algorithm either to	*/
/* 			   to maintain the connection or delete the 	*/
/*			   connection depending which stack we talking  */
/*			   Added the functions getLocalAddr, getTaskId	*/
/*			   getRemoteAddr, addTaskForLocalAddress	*/
/* 06/11/96  IKE    06     Changed addToMFramesList to add only one	*/
/* 			   Multicast frame per port			*/
/* 05/22/96  IKE    05     Header file cleanup. Enhanced Logging	*/
/*			   Put some more comments			*/
/*			   Changed timeout_error.Changed verifyCrc not 	*/
/*			   to include EOF_FIELD_LEN to calculate crc	*/
/*			   Changed addToDevList to add poll group	*/
/*			   Added getRetCode, isInfoQueuedUp, 		*/
/*			   isPollQueuedUp, isOkToSendxxx, deleteFrame, 	*/
/*			   addToPollGroupList				*/
/*			   Added some functions to deal with taskList 	*/
/*			   and eventList				*/
/*			   Got rid of nackAllIFramesFromTxList		*/
/* 04/05/96  IKE    04     Change Logging from device osi terminology	*/
/* 			   to local remote terminology			*/
/* 04/05/96  rkr    02     Header file and SAVE_CALLING cleanup 	*/
/* 04/04/96  rkr    01     Cleanup					*/
/* 10/25/95  IKE   1.00    Initial release                              */
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

#ifdef ADLC_TESTING
#include "fkeydefs.h"
#endif

/************************************************************************/
/*      Static Function Declarations                           		*/
/************************************************************************/

static ST_UINT16 updateCrc (ST_INT16 c, ST_UINT16 crc);

/************************************************************************/
/*			eventTypeOf					*/
/* input : control byte of the frame					*/
/* output : UA,INFO,RR,RNR,DM etc.					*/
/* Based on Table 1 pg 13 ISO/IEC VWXYZ: 199A (E)			*/
/************************************************************************/
ST_UCHAR eventTypeOf(ST_UCHAR ctrlByte)
  {
  if(!(ctrlByte & 0x01))
    return(INFO_CODE_GET(ctrlByte));
  else if((ctrlByte & 0x03) == 0x03)
    return(UNUM_CODE_GET(ctrlByte));
  else 
    return(SUPR_CODE_GET(ctrlByte));
  }
/************************************************************************/
/*			    isInfoQueuedUp				*/
/************************************************************************/
ST_BOOLEAN isInfoQueuedUp(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_REQ *rptr;	 	/* request pointer			*/
ST_BOOLEAN ret = SD_FALSE;

  rptr = pptr->txAdlcReqList;
  while (rptr && ret == SD_FALSE)
    {
    if (rptr->cptr == cptr)
      ret = SD_TRUE;
    rptr = (ADLC_REQ *) list_get_next (pptr->txAdlcReqList,rptr);
    }
  return (ret);
  }
/************************************************************************/
/*			    isPollQueuedUp				*/
/************************************************************************/
ST_BOOLEAN isPollQueuedUp(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;
ST_BOOLEAN ret = SD_FALSE;

  fptr = pptr->txSFramesList;
  while (fptr && ret == SD_FALSE)
    {
    if (fptr->cptr == cptr)
      ret = SD_TRUE;
    fptr = (ADLC_FRAME *) list_get_next (pptr->txSFramesList,fptr);
    }
  return (ret);
  }
/************************************************************************/
/*			    isUFrameQueuedUp				*/
/************************************************************************/
ST_BOOLEAN isUFrameQueuedUp(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;
ST_BOOLEAN ret = SD_FALSE;

  fptr = pptr->txUFramesList;
  while (fptr && ret == SD_FALSE)
    {
    if (fptr->cptr == cptr)
      ret = SD_TRUE;
    fptr = (ADLC_FRAME *) list_get_next (pptr->txUFramesList,fptr);
    }
  return (ret);
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN isConStateInInfoTransfer(ADLC_CON_INFO *cptr)
  {
ST_BOOLEAN ret;

  switch(cptr->state)
    {
    case ADLC_S_NORMAL :
    case ADLC_S_REMOTE_BUSY :
      ret = SD_TRUE;
    break;

    default :
      ret = SD_FALSE;
    break;
    }
  return (ret);
  }

/************************************************************************/
/*			isOkToPoll					*/
/************************************************************************/
ST_BOOLEAN isOkToPoll(ADLC_CON_INFO *cptr)
  {
  if ((isConStateInInfoTransfer(cptr) == SD_TRUE) && (isPollQueuedUp(cptr)) == SD_FALSE)
    return(SD_TRUE);
  return(SD_FALSE);
  }
/************************************************************************/
/*			isOkToSendInfo					*/
/* Note1 : 3rd Condition is put because we do not want to send INFO 	*/
/* in case of retransmits						*/
/************************************************************************/
ST_BOOLEAN isOkToSendInfo(ADLC_CON_INFO *cptr,ST_BOOLEAN justCheck)
  {
#if defined(ADLC_MASTER)
  if ((cptr->txRxMode == TX_MODE) && (cptr->dptr->txRxMode == TX_MODE) 
      && (cptr->state == ADLC_S_NORMAL) && (cptr->retries == 0))
    return (SD_TRUE);
  return (SD_FALSE); 
#endif /* ADLC_MASTER */
#if defined(ADLC_SLAVE)
  if (ADLC_RUN_MODE(cptr->pptr) == CONTENTION_MODE || justCheck)
    {
    if (cptr->state == ADLC_S_NORMAL)
      return (SD_TRUE);
    return (SD_FALSE);
    }
  else
    {
    if ((cptr->dptr->txRxMode == TX_MODE) && (cptr->txRxMode == TX_MODE) && (cptr->state == ADLC_S_NORMAL) && (cptr->retries == 0))
      return(SD_TRUE);
    return (SD_FALSE);
    }
#endif /* ADLC_SLAVE */
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN isOkToSendSupr(ADLC_CON_INFO *cptr,ST_BOOLEAN justCheck)
  {
#if defined(ADLC_MASTER)
  if (cptr->txRxMode == TX_MODE && cptr->dptr->txRxMode == TX_MODE)
    return(SD_TRUE);
  return(SD_FALSE);
#endif /* ADLC_MASTER */
#if defined(ADLC_SLAVE)
  if (ADLC_RUN_MODE(cptr->pptr) == CONTENTION_MODE || justCheck)
    {
    return (SD_TRUE);
    }
  else
    {
    if (cptr->dptr->txRxMode == TX_MODE && cptr->txRxMode == TX_MODE)
      return(SD_TRUE);
    return(SD_FALSE);
    }
#endif /* ADLC_SLAVE */
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN isOkToSendUnum(ADLC_CON_INFO *cptr,ST_BOOLEAN justCheck)
  {
  return isOkToSendSupr(cptr,justCheck);
  }
/************************************************************************/

/************************************************************************/
ST_BOOLEAN isOkToSendMult(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck)
  {
#if defined(ADLC_SLAVE)
  return(SD_TRUE); 
  }
#endif
#if defined(ADLC_MASTER)
ADLC_DEV_INFO *dptr;
ST_BOOLEAN ret = SD_TRUE;

  dptr = pptr->devList;
  while (dptr && ret == SD_TRUE)
    {
    if (dptr->txRxMode == RX_MODE)
      ret = SD_FALSE;
    else
      dptr = (ADLC_DEV_INFO *) list_get_next (pptr->devList,dptr);
    }
  return (ret);
  }
#endif /* ADLC_MASTER */

/************************************************************************/
/*		                  getPfAndEosBit			*/
/************************************************************************/
ST_VOID getPfAndEosBit(ADLC_FRAME *nextFptr,ST_UCHAR sendCount,
		    ST_BOOLEAN *pfBit,ST_BOOLEAN *eosBit, ADLC_PORT_INFO *pptr)
  {
  if (nextFptr == NULL) /* is end of segment ?	*/
    *eosBit = 0;
  else
    *eosBit = 1;			

/* P/F Bit will be set if we have reached the maximum outstanding number*/
/* without ack or it is end of segmet					*/

  if ((sendCount == (ST_UCHAR) (ADLC_MODULUS(pptr) - 1)) || (*eosBit == 0))
    *pfBit = 1;
  else
    *pfBit = 0;
  }
/************************************************************************/
/* 			adlc_except					*/
/* This function should never be called.It indicates a programming error*/
/************************************************************************/
ST_VOID adlc_except (SD_CONST ST_CHAR *file, ST_INT line)
  {
  ADLC_LOG_ERR2("ADLC ERROR Program exception.  File = %s, Line = %d.", file, line);
  }

/************************************************************************/
/* 				resetConnection				*/
/* This functions basically frees all the resources associated with	*/
/* this connection and resets all the structure members			*/
/************************************************************************/

ST_VOID resetConnection(ADLC_CON_INFO *cptr,ST_INT attrib)
  {
#if defined(ADLC_LM)
ST_INT stackTaskId;
ST_UINT16 localAddr;
#endif
ST_BOOLEAN bDeleteConnection;

/* unlink and free all the buffers associated with this connection	*/
  deleteAllFrames(cptr);

#if defined(ADLC_LM)
  if (getTaskId(cptr,&stackTaskId) != SD_SUCCESS)
    bDeleteConnection = SD_TRUE;
  else 
    {
    switch (stackTaskId)
      {
      case RLI_TASK_ID_COGW :
      case RLI_TASK_ID_TRIM7 :
        bDeleteConnection = SD_FALSE;
      break;   
      default :	
        bDeleteConnection = SD_TRUE;
      break;
      }
    }
#endif

#if !defined(ADLC_LM)
  if (stackTaskId == RLI_TASK_ID_RSGW)
    bDeleteConnection = SD_TRUE;
  else
    bDeleteConnection = SD_FALSE;
#endif

/* Either delete the connection or reset the structure members		*/
  if (attrib == ADLC_DELETE_CON && bDeleteConnection == SD_TRUE)
    {
#if defined(ADLC_LM)
    localAddr = getLocalAddr(cptr);
    deleteTaskFromLocalAddress(localAddr);
#endif
    deleteConnection(cptr);
    }
  else 
    {
    resetStateVarsForConnection(cptr);	
    cptr->state = ADLC_S_DISCONNECT_PHASE;	
#if defined(ADLC_MASTER)
    startConRetryTimeout(cptr);
#endif
    }
  }

/************************************************************************/

/************************************************************************/
ST_VOID resetStateVarsForConnection(ADLC_CON_INFO *cptr)
  {
  cptr->vR = 0;			
  cptr->vS = 0;			
  cptr->infoBufLen = 0;		
  cptr->sendCount = 0;		
  }
/************************************************************************/
/*		                 verifyCrc				*/
/* This function verifies the crc in the given frame			*/
/* The frame is assumed to be a buffer from SOF until CRC		*/
/* CRC is measured from frame[1] to frame[len - 2] where 2 bytes are of	*/
/* of CRC itself							*/
/************************************************************************/
ST_RET verifyCrc(ST_UCHAR *frame, ST_UINT len)
  {
ST_UINT16 crc_received = 0;	
ST_UINT16 crc;			
ST_UINT crcLen;
ST_UINT crcOffset;

/*  crcOffset = len - (CRC_FIELD_LEN + EOF_FIELD_LEN);			*/
  crcOffset = len - CRC_FIELD_LEN;
  crc_received = (frame[crcOffset] << 8) | (frame[crcOffset + 1]);

/*  crcLen = len - (SOF_FIELD_LEN + CRC_FIELD_LEN + EOF_FIELD_LEN);	*/
  crcLen = len - (SOF_FIELD_LEN + CRC_FIELD_LEN);
  crc = calcCrc(&frame[SOF_FIELD_LEN],crcLen); 

  if (crc != crc_received)
    return (SD_FAILURE);
  return(SD_SUCCESS);
  }
/************************************************************************/
/*                             calcCrc					*/
/* This function calculates the crc in the given frame			*/
/************************************************************************/
ST_UINT16 calcCrc (ST_UCHAR *data, ST_UINT length)
  {
ST_UINT16 crc = (ST_UINT16)~0;
register ST_UINT i;

  for (i = 0; i < length; i++)
    crc = updateCrc (data[i],crc);
  crc = ~crc;
  ROTATE(crc);
  return(crc);
  }
/************************************************************************/
/*                             	updateCrc				*/
/* This function calculates the CRC-16 for an individual byte.		*/
/* The current CRC value is passed in.  The new CRC value is returned.	*/
/* WARNING:								*/
/* The CRC must be initialized before calling this function with the	*/
/* first byte of data. 							*/
/************************************************************************/
static ST_UINT16 updateCrc (ST_INT16 c, ST_UINT16 crc)
 {
ST_INT16 index;
  index = (crc ^ c) & 0x00FF;
  crc = ( (crc >> 8) & 0x00FF) ^ adlc_crc_table [index];
  return(crc);
  }

/************************************************************************/
/*			getPortPtrFromPort				*/
/* This function gets the port pointer from the passed port		*/
/************************************************************************/

ADLC_PORT_INFO *getPortPtrFromPort(ST_INT port)
  {
ADLC_PORT_INFO *pptr = NULL;

  pptr = portList;
  while (pptr)
    {
    if (pptr->port == port)
      break;
    pptr = (ADLC_PORT_INFO *) list_get_next (portList,pptr);
    }
  return (pptr);
  }

/************************************************************************/
/* 			getDevPtrFromAddress				*/
/* This function gets the device pointer from the passed addr		*/
/************************************************************************/

ADLC_DEV_INFO *getDevPtrFromAddress(ST_UINT16 addr)
  {
ADLC_PORT_INFO *pptr = portList;
ADLC_DEV_INFO *dptr = NULL;
ST_BOOLEAN done = SD_FALSE;

  while (pptr && done == SD_FALSE)
    {
    dptr = pptr->devList;
    while (dptr && done == SD_FALSE)
      {
      if (dptr->address == addr)
        done = SD_TRUE;
      else
        dptr = (ADLC_DEV_INFO *) list_get_next (pptr->devList,dptr);
      }
    pptr = (ADLC_PORT_INFO *) list_get_next (portList,pptr);
    }
  return (dptr);
  }

/************************************************************************/
/* 			getDevPtrFromName				*/
/* This function gets the device pointer from the passed name		*/
/************************************************************************/

ADLC_DEV_INFO *getDevPtrFromName(ST_CHAR *name)
  {
ADLC_PORT_INFO *pptr = portList;
ADLC_DEV_INFO *dptr = NULL;
ST_BOOLEAN done = SD_FALSE;

  if (!name || (strlen(name) == 0) )
    return (NULL);

  while (pptr && done == SD_FALSE)
    {
    dptr = pptr->devList;
    while (dptr && done == SD_FALSE)
      {
      if (!strcmp(dptr->name,name) )
        done = SD_TRUE;
      else
        dptr = (ADLC_DEV_INFO *) list_get_next (pptr->devList,dptr);
      }
    pptr = (ADLC_PORT_INFO *) list_get_next (portList,pptr);
    }
  return (dptr);
  }

/************************************************************************/
/*			     getConPtrFromDptr				*/
/* can not pass dptr = NULL						*/
/************************************************************************/
ADLC_CON_INFO *getConPtrFromDptr(ADLC_DEV_INFO *dptr,ST_UINT16 masterAddr)
  {
ADLC_CON_INFO *cptr = NULL;

  cptr = dptr->conList;
  while (cptr)
    {
    if (cptr->masterAddr == masterAddr)
      break;
    cptr = (ADLC_CON_INFO *) list_get_next(dptr->conList,cptr);
    }	
  return(cptr);
  }
/************************************************************************/
/*                            getConPtrFromAddrPair			*/
/************************************************************************/
ADLC_CON_INFO *getConPtrFromAddrPair(ST_UINT16 slaveAddr,ST_UINT16 masterAddr)
  {
ADLC_DEV_INFO *dptr = NULL;
ADLC_CON_INFO *cptr = NULL;

  if ((dptr = getDevPtrFromAddress(slaveAddr)) != NULL)
    cptr = getConPtrFromDptr(dptr,masterAddr);

  return(cptr);
  }
/************************************************************************/
/*			addToPortList					*/
/* This function allocates pptr, fills it and adds to portList		*/
/************************************************************************/
ADLC_PORT_INFO *addToPortList(ST_INT port, const ADLC_CFG_INFO *pstAdlcCfg)
  {
ADLC_PORT_INFO *pptr;

  pptr = (ADLC_PORT_INFO *) ipcChkCalloc(1,sizeof(ADLC_PORT_INFO));
  memcpy(&pptr->adlcCfg, pstAdlcCfg, sizeof(ADLC_CFG_INFO));
  pptr->rxBuf = (ST_UCHAR *) ipcChkCalloc(1,ADLC_FRAME_LEN(pptr));
  pptr->port = port;
#if defined(ADLC_MASTER)
  pptr->txRxMode = TX_MODE; 	/* we can transmit for this device	*/
#endif 
#if defined(ADLC_SLAVE)
  pptr->txRxMode = RX_MODE;
#endif
  pptr->runMode = ADLC_RUN_MODE(pptr);
  list_add_last ((ST_VOID **) &portList,pptr);
  return(pptr);
  }

/************************************************************************/
/*			addToDevList					*/
/* This function allocates dptr, fills it and adds to pptr->devList	*/
/************************************************************************/
ADLC_DEV_INFO *addToDevList(ADLC_PORT_INFO *pptr,ST_CHAR *name,
			    ST_UINT16 address,
			    ST_INT pollGroup,
			    ST_BOOLEAN connectAll)
  {
ADLC_DEV_INFO *dptr;
#if defined(ADLC_MASTER)
POLLED_DEV *pstPollDev = NULL;
POLL_GROUP *pstPollGrp = NULL;
#endif /* ADLC_MASTER */

  dptr = (ADLC_DEV_INFO *) ipcChkCalloc(1,sizeof(ADLC_DEV_INFO));
  dptr->pptr = pptr;
  dptr->address = address;
  strcpy(dptr->name,name);

#if defined(ADLC_MASTER)
  dptr->txRxMode = TX_MODE; 	/* we can transmit for this device	*/
  dptr->connectAll = connectAll;
  addDevToPollGrp(dptr,pollGroup);
#endif 

#if defined(ADLC_SLAVE)
  dptr->txRxMode = RX_MODE; 	/* we can't transmit for this device	*/
#endif

  list_add_last ((ST_VOID **) &pptr->devList,dptr);
  return (dptr);
  }
/************************************************************************/
/*			addToConList					*/
/* This function allocates cptr, fills it and adds to dptr->conList	*/
/************************************************************************/
ADLC_CON_INFO *addToConList(ADLC_DEV_INFO *dptr,ST_UINT16 masterAddr)
  {
ADLC_CON_INFO *cptr;

  cptr = (ADLC_CON_INFO *) ipcChkCalloc(1,sizeof(ADLC_CON_INFO));
  cptr->masterAddr = masterAddr;
  cptr->dptr = dptr;
  cptr->pptr = dptr->pptr;
  cptr->txRxMode = dptr->txRxMode;
  cptr->state = ADLC_S_DISCONNECT_PHASE; 	
/* add this connection pointer to device list	*/
  list_add_last((ST_VOID **) &dptr->conList,cptr); 
  return (cptr);
  }

/************************************************************************/
/* This function allocates rptr,fills it and adds to pptr->txAdlcReqList*/
/************************************************************************/
ADLC_REQ *addToReqList(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_REQ *rptr;

  rptr = (ADLC_REQ *)ipcChkCalloc(1,sizeof(ADLC_REQ));
  cptr->txAdlcReqCount ++;
  rptr->cptr = cptr;
  list_add_last ((ST_VOID **)  &pptr->txAdlcReqList,rptr);
  return (rptr);
  }

/************************************************************************/
/*				addToSFramesList			*/
/* This function allocates fptr, fills it and adds a Supervisory Frame  */
/* transmission								*/
/************************************************************************/
ST_VOID addToSFramesList(ADLC_CON_INFO *cptr,ST_UCHAR cmd)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;	/* frame pointer		*/
#if defined(ADLC_LM)
ST_INT retCode;
#endif
  fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
  fptr->frameLen = MIN_ADLC_FRAME_LEN;
  fptr->cptr = cptr;
  fptr->cmd = cmd;
#if defined(ADLC_LM)
  fptr->addToListTime = sGetMsTime();
#endif
  list_add_last((ST_VOID **) &pptr->txSFramesList,fptr);
#if defined(ADLC_LM)
  if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
    logIpcError(retCode,"FrameInList Semaphore Post Failed",thisFileName,__LINE__);
#endif
  }

/************************************************************************/
/*			addToUFramesList				*/
/* This function allocates and adds an Unnumbered Frame for transmission*/
/************************************************************************/
ST_VOID addToUFramesList(ADLC_CON_INFO *cptr,ST_UCHAR cmd)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;	/* frame pointer		*/
#if defined(ADLC_LM)
ST_INT retCode;
#endif

  fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
  fptr->frameLen = MIN_ADLC_FRAME_LEN;
  fptr->cptr = cptr;
  fptr->cmd = cmd;
#if defined(ADLC_LM)
  fptr->addToListTime = sGetMsTime();
#endif
  list_add_last((ST_VOID **) &pptr->txUFramesList,fptr);
#if defined(ADLC_LM)
  if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
    logIpcError(retCode,"FrameInList Post Semaphore Failed",thisFileName,__LINE__);
#endif
  }
#if defined(ADLC_MASTER)
/************************************************************************/
/*			addToRFramesList				*/
/* This function allocates and adds a Redirect Frame for transmission	*/
/************************************************************************/
ST_VOID addToRFramesList(ADLC_PORT_INFO *pptr)
  {
ADLC_FRAME *fptr;
#if defined(ADLC_LM)
ST_INT retCode;
#endif

  fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
  fptr->u.redrBuf = (ST_UCHAR *) ipcChkCalloc(1,pptr->rxLen);
  memcpy(fptr->u.redrBuf,pptr->rxBuf,pptr->rxLen);
  fptr->frameLen = pptr->rxLen;
  list_add_last((ST_VOID **) &pptr->txRFramesList,fptr);
#if defined(ADLC_LM)
  if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
    logIpcError(retCode,"FrameInList Post Semaphore Failed",thisFileName,__LINE__);
#endif
  }
#endif /* ADLC_MASTER */
/************************************************************************/
/* 			addToMFramesInTxList				*/
/* The reason that we encode the whole frame here instead of doing	*/
/* later like we do in other kinds of frames that for these special 	*/
/* addresses we don't have connection					*/
/* So we can not extract src and dst addresses there			*/
/* The other reason is that because we do not encode vR and vS so the 	*/
/* encoded message will not be changed from here until we really 	*/
/* transmit it 								*/
/* If we have already queued on multicast frame we do not need to do	*/
/* again because all multicast frames are same				*/
/************************************************************************/
ST_VOID addToMFramesInTxList(ST_UINT16 srcAddr,ST_UINT16 dstAddr, 
			 ST_VOID *data, ST_INT dataSize)
  {
ADLC_PORT_INFO *pptr;
ADLC_PORT_INFO *nextPptr;
ADLC_FRAME *fptr;
ST_UINT16 frameFormat;
ST_BOOLEAN eosBit;
ST_UCHAR ctrlByte;
ST_UCHAR vS;
ST_UCHAR vR;
ST_INT frameLen;
ST_INT dataLen;
ST_INT totalLen;
ST_CHAR *pData;
#if defined(ADLC_LM)
ST_INT retCode;
#endif

/* add this multicast message to all ports			*/
  pptr = portList;
  while (pptr)
    {
    nextPptr = (ADLC_PORT_INFO *) list_get_next(portList,pptr);
    if (list_get_sizeof(pptr->txMFramesList) > ADLC_MAX_ADLC_REQS(pptr) * 2) 
      {
      pptr = nextPptr;
      continue;
      }

/* initialize some local variable				*/
    totalLen = dataSize;
    pData = (ST_CHAR *) data;
    vS = 0;
    vR = 0;
    eosBit = 1;		  /* by default not end of segment	*/
    ctrlByte = 0x00;

/* Attach smaller ADLC Info frames of this large LSDU		*/
    while (totalLen > 0)
      {
      if (totalLen > ADLC_INFO_LEN(pptr))
	{
        dataLen = ADLC_INFO_LEN(pptr);
	}
      else
	{
        dataLen = totalLen;
	eosBit = 0;
 	}

      frameLen = dataLen + MIN_INFO_FRAME_LEN;
      /* Prepare Frame Format						*/
      frameFormat = getFrameFormat(frameLen,eosBit,0x9);

      /* Prepare Control Byte							*/
      ctrlByte = INFO;
      INFO_NR_SET(ctrlByte,vR);
      INFO_NS_SET(ctrlByte,vS);

      fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
      fptr->u.infoBuf = (ST_UCHAR *) ipcChkCalloc(1,frameLen);
      fptr->infoLen = dataLen;
      fptr->frameLen = frameLen;
      memcpy(&fptr->u.infoBuf[INFO_FIELD_OFFSET],pData,dataLen);

      /* All the needed information is ready so do encoding			*/
      enc_adlc_frame(fptr->u.infoBuf,srcAddr,dstAddr,ctrlByte,frameFormat,dataLen,pptr);

      fptr->status = STAT_STANDBY;
      list_add_last ((ST_VOID **) &pptr->txMFramesList,fptr);
      pData += dataLen;		/* increment data pointer	*/
      vS = INC_MODULO_N(vS,1,ADLC_MODULUS(pptr));
      totalLen -= dataLen;		/* decrement data length	*/
      }
#if defined(ADLC_LM)
    if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
      logIpcError(retCode,"FrameInList Post Semaphore Failed",thisFileName,__LINE__);
#endif
    pptr = nextPptr;
    }
  }
/************************************************************************/
/*			addToMFramesInRxList				*/
/* This function allocates and adds an Info Frame for receiving		*/
/* The len passed in argument is INFO len, so we allocate the 		*/
/* buffer of the frame to store this INFO field				*/
/************************************************************************/
ST_VOID addToMFramesInRxList(ADLC_PORT_INFO *pptr,ST_UCHAR *data,ST_INT dataLen)
  {
ADLC_FRAME *fptr;

  fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
  fptr->u.infoBuf = (ST_UCHAR *) ipcChkCalloc(1,dataLen);
  fptr->infoLen = dataLen;
  fptr->frameLen = dataLen;
  memcpy(fptr->u.infoBuf,data,dataLen);
  pptr->infoBufLen += fptr->infoLen;
  list_add_last ((ST_VOID **)  &pptr->rxMFramesList,fptr);
  }
/************************************************************************/
/*			addToIFramesInTxList				*/
/* This function allocates and adds an Info Frame for transmission	*/
/* The len passed in argument is just INFO len, so we allocate the 	*/
/* buffer of the frame to store this INFO plus non info bytes		*/
/************************************************************************/
ST_VOID addToIFramesInTxList(ADLC_REQ *rptr,ST_UCHAR *data,ST_INT dataLen)
  {
ADLC_CON_INFO *cptr = rptr->cptr;
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;
#if defined(ADLC_LM)
ST_RET retCode;
#endif

  fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
  fptr->u.infoBuf = (ST_UCHAR *) ipcChkCalloc(1,dataLen + MIN_INFO_FRAME_LEN);
  fptr->infoLen = dataLen;
  fptr->frameLen = dataLen + MIN_INFO_FRAME_LEN;
  memcpy(&fptr->u.infoBuf[INFO_FIELD_OFFSET],data,dataLen);
  fptr->cptr = cptr;
  fptr->status = STAT_STANDBY;
#if defined(ADLC_LM)
  fptr->addToListTime = sGetMsTime();
#endif
  list_add_last ((ST_VOID **) &rptr->txIFramesList,fptr);
#if defined(ADLC_LM)
  if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
    logIpcError(retCode,"FrameInList Post Semaphore Failed",thisFileName,__LINE__);
#endif
  }	
/************************************************************************/
/*			addToIFramesInRxList				*/
/* This function allocates and adds an Info Frame for receiving		*/
/* The len passed in argument is INFO len, so we allocate the 		*/
/* buffer of the frame to store this INFO field				*/
/************************************************************************/
ST_VOID addToIFramesInRxList(ADLC_CON_INFO *cptr,ST_UCHAR *data,ST_INT dataLen)
  {
ADLC_FRAME *fptr;

  fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
  fptr->u.infoBuf = (ST_UCHAR *) ipcChkCalloc(1,dataLen);
  fptr->infoLen = dataLen;
  fptr->frameLen = dataLen;
  memcpy(fptr->u.infoBuf,data,dataLen);
  cptr->infoBufLen += fptr->infoLen;
  fptr->cptr = cptr;
  list_add_last ((ST_VOID **)  &cptr->rxIFramesList,fptr);
  }
/************************************************************************/
/*                             deleteAllConnections			*/
/************************************************************************/
ST_VOID deleteAllConnections(ADLC_DEV_INFO *dptr)
  {
ADLC_CON_INFO *cptr;

  while (dptr->conList)
    {
    cptr = (ADLC_CON_INFO *) list_get_first((ST_VOID **) &dptr->conList);
    deleteAllFrames(cptr);
    ipcChkFree(cptr);
    }
  }
/************************************************************************/
/*                           deleteConnection				*/
/************************************************************************/
ST_VOID deleteConnection(ADLC_CON_INFO *cptr)
  {
ADLC_DEV_INFO *dptr = cptr->dptr;
  list_unlink((ST_VOID **) &dptr->conList,cptr); 
  ipcChkFree(cptr);
  }
/************************************************************************/
/*                                deleteDevice				*/
/************************************************************************/
ST_VOID deleteDevice(ADLC_DEV_INFO *dptr)
  {
ADLC_PORT_INFO *pptr = dptr->pptr;
  list_unlink((ST_VOID **)&pptr->devList,dptr);
  ipcChkFree (dptr);
  }
/************************************************************************/
/*			deleteAllFrames					*/
/* unlink and free all the buffers associated with the connection	*/
/* including S,U,I transmit frames (linked with the pptr) and I 	*/
/* receiving frame (linked with cptr)					*/
/************************************************************************/
ST_VOID deleteAllFrames(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;

/* if this connection was outstanding for the port do cleanup	*/
  if (pptr->outstandingCptr == cptr)
    {	
    pptr->outstandingCptr = NULL;
#if defined(ADLC_MASTER)    
    stopRespTimeout(pptr);
#endif    
    }
	
/* unlink frames for that cptr from SFamesList			*/
  deleteSFramesList(cptr);

/* unlink frames for that cptr from UFamesList			*/
  deleteUFramesList(cptr);

/* unlink frames for that cptr from IFamesList			*/
  deleteIFramesFromTxList(cptr);

/* unlink frames for that cptr from rxIFamesList			*/
  deleteIFramesFromRxList(cptr);

#if defined(UCA_SMP)
/* unlink frames for that cptr from UIFamesList			*/
  deleteUIFramesFromTxList(cptr);

/* unlink frames for that cptr from rxUIFamesList			*/
  deleteUIFramesFromRxList(cptr);
#endif
  }
/************************************************************************/
/* 			deleteSFramesList				*/
/* This function goes through the whole list of Supervisory Frames	*/
/* searches for our connection then unlinks and frees that frame	*/
/************************************************************************/
ST_VOID deleteSFramesList(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;	/* frame pointer		*/
ADLC_FRAME *nextFptr;

  fptr = pptr->txSFramesList;
  while (fptr)
    {
    nextFptr = (ADLC_FRAME *) list_get_next (pptr->txSFramesList,fptr);
    if (cptr != fptr->cptr)	/* go to next frame		*/
      {
      fptr = nextFptr;
      continue;
      }
    list_unlink((ST_VOID **) &pptr->txSFramesList,fptr); /* we found the frame	*/
    ipcChkFree(fptr);
    fptr = nextFptr;
    }
  }
/************************************************************************/
/* 		deleteSFramesListWithoutPf				*/
/* This function goes through the whole list of Supervisory Frames	*/
/* searches for our connection then unlinks and frees that frame	*/
/************************************************************************/
ST_VOID deleteSFramesListWithoutPf(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;	/* frame pointer		*/
ADLC_FRAME *nextFptr;

  fptr = pptr->txSFramesList;
  while (fptr)
    {
    nextFptr = (ADLC_FRAME *) list_get_next (pptr->txSFramesList,fptr);
    if (cptr != fptr->cptr)	/* go to next frame		*/
      {
      fptr = nextFptr;
      continue;
      }
    if (PF_GET(fptr->cmd) == SD_FALSE)
      {
      list_unlink((ST_VOID **) &pptr->txSFramesList,fptr); /* we found the frame	*/
      ipcChkFree(fptr);
      }	
    fptr = nextFptr;
    }
  }
/************************************************************************/
/* 			deleteUFramesList				*/
/* This function goes through the whole list of Unnumbered Frames	*/
/* searches for our connection then unlinks and frees that frame	*/
/************************************************************************/
ST_VOID deleteUFramesList(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;	/* frame pointer		*/
ADLC_FRAME *nextFptr;

  fptr = pptr->txUFramesList;
  while (fptr)
    {
    nextFptr = (ADLC_FRAME *) list_get_next (pptr->txUFramesList,fptr);
    if (cptr != fptr->cptr)
      {
      fptr = nextFptr;
      continue;
      }
    list_unlink((ST_VOID **) &pptr->txUFramesList,fptr);
    ipcChkFree(fptr);
    fptr = nextFptr;
    }
  }

/************************************************************************/
/* 			deleteIFramesFromRxList				*/
/************************************************************************/
ST_VOID deleteIFramesFromRxList(ADLC_CON_INFO *cptr)
  {
ADLC_FRAME *fptr;	/* frame pointer		*/

/* unlink frames for that cptr from rxIFamesList			*/
  while (cptr->rxIFramesList)
    {
    fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &cptr->rxIFramesList);
    ipcChkFree (fptr->u.infoBuf);
    ipcChkFree (fptr);	
    }
  cptr->infoBufLen = 0;
  }
/************************************************************************/
/*		deleteIFramesFromTxList				*/
/* This function unlinks and frees all the frames associated with the 	*/
/* particular device in the transmission list of Info Frames		*/
/************************************************************************/
ST_VOID deleteIFramesFromTxList(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_REQ *rptr;	 	/* request pointer			*/
ADLC_REQ *nextRptr;
ADLC_FRAME *fptr;	/* frame pointer			*/

  rptr = pptr->txAdlcReqList;
  while (rptr)
    {
    nextRptr = (ADLC_REQ *) list_get_next (pptr->txAdlcReqList,rptr);
    if (rptr->cptr != cptr)	
      {				/* go to next request		*/
      rptr = nextRptr;
      continue;
      }
    while (rptr->txIFramesList)
      {
      fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &rptr->txIFramesList);
      ipcChkFree (fptr->u.infoBuf);
      ipcChkFree (fptr);	
      }
    deleteReqFrame(rptr);	/* all done with this rptr			*/
    rptr = nextRptr;
    }
  }

/************************************************************************/
/*			deleteAckedIFramesFromTxList			*/
/* we have to acknowledge the frame upto nR				*/
/* Precaution : this function assumes that nR is valid			*/
/************************************************************************/
ST_VOID deleteAckedIFramesFromTxList(ADLC_CON_INFO *cptr,ST_UCHAR nR)
  {
ST_INT framesSent;
ST_INT framesNacked;
ST_INT framesAcked;
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_REQ *rptr;	 	/* request pointer			*/
ADLC_REQ *nextRptr;
ADLC_FRAME *fptr;	/* frame pointer			*/
ADLC_FRAME *nextFptr;
ST_UCHAR vS = cptr->vS;

  framesSent = (ST_INT) cptr->sendCount;
  framesNacked = (ST_INT) DIFF_MODULO_N(vS,nR,ADLC_MODULUS(pptr));
  framesAcked = framesSent - framesNacked;

  rptr = pptr->txAdlcReqList;
  while (rptr && framesSent > 0)
    {
    nextRptr = (ADLC_REQ *) list_get_next (pptr->txAdlcReqList,rptr);
    if (rptr->cptr != cptr)
      {
      rptr = nextRptr;
      continue;
      }
    fptr = rptr->txIFramesList;
    while (fptr && framesSent > 0)
      {
      nextFptr = (ADLC_FRAME *) list_get_next (rptr->txIFramesList,fptr);
      if (framesAcked > 0)
        {
	/* fptr -> status = STAT_ACKED	*/
    	list_unlink((ST_VOID **) &rptr->txIFramesList,fptr);
        ipcChkFree (fptr->u.infoBuf);
        ipcChkFree (fptr);	
        framesAcked --;
        }
      else if (framesNacked > 0)
        {
        fptr -> status = STAT_NACKED;	
        framesNacked --;
        }
      framesSent --;
      fptr = nextFptr;
      }
    if (rptr->txIFramesList == NULL) /* all done with this rpt delete it*/
      {
#if defined(ADLC_LM) /* right now only Link Manager gives write done event */
      giveIndicationToStack(cptr,ADLC_CMD_WRITE_PACKET_DONE,SD_SUCCESS, rptr->usr1);
#endif
      deleteReqFrame(rptr);		
      }
    rptr = nextRptr;
    }
  cptr->vS = nR;
  cptr->sendCount = 0;
  }

/************************************************************************/
/*                         deleteMFramesFromTxList			*/
/* Will delete all the Multicast Frames from transmission List		*/
/************************************************************************/
ST_VOID deleteMFramesFromTxList(ADLC_PORT_INFO *pptr)
  {
ADLC_FRAME *fptr;	/* frame pointer		*/

/* unlink frames for that pptr from txMFamesList			*/
  while (pptr->txMFramesList)
    {
    fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &pptr->txMFramesList);
    ipcChkFree (fptr->u.infoBuf);
    ipcChkFree (fptr);	
    }
  }

/************************************************************************/
/*                         deleteMFramesFromRxList			*/
/************************************************************************/
ST_VOID deleteMFramesFromRxList(ADLC_PORT_INFO *pptr)
  {
ADLC_FRAME *fptr;	/* frame pointer		*/

/* unlink frames for that pptr from rxMFamesList			*/
  while (pptr->rxMFramesList)
    {
    fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &pptr->rxMFramesList);
    ipcChkFree (fptr->u.infoBuf);
    ipcChkFree (fptr);	
    }
  pptr->infoBufLen = 0;
  }

/************************************************************************/

/************************************************************************/
ST_VOID deleteFrame(ADLC_FRAME *fptr,ADLC_FRAME **framesList)
  {
  list_unlink((ST_VOID **) framesList,fptr);
  ipcChkFree(fptr);
  }

/************************************************************************/
/*                             deleteReqFrame				*/
/************************************************************************/
ST_VOID deleteReqFrame(ADLC_REQ *rptr)
  {
ADLC_PORT_INFO *pptr = rptr->cptr->pptr;

  rptr->cptr->txAdlcReqCount --; /* The write request is all completed */
  list_unlink((ST_VOID **) &pptr->txAdlcReqList,rptr);
  ipcChkFree(rptr);
  }
/************************************************************************/
/* 			getDevPtrFromName				*/
/* This function gets the device pointer from the passed name		*/
/************************************************************************/
ST_BOOLEAN isDevPtrValid(ADLC_DEV_INFO *rDptr)
  {
ADLC_PORT_INFO *pptr = portList;
ADLC_DEV_INFO *dptr = NULL;
ST_BOOLEAN done = SD_FALSE;

  while (pptr && done == SD_FALSE)
    {
    dptr = pptr->devList;
    while (dptr && done == SD_FALSE)
      {
      if (dptr == rDptr)
        done = SD_TRUE;
      else
        dptr = (ADLC_DEV_INFO *) list_get_next (pptr->devList,dptr);
      }
    pptr = (ADLC_PORT_INFO *) list_get_next (portList,pptr);
    }
  return (done);
  }

/************************************************************************/
/*                         nackAllIFramesFromTxList			*/
/************************************************************************/
ST_VOID nackAllIFramesFromTxList(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_REQ *rptr;	 	/* request pointer			*/
ADLC_REQ *nextRptr;
ADLC_FRAME *fptr;	/* frame pointer			*/
ADLC_FRAME *nextFptr;
ST_INT framesSent;
ST_INT framesNacked;

  framesSent = (ST_INT) cptr->sendCount;
  framesNacked = framesSent;
  rptr = pptr->txAdlcReqList;
  while (rptr && framesNacked > 0)
    {
    nextRptr = (ADLC_REQ *) list_get_next (pptr->txAdlcReqList,rptr);
    if (rptr->cptr != cptr)
      {
      rptr = nextRptr;
      continue;
      }
    fptr = rptr->txIFramesList;
    while (fptr && framesNacked > 0)
      {
      nextFptr = (ADLC_FRAME *) list_get_next (rptr->txIFramesList,fptr);
      if (fptr->status == STAT_PROGRESS)
        {
        fptr->status = STAT_NACKED;
        framesNacked --;
	}
      fptr = nextFptr;
      }
    rptr = nextRptr;
    }

  cptr->vS = DEC_MODULO_N(cptr->vS,framesSent,ADLC_MODULUS(pptr));
  cptr->sendCount = 0;
  }

/************************************************************************/

/************************************************************************/
ST_UINT16 getLocalAddr(ADLC_CON_INFO *cptr)
  {
#if defined(ADLC_MASTER)
  return (cptr->masterAddr);
#endif
#if defined(ADLC_SLAVE)
  return (cptr->dptr->address);
#endif
  }
/************************************************************************/

/************************************************************************/
ST_UINT16 getRemoteAddr(ADLC_CON_INFO *cptr)
  {
#if defined(ADLC_MASTER)
  return (cptr->dptr->address);
#endif
#if defined(ADLC_SLAVE)
  return (cptr->masterAddr);
#endif
  }
/************************************************************************/
/*				adlcExit				*/
/* frees the memory before which was allocated at the startup time	*/
/************************************************************************/
ST_VOID adlcExit (ST_VOID)
  {
ADLC_DEV_INFO *dptr;
ADLC_PORT_INFO *pptr;
CFG_PORT   *pstCfgPort;

#if defined(ADLC_MASTER)
POLL_GROUP *pstPollGroup;
POLLED_DEV *pstPollDev;
#endif

#if defined(ADLC_LM)
ADLC_TASK_INFO *tptr;
#endif

#if !defined(ADLC_LM)
ADLC_EVENT_INFO *eptr;
#endif

#if !defined(ADLC_LM)
  stimeExit();
#endif

  while (portList)
    {
    pptr = (ADLC_PORT_INFO *) list_get_first((ST_VOID **) &portList);
    while (pptr->devList)
      {
      dptr = (ADLC_DEV_INFO *) list_get_first((ST_VOID **) &pptr->devList);
      deleteAllConnections(dptr); 
      ipcChkFree (dptr);	
      }
    deleteMFramesFromTxList(pptr);
    deleteMFramesFromRxList(pptr);
    ipcChkFree(pptr->rxBuf);
    ipcChkFree (pptr);	
    }

  while (portConfigList)	/* a global variable		*/
    {
    pstCfgPort = (CFG_PORT *) list_get_first((ST_VOID **) &portConfigList);
#if !defined(ADLC_LM)
    sm_exit(pstCfgPort->port);
#endif
    ipcChkFree(pstCfgPort);
    }
#if defined(ADLC_MASTER)
  while (pollGroupList)		/* a global variable		*/
    {
    pstPollGroup = (POLL_GROUP *) list_get_first((ST_VOID **) &pollGroupList);
    while (pstPollGroup->pollDevList)
      {
      pstPollDev = (POLLED_DEV *) list_get_first((ST_VOID **)&pstPollGroup->pollDevList);
      ipcChkFree(pstPollDev);
      }
    ipcChkFree(pstPollGroup);
    }
#endif /* ADLC_MASTER */

#if !defined(ADLC_LM)
  while (eventList)  		/* a global variable		*/
    {
    eptr = (ADLC_EVENT_INFO *) list_get_first((ST_VOID **) &eventList);
    ipcChkFree(eptr);
    }
#endif
#if defined(ADLC_LM)
  while (taskList)  		/* a global variable		*/
    {
    tptr = list_get_first((ST_VOID **) &taskList);
    ipcChkFree(tptr);
    }
#endif /* ADLC_LM */
  }
/************************************************************************/
/*			adlcEndLogging					*/
/************************************************************************/
ST_VOID adlcEndLogging (ST_VOID)
  {
  /* un - alloc logging ctrl struct					*/
  }
#if defined(ADLC_MASTER)
/************************************************************************/
/*			addToPollGroupList				*/
/************************************************************************/
POLL_GROUP *addToPollGroupList(ST_INT pollGrpID,ST_DOUBLE pollRate)
  {
POLL_GROUP *pstPollGroup;

  pstPollGroup = (POLL_GROUP *) ipcChkCalloc(1, sizeof(POLL_GROUP));
  pstPollGroup->pollGrpID = pollGrpID; /* This is the first poll group		*/
  pstPollGroup->pollRate = pollRate;  /* will be polled every second		*/
  list_add_last((ST_VOID **) &pollGroupList, (ST_VOID *) pstPollGroup);
  return (pstPollGroup);
  }
/************************************************************************/
/*			     findPollGroup				*/
/* search list of poll groups (pollGroupList) for matching ID		*/
/************************************************************************/
POLL_GROUP *findPollGroup(ST_INT pollGrpID)
  {
POLL_GROUP	*pstPollGroup;

  pstPollGroup = pollGroupList;
  while (pstPollGroup)
    {
    if (pstPollGroup->pollGrpID == pollGrpID)
      break;
    pstPollGroup = (POLL_GROUP *) list_get_next(pollGroupList, pstPollGroup);
    }

  return (pstPollGroup);
  }
/************************************************************************/
/*			findPolledDev					*/
/* search polled devices for devName					*/
/************************************************************************/

POLLED_DEV *findPolledDevByName(char *devName)
  {
POLL_GROUP	*pstPollGrp;
POLLED_DEV	*pstPollDev = NULL;
ST_BOOLEAN		bFound = SD_FALSE;

  if (!devName || (strlen(devName) == 0) )
    return (NULL);

  pstPollGrp = pollGroupList;
  while (pstPollGrp && !bFound)
    {
    pstPollDev = pstPollGrp->pollDevList;
    while (pstPollDev && !bFound)
      {
      if (!strcmp(pstPollDev->dptr->name, devName))
        bFound = SD_TRUE;
      else
        pstPollDev = (POLLED_DEV *) list_get_next(pstPollGrp->pollDevList, pstPollDev);
      }
    pstPollGrp = (POLL_GROUP *) list_get_next(pollGroupList, pstPollGrp);
    }

  return (pstPollDev);
  }
/************************************************************************/
/*			findPolledDev					*/
/* search polled devices for devName					*/
/************************************************************************/

POLLED_DEV *findPolledDevByAddress(ST_UINT16 addr)
  {
POLL_GROUP	*pstPollGrp;
POLLED_DEV	*pstPollDev = NULL;
ST_BOOLEAN		bFound = SD_FALSE;

  pstPollGrp = pollGroupList;
  while (pstPollGrp && !bFound)
    {
    pstPollDev = pstPollGrp->pollDevList;
    while (pstPollDev && !bFound)
      {
      if (pstPollDev->dptr->address == addr)
        bFound = SD_TRUE;
      else
        pstPollDev = (POLLED_DEV *) list_get_next(pstPollGrp->pollDevList, pstPollDev);
      }
    pstPollGrp = (POLL_GROUP *) list_get_next(pollGroupList, pstPollGrp);
    }

  return (pstPollDev);
  }
/************************************************************************/
/*			    addDevToPollGrp				*/
/************************************************************************/

ST_RET addDevToPollGrp(ADLC_DEV_INFO *dptr,ST_INT pollGroup)
  {
POLL_GROUP	*pstPollGrp;
POLLED_DEV	*pstPollDev;

  dptr->pollGroup = pollGroup;

  if (pollGroup == 0)	/* we do not want this device to be polled */
    return (SD_SUCCESS);

  if ((pstPollGrp = findPollGroup(pollGroup)) == NULL)
    return (SD_FAILURE);

  pstPollDev = pstPollGrp->pollDevList;
  /* see if it was already added to the poll group	*/
  while (pstPollDev)
    {
    if (pstPollDev->dptr == dptr)
      break;
    pstPollDev = (POLLED_DEV *) list_get_next(pstPollGrp->pollDevList, pstPollDev);
    }
  /* it wasn't already added to the poll group	*/
  if (!pstPollDev)
    {
    pstPollDev = (POLLED_DEV *) ipcChkCalloc(1, sizeof(POLLED_DEV));
    pstPollDev->dptr = dptr;
    pstPollDev->bSuspendPolling = SD_FALSE;
    list_add_last((ST_VOID **) &pstPollGrp->pollDevList, pstPollDev);
    }
  return (SD_SUCCESS);
  }
#endif /* ADLC_MASTER */

#if !defined(ADLC_LM)
/************************************************************************/
/*			addToEventList					*/
/************************************************************************/
ADLC_EVENT_INFO *addToEventList(ST_INT adlcEventOp,ST_VOID *adlcEventCb,ST_INT adlcRetCode)
  {
ADLC_EVENT_INFO *eptr;

  eptr = (ADLC_EVENT_INFO *) ipcChkCalloc(1,sizeof(ADLC_EVENT_INFO));
  eptr->adlcEventOp = adlcEventOp;
  eptr->adlcEventCb = adlcEventCb;  	
  eptr->adlcRetCode = adlcRetCode;  	
  list_add_last ((ST_VOID **) &eventList,eptr);
  return (eptr);
  }
#endif /* !ADLC_LM */
/************************************************************************/
/*			addToCfgPortList				*/
/* This func allocates pstCfgPort, fills it and adds to portConfigList	*/
/************************************************************************/
CFG_PORT *addToCfgPortList(ST_INT port)
  {
CFG_PORT *pstCfgPort;

  pstCfgPort = (CFG_PORT *) ipcChkCalloc(1, sizeof(CFG_PORT));
  pstCfgPort->port = port;
  list_add_last((ST_VOID **) &portConfigList, pstCfgPort);
  return (pstCfgPort);
  }
/************************************************************************/
/*			       findCfgPort				*/
/* search portConfigList for given port - return cfg port pointer	*/
/************************************************************************/

CFG_PORT *findCfgPort(ST_INT port)
  {
CFG_PORT *pstCfgPort;

  pstCfgPort = portConfigList;
  while (pstCfgPort)
    {
    if (pstCfgPort->port == port)
      break;
    pstCfgPort = (CFG_PORT *) list_get_next(portConfigList, pstCfgPort);
    }

  return (pstCfgPort);
  }
#if defined(ADLC_LM)
/************************************************************************/
/*			       addToTaskList				*/
/************************************************************************/
ADLC_TASK_INFO *addToTaskList(ST_UINT16 localAddr,ST_INT taskId,HQUEUE qHandle)
  {
ADLC_TASK_INFO *tptr;

  tptr = (ADLC_TASK_INFO *) ipcChkCalloc(1,sizeof(ADLC_TASK_INFO));
  tptr->localAddr = localAddr;
  tptr->taskId = taskId;  	
  tptr->qHandle = qHandle;
  list_add_last ((ST_VOID **) &taskList,tptr);
  return (tptr);
  }
/************************************************************************/
/*                          getTaskPtrFromLocalAddress			*/
/************************************************************************/
ADLC_TASK_INFO *getTaskPtrFromLocalAddress(ST_UINT16 localAddr)
  {
ADLC_TASK_INFO *tptr;

  tptr = taskList;
  while (tptr)
    {
    if (tptr->localAddr == localAddr)
      break;
    tptr = (ADLC_TASK_INFO *) list_get_next (taskList,tptr);
    }
  return(tptr);
  }
/************************************************************************/
/*                          getTaskPtrFromTaskId			*/
/************************************************************************/
ADLC_TASK_INFO *getTaskPtrFromTaskId(ST_INT taskId)
  {
ADLC_TASK_INFO *tptr;

  tptr = taskList;
  while (tptr)
    {
    if (tptr->taskId == taskId)
      break;
    tptr = (ADLC_TASK_INFO *) list_get_next (taskList,tptr);
    }
  return(tptr);
  }
/************************************************************************/
/*			deleteTask					*/
/************************************************************************/
ST_VOID deleteTask(ADLC_TASK_INFO *tptr)
  {
  list_unlink((ST_VOID **) &taskList,tptr);
  ipcChkFree(tptr);
  }
/************************************************************************/
/*			deleteTaskFromLocalAddress				*/
/************************************************************************/
ST_VOID deleteTaskFromLocalAddress(ST_UINT16 localAddr)
  {
ADLC_TASK_INFO *tptr;

  if ((tptr = getTaskPtrFromLocalAddress(localAddr)) != NULL)
    deleteTask(tptr);
  }
/************************************************************************/
/*			       addTaskForLocalAddress			*/
/* We add the task so we will be able to pass to proper task by looking	*/
/* at the local address	when we receive a full packet			*/
/* See also deleteTaskForLocalAddress which does the opposite of this 	*/
/* function								*/
/************************************************************************/
ST_RET addTaskForLocalAddress(ST_UINT16 localAddr,ST_INT taskId)
  {
ADLC_TASK_INFO *tptr;
HQUEUE qHandle = {0};

/* Check if we already have added this task				*/
  if ((tptr = getTaskPtrFromLocalAddress(localAddr)) != NULL)
     {
     if (tptr->taskId != taskId) /* make sure it is the same task	*/
     /* hey some other task is trying to use the same local address	*/
       return(E_ADLC_DUPLICATE_LOCAL_ADDR);
     }
  tptr = addToTaskList(localAddr,taskId,qHandle);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			getTaskId					*/
/* It resolves the taskId						*/
/************************************************************************/
ST_INT getTaskId(ADLC_CON_INFO *cptr,ST_INT *taskId)
  {
ADLC_TASK_INFO *tptr;
ST_UINT16 localAddr;

  if (cptr == NULL) /* this is multicast packet		*/
    {
    if (connectedTask[RLI_TASK_ID_COGW] == SD_TRUE ||
        connectedTask[RLI_TASK_ID_MCGW] == SD_TRUE ||
        connectedTask[RLI_TASK_ID_TRIM7] == SD_TRUE)
      return (SD_SUCCESS);
	
    return (SD_FAILURE);
    }

  localAddr = getLocalAddr(cptr);

  if ((tptr = getTaskPtrFromLocalAddress(localAddr)) == NULL)
    return (SD_FAILURE);

  *taskId = tptr->taskId;
  return (SD_SUCCESS);
  }
/************************************************************************/

/************************************************************************/
ST_VOID logIpcError(ST_INT retCode, ST_CHAR *str, ST_CHAR *file, ST_INT line)
  {
ST_CHAR errBuf [ERR_STR_LEN];

  ipcGetErrString(retCode, errBuf, sizeof (errBuf));
  ADLC_LOG_ERR1("ADLC ERROR: %s",str);
  ADLC_LOG_CERR2("            retCode = 0x%4.4X %s",retCode, errBuf);
  ADLC_LOG_CERR2("            File = %s, Line = %d.",file, line);
  }
/************************************************************************/
/*                           deletePort					*/
/************************************************************************/
ST_VOID deletePort(ADLC_PORT_INFO *pptr)
  {
  list_unlink(&portList,pptr); 
  ipcChkFree(pptr->rxBuf);
  ipcChkFree(pptr);
  }
/************************************************************************/
/*                           deleteCfgPort				*/
/************************************************************************/
ST_VOID deleteCfgPort(CFG_PORT *pstCfgPort)
  {
  list_unlink(&portConfigList,pstCfgPort); 
  ipcChkFree(pstCfgPort);
  }
#endif /* ADLC_LM */

#if defined(UCA_SMP)
/************************************************************************/
/* This function allocates rptr,fills it and adds to pptr->txAdlcReqList*/
/************************************************************************/
ADLC_REQ *addToExtReqList(ADLC_CON_INFO *cptr,ADLC_REQ **adlcReqList,ST_LONG userId)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_REQ *rptr;

  rptr = (ADLC_REQ *)ipcChkCalloc(1,sizeof(ADLC_REQ));
  cptr->txExtAdlcReqCount ++;
  rptr->cptr = cptr;
  rptr->userId = userId;
  list_add_last ((ST_VOID **) adlcReqList,rptr);
  return (rptr);
  }
/************************************************************************/
/*                             deleteExtReqFrame			*/
/************************************************************************/
ST_VOID deleteExtReqFrame(ADLC_REQ *rptr)
  {
ADLC_PORT_INFO *pptr = rptr->cptr->pptr;

  rptr->cptr->txExtAdlcReqCount --; /* The write request is all completed */
  list_unlink((ST_VOID **) &pptr->txExtAdlcReqList,rptr);
  ipcChkFree(rptr);
  }
/************************************************************************/
/*			addToUIFramesInTxList				*/
/* This function allocates and adds an Info Frame for transmission	*/
/* The len passed in argument is just INFO len, so we allocate the 	*/
/* buffer of the frame to store this INFO plus non info bytes		*/
/************************************************************************/
ST_VOID addToUIFramesInTxList(ADLC_REQ *rptr,ST_UCHAR *data,ST_INT dataLen)
  {
ADLC_CON_INFO *cptr = rptr->cptr;
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_FRAME *fptr;
#if defined(ADLC_LM)
ST_RET retCode;
#endif

  fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
  fptr->u.infoBuf = (ST_UCHAR *) ipcChkCalloc(1,dataLen + MIN_INFO_FRAME_LEN);
  fptr->infoLen = dataLen;
  fptr->frameLen = dataLen + MIN_INFO_FRAME_LEN;
  fptr->cmd = UI + PF_BIT;
  memcpy(&fptr->u.infoBuf[INFO_FIELD_OFFSET],data,dataLen);
  fptr->cptr = cptr;
  fptr->status = STAT_STANDBY;
  list_add_last ((ST_VOID **) &rptr->txUIFramesList,fptr);
#if defined(ADLC_LM)
  if ((retCode = ipcPostCommonEvent(pptr->frameInListSem)) != SD_SUCCESS)
    logIpcError(retCode,"FrameInList Post Semaphore Failed",thisFileName,__LINE__);
#endif
  }	
/************************************************************************/
/*			addToUIFramesInRxList				*/
/* This function allocates and adds an UI Frame for receiving		*/
/* The len passed in argument is INFO len, so we allocate the 		*/
/* buffer of the frame to store this INFO field				*/
/************************************************************************/
ST_VOID addToUIFramesInRxList(ADLC_CON_INFO *cptr,ST_UCHAR *data,ST_INT dataLen)
  {
ADLC_FRAME *fptr;

  fptr = (ADLC_FRAME *)ipcChkCalloc(1,sizeof(ADLC_FRAME));
  fptr->u.infoBuf = (ST_UCHAR *) ipcChkCalloc(1,dataLen);
  fptr->infoLen = dataLen;
  fptr->frameLen = dataLen;
  memcpy(fptr->u.infoBuf,data,dataLen);
  cptr->extInfoBufLen += fptr->infoLen;
  fptr->cptr = cptr;
  list_add_last ((ST_VOID **)  &cptr->rxUIFramesList,fptr);
  }
/************************************************************************/
/* 			deleteUIFramesFromRxList			*/
/************************************************************************/
ST_VOID deleteUIFramesFromRxList(ADLC_CON_INFO *cptr)
  {
ADLC_FRAME *fptr;	/* frame pointer		*/

/* unlink frames for that cptr from rxUIFamesList			*/
  while (cptr->rxUIFramesList)
    {
    fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &cptr->rxUIFramesList);
    ipcChkFree (fptr->u.infoBuf);
    ipcChkFree (fptr);	
    }
  cptr->extInfoBufLen = 0;
  }
/************************************************************************/
/*		deleteUIFramesFromTxList				*/
/* This function unlinks and frees all the frames associated with the 	*/
/* particular device in the transmission list of Unnumbered Info Frames	*/
/************************************************************************/
ST_VOID deleteUIFramesFromTxList(ADLC_CON_INFO *cptr)
  {
ADLC_PORT_INFO *pptr = cptr->pptr;
ADLC_REQ *rptr;	 	/* request pointer			*/
ADLC_REQ *nextRptr;
ADLC_FRAME *fptr;	/* frame pointer			*/

  rptr = pptr->txExtAdlcReqList;
  while (rptr)
    {
    nextRptr = (ADLC_REQ *) list_get_next (pptr->txExtAdlcReqList,rptr);
    if (rptr->cptr != cptr)	
      {				/* go to next request		*/
      rptr = nextRptr;
      continue;
      }
    while (rptr->txUIFramesList)
      {
      fptr = (ADLC_FRAME *) list_get_first ((ST_VOID **) &rptr->txUIFramesList);
      ipcChkFree (fptr->u.infoBuf);
      ipcChkFree (fptr);	
      }
    deleteExtReqFrame(rptr);	/* all done with this rptr			*/
    rptr = nextRptr;
    }
  }
/************************************************************************/

/************************************************************************/
ST_RET sendExtWriteDoneToStack(ADLC_CON_INFO *cptr, ST_LONG userId,
			    ST_CHAR *recTime, ST_INT bufLen,
			    ST_RET retCode)
  {
#if defined(ADLC_LM)
ADLC_CMD cmdMsg;
ST_INT stackTaskId;
#else
ADLC_EXT_WRITE_PACKET_DONE *pstExtWriteDone;
ADLC_EVENT_INFO *eptr;
#endif
ST_INT opCode = ADLC_CMD_EXT_WRITE_PACKET_DONE;

  ADLC_LOG_USER5("Issuing %s to stack for Connection 0x%x->0x%x\n"
		  " cmdRslt = %s user Id = %ld",
		  getOpCode(opCode),cptr->masterAddr,
		  cptr->dptr->address,getRetCode(retCode),userId);

#if defined(ADLC_LM)
  if (getTaskId(cptr,&stackTaskId) != SD_SUCCESS)
    {
/*  ADLC_LOG_ERR1("ADLC ERROR Do not know which queue to write to for local address = 0x%x",localAddr); */
    return (SD_FAILURE);
    }
  memset (&cmdMsg, '\x0', sizeof (ADLC_CMD));
  cmdMsg.hdr.doneSem = 0;		/* Semaphore not used.	*/
  cmdMsg.hdr.cmdRslt = retCode;
  cmdMsg.hdr.cmdCode = opCode;
  cmdMsg.u.extWritePacketDone.i.userId = userId;
  cmdMsg.u.extWritePacketDone.i.localAddr  = getLocalAddr(cptr);
  cmdMsg.u.extWritePacketDone.i.remoteAddr = getRemoteAddr(cptr);
  memcpy(&cmdMsg.u.extWritePacketDone.i.recTime,recTime,bufLen);
  return sendDataMsgToStack(&cmdMsg,stackTaskId);
#endif /* ADLC_LM */

#if !defined(ADLC_LM)
  pstExtWriteDone = (ADLC_EXT_WRITE_PACKET_DONE *) ipcChkCalloc(1,sizeof(ADLC_EXT_WRITE_PACKET_DONE));	
  pstExtWriteDone->i.userId = userId;
  pstExtWriteDone->i.localAddr = getLocalAddr(cptr);
  pstExtWriteDone->i.remoteAddr = getRemoteAddr(cptr);
  memcpy(pstExtWriteDone->i.recTime,recTime,bufLen);
  eptr = addToEventList(opCode,pstExtWriteDone,retCode);
  return (SD_SUCCESS);
#endif
  }
#endif /* UCA_SMP */


	
