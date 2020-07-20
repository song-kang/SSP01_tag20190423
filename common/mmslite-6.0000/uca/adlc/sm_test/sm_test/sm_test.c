/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1998, All Rights Reserved.		        	*/
/*									*/
/* MODULE NAME : sm_test.c						*/
/* PRODUCT(S)  : MMS-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 05/27/98  RWM            Created                                     */
/************************************************************************/

/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

#include "fkeydefs.h"   /* Function key handling */
#include "scrndefs.h"

/* Some other SISCO files						*/
#include "gen_list.h"

#include <windows.h>

/* ADLC files								*/
#include "adlc_sm.h"
#include "adlc_log.h"
#include "adlc_usr.h"

#include "mem_chk.h"

/************************************************************************/
/* Local Defines */

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;	/* Define for SLOG mechanism	*/


#define TOTAL_NUM_CLP 6 /* total number of command line parameters */

/* the following strings that are defined are used for the */
/* parsing of the command line parameters sent to this application */
#define PREFIX_PORT   "port"
#define PREFIX_BAUD   "baud"
#define PREFIX_PARITY "parity"
#define PREFIX_DATA   "data"
#define PREFIX_STOP   "stop"

/* total number of baud rates that are available */
#define NUM_BAUDS     15

/* maximum number of tests to perform */
#define MAX_TESTS     100

/* defines for both the maximum test packet size */
/* and the minimum packet size */
#define MIN_PACKET_SIZE   9
#define MAX_PACKET_SIZE   1000

/* length of the packets header containing the type of */
/* of the packet (1 byte) and the size of the packet (4 bytes) */
#define PKT_TYPE_SIZE_LENGTH 5

/* length of the packets header type parameter (1 byte) */
#define PKT_TYPE_LENGTH      1

/* length of a incoming packet from the server as a result */
/* of a control line test request */
#define CTRL_LINE_TEST_PKT_LENGTH 6

/* the following are indexes into a control line test packet */
/* for each seperate control line */
#define INDEX_RTS 1
#define INDEX_DTR 2
#define INDEX_CTS 3
#define INDEX_DSR 4
#define INDEX_DCD 5

/* byte index where the packet type is located */
#define PKT_TYPE_INDEX       0

/* code used to mark a packet as a test packet */
#define TEST_PACKET           2
#define CTRL_LINE_TEST_PACKET 3

/* the maximum time alloted waiting for a response */
/* to a test packet sent to the server */
#define MAX_TIMEOUT       10

/* the length of time to let the server initialize after */
/* sending a start packet to it */
#define MAX_INIT_TIMOUT   2

/* the maximum time alloted between consecutive reads */
/* while assembling a packet that is being read */
#define MAX_READ_TIMEOUT  5

/* the size of either a start or stop packet */
#define NON_TEST_PACKET_SIZE 2

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
                        
#define DRVT_LOG3(a,b,c,d) {\
                           if (loggingOn)\
                             {\
                             slog (sLogCtrl,DRV_TEST_TYPE,\
                                   thisFileName,__LINE__,a,b,c,d);\
                             }\
                           }
                         
                         
#define DRVT_LOG4(a,b,c,d,e) {\
                             if (loggingOn)\
                               {\
                               slog (sLogCtrl,DRV_TEST_TYPE,\
                                     thisFileName,__LINE__,a,b,c,d,e);\
                               }\
                             }

