/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : slog.h							*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/09/12  RWM           Reserved port block for Input Conditioner    */
/* 03/12/12  EJV           slogSetHdr: added slogHdrMaxSize parameter.	*/ 
/*			   Added SLOG_MAX_THREADID def&upd SLOG_MAX_HDR.*/
/* 03/09/12  RWM           Reserved port for IPC_LOG_PORT_INPUT_CONDITIONER*/
/* 02/29/12  KCR           Reserved port for IPC_LOG_PORT_EDNA_SCEN_GEN */
/* 01/31/12  MDE	   Removed thisFileName from SEV continuation   */
/* 01/06/12  KCR           Reserved port for UAP_eDnaInterface          */
/* 11/30/11  RWM           Reserved port for GDA Server Sample          */
/* 11/03/11  MDE	   Added PROCESS_DUMP defines			*/
/* 07/14/11  MDE	   Added port defines for various UAP elements	*/
/* 07/07/11  MDE	   Completed no-SEV macros			*/
/* 06/29/11  MDE	   Added SEV Trace macros, etc..		*/
/* 03/30/11  JRB	   Added logcfgx_unconfigure.			*/
/* 02/14/11  RWM           Added entries for all UIB components         */
/* 01/14/10  MDE           Added SLOGIPC_MSG_TYPE_LOG_MEM command	*/
/* 11/18/10  MDE           Added heartbeat				*/
/* 11/10/10  MDE           Added return vals to slog_ipc_std_cmd_service*/
/* 10/19/10  MDE           Added SEV commands				*/
/* 04/12/10  EJV           Added IPC_LOG_PORT_ICCP_TOOLKIT		*/
/* 02/19/10  MDE           Added lineCountLimit				*/
/* 02/23/10  EJV	   Corr SLOG_MAX_HDR (+1 for space, +1 for NULL)*/
/*			   per slogSetHdr. Added SLOG_MAX_LINE_BREAK.	*/
/* 08/25/09  RKR     90    Added more forward references                */
/* 06/10/09  EJV     89    Added LOGCFG_CFG mask and macros.		*/
/* 06/01/09  MDE     88    Added SlogClient features			*/
/* 04/06/09  EJV     87    Added LogCfgLogSettings.			*/
/* 03/26/09  KCR     86    Added PI-AF Adapter                          */
/* 03/10/09  MDE     84    Now compile in SEAL by default		*/
/* 01/31/09  EJV     84    Added comment re: GEN_SOCK_WAKEUP_PORT_BASE.	*/
/*			   Added IPC_LOG_PORT_SNAP.			*/
/* 01/24/09  MDE     83    Added IPC_LOG_LOGVIEWER			*/
/* 12/01/08  JRB     82    Move forward references to top of file.	*/
/* 11/07/08  MDE     82    Added 'Get Log Masks' feature support	*/
/* 10/08/08  MDE     81    Added SEAL					*/
/* 08/22/08  RKR     80    Added ICCP Lite                              */
/* 08/11/08  MDE     79    Added 'initialized'				*/
/* 08/04/08  MDE     78    Added slogIpcEventEx				*/
/* 07/30/08  DWL     77    Added COMTRADE listening port		*/
/* 07/25/08  MDE     76    Moved slogIpcCtx into LOG_CTRL		*/
/* 07/07/08  MDE     75    Added slogIpcActiveCount			*/
/* 05/09/08  RKR     74    Added new IPC Commands for Clone and Reset   */
/* 03/27/08  EJV     73    slog_get_index_file_name: added destLen param*/
/*                         and changed to return result.		*/
/* 03/04/08  EJV     72    Added IPC_LOG_PORT_SNAP_LITE_* ports.	*/
/*			   Removed MMSEASE_MOSI def in description.	*/
/* 			   Added LogMaskMapCtrl for 142-XXX products, 	*/
/*			     revised comments.				*/
/* 03/10/08  MDE     71    Added slog_ipc_std_cmd_service		*/
/* 03/04/08  RWM     70    Changed MAX_LOG_SIZE to 5000                 */
/* 12/14/07  DSF     69    Added sNonStandardLogMode to support .NET	*/
/*			   Logger class					*/
/* 11/30/07  DSF     68    Added IPC_LOG_IOS_GATEWAY			*/
/* 11/08/07  MDE     67    Added IPC_LOG_PORT_SDE_OPCUA_SERVER		*/
/* 10/23/07  MDE     66    Added ElapsedTime option for Windows		*/
/* 10/11/07  MDE     65    Added UAP ports				*/
/* 09/20/07  MDE     64    Added slogIpcCallingEnable, listen port	*/
/* 03/01/07  MDE     63    Tweaks to SLOGIPC command message types	*/
/* 02/20/07  CRM     62    Added SLOGIPCCALLING_NAME			*/
/* 01/22/07  MDE     61    Added Standard command message types		*/
/* 01/08/07  EJV     60    slogIpcStop: added lc param.			*/
/* 04/07/06  MDE     59    Added IPC_LOG_PORT_ICCPCFG			*/
/* 03/14/06  MDE     58    Increased SLOG_MAX_FNAME to 64 (was 32)	*/
/* 03/30/06  CRM     57    Added _slogXML, and macros			*/
/* 02/17/06  MDE     56    Added logcfg_exx, preferredTag, etc..	*/
/* 02/15/06  EJV     55    IPC_LOG_PORT_MMS_EASE - changed comment. 	*/
/* 02/02/06  EJV     54    LOGCFGX_TAG_VAL: added/changed fields.	*/
/* 12/05/05  EJV     53    Added IPC_LOG_PORT_MMS_EASE. Changed comment.*/
/* 10/28/05  EJV     52    Added MMS-LITE ports				*/
/* 10/14/05  EJV     51    Add SLOGIPC_NAME def & slogIpcEvent proto	*/
/* 08/10/05  MDE     50    Added slog_start, slog_end, index file	*/
/* 08/05/05  EJV     49    Added extern ssleLogMaskMapCtrl.     	*/
/* 08/02/05  MDE     48    Added IOS ports				*/
/* 06/30/05  EJV     47    Added extern genlistDebugMapCtrl.		*/
/*			   Arranged xxxMaskMapCtrl in alphabetical order*/
/*			   Del duplicate logCfgAddMaskGroup proto.	*/
/*			   Del logCfgAddAll proto (user must add all).	*/
/*			   Del logcfgx (use logcfgx_ex instead)		*/
/* 04/29/05  EJV     46    Added IPC_LOG_PORT_RFC1006_LISTENER def.	*/
/* 03/31/05  MDE     45    Added more SLOGALWAYS macros			*/
/* 02/22/05  JRB     44    slogIpc: add SD_CONST to some args.		*/
/* 02/16/05  JRB     43    Del #if around forward references.		*/
/* 02/10/05  MDE     42    Added STRINGBUF and DOUBLE DATATYPE's	*/
/* 02/02/05  MDE     41    Smart mode work				*/
/* 01/27/05  MDE     40    LINUX warning cleanup			*/
/* 01/19/05  MDE     39    Added LogCfg defines, etc..			*/
/* 01/19/05  EJV     38    AIX:del extern in struct tag_GEN_SOCK,C++ err*/
/* 12/06/04  ASK     37    Added SLOG IPC "smart" client features. Added*/
/*			   RUINT define for new logcfgx.		*/
/* 11/22/04  JRB     36    Add max_msg_size & msg_buf to LOG_CTRL.	*/
/*			   Add slog_max_msg_size_set proto.		*/
/*			   Add slog_max_msg_size_get macro.		*/
/*			   slogDelBuf: add (LOG_CTRL *) arg.		*/
/* 10/12/04  MDE     35    Added logcfgx mask extension mechanism 	*/
/* 08/04/04  EJV     34    LOGMEM_ITEM: chg log time,related to prev chg*/
/*		           Del slogTime, slogMs global variables.	*/
/* 07/09/04  EJV     33    Del DATETIME_MILLI_EN,add 'obsolete' comments*/
/* 06/24/04  DSF     32    Added DATETIME_MILLI_EN			*/
/* 06/24/04  EJV     31    IPC_LOG_CTRL: add portUsed.			*/
/* 06/21/04  EJV     30    IPC_LOG_CTRL: add portCnt, appId fields.	*/
/* 05/26/04  EJV     29    Added comment to keep track on used IPC ports*/
/* 05/20/04  EJV     28    IPC_LOG_CTRL: limited log message queue.	*/
/* 05/18/04  MDE     27    Removed LOG_IPC_SUPPORT #ifdef's		*/
/* 05/11/04  EJV     26    Revised LOG_IPC_SUPPORT code to use gensock2.*/
/*			   Added SLOG_MAX_LOGTYPESTR, SLOG_MAX_HDR.	*/
/*			   Added slogSetHdr() proto.			*/
/* 02/10/04  KCR     25    Added slogDelBuf()                           */
/* 10/24/03  JRB     24    Move gs_debug_sel from glbsem.h to here.	*/
/*			   Move clnp_debug_sel from clnp_usr.h to here.	*/
/*			   Del windows.h (see sysincs.h).		*/
/* 06/20/03  JRB     23    Del ;  from SLOGALWAYS* too.			*/
/* 05/08/03  JRB     22    Del {} from SLOGALWAYS* so they work in	*/
/*			   almost any context.				*/
/* 05/07/03  DSF     21    Increased the value of SLOG_MAX_FNAME	*/
/* 			   Added support for sErrLogCtrl		*/
/* 10/17/02  JRB     20    Add "Base" macros SLOG_*, SLOGC_*, SLOGH.	*/
/* 03/11/02  JRB     19    SLOG_PAUSE, SLOG_LF, SLOG_CLRSCR do nothing.	*/
/* 04/26/01  DSF     18    Define SOCKET if not yet defined		*/
/* 04/16/01  DSF     17    Include windows.h instead of including	*/
/*			   winsock.h directly				*/
/* 03/23/01  MDE     16    Added _slogStr, SLOGALWAYSS 			*/
/* 11/27/00  JRB     15    Added read_log_cfg_file, logcfgx protos.	*/
/* 08/22/00  KCR     14    Added FIL_CTRL_NO_LOG_HDR			*/
/* 03/09/00  MDE     13    Added SD_CONST modifiers to LOGMEM_ITEM	*/
/* 09/13/99  MDE     12    Added SD_CONST modifiers			*/
/* 11/11/98  DSF     11    Minor changes to _slog_dyn_log_fun		*/
/* 10/16/98  DSF     10    Spelling					*/
/* 10/08/98  MDE     09    Migrated to updated SLOG interface		*/
/* 06/15/98  NAV     08    Conditionally include winsock.h or winsock2.h*/
/* 06/03/98  MDE     07    Replaced 'winsock.h' with 'windows.h'	*/
/* 12/12/97  MDE     06    Added DEBUG_SISCO versions of SLOGALWAYSx	*/
/* 11/05/97  DSF     05    Added SYSTIME_EN				*/
/* 09/12/97  DSF     04    Expose slogSetTimeText ()			*/
/* 06/11/97  MDE     03    Include time.h for all			*/
/* 05/27/97  DSF     02    Added IPC logging capability			*/
/* 05/27/97  DSF     01    Include time.h for sun			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef SLOG_INCLUDED
#define SLOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


/* Allow compile out for now, will phase out this option */
#define SLOGIPC_SEAL

#include "gen_list.h"
#ifdef SLOGIPC_SEAL
#include "sseal.h"
#endif

#if defined(SEV_SUPPORT)
#include "sev.h"
#endif

#include <stdio.h>
#include <time.h>	/* for time_t */

#define TIME_BUF_LEN 30

/************************************************************************/
struct tag_GEN_SOCK;		/* forward reference	*/
struct tag_GEN_SOCK_DATA;	/* forward reference	*/
struct log_ctrl;		/* forward reference	*/
struct tag_SLOGIPC_RX_IND;	/* forward reference	*/
struct tag_SLOGIPC_CONNECT_IND;	/* forward reference	*/
struct tag_SLOGIPC_DISCONNECT_IND;	/* forward reference	*/

/************************************************************************/
/* Memory logging string buffer size limit				*/
/* NOTE : Do not change this lightly! QMEM.C should be looked at	*/
/************************************************************************/

