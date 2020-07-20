/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2002-2006, All Rights Reserved					*/
/*								      	*/
/* MODULE NAME : iec_demo.c				    	      	*/
/* PRODUCT(S)  : MMSEASE-LITE					      	*/
/*								      	*/
/* MODULE DESCRIPTION : 					      	*/
/*								      	*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :			      	*/
/*	NONE								*/						
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/17/12  JRB	   Increase delay between GOOSE to see max retrans.*/
/*			   Avoid overflow on scanf call.		*/
/* 01/12/10  JRB     21    Fix log messages.				*/
/* 08/01/08  JRB     20    Don't destroy Pub/Sub here. Destroy them in	*/
/*			   iec_main.c where they are created.		*/
/* 07/02/08  JRB     19    create_subscription..: chg ConfRev to ST_UINT32*/
/* 06/20/08  JRB     18    Use logcfgx to configure IEC_GOOSE log masks.*/
/* 05/14/08  GLB     17    Added slog_ipc_std_cmd_service		*/
/* 10/30/06  JRB     16    Use new mvl_vmd_* object handling functions.	*/
/* 08/09/06  JRB     15    Use SCL to configure (define USE_OLD_CONFIG	*/
/*			   to use old configuration code).		*/
/*			   Use iecGooseSubscribe2 (much simpler).	*/
/*			   Move mutex create/destroy to main.		*/
/*			   Don't use u_mvl_get_nvl (not needed). Use	*/
/*			    only mvl_find_nvl.				*/
/* 06/23/06  MDE     14    Added time_t cast				*/
/* 08/01/05  JRB     13    Fix printf & LOG format.			*/
/* 02/08/05  JRB     12    Del broken iecGooseReleaseAll, instead call	*/
/*			   iecGoosePubDestroy or iecGooseUnSubscribe.	*/
/*			   Del get_msg_thread (never worked).		*/
/*			   Subscriber didn't work as thread on Windows	*/
/*			   because subnet completion routines never	*/
/*			   called, so don't allow it.			*/
/*			   Use same "decode_mode" for all subscriptions.*/
/*			   Move IEC_GOOSE_LOG_*_TEXT to iec_comn.c so	*/
/*			   other applications can use them.		*/
/*			   Fix logging in callback_function.		*/
/* 10/07/04  JRB     11    Call get_goose_messages for ALL systems.	*/
/*			   Fix retrans and TAL timers.			*/
/* 09/01/04  JRB     10    Add "sx_arb.h".				*/
/* 05/19/04  JRB     09    Use "retrans_thread" only if USE_FRAMEWORK..	*/
/*			   defined (some systems don't have threads).	*/
/* 11/06/03  JRB     08    Always use "retrans_thread". 		*/
/*			   Wake up "retrans_thread" w/ retrans_event_sem*/
/*			   Use sMsSleep.				*/
/* 10/09/03  JRB     07    Use new shorter log mask names.		*/
/* 01/13/03  ASK     06    Remove 'Both' option in demo_init if single  */
/*			   threaded. More cleanup.			*/
/* 01/10/03  JRB     05    Del all refs to dataRef, elementId struct	*/
/*			   members & related code.			*/
/*			   Del unneeded args from iecGooseSubscribe.	*/
/*			   Make function & flag names more consistent.	*/
/* 12/10/02  ASK     04    Changes for new IEC Goose encoding, update   */
/*			   utctime vars. Change logging to use 		*/
/*			   SXD_ARB_DATA_CTRL, misc cleanup		*/
/* 03/06/02  JRB     03    Disable threads.				*/
/* 02/20/01  JRB     02    Fix format.					*/
/* 02/14/02  HSF     01    Created					*/
/************************************************************************/
#include "iec_demo.h"
#include "glbsem.h"
#include "mvl_defs.h"
#include "sx_arb.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__; 
#endif

extern ST_BOOLEAN doIt;
extern SXD_ARB_DATA_CTRL mlog_arb_log_ctrl;

#if defined(USE_OLD_CONFIG)	/* DEBUG: should old config code be deleted?*/

/* This demo code is intended to be executed as both  */
/* a publisher and subscriber independantly (e.g. separate executables)  */
/* both sides shall be driven by a DataSet whose name is "mydom/mydataset".  */
/* this can be changed by changing the sub_datasetRef initialization value  */
/* the appropriate DataSet must be created with Foundry  */

