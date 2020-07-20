/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2005, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_var.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* 	This module contains global variables used by the ADLC		*/
/* libraries.								*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 10/04/07  MDE    25     Tweaked LOGCFG_VALUE_GROUP/LOGCFGX_VALUE_MAP	*/
/* 05/23/05  EJV    24     Add adlcLogMaskMapCtrl for parsing logcfg.xml*/
/* 09/05/03  EJV    23     Chg adlc_debug_sel to ST_UINT from ST_ULONG.	*/
/* 09/23/02  KCR    22     Removed extra include files                  */
/* 09/23/02  KCR    21     Moved thisFileName before includes           */
/* 09/08/99  KCR    20     Manually initialize mem_chk and glbsem      	*/
/* 02/03/99  JRB    19     ALWAYS enable ADLC error logging at startup.	*/
/* 10/08/98  MDE    18     Migrated to updated SLOG interface		*/
/* 03/05/98  IKE    17     Added Critical Sections to protect Adlc Data	*/
/* 09/18/97  IKE    16     Added telephone support			*/
/* 07/16/97  IKE    15     Added device statistics and device filt log	*/
/* 05/12/97  IKE    14     Changed to MMSEASE 7.0 Data Types		*/
/* 02/24/97  IKE    13     Changed logMask6 to use adlc_debug_sel	*/
/*			   CFG_PORT also for DOS so multiple ports can	*/
/*			   be initialized in dos platform		*/
/* 02/03/97  IKE    12     adlcCfg per port basis			*/
/* 01/17/97  IKE    11     Added initSiscoUtilGlbVarsForAdlc 		*/
/* 12/03/96  IKE    10     WIN32 Support, header files cleanup		*/ 
/* 11/15/96  IKE    09     Changed the way the glb data was initialized	*/
/*			   Now it gets initialized by function call not	*/
/*			   at declaration time. did for embedded systems*/
/* 11/06/96  IKE    08     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/04/96  IKE    07     Changed pstPollGroups to pollGroupList	*/
/*			   Added connectedTask				*/
/* 09/11/96  IKE    06     Removed slogCtrl, changed serMgrSleep from 32*/ 
/* 06/18/96  IKE    05     Added a global var stackTaskId for DOS 	*/
/*			   platform to differentiate which stack it is	*/
/*			   linked to					*/
/* 05/22/96  IKE    04     Added eventList and got rid of global vars	*/
/*			   to pass the events to DOS stack		*/
/*			   Added taskList. Added ADLC_ERROR		*/
/* 04/05/96  rkr    03     Header file and SAVE_CALLING cleanup 	*/
/* 04/04/96  rkr    02	   Cleanup					*/
/* 01/10/96  IKE   1.00    Initial release                             	*/
/************************************************************************/
/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

/* Some other SISCO files						*/
#include "cfg_util.h"
#include "mem_chk.h"
#include "glbsem.h"

/* ADLC files								*/
#include "adlc.h"

ADLC_CFG_INFO   adlcCfg;
ADLC_PORT_INFO *portList; /* The complete linked list of ports	*/
CFG_PORT *portConfigList; 	/* global port cfg data */
ADLC_STATS adlcStats; 	  /* The statistics data structure	*/
ST_BOOLEAN terminateFlag;
ST_UINT adlc_debug_sel;

#ifdef DEBUG_SISCO
SD_CONST ST_CHAR *_adlc_err_logstr = "ADLC_MASK_LOG_ERR";
SD_CONST ST_CHAR *_adlc_nerr_logstr = "ADLC_MASK_LOG_NERR";
SD_CONST ST_CHAR *_adlc_flow_logstr = "ADLC_MASK_LOG_FLOW";
SD_CONST ST_CHAR *_adlc_io_logstr = "ADLC_MASK_LOG_IO";
SD_CONST ST_CHAR *_adlc_debug_logstr = "ADLC_MASK_LOG_DEBUG";
SD_CONST ST_CHAR *_adlc_always_logstr = "ADLC_MASK_LOG_ALWAYS";
SD_CONST ST_CHAR *_adlc_sm_logstr = "ADLC_MASK_LOG_SM";
SD_CONST ST_CHAR *_adlc_info_frame_logstr = "ADLC_MASK_LOG_INFO_FRAME";
SD_CONST ST_CHAR *_adlc_user_logstr = "ADLC_MASK_LOG_USER";

#if (defined (ADLC_SLAVE) || defined (ADLC_MASTER))

