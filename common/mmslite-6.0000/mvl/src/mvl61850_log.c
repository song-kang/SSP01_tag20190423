/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2011-2012 All Rights Reserved					*/
/*									*/
/* MODULE NAME : mvl61850_log.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Code to support IEC 61850 Log as a Server.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			mvl61850_log_ctrl_create			*/
/*			mvl61850_log_ctrl_destroy			*/
/*			mvl61850_log_ctrl_destroy_all			*/
/*			mvl61850_log_dataset_create			*/
/*			mvl61850_log_dataset_destroy			*/
/*			mvl61850_log_change_scan			*/
/*			mvl61850_log_entry_add				*/
/*			mvl61850_log_entry_remove			*/
/*			mvl61850_log_service				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/17/12  JRB	   Initial revision.				*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_acse.h"	/* need mvl_cfg_info	*/
#include "mvl_uca.h"
#include "mvl_log.h"

static ST_VOID mvl61850_log_scan_read (MVL61850_LOG_CTRL *logCtrl,
			ST_VOID (*scan_va_done_fun)(MVL_IND_PEND *indCtrl,
					MVL_VAR_ASSOC *va),
			ST_RET (*scan_done_fun)(MVL_IND_PEND *indCtrl)
			);

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Global variables							*/
/************************************************************************/
MVL61850_LOG_CTRL *mvl61850_log_ctrl_list;	/* Linked list of 61850 LCBs.*/

/************************************************************************/
/*			mvl61850_lcb_cleanup				*/
/* Cleanup LCB info to prepare for next log entry.			*/
/************************************************************************/
ST_VOID mvl61850_lcb_cleanup (MVL61850_LOG_CTRL *logCtrl)
  {
ST_INT j;
ST_INT num_var = logCtrl->dsNvl->num_of_entries;
  /* Reset Triggered and all "reason" to prepare for next trigger.	*/
  for (j = 0; j < num_var; j++)
    logCtrl->var_info[j].reason = 0;    
  logCtrl->Triggered = SD_FALSE;
  }

/************************************************************************/
/*			mvl61850_log_change_scan_done			*/
/* Used by code that scans for dchg, qchg, dupd changes.		*/
/* This function is called when the scan is complete.			*/
/* (i.e. 'mvlu_rd_prim_done' has been called for all "leafs" of all	*/
/* variables in the dataset). This saves the "reason" for each change.	*/
/* CRITICAL: when all done, this function must free the temporary	*/
/*   MVL_IND_PEND struct by calling "mvlu_integrity_scan_destroy".	*/
/************************************************************************/
ST_RET mvl61850_log_change_scan_done (MVL_IND_PEND *indCtrl)
  {
ST_RET retCode = SD_SUCCESS;	/* nothing to fail here	*/
ST_INT j;			/* index to array of variables	*/
MVL61850_LOG_CTRL *logCtrl;
MVL_NVLIST_CTRL *dsNvl;
ST_UCHAR reason;	/* reason set by leaf functions.	*/
ST_UCHAR reason_masked;	/* reason masked with TrgOps.	*/

  logCtrl = indCtrl->usr_ind_ctrl;	/* saved at start of scan	*/
  dsNvl = logCtrl->dsNvl;

  /* rpt_reason has been set by leaf functions for each variable of Dataset*/
  /* It should always be dchg, qchg, or dupd (no need to check).	*/
  /* Compare each reason with TrgOps, set "var_info[].reason" if they match.*/
  /* NOTE: reason may be overwritten later on Integrity scan.		*/
  for (j = 0;  j < dsNvl->num_of_entries;  j++)
    {
    reason = dsNvl->entries[j]->rpt_reason;
    reason_masked = logCtrl->lcb_data->TrgOps.data[0] & reason;
    if (reason_masked != 0)
      {	/* reason matched TrgOps. Set Triggered, var_info[].reason	*/
      logCtrl->Triggered = SD_TRUE;
      logCtrl->var_info[j].reason = reason_masked;
      }
    else
      MVLU_LOG_FLOW0 ("VA change ignored: Reason doesn't match TrgOps");
    }

  /* CRITICAL: free the temporary MVL_IND_PEND struct.	*/
  mvlu_integrity_scan_destroy (indCtrl);
  return (retCode);
  }

