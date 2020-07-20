/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2003, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_cmd.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 01/15/03  KCR    32     Do not add port if TAPI line disconnected    */
/* 11/24/02  KCR    31     Added rdThreadError handling semaphore       */
/* 09/23/02  KCR    30     Cleaned up general PClint errors             */
/* 09/23/02  KCR    29     Removed extra include files                  */
/* 09/23/02  KCR    28     Moved thisFileName before includes           */
/* 09/10/01  KCR    27     Do not Delete Port configuration   		*/
/* 09/13/99  MDE    26     Added SD_CONST modifiers         		*/
/* 08/13/98  JRB    25     Lint cleanup.				*/
/* 06/05/98  IKE    24     Cleaned up adlcRemovePort			*/
/* 03/04/98  IKE    23     Removed adlcIpcxxx functions			*/
/* 02/10/98  IKE    22     Changed the function name to sendMyselfMsgCmd*/
/* 			   for testing multiple instances of slaves	*/
/* 09/23/97  IKE    21     Added adlcExtWritePacket for define UCA_SMP	*/
/*			   Clean up of header files for non IPC version */
/*			   of WIN32 MMS lite				*/
/* 09/18/97  IKE    20     Added telephone support			*/
/* 07/16/97  IKE    19     Added support for new ADLC commands		*/
/*                         adlcGetDevStats and adlcFilterDevLog.	*/
/* 05/12/97  IKE    18     Changed to MMSEASE 7.0 Data Types		*/
/* 02/03/97  IKE    17     adlcCfg per port basis			*/
/*			   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/*			   CFG_PORT also for DOS so multiple ports can	*/
/*			   be initialized in dos platform		*/
/* 01/17/97  IKE    16     Modified connectAll and disconnectAll API not*/
/* 			   to change the state of connection.		*/
/* 12/03/96  IKE    15     WIN32 Support, header files cleanup		*/ 
/* 11/15/96  IKE    14     Added adlcInitGlbVars			*/
/* 11/06/96  IKE    13     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/28/96  KCR    12     Converted to mulitple thread safe alloc funs */
/* 10/04/96  IKE    11     Called adlcAddDev in adlcConnectAll for slave*/
/*			   Added pollInit in adlcInit			*/
/* 08/16/96  IKE    10     Removed isOkToAddToMFramesList		*/
/* 08/05/96  IKE    09     Modified adlcConnectAll as requested by JRB	*/
/*			   and HSF					*/
/* 07/17/96  IKE    08     Removed some lint and C6.00 warnings 	*/
/* 06/21/96  IKE    07     Changed some ADLC_LOG_IO to ADLC_LOG_USER	*/
/* 06/18/96  IKE    06     Added stimeInit() function. set the task id 	*/
/* 			   to RLI_TASK_ID_TRIM7 for adlcConnectAll	*/
/* 06/11/96  IKE    05     Added CNP related code			*/
/* 05/22/96  IKE    04     Cleaned up header files, put some comments	*/
/*			   Used gen_list(eventList) to give upbound 	*/
/*			   events to DOS stack insted of global vars. 	*/
/*			   Modified connect and disconnect node API not */
/* 			   to change the state of connection. Modified	*/
/*			   adlcDisconnectNode to delete the connection 	*/
/*			   for slave. Modified adlcAddDev for adding a	*/
/*			   poll group					*/
/* 04/05/96  IKE    03     Change Logging from device osi terminology	*/
/* 			   to local remote terminology			*/
/* 04/05/96  rkr    02     Header file and SAVE_CALLING cleanup 	*/
/* 04/04/96  rkr    01     Cleanup					*/
/* 01/10/96  IKE   1.00    Initial release                              */
/************************************************************************/

static char *thisFileName = __FILE__;
/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

/* Some other SISCO files						*/
#include "cfg_util.h"
#include "mem_chk.h"
#include "memlog.h"
#include "stime.h"

/* ADLC files								*/
#include "adlc.h"
#if defined(ADLC_LM)
#include "adlc_eve.h"
#endif

#if defined(CNP)
#include "cnp_usr.h"
#endif
/************************************************************************/
/* Static variables							*/
/************************************************************************/

ST_VOID checkTelDial(ST_INT port);

/************************************************************************/
/*      Static Function Declarations                           		*/
/************************************************************************/
#if !defined(ADLC_LM)
static ST_RET portsInit(ST_VOID);
#endif
#if !defined(ADLC_LM)
/************************************************************************/
/*			adlcInit					*/
/* Calling this function is necessary before any activity with ADLC	*/
/************************************************************************/
ST_RET adlcInit(ST_VOID)
  {
/* Initialize global variables 						*/
  adlcInitGlbVars();

  if (adlcConfigure() != SD_SUCCESS)
    return (SD_FAILURE);

  if (stimeInit ())
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Could not initialize High Resolution Timer");
    return (SD_FAILURE);
    }

#if defined(ADLC_MASTER)
    if (pollInit() != SD_SUCCESS)
      return (SD_FAILURE);
#endif
#if defined(CNP)
  if (cnpInit() != SD_SUCCESS)
    {
    ADLC_LOG_ERR0("ADLC ERROR Could not initialize CNP");
    return (SD_FAILURE);
    }
#endif

  if (portsInit() != SD_SUCCESS)
    return (SD_FAILURE);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			   adlcRegisterClient				*/
/************************************************************************/

ST_RET adlcRegisterClient (ST_UINT16 localAddr)
  {
ADLC_CONNECT_ALL stConAll;
  stConAll.i.localAddr = localAddr;
  return (adlcConnectAll (&stConAll));
  }

