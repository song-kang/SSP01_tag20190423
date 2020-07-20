/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1998, All Rights Reserved.		        	*/
/*									*/
/* MODULE NAME : sm_targt.c						*/
/* PRODUCT(S)  : MMS-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/*   This module contains all of the functionality necessary for the    */
/*   target application that will be used to test driver functionality  */
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/05/03  EJV    02     Chg adlc_debug_sel to ST_UINT from ST_ULONG	*/
/* 05/29/98  RWM            Created                                     */
/************************************************************************/
#include <time.h>
#include "glbtypes.h"
#include "gen_list.h"
#include "adlc_sm.h"

/************************************************************************/
/* flag to identify if a packet is a start test packet */
#define START_PACKET 0

/* flag to identify if a packet is a stop test packet */
#define STOP_PACKET  1

/* flag to identify if a packet is a test packet */
#define TEST_PACKET  2

/* flag to identify if a packet is a control line test packet */
#define CTRL_LINE_TEST_PACKET 3

/* the size of either a start packet or a stop packet */
#define NON_TEST_PACKET_SIZE 2

/* the maximum allowed size for a packet */
#define MAX_PACKET_SIZE   1000

/* the number of bytes that constitute a packets type spec */
/* and its size spec */
#define PKT_TYPE_SIZE_LENGTH 5

/* the number of bytes that indicate a packets type (start, */ 
/* stop, or test) */
#define PKT_TYPE_LENGTH      1

/* the byte index where the packet type is specified within the packet */
/* i.e. the packet type is the first byte of the packet received from  */
/* the client application */
#define PKT_TYPE_INDEX       0

/* the byte index (within either a start packet or stop packet) that */
/* identifies whether a test will use the sm_getc and sm_putc functions */
/* or use the sm_get and sm_put functions */
#define PKT_CHAR_RES_INDEX   1

/* flag to identify that the functions sm_get and sm_put will be */
/* used during a test procedure */
#define CHARACTER_RES_OFF    0

/* flag to identify that the functions sm_getc and sm_putc will be */
/* used during a test procedure */
#define CHARACTER_RES_ON     1

/* maximum number of seconds that must elapse between read attempts */
/* while assembling an incoming packet */
#define MAX_READ_TIMEOUT  5

/* the following are indexes into a control line test packet */
/* for each seperate control line */
#define INDEX_RTS 1
#define INDEX_DTR 2
#define INDEX_CTS 3
#define INDEX_DSR 4
#define INDEX_DCD 5

/* length of a outgoing packet from the server as a result */
/* of a control line test request */
#define CTRL_LINE_TEST_PKT_LENGTH 6


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
#endif

#if defined (DEBUG_SISCO)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;	/* Define for SLOG mechanism	*/
#include "slog.h"
/* the following are macros used to log the packet traffic of the server */

/* slog defines */
#define DRV_TEST_TYPE 1667

#define DRVT_LOG0(a) {\
                     slog (sLogCtrl,DRV_TEST_TYPE,\
                           thisFileName,__LINE__,a);\
                     }

#define DRVT_LOG1(a,b) {\
                       slog (sLogCtrl,DRV_TEST_TYPE,\
                             thisFileName,__LINE__,a,b);\
                       }
                       
#define DRVT_LOG2(a,b,c) {\
                         slog (sLogCtrl,DRV_TEST_TYPE,\
                               thisFileName,__LINE__,a,b,c);\
                         }

#define DRVT_LOG3(a,b,c,d) {\
                           slog (sLogCtrl,DRV_TEST_TYPE,\
                                 thisFileName,__LINE__,a,b,c,d);\
                           }
                         
                         
#define DRVT_LOG4(a,b,c,d,e) {\
                             slog (sLogCtrl,DRV_TEST_TYPE,\
                                   thisFileName,__LINE__,a,b,c,d,e);\
                             }

#define DRVT_LOG5(a,b,c,d,e,f) {\
                               slog (sLogCtrl,DRV_TEST_TYPE,\
                                     thisFileName,__LINE__,a,b,c,d,e,f);\
                               }
                         
