/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc.h							*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*	Function prototypes and commands definations as defined in	*/
/*	ISO/IEC 4335 for frame format					*/
/* Note : For ADLC context packet refers to information passing with 	*/
/* stack while frame refers to information passing with Serial Manager	*/
/*                                                                      */
/* This module can be divided into five parts               */
/* 1. First part deals with all the numeric defines and command defn    */
/*    as in ISO specs                           */
/* 2. This part deals with the defination of the data structures used   */
/* 3. This part has function like macros used to get and set various    */
/*    fields                                				*/
/* 4. This has the declaration of all the global variables as extern.   */
/*    All the global variables are declared in adlc_var.c       	*/ 
/* 5. This part has the prototypes of global functions shared between   */
/*    modules. All the functions are put separately according to the    */
/*    module it belongs to                      			*/
/* Note : For ADLC context packet refers to information passing with    */
/* stack while frame refers to information passing with Serial Manager  */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev     Comments                                     */
/* --------  ---  ------   -------------------------------------------  */
/* 11/24/02  KCR    35     Added rdThreadError handling semaphore       */
/* 09/22/02  KCR    34     Added paranthesis to macros          	*/
/* 06/15/01  JRB    33     ipcChk* macros call chk_*, not x_chk_*.	*/
/* 03/03/00  KCR    32     Exit Thread using events not TerminateThread() */
/* 10/01/99  NAV    31     Added more SD_CONST modifiers		*/
/* 09/13/99  MDE    30     Added SD_CONST modifiers			*/
/* 04/20/98  IKE    29     Moved u_smp_record_time in Serial Manager	*/
/* 03/05/98  IKE    28     Added Critical Sections to protect Adlc Data */
/*			   Removed adlcIpcxxx functions			*/
/* 02/04/98  IKE    27     Added deleteUIFramesFromTxList to fix bug	*/
/* 10/20/97  IKE    26     Added addToListTime for Link Manager		*/
/* 09/23/97  IKE    25     Added adlcExtWritePacket for define UCA_SMP	*/
/* 09/18/97  IKE    24     Added telephone support			*/
/* 07/16/97  IKE    23     Added device statistics and device filt log	*/
/* 06/12/97  IKE    22     Made ADLC protocol transaction oriented	*/
/* 05/12/97  IKE    21     Changed to MMSEASE 7.0 Data Types		*/
/* 02/24/96  IKE    20	   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/*			   CFG_PORT also for DOS so multiple ports can	*/
/*			   be initialized in dos platform		*/
/* 02/03/97  IKE    19     adlcCfg per port basis			*/
/* 01/23/97  IKE    18     Added txRxModeCounter for proper behaviour 	*/
/*			   of turnaround timers				*/
/* 01/17/97  IKE    17     Added defn of initSiscoUtilGlbVarsForAdlc	*/
/*			   and addToCfgPort				*/
/* 12/03/96  IKE    16     WIN32 Support, header files cleanup		*/ 
/*			   Added adlc_sm.h, some func prototypes for 	*/
/*			   adlc_tst.c					*/
/* 11/15/96  IKE    15     Added adlcInitGlbVars			*/
/* 11/06/96  IKE    14     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/28/96  KCR    13     Put ipcChkxxx macros to replace chk_xxx	*/
/* 09/30/96  IKE    12     Moved adlc_stats from adlc.h to adlc_usr.h 	*/
/*			   so other apps could reference it		*/
/*			   Added communicationMedia, put ifdef MASTER	*/
/*			   in ADLC_CFG struct for members that were used*/
/*			   for master only, added reconfigure option	*/
/* 09/17/96  IKE    11     Added some struct members to ADLC_CFG_INFO 	*/
/*			   and ADLC_PORT_INFO structure			*/
/* 08/22/96  IKE    10     Support for contention avoidance		*/
/*			   Changed > to >= in CHECK_TIMEOUT macro	*/
/* 08/16/96  IKE    09     Added rxMFramesList, vR and infoBufLen in 	*/
/*			   port struct (to make UI into INFO), added	*/
/* 			   turnaroundtime in cfg and device struct, 	*/
/*			   changed UPDATE_TIMEOUT			*/
/* 08/05/96  IKE    08     Added connectAll to device struct, mode to	*/
/*			   port struct and radioDelayTime to cfg struct	*/
/* 07/17/96  IKE    07     Removed some lint and C6.00 warnings by type	*/
/*			   casting to ST_UCHAR in some macros		*/
/* 06/20/96  KCR    06     Increased MAX_ADLC_FRAME_LEN from 256 to 2048*/
/* 06/18/96  IKE    05     Changed the type of configured timeout into 	*/
/*			   ST_DOUBLE. Added stackTaskId for DOS platform.  */
/*                 	   Changed time related macros to functions and	*/
/*			   moved to module adlc_tm.c			*/
/* 06/11/96  IKE    04     Added cnp_usr.h, frame timeout and SM_PUT    */ 
/* 05/22/96  IKE    03     Added defn for three types of run mode	*/
/*			   Modified xxx_ADDR_FIELD_OFFSET. Put suffix	*/
/*			   "ADLC_S" for link states for the ADLC 	*/
/*			   connection state machine  			*/
/*			   Added eventList and taskList			*/
/*			   Added prototypes of added functions		*/
/* 04/04/96  rkr    02     Cleanup					*/
/* 07/18/95  IKE    01     Initial release. 				*/
/************************************************************************/ 

#ifndef _ADLC_H_
#define _ADLC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gen_list.h"
#include "adlc_usr.h"
#if defined(ADLC_LM)
#include "adlc_ipc.h"
#endif
#include "adlc_log.h"
#include "adlc_sm.h"

#if defined(ADLC_TESTING)
#include <assert.h>
#define ADLC_ASSERT(f) assert(f);
#else
  #define ADLC_ASSERT(f)
#endif 

#if !defined(ADLC_LM)
/* At one time these macros called "x_chk_*" for DEBUG or		*/
/* nonDEBUG, to be sure that the semaphore was locked.			*/
/* Now "chk_*" always lock the sempaphore, so use them.			*/
#define ipcChkCalloc(x,y)   chk_calloc  (x,y)
#define ipcChkMalloc(x)     chk_malloc  (x)
#define ipcChkFree(x)       chk_free (x)
#endif

#ifdef CNP
#include "cnp_usr.h"
#endif

/************************************************************************/  
/* Constants related to configuration					*/
/************************************************************************/  
#define MAX_ADLC_REQS		10
#define MAX_ADLC_FRAME_LEN	2048 	/* 2048 = 2035 + 11 + 2  	*/

/* Following are the run mode for ADLC Master				*/
#define POLLED_MODE			0	
#define ENHANCED_POLLED_MODE		1	
#define CONTENTION_MODE			2

/* Following are the communication media for ADLC */
#define CABLE_MEDIA		0
#define RADIO_MEDIA		1