#define SLOG_MEM_BUF_SIZE 125

/* Memory logging item control						*/
/* A table of these is established at initialization time		*/

#define LMF_USED	0x0001
#define LMF_HEADER	0x0002
#define LMF_HEX		0x0004


typedef struct logmem_item
  {
  ST_UINT flags;		/* see LMF_xxx flags			*/
  ST_INT logType;		/* log type storage			*/
  SD_CONST ST_CHAR *logTypeStr;	/* log type string			*/
  ST_INT lineNum;		/* source file line number		*/
  SD_CONST ST_CHAR *sourceFile;	/* source file name, NULL for none	*/
  ST_CHAR slogTimeText[TIME_BUF_LEN];  
  ST_CHAR *string;		/* text						*/
  } LOGMEM_ITEM;


/************************************************************************/
/* File Logging State flags						*/
/************************************************************************/

#define FIL_STATE_OPEN 	       0x0001
#define FIL_STATE_NEED_WIPE    0x0002

/* File Logging Control flags						*/

#define FIL_CTRL_WIPE_EN       0x0001
#define FIL_CTRL_WRAP_EN       0x0002
#define FIL_CTRL_MSG_HDR_EN    0x0004	
#define FIL_CTRL_NO_LOG_HDR    0x0040	

/* File output mode flags						*/

#define FIL_CTRL_NO_APPEND     0x0008	
#define FIL_CTRL_HARD_FLUSH    0x0010	
#define FIL_CTRL_SETBUF_EN     0x0020	

typedef struct file_log_ctrl
  {
/* User sets these elements						*/
  ST_ULONG  maxSize;
  ST_CHAR  *fileName;		/* destination filename			*/
  ST_UINT ctrl;			/* see FIL_CTRL_xxx flags		*/
  ST_LONG wipeFilePos;

/* Internal use only							*/
  ST_UINT state;		/* see FIL_STATE_xxx flags		*/
  FILE  *fp;
  } FILE_LOG_CTRL;

/* Memory Logging State flags						*/

#define MEM_STATE_INIT        0x0001

/* Memory Logging Control flags						*/

#define MEM_CTRL_MSG_HDR_EN   0x0001
#define MEM_CTRL_AUTODUMP_EN  0x0002
#define MEM_CTRL_HEX_LOG      0x0004

typedef struct mem_log_ctrl
  {
/* User sets these elements						*/
  ST_INT maxItems;	 /* # items to allocate at powerup		*/
  ST_CHAR *dumpFileName; /* where memory dump goes			*/
  ST_UINT ctrl;	    	 /* see MEM_CTRL_xxx flags			*/

/* Internal use only							*/
  ST_UINT state;	/* see MEM_STATE_xxx flags			*/
  LOGMEM_ITEM *item; 	/* Item table					*/
  ST_INT nextPut; 	/* Current position (where last was put		*/
  } MEM_LOG_CTRL;

/************************************************************************/
/*		Socket Slogging Subsystem				*/
/************************************************************************/
#define _SLOGIPC_MAX_OPTIONS_LEN	1000
#define _SLOGIPC_MAX_DATA_SIZE	     1000000


#define	SLOGIPC_NAME			"SLOGIPC"  /* used in GEN_SOCK	*/
#define	SLOGIPCCALLING_NAME		"SLOGIPCCALLING"  /* used in GEN_SOCK	*/

/* gensock2 ports: reserve 90:			           55050-55139  */
/* see GEN_SOCK_WAKEUP_PORT_* defines in gensock2.h			*/

/* Default listen port for LogViewer	*/
#define IPC_LOG_LOGVIEWER_PORT		55146

/* default parameters for socket logging subsystem			*/
#define IPC_LOG_BASE_PORT		55147

/* ports below are taken by following SISCO applications:		*/
#define IPC_LOG_PORT_AXS4ICCP		IPC_LOG_BASE_PORT+1 /* 55148 */
#define IPC_LOG_PORT_OSILL2		IPC_LOG_BASE_PORT+2 /* 55149 */
#define IPC_LOG_PORT_OSILL2_IPC		IPC_LOG_BASE_PORT+3 /* 55150 */
#define IPC_LOG_PORT_AXS4MMS		IPC_LOG_BASE_PORT+4 /* 55151 */
#define IPC_LOG_PORT_AXS4IEC61850	IPC_LOG_BASE_PORT+5 /* 55152 */
#define IPC_LOG_PORT_AXS4GOOSE_OLD	IPC_LOG_BASE_PORT+6 /* 55153 */
#define IPC_LOG_PORT_RFC1006_LISTENER	IPC_LOG_BASE_PORT+7 /* 55154 */
#define IPC_LOG_PORT_ICCPCFG		IPC_LOG_BASE_PORT+8 /* 55155 */

/* UIB ports: reserve 10:			           55157-55167 */	
#define IPC_LOG_PORT_UIBBASE            IPC_LOG_BASE_PORT+10  /* 55157 */
#define IPC_LOG_PORT_OPCDASERVER        IPC_LOG_BASE_PORT+10  /* 55157 */
#define IPC_LOG_PORT_OPCDACLIENT        IPC_LOG_BASE_PORT+11  /* 55158 */
#define IPC_LOG_PORT_OPCHDASERVER       IPC_LOG_BASE_PORT+12  /* 55159 */
#define IPC_LOG_PORT_OPCHDACLIENT       IPC_LOG_BASE_PORT+13  /* 55160 */
#define IPC_LOG_PORT_UIBREGISTRAR       IPC_LOG_BASE_PORT+14  /* 55161 */
#define IPC_LOG_PORT_UIBCONFIG          IPC_LOG_BASE_PORT+15  /* 55162 */
#define IPC_LOG_PORT_GDASERVERSMS       IPC_LOG_BASE_PORT+16  /* 55163 */
#define IPC_LOG_PORT_SMSUPDATER         IPC_LOG_BASE_PORT+17  /* 55164 */
#define IPC_LOG_PORT_GDAMCM             IPC_LOG_BASE_PORT+18  /* 55165 */
#define IPC_LOG_PORT_GDASERVERSAMPLE    IPC_LOG_BASE_PORT+19  /* 55166 */

#define IPC_LOG_NUMPORTS_UIB		10

#define IPC_LOG_PORT_SNAP		IPC_LOG_BASE_PORT+30 /* 55177 */

#define IPC_LOG_PORT_SNAP_LITE_DAEMON	IPC_LOG_BASE_PORT+33 /* 55180 */
#define IPC_LOG_PORT_SNAP_LITE_UTIL	IPC_LOG_BASE_PORT+34 /* 55181 */

/* ICCP Toolkit demo programs: reserve 2		  55188-55189 */
#define IPC_LOG_PORT_ICCP_TOOLKIT	IPC_LOG_BASE_PORT+41 /* 55188 */

/* MMS-EASE demo programs: reserve 10			   55190-55199 */
#define IPC_LOG_PORT_MMS_EASE		IPC_LOG_BASE_PORT+43 /* 55190 */

/* MMS-LITE demo programs: reserve 10			   55200-55209 */
#define IPC_LOG_PORT_MMS_LITE		IPC_LOG_BASE_PORT+53 /* 55200 */

/* ICCP-LITE demo programs: reserve 5			   55210-55214 */
#define IPC_LOG_PORT_ICCP_LITE		IPC_LOG_BASE_PORT+63 /* 55210 */

/* USPS IOS ports: reserve 100 :			   55347-55447 */	
#define IPC_LOG_IOS_MPECOM		IPC_LOG_BASE_PORT+200 /* 55347 */
#define IPC_LOG_IOS_MPE			IPC_LOG_BASE_PORT+201 /* 55348 */
#define IPC_LOG_IOS_GATEWAY		IPC_LOG_BASE_PORT+202 /* 55349 */

/* UAP related elements */
#define IPC_LOG_PORT_AXS4GOOSE		 IPC_LOG_BASE_PORT+300 /* 55447 */
#define IPC_LOG_PORT_DSPI		 IPC_LOG_BASE_PORT+301 /* 55448 */
#define IPC_LOG_PORT_SCLDEVLOADEX        IPC_LOG_BASE_PORT+302 /* 55449 */
#define IPC_LOG_PORT_UAPPC		 IPC_LOG_BASE_PORT+303 /* 55450 */
#define IPC_LOG_PORT_GOOSEBLASTER	 IPC_LOG_BASE_PORT+304 /* 55451 */
#define IPC_LOG_PORT_GOOSEBLASTERCONTROL IPC_LOG_BASE_PORT+305 /* 55452 */
#define IPC_LOG_PORT_SDE_ATM		 IPC_LOG_BASE_PORT+306 /* 55453 */
#define IPC_LOG_PORT_SDE_MONITOR	 IPC_LOG_BASE_PORT+307 /* 55454 */
#define IPC_LOG_PORT_SDE_DRIVER		 IPC_LOG_BASE_PORT+308 /* 55455 */
#define IPC_LOG_PORT_SDE_OPCUA_SERVER	 IPC_LOG_BASE_PORT+309 /* 55456 */
#define IPC_LOG_PORT_PI_SCEN_GEN 	 IPC_LOG_BASE_PORT+310 /* 55457 */

#define IPC_LOG_PORT_UAP_DB_CTRL 	 IPC_LOG_BASE_PORT+311 /* 55458 */
#define IPC_LOG_PORT_UAP_SCEN_PLAYER 	 IPC_LOG_BASE_PORT+312 /* 55459 */
#define IPC_LOG_PORT_UAP_OPC_CLIENT 	 IPC_LOG_BASE_PORT+313 /* 55460 */	
#define IPC_LOG_PORT_UAP_OPC_SERVER 	 IPC_LOG_BASE_PORT+314 /* 55461 */
#define IPC_LOG_PORT_UAP_EVENT_MANAGER 	 IPC_LOG_BASE_PORT+315 /* 55462 */
#define IPC_LOG_PORT_UAP_EDNA_INTERFACE  IPC_LOG_BASE_PORT+316 /* 55463 */
#define IPC_LOG_PORT_EDNA_SCEN_GEN 	 IPC_LOG_BASE_PORT+317 /* 55464 */

/* PI-AF Adapter ports */
#define IPC_LOG_PORT_PIAF_MANAGEMENT_TOOLS	 IPC_LOG_BASE_PORT+340 /* 55487 */
#define IPC_LOG_PORT_PIAF_MCM				 IPC_LOG_BASE_PORT+341 /* 55488 */
#define IPC_LOG_PORT_USL_EXPLORER			 IPC_LOG_BASE_PORT+342 /* 55489 */

/* COMTRADE Utility ports */
#define IPC_LOG_PORT_COMTRADE_UTILITY	 IPC_LOG_BASE_PORT+350 /* 55497 */

/* Used when LogViewer is a log source */
#define IPC_LOG_LOGVIEWER		 IPC_LOG_BASE_PORT+351 /* 55498 */

/* Start UAP analytics at this value */
#define IPC_LOG_PORT_UAP_AA_BASE 	 IPC_LOG_BASE_PORT+400 /* 55547  */


#define IPC_LOG_PORT_INPUT_CONDITIONER_BASE IPC_LOG_BASE_PORT+500 /* 55647 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_0    IPC_LOG_BASE_PORT+500 /* 55647 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_1    IPC_LOG_BASE_PORT+501 /* 55648 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_2    IPC_LOG_BASE_PORT+501 /* 55649 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_3    IPC_LOG_BASE_PORT+501 /* 55650 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_4    IPC_LOG_BASE_PORT+501 /* 55651 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_5    IPC_LOG_BASE_PORT+501 /* 55652 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_6    IPC_LOG_BASE_PORT+501 /* 55653 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_7    IPC_LOG_BASE_PORT+501 /* 55654 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_8    IPC_LOG_BASE_PORT+501 /* 55655 */
#define IPC_LOG_PORT_INPUT_CONDITIONER_9    IPC_LOG_BASE_PORT+501 /* 55656 */

#define IPC_LOG_MAX_CONNECTIONS		10
#define IPC_LOG_MAX_QUEUE_CNT		100

