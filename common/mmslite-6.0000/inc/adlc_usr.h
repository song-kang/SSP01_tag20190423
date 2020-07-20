/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-1997, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_usr.h						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*									*/
/* MODULE DESCRIPTION :	This header file defines data structures	*/
/*			and types for ADLC				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/05/03  EJV    21     Chg mms_debug_sel to ST_UINT, and deleted	*/
/*			     L suffix from MMS_LOG_... masks.		*/
/* 10/17/01  JRB    20     Del #ifdef UCA_SMP				*/
/* 02/04/99  JRB    19     Added adlcStartLinkMan..			*/
/* 10/08/98  MDE    18     Removed ADLC_MASK_LOG_ALWAYS			*/
/* 10/21/97  IKE    17     Changed OSI_ADDR to LOCAL_ADDR in error codes*/
/* 10/14/97  IKE    16     Split into two modules and put all the ipc	*/
/*			   specific stuff into adlc_ipc.h		*/
/* 10/1/97   IKE    15     Removed ADLC_LM define if _WIN32 or OS2 is 	*/
/*			   defined					*/
/* 09/26/97  IKE    14     Added ADLC_M_LOCAL_BASE_ADDR 		*/
/* 09/22/97  IKE    13     Added SMP(Station Managment Protocol) support*/ 
/* 09/18/97  IKE    12     Added telephone support			*/
/* 07/16/97  IKE    11     Added support for new ADLC commands		*/
/*                         adlcGetDevStats and adlcFilterDevLog.	*/
/* 05/12/97  IKE    10     Changed to MMSEASE 7.0 Data Types		*/
/* 04/10/97  EJV    09     Replaced error codes base 0x06xx with 0xA6xx */
/* 03/13/97  EJV    08     Changes for BRIDGE-132-095 (COGW,RSGW,MCGW):	*/
/*                           - added adlcLogState function;		*/
/* 02/24/97  IKE    07     Moved the log masks defn from adlc_log.h	*/
/* 01/22/97  IKE    06     Put prototypes for adlcOpenInterface and 	*/
/*			   adlcCloseInterface				*/
/* 12/03/96  IKE    05     WIN32 Support				*/
/* 09/30/96  IKE    04     Removed ifdef PLAIN_DOS for adlcRead func	*/
/*			   so Lean-t stack can use same funct for OS2 	*/
/* 05/22/96  IKE    03     Added some more error codes			*/
/* 04/04/96  rkr    02     Cleanup					*/
/* 01/02/96  IKE    01     Created					*/
/************************************************************************/

#ifndef ADLC_USR_H
#define ADLC_USR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "adlc_sts.h"

#include "smp_usr.h"

#define ADLC_ALL_ES 0x8080		/* ES-Hello Multicast Address	*/
#define ADLC_ALL_IS 0x8081		/* IS-Hello Multicast Address 	*/
#define ALL_STATIONS_ADDR 0xFFFF	/* Broadcast Address		*/
#define NO_STATION_ADDR 0x0000		/* No station address		*/
#define ADLC_M_BASE_LOC_ADDR 0x2000     /* For Adlc Master local Addr	*/
#define ADLC_USR_M_BASE_LOC_ADDR 0x3000 /* For user apps local adlc addr*/

#define CMD_BASE_ADLC_LM		600

/****************************************************************/
/* Log Control							*/
/****************************************************************/
/* adlc logging  mask						*/
#define ADLC_MASK_LOG_ERR      		0x00010000
#define ADLC_MASK_LOG_NERR     		0x00020000
#define ADLC_MASK_LOG_FLOW     		0x00040000
#define ADLC_MASK_LOG_IO       		0x00080000
#define ADLC_MASK_LOG_HEXIO        	0x00100000
#define ADLC_MASK_LOG_DEBUG        	0x00200000
#define ADLC_MASK_LOG_HEXDEBUG     	0x00400000
#define ADLC_MASK_LOG_SM       		0x01000000
#define ADLC_MASK_LOG_HEXSM        	0x02000000
#define ADLC_MASK_LOG_INFO_FRAME   	0x04000000
#define ADLC_MASK_LOG_HEXINFO_FRAME     0x08000000
#define ADLC_MASK_LOG_USER     		0x10000000
#define ADLC_MASK_LOG_HEXUSER      	0x20000000

