/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_sm.h						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* This module contains functions that provide for serial port communi-	*/
/* cations for the Digiboard.						*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*	ST_INT sm_init(port,baud,parity,data,stop)			*/
/*	ST_INT sm_exit(port)						*/
/*      ST_INT sm_put (port,len,buffer)                             	*/
/* 	ST_INT sm_putc (port,ch)					*/
/*	ST_INT sm_rx_cnt (port)						*/
/*	ST_INT sm_get (port,packet,toRead,bytesRead) 			*/
/*      ST_INT sm_getc (port)                                       	*/
/*      ST_INT sm_rx_flush(port)					*/
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev      Comments                                    */
/* --------  ---  ------   -------------------------------------------  */
/* 11/24/02  KCR    12     Added rdThreadError handling semaphore       */
/* 04/20/98  IKE    11     Moved u_smp_record_time in Serial Manager	*/
/* 10/06/97  IKE    10     Added more baud rates 			*/
/* 09/18/97  IKE    09     Added telephone support			*/
/* 05/12/97  IKE    08     Changed to MMSEASE 7.0 Data Types		*/
/* 02/03/97  IKE    07     adlcCfg per port basis			*/
/*			   CFG_PORT also for DOS so multiple ports can	*/
/*			   be initialized in dos platform		*/
/* 09/19/96  IKE    06     Rearranged sm_get and sm_set functions	*/
/* 08/22/96  IKE    05     Added sm_get_dcd for contention avoidance	*/
/* 06/11/96  IKE    04     Added more functions to check RS232 signals  */
/* 05/22/96  IKE    03     Added functions to control RS232 signals	*/
/* 04/04/96  rkr    02	   Cleanup					*/
/* 11/12/95  IKE    1      Created					*/
/************************************************************************/

#ifndef _ADLC_SM_H_
#define _ADLC_SM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define     COM1        0                       /* Async Adapter COM1: */
#define     COM2        1                       /* Async Adapter COM2: */
#define     COM3        2                       /* Async Adapter COM3: */
#define     COM4        3                       /* Async Adapter COM4: */
#define     COM5        4                       /* Async Adapter COM5: */
#define     COM6        5                       /* Async Adapter COM6: */
#define     COM7        6                       /* Async Adapter COM7: */
#define     COM8        7                       /* Async Adapter COM8: */
#define     COM9        8                       /* Async Adapter COM9: */
#define     COM10       9                       /* Async Adapter COM10: */
#define     COM11       10 							/* ETC. */
#define     COM12       11 
#define     COM13       12 
#define     COM14       13 
#define     COM15       14 
#define     COM16       15 
#define     COM17       16 
#define     COM18       17 
#define     COM19       18 
#define     COM20       19 
#define     COM21       20 
#define     COM22       21 
#define     COM23       22 
#define     COM24       23 
#define     COM25       24 
#define     COM26       25 
#define     COM27       26 
#define     COM28       27
#define     COM29       28 
#define     COM30       29 
#define     COM31       30 
#define     COM32       31 
#define     COM33       32 
#define     COM34       33 

#define     LOW		0
#define     HIGH	1

ST_RET sm_init (ST_INT,ST_INT,ST_INT,ST_INT,ST_INT);
ST_RET sm_exit (ST_INT);
ST_RET sm_put (ST_INT,ST_INT, ST_CHAR *);
ST_RET sm_putc (ST_INT, ST_CHAR);
ST_INT sm_rx_cnt (ST_INT port);
ST_INT sm_tx_cnt (ST_INT port);
ST_RET sm_get (ST_INT,ST_CHAR *,ST_INT,ST_INT *);
ST_INT sm_getc (ST_INT);
ST_VOID sm_rx_flush (ST_INT);
ST_RET sm_set_signals (ST_INT port);
ST_RET sm_clear_signals (ST_INT port);
ST_RET sm_set_rts (ST_INT port);
ST_RET sm_clear_rts (ST_INT port);
ST_RET sm_set_dtr (ST_INT port);
ST_RET sm_clear_dtr (ST_INT port);
ST_RET sm_set_rts_clear_dtr(ST_INT port);
ST_RET sm_clear_rts_set_dtr(ST_INT port);
ST_INT sm_get_rts (ST_INT port);
ST_INT sm_get_dtr (ST_INT port);
ST_INT sm_get_cts (ST_INT port);
ST_INT sm_get_dsr (ST_INT port);
ST_INT sm_get_dcd (ST_INT port);