/* SMART MODE */
/* Format of smart message is 3 unsigned long integers:			*/
/* 	unsigned long	magicNumber = SLOGIPC_MAGIC_NUMBER;		*/
/* 	unsigned long	messageType = SLOGIPC_MSG_TYPE_XXX		*/
/* 	unsigned long	messageLen = dataLength 			*/

/* Followed by one of:					*/
/* 	nothing						*/
/* 	char[messageLen]				*/
/* 	unsigned long	missedMessageCount		*/

#define SLOGIPC_MAGIC_NUMBER_SEAL   0x1F2E3D4D
#define SLOGIPC_MAGIC_NUMBER_NOSEAL 0x1F2E3D4C

typedef struct
  {
  ST_UINT32 magicNumber;
  ST_UINT32 messageType;
  ST_UINT32 messageLen;
  } SLOGIPC_SMART_MSG_HDR;

ST_VOID _slogIpcInitHdr (struct tag_GEN_SOCK *pSock,
			 struct tag_GEN_SOCK_DATA *sockData, 
			 ST_UINT32 msgType, ST_INT dataLen);

#ifdef SLOGIPC_SEAL
typedef struct
  {
  ST_UINT32 magicNumber;
  ST_UINT32 messageType;
  ST_UINT32 messageLen;
  S_SEAL    seal;
  } SLOGIPC_SMART_MSG_HDRS;
ST_VOID _slogIpcSetSeal (SLOGIPC_SMART_MSG_HDRS *sHdrS);
ST_RET _slogIpcCheckSeal (struct log_ctrl *lc, SLOGIPC_SMART_MSG_HDRS *sHdrS);
#endif

/* message types */
#define SLOGIPC_MSG_TYPE_LOG      	0x00000000
#define SLOGIPC_MSG_TYPE_RESERVED 	0x00000001
#define SLOGIPC_MSG_TYPE_APPID    	0x00000002
#define SLOGIPC_MSG_TYPE_START    	0x00000003
#define SLOGIPC_MSG_TYPE_STOP     	0x00000004

/* Standard command message response types */
#define SLOGIPC_MSG_TYPE_RESP_SUCCESS	0x00000005
#define SLOGIPC_MSG_TYPE_RESP_FAILURE	0x00000006

/* Standard command message types */
#define SLOGIPC_MSG_TYPE_OPTIONS	0x00000007
#define SLOGIPC_MSG_TYPE_READ_LOGCFG  	0x00000008
#define SLOGIPC_MSG_TYPE_WRITE_LOGCFG 	0x00000009

/* IF we missed log messages ... */
#define SLOGIPC_MSG_TYPE_LOG_MISSED    	0x0000000A

/* Heartbeat, used to detect connections that are down */
#define SLOGIPC_MSG_TYPE_HEARTBEAT 	0x0000000B

#define SLOGIPC_MSG_TYPE_CLONE_LOG  	0x00000010
#define SLOGIPC_MSG_TYPE_RESET_LOG  	0x00000020
#define SLOGIPC_MSG_TYPE_WRITE_MSGDATA 	0x00000030
#define SLOGIPC_MSG_TYPE_GET_LOGMASKS 	0x00000040
#define SLOGIPC_MSG_TYPE_LOG_MEM  	0x00000045


/* SEV commands */
#define SLOGIPC_MSG_TYPE_SEV_LOG_STATE 		0x00000050
#define SLOGIPC_MSG_TYPE_SEV_STOP 		0x00000051
#define SLOGIPC_MSG_TYPE_SEV_LOG_STATE_AND_STOP 0x00000053
#define SLOGIPC_MSG_TYPE_SEV_TRACE_PAUSE	0x00000054
#define SLOGIPC_MSG_TYPE_SEV_TRACE_RESUME	0x00000055
#define SLOGIPC_MSG_TYPE_SEV_CLONE    		0x00000056
#define SLOGIPC_MSG_TYPE_PROCESS_DUMP    	0x00000057

/* General application specific command */
#define SLOGIPC_MSG_TYPE_CMD      	0x00000100

/* Defines for options (bitmasked) */
#define LOG_IPC_EDIT_LOGCFG     	0x00000001

/* configuration parameters for socket logging & command subsystem	*/
typedef struct ipc_log_ctrl_tag
  {
  /* Used for client (calling) connections */
  ST_CHAR   *callingIp;		/* IP or host name of host to connect to*/
  ST_UINT16  callingPort;	/* port of host to connect to		*/
  ST_ULONG   callingBackoff;	/* ms 					*/
  ST_INT     sealMode;
  ST_INT     sealTimeWindow;

  /* Used for server (called) connections */
  ST_UINT16  port;		/* listening port number		*/
  ST_UINT16  portCnt;		/* num of listening ports starting with	*/
				/* base port, for multiple app instances*/

  ST_UINT16  portUsed;		/* this the listening port actually used*/
  ST_UINT    maxConns;		/* max connections that can be accepted	*/

  /* Misc parameters */
  ST_UINT    maxQueCnt;		/* max num of logs that can be queued	*/

  ST_CHAR   *appId;		/* pointer to NULL terminated string	*/
				/* identifying the application, sent to	*/
				/* IPC Logging Client in first message	*/

  ST_UINT32  options;		/* Options supported by the app		*/
  ST_INT lineCountLimit;	/* Limit # lines for long text or hex	*/ 

  ST_DOUBLE heartbeatTxPeriod;	/* How often to send heartbeat		*/

  /* command processing function pointer - to be set by user application  */
  ST_VOID (*slog_ipc_cmd_fun) (struct tag_SLOGIPC_RX_IND *slogIpcRxInd);

  /* Client override for SLOGIPC indications, typically used by clients	*/
  ST_VOID (*slog_ipc_rx_ind)         (struct tag_SLOGIPC_RX_IND *slogIpcRxInd);
  ST_VOID (*slog_ipc_connect_ind)    (struct tag_SLOGIPC_CONNECT_IND *slogIpcConnectInd);
  ST_VOID (*slog_ipc_disconnect_ind) (struct tag_SLOGIPC_DISCONNECT_IND *slogIpcDisconnectInd);
  ST_BOOLEAN client;
  } IPC_LOG_CTRL;




ST_VOID slogIpcCallingEnable (ST_BOOLEAN enable);
ST_VOID slogIpcCallingEnableEx (struct log_ctrl *lc, ST_BOOLEAN enable);
ST_VOID slogIpcHeartbeatService(struct log_ctrl *lc);

ST_INT slogIpcActiveCount (struct log_ctrl *lc);

/************************************************************************/
/* SLOG IPC Context */

/* connection states */
#define SLOGIPC_CON_IDLE	0
#define SLOGIPC_CONNECTED	1
#define SLOGIPC_CON_CLOSING	2

/* Remote connection info */
typedef struct slogipc_con_ctrl_tag
  {
  DBL_LNK	 link;			/* link list 				*/
  struct tag_GEN_SOCK      *conSock;	/* pointer to socket info		*/
  
  ST_INT 		    sealMode;	/* Calculated parameters		*/
  ST_INT 		    hdrSize;	 
  ST_INT 		    magicNumber;

  ST_UINT		    conState;	/* connection state			*/
  ST_BOOLEAN		    txEnable;	/* Used in smart mode			*/
  struct tag_GEN_SOCK_DATA *sendQue;	/* Pending send queue to the Remote	*/
  ST_UINT		    sendQueCnt;	/* number of messages to send in queue	*/
  struct tag_GEN_SOCK_DATA *rcvBuf;	/* alloc buf to drain any rcvd bytes	*/

  ST_VOID		   *usr;	/* User */
  } SLOGIPC_CON_CTRL;


/* SLOGIPC context states */
#define	SLOGIPC_CTX_IDLE		0
#define	SLOGIPC_CTX_ACTIVE		1
#define	SLOGIPC_CTX_TERMINATING		2

/* This structure holds core parameters */
typedef struct slogipc_ctx_tag
  {
  ST_UINT		      state;		/* SLOGIPC_CTX_*					*/
  ST_BOOLEAN		      smartMode;	/* SD_TRUE if smart mode enabled, SD_FALSE otherwise	*/

  ST_BOOLEAN		      client; 		/* SD_TRUE for SLOGIPC client use			*/
  ST_INT 		      sealMode;		/* SSEAL_MODE_NONE, SSEAL_MODE_FALLBACK, SSEAL_MODE_ALL	*/

  SLOGIPC_CON_CTRL	      *conCtrlList;	/* list of connect CTRLs				*/
  struct tag_GEN_SOCK         *listenSock;	/* listen socket for conns from remote slog Clients	*/
  struct tag_GEN_SOCK         *callingSock;	/* calling socket for conns to remote slog Client	*/
  struct log_ctrl	      *lc;		/* save to access appId when connected			*/
  struct tag_GEN_SOCK_CTXT    *sockCtx;		/* this is gensock2 context for slogipc.c		*/

  ST_BOOLEAN 	    	      callingEnabled;
  ST_DOUBLE 	    	      nextCallingConnectTime;

  ST_VOID		     *usr;
  } SLOGIPC_CTX;


/* SLOG IPC Client */
typedef struct tag_SLOGIPC_RX_IND
  {
  SLOGIPC_CON_CTRL         *conCtrl;
  SLOGIPC_CTX 	           *ctx;
  struct tag_GEN_SOCK      *pSock;
  struct tag_GEN_SOCK_DATA *sockData;
  ST_CHAR 	           *msgData;
  ST_UINT32  	            msgType;
  ST_UINT32  	            msgDataLen;
  } SLOGIPC_RX_IND;


typedef struct tag_SLOGIPC_CONNECT_IND
  {
  SLOGIPC_CON_CTRL         *conCtrl;
  SLOGIPC_CTX 	           *ctx;
  struct tag_GEN_SOCK      *pSock;
  } SLOGIPC_CONNECT_IND;

typedef struct tag_SLOGIPC_DISCONNECT_IND
  {
  SLOGIPC_CON_CTRL         *conCtrl;
  SLOGIPC_CTX 	           *ctx;
  struct tag_GEN_SOCK      *pSock;
  } SLOGIPC_DISCONNECT_IND;


/************************************************************************/
/* LOG CONTROL								*/
/* A structure of this type is used for each independent logging entity	*/
/************************************************************************/

/* Log control mask values						*/
#define LOG_MEM_EN	     	0x0001L
#define LOG_FILE_EN	     	0x0002L
#define LOG_TIME_EN	     	0x0008L
#define LOG_ELAPSEDTIME_EN     	0x0020L		/* Windows only */

#define LOG_TIMEDATE_EN	     	0x0010L		/* obsolete - ignored	*/
#define LOG_DIFFTIME_EN	     	0x0020L		/* obsolete - ignored	*/
#define LOG_TIME_INITIALIZED 	0x0040L		/* obsolete - ignored	*/
#define LOG_SYSTIME_EN	     	0x0100L		/* obsolete - ignored	*/

#define LOG_NO_HEADER_CR     	0x0200L
#define LOG_FILENAME_SUPPRESS 	0x0400L
#define LOG_LOGTYPE_SUPPRESS  	0x0800L

#define LOG_IPC_LISTEN_EN      	0x1000L
#define LOG_IPC_CALL_EN     	0x2000L
#define LOG_IPC_SMART     	0x4000L
#define LOG_IPC_EN   (LOG_IPC_LISTEN_EN|LOG_IPC_CALL_EN)

#define SLOG_NORMAL 	 0
#define SLOG_CONT 	-1
/* Dynamic logging standard command definitions				*/
#define	SLOG_DYN_PAUSE  -10	/* OBSOLETE	*/
#define	SLOG_DYN_LF     -11	/* OBSOLETE	*/
#define	SLOG_DYN_CLRSCR -12	/* OBSOLETE	*/