extern ST_UINT adlc_debug_sel;
/****************************************************************/
/* ADLC Command OpCodes							*/
/****************************************************************/

#define ADLC_CMD_CONNECT_NODE			(CMD_BASE_ADLC_LM + 1)
#define ADLC_CMD_DISCONNECT_NODE		(CMD_BASE_ADLC_LM + 2)
#define ADLC_CMD_CONNECT_NODE_DONE		(CMD_BASE_ADLC_LM + 3)
#define ADLC_CMD_DISCONNECT_NODE_DONE		(CMD_BASE_ADLC_LM + 4)
#define ADLC_CMD_CONNECT_ALL			(CMD_BASE_ADLC_LM + 5)
#define ADLC_CMD_DISCONNECT_ALL			(CMD_BASE_ADLC_LM + 6)
#define ADLC_CMD_WRITE_PACKET			(CMD_BASE_ADLC_LM + 7)
#define ADLC_CMD_GET_CON_STATUS			(CMD_BASE_ADLC_LM + 8)
#define ADLC_CMD_ADD_DEV			(CMD_BASE_ADLC_LM + 9)
#define ADLC_CMD_REMOVE_DEV			(CMD_BASE_ADLC_LM + 10)
#define ADLC_CMD_START_POLL_DEV			(CMD_BASE_ADLC_LM + 11)
#define ADLC_CMD_STOP_POLL_DEV			(CMD_BASE_ADLC_LM + 12)
#define ADLC_CMD_CHANGE_POLL_RATE		(CMD_BASE_ADLC_LM + 13)
#define ADLC_CMD_QUERY_POLL_INFO		(CMD_BASE_ADLC_LM + 14)

/* #if ADLC_LM	*/
#define ADLC_CMD_SET_SM_SLEEP			(CMD_BASE_ADLC_LM + 15)
#define ADLC_CMD_GET_SM_SLEEP			(CMD_BASE_ADLC_LM + 16)
#define ADLC_CMD_GET_DEV_STATS			(CMD_BASE_ADLC_LM + 17)
#define ADLC_CMD_FILTER_DEV_LOG			(CMD_BASE_ADLC_LM + 18)
#define ADLC_CMD_ADD_PORT			(CMD_BASE_ADLC_LM + 19)
#define ADLC_CMD_REMOVE_PORT			(CMD_BASE_ADLC_LM + 20)
#define ADLC_CMD_TEL_DIAL			(CMD_BASE_ADLC_LM + 21)
#define ADLC_CMD_TEL_DIAL_DONE			(CMD_BASE_ADLC_LM + 22)
#define ADLC_CMD_TEL_HANGUP			(CMD_BASE_ADLC_LM + 23)
#define ADLC_CMD_TEL_HANGUP_DONE		(CMD_BASE_ADLC_LM + 24)
/* #endif ADLC_LM */

#define ADLC_CMD_EXT_WRITE_PACKET     		(CMD_BASE_ADLC_LM + 25)
#define ADLC_CMD_EXT_WRITE_PACKET_DONE		(CMD_BASE_ADLC_LM + 26)

#define ADLC_CMD_WRITE_PACKET_DONE		(CMD_BASE_ADLC_LM + 27)


#define ADLC_DEV_NAME_SIZE	80

/************************************************************************/  
/* 			ADLC_CONNECT_NODE				*/
/* This is a bidirectional event					*/
/* This could be an event from stack to ADLC and vice versa		*/
/************************************************************************/  
typedef struct tagAdlcConnectNode
  {
  struct
    {
    ST_UINT16 localAddr;		/* OSI node address			*/
    ST_UINT16 remoteAddr; 		/* ADLC device address		*/
    ST_ULONG  usr1;			/* user data			*/
    } i;
  } ADLC_CONNECT_NODE;

/************************************************************************/  
/* 			ADLC_DISCONNECT_NODE			*/
/* This is a bidirectional event					*/
/* This could be an event from stack to ADLC and vice versa		*/
/************************************************************************/  