#define DRVT_CLOG0(a) {\
                      slog (sLogCtrl,SLOG_CONT,\
                           thisFileName,__LINE__,a);\
                      }

#define DRVT_CLOG1(a,b) {\
                        slog (sLogCtrl,SLOG_CONT,\
                             thisFileName,__LINE__,a,b);\
                        }

#define DRVT_CLOG2(a,b,c) {\
                          slog (sLogCtrl,SLOG_CONT,\
                               thisFileName,__LINE__,a,b,c);\
                          }
                        
#define DRVT_HEX_LOG(a,b) {\
                          slogHex (sLogCtrl,SLOG_CONT,\
                                   thisFileName,__LINE__,a,b);\
                          }

ST_RET 	m_set_log_config ();
                          
#else

/* these macros do nothing */
#define DRVT_LOG0(a)
#define DRVT_LOG1(a,b)
#define DRVT_LOG2(a,b,c)
#define DRVT_LOG3(a,b,c,d)
#define DRVT_LOG4(a,b,c,d,e)
#define DRVT_LOG5(a,b,c,d,e,f)
#define DRVT_CLOG0(a)
#define DRVT_CLOG1(a,b)
#define DRVT_CLOG2(a,b,c)
#define DRVT_HEX_LOG(a,b)

#endif

/************************************************************************/

ST_INT  port   = COM2;      /* com port for test (i.e. COM2) */
ST_INT  baud   = BAUD9600;  /* baud rate of 115200 for test */
ST_INT  parity = NOPAR;     /* no parity */
ST_INT  data   = DATA8;     /* data bits = 8 */
ST_INT  stop   = STOP1;     /* stop bits = 1 */

ST_UINT adlc_debug_sel;    /* needed for compilation */
/************************************************************************/

/************************************************************************/
/*                                main                                  */
/* PURPOSE : This function is the main entry point into this app.       */
/* This routine will initialize the com port for serial communications  */
/* and loop endlessly while receiving packets and echoing them back     */
/* INPUT : NONE                                                         */
/* OUTPUT : NONE                                                        */
/************************************************************************/

