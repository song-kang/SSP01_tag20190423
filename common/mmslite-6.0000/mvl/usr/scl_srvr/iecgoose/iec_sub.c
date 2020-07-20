/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2002-2006, All Rights Reserved					*/
/*									*/
/* MODULE NAME : iec_sub.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who           Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/17/12  JRB	   GOOSE subscriber code copied from iec_demo.c	*/
/************************************************************************/
#include "iec_demo.h"
#include "glbsem.h"
#include "mvl_defs.h"
#include "sx_defs.h"
#include "sx_arb.h"
#include "mloguser.h"	/* for mlog_arb_log_ctrl	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__; 
#endif

/************************************************************************/
/*				callback_function			*/
/* User function called by IEC GOOSE Framework when a GOOSE message	*/
/* has been received. This example simply logs the received GOOSE message.*/
/************************************************************************/
ST_VOID callback_function(IEC_GOOSE_RX_USER_INFO *goosehandle, 
	GSE_IEC_CTRL *gptr,
	ST_VOID *user,	/* same as goosehandle->usr (usually NULL)	*/
			/* may be set by the user			*/
	ST_UINT16 reason)
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