/************************************************************************/
/*			       adlcRead					*/
/* It basically uses the adlcGetEvent. The difference is that the user  */
/* of this function is only interested in ADLC_WRITE_PACKET event so if	*/
/* other event came from adlc frame engine do not pass it but free      */
/* the control block													*/
/************************************************************************/
ADLC_WRITE_PACKET *adlcRead(ST_VOID)
  {
ADLC_WRITE_PACKET *adlcPacket = NULL;
ST_INT adlcOpcode;
ST_RET retCode;
ST_VOID *adlcCb;

  adlcCb = adlcGetEvent(&adlcOpcode,&retCode);
  if (adlcCb)
    if (adlcOpcode == ADLC_CMD_WRITE_PACKET)
      adlcPacket = (ADLC_WRITE_PACKET *) adlcCb;
#if defined(UCA_SMP)
    /* In this piece of code ADLC_CMD_EXT_WRITE_PACKET is converted	*/
    /* to  ADLC_CMD_WRITE_PACKET for the convenience of the application.*/
    /* It also allows the application to process and free the data.	*/
    /* In normal case the application will call the adlcGetEvent and	*/
    /* process the events. But if all events are ignored except data	*/
    /* then this function may be more convenient to use.		*/
    else if (adlcOpcode == ADLC_CMD_EXT_WRITE_PACKET)
      {	
      ADLC_EXT_WRITE_PACKET *adlcExtPacket;

      adlcExtPacket = (ADLC_EXT_WRITE_PACKET *) adlcCb;
      adlcPacket = (ADLC_WRITE_PACKET *) chk_calloc (1, sizeof (ADLC_WRITE_PACKET));
      adlcPacket->i.localAddr = adlcExtPacket->i.localAddr;
      adlcPacket->i.remoteAddr = adlcExtPacket->i.remoteAddr;
      adlcPacket->i.dataSize = adlcExtPacket->i.dataSize;
      adlcPacket->i.data = adlcExtPacket->i.data;
      chk_free(adlcExtPacket);
      }
#endif /* UCA_SMP */
    else
      ipcChkFree(adlcCb); 
  return (adlcPacket);
  }
/************************************************************************/
/*		     		adlcGetEvent				*/
/************************************************************************/
ST_VOID *adlcGetEvent(ST_INT *opCode, ST_RET *retCode)
  {
ST_VOID *adlcEventCb = NULL;
ST_INT adlcEventOp = 0; 
ST_RET adlcRetCode = SD_SUCCESS;
ADLC_EVENT_INFO *eptr;

  adlcService();
  if (eventList)
    {	
    eptr = (ADLC_EVENT_INFO *) list_get_first((ST_VOID **) &eventList);
    adlcEventOp = eptr->adlcEventOp;
    adlcEventCb = eptr->adlcEventCb;
    adlcRetCode = eptr->adlcRetCode;
    ipcChkFree(eptr);
    } 
  *retCode = adlcRetCode;
  *opCode = adlcEventOp;
  return(adlcEventCb);
  }
/************************************************************************/
/*				adlcWrite				*/
/************************************************************************/
ST_RET adlcWrite(ADLC_WRITE_PACKET *adlcPacket)
  {
  return (adlcWritePacket(adlcPacket));
  }
#endif /* !ADLC_LM */
/************************************************************************/
/*			       adlcConnectNode				*/
/************************************************************************/
ST_RET adlcConnectNode(ADLC_CONNECT_NODE *pstConNode)
  {
ADLC_DEV_INFO *dptr;
ADLC_CON_INFO *cptr;

  ADLC_LOG_USER0("Connect issued from the stack");
  ADLC_LOG_CUSER2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		 pstConNode->i.remoteAddr,pstConNode->i.localAddr);

#if defined(ADLC_SLAVE)
  if ((dptr = getDevPtrFromAddress(pstConNode->i.localAddr)) == NULL)
    {
    ADLC_LOG_NERR1 ("ADLC ERROR Local Address 0x%x received from stack is invalid",
    		       pstConNode->i.localAddr);
    return (E_ADLC_INVALID_DEV_ADDR);
    }
  if ((cptr = getConPtrFromDptr(dptr,pstConNode->i.remoteAddr)) == NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Already disconnected");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		    pstConNode->i.remoteAddr,pstConNode->i.localAddr);
    return (E_ADLC_CON_DOES_NOT_EXIST);
    }

  if (cptr->state == ADLC_S_NORMAL)
    return (SD_SUCCESS);

  return (SD_FAILURE);
#endif /* ADLC_SLAVE */

#if defined(ADLC_MASTER)
  if ((dptr = getDevPtrFromAddress(pstConNode->i.remoteAddr)) == NULL)
    {
    ADLC_LOG_NERR1 ("ADLC ERROR Remote Address 0x%x received from stack is invalid",
    		       pstConNode->i.remoteAddr);
    return (E_ADLC_INVALID_DEV_ADDR);
    }
  if ((cptr = getConPtrFromDptr(dptr,pstConNode->i.localAddr)) != NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Connection request already issued");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		    pstConNode->i.remoteAddr,pstConNode->i.localAddr);
    return (E_ADLC_CON_EXISTS);
    }

/* add this connection pointer to device list	*/
  cptr = addToConList(dptr,pstConNode->i.localAddr);

/* add the connection request to port list	*/
  addToUFramesList(cptr,SNRM+PF_BIT);

/*  cptr->state = ADLC_S_LINK_SETUP;					*/
  return (SD_SUCCESS);
#endif /* ADLC_MASTER */
  }
/************************************************************************/
/*			      adlcDisconnectNode			*/
/************************************************************************/
ST_RET adlcDisconnectNode(ADLC_DISCONNECT_NODE *pstDiscNode)
  {
ADLC_DEV_INFO *dptr;
ADLC_CON_INFO *cptr;

  ADLC_LOG_USER0("Disconnect issued from the stack");
  ADLC_LOG_CUSER2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		 pstDiscNode->i.remoteAddr,pstDiscNode->i.localAddr);

#if defined(ADLC_MASTER)
  if ((dptr = getDevPtrFromAddress(pstDiscNode->i.remoteAddr)) == NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Remote Address 0x%x received from stack is invalid",
    		       pstDiscNode->i.remoteAddr);
    return (E_ADLC_INVALID_DEV_ADDR);
    }

  if ((cptr = getConPtrFromDptr(dptr,pstDiscNode->i.localAddr)) == NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Already disconnected");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		    pstDiscNode->i.remoteAddr,pstDiscNode->i.localAddr);
    return (E_ADLC_CON_DOES_NOT_EXIST);
    }

/* add the disconnect request to port list	*/
  addToUFramesList(cptr,DISC+PF_BIT);

