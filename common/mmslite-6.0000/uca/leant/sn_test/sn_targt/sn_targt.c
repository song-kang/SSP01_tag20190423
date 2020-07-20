/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998-2008, All Rights Reserved.					*/
/*									*/
/* MODULE NAME : sn_targt.c						*/
/* PRODUCT(S)  : MMS-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/01/08  JRB    11     Add MAC address cmd line arg (like sn_test).	*/
/* 07/23/08  GLB    10     Update Copyright.                            */
/* 05/08/08  JRB    09     Add network_device cmd line arg for _WIN32.	*/
/*			   Chk return from clnp_snet_init.		*/
/*			   Chg main return from void to "int".		*/
/* 02/14/07  GLB    08     Added welcome() to print Copyright info      */
/* 06/02/06  JRB    07     Use new clnp_snet_write_raw funct.		*/
/* 04/13/05  JRB    06     Del clnp_err_logstr. Use lean_var.c.		*/
/* 12/11/03  JRB    05     Don't use _itoa, use sprintf instead.	*/
/* 05/01/00  RKR    03      Updated the slog				*/
/* 10/29/98  RWM    02      Fixed handling of sequence numbers within   */
/*                          the pdu so it is processor independant      */
/* 05/06/98  RWM            Created                                     */
/************************************************************************/
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include "glbtypes.h"
#include "clnp_log.h"	/* for debugging purposes */
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "str_util.h"

/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism	*/

static ST_VOID welcome (ST_VOID);

#define ETHE_MAC_LEN   6 /* the length of an ethernet mac address */

#define TEST_PDU  0 /* pdu that is meant for a test */
#define START_PDU 1 /* pdu that will init the server for a test */
#define STOP_PDU  2 /* pdu that will prompt the server to stop a test */

/* this is the length of a pdu header */
#define HEADER_LENGTH 4

/* this is the length of a pdu header along with the control block */
#define HDR_CTRL_BLOCK_LENGTH 9
/* a header control block consists of a CLNP or SMP header followed by a   */
/* 4 byte sequence number. The last byte of the block (which follows the   */
/* sequence number) consists of the pdu type (whether it is a start test   */
/* pdu, stop test pdu, or a test pdu).                                     */

#if defined (DEBUG_SISCO)
/* the following are macros used to log the pdu traffic of the server */

/* slog defines */
#define DRV_TEST_TYPE 1667
SD_CONST ST_CHAR *SD_CONST _drvt_err_logstr = "DRVT_LOG_ERR";
#define DRVT_LOG0(a) {\
                     _slog (sLogCtrl,_drvt_err_logstr,\
                           thisFileName,__LINE__,a);\
                     }

#define DRVT_LOG1(a,b) {\
                       _slog (sLogCtrl,_drvt_err_logstr,\
                             thisFileName,__LINE__,a,b);\
                       }
                       
#define DRVT_LOG2(a,b,c) {\
                         _slog (sLogCtrl,_drvt_err_logstr,\
                               thisFileName,__LINE__,a,b,c);\
                         }

#define DRVT_CLOG0(a) {\
                      _slogc (sLogCtrl,a);\
                      }

#define DRVT_CLOG1(a,b) {\
                        _slogc (sLogCtrl,a,b);\
                        }

#define DRVT_CLOG2(a,b,c) {\
                          _slogc (sLogCtrl,a,b,c);\
                          }
                        
#define DRVT_HEX_LOG(a,b) {\
                          _slogHex (sLogCtrl,a,b);\
                          }
                          
#else

/* these macros do nothing */
#define DRVT_LOG0(a)
#define DRVT_LOG1(a,b)
#define DRVT_LOG2(a,b,c)
#define DRVT_CLOG0(a)
#define DRVT_CLOG1(a,b)
#define DRVT_CLOG2(a,b,c)
#define DRVT_HEX_LOG(a,b)

#endif

/************************************************************************/

CLNP_PARAM clnp_param;  /* global structure that is required for linking */
ST_UINT clnp_debug_sel; /* global structure that is required for linking */

ST_UCHAR localMac [ETHE_MAC_LEN]; /* this holds the servers mac address */
ST_UCHAR allEsMac [ETHE_MAC_LEN]; /* this holds the all es mac address */

ST_UCHAR remMac [6];	/* Remote MAC (i.e. machine running sn_test).	*/