/*goose to be subscribed for  */
ST_CHAR sub_datasetRef[] = "mydom/mydataset";
ST_CHAR sub_gcRef[] = "mydom/mygcRef";
ST_CHAR sub_AppID[] = "testAppID";
ST_UCHAR sub_mac[MAC_ADDR_SIZE] = {0x09, 0x00,0x00,0x00,0x00,0x01};

/*goose to be subscribed for  */
ST_CHAR sub_datasetRef1[] = "mydom/mydataset1";
ST_CHAR sub_gcRef1[] = "mydom/mygcRef1";
ST_CHAR sub_AppID1[] = "testAppID1";
ST_UCHAR sub_mac1[MAC_ADDR_SIZE] = {0x09, 0x00,0x00,0x00,0x00,0x01};

/*goose to be subscribed for  */
ST_CHAR sub_datasetRef2[] = "mydom/mydataset2";
ST_CHAR sub_gcRef2[] = "mydom/mygcRef2";
ST_CHAR sub_AppID2[] = "testAppID2";
ST_UCHAR sub_mac2[MAC_ADDR_SIZE] = {0x09, 0x00,0x00,0x00,0x00,0x02};

/*information for first GOOSE to be published  */
ST_CHAR pub_datasetRef[] = "mydom/mydataset";
ST_CHAR pub_gcRef[] = "mydom/mygcRef";
ST_CHAR pub_AppID[] = "testAppID";
ST_UINT16 pub_tci = ETYPE_TCI_GOOSE;
ST_UINT16 pub_etypeID =	ETYPE_TYPE_GOOSE;
ST_UINT16 pub_appID = 0x0000;
ST_UCHAR pub_mac[MAC_ADDR_SIZE] = {0x09, 0x00,0x00,0x00,0x00,0x01};

/*information for second GOOSE to be published  */
ST_CHAR pub_datasetRef1[] = "mydom/mydataset1";
ST_CHAR pub_gcRef1[] = "mydom/mygcRef1";
ST_CHAR pub_AppID1[] = "testAppID1";
ST_UINT16 pub_tci1 = ETYPE_TCI_GOOSE;
ST_UINT16 pub_etypeID1 = ETYPE_TYPE_GOOSE;
ST_UINT16 pub_appID1 = 0x0101;
ST_UCHAR pub_mac1[MAC_ADDR_SIZE] = {0x09, 0x00,0x00,0x00,0x00,0x01};

/*information for third GOOSE to be published  */
ST_CHAR pub_datasetRef2[] = "mydom/mydataset2";
ST_CHAR pub_gcRef2[] = "mydom/mygcRef2";
ST_CHAR pub_AppID2[] = "testAppID2";
ST_UINT16 pub_tci2 = ETYPE_TCI_GOOSE;
ST_UINT16 pub_etypeID2 = ETYPE_TYPE_GOOSE;
ST_UINT16 pub_appID2 = 0x0202;
ST_UCHAR pub_mac2[MAC_ADDR_SIZE] = {0x09, 0x00,0x00,0x00,0x00,0x02};
#endif	/* defined(USE_OLD_CONFIG)	*/


#define SUB_CONF_REV_NUM 32
#define PUB_CONF_REV_NUM 32
#define RETRANS_RATE 4		/* How often to chk for retransmission (msec)*/

RETRANS_CURVE retrans_curve;
ST_EVENT_SEM retrans_event_sem;	/* retrans thread event semaphore*/

/**********************************************************/
/*		init_retrans_curve()					*/
/* Application code to initialize a default retans curve  */
/**********************************************************/
ST_VOID init_retrans_curve(RETRANS_CURVE *retrans_curve)
  {
  /*initializes default retransmission curve  */
  retrans_curve->num_retrans = 16;
  retrans_curve->retrans[0] = 2+MAX_IO_SCAN_TIME;
  retrans_curve->retrans[1] = 2+MAX_IO_SCAN_TIME;
  retrans_curve->retrans[2] = 4+MAX_IO_SCAN_TIME;
  retrans_curve->retrans[3] = 8;
  retrans_curve->retrans[4] = 16;
  retrans_curve->retrans[5] = 32;
  retrans_curve->retrans[6] = 64;
  retrans_curve->retrans[7] = 128;
  retrans_curve->retrans[8] = 256;
  retrans_curve->retrans[9] = 512;
  retrans_curve->retrans[10] = 1024;
  retrans_curve->retrans[11] = 2048;
  retrans_curve->retrans[12] = 4096;
  retrans_curve->retrans[13] = 8192;
  retrans_curve->retrans[14] = 16384;
  retrans_curve->retrans[15] = 32768;
  }