/************************************************************************/
/*			mvl61850_log_change_scan			*/
/* Scan all IEC 61850 Log Datasets for dchg, qchg, dupd.		*/
/* NOTE: dsNvl->already_scanned is used to avoid rescanning if a Dataset*/
/*    is used in more than one Log Control Block. Same flag is also used*/
/*    for Report scan (mvl61850_rpt_change_scan), but in separate loop.	*/
/************************************************************************/
ST_VOID mvl61850_log_change_scan (ST_VOID)
  {
MVL61850_LOG_CTRL *logCtrl;
MVL_NVLIST_CTRL *dsNvl;
ST_INT j;
ST_DOUBLE timeNow;

  /* Reset "already_scanned=SD_FALSE" for each NVL.				*/
  /* Reset it for every NVL, even if not enabled (faster).			*/
  for (logCtrl = mvl61850_log_ctrl_list;
       logCtrl != NULL;
       logCtrl = (MVL61850_LOG_CTRL *) list_get_next (mvl61850_log_ctrl_list, logCtrl))
    {
    if (logCtrl->dsNvl)	/* might be NULL	*/
      logCtrl->dsNvl->already_scanned = SD_FALSE;
    }

  timeNow = sGetMsTime ();	/* get time just once. Use it in loop.*/

  /* Loop through all Log NVLs to scan for data changes.		*/
  /* Set already_scanned=SD_TRUE for each NVL. If same NVL used in another	*/
  /* Log Control, do NOT rescan it.					*/
  for (logCtrl = mvl61850_log_ctrl_list;
       logCtrl != NULL;
       logCtrl = (MVL61850_LOG_CTRL *) list_get_next (mvl61850_log_ctrl_list, logCtrl))
    {
    dsNvl = logCtrl->dsNvl;
    if (dsNvl == NULL)
      continue;	/* Dataset not known. Ignore this Log Control for now.	*/

    /* If scanRateMs > 0, check timer. If not, go ahead and scan now.	*/
    /* NOTE: if scanRateMs is too small, there may be extra delay between scans.*/
    if (logCtrl->scanRateMs > 0)
      {	/* check timer to see if it's time to scan	*/
      if (timeNow < logCtrl->next_change_scan_time)
        continue;	/* DO NOT scan now. Skip to next logCtrl	*/
      else
        logCtrl->next_change_scan_time = timeNow + logCtrl->scanRateMs;
      }

    /* NOTE: dsNvl checked for NULL above.	*/
    if (dsNvl->already_scanned==SD_FALSE)	/* dsNvl might be NULL*/
      {		/* This NVL not yet scanned.	*/
      /* If Log enabled, start scan.*/
      if (logCtrl->lcb_data->LogEna)
        {
        /* Initialize "rpt_reason=0" for all variables. May be changed	*/
        /* in leaf functions. Checked in mvl61850_log_change_scan_va_done.*/
        /* NOTE: "rpt_reason" is really the Log reason in this case.	*/
        for (j = 0; j < dsNvl->num_of_entries; j++)
          dsNvl->entries[j]->rpt_reason = 0;

        /* Scan for dchg, qchg, dupd.					*/
        /* This function "begins" the scan. When the scan is		*/
        /* complete, "mvl61850_log_change_scan_done" is called to build	*/
        /* the Log entry. If ALL "leaf" functions are synchronous,	*/
        /* "mvl61850_log_change_scan_done" is called BEFORE this function returns.*/
        /* CRITICAL: this function allocates a temporary MVL_IND_PEND	*/
        /*   which must be freed by mvl61850_log_change_scan_done.	*/
        mvl61850_log_scan_read (logCtrl,
                NULL,	/* don't need "scan_va_done" function	*/
                mvl61850_log_change_scan_done);
        dsNvl->already_scanned = SD_TRUE;	/* this NVL scanned. Don't scan again for another LCB*/
        }
      }
    }
  }

/************************************************************************/
/*			mvl61850_log_dataset_create			*/
/* Create IEC 61850 Log Control Block resources for a dataset.		*/
/* NOTE: if old dataset exists, this automatically destroys it first.	*/
/************************************************************************/
ST_VOID mvl61850_log_dataset_create (MVL61850_LOG_CTRL *logCtrl,
	MVL_NVLIST_CTRL *dsNvl)
  {
ST_INT j;

  /* If old dataset exists, must destroy it first.	*/
  if (logCtrl->dsNvl)
    {
    mvl61850_log_dataset_destroy (logCtrl);	
    }

  if (dsNvl)
    {	/* do nothing if dsNvl is NULL	*/
    /* Alloc array of MVL61850_LOG_VAR_INFO structures.	*/
    /* Fill in var_tag in each structure.			*/
    logCtrl->var_info = (MVL61850_LOG_VAR_INFO *) chk_calloc (dsNvl->num_of_entries, sizeof (MVL61850_LOG_VAR_INFO));
    for (j = 0; j < dsNvl->num_of_entries; j++)
      {
      mvl61850_objref_create (dsNvl->entries[j]->name, &dsNvl->va_scope[j], logCtrl->var_info[j].var_tag);
      }
    logCtrl->dsNvl = dsNvl;	/* now save new dataset	*/
    }
  return;
  }

/************************************************************************/
/*			mvl61850_log_dataset_destroy			*/
/* Destroy resources created by mvl61850_log_dataset_create.		*/
/************************************************************************/
ST_VOID mvl61850_log_dataset_destroy (MVL61850_LOG_CTRL *logCtrl)
  {
  if (logCtrl->dsNvl != NULL)
    {		/* if dsNvl is already NULL, do nothing	*/
    chk_free (logCtrl->var_info);
    logCtrl->lcb_data->DatSet[0] = '\0';	/* clear DatSet	in LCB	*/
    logCtrl->dsNvl = NULL;
    }
  return;
  }

