/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_cfg.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*     Handle the configurable items for the ADLC LinkManager,		*/
/*     FrameEngine and SerialManager					*/
/* Note : The xxxBegin functions will push the table to the stack, the 	*/
/*        intermediate functions will retrieve the value, the xxxEnd	*/
/*        function will pop from the stack and make sure that all the 	*/
/* 	  values entered in the intermediate functions are valid	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/* ST_RET adlcConfigure(ST_VOID)						*/
/* ST_RET adlcReConfigure(ST_VOID)					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/23/02  KCR    42     Cleaned up general PClint errors             */
/* 09/23/02  KCR    41     Removed extra include files                  */
/* 09/23/02  KCR    40     Moved thisFileName before includes           */
/* 04/25/00  RKR    39     Ifdefed out read_log_cfg_file for MMS_LITE	*/
/* 09/09/99  KCR    38     Added logging cfg back in, required by DCSI 	*/
/* 09/13/99  MDE    37     Added SD_CONST modifiers         		*/
/* 02/03/99  JRB    36     Delete logging cfg altogether (much cleaner,	*/
/*			   fixes BUG that disabled All File Logging).	*/
/* 10/13/98  JRB    35     Enable logging cfg only if !MMS_LITE. For	*/
/*			   MMS_LITE, one cfg file controls all logging.	*/
/*			   DO NOT call "slogCloseFile".			*/
/* 10/08/98  MDE    34     Minor log cleanup				*/
/* 08/13/98  JRB    33     Lint cleanup.				*/
/* 04/20/98  IKE    32     Added updatexxx stubs			*/
/* 10/16/97  IKE    31     Changed default to cable and polled mode	*/
/*			   Put ifdef around adlcSetDefaultSlogValues	*/
/* 10/06/97  IKE    30     Added more baud rates, 			*/
/*			   Clean up of header files for non IPC version */
/*			   of WIN32 MMS lite				*/
/* 09/18/97  IKE    29     Added telephone support			*/
/* 06/15/97  IKE    28     Changed the DEFAULT_LOG_MASK			*/
/* 06/04/97  MDE    27     Changed acse2log.h to acse2usr.h		*/
/* 05/12/97  IKE    26     Changed to MMSEASE 7.0 Data Types		*/
/* 04/30/97  IKE    25     Removed ifdef __OS2__ around slog_remote_fun */
/* 04/30/97  IKE    24     Changed logMask6 to use ipc_debug_sel	*/
/* 03/06/97  IKE    23     Added IPC_LOG_DEBUG	 			*/
/* 02/24/97  IKE    22     Changed logMask6 to use adlc_debug_sel	*/
/*			   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/*			   CFG_PORT also for DOS so multiple ports can	*/
/*			   be initialized in dos platform		*/
/* 02/03/97  IKE    21     adlcCfg per port basis			*/
/* 01/22/97  IKE    20     Took default no apped action out		*/
/* 01/17/97  JRB    19     Only set slog_remote_fun for __OS2__.	*/
/* 01/17/97  IKE    18     Added "mFindNodeEn" and "lDebugEn"		*/
/*			   setCfgPort now uses addToCfgPortList		*/
/*			   Changed local var name to pptr or pstCfgPort	*/
/*			   to make it consistent with other part of code*/
/*			   Made all the functions static		*/
/* 12/03/96  IKE    17     WIN32 Support, header files cleanup		*/ 
/* 10/28/96  KCR    16     Converted to mulitple thread safe alloc funs */
/* 10/04/96  IKE    15     Added Reconfigure Option,			*/
/*			   Added "CommunicationMedia", Put ifdef MASTER */
/*			   around cfg parms which were for master only  */
/* 			   Replaced the code to add a device and into	*/
/*			   poll group by function calls			*/
/* 09/27/96  IKE    14     Removed DEBUG_SISCO and changed the default  */
/*			   value of port turnaround time		*/ 
/* 09/17/96  IKE    13     Added "RadioDekeyDelay","RadioStartOfTxDelay"*/
/*			   "PortTurnaroundTime","PreambleLen" and	*/
/*			   "PortUsage"			     		*/
/* 			   Removed slogCtrl Allocation, changed retries */
/*			   from 2 to 3, modified logging in ADLCParmsEnd*/
/* 08/22/96  IKE    12     Added IPC_LOG_ALWAYS 			*/
/* 08/16/96  IKE    11     Added "DeviceTurnaroundTime"			*/
/* 08/05/96  IKE    10     Added "ConnectAll" and "RadioDekeyDelay"	*/
/* 07/24/96  MDE    09     Added DEBUG_SISCO to TP4/ACSE ifdefs		*/
/* 06/21/96  IKE    08     Added following keywords in kwtblLogAttr tabl*/
/*   			     ADLC_LOG_SM      				*/
/*   			     ADLC_LOG_HEXSM      			*/
/*   			     ADLC_LOG_INFO_FRAME  			*/
/*   			     ADLC_LOG_HEXINFO_FRAME  			*/
/*   			     ADLC_LOG_USER  				*/
/*   			     ADLC_LOG_HEXUSER  				*/
/*			   some cleanup, changed some default  adlcCfg 	*/
/*			   values, changed verifyPollGroups to let 	*/
/* 			   configure less than 3 poll groups		*/
/* 06/18/96  IKE    07     Added MaxConnectRetryTimeout. Changed all the*/
/*			   parameters which were read in sec into m sec	*/
/* 06/12/96  KCR    06     Fixed multiple COM port config in OS2	*/
/* 06/11/96  IKE    05     Added getCfgPort. Added CNP related code	*/
/* 05/22/96  IKE    04     Header file cleanup. Added cfgError functions*/
/* 			   Added "RunMode" in adlc configuration. Added */
/*			   initPort and got rid of findPort		*/
/* 04/05/96  rkr    03	   More cleanup, merged missing cfg functions	*/
/* 04/04/96  rkr    02	   Cleanup					*/
/* 02/15/96  IKE    01	   Merged DOS adlc_cfg.c and rearrangement	*/
/* 12/15/95  NAV   1.00    Created                                      */
/************************************************************************/
static char *thisFileName = __FILE__;

/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"
#include <ctype.h>

/* Some other SISCO files						*/
#include "cfg_util.h"
#include "mem_chk.h"
#include "memlog.h"

/* ADLC files								*/
#include "adlc.h"

/************************************************************************/
/* Local defines							*/
/************************************************************************/

#define	POLL_HI_FREQ_TEXT	"HiFreq"
#define POLL_MED_FREQ_TEXT	"MedFreq"
#define POLL_LOW_FREQ_TEXT	"LowFreq"

#define ENV_VAR_LEN			255
#define ADLC_LM_CFG_FILE		"adlc.cfg"
/************************************************************************/
/* Static variables							*/
/************************************************************************/


static	ST_CHAR    	cfgFile [ENV_VAR_LEN + 1];

static  ADLC_ADD_DEV 	stCurrAddDev;
static	ST_BOOLEAN		bSecondPass = SD_FALSE;
static	ST_BOOLEAN		bReConfigureFlag = SD_FALSE;

#if defined(ADLC_MASTER)
static ST_INT  usCurrPollGrp=0;
#endif

#if defined(ADLC_LM)
static	SM_LINE_PARAM   *pstLineParam;
static	SM_HAND_SHAKE   *pstHandShake;		
static	SM_FLOW_CTRL	*pstFlowCtrl;
#endif

#if !defined(ADLC_LM)
static	CFG_PORT   *pstCurrCfgPort;
#endif
/************************************************************************/
/*  Functions prototypes 						*/
/************************************************************************/
static ST_VOID adlcSetDefaultSlogValues (ST_VOID);
static ST_VOID adlcSetDefaultCfgValues (ST_VOID);

static ST_RET adlcReadCfgFile (ST_CHAR *configFile);

static ST_VOID  cfgConfigBegin (ST_VOID);
static ST_VOID  cfgConfigEnd (ST_VOID);