#define FRAME_FORMAT_TYPE3	0x9	/* used for polling		*/
#define FRAME_FORMAT_TYPE4	0xA	/* used for contention		*/
#define MIN_ADLC_FRAME_LEN	11  	/* Supervisory and Unnumbered	*/
#define MIN_INFO_FRAME_LEN	13  

#define SOF_FIELD_LEN		1
#define SOF_FIELD_OFFSET	0

#define FORMAT_FIELD_LEN	2
#define FORMAT_FIELD_OFFSET	1

#define DST_ADDR_FIELD_LEN	2
#define DST_ADDR_FIELD_OFFSET	3

#define SRC_ADDR_FIELD_LEN	2
#define SRC_ADDR_FIELD_OFFSET	5

#define CTRL_FILED_LEN		1
#define CTRL_FIELD_OFFSET	7

#define CRC_FIELD_LEN		2
#define CRC_FIELD_OFFSET	8

#define INFO_FIELD_OFFSET	10

#define EOF_FIELD_LEN		1


#define PF_BIT 0x10		/* Poll / Final Bit		*/
#define INFORMATION_FORMAT 0x00	/* last bit set to zero		*/
#define INFO 0x00		/* Information			*/

#define SUPERVISORY_FORMAT 0x01 /* last bit = 1, 2nd last = 0	*/
#define RR   0x01		/* Receive Ready		*/
#define RNR  0x05		/* Receive Not Ready		*/
#define REJ  0x09		/* Reject			*/

#define UNNUMBERED_FORMAT  0x03	/* last two bits are set	*/ 
#define UI   0x03		/* Unnumbered Information	*/
#define SNRM 0x83		/* Set Normal Response		*/
#define UA   0x63		/* Unnumbered Acknowledgment	*/
#define DISC 0x43	  	/* Disconnect			*/
#define RD   0x43		/* Request Disconnect		*/
#define DM   0x0F		/* Disconnect Mode		*/
#define FRMR 0x87		/* Frame Reject			*/

/************************************************************************/  
/* flag attributes used when using function resetConnection		*/
/************************************************************************/  
#define ADLC_DELETE_CON		1
#define ADLC_RESET_ONLY		2

#if !defined(ADLC_LM)
/************************************************************************/  
/* used for global variable stackTaskId. We need to know in dos	which 	*/
/* stack is linked to with ADLC to do different actions for different 	*/
/* stack 								*/
/* LM has this capability that it can run over both stacks at the same	*/
/* time. and it will know by mapping the local address with task id	*/
/************************************************************************/  
#define RLI_TASK_ID_RSGW	4
#define RLI_TASK_ID_TRIM7	7
#endif /* !ADLC_LM */

/************************************************************************/  
/*			ADLC_CFG_INFO					*/
/*									*/
/************************************************************************/  
typedef struct tagAdlcCfgInfo
  {
  ST_INT modulus;
  ST_INT frameLen;
  ST_INT maxAdlcReqs;
  ST_INT runMode;
  ST_INT communicationMedia;  
  ST_INT retries;
  ST_UCHAR sof;
  ST_UCHAR eof;
  ST_DOUBLE responseTimeout;	/* refers to responseTimeout in port struct */
  ST_DOUBLE frameTimeout;		/* refers to frameTimeout in port struct */
  ST_DOUBLE signalTimeout;		/* refers to signalTimeout in port struct */
  ST_DOUBLE radioDekeyDelay;	/* refers to signalTimeout in port struct */
  ST_DOUBLE radioStartOfTxDelay;	/* refers to signalTimeout in port struct */
  ST_INT preambleLen;
#if defined(ADLC_MASTER)
  ST_DOUBLE maxConnectRetryTimeout;
  ST_DOUBLE deviceTurnaroundTime;	/* refers to turnaround time in dev struct */
  ST_DOUBLE portTurnaroundTime;	/* refers to turnaround time in port struct */
  ST_INT portUsage;
#endif /* ADLC_MASTER */
  } ADLC_CFG_INFO;

/************************************************************************/  
/* 			ADLC_FRAME					*/
/*									*/
/* The following structure is used to read and write from the lower 	*/
/* layer (Serial Manager)						*/
/************************************************************************/  
/* MMSOP_STATUS defines	*/
#define STAT_STANDBY				0
#define STAT_PROGRESS				1
#define STAT_NACKED				2
#define STAT_ACKED				3

typedef struct tagAdlcFrame
  {
  DBL_LNK p;		/* required for link list operations		*/
  struct tagAdlcConInfo *cptr;  /* Connection which it belongs to	*/
  ST_INT status;	/* one of the above				*/
  ST_UCHAR cmd;
  union
    {
    ST_UCHAR *infoBuf;  
    ST_UCHAR *redrBuf;  
    ST_UCHAR suprBuf[MIN_ADLC_FRAME_LEN];
    ST_UCHAR unumBuf[MIN_ADLC_FRAME_LEN];
    }u;
  ST_INT infoLen;  /* For info frame it is used for len of INFO msg	*/
  ST_INT frameLen; /* Total length of the frame including headers and trailers */
#if defined(ADLC_LM)
  DOUBLE addToListTime;
#endif
  } ADLC_FRAME;

/************************************************************************/  
/* 				ADLC_REQ				*/
/* The following structure is to store one complete LSDU INFO		*/
/* frame received from user application (Stack)				*/		
/************************************************************************/  
typedef struct tagAdlcReq
  {
  DBL_LNK p;	 		/* required for link list operations	*/
#if defined(ADLC_LM)
  RLI_IPC_HDR hdr;	 	/* IPC info including semaphore		*/
#endif
  struct tagAdlcConInfo *cptr;  /* Connection which it belongs to	*/
  ADLC_FRAME *txIFramesList; 	/* Head to the segments of LSDU (small frames) */
  ST_ULONG  usr1;		/* user data - from original ADLC_CMD	*/
#if defined(UCA_SMP)
  ADLC_FRAME *txUIFramesList; 	/* Head to the segments of LSDU (small frames) */
  ST_LONG userId;
#endif
  } ADLC_REQ;

/************************************************************************/  
/*			ADLC_CON_INFO					*/
/*									*/
/* ADLC connection information based on (Dn,Nn)	where Dn represents	*/
/* a device and Nn represents an OSI node				*/
/************************************************************************/  
/* State Machine Design	per connection for "state"			*/
#define ADLC_S_DISCONNECT_PHASE 		0	/* setting up data link	*/
#define ADLC_S_LINK_DISCONNECT 			1	/* setting up data link	*/
#define ADLC_S_LINK_SETUP 			2	/* setting up data link	*/ 
#define ADLC_S_NORMAL 				3	/* information transfer	*/
#define ADLC_S_REMOTE_BUSY 			4	/* information transfer	*/
#define ADLC_S_SEND_REJECT 			5	/* information transfer	*/
#define ADLC_S_FRAME_REJECT 			6	/* for slave only	*/
#define ADLC_S_RESET 				7	/* for slave only 	*/