/************************************************************************/
/*                       mvl61850_log_ctrl_create			*/
/* Create IEC 61850 Log Control Block.					*/
/* The "dsNvl" arg may be NULL. If so, the LCB may not be enabled until	*/
/* it is set.								*/
/************************************************************************/
MVL61850_LOG_CTRL *mvl61850_log_ctrl_create (
	ST_CHAR *domName,		/* Domain name			*/
        ST_CHAR *lcbName,		/* LCB name			*/
	MVL_JOURNAL_CTRL *journal,	/* MMS journal for this Log	*/
	APP_REF *orig_app,		/* Appl Ref of Originator	*/
	MVL_NVLIST_CTRL *dsNvl,		/* DataSet (may be NULL)	*/
	MVL_VAR_ASSOC *base_va,		/* base variable (the LN).	*/
	MVL61850_LCB_DATA *lcb_data,	/* ptr to LCB data within the LN*/
	ST_INT maxEntries,		/* Max number of Log entries	*/
	ST_UINT scanRateMs)		/* Log scan rate (millisec)	*/
  {
MVL61850_LOG_CTRL *logCtrl;
ST_RET status = SD_SUCCESS;		/* changed if anything fails	*/
size_t domNameSize, lcbNameSize, allocSize;
ST_INT32 tmp_int32[2];	/* Use to initialize EntryID	*/

  domNameSize = strlen (domName) + 1;	/* add 1 for NULL	*/
  lcbNameSize = strlen (lcbName) + 1;	/* add 1 for NULL	*/

  /* Allocate MVL61850_LOG_CTRL struct. Allow extra space after struct	*/
  /* to store domName, lcbName strings.					*/
  allocSize = sizeof (MVL61850_LOG_CTRL) + domNameSize + lcbNameSize;
  logCtrl = (MVL61850_LOG_CTRL *) chk_calloc (1, allocSize);

  /* Set "dom_name" pointer immediately after this structure.	*/
  logCtrl->dom_name = (ST_CHAR *) (logCtrl + 1);
  /* Set "lcb_name" pointer immediately after "dom_name".	*/
  logCtrl->lcb_name = logCtrl->dom_name + domNameSize;

  /* Initialize structure members.	*/
  strcpy (logCtrl->dom_name, domName);
  strcpy (logCtrl->lcb_name, lcbName);

  logCtrl->orig_app = *orig_app;	/* this copies whole struct	*/

  logCtrl->base_va = base_va;		/* save base variable (the LN)	*/

  logCtrl->lcb_data = lcb_data;		/* save ptr to LCB data		*/
  logCtrl->journal = journal;		/* save ptr to MMS Journal	*/
  logCtrl->maxEntries = maxEntries;
  logCtrl->scanRateMs = (ST_DOUBLE) scanRateMs;

  /* Set the initial "EntryID" value.	*/
  /* Put current time in first 4 bytes, 0 in last 4 bytes.	*/
  tmp_int32 [0] = (ST_INT32) time (NULL);
  tmp_int32 [1] = 0;
  memcpy (logCtrl->EntryID, tmp_int32, 8);

  /* We need a few basic typeids. Just find the RPT typeids and use them.*/
  if (status == SD_SUCCESS)
    {
    if (mvlu_rpt_find_typeids (&logCtrl->rpt_typeids))
      {
      MVL_LOG_ERR0 ("Cannot find types needed for IEC 61850 Log Control BLock");
      status = SD_FAILURE;
      }
    }

  /* If all successful, add struct to list, else free struct now.	*/
  if (status == SD_SUCCESS)
    {
    /* mvl61850_log_dataset_create initializes everything related to dsNvl.*/
    /* NOTE: this also allocates logCtrl->var_info (depends on dsNvl).*/
    mvl61850_log_dataset_create (logCtrl, dsNvl);
    list_add_last((ST_VOID **) &mvl61850_log_ctrl_list, (ST_VOID *) logCtrl);
    }
  else
    {
    chk_free (logCtrl);	/* free structure created at top of function	*/
    logCtrl = NULL;			/* set error return value	*/
    }
  return (logCtrl);
  }

/************************************************************************/
/*                       mvl61850_log_ctrl_destroy			*/
/* Free a MVL61850_LOG_CTRL allocated by 'mvl61850_log_ctrl_create'.	*/
/* NOTE: mvl61850_log_ctrl_create didn't create the journal saved in	*/
/*       logCtrl->journal, so we don't destroy it. Caller should do that.*/
/************************************************************************/

ST_VOID mvl61850_log_ctrl_destroy (MVL61850_LOG_CTRL *logCtrl)
  {
MVL_JOURNAL_ENTRY *entry;

  /* Take it off the list */
  list_unlink ((ST_VOID **) &mvl61850_log_ctrl_list, (ST_VOID *) logCtrl);

  /* Discard all entries in the Log (loop until head of list is NULL).	*/
  while ((entry = logCtrl->journal->entries) != NULL)
    {
    mvl61850_log_entry_remove (logCtrl, entry);
    logCtrl->journal->numEntries--;
    }
  /* If everything worked, numEntries should now be 0.	*/

  mvl61850_log_dataset_destroy (logCtrl);

  chk_free (logCtrl);
  }

