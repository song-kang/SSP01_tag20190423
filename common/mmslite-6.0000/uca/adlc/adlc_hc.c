/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-1996, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_hc.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*  ADLC hard code module						*/
/*  Allows you to hard code the data needed to run ADLC			*/
/*  This module replaces adlc_cfg.c 					*/	
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/* adlcConfigure							*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/13/99  MDE    13     Added SD_CONST modifiers			*/
/* 10/08/98  MDE    12     Minor log cleanup				*/
/* 10/20/97  IKE    11	   Put ifdef around adlcSetDefaultSlogValues	*/
/* 06/15/97  IKE    10     Changed the DEFAULT_LOG_MASK			*/
/* 05/12/97  IKE    09     Changed to MMSEASE 7.0 Data Types		*/
/* 02/24/97  IKE    08	   Removed #ifdef PLAIN_DOS and used ADLC_LM	*/
/*			   define for ipc specific code			*/
/*			   CFG_PORT also for DOS so multiple ports can	*/
/*			   be initialized in dos platform		*/
/* 02/24/97  IKE    07     Changed logMask6 to use adlc_debug_sel	*/
/* 02/03/97  IKE    06     adlcCfg per port basis			*/
/* 12/03/96  IKE    05     WIN32 Support, header files cleanup		*/ 
/* 11/06/96  IKE    04     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/04/96  IKE    03     Changed DEBUG_MMS to DEBUG_SISCO		*/
/* 09/17/96  IKE    02     Removed slogCtrl allocation for new slog lib	*/
/* 04/11/96  IKE    01	   Created					*/
/************************************************************************/
/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

/* Some other SISCO files						*/
#include "cfg_util.h"
#include "slog.h"
#include "mem_chk.h"
#include "memlog.h"

/* ADLC files								*/
#include "adlc.h"
#include "adlc_log.h"

/************************************************************************/
/* Static variables							*/
/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;

/************************************************************************/
/*      Static Function Declarations                           		*/
/************************************************************************/
static ST_VOID adlcSetDefaultSlogValues (ST_VOID);
static ST_VOID adlcSetDefaultCfgValues (ADLC_CFG_INFO *pstAdlcCfg);
static ST_VOID adlcSetDefaultSmValues (ST_VOID);
static ST_VOID initPortsAndDevices(ST_VOID); 
#if defined(ADLC_MASTER)
static ST_VOID initPollGroups (ST_VOID);
#endif

#if defined(ADLC_LM)
/************************************************************************/

/************************************************************************/
ST_RET adlcReConfigure(ST_VOID)
  {
  return (SD_SUCCESS);
  }
#endif /* ADLC_LM */
/************************************************************************/
/*			adlcConfigure					*/
/* This is the global function in this module. It allows		*/
/* you to hard code the data needed to run ADLC				*/
/************************************************************************/
ST_RET adlcConfigure(ST_VOID)
  {
#if defined(DEBUG_SISCO)

/* If this is the application then we will set up SLOGGING	*/
#if defined(ADLC_LM) || defined(ADLC_TESTING)
  adlcSetDefaultSlogValues ();
#endif

/* Make sure errors are ennunciated                 */
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_ERR);

#endif /* DEBUG_SISCO */

/* Set the Serial Manager specific info 				*/
  adlcSetDefaultSmValues (); 

#if defined(ADLC_MASTER)
  initPollGroups();
#endif

  initPortsAndDevices();

  return (SD_SUCCESS);
  }		  					

#if defined(DEBUG_SISCO)
/************************************************************************/
/*			adlcSetDefaultSlogValues			*/
/************************************************************************/
static ST_VOID adlcSetDefaultSlogValues (ST_VOID)
  {
  ADLC_SET_CTRL (LOG_FILE_EN);		    /* Use File logging 	*/
  ADLC_SET_CTRL (LOG_TIME_EN); 		    /* Std Time/Date logging	*/

/* File Logging Control defaults 					*/
  sLogCtrl->fc.fileName = "adlc.log";
  sLogCtrl->fc.maxSize = 1000000L;

  ADLC_SET_FILE_CTRL (FIL_CTRL_WIPE_EN | FIL_CTRL_WRAP_EN | FIL_CTRL_MSG_HDR_EN);

  /* Default Logging Mask */
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_ERR);

/* We can set different log masks for debugging purposes Following is one example  */
/* Please refer to documentation for all the available log masks		   */	
/*  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_ERR | ADLC_MASK_LOG_NERR | ADLC_MASK_LOG_IO); */

  }
