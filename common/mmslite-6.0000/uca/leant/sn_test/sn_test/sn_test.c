/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998-2008, All Rights Reserved.					*/
/*									*/
/* MODULE NAME : sn_test.c						*/
/* PRODUCT(S)  : MMS-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/28/12  JRB	   Print cr_sn_test at startup.			*/
/* 09/13/10  JRB	   Add cr_sn_test copyright string.		*/
/* 08/01/08  JRB    11     Clarify "Usage" message.			*/
/* 07/23/08  GLB    10     Update Copyright.                            */
/* 05/08/08  JRB    09     Add network_device cmd line arg for _WIN32.	*/
/*			   Chk return from clnp_snet_init.		*/
/*			   Chg main return from void to "int".		*/
/* 01/22/08  JRB    08     Use ascii_to_hex_str to set remMac & delete	*/
/*			   getMacAddress.				*/
/* 02/14/07  GLB    07     Added welcome() to print Copyright info      */
/* 06/02/06  JRB    06     Use new clnp_snet_write_raw funct.		*/
/*			   Make static clnp_snet_write. Why? See fct hdr*/
/* 12/11/03  JRB    05     Don't use _itoa, use sprintf instead.	*/
/* 06/18/01  JRB    04	   Added _clnp_err_logstr, etc. so don't have	*/
/*			   to link to clnp_usr.obj.			*/
/* 04/06/00  JRB    03     Use new subnet function "clnp_snet_free"	*/
/*			   instead of "free_lpdu" flag. More flexible.	*/
/* 10/29/98  RWM    02      Fixed handling of sequence numbers within   */
/*                          the pdu so it is processor independant      */
/* 10/12/98  RWM    01      Added include for windows.h                 */
/* 05/06/98  RWM            Created                                     */
/************************************************************************/
#include "glbtypes.h"
#include "clnp_log.h"	/* temporary for debug */
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "fkeydefs.h"   /* Function key handling */
#include "scrndefs.h"
#include "mem_chk.h"
#include "gen_list.h"
#include "stime.h"
#include "str_util.h"
#include <signal.h>
#if defined (_WIN32)
#include <windows.h>
#endif

const ST_CHAR cr_sn_test[] =
 "(c) COPYRIGHT SYSTEMS INTEGRATION SPECIALISTS COMPANY INC., 1998 - 2010.  All Rights Reserved.";

/************************************************************************/
/* Local Defines */

static ST_VOID welcome (ST_VOID);

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;	/* Define for SLOG mechanism	*/

#define ETHE_MAC_LEN  6

/* MAX TESTS must be at least 2 */
#define MAX_TESTS     100

/* this is the length of a pdu header */
#define HEADER_LENGTH 4

/* this is the length of a pdu header along with the control block */
#define HDR_CTRL_BLOCK_LENGTH 9

/* this is the maximum buffer allowed for entering in a valid mac address */
#define MAC_BUFFER_LENGTH     25

/* the following two defines determine whether a test will be sent to */
/* ALL ES or a remote MAC address */
#define DEST_ALL_ES  0
#define DEST_REM_MAC 1

/* the following two defines determine whether a test will use CLNP headers */
/* within the pdus or will use SMP headers */
#define CLNP_HEADER_TYPE 0
#define SMP_HEADER_TYPE  1

/* the following three defines are codes used within a pdu control block */
/* they indicate whether a pdu will start or stop a test or whether the */
/* pdu is a test pdu */
#define TEST_PDU     0
#define START_PDU    1
#define STOP_PDU     2

/* timout waiting for a response to a pdu */
#define MAX_TIMEOUT       10
/* time alloted to let the server initialize after sending a start pdu */
#define MAX_INIT_TIMOUT   2
/* High Performance test finish timeout */
#define HP_TEST_FINISH_TO 2

/* the length of the all es pdu that is used */
#define ALL_ES_PDU_LENGTH 82

/* slog defines */
#define DRV_TEST_TYPE 1667

#define DRVT_LOG0(a) {\
                     if (loggingOn)\
                       {\
                       slog (sLogCtrl,DRV_TEST_TYPE,\
                             thisFileName,__LINE__,a);\
                       }\
                     }

#define DRVT_LOG1(a,b) {\
                       if (loggingOn)\
                         {\
                         slog (sLogCtrl,DRV_TEST_TYPE,\
                               thisFileName,__LINE__,a,b);\
                         }\
                       }
                       
#define DRVT_LOG2(a,b,c) {\
                         if (loggingOn)\
                           {\
                           slog (sLogCtrl,DRV_TEST_TYPE,\
                                 thisFileName,__LINE__,a,b,c);\
                           }\
                         }
                        
#define DRVT_CLOG0(a) {\
                      if (loggingOn)\
                        {\
                        slog (sLogCtrl,SLOG_CONT,\
                             thisFileName,__LINE__,a);\
                        }\
                      }

#define DRVT_CLOG1(a,b) {\
                        if (loggingOn)\
                          {\
                          slog (sLogCtrl,SLOG_CONT,\
                               thisFileName,__LINE__,a,b);\
                          }\
                        }

#define DRVT_CLOG2(a,b,c) {\
                          if (loggingOn)\
                            {\
                            slog (sLogCtrl,SLOG_CONT,\
                                 thisFileName,__LINE__,a,b,c);\
                            }\
                          }
                        
#define DRVT_HEX_LOG(a,b) {\
                          if (loggingOn)\
                            {\
                            slogHex (sLogCtrl,SLOG_CONT,\
                                     thisFileName,__LINE__,a,b);\
                            }\
                          }

/* Local Structures */
typedef struct snReqItem
{
DBL_LNK     l;
SN_UNITDATA snReq;
} SN_REQ_ITEM;
                         
/************************************************************************/
/* Global Variables */

ST_BOOLEAN doIt; /* variable that indicates that the application should */
                 /* continue to looping and accepting user input */

ST_BOOLEAN loggingOn; /* variable the indicates whether logging is enabled */

HANDLE kbdEvent; /* keyboard event handle */

CLNP_PARAM clnp_param; /* global variable that is required for linking */

/* CLNP is using clnp_debug_sel global variable to turn on/off different*/
/* types of logging.							*/
/* if 0 nothing will be logged		*/
ST_UINT clnp_debug_sel;

/* boolean flag to indicate whether the high perf test should continue */
ST_BOOLEAN  contTesting;

ST_UCHAR localMac [ETHE_MAC_LEN]; /* used to store local mac */
ST_UCHAR allEsMac [ETHE_MAC_LEN]; /* used to store ALL ES mac */
ST_UCHAR remMac   [ETHE_MAC_LEN]; /* used to store server mac */

/* used to store the maximum length of a pdu */
ST_UINT16 maxPduLen;

/* array of test sn requests */
SN_UNITDATA sn_reqs [MAX_TESTS];

ST_UCHAR CLNP_PDU_HEADER[] = {0xFE, 0xFE, 0x03, 0x8F}; /* CLNP pdu header */
ST_UCHAR SMP_PDU_HEADER[] = {0xFB, 0xFB, 0x03, 0x8F}; /* SMP pdu header */

/* this is the hardcoded all es pdu that will be used for tests */
ST_UCHAR ALL_ES_PDU[ALL_ES_PDU_LENGTH] 
  = {0xFE, 0xFE, 0x03, 0x81, 0x33, 0x01, 0x32, 0x1C,
     0x00, 0x4F, 0x00, 0x00, 0x14, 0x39, 0x84, 0x0F,
     0x80, 0x11, 0x38, 0x06, 0x00, 0x00, 0x00, 0x01,
     0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
     0x01, 0x14, 0x39, 0x84, 0x0F, 0x80, 0x11, 0x38,
     0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x1B, 0xE2,
     0x00, 0x00, 0x00, 0x01, 0x40, 0xC1, 0x02, 0x00,
     0x01, 0xC2, 0x02, 0x00, 0x01, 0xC0, 0x01, 0x07,
     0xC4, 0x01, 0x01, 0xC6, 0x01, 0x00, 0xC3, 0x02,
     0xBA, 0x5E};