typedef ADLC_CONNECT_NODE ADLC_DISCONNECT_NODE;


/************************************************************************/  
/* 			ADLC_CONNECT_NODE_DONE				*/
/* This is event from ADLC to stack					*/
/************************************************************************/  

typedef ADLC_CONNECT_NODE ADLC_CONNECT_NODE_DONE;

/************************************************************************/  
/* 			ADLC_DISCONNECT_NODE_DONE			*/
/* This is event from ADLC to stack					*/
/************************************************************************/  
typedef ADLC_CONNECT_NODE ADLC_DISCONNECT_NODE_DONE;

/************************************************************************/  
/* 			ADLC_CONNECT_ALL				*/
/* This is an event from stack to ADLC					*/
/************************************************************************/  
typedef struct tagAdlcConnectAll
  {
  struct
    {
    ST_UINT16 localAddr;		/* OSI node address			*/
    ST_ULONG  usr1;			/* user data			*/
    } i;
  } ADLC_CONNECT_ALL;

/************************************************************************/  
/* 			ADLC_DISCONNECT_ALL				*/
/* This is an event from stack to ADLC					*/
/************************************************************************/  
typedef ADLC_CONNECT_ALL ADLC_DISCONNECT_ALL;

/************************************************************************/  
/* 			ADLC_WRITE_PACKET				*/
/* This is a bidirectional event					*/
/* This could be an event from stack to ADLC and vice versa		*/
/************************************************************************/  
typedef struct tagAdlcWritePacket
  {
  struct				/* input parameters		*/
    {
    ST_UINT16	remoteAddr;		/* device where data is going 	*/
    ST_UINT16	localAddr;		/* device where data comes from	*/
    ST_INT	dataSize;		
    ST_VOID	*data;			/* allocated memory		*/
    ST_ULONG  usr1;			/* user data			*/
    } i;
  } ADLC_WRITE_PACKET;

/************************************************************************/  
/* 			ADLC_WRITE_PACKET_DONE				*/
/* This is an event from ADLC to stack					*/
/************************************************************************/  
typedef ADLC_WRITE_PACKET ADLC_WRITE_PACKET_DONE;

/************************************************************************/  
/*			ADLC_GET_CON_STATUS				*/
/* This is an event from any task to ADLC				*/
/************************************************************************/  

#define ADLC_CON_STATUS_ONLINE		1
#define ADLC_CON_STATUS_OFFLINE		2
#define ADLC_CON_STATUS_UNKNOWN		3

typedef struct tagAdlcGetConStat
  {
  struct				/* input parameters		*/
    {
    ST_UINT16	remoteAddr;		/* ADLC device /remote 	addr	*/
    ST_UINT16	localAddr;		/* OSI node / local addr	*/
    } i;
  struct				/* output parameters		*/
    {
    ST_INT conStatusOut;
    } o;
  }  ADLC_GET_CON_STATUS;

#define ADLC_NO_POLLING		0
#define ADLC_POLL_FREQ_HI	1
#define ADLC_POLL_FREQ_MED	2
#define ADLC_POLL_FREQ_LOW	3

/************************************************************************/  
/*			ADLC_ADD_DEV					*/
/* This is an event from any task to ADLC				*/
/************************************************************************/  
typedef struct tagAdlcAddDev
  {
  struct				/* input parameters		*/
    {
    ST_CHAR	devName[ADLC_DEV_NAME_SIZE+1];
    ST_UINT16	address;
    ST_INT	pollGroup;		/* defined above		*/
    ST_INT	port;			/* what port it belongs to	*/		
    ST_BOOLEAN     connectAll;  /* should we try to connect it if connectAll issued */ 
    ST_ULONG  usr1;			/* user data			*/
    } i;
  } ADLC_ADD_DEV;

/************************************************************************/  
/*			ADLC_REMOVE_DEV					*/
/* This is an event from any task to ADLC				*/
/************************************************************************/  
typedef struct tagAdlcRemoveDev
  {
  struct				/* input parameters		*/
    {
    ST_CHAR devName[ADLC_DEV_NAME_SIZE+1];
    ST_ULONG  usr1;			/* user data			*/
    } i;
  } ADLC_REMOVE_DEV;