typedef struct tagAdlcConInfo
  {
  DBL_LNK p;				/* required for link list operations */
  ST_UINT16 masterAddr;			/* ISO node address */
  ST_INT txRxMode;			/* TX_MODE or RX_MODE */
  struct tagAdlcDevInfo *dptr;		/* ADLC device structure pointer */
  struct tagAdlcPortInfo *pptr;		/* port which it belongs to */
  ST_INT state;				/* Connection state defined above */
  ST_UCHAR vR;				/* Receive State Variable */
  ST_UCHAR vS;				/* Send State Variable */
  ST_UCHAR sendCount;			/* How many frames we have sent */
  ST_INT infoBufLen;			/* Length of info message for rxIFramesList */
  ST_INT txAdlcReqCount;		/* How many write request ?? */
  ST_INT retries;			/* How many retries are allowed */
  ADLC_FRAME *rxIFramesList;		/* Info frames which are not complete LSDU */
#if defined(ADLC_MASTER)
  ST_DOUBLE connectRetryTimeout;		/* To store the time when we will try to reconnect */
#endif
#if defined(UCA_SMP)
  ST_INT txExtAdlcReqCount;		/* How many write request ?? */
  ADLC_FRAME *rxUIFramesList;		/* Info frames which are not complete LSDU */
  ST_INT extInfoBufLen;			/* Length of ui message for rxUIFramesList */
#endif
  } ADLC_CON_INFO;
/************************************************************************/  
/*			ADLC_DEV_INFO					*/
/*									*/
/* Tracking structure for device. This is the main entry point to 	*/
/* reference to almost any device related info. This will be allocated 	*/
/* at the startup when reading from configuration file			*/
/************************************************************************/  
#define MAX_DEV_NAME_SIZE		80

/* Mode could be one of the following					*/
/* ADLC slave will only have two modes TX_MODE and RX_MODE		*/
/* Slave will initially be at RX_MODE. It will go to TX_MODE when he 	*/
/* sees the P/F bit and after transmission it will go back to RX_MODE	*/
/* In case of timeout it will remain in RX_MODE				*/
/* ADLC master will initially be in TX_MODE. It will go in RX_MODE only	*/
/* when it expects the response back from the slave. In case of timeout	*/
/* it will go back to TX_MODE. 						*/
#define TX_MODE				0
#define RX_MODE				1

typedef struct tagAdlcDevInfo
  {              
  DBL_LNK p;				/* required for link list operations */
  ST_CHAR name[MAX_DEV_NAME_SIZE+1];  	/* storage for device name */
  ST_UINT16 address;			/* Device's 16-bit address */
  ST_INT txRxMode;			/* TX_MODE or RX_MODE */
  struct tagAdlcPortInfo *pptr;		/* port which it belongs to */
  ADLC_CON_INFO *conList;
#if defined(ADLC_MASTER)
  ST_INT pollGroup;			/* 1 to MAX_POLL_GROUPS */
  ST_BOOLEAN connectAll;			/* flag only used in adlcConnectAll() */
  ST_DOUBLE turnaroundTime;	
#if defined(ADLC_LM)
  ST_INT txRxModeCounter; 		/* for proper behaviour of turnaround timer */
#endif /* ADLC_LM */
#endif /* ADLC_MASTER */
#if defined(ADLC_LM)
  ADLC_DEV_STATS devStats;
#endif /* ADLC_LM */
  } ADLC_DEV_INFO;

/************************************************************************/  
/*			ADLC_PORT_INFO					*/
/*									*/
/* Tracking structure for network 					*/
/* will be allocated at the startup when reading from configuration file*/
/*									*/
/* txRxMode : is either TX_MODE or RX_MODE. 				*/
/* For master It is used in code in the following three ways		*/
/* pptr->txRxMode = RX_MODE when p/f bit given to a slave		*/
/* pptr->txRxMode = TX_MODE when port turnaround time or timeout has 	*/
/* elapsed								*/
/* For slave It is used in code in the following three ways		*/
/* pptr->txRxMode = TX_MODE when p/f is received from master		*/
/* pptr->txRxMode = RX_MODE when p/f bit given from slave		*/
/* if pptr->txRxMode == TX_MODE to see if we got p/f from master	*/
/* runMode : is POLLED_MODE, ENHANCED_POLLED_MODE or CONTENTION_MODE	*/
/* The idea is when runMode is configured in CONTENTION_MODE, then 	*/
/* pptr->runMode can change dynamically else pptr->runMode will remain	*/
/* same as configured runMode (i.e. ADLC_RUN_MODE(pptr))		*/
/* For slave it is used in the following ways				*/
/* pptr->runMode = POLLED_MODE when p/f is received from master		*/
/* pptr->runMode = CONTENTION_MODE when p/f bit given from slave	*/
/* if pptr->runMode == CONTENTION_MODE to encode the frame format	*/
/* For Master it is used in the following ways				*/
/* pptr->runMode = ENHANCED_POLLED_MODE when dcd is high most of times	*/
/* pptr->runMode = CONTENTION_MODE when RR frames are more than INFO 	*/
/* if pptr->runMode == CONTENTION txStateService is called else 	*/
/* txService is called							*/
/************************************************************************/  
/* States for getting the frame from the port used in "rxState"		*/
#define		STATE_WAIT_SOF		0
#define		STATE_WAIT_LEN		1
#define		STATE_WAIT_CRC		2
#define		STATE_WAIT_DATA		3
#define		STATE_WAIT_EOF		4

/* States for slave tranmit algorithm used in "signalState"		*/
#define 	SL_TX_STATE_IDLE				0
#define 	SL_TX_STATE_PF_RCVD				1
#define 	SL_TX_STATE_WAIT_FOR_CTS_HIGH			2
#define		SL_TX_STATE_START_OF_TX_DELAY			3
#define 	SL_TX_STATE_OK_TO_TX				4
#define 	SL_TX_STATE_WAIT_FOR_TX_BUF_EMPTY		5
#define		SL_TX_STATE_DEKEY_DELAY				6
#define 	SL_TX_STATE_WAIT_FOR_CTS_LOW			7
#define		SL_TX_STATE_UNSOL_TX 				8
#define    	SL_TX_STATE_WAIT_FOR_LINK 			9
#define		SL_TX_STATE_START_OF_UNSOL_TX_DELAY	        10
#define 	SL_TX_STATE_OK_TO_UNSOL_TX			11

