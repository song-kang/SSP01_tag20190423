/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_get.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* 	This module contains Debug information for ADLC	used for 	*/
/* logging only								*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    13     Cleaned up general PClint errors             */
/* 09/23/02  KCR    12     Removed extra include files                  */
/* 09/23/02  KCR    11     Moved thisFileName before includes           */
/* 08/14/02  KCR    10     Moved getRetCode() outside of excluded area  */
/* 09/13/99  MDE    09     Added SD_CONST modifiers         		*/
/* 10/21/97  IKE    08     Changed OSI_ADDR to LOCAL_ADDR in error codes*/
/* 10/07/97  IKE    07     Added adlcExtWritePacket for define UCA_SMP	*/
/* 09/18/97  IKE    06     Added telephone support			*/
/* 05/12/97  IKE    05     Changed to MMSEASE 7.0 Data Types		*/
/* 02/24/97  IKE    04	   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/* 12/03/96  IKE    03     WIN32 Support, header files cleanup		*/ 
/* 			   Added std commands in getOpcode function	*/	
/* 10/04/96  IKE    02	   Added getCommunicationMedia, some cleanup	*/
/*			   Changed DEBUG_MMS to DEBUG_SISCO		*/
/* 01/10/96  IKE    01      Initial release                             	*/
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

#if defined(ADLC_TESTING) || defined(DEBUG_SISCO)
/************************************************************************/
/*		                getCommandName				*/
/* Used for Logging only: both commands and responses			*/
/************************************************************************/
ST_CHAR *getCommandName(ST_UCHAR ctrlByte)
  {
  if(!(ctrlByte & 0x01))
    return("INFO");
  else if((ctrlByte & 0x03) == 0x03)
    {
    switch(UNUM_CODE_GET(ctrlByte))
      {
      case UI   : return("UI");
      case SNRM : return("SNRM");
      case UA   : return("UA");
      case DISC : return("DISC");
      case DM   : return("DM");
      case FRMR : return("FRMR");
      default   : return("UNKNOWN");
      }
    }
  else 
    {
    switch(SUPR_CODE_GET(ctrlByte))
      {
      case RR  :	return("RR");
      case RNR :	return("RNR");
      case REJ :	return("REJ");
      default  : 	return("UNKNOWN");
      }
    }
  }

/************************************************************************/
/*	                          getConState				*/
/*  Used for Logging only						*/
/************************************************************************/
ST_CHAR *getConState (ST_INT state)
  {
  switch (state)
    {
    case ADLC_S_DISCONNECT_PHASE :	return("ADLC_S_DISCONNECT_PHASE");
    case ADLC_S_LINK_DISCONNECT : 	return("ADLC_S_LINK_DISCONNECT");
    case ADLC_S_LINK_SETUP : 		return("ADLC_S_LINK_SETUP");
    case ADLC_S_NORMAL : 		return("ADLC_S_NORMAL");
    case ADLC_S_REMOTE_BUSY : 		return("ADLC_S_REMOTE_BUSY");
    case ADLC_S_SEND_REJECT : 		return("ADLC_S_SEND_REJECT");
    case ADLC_S_FRAME_REJECT :		return("ADLC_S_FRAME_REJECT");
    case ADLC_S_RESET :			return("ADLC_S_RESET");
    default :				return("UNKNOWN");
    }
  }
/************************************************************************/
/*	                          getTxRxMode				*/
/*  Used for Logging only						*/
/************************************************************************/
ST_CHAR *getTxRxMode (ST_INT mode)
  {
  switch (mode)
    {
    case RX_MODE :		return("RX_MODE");
    case TX_MODE :		return("TX_MODE");
    default 	 :		return("UNKNOWN");
    }
  }
/************************************************************************/
/*	                          getRunMode				*/
/*  Used for Logging only						*/
/************************************************************************/
ST_CHAR *getRunMode (ST_INT mode)
  {
  switch (mode)
    {
    case POLLED_MODE :		 return("POLLED_MODE");		
    case ENHANCED_POLLED_MODE :	 return("ENHANCED_POLLED_MODE");
    case CONTENTION_MODE :	 return("CONTENTION_MODE");
    default :			 return("UNKNOWN");
    }
  }
/************************************************************************/
/*	                       getCommunicationMedia			*/
/*  Used for Logging only						*/
/************************************************************************/
ST_CHAR *getCommunicationMedia (ST_INT media)
  {
  switch (media)
    {
    case CABLE_MEDIA :		 return("CABLE_MEDIA");
    case RADIO_MEDIA :	         return("RADIO_MEDIA");
    default :			 return("UNKNOWN");
    }
  }