/************************************************************************/
/*			mvl61850_log_ctrl_destroy_all			*/
/* Destroy all 61850 Log controls.					*/
/************************************************************************/
ST_VOID mvl61850_log_ctrl_destroy_all ()
  {
MVL61850_LOG_CTRL *logCtrl;
  /* Destroy first entry on the list, until list is empty (NULL).	*/
  while ((logCtrl = mvl61850_log_ctrl_list) != NULL)
    {
    mvl61850_log_ctrl_destroy (logCtrl);	/* remove from list AND free it*/
    }
  }

/************************************************************************/
/* 			fill_journal_var_info				*/
/* This function reads the variable "va" at THIS MOMENT in time,	*/
/* converts it to ASN.1 encoding, and then fills in the VAR_INFO	*/
/* structure appropriately.						*/
/* This function may be replaced by any function that fills in the	*/
/* VAR_INFO structure with appropriate "time stamped" data.		*/
/************************************************************************/
static ST_RET fill_journal_var_info (VAR_INFO *var_info,
	ST_CHAR *var_tag,	/* Journal variable tag	*/
	MVL_VAR_ASSOC *va)
  {
ST_RET rc;
ST_INT varDataLen;
ST_UCHAR *asn1_buf;

  var_info->var_tag = var_tag; 	/* copy variable tag to struct	*/

  asn1_buf = (ST_UCHAR *) chk_malloc (mvl_cfg_info->max_msg_size);
  /* First arg is (MVL_NET_INFO *), used for AA-SPEC variables.		*/
  /* Journal should not contain any AA-SPEC vars, so passing NULL is OK.*/
  rc = mvl_get_va_asn1_data (NULL, va, SD_FALSE, NULL, 
                             asn1_buf, mvl_cfg_info->max_msg_size, &varDataLen);
  if (rc == SD_SUCCESS)
    {
    /* ASN.1 conversion OK. Allocate buffer for ASN.1 data & copy to it.*/
    /* "mvl_get_va_asn1_data" encodes at beginning of buffer, so	*/
    /* "asn1_buf" points to ASN.1 data.					*/

    var_info->value_spec.data = (ST_UCHAR *) chk_malloc (varDataLen);
    memcpy (var_info->value_spec.data, asn1_buf, varDataLen);
    var_info->value_spec.len = varDataLen;
    }
  else
    {	/* could not convert variable data to ASN.1	*/
    MVL_LOG_ERR0 ("Illegal variable data for Journal Entry.");
    }
  chk_free (asn1_buf);
  return (rc);
  }

/************************************************************************/
/* 			fill_journal_var_info_2				*/
/* Same as fill_journal_var_info but encodes data directly based on type*/
/* definition without using MVL_VAR_ASSOC.				*/
/************************************************************************/
static ST_RET fill_journal_var_info_2 (VAR_INFO *var_info,
	ST_CHAR *var_tag,	/* Journal variable tag	*/
	ST_CHAR *data,		/* data to encode	*/
	RUNTIME_TYPE *rt,	/* type of data		*/
	ST_INT num_rt)
  {
ST_RET rc;
ST_INT asn1_len;
ST_UCHAR *asn1_buf;
ST_UCHAR *asn1_ptr;
ASN1_ENC_CTXT aCtx;

  var_info->var_tag = var_tag; 	/* copy variable tag to struct	*/

  /* Alloc temporary encode buffer. Freed before return.	*/
  asn1_buf = (ST_UCHAR *) chk_malloc (mvl_cfg_info->max_msg_size);

  asn1r_strt_asn1_bld (&aCtx, asn1_buf, mvl_cfg_info->max_msg_size);
  rc = ms_local_to_asn1 (&aCtx, rt, num_rt, data);
  if (rc == SD_SUCCESS)
    {
    /* ASN.1 conversion OK. Allocate buffer for ASN.1 data & copy to it.*/
    asn1_ptr = ASN1_ENC_PTR(&aCtx);	/* get ptr to encoded data	*/
    asn1_len = (ST_INT) ASN1_ENC_LEN(&aCtx);
  
    var_info->value_spec.data = (ST_UCHAR *) chk_malloc (asn1_len);
    memcpy (var_info->value_spec.data, asn1_ptr, asn1_len);
    var_info->value_spec.len = asn1_len;
    }
  else	/* Could not get journal data converted to ASN.1 	*/
    {
    MVL_LOG_ERR1 ("ERROR encoding Journal Entry = 0x%X", rc);
    }
  chk_free (asn1_buf);
  return (rc);
  }