/*  cptr->state = ADLC_S_LINK_DISCONNECT;				*/
  return (SD_SUCCESS);
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
  if ((dptr = getDevPtrFromAddress(pstDiscNode->i.localAddr)) == NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Local Address 0x%x received from stack is invalid",
    		       pstDiscNode->i.localAddr);
    return (E_ADLC_INVALID_DEV_ADDR);
    }

  if ((cptr = getConPtrFromDptr(dptr,pstDiscNode->i.remoteAddr)) == NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Already disconnected");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		    pstDiscNode->i.remoteAddr,pstDiscNode->i.localAddr);
    return (E_ADLC_CON_DOES_NOT_EXIST);
    }

  giveIndicationToStack(cptr,ADLC_CMD_DISCONNECT_NODE_DONE,SD_SUCCESS, 0L);
  resetConnection(cptr,ADLC_DELETE_CON);

  return (SD_SUCCESS);
#endif /* ADLC_SLAVE */
  }
/************************************************************************/
/*		               adlcConnectAll				*/
/* This function is a request to connect to all ADLC Devices for an OSI	*/
/* node whose address is passed into the arguments			*/
/************************************************************************/
ST_RET adlcConnectAll(ADLC_CONNECT_ALL *pstConAll)
  {
#if defined(ADLC_SLAVE)	&& !defined(CNP)
ADLC_ADD_DEV stAddDev;
ADLC_PORT_INFO *pptr = portList;

  if (getDevPtrFromAddress(pstConAll->i.localAddr))
    return (SD_SUCCESS);

   /* dev is not in local dib we will add dynamically */
  stAddDev.i.address = pstConAll->i.localAddr;
  stAddDev.i.pollGroup = 0;
  stAddDev.i.connectAll = SD_TRUE;
  if (pptr)
    stAddDev.i.port = pptr->port;
  sprintf(stAddDev.i.devName,"node_%d",pstConAll->i.localAddr);
  return adlcAddDev(&stAddDev);
#endif

#if defined(ADLC_SLAVE)	&& defined(CNP)
  return (SD_SUCCESS);
#endif

#if defined(ADLC_MASTER) 
ADLC_PORT_INFO *pptr;
ADLC_DEV_INFO *dptr;
ADLC_CON_INFO *cptr;

#if !defined(ADLC_LM)
  stackTaskId = RLI_TASK_ID_TRIM7; /* we assume that it has been linked to TRIM7 */
#endif

  ADLC_LOG_USER0("Connect issued from the stack for all Remote addresses");
  ADLC_LOG_CUSER1("Local Addr = 0x%x ",pstConAll->i.localAddr);
  pptr = portList;
  while (pptr)
    {
    dptr = pptr->devList;
    while (dptr)
      {
      if (dptr->connectAll)  /* if that device is configured for connect all */
        {
        if ((cptr = getConPtrFromDptr(dptr,pstConAll->i.localAddr)) != NULL)
          {
          ADLC_LOG_NERR0 ("ADLC ERROR Connection request already issued for");
          ADLC_LOG_CNERR2("Local Addr = 0x%x Remote Addr = 0x%x", 
    	      	           pstConAll->i.localAddr,dptr->address);
          }
        else
          {	
          /* add this connection pointer to device list	*/
          cptr = addToConList(dptr,pstConAll->i.localAddr);

          /* add the connection request to port list	*/
          addToUFramesList(cptr,SNRM+PF_BIT);
/*         cptr->state = ADLC_S_LINK_SETUP;		*/
	  }
 	}  /* if device is configured for connect all	*/
      dptr = (ADLC_DEV_INFO *) list_get_next (pptr->devList,dptr);
      }
    pptr = (ADLC_PORT_INFO *) list_get_next (portList,pptr);
    }
  return (SD_SUCCESS);
#endif /* ADLC_MASTER */
  }
/************************************************************************/
/*                           adlcDisconnectAll				*/
/* This function is a request to disconnect to all ADLC Devices for an	*/
/* OSI node whose address is passed into the arguments			*/
/************************************************************************/
ST_RET adlcDisconnectAll(ADLC_DISCONNECT_ALL *pstDiscAll)
  {
#if defined(ADLC_SLAVE)
  return(SD_FAILURE);
#endif

#if defined(ADLC_MASTER)
ADLC_PORT_INFO *pptr;
ADLC_DEV_INFO *dptr;
ADLC_CON_INFO *cptr;

  ADLC_LOG_USER0("Disconnect issued from the stack for all Remote addresses");
  ADLC_LOG_CUSER1("Local Addr = 0x%x ",pstDiscAll->i.localAddr);
  pptr = portList;
  while (pptr)
    {
    dptr = pptr->devList;
    while (dptr)
      {
      if ((cptr = getConPtrFromDptr(dptr,pstDiscAll->i.localAddr)) != NULL)
        {
        /* add the disconnect request to port list	*/
        addToUFramesList(cptr,DISC+PF_BIT);
/*      cptr->state = ADLC_S_LINK_DISCONNECT;		*/
	}
      dptr = (ADLC_DEV_INFO *) list_get_next (pptr->devList,dptr);
      }
    pptr = (ADLC_PORT_INFO *) list_get_next (portList,pptr);
    }
  return (SD_SUCCESS);
#endif /* ADLC_MASTER */
  }