/************************************************************************/
/*                               getOpcode				*/
/*  Used for Logging only						*/
/************************************************************************/
ST_CHAR *getOpCode (ST_INT opcode)
  {
  switch (opcode)
    {
    case ADLC_CMD_CONNECT_NODE :	 return("Connect");
    case ADLC_CMD_DISCONNECT_NODE : 	 return("Disconnect");
    case ADLC_CMD_CONNECT_NODE_DONE :	 return("Connect Done");
    case ADLC_CMD_DISCONNECT_NODE_DONE : return("Disconnect Done");
    case ADLC_CMD_CONNECT_ALL : 	 return("Connect All");
    case ADLC_CMD_DISCONNECT_ALL : 	 return("Disconnect All");
    case ADLC_CMD_WRITE_PACKET : 	 return("Write Packet");
    case ADLC_CMD_WRITE_PACKET_DONE : 	 return("Write Packet Done");
    case ADLC_CMD_GET_CON_STATUS : 	 return("Connection Status");
    case ADLC_CMD_ADD_DEV : 		 return("Add Device");
    case ADLC_CMD_REMOVE_DEV : 		 return("Remove Device");
    case ADLC_CMD_START_POLL_DEV : 	 return("Start Poll");
    case ADLC_CMD_STOP_POLL_DEV : 	 return("Stop Poll");
    case ADLC_CMD_CHANGE_POLL_RATE : 	 return("Change Poll");
    case ADLC_CMD_QUERY_POLL_INFO : 	 return("Query Poll");
#if defined(ADLC_LM)
    case RLI_CMD_GET_STATUS :		 return("Get Status");
    case RLI_CMD_GET_STATS :		 return("Get Statistics");
    case RLI_CMD_RESET_STATS :		 return("Reset Statistics");
    case RLI_CMD_GET_PERF :		 return("Get Performance");
    case RLI_CMD_SET_PERF :		 return("Set Performance");
    case RLI_CMD_GET_LOG_MASK :		 return("Get Log Mask");
    case RLI_CMD_SET_LOG_MASK :		 return("Set Log Mask");
    case RLI_CMD_LOG_STATE :		 return("Log State");
    case RLI_CMD_LOG_CONFIG :		 return("Log Configuration");
    case RLI_CMD_EXECUTE :		 return("Execute");
    case RLI_CMD_RECONFIGURE :		 return("Reconfigure");
    case RLI_CMD_TERMINATE :		 return("Terminate");
    case RLI_CMD_OPEN_INTERFACE :	 return("Open Interface");
    case RLI_CMD_CLOSE_INTERFACE :	 return("Close Interface");
    case ADLC_CMD_TEL_DIAL :		 return("Tel Dial");
    case ADLC_CMD_TEL_DIAL_DONE : 	 return("Tel Dial Done");
    case ADLC_CMD_TEL_HANGUP :		 return("Tel Hangup");
    case ADLC_CMD_TEL_HANGUP_DONE : 	 return("Tel Hangup done");
#endif
#if defined(UCA_SMP)
    case ADLC_CMD_EXT_WRITE_PACKET :	 return("Ext Write");
    case ADLC_CMD_EXT_WRITE_PACKET_DONE: return("Ext Write Done");
#endif
    default :			 	 return("UNKNOWN");
    }
  }

/************************************************************************/
/*	                      getSignalState				*/
/*  Used for Logging only						*/
/************************************************************************/
ST_CHAR *getSignalState(ST_INT signalState)
  {
  switch (signalState)
    {
#if defined(ADLC_MASTER)
    case MA_TX_STATE_IDLE :			 return("MA_TX_STATE_IDLE");
    case MA_TX_STATE_WAIT_FOR_CTS_HIGH :	 return("MA_TX_STATE_WAIT_FOR_CTS_HIGH");
    case MA_TX_STATE_WAIT_FOR_DCD_LOW :		 return("MA_TX_STATE_WAIT_FOR_DCD_LOW");
    case MA_TX_STATE_OK_TO_TX :			 return("MA_TX_STATE_OK_TO_TX");
    case MA_TX_STATE_WAIT_FOR_TX_BUF_EMPTY :	 return("MA_TX_STATE_WAIT_FOR_TX_BUF_EMPTY");
    case MA_TX_STATE_DEKEY_DELAY :		 return("MA_TX_STATE_DEKEY_DELAY");
    case MA_TX_STATE_WAIT_FOR_CTS_LOW :		 return("MA_TX_STATE_WAIT_FOR_CTS_LOW");
    case MA_TX_STATE_WAIT_FOR_CTS_HIGH_DCD_LOW : return("MA_TX_STATE_WAIT_FOR_CTS_HIGH_DCD_LOW");
#endif
#if defined(ADLC_SLAVE)
    case SL_TX_STATE_IDLE :			 return("SL_TX_STATE_IDLE");			
    case SL_TX_STATE_PF_RCVD :			 return("SL_TX_STATE_PF_RCVD");			
    case SL_TX_STATE_WAIT_FOR_CTS_HIGH :	 return("SL_TX_STATE_WAIT_FOR_CTS_HIGH");		
    case SL_TX_STATE_START_OF_TX_DELAY :	 return("SL_TX_STATE_START_OF_TX_DELAY");		
    case SL_TX_STATE_OK_TO_TX :			 return("SL_TX_STATE_OK_TO_TX");			
    case SL_TX_STATE_WAIT_FOR_TX_BUF_EMPTY :	 return("SL_TX_STATE_WAIT_FOR_TX_BUF_EMPTY");	
    case SL_TX_STATE_DEKEY_DELAY :		 return("SL_TX_STATE_DEKEY_DELAY");			
    case SL_TX_STATE_WAIT_FOR_CTS_LOW :		 return("SL_TX_STATE_WAIT_FOR_CTS_LOW");		
    case SL_TX_STATE_UNSOL_TX  :		 return("SL_TX_STATE_UNSOL_TX ");			
    case SL_TX_STATE_WAIT_FOR_LINK  :		 return("SL_TX_STATE_WAIT_FOR_LINK ");		
    case SL_TX_STATE_START_OF_UNSOL_TX_DELAY :	 return("SL_TX_STATE_START_OF_UNSOL_TX_DELAY");	
    case SL_TX_STATE_OK_TO_UNSOL_TX :		 return("SL_TX_STATE_OK_TO_UNSOL_TX");		
#endif
    default :			 	 return("UNKNOWN");
    }
  }