/************************************************************************/

#if defined (DEBUG_SISCO)
/* these function calls are necessary for logging */
ST_RET 	m_set_log_config ();
ST_CHAR *convertMacToString (ST_UCHAR *mac);
#endif

/************************************************************************/
/*                              printUsage                              */
/* This function will print the usage of this application to the user   */ 
/************************************************************************/
ST_VOID printUsage ()
  {
#if defined(_WIN32)
  printf ("\n\n Usage : sn_targt \"mac address\" network_device");
  printf ("\n   mac address    = MAC address (in hex) of REMOTE machine running 'sn_test'");
  printf ("\n   network_device = WinPcap network device as configured in osicfg.xml");
  printf ("\n   Example : sn_targt \"00 60 08 06 AC 27\" \\Device\\NPF_{66511A69-95AF-48A6-8E1C-457F9522CE2A}");
#else
  printf ("\n\n Usage : sn_targt \"mac address\"");
  printf ("\n   mac address    = MAC address (in hex) of REMOTE machine running 'sn_test'");
  printf ("\n   Example : sn_targt \"00 60 08 06 AC 27\"");
#endif
  }
  
/************************************************************************/
/*                                main                                  */
/* This function is the main entry point to the server. The server will */
/* initialize its CLNP settings, get the local mac address, get the all */
/* es mac address, and loop endlessly while receiving pdus from the     */
/* client application and subsequently echo them back to the client app */
/************************************************************************/