/************************************************************************/
/*			     adlcWritePacket				*/
/* This function is used to write a packet to ADLC from stack. localAddr*/
/* and remoteAddr are provided						*/
/************************************************************************/
ST_RET adlcWritePacket(ADLC_WRITE_PACKET *adlcPacket)
  {
ADLC_DEV_INFO *dptr;
ADLC_CON_INFO *cptr;
ADLC_REQ *rptr;
ST_INT totalLen;
ST_INT dataLen;
ST_UCHAR *pData;

  ADLC_LOG_USER3("Write request issued from stack Remote = 0x%x, Local = 0x%x Size = %d",
    		  adlcPacket->i.remoteAddr,adlcPacket->i.localAddr,adlcPacket->i.dataSize);
  ADLC_LOG_USER_HEX (adlcPacket->i.dataSize,adlcPacket->i.data);

/* if it is a multicast address we do not have any connection just add to	*/
/* Multicast Frames List and return from this function				*/

  if (IS_MULTICAST_ADDR(adlcPacket->i.remoteAddr))
    {
    addToMFramesInTxList(adlcPacket->i.localAddr,adlcPacket->i.remoteAddr,
		     adlcPacket->i.data,adlcPacket->i.dataSize);
    return (SD_SUCCESS);
    }
#if defined(ADLC_MASTER)
  if ((dptr = getDevPtrFromAddress(adlcPacket->i.remoteAddr)) == NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Remote Address 0x%x received from stack is invalid",
    		       adlcPacket->i.remoteAddr);
    return (E_ADLC_INVALID_DEV_ADDR);
    }

  if ((cptr = getConPtrFromDptr(dptr,adlcPacket->i.localAddr)) == NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Already disconnected");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		    adlcPacket->i.remoteAddr,adlcPacket->i.localAddr);
    return (E_ADLC_CON_DOES_NOT_EXIST);
    }
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
  if ((dptr = getDevPtrFromAddress(adlcPacket->i.localAddr)) == NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Local Address 0x%x received from stack is invalid",
    		       adlcPacket->i.localAddr);
    return (E_ADLC_INVALID_DEV_ADDR);
    }

  if ((cptr = getConPtrFromDptr(dptr,adlcPacket->i.remoteAddr)) == NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Already disconnected");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		    adlcPacket->i.remoteAddr,adlcPacket->i.localAddr);
    return (E_ADLC_CON_DOES_NOT_EXIST);
    }
#endif /* ADLC_SLAVE */                    
  if (cptr->txAdlcReqCount > ADLC_MAX_ADLC_REQS(dptr->pptr))
    {
    ADLC_LOG_ERR0 ("ADLC ERROR adlcWritePacket Failed - Transmit Buffer full for");
    ADLC_LOG_CERR3("Remote Addr = 0x%x, Local Addr = 0x%x Write Requests = %d", 
	  	    adlcPacket->i.remoteAddr,adlcPacket->i.localAddr,cptr->txAdlcReqCount);
    return (E_ADLC_TX_BUF_FULL);
    }

/* Here you copy one ADLC request into your control block		*/
  rptr = addToReqList(cptr);
  rptr->usr1 = adlcPacket->i.usr1;	// save user data

/* Initialize local variables						*/
  totalLen = adlcPacket->i.dataSize;
  pData = (ST_UCHAR *) adlcPacket->i.data;

/* Attach smaller ADLC Info frames of this large LSDU		*/
  while (totalLen > 0)
    {
    if (totalLen > ADLC_INFO_LEN(dptr->pptr))
      dataLen = ADLC_INFO_LEN(dptr->pptr);
    else
      dataLen = totalLen;
    addToIFramesInTxList(rptr,pData,dataLen);
    pData += dataLen;		/* increment data pointer	*/
    totalLen -= dataLen;		/* decrement data length	*/
    }
  return(SD_SUCCESS);
  }

#if defined(UCA_SMP)
/************************************************************************/
/*			     adlcExtWritePacket				*/
/* This function is used to write a packet to ADLC from stack. localAddr*/
/* and remoteAddr are provided						*/
/************************************************************************/
ST_RET adlcExtWritePacket(const ADLC_EXT_WRITE_PACKET *pstExtWrite)
  {
ADLC_DEV_INFO *dptr;
ADLC_CON_INFO *cptr;
ADLC_PORT_INFO *pptr;
ADLC_REQ *rptr;
ST_INT totalLen;
ST_INT dataLen;
ST_UCHAR *pData;

  ADLC_LOG_USER4("Extended Write request issued from stack Remote = 0x%x, Local = 0x%x\n" 
		 " Size = %d userId = %ld",
    		  pstExtWrite->i.remoteAddr,pstExtWrite->i.localAddr,
		  pstExtWrite->i.dataSize,pstExtWrite->i.userId);
  ADLC_LOG_USER_HEX (pstExtWrite->i.dataSize,pstExtWrite->i.data);

#if defined(ADLC_MASTER)
  if ((dptr = getDevPtrFromAddress(pstExtWrite->i.remoteAddr)) == NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Remote Address 0x%x received from stack is invalid",
    		       pstExtWrite->i.remoteAddr);
    return (E_ADLC_INVALID_DEV_ADDR);
    }

  if ((cptr = getConPtrFromDptr(dptr,pstExtWrite->i.localAddr)) == NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Already disconnected");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		    pstExtWrite->i.remoteAddr,pstExtWrite->i.localAddr);
    return (E_ADLC_CON_DOES_NOT_EXIST);
    }
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
  if ((dptr = getDevPtrFromAddress(pstExtWrite->i.localAddr)) == NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Local Address 0x%x received from stack is invalid",
    		       pstExtWrite->i.localAddr);
    return (E_ADLC_INVALID_DEV_ADDR);
    }

  if ((cptr = getConPtrFromDptr(dptr,pstExtWrite->i.remoteAddr)) == NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR Already disconnected");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		    pstExtWrite->i.remoteAddr,pstExtWrite->i.localAddr);
    return (E_ADLC_CON_DOES_NOT_EXIST);
    }
#endif /* ADLC_SLAVE */                    
  pptr = dptr->pptr;
  if (cptr->txExtAdlcReqCount > ADLC_MAX_ADLC_REQS(dptr->pptr))
    {
    ADLC_LOG_ERR0 ("ADLC ERROR adlcExtWritePacket Failed - Transmit Buffer full for");
    ADLC_LOG_CERR4("Remote Addr = 0x%x, Local Addr = 0x%x user ID = %ld Write Requests = %d", 
	  	    pstExtWrite->i.remoteAddr,pstExtWrite->i.localAddr,
		    pstExtWrite->i.userId,cptr->txExtAdlcReqCount);
    return (E_ADLC_TX_BUF_FULL);
    }

/* Here you copy one ADLC request into your control block		*/
  rptr = addToExtReqList(cptr,&pptr->txExtAdlcReqList,pstExtWrite->i.userId);

/* Initialize local variables						*/
  totalLen = pstExtWrite->i.dataSize;
  pData = (ST_UCHAR *) pstExtWrite->i.data;