SN_UNITDATA allEsReq;
/************************************************************************/
/* Function prototypes */

ST_VOID printUsage ();
ST_VOID doMainMenu ();

ST_BOOLEAN strget (ST_CHAR *str);

ST_BOOLEAN getMacAddress ();

ST_VOID nofun ();
ST_VOID (*menu_set_fun)() = nofun;

ST_VOID doSnglClnpTestDirect (ST_VOID);
ST_VOID doSnglClnpTestAllEs (ST_VOID);
ST_VOID doSnglSmpTestDirect (ST_VOID);
ST_VOID doClnpTestDirect (ST_VOID);
ST_VOID doSmpTestDirect (ST_VOID);
ST_VOID doPduSetTest (ST_INT testType, ST_INT headerType, ST_INT numTests);
ST_VOID doHighPerfPduSetTest (ST_VOID);
ST_INT fillPipeline (SN_REQ_ITEM **snReqList, ST_INT numOut, ST_INT32 *sqncNum);
ST_VOID ctrlCfun (ST_INT i);
ST_VOID initializeTestSet (ST_INT dest, ST_INT headerType);
ST_CHAR *convertMacToString (ST_UCHAR *mac);
ST_VOID wait_any_event (ST_LONG max_ms_delay);
ST_RET 	m_set_log_config ();


/* external functions */
ST_RET term_init (ST_VOID);
ST_BOOLEAN ask (ST_CHAR *question, ST_BOOLEAN default_ans);

#ifdef DEBUG_SISCO
/* NOTE: these variables are usually in clnp_usr.c, but we don't want	*/
/* to include extra modules, so we copied them here.			*/
SD_CONST ST_CHAR *SD_CONST _clnp_err_logstr = "CLNP_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _clnp_nerr_logstr = "CLNP_LOG_NERR";
SD_CONST ST_CHAR *SD_CONST _clnp_req_logstr = "CLNP_LOG_REQ";
SD_CONST ST_CHAR *SD_CONST _clnp_ind_logstr = "CLNP_LOG_IND";
SD_CONST ST_CHAR *SD_CONST _clnp_enc_dec_logstr = "CLNP_LOG_ENC_DEC";
SD_CONST ST_CHAR *SD_CONST _clnp_llc_enc_dec_logstr = "CLNP_LOG_LLC_ENC_DEC";
SD_CONST ST_CHAR *SD_CONST _clnp_always_logstr = "CLNP_LOG_ALWAYS";
SD_CONST ST_CHAR *SD_CONST _clsns_req_logstr = "CLSNS_LOG_REQ";
SD_CONST ST_CHAR *SD_CONST _clsns_ind_logstr = "CLSNS_LOG_IND";
#endif

/************************************************************************/

/************************************************************************/
/*                                nofun                                 */
/* This function does effectively nothing                               */
/************************************************************************/

ST_VOID nofun ()
  {

  }

/************************************************************************/
/*			clnp_snet_write					*/
/* Replacement for obsolete function.					*/
/* Library function clnp_snet_write was replaced with clnp_snet_write_raw,*/
/* but tricky to replace everywhere in this module, so this static	*/
/* function was added.							*/
/************************************************************************/
ST_RET clnp_snet_write (SN_UNITDATA *snReq)
  {
static ST_UCHAR raw_packet [ETHE_MAX_LEN_LSDU];	/* raw packet buffer	*/

  memcpy (raw_packet, snReq->rem_mac, CLNP_MAX_LEN_MAC);/* DST MAC*/
  memcpy (&raw_packet[6], snReq->loc_mac, CLNP_MAX_LEN_MAC);/* SRC MAC*/
  /* Set the 802.3 length/type field (bytes 12 and 13).	*/
  raw_packet[12] = (ST_UCHAR) (snReq->lpdu_len >> 8);	
  raw_packet[13] = (ST_UCHAR) (snReq->lpdu_len & 0xff);
  memcpy (&raw_packet[14], snReq->lpdu, snReq->lpdu_len);

  return (clnp_snet_write_raw (raw_packet, snReq->lpdu_len+14));
  }

/************************************************************************/
/*                                main                                  */
/* This function is the main entry point into this appliaction. Its     */
/* purpose is to intialize the subnetwork functionality, prompt the     */
/* user for the server's mac address, prompt the user for logging       */
/* functionality, print a menu of all the available functionality, and  */
/* accept input from the user in order to trigger tests                 */
/************************************************************************/

int main (int argc, char *argv[])
  {
ST_INT i;
ST_BOOLEAN macRetrieved;
ST_UINT remMacLen;	/* len of remote MAC on command line	*/

  welcome ();

  /* intialize the terminal */
  term_init();
  
#if defined(_WIN32)
  /* On Windows, 2 arguments (MAC & network_device) required.	*/
  /* NOTE: If 2nd argument useful on other platform, add to	*/
  /*       "#if defined..." above.				*/
  if (argc >= 3)
    {
    /* Use 1st argument to set "remMac" global variable.	*/
    if (ascii_to_hex_str (remMac, &remMacLen, 6, argv[1]) == SD_SUCCESS
        && remMacLen == 6)
      macRetrieved = SD_TRUE;
    /* Use 2nd argument to set global clnp_param.network_device.*/
    clnp_param.network_device = argv[2];
    }
#else
  /* On other platforms, only 1 argument (MAC) required.	*/
  if (argc >= 2)
    {
    /* Use 1st argument to set "remMac" global variable.	*/
    if (ascii_to_hex_str (remMac, &remMacLen, 6, argv[1]) == SD_SUCCESS
        && remMacLen == 6)
      macRetrieved = SD_TRUE;
    }
#endif
  else
    {	/* missing command line agument	*/
    printUsage();
    return (1);
    }
  
  /* check to see if the mac address was retrieved from the command line */
  /* if it wasn't return from the function */
  if (macRetrieved == SD_FALSE)
    {
    printUsage();
    return (2);
    }
  
  /* initialize logging parameters */
  m_set_log_config ();
  
  /* initialize the interface between clnp and the subnetwork */
  if (clnp_snet_init (&clnp_param))
    {
    printf ("\n clnp_snet_init failed. Check parameters.");
    return (3);
    }
  
  /* get my local mac address and save it off for later use */
  clnp_snet_get_local_mac (localMac);
  
  /* get the end system mac address and save it off for later use */
  clnp_snet_get_all_es_mac (allEsMac);
  
  /* get the maximum pdu allowed */
  maxPduLen = clnp_snet_get_max_udata_len ();

  /* ask the user if they would like logging turned on */
  loggingOn = ask (" Would you like to log while testing (Y) ? ", SD_TRUE);
  
  /* print the main menu */
  doMainMenu ();
  
  doIt = SD_TRUE;
  
  /* keep looping to get the users input until */
  /* F10 is hit */
  while (doIt == SD_TRUE)
    {
    wait_any_event (100);

    if (check_key () == 1)
      break;
    }
  
  /* free any of the pdus */
  for (i = 0; i < MAX_TESTS; i++)
    {
    if (sn_reqs[i].lpdu != NULL)
      chk_free (sn_reqs[i].lpdu);
    }
  
  /* terminate the session */
  clnp_snet_term ();
  return (0);
  }