#endif /* DEBUG_SISCO */
/************************************************************************/
/*			adlcSetDefaultCfgValues				*/
/************************************************************************/
static ST_VOID adlcSetDefaultCfgValues (ADLC_CFG_INFO *pstAdlcCfg)
  {
  pstAdlcCfg->modulus = 8;
  pstAdlcCfg->frameLen = 256;
  pstAdlcCfg->maxAdlcReqs = MAX_ADLC_REQS;
  pstAdlcCfg->communicationMedia = RADIO_MEDIA;
  pstAdlcCfg->retries = 3;
  pstAdlcCfg->sof = 0x7E;
  pstAdlcCfg->eof = 0x0D;
  pstAdlcCfg->responseTimeout = 2.0 * 1000;
  pstAdlcCfg->frameTimeout = 5.0 * 1000;
  pstAdlcCfg->signalTimeout = 5.0 * 1000;
  pstAdlcCfg->radioDekeyDelay = 10.0;
  pstAdlcCfg->radioStartOfTxDelay = 10.0;
  pstAdlcCfg->preambleLen = 0;
#if defined(ADLC_MASTER)
  pstAdlcCfg->runMode = ENHANCED_POLLED_MODE;
  pstAdlcCfg->maxConnectRetryTimeout = 60.0 * 30 * 1000; /* half an hour */
  pstAdlcCfg->deviceTurnaroundTime = 50.0;
  pstAdlcCfg->portTurnaroundTime = 0;
  pstAdlcCfg->portUsage = 80;
#endif
#if defined(ADLC_SLAVE)
  pstAdlcCfg->runMode = POLLED_MODE;
#endif
  }
/************************************************************************/
/*			   adlcSetDefaultSmValues			*/
/* The following function configures COM2 for serial transmission and	*/
/* sets the communication parameters.					*/
/************************************************************************/

static ST_VOID adlcSetDefaultSmValues (ST_VOID)
  {
CFG_PORT *pstCfgPort;

  pstCfgPort = addToCfgPortList(COM2);

#if !defined(ADLC_LM)
  pstCfgPort->baud = BAUD4800;
  pstCfgPort->parity = NOPAR;
  pstCfgPort->data = DATA8;
  pstCfgPort->stop = STOP1;
#endif                       

#if defined(ADLC_LM)
  /* load any defaults		*/
  pstCfgPort->stLineParam.baudRate = 4800;
  pstCfgPort->stLineParam.parity = 0;
  pstCfgPort->stLineParam.dataBits = 8;
  pstCfgPort->stLineParam.stopBits = 0;
#endif /* ADLC_LM */
  }

#if defined(ADLC_MASTER)
/************************************************************************/
/*			initPollGroups					*/
/* This function initializes the global variable 'pollGroupList'	*/
/* The first argument to the  'addToPollGroupList' is the Poll Group ID	*/
/*  and the second argument is the poll rate. 				*/
/* So in the following example we create one poll group.		*/
/* Devices belonging to that poll group will be polled every second	*/
/************************************************************************/
static ST_VOID initPollGroups(ST_VOID)
  {
POLL_GROUP *pstPollGroup;
  pstPollGroup = addToPollGroupList(1,1);
  }
#endif
/************************************************************************/
/*			initPortsAndDevices				*/
/* This function initializes the global variable 'portList'		*/
/* The following code initializes one port and then it will initialize 	*/
/* three adlc devices named "dev1", "dev2" and "dev3" with addresses 	*/
/* 1964,1967 and 1971 respectively. 					*/
/* The last argument to function 'addToDevList' is poll group.		*/
/************************************************************************/
static ST_VOID initPortsAndDevices(ST_VOID)
  {
ADLC_PORT_INFO *pptr;
ADLC_DEV_INFO *dptr;
ADLC_CFG_INFO adlcCfg;

/* Set the ADLC specific configuration info for one port		*/
  adlcSetDefaultCfgValues (&adlcCfg);

/* configure one port, the first argument should be same as used	*/
/* in adlcSetDefaultSmValues						*/
  pptr = addToPortList(COM2,&adlcCfg); 

/* Configure three devices for this port				*/
  dptr = addToDevList(pptr,"dev1",1964,1,0);
  dptr = addToDevList(pptr,"dev2",1967,1,0);
  dptr = addToDevList(pptr,"dev3",1971,1,0);
  }