/********************************************************************/
/*	display_to_screen_log_fun				*/
/* function that is called to display logged buffers to the screen	*/
/********************************************************************/
ST_VOID display_to_screen_log_fun (LOG_CTRL *lc, 
			SD_CONST ST_INT logType, 
			SD_CONST ST_CHAR *SD_CONST sourceFile, 
			SD_CONST ST_INT lineNum,
			SD_CONST ST_INT bufLen, 
			SD_CONST ST_CHAR *buf)
{
  printf("%s",buf);
}

#ifdef USE_FRAMEWORK_THREADS
/********************************************************************/
/*				retrans_thread										*/
/* thread that sleeps and then checks for RETRANSMISSIONS			*/
/********************************************************************/
ST_THREAD_RET ST_THREAD_CALL_CONV retrans_thread(ST_THREAD_ARG ta) 
{
ST_LONG min_retrans_timeout = -1L;	/* start with "infinite" timeout*/

  while(doIt)
    {
    /* Wait as long as possible on global "retrans_event_sem".*/
    gs_wait_event_sem (retrans_event_sem, min_retrans_timeout);
    /* Every time retrans_goose is called, timeout is adjusted as needed.*/
    min_retrans_timeout = retrans_goose(min_retrans_timeout);
    }
  gs_free_event_sem (retrans_event_sem);
  return ST_THREAD_RET_VAL;
}
#endif	/* USE_FRAMEWORK_THREADS	*/

#if defined(USE_OLD_CONFIG)
/********************************************************************/
/*				execute_as_publisher								*/
/* function that causes the demo to act as a publisher of IEC		*/
/********************************************************************/
ST_THREAD_RET ST_THREAD_CALL_CONV execute_as_publisher (ST_THREAD_ARG ta) 
{
IEC_GOOSE_SEND_USER_INFO *goosehandle=NULL;
IEC_GOOSE_SEND_USER_INFO *goosehandle1=NULL;
IEC_GOOSE_SEND_USER_INFO *goosehandle2=NULL;
time_t time1,time2;
GSE_IEC_CTRL *gptr,*gptr1,*gptr2;
RETRANS_CURVE retrans_curve;	/*storage for retransmission curve to be used*/
#ifdef USE_FRAMEWORK_THREADS
ST_THREAD_HANDLE thread1Handle;
ST_THREAD_ID thread1Id;
#endif
ST_DOUBLE TimeOld;
ST_DOUBLE TimeNew;
ST_LONG   TimeElapsed;

  printf("\n\nexecuting as publisher.....");

  init_retrans_curve(&retrans_curve);

  /*set up 1st goose, goose will retransmit and have stNum changed   */
  /*every two minutes  */
  if( ( goosehandle = iecGoosePubCreate(pub_mac,pub_gcRef,pub_datasetRef,pub_AppID,PUB_CONF_REV_NUM,
        SD_TRUE, pub_tci, pub_etypeID, pub_appID)) ==NULL)
    return ST_THREAD_RET_VAL;

  gptr = goosehandle->gptr;
  gptr->test = SD_FALSE;

  /*set up 2nd goose, goose will retansmit and then be removed  */
  /*from the pool after 2 minutes.  Publishes to the same  */
  /*MAC address as the first GOOSE (used to test reception and timeout code)  */
  if( ( goosehandle1 = iecGoosePubCreate(pub_mac1,pub_gcRef1,pub_datasetRef1,pub_AppID1,PUB_CONF_REV_NUM,
        SD_TRUE, pub_tci1, pub_etypeID1, pub_appID1)) ==NULL)
    {
    iecGoosePubDestroy (goosehandle);
    return ST_THREAD_RET_VAL;
    }

  gptr1 = goosehandle1->gptr;
  gptr1->test = SD_TRUE;

  /*set up 3rd goose, goose will retansmit only.  */
  /*has the same dataset as the second goose  */
  if( ( goosehandle2 = iecGoosePubCreate(pub_mac2,pub_gcRef2,pub_datasetRef2,pub_AppID2,PUB_CONF_REV_NUM,
       SD_TRUE, pub_tci2, pub_etypeID2, pub_appID2)) ==NULL)
    {
    /* Destroy 1st and 2nd goose.	*/
    iecGoosePubDestroy (goosehandle);
    iecGoosePubDestroy (goosehandle1);
    return ST_THREAD_RET_VAL;
    }
  gptr2 = goosehandle2->gptr;
  gptr2->test = SD_TRUE;

  /* set up the initial event time stamp  */
  gptr2->utcTime.secs = gptr1->utcTime.secs = gptr->utcTime.secs = (ST_UINT32) time(NULL);
  gptr->utcTime.fraction = 0;
  gptr->utcTime.qflags = 0x0;

#ifdef USE_FRAMEWORK_THREADS
  /* Init global retransmit event semaphore	*/
  retrans_event_sem = gs_get_event_sem (SD_FALSE);/*auto reset*/

  gs_start_thread (&retrans_thread, (ST_THREAD_ARG)NULL, 
		 &thread1Handle, &thread1Id);
#endif

  iecGoosePublish (goosehandle);
  iecGoosePublish (goosehandle1);
  iecGoosePublish (goosehandle2);
  time1 = time(NULL);

  TimeOld = sGetMsTime ();	/* init old time	*/
  while(doIt == SD_TRUE)	/* if false, CTRL+C was pressed */
    {
    sMsSleep(RETRANS_RATE);

#ifndef USE_FRAMEWORK_THREADS			
    TimeNew = sGetMsTime ();				/* get current time*/
    TimeElapsed = (ST_LONG) (TimeNew - TimeOld);	/* calc elapsed time*/
    TimeOld = TimeNew;					/* update old time*/
    retrans_goose(TimeElapsed);
#endif

    time2 = time(NULL);

    if((time2-time1) >= 300)		/*wait a while to change data  */
      {
      /*set up the event time stamp  */
      gptr->utcTime.secs = (ST_UINT32) time(NULL);
      gptr->utcTime.fraction = 0;
      gptr->utcTime.qflags = 0x0;

      iecGoosePublish (goosehandle);
      iecGoosePublish (goosehandle2);
      time1 = time2;

      if(goosehandle1)
        {
        printf("\n\nDestroying one Publishing Goose");
        iecGoosePublish (goosehandle1);		/*done to get an immediate timeout  */
        iecGoosePubDestroy (goosehandle1);
        goosehandle1=NULL;
        }
      }
    }
  /* Destroy all publishers.	*/
  if (iecGoosePubDestroy (goosehandle) !=SD_SUCCESS)
    printf("\n\nunable to remove goosehandle");
  if (goosehandle1!=NULL && iecGoosePubDestroy (goosehandle1) !=SD_SUCCESS)
    printf("\n\nunable to remove goosehandle1");
  if (iecGoosePubDestroy (goosehandle2) !=SD_SUCCESS)
    printf("\n\nunable to remove goosehandle2");

  assert (pub_ctrl.pub_list == NULL);	/* make sure list cleaned up	*/

  return ST_THREAD_RET_VAL;
}
#endif	/* defined(USE_OLD_CONFIG)	*/