/************************************************************************/
/*                              printUsage                              */
/* This function will print the usage of this application to the user   */ 
/************************************************************************/
  
ST_VOID printUsage ()
  {
  printf ("\n MMS CLNP DRIVER TEST");
#if defined(_WIN32)
  printf ("\n\n Usage : sn_test \"mac address\" network_device");
  printf ("\n   mac address    = MAC address (in hex) of REMOTE machine running 'sn_targt'");
  printf ("\n   network_device = WinPcap network device as configured in osicfg.xml");
  printf ("\n   Example : sn_test \"00 60 08 06 AC 27\" \\Device\\NPF_{66511A69-95AF-48A6-8E1C-457F9522CE2A}");
#else
  printf ("\n\n Usage : sn_test \"mac address\"");
  printf ("\n   mac address    = MAC address (in hex) of REMOTE machine running 'sn_targt'");
  printf ("\n   Example : sn_test \"00 60 08 06 AC 27\"");
#endif
  wait_msg ("");
  }
  
/************************************************************************/
/*                              doMainMenu                              */
/* This function will print the main menu of the application            */ 
/************************************************************************/

ST_VOID doMainMenu ()
  {
  CLEARSCR;
  printf ("\n\n");
  printf ("\n**********************************************************");
  printf ("\n***                  MMS CLNP DRIVER TEST              ***");
  printf ("\n**********************************************************");
  printf ("\n\t          SELECT DESIRED ACTIVITY\n\n");
  printf ("\t  F1  - TEST SINGLE CLNP HEADER DIRECT PDU\n");
  printf ("\t  F2  - TEST SINGLE CLNP HEADER ALL ES PDU\n");
  printf ("\t  F3  - TEST SINGLE SMP HEADER DIRECT PDU\n");
  printf ("\t  F4  - TEST CLNP HEADER DIRECT PDU SET\n");
  printf ("\t  F5  - TEST SMP HEADER DIRECT PDU SET\n");
  printf ("\t  F6  - HIGH PERFORMANCE TEST\n");
  printf ("\t  F10 - EXIT DRIVER TEST\n");
  
  flush_keys ();
  fun_null ();
  
  funct_1 = doSnglClnpTestDirect;
  funct_2 = doSnglClnpTestAllEs;
  funct_3 = doSnglSmpTestDirect;
  funct_4 = doClnpTestDirect;
  funct_5 = doSmpTestDirect;
  funct_6 = doHighPerfPduSetTest;
  
  menu_set_fun = doMainMenu;     /* used to reset the menu	*/
  }

/************************************************************************/
/*                         doSnglClnpTestDirect                         */
/* This function will call doPduSetTest sending 1 pdu directly to the   */
/* server with a CLNP header in the pdu                                 */
/************************************************************************/

ST_VOID doSnglClnpTestDirect (ST_VOID)
  {
  doPduSetTest (DEST_REM_MAC, CLNP_HEADER_TYPE, 1);
  }
  
/************************************************************************/
/*                          doSnglClnpTestAllEs                         */
/* This function willl call doPduSetTest sending 1 pdu to all es using  */
/* a CLNP header within the pdu                                         */
/************************************************************************/

ST_VOID doSnglClnpTestAllEs (ST_VOID)
  {
  doPduSetTest (DEST_ALL_ES, CLNP_HEADER_TYPE, 1);
  }

/************************************************************************/
/*                          doSnglSmpTestDirect                         */
/* This function will call doPduSetTest sending 1 pdu directly to the   */
/* server with a SMP header in the the pdu                              */
/************************************************************************/

ST_VOID doSnglSmpTestDirect (ST_VOID)
  {
  doPduSetTest (DEST_REM_MAC, SMP_HEADER_TYPE, 1);
  }

/************************************************************************/
/*                           doClnpTestDirect                           */
/* This function will call doPduSetTest sending pdus directly to the    */
/* server with CLNP headers in the pdus                                 */
/************************************************************************/

ST_VOID doClnpTestDirect (ST_VOID)
  {
  doPduSetTest (DEST_REM_MAC, CLNP_HEADER_TYPE, MAX_TESTS);
  }

/************************************************************************/
/*                           doSmpTestDirect                            */
/* This function will call doPduSetTest sending pdus directly to the    */
/* server with SMP headers in the pdus                                  */
/************************************************************************/

ST_VOID doSmpTestDirect (ST_VOID)
  {
  doPduSetTest (DEST_REM_MAC, SMP_HEADER_TYPE, MAX_TESTS);
  }

/************************************************************************/
/*                             doPduSetTest                             */
/* This function accepts 3 arguments. These three are the test type     */
/* (whether the destination is all es or direct), the header type (CLNP */
/* or SMP), and the number of pdus to be sent out in the test           */
/* This function will send out the requested pdus and will wait for     */
/* MAX_TIMEOUT to receive it back                                       */
/************************************************************************/