static ST_VOID  cfgADLCParmsBegin (ST_VOID);
static ST_VOID  cfgADLCParmsEnd (ST_VOID);
static ST_VOID  cfgADLCBufSize (ST_VOID);
static ST_VOID  cfgADLCFrameLen (ST_VOID);
static ST_VOID  cfgADLCMaxReqs (ST_VOID);
static ST_VOID  cfgRunMode (ST_VOID);
static ST_VOID  cfgCommunicationMedia (ST_VOID);
static ST_VOID  cfgSof (ST_VOID);
static ST_VOID  cfgEof (ST_VOID);
static ST_VOID  cfgRetries (ST_VOID);
static ST_VOID  cfgResponseTimeout (ST_VOID);
static ST_VOID  cfgFrameTimeout (ST_VOID);
static ST_VOID  cfgMaxConnectRetryTimeout (ST_VOID);
static ST_VOID  cfgSignalTimeout (ST_VOID);
static ST_VOID  cfgRadioDekeyDelay (ST_VOID);
static ST_VOID  cfgRadioStartOfTxDelay (ST_VOID);
static ST_VOID  cfgDeviceTurnaroundTime (ST_VOID);
static ST_VOID  cfgPortTurnaroundTime (ST_VOID);
static ST_VOID  cfgPreambleLen (ST_VOID);
static ST_VOID  cfgPortUsage (ST_VOID);

#if defined(CNP)
#include "cnp_log.h"
#include "cnp_usr.h"
#include "cnp.h"
static ST_VOID  cfgCNPParmsBegin (ST_VOID);
static ST_VOID  cfgCNPParmsEnd (ST_VOID);
static ST_VOID  cfgIedPort (ST_VOID);
static ST_VOID  cfgRadioPort (ST_VOID);
static ST_VOID cnpSetDefaultValues (ST_VOID);
#endif

static ST_VOID  cfgPollParmsBegin (ST_VOID);
static ST_VOID  cfgPollParmsEnd (ST_VOID);
static ST_VOID  cfgPollParmsGroup (ST_VOID);
static ST_VOID  cfgPollParmsRate (ST_VOID);

static ST_VOID  cfgSerialParmsBegin (ST_VOID);
static ST_VOID  cfgSerialParmsEnd (ST_VOID);

static ST_VOID  cfgPortParmsBegin (ST_VOID);
static ST_VOID  cfgPortParmsEnd (ST_VOID);
static ST_VOID  cfgPort (ST_VOID);
static ST_VOID  cfgBaudRate (ST_VOID);
static ST_VOID  cfgParity (ST_VOID);
static ST_VOID  cfgDataBits (ST_VOID);
static ST_VOID  cfgStopBits (ST_VOID);
static ST_VOID  cfgDTRHandShake (ST_VOID);
static ST_VOID  cfgCTSHandShake (ST_VOID);
static ST_VOID  cfgDSRHandShake (ST_VOID);
static ST_VOID  cfgDCDHandShake (ST_VOID);
static ST_VOID  cfgDSRInputSensitivity (ST_VOID);
static ST_VOID  cfgRTSHandShake (ST_VOID);
static ST_VOID  cfgEnableSendFlow (ST_VOID);
static ST_VOID  cfgEnableRecFlow (ST_VOID);
static ST_VOID  cfgXONChar (ST_VOID);
static ST_VOID  cfgXOFFChar (ST_VOID);

static ST_VOID  cfgDevicesBegin (ST_VOID);
static ST_VOID  cfgDevicesEnd (ST_VOID);
static ST_VOID  cfgDevicePort (ST_VOID);
static ST_VOID  cfgDeviceName (ST_VOID);
static ST_VOID  cfgDeviceAddress (ST_VOID);
static ST_VOID  cfgDevicePollGrp (ST_VOID);
static ST_VOID  cfgDeviceConnectAll (ST_VOID);

static ST_VOID  cfgMemCheckBegin (ST_VOID);
static ST_VOID  cfgMemCheckEnd (ST_VOID);
static ST_VOID  cfgMemFillEn (ST_VOID);
static ST_VOID  cfgMemHeapCheckEn (ST_VOID);
static ST_VOID  cfgMemCheckListEn (ST_VOID);
static ST_VOID  cfgMemFindNodeEn (ST_VOID);
static ST_VOID  cfgListDebugEn (ST_VOID);

static ST_VOID cfgAdlcErrorBegin (ST_VOID);
static ST_VOID cfgAdlcErrorEnd (ST_VOID);
static ST_VOID  cfgSofErrorEn (ST_VOID);
static ST_VOID  cfgEofErrorEn (ST_VOID);
static ST_VOID  cfgNrErrorEn (ST_VOID);
static ST_VOID  cfgNsErrorEn (ST_VOID);
static ST_VOID  cfgLenErrorEn (ST_VOID);

static ST_VOID setCfgPort(ST_INT port);

static ST_CHAR logFile [ENV_VAR_LEN + 1];
#define ADLC_LM_LOG_FILE		"adlc.log"
#define ADLC_LM_LOG_FILE_MAX_SIZE	256000

#if defined(ADLC_LM)
static ST_VOID _GetHex (char *pString, int *pNumber);
#endif /* ADLC_LM */

#if !defined(ADLC_LM)
static ST_VOID setBaud(ST_LONG);
static ST_VOID setDataBits(ST_INT16);
#endif

#if defined(ADLC_MASTER)
static ST_RET verifyPollGroups(ST_VOID);
#endif 

static ADLC_PORT_INFO *initPort(ST_INT port);
static ST_RET extractPortNumber(ST_CHAR *portName, ST_INT *portNumber);
static ST_RET getCfgPort(ST_INT *port);
/*-----------------------------------------------------------------------
  Logger Logging Configuration keyword tables
------------------------------------------------------------------------*/