/************************************************************************/  
/*			ADLC_START_POLL_DEV				*/
/* This is an event from any task to ADLC				*/
/************************************************************************/  
typedef struct tagAdlcStartPollDev
  {
  struct				/* input parameters		*/
    {
    ST_CHAR devName[ADLC_DEV_NAME_SIZE+1];
    ST_ULONG  usr1;			/* user data			*/
    } i;
  } ADLC_START_POLL_DEV;

/************************************************************************/  
/*			ADLC_STOP_POLL_DEV				*/
/* This is an event from any task to ADLC				*/
/************************************************************************/  
typedef struct tagAdlcStopPollDev
  {
  struct				/* input parameters		*/
    {
    ST_CHAR devName[ADLC_DEV_NAME_SIZE+1];
    ST_ULONG  usr1;			/* user data			*/
    } i;
  } ADLC_STOP_POLL_DEV;

/************************************************************************/  
/*			ADLC_CHANGE_POLL_RATE				*/
/* This is an event from any task to ADLC				*/
/************************************************************************/  
/* Notes on changing the poll rate for a device:
	To poll the device more frequently set 
		pollFreqDelta = ADLC_INC_POLL_FREQ;
	The device will be moved to the poll group that polls more 
	frequently.

	Example:  current pollGroup=3 with pollRate=n seconds
	          new     pollGroup=2 with pollRate=n-m seconds		

	Reverse the above discussion to decrease the polling frequency.	*/

#define ADLC_INC_POLL_FREQ	1
#define ADLC_DEC_POLL_FREQ	2

typedef struct tagAdlcChangePollRate
  {
  struct				/* input parameters		*/
    {
    ST_CHAR 	devName[ADLC_DEV_NAME_SIZE+1];
    ST_INT	pollFreqDelta;		/* change to poll freq as 	*/
    ST_ULONG  usr1;			/* user data			*/
    } i;				/* defined above		*/
  } ADLC_CHANGE_POLL_RATE;

/************************************************************************/  
/*			ADLC_QUERY_POLL_INFO				*/
/* This is an event from any task to ADLC				*/
/************************************************************************/  
/* Notes on Polling:
	Poll groups range from 1 to n (as of 01/17/96 n=3).  All devices
	associated with a poll group are polled at the pollRate set for
	the poll group.  The smaller the poll group the more frequently
	the devices are polled.						*/

typedef struct tagQueryPollInfo
  {
  struct				/* input parameters		*/
    {
    ST_CHAR 	devName[ADLC_DEV_NAME_SIZE+1];
    ST_ULONG  usr1;			/* user data			*/
    } i;
  struct
    {
    ST_INT	pollGroup;		/* current poll group for device*/
    ST_DOUBLE	pollRate;		/* in mili seconds		*/
    ST_BOOLEAN	bPollingSuspended;	/* polling was stopped by user	*/
    } o;
  } ADLC_QUERY_POLL_INFO;

/************************************************************************/  
/* 			ADLC_EXT_WRITE_PACKET				*/
/* This is a bidirectional event					*/
/* This could be an event from stack to ADLC and vice versa		*/
/************************************************************************/  
typedef struct tagAdlcExtWritePacket
  {
  struct				/* input parameters		*/
    {
    ST_UINT16	remoteAddr;		/* device where data is going 	*/
    ST_UINT16	localAddr;		/* device where data comes from	*/
    ST_INT	dataSize;		
    ST_VOID	*data;			/* allocated memory		*/
    ST_LONG     userId;			/* user passes for done func	*/
    ST_CHAR recTime[SMP_SIZE_OF_REC_TIME]; /* buffer to hold time stamp */
    ST_ULONG  usr1;			/* user data			*/
    } i;
  } ADLC_EXT_WRITE_PACKET;

/************************************************************************/  
/* 			ADLC_EXT_WRITE_PACKET_DONE			*/
/* This is event from ADLC to stack 					*/
/************************************************************************/  
typedef ADLC_EXT_WRITE_PACKET ADLC_EXT_WRITE_PACKET_DONE;