ST_VOID main ()
  {
ST_RET     result;          /* the result of a read */
time_t     timeStartRead;
time_t     timeCurrentRead;
time_t     totalTimeBtwReads;
ST_INT     pktsSentSucc;    /* number of packets sent successfully */
ST_INT     pktsSentFail;    /* number of packets sent unsuccessfully */
ST_INT     pktsRcv;         /* number of packets received */
ST_INT     totalBytesRead;  /* total bytes read */
ST_INT     getResult;
ST_INT     i;
ST_INT     bytesRead;       /* bytes read when sm_put succeeds */
ST_LONG    *pktSize;
ST_LONG    sqncNum;         /* the current sequence number */
ST_LONG    *sqncNumRec;     /* pointer to sequence number received within packet */
ST_CHAR    packetIn [MAX_PACKET_SIZE];
ST_CHAR    *packetPtr;
ST_CHAR    charIn;
ST_BOOLEAN resolutionChar;
ST_BOOLEAN pktReceived;

#if defined (DEBUG_SISCO)
  /* set up the logging */
  m_set_log_config ();
#endif


  DRVT_LOG0 ("Test started");

  /* initialize resolutionChar to be False */
  resolutionChar = SD_FALSE;
  
  /* initialize the serial manager */
  result = sm_init (port, baud, parity, data, stop);

  /* loop forever */
  while (SD_TRUE)
    {
    pktReceived = SD_FALSE;
    
    if (resolutionChar == SD_TRUE)
      {
      /* read for an incoming packet */
      packetPtr = packetIn;
      getResult = sm_getc (port);
    
      /* check to see if a character was read */
      if (getResult != -1)
        {
        /* set pktReceived to true since we have received the */
        /* first character of a packet */
        pktReceived = SD_TRUE;
        
        charIn = getResult;
        
        /* set the first byte of the packet */
        *packetPtr++ = charIn;
        
        /* initialize totalBytesRead to one */
        totalBytesRead = 1;
  
        /* set totalTimeBtwReads to zero */
        totalTimeBtwReads = 0;
        time (&timeCurrentRead);
        timeStartRead = timeCurrentRead;
        
        /* check to see if the packet is a test packet */
        if (packetIn[PKT_TYPE_INDEX] == TEST_PACKET)
          {
          /* now loop until we timeout, run out of buffer space */
          /* or the packet is completely received */
          while ((totalBytesRead < MAX_PACKET_SIZE) &&
                 (totalTimeBtwReads < MAX_READ_TIMEOUT))
            {
            /* try to read another character */
            getResult = sm_getc (port);
  
            /* check to see if a character was read */
            if (getResult != -1)
              {
              /* a character was read */
              charIn = getResult;
              
              /* set the next byte in the packet */
              *packetPtr++ = charIn;
              
              /* increment the total number of bytes read in the packet */
              totalBytesRead++;
              
              /* check to see if we have enough bytes of the packet */
              /* to determine the total length of the packet */
              if (totalBytesRead == PKT_TYPE_SIZE_LENGTH)
                /* derive the packet size */
                pktSize = (ST_LONG *) &packetIn[PKT_TYPE_LENGTH];
              
              /* check to see if the totalBytesRead has met or exceeded */
              /* the number of bytes within the packets type and size */
              /* control block settings */
              if (totalBytesRead >= PKT_TYPE_SIZE_LENGTH)
                /* check to see if the totalBytesRead is equal to the packet size */
                if (totalBytesRead == *pktSize)
                  /* the packet has been read completely so break out of the while loop */
                  break;
  
              /* reset the total time between reads to zero */
              totalTimeBtwReads = 0;
              time (&timeCurrentRead);
              timeStartRead = timeCurrentRead;
              }
            else
              {
              /* calculate the totalTimeBtwReads */
              time (&timeCurrentRead);
              totalTimeBtwReads = timeCurrentRead - timeStartRead;
              }
            }
          }
        else
          {
          /* the packet is a stop or start packet */
          /* check to see if the packet is a start packet */
          if (packetIn[PKT_TYPE_INDEX] == START_PACKET)
            {
            /* the packet is a start packet so lets get the */
            /* character resolution that we will use for this */
            /* test */
            while ((totalBytesRead < NON_TEST_PACKET_SIZE) &&
                   (totalTimeBtwReads < MAX_READ_TIMEOUT))
              {
              /* try to read another character */
              getResult = sm_getc (port);
    
              /* check to see if a character was read */
              if (getResult != -1)
                {
                /* a character was read */
                charIn = getResult;
                
                /* set the next byte in the packet */
                *packetPtr++ = charIn;
                
                /* increment the total number of bytes read in the packet */
                totalBytesRead++;
                
                /* reset the total time between reads to zero */
                totalTimeBtwReads = 0;
                time (&timeCurrentRead);
                timeStartRead = timeCurrentRead;
                }
              else
                {
                /* calculate the totalTimeBtwReads */
                time (&timeCurrentRead);
                totalTimeBtwReads = timeCurrentRead - timeStartRead;
                }
              }
            }
          }
        }
      }  
    else
      {
      /* read for an incoming packet */
      
      /* set packetPtr to point to beginning of buffer */
      packetPtr = packetIn;
      
      /* attempt to read the beginning of a packet */
      result =  sm_get (port, packetPtr, MAX_PACKET_SIZE, &bytesRead);
      
      /* set the total bytes read to the number of bytes read */
      totalBytesRead = bytesRead;
      
      if ((result == SD_SUCCESS) && (totalBytesRead > 0))
        {
        /* we have successfully gotten the start of a packet */

        /* set pktReceived to true since we have received the */
        /* first part of a packet */
        pktReceived = SD_TRUE;
        
        /* lets find out how many bytes are total within this packet */
        pktSize = (ST_LONG *) &packetPtr[PKT_TYPE_LENGTH];
  
        /* set totalTimeBtwReads to zero */
        totalTimeBtwReads = 0;
        
        /* set the current read time */
        time (&timeCurrentRead);
        
        /* set the start read time equal to the current read time */
        timeStartRead = timeCurrentRead;
        
        if (packetIn[PKT_TYPE_INDEX] == TEST_PACKET)
          {
          /* now loop until we timeout or the packet is completely received */
          while ((totalBytesRead < *pktSize) &&
                 (totalBytesRead < MAX_PACKET_SIZE) &&
                 (totalTimeBtwReads < MAX_READ_TIMEOUT))
            {
            /* increment packetPtr by bytesRead */
            packetPtr += bytesRead;
            
            /* try to read some more of the packet */
            result = sm_get (port, packetPtr, MAX_PACKET_SIZE - totalBytesRead, &bytesRead);
            
            /* increment totalBytesRead by number of bytes read during read */
            totalBytesRead += bytesRead;
            
            if (result == SD_SUCCESS)
              {
              /* reset the total time between reads to zero */
              totalTimeBtwReads = 0;
              time (&timeCurrentRead);
              timeStartRead = timeCurrentRead;
              }
            else
              {
              /* calculate the totalTimeBtwReads */
              time (&timeCurrentRead);
              totalTimeBtwReads = timeCurrentRead - timeStartRead;
              }
            }
          }
        else
          {
          /* the packet is a stop or start packet */
          /* check to see if the packet is a start packet */
          if (packetIn[PKT_TYPE_INDEX] == START_PACKET)
            {
            /* the packet is a start packet so lets get the */
            /* character resolution that we will use for this */
            /* test */
            while ((totalBytesRead < NON_TEST_PACKET_SIZE) &&
                   (totalTimeBtwReads < MAX_READ_TIMEOUT))
              {
              packetPtr += bytesRead;
              result = sm_get (port, packetPtr, MAX_PACKET_SIZE - totalBytesRead, &bytesRead);
              totalBytesRead += bytesRead;
              
              if (result == SD_SUCCESS)
                {
                /* reset the total time between reads to zero */
                totalTimeBtwReads = 0;
                time (&timeCurrentRead);
                timeStartRead = timeCurrentRead;
                }
              else
                {
                /* calculate the totalTimeBtwReads */
                time (&timeCurrentRead);
                totalTimeBtwReads = timeCurrentRead - timeStartRead;
                }
              }
            }
          }
        }
      }        

    if (pktReceived == SD_TRUE)
      {
      /* log the incoming packet */
      DRVT_LOG5 ("Packet Received: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
                 port, baud, parity, data, stop);
      DRVT_HEX_LOG (totalBytesRead, packetIn);
  
      /* log the incoming packet source */
      DRVT_CLOG0 ("Packet received from client");
      
      /* check to see if the packet is a start test packet */
      if (packetIn[PKT_TYPE_INDEX] == START_PACKET)
        {
        /* start test packet received */
        DRVT_CLOG0 ("Start Test Packet received from client");
        
        /* reset the statistics */
        pktsSentSucc = 0;
        pktsSentFail = 0;
        pktsRcv = 0;
        sqncNum = 0;
        
        /* check to see if the start packet specifies character res */
        if (packetIn[PKT_CHAR_RES_INDEX] == CHARACTER_RES_ON)
          resolutionChar = SD_TRUE;
        else
          resolutionChar = SD_FALSE;
        }
  
      /* check to see if the packet is a stop test packet */
      if (packetIn[PKT_TYPE_INDEX] == STOP_PACKET)
        {
        /* stop test packet received */
        DRVT_CLOG0 ("Stop Test Packet received from client");
        
        /* log the statistics */
        DRVT_LOG1  ("Number of Test packets received           : %d",
                    pktsRcv);
        DRVT_CLOG1 ("Number of Test packets sent successfully  : %d",
                    pktsSentSucc);
        DRVT_CLOG1 ("Number of Test packets sent unsucessfully : %d",
                    pktsSentFail);
        }
  
      if (packetIn[PKT_TYPE_INDEX] == CTRL_LINE_TEST_PACKET)
        {
        /* control line test packet received */
        DRVT_CLOG0 ("Control line test packet received from client");
        
        /* set up the packet with control line states  */
        packetIn[INDEX_RTS] = sm_get_rts (port);
        packetIn[INDEX_DTR] = sm_get_dtr (port);
        packetIn[INDEX_CTS] = sm_get_cts (port);
        packetIn[INDEX_DSR] = sm_get_dsr (port);
        packetIn[INDEX_DCD] = sm_get_dcd (port);
        
        /* send the packet */
        result = sm_put (port, CTRL_LINE_TEST_PKT_LENGTH, packetIn);

        /* log the packet */
        DRVT_LOG5 ("Packet Sent: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
                   port, baud, parity, data, stop);
        DRVT_HEX_LOG (CTRL_LINE_TEST_PKT_LENGTH, packetIn);

        /* check to see if the packets was written correctly */
        if (result == SD_SUCCESS)
          {
          /* log that the packets was sent successfully */
          DRVT_CLOG0 ("Control line packet sent successfully");
          }
        else
          {
          /* log that the packets was sent unsuccessfully */
          DRVT_CLOG1 ("Failure to put control line packet; Error Code : %d", result);
          }
        }
  
      /* check to see if the packet is a test packet */
      if (packetIn[PKT_TYPE_INDEX] == TEST_PACKET)
        {
        /* increment frames received counter */
        pktsRcv++;
        
        /* set sqncNumRec to point to the sequence number within the packet */
        sqncNumRec = (ST_LONG *) &packetIn[PKT_TYPE_SIZE_LENGTH];
        
        /* check to see if the sequence numbers are mis-aligned */
        if (*sqncNumRec != sqncNum)
          {
          /* log the error in sequence numbers */
          DRVT_CLOG0  ("Error in Sequence Numbers!");

          /* log the expected and received sequence numbers */  
          DRVT_CLOG2 ("Expected Sequence Number : %d, Received Sequence Number : %d",
                      sqncNum, *sqncNumRec);
          }
        else
          {
          /* log the sequence number */
          DRVT_CLOG1 ("Sequence Number : %d", sqncNum);
          }
  
        if (resolutionChar == SD_TRUE)
          {
          /* echo back the packet */
          for (i = 0; i < totalBytesRead; i++)
            {
            result = sm_putc (port, packetIn[i]);
            if (result != SD_SUCCESS)
              /* a character wasn't written correctly so abort */
              /* sending the packet any further */
              break;
            }
          }
        else
          {
          /* echo back the packet */
          result = sm_put (port, totalBytesRead, packetIn);
          }
  
        /* log the packet */
        DRVT_LOG5 ("Packet Sent: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
                   port, baud, parity, data, stop);
        DRVT_HEX_LOG (totalBytesRead, packetIn);
        
        /* check to see if the packets was written correctly */
        if (result == SD_SUCCESS)
          {
          /* increment the frames sent successfully counter */
          pktsSentSucc++;
          
          /* log that the packets was sent successfully */
          DRVT_CLOG0 ("Packet sent successfully");
          }
        else
          {
          /* increment the packets sent unsuccessfully counter */
          pktsSentFail++;
  
          /* log that the packets was sent unsuccessfully */
          DRVT_CLOG1 ("Failure to put; Error Code : %d", result);
          }
        
        /* increment sequence counter */
        sqncNum++;
        }
      }
    }      

  /* terminate the session */
  sm_exit (port);
  }


#if defined (DEBUG_SISCO)

/************************************************************************/
/*                          m_set_log_config				*/
/* PURPOSE : This function will intialize the logging functionality of  */
/* the server                                                           */
/************************************************************************/

ST_RET 	m_set_log_config ()
  {
LOG_CTRL *lc;

  adlc_debug_sel = 0xFFFFFFFF;	/* turn on all logging	*/

  /* Initialize the logging for MMS-EASE */
  lc = sLogCtrl;

  /* If no file is present, use defaults */
  /* Use File logging */
  lc->logCtrl = LOG_FILE_EN;

  /* Use time/date time log */
  lc->logCtrl |= LOG_TIME_EN;

  /* File Logging Control defaults */
  lc->fc.fileName = "mms.log";
  lc->fc.maxSize = 250000L;
  lc->fc.ctrl = (FIL_CTRL_WIPE_EN |
                 FIL_CTRL_WRAP_EN |
                 FIL_CTRL_NO_APPEND |
                 FIL_CTRL_MSG_HDR_EN);


  return (SD_SUCCESS);
  }

#endif  