/************************************************************************/
/*			mvl61850_log_entry_add				*/
/* Generate Log (Journal) entry and add it to the Journal.		*/
/* This follows the encoding rules of IEC 61850-8-1.			*/
/* NOTE: entry removed by calling mvl61850_log_entry_remove.		*/
/************************************************************************/
ST_RET mvl61850_log_entry_add (MVL61850_LOG_CTRL *logCtrl)
  {
MVL_JOURNAL_ENTRY *entry;	/* new entry being added to Journal	*/
ST_INT j;
ST_RET retcode = SD_SUCCESS;
MMS_BTIME6 btime6;
RUNTIME_TYPE *reason_rt_first;
ST_INT reason_rt_num;

  /* Get Runtime type for "reason".	*/
  retcode = mvl_get_runtime (logCtrl->rpt_typeids.bstr6,
               &reason_rt_first, &reason_rt_num);
  if (retcode)
    {	/* this should never fail	*/
    MVL_LOG_ERR0 ("Cannot find type needed for Log Entry.");
    return (retcode);
    }

  u_mvlu_rpt_time_get (&btime6);	/* get current time from user	*/

  /* Allocate Journal struct. Luckily "entry->entry_id" is 8 bytes	*/
  /* (that is the size we need).					*/
  entry = (MVL_JOURNAL_ENTRY *) chk_calloc (1, sizeof (MVL_JOURNAL_ENTRY));
  entry->entry_id_len = 8;
  /* Point to last 4 bytes of EntryID, cast to INT32 & increment.*/
  ++(*(ST_INT32 *)(&logCtrl->EntryID[4]));
  /* Copy EntryID to this log entry	*/
  memcpy (entry->entry_id, logCtrl->EntryID, 8);

  entry->orig_app = logCtrl->orig_app;	/* this copies whole structure	*/

  entry->occur_time.form = MMS_BTOD6;
  entry->occur_time.ms   = btime6.ms;	/* milliseconds since midnight	*/
  entry->occur_time.day  = btime6.day;	/* Days since 1-1-1984		*/

  /* Update OldEntr, NewEntr, OldEntrTim, NewEntrTim of LCB as needed.	*/
  if (logCtrl->journal->numEntries == 0)
    {
    memcpy (logCtrl->lcb_data->OldEntr, logCtrl->EntryID, 8);
    logCtrl->lcb_data->OldEntrTim.ms  = btime6.ms;
    logCtrl->lcb_data->OldEntrTim.day = btime6.day;
    }
  memcpy (logCtrl->lcb_data->NewEntr, logCtrl->EntryID, 8);
  logCtrl->lcb_data->NewEntrTim.ms  = btime6.ms;
  logCtrl->lcb_data->NewEntrTim.day = btime6.day;
    
  entry->entry_form_tag = JE_FORM_DATA;

  /* Update the current state to store in "Event Transition Record" of Journal Entry.*/
  if (logCtrl->cur_state == EC_STATE_DISABLED)
    {
    /* Change cur_state ONLY when LogEna transitions.	*/
    if (logCtrl->lcb_data->LogEna)
      {	/* transition to enabled	*/
      /* set to intermediate state to indicate transition.	*/
      /* Changed below to EC_STATE_ACTIVE after log entry added.*/
      logCtrl->cur_state = EC_STATE_IDLE;
      }
    }
  else if (logCtrl->cur_state == EC_STATE_ACTIVE)
    {
    /* Change cur_state ONLY when LogEna transitions.	*/
    if (logCtrl->lcb_data->LogEna == 0)
      {	/* transition to disabled	*/
      logCtrl->cur_state = EC_STATE_DISABLED;
      }
    }
  else
    /* Invalid state. Should NEVER happen. Just set to something valid.*/
    logCtrl->cur_state = EC_STATE_DISABLED;

  /* Fill in "Event Transition Record" of Journal Entry.	*/
  entry->ef.data.event_pres = SD_TRUE;
  entry->ef.data.cur_state = logCtrl->cur_state;
  entry->ef.data.evcon_name.object_tag = DOM_SPEC;
  entry->ef.data.evcon_name.domain_id = logCtrl->dom_name;
  entry->ef.data.evcon_name.obj_name.item_id = logCtrl->lcb_name;

  if (logCtrl->cur_state == EC_STATE_ACTIVE)	/* data included only in this state	*/
    {
    entry->ef.data.list_of_var_pres = 1;
    entry->ef.data.num_of_var = logCtrl->dsNvl->num_of_entries * 2;	/* double to include reason*/
    entry->ef.data.list_of_var = (VAR_INFO *) chk_calloc (entry->ef.data.num_of_var,
                                     sizeof (VAR_INFO));
  
    /* Fill in 2 Journal vars for each Dataset var.	*/
    for (j = 0; j < logCtrl->dsNvl->num_of_entries; j++)
      {
      /* Add Journal Variable for "data".		*/
      if (fill_journal_var_info (
          &entry->ef.data.list_of_var [j*2],	/* VAR_INFO to fill in	*/
          logCtrl->var_info[j].var_tag,		/* variable tag		*/
          logCtrl->dsNvl->entries [j]))
        {		/* Failed.	*/
        retcode = SD_FAILURE;
        break;
        }
      /* Add Journal Variable for "Reason".			*/
      /* NOTE: The example in 61850-8-1 says different, but the text	*/
      /*       says the variable tag shall be "ReasonCode".		*/
      if (fill_journal_var_info_2 (
          &entry->ef.data.list_of_var [j*2+1],	/* VAR_INFO to fill in	*/
          "ReasonCode",				/* variable tag		*/
          (ST_CHAR *) &logCtrl->var_info[j].reason,
          reason_rt_first, reason_rt_num))
        {		/* Failed.	*/
        retcode = SD_FAILURE;
        break;
        }
      }		/* end loop	*/

    /* If something failed in loop, free list_of_var allocated above.	*/
    if (retcode)
      {
      entry->ef.data.list_of_var_pres = SD_FALSE;
      chk_free (entry->ef.data.list_of_var);
      }
    }

  if (logCtrl->cur_state == EC_STATE_IDLE)
    logCtrl->cur_state = EC_STATE_ACTIVE;	/* IDLE is only transition state*/

  /* If Journal is full, discard oldest entry.				*/
  if (logCtrl->journal->numEntries >= logCtrl->maxEntries)
    {
    /* Remove oldest entry.	*/
    mvl61850_log_entry_remove (logCtrl, logCtrl->journal->entries);	/* remove first (oldest) entry*/
    /* Update OldEntr, OldEntrTim of LCB after oldest removed.	*/
    memcpy (logCtrl->lcb_data->OldEntr, logCtrl->journal->entries->entry_id, 8);
    /* OldEntrTim is MMS_BTIME6, occur_time is MMS_BTOD (form=MMS_BTOD6).*/
    logCtrl->lcb_data->OldEntrTim.ms  = logCtrl->journal->entries->occur_time.ms;
    logCtrl->lcb_data->OldEntrTim.day = logCtrl->journal->entries->occur_time.day;
    }
  else
    logCtrl->journal->numEntries++;

  /* Add new entry to Journal.	*/
  list_add_last((ST_VOID **) &logCtrl->journal->entries, (ST_VOID *) entry);


  mvl61850_lcb_cleanup (logCtrl);	/* CRITICAL: prepare for next log entry	*/

  return (retcode);
  }