/* Attach smaller ADLC Info frames of this large LSDU		*/
  while (totalLen > 0)
    {
    if (totalLen > ADLC_INFO_LEN(dptr->pptr))
      dataLen = ADLC_INFO_LEN(dptr->pptr);
    else
      dataLen = totalLen;
    addToUIFramesInTxList(rptr,pData,dataLen);
    pData += dataLen;		/* increment data pointer	*/
    totalLen -= dataLen;		/* decrement data length	*/
    }
  return(SD_SUCCESS);
  }
#endif /* UCA_SMP */
/************************************************************************/
/*			adlcGetConStatus				*/
/************************************************************************/

ST_RET adlcGetConStatus(ADLC_GET_CON_STATUS *pstGetConStatus)
  {
ADLC_CON_INFO	*cptr;

  ADLC_LOG_USER0("Connection Status request issued");
  ADLC_LOG_CUSER2("Remote Addr = 0x%x, Local Addr = 0x%x", 
    		 pstGetConStatus->i.remoteAddr,pstGetConStatus->i.localAddr);

  if ((cptr = getConPtrFromAddrPair(pstGetConStatus->i.remoteAddr,pstGetConStatus->i.localAddr)) == NULL)
    {
    ADLC_LOG_ERR0 ("ADLC ERROR adlcGetConStatus Failed - No connection for");
    ADLC_LOG_CERR2("Remote Addr = 0x%x, Local Addr = 0x%x", 
	  	       pstGetConStatus->i.remoteAddr,pstGetConStatus->i.localAddr);
    return (E_ADLC_INVALID_CON_PAIR);
    }

  switch (cptr->state)
    {
    case ADLC_S_DISCONNECT_PHASE:
    case ADLC_S_LINK_DISCONNECT:
    case ADLC_S_LINK_SETUP:
      pstGetConStatus->o.conStatusOut = ADLC_CON_STATUS_OFFLINE;
    break;

    case ADLC_S_NORMAL:
    case ADLC_S_REMOTE_BUSY:
    case ADLC_S_SEND_REJECT:
      pstGetConStatus->o.conStatusOut = ADLC_CON_STATUS_ONLINE;
    break;

    default:	/* should never happen			*/
      pstGetConStatus->o.conStatusOut = ADLC_CON_STATUS_UNKNOWN;
    break;
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			       adlcAddDev				*/
/************************************************************************/

ST_RET adlcAddDev(ADLC_ADD_DEV *pstAddDev)
  {
ADLC_PORT_INFO *pptr;
ADLC_DEV_INFO *dptr;

#if defined(ADLC_MASTER)
POLL_GROUP *pstPollGrp = NULL;
#endif /* ADLC_MASTER */

  ADLC_LOG_USER3("adlcAddDev request issued for %s (Addr = 0x%x) on COM%d",
   pstAddDev->i.devName, pstAddDev->i.address, GET_COM_NO(pstAddDev->i.port));

  if ((pptr = getPortPtrFromPort(pstAddDev->i.port)) == NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Port = COM%d received is invalid",
   	            GET_COM_NO(pstAddDev->i.port));
    return (E_ADLC_INVALID_PORT);
    }

  /* ensure unique device address						*/
  if ((dptr = getDevPtrFromAddress(pstAddDev->i.address)) != NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR adlcAddDev Failed - Device Address 0x%x already exists",
    		       pstAddDev->i.address);
    return (E_ADLC_DEVICE_EXISTS);
    }

  if ((dptr = getDevPtrFromName(pstAddDev->i.devName)) != NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR adlcAddDev Failed - Device %s already exists",
    		       pstAddDev->i.devName);
    return (E_ADLC_DEVICE_EXISTS);
    }

#if defined(ADLC_MASTER)
  /* now find the poll group struct						*/
  if (pstAddDev->i.pollGroup)	/* if polling is requested for that device	*/
    {
    if ((pstPollGrp = findPollGroup(pstAddDev->i.pollGroup)) == NULL)
      {
      ADLC_LOG_NERR1("ADLC ERROR  AddDevice Failed - Invalid PollFreq %d",
                    pstAddDev->i.pollGroup);
      return (E_ADLC_INVALID_POLL_FREQ);
      }
    }
#endif /* ADLC_MASTER */

  /* every thing is ok, so allocate memory device and add to devList	*/
  /* also add to poll group list if pollGroup is present		*/
  dptr = addToDevList(pptr,pstAddDev->i.devName,pstAddDev->i.address,
  			   pstAddDev->i.pollGroup,pstAddDev->i.connectAll);

  return (SD_SUCCESS);
  }
/************************************************************************/
/*			       adlcRemoveDev				*/
/************************************************************************/

ST_RET adlcRemoveDev(ADLC_REMOVE_DEV *pstRemoveDev)
  {
ADLC_DEV_INFO *dptr;

#if defined(ADLC_MASTER)
POLL_GROUP *pstPollGrp;
POLLED_DEV *pstPollDev;
#endif

  ADLC_LOG_USER1("adlcRemoveDev request issued for %s ", pstRemoveDev->i.devName);

  if ((dptr = getDevPtrFromName(pstRemoveDev->i.devName)) == NULL)
    {
    ADLC_LOG_NERR1 ("ADLC ERROR adlcRemoveDev Failed - %s does not exist",
    		       pstRemoveDev->i.devName);
    return (E_ADLC_DEV_DOES_NOT_EXIST);
    }
#if defined(ADLC_MASTER)
  if (dptr->pollGroup) /* if this device was configured for polling */
    {
    pstPollGrp = findPollGroup(dptr->pollGroup);
    if (pstPollGrp == NULL) /* should never happen */
      {
      ADLC_LOG_NERR1("ADLC ERROR  findPollGroup Failed - Invalid Poll Group %u",
		   dptr->pollGroup);
      return (E_ADLC_INTERNAL_ERR);
      }
    pstPollDev = findPolledDevByAddress(dptr->address);
    ADLC_ASSERT (pstPollDev->dptr == dptr);
    if (pstPollDev) /* if that device was configured for polling		*/
      {		  /* unlink from poll group and free the buffer		*/
      list_unlink((ST_VOID **)&pstPollGrp->pollDevList,pstPollDev);
      ipcChkFree(pstPollDev);
      }
    }
#endif /* ADLC_MASTER */
  deleteAllConnections(dptr);
  deleteDevice(dptr);

  return (SD_SUCCESS);
  }