#if !defined(ADLC_LM)
/*  The INIT (ah=00, Int 14h) communications parameters */

#define     BAUD110     0                       /* Baud rate 110 */
#define     BAUD150     1                       /* Baud rate 150 */
#define     BAUD300     2                       /* Baud rate 300 */
#define     BAUD600     3                       /* Baud rate 600 */
#define     BAUD1200    4                       /* Baud rate 1200 */
#define     BAUD2400    5                       /* Baud rate 2400 */
#define     BAUD4800    6                       /* Baud rate 4800 */
#define     BAUD9600    7                       /* Baud rate 9600 */
#define     BAUD19200   8                       /* Baud rate 19200 */
#define     BAUD38400   9                       /* Baud rate 38400 */
#define     BAUD57K  	10                      /* Baud rate 57.4K */
#define     BAUD115K 	11                      /* Baud rate 115.2K */
#define     BAUD56K  	12                      /* Baud rate 56K  */
#define     BAUD128K  	13                      /* Baud rate 128K  */
#define     BAUD256K  	14                      /* Baud rate 256K  */

#define     DATA7       0                       /* Data bits 7 */
#define     DATA8       1                       /* Data bits 8 */

#define     NOPAR       0                       /* Parity none */
#define     ODDPAR      1                       /* Parity odd */
#define     EVENPAR     2                       /* Parity even */

#define     STOP1       0                       /* Stop bits 1 */
#define     STOP2       1                       /* Stop bits 2 */

#endif /* !ADLC_LM */

#if defined(ADLC_LM)
/* Request OpCode Definitions						*/
#define SM_INITIALIZE_PORT_REQ		0	/* open the port	*/
#define SM_TERMINATE_PORT_REQ		1	/* close the port	*/
#define SM_READ_REQ			2	/* read from the port	*/
#define SM_WRITE_REQ			3	/* write to the port	*/
#define SM_IN_BYTE_COUNT_REQ		4	/* check the input Q	*/
#define SM_OUT_BYTE_COUNT_REQ		5	/* check the output Q	*/
#define SM_GET_STATUS_REQ		6	/* get modem status	*/
#define SM_SET_STATUS_REQ		7	/* set modem status	*/
#define SM_RX_FLUSH_REQ			8	/* flush recieve buffer	*/
#define SM_GET_FRAME_REQ		9	/* respond to frameArr  */

/* Request Error Code Definitions					*/
#define SM_NO_ERR			0
#define SM_INIT_ERR			1
#define SM_TX_ERR			2
#define SM_RX_ERR			3
#define SM_INVALID_REQ			4
#define SM_PORT_ALREADY_OPEN		5
#define SM_OPEN_ERROR			6
#define SM_SET_BAUD_FAILURE		7
#define SM_SET_LINE_FAILURE		8
#define SM_SET_FLOW_FAILURE		9
#define SM_PORT_NOT_OPEN		10
#define SM_INVALID_PARAM		11
#define SM_ERROR			12
#define SM_NO_FRAMES_AVAILABLE		13
#define SM_NO_WRITE_CONF_AVIAL		14
#define SM_SEM_OPEN_FAILURE		15
#define SM_TERMINATED			16

/* Request Status Code Definitions					*/
#define SM_COMPLETED			0
#define SM_PENDING			1
#define SM_UNDERWAY			2
#define SM_ABORTED			3	
#define SM_FAILED			4
#define SM_TX_RX_IN_PROCESS		5	/* pend TX & RX on terminate*/
#define SM_TX_IN_PROCESS		6	/* pending TX on terminate */
#define SM_RX_IN_PROCESS		7	/* pending RX on terminate */

/* Request Specific Structures						*/
typedef struct smRdwrReq
  {
  ST_INT len;			/* num of bytes to read or write 	*/
  ST_CHAR *data;			/* user allocates and frees for read or	*/
  }SM_RDWR_REQ;			/* write				*/

typedef struct smFrameReq
  {
  ST_INT port;			/* port the frame was received from	*/
  ST_INT len;			/* frame length				*/
  ST_CHAR    *data;		/* SM allocates - user frees!!!		*/
#if defined(UCA_SMP)
  ST_CHAR recTime[SMP_SIZE_OF_REC_TIME]; /* buffer to hold time stamp */
#endif
  }SM_FRAME_REQ;