/************************************************************************/
/*				callback_function			*/
/* User function called by IEC GOOSE Framework when a GOOSE message	*/
/* has been received. This example simply logs the received GOOSE message.*/
/************************************************************************/
ST_VOID callback_function(IEC_GOOSE_RX_USER_INFO *goosehandle, 
			GSE_IEC_CTRL *gptr, ST_VOID *user, ST_UINT16 reason)
{
ST_INT numDataEntries,i;
GSE_IEC_DATA_ENTRY *DataEntry;
ST_INT rtlen;
RUNTIME_TYPE *rt_array;
  
  /* log this first so continuation logs don't get appended other logs*/
  IEC_GOOSE_LOG_FLOW1 ("IEC GOOSE Event Received (reason = 0x%X)", reason);

  if(reason & GOOSE_CALLBACK_REASON_CONFREV_MISMATCH)
    {
    IEC_GOOSE_LOG_CFLOW0 ("Configuration Revision Mismatch");
    return;
    }

  if (reason & GOOSE_CALLBACK_REASON_TIMEALLOWED_TO_LIVE_TIMEOUT)
    {	
    IEC_GOOSE_LOG_CFLOW3("GOOSE timeAllowedToLive=%lu (ms) Expired [DataSetRef:%s, GcRef:%s]",
			(ST_ULONG)gptr->timeToLive, gptr->dataSetRef, gptr->gcRef);
    printf("\n\nGOOSE timeAllowedToLive=%lu (ms) Expired [DataSetRef:%s, GcRef:%s]",
			(ST_ULONG)gptr->timeToLive, gptr->dataSetRef, gptr->gcRef);
    return;
    }

  if (reason & GOOSE_CALLBACK_REASON_OUT_OF_SEQUENCE_DETECTED)
    IEC_GOOSE_LOG_CFLOW0("Out of Sequence");

  if (reason & GOOSE_CALLBACK_REASON_NEED_COMMISSIONING)
    IEC_GOOSE_LOG_CFLOW0("Needs Commissioning");

  if(reason & GOOSE_CALLBACK_REASON_TEST_MODE)
    IEC_GOOSE_LOG_CFLOW0("Test Mode");

  if(reason & GOOSE_CALLBACK_REASON_GCREF_MISMATCH)
    IEC_GOOSE_LOG_CFLOW0("GcRef mismatch");

  if (reason & GOOSE_CALLBACK_REASON_APPID_MISMATCH)
    IEC_GOOSE_LOG_CFLOW0("AppID mismatch");

  if (reason & GOOSE_CALLBACK_REASON_DATSET_MISMATCH)
    IEC_GOOSE_LOG_CFLOW0("Dataset mismatch");

  if (reason & GOOSE_CALLBACK_REASON_STATECHANGE_DATA_UPDATED)
    {
    /* Log the dataEntries contained in the received GOOSE.	*/
    numDataEntries = gptr->numDataEntries;
    DataEntry = gptr->dataEntries;
    for(i=0;i<numDataEntries; ++i)
      {
      if(DataEntry->userInfo != NULL)
        {	/* DataEntry is valid	*/
        IEC_GOOSE_LOG_CFLOW1 ("Data Updated for dataEntries [%d]", i);

        rtlen = DataEntry->numRuntimeTypes;
        rt_array = DataEntry->runtimeTypeHead;

        /* This logs the data according to the type defined in DataEntry->runtimeTypeHead.*/
        /* NOTE: user may process the data differently by replacing	*/
        /* mlog_arb_log_ctrl with a different function pointer array.	*/
        sxd_process_arb_data(DataEntry->dataBuf,rt_array,rtlen,NULL,&mlog_arb_log_ctrl,NULL);
        }
      else
        {	/* NOTE: this should almost never happen	*/
        IEC_GOOSE_LOG_CFLOW1 ("Data could not be decoded for dataEntries [%d]", i);
        }
      ++DataEntry;
      }
    }
}