/************************************************************************/
/*			mvl61850_log_entry_remove			*/
/* Remove entry from the Journal and free all resources.		*/
/* NOTE: entry added by calling mvl61850_log_entry_add.			*/
/************************************************************************/
ST_VOID mvl61850_log_entry_remove (MVL61850_LOG_CTRL *logCtrl, MVL_JOURNAL_ENTRY *entry)
  {
VAR_INFO *var_info;
ST_INT j;
  /* Remove from list.	*/
  list_unlink ((ST_VOID **) &logCtrl->journal->entries, entry);

  /* Free all data for this Journal.	*/
  for (j = 0; j < entry->ef.data.num_of_var; j++)
    {
    var_info = &entry->ef.data.list_of_var [j];
    chk_free (var_info->value_spec.data);
    }
  if (entry->ef.data.list_of_var)
    chk_free (entry->ef.data.list_of_var);
  chk_free (entry);
  }

/************************************************************************/
/*			mvl61850_log_integrity_chk_timeout		*/
/* If Integrity period timeout occurred, generate Log entry.		*/
/* NOTE: data scan already done at start of mvl61850_log_service.	*/
/*       No need to rescan data. Just send the data from the data scan.	*/
/* RETURNS: SD_TRUE if IntgPd is set and timeout occurred		*/
/*          SD_FALSE otherwise						*/
/************************************************************************/
ST_RET mvl61850_log_integrity_chk_timeout (MVL61850_LOG_CTRL *logCtrl, ST_DOUBLE timeNow)
  {
ST_RET retVal;
ST_INT j;
ST_RET retcode;

  if (logCtrl->lcb_data->IntgPd != 0
      && BSTR_BIT_GET (logCtrl->lcb_data->TrgOps.data, TRGOPS_BITNUM_INTEGRITY)	/* enabled*/
      && timeNow > logCtrl->next_integ_scan_time)	/* timeout occurred*/
    {
    MVLU_LOG_FLOW1 ("IEC 61850 Log Integrity Period timeout for LCB '%s'", logCtrl->lcb_name);

    /* Set "reason == INTEGRITY" for each variable of Dataset now.	*/
    /* Overwrite any reason set by leaf functions.			*/
    for (j = 0; j < logCtrl->dsNvl->num_of_entries; j++)
      logCtrl->var_info[j].reason = MVLU_TRGOPS_INTEGRITY;

    /* Generate Log entry and add it to the Log (linked list).	*/
    retcode = mvl61850_log_entry_add (logCtrl);

    if (retcode != SD_SUCCESS)
      MVL_LOG_ERR1 ("IEC 61850 Log entry could not be generated. err=0x%X", retcode);

    /* set next integrity timeout	*/
    logCtrl->next_integ_scan_time = sGetMsTime ()
                     + (ST_DOUBLE) logCtrl->lcb_data->IntgPd;
    retVal = SD_TRUE;
    }
  else
    retVal = SD_FALSE;
  return (retVal);
  }

/************************************************************************/
/*			mvl61850_log_service				*/
/* Perform servicing for all IEC 61850 Log Control Blocks.		*/
/* Scanning for changes (dchg, qchg, etc) is automatic		  	*/
/* (see mvl61850_log_change_scan).					*/
/************************************************************************/