/*States for master tranmit in contention used in "signalState"	*/
#define		MA_TX_STATE_IDLE 				0
#define		MA_TX_STATE_WAIT_FOR_CTS_HIGH 			1
#define		MA_TX_STATE_WAIT_FOR_DCD_LOW 			2
#define		MA_TX_STATE_OK_TO_TX 				3
#define		MA_TX_STATE_WAIT_FOR_TX_BUF_EMPTY 		4
#define		MA_TX_STATE_DEKEY_DELAY 				5
#define		MA_TX_STATE_WAIT_FOR_CTS_LOW 			6
#define		MA_TX_STATE_WAIT_FOR_CTS_HIGH_DCD_LOW		7
typedef struct tagAdlcPortInfo
  {
  DBL_LNK p;				/* required for link list operations */
  ST_INT port;                		/* digiboard port ID     	 */
  ST_INT rxState;			/* receive state */
  ST_UCHAR *rxBuf;
  ST_UINT rxLen;
  ADLC_CON_INFO *outstandingCptr;  /* At one time only one con. have P/F set */
  ADLC_DEV_INFO *devList;  /* linked list of devices associated with this port */
  ADLC_REQ *txAdlcReqList;   /* linked list of write packet list from stack */
  ADLC_FRAME *txUFramesList; /* linked list of unnumbered frames */
  ADLC_FRAME *txSFramesList; /* linked list of supervisory frames */
  ADLC_FRAME *txMFramesList; /* linked list of multicast frames	to tx */
  ADLC_FRAME *rxMFramesList; /* linked list of multicast frames	to rx not complete LSDU */
  ST_UCHAR vR;		     /* receive state variable for multicast frames */
  ST_INT infoBufLen;	     /* Length of info message for rxMFramesList  */
  ST_DOUBLE responseTimeout;    /* The time request with P/F set was sent */
  ST_INT signalState;	     /* used for transmit algorithm	 */
  ST_DOUBLE signalTimeout;	     /* used for transmit algorithm	 */
  ST_INT txRxMode;	     /* TX_MODE or RX_MODE	 */
  ST_INT runMode;	     /* POLLED_MODE, ENHANCED_POLLED_MODE or CONTENTION_MODE */
  ADLC_CFG_INFO adlcCfg;
#if defined(ADLC_MASTER)
  ADLC_FRAME *txRFramesList; /* linked list of redirect frames */
  ST_DOUBLE turnaroundTime;	
  ST_ULONG rxInfo;		/* Received INFO frames used for switching modes*/
  ST_ULONG rxRr;		/* Received RR frames used for switching modes*/
  ST_ULONG txInfo;      /* Transmitted INFO frames used for transmit algorithm */
  ST_ULONG txRr;        /* Transmitted RR frames used for transmit algorithm */
  ST_BOOLEAN bTxRrFlag;     /* should RR have higher priority than INFO ?   */
  ST_DOUBLE lastSnrmTime;  /* the recorded time when last snrm was sent	   */
  ST_INT  prQueArray[12];  /* array of int which will store priority order  */ 
#endif
#if defined(ADLC_LM)
  HEV  txDoneSem;		/* sem to wait on the sm to be waited */
  HEV  frameArrivalSem;		/* sem to wait for frame to rx */
  HEV  frameInListSem;		/* sem to wait for frame to tx */
  HEV  rdThreadErrSem;		/* sem to clean up from read thread error */
  ST_BOOLEAN  bPortBusy;	/* if true - tx is pending */
#if defined(ADLC_MASTER)
  HTIMER   hTxStateTimer;  	/* used in txStateService to drive state mach */
  ST_INT txRxModeCounter; 	/* for proper behaviour of turnaround timer */
#endif	/* ADLC_MASTER	*/
#endif  /* ADLC_LM */
#if !defined(ADLC_LM)
  ST_DOUBLE frameTimeout;
#endif
#if defined(UCA_SMP)
  ADLC_REQ *txExtAdlcReqList;   /* linked list of ext write packet list from stack */
  ST_CHAR recTime[SMP_SIZE_OF_REC_TIME]; /* buffer to hold time stamp */
#endif
  } ADLC_PORT_INFO;

#if defined(ADLC_MASTER)
/************************************************************************/  
/*			POLLED_DEV					*/
/*									*/
/*			Link Manager Polling Structure			*/
/************************************************************************/  

typedef struct tagPolledDevices
  {
  DBL_LNK	l;		/* gen list link			*/
  ADLC_DEV_INFO	*dptr;	/* pointer to device structure		*/
  ST_BOOLEAN	bSuspendPolling; /* don't poll this one for a while	*/
  } POLLED_DEV;

/************************************************************************/  
/*			POLL_GROUP					*/
/*									*/
/************************************************************************/  
#define FIRST_POLL_GROUP	1
#define MAX_POLL_GROUPS		3

/* assume that pollGrpID's start at 1 and go up to MAX_POLL_GROUPS	*/
/* assume that poll group 1 is polled most frequently and poll group  	*/
/* MAX_POLL_GROUPS is polled least frequently.				*/
/* when requested to change the poll rate for a device, change the poll	*/
/* group for the device - increment poll group to decrease polling or	*/
/* decrement poll group to increase poll frequency			*/

typedef struct tagPollGroup
  {
  DBL_LNK 	l; 		/* gen list link			*/
  ST_INT	pollGrpID;	/* a unique int 1 to MAX_POLL_GROUPS	*/
  ST_DOUBLE	pollRate;	/* in m seconds from config file	*/
  POLLED_DEV	*pollDevList;	/* a linked list of devices to poll	*/
#if !defined(ADLC_LM)
  ST_DOUBLE pollTime;		/* To store the time of last poll	*/
#endif
#if defined(ADLC_LM)
  HTIMER	hTimer;		/* used in MuxWaitSem to trigger poll	*/
#endif
  } POLL_GROUP;

#endif /* ADLC_MASTER */

#if defined(ADLC_LM)
/************************************************************************/  
/*			ADLC_TASK_INFO 					*/
/*									*/
/************************************************************************/
typedef struct tagAdlcTaskInfo
  {
  DBL_LNK l; 		/* gen list link			*/
  ST_UINT16 localAddr;
  HQUEUE qHandle;
  ST_INT taskId;
/*  PID pid; */
  } ADLC_TASK_INFO;

/************************************************************************/  
/*			ADLC_FILTER_DEV_LOG_INFO			*/
/*									*/
/************************************************************************/
typedef struct tagAdlcFilterDevLogInfo
  {
  ST_UINT16 address;
  ST_BOOLEAN bFilter;
  ST_ULONG saveIoLogMask;
  } ADLC_FILTER_DEV_LOG_INFO;

#endif  /* ADLC_LM */

#if !defined(ADLC_LM)
/************************************************************************/  
/*			ADLC_EVENT_INFO 				*/
/*									*/
/************************************************************************/
typedef struct tagAdlcEventInfo
  {
  DBL_LNK l; 		/* gen list link			*/
  ST_INT adlcEventOp;
  ST_INT adlcRetCode;
  ST_VOID *adlcEventCb;
  } ADLC_EVENT_INFO;
