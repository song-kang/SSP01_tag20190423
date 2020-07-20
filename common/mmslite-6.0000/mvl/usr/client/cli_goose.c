/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2005, All Rights Reserved					*/
/*								      	*/
/* MODULE NAME : cli_goose.c				    	      	*/
/* PRODUCT(S)  : MMSEASE-LITE					      	*/
/*								      	*/
/* MODULE DESCRIPTION : 					      	*/
/*	Sample code for subscribing to IEC 61850 GOOSE messages.	*/
/*								      	*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :			      	*/
/*			callback_function				*/
/*			goose_init					*/						
/*									*/
/* NOTE: create_subscription_from_nvl in GOOSE Framework requires too	*/
/*   much MVL Server code. It also requires client to create dummy vars	*/
/*   and NVL that it shouldn't need.  It only needs to know the "types"	*/
/*   of the variables received in the GOOSE message. Client apps should	*/
/*   instead use the new function "iecGooseSubscribe2" as in this sample.*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/08/05  JRB     02    Del clnp_snet_read_hook_add call. Call it	*/
/*			   from  client.c to allow other protocols.	*/
/* 07/22/05  JRB     01    Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "sx_arb.h"	/* need sxd_process_arb_data	*/
#include "client.h"
#include "iec_demo.h"	/* definitions from "iecgoose" sample app	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#if defined(USE_FRAMEWORK_THREADS)
#error This code not tested in 'threaded' mode. Undefine USE_FRAMEWORK_THREADS.
#endif

/* Need this global from mlogavar.c to pass to sxd_process_arb_data.	*/
extern SXD_ARB_DATA_CTRL mlog_arb_log_ctrl;

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
  
  IEC_GOOSE_LOG_FLOW0 ("IEC GOOSE Event Received");	/* log this first so continuation*/
							/* logs don't get appended other logs*/
  IEC_GOOSE_LOG_CFLOW1 ("reason = 0x%X", reason);

  if(reason & GOOSE_CALLBACK_REASON_CONFREV_MISMATCH)
    {
    IEC_GOOSE_LOG_CFLOW0 ("Configuration Revision Mismatch");
    return;
    }

  if (reason & GOOSE_CALLBACK_REASON_TIMEALLOWED_TO_LIVE_TIMEOUT)
    {	
    IEC_GOOSE_LOG_CFLOW3("GOOSE timeAllowedToLive=%u (ms) Expired [DatSet:%s, GoCBRef:%s]",
			gptr->timeToLive, gptr->dataSetRef, gptr->gcRef);
    printf("\n\nGOOSE timeAllowedToLive=%u (ms) Expired [DatSet:%s, GoCBRef:%s]",
			gptr->timeToLive, gptr->dataSetRef, gptr->gcRef);
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

/************************************************************************/
/*			fill_typeid_array				*/
/************************************************************************/
ST_RET fill_typeid_array (ST_CHAR **typename_array, ST_INT *typeid_array, ST_INT num_entries)
  {
ST_RET ret = SD_SUCCESS;
ST_INT j;

  for(j=0; j < num_entries; j++)
    {
    typeid_array [j] = mvl_typename_to_typeid (typename_array[j]);
    if (typeid_array [j] < 0)
      {
      SLOGALWAYS1 ("Cannot find type id for type name='%s'", typename_array[j]);
      ret = SD_FAILURE;
      break;
      }
    }
  return (ret);
  }

/************************************************************************/
/*			goose_init					*/
/* This function subscribes for the GOOSE message that is sent by the	*/
/* "IEC GOOSE Framework" sample app (source code in "iecgoose" directory).*/
/*									*/
/* NOTE: decode_mode = GOOSE_DEC_MODE_LAST_RX may be a bit confusing to	*/
/*   use because last PDU may be decoded many times with same sqNum.	*/
/*   Therefore, it is NOT used in this sample application.		*/
/************************************************************************/
IEC_GOOSE_RX_USER_INFO *goose_init (ST_VOID)	
{
/* Information needed for GOOSE subscription. Alternatively, this	*/
/* information could be obtained from a config file or passed as args.	*/
ST_CHAR sub_datasetRef[] = "mydom/mydataset";
ST_CHAR sub_gcRef[] = "mydom/mygcRef";
ST_CHAR sub_AppID[] = "testAppID";
ST_UCHAR sub_mac[MAC_ADDR_SIZE] = {0x09, 0x00,0x00,0x00,0x00,0x01};
ST_INT32 sub_confRev = 32;
/* Type names used here (phv_type, etc.) defined in ODF file.	*/
ST_CHAR *typename_array [] ={"phv_type", "phsx_type", "phsx_type"};
ST_INT decode_mode = GOOSE_DEC_MODE_IMMEDIATE;

/* Other variables	*/
IEC_GOOSE_RX_USER_INFO *goosehandle = NULL;
ST_INT typeid_array [10];
ST_INT numDataSetEntries;

  numDataSetEntries = sizeof (typename_array)/sizeof (ST_CHAR *);

  /*set the logging  */
  iec_goose_usr_debug |= IEC_GOOSE_LOG_RX;
  iec_goose_usr_debug |= IEC_GOOSE_LOG_TX;
#if 1	/* enable these for extra logging.	*/
  iec_goose_usr_debug |= IEC_GOOSE_LOG_FLOW;
  iec_goose_usr_debug |= IEC_GOOSE_LOG_RETRANS;
#endif

  /*set up the first subscription  */
  if (fill_typeid_array (typename_array, typeid_array, numDataSetEntries) != SD_SUCCESS)
    printf("\nError finding typeids for GOOSE subscription for: %s",sub_datasetRef);
  else if ((goosehandle = iecGooseSubscribe2 (sub_mac,sub_gcRef,sub_datasetRef,sub_AppID,sub_confRev,
				numDataSetEntries,typeid_array,callback_function,
				decode_mode)) == NULL)
    printf("\nError creating GOOSE subscription for: %s",sub_datasetRef);

  return (goosehandle);
}