ST_VOID mvl61850_log_service ()
  {
MVL61850_LOG_CTRL *logCtrl;
ST_DOUBLE timeNow;
ST_RET retcode;

/* Scan all Log data for changes (dchg, qchg, dupd).	*/
  mvl61850_log_change_scan ();

/* See if it is time to generate Log entries	*/
  timeNow = sGetMsTime ();
  for (logCtrl = mvl61850_log_ctrl_list;
       logCtrl != NULL;
       logCtrl = (MVL61850_LOG_CTRL *) list_get_next (mvl61850_log_ctrl_list, logCtrl))
    {
    /* NOTE: if dsNvl is NULL, don't need to do anything for this logCtrl.*/
    if (logCtrl->dsNvl==NULL)
      continue;

    /* If LCB enabled, chk triggers, then chk for Integrity period timeout.*/
    if (logCtrl->lcb_data->LogEna)
      {
      /* Chk for dchg, qchg, dupd triggers.				*/
      /* NOTE: Triggered == SD_TRUE ONLY if changes match the TrgOps.	*/
      /* CRITICAL: check these triggers BEFORE mvl61850_log_integrity_chk_timeout*/
      /*           so reasons don't get overwritten.			*/
      if (logCtrl->Triggered)
        {
        MVLU_LOG_FLOW1 ("IEC 61850 Non-Integrity Log entry added for LCB '%s'", logCtrl->lcb_name);
        /* Generate and add entry to the Log (Journal).	*/
        retcode = mvl61850_log_entry_add (logCtrl);
        if (retcode)
          MVL_LOG_ERR1 ("Error = 0x%X adding IEC 61850 Log (Journal) entry", retcode);
        }

      /* This function checks if Integrity is enabled in TrgOps.	*/
      /* If so, it checks for Integrity Period timeout.		*/
      mvl61850_log_integrity_chk_timeout (logCtrl, timeNow);
      }	/* end if Log enabled	*/
    }
  }

/*
 *			mvl61850_log_scan_read
 * This function "BEGINS" the scan of ALL data for one IEC 61850 Log DataSet (NVL).
 * - It allocates & initializes a temporary MVL_IND_PEND structure.
 * - It calls "u_mvl_read_ind" to begin the scan.
 * - The scan may complete synchronously or asynchronously.
 * - When the scan completes, the funct pointed to by "scan_done_fun" arg
 *   is called to cleanup.
 * CRITICAL: Function pointed to by "scan_done_fun" must call
 * "mvlu_integrity_scan_destroy" to free temporary MVL_IND_PEND struct.
 */

static ST_VOID mvl61850_log_scan_read (MVL61850_LOG_CTRL *logCtrl,
			ST_VOID (*scan_va_done_fun)(MVL_IND_PEND *indCtrl,
					MVL_VAR_ASSOC *va),
			ST_RET (*scan_done_fun)(MVL_IND_PEND *indCtrl)
			)
  {
MVL_IND_PEND *indCtrl;

  /* Create a simulated read indication for the DataSet NVL.	*/
  indCtrl = mvlu_setup_scan_read (logCtrl->dsNvl,
                          scan_va_done_fun, scan_done_fun);

  /* Save logCtrl to use in "*scan_done_fun"	*/
  indCtrl->usr_ind_ctrl = logCtrl;

  /* OK, now fire off the read indication functions.	*/
  u_mvl_read_ind (indCtrl);
  }

/************************************************************************/
/*			mvl61850_log_ctrl_find				*/
/* Find IEC 61850 Log Control.						*/
/************************************************************************/
MVL61850_LOG_CTRL *mvl61850_log_ctrl_find (
	MVL_VAR_ASSOC *base_va,		/* base variable (the LN).	*/
	ST_VOID *primData)		/* ptr to attribute in LCB of LN*/
  {
MVL61850_LOG_CTRL *logCtrl;

  /* Find the LCB that contains this attribute.	*/
  for (logCtrl = mvl61850_log_ctrl_list;
       logCtrl != NULL;
       logCtrl = (MVL61850_LOG_CTRL *) list_get_next (mvl61850_log_ctrl_list, logCtrl))
    {
    if (logCtrl->base_va == base_va &&
        (primData >= (ST_VOID *) logCtrl->lcb_data) &&		/* start of struct*/
        (primData < (ST_VOID *)(logCtrl->lcb_data + 1)))	/* end of struct*/
      {
      break;	/* just return current logCtrl.	*/
      }
    }
  return (logCtrl);	/* NULL if not found*/
  }

/*======================================================================*/
/*======================================================================*/
/* Leaf functions for LCB attributes.					*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			mvl61850_lcb_logena_wr_ind			*/
/* Write 'LogEna' attribute in IEC 61850 LCB.				*/
/* Find the LCB. Don't allow it to be enabled unless DatSet is set.	*/
/************************************************************************/
ST_VOID mvl61850_lcb_logena_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_RET retCode;
MVL61850_LOG_CTRL *logCtrl;
MVL_VAR_ASSOC *va;

  retCode = SD_FAILURE;
  va = mvluWrVaCtrl->wrVaCtrl->va;

  if (*(ST_UINT8 *) mvluWrVaCtrl->primData != 0)
    {	/* trying to Enable LCB	*/
    logCtrl = mvl61850_log_ctrl_find (va->base_va, mvluWrVaCtrl->primData);
    if (logCtrl != NULL && logCtrl->dsNvl != NULL)
      {
      /* found LCB and DatSet is valid, so allow the write to Enable the LCB. */
      retCode = SD_SUCCESS;
      }
    }
  else
    {	/* trying to Disable LCB	*/
    /* Always allow a write to Disable the LCB.	*/
    retCode = SD_SUCCESS;
    }
  if (retCode != SD_SUCCESS)
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_TEMP_UNAVAIL;	/* only possible error*/

  mvlu_wr_prim_done (mvluWrVaCtrl, retCode);
  }