#endif
/************************************************************************/  
/* Function like macros to get and set various fields 			*/
/************************************************************************/  
/* This macro gets the frame format from the frame			*/
#define FRAME_FORMAT_GET(frame) ((frame[1] << 8) | (frame[2]))

/* This macro gets the length of the frame from the frame 		*/
#define FRAME_LEN_GET(frame) (((frame[1] << 8) | (frame[2])) & 0x07FF)

/* This macro sets the length of the frame in the frame format field	*/
#define FRAME_LEN_SET(x,r) (x = ((x) & 0xF800) | ((r) & 0x07FF))

/* This macro gets the type of the frame from the frame 		*/
#define FRAME_TYPE_GET(frame) (ST_UCHAR) (frame[1] >> 4)

/* This macro sets the type of the frame in the frame format field	*/
#define FRAME_TYPE_SET(x,y) (x = ((x) & 0x0FFF) | ((y) << 12))

/* This macro gets the eos bit from the frame				*/
#define EOS_GET(frame) (frame[1] & 0x08)

/* This macro sets the eos in the frame format field			*/
#define EOS_SET(x) (x = ((x) & 0xF7FF))

/*  This macro gets the destination address from the frame		*/
#define DST_ADDR_GET(frame) (ST_UINT16) (frame[3] << 8 | frame[4])

/*  This macro gets the source address from the frame			*/
#define SRC_ADDR_GET(frame) (ST_UINT16) (frame[5] << 8 | frame[6])

/*  This macro gets the device address from the receing frame 		*/
#if defined(ADLC_MASTER)
#define DEV_ADDR_GET(frame) SRC_ADDR_GET(frame)
#endif
#if defined(ADLC_SLAVE)
#define DEV_ADDR_GET(frame) DST_ADDR_GET(frame)
#endif

/* This macro gets the control byte from the frame			*/
#define CTRL_BYTE_GET(frame) (frame[7])

/* This macro gets the Poll Final Bit from the control byte	*/
#define PF_GET(x) ((x) & 0x10)	/* Poll Final Bit get		*/

/* This macros tells to have a poll/final bit depending on frame type */
#define PF_DECIDE(frameType) (frameType == FRAME_FORMAT_TYPE3 ? PF_BIT : 0x0)

/* This macro sets the Poll Final Bit in the control byte		*/
#define PF_SET(x) (x = (x) | (ST_UCHAR) 0x10)	/* Poll Final Bit set		*/

/* These get the code from the control byte including the format type	*/
#define UNUM_CODE_GET(x) (ST_UCHAR) ((x) & 0xEF)
#define SUPR_CODE_GET(x) (ST_UCHAR) ((x) & 0x0F)
#define INFO_CODE_GET(x) (ST_UCHAR) ((x) & 0x01)


/* These macros get the N(R) and N(S) in an info command. */
#define INFO_NR_GET(x) (ST_UCHAR) (((x) & 0xE0) >> 5)
#define INFO_NS_GET(x) (ST_UCHAR) (((x) & 0x0E) >> 1)

/* These macros alter the N(R) and N(S) in an info command. */
#define INFO_NS_SET(x,r) (x = (x) | (ST_UCHAR) ((r) << 1))
#define INFO_NR_SET(x,r) (x = (x) | (ST_UCHAR) ((r) << 5))

/* This macro gets the N(R) in a supervisory command. */
#define SUPR_NR_GET(x) (ST_UCHAR) (((x) & 0xE0) >> 5)

/* This macro alters the N(R) in a supervisory command. */
#define SUPR_NR_SET(x,r)  (x = (x) | (ST_UCHAR) ((r) << 5))

/* This macro gets the crc from the frame				*/
#define FRAME_CRC_GET(frame,offset) ((frame[offset] << 8) | (frame[offset + 1]))


/* This macro rotates the bytes in unsigned int				*/
#define ROTATE(x) { ST_UINT16 tmp;\
		    tmp = (x) & 0xFF;\
		    x = (x) >> 8;\
		    x |= tmp << 8;\
		  }

/* Modulo-N increment and decrement macros */
#define DEC_MODULO_N(x,y,z)  (ST_UCHAR) (((x) >= (y)) ? ((x) - (y)) : (((x) + (z)) - (y)))
#define INC_MODULO_N(x,y,z)  (ST_UCHAR) (((x) + (y)) % (z))
#define DIFF_MODULO_N(x,y,z) (ST_UCHAR) (((x) >= (y)) ? (x) - (y) : ((x) + (z)) - (y))

/* if the most significant bit is set it is either broadcast or multicast	*/
#define IS_MULTICAST_ADDR(x)	((x) & 0x8000)

/* if the bit 13 is not set it is a remote address			*/
#define IS_REMOTE_ADDR(x)	!((x) & 0x2000)

/* Used to change port=0 into COM1, port=1 into COM2 etc.		*/
#define GET_COM_NO(x)		((x) + 1)

/* Time Management Macros						*/
#define START_TIMEOUT(x,y)	(x = sGetMsTime() + (y))
#define UPDATE_TIMEOUT(x,y)	if (x) x = sGetMsTime() + (y)
#define STOP_TIMEOUT(x)		(x = 0.0)
#define CHECK_TIMEOUT(x)	((x) && (sGetMsTime() >= (x)))
#define IS_TIMEOUT_ENABLED(x)	(x)
#define IS_TIMEOUT_DISABLED(x)  !(x)

/* Macros to work on ADLC_CFG_INFO structure				*/
#define ADLC_MODULUS(x)			x->adlcCfg.modulus
#define ADLC_FRAME_LEN(x)		x->adlcCfg.frameLen
#define ADLC_INFO_LEN(x)		(x->adlcCfg.frameLen - MIN_INFO_FRAME_LEN)
#define ADLC_MAX_ADLC_REQS(x)   	x->adlcCfg.maxAdlcReqs
#define ADLC_RUN_MODE(x)		x->adlcCfg.runMode
#define ADLC_COMMUNICATION_MEDIA(x)	x->adlcCfg.communicationMedia
#define ADLC_RETRIES(x)			x->adlcCfg.retries
#define ADLC_SOF(x)			x->adlcCfg.sof
#define ADLC_EOF(x)			x->adlcCfg.eof
#define ADLC_RESPONSE_TIMEOUT(x)	x->adlcCfg.responseTimeout
#define ADLC_FRAME_TIMEOUT(x)		x->adlcCfg.frameTimeout
#define ADLC_SIGNAL_TIMEOUT(x)		x->adlcCfg.signalTimeout
#define ADLC_RADIO_DEKEY_DELAY(x)	x->adlcCfg.radioDekeyDelay
#define ADLC_RADIO_START_OF_TX_DELAY(x)	x->adlcCfg.radioStartOfTxDelay
#define ADLC_PREAMBLE_LEN(x)		x->adlcCfg.preambleLen