/********************************************************************/
/*				create_subscription_from_nvl						*/
/* function that causes the demo to act as a subscriber of IEC		*/
/********************************************************************/
IEC_GOOSE_RX_USER_INFO *create_subscription_from_nvl(ST_CHAR *dataSetRef, 
						   ST_CHAR *gcRef,
						   ST_CHAR *AppID,
						   ST_UINT32 ConfRev,
						   ST_INT  mode_of_decode,
						   ST_UCHAR *mac)
{
  ST_INT j;
  IEC_GOOSE_RX_USER_INFO *goosehandle = NULL;
  OBJECT_NAME nvl_obj;
  MVL_NVLIST_CTRL *nvl_ptr;
  ST_INT *typeid_array;		/* allocated array of typeids	*/
  ST_CHAR domain[MAX_IDENT_LEN+1];
  ST_CHAR item[MAX_IDENT_LEN+1];


  nvl_obj.domain_id = domain;
  nvl_obj.obj_name.item_id = item;

  /*find the data set via the reference  */
  /*need to convert reference first  */
  if(convert_reference_to_mms_obj( dataSetRef, &nvl_obj)==NULL)
    return(goosehandle);

  /*now find the actual namevariable list  */
  if( (nvl_ptr = mvl_vmd_find_nvl(&mvl_vmd, &nvl_obj, NULL))==NULL)
    return(goosehandle);

  /* allocate temporary typeid array and fill it in	*/
  typeid_array = (ST_INT *) chk_malloc (nvl_ptr->num_of_entries * sizeof(ST_INT));
  for(j=0; j< nvl_ptr->num_of_entries; j++)
    typeid_array [j] = nvl_ptr->entries[j]->type_id;

  goosehandle = iecGooseSubscribe2(mac,
       	 gcRef,
       	 dataSetRef,
       	 AppID,
       	 ConfRev,
       	 nvl_ptr->num_of_entries,
       	 typeid_array,
       	 callback_function,
       	 mode_of_decode);		
  chk_free (typeid_array);

  return(goosehandle);
}

/********************************************************************/
/*		io_scan_simulator					*/
/* thread that decodes messages based upon an I/O scan of 5 seconds	*/
/********************************************************************/
ST_THREAD_RET ST_THREAD_CALL_CONV io_scan_simulator(ST_THREAD_ARG ta) 
{
ST_INT stuff_to_do = SD_TRUE;

  while(stuff_to_do == SD_TRUE)
    {
    sMsSleep(10000);		/*sleep for a while	*/
    iecGooseLastRxDecode();			/*nothing left to scan  */
    }

  return ST_THREAD_RET_VAL;
}