typedef struct smLineParam
  {
  ST_INT baudRate;	/* 300, 1200, 4800, 9600, 19.2, 38.4, 57.6 	*/
  ST_INT dataBits;	/* 5, 6, 7, 8					*/
  ST_INT parity;	/* 0=none, 1=odd, 2=even, 3=mark, 4=space	*/
  ST_INT stopBits;	/* 0=>1, 1=>1.5 stop bits, 2=>2			*/
  }SM_LINE_PARAM;

typedef struct smHandShake
  {
  ST_BOOLEAN enableDTR;		/* enable DTR input handshaking		*/
  ST_BOOLEAN enableCTS;		/* enable CTS output handshaking	*/
  ST_BOOLEAN enableDSRout;		/* enable DSR output handshaking	*/
  ST_BOOLEAN enableDCD;		/* enable DCD output handshaking	*/
  ST_BOOLEAN enableDSRin;		/* enable DSR input sensitivity		*/
  ST_BOOLEAN enableRTS;		/* enable RTS input handshaking		*/
  }SM_HAND_SHAKE;

typedef struct smFlowCtrl
  {
  ST_BOOLEAN enableTxFlow;		/* enable auto transmit XON/XOFF	*/
  ST_BOOLEAN enableRxFlow;		/* enable auto receive XON/XOFF		*/
  ST_UCHAR   xonChar;		/* usually 0x11				*/
  ST_UCHAR   xoffChar;		/* usually 0x13				*/
  }SM_FLOW_CTRL;

typedef struct smInitReq
  {
  SM_LINE_PARAM	initLineParam;
  SM_HAND_SHAKE initHandShake;
  SM_FLOW_CTRL  initFlowCtrl;
  HANDLE hCom;
  ST_INT frameLen;
  ST_UCHAR sof;
  ST_UCHAR eof;
  HEV           txDoneSem;
  HEV		frameArrivalSem;
  HEV           rdThreadErrSem;
  }SM_INIT_REQ;

typedef struct smStatusReq
  {
  ST_BOOLEAN dtr;
  ST_BOOLEAN rts;
  ST_BOOLEAN dcd;
  ST_BOOLEAN cts;
  ST_BOOLEAN dsr;
  }SM_STATUS_REQ;

/************************************************************************/
/*				SM_REQ					*/
/* Every request to serial Manager uses this data structure		*/
/************************************************************************/
typedef struct smReq
  {
  struct dbl_lnk l;
  ST_INT opCode;		/* operation requested			*/
  ST_INT status;
  ST_INT errCode;
  ST_INT port;
  union
    {
    SM_RDWR_REQ	  readWriteReq;
    SM_INIT_REQ	  initReq;
    SM_STATUS_REQ setStatReq;
    } i;			/* input data				*/ 
  union
    {
    SM_RDWR_REQ       readReq;
    SM_STATUS_REQ     getStatReq;
    ST_INT	      byteCount;
    SM_FRAME_REQ      frameArrReq;
    }o;					/* output data			*/
  }SM_REQ;

/* serial manager user access functions					*/
ST_RET sm_get_status (ST_INT port,SM_REQ *pstReq);
ST_VOID sm_set_status(ST_INT port, SM_REQ *pstReq);
ST_RET _SmInitialize(ST_VOID);	/* startup code				*/
ST_RET _SmRequest(SM_REQ *);	/* service user requests		*/
ST_VOID _SmTerminate(ST_VOID);	/* cleanup code				*/

#ifdef ADLC_TESTING
ST_VOID _SmShowStats(ST_INT port);
#endif

#endif /* ADLC_LM */

/************************************************************************/
/*				CFG_PORT				*/
/* This data structure is used only when reading the configuration File	*/
/* and when opening a port, after that it is not used any more		*/
/************************************************************************/
typedef struct cfgPortStuff
  {
  struct dbl_lnk l;
  ST_INT port;
#if !defined(ADLC_LM)
  ST_INT baud;
  ST_INT parity;
  ST_INT data;
  ST_INT stop;
#endif
#if defined(ADLC_LM)
  SM_LINE_PARAM	stLineParam;
  SM_HAND_SHAKE stHandShake;
  SM_FLOW_CTRL  stFlowCtrl;
#endif
  }CFG_PORT;

#ifdef __cplusplus
}
#endif

#endif	/* adlc_sm.h already included	*/