#if defined(ADLC_MASTER)
#define ADLC_MAX_CONNECT_RETRY_TIMEOUT(x)  x->adlcCfg.maxConnectRetryTimeout
#define ADLC_DEVICE_TURNAROUND_TIME(x)	   x->adlcCfg.deviceTurnaroundTime
#define ADLC_PORT_TURNAROUND_TIME(x)	   x->adlcCfg.portTurnaroundTime
#define ADLC_PORT_USAGE(x)		   x->adlcCfg.portUsage
#endif /* ADLC_MASTER */

#if defined(CNP)
/* This macro checks if there is data on port and it gives the char else -1 */
#define RX_BYTE(port) ((cnp_cnt (port)) ? cnp_getc (port) : -1 )

/* This macro puts the data on the port					    */
#define SM_PUT(a,b,c) cnp_put(a,b,c)
#else 

#define RX_BYTE(port) ((sm_rx_cnt (port)) ? sm_getc (port) : -1 )
#define SM_PUT(a,b,c) sm_put(a,b,c)

#endif /* CNP */

#ifdef ADLC_TESTING

/* Macro to get a random integer within a specified range */
#define GET_RANDOM_BETWEEN( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))

/* Macro to get one random value out of two		 */
#define GET_RANDOM_OF(val1,val2) (rand() < RAND_MAX / 2 ? val1 : val2)

#endif
/************************************************************************/  
/* Global Variables Declarations					*/
/************************************************************************/  
extern  ADLC_CFG_INFO   adlcCfg;
extern ADLC_PORT_INFO *portList;
extern CFG_PORT *portConfigList;
extern const ST_UINT16 adlc_crc_table[256];
extern ADLC_STATS adlcStats;
extern ST_BOOLEAN terminateFlag;

#if defined(ADLC_MASTER)
extern POLL_GROUP *pollGroupList;
extern ST_ULONG gRrFrames;
extern ST_ULONG gInfoFrames;
#endif

#if defined(ADLC_ERROR)
extern ADLC_ERROR_INFO adlcError;
#endif

#if !defined(ADLC_LM)
extern ADLC_EVENT_INFO *eventList;
extern ST_INT stackTaskId;
#endif

#if defined(ADLC_LM)
extern ADLC_TASK_INFO *taskList;
extern ST_INT connectedTask[RLI_TASK_ID_LAST + 1];
extern ST_INT serMgrSleep;
extern PID	myPID;
extern ADLC_FILTER_DEV_LOG_INFO stFiltDevLog;
extern CRITICAL_SECTION protectAdlcData; /* to guard adlc data between threads */
#endif
/************************************************************************/  
/* Global Functions Declarations					*/
/* make the following functions static if you can			*/
/************************************************************************/  
/* adlc_utl.c							*/
ST_UCHAR eventTypeOf(ST_UCHAR ctrlByte);
ST_VOID timeout_error (ADLC_CON_INFO *cptr);
ST_VOID resetConnection (ADLC_CON_INFO *cptr,ST_INT attrib);
ST_VOID resetStateVarsForConnection(ADLC_CON_INFO *cptr);
ST_RET verifyCrc(ST_UCHAR *frame, ST_UINT frameLen);
ST_UINT16 calcCrc (ST_UCHAR *data, ST_UINT length);

ADLC_PORT_INFO *getPortPtrFromPort(ST_INT port);
ADLC_DEV_INFO *getDevPtrFromAddress(ST_UINT16 addr);
ADLC_DEV_INFO *getDevPtrFromName(ST_CHAR *name);
ADLC_CON_INFO *getConPtrFromAddrPair(ST_UINT16 slaveAddr,ST_UINT16 masterAddr);
ADLC_CON_INFO *getConPtrFromDptr(ADLC_DEV_INFO *dptr,ST_UINT16 masterAddr);

ADLC_PORT_INFO *addToPortList(ST_INT port, const ADLC_CFG_INFO *pstAdlcCfg);
CFG_PORT *addToCfgPortList(ST_INT port);
ADLC_DEV_INFO *addToDevList(ADLC_PORT_INFO *pptr, ST_CHAR *name,
			    ST_UINT16 address,ST_INT pollGroup,ST_BOOLEAN connectAll);
ADLC_CON_INFO *addToConList(ADLC_DEV_INFO *dptr,ST_UINT16 masterAddr);
ST_VOID addToSFramesList(ADLC_CON_INFO *cptr, ST_UCHAR cmd);
ST_VOID addToUFramesList(ADLC_CON_INFO *cptr, ST_UCHAR cmd);
ST_VOID addToRFramesList(ADLC_PORT_INFO *pptr);
ST_VOID addToMFramesInTxList(ST_UINT16 srcAddr,ST_UINT16 dstAddr, ST_VOID *data, ST_INT dataLen);
ST_VOID addToMFramesInRxList(ADLC_PORT_INFO *pptr,ST_UCHAR *data,ST_INT dataLen);
ADLC_REQ *addToReqList(ADLC_CON_INFO *cptr);
ST_VOID addToIFramesInTxList(ADLC_REQ *rptr,ST_UCHAR *data,ST_INT len);
ST_VOID addToIFramesInRxList(ADLC_CON_INFO *cptr,ST_UCHAR *data,ST_INT len);

ST_VOID deleteAllConnections(ADLC_DEV_INFO *dptr);
ST_VOID deleteConnection(ADLC_CON_INFO *cptr);
ST_VOID deleteDevice(ADLC_DEV_INFO *dptr);
ST_VOID deletePort(ADLC_PORT_INFO *pptr);
ST_VOID deleteCfgPort(CFG_PORT *pstCfgPort);
ST_VOID deleteAllFrames(ADLC_CON_INFO *cptr);
ST_VOID deleteSFramesList(ADLC_CON_INFO *cptr);
ST_VOID deleteSFramesListWithoutPf(ADLC_CON_INFO *cptr);
ST_VOID deleteUFramesList(ADLC_CON_INFO *cptr);
ST_VOID deleteIFramesFromRxList(ADLC_CON_INFO *cptr);
ST_VOID deleteIFramesFromTxList(ADLC_CON_INFO *cptr);
ST_VOID deleteMFramesFromTxList(ADLC_PORT_INFO *pptr);
ST_VOID deleteMFramesFromRxList(ADLC_PORT_INFO *pptr);
ST_VOID deleteAckedIFramesFromTxList(ADLC_CON_INFO *cptr,ST_UCHAR nR);
ST_VOID deleteFrame(ADLC_FRAME *fptr,ADLC_FRAME **framesList);
ST_VOID deleteReqFrame (ADLC_REQ *rptr);