typedef struct log_ctrl
  {
/* User sets these elements						*/
  ST_UINT32 logCtrl; 	/* Logging Control flags - see LOG_xxx defines	*/
  FILE_LOG_CTRL fc;	/* File logging control				*/
  MEM_LOG_CTRL  mc;	/* Memory logging control			*/
  IPC_LOG_CTRL  ipc;	/* IPC logging control				*/


/* Application specific information					*/
  ST_UINT32 logMask1;	/* These bit masked variables are used by the	*/
  ST_UINT32 logMask2;	/* user application to determine whether a item	*/
  ST_UINT32 logMask3;	/* is to be logged. 6 ST_INT32's == 192 bits	*/
  ST_UINT32 logMask4;
  ST_UINT32 logMask5;
  ST_UINT32 logMask6;

/* Internal variables.							*/
  ST_BOOLEAN initialized; /* Set when LOG_CTRL has been initializled	*/
  ST_INT max_msg_size;	  /* max allowed log msg size.			*/
  ST_CHAR *msg_buf;	  /* buffer for log msg				*/

/* static vars */
  SLOGIPC_CTX slogIpcCtx;
  } LOG_CTRL;

/* Max size of any single log statement (sprintf)			*/

#define MAX_LOG_SIZE 5000

/* Set the following variable to a number larger than the default of    */
/* MAX_LOG_SIZE if you want to log more than 500 bytes in a single      */
/* message.                                                             */

extern ST_INT sl_max_msg_size;

/************************************************************************/
/* Main entry points into the SLOG library				*/
/************************************************************************/

ST_VOID slog (LOG_CTRL *lc, 
	      SD_CONST ST_INT logType, 
	      SD_CONST ST_CHAR *SD_CONST sourceFile, 
	      SD_CONST ST_INT lineNum, 
	      SD_CONST ST_CHAR *format, ...);

ST_VOID slogx (ST_UINT32 doit, LOG_CTRL *lc, 
	       SD_CONST  ST_INT logType, 
	       SD_CONST ST_CHAR *SD_CONST sourceFile, 
	       SD_CONST ST_INT lineNum, 
	       SD_CONST ST_CHAR *format, ...);

ST_VOID slogHex (LOG_CTRL *lc, 
		 SD_CONST ST_INT logType, 
		 SD_CONST ST_CHAR *SD_CONST fileName, 
		 SD_CONST ST_INT lineNum,
	  	 SD_CONST ST_INT numBytes, 
		 SD_CONST ST_VOID *hexData);


ST_VOID _slog (LOG_CTRL *lc, 
	       SD_CONST ST_CHAR *SD_CONST logTypeStr, 
	       SD_CONST ST_CHAR *SD_CONST sourceFile, 
	       SD_CONST ST_INT	lineNum, 
	       SD_CONST ST_CHAR *format, ...);
ST_VOID _slogc (LOG_CTRL *lc, SD_CONST ST_CHAR *format, ...);

ST_VOID _slogx (ST_UINT32 doit, LOG_CTRL *lc, 
		SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		SD_CONST ST_CHAR *SD_CONST sourceFile, 
		SD_CONST ST_INT	lineNum, 
		SD_CONST ST_CHAR *format, ...);

ST_VOID _slogHex (LOG_CTRL *lc, 
		  ST_INT numBytes, 
		  SD_CONST ST_VOID *hexData);

ST_VOID _slogXML (LOG_CTRL *lc, 
  	      SD_CONST ST_CHAR *SD_CONST logTypeStr, 
  	      SD_CONST ST_CHAR *SD_CONST sourceFile, 
  	      SD_CONST ST_INT lineNum, 
  	      ST_UINT  numBytes,
  	      SD_CONST ST_CHAR *textData);

ST_VOID _slogStr (LOG_CTRL *lc, ST_CHAR *strData);

ST_RET slog_start (LOG_CTRL *lc, ST_INT max_msg_size);

typedef struct
  {
  LOG_CTRL *lc;
  ST_INT max_msg_size;
  ST_BOOLEAN client;
  ST_VOID (*slog_ipc_rx_ind)        (struct tag_SLOGIPC_RX_IND *slogIpcRxInd);
  ST_VOID (*slog_ipc_connect_ind)   (struct tag_SLOGIPC_CONNECT_IND *slogIpcConnectInd);
  ST_VOID (*slog_ipc_disconnect_ind)(struct tag_SLOGIPC_DISCONNECT_IND *slogIpcDisconnectInd);
  } SLOG_INIT_PARAM;
ST_RET slog_start_ex (SLOG_INIT_PARAM *ip);

ST_RET slog_end (LOG_CTRL *lc);

ST_RET slog_max_msg_size_set (LOG_CTRL *lc, ST_INT max_msg_size);
/* Use macro to get max msg size. Faster.	*/
#define slog_max_msg_size_get(log_ctrl)		(log_ctrl->max_msg_size)

ST_VOID slogIpcSendData (struct tag_GEN_SOCK *pSock, ST_UINT32 msgType, 
			 ST_INT dataLen, ST_UCHAR *data);
ST_VOID slogIpcSendRawData (LOG_CTRL *lc, ST_INT bufLen, ST_CHAR *buf, ST_BOOLEAN sendAlways);

/* Standard command handling support */
typedef struct
  {
  DBL_LNK l;
  SLOGIPC_RX_IND slogIpcRxInd;
  ST_CHAR *msgData;
  } SLOGIPC_CMD;

extern SLOGIPC_CMD *slogIpcCmdList;
extern ST_INT       slogIpcMaxCmdPend;

ST_VOID slog_ipc_std_cmd_fun (SLOGIPC_RX_IND *slogIpcRxInd);

/* Return codes from standard command handler */
#define SLOGIPC_CMD_NONE_OK			 0
#define SLOGIPC_CMD_UNKNOWN_FAILED		 1

#define SLOGIPC_CMD_READ_LOGCFG_OK		 2
#define SLOGIPC_CMD_READ_LOGCFG_FAILED		 3
#define SLOGIPC_CMD_WRITE_LOGCFG_OK		 4
#define SLOGIPC_CMD_WRITE_LOGCFG_FAILED		 5
#define SLOGIPC_CMD_CLONE_OK			 6
#define SLOGIPC_CMD_CLONE_FAILED		 7
#define SLOGIPC_CMD_RESET_OK			 8
#define SLOGIPC_CMD_RESET_FAILED		 9
#define SLOGIPC_CMD_WRITE_MSGDATA_OK		10
#define SLOGIPC_CMD_WRITE_MSGDATA_FAILED	11
#define SLOGIPC_CMD_GET_LOGMASKS_OK		12
#define SLOGIPC_CMD_GET_LOGMASKS_FAILED		13
						 
#define SLOGIPC_CMD_SEV_STOP_OK			14
#define SLOGIPC_CMD_SEV_STOP_FAILED		15
#define SLOGIPC_CMD_SEV_CLONE_OK		16
#define SLOGIPC_CMD_SEV_CLONE_FAILED		17
#define SLOGIPC_CMD_SEV_LOG_AND_STOP_OK		18
#define SLOGIPC_CMD_SEV_LOG_AND_STOP_FAILED	19
#define SLOGIPC_CMD_SEV_LOG_OK			10
#define SLOGIPC_CMD_SEV_LOG_FAILED		11
#define SLOGIPC_CMD_SEV_PAUSE_OK		12
#define SLOGIPC_CMD_SEV_PAUSE_FAILED		13
#define SLOGIPC_CMD_SEV_RESUME_OK		14
#define SLOGIPC_CMD_SEV_RESUME_FAILED		15

#define SLOGIPC_CMD_PROCESS_DUMP_OK		16
#define SLOGIPC_CMD_PROCESS_DUMP_FAILED		17

#define SLOGIPC_CMD_EXTERN_HANDLER		50

ST_INT slog_ipc_std_cmd_service (ST_CHAR *logCfgFile, 	    /* As used in logcfgx_exx	*/
				  ST_CHAR *fileNamePrefix,  /* ditto		  	*/
                                  ST_CHAR *cName,	    /* ditto 		  	*/
				  ST_BOOLEAN callingEnable,
				  ST_RET (*logReloadHandler) (ST_VOID),
				  ST_RET (*extendedCmdHandler) (SLOGIPC_CMD *slogIpcCmd));

ST_INT slog_ipc_std_cmd_service_ex (struct log_ctrl *lc,
				  ST_CHAR *logCfgFile, 	    /* As used in logcfgx_exx	*/
				  ST_CHAR *fileNamePrefix,  /* ditto		  	*/
                                  ST_CHAR *cName,	    /* ditto 		  	*/
				  ST_BOOLEAN callingEnable,
				  ST_RET (*logReloadHandler) (ST_VOID),
				  ST_RET (*extendedCmdHandler) (SLOGIPC_CMD *slogIpcCmd));
/************************************************************************/
/* Main entry points into the SLOG library 				*/
/************************************************************************/

/* File Logging Utility Functions					*/

ST_VOID slogCloseFile (LOG_CTRL *lc);
ST_VOID slogCloneFile (LOG_CTRL *lc, SD_CONST ST_CHAR *newfile);
ST_VOID slogCloneFileEx (LOG_CTRL *lc, SD_CONST ST_CHAR *newFile);
ST_VOID slogDeleteFile (LOG_CTRL *lc);
ST_VOID slogCallStack (LOG_CTRL *lc, SD_CONST ST_CHAR *txt);
ST_VOID slogDelBuf (LOG_CTRL *lc);

/* Memory Logging Utility Functions					*/

ST_VOID slogDumpMem (LOG_CTRL *lc);
ST_VOID slogResetMem (LOG_CTRL *lc);
ST_CHAR *slogGetMemMsg (LOG_CTRL *lc, SD_CONST ST_INT msgNum);
ST_INT slogGetMemCount (LOG_CTRL *lc);

/* Internal functions							*/

ST_VOID slogFile (LOG_CTRL *lc, SD_CONST ST_INT logType, 
		SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		SD_CONST ST_CHAR *SD_CONST sourceFile, 
		SD_CONST ST_INT lineNum, 
		SD_CONST ST_INT bufLen, SD_CONST ST_CHAR *buf);

ST_VOID slogMem (LOG_CTRL *lc, 
		SD_CONST ST_INT logType, 
		SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		SD_CONST ST_CHAR *SD_CONST sourceFile, 
		SD_CONST ST_INT lineNum, 
		ST_INT bufLen, SD_CONST ST_CHAR *buf);

ST_RET  slogIpcInit (LOG_CTRL *lc);
ST_RET  slogIpcStop (LOG_CTRL *lc);
ST_RET  slogIpcEvent (ST_VOID);
ST_RET  slogIpcEventEx (LOG_CTRL *lc);

ST_VOID slogIpc (LOG_CTRL *lc, ST_INT logType,
		 SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		 SD_CONST ST_CHAR *SD_CONST sourceFile,
		 ST_INT lineNum, ST_INT bufLen, 
                 ST_CHAR *buf);

ST_VOID slogMemInit (LOG_CTRL *lc);
ST_RET  read_log_cfg_file (ST_CHAR *cfg_filename);
ST_RET  slog_get_index_file_name (LOG_CTRL *lc, ST_CHAR *dest, ST_INT destLen);

/************************************************************************/

/* dataType defines */
#define _LOGCFG_DATATYPE_UINT_MASK	0
#define _LOGCFG_DATATYPE_UINT32_MASK	1
#define _LOGCFG_DATATYPE_RUINT32_MASK	2
#define _LOGCFG_DATATYPE_BOOLEAN	3
#define _LOGCFG_DATATYPE_INT		4
#define _LOGCFG_DATATYPE_LONG		5
#define _LOGCFG_DATATYPE_INT16		6
#define _LOGCFG_DATATYPE_INT32		7
#define _LOGCFG_DATATYPE_UINT		8
#define _LOGCFG_DATATYPE_ULONG		9
#define _LOGCFG_DATATYPE_UINT16	       10
#define _LOGCFG_DATATYPE_UINT32	       11
#define _LOGCFG_DATATYPE_STRING	       12
#define _LOGCFG_DATATYPE_STRINGBUF     13
#define _LOGCFG_DATATYPE_DOUBLE	       14
#define _LOGCFG_DATATYPE_FILENAME      15
#define _LOGCFG_DATATYPE_CALLBACK      16


typedef struct 
  {
  ST_CHAR *tag;		 /* NULL if must use perferredTag field */
  ST_UINT  mask;
  ST_VOID *addr;
  ST_INT   dataType;
  ST_CHAR *description;	 /* NULL for no description */
  ST_CHAR *preferredTag; /* NULL if must use tag field */
  } LOGCFGX_VALUE_MAP;