ST_VOID doPduSetTest (ST_INT testType, ST_INT headerType, ST_INT numTests)
  {
time_t      timeStart;
time_t      timeCurrent;
time_t      timeElapsed;
ST_RET      result;
ST_INT32    i;
ST_INT      numSuccessSnd;
ST_INT      numFailureSnd;
ST_INT      numSuccessRcv;
ST_INT      numTimeoutRcv;
ST_INT      numInvLngthRcv;
ST_INT      numInvPduCntsRcv;
ST_BOOLEAN  pduReceived;
ST_UCHAR    *sqncPtr;
ST_INT32    sqncNum;
SN_UNITDATA snReq;
SN_UNITDATA *snReqSent;
  
  /* initialize statistics for test */
  numSuccessSnd = 0; /* number of pdus successfully sent */
  numFailureSnd = 0; /* number of pdus failed to be sent */
  numSuccessRcv = 0; /* number of pdus successfully received */
  numTimeoutRcv = 0; /* number of pdus timed out on waiting to be received */
  numInvLngthRcv = 0; /* number of pdus received with invalid lengths */
  numInvPduCntsRcv = 0; /* number of pdus received with invalid contents */
  
  /* initialize the test pdus to be sent and received */
  initializeTestSet (testType, headerType);
  
  /* create a start pdu to send out to the server */
  snReq.lpdu = chk_calloc (1, HDR_CTRL_BLOCK_LENGTH);
  snReq.lpdu_len = HDR_CTRL_BLOCK_LENGTH;
  memcpy (snReq.rem_mac, remMac, ETHE_MAC_LEN);
  memcpy (snReq.loc_mac, localMac, ETHE_MAC_LEN);
  memcpy (snReq.lpdu, CLNP_PDU_HEADER, HEADER_LENGTH);
  snReq.lpdu[HEADER_LENGTH + sizeof (ST_LONG)] = START_PDU;
  
  /* send out the start pdu and wait up to MAX_INIT_TIMOUT seconds for the */
  /* server to initialize */
  result = clnp_snet_write (&snReq);

  /* log the start pdu */
  DRVT_LOG1  ("Local Mac: %s",
              convertMacToString (snReq.loc_mac));
  DRVT_CLOG1 ("Remote Mac: %s",
              convertMacToString (snReq.rem_mac));
  DRVT_HEX_LOG (snReq.lpdu_len, snReq.lpdu);
  
  /* free the pdu */
  chk_free (snReq.lpdu);
  
  /* check to see if the write was a success */
  if (result == SD_SUCCESS)
    {
    printf ("\n Start Test PDU sent successfully\n");
    DRVT_CLOG0 ("Start Test PDU sent successfully");
    }
  else
    {
    printf ("\n Failure on sending Start Test PDU; Retcode = %d\n", result);
    DRVT_CLOG1 ("Failure on sending Start Test PDU; Retcode = %d", result);
    return;
    }

  /* wait up to MAX_INIT_TIMOUT seconds */
  timeElapsed = 0;
  time (&timeCurrent);
  timeStart = timeCurrent;
 
  /* loop until timeElapsed >= MAX_INIT_TIMEOUT */
  while (timeElapsed < MAX_INIT_TIMOUT)
    {
    time (&timeCurrent);
    timeElapsed = timeCurrent - timeStart;
    }

  /* check the test type */
  if (testType == DEST_ALL_ES)
    {
    /* test type is ALL ES */
    if (headerType == CLNP_HEADER_TYPE)
      {
      /* header type is CLNP */
      printf ("\n Performing Test : CLNP HEADER ALL ES PDU SET...\n");
      DRVT_LOG0 ("Performing Test : CLNP HEADER ALL ES PDU SET...");
      }
    else
      {
      /* header type is SMP */
      printf ("\n Performing Test : SMP HEADER ALL ES PDU SET...\n");
      DRVT_LOG0 ("Performing Test : SMP HEADER ALL ES PDU SET...");
      }
    }
  else
    {
    /* test type is direct */
    if (headerType == CLNP_HEADER_TYPE)
      {
      /* header type is CLNP */
      printf ("\n Performing Test : CLNP HEADER DIRECT PDU SET...\n");
      DRVT_LOG0 ("Performing Test : CLNP HEADER DIRECT PDU SET...");
      }
    else
      {
      /* header type is SMP */
      printf ("\n Performing Test : SMP HEADER DIRECT PDU SET...\n");
      DRVT_LOG0 ("Performing Test : SMP HEADER DIRECT PDU SET...");
      }
    }
  
  /* send out the pdu's and wait to receieve them in order */
  for (i = 0; i < numTests; i++)
    {
    /* initialize time elapsed to zero */
    timeElapsed = 0;
    
    /* set the pdu received flag to false */
    pduReceived = SD_FALSE;
    
    /* check to see which pdu to send */
    if (testType == DEST_ALL_ES)
      snReqSent = &allEsReq;
    else
      snReqSent = &sn_reqs[i];
      
    /* send out the pdu */
    result = clnp_snet_write (snReqSent);
    
    /* log the pdu */
    DRVT_LOG1  ("Local Mac: %s",
                convertMacToString (snReqSent->loc_mac));
    DRVT_CLOG1 ("Remote Mac: %s",
                convertMacToString (snReqSent->rem_mac));
    DRVT_HEX_LOG (snReqSent->lpdu_len, snReqSent->lpdu);

    /* check to see if the write was a success */
    if (result == SD_SUCCESS)
      {
      /* log the success of sending the pdu */
      DRVT_CLOG0 ("PDU sent successfully");
      numSuccessSnd++;
      }
    else
      {
      /* log the failure of sending the pdu */
      DRVT_CLOG1 ("Failure on sending PDU; Retcode = %d", result);
      numFailureSnd++;
      break;
      }
      
    /* get the time that the pdu was sent out */
    time (&timeStart);
    
    /* loop until timeElapsed >= MAX_TIMEOUT */
    while (timeElapsed < MAX_TIMEOUT)
      {
      /* read for the pdu that was passed out */
      result = clnp_snet_read (&snReq);
      
      /* check to see if we successfully received a pdu */
      if (result == SD_SUCCESS)
        {
        /* log the pdu received */
        DRVT_LOG1  ("Local Mac: %s",
                    convertMacToString (snReq.loc_mac));
        DRVT_CLOG1 ("Remote Mac: %s",
                    convertMacToString (snReq.rem_mac));
        DRVT_HEX_LOG (snReq.lpdu_len, snReq.lpdu);
        
        /* check to see if the remote mac address is in the sn req */
        if (memcmp (snReq.rem_mac, remMac, ETHE_MAC_LEN) == 0)
          {
          /* a pdu was receieved, now check to see if it is the same */
          /* one that was just sent out on the wire to the server */
          
          if (testType != DEST_ALL_ES)
            {
            /* get the sequence number */
            sqncPtr = (ST_UCHAR *) &snReq.lpdu[HEADER_LENGTH];
            
            sqncNum = (((ST_UCHAR) (sqncPtr[0] << 24)) |
                       ((ST_UCHAR) (sqncPtr[1] << 16)) |
                       ((ST_UCHAR) (sqncPtr[2] << 8))  |
                       ((ST_UCHAR) (sqncPtr[3])));
            
            /* compare the sequence numbers */
            if (i != sqncNum)
              {
              /* log the error in sequence numbers */
              DRVT_CLOG0  ("Error in Sequence Numbers!");
  
              /* log the expected and received sequence numbers */  
              DRVT_CLOG2 ("Expected Sequence Number : %d, Received Sequence Number : %d",
                          i, sqncNum);
              }
            else
              {
              /* log the sequence number */
              DRVT_CLOG1 ("Sequence Number : %d", i);

              /* check the size of both pdus first*/
              if (snReqSent->lpdu_len == snReq.lpdu_len)
                {
                /* sizes are the same, now lets check the pdu's themselves */
                if (memcmp(snReqSent->lpdu, snReq.lpdu, snReq.lpdu_len) == 0)
                  {
                  /* This is the pdu that we sent out! Hooray! */
                  DRVT_CLOG0("Pdu received correctly");
                  numSuccessRcv++;
                  }
                else
                  {
                  /* This is not the pdu that we sent out. */
                  DRVT_CLOG0("Pdu received incorrectly; Invalid pdu contents");
                  numInvPduCntsRcv++;
                  }
                }
              else
                {
                /* This is not the pdu that we sent out. */
                DRVT_CLOG0("Pdu received incorrectly; Invalid length");
                numInvLngthRcv++;
                }
              }
            }
          else
            {
            /* check the size of both pdus first*/
            if (snReqSent->lpdu_len == snReq.lpdu_len)
              {
              /* sizes are the same, now lets check the pdu's themselves */
              if (memcmp(snReqSent->lpdu, snReq.lpdu, snReq.lpdu_len) == 0)
                {
                /* This is the pdu that we sent out! Hooray! */
                DRVT_CLOG0("Pdu received correctly");
                numSuccessRcv++;
                }
              else
                {
                /* This is not the pdu that we sent out. */
                DRVT_CLOG0("Pdu received incorrectly; Invalid pdu contents");
                numInvPduCntsRcv++;
                }
              }
            else
              {
              /* This is not the pdu that we sent out. */
              DRVT_CLOG0("Pdu received incorrectly; Invalid length");
              numInvLngthRcv++;
              }
            }
          /* set the pdu received flag to true */    
          pduReceived = SD_TRUE;
          break;
          }
        else
          {
          /* log that the pdu was received from a mac other than the server */
          DRVT_CLOG0 ("Pdu received from MAC other than server");
          }

        /* we are done with the processing, free anything		*/
        /* allocated by sub-network					*/
        clnp_snet_free (&snReq);
        }
      
      /* get the current time */
      time (&timeCurrent);
      
      /* derive the time elapsed between the current time and the time */
      /* that the pdu was sent out on the wire */
      timeElapsed = timeCurrent - timeStart;
      }
    
    /* check to see if we timed out waiting for the pdu to */  
    /* be returned */
    if (pduReceived != SD_TRUE)
      {
      /* the timeout elapsed so log an error */
      DRVT_LOG1 ("Local Mac: %s",
                  convertMacToString (snReqSent->loc_mac));
      DRVT_CLOG1 ("Remote Mac: %s",
                  convertMacToString (snReqSent->rem_mac));
      DRVT_HEX_LOG (snReqSent->lpdu_len, snReqSent->lpdu);
      DRVT_CLOG0 ("Timeout waiting for echo from server");
      numTimeoutRcv++;
      }

    /* break out if any failures occur */
    if (numFailureSnd > 0)
      break;
    
    if (numTimeoutRcv > 0)  
      break;
    
    if (numTimeoutRcv > 0)
      break;
        
    if (numInvLngthRcv > 0)
      break;
    
    if (numInvPduCntsRcv > 0)
      break;
    }

  /* create a stop pdu to send out to the server */
  snReq.lpdu = chk_calloc (1, HDR_CTRL_BLOCK_LENGTH);
  snReq.lpdu_len = HDR_CTRL_BLOCK_LENGTH;
  memcpy (snReq.rem_mac, remMac, ETHE_MAC_LEN);
  memcpy (snReq.loc_mac, localMac, ETHE_MAC_LEN);
  memcpy (snReq.lpdu, CLNP_PDU_HEADER, HEADER_LENGTH);
  snReq.lpdu[HEADER_LENGTH + sizeof (ST_LONG)] = STOP_PDU;
  
  /* send out the start pdu and wait up to 2 seconds for the */
  /* server to initialize */
  result = clnp_snet_write (&snReq);
  
  /* log the stop pdu */
  DRVT_LOG1  ("Local Mac: %s",
              convertMacToString (snReq.loc_mac));
  DRVT_CLOG1 ("Remote Mac: %s",
              convertMacToString (snReq.rem_mac));
  DRVT_HEX_LOG (snReq.lpdu_len, snReq.lpdu);
  
  /* free the pdu */
  chk_free (snReq.lpdu);
    
  /* check to see if the write was a success */
  if (result == SD_SUCCESS)
    {
    printf ("\n Stop Test PDU sent successfully\n");
    DRVT_CLOG0 ("Stop Test PDU sent successfully");
    }
  else
    {
    printf ("\n Failure on sending Stop Test PDU; Retcode = %d\n", result);
    DRVT_CLOG1 ("Failure on sending Stop Test PDU; Retcode = %d", result);
    }
    
  /* print the results */
  if (testType == DEST_ALL_ES)
    {
    if (headerType == CLNP_HEADER_TYPE)
      {
      printf ("\n CLNP HEADER ALL ES PDU SET : Test Results\n");
      DRVT_LOG0  ("CLNP HEADER ALL ES PDU SET : Test Results\n");
      }
    else
      {
      printf ("\n SMP HEADER ALL ES PDU SET : Test Results\n");
      DRVT_LOG0  ("SMP HEADER ALL ES PDU SET : Test Results\n");
      }
    }
  else
    {
    if (headerType == CLNP_HEADER_TYPE)
      {
      printf ("\n CLNP HEADER DIRECT PDU SET : Test Results\n");
      DRVT_LOG0  ("CLNP HEADER DIRECT PDU SET : Test Results\n");
      }
    else
      {
      printf ("\n SMP HEADER DIRECT PDU SET : Test Results\n");
      DRVT_LOG0  ("SMP HEADER DIRECT PDU SET : Test Results\n");
      }
    }
    
  printf ("\n   Number of PDUs sent successfully :              %d", 
          numSuccessSnd);
  printf ("\n   Number of PDUs received successfully :          %d", 
          numSuccessRcv);
  printf ("\n   Number of PDUs failed to be sent :              %d", 
          numFailureSnd);
  printf ("\n   Number of PDUs timed out receiving :            %d", 
          numTimeoutRcv);
  printf ("\n   Number of PDUs received with invalid lengths :  %d",
          numInvLngthRcv);
  printf ("\n   Number of PDUs received with invalid contents : %d\n",
          numInvPduCntsRcv);

  /* log the results */
  DRVT_CLOG1 ("  Number of PDUs sent successfully :              %d",
              numSuccessSnd);
  DRVT_CLOG1 ("  Number of PDUs received successfully :          %d",
              numSuccessRcv);
  DRVT_CLOG1 ("  Number of PDUs failed to be sent :              %d",
              numFailureSnd); 
  DRVT_CLOG1 ("  Number of PDUs timed out receiving :            %d",
              numTimeoutRcv);
  DRVT_CLOG1 ("  Number of PDUs received with invalid lengths :  %d",
              numInvLngthRcv);
  DRVT_CLOG1 ("  Number of PDUs received with invalid contents : %d",
              numInvPduCntsRcv);

  wait_msg ("Done performing test.");
  
  (*menu_set_fun) ();
  }