ST_BOOLEAN isDevPtrValid(ADLC_DEV_INFO *rDptr);
ST_VOID nackAllIFramesFromTxList(ADLC_CON_INFO *cptr);
ST_BOOLEAN isInfoQueuedUp(ADLC_CON_INFO *cptr);
ST_BOOLEAN isPollQueuedUp(ADLC_CON_INFO *cptr);
ST_BOOLEAN isUFrameQueuedUp(ADLC_CON_INFO *cptr);
ST_BOOLEAN isOkToPoll(ADLC_CON_INFO *cptr);
ST_BOOLEAN isOkToSendUnum(ADLC_CON_INFO *cptr,ST_BOOLEAN justCheck);
ST_BOOLEAN isOkToSendSupr(ADLC_CON_INFO *cptr,ST_BOOLEAN justCheck);
ST_BOOLEAN isOkToSendInfo(ADLC_CON_INFO *cptr,ST_BOOLEAN justCheck);
ST_BOOLEAN isOkToSendMult(ADLC_PORT_INFO *pptr,ST_BOOLEAN justCheck);
ST_BOOLEAN isConStateInInfoTransfer(ADLC_CON_INFO *cptr);
ST_VOID getPfAndEosBit(ADLC_FRAME *nextFptr,ST_UCHAR sendCount,
		    ST_BOOLEAN *pfBit,ST_BOOLEAN *eosBit, ADLC_PORT_INFO *pptr);
ST_VOID adlc_except (SD_CONST ST_CHAR *file, ST_INT line);
ST_UINT16 getLocalAddr(ADLC_CON_INFO *cptr);
ST_UINT16 getRemoteAddr(ADLC_CON_INFO *cptr);
CFG_PORT *findCfgPort(ST_INT port);
ST_VOID adlcEndLogging (ST_VOID);

#if defined(ADLC_MASTER)
POLL_GROUP *findPollGroup(ST_INT pollGrpID);
POLL_GROUP *addToPollGroupList(ST_INT pollGrpID,ST_DOUBLE pollRate);
ST_RET addDevToPollGrp(ADLC_DEV_INFO *dptr,ST_INT pollGroup);
POLLED_DEV *findPolledDevByName(char *devName);
POLLED_DEV *findPolledDevByAddress(ST_UINT16 addr);
#endif

/* adlc_dec.c								*/
ST_RET getFrame(ADLC_PORT_INFO *pptr);
ST_RET processAdlcFrame(ADLC_PORT_INFO *pptr);
ST_VOID giveIndicationToStack (ADLC_CON_INFO *cptr,ST_INT opcode,ST_RET retCode, ST_LONG usr1);

/* adlc_enc.c								*/
ST_RET send_info_frame(ADLC_FRAME *fptr,ST_BOOLEAN pfBit,ST_BOOLEAN eosBit);
ST_RET send_supr_frame(ADLC_FRAME *fptr);
ST_RET send_unum_frame(ADLC_FRAME *fptr);
ST_RET send_ui_frame(ADLC_FRAME *fptr);
ST_RET send_redr_frame(ADLC_FRAME *fptr,ADLC_PORT_INFO *pptr);
ST_RET send_mult_frame(ADLC_FRAME *fptr,ADLC_PORT_INFO *pptr);
ST_VOID enc_adlc_frame(ST_UCHAR *frame,ST_UINT16 srcAddr,ST_UINT16 dstAddr,ST_UCHAR ctrlByte,
		    ST_UINT16 frameFormat,ST_INT dataLen, ADLC_PORT_INFO *pptr);
ST_UINT16 getFrameFormat(ST_INT frameLen,ST_BOOLEAN eosBit,ST_UCHAR frameType);

/* adlc_ssv.c adlc_msv.c */
ST_VOID adlcService(ST_VOID);
ST_BOOLEAN checkTxService(ADLC_PORT_INFO *pptr);
ST_VOID txService(ADLC_PORT_INFO *pptr);
ST_VOID rxService(ADLC_PORT_INFO *pptr);
ST_VOID txStateService(ADLC_PORT_INFO *pptr);
ST_VOID timeoutService(ADLC_PORT_INFO *pptr);
ST_VOID stopWaitingForResp(ADLC_CON_INFO *cptr, ST_BOOLEAN bResetRetries);

#if defined(ADLC_SLAVE)
ST_VOID startSignalTimeout(ADLC_PORT_INFO *pptr);
ST_VOID stopSignalTimeout(ADLC_PORT_INFO *pptr);
ST_BOOLEAN checkSignalTimeout(ADLC_PORT_INFO *pptr);
ST_VOID goInTxMode(ADLC_CON_INFO *cptr);
ST_VOID goInRxMode(ADLC_CON_INFO *cptr);
ST_VOID waitForResponse(ADLC_CON_INFO *cptr);
#endif /* ADLC_SLAVE */

#if defined(ADLC_MASTER)
ST_VOID pollDevice(ADLC_DEV_INFO *dptr);
ST_VOID pollConnection(ADLC_CON_INFO *cptr);
ST_VOID waitForResponse(ADLC_CON_INFO *cptr);
ST_VOID startConRetryTimeout(ADLC_CON_INFO *cptr);
ST_BOOLEAN checkConRetryTimeout(ADLC_CON_INFO *cptr);
ST_VOID stopConRetryTimeout(ADLC_CON_INFO *cptr);
ST_VOID startDeviceTurnaroundTimeout(ADLC_DEV_INFO *dptr);
ST_BOOLEAN checkDeviceTurnaroundTimeout(ADLC_DEV_INFO *dptr);
ST_VOID stopDeviceTurnaroundTimeout(ADLC_DEV_INFO *dptr);
ST_VOID startPortTurnaroundTimeout(ADLC_PORT_INFO *pptr);
ST_BOOLEAN checkPortTurnaroundTimeout(ADLC_PORT_INFO *pptr);
ST_VOID stopPortTurnaroundTimeout(ADLC_PORT_INFO *pptr);
ST_RET startPollTimer(POLL_GROUP *pstPollGrp);
ST_RET updatePollTimer(POLL_GROUP *pstPollGrp);
ST_RET stopPollTimer(POLL_GROUP *pstPollGrp);
ST_BOOLEAN checkPollTimer(POLL_GROUP *pstPollGrp);
ST_RET pollInit(ST_VOID);

ST_VOID triggerLogicAnalyzer(ST_INT port);
#endif /* ADLC_MASTER */