typedef struct 
  {
  DBL_LNK 	 l;
  ST_CHAR 	*name;
  ST_INT         numMaskMap;
  LOGCFGX_VALUE_MAP *maskMapTbl;
  } LOGCFG_VALUE_GROUP;


ST_RET logcfgx_exx (LOG_CTRL *destLogCtrl, ST_CHAR *logFileName, ST_CHAR *fileNamePrefix, 
		    ST_BOOLEAN masksOnly, ST_BOOLEAN saveTagVals, ST_CHAR *compName);
ST_RET logcfgx_ex (LOG_CTRL *destLogCtrl, ST_CHAR *logFileName, ST_CHAR *fileNamePrefix, 
		   ST_BOOLEAN masksOnly, ST_BOOLEAN saveTagVals);
ST_VOID logcfgx_unconfigure (LOG_CTRL *lc);
ST_VOID logCfgAddMaskGroup (LOGCFG_VALUE_GROUP *logMaskGroup);
ST_VOID logCfgRemoveMaskGroup (LOGCFG_VALUE_GROUP *logMaskGroup);
ST_VOID logCfgRemoveAllMaskGroups (ST_VOID);
ST_VOID logCfgLogSettings (LOG_CTRL *lc, ST_BOOLEAN bLogMasks);
ST_RET _logcfg_get_logmasks (ST_CHAR *xmlBuf, ST_INT *xmlSizeIo);
ST_INT _logcfg_get_logmasks_size (ST_VOID);

/* ST_VOID (*logCfgCallbackFun) (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag, LOGCFGX_VALUE_MAP *valMap); */


extern LOGCFG_VALUE_GROUP acseLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP adlcLogMaskMapCtrl;	/* MMS-LITE                      */
extern LOGCFG_VALUE_GROUP asn1LogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP clnpLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE           */
extern LOGCFG_VALUE_GROUP genlistDebugMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP gsLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP icfgLogMaskMapCtrl;	/*           ICCP-LITE           */
extern LOGCFG_VALUE_GROUP logcfgMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP logCfgLogCtrlMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP memDebugMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP memLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP miLogMaskMapCtrl;	/*           ICCP-LITE           */
extern LOGCFG_VALUE_GROUP mmsLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP mvlLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE           */
extern LOGCFG_VALUE_GROUP secLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP ssleLogMaskMapCtrl;   /* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP ssleASN1LogMaskMapCtrl;
extern LOGCFG_VALUE_GROUP smpLogMaskMapCtrl;	/* MMS-LITE                      */
extern LOGCFG_VALUE_GROUP sockLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP suicLogMaskMapCtrl;	/*                      MMS-EASE */
extern LOGCFG_VALUE_GROUP sxLogMaskMapCtrl;	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP tp4LogMaskMapCtrl; 	/* MMS-LITE, ICCP-LITE, MMS-EASE */
extern LOGCFG_VALUE_GROUP usrLogMaskMapCtrl;	/* MMS-LITE                      */
/* For 142-XXX products (MMS_EASE over Lite Stack) */
extern LOGCFG_VALUE_GROUP ipcLogMaskMapCtrl;
extern LOGCFG_VALUE_GROUP sscLogMaskMapCtrl;


typedef struct
  {
  DBL_LNK l;
  ST_CHAR *tag;
  ST_CHAR *tagPath;
  ST_INT dataType;
  union
    {
    ST_BOOLEAN 	b;
    ST_INT 	sInt;
    ST_LONG 	sLong;
    ST_INT16 	sInt16;
    ST_INT32 	sInt32;
    ST_UINT 	uInt;
    ST_ULONG 	uLong;
    ST_UINT16 	uInt16;
    ST_UINT32 	uInt32;
    ST_DOUBLE   dbl;
    ST_CHAR    *str;
    }u;
  } LOGCFGX_TAG_VAL;

LOGCFGX_TAG_VAL *logCfgFindTagVal (ST_CHAR *tag);
ST_VOID logCfgClearTagVals (ST_VOID);


/************************************************************************/
/************************************************************************/
/* logcfg_debug_sel bit assignments					*/
#define LOGCFG_ERR  	0x0001   
#define LOGCFG_NERR 	0x0002   
#define LOGCFG_FLOW  	0x0004   
#define LOGCFG_CFG 	0x0008		/* to log the logging settings	*/

extern ST_UINT logcfg_debug_sel;

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST _logcfg_log_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _logcfg_log_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST _logcfg_log_flow_logstr;
extern SD_CONST ST_CHAR *SD_CONST _logcfg_log_cfg_logstr;
 
/************************************************************************/
/* Err log macros 			*/
#define LOGCFG_ERR0(a) \
    SLOG_0 (logcfg_debug_sel & LOGCFG_ERR,_logcfg_log_err_logstr,a)
#define LOGCFG_ERR1(a,b) \
    SLOG_1 (logcfg_debug_sel & LOGCFG_ERR,_logcfg_log_err_logstr,a,b)
#define LOGCFG_ERR2(a,b,c) \
    SLOG_2 (logcfg_debug_sel & LOGCFG_ERR,_logcfg_log_err_logstr,a,b,c)
#define LOGCFG_ERR3(a,b,c,d) \
    SLOG_3 (logcfg_debug_sel & LOGCFG_ERR,_logcfg_log_err_logstr,a,b,c,d)
#define LOGCFG_ERR4(a,b,c,d,e) \
    SLOG_4 (logcfg_debug_sel & LOGCFG_ERR,_logcfg_log_err_logstr,a,b,c,d,e)

/* Err continuation log macros		*/
#define LOGCFG_CERR0(a) \
    SLOGC_0 (logcfg_debug_sel & LOGCFG_ERR,a)
#define LOGCFG_CERR1(a,b) \
    SLOGC_1 (logcfg_debug_sel & LOGCFG_ERR,a,b)
#define LOGCFG_CERR2(a,b,c) \
    SLOGC_2 (logcfg_debug_sel & LOGCFG_ERR,a,b,c)
#define LOGCFG_CERR3(a,b,c,d) \
    SLOGC_3 (logcfg_debug_sel & LOGCFG_ERR,a,b,c,d)
#define LOGCFG_CERR4(a,b,c,d,e) \
    SLOGC_4 (logcfg_debug_sel & LOGCFG_ERR,a,b,c,d,e)

/************************************************************************/
/* NErr log macros 			*/
#define LOGCFG_NERR0(a) \
    SLOG_0 (logcfg_debug_sel & LOGCFG_NERR,_logcfg_log_nerr_logstr,a)
#define LOGCFG_NERR1(a,b) \
    SLOG_1 (logcfg_debug_sel & LOGCFG_NERR,_logcfg_log_nerr_logstr,a,b)
#define LOGCFG_NERR2(a,b,c) \
    SLOG_2 (logcfg_debug_sel & LOGCFG_NERR,_logcfg_log_nerr_logstr,a,b,c)
#define LOGCFG_NERR3(a,b,c,d) \
    SLOG_3 (logcfg_debug_sel & LOGCFG_NERR,_logcfg_log_nerr_logstr,a,b,c,d)
#define LOGCFG_NERR4(a,b,c,d,e) \
    SLOG_4 (logcfg_debug_sel & LOGCFG_NERR,_logcfg_log_nerr_logstr,a,b,c,d,e)

/* NErr continuation log macros		*/
#define LOGCFG_CNERR0(a) \
    SLOGC_0 (logcfg_debug_sel & LOGCFG_NERR,a)
#define LOGCFG_CNERR1(a,b) \
    SLOGC_1 (logcfg_debug_sel & LOGCFG_NERR,a,b)
#define LOGCFG_CNERR2(a,b,c) \
    SLOGC_2 (logcfg_debug_sel & LOGCFG_NERR,a,b,c)
#define LOGCFG_CNERR3(a,b,c,d) \
    SLOGC_3 (logcfg_debug_sel & LOGCFG_NERR,a,b,c,d)
#define LOGCFG_CNERR4(a,b,c,d,e) \
    SLOGC_4 (logcfg_debug_sel & LOGCFG_NERR,a,b,c,d,e)

/************************************************************************/
/* Flow log macros 			*/
#define LOGCFG_FLOW0(a) \
    SLOG_0 (logcfg_debug_sel & LOGCFG_FLOW,_logcfg_log_flow_logstr,a)
#define LOGCFG_FLOW1(a,b) \
    SLOG_1 (logcfg_debug_sel & LOGCFG_FLOW,_logcfg_log_flow_logstr,a,b)
#define LOGCFG_FLOW2(a,b,c) \
    SLOG_2 (logcfg_debug_sel & LOGCFG_FLOW,_logcfg_log_flow_logstr,a,b,c)
#define LOGCFG_FLOW3(a,b,c,d) \
    SLOG_3 (logcfg_debug_sel & LOGCFG_FLOW,_logcfg_log_flow_logstr,a,b,c,d)
#define LOGCFG_FLOW4(a,b,c,d,e) \
    SLOG_4 (logcfg_debug_sel & LOGCFG_FLOW,_logcfg_log_flow_logstr,a,b,c,d,e)

/* Flow continuation log macros		*/
#define LOGCFG_CFLOW0(a) \
    SLOGC_0 (logcfg_debug_sel & LOGCFG_FLOW,a)
#define LOGCFG_CFLOW1(a,b) \
    SLOGC_1 (logcfg_debug_sel & LOGCFG_FLOW,a,b)
#define LOGCFG_CFLOW2(a,b,c) \
    SLOGC_2 (logcfg_debug_sel & LOGCFG_FLOW,a,b,c)
#define LOGCFG_CFLOW3(a,b,c,d) \
    SLOGC_3 (logcfg_debug_sel & LOGCFG_FLOW,a,b,c,d)
#define LOGCFG_CFLOW4(a,b,c,d,e) \
    SLOGC_4 (logcfg_debug_sel & LOGCFG_FLOW,a,b,c,d,e)

/* Cfg log macros 			*/
#define LOGCFG_CFG0(a) \
    SLOG_0 (logcfg_debug_sel & LOGCFG_CFG,_logcfg_log_cfg_logstr,a)
#define LOGCFG_CFG1(a,b) \
    SLOG_1 (logcfg_debug_sel & LOGCFG_CFG,_logcfg_log_cfg_logstr,a,b)
#define LOGCFG_CFG2(a,b,c) \
    SLOG_2 (logcfg_debug_sel & LOGCFG_CFG,_logcfg_log_cfg_logstr,a,b,c)
#define LOGCFG_CFG3(a,b,c,d) \
    SLOG_3 (logcfg_debug_sel & LOGCFG_CFG,_logcfg_log_cfg_logstr,a,b,c,d)
#define LOGCFG_CFG4(a,b,c,d,e) \
    SLOG_4 (logcfg_debug_sel & LOGCFG_CFG,_logcfg_log_cfg_logstr,a,b,c,d,e)

/* Cfg continuation log macros		*/
#define LOGCFG_CCFG0(a) \
    SLOGC_0 (logcfg_debug_sel & LOGCFG_CFG,a)
#define LOGCFG_CCFG1(a,b) \
    SLOGC_1 (logcfg_debug_sel & LOGCFG_CFG,a,b)
#define LOGCFG_CCFG2(a,b,c) \
    SLOGC_2 (logcfg_debug_sel & LOGCFG_CFG,a,b,c)
#define LOGCFG_CCFG3(a,b,c,d) \
    SLOGC_3 (logcfg_debug_sel & LOGCFG_CFG,a,b,c,d)
#define LOGCFG_CCFG4(a,b,c,d,e) \
    SLOGC_4 (logcfg_debug_sel & LOGCFG_CFG,a,b,c,d,e)

/************************************************************************/
/************************************************************************/
/* Global variables for the SLOG library.				*/
/************************************************************************/

/* Log masks for different components of SISCO products.		*/
extern ST_UINT gs_debug_sel;
extern ST_UINT clnp_debug_sel;


extern LOG_CTRL *sLogCtrl;
extern LOG_CTRL *sErrLogCtrl;

extern ST_BOOLEAN sNonStandardLogMode;

/************************************************************************/
/* These elements are used for creating time strings			*/
/************************************************************************/