#if defined(ADLC_MASTER)
/************************************************************************/
/*			adlcStartPollingDev				*/
/************************************************************************/

ST_RET adlcStartPollingDev(ADLC_START_POLL_DEV *pstStartPollDev)
  {
POLLED_DEV	*pstDev;
ST_RET		rtnCode;

  pstDev = findPolledDevByName(pstStartPollDev->i.devName);
  if (pstDev)
    {
    pstDev->bSuspendPolling = SD_FALSE;
    rtnCode = SD_SUCCESS;
    ADLC_LOG_FLOW1("LinkManager:  Resumed polling for device '%s'",
                    pstStartPollDev->i.devName);
    }
  else
    {
    rtnCode = E_ADLC_INVALID_DEV_NAME;
    ADLC_LOG_NERR1("LinkManager:  StartPollingDevice Failed - invalid device '%s'",
                   pstStartPollDev->i.devName);
    }

  return (rtnCode);
  }

/************************************************************************/
/*			adlcStopPollingDev				*/
/************************************************************************/

ST_RET adlcStopPollingDev(ADLC_STOP_POLL_DEV *pstStopPollDev)
  {
POLLED_DEV	*pstDev;
ST_RET		rtnCode;

  pstDev = findPolledDevByName(pstStopPollDev->i.devName);
  if (pstDev)
    {
    pstDev->bSuspendPolling = SD_TRUE;
    rtnCode = SD_SUCCESS;
    ADLC_LOG_FLOW1("LinkManager:  Suspended polling for device '%s'",
                    pstStopPollDev->i.devName);
    }
  else
    {
    rtnCode = E_ADLC_INVALID_DEV_NAME;
    ADLC_LOG_NERR1("LinkManager:  StopPollingDevice Failed - invalid device '%s'",
                   pstStopPollDev->i.devName);
    }
  return (rtnCode);
  }

/************************************************************************/
/*			adlcChangePollRate				*/
/************************************************************************/