struct cfg_kw_el kwtblCfgBegin [] =
  {
    {"%ConfigurationBegin",	cfgConfigBegin,           NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblBegin [] =
  {
    {"%ADLCParmsBegin", 	cfgADLCParmsBegin,   	NULL },
    {"%PollParmsBegin", 	cfgPollParmsBegin,   	NULL },
    {"%SerialParmsBegin", 	cfgSerialParmsBegin,   	NULL },
    {"%DevicesBegin", 		cfgDevicesBegin,   	NULL },
    {"%MemCheckBegin",	  	cfgMemCheckBegin,    NULL },
    {"%AdlcErrorBegin", 	cfgAdlcErrorBegin,   	NULL },
#if defined(CNP)
    {"%CNPParmsBegin",		cfgCNPParmsBegin,  	NULL },
#endif
    {"%ConfigurationEnd",	cfgConfigEnd,  		NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblADLCAttr [] =
  {
    {"BufSize",			cfgADLCBufSize,		       	NULL},
    {"FrameLen",		cfgADLCFrameLen,	       	NULL},
    {"MaxADLCReqs", 		cfgADLCMaxReqs,		       	NULL},
    {"RunMode",			cfgRunMode,		       	NULL},
    {"CommunicationMedia",	cfgCommunicationMedia,	       	NULL},
    {"Sof", 			cfgSof,			       	NULL},
    {"Eof", 			cfgEof,			       	NULL},
    {"Retries", 		cfgRetries,		       	NULL},
    {"ResponseTimeout", 	cfgResponseTimeout,	       	NULL},
    {"FrameTimeout", 		cfgFrameTimeout,	       	NULL},
    {"MaxConnectRetryTimeout", 	cfgMaxConnectRetryTimeout,	NULL},
    {"SignalTimeout", 		cfgSignalTimeout,	       	NULL},
    {"RadioDekeyDelay", 	cfgRadioDekeyDelay,	       	NULL},
    {"RadioStartOfTxDelay", 	cfgRadioStartOfTxDelay,	       	NULL},
    {"DeviceTurnaroundTime", 	cfgDeviceTurnaroundTime,	NULL},
    {"PortTurnaroundTime", 	cfgPortTurnaroundTime,		NULL},
    {"PreambleLen",		cfgPreambleLen,			NULL},
    {"PortUsage",		cfgPortUsage,			NULL},
    {"%ADLCParmsEnd",		cfgADLCParmsEnd,		NULL},
    { NULL, NULL, NULL}
  };

#if defined(CNP)
struct cfg_kw_el kwtblCNPAttr [] =
  {
    {"RadioPort",	cfgRadioPort,		NULL},
    {"IEDPort",		cfgIedPort,		NULL},
    {"%CNPParmsEnd",	cfgCNPParmsEnd,		NULL},
    { NULL, NULL, NULL}
  };
#endif /* CNP */

struct cfg_kw_el kwtblPollParmsBegin [] =
  {
    {"%PollParmsBegin", 	cfgPollParmsBegin,   	NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblPollAttr [] =
  {
    {"PollGroup",	cfgPollParmsGroup,	NULL},
    {"PollRate",	cfgPollParmsRate,	NULL},
    {"%PollParmsEnd",	cfgPollParmsEnd,	NULL},
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblSerialParmsBegin [] =
  {
    {"%SerialParmsBegin", 	cfgSerialParmsBegin,   	NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblSerialParmsEnd [] =
  {
    {"%PortParmsBegin", 	cfgPortParmsBegin,   	NULL },
    {"%SerialParmsEnd", 	cfgSerialParmsEnd,   	NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblPortParmsBegin [] =
  {
    {"%PortParmsBegin", 	cfgPortParmsBegin,   	NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblSerialAttr [] =
  {
    {"Port",			cfgPort,		NULL},
    {"BaudRate",             	cfgBaudRate,    	NULL},
    {"Parity",			cfgParity,		NULL},
    {"DataBits",		cfgDataBits,		NULL},
    {"StopBits",		cfgStopBits,		NULL},
    {"DTRHandShake",		cfgDTRHandShake,	NULL},
    {"CTSHandShake",		cfgCTSHandShake,	NULL},
    {"DSRHandShake",		cfgDSRHandShake,	NULL},
    {"DCDHandShake",		cfgDCDHandShake,	NULL},
    {"DSRInputSensitivity",	cfgDSRInputSensitivity,	NULL},
    {"RTSHandShake",		cfgRTSHandShake,	NULL},
    {"EnableSendFlow",		cfgEnableSendFlow,	NULL},
    {"EnableReceiveFlow",	cfgEnableRecFlow,	NULL},
    {"XONChar",			cfgXONChar,		NULL},
    {"XOFFChar", 		cfgXOFFChar,		NULL},
    {"%PortParmsEnd",		cfgPortParmsEnd,	NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblDevicesBegin [] =
  {
    {"%DevicesBegin", 	cfgDevicesBegin,   	NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblDeviceAttr [] =
  {
    {"Port", 		cfgDevicePort,		NULL},
    {"Name",		cfgDeviceName,		NULL},
    {"Address", 	cfgDeviceAddress,	NULL},
    {"PollGrp",         cfgDevicePollGrp,	NULL},
    {"connectAll",      cfgDeviceConnectAll,	NULL},
    {"%DevicesEnd",	cfgDevicesEnd,		NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblMemCheckBegin [] =
  {
    {"%MemCheckBegin",	  cfgMemCheckBegin,    NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblMemCheckAttr [] =
  {
    {"mFillEn",		cfgMemFillEn,        NULL },
    {"mHeapCheckEn",	cfgMemHeapCheckEn,   NULL },
    {"mCheckListEn",	cfgMemCheckListEn,   NULL },
    {"mFindNodeEn",	cfgMemFindNodeEn,    NULL },
    {"lDebugEn",	cfgListDebugEn,      NULL },
    {"%MemCheckEnd",	cfgMemCheckEnd,      NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblAdlcErrorAttr [] =
  {
    {"sof",	cfgSofErrorEn,  NULL },
    {"eof",	cfgEofErrorEn,  NULL },
    {"nr",	cfgNrErrorEn,   NULL },
    {"ns",	cfgNsErrorEn,   NULL },
    {"len",	cfgLenErrorEn,  NULL },
    {"%AdlcErrorEnd", cfgAdlcErrorEnd,      NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el kwtblCfgEnd [] =
  {
    {"%ConfigurationEnd",	cfgConfigEnd,  		NULL },
    { NULL, NULL, NULL}
  };

#if defined(ADLC_LM)
/************************************************************************/

/************************************************************************/
ST_RET adlcReConfigure(ST_VOID)
  {
  bReConfigureFlag = SD_TRUE;
#if !defined(MMS_LITE)
  read_log_cfg_file ("log.cfg");                    
#endif
  return cfg_process_file (cfgFile, kwtblCfgBegin);	
  }

/************************************************************************/
ST_RET updateAdlcLoggingParameters(ST_VOID)
  {
// dummy function for compatibility with adlc_odb.c
  return (SD_SUCCESS);
  }

/************************************************************************/
ST_RET updateAdlcPollParameters(ST_UINT hi,ST_UINT med, ST_UINT low)
  {
  return (SD_SUCCESS);
  }
/************************************************************************/
ST_RET adlcCloseDatabase (ST_VOID)
  {
  return (SD_SUCCESS);
  }

#endif /* ADLC_LM */
/************************************************************************/
/* The logging process will start with default values if the cfg file	*/
/* could not be found or error occurrs while reading the configuration	*/
/* file.								*/
/************************************************************************/
ST_RET adlcConfigure(ST_VOID)
  {
#if defined(ADLC_LM) 
  adlcSetDefaultSlogValues ();
#endif

/* Set the ADLC specific configuration info				*/
  adlcSetDefaultCfgValues ();

#if defined(CNP)
/* Set the default CNP values						*/
  cnpSetDefaultValues ();
#endif

  /* Read the configuration file                                	*/
  sprintf (cfgFile, "%s", ADLC_LM_CFG_FILE);
  return (adlcReadCfgFile (cfgFile));
  }

/************************************************************************/
/*                          adlcSetDefaultSlogValues		       	*/
/************************************************************************/
static ST_VOID adlcSetDefaultSlogValues (ST_VOID)
  {
  /* Reset the sLogCtrl variable in case reading of cfg file failed	*/
  /* and some values were already set					*/
  memset (sLogCtrl, 0, sizeof (LOG_CTRL));

  /* set default name for log file			        */
  sprintf (logFile, "%s", ADLC_LM_LOG_FILE);
    
  ADLC_SET_CTRL (LOG_FILE_EN);		    /* Use File logging 	*/
  ADLC_SET_CTRL (LOG_TIME_EN); /* Std Time/Date logging	*/

  /* File Logging Control defaults					*/
  sLogCtrl->fc.fileName = logFile;
  sLogCtrl->fc.maxSize = ADLC_LM_LOG_FILE_MAX_SIZE;
  ADLC_SET_FILE_CTRL (FIL_CTRL_WIPE_EN | FIL_CTRL_WRAP_EN | FIL_CTRL_MSG_HDR_EN);
  }

/************************************************************************/
/*			adlcSetDefaultCfgValues				*/
/************************************************************************/

static ST_VOID adlcSetDefaultCfgValues (ST_VOID)
  {
  adlcCfg.modulus = 8;
  adlcCfg.frameLen = 256;
  adlcCfg.maxAdlcReqs = MAX_ADLC_REQS;
  adlcCfg.communicationMedia = CABLE_MEDIA;
  adlcCfg.retries = 3;
  adlcCfg.sof = 0x7E;
  adlcCfg.eof = 0x0D;
  adlcCfg.responseTimeout = 2.0 * 1000;
  adlcCfg.frameTimeout = 5.0 * 1000;
  adlcCfg.signalTimeout = 5.0 * 1000;
  adlcCfg.radioDekeyDelay = 10.0;
  adlcCfg.radioStartOfTxDelay = 10.0;
  adlcCfg.preambleLen = 0;
#if defined(ADLC_MASTER)
  adlcCfg.runMode = POLLED_MODE;
  adlcCfg.maxConnectRetryTimeout = 60.0 * 30 * 1000; /* half an hour */
  adlcCfg.deviceTurnaroundTime = 50.0;
  adlcCfg.portTurnaroundTime = 0;
  adlcCfg.portUsage = 80;
#endif
#if defined(ADLC_SLAVE)
  adlcCfg.runMode = POLLED_MODE;
#endif
  }

/************************************************************************/
/*                             adlcReadCfgFile                          */
/************************************************************************/
static ST_RET adlcReadCfgFile (ST_CHAR *configFile)
  {
ST_RET ret;
ST_UINT cfg_log_mask_save;

  /* read in log masks */
#if !defined(MMS_LITE)
  read_log_cfg_file ("log.cfg");
#endif

  /* If errors in configuration file, use defaults                      */

  /* This function will initialize the state table for reading the	*/
  /* configuration file and parse the file until EOF or end flag is set	*/

  bSecondPass = SD_FALSE;
  ret = cfg_process_file (configFile, kwtblCfgBegin);	

  /* check for errors encountered by the state functions                */
  if (ret != SD_SUCCESS)
    {
    adlcExit();		/*  Free Memory if we have allocated so far	*/
    /* Error encoutered in configuration file, reparse the file and	*/
    /* output configuration error messages to a log file		*/
    cfg_log_mask_save = cfg_log_mask;
    cfg_log_mask |= CFG_LOG_ERR | CFG_LOG_FLOW;
    bSecondPass = SD_TRUE;
    ret = cfg_process_file (cfgFile, kwtblCfgBegin);
    cfg_log_mask = cfg_log_mask_save;
    }
  return (ret);
  }

/*-----------------------------------------------------------------------
 *  Internal functions for handling state tables
 -----------------------------------------------------------------------*/
/************************************************************************/
/*			cfgConfigBegin					*/
/* key word = 'ConfigurationBegin'					*/
/************************************************************************/
static ST_VOID  cfgConfigBegin (ST_VOID)
  {
  /* Initial state for parsing the configuration file			*/
  /* Configuration file successfully opened, read first section		*/

  if (cfg_tbl_push (kwtblBegin) != SD_SUCCESS) 
    cfg_set_config_err ();
  }
/************************************************************************/
/*			cfgConfigEnd					*/
/* keyword  = 'ConfigurationEnd'					*/
/************************************************************************/
static ST_VOID  cfgConfigEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();

  cfg_set_endfile_ok ();	/* this is the end of configuration	*/
  }

/*-----------------------------------------------------------------------
 *	Processing of keywords found in the sections:
 *		%ADLCParmsBegin ... %ADLCParmsEnd
 *----------------------------------------------------------------------*/

/************************************************************************/
/*			cfgADLCParmsBegin				*/
/************************************************************************/
static ST_VOID  cfgADLCParmsBegin (ST_VOID)
  {
  if (cfg_tbl_push (kwtblADLCAttr) != SD_SUCCESS) 
      cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgADLCBufSize					*/
/* found 'BufSize' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgADLCBufSize (ST_VOID)
  {
ST_INT16	sVal;

  if (cfg_get_short ((ST_INT16 *) &sVal) == SD_SUCCESS)
    adlcCfg.modulus = sVal;
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgADLCFrameLen					*/
/* found 'FrameLen' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgADLCFrameLen (ST_VOID)
  {
ST_INT16	sVal;

  if (cfg_get_short ((ST_INT16 *) &sVal) == SD_SUCCESS)
    adlcCfg.frameLen = sVal;
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgADLCMaxReqs					*/
/* found 'MaxADLCReqs' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgADLCMaxReqs (ST_VOID)
  {
ST_INT16	sVal;

  if (cfg_get_short ((ST_INT16 *) &sVal) == SD_SUCCESS)
    adlcCfg.maxAdlcReqs = sVal;
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgRunMode					*/
/* found 'RunMode' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgRunMode (ST_VOID)
  {
ST_CHAR    *runMode = NULL;


  if ((runMode = cfg_get_string_ptr ()) != NULL)
    {
    if (!strcmpi(runMode, "Poll"))
      adlcCfg.runMode = POLLED_MODE;	
    else if (!strcmpi(runMode, "EnhancedPoll"))
      adlcCfg.runMode = ENHANCED_POLLED_MODE;	
    else if (!strcmpi(runMode, "Contention"))
      adlcCfg.runMode = CONTENTION_MODE;
    else
      cfg_set_config_err();
    }
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgCommunicationMedia				*/
/* found 'CommunicationMedia' keyword, get the value			*/
/************************************************************************/

static ST_VOID  cfgCommunicationMedia (ST_VOID)
  {
ST_CHAR    *communicationMedia = NULL;


  if ((communicationMedia = cfg_get_string_ptr ()) != NULL)
    {
    if (!strcmpi(communicationMedia, "Cable"))
      adlcCfg.communicationMedia = CABLE_MEDIA;
    else if (!strcmpi(communicationMedia, "Radio"))
      adlcCfg.communicationMedia = RADIO_MEDIA;
    else
      cfg_set_config_err();
    }
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgSof						*/
/* found 'Sof' keyword, get the value					*/
/************************************************************************/

static ST_VOID  cfgSof (ST_VOID)
  {
ST_UINT16	sVal;

  if (cfg_get_hex_ushort ((ST_UINT16 *) &sVal) == SD_SUCCESS)
    adlcCfg.sof = (ST_UCHAR) sVal;
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgEof						*/
/* found 'Eof' keyword, get the value					*/
/************************************************************************/

static ST_VOID  cfgEof (ST_VOID)
  {
ST_UINT16	sVal;

  if (cfg_get_hex_ushort ((ST_UINT16 *) &sVal) == SD_SUCCESS)
    adlcCfg.eof =  (ST_UCHAR) sVal;
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgRetries					*/
/* found 'Retries' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgRetries (ST_VOID)
  {
ST_INT16	sVal;

  if (cfg_get_short ((ST_INT16 *) &sVal) == SD_SUCCESS)
    adlcCfg.retries = sVal;
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgResponseTimeout				*/
/* found 'ResponseTimeout' keyword, get the value			*/
/************************************************************************/

static ST_VOID  cfgResponseTimeout (ST_VOID)
  {
ST_ULONG	ulVal;

  if (cfg_get_ulong (&ulVal) == SD_SUCCESS)
    adlcCfg.responseTimeout = (ST_DOUBLE) ulVal;
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgFrameTimeout					*/
/* found 'FrameTimeout' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgFrameTimeout (ST_VOID)
  {
ST_ULONG	ulVal;

  if (cfg_get_ulong (&ulVal) == SD_SUCCESS)
    adlcCfg.frameTimeout = (ST_DOUBLE) ulVal;
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgSignalTimeout					*/
/* found 'SignalTimeout' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgSignalTimeout (ST_VOID)
  {
ST_ULONG	ulVal;

  if (cfg_get_ulong (&ulVal) == SD_SUCCESS)
    adlcCfg.signalTimeout = (ST_DOUBLE) ulVal;
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgRadioDekeyDelay					*/
/* found 'RadioDekeyDelay' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgRadioDekeyDelay (ST_VOID)
  {
ST_ULONG	ulVal;

  if (cfg_get_ulong (&ulVal) == SD_SUCCESS)
    adlcCfg.radioDekeyDelay = (ST_DOUBLE) ulVal;
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgRadioStartOfTxDelay					*/
/* found 'RadioStartOfTxDelay' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgRadioStartOfTxDelay (ST_VOID)
  {
ST_ULONG	ulVal;

  if (cfg_get_ulong (&ulVal) == SD_SUCCESS)
    adlcCfg.radioStartOfTxDelay = (ST_DOUBLE) ulVal;
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgPreambleLen					*/
/* found 'PreambleLen' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgPreambleLen (ST_VOID)
  {
ST_INT16	sVal;

  if (cfg_get_short ((ST_INT16 *) &sVal) == SD_SUCCESS)
    adlcCfg.preambleLen = sVal;
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*		cfgMaxConnectRetryTimeout				*/
/* found 'MaxReconnectTimeout' keyword, get the value			*/
/************************************************************************/

static ST_VOID  cfgMaxConnectRetryTimeout (ST_VOID)
  {
ST_ULONG	ulVal;

  if (cfg_get_ulong (&ulVal) == SD_SUCCESS)
    {
#if defined(ADLC_MASTER)
    adlcCfg.maxConnectRetryTimeout = (ST_DOUBLE) ulVal;
#endif /* ADLC_MASTER */
    }
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgDeviceTurnaroundTime				*/
/* found 'DeviceTurnaroundTime' keyword, get the value			*/
/************************************************************************/

static ST_VOID  cfgDeviceTurnaroundTime (ST_VOID)
  {
ST_ULONG	ulVal;

  if (cfg_get_ulong (&ulVal) == SD_SUCCESS)
    {
#if defined(ADLC_MASTER)
    adlcCfg.deviceTurnaroundTime = (ST_DOUBLE) ulVal;
#endif /* ADLC_MASTER */
    }
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgPortTurnaroundTime				*/
/* found 'PortTurnaroundTime' keyword, get the value			*/
/************************************************************************/

static ST_VOID  cfgPortTurnaroundTime (ST_VOID)
  {
ST_ULONG	ulVal;

  if (cfg_get_ulong (&ulVal) == SD_SUCCESS)
    {
#if defined(ADLC_MASTER)
    adlcCfg.portTurnaroundTime = (ST_DOUBLE) ulVal;
#endif /* ADLC_MASTER */
    }
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgPortUsage					*/
/* found 'PortUsage' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgPortUsage (ST_VOID)
  {
ST_INT16	sVal;

  if (cfg_get_short ((ST_INT16 *) &sVal) == SD_SUCCESS)
    {
#if defined(ADLC_MASTER)
    adlcCfg.portUsage = sVal;
#endif /* ADLC_MASTER */
    }
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgADLCParmsEnd					*/
/************************************************************************/
static ST_VOID  cfgADLCParmsEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) == SD_SUCCESS)
    {
/* make sure that frameLen is configured right			*/
    if (adlcCfg.frameLen < MIN_INFO_FRAME_LEN || adlcCfg.frameLen > MAX_ADLC_FRAME_LEN)
      {
      cfg_set_config_err ();
      if (bSecondPass || bReConfigureFlag)
        {
        ADLC_LOG_ERR4("ADLC ERROR Frame Length %d should be between %d and %d, Default %d used",
			adlcCfg.frameLen,MIN_ADLC_FRAME_LEN,MAX_ADLC_FRAME_LEN,MAX_ADLC_FRAME_LEN);
	}
      adlcCfg.frameLen = MAX_ADLC_FRAME_LEN;
      }

/* make sure that maxAdlcReqs is configured right			*/
    if (adlcCfg.maxAdlcReqs > MAX_ADLC_REQS) 
      {
      cfg_set_config_err ();
      if (bSecondPass || bReConfigureFlag)
        {
        ADLC_LOG_ERR2("ADLC ERROR MaxAdlcReqs  %d too large, Default %d used",
		        adlcCfg.maxAdlcReqs, MAX_ADLC_REQS);
	}
      adlcCfg.maxAdlcReqs = MAX_ADLC_REQS;
      }

/* make sure that if we are using cable we can not use contention and no delays are reqd.*/
    if (adlcCfg.communicationMedia == CABLE_MEDIA)
      {
      adlcCfg.radioDekeyDelay = 0;
      adlcCfg.radioStartOfTxDelay = 0;
#if defined(ADLC_MASTER)
      adlcCfg.runMode = POLLED_MODE;
      adlcCfg.deviceTurnaroundTime = 0;
      adlcCfg.portTurnaroundTime = 0;
#endif
#if defined(ADLC_SLAVE)
      adlcCfg.runMode = POLLED_MODE;
#endif
      }
/* make sure that portUsage is configured right				*/
#if defined(ADLC_MASTER)
    if (adlcCfg.portUsage < 0 || adlcCfg.portUsage > 100)
      {
      cfg_set_config_err ();
      if (bSecondPass || bReConfigureFlag)
        {
        ADLC_LOG_ERR2("ADLC ERROR portUsage %d should be between 0 and 100, Default %d used",
		        adlcCfg.portUsage, 80)
	}
      adlcCfg.portUsage = 80;
      }
#endif /* ADLC_MASTER */
    }
  else 
    cfg_set_config_err ();
  }
/*-----------------------------------------------------------------------
 *	Processing of keywords found in the sections:
 *		%PollParmsBegin ... %PollParmsEnd
 *----------------------------------------------------------------------*/

/************************************************************************/
/*			cfgPollParmsBegin				*/
/************************************************************************/
static ST_VOID  cfgPollParmsBegin (ST_VOID)
  {
  if (cfg_tbl_push (kwtblPollAttr) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgPollParmsGroup				*/
/* found 'PollGroup' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgPollParmsGroup (ST_VOID)
  {
ST_CHAR    *pollFreq = NULL;

  if ((pollFreq = cfg_get_string_ptr ()) != NULL)
    {
#if defined(ADLC_MASTER)
    if (!strcmpi(pollFreq, POLL_HI_FREQ_TEXT))
      usCurrPollGrp = ADLC_POLL_FREQ_HI;
    else if (!strcmpi(pollFreq, POLL_MED_FREQ_TEXT))
      usCurrPollGrp = ADLC_POLL_FREQ_MED;
    else if (!strcmpi(pollFreq, POLL_LOW_FREQ_TEXT))
      usCurrPollGrp = ADLC_POLL_FREQ_LOW;
    else
      cfg_set_config_err();
#endif /* ADLC_MASTER */
    }
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgPollParmsRate				*/
/* found 'PollRate' keyword, get the value				*/
/************************************************************************/

static ST_VOID  cfgPollParmsRate (ST_VOID)
  {
ST_ULONG	pollRate;

#if defined(ADLC_MASTER)
POLL_GROUP *pstPollGroup;
POLL_GROUP *pstNewPollGroup = NULL;
#endif

  if (cfg_get_ulong (&pollRate) == SD_SUCCESS)
    {
#if defined(ADLC_MASTER)
    if (usCurrPollGrp > 0)
      {
      pstPollGroup = findPollGroup(usCurrPollGrp);
      if (!pstPollGroup)   /* no poll group is configured with usCurrPollGroup */
        pstNewPollGroup = addToPollGroupList(usCurrPollGrp, (double) pollRate);
#if defined(ADLC_LM)
      if (bReConfigureFlag)   /* if we are reconfiguring */
        {
        if (pstPollGroup && pstPollGroup->pollRate != pollRate)	
          {		/* poll grp was there but poll rate was changed */
	  pstPollGroup->pollRate = pollRate;   /* update polling rate   */
          stopPollTimer(pstPollGroup);
	  startPollTimer(pstPollGroup);
	  }
	else if (pstNewPollGroup)   /* in reconfiguring some one put new poll grp */
	  {
	  startPollTimer(pstNewPollGroup);
	  }
        }
#endif /* ADLC_LM */
      }
#endif /* ADLC_MASTER */
    }
  else
    cfg_set_config_err();
  }
/************************************************************************/
/*			cfgPollParmsEnd					*/
/************************************************************************/
static ST_VOID  cfgPollParmsEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) == SD_SUCCESS)
    {
#if defined(ADLC_MASTER)
    if (verifyPollGroups() != SD_SUCCESS)
      {
      cfg_set_config_err ();
      if (bSecondPass || bReConfigureFlag)
        ADLC_LOG_ERR0("ADLC ERROR Poll Groups are not configured properly");
      }
#endif
    }
  else 
    cfg_set_config_err ();
  }
/*-----------------------------------------------------------------------
 *	Processing of keywords found in the sections:
 *		%SerialParmsBegin ... %SerialParmsEnd
 *		%PortParmsBegin   ... %PortParmsEnd
 *----------------------------------------------------------------------*/

/************************************************************************/
/*			cfgSerialParmsBegin			*/
/************************************************************************/
static ST_VOID  cfgSerialParmsBegin (ST_VOID)
  {
  if (cfg_tbl_push (kwtblSerialParmsEnd) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgPortParmsBegin			*/
/************************************************************************/
static ST_VOID  cfgPortParmsBegin (ST_VOID)
  {
  if (cfg_tbl_push (kwtblSerialAttr) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgPortParmsEnd					*/
/************************************************************************/
static ST_VOID  cfgPortParmsEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgSerialParmsEnd				*/
/************************************************************************/
static ST_VOID  cfgSerialParmsEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgPort 					*/
/* Found 'Port' keyword, get the value and setup data structs  		*/
/************************************************************************/

static ST_VOID cfgPort (ST_VOID)
  {
ST_INT	 port;
ADLC_PORT_INFO *pptr;


  if (getCfgPort(&port) != SD_SUCCESS)
    {
    cfg_set_config_err();
    return;
    }
#if defined(ADLC_LM)
  if (bReConfigureFlag)
    return;
#endif

/* search the adlc port list  - if not found call addToPortList		*/
  pptr = initPort(port);
  setCfgPort(port);
  }

/************************************************************************/
/*			cfgBaudRate 					*/
/* Found 'BaudRate' keyword, get the value                          	*/
/************************************************************************/

static ST_VOID  cfgBaudRate (ST_VOID)
  {
ST_LONG	lVal;

  if (cfg_get_long (&lVal) == SD_SUCCESS)
    {
#if defined(ADLC_LM)
    if (bReConfigureFlag == SD_FALSE)
      pstLineParam->baudRate = (ST_INT) lVal;
#endif
#if !defined(ADLC_LM)
    setBaud(lVal);
#endif 
    }
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgParity 					*/
/* Found 'Parity' keyword, get the value                          	*/
/************************************************************************/
 
static ST_VOID  cfgParity (ST_VOID)
  {
ST_CHAR  *parity = NULL;

  if ((parity = cfg_get_string_ptr ()) != NULL)
    {
#if defined(ADLC_LM)
    if (bReConfigureFlag == SD_FALSE)
      {
      if (!strcmpi(parity, "none"))
        pstLineParam->parity = 0;
      else if (!strcmpi(parity, "odd"))
        pstLineParam->parity = 1;
      else if (!strcmpi(parity, "even"))
        pstLineParam->parity = 2;
      else if (!strcmpi(parity, "mark"))
        pstLineParam->parity = 0;
      else if (!strcmpi(parity, "space"))
        pstLineParam->parity = 0;
      else
        cfg_set_config_err();
      }
#endif
#if !defined(ADLC_LM)
    if (!strcmpi(parity, "none"))
      pstCurrCfgPort->parity = NOPAR;	
    else if (!strcmpi(parity, "odd"))
      pstCurrCfgPort->parity = ODDPAR;	
    else if (!strcmpi(parity, "even"))
      pstCurrCfgPort->parity = EVENPAR;	
    else if (!strcmpi(parity, "mark"))
      pstCurrCfgPort->parity = NOPAR;	
    else if (!strcmpi(parity, "space"))
      pstCurrCfgPort->parity = NOPAR;	
    else
      cfg_set_config_err();
#endif
    }
  else
    cfg_set_config_err();

  }

/************************************************************************/
/*			cfgDataBits 					*/
/* Found 'DataBits' keyword, get the value                          	*/
/************************************************************************/
 
static ST_VOID  cfgDataBits (ST_VOID)
  {
ST_INT16	sVal;

  if (cfg_get_short ((ST_INT16 *) &sVal) == SD_SUCCESS)
    {
    if ( (sVal>=5) && (sVal<=8) )
      {
#if defined(ADLC_LM)
      if (bReConfigureFlag == SD_FALSE)
        pstLineParam->dataBits = sVal;
#endif 
#if !defined(ADLC_LM)
      setDataBits(sVal);
#endif
      }
    else
      cfg_set_config_err();
    }
  else
    cfg_set_config_err();

  }

/************************************************************************/
/*			cfgStopBits 					*/
/* Found 'StopBits' keyword, get the value                          	*/
/************************************************************************/
 
static ST_VOID  cfgStopBits (ST_VOID)
  {
ST_CHAR  *stopBits = NULL;

  if ((stopBits = cfg_get_string_ptr ()) != NULL)
    {
#if defined(ADLC_LM)
    if (bReConfigureFlag == SD_FALSE)
      {
      if (!strcmpi(stopBits, "1"))
        pstLineParam->stopBits = 0;
      else if (!strcmpi(stopBits, "1.5"))
        pstLineParam->stopBits = 1;
      else if (!strcmpi(stopBits, "2"))
        pstLineParam->stopBits = 2;
      else
        cfg_set_config_err();
      }	
#endif
#if !defined(ADLC_LM)
    if (!strcmpi(stopBits, "1"))
      pstCurrCfgPort->stop = STOP1;
    else if (!strcmpi(stopBits, "1.5"))
      pstCurrCfgPort->stop = STOP1;
    else if (!strcmpi(stopBits, "2"))
      pstCurrCfgPort->stop = STOP2;
    else
      cfg_set_config_err();
#endif
    }
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgDTRHandShake					*/
/* Found 'DTRHandShake' keyword, set the boolean                        */
/************************************************************************/

static ST_VOID  cfgDTRHandShake (ST_VOID)
  {
#if defined(ADLC_LM)
  if (bReConfigureFlag == SD_FALSE)
    pstHandShake->enableDTR = SD_TRUE;
#endif /* ADLC_LM */
  }

/************************************************************************/
/*			cfgCTSHandShake					*/
/* Found 'CTSHandShake' keyword, set the boolean                        */
/************************************************************************/

static ST_VOID  cfgCTSHandShake (ST_VOID)
  {
#if defined(ADLC_LM)
  if (bReConfigureFlag == SD_FALSE)
    pstHandShake->enableCTS = SD_TRUE;
#endif /* ADLC_LM */
  }

/************************************************************************/
/*			cfgDSRHandShake					*/
/* Found 'DSRHandShake' keyword, set the boolean                        */
/************************************************************************/

static ST_VOID  cfgDSRHandShake (ST_VOID)
  {
#if defined(ADLC_LM)
  if (bReConfigureFlag == SD_FALSE)
    pstHandShake->enableDSRout = SD_TRUE;
#endif /* ADLC_LM */
  }

/************************************************************************/
/*			cfgDCDHandShake					*/
/* Found 'DCDHandShake' keyword, set the boolean                        */
/************************************************************************/

static ST_VOID  cfgDCDHandShake (ST_VOID)
  {
#if defined(ADLC_LM)
  if (bReConfigureFlag == SD_FALSE)
    pstHandShake->enableDCD = SD_TRUE;
#endif /* ADLC_LM */
  }

/************************************************************************/
/*			cfgDSRInputSensitivity				*/
/* Found 'DSRInputSensitivity' keyword, set the boolean                 */
/************************************************************************/

static ST_VOID  cfgDSRInputSensitivity (ST_VOID)
  {
#if defined(ADLC_LM)
  if (bReConfigureFlag == SD_FALSE)
    pstHandShake->enableDSRin = SD_TRUE;
#endif /* ADLC_LM */
  }

/************************************************************************/
/*			cfgRTSHandShake					*/
/* Found 'RTSHandShake' keyword, set the boolean                        */
/************************************************************************/

static ST_VOID  cfgRTSHandShake (ST_VOID)
  {
#if defined(ADLC_LM)
  if (bReConfigureFlag == SD_FALSE)
    pstHandShake->enableRTS = SD_TRUE;
#endif /* ADLC_LM */
  }

/************************************************************************/
/*			cfgEnableSendFlow				*/
/* Found 'EnableSendFlow' keyword, set the boolean                      */
/************************************************************************/

static ST_VOID  cfgEnableSendFlow (ST_VOID)
  {
#if defined(ADLC_LM)
  if (bReConfigureFlag == SD_FALSE)
    pstFlowCtrl->enableTxFlow = SD_TRUE;
#endif /* ADLC_LM */
  }

/************************************************************************/
/*			cfgEnableRecFlow				*/
/* Found 'EnableRecFlow' keyword, set the boolean                      */
/************************************************************************/

static ST_VOID  cfgEnableRecFlow (ST_VOID)
  {
#if defined(ADLC_LM)
  if (bReConfigureFlag == SD_FALSE)
    pstFlowCtrl->enableRxFlow = SD_TRUE;
#endif /* ADLC_LM */
  }

/************************************************************************/
/*			cfgXONChar					*/
/* Found 'XONChar' keyword, get the char	                      	*/
/************************************************************************/

static ST_VOID  cfgXONChar (ST_VOID)
  {
ST_CHAR *getSome = NULL;

#if defined(ADLC_LM)
ST_CHAR *p;
int  num;
#endif

  if ((getSome = cfg_get_string_ptr ()) != NULL)
    {
#if defined(ADLC_LM)
    if (bReConfigureFlag)
      return;
    p = getSome;
    if ( (p[0]=='\\') && ((p[1]=='x') || (p[1]=='X')) )
      {
      p+=2;
      _GetHex(p, &num);
      pstFlowCtrl->xonChar = (ST_UCHAR) num;
      }
    else if (isdigit(p[0]))
      {
      num = atoi(p);
      pstFlowCtrl->xonChar = (ST_UCHAR) num;
      }
    else
      pstFlowCtrl->xonChar = (ST_UCHAR) p[0];
#endif /* ADLC_LM */
    }
  else
    cfg_set_config_err();

  }

/************************************************************************/
/*			cfgXOFFChar					*/
/* Found 'XOFFChar' keyword, get the char	                      	*/
/************************************************************************/

static ST_VOID  cfgXOFFChar (ST_VOID)
  {
ST_CHAR *getSome = NULL;

#if defined(ADLC_LM)
ST_CHAR *p;
int  num;
#endif

  if ((getSome = cfg_get_string_ptr ()) != NULL)
    {
#if defined(ADLC_LM)
    if (bReConfigureFlag)
      return;
    p = getSome;
    if ( (p[0]=='\\') && ((p[1]=='x') || (p[1]=='X')) )
      {
      p+=2;
      _GetHex(p, &num);
      pstFlowCtrl->xoffChar = (ST_UCHAR) num;
      }
    else if (isdigit(p[0]))
      {
      num = atoi(p);
      pstFlowCtrl->xoffChar = (ST_UCHAR) num;
      }
    else
      pstFlowCtrl->xoffChar = (ST_UCHAR) p[0];
#endif /* ADLC_LM */
    }
  else
    cfg_set_config_err();
  }
/*-----------------------------------------------------------------------
 *	Processing of keywords found in the sections:
 *		%DevicesBegin ... %DevicesEnd
 *----------------------------------------------------------------------*/

/************************************************************************/
/*			cfgDevicesBegin					*/
/************************************************************************/
static ST_VOID  cfgDevicesBegin (ST_VOID)
  {
  if (cfg_tbl_push (kwtblDeviceAttr) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgDevicesEnd					*/
/************************************************************************/
static ST_VOID  cfgDevicesEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgDevicePort					*/
/* Found 'Port' keyword, set the value	        	              	*/
/************************************************************************/

static ST_VOID  cfgDevicePort (ST_VOID)
  {
ST_INT	 port;

  if (getCfgPort(&port) == SD_SUCCESS)
    stCurrAddDev.i.port = port;
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgDeviceName					*/
/* Found 'Name' keyword, set the string	        	              	*/
/************************************************************************/

static ST_VOID  cfgDeviceName (ST_VOID)
  {
ST_CHAR *name = NULL;

  if ((name = cfg_get_string_ptr ()) != NULL)
    strcpy(stCurrAddDev.i.devName, name);
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgDeviceAddress				*/
/* Found 'Address' keyword, set the value	        	        */
/************************************************************************/

static ST_VOID  cfgDeviceAddress (ST_VOID)
  {
ST_UINT16		sVal;

  if (cfg_get_ushort ((ST_UINT16 *) &sVal) == SD_SUCCESS)
    {
    stCurrAddDev.i.address = sVal;
#if defined(ADLC_LM)
    if (bReConfigureFlag)
      {
      if (getDevPtrFromAddress(stCurrAddDev.i.address))	/* device already added */
        return;
      }
#endif
    if (adlcAddDev(&stCurrAddDev) != SD_SUCCESS)
      cfg_set_config_err();
    }	
  else
    cfg_set_config_err();
  }

/************************************************************************/
/*			cfgDevicePollGrp				*/
/* Found 'PollGrp' keyword, set the value	        	        */
/************************************************************************/

static ST_VOID  cfgDevicePollGrp (ST_VOID)
  {
ST_CHAR    *pollFreq = NULL;

#if defined(ADLC_MASTER)
ST_UINT16  usPollGrp = 0;
ADLC_DEV_INFO *dptr = NULL;
#endif /* ADLC_MASTER */  

#if defined(ADLC_MASTER) && defined(ADLC_LM)
POLL_GROUP	*pstCurrPollGrp, *pstNewPollGrp;
POLLED_DEV *pstPollDev;
ST_BOOLEAN bAddPollGrpFlag = SD_FALSE;
#endif

  if ((pollFreq = cfg_get_string_ptr ()) == NULL)
    {
    cfg_set_config_err();
    return;
    }
    
#if defined(ADLC_MASTER)
  if (!strcmpi(pollFreq, POLL_HI_FREQ_TEXT))
    usPollGrp = ADLC_POLL_FREQ_HI;
  else if (!strcmpi(pollFreq, POLL_MED_FREQ_TEXT))
    usPollGrp = ADLC_POLL_FREQ_MED;
  else if (!strcmpi(pollFreq, POLL_LOW_FREQ_TEXT))
    usPollGrp = ADLC_POLL_FREQ_LOW;
  else
    {
    cfg_set_config_err();
    return;
    }

  if ((dptr = getDevPtrFromAddress(stCurrAddDev.i.address)) == NULL)
    {
    cfg_set_config_err();
    return;
    }
#if defined(ADLC_LM)
  if (bReConfigureFlag)
    {
    if (dptr->pollGroup && dptr->pollGroup != usPollGrp)	/* user has changed the poll group */
      {
      pstCurrPollGrp = findPollGroup(dptr->pollGroup);
      pstNewPollGrp  = findPollGroup(usPollGrp);
      pstPollDev = findPolledDevByAddress(dptr->address);
      if (!pstCurrPollGrp || !pstNewPollGrp || !pstPollDev)
        cfg_set_config_err();
      else
        {
        /* move the device from one poll group and add it to the other	*/
        list_move_to_first((ST_VOID **) &pstCurrPollGrp->pollDevList,
                     (ST_VOID **) &pstNewPollGrp->pollDevList, pstPollDev);

        /* update the device info structure poll group reference		*/
        dptr->pollGroup = usPollGrp;
	}
      }
    else
      {
      bAddPollGrpFlag = SD_TRUE;
      }
    }
  else /* it is not reconfiguration */
    {
    bAddPollGrpFlag = SD_TRUE;
    }

  if  (bAddPollGrpFlag == SD_TRUE)
    {
    if (addDevToPollGrp(dptr,usPollGrp) != SD_SUCCESS)
      cfg_set_config_err();
    }
#endif /* ADLC_LM */

#if !defined(ADLC_LM)
  if (addDevToPollGrp(dptr,usPollGrp) != SD_SUCCESS)
    cfg_set_config_err();
#endif /* !ADLC_LM */
#endif /* ADLC_MASTER */
  }

/************************************************************************/
/*			cfgDeviceConnectAll				*/
/* Found 'ConnectAll' keyword, set the value	        	        */
/************************************************************************/

static ST_VOID  cfgDeviceConnectAll (ST_VOID)
  {
#if defined(ADLC_MASTER)
ADLC_DEV_INFO *dptr;

  if ((dptr = getDevPtrFromAddress(stCurrAddDev.i.address)) != NULL)
    dptr->connectAll = SD_TRUE;
  else
    cfg_set_config_err();
#endif
  }

/*-----------------------------------------------------------------------
 *	Processing of keywords found in the sections:
 *		%MemCheckBegin ... %MemCheckEnd
 *----------------------------------------------------------------------*/

/************************************************************************/
/*			cfgMemCheckBegin			        */
/************************************************************************/
static ST_VOID  cfgMemCheckBegin (ST_VOID)
  {
  if (cfg_tbl_push (kwtblMemCheckAttr) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgMemCheckEnd					*/
/************************************************************************/
static ST_VOID  cfgMemCheckEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgMemFillEn					*/
/************************************************************************/
static ST_VOID  cfgMemFillEn (ST_VOID)
  {
  /* Found 'mFillEn' flag                                                */
  m_fill_en = SD_TRUE;
  }

/************************************************************************/
/*			cfgMemHeapCheckEn				*/
/************************************************************************/
static ST_VOID  cfgMemHeapCheckEn (ST_VOID)
  {
  /* Found 'mHeapCheckEn' flag                                                */
  m_heap_check_enable = SD_TRUE;
  }

/************************************************************************/
/*			cfgMemCheckListEn					*/
/************************************************************************/
static ST_VOID  cfgMemCheckListEn (ST_VOID)
  {
  /* Found 'mCheckListEn' flag                                                */
  m_check_list_enable = SD_TRUE;
  }
/************************************************************************/
/*			cfgMemFindNodeEn				*/
/************************************************************************/
static ST_VOID  cfgMemFindNodeEn (ST_VOID)
  {
  /* Found 'mFindNodeEn' flag                                            */
  m_find_node_enable = SD_TRUE;
  }
/************************************************************************/
/*			cfgListDebugEn					*/
/************************************************************************/
static ST_VOID  cfgListDebugEn (ST_VOID)
  {
  /* Found 'lDebugEn' flag                                                */
  list_debug_sel = SD_TRUE;
  }
/*-----------------------------------------------------------------------
 *	Processing of keywords found in the sections:
 *		%AdlcErrorBegin ... %AdlcErrorEnd
 *----------------------------------------------------------------------*/

/************************************************************************/
/* 			cfgAdlcErrorBegin 				*/
/************************************************************************/
static ST_VOID  cfgAdlcErrorBegin (ST_VOID)
  {
  if (cfg_tbl_push (kwtblAdlcErrorAttr) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgAdlcErrorEnd					*/
/************************************************************************/
static ST_VOID  cfgAdlcErrorEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfgSofErrorEn					*/
/************************************************************************/
static ST_VOID  cfgSofErrorEn (ST_VOID)
  {
  /* Found 'sof' flag                                                */
#if defined(ADLC_ERROR)
  adlcError.sof = SD_TRUE;
#endif
  }

/************************************************************************/
/*			cfgEofErrorEn					*/
/************************************************************************/
static ST_VOID  cfgEofErrorEn (ST_VOID)
  {
  /* Found 'eof' flag                                                */
#if defined(ADLC_ERROR)
  adlcError.eof = SD_TRUE;
#endif /* ADLC_ERROR */
  }

/************************************************************************/
/*			cfgNrErrorEn					*/
/************************************************************************/
static ST_VOID  cfgNrErrorEn (ST_VOID)
  {
  /* Found 'nr' flag                                                */
#if defined(ADLC_ERROR)
  adlcError.nr = SD_TRUE;
#endif /* ADLC_ERROR */
  }
/************************************************************************/
/*			cfgNsErrorEn					*/
/************************************************************************/
static ST_VOID  cfgNsErrorEn (ST_VOID)
  {
  /* Found 'ns' flag                                                */
#if defined(ADLC_ERROR)
  adlcError.ns = SD_TRUE;
#endif /* ADLC_ERROR */
  }
/************************************************************************/
/*			cfgLenErrorEn					*/
/************************************************************************/
static ST_VOID  cfgLenErrorEn (ST_VOID)
  {
  /* Found 'len' flag                                                */
#if defined(ADLC_ERROR)
  adlcError.len = SD_TRUE;
#endif /* ADLC_ERROR */
  }

#if defined(ADLC_LM)

/************************************************************************/
/*				_GetHex					*/
/************************************************************************/

static ST_VOID _GetHex (ST_CHAR *pString, ST_INT *pNumber)
  {
ST_INT number = 0;
ST_INT digit;


  while (*pString)
    {
    if (isdigit (*pString))
      digit = *pString - '0';
    else
      digit =  (toupper (*pString) - 'A') + 10;
      
    number = (16 * number) + digit;
    
    pString++;
    }

  *pNumber = number;
  }


#endif /* ADLC_LM */

#if !defined(ADLC_LM)

/************************************************************************/
/*		                  setBaud				*/
/************************************************************************/
static ST_VOID setBaud(ST_LONG lVal)
  {      
  switch (lVal)
    {
    case 2400 : 
      pstCurrCfgPort->baud = BAUD2400;
    break;		
    case 4800 : 
      pstCurrCfgPort->baud = BAUD4800;
    break;		
    case 9600 : 
      pstCurrCfgPort->baud = BAUD9600;
    break;		
      case 19200 :
	pstCurrCfgPort->baud = BAUD19200;
      break;		
    case 38400 : 
      pstCurrCfgPort->baud = BAUD38400;
    break;		
    case 56000 : 
      pstCurrCfgPort->baud = BAUD56K;
    break;		
    case 57400 : 
      pstCurrCfgPort->baud = BAUD57K;
    break;		
    case 115200 : 
      pstCurrCfgPort->baud = BAUD115K;
    break;		

    default :	
      cfg_set_config_err();
    break;
    }
  }	
		
/************************************************************************/
/*		      		setDataBits				*/
/************************************************************************/
static ST_VOID setDataBits(ST_INT16 sVal)
  {
  switch (sVal)
    {
    case 7 : 
      pstCurrCfgPort->data = DATA7;
    break;
    case 8 :
      pstCurrCfgPort->data = DATA8;
    break;
    default :
      cfg_set_config_err();
    break;
    }
  }

#endif /* !ADLC_LM */

#if defined(ADLC_MASTER)
/************************************************************************/
/*			verifyPollGroups				*/
/* ensures pollRate in ascending order					*/
/************************************************************************/

static ST_RET verifyPollGroups(ST_VOID)
  {
POLL_GROUP	*pstPollGrp;
ST_DOUBLE		dPrevRate = 0.0;
ST_INT		i = 0;
ST_INT		rtnCode = SD_SUCCESS;
  
  pstPollGrp = pollGroupList;
  while (pstPollGrp && i++ < MAX_POLL_GROUPS)
    { 
    if (pstPollGrp->pollRate >= dPrevRate)
      dPrevRate = pstPollGrp->pollRate;
    else
      {
      rtnCode = SD_FAILURE;
      break;
      }
    pstPollGrp = (POLL_GROUP *) list_get_next(pollGroupList, pstPollGrp);
    }

  return (rtnCode);
  }
#endif /* ADLC_MASTER */
/************************************************************************/
/*			initPort					*/
/* search the adlc port list  - if not found call addToPortList		*/
/************************************************************************/

static ADLC_PORT_INFO *initPort(ST_INT port)
  {
ADLC_PORT_INFO *pptr;

  pptr = portList;
  while (pptr)
    {
    if (pptr->port == port)
      return (pptr);
    pptr = (ADLC_PORT_INFO *) list_get_next(portList, pptr);
    }

  pptr = addToPortList(port,&adlcCfg);
  return (pptr);
  }

/************************************************************************/
/*			setCfgPort					*/
/* search the config port list and set up global pointers 		*/
/* used for lm	to set the communication parameter for the port		*/
/************************************************************************/

static ST_VOID setCfgPort(ST_INT port)
  {
CFG_PORT *pstCfgPort;

  pstCfgPort = findCfgPort(port);
  if (pstCfgPort == NULL)	/* not configured		*/
    {
    pstCfgPort = addToCfgPortList(port);

    /* load any defaults		*/
#if !defined(ADLC_LM)
    pstCfgPort->baud = BAUD4800;
    pstCfgPort->parity = NOPAR;
    pstCfgPort->data = DATA8;
    pstCfgPort->stop = STOP1;
    }
  pstCurrCfgPort = pstCfgPort;
#endif                       

#if defined(ADLC_LM)
    pstCfgPort->stLineParam.baudRate = 4800;
    pstCfgPort->stLineParam.parity = 0;
    pstCfgPort->stLineParam.dataBits = 8;
    pstCfgPort->stLineParam.stopBits = 0;
    }

  /* setup static globals		*/
  pstLineParam = &pstCfgPort->stLineParam;
  pstHandShake = &pstCfgPort->stHandShake;
  pstFlowCtrl  = &pstCfgPort->stFlowCtrl;
#endif                       
  }

/************************************************************************/
/*			       extractPortNumber			*/
/* will change COM1 to 0 COM2 to 1 COM3 to 2 etc			*/
/************************************************************************/
static ST_RET extractPortNumber(ST_CHAR *portName, ST_INT *portNumber)
  {
ST_CHAR *ptr;
  if (strnicmp(portName,"COM",3) != 0)
    return(SD_FAILURE);
    
  ptr = portName + 3;
  *portNumber = atoi(ptr) - 1;
  if (*portNumber < 0)
    return(SD_FAILURE);
    
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			getCfgPort					*/
/************************************************************************/
static ST_RET getCfgPort(ST_INT *port)
  {
ST_CHAR *portString;
  if ((portString = cfg_get_string_ptr()) == NULL)
    return (SD_FAILURE);

  if (extractPortNumber(portString,port) != SD_SUCCESS)
    return (SD_FAILURE);
  return (SD_SUCCESS);
  }
#if defined(CNP)
/*-----------------------------------------------------------------------
 *	Processing of keywords found in the sections:
 *		%CNPParmsBegin ... %CNPParmsEnd
 *----------------------------------------------------------------------*/

/************************************************************************/

/************************************************************************/
ST_VOID cnpSetDefaultValues (ST_VOID)
  {
ST_INT i;

  radio.port = COM5;	/* default */
  cnpCfg.signalTimeout = 30 * 1000;
  ied.numOfPorts = 7;

  for (i = 0; i < ied.numOfPorts; i++)
    {
    ied.portInfo[i].port = (radio.port + 1) + i;
    ied.portInfo[i].state = IED_SELECT_STATE;
    }
  } 
/************************************************************************/
/*			cfgCNPParmsBegin				*/
/************************************************************************/
ST_VOID  cfgCNPParmsBegin (ST_VOID)
  {
  if (cfg_tbl_push (kwtblCNPAttr) != SD_SUCCESS) 
      cfg_set_config_err ();
  ied.numOfPorts = 0;
  }

/************************************************************************/
/*			cfgRadioPort					*/
/* found 'RadioPort' keyword, get the value				*/
/************************************************************************/

ST_VOID  cfgRadioPort (ST_VOID)
  {
ST_INT	 port;

  if (getCfgPort(&port) != SD_SUCCESS)
    {
    cfg_set_config_err();
    return;
    }

  radio.port = port;
  }

/************************************************************************/
/*			cfgIedPort					*/
/* found 'IEDPort' keyword, get the value				*/
/************************************************************************/
ST_VOID  cfgIedPort (ST_VOID)
  {
ST_INT	 port;
ST_INT i;

  i = ied.numOfPorts;
  if (i >= MAX_PORTS)
    {
    cfg_set_config_err();
    return;
    }
  if (getCfgPort(&port) != SD_SUCCESS)
    {
    cfg_set_config_err();
    return;
    }

  ied.portInfo[i].port = port;
  ied.portInfo[i].state = IED_SELECT_STATE;
  i++;
  ied.numOfPorts = i;
  }
/************************************************************************/
/*			cfgCNPParmsEnd					*/
/************************************************************************/
ST_VOID  cfgCNPParmsEnd (ST_VOID)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();

  if (ied.numOfPorts == 0) /* user did not configure any ied port */
    ied.numOfPorts = 7;
  }
#endif /* CNP */