#if defined(USE_OLD_CONFIG)
/********************************************************************/
/*				execute_as_subscriber								*/
/* function that causes the demo to act as a subscriber of IEC		*/
/********************************************************************/
ST_THREAD_RET ST_THREAD_CALL_CONV execute_as_subscriber (ST_THREAD_ARG ta) 
{
#if 1	/* DEBUG: try both modes.	*/
  ST_INT decode_mode = GOOSE_DEC_MODE_IMMEDIATE;
#else
  ST_INT decode_mode = GOOSE_DEC_MODE_LAST_RX;
#endif
  IEC_GOOSE_RX_USER_INFO *goosehandle = NULL;
  IEC_GOOSE_RX_USER_INFO *goosehandle1 = NULL;
  IEC_GOOSE_RX_USER_INFO *goosehandle2 = NULL;
  IEC_GOOSE_RX_USER_INFO *goosehandle3 = NULL;
#ifdef USE_FRAMEWORK_THREADS
  ST_THREAD_HANDLE thread1Handle;
  ST_THREAD_ID thread1Id;
#endif
ST_DOUBLE TimeOld;
ST_DOUBLE TimeNew;
ST_DOUBLE TimeLastDecode;
ST_LONG   TimeElapsed;

  printf("\n\nexecuting as subscriber.....");

  /*set up the first subscription  */
  if( (goosehandle = create_subscription_from_nvl(sub_datasetRef, 
				sub_gcRef,
				sub_AppID,
				SUB_CONF_REV_NUM,
				decode_mode,
				sub_mac)) == NULL)
    printf("\nUnable to create subscription for: %s",sub_datasetRef);
 

  if( (goosehandle1 = create_subscription_from_nvl(sub_datasetRef1,
 					sub_gcRef1,
       					sub_AppID1,
       					SUB_CONF_REV_NUM,
					decode_mode,
       					sub_mac1)) == NULL)
    printf("\nUnable to create subscription for: %s",sub_datasetRef);

  if( (goosehandle2 = create_subscription_from_nvl(sub_datasetRef2, 
      					sub_gcRef2,
       					sub_AppID2,
       					SUB_CONF_REV_NUM,
					decode_mode,
       					sub_mac2)) == NULL)
    printf("\nUnable to create subscription for: %s",sub_datasetRef);


   /*this one to be created and immediately destroyed (just for testing)  */
   if( (goosehandle3 = create_subscription_from_nvl(sub_datasetRef2, 
       					sub_gcRef2,
       					sub_AppID2,
       					SUB_CONF_REV_NUM,
					decode_mode,
       					sub_mac2)) == NULL)
    printf("\nUnable to create subscription for: %s",sub_datasetRef);


  if((goosehandle3!=NULL) && (iecGooseUnSubscribe(goosehandle3)!=SD_SUCCESS))
    printf("\n\nunable to remove goosehandle3");

  /*start a thread to receive goose  */

#ifdef USE_FRAMEWORK_THREADS
  /*scan to decode GEESE on a scan basis  */
  gs_start_thread (&io_scan_simulator, (ST_THREAD_ARG)NULL, 
       &thread1Handle, &thread1Id);
#endif

  TimeOld = TimeLastDecode = sGetMsTime ();	/* init old time	*/
  while(doIt == SD_TRUE) 	 /* if false, CTRL+C was pressed */
    {
    get_goose_messages();

    sMsSleep(1);				/*sleep the thread for 1 msec  */

    TimeNew = sGetMsTime ();				/* get current time*/
    TimeElapsed = (ST_LONG) (TimeNew - TimeOld);	/* calc elapsed time*/
    TimeOld = TimeNew;					/* update old time*/
    chk_iec_goose_timeout (TimeElapsed);	/* look for expired TAL	*/

#if !defined(USE_FRAMEWORK_THREADS)
    /* If threads enabled, this is done by io_scan_simulator thread.*/
    if(decode_mode == GOOSE_DEC_MODE_LAST_RX)
      {
      if (TimeNew > TimeLastDecode + 10000)	/* simulate user decode every 10 seconds*/
        {
        iecGooseLastRxDecode();
        TimeLastDecode = TimeNew;
        }
      }
#endif
    }

  /* Destroy all subscribers.	*/
  if (iecGooseUnSubscribe (goosehandle) !=SD_SUCCESS)
    printf("\n\nunable to remove goosehandle");
  if (iecGooseUnSubscribe (goosehandle1) !=SD_SUCCESS)
    printf("\n\nunable to remove goosehandle1");
  if (iecGooseUnSubscribe (goosehandle2) !=SD_SUCCESS)
    printf("\n\nunable to remove goosehandle2");

  assert (sub_ctrl.sub_list == NULL);	/* make sure list cleaned up	*/

  return ST_THREAD_RET_VAL;
}
#endif	/* defined(USE_OLD_CONFIG)	*/