/* ADLC defined error codes. The error base 0x3600 is defined in glberror.h */

#define E_ADLC_INVALID_DEV_NAME		0x3601
#define E_ADLC_INVALID_DEV_ADDR		0x3602
#define E_ADLC_DEVICE_EXISTS		0x3603
#define E_ADLC_DEV_DOES_NOT_EXIST	0x3604

#define E_ADLC_INVALID_LOCAL_ADDR	0x3605
#define E_ADLC_INVALID_CON_PAIR		0x3606
#define E_ADLC_CON_EXISTS		0x3607
#define E_ADLC_CON_DOES_NOT_EXIST	0x3608
#define E_ADLC_TX_BUF_FULL		0x3609

#define E_ADLC_CANT_POLL_FASTER		0x360A
#define E_ADLC_CANT_POLL_SLOWER		0x360B
#define E_ADLC_INVALID_POLL_DELTA	0x360C
#define E_ADLC_INTERNAL_ERR		0x360D
#define E_ADLC_DEVICE_NOT_POLLED	0x360E
#define E_ADLC_INVALID_HQUEUE		0x360F
#define E_ADLC_INVALID_PORT		0x3610
#define E_ADLC_INVALID_POLL_FREQ	0x3611
#define E_ADLC_DUPLICATE_LOCAL_ADDR	0x3612
#define E_ADLC_PACKET_LEN_NOT_VALID	0x3613

#define E_ADLC_RESPONSE_TIMEOUT		0x3614
#define E_ADLC_INVALID_CMD		0x3615
#define E_ADLC_CANT_INIT_PORT		0x3616

#define E_ADLC_CREATE_THREAD		0x3617
#define E_ADLC_CANT_INIT_TAPI 		0x3618
#define E_ADLC_TEL_LINE_UNAVAIL 	0x3619
#define E_ADLC_CANT_INIT_TEL_LINE  	0x361A

#define E_ADLC_PORT_EXISTS		0x361B
#define E_ADLC_PORT_DOES_NOT_EXIST	0x361C

#define E_ADLC_SERIAL_TX		0x361D
#define E_ADLC_SERIAL_RX		0x361E

#define E_ADLC_CANT_OPEN_DB		0x361F
ST_RET adlcWrite(ADLC_WRITE_PACKET *adlcPacket);
ADLC_WRITE_PACKET *adlcRead(ST_VOID);
ST_VOID *adlcGetEvent(ST_INT *opcode, ST_RET *retCode);
ST_RET adlcRegisterClient (ST_UINT16 localAddr);

/* routines to service ADLC commands					*/
ST_RET adlcStartLinkManager (ST_VOID);
ST_RET adlcInit(ST_VOID);
ST_RET adlcConnectNode(ADLC_CONNECT_NODE *pstConNode);
ST_RET adlcDisconnectNode(ADLC_DISCONNECT_NODE *pstDiscNode);
ST_RET adlcConnectAll(ADLC_CONNECT_ALL *pstConAll);
ST_RET adlcDisconnectAll(ADLC_DISCONNECT_ALL *pstDiscAll);
ST_RET adlcWritePacket(ADLC_WRITE_PACKET *adlcPacket);
ST_RET adlcGetConStatus(ADLC_GET_CON_STATUS *pstGetConStatus);
ST_RET adlcAddDev(ADLC_ADD_DEV *pstAddDev);
ST_RET adlcRemoveDev(ADLC_REMOVE_DEV *pstRemoveDev);
ST_RET adlcStartPollingDev(ADLC_START_POLL_DEV *pstStartPollDev);
ST_RET adlcStopPollingDev(ADLC_STOP_POLL_DEV *pstStopPollDev);
ST_RET adlcChangePollRate(ADLC_CHANGE_POLL_RATE *pstChangePollRate);
ST_RET adlcQueryPollInfo(ADLC_QUERY_POLL_INFO *pstQueryPollInfo);
ST_VOID adlcExit (ST_VOID);

ST_RET adlcExtWritePacket(const ADLC_EXT_WRITE_PACKET *pstExtWrite);

#ifdef __cplusplus
}
#endif	

#endif	/* end ADLC_USR */