/* adlc_tm.c		*/
ST_VOID startRespTimeout(ADLC_PORT_INFO *pptr);
ST_VOID updateRespTimeout(ADLC_PORT_INFO *pptr);
ST_VOID stopRespTimeout(ADLC_PORT_INFO *pptr);
ST_BOOLEAN checkRespTimeout(ADLC_PORT_INFO *pptr);
ST_BOOLEAN isRespTimeoutEnabled(ADLC_PORT_INFO *pptr);
ST_BOOLEAN isRespTimeoutDisabled(ADLC_PORT_INFO *pptr); 
ST_VOID startSignalTimeout(ADLC_PORT_INFO *pptr); 
ST_VOID updateSignalTimeout(ADLC_PORT_INFO *pptr); 
ST_VOID stopSignalTimeout(ADLC_PORT_INFO *pptr); 
ST_BOOLEAN checkSignalTimeout(ADLC_PORT_INFO *pptr);
ST_VOID startRadioDelayTimeout(ADLC_PORT_INFO *pptr); 
ST_VOID startFrameTimeout(ADLC_PORT_INFO *pptr); 
ST_BOOLEAN checkFrameTimeout(ADLC_PORT_INFO *pptr); 
ST_VOID stopFrameTimeout(ADLC_PORT_INFO *pptr); 
ST_VOID time_delay (ST_DOUBLE tme);

/* adlc_log.c */
ST_VOID logAdlcStats(ST_VOID);

/* adlc_get.c */
ST_CHAR *getCommandName(ST_UCHAR ctrlByte);
ST_CHAR *getConState(ST_INT state);
ST_CHAR *getTxRxMode(ST_INT mode);
ST_CHAR *getRunMode(ST_INT mode);
ST_CHAR *getCommunicationMedia(ST_INT media);
ST_CHAR *getOpCode(ST_INT opcode);
ST_CHAR *getRetCode(ST_RET retCode);
ST_CHAR *getSignalState(ST_INT signalState);

/* adlc_var.c			*/
ST_VOID adlcInitGlbVars(ST_VOID);
ST_VOID initSiscoUtilGlbVarsForAdlc(ST_VOID);

/* adlc_lsm.c */
ST_VOID rdThreadErrorHandling(ADLC_PORT_INFO *pptr);

/* adlc_cfg.c adlc_odb.c adlc_hc.c	*/
ST_RET adlcConfigure (ST_VOID);
#if defined(ADLC_LM)
ST_RET adlcReConfigure (ST_VOID);
ST_RET updateAdlcLoggingParameters(ST_VOID);
ST_RET updateAdlcPollParameters(ST_UINT hi,ST_UINT med, ST_UINT low);
ST_RET adlcCloseDatabase (ST_VOID);
#endif

/* adlc_tst.c */
#ifdef ADLC_TESTING
ST_VOID testSetMain (ST_VOID);
ST_VOID switchMenu(ST_VOID);
ST_VOID memCheckError (ST_VOID);
ST_VOID screenLogFun (LOG_CTRL *lc, 
		      SD_CONST ST_INT logType, 
		      SD_CONST ST_CHAR *SD_CONST sourceFile, 
		      SD_CONST ST_INT lineNum,
		      SD_CONST ST_INT bufLen, 
		      SD_CONST ST_CHAR *buf);
ST_VOID doEnableScreenLog (ST_VOID);
ST_VOID doDisableScreenLog (ST_VOID);
#if defined(__OS2__)
extern HEV o_kbd_event_sem;
#endif
#if defined(_WIN32)
extern HEV kbdEvent;
#endif
#if defined(ADLC_LM)
ST_RET handleKeyboardEvent(ST_ULONG userId,  ST_ULONG postCount);
ST_RET testStartKeyboard (ST_VOID);
ST_RET testEndKeyboard (ST_VOID);
#endif  /* ADLC_LM */
#endif

#if !defined(ADLC_LM)
ADLC_EVENT_INFO *addToEventList(ST_INT adlcEventOp,ST_VOID *adlcEventCb,ST_INT adlcRetCode);
#endif

#if defined(ADLC_LM)
ST_RET exitCallBack (ST_ULONG userId, ST_ULONG postCount);

ST_RET term_init (ST_VOID);
ST_RET term_rest (ST_VOID);
ST_RET adlcQueInit (ST_VOID);
ADLC_TASK_INFO *addToTaskList(ST_UINT16 localAddr,ST_INT taskId,HQUEUE qHandle);
ADLC_TASK_INFO *getTaskPtrFromLocalAddress(ST_UINT16 addr);
ADLC_TASK_INFO *getTaskPtrFromTaskId(ST_INT taskId);
ST_VOID deleteTask(ADLC_TASK_INFO *tptr);
ST_VOID deleteTaskFromLocalAddress(ST_UINT16 localAddr);
ST_RET addTaskForLocalAddress(ST_UINT16 addr,ST_INT taskId);
ST_VOID deleteTaskFromTaskId(ST_INT taskId);
ST_INT getTaskId(ADLC_CON_INFO *cptr,ST_INT *taskId);
ST_VOID logIpcError(ST_INT retCode, ST_CHAR *str, ST_CHAR *file, ST_INT line);
ST_VOID updateStatusBar(ST_CHAR *szBuff);
ST_VOID screenLogFun (LOG_CTRL *lc, 
		      SD_CONST ST_INT logType, 
		      SD_CONST ST_CHAR *SD_CONST sourceFile, 
		      SD_CONST ST_INT lineNum,
		      SD_CONST ST_INT bufLen, 
		      SD_CONST ST_CHAR *buf);

/* adlc_lm.c */
ST_RET handleCmdQueEvent (ST_ULONG userId, ST_ULONG qMsgLen, ST_VOID *qMsg);
ST_RET handleDataQueEvent (ST_ULONG userId, ST_ULONG qMsgLen, ST_VOID *qMsg);
ST_RET sendDataMsgToStack(ADLC_CMD *srcPstCmd,ST_INT stackTaskId);
ST_RET sendDataMsgToStackNoAlloc(ADLC_CMD *dstPstCmd,ST_INT stackTaskId);
#endif  /* ADLC_LM */

#if defined(__OS2__)
ST_VOID exitLogging (ST_ULONG ulTermCode);
ST_VOID exitCleanFun (ST_ULONG ulTermCode);
#endif 

#if defined(_WIN32)
ST_VOID exitLogging (ST_VOID);
ST_VOID exitCleanFun (ST_VOID);
#endif 

#if defined(UCA_SMP)
ADLC_REQ *addToExtReqList(ADLC_CON_INFO *cptr,ADLC_REQ **adlcReqList,ST_LONG userId);
ST_VOID deleteExtReqFrame(ADLC_REQ *rptr);
ST_VOID addToUIFramesInTxList(ADLC_REQ *rptr,ST_UCHAR *data,ST_INT dataLen);
ST_VOID addToUIFramesInRxList(ADLC_CON_INFO *cptr,ST_UCHAR *data,ST_INT dataLen);
ST_VOID deleteUIFramesFromRxList(ADLC_CON_INFO *cptr);
ST_VOID deleteUIFramesFromTxList(ADLC_CON_INFO *cptr);
ST_RET sendExtWriteDoneToStack(ADLC_CON_INFO *cptr, ST_LONG userId, 
			    ST_CHAR *timeSync, ST_INT bufLen,
			    ST_RET retCode);
#endif /* UCA_SMP */

#ifdef __cplusplus
}
#endif

#endif /* _ADLC_H */