#if !defined(USE_OLD_CONFIG)
/************************************************************************/
/*				execute_as_publisher			*/
/* Publish IEC GOOSE messages.						*/
/* This version of the function uses SCL to configure.			*/
/************************************************************************/
ST_THREAD_RET ST_THREAD_CALL_CONV execute_as_publisher (ST_THREAD_ARG ta) 
{
IEC_GOOSE_SEND_USER_INFO *goosehandle;
time_t time1,time2;
RETRANS_CURVE retrans_curve;	/*storage for retransmission curve to be used*/
#ifdef USE_FRAMEWORK_THREADS
ST_THREAD_HANDLE thread1Handle;
ST_THREAD_ID thread1Id;
#endif
ST_DOUBLE TimeOld;
#if !defined(USE_FRAMEWORK_THREADS)
ST_DOUBLE TimeNew;
ST_LONG   TimeElapsed;
#endif

  printf("\n\nexecuting as publisher.....");

  /* NOTE: Already initialized GOOSE Publishers in main (based on SCL config)*/

  init_retrans_curve(&retrans_curve);

#ifdef USE_FRAMEWORK_THREADS
  /* Init global retransmit event semaphore	*/
  retrans_event_sem = gs_get_event_sem (SD_FALSE);/*auto reset*/

  gs_start_thread (&retrans_thread, (ST_THREAD_ARG)NULL, 
		 &thread1Handle, &thread1Id);
#endif

  /* Publish any GOOSE configured in SCL file.	*/
  for (goosehandle = pub_ctrl.pub_list;
       goosehandle != NULL;
       goosehandle = (IEC_GOOSE_SEND_USER_INFO *) list_get_next (pub_ctrl.pub_list, goosehandle))
    {
    goosehandle->gptr->utcTime.secs = (ST_UINT32) time(NULL);	/* use current time in each GOOSE*/
    iecGoosePublish (goosehandle);
    }
  time1 = time(NULL);

  TimeOld = sGetMsTime ();	/* init old time  	*/
  while(doIt)       	        /* if false, CTRL+C was pressed */
    {
    sMsSleep(RETRANS_RATE);

#ifndef USE_FRAMEWORK_THREADS			
    TimeNew = sGetMsTime ();				/* get current time*/
    TimeElapsed = (ST_LONG) (TimeNew - TimeOld);	/* calc elapsed time*/
    TimeOld = TimeNew;					/* update old time*/
    retrans_goose(TimeElapsed);
#endif

    time2 = time(NULL);

    if((time2-time1) >= 300)		/* wait a while to change data	*/
      {		/* just publish first GOOSE on list	*/
      /*set up the event time stamp  */
      pub_ctrl.pub_list->gptr->utcTime.secs = (ST_UINT32) time(NULL);
      pub_ctrl.pub_list->gptr->utcTime.fraction = 0;
      pub_ctrl.pub_list->gptr->utcTime.qflags = 0x0;

      iecGoosePublish (pub_ctrl.pub_list);
      time1 = time2;
      }

      /* At runtime, periodically need to service SLOG commands and calling connections.   */
      /* The timing of this service is not critical, but to be responsive a default of     */
      /* 100ms works well.                                                                 */
#if defined (DEBUG_SISCO)
      slog_ipc_std_cmd_service ("logcfg.xml", NULL, NULL, SD_TRUE,  NULL, NULL);
#endif

   }	/* end "main" loop	*/

  return ST_THREAD_RET_VAL;
}