extern ST_CHAR slogTimeText[TIME_BUF_LEN];  
ST_VOID slogSetTimeText (LOG_CTRL *lc);

/************************************************************************/
/* slog header defines and buffer					*/
/************************************************************************/

#define SLOG_MAX_FNAME    	64
#define SLOG_MAX_LOGTYPESTR	40			/* logTypeStr cutoff if too long*/
#define SLOG_MAX_THREADID    	50	/* ThreadId=hex (decimal)	*/
#define SLOG_MAX_LINE_BREAK	2	/* max lineBreak (NL or CR NL)	*/
#define SLOG_MAX_HDR		(SLOG_MAX_LINE_BREAK +	\
				SLOG_MAX_LINE_BREAK +	\
				TIME_BUF_LEN+1 +	\
				SLOG_MAX_LOGTYPESTR+1 +	\
				SLOG_MAX_FNAME+14 +	\
				SLOG_MAX_THREADID +	\
				SLOG_MAX_LINE_BREAK +	\
				2 +			\
				1)			

/*		(SLOG_MAX_LINE_BREAK +	line break between slog messages*/
/*		SLOG_MAX_LINE_BREAK +	line break before slog hdr	*/
/*		TIME_BUF_LEN+1 +	slogTimeText buf size + 1 space	*/
/*		SLOG_MAX_LOGTYPESTR+1 +	limit for logTypeStr  + 1 space	*/
/*		SLOG_MAX_FNAME+14 +	fName & lineNum formatting	*/
/*		SLOG_MAX_THREADID +	ThreadId=hex (decimal)		*/
/*		SLOG_MAX_LINE_BREAK +	line break after slog hdr	*/
/*		2 +			2 spaces before slog msg	*/
/*		1)			NULL terminating string		*/


ST_VOID slogTrimFileName (ST_CHAR *dest, SD_CONST ST_CHAR *fullName);
ST_VOID slogSetHdr (LOG_CTRL *lc, SD_CONST ST_INT logType, 
                    SD_CONST ST_CHAR *SD_CONST logTypeStr, 
                    SD_CONST ST_CHAR *SD_CONST sourceFile, 
                    SD_CONST ST_INT lineNum, 
                    ST_CHAR *slogHdr, size_t slogHdrMaxSize,
                    ST_CHAR *lineBreak);

/************************************************************************/
/* dynamic loging function						*/
/************************************************************************/

extern ST_VOID (*slog_dyn_log_fun) (LOG_CTRL *lc, 
			SD_CONST ST_INT logType, 
                        SD_CONST ST_CHAR *SD_CONST sourceFile, 
			SD_CONST ST_INT lineNum,
                        SD_CONST ST_INT bufLen, 
			SD_CONST ST_CHAR *buf);
extern ST_VOID (*_slog_dyn_log_fun) (LOG_CTRL *lc, 
			SD_CONST ST_CHAR *timeStr, 
			SD_CONST ST_INT logType, 
			SD_CONST ST_CHAR *SD_CONST logTypeStr,
                        SD_CONST ST_CHAR *SD_CONST sourceFile, 
			SD_CONST ST_INT lineNum,
                        SD_CONST ST_INT bufLen, 
			SD_CONST ST_CHAR *buf);

/************************************************************************/
/* This function pointer is invoked during slow SLOG operations, such	*/
/* as 'clone' and 'find oldest message'					*/

extern ST_VOID (*slog_service_fun) (ST_VOID);

/************************************************************************/
/* Remote logging function						*/
/************************************************************************/

extern ST_VOID (*slog_remote_fun) (ST_UINT32 slog_remote_flags,
			LOG_CTRL *lc, 
			SD_CONST ST_INT logType, 
                        SD_CONST ST_CHAR *SD_CONST sourceFile, 
			SD_CONST ST_INT lineNum,
                        SD_CONST ST_INT bufLen, 
			SD_CONST ST_CHAR *buf);

extern ST_VOID (*_slog_remote_fun) (ST_UINT32 slog_remote_flags,
			LOG_CTRL *lc, 
			SD_CONST ST_INT logType, 
                        SD_CONST ST_CHAR *SD_CONST logTypeStr, 
                        SD_CONST ST_CHAR *SD_CONST sourceFile, 
			SD_CONST ST_INT lineNum,
                        SD_CONST ST_INT bufLen, 
			SD_CONST ST_CHAR *buf);

/* Parameter for use with remote logging function pointer		*/
/* SLOG will pass this global variable to the slog_remote_fun function	*/
extern ST_UINT32 slogRemoteFlags;

/************************************************************************/
/* Standard dynamic logging control definitions				*/
/************************************************************************/
/* These macros are OBSOLETE. They now do nothing. Previously they	*/
/* passed logType values of SLOG_DYN_PAUSE, SLOG_DYN_LF, SLOG_DYN_CLRSCR*/
/* respectively to slog.						*/

#define SLOG_PAUSE(lc,msg)
#define SLOG_LF(lc)
#define SLOG_CLRSCR(lc)

/************************************************************************/
/* Base macro types, useful for typical user logging, to reduce user	*/
/* macro verbosity							*/
/************************************************************************/

#define SLOGH1(lc,mask,id,x,y) {\
                           if (lc->logMask1 & mask)\
                             slogHex (sLogCtrl,id, thisFileName,__LINE__,x,y);}


#define SLOGH2(lc,mask,id,x,y) {\
                           if (lc->logMask2 & mask)\
                             slogHex (sLogCtrl,id, thisFileName,__LINE__,x,y);}

#define SLOGH3(lc,mask,id,x,y) {\
                           if (lc->logMask3 & mask)\
                             slogHex (sLogCtrl,id, thisFileName,__LINE__,x,y);}

#define SLOGH4(lc,mask,id,x,y) {\
                           if (lc->logMask4 & mask)\
                             slogHex (sLogCtrl,id, thisFileName,__LINE__,x,y);}

#define SLOGH5(lc,mask,id,x,y) {\
                           if (lc->logMask5 & mask)\
                             slogHex (sLogCtrl,id, thisFileName,__LINE__,x,y);}

#define SLOGH6(lc,mask,id,x,y) {\
                           if (lc->logMask6 & mask)\
                             slogHex (sLogCtrl,id, thisFileName,__LINE__,x,y);}



/* ******************* LOG MASK 1 MACROS *************************	*/

#define SLOG1_0(lc,mask,id,a) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a);\
                            }

#define SLOG1_1(lc,mask,id,a,b) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b);\
                            }

#define SLOG1_2(lc,mask,id,a,b,c) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c);\
                            }

#define SLOG1_3(lc,mask,id,a,b,c,d) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d);\
                            }

#define SLOG1_4(lc,mask,id,a,b,c,d,e) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e);\
                            }

#define SLOG1_5(lc,mask,id,a,b,c,d,e,f) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f);\
                            }

#define SLOG1_6(lc,mask,id,a,b,c,d,e,f,g) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g);\
                            }

#define SLOG1_7(lc,mask,id,a,b,c,d,e,f,g,h) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h);\
                            }

#define SLOG1_8(lc,mask,id,a,b,c,d,e,f,g,h,i) {\
                            if (lc->logMask1 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i);\
                            }



/* ******************* LOG MASK 2 MACROS *************************	*/
#define SLOG2_0(lc,mask,id,a) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a);\
                            }

#define SLOG2_1(lc,mask,id,a,b) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b);\
                            }

#define SLOG2_2(lc,mask,id,a,b,c) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c);\
                            }

#define SLOG2_3(lc,mask,id,a,b,c,d) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d);\
                            }

#define SLOG2_4(lc,mask,id,a,b,c,d,e) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e);\
                            }

#define SLOG2_5(lc,mask,id,a,b,c,d,e,f) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f);\
                            }

#define SLOG2_6(lc,mask,id,a,b,c,d,e,f,g) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g);\
                            }

#define SLOG2_7(lc,mask,id,a,b,c,d,e,f,g,h) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h);\
                            }

#define SLOG2_8(lc,mask,id,a,b,c,d,e,f,g,h,i) {\
                            if (lc->logMask2 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i);\
                            }



/* ******************* LOG MASK 3 MACROS *************************	*/
#define SLOG3_0(lc,mask,id,a) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a);\
                            }

#define SLOG3_1(lc,mask,id,a,b) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b);\
                            }

#define SLOG3_2(lc,mask,id,a,b,c) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c);\
                            }

#define SLOG3_3(lc,mask,id,a,b,c,d) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d);\
                            }

#define SLOG3_4(lc,mask,id,a,b,c,d,e) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e);\
                            }

#define SLOG3_5(lc,mask,id,a,b,c,d,e,f) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f);\
                            }

#define SLOG3_6(lc,mask,id,a,b,c,d,e,f,g) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g);\
                            }

#define SLOG3_7(lc,mask,id,a,b,c,d,e,f,g,h) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h);\
                            }

#define SLOG3_8(lc,mask,id,a,b,c,d,e,f,g,h,i) {\
                            if (lc->logMask3 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i);\
                            }



/* ******************* LOG MASK 4 MACROS ************************* 	*/
#define SLOG4_0(lc,mask,id,a) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a);\
                            }

#define SLOG4_1(lc,mask,id,a,b) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b);\
                            }

#define SLOG4_2(lc,mask,id,a,b,c) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c);\
                            }

#define SLOG4_3(lc,mask,id,a,b,c,d) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d);\
                            }

#define SLOG4_4(lc,mask,id,a,b,c,d,e) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e);\
                            }

#define SLOG4_5(lc,mask,id,a,b,c,d,e,f) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f);\
                            }

#define SLOG4_6(lc,mask,id,a,b,c,d,e,f,g) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g);\
                            }

#define SLOG4_7(lc,mask,id,a,b,c,d,e,f,g,h) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h);\
                            }

#define SLOG4_8(lc,mask,id,a,b,c,d,e,f,g,h,i) {\
                            if (lc->logMask4 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i);\
                            }




/* ******************* LOG MASK 5 MACROS *************************	*/
#define SLOG5_0(lc,mask,id,a) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a);\
                            }

#define SLOG5_1(lc,mask,id,a,b) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b);\
                            }

#define SLOG5_2(lc,mask,id,a,b,c) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c);\
                            }

#define SLOG5_3(lc,mask,id,a,b,c,d) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d);\
                            }

#define SLOG5_4(lc,mask,id,a,b,c,d,e) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e);\
                            }

#define SLOG5_5(lc,mask,id,a,b,c,d,e,f) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f);\
                            }

#define SLOG5_6(lc,mask,id,a,b,c,d,e,f,g) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g);\
                            }

#define SLOG5_7(lc,mask,id,a,b,c,d,e,f,g,h) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h);\
                            }

#define SLOG5_8(lc,mask,id,a,b,c,d,e,f,g,h,i) {\
                            if (lc->logMask5 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i);\
                            }



/* ******************* LOG MASK 6 MACROS ************************* 	*/
#define SLOG6_0(lc,mask,id,a) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a);\
                            }

#define SLOG6_1(lc,mask,id,a,b) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b);\
                            }

#define SLOG6_2(lc,mask,id,a,b,c) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c);\
                            }

#define SLOG6_3(lc,mask,id,a,b,c,d) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d);\
                            }

#define SLOG6_4(lc,mask,id,a,b,c,d,e) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e);\
                            }

#define SLOG6_5(lc,mask,id,a,b,c,d,e,f) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f);\
                            }

#define SLOG6_6(lc,mask,id,a,b,c,d,e,f,g) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g);\
                            }

#define SLOG6_7(lc,mask,id,a,b,c,d,e,f,g,h) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h);\
                            }

#define SLOG6_8(lc,mask,id,a,b,c,d,e,f,g,h,i) {\
                            if (lc->logMask6 & mask)\
                               slog (sLogCtrl,id, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i);\
                            }



/************************************************************************/
/************************************************************************/
#if defined(DEBUG_SISCO)
/************************************************************************/

extern ST_CHAR *_slogAlwaysLogTypeStr;
extern ST_CHAR *_slogXMLLogTypeStr;

