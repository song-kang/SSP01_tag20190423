/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2012 - 2012, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_jinit2.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/* 	InitializeJournal Response function for IEC 61850 only.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			mvl61850_log_jinit_resp				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/21/12  JRB	   Created.					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mvl_defs.h"
#include "mvl_log.h"
#include "mvl_uca.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			mvl61850_log_jinit_resp				*/
/* Send InitializeJournal response for IEC 61850.			*/
/* This should be called from u_mvl_jinit_ind.				*/
/* NOTE: This should only be used in IEC 61850 server applications.	*/
/************************************************************************/
ST_VOID mvl61850_log_jinit_resp (MVL_IND_PEND *indCtrl)
  {
JINIT_REQ_INFO *req_info = indCtrl->u.jinit.req_info;
JINIT_RESP_INFO resp_info;
MVL_JOURNAL_CTRL *journal;
MVL61850_LOG_CTRL *logCtrl;
MVL_JOURNAL_ENTRY *entry;	/* Journal entry to check & maybe delete.*/

  /* Limit time form MUST BE MMS_BTOD6.	*/
  if (req_info->limit_spec_pres && req_info->limit_time.form != MMS_BTOD6)
    {
    MVL_LOG_NERR1 ("InitializeJournal - limit time form = %d not supported", 
        req_info->limit_time.form);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_OBJ_UNDEFINED);
    return;
    }  

  /* Find the Journal.	*/
  journal = mvl_vmd_find_jou (&mvl_vmd, &req_info->jou_name, indCtrl->event->net_info);
  if (journal == NULL)
    {
    MVL_LOG_NERR1 ("InitializeJournal - could not find journal '%s'", 
        req_info->jou_name.obj_name.vmd_spec);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_OBJ_UNDEFINED);
    return;
    }  

  /* Find the LCB that controls this Journal.	*/
  for (logCtrl = mvl61850_log_ctrl_list;
       logCtrl != NULL;
       logCtrl = (MVL61850_LOG_CTRL *) list_get_next (mvl61850_log_ctrl_list, logCtrl))
    {
    if (logCtrl->journal == journal)
      break;	/* logCtrl points to correct LCB	*/
    }
  if (logCtrl == NULL)
    {
    MVL_LOG_NERR1 ("InitializeJournal - could not LCB for this journal '%s'", 
        req_info->jou_name.obj_name.vmd_spec);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_OBJ_UNDEFINED);
    return;
    }
    
  /* Init count of deleted entries. Incremented when each entry deleted.*/
  resp_info.del_entries = 0;

  /* Loop checking oldest Journal entry. If it fits criteria, delete it.*/
  while ((entry = journal->entries) != NULL)
    {
    if (req_info->limit_spec_pres)
      {
      if (entry->occur_time.day < req_info->limit_time.day ||
          (entry->occur_time.day == req_info->limit_time.day && entry->occur_time.ms < req_info->limit_time.ms))
        {	/* entry time < limit time	*/
        mvl61850_log_entry_remove (logCtrl, entry);	/* delete this entry*/
        resp_info.del_entries++;
        }
      else if (entry->occur_time.day == req_info->limit_time.day &&
               entry->occur_time.ms  == req_info->limit_time.ms)
        {	/* entry time == limit time	*/
        ST_BOOLEAN stopAfterDel = SD_FALSE;	/* set this to stop AFTER entry deleted.*/
        /* If limit entry is present, and this entry matches, stop AFTER entry deleted.*/
        if (req_info->limit_entry_pres)
          {
          if (req_info->limit_entry_len == entry->entry_id_len &&
              memcmp (entry->entry_id, req_info->limit_entry, req_info->limit_entry_len) == 0)
            stopAfterDel = SD_TRUE;	/* Stop below AFTER entry deleted.	*/
          }
        mvl61850_log_entry_remove (logCtrl, entry);	/* delete this entry*/
        resp_info.del_entries++;
        if (stopAfterDel)
          break;	/* stop looping now	*/
        }
      else
        {	/* entry time > limit time	*/
        /* DO NOT delete this entry.	*/
        break;	/* stop looping now	*/
        }
      }
    else
      {	/* limit spec not present.	*/
      /* In this case, loop continues until ALL entries deleted.	*/
      mvl61850_log_entry_remove (logCtrl, entry);	/* delete this entry*/
      resp_info.del_entries++;
      }
    }

  /* Finally, if any entries deleted, update the LCB.	*/
  if (resp_info.del_entries)
    {
    /* Update OldEntr, OldEntrTim, NewEntr, NewEntrTim of LCB.	*/
    /* NOTE: journal == logCtrl->journal	*/
    if (journal->entries == NULL)
      {	/* Journal is empty. Reset OldEntr* and NewEntr* to all 0.	*/
      memset (logCtrl->lcb_data->OldEntr, 0, 8);
      logCtrl->lcb_data->OldEntrTim.ms  = 0;
      logCtrl->lcb_data->OldEntrTim.day = 0;
      memset (logCtrl->lcb_data->NewEntr, 0, 8);
      logCtrl->lcb_data->NewEntrTim.ms  = 0;
      logCtrl->lcb_data->NewEntrTim.day = 0;
      }
    else
      {	/* Journal is NOT empty. Update OldEntr*. Do not change NewEntr*.*/
      memcpy (logCtrl->lcb_data->OldEntr, journal->entries->entry_id, 8);
      /* OldEntrTim is MMS_BTIME6, occur_time is MMS_BTOD (form=MMS_BTOD6).*/
      logCtrl->lcb_data->OldEntrTim.ms  = journal->entries->occur_time.ms;
      logCtrl->lcb_data->OldEntrTim.day = journal->entries->occur_time.day;
      }
    }

  /* IMPORTANT: update number of journal entries after all deletions.	*/
  journal->numEntries = list_get_sizeof (journal->entries);

  /* Send normal response.	*/
  indCtrl->u.jinit.resp_info = &resp_info;
  mplas_jinit_resp (indCtrl);
  }