#else
/* Intel compiler needs something to compile: */
static void dummy(void)
{
}
#endif /* ADLC_TESTING || DEBUG_SISCO */

/************************************************************************/
/*                               getRetcode				*/
/*  Used for Logging only						*/
/************************************************************************/
ST_CHAR *getRetCode (ST_RET retCode)
  {
#if defined(ADLC_LM)
static ST_CHAR errBuf[256];
#endif
  switch (retCode)
    {
    case SD_SUCCESS : return("Success");
    case SD_FAILURE : return("Failure");
    case E_ADLC_INVALID_DEV_NAME :	return ("Invalid Device Name");
    case E_ADLC_INVALID_DEV_ADDR :	return ("Invalid Device Addr");
    case E_ADLC_DEVICE_EXISTS :		return ("Device already exists");
    case E_ADLC_DEV_DOES_NOT_EXIST :	return ("Device does not exist");

    case E_ADLC_INVALID_LOCAL_ADDR :	return ("Invalid local address");
    case E_ADLC_INVALID_CON_PAIR :	return ("Invalid Connection Pair");
    case E_ADLC_CON_EXISTS :		return ("Connection already exists");
    case E_ADLC_CON_DOES_NOT_EXIST :	return ("Connection does not exist");
    case E_ADLC_TX_BUF_FULL :		return ("Transmit Buffer Full");

    case E_ADLC_INVALID_PORT :		return ("Invalid port");
    case E_ADLC_CANT_INIT_PORT :	return ("Port can not be initialized");
    case E_ADLC_DUPLICATE_LOCAL_ADDR :	return ("Duplicalte local address");
    case E_ADLC_PACKET_LEN_NOT_VALID :	return("Packet Length invalid");	
    case E_ADLC_RESPONSE_TIMEOUT : 	return("Response Timeout");	
    case E_ADLC_INTERNAL_ERR :		return ("Internall error");

#if defined(ADLC_MASTER)
    case E_ADLC_CANT_POLL_FASTER :	return ("Can not poll faster");	
    case E_ADLC_CANT_POLL_SLOWER :	return ("Can not poll slower");
    case E_ADLC_INVALID_POLL_DELTA :	return ("Invalid poll difference");
    case E_ADLC_DEVICE_NOT_POLLED :	return ("Device Not polled");
    case E_ADLC_INVALID_POLL_FREQ :	return ("Invalid poll frequency");
#endif
#if defined(ADLC_LM)
    case E_ADLC_INVALID_HQUEUE :	return ("Invalid Queue Handle");	
    case E_ADLC_CREATE_THREAD  :	return ("Can not create thread");
    case E_ADLC_CANT_INIT_TAPI :	return ("Can not initialize telephony");
    case E_ADLC_TEL_LINE_UNAVAIL :	return ("Telephone line not available");	
    case E_ADLC_CANT_INIT_TEL_LINE : 	return ("Can not initialize telephone line");
    case E_ADLC_PORT_EXISTS :		return ("Port already exists");
    case E_ADLC_PORT_DOES_NOT_EXIST :	return ("Port does not exist");
    default :  ipcGetErrString (retCode, errBuf, sizeof (errBuf));
	       return (errBuf);	
#else
    default :			 	return("UNKNOWN");
#endif
    }
  }