#define SLOGALWAYS0(a) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a)
#define SLOGALWAYS1(a,b) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b)
#define SLOGALWAYS2(a,b,c) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c)
#define SLOGALWAYS3(a,b,c,d) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d)
#define SLOGALWAYS4(a,b,c,d,e) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d,e)
#define SLOGALWAYS5(a,b,c,d,e,f) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d,e,f)
#define SLOGALWAYS6(a,b,c,d,e,f,g) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d,e,f,g)
#define SLOGALWAYS7(a,b,c,d,e,f,g,h) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d,e,f,g,h)
#define SLOGALWAYS8(a,b,c,d,e,f,g,h,i) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i)
#define SLOGALWAYS9(a,b,c,d,e,f,g,h,i,j) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i,j)
#define SLOGALWAYS10(a,b,c,d,e,f,g,h,i,j,k) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i,j,k)
#define SLOGALWAYS11(a,b,c,d,e,f,g,h,i,j,k,l) \
    _slog (sLogCtrl,_slogAlwaysLogTypeStr, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i,j,k,l)

#define SLOGCALWAYS0(a) \
    _slogc (sLogCtrl,a)
#define SLOGCALWAYS1(a,b) \
    _slogc (sLogCtrl,a,b)
#define SLOGCALWAYS2(a,b,c) \
    _slogc (sLogCtrl,a,b,c)
#define SLOGCALWAYS3(a,b,c,d) \
    _slogc (sLogCtrl,a,b,c,d)
#define SLOGCALWAYS4(a,b,c,d,e) \
    _slogc (sLogCtrl,a,b,c,d,e)
#define SLOGCALWAYS5(a,b,c,d,e,f) \
    _slogc (sLogCtrl,a,b,c,d,e,f)
#define SLOGCALWAYS6(a,b,c,d,e,f,g) \
    _slogc (sLogCtrl,a,b,c,d,e,f,g)
#define SLOGCALWAYS7(a,b,c,d,e,f,g,h) \
    _slogc (sLogCtrl,a,b,c,d,e,f,g,h)
#define SLOGCALWAYS8(a,b,c,d,e,f,g,h,i) \
    _slogc (sLogCtrl,a,b,c,d,e,f,g,h,i)

#define SLOGALWAYSH(numBytes,dataPtr) \
    _slogHex (sLogCtrl, numBytes, dataPtr)

#define SLOGALWAYSS(dataPtr) \
    _slogStr (sLogCtrl, dataPtr)

#define SLOGALWAYSX(numBytes, dataPtr) \
    _slogXML (sLogCtrl, _slogXMLLogTypeStr, thisFileName, __LINE__, numBytes, dataPtr)

/************************************************************************/
#else	/* #if defined(DEBUG_SISCO) */
/************************************************************************/

#define SLOGALWAYS0(a)
#define SLOGALWAYS1(a,b)
#define SLOGALWAYS2(a,b,c)
#define SLOGALWAYS3(a,b,c,d)
#define SLOGALWAYS4(a,b,c,d,e)
#define SLOGALWAYS5(a,b,c,d,e,f)
#define SLOGALWAYS6(a,b,c,d,e,f,g)
#define SLOGALWAYS7(a,b,c,d,e,f,g,h)
#define SLOGALWAYS8(a,b,c,d,e,f,g,h,i)
#define SLOGALWAYS9(a,b,c,d,e,f,g,h,i,j)
#define SLOGALWAYS10(a,b,c,d,e,f,g,h,i,j,k)
#define SLOGALWAYS11(a,b,c,d,e,f,g,h,i,j,k,l)
#define SLOGCALWAYS0(a)
#define SLOGCALWAYS1(a,b)
#define SLOGCALWAYS2(a,b,c)
#define SLOGCALWAYS3(a,b,c,d)
#define SLOGCALWAYS4(a,b,c,d,e)
#define SLOGCALWAYS5(a,b,c,d,e,f)
#define SLOGCALWAYS6(a,b,c,d,e,f,g)
#define SLOGCALWAYS7(a,b,c,d,e,f,g,h)
#define SLOGCALWAYS8(a,b,c,d,e,f,g,h,i)
#define SLOGALWAYSH(numBytes,dataPtr)
#define SLOGALWAYSS(dataPtr)
#define SLOGALWAYSX(numBytes,dataPtr)

#endif

/************************************************************************/
/************************************************************************/
/************************************************************************/
/* These macros are used in the standard SLOG helper macros and allow SLOG messages to also be logged to the SEV trace */

#ifdef SEV_SUPPORT
#define SEV_TRACE_SLOG0(ena,str,fmt)                  if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt)
#define SEV_TRACE_SLOG1(ena,str,fmt,a)                if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt,a)
#define SEV_TRACE_SLOG2(ena,str,fmt,a,b)              if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt,a,b)
#define SEV_TRACE_SLOG3(ena,str,fmt,a,b,c)            if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c)
#define SEV_TRACE_SLOG4(ena,str,fmt,a,b,c,d)          if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d)
#define SEV_TRACE_SLOG5(ena,str,fmt,a,b,c,d,e)        if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e)
#define SEV_TRACE_SLOG6(ena,str,fmt,a,b,c,d,e,f)      if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f)
#define SEV_TRACE_SLOG7(ena,str,fmt,a,b,c,d,e,f,g)    if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f,g)
#define SEV_TRACE_SLOG8(ena,str,fmt,a,b,c,d,e,f,g,h)  if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f,g,h)

#define SEV_TRACE_SLOGC0(ena,fmt)                     if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt)
#define SEV_TRACE_SLOGC1(ena,fmt,a)                   if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt,a)
#define SEV_TRACE_SLOGC2(ena,fmt,a,b)                 if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt,a,b)
#define SEV_TRACE_SLOGC3(ena,fmt,a,b,c)               if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt,a,b,c)
#define SEV_TRACE_SLOGC4(ena,fmt,a,b,c,d)             if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt,a,b,c,d)
#define SEV_TRACE_SLOGC5(ena,fmt,a,b,c,d,e)           if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt,a,b,c,d,e)
#define SEV_TRACE_SLOGC6(ena,fmt,a,b,c,d,e,f)         if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt,a,b,c,d,e,f)
#define SEV_TRACE_SLOGC7(ena,fmt,a,b,c,d,e,f,g)       if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt,a,b,c,d,e,f,g)
#define SEV_TRACE_SLOGC8(ena,fmt,a,b,c,d,e,f,g,h)     if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sLogCtrl,"","",__LINE__,fmt,a,b,c,d,e,f,g,h)

#define SEV_TRACE_SLOGE0(ena,str,fmt)                  if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt)
#define SEV_TRACE_SLOGE1(ena,str,fmt,a)                if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt,a)
#define SEV_TRACE_SLOGE2(ena,str,fmt,a,b)              if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b)
#define SEV_TRACE_SLOGE3(ena,str,fmt,a,b,c)            if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c)
#define SEV_TRACE_SLOGE4(ena,str,fmt,a,b,c,d)          if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d)
#define SEV_TRACE_SLOGE5(ena,str,fmt,a,b,c,d,e)        if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e)
#define SEV_TRACE_SLOGE6(ena,str,fmt,a,b,c,d,e,f)      if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f)
#define SEV_TRACE_SLOGE7(ena,str,fmt,a,b,c,d,e,f,g)    if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f,g)
#define SEV_TRACE_SLOGE8(ena,str,fmt,a,b,c,d,e,f,g,h)  if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f,g,h)

#define SEV_TRACE_SLOGEC0(ena,fmt)                     if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt)
#define SEV_TRACE_SLOGEC1(ena,fmt,a)                   if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt,a)
#define SEV_TRACE_SLOGEC2(ena,fmt,a,b)                 if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt,a,b)
#define SEV_TRACE_SLOGEC3(ena,fmt,a,b,c)               if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt,a,b,c)
#define SEV_TRACE_SLOGEC4(ena,fmt,a,b,c,d)             if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt,a,b,c,d)
#define SEV_TRACE_SLOGEC5(ena,fmt,a,b,c,d,e)           if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt,a,b,c,d,e)
#define SEV_TRACE_SLOGEC6(ena,fmt,a,b,c,d,e,f)         if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt,a,b,c,d,e,f)
#define SEV_TRACE_SLOGEC7(ena,fmt,a,b,c,d,e,f,g)       if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt,a,b,c,d,e,f,g)
#define SEV_TRACE_SLOGEC8(ena,fmt,a,b,c,d,e,f,g,h)     if(_sevCfg.trace.sevOptions.slogTraceMode)sevTraceSlog(ena,sErrLogCtrl,"","",__LINE__,fmt,a,b,c,d,e,f,g,h)

#else
#define SEV_TRACE_SLOG0(ena,str,fmt)
#define SEV_TRACE_SLOG1(ena,str,fmt,a)
#define SEV_TRACE_SLOG2(ena,str,fmt,a,b)
#define SEV_TRACE_SLOG3(ena,str,fmt,a,b,c)
#define SEV_TRACE_SLOG4(ena,str,fmt,a,b,c,d)
#define SEV_TRACE_SLOG5(ena,str,fmt,a,b,c,d,e)
#define SEV_TRACE_SLOG6(ena,str,fmt,a,b,c,d,e,f)
#define SEV_TRACE_SLOG7(ena,str,fmt,a,b,c,d,e,f,g)
#define SEV_TRACE_SLOG8(ena,str,fmt,a,b,c,d,e,f,g,h)

#define SEV_TRACE_SLOGC0(ena,fmt)
#define SEV_TRACE_SLOGC1(ena,fmt,a)
#define SEV_TRACE_SLOGC2(ena,fmt,a,b)
#define SEV_TRACE_SLOGC3(ena,fmt,a,b,c)
#define SEV_TRACE_SLOGC4(ena,fmt,a,b,c,d)
#define SEV_TRACE_SLOGC5(ena,fmt,a,b,c,d,e)
#define SEV_TRACE_SLOGC6(ena,fmt,a,b,c,d,e,f)
#define SEV_TRACE_SLOGC7(ena,fmt,a,b,c,d,e,f,g)
#define SEV_TRACE_SLOGC8(ena,fmt,a,b,c,d,e,f,g,h)

#define SEV_TRACE_SLOGE0(ena,str,fmt)
#define SEV_TRACE_SLOGE1(ena,str,fmt,a)
#define SEV_TRACE_SLOGE2(ena,str,fmt,a,b)
#define SEV_TRACE_SLOGE3(ena,str,fmt,a,b,c)
#define SEV_TRACE_SLOGE4(ena,str,fmt,a,b,c,d)
#define SEV_TRACE_SLOGE5(ena,str,fmt,a,b,c,d,e)
#define SEV_TRACE_SLOGE6(ena,str,fmt,a,b,c,d,e,f)
#define SEV_TRACE_SLOGE7(ena,str,fmt,a,b,c,d,e,f,g)
#define SEV_TRACE_SLOGE8(ena,str,fmt,a,b,c,d,e,f,g,h)

#define SEV_TRACE_SLOGEC0(ena,fmt)
#define SEV_TRACE_SLOGEC1(ena,fmt,a)
#define SEV_TRACE_SLOGEC2(ena,fmt,a,b)
#define SEV_TRACE_SLOGEC3(ena,fmt,a,b,c)
#define SEV_TRACE_SLOGEC4(ena,fmt,a,b,c,d)
#define SEV_TRACE_SLOGEC5(ena,fmt,a,b,c,d,e)
#define SEV_TRACE_SLOGEC6(ena,fmt,a,b,c,d,e,f)
#define SEV_TRACE_SLOGEC7(ena,fmt,a,b,c,d,e,f,g)
#define SEV_TRACE_SLOGEC8(ena,fmt,a,b,c,d,e,f,g,h)
#endif

/************************************************************************/
/************************************************************************/
/* "Base" set of macros.						*/
/* All other LOG macros should call these macros.			*/
/* TRICK: These macros all use "do{...}while(0)".			*/
/*        This works in any context without adding extra {}.		*/
/************************************************************************/



#ifdef DEBUG_SISCO
/* Basic log macros (all call _slog)	*/

#define SLOG_0(ena,str,fmt) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt); 			SEV_TRACE_SLOG0(ena,str,fmt);\
	} while(0)

#define SLOG_1(ena,str,fmt,a) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt,a); 			SEV_TRACE_SLOG1(ena,str,fmt,a);\
	} while(0)