/************************************************************************/
/*				execute_as_subscriber			*/
/* Subscribe to receive IEC GOOSE messages.				*/
/* This version of the function uses SCL to configure.			*/
/************************************************************************/
ST_THREAD_RET ST_THREAD_CALL_CONV execute_as_subscriber (ST_THREAD_ARG ta) 
{
#if 1	/* DEBUG: try both modes.	*/
  ST_INT decode_mode = GOOSE_DEC_MODE_IMMEDIATE;
#else
  ST_INT decode_mode = GOOSE_DEC_MODE_LAST_RX;
#endif
#ifdef USE_FRAMEWORK_THREADS
  ST_THREAD_HANDLE thread1Handle;
  ST_THREAD_ID thread1Id;
#endif
ST_DOUBLE TimeOld;
ST_DOUBLE TimeNew;
ST_DOUBLE TimeLastDecode;
ST_LONG   TimeElapsed;

  printf("\n\nexecuting as subscriber.....");

  /* NOTE: Already initialized GOOSE Subscribers in main (based on SCL config)*/

#ifdef USE_FRAMEWORK_THREADS
  /*scan to decode GEESE on a scan basis  */
  gs_start_thread (&io_scan_simulator, (ST_THREAD_ARG)NULL, 
       &thread1Handle, &thread1Id);
#endif

  TimeOld = TimeLastDecode = sGetMsTime ();	/* init old time	*/
  while(doIt) 	                        /* if false, CTRL+C was pressed */
    {
    get_goose_messages();

    /* Give up CPU, but not too long. Need to wake up			*/
    /* often to check for timeout (see chk_iec_goose_timeout below).	*/
    sMsSleep(1);

    TimeNew = sGetMsTime ();				/* get current time*/
    TimeElapsed = (ST_LONG) (TimeNew - TimeOld);	/* calc elapsed time*/
    TimeOld = TimeNew;					/* update old time*/
    chk_iec_goose_timeout (TimeElapsed);	/* look for expired TAL	*/

#if !defined(USE_FRAMEWORK_THREADS)
    /* If threads enabled, this is done by io_scan_simulator thread.*/
    if(decode_mode == GOOSE_DEC_MODE_LAST_RX)
      {
      if (TimeNew > TimeLastDecode + 10000)	/* simulate user decode every 10 seconds*/
        {
        iecGooseLastRxDecode();
        TimeLastDecode = TimeNew;
        }
      }
#endif

      /* At runtime, periodically need to service SLOG commands and calling connections.   */
      /* The timing of this service is not critical, but to be responsive a default of     */
      /* 100ms works well.                                                                 */
#if defined (DEBUG_SISCO)
      slog_ipc_std_cmd_service ("logcfg.xml", NULL, NULL, SD_TRUE,  NULL, NULL);
#endif

   }

  return ST_THREAD_RET_VAL;
}

#endif	/* !defined(USE_OLD_CONFIG)	*/

/********************************************************************/
/*	demo_init						*/
/* function that initializes demo and then executes it		*/
/********************************************************************/
ST_VOID demo_init(ST_VOID)
{
ST_CHAR inp_string[10];
#ifdef USE_FRAMEWORK_THREADS
ST_RET ret;
ST_THREAD_HANDLE thread1Handle;
ST_THREAD_ID thread1Id;
#endif

  init_retrans_curve(&retrans_curve);

#if defined(USE_FRAMEWORK_THREADS)
  printf("\n\nAct as an IEC GOOSE Publisher? (P=Publisher, S=Subscriber, B=Both)");
#else
  printf("\n\nAct as an IEC GOOSE Publisher? (P=Publisher, S=Subscriber)");
#endif
/*	slog_dyn_log_fun = display_to_screen_log_fun;	     */	/*set logging to display to screen  */
  scanf("%1s",inp_string);  /* Get only the first input character.  */

  /*start a thread and then return  */
  if( ( toupper(inp_string[0])=='P') || ( toupper(inp_string[0])=='B'))
#if defined(USE_FRAMEWORK_THREADS)
    ret = gs_start_thread (&execute_as_publisher, (ST_THREAD_ARG)NULL, 
       &thread1Handle, &thread1Id);
#else
    execute_as_publisher ( (ST_THREAD_ARG)NULL);
#endif

  if( ( toupper(inp_string[0])=='S') || ( toupper(inp_string[0])=='B'))
    /* NOTE: Can't spawn it as a thread on Windows because subnet	*/
    /* completion routines wouldn't get called.				*/
    execute_as_subscriber ( (ST_THREAD_ARG)NULL);

#if defined(USE_FRAMEWORK_THREADS)
  /* Main thread must sleep forever, but wake up periodically to	*/
  /* check for Ctrl-C.							*/
  while (doIt)
    {
    sMsSleep(2000);
    }
  /* Wake up retrans_thread so it can finish.	*/
  gs_signal_event_sem (retrans_event_sem);

  sMsSleep(1000);	/* let other threads finish	*/
#endif
}