#define DRVT_LOG5(a,b,c,d,e,f) {\
                               if (loggingOn)\
                                 {\
                                 slog (sLogCtrl,DRV_TEST_TYPE,\
                                       thisFileName,__LINE__,a,b,c,d,e,f);\
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

typedef struct packetItem
  {
  ST_CHAR *packet;
  ST_INT  length;
  } PACKET_ITEM;

/************************************************************************/
ST_INT  port;   /* com port where all tests will take place */
ST_INT  baud;   /* baud rate of the serial connection */
ST_INT  parity; /* flag for parity (OFF, ODD, EVEN) */
ST_INT  data;   /* number of data bits (7, 8) */
ST_INT  stop;   /* number of stop bits (1, 2) */

ST_BOOLEAN doIt; /* variable that indicates that the application should */
                 /* continue to looping and accepting user input */

ST_BOOLEAN loggingOn; /* variable the indicates whether logging is enabled */

HANDLE kbdEvent; /* keyboard event handle */

/* array of valid baud rates */
ST_INT validBauds [] = {110, 150, 300, 600, 1200, 2400, 
                        4800, 9600, 19200, 38400, 57600,
                        115200, 56000, 128000, 256000}; 

/* start packet without character resolution */                        
ST_CHAR startPkt     [] = {0x00, 0x00};

/* stop packet without character resolution  */
ST_CHAR stopPkt      [] = {0x01, 0x00};

/* start packet with character resolution    */
ST_CHAR startPktChar [] = {0x00, 0x01};

/* stop packet with character resolution     */
ST_CHAR stopPktChar  [] = {0x01, 0x01};

/* control line test packet */
ST_CHAR lineTestPkt [] = {0x03};

/* test packets to be used during testing */
PACKET_ITEM testPackets [MAX_TESTS];

/* */
ST_CHAR snglPktCharTest [] = "SINGLE PACKET CHARACTER RES DRIVER TEST";
ST_CHAR mtplPktCharTest [] = "MULTIPLE PACKET CHARACTER RES DRIVER TEST";
ST_CHAR snglPktTest [] = "SINGLE PACKET DRIVER TEST";
ST_CHAR mtplPktTest [] = "MULTIPLE PACKET DRIVER TEST";
/************************************************************************/
ST_VOID printUsage ();
ST_RET 	m_set_log_config ();
ST_BOOLEAN getCommandLineParam (ST_CHAR *clp, ST_CHAR *prefix, ST_INT *value);
ST_VOID wait_any_event (ST_LONG max_ms_delay);
ST_VOID doMainMenu ();
ST_VOID initializeTestSet ();
ST_VOID doSinglePacketTest (ST_VOID);
ST_VOID doMultiplePacketTest (ST_VOID);
ST_VOID doSinglePacketCharResTest (ST_VOID);
ST_VOID doMultiplePacketCharResTest (ST_VOID);
ST_VOID doPacketSetTest (ST_BOOLEAN resolutionChar, ST_INT numTests,
                         ST_CHAR *testTitle);
ST_VOID doCtrlLineTest (ST_VOID);

ST_VOID nofun ();
ST_VOID (*menu_set_fun)() = nofun;

/* external functions */
ST_RET term_init (ST_VOID);
ST_BOOLEAN ask (ST_CHAR *question, ST_BOOLEAN default_ans);
/************************************************************************/

/************************************************************************/
/*                                nofun                                 */
/* This function does effectively nothing                               */
/************************************************************************/

ST_VOID nofun ()
  {

  }

/************************************************************************/
/*                                main                                  */
/* PURPOSE: This function is the main entry point for this application. */
/* The purpose of this function is to read the command line parameters, */
/* verify the command line parameters, set up logging confirguration,   */
/* initialize the com port that was specified within the command line,  */
/* print the main menu, and loop while querying for user input for the  */
/* menu system                                                          */
/* INPUT : argc - number of argument passed to main                     */
/*         *argv[] - array of strings pointers that constitute the      */
/*                   command line parameters sent to main               */
/* OUTPUT: NONE                                                         */
/************************************************************************/

ST_VOID main (ST_INT argc, ST_CHAR *argv[])
  {
ST_INT     i;  
ST_RET     result;
ST_BOOLEAN portRet; /* port was retrieved from command line */
ST_BOOLEAN baudRet; /* baud rate was retrieved from command line */
ST_BOOLEAN parityRet; /* parity was retrieved from the command line */
ST_BOOLEAN dataRet; /* data was retrieved from the command line */
ST_BOOLEAN stopRet; /* stop was retrieved from the command line */
  
  /* intialize the terminal */
  term_init();
  
  /* check to make sure that there was a command line parameter provided */
  if (argc < TOTAL_NUM_CLP)
    {
    /* no com port was provided */
    printUsage ();
    return;
    }
  
  /* check all of the command line parameters for a com port */
  for (i = 1; i < argc; i++)
    {
    if (portRet != SD_TRUE)
      portRet = getCommandLineParam (argv[i], PREFIX_PORT, &port);
    if (baudRet != SD_TRUE)
      baudRet = getCommandLineParam (argv[i], PREFIX_BAUD, &baud);
    if (parityRet != SD_TRUE)  
      parityRet = getCommandLineParam (argv[i], PREFIX_PARITY, &parity);
    if (dataRet != SD_TRUE)
      dataRet = getCommandLineParam (argv[i], PREFIX_DATA, &data);
    if (stopRet != SD_TRUE)
      stopRet = getCommandLineParam (argv[i], PREFIX_STOP, &stop);
    }

  /* check to see if all of the command line parameters were retrieved */
  if ((portRet && baudRet && parityRet && dataRet && stopRet) != SD_TRUE)
    {
    /* command line parameters not filled in correctly */
    printUsage ();
    return;
    }

  /* check to see if all of the command line parameter values are valid */
  if (port < 1)
    {
    /* invalid port entered */
    printf ("\n Invalid port entered.\n");
    printUsage ();
    return;
    }
  
  /* adjust port to be zero based (i.e. (port 1) <=> (0) */
  port -= 1;
  
  /* check to see if the baud rate is valid */ 
  for (i = 0; i < NUM_BAUDS; i++)
    {
    if (validBauds[i] == baud)
      break;
    }
  
  /* check to see if all possible bauds were checked */
  if (i == NUM_BAUDS)
    {
    /* invalid baud rate entered */
    printf ("\n Invalid baud entered.\n");
    printUsage ();
    return;
    }
  
  /* convert baud to a baud code */
  baud = i;
  
  /* check the parity */
  if ((parity < 0) || (parity > 2))
    {
    /* invalid parity flag entered */
    printf ("\n Invalid parity flag entered.\n");
    printUsage ();
    return;
    }

  /* check the data bits */
  if ((data < 7) || (data > 8))
    {
    /* invalid data bits entered */
    printf ("\n Invalid data bits entered.\n");
    printUsage ();
    return;
    }

  /* convert data bits to code */
  data -= 7;
  
  /* check the stop bits */
  if ((stop < 1) || (stop > 2))
    {
    /* invalid stop bits entered */
    printf ("\n Invalid stop bits entered.\n");
    printUsage ();
    return;
    }

  /* convert stop bits to code */
  stop -= 1; 
  
  /* ask the user if they would like logging turned on */
  loggingOn = ask (" Would you like to log while testing (Y) ? ", SD_TRUE);
    
  /* initialize logging parameters */
  m_set_log_config ();
  
  /* initialize the serial manager */
  result = sm_init (port, baud, parity, data, stop);
  
  if (result != SD_SUCCESS)
    {
    /* indicate that the initialization process failed */
    printf ("\n Initialization of COM %d failed; Retcode = %d", 
            (port + 1), result);
    printf ("\n Terminating SM DRIVER TEST...");
    DRVT_LOG2 ("Initialization of COM %d failed; Retcode = %d",
               (port + 1), result);
    DRVT_LOG0 ("Terminating SM DRIVER TEST...");
    }

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
    
  /* de-initialize port */  
  sm_exit (port);
  
  /* free all packets */
  for (i = 0; i < MAX_TESTS; i++)
    {
    if (testPackets[i].packet != NULL)
      chk_free (testPackets[i].packet);
    }
  }

/************************************************************************/
/*                              printUsage                              */
/* This function will print the usage of this application to the user   */ 
/* INPUT : NONE                                                         */
/* OUTPUT: NONE                                                         */
/************************************************************************/

ST_VOID printUsage ()
  {
  printf ("\n MMS SM DRIVER TEST");
  printf ("\n\n Usage : sm_test -port[] -baud[] -parity[] -data[] -stop[]");
  printf ("\n\n -port[port number]\t\tport number to perform the test on");
  printf ("\n -baud[baud rate]\t\tbaud rate to perform the test at");
  printf ("\n -parity[0|1|2]\t\t\t parity off [0], parity odd [1], parity even [2]");
  printf ("\n -data[7|8]\t\t\tspecify how many data bits will be used");
  printf ("\n -stop[1|2]\t\t\tspecify how many stop bits will be used");
  /*printf ("\n\n Example : sm_test -port1 -baud2400 -parity0 -data7 -stop1");*/
  printf ("\n\n Example : sm_test -port2 -baud9600 -parity0 -data8 -stop1");
  wait_msg ("");
  }

/************************************************************************/
/*                          getCommandLineParam                         */
/* PURPOSE : This function will extract a value from a command line     */
/* parameter (i.e. -port2 <=> COM2) and set the integer pointed to by   */
/* *value with the value if and only if the character string pointed to */
/* by *prefix is found within the character string pointed to *clp      */
/* INPUT : *clp - a pointer to a command line parameter (i.e. -port2)   */
/*         *prefix - a pointer to the prefix to search for within *clp  */
/*         *value - a pointer to an integer whose contents will be set  */
/*                  if the *prefix is found within *clp                 */
/* OUTPUT : SD_TRUE - if the function set the integer pointed to by     */
/*                    value                                             */
/*          SD_FALSE - if the function does not set the integer pointed */
/*                     to by value                                      */
/************************************************************************/

ST_BOOLEAN getCommandLineParam (ST_CHAR *clp, ST_CHAR *prefix, ST_INT *value)
  {
ST_CHAR *prefixPos;
  
  /* check to see if parameter has correct prefix */
  prefixPos = strstr(clp, prefix);
  
  /* check to see if the prefix does not exist */
  if (prefixPos == NULL)
    /* we failed to extract the clp from this string */
    return (SD_FALSE);
  
  /* point clp to integer value of the port */
  clp = prefixPos;
  clp += strlen (prefix);
  
  /* check to see if the character provided is an integer */
  if (isdigit (*clp) == SD_FALSE)
    return (SD_FALSE);
  
  /* set the integer pointed to by *value */
  *value = atoi (clp);
  
  /* return that the function successfully set the integer */
  /* pointed to by *value                                  */
  return (SD_TRUE);
  }

/************************************************************************/
/*                              doMainMenu                              */
/* PURPOSE : This function will print the main menu of the application  */ 
/* INPUT : NONE                                                         */
/* OUTPUT : NONE                                                        */
/************************************************************************/

ST_VOID doMainMenu ()
  {
  CLEARSCR;
  printf ("\n\n");
  printf ("\n**********************************************************");
  printf ("\n***                  MMS CLNP DRIVER TEST              ***");
  printf ("\n**********************************************************");
  printf ("\n\t          SELECT DESIRED ACTIVITY\n\n");
  printf ("\t  F1  - SINGLE PACKET CHARACTER RES DRIVER TEST\n");
  printf ("\t  F2  - MULTIPLE PACKET CHARACTER RES DRIVER TEST\n");
  printf ("\t  F3  - SINGLE PACKET DRIVER TEST\n");
  printf ("\t  F4  - MULTIPLE PACKET DRIVER TEST\n");
  printf ("\t  F5  - DISPLAY CONTROL LINE STATES\n");
  printf ("\t  F10 - EXIT DRIVER TEST\n");
  
  flush_keys ();
  fun_null ();
  
  funct_1 = doSinglePacketCharResTest;
  funct_2 = doMultiplePacketCharResTest;
  funct_3 = doSinglePacketTest;
  funct_4 = doMultiplePacketTest;
  funct_5 = doCtrlLineTest;
  
  menu_set_fun = doMainMenu;     /* used to reset the menu	*/
  }


/************************************************************************/
/*                      doSinglePacketCharResTest                       */
/* PURPOSE : This function will call doPacketSetTest and specify that   */
/* the server receive and send its packets using the functions sm_getc  */
/* and sm_putc respectively. It will specify that doPacketSetTest only  */
/* send one test packet                                                 */
/* INPUT : NONE                                                         */
/* OUTPUT : NONE                                                        */
/************************************************************************/

ST_VOID doSinglePacketCharResTest (ST_VOID)
  {
  doPacketSetTest (SD_TRUE, 1, snglPktCharTest);
  }

/************************************************************************/
/*                     doMultiplePacketCharResTest                      */
/* PURPOSE : This function will call doPacketSetTest and specify that   */
/* the server receive and send its packets using the functions sm_getc  */
/* and sm_putc respectively. It will specify that doPacketSetTest       */
/* send MAX_TESTS test packets                                          */
/* INPUT : NONE                                                         */
/* OUTPUT : NONE                                                        */
/************************************************************************/

ST_VOID doMultiplePacketCharResTest (ST_VOID)
  {
  doPacketSetTest (SD_TRUE, MAX_TESTS, mtplPktCharTest);
  }

/************************************************************************/
/*                          doSinglePacketTest                          */
/* PURPOSE : This function will call doPacketSetTest and specify that   */
/* the server receive and send its packets using the functions sm_get   */
/* and sm_put respectively. It will specify that doPacketSetTest only   */
/* send 1 test packet                                                   */
/* INPUT : NONE                                                         */
/* OUTPUT : NONE                                                        */
/************************************************************************/

ST_VOID doSinglePacketTest (ST_VOID)
  {
  doPacketSetTest (SD_FALSE, 1, snglPktTest);
  }

/************************************************************************/
/*                          doSinglePacketTest                          */
/* PURPOSE : This function will call doPacketSetTest and specify that   */
/* the server receive and send its packets using the functions sm_get   */
/* and sm_put respectively. It will specify that doPacketSetTest        */
/* send MAX_TESTS test packets                                          */
/* INPUT : NONE                                                         */
/* OUTPUT : NONE                                                        */
/************************************************************************/

ST_VOID doMultiplePacketTest (ST_VOID)
  {
  doPacketSetTest (SD_FALSE, MAX_TESTS, mtplPktTest);
  }
  

  
/************************************************************************/
/*                           doPacketSetTest                            */
/* PURPOSE : This function will perform the test process of sending     */
/* packets to the server and receiving packets from the server. It will */
/* keep the statistics of the number of packets sent successfully, sent */
/* unsucessfully, received successfully, timeout waiting to be received,*/
/* received with invalid lengths, and received with invalid contents.   */
/* This function will either specify that the server use (sm_get and    */
/* sm_put) or (sm_getc and sm_putc) during the testing process          */
/* INPUT : resolutionChar - if SD_TRUE then the test will specify to    */
/*                          server that the functions sm_getc and       */
/*                          sm_putc should be used during the testing   */
/*                          procedure instead of the functions sm_get   */
/*                          and sm_put                                  */
/*         numTests - the number of test packets to be sent and         */
/*                    received during the testing process               */
/************************************************************************/

ST_VOID doPacketSetTest (ST_BOOLEAN resolutionChar, ST_INT numTests, 
                         ST_CHAR *testTitle)
  {
time_t      timeStart;
time_t      timeCurrent;
time_t      timeElapsed;
time_t      timeStartRead;
time_t      timeCurrentRead;
time_t      totalTimeBtwReads;
ST_RET      result;
ST_INT      i;
ST_INT      numSuccessSnd;
ST_INT      numFailureSnd;
ST_INT      numSuccessRcv;
ST_INT      numTimeoutRcv;
ST_INT      numInvLngthRcv;
ST_INT      numInvPduCntsRcv;
ST_BOOLEAN  packetRcvd;
ST_CHAR     packetIn [MAX_PACKET_SIZE];
ST_CHAR     *packetPtr;
ST_INT      bytesRead;
ST_INT      totalBytesRead;
ST_LONG     *pktSize;
  
  /* initialize statistics for test */
  numSuccessSnd = 0; /* number of pdus successfully sent */
  numFailureSnd = 0; /* number of pdus failed to be sent */
  numSuccessRcv = 0; /* number of pdus successfully received */
  numTimeoutRcv = 0; /* number of pdus timed out on waiting to be received */
  numInvLngthRcv = 0; /* number of pdus received with invalid lengths */
  numInvPduCntsRcv = 0; /* number of pdus received with invalid contents */
  
  /* initialize the test packets to be sent and received */
  initializeTestSet ();
  
  /* check to see if the resolution is character specific */
  if (resolutionChar == SD_TRUE)
    /* send a character resolution start packet to start the test */
    result = sm_put (port, NON_TEST_PACKET_SIZE, startPktChar);
  else
    /* send a start packet to start the test */
    result = sm_put (port, NON_TEST_PACKET_SIZE, startPkt);
  
  /* log the start packet */
  DRVT_LOG5 ("Packet Sent: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
             port, baud, parity, data, stop);

  if (resolutionChar == SD_TRUE)
    {
    /* log the character resolution stop packet */
    DRVT_HEX_LOG (NON_TEST_PACKET_SIZE, startPktChar);
    }
  else
    {
    /* log the stop packet */
    DRVT_HEX_LOG (NON_TEST_PACKET_SIZE, startPkt);
    }
    
  /* check to see if the put was a success */
  if (result == SD_SUCCESS)
    {
    printf ("\n Start Test Packet sent successfully\n");
    DRVT_CLOG0 ("Start Test Packet sent successfully");
    }
  else
    {
    printf ("\n Failure on sending Start Test Packet; Retcode = %d\n", result);
    DRVT_CLOG1 ("Failure on sending Start Test Packet; Retcode = %d", result);
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

  printf ("\n Performing Test : %s\n", testTitle);
  DRVT_LOG1 ("Performing Test : %s", testTitle);
  
  /* send out the packets and wait to receieve them in order */
  for (i = 0; i < numTests; i++)
    {
    /* initialize time elapsed to zero */
    timeElapsed = 0;
    
    /* set the packets received flag to false */
    packetRcvd = SD_FALSE;
    
    /* send out the packet */
    result = sm_put (port, 
                     testPackets[i].length, 
                     testPackets[i].packet);

    /* log the packet */
    DRVT_LOG5 ("Packet Sent: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
               port, baud, parity, data, stop);
    DRVT_HEX_LOG (testPackets[i].length, testPackets[i].packet);

    /* check to see if the put was a success */
    if (result == SD_SUCCESS)
      {
      /* log the success of sending the packet */
      DRVT_CLOG0 ("Packet sent successfully");
      numSuccessSnd++;
      }
    else
      {
      /* log the failure of sending the packet */
      DRVT_CLOG1 ("Failure on sending Packet; Retcode = %d", result);
      numFailureSnd++;
      break;
      }
      
    /* get the time that the packet was sent out */
    time (&timeStart);
    
    /* loop until timeElapsed >= MAX_TIMEOUT */
    while (timeElapsed < MAX_TIMEOUT)
      {
      /* read for the packet that was passed out */
      packetPtr = packetIn;
      result =  sm_get (port, packetPtr, MAX_PACKET_SIZE, &bytesRead);
      totalBytesRead = bytesRead;
      
      /* check to see if the get was a success if the number */
      /* of bytes read is greater than zero */
      if ((result == SD_SUCCESS) && (totalBytesRead > 0))
        {
        /* we have successfully gotten the start of a packet */
        /* now lets find out how many bytes are total within this packet */

        /* set totalTimeBtwReads to zero */
        totalTimeBtwReads = 0;
        time (&timeCurrentRead);
        timeStartRead = timeCurrentRead;
        
        /* now loop until we timeout or the packet is completely received */
        /* or our buffer runs out of room */
        while ((totalBytesRead < MAX_PACKET_SIZE) &&
               (totalTimeBtwReads < MAX_READ_TIMEOUT))
          {
          packetPtr += bytesRead;
          result = sm_get (port, packetPtr, MAX_PACKET_SIZE - totalBytesRead, &bytesRead);
         
          /* calculate the total bytes read so far */
          totalBytesRead += bytesRead;
          
          if (totalBytesRead >= PKT_TYPE_SIZE_LENGTH)
            {
            /* derive the packet size */
            pktSize = (ST_LONG *) &packetIn[PKT_TYPE_LENGTH];
            if (totalBytesRead == *pktSize)
              /* the packet has been read completely so break out of the while loop */
              break;
            }

          if ((result == SD_SUCCESS) && (bytesRead > 0))
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

        /* log the packet received */
        DRVT_LOG5 ("Packet Received: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
                   port, baud, parity, data, stop);
        DRVT_HEX_LOG (totalBytesRead, packetIn);
        
        /* compare the sequence numbers */
        if (i != (ST_LONG) testPackets[i].packet[PKT_TYPE_SIZE_LENGTH])
          {
          /* log the error in sequence numbers */
          DRVT_CLOG0  ("Error in Sequence Numbers!");
  
          /* log the expected and received sequence numbers */  
          DRVT_CLOG2 ("Expected Sequence Number : %d, Received Sequence Number : %d",
                      i, (ST_LONG) testPackets[i].packet[PKT_TYPE_SIZE_LENGTH]);
          }
        else
          {
          /* log the sequence number */
          DRVT_CLOG1 ("Sequence Number : %d", i);
          
          /* check the size of both packets */
          if (totalBytesRead == testPackets[i].length)
            {
            /* sizes are the same, now lets check the packets themselves */
            if (memcmp(testPackets[i].packet, packetIn, totalBytesRead) == 0)
              {
              /* This is the packet that we sent out! Hooray! */
              DRVT_CLOG0("Packet received correctly");
              numSuccessRcv++;
              }
            else
              {
              /* This is not the pdu that we sent out. */
              DRVT_CLOG0("Packet received incorrectly; Invalid packet contents");
              numInvPduCntsRcv++;
              }
            }
          else
            {
            /* This is not the packet that we sent out. */
            DRVT_CLOG0("Packet received incorrectly; Invalid length");
            numInvLngthRcv++;
            }
          }
  
        /* set the pdu received flag to true */    
        packetRcvd = SD_TRUE;
        break;
        }
      
      /* get the current time */
      time (&timeCurrent);
      
      /* derive the time elapsed between the current time and the time */
      /* that the pdu was sent out on the wire */
      timeElapsed = timeCurrent - timeStart;
      }
    
    /* check to see if we timed out waiting for the pdu to */  
    /* be returned */
    if (packetRcvd != SD_TRUE)
      {
      /* the timeout elapsed so log an error */
      DRVT_LOG0 ("Timeout waiting for echo from server");
      DRVT_HEX_LOG (testPackets[i].length, testPackets[i].packet);
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

  /* check to see if the resolution is character specific */
  if (resolutionChar == SD_TRUE)
    /* send a character resolution stop packet to start the test */
    result = sm_put (port, NON_TEST_PACKET_SIZE, stopPktChar);
  else
    /* send a start packet to stop the test */
    result = sm_put (port, NON_TEST_PACKET_SIZE, stopPkt);

  /* log the stop packet */
  DRVT_LOG5 ("Packet Sent: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
             port, baud, parity, data, stop);
  
  if (resolutionChar == SD_TRUE)
    {
    /* log the character resolution stop packet */
    DRVT_HEX_LOG (NON_TEST_PACKET_SIZE, stopPktChar);
    }
  else
    {
    /* log the stop packet */
    DRVT_HEX_LOG (NON_TEST_PACKET_SIZE, stopPkt);
    }
  
  /* check to see if the put was a success */
  if (result == SD_SUCCESS)
    {
    printf ("\n Stop Test Packet sent successfully\n");
    DRVT_CLOG0 ("Stop Test Packet sent successfully");
    }
  else
    {
    printf ("\n Failure on sending Stop Test Packet; Retcode = %d\n", result);
    DRVT_CLOG1 ("Failure on sending Stop Test Packet; Retcode = %d", result);
    }
    
  printf ("\n Test Results\n");
  DRVT_LOG0  ("Test Results\n");
    
  printf ("\n   Number of Packets sent successfully :              %d", 
          numSuccessSnd);
  printf ("\n   Number of Packets received successfully :          %d", 
          numSuccessRcv);
  printf ("\n   Number of Packets failed to be sent :              %d", 
          numFailureSnd);
  printf ("\n   Number of Packets timed out receiving :            %d", 
          numTimeoutRcv);
  printf ("\n   Number of Packets received with invalid lengths :  %d",
          numInvLngthRcv);
  printf ("\n   Number of Packets received with invalid contents : %d\n",
          numInvPduCntsRcv);

  /* log the results */
  DRVT_CLOG1 ("  Number of Packets sent successfully :              %d",
              numSuccessSnd);
  DRVT_CLOG1 ("  Number of Packets received successfully :          %d",
              numSuccessRcv);
  DRVT_CLOG1 ("  Number of Packets failed to be sent :              %d",
              numFailureSnd); 
  DRVT_CLOG1 ("  Number of Packets timed out receiving :            %d",
              numTimeoutRcv);
  DRVT_CLOG1 ("  Number of Packets received with invalid lengths :  %d",
              numInvLngthRcv);
  DRVT_CLOG1 ("  Number of Packets received with invalid contents : %d",
              numInvPduCntsRcv);

  wait_msg ("Done performing test.");
  
  (*menu_set_fun) ();
  }

/************************************************************************/
/*                            doCtrlLineTest                            */
/************************************************************************/

ST_VOID doCtrlLineTest (ST_VOID)
  {
ST_RET  result;
time_t  timeStart;
time_t  timeCurrent;
time_t  timeElapsed;
time_t  timeStartRead;
time_t  timeCurrentRead;
time_t  totalTimeBtwReads;
ST_INT  bytesRead;
ST_INT  totalBytesRead;
ST_CHAR packetIn [MAX_PACKET_SIZE];
ST_CHAR *packetPtr;

  result = sm_put (port, 1, lineTestPkt);

  /* log the control line test packet */
  DRVT_LOG5 ("Packet Sent: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
             port, baud, parity, data, stop);

  DRVT_HEX_LOG (1, lineTestPkt);
             
  if (result == SD_SUCCESS)
    {
    printf ("\n Control Line Test Packet sent successfully\n");
    DRVT_CLOG0 ("Control Line Test Packet sent successfully");
    }
  else
    {
    printf ("\n Failure on sending Control Line Test Packet; Retcode = %d\n", result);
    DRVT_CLOG1 ("Failure on sending Control Line Packet; Retcode = %d", result);
    }

  printf ("\n Waiting for response from server...\n");
  DRVT_LOG0 ("Waiting for response from server...");
  
  /* get the time that the packet was sent out */
  time (&timeStart);

  /* initialize timeElapsed to zero */
  timeElapsed = 0;
  
  /* loop until timeElapsed >= MAX_TIMEOUT */
  while (timeElapsed < MAX_TIMEOUT)
    {
    /* read for the packet that was passed out */
    packetPtr = packetIn;
    result =  sm_get (port, packetPtr, MAX_PACKET_SIZE, &bytesRead);
    totalBytesRead = bytesRead;
    
    /* check to see if the get was a success if the number */
    /* of bytes read is greater than zero */
    if ((result == SD_SUCCESS) && (totalBytesRead > 0))
      {
      /* we have successfully gotten the start of a packet */
      /* now lets find out how many bytes are total within this packet */

      /* set totalTimeBtwReads to zero */
      totalTimeBtwReads = 0;
      time (&timeCurrentRead);
      timeStartRead = timeCurrentRead;
      
      /* now loop until we timeout or the packet is completely received */
      /* or our buffer runs out of room */
      while ((totalBytesRead < CTRL_LINE_TEST_PKT_LENGTH) &&
             (totalTimeBtwReads < MAX_READ_TIMEOUT))
        {
        packetPtr += bytesRead;
        result = sm_get (port, packetPtr, MAX_PACKET_SIZE - totalBytesRead, &bytesRead);
       
        /* calculate the total bytes read so far */
        totalBytesRead += bytesRead;
        
        if ((result == SD_SUCCESS) && (bytesRead > 0))
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

      /* log the packet received */
      DRVT_LOG5 ("Packet Received: port = %d, baud = %d, parity = %d, data = %d, stop = %d",
                 port, baud, parity, data, stop);
      DRVT_HEX_LOG (totalBytesRead, packetIn);
      
      break;
      }
    
    /* get the current time */
    time (&timeCurrent);
    
    /* derive the time elapsed between the current time and the time */
    /* that the pdu was sent out on the wire */
    timeElapsed = timeCurrent - timeStart;
    }
    
  if (totalBytesRead > 0)
    {
    /* verify that the control line test packet is valid */
    if (totalBytesRead == CTRL_LINE_TEST_PKT_LENGTH)
      {
      /* the length of the control line packet is valid */
      /* now check that the packet type is correct */
      if (packetIn[PKT_TYPE_INDEX] == CTRL_LINE_TEST_PACKET)
        {
        /* the packet type is correct */
        /* now lets examine the states of the control lines */
        printf ("\n Server Control Line States :");
        DRVT_LOG0 ("Server Control Line States :");
        
        if (packetIn[INDEX_RTS] == 0)
          {
          /* RTS is low */
          printf ("\n   RTS Line : Low");
          DRVT_CLOG0 ("  RTS Line : Low");
          }
        else
          {
          if (packetIn[INDEX_RTS] == 1)
            {
            /* RTS is high */
            printf ("\n   RTS Line : High");
            DRVT_CLOG0 ("  RTS Line : High");
            }
          else
            {
            /* Invalid RTS value */
            printf ("\n   RTS Line : Invalid state reported");
            DRVT_CLOG0 ("  RTS Line : Invalid state reported");
            }
          }
        if (packetIn[INDEX_DTR] == 0)
          {
          /* DTR is low */
          printf ("\n   DTR Line : Low");
          DRVT_CLOG0 ("  DTR Line : Low");
          }
        else
          {
          if (packetIn[INDEX_DTR] == 1)
            {
            /* DTR is high */
            printf ("\n   DTR Line : High");
            DRVT_CLOG0 ("  DTR Line : High");
            }
          else
            {
            /* Invalid DTR value */
            printf ("\n   DTR Line : Invalid state reported");
            DRVT_CLOG0 ("  DTR Line : Invalid state reported");
            }
          }
        if (packetIn[INDEX_CTS] == 0)
          {
          /* CTS is low */
          printf ("\n   CTS Line : Low");
          DRVT_CLOG0 ("  CTS Line : Low");
          }
        else
          {
          if (packetIn[INDEX_CTS] == 1)
            {
            /* CTS is high */
            printf ("\n   CTS Line : High");
            DRVT_CLOG0 ("  CTS Line : High");
            }
          else
            {
            /* Invalid CTS value */
            printf ("\n   CTS Line : Invalid state reported");
            DRVT_CLOG0 ("  CTS Line : Invalid state reported");
            }
          }
        if (packetIn[INDEX_DSR] == 0)
          {
          /* DSR is low */
          printf ("\n   DSR Line : Low");
          DRVT_CLOG0 ("  DSR Line : Low");
          }
        else
          {
          if (packetIn[INDEX_DSR] == 1)
            {
            /* DSR is high */
            printf ("\n   DSR Line : High");
            DRVT_CLOG0 ("  DSR Line : High");
            }
          else
            {
            /* Invalid DSR value */
            printf ("\n   DSR Line : Invalid state reported");
            DRVT_CLOG0 ("  DSR Line : Invalid state reported");
            }
          }
        if (packetIn[INDEX_DCD] == 0)
          {
          /* DCD is low */
          printf ("\n   DCD Line : Low");
          DRVT_CLOG0 ("  DCD Line : Low");
          }
        else
          {
          if (packetIn[INDEX_DCD] == 1)
            {
            /* DCD is high */
            printf ("\n   DCD Line : High");
            DRVT_CLOG0 ("  DCD Line : High");
            }
          else
            {
            /* Invalid DCD value */
            printf ("\n   DCD Line : Invalid state reported");
            DRVT_CLOG0 ("  DCD Line : Invalid state reported");
            }
          }
        }
      else
        {
        /* print and log that the packet is not a control line packet */
        printf ("\n Invalid control line test packet");
        DRVT_CLOG0 ("Invalid control line test packet");
        }
      }
    else
      {
      /* print and log that the control line packet is incomplete */
      printf ("\n Invalid control line test packet; Invalid length");
      DRVT_CLOG0 ("Invalid control line test packet; Invalid length");
      }
    }
  else
    {
    /* log that the response was not received by the server */
    printf ("\n Timeout waiting for Control Line response from server");
    DRVT_LOG0 ("Timeout waiting for Control Line response from server");
    }
  
  printf ("\n\n Actual Control Line States : ");
  DRVT_CLOG0 ("Actual Control Line States : ");
  
  if (sm_get_rts (port) == 1)
    {
    printf ("\n   RTS Line : High");
    DRVT_CLOG0 ("  RTS Line : High");
    }
  else
    {
    printf ("\n   RTS Line : Low");
    DRVT_CLOG0 ("  RTS Line : Low");
    }

  if (sm_get_dtr (port) == 1)
    {
    printf ("\n   DTR Line : High");
    DRVT_CLOG0 ("  DTR Line : High");
    }
  else
    {
    printf ("\n   DTR Line : Low");
    DRVT_CLOG0 ("  DTR Line : Low");
    }
  
  if (sm_get_cts (port) == 1)
    {
    printf ("\n   CTS Line : High");
    DRVT_CLOG0 ("  CTS Line : High");
    }
  else
    {
    printf ("\n   CTS Line : Low");
    DRVT_CLOG0 ("  CTS Line : Low");
    }

  if (sm_get_dsr (port) == 1)
    {
    printf ("\n   DSR Line : High");
    DRVT_CLOG0 ("  DSR Line : High");
    }
  else
    {
    printf ("\n   DSR Line : Low");
    DRVT_CLOG0 ("  DSR Line : Low");
    }
  
  if (sm_get_dcd (port) == 1)
    {
    printf ("\n   DCD Line : High\n");
    DRVT_CLOG0 ("  DCD Line : High");
    }
  else
    {
    printf ("\n   DCD Line : Low\n");
    DRVT_CLOG0 ("  DCD Line : Low");
    }

  wait_msg ("Done performing test.");

  (*menu_set_fun) ();
  }

/************************************************************************/
/*                          initializeTestSet                           */
/* PURPOSE : This function will intialize the test packets that will be */
/* used during the testing process between this application and the     */
/* server.                                                              */
/* INPUT : NONE                                                         */
/* OUTPUT : NONE                                                        */
/************************************************************************/

ST_VOID initializeTestSet ()
  {
ST_INT  i;
ST_INT  j;
ST_INT  randSize;
ST_LONG *sqncNum;
ST_LONG *pktSize;

  /* free any packets */
  for (i = 0; i < MAX_TESTS; i++)
    {
    if (testPackets[i].packet != NULL)
      chk_free (testPackets[i].packet);
    }
  
  /* First packet is smallest possible packet */
  testPackets[0].length = MIN_PACKET_SIZE;
  testPackets[0].packet = chk_malloc (MIN_PACKET_SIZE);
  
  /* Second packet is largest possible packet */
  testPackets[1].length = MAX_PACKET_SIZE;
  testPackets[1].packet = chk_malloc (MAX_PACKET_SIZE);
  
  /* seed the random functionality */
  srand( (unsigned)time( NULL ) );
  
  /* remaining packets are of random sizes */  
  for (i = 2; i < MAX_TESTS; i++)
    {
    /* set the packet sizes randomly from MIN_PACKET_SIZE to MAX_PACKET_SIZE */
    randSize = (rand () % (MAX_PACKET_SIZE - MIN_PACKET_SIZE)) + MIN_PACKET_SIZE;
    testPackets[i].length = randSize;
    testPackets[i].packet = chk_malloc (randSize);
    }
  
  /* set the packet contents */
  for (i = 0; i < MAX_TESTS; i++)
    {
    /* set the packet as a test packet */
    testPackets[i].packet[PKT_TYPE_INDEX] = TEST_PACKET;
    
    /* store the packet size in the packet */
    pktSize = (ST_LONG *) &testPackets[i].packet[PKT_TYPE_LENGTH];
    *pktSize = testPackets[i].length;
    
    /* set the sequence number */
    sqncNum = (ST_LONG *) &testPackets[i].packet[PKT_TYPE_SIZE_LENGTH];
    *sqncNum = i;
    
    /* fill in the rest of the packet */
    for (j = MIN_PACKET_SIZE; j < testPackets[i].length; j++)
      {
      testPackets[i].packet[j] = rand ();
      }
    }
  }
  
  
/************************************************************************/
/*                          m_set_log_config				*/
/* PURPOSE : This function will intialize the logging functionality of  */
/* the client                                                           */
/* INPUT : NONE                                                         */
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

/************************************************************************/
/*                          wait_any_event				*/
/* PURPOSE : The following function is essential for detecting keyboard */
/* events created by the user                                           */
/* INPUT : max_ms_delay - the maximum number of ms to wait for a key    */
/*                        to be struck.                                 */
/* OUTPUT : NONE                                                        */
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
  