/************************************************************************/
/*				ctrlCfun				*/
/************************************************************************/
/* This function handles the ^c, and allows us to cleanup on exit	*/

ST_VOID ctrlCfun (ST_INT i)
  {
  contTesting = SD_FALSE;
  }


/************************************************************************/
/*                        doHighPerfPduSetTest                          */
/* This function performs a high performance test by sending out n      */
/* number of pdus and then subsequently waits for those n number of     */
/* pdus to be returned. The header type is CLNP and the total number of */
/* pdus sent out are n * MAX_TESTS                                      */
/************************************************************************/

ST_VOID doHighPerfPduSetTest (ST_VOID)
  {
ST_DOUBLE   timeTestStart;
ST_DOUBLE   timeTestEnd;
ST_DOUBLE   timeTestDiff;
time_t      timeStart;
time_t      timeCurrent;
time_t      timeElapsed;
ST_CHAR     strNumOut [10];
ST_RET      result;
ST_INT      numSentSucc;
ST_INT      numSuccessSnd;
ST_INT      numFailureSnd;
ST_INT      numSuccessRcv;
ST_INT      numInvLngthRcv;
ST_INT      numInvPduCntsRcv;
ST_INT      numMissed;
ST_INT      numOut;
ST_INT      numToFill;
ST_UCHAR    *sqncPtr;
ST_INT32    sqncNum;
ST_INT32    sqncNumExpct;
ST_INT32    sqncNumRcvd;
SN_REQ_ITEM *snReqList;
SN_REQ_ITEM *reqItem;
SN_UNITDATA snReq;
ST_BOOLEAN  countdownBegun;

  /* seed the random functionality */
  srand( (unsigned)time( NULL ) );
  
  /* set up the interrupt handling function ctrlCFun to catch <CTRL-C> */
  signal (SIGINT, ctrlCfun);
  
  /* get the max number of outstanding requests */
  printf ("\n Please enter in the size of the pipeline : ");
  
  /* get input from the user */
  strget (strNumOut);
  
  /* convert the string to a number */
  numOut = atoi (strNumOut);
  
  /* verify that the string is indeed a number */
  if (numOut <= 0)
    {
    wait_msg ("Invalid pipeline size");
    (*menu_set_fun) ();
    return;
    }
  
  /* initialize statistics for test */
  numSuccessSnd = 0; /* number of pdus successfully sent */
  numFailureSnd = 0; /* number of pdus failed to be sent */
  numSuccessRcv = 0; /* number of pdus successfully received */
  numInvLngthRcv = 0; /* number of pdus received with invalid lengths */
  numInvPduCntsRcv = 0; /* number of pdus received with invalid contents */
  
  /* create a start pdu to send out to the server */
  snReq.lpdu = chk_calloc (1, HDR_CTRL_BLOCK_LENGTH);
  snReq.lpdu_len = HDR_CTRL_BLOCK_LENGTH;
  memcpy (snReq.rem_mac, remMac, ETHE_MAC_LEN);
  memcpy (snReq.loc_mac, localMac, ETHE_MAC_LEN);
  memcpy (snReq.lpdu, CLNP_PDU_HEADER, HEADER_LENGTH);
  snReq.lpdu[HEADER_LENGTH + sizeof (ST_LONG)] = START_PDU;
  
  /* send out the start pdu and wait up to 2 seconds for the */
  /* server to initialize */
  result = clnp_snet_write (&snReq);
  
  /* log the start pdu */
  DRVT_LOG1  ("Local Mac: %s",
              convertMacToString (snReq.loc_mac));
  DRVT_CLOG1 ("Remote Mac: %s",
              convertMacToString (snReq.rem_mac));
  DRVT_HEX_LOG (snReq.lpdu_len, snReq.lpdu);
  
  /* free the pdu */
  chk_free (snReq.lpdu);
  
  /* check to see if the write was a success */
  if (result == SD_SUCCESS)
    {
    printf ("\n Start Test PDU sent successfully\n");
    DRVT_CLOG0 ("Start Test PDU sent successfully");
    }
  else
    {
    printf ("Failure on sending Start Test PDU; Retcode = %d\n", result);
    DRVT_CLOG1 ("Failure on sending Start Test PDU; Retcode = %d", result);
    return;
    }

  /* wait up to MAX_INIT_TIMOUT seconds */
  timeElapsed = 0;
  time (&timeCurrent);
  timeStart = timeCurrent;
 
  /* loop until timeElapsed >= MAX_INIT_TIMEOUT */
  while (timeElapsed < MAX_INIT_TIMOUT)
    {
    time (&timeCurrent);
    timeElapsed = timeCurrent - timeStart;
    }
  
  /* set contTesting flag to true */
  contTesting = SD_TRUE;
  
  /* start sequence numbers at zero */
  sqncNum = 0;

  /* initialize snReq List to NULL */
  snReqList = NULL;

  printf ("\n Performing Test : HIGH PERFORMANCE...\n");
  printf ("\n ****** PRESS <CTRL-C> TO STOP *******\n");
  DRVT_LOG0 ("Performing Test : HIGH PERFORMANCE...");
  
  /* Initialize SISCO timer */
  stimeInit ();

  /* set the number of missed pdus to zero */
  numMissed = 0;

  /* initialize the time elapsed to zero */  
  timeElapsed = 0;
  
  /* initialize countdownBegun to SD_FALSE */
  countdownBegun = SD_FALSE;
  
  /* get the time that the test was started */
  timeTestStart = sGetMsTime ();

  /* fill the pipeline */
  numSuccessSnd = fillPipeline (&snReqList, numOut, &sqncNum);
  numFailureSnd = numOut - numSuccessSnd;
  
  /* keep looping until contTesting is set to false */
  while ((contTesting == SD_TRUE) || (timeElapsed < HP_TEST_FINISH_TO))
    {
    /* read for the pdu that was passed out */
    result = clnp_snet_read (&snReq);
    
    /* check to see if we successfully received a pdu */
    if (result == SD_SUCCESS)
      {
      /* log the pdu received */
      DRVT_LOG1  ("Local Mac: %s",
                  convertMacToString (snReq.loc_mac));
      DRVT_CLOG1 ("Remote Mac: %s",
                  convertMacToString (snReq.rem_mac));
      DRVT_HEX_LOG (snReq.lpdu_len, snReq.lpdu);
      
      /* check to see if the remote mac address is in the sn req */
      if (memcmp (snReq.rem_mac, remMac, ETHE_MAC_LEN) == 0)
        {
        /* a pdu was receieved, now check to see if it is the same */
        /* one that was just sent out on the wire to the server */

        /* get sequence number to expect */
        sqncPtr = (ST_UCHAR *) &snReqList->snReq.lpdu[HEADER_LENGTH];
        sqncNumExpct = (((ST_UCHAR) (sqncPtr[0] << 24)) |
                        ((ST_UCHAR) (sqncPtr[1] << 16)) |
                        ((ST_UCHAR) (sqncPtr[2] << 8))  |
                        ((ST_UCHAR) (sqncPtr[3])));
        
        
        /* get sequence number received */
        sqncPtr = (ST_UCHAR *) &snReq.lpdu[HEADER_LENGTH];
        sqncNumRcvd = (((ST_UCHAR) (sqncPtr[0] << 24)) |
                       ((ST_UCHAR) (sqncPtr[1] << 16)) |
                       ((ST_UCHAR) (sqncPtr[2] << 8))  |
                       ((ST_UCHAR) (sqncPtr[3])));
        
        /* compare the sequence numbers */
        if (sqncNumExpct != sqncNumRcvd)
          {
          /* log the error in sequence numbers */
          DRVT_CLOG0  ("Error in Sequence Numbers!");

          /* log the expected and received sequence numbers */  
          DRVT_CLOG2 ("Expected Sequence Number : %d, Received Sequence Number : %d",
                      sqncNumExpct, sqncNumRcvd);

          if ((sqncNumRcvd - sqncNumExpct) < numOut)
            {
            /* try to re-sync */
            /* initialize numToFill to zero */
            numToFill = 0;
                        
            while ((ST_LONG) snReqList->snReq.lpdu[HEADER_LENGTH]
                     <= (ST_LONG) snReq.lpdu[HEADER_LENGTH])
              {
              /* get and unlink the first in the list of pdus */
              reqItem = list_get_first (&snReqList);
              
              /* free the memory */
              chk_free (reqItem->snReq.lpdu);
              chk_free (reqItem);
              
              /* increment the number of pdus to fill into the pipeline */
              numToFill++;
  
              /* check to see if the list is empty */
              if (snReqList == NULL)
                break;
              }
          
            /* increment the number of missed pdus by numToFill */
            numMissed += numToFill;
            
            if (contTesting == SD_TRUE)
              {
              /* fill the pipeline */
              numSentSucc = fillPipeline (&snReqList, numToFill, &sqncNum);
              numSuccessSnd += numSentSucc;
              numFailureSnd += numToFill - numSentSucc;
              }
            }
          }
        else
          {
          /* log the sequence number */
          DRVT_CLOG1 ("Sequence Number : %d", (ST_LONG) snReq.lpdu[HEADER_LENGTH]);
          
          /* check the size of both pdus first*/
          if (snReqList->snReq.lpdu_len == snReq.lpdu_len)
            {
            /* sizes are the same, now lets check the pdu's themselves */
            if (memcmp(snReqList->snReq.lpdu, snReq.lpdu, snReq.lpdu_len) == 0)
              {
              /* This is the pdu that we sent out! Hooray! */
              DRVT_CLOG0("Pdu received correctly");
              numSuccessRcv++;
              }
            else
              {
              /* This is not the pdu that we sent out. */
              DRVT_CLOG0("Pdu received incorrectly; Invalid pdu contents");
              numInvPduCntsRcv++;
              }
            }
          else
            {
            /* This is not the pdu that we sent out. */
            DRVT_CLOG0("Pdu received incorrectly; Invalid length");
            numInvLngthRcv++;
            }
            
          /* remove sn req from list */
          reqItem = list_get_first (&snReqList);
          
          /* free the memory */
          chk_free (reqItem->snReq.lpdu);
          chk_free (reqItem);
          
          if (contTesting == SD_TRUE)
            {
            /* fill pipeline */
            numSentSucc = fillPipeline (&snReqList, 1, &sqncNum);
            numSuccessSnd += numSentSucc;
            numFailureSnd += 1 - numSentSucc;
            }
          }
        }
      else
        {
        /* log that the pdu received was from mac other than server */
        DRVT_CLOG0 ("Pdu received from MAC other than server");
        }
      /* we are done with the processing, free anything			*/
      /* allocated by sub-network					*/
      clnp_snet_free (&snReq);
      }
      
    if (contTesting == SD_FALSE)
      {
      if (countdownBegun == SD_TRUE)
        {
        /* derive the timeElapsed */
        time (&timeCurrent);
        timeElapsed = timeCurrent - timeStart;
        }
      else
        {
        if (result == SD_SUCCESS)
          {
          /* get the time that the test ended */
          timeTestEnd = sGetMsTime ();
          }
        else
          {
          if (countdownBegun == SD_FALSE)
            {
            /* start the countdown to completion of the test */
            time (&timeStart);
            timeCurrent = timeStart;
            countdownBegun = SD_TRUE;
            }
          }
        }
      }
    }
  
  /* get the time that the time was run */
  timeTestDiff = timeTestEnd - timeTestStart;
  
  /* clear out the remaining requests in the list */
  while (snReqList != NULL)
    {
    /* remove sn req from list */
    reqItem = list_get_first (&snReqList);
    
    /* free the memory */
    chk_free (reqItem->snReq.lpdu);
    chk_free (reqItem);
    }
      
  /* create a stop pdu to send out to the server */
  snReq.lpdu = chk_calloc (1, HDR_CTRL_BLOCK_LENGTH);
  snReq.lpdu_len = HDR_CTRL_BLOCK_LENGTH;
  memcpy (snReq.rem_mac, remMac, ETHE_MAC_LEN);
  memcpy (snReq.loc_mac, localMac, ETHE_MAC_LEN);
  memcpy (snReq.lpdu, CLNP_PDU_HEADER, HEADER_LENGTH);
  snReq.lpdu[HEADER_LENGTH + sizeof (ST_LONG)] = STOP_PDU;
  
  /* send out the start pdu and wait up to 2 seconds for the */
  /* server to initialize */
  result = clnp_snet_write (&snReq);
  
  /* log the stop pdu */
  DRVT_LOG1  ("Local Mac: %s",
              convertMacToString (snReq.loc_mac));
  DRVT_CLOG1 ("Remote Mac: %s",
              convertMacToString (snReq.rem_mac));
  DRVT_HEX_LOG (snReq.lpdu_len, snReq.lpdu);

  /* free the pdu */
  chk_free (snReq.lpdu);
  
  /* check to see if the write was a success */
  if (result == SD_SUCCESS)
    {
    printf ("\n Stop Test PDU sent successfully\n");
    DRVT_CLOG0 ("Stop Test PDU sent successfully");
    }
  else
    {
    printf ("\n Failure on sending Stop Test PDU; Retcode = %d\n", result);
    DRVT_CLOG1 ("Failure on sending Stop Test PDU; Retcode = %d", result);
    }
  
  /* print the results */
  printf ("\n HIGH PERFORMANCE TEST : Test Results\n");
  DRVT_LOG0  ("HIGH PERFORMANCE TEST : Test Results");
    
  printf ("\n   Number of PDUs sent successfully :              %d", 
          numSuccessSnd);
  printf ("\n   Number of PDUs received successfully :          %d", 
          numSuccessRcv);
  printf ("\n   Number of PDUs failed to be sent :              %d", 
          numFailureSnd);
  printf ("\n   Number of PDUs failed to be received :          %d",
          numMissed);
  printf ("\n   Number of PDUs received with invalid lengths :  %d",
          numInvLngthRcv);
  printf ("\n   Number of PDUs received with invalid contents : %d",
          numInvPduCntsRcv);
  printf ("\n   Total Test Time (in seconds) :                  %f",
          (timeTestDiff/1000));
  printf ("\n   (PDUs Sent & Received Successfully/Second) :    %f\n",
          (numSuccessRcv/(timeTestDiff / 1000)));
          
  /* log the results */
  DRVT_CLOG1 ("  Number of PDUs sent successfully :              %d",
              numSuccessSnd);
  DRVT_CLOG1 ("  Number of PDUs received successfully :          %d",
              numSuccessRcv);
  DRVT_CLOG1 ("  Number of PDUs failed to be sent :              %d",
              numFailureSnd); 
  DRVT_CLOG1 ("  Number of PDUs failed to be received :          %d",
              numMissed);
  DRVT_CLOG1 ("  Number of PDUs received with invalid lengths :  %d",
              numInvLngthRcv);
  DRVT_CLOG1 ("  Number of PDUs received with invalid contents : %d",
              numInvPduCntsRcv);
  DRVT_CLOG1 ("  Total Test Time (in seconds) :                  %f",
              (timeTestDiff/1000));
  DRVT_CLOG1 ("  (PDUs Sent & Received Successfully/Second) :    %f",
              (numSuccessRcv/(timeTestDiff / 1000)));


  wait_msg ("Done performing test.");
  
  (*menu_set_fun) ();
  }

