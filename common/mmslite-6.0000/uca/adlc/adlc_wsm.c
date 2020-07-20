/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-1996, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_wsm.c						*/
/* PRODUCT(S)  : ADLC Serial Manager for WIN32				*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* This module provides a template for the Serial Manager used for ADLC	*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*	ST_RET sm_init(port,baud,parity,data,stop			*/
/*	ST_RET sm_exit(port)						*/
/*      ST_RET sm_put (port,len,buffer)                             	*/
/* 	ST_RET sm_putc (port,ch)					*/
/*	ST_INT sm_rx_cnt (port)						*/
/*	ST_RET sm_get (port,packet,toRead,bytesRead)			*/
/*      ST_INT sm_getc (port)                                       	*/
/*      ST_VOID sm_rx_flush(port)					*/
/*	ST_RET sm_set_signals (port)					*/
/*	ST_RET sm_clear_signals (port)					*/
/*	ST_RET sm_set_rts (port)					*/
/*	ST_RET sm_clear_rts (port)					*/
/*	ST_RET sm_set_dtr (port)					*/
/*	ST_RET sm_clear_dtr (port)					*/
/*	ST_RET sm_set_rts_clear_dtr(port)				*/
/*	ST_RET sm_clear_rts_set_dtr(port)				*/
/*	ST_INT sm_get_rts (port)					*/
/*	ST_INT sm_get_dtr (port)					*/
/*	ST_INT sm_get_cts (port)					*/
/*	ST_INT sm_get_dsr (port)					*/
/*	ST_INT sm_get_dcd (port)					*/
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev      Comments                                    */
/* --------  ---  ------   -------------------------------------------  */
/* 09/13/99  MDE    5      Added SD_CONST modifiers			*/
/* 08/13/98  JRB    4      Lint cleanup.				*/
/* 02/04/98  IKE    3      Fixed the problem of opening COM10 and higher*/
/* 10/10/97  IKE    2	   Change CreateFile to use non overlapped	*/
/* 10/02/97  IKE    1      Created					*/
/************************************************************************/
/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

/* Some other SISCO files						*/
#include "gen_list.h"

#include <windows.h>

/* ADLC files								*/
#include "adlc_sm.h"
#include "adlc_log.h"
#include "adlc_usr.h"
/************************************************************************/
/*				SM_PORT_CTRL				*/
/************************************************************************/
typedef struct smPortCtrl
  {
  ST_INT  port;	    	/* the port number			*/
  HANDLE hCom;			/* Windows handle for comm resource	*/
  }SM_PORT_CTRL;

#define MAX_NUM_PORTS 64

/************************************************************************/
/*      Static Function Declarations                           		*/
/************************************************************************/
static ST_RET sm_log_error(ST_INT port, ST_RET errCode, ST_CHAR *str, ST_CHAR *file, ST_INT line);	
static HANDLE getHCom(ST_INT port);
static ST_RET setHCom(ST_INT port, HANDLE hCom);
static HANDLE MyGetCommState (ST_INT port, DCB *pstDcb);
static ST_RET MySetCommState(ST_INT port, HANDLE hCom, DCB *pstDcb);
static HANDLE MyGetModemStatus (ST_INT port, DWORD *pModemStatus);
static ST_RET convertForDcb(ST_INT baud,DWORD *BaudRate,
			      ST_INT parity, BYTE *Parity,
			      ST_INT data, BYTE *ByteSize,
			      ST_INT stop, BYTE *StopBits);

/************************************************************************/
/* Static variables							*/
/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
static SM_PORT_CTRL smPCtrl[MAX_NUM_PORTS];