int main (int argc, char *argv[])
  {
ST_RET      result;          /* the result of a read */
SN_UNITDATA snReq;           /* structure to hold incoming and outgoing pdus */
ST_INT      pdusSentSucc;    /* number of pdus sent successfully */
ST_INT      pdusSentFail;    /* number of pdus sent unsuccessfully */
ST_INT      pdusRcv;         /* number of pdus received */
ST_INT32    sqncNum;         /* the current sequence number */
ST_INT32    sqncNumRec;     /* pointer to sequence number received within pdu */
ST_UCHAR    *sqncPtr;
ST_BOOLEAN macRetrieved;
ST_UINT remMacLen;	/* len of remote MAC on command line	*/
static ST_UCHAR raw_packet [ETHE_MAX_LEN_LSDU];	/* raw packet buffer	*/

  welcome ();

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

#if defined (DEBUG_SISCO)
  /* set up the logging */
  m_set_log_config ();
#endif
  
  /* initialize the interface between clnp and the subnetwork */
  if (clnp_snet_init (&clnp_param))
    {
    printf ("\n clnp_snet_init failed. Check parameters.");
    return (2);
    }

  /* get my local mac address and save it off for later use */
  clnp_snet_get_local_mac (localMac);
  
  /* get the end system mac address and save it off for later use */
  clnp_snet_get_all_es_mac (allEsMac);
  
  /* loop forever */
  while (SD_TRUE)
    {
    /* read for an incoming pdu */
    result = clnp_snet_read (&snReq);

    /* check to see if the read was a success */
    if (result == SD_SUCCESS)
      {
      /* log the incoming pdu */
      DRVT_LOG1  ("Local Mac: %s",
                  convertMacToString (snReq.loc_mac));
      DRVT_CLOG1 ("Remote Mac: %s",
                  convertMacToString (snReq.rem_mac));
      DRVT_HEX_LOG (snReq.lpdu_len, snReq.lpdu);

      /* check to see if the pdu was sent by the client */
      if (memcmp (remMac, snReq.rem_mac, ETHE_MAC_LEN) == 0)
        {
        /* log the incoming pdus source */
        DRVT_CLOG0 ("Pdu received from client");
        
        /* check to see if the pdu is a start test pdu */
        if (snReq.lpdu[HDR_CTRL_BLOCK_LENGTH - 1] == START_PDU)
          {
          /* start test pdu received */
          DRVT_CLOG0 ("Start Test PDU received from client");
          
          /* reset the statistics */
          pdusSentSucc = 0;
          pdusSentFail = 0;
          pdusRcv = 0;
          sqncNum = 0;
          }

        /* check to see if the pdu is a stop test pdu */
        if (snReq.lpdu[HDR_CTRL_BLOCK_LENGTH - 1] == STOP_PDU)
          {
          /* stop test pdu received */
          DRVT_CLOG0 ("Stop Test PDU received from client");
          
          /* log the statistics */
          DRVT_LOG1  ("Number of Test pdus received           : %d",
                      pdusRcv);
          DRVT_CLOG1 ("Number of Test pdus sent successfully  : %d",
                      pdusSentSucc);
          DRVT_CLOG1 ("Number of Test pdus sent unsucessfully : %d",
                      pdusSentFail);
          }

        /* check to see if the pdu is a test pdu */
        if (snReq.lpdu[HDR_CTRL_BLOCK_LENGTH - 1] == TEST_PDU)
          {
          /* increment pdus received counter */
          pdusRcv++;
          
          /* get the sequence number */
          sqncPtr = (ST_UCHAR *) &snReq.lpdu[HEADER_LENGTH];
          sqncNumRec = (((ST_UCHAR) (sqncPtr[0] << 24)) | 
                        ((ST_UCHAR) (sqncPtr[1] << 16)) |
                        ((ST_UCHAR) (sqncPtr[2] << 8))  |
                        ((ST_UCHAR) (sqncPtr[3])));
                        
          /* check to see if the sequence numbers are mis-aligned */
          if (sqncNumRec != sqncNum)
            {
            /* log the error in sequence numbers */
            DRVT_CLOG0  ("Error in Sequence Numbers!");
            /* log the expected and received sequence numbers */  
            DRVT_CLOG2 ("Expected Sequence Number : %d, Received Sequence Number : %d",
                        sqncNum, sqncNumRec);
            }
          else
            {
            /* log the sequence number */
            DRVT_CLOG1 ("Sequence Number : %d", sqncNum);
            }

                      
          /* check to see if the loc_mac is the all es mac */
          if (memcmp (snReq.loc_mac, allEsMac, ETHE_MAC_LEN) == 0)
            {
            /* send it back to the clients mac mac */
            memcpy (snReq.rem_mac, remMac, ETHE_MAC_LEN);
            }
            
          /* set the local mac address */
          memcpy (snReq.loc_mac, localMac, ETHE_MAC_LEN);
  
          /* echo back the pdu */
          memcpy (raw_packet, snReq.rem_mac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
          memcpy (&raw_packet[6], snReq.loc_mac, CLNP_MAX_LEN_MAC);/* SRC MAC*/
          /* Set the 802.3 length/type field (bytes 12 and 13).	*/
          raw_packet[12] = (ST_UCHAR) (snReq.lpdu_len >> 8);	
          raw_packet[13] = (ST_UCHAR) (snReq.lpdu_len & 0xff);
          memcpy (&raw_packet[14], snReq.lpdu, snReq.lpdu_len);
          result = clnp_snet_write_raw (raw_packet, snReq.lpdu_len+14);

          /* log the pdu */
          DRVT_LOG1  ("Local Mac: %s",
                      convertMacToString (snReq.loc_mac));
          DRVT_CLOG1 ("Remote Mac: %s",
                      convertMacToString (snReq.rem_mac));
          DRVT_HEX_LOG (snReq.lpdu_len, snReq.lpdu);
          
          /* check to see if the pdu was written correctly */
          if (result == SD_SUCCESS)
            {
            /* increment the pdus sent successfully counter */
            pdusSentSucc++;
            
            /* log that the pdu was sent successfully */
            DRVT_CLOG0 ("PDU sent successfully");
            }
          else
            {
            /* increment the pdus sent unsuccessfully counter */
            pdusSentFail++;

            /* log that the pdu was sent unsuccessfully */
            DRVT_CLOG1 ("Failure to write; Error Code : %d", result);
            }
          
          /* increment sequence counter */
          sqncNum++;
          }
        }  
      else
        {
        /* log that the pdu was received from a source other than the client */
        DRVT_CLOG0 ("PDU received from MAC other than client");
        }
      }        
    }

  /* terminate the session */
  clnp_snet_term ();
  return (0);
  }


#if defined (DEBUG_SISCO)

/************************************************************************/
/*                          m_set_log_config				*/
/* This function will intialize the logging functionality of the server */
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
  
#endif  

/************************************************************************/
/*                       welcome                                        */
/************************************************************************/

static ST_VOID welcome ()
  {
  printf ("\n SISCO copyright (c) Copyright Systems Integration Specialists Company, Inc.,");
  printf ("\n                  1986 - 2008, All Rights Reserved");
  }