ST_RET adlcChangePollRate(ADLC_CHANGE_POLL_RATE *pstChangePollRate)
  {
ADLC_DEV_INFO	*dptr;
POLLED_DEV	*pstPollDev;
POLL_GROUP	*pstCurrPollGrp, *pstNewPollGrp;
ST_UINT16		currPollID;
ST_UINT16		newPollID = 0;
ST_RET ret = SD_SUCCESS;

  /* find the device info struct for this device			*/
  dptr = getDevPtrFromName(pstChangePollRate->i.devName);
  if (!dptr)
    {
    ADLC_LOG_NERR1("LinkManager:  ChangePollRate Failed - invalid device '%s'",
                   pstChangePollRate->i.devName);
    return (E_ADLC_INVALID_DEV_NAME);
    }

  /* verify the change of poll group					*/
  currPollID = dptr->pollGroup;
  switch (pstChangePollRate->i.pollFreqDelta)
    {
    case ADLC_INC_POLL_FREQ:
      newPollID = currPollID-1;
      if (newPollID < FIRST_POLL_GROUP)
        {
	ADLC_LOG_NERR1("LinkManager:  ChangePollRate Failed - '%s' Already highest frequency",
	                pstChangePollRate->i.devName);
        ret = E_ADLC_CANT_POLL_FASTER;
	}
    break;

    case ADLC_DEC_POLL_FREQ:
      newPollID = currPollID +1;
      if (newPollID > MAX_POLL_GROUPS)
        {
	ADLC_LOG_NERR1("LinkManager:  ChangePollRate Failed - '%s' Already lowest frequency",
	                pstChangePollRate->i.devName);
        ret = E_ADLC_CANT_POLL_SLOWER;
	}
    break;

    default:
      ADLC_LOG_NERR1("LinkManager:  ChangePollRate Failed - Device='%s' - No freq change requested",
                      pstChangePollRate->i.devName);
      ret = E_ADLC_INVALID_POLL_DELTA;
    break;
    }

  if (ret)	/* see if any case statement caused an error		*/
    return (ret);

  /* find the poll dev struct and poll group struct for this device	*/
  pstPollDev = findPolledDevByName(pstChangePollRate->i.devName);
  if (!pstPollDev)
    {
    ADLC_LOG_NERR2("LinkManager:  ChangePollRate Failed - Device '%s' not in poll group '%u'",
                    pstChangePollRate->i.devName, dptr->pollGroup);
    return (E_ADLC_DEVICE_NOT_POLLED);
    }

  pstCurrPollGrp = findPollGroup(currPollID);
  pstNewPollGrp  = findPollGroup(newPollID);
  if (!pstCurrPollGrp || !pstNewPollGrp)
    {
    ADLC_LOG_NERR0("LinkManager:  ChangePollRate Failed - Damaged Poll Group Structure!");
    return (E_ADLC_INTERNAL_ERR);
    }

  /* move the device from one poll group and add it to the other	*/
  list_move_to_first((ST_VOID **) &pstCurrPollGrp->pollDevList,
                     (ST_VOID **) &pstNewPollGrp->pollDevList, pstPollDev);

  /* update the device info structure poll group reference		*/
  dptr->pollGroup = newPollID;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			adlcQueryPollInfo				*/
/************************************************************************/

ST_RET adlcQueryPollInfo(ADLC_QUERY_POLL_INFO *pstQueryPollInfo)
  {
ADLC_DEV_INFO	*dptr;
POLLED_DEV	*pstPollDev;
POLL_GROUP	*pstPollGrp;

  dptr = getDevPtrFromName(pstQueryPollInfo->i.devName);
  if (!dptr)
    {
    ADLC_LOG_NERR1("LinkManager:  QueryPollInfo Failed - invalid device '%s'",
                   pstQueryPollInfo->i.devName);
    return (E_ADLC_INVALID_DEV_NAME);
    }

  pstPollGrp = findPollGroup(dptr->pollGroup);
  if (!pstPollGrp)
    {
    ADLC_LOG_NERR1("LinkManager:  QueryPollInfo Failed - Invalid Poll Group %u",
		   dptr->pollGroup);
    return (E_ADLC_INTERNAL_ERR);
    }

  pstPollDev = findPolledDevByName(pstQueryPollInfo->i.devName);
  if (!pstPollDev)
    {
    ADLC_LOG_NERR2("LinkManager:  QueryPollInfo Failed - Device '%s' not in poll group '%u'",
                    pstQueryPollInfo->i.devName, dptr->pollGroup);
    return (E_ADLC_DEVICE_NOT_POLLED);
    }

  pstQueryPollInfo->o.pollGroup = dptr->pollGroup;
  pstQueryPollInfo->o.pollRate = pstPollGrp->pollRate;
  pstQueryPollInfo->o.bPollingSuspended = pstPollDev->bSuspendPolling;

  return (SD_SUCCESS);
  }
#endif /* ADLC_MASTER */

#if defined(__OS2__)
/************************************************************************/
/*			adlcGetSmSleepTime				*/
/************************************************************************/

ST_RET adlcGetSmSleepTime(ADLC_GET_SM_SLEEP *pstGetSmSleep)
  {
  pstGetSmSleep->o.nSleepTime = serMgrSleep;
  ADLC_LOG_FLOW1("LinkManager:  Retrieved SerialManager sleep time of %d",
                  serMgrSleep);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			adlcSetSmSleepTime				*/
/************************************************************************/

ST_RET adlcSetSmSleepTime(ADLC_SET_SM_SLEEP *pstSetSmSleep)
  {
ST_RET rtnVal;

  if (pstSetSmSleep->i.nSleepTime > 0)
    {
    serMgrSleep = pstSetSmSleep->i.nSleepTime;
    ADLC_LOG_FLOW1("LinkManager:  SerialManager sleep time set to %d",
                    serMgrSleep);
    rtnVal = SD_SUCCESS;
    }
  else
    {
    rtnVal = SD_FAILURE;
    ADLC_LOG_ERR1("LinkManager:  Invalid Sleep Time %d - Sleep Time not changed",
                   pstSetSmSleep->i.nSleepTime);
    }

  return (rtnVal);
  }
#endif /* __OS2 */

#if !defined(ADLC_LM)  
/************************************************************************/

/************************************************************************/
static ST_RET portsInit(ST_VOID)
  {
ST_RET ret = SD_SUCCESS;
ADLC_PORT_INFO *pptr;
CFG_PORT *pstCfgPort;

/* initialize all the ports with their communication params and control lines */
  pptr = portList;
  while (pptr && ret == SD_SUCCESS)
    {
    if ((pstCfgPort = findCfgPort(pptr->port)) == NULL)
      ret = SD_FAILURE;

    if (ret == SD_SUCCESS) /* found the config port to get communication params */
      {			/* now initialize the port			     */
      if (sm_init(pstCfgPort->port,pstCfgPort->baud,pstCfgPort->parity,
		  pstCfgPort->data, pstCfgPort->stop) != SD_SUCCESS)
        {
        ADLC_LOG_ERR1 ("ADLC ERROR Unable to initialize COM%d",
	              GET_COM_NO(pptr->port));
	ret = SD_FAILURE;
        }
      if (ret == SD_SUCCESS) /* port initialized successfully now initialize control lines */
        {
#if defined(ADLC_MASTER)
        if (pptr->runMode == CONTENTION_MODE)
          ret = sm_clear_rts_set_dtr(pptr->port);
        else
          ret = sm_set_signals(pptr->port);
#endif

#if defined(ADLC_SLAVE)
        ret = sm_clear_rts_set_dtr(pptr->port);
#endif
        if (ret)
          ADLC_LOG_ERR1("ADLC ERROR Could not initialize serial control lines for COM%d",
	              GET_COM_NO(pptr->port));
	}
      }
    pptr = (ADLC_PORT_INFO *) list_get_next (portList,pptr);
    }
  return (ret);
  }
#endif /* !defined ADLC_LM */

#if defined(ADLC_LM)
/************************************************************************/
/*			adlcGetDevStats					*/
/* This function gets the statisctics based on a device			*/
/************************************************************************/
ST_RET adlcGetDevStats(ADLC_GET_DEV_STATS *pstGetDevStats)
  {
ADLC_DEV_INFO	*dptr;

  ADLC_LOG_USER1("Device(Addr =  0x%x) Get Statistics request issued",
		  pstGetDevStats->i.address);
  /* ensure valid device address						*/
  if ((dptr = getDevPtrFromAddress(pstGetDevStats->i.address)) == NULL)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR adlcGetDevStats Failed - Device Address 0x%x is invalid",
    		       pstGetDevStats->i.address);
    return (E_ADLC_INVALID_DEV_ADDR);
    }

  memcpy(&pstGetDevStats->o.devStats, &dptr->devStats, sizeof(ADLC_DEV_STATS));
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			adlcFilterDevLog				*/
/* This function will set the log so only one device activity could be	*/
/* logged								*/
/************************************************************************/
ST_RET adlcFilterDevLog(ADLC_FILTER_DEV_LOG *pstFilterDevLog)
  {
ADLC_DEV_INFO	*dptr;

  if (pstFilterDevLog->i.bFilter)
    {
    ADLC_LOG_USER1("Device(Addr =  0x%x) Filter Device Logging ON request issued",
                   pstFilterDevLog->i.address);    /* ensure valid device address						*/
    if ((dptr = getDevPtrFromAddress(pstFilterDevLog->i.address)) == NULL)
      {
      ADLC_LOG_ERR1 ("ADLC ERROR adlcFilterDevLog Failed - Device Address 0x%x is invalid",
			pstFilterDevLog->i.address);
      return (E_ADLC_INVALID_DEV_ADDR);
      }
    /* save the state of IO and HEXIO logging flag only when for the	*/
    /* first time filtering of a device logging is requested.		*/
    if (!stFiltDevLog.bFilter)
      {
      stFiltDevLog.saveIoLogMask = adlc_debug_sel & (ADLC_MASK_LOG_IO | ADLC_MASK_LOG_HEXIO);
      ADLC_CLR_LOG_MASK (ADLC_MASK_LOG_IO | ADLC_MASK_LOG_HEXIO);
      }
    stFiltDevLog.address = pstFilterDevLog->i.address;
    stFiltDevLog.bFilter = SD_TRUE;
    }
  else
    {
    ADLC_LOG_USER0("Filter Device Logging OFF request issued");
	stFiltDevLog.bFilter = SD_FALSE;
    ADLC_SET_LOG_MASK (stFiltDevLog.saveIoLogMask);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			       adlcAddPort				*/
/************************************************************************/

ST_RET adlcAddPort(ADLC_ADD_PORT *pstAddPort)
  {
ADLC_PORT_INFO *pptr;
CFG_PORT *pstCfgPort;
ST_INT port = pstAddPort->i.port;
SM_REQ stSmReq;
ST_RET retCode;

  ADLC_LOG_USER2("adlcAddPort issued for COM%d hCom = 0x%x",
		  GET_COM_NO(port),pstAddPort->i.hCom);

/* make sure the port is not there then add the port */
  pptr = getPortPtrFromPort(port);
  if (pptr)
    {
    ADLC_LOG_USER1("Port already exists for COM%d", GET_COM_NO(port));
    return (E_ADLC_PORT_EXISTS);
    }
    
  if (pstAddPort->i.hCom)  // if this port is added for a TAPI phone connection
    {
    if (checkTelComValid(port, pstAddPort->i.hCom) == FALSE)
      {
      ADLC_LOG_USER1("Line disconnected (port not added) COM%d", GET_COM_NO(port));
      return (E_ADLC_CANT_INIT_PORT);
      }
    }
    
  pptr = addToPortList(pstAddPort->i.port,&adlcCfg);

/* also add the structure used for configuring the port */
  pstCfgPort = findCfgPort(port);
  if (pstCfgPort == NULL)
    pstCfgPort = addToCfgPortList(port);

/* initialize and start all the events associated with the port */
  retCode = initHandlePortEvents(pptr);
  if (retCode)
    {
    deletePort(pptr); /* resources allocated in addToPortList */
    return (retCode);
    }
    
  /* make port initialize request		*/
  stSmReq.opCode = SM_INITIALIZE_PORT_REQ;
  stSmReq.port = pptr->port;
  stSmReq.i.initReq.hCom = pstAddPort->i.hCom;
  stSmReq.i.initReq.txDoneSem = pptr->txDoneSem;
  stSmReq.i.initReq.frameArrivalSem = pptr->frameArrivalSem;
  stSmReq.i.initReq.rdThreadErrSem = pptr->rdThreadErrSem;
  stSmReq.i.initReq.frameLen = ADLC_FRAME_LEN(pptr);
  stSmReq.i.initReq.sof = ADLC_SOF(pptr);
  stSmReq.i.initReq.eof = ADLC_EOF(pptr);
  memcpy(&stSmReq.i.initReq.initLineParam, &pstCfgPort->stLineParam,
  sizeof(SM_LINE_PARAM));
  memcpy(&stSmReq.i.initReq.initHandShake, &pstCfgPort->stHandShake,
  sizeof(SM_HAND_SHAKE));
  memcpy(&stSmReq.i.initReq.initFlowCtrl, &pstCfgPort->stFlowCtrl,
  sizeof(SM_FLOW_CTRL));

  if (_SmRequest(&stSmReq)!=SD_SUCCESS)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Unable to initialize COM%d",
            GET_COM_NO(pptr->port));
    deletePortEvents(pptr);    /* resources alloc in initHandlePortEvents */
    deletePort(pptr); /* resources allocated in addToPortList */
    return (E_ADLC_CANT_INIT_PORT);
    }

  checkTelDial(port);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			       adlcRemovePort				*/
/************************************************************************/

ST_RET adlcRemovePort(ADLC_REMOVE_PORT *pstRemovePort)
  {
ADLC_PORT_INFO *pptr;
ADLC_DEV_INFO *dptr;
ST_INT port = pstRemovePort->i.port;
SM_REQ stSmReq;
#if defined(ADLC_MASTER)
POLL_GROUP *pstPollGrp;
POLLED_DEV *pstPollDev;
#endif

/* see if the port is there if not return error */
  pptr = getPortPtrFromPort(port);
  if (pptr == NULL)
    return (E_ADLC_PORT_DOES_NOT_EXIST);

  ADLC_LOG_USER1("adlcRemovePort issued for COM%d",GET_COM_NO(port));
/* so we found a port remove all the resources associated with it */
  while (pptr->devList)
    {
    dptr = list_get_first((ST_VOID **) &pptr->devList);
#if defined(ADLC_MASTER)
    if (dptr->pollGroup) /* if this device was configured for polling */
      {
      pstPollGrp = findPollGroup(dptr->pollGroup);
      if (pstPollGrp == NULL) /* should never happen */
        {
        ADLC_LOG_NERR1("ADLC ERROR  findPollGroup Failed - Invalid Poll Group %u",
		   dptr->pollGroup);
        deleteAllConnections(dptr); 
        ipcChkFree (dptr);	
	continue;
        }
      pstPollDev = findPolledDevByAddress(dptr->address);
      ADLC_ASSERT (pstPollDev->dptr == dptr);
      if (pstPollDev) /* if that device was configured for polling		*/
        {		  /* unlink from poll group and free the buffer		*/
        list_unlink((ST_VOID **)&pstPollGrp->pollDevList,pstPollDev);
        ipcChkFree(pstPollDev);
        }
      }
#endif
    deleteAllConnections(dptr); 
    ipcChkFree (dptr);	
    }
  deleteMFramesFromTxList(pptr);
  deleteMFramesFromRxList(pptr);
  deletePortEvents(pptr);
  deletePort(pptr);
  
/* remove from serial Manager */
  stSmReq.port = port;
  stSmReq.opCode = SM_TERMINATE_PORT_REQ;
  if (_SmRequest(&stSmReq)!=SD_SUCCESS)
    {
    ADLC_LOG_ERR1 ("ADLC ERROR Unable to terminate COM%d",
           GET_COM_NO(port));
    return(E_ADLC_INVALID_PORT);
    }
  return (SD_SUCCESS);
  }
#endif /* ADLC_LM */