/************************************************************************/
/*                      	sm_init                                 */
/* PURPOSE : This function is used to initialize a port and set the 	*/
/* configuration for any communication port. If necessary resources	*/
/* associated with the port could be allocated in this function		*/
/* Use this prior to program startup					*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/*	   baud - baud rate e.g BAUD110 defined in adlc_sm.h		*/
/*	   parity - parity e.g. NOPAR defined in adlc_sm.h		*/
/* 	   data - data bits DATA7 or DATA8 defined in adlc_sm.h		*/
/* 	   stop - stop bits STOP1 or STOP2 defined in adlc_sm.h		*/
/* RETURNS : SD_SUCCESS - port was initialized successfully		*/
/*	     SD_FAILURE - port was not initialized successfully		*/
/************************************************************************/
ST_RET sm_init (ST_INT port,ST_INT baud, ST_INT parity,
		 ST_INT data, ST_INT stop)
  {
ST_CHAR portName[24];
HANDLE hCom;
DCB dcb;
BOOL fSuccess;
DWORD BaudRate;
BYTE Parity;
BYTE ByteSize;
BYTE StopBits;
ST_RET ret;

  /* figure out the com port file name to use				*/
  /* Note that a special named format is used for COM10 and higher      */
  if (port + 1 < 10)
    sprintf(portName,"COM%d",port+1);
  else
    sprintf(portName,"%sCOM%d","\\\\.\\",port+1);

  /* now open up the port						*/
  hCom = CreateFile(portName,
	 	    GENERIC_READ | GENERIC_WRITE,
		    0,	/* comm devices must be opened with exclusive access	*/
		    NULL, /* no security attributes */
		    OPEN_EXISTING, /* comm devices must use OPEN_EXISTING	*/		
		    0, /* not overlapped I/O				*/
		    NULL /* hTemplate must be NULL for comm devices	*/
		    );

  if (hCom == INVALID_HANDLE_VALUE)
    return sm_log_error(port,SD_FAILURE, "Unable to open",__FILE__,__LINE__);
  setHCom(port,hCom);

  /* Get the current control setting for this port			*/
  fSuccess = GetCommState(hCom, &dcb);
  if (!fSuccess)
    return sm_log_error(port,SD_FAILURE,"Unable to get settings for",__FILE__,__LINE__);

  ret = convertForDcb(baud, &BaudRate, 
		      parity,&Parity,
		      data, &ByteSize,
		      stop, &StopBits);

/* if converted ok then change dcb else use control panel settings	*/
  if (ret == SD_SUCCESS) 
    {
    dcb.BaudRate = BaudRate;
    dcb.Parity   = Parity;
    dcb.ByteSize = ByteSize;
    dcb.StopBits = StopBits;
    }
  else
    {
    ADLC_LOG_ERR0("ADLC ERROR Communication params wrong, will use control panel settings");
    }

/* Disable CTS and DSR for output flow control				*/
  dcb.fOutxCtsFlow = SD_FALSE;
  dcb.fOutxDsrFlow = SD_FALSE;

/* Set the control setting for this port				*/
  fSuccess = SetCommState(hCom, &dcb);
  if (!fSuccess)
    return sm_log_error(port,SD_FAILURE, "Unable to set comm parameters for",__FILE__,__LINE__);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*                                  sm_exit                             */
/* PURPOSE : This function is used to de-initialize the port and free 	*/
/* the resources associated with the port. Use this prior to program 	*/
/* termination.								*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURNS : SD_SUCCESS - port was terminated successfully			*/
/*	     SD_FAILURE - port was not not initialized before calling this	*/
/************************************************************************/

ST_RET sm_exit (ST_INT port)
  {
HANDLE hCom;
BOOL fSuccess;

  hCom = getHCom(port);
  if (hCom == NULL)
    return sm_log_error(port,SD_FAILURE,"Uninitialized port",__FILE__,__LINE__);

  fSuccess = CloseHandle(hCom);
  if (!fSuccess)
    return sm_log_error(port,SD_FAILURE,"Unable to close handle for",__FILE__,__LINE__);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*                               sm_put                                 */
/* PURPOSE : This function is used to write a buffer to a comm port	*/
/* The buffer will be allocated and freed by ADLC			*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* 	   len - length of the buffer to be sent to the port		*/
/*	   packet - pointer to the data to be sent to the port		*/
/* RETURNS : SD_SUCCESS - data sent OK.					*/
/*	     SD_FAILURE - no characters transmitted			*/
/************************************************************************/

ST_RET sm_put (ST_INT port,ST_INT len,ST_CHAR *packet)
  {
HANDLE hCom;
BOOL fSuccess;
DWORD dwToWrite;
DWORD dwWritten;

  hCom = getHCom(port);
  if (hCom == NULL)
    return sm_log_error(port,SD_FAILURE,"Uninitialized port",__FILE__,__LINE__);
  dwToWrite = (DWORD) len;
  fSuccess = WriteFile(hCom, packet, dwToWrite, &dwWritten, NULL);

  if (!fSuccess)
    return sm_log_error(port,SD_FAILURE,"Unable to write",__FILE__,__LINE__);

  if (dwWritten != dwToWrite)
    return sm_log_error(port,SD_FAILURE,"Could not write all bytes",__FILE__,__LINE__);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*	                          sm_putc				*/
/* PURPOSE : This function is used to write a character to a comm port	*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/*	   ch - a single character/byte to send				*/
/* RETURNS : SD_SUCCESS - character sent OK.				*/
/*	     SD_FAILURE - character not transmitted			*/
/************************************************************************/
ST_INT sm_putc(ST_INT port, ST_CHAR ch)
  {
  return sm_put(port,1,&ch);
  }
/************************************************************************/
/*                               sm_rx_cnt                              */
/* PURPOSE : This function is to get the number of characters in the 	*/
/* receive buffer							*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURNS : number of received characters in the input buffer		*/
/************************************************************************/

ST_INT sm_rx_cnt (ST_INT port)
  {
HANDLE hCom;
COMSTAT cs;
DWORD dwError;

  hCom = getHCom(port);
  if (hCom == NULL)
    {
    sm_log_error(port,SD_FAILURE,"Uninitialized port",__FILE__,__LINE__);
    return (0);
    }

  /* Get the com status				*/	
  ClearCommError(hCom, &dwError, &cs);
  if (dwError)
    {
    sm_log_error(port,SD_FAILURE,"Unable to get status for",__FILE__,__LINE__);
    return (0);
    }
  return ((ST_INT) cs.cbInQue);
  }

/************************************************************************/
/*                               sm_tx_cnt                              */
/* PURPOSE : This function is to get the number of characters in the 	*/
/* transmit buffer							*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : number of characters in the transmit buffer			*/
/************************************************************************/
ST_INT sm_tx_cnt (ST_INT port)
  {
HANDLE hCom;
COMSTAT cs;
DWORD dwError;

  hCom = getHCom(port);
  if (hCom == NULL)
    {
    sm_log_error(port,SD_FAILURE,"Uninitialized port",__FILE__,__LINE__);
    return (0);
    }

  /* Get the com status				*/	
  ClearCommError(hCom, &dwError, &cs);
  if (dwError)
    {
    sm_log_error(port,SD_FAILURE,"Unable to get status for",__FILE__,__LINE__);
    return (0);
    }
  return ((ST_INT) cs.cbOutQue);
  }
/************************************************************************/
/*                      sm_get                                          */
/* PURPOSE : This function is used to read a buffer from a comm port	*/
/* The buffer will be allocated and freed by ADLC			*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* 	   packet - The pointer where data is to be read from the port	*/
/*	   toRead - the length of the buffer to be read from the port.	*/
/*	   bytesRead - Bytes actual read				*/
/* RETURN  : SD_SUCCESS - data was read OK.				*/
/*	     SD_FAILURE - no data was read				*/
/************************************************************************/
ST_RET sm_get (ST_INT port,ST_CHAR *packet, ST_INT toRead, ST_INT *bytesRead)
  {
DWORD nBytesRead;
DWORD nToRead;
HANDLE hCom;
BOOL fSuccess;
DWORD dwError;
COMSTAT cs;

  hCom = getHCom(port);
  if (hCom == NULL)
    return sm_log_error(port,SD_FAILURE,"Uninitialized port",__FILE__,__LINE__);

  /* Get the com status				*/	
  ClearCommError(hCom, &dwError, &cs);
  if (dwError)
    return sm_log_error(port,SD_FAILURE,"Unable to get status for",__FILE__,__LINE__);

  /* if too many bytes to read just read the maximum		*/
  if (cs.cbInQue > (DWORD) toRead)
    nToRead = toRead;
  else
    nToRead = cs.cbInQue;

  /* if no bytes to read, return */
  if (nToRead == 0)
    {
    *bytesRead = 0;
    return (SD_SUCCESS);
    }

  nBytesRead = 0;
  fSuccess = ReadFile(hCom, packet, nToRead, &nBytesRead,NULL);
  if (!fSuccess)
    return sm_log_error(port,SD_FAILURE,"Unable to read",__FILE__,__LINE__);

  *bytesRead = (ST_INT) nBytesRead;

  return (SD_SUCCESS);
  }

/************************************************************************/
/*                      sm_getc                                         */
/* PURPOSE : This function is used to receive a character from the port	*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : -1 no character available					*/
/* 	    >= 0 byte received						*/
/************************************************************************/

ST_INT sm_getc (ST_INT port)
  {
ST_CHAR ch;
ST_RET ret;
ST_INT bytesRead;

  ret = sm_get (port, &ch, 1, &bytesRead);
  if (ret || bytesRead == 0)
    return (-1);
  else 
    return ((ST_INT) ch & 0xFF); /* because of sign extension		*/
  }

/************************************************************************/
/*                      sm_rx_flush                                     */
/* PURPOSE : This function is used to flush out the receive buffer	*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : ST_VOID							*/
/************************************************************************/

ST_VOID sm_rx_flush (ST_INT port)
  {
  }

/*-----------------------------------------------------------------------
 * Following are the functions to set or clear different control signals
 *----------------------------------------------------------------------*/

/************************************************************************/
/*                      sm_set_signals                                  */
/* PURPOSE : This function pulls the DTR and RTS signals high          	*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : SD_SUCCESS - control lines were set OK			*/
/*	    SD_FAILURE - control lines were not set OK			*/
/************************************************************************/

ST_RET sm_set_signals (ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (SD_FAILURE);

/* Fill in the DCB							*/
  dcb.fRtsControl = RTS_CONTROL_ENABLE;
  dcb.fDtrControl = DTR_CONTROL_ENABLE;
  
  return MySetCommState(port, hCom, &dcb);
  }

/************************************************************************/
/*                      sm_clear_signals                                */
/* PURPOSE : This function pulls the DTR and RTS signals low            */
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : SD_SUCCESS - control lines were cleared OK			*/
/* 	    SD_FAILURE - control lines were not cleared OK		*/
/************************************************************************/

ST_RET sm_clear_signals (ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (SD_FAILURE);

/* Fill in the DCB							*/
  dcb.fRtsControl = RTS_CONTROL_DISABLE;
  dcb.fDtrControl = DTR_CONTROL_DISABLE;
  
  return MySetCommState(port, hCom, &dcb);
  }

/************************************************************************/
/*                      sm_set_rts                                  	*/
/* PURPOSE : This function pulls the RTS signal high			*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : SD_SUCCESS - RTS line was set OK				*/
/*	    SD_FAILURE - RTS line was not set OK			*/
/************************************************************************/

ST_RET sm_set_rts (ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (SD_FAILURE);

/* Fill in the DCB							*/
  dcb.fRtsControl = RTS_CONTROL_ENABLE;
  
  return MySetCommState(port, hCom, &dcb);
  }

/************************************************************************/
/*                      sm_clear_rts                                  	*/
/* PURPOSE : This function pulls the RTS signal low			*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : SD_SUCCESS - RTS line was cleared OK			*/
/*	    SD_FAILURE - RTS line was not cleared OK			*/
/************************************************************************/

ST_RET sm_clear_rts (ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (SD_FAILURE);

/* Fill in the DCB							*/
  dcb.fRtsControl = RTS_CONTROL_DISABLE;
  
  return MySetCommState(port, hCom, &dcb);
  }

/************************************************************************/
/*                      sm_set_dtr                                  	*/
/* PURPOSE : This function pulls the DTR signal high			*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : SD_SUCCESS - DTR line was set OK				*/
/*	    SD_FAILURE - DTR line was not set OK			*/
/************************************************************************/

ST_RET sm_set_dtr (ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (SD_FAILURE);

/* Fill in the DCB							*/
  dcb.fDtrControl = DTR_CONTROL_ENABLE;
  
  return MySetCommState(port, hCom, &dcb);
  }
/************************************************************************/
/*                      sm_clear_dtr                                  	*/
/* PURPOSE : This function pulls the DTR signal low			*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : SD_SUCCESS - DTR line was cleared OK			*/
/*	    SD_FAILURE - DTR line was not cleared OK			*/
/************************************************************************/

ST_RET sm_clear_dtr (ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (SD_FAILURE);

/* Fill in the DCB							*/
  dcb.fDtrControl = DTR_CONTROL_DISABLE;
  
  return MySetCommState(port, hCom, &dcb);
  }
/************************************************************************/
/*			sm_set_rts_clear_dtr				*/
/* PURPOSE : This function pulls the RTS signal high and DTR signal low */
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : SD_SUCCESS - RTS line was set and DTR was cleared OK	*/
/* 	    SD_FAILURE - RTS line was not set or DTR was not cleared OK	*/
/************************************************************************/
ST_RET sm_set_rts_clear_dtr(ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (SD_FAILURE);

/* Fill in the DCB							*/
  dcb.fRtsControl = RTS_CONTROL_ENABLE;
  dcb.fDtrControl = DTR_CONTROL_DISABLE;
  
  return MySetCommState(port, hCom, &dcb);
  }
/************************************************************************/
/*			sm_clear_rts_set_dtr				*/
/* PURPOSE : This function pulls the RTS signal low and DTR signal high */
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : SD_SUCCESS - RTS line was cleared and DTR was set OK	*/
/* 	    SD_FAILURE - RTS line was not cleared or DTR was not set OK	*/
/************************************************************************/
ST_RET sm_clear_rts_set_dtr(ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (SD_FAILURE);

/* Fill in the DCB							*/
  dcb.fRtsControl = RTS_CONTROL_DISABLE;
  dcb.fDtrControl = DTR_CONTROL_ENABLE;
  
  return MySetCommState(port, hCom, &dcb);
  }

/*-----------------------------------------------------------------------
 * Following are the functions to get status of different control signals
 *----------------------------------------------------------------------*/

/************************************************************************/
/*			sm_get_rts					*/
/* PURPOSE : This function gets the status of RTS signal 		*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : HIGH - RTS signal is turned on				*/
/* 	    LOW  - RTS signal is turned off				*/
/************************************************************************/
ST_INT sm_get_rts (ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (LOW);

  if (dcb.fRtsControl == RTS_CONTROL_ENABLE)
    return (HIGH);
  else
    return (LOW);
  }
/************************************************************************/
/*			sm_get_dtr					*/
/* PURPOSE : This function gets the status of DTR signal 		*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : HIGH - DTR signal is turned on				*/
/* 	    LOW  - DTR signal is turned off				*/
/************************************************************************/
ST_INT sm_get_dtr (ST_INT port)
  {
DCB dcb;
HANDLE hCom;

  if ((hCom = MyGetCommState(port,&dcb)) == NULL)
    return (LOW);

  if (dcb.fDtrControl == DTR_CONTROL_ENABLE)
    return (HIGH);
  else
    return (LOW);
  }
/************************************************************************/
/*			sm_get_cts					*/
/* PURPOSE : This function gets the status of CTS signal 		*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : HIGH - CTS signal is turned on				*/
/* 	    LOW  - CTS signal is turned off				*/
/************************************************************************/
ST_INT sm_get_cts (ST_INT port)
  {
DWORD modemStatus;
HANDLE hCom;

  if ((hCom = MyGetModemStatus(port,&modemStatus)) == NULL)
    return (LOW);

  if (modemStatus & MS_CTS_ON)
    return (HIGH);
  else
    return (LOW);
  }
/************************************************************************/
/*			sm_get_dsr					*/
/* PURPOSE : This function gets the status of DSR signal 		*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : HIGH - DSR signal is turned on				*/
/* 	    LOW  - DSR signal is turned off				*/
/************************************************************************/
ST_INT sm_get_dsr (ST_INT port)
  {
DWORD modemStatus;
HANDLE hCom;

  if ((hCom = MyGetModemStatus(port,&modemStatus)) == NULL)
    return (LOW);

  if (modemStatus & MS_DSR_ON)
    return (HIGH);
  else
    return (LOW);
  }
/************************************************************************/
/*			sm_get_dcd					*/
/* PURPOSE : This function gets the status of DCD signal 		*/
/* INPUT : port - communication port e.g. COM1 defined in adlc_sm.h	*/
/* RETURN : HIGH - DCD signal is turned on				*/
/* 	    LOW  - DCD signal is turned off				*/
/************************************************************************/
ST_INT sm_get_dcd (ST_INT port)
  {
DWORD modemStatus;
HANDLE hCom;

  if ((hCom = MyGetModemStatus(port,&modemStatus)) == NULL)
    return (LOW);

  if (modemStatus & MS_RLSD_ON)
    return (HIGH);
  else
    return (LOW);
  }
/************************************************************************/

/************************************************************************/
static ST_RET sm_log_error(ST_INT port, ST_RET errCode, ST_CHAR *str, ST_CHAR *file, ST_INT line)	
  {
DWORD dwError;

  dwError = GetLastError();
  ADLC_LOG_ERR3 ("ADLC ERROR SerialManager: %s COM%d (dwError=%ld) ",
                  str,port+1, dwError);
  ADLC_LOG_CERR2("            File = %s, Line = %d.",file, line);
  return (errCode);
  }
/************************************************************************/

/************************************************************************/
static HANDLE getHCom(ST_INT port)
  {
  if (port >= MAX_NUM_PORTS)
    return (NULL);
  return (smPCtrl[port].hCom);
  }
/************************************************************************/

/************************************************************************/
static ST_RET setHCom(ST_INT port, HANDLE hCom)
  {
  smPCtrl[port].hCom = hCom;
  return (SD_SUCCESS);
  }
/************************************************************************/
/* INPUT : port 							*/
/* OUTPUT hCom and dcb structure					*/
/************************************************************************/
static HANDLE MyGetCommState (ST_INT port, DCB *pstDcb)
  {
HANDLE hCom;
BOOL fSuccess;

  hCom = getHCom(port);
  if (hCom == NULL)
    {
    sm_log_error(port,SD_FAILURE,"Uninitialized port",__FILE__,__LINE__);
    return (NULL);
    }

  fSuccess = GetCommState(hCom, pstDcb);
  if (!fSuccess)
    {
    sm_log_error(port,SD_FAILURE,"Unable to get settings for",__FILE__,__LINE__);
    return (NULL);
    }

  return (hCom);
  }
/************************************************************************/
/* INPUT :  port, hCom , dcb						*/
/* RETURNS : SD_SUCCESS or SD_FAILURE					*/
/************************************************************************/
static ST_RET MySetCommState(ST_INT port, HANDLE hCom, DCB *pstDcb)
  {
BOOL fSuccess;

/* Set the control setting for this port				*/
  fSuccess = SetCommState(hCom, pstDcb);
  if (!fSuccess)
    return sm_log_error(port, SD_FAILURE,"Unable to set settings for",__FILE__,__LINE__);

  return (SD_SUCCESS);
  }
/************************************************************************/
/* INPUT : port 							*/
/* OUTPUT hCom and modemStatus						*/
/************************************************************************/
static HANDLE MyGetModemStatus (ST_INT port, DWORD *pModemStatus)
  {
HANDLE hCom;
BOOL fSuccess;

  hCom = getHCom(port);
  if (hCom == NULL)
    {
    sm_log_error(port,SD_FAILURE,"Uninitialized port",__FILE__,__LINE__);
    return (NULL);
    }

  fSuccess = GetCommModemStatus(hCom, pModemStatus);
  if (!fSuccess)
    {
    sm_log_error(port,SD_FAILURE,"Unable to get status for",__FILE__,__LINE__);
    return (NULL);
    }

  return (hCom);
  }

/************************************************************************/

/************************************************************************/
static ST_RET convertForDcb(ST_INT baud,DWORD *BaudRate,
			      ST_INT parity, BYTE *Parity,
			      ST_INT data, BYTE *ByteSize,
			      ST_INT stop, BYTE *StopBits)
  {
ST_RET ret = SD_SUCCESS;

  switch (baud)
    {
    case BAUD2400   : 
      *BaudRate = 2400;
    break;
    case BAUD4800   : 
      *BaudRate = 4800;
    break;
    case BAUD9600   : 
      *BaudRate = 9600;
    break;
    case BAUD19200  : 
      *BaudRate = 19200;
    break;
    case BAUD38400  : 
      *BaudRate = 38400;
    break;
    case BAUD57K :
      *BaudRate = 57600;
    break;
    case BAUD115K :
      *BaudRate = 115200;
    break;
    case BAUD56K :
      *BaudRate = 56000;
    break;
    case BAUD128K :
      *BaudRate = 128000;
    break;
    case BAUD256K :
      *BaudRate = 256000;
    break;

    default 	    : 
      ret = SD_FAILURE; 	      
    break;
    }
  if (ret)
    return (ret);
  
  switch (data)
    {
    case DATA7 :
      *ByteSize = 7;
    break;      
    case DATA8 :
      *ByteSize = 8;
    break;      
    default 	    : 
      ret = SD_FAILURE; 	      
    break;
    }

  if (ret)
    return (ret);

  switch(parity)
    {
    case NOPAR :
      *Parity = NOPARITY;	
    break;
    case ODDPAR :
      *Parity = ODDPARITY;	
    break;
    case EVENPAR :
      *Parity = EVENPARITY;	
    break;
    default 	    : 
      ret = SD_FAILURE; 	      
    break;
    }
  if (ret)
    return (ret);

  switch (stop)
    {
    case STOP1 :
      *StopBits = ONESTOPBIT;
    break;
    case STOP2 :
      *StopBits = TWOSTOPBITS;
    break;
    default : 
      ret = SD_FAILURE; 	      
    break;
    }
  return (ret);
  }