/************************************************************************/
/*			mvl61850_lcb_datset_wr_ind			*/
/* Write DatSet in IEC 61850 LCB.					*/
/************************************************************************/
ST_VOID mvl61850_lcb_datset_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_RET retCode;
MVL_NVLIST_CTRL *nvl;
ST_INT16 failure = ARE_TEMP_UNAVAIL;	/* use on err if no other value set*/
MVL61850_LOG_CTRL *logCtrl;
ST_INT prim_offset;	/* need this to find current DatSet value	*/
ST_CHAR *currDatSet;	/* Current DatSet value (before this Write)	*/
MVL_VAR_ASSOC *va;
OBJECT_NAME *object_name;	/* derived from DatSet name	*/

  retCode = SD_FAILURE;
  va = mvluWrVaCtrl->wrVaCtrl->va;

  logCtrl = mvl61850_log_ctrl_find (va->base_va, mvluWrVaCtrl->primData);
  if (logCtrl != NULL)
    {
    if (logCtrl->lcb_data->LogEna != 0)
      MVLU_LOG_FLOW0 ("Cannot write to LCB 'DatSet' because LCB enabled");
    else
      {
      /* mvluWrVaCtrl->primData should point to logCtrl->lcb_data->DatSet.*/

      /* Get current DatSet (already overwritten but saved in "oldData").*/
      prim_offset = (ST_INT) (mvluWrVaCtrl->primData - (ST_CHAR *) va->data);
      currDatSet = (ST_CHAR *) mvluWrVaCtrl->wrVaCtrl->oldData + prim_offset;
      if (strcmp (currDatSet, mvluWrVaCtrl->primData) == 0)
        {
        retCode = SD_SUCCESS;	/* Same val written. Nothing to do but allow it*/
        }
      else if (mvluWrVaCtrl->primData[0] == '\0')
        {	/* client wrote empty string, and it was NOT already empty	*/
        mvl61850_log_dataset_destroy (logCtrl);
        retCode = SD_SUCCESS;
        }
      else if (strlen ((ST_CHAR *) mvluWrVaCtrl->primData) > MVL61850_MAX_OBJREF_LEN)
        {
        failure = ARE_OBJ_VALUE_INVALID;
        }
      else
        {
        /* Find the NVL for the new DatSet (ObjectReference).	*/
        object_name = mvl61850_objref_to_objname (
                 mvluWrVaCtrl->primData);	/* new DatSet (ObjectReference)*/
        if (object_name)
          {
          nvl = mvl_vmd_find_nvl (&mvl_vmd, object_name, 
                 mvluWrVaCtrl->indCtrl->event->net_info);
          chk_free (object_name);	/* done with this	*/
          }
        else
          nvl = NULL;	/* ObjRef is invalid so can't find NVL	*/

        if (nvl)
          {
          /* Replace "dataset".	*/
          /* NOTE: if old dataset exists, this automatically destroys it first.*/
          mvl61850_log_dataset_create (logCtrl, nvl);
          retCode = SD_SUCCESS;
          }
        else	/* NVL cannot be found	*/
          {
          MVL_LOG_ERR1 ("LCB DatSet '%s' cannot be found. Write failed.", mvluWrVaCtrl->primData);
          failure = ARE_OBJ_VALUE_INVALID;
          }
        }
      }
    }
  else
    {	/* LCB not found*/
    MVL_LOG_ERR0 ("LCB not found. Cannot write to LCB 'DatSet'.");
    failure = ARE_OBJ_UNDEFINED;
    }

  if (retCode != SD_SUCCESS)
    mvluWrVaCtrl->wrVaCtrl->failure = failure;

  mvlu_wr_prim_done (mvluWrVaCtrl, retCode);
  }
/************************************************************************/
/*			mvl61850_lcb_other_wr_ind			*/
/* Write other attribute in IEC 61850 LCB.				*/
/* This should work for LogRef, TrgOps, or IntgPd.			*/
/* Nothing special to do. Just make sure this log is not enabled.	*/
/************************************************************************/
ST_VOID mvl61850_lcb_other_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_RET retCode;
ST_INT16 failure = ARE_TEMP_UNAVAIL;	/* use on err if no other value set*/
MVL61850_LOG_CTRL *logCtrl;
MVL_VAR_ASSOC *va;

  retCode = SD_FAILURE;
  va = mvluWrVaCtrl->wrVaCtrl->va;

  logCtrl = mvl61850_log_ctrl_find (va->base_va, mvluWrVaCtrl->primData);
  if (logCtrl != NULL)
    {
    if (logCtrl->lcb_data->LogEna != 0)
      MVLU_LOG_FLOW0 ("Cannot write to LCB attribute (LogRef, TrgOps, or IntgPd) because LCB enabled");
    else
      {
      retCode = SD_SUCCESS;	/* just allow the write	*/
      }
    }
  if (retCode != SD_SUCCESS)
    mvluWrVaCtrl->wrVaCtrl->failure = failure;

  mvlu_wr_prim_done (mvluWrVaCtrl, retCode);
  }