/************************************************************************/
/*                             fillPipeline                             */
/* This function will fill the pipeline for the high performance test.  */
/* The arguments to this function include a pointer to the pointer to   */
/* the head of the pdu list, the number of pdus to be piped out, and a  */
/* pointer to the sequence number                                       */
/************************************************************************/

ST_INT fillPipeline (SN_REQ_ITEM **snReqList, ST_INT numOut, ST_INT32 *sqncNum)
  {
ST_INT      i;
ST_INT      j;
ST_INT      numSuccessSnd;
ST_RET      result;
ST_UCHAR    *sqncPtr;
SN_REQ_ITEM *reqItem;

  /* initialize number of requests sent successfully */
  numSuccessSnd = 0;
  
  /* send out the pdu's and wait to receieve them in order */
  /* fill up the pipeline */
  for (i = 0; i < numOut; i++)
    {
    /* allocate memory for req item */
    reqItem = chk_calloc (1, sizeof (SN_REQ_ITEM));
    
    /* set local mac */
    memcpy(reqItem->snReq.loc_mac, localMac, ETHE_MAC_LEN);
    
    /* set remote mac */
    memcpy(reqItem->snReq.rem_mac, remMac, ETHE_MAC_LEN);
    
    /* set pdu length to maximum possible */
    reqItem->snReq.lpdu_len = maxPduLen;
    
    /* allocate space for pdu */
    reqItem->snReq.lpdu = chk_malloc (maxPduLen);
    
    /* set up header */
    memcpy (reqItem->snReq.lpdu, CLNP_PDU_HEADER, HEADER_LENGTH);
    
    /* set the sequence number */
    sqncPtr = (ST_UCHAR *) &reqItem->snReq.lpdu[HEADER_LENGTH];
    
    sqncPtr[0] = (ST_UCHAR) ((*sqncNum >> 24) & 0x000000ff);
    sqncPtr[1] = (ST_UCHAR) ((*sqncNum >> 16) & 0x000000ff);
    sqncPtr[2] = (ST_UCHAR) ((*sqncNum >> 8)  & 0x000000ff);
    sqncPtr[3] = (ST_UCHAR)  (*sqncNum & 0x000000ff);
    
    /* set pdu as a test pdu */
    reqItem->snReq.lpdu[HEADER_LENGTH + sizeof(ST_LONG)] = TEST_PDU;
    
    /* randomly set contents of pdu */
    for (j = HDR_CTRL_BLOCK_LENGTH; j < reqItem->snReq.lpdu_len; j++)
      {
      reqItem->snReq.lpdu[j] = rand ();
      }
    
    /* append the request to the end of the list */
    list_add_last (snReqList, reqItem);
    
    /* send out the request */
    result = clnp_snet_write (&reqItem->snReq);
    
    /* log the pdu */
    DRVT_LOG1  ("Local Mac: %s",
                convertMacToString (reqItem->snReq.loc_mac));
    DRVT_CLOG1 ("Remote Mac: %s",
                convertMacToString (reqItem->snReq.rem_mac));
    DRVT_HEX_LOG (reqItem->snReq.lpdu_len, reqItem->snReq.lpdu);

    /* check to see if the write was a success */
    if (result == SD_SUCCESS)
      {
      /* log the success of sending the pdu */
      DRVT_CLOG0 ("PDU sent successfully");
      numSuccessSnd++;
      }
    else
      {
      /* log the failure of sending the pdu */
      DRVT_CLOG1 ("Failure on sending PDU; Retcode = %d", result);
      }
    
    /* increment the sequence number */
    (*sqncNum)++;
    }
  return (numSuccessSnd);
  }

  