#define SLOG_2(ena,str,fmt,a,b) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt,a,b);		SEV_TRACE_SLOG2(ena,str,fmt,a,b);\
	} while(0)

#define SLOG_3(ena,str,fmt,a,b,c) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c);		SEV_TRACE_SLOG3(ena,str,fmt,a,b,c);\
	} while(0)

#define SLOG_4(ena,str,fmt,a,b,c,d) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d);		SEV_TRACE_SLOG4(ena,str,fmt,a,b,c,d);\
	} while(0)

#define SLOG_5(ena,str,fmt,a,b,c,d,e) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e);		SEV_TRACE_SLOG5(ena,str,fmt,a,b,c,d,e);\
	} while(0)

#define SLOG_6(ena,str,fmt,a,b,c,d,e,f) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f);	SEV_TRACE_SLOG6(ena,str,fmt,a,b,c,d,e,f);\
	} while(0)

#define SLOG_7(ena,str,fmt,a,b,c,d,e,f,g) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f,g);	SEV_TRACE_SLOG7(ena,str,fmt,a,b,c,d,e,f,g);\
	} while(0)

#define SLOG_8(ena,str,fmt,a,b,c,d,e,f,g,h) \
	do {if (ena) _slog (sLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f,g,h);   	SEV_TRACE_SLOG8(ena,str,fmt,a,b,c,d,e,f,g,h);\
	} while(0)

/* Continuation macros (all call _slogc).	*/
#define SLOGC_0(ena,fmt) \
	do {if (ena) _slogc (sLogCtrl,fmt); 						SEV_TRACE_SLOGC0(ena,fmt);\
	} while(0)

#define SLOGC_1(ena,fmt,a) \
	do {if (ena) _slogc (sLogCtrl,fmt,a); 						SEV_TRACE_SLOGC1(ena,fmt,a);\
	} while(0)

#define SLOGC_2(ena,fmt,a,b) \
	do {if (ena) _slogc (sLogCtrl,fmt,a,b); 	      				SEV_TRACE_SLOGC2(ena,fmt,a,b);\
	} while(0)

#define SLOGC_3(ena,fmt,a,b,c) \
	do {if (ena) _slogc (sLogCtrl,fmt,a,b,c);					SEV_TRACE_SLOGC3(ena,fmt,a,b,c);\
	} while(0)

#define SLOGC_4(ena,fmt,a,b,c,d) \
	do {if (ena) _slogc (sLogCtrl,fmt,a,b,c,d);					SEV_TRACE_SLOGC4(ena,fmt,a,b,c,d);\
	} while(0)

#define SLOGC_5(ena,fmt,a,b,c,d,e) \
	do {if (ena) _slogc (sLogCtrl,fmt,a,b,c,d,e);					SEV_TRACE_SLOGC5(ena,fmt,a,b,c,d,e);\
	} while(0)

#define SLOGC_6(ena,fmt,a,b,c,d,e,f) \
	do {if (ena) _slogc (sLogCtrl,fmt,a,b,c,d,e,f);					SEV_TRACE_SLOGC6(ena,fmt,a,b,c,d,e,f);\
	} while(0)

#define SLOGC_7(ena,fmt,a,b,c,d,e,f,g) \
	do {if (ena) _slogc (sLogCtrl,fmt,a,b,c,d,e,f,g);				SEV_TRACE_SLOGC7(ena,fmt,a,b,c,d,e,f,g);\
	} while(0)

#define SLOGC_8(ena,fmt,a,b,c,d,e,f,g,h) \
	do {if (ena) _slogc (sLogCtrl,fmt,a,b,c,d,e,f,g,h);				SEV_TRACE_SLOGC8(ena,fmt,a,b,c,d,e,f,g,h);\
	} while(0)

/* Hex logging macro (calls _slogHex)	*/
#define SLOGH(ena,num,ptr) \
	do {if (ena) _slogHex (sLogCtrl,num,ptr);} while(0)

/* XML logging macro (calls _slogXML)	*/
#define SLOGX(ena,num,ptr) \
    do {if (ena) _slogXML (sLogCtrl, _slogXMLLogTypeStr, thisFileName, __LINE__, num, ptr);} while(0)

#else	/* !DEBUG_SISCO	*/

/* Non-DEBUG macros for EVERYONE! Yeah!! Never have to create another one.*/
/* Basic log macros (do nothing)	*/
#define SLOG_0(ena,str,fmt)
#define SLOG_1(ena,str,fmt,a)
#define SLOG_2(ena,str,fmt,a,b)
#define SLOG_3(ena,str,fmt,a,b,c)
#define SLOG_4(ena,str,fmt,a,b,c,d)
#define SLOG_5(ena,str,fmt,a,b,c,d,e)
#define SLOG_6(ena,str,fmt,a,b,c,d,e,f)
#define SLOG_7(ena,str,fmt,a,b,c,d,e,f,g)
#define SLOG_8(ena,str,fmt,a,b,c,d,e,f,g,h)

/* Continuation macros (do nothing).	*/
#define SLOGC_0(ena,fmt)
#define SLOGC_1(ena,fmt,a)
#define SLOGC_2(ena,fmt,a,b)
#define SLOGC_3(ena,fmt,a,b,c)
#define SLOGC_4(ena,fmt,a,b,c,d)
#define SLOGC_5(ena,fmt,a,b,c,d,e)
#define SLOGC_6(ena,fmt,a,b,c,d,e,f)
#define SLOGC_7(ena,fmt,a,b,c,d,e,f,g)
#define SLOGC_8(ena,fmt,a,b,c,d,e,f,g,h)

/* Hex logging macro (do nothing)	*/
#define SLOGH(ena,num,ptr)
/* XML logging macro (do nothing)	*/
#define SLOGX(ena,num,ptr)
#endif	/* !DEBUG_SISCO	*/


/************************************************************************/
/* "Base" set of macros for error log					*/
/************************************************************************/

#ifdef DEBUG_SISCO
/* Basic log macros (all call _slog)	*/
#define SELOG_0(ena,str,fmt) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt);			SEV_TRACE_SLOGE0(ena,str,fmt);\
	} while(0)

#define SELOG_1(ena,str,fmt,a) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt,a);		SEV_TRACE_SLOGE1(ena,str,fmt,a);\
	} while(0)

#define SELOG_2(ena,str,fmt,a,b) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b);		SEV_TRACE_SLOGE2(ena,str,fmt,a,b);\
	} while(0)

#define SELOG_3(ena,str,fmt,a,b,c) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c);		SEV_TRACE_SLOGE3(ena,str,fmt,a,b,c);\
	} while(0)

#define SELOG_4(ena,str,fmt,a,b,c,d) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d);		SEV_TRACE_SLOGE4(ena,str,fmt,a,b,c,d);\
	} while(0)

#define SELOG_5(ena,str,fmt,a,b,c,d,e) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e);	SEV_TRACE_SLOGE5(ena,str,fmt,a,b,c,d,e);\
	} while(0)

#define SELOG_6(ena,str,fmt,a,b,c,d,e,f) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f);	SEV_TRACE_SLOGE6(ena,str,fmt,a,b,c,d,e,f);\
	} while(0)

#define SELOG_7(ena,str,fmt,a,b,c,d,e,f,g) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f,g);	SEV_TRACE_SLOGE7(ena,str,fmt,a,b,c,d,e,f,g);\
	} while(0)

#define SELOG_8(ena,str,fmt,a,b,c,d,e,f,g,h) \
	do {if (ena) _slog (sErrLogCtrl,str,thisFileName,__LINE__,fmt,a,b,c,d,e,f,g,h);	SEV_TRACE_SLOGE8(ena,str,fmt,a,b,c,d,e,f,g,h);\
	} while(0)

/* Continuation macros (all call _slogc).	*/
#define SELOGC_0(ena,fmt) \
	do {if (ena) _slogc (sErrLogCtrl,fmt);						SEV_TRACE_SLOGEC0(ena,fmt);\
	} while(0)

#define SELOGC_1(ena,fmt,a) \
	do {if (ena) _slogc (sErrLogCtrl,fmt,a);					SEV_TRACE_SLOGEC1(ena,fmt,a);\
	} while(0)

#define SELOGC_2(ena,fmt,a,b) \
	do {if (ena) _slogc (sErrLogCtrl,fmt,a,b);					SEV_TRACE_SLOGEC2(ena,fmt,a,b);\
	} while(0)

#define SELOGC_3(ena,fmt,a,b,c) \
	do {if (ena) _slogc (sErrLogCtrl,fmt,a,b,c);					SEV_TRACE_SLOGEC3(ena,fmt,a,b,c);\
	} while(0)

#define SELOGC_4(ena,fmt,a,b,c,d) \
	do {if (ena) _slogc (sErrLogCtrl,fmt,a,b,c,d);					SEV_TRACE_SLOGEC4(ena,fmt,a,b,c,d);\
	} while(0)

#define SELOGC_5(ena,fmt,a,b,c,d,e) \
	do {if (ena) _slogc (sErrLogCtrl,fmt,a,b,c,d,e);				SEV_TRACE_SLOGEC5(ena,fmt,a,b,c,d,e);\
	} while(0)

#define SELOGC_6(ena,fmt,a,b,c,d,e,f) \
	do {if (ena) _slogc (sErrLogCtrl,fmt,a,b,c,d,e,f);				SEV_TRACE_SLOGEC6(ena,fmt,a,b,c,d,e,f);\
	} while(0)

#define SELOGC_7(ena,fmt,a,b,c,d,e,f,g) \
	do {if (ena) _slogc (sErrLogCtrl,fmt,a,b,c,d,e,f,g);				SEV_TRACE_SLOGEC7(ena,fmt,a,b,c,d,e,f,g);\
	} while(0)

#define SELOGC_8(ena,fmt,a,b,c,d,e,f,g,h) \
	do {if (ena) _slogc (sErrLogCtrl,fmt,a,b,c,d,e,f,g,h);				SEV_TRACE_SLOGEC8(ena,fmt,a,b,c,d,e,f,g,h);\
	} while(0)


/* Hex logging macro (calls _slogHex)	*/
#define SELOGH(ena,num,ptr) \
	do {if (ena) _slogHex (sErrLogCtrl,num,ptr);} while(0)

/* XML logging macro (calls _slogXML)	*/
#define SELOGX(ena,num,ptr) \
    do {if (ena) _slogXML (sErrLogCtrl, _slogXMLLogTypeStr, thisFileName, __LINE__, num, ptr);} while(0)

#else	/* !DEBUG_SISCO	*/

/* Non-DEBUG macros for EVERYONE! Yeah!! Never have to create another one.*/
/* Basic log macros (do nothing)	*/
#define SELOG_0(ena,str,fmt)
#define SELOG_1(ena,str,fmt,a)
#define SELOG_2(ena,str,fmt,a,b)
#define SELOG_3(ena,str,fmt,a,b,c)
#define SELOG_4(ena,str,fmt,a,b,c,d)
#define SELOG_5(ena,str,fmt,a,b,c,d,e)
#define SELOG_6(ena,str,fmt,a,b,c,d,e,f)
#define SELOG_7(ena,str,fmt,a,b,c,d,e,f,g)
#define SELOG_8(ena,str,fmt,a,b,c,d,e,f,g,h)

/* Continuation macros (do nothing).	*/
#define SELOGC_0(ena,fmt)
#define SELOGC_1(ena,fmt,a)
#define SELOGC_2(ena,fmt,a,b)
#define SELOGC_3(ena,fmt,a,b,c)
#define SELOGC_4(ena,fmt,a,b,c,d)
#define SELOGC_5(ena,fmt,a,b,c,d,e)
#define SELOGC_6(ena,fmt,a,b,c,d,e,f)
#define SELOGC_7(ena,fmt,a,b,c,d,e,f,g)
#define SELOGC_8(ena,fmt,a,b,c,d,e,f,g,h)

/* Hex logging macro (do nothing)	*/
#define SELOGH(ena,num,ptr)
/* XML logging macro (do nothing)	*/
#define SELOGX(ena,num,ptr)
#endif	/* !DEBUG_SISCO	*/


#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/