LOGCFGX_VALUE_MAP adlcLogMaskMaps[] =
  {
    {"ADLC_MASK_LOG_ERR",	    ADLC_MASK_LOG_ERR,	         &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"ADLC_MASK_LOG_NERR",	    ADLC_MASK_LOG_NERR,	         &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"ADLC_MASK_LOG_FLOW",	    ADLC_MASK_LOG_FLOW,	         &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Flow"},
    {"ADLC_MASK_LOG_IO",	    ADLC_MASK_LOG_IO,	         &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "IO"},
    {"ADLC_MASK_LOG_HEXIO",	    ADLC_MASK_LOG_HEXIO,   	 &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "HexIO"},
    {"ADLC_MASK_LOG_DEBUG",	    ADLC_MASK_LOG_DEBUG,	 &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Debug"},
    {"ADLC_MASK_LOG_HEXDEBUG",	    ADLC_MASK_LOG_HEXDEBUG,	 &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "HexDebug"},
    {"ADLC_MASK_LOG_SM",	    ADLC_MASK_LOG_SM,	         &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "SM"},
    {"ADLC_MASK_LOG_HEXSM",	    ADLC_MASK_LOG_HEXSM,	 &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "HexSM"},
    {"ADLC_MASK_LOG_INFO_FRAME",    ADLC_MASK_LOG_INFO_FRAME,	 &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "InfoFrame"},
    {"ADLC_MASK_LOG_HEXINFO_FRAME", ADLC_MASK_LOG_HEXINFO_FRAME, &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "HexInfoFrame"},
    {"ADLC_MASK_LOG_USER",	    ADLC_MASK_LOG_USER,	         &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "User"},
    {"ADLC_MASK_LOG_HEXUSER",	    ADLC_MASK_LOG_HEXUSER,	 &adlc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "HexUser"}
  };

LOGCFG_VALUE_GROUP adlcLogMaskMapCtrl =
  {
  {NULL,NULL},
  "AdlcLogMasks",	 /* Parent Tag */
  sizeof(adlcLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  adlcLogMaskMaps
  };
#endif
#endif /* DEBUG_SISCO */

#if defined(ADLC_MASTER)
POLL_GROUP *pollGroupList;
ST_ULONG gRrFrames;
ST_ULONG gInfoFrames;
#endif

#if !defined(ADLC_LM)
ADLC_EVENT_INFO *eventList;
ST_INT stackTaskId;
#endif

#if defined(ADLC_LM)
ADLC_TASK_INFO *taskList;
ST_INT connectedTask[RLI_TASK_ID_LAST + 1];
ST_INT serMgrSleep;
PID myPID;
ADLC_FILTER_DEV_LOG_INFO stFiltDevLog;
CRITICAL_SECTION protectAdlcData; /* to guard adlc data between threads */
#endif

const ST_UINT16 adlc_crc_table[256] =  {
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, /* 00-07 */
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7, /* 08-0f */
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, /* 10-17 */
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876, /* 18-1f */
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, /* 20-27 */
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, /* 28-2f */
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, /* 30-37 */
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974, /* 38-3f */
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb, /* 40-47 */
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, /* 48-4f */
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, /* 50-57 */
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, /* 58-5f */
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, /* 60-67 */
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1, /* 68-6f */
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, /* 70-77 */
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, /* 78-7f */
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, /* 80-87 */
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff, /* 88-8f */
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, /* 90-97 */
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, /* 98-9f */
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, /* a0-a7 */
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, /* a8-af */
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134, /* b0-b7 */
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, /* b8-bf */
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, /* c0-c7 */
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, /* c8-cf */
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, /* d0-d7 */
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, /* d8-df */
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, /* e0-e7 */
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9, /* e8-ef */
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, /* f0-f7 */
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78	/* f8-ff */
};
/************************************************************************/

/************************************************************************/
ST_VOID adlcInitGlbVars(ST_VOID)
  {
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_ERR);	/* Enable error logging	*/

  portList = NULL;
  terminateFlag = SD_FALSE;

#if defined(ADLC_MASTER)
  pollGroupList = NULL;	
  gRrFrames = 1;
  gInfoFrames = 1;
#endif

#if !defined(ADLC_LM)
  eventList = NULL;
  stackTaskId = RLI_TASK_ID_RSGW;	/* assuming we are linked to RSGW */
#endif

#if defined(ADLC_LM)
  taskList = NULL;
  portConfigList = NULL; 		/* global port cfg data */
  serMgrSleep = 10;			/* just a guess			*/
#endif
  }
/************************************************************************/

/************************************************************************/
ST_VOID initSiscoUtilGlbVarsForAdlc(ST_VOID)
  {
#if defined(ADLC_TESTING)
/* set a call back function if memory problem is detected		*/
  mem_chk_err = memCheckError;
#endif

/* to fill freed memory with 0xCC	by util default it is SD_FALSE	*/
  m_fill_en = SD_FALSE;		

/* for heap checking after every memory alloc/free, util default = SD_FALSE */
  m_heap_check_enable = SD_FALSE;	

/* for allocated memory list checking after every memory alloc/free, = SD_FALSE */
  m_check_list_enable = SD_FALSE;		

/* enables the search for memory list before chk_free, util default = SD_TRUE */
  m_find_node_enable = SD_FALSE;

/* enables link list checking, util default = SD_TRUE			*/
  list_debug_sel = SD_FALSE;

  init_mem_chk ();
  gs_install();
  }