/************************************************************************/
/*                          initializeTestSet                           */
/* The following function will initialize the test set of subnetwork    */
/* requests used by this application. The dest argument determines      */
/* whether the test set will be sent directly to the server or through  */
/* all es. The argument headerType determines whether the header will   */
/* be CLNP or SMP                                                       */
/************************************************************************/

ST_VOID initializeTestSet (ST_INT dest, ST_INT headerType)
  {
ST_INT32 i;
ST_INT   j;
ST_INT   randSize;
ST_UCHAR *sqncPtr;
  
  /* free any of the pdus */
  for (i = 0; i < MAX_TESTS; i++)
    {
    if (sn_reqs[i].lpdu != NULL)
      chk_free (sn_reqs[i].lpdu);
    }
    
  /* set all of the local and remote macs to the correct values */
  for (i = 0; i < MAX_TESTS; i++)
    {
    memcpy (sn_reqs[i].loc_mac, localMac, ETHE_MAC_LEN);
    if (dest == DEST_ALL_ES)
      memcpy (sn_reqs[i].rem_mac, allEsMac, ETHE_MAC_LEN);
    else
      memcpy (sn_reqs[i].rem_mac, remMac, ETHE_MAC_LEN);
    }
  
  /* First pdu is smallest poss pdu */
  sn_reqs[0].lpdu_len = HDR_CTRL_BLOCK_LENGTH + 1;
  sn_reqs[0].lpdu = chk_malloc (HDR_CTRL_BLOCK_LENGTH + 1);

  /* Second pdu is largest poss pdu */
  sn_reqs[1].lpdu_len = maxPduLen;
  sn_reqs[1].lpdu = chk_malloc (maxPduLen);
  
  /* seed the random functionality */
  srand( (unsigned)time( NULL ) );
  
  /* rest of pdus are of random sizes */
  for (i = 2; i < MAX_TESTS; i++)
    {
    /* set the pdu sizes randomly from (maxPduLen - HDR_CTRL_BLOCK_LENGTH) to maxPduLen */
    randSize = (rand () % (maxPduLen - HDR_CTRL_BLOCK_LENGTH)) + HDR_CTRL_BLOCK_LENGTH;
    sn_reqs[i].lpdu_len = randSize;
    sn_reqs[i].lpdu = chk_malloc (randSize);
    }
  
  /* check to see what header type is to be used */
  if (headerType == CLNP_HEADER_TYPE)
    {
    /* use the CLNP header */
    for (i = 0; i < MAX_TESTS; i++)
      {
      memcpy (sn_reqs[i].lpdu, CLNP_PDU_HEADER, HEADER_LENGTH);
      }
    }
  else
    {
    /* use the SMP header */
    for (i = 0; i < MAX_TESTS; i++)
      {
      memcpy (sn_reqs[i].lpdu, SMP_PDU_HEADER, HEADER_LENGTH);
      }
    }

  for (i = 0; i < MAX_TESTS; i++)
    {
    /* set the sequence number */
    sqncPtr = (ST_UCHAR *) &sn_reqs[i].lpdu[HEADER_LENGTH];
    
    sqncPtr[0] = (ST_UCHAR) ((i >> 24) & 0x000000ff);
    sqncPtr[1] = (ST_UCHAR) ((i >> 16) & 0x000000ff);
    sqncPtr[2] = (ST_UCHAR) ((i >> 8)  & 0x000000ff);
    sqncPtr[3] = (ST_UCHAR)  (i & 0x000000ff);
    
    /* set the pdu type */
    sn_reqs[i].lpdu[HEADER_LENGTH + sizeof(ST_LONG)] = TEST_PDU;
    
    /* fill in the rest of the pdu with random data */
    for (j = HDR_CTRL_BLOCK_LENGTH; j < sn_reqs[i].lpdu_len; j++)
      {
      sn_reqs[i].lpdu[j] = rand ();
      }
    }    
  
  /* set up the hardcoded all es pdus */
  memcpy (allEsReq.loc_mac, localMac, ETHE_MAC_LEN);
  memcpy (allEsReq.rem_mac, allEsMac, ETHE_MAC_LEN);
  allEsReq.lpdu_len = ALL_ES_PDU_LENGTH;
  allEsReq.lpdu = ALL_ES_PDU;
  }

/************************************************************************/
/*                          wait_any_event				*/
/* The following function is essential for detecting keyboard events    */
/* created by the user                                                  */
/************************************************************************/

ST_VOID wait_any_event (ST_LONG max_ms_delay)
  {
DWORD rc;
int i;

  /* Just do a maxwait on the keyboard */
  rc = WaitForSingleObject (kbdEvent, max_ms_delay);

  if (rc == 0xFFFFFFFF)
    {
    printf ("\n WaitForMultipleObjects () failed, rc = %d", GetLastError ());
    return;
    }

  if (rc == WAIT_TIMEOUT)
    return;		/* nothing going on at the moment */

  i = rc - WAIT_OBJECT_0;

  switch (i)
    {
    case 0:		/* Keyboard event, autoreset */
      if (ResetEvent (kbdEvent) == SD_FALSE)
        printf ("ERROR: Reset Kbd Event, rc = %d", GetLastError);
    break;

    default:
    break;
    }
  }

/************************************************************************/
/*                          convertMacToString				*/
/* This function will return a character pointer to a string that       */
/* represents the mac address sent to it through the function argument  */
/* ST_UCHAR *mac                                                        */
/************************************************************************/
  
ST_CHAR *convertMacToString (ST_UCHAR *mac)
  {
ST_INT i;
static ST_CHAR macOut[18];
ST_CHAR *cPtr;
  
  cPtr = macOut;
  
  for (i = 0; i < ETHE_MAC_LEN; i++)
    {
    sprintf (cPtr, "%02x", mac[i]);
    cPtr+=2;
    if (i != (ETHE_MAC_LEN - 1))
      {
      *cPtr++ = ' ';
      }
    } 
    
  return (macOut);
  }
  
/************************************************************************/
/*                          m_set_log_config				*/
/* This function will intialize the logging functionality of the client */
/************************************************************************/

ST_RET 	m_set_log_config ()
  {
LOG_CTRL *lc;

  /* Initialize the logging for MMS-EASE */
  lc = sLogCtrl;

  /* If no file is present, use defaults */
  /* Use File logging */
  lc->logCtrl = LOG_FILE_EN;

  /* Use time/date time log */
  lc->logCtrl |= (LOG_TIME_EN);

  /* File Logging Control defaults */
  lc->fc.fileName = "mms.log";
  lc->fc.maxSize = 250000L;
  lc->fc.ctrl = (FIL_CTRL_WIPE_EN |
                 FIL_CTRL_WRAP_EN |
                 FIL_CTRL_NO_APPEND |
                 FIL_CTRL_MSG_HDR_EN);


  return (SD_SUCCESS);
  }

/************************************************************************/
/*                       welcome                                                */
/************************************************************************/

static ST_VOID welcome ()
  {
  printf ("\n%s", cr_sn_test);
  }

