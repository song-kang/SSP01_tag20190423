/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2006-2006 All Rights Reserved					*/
/*									*/
/* MODULE NAME : mvl61850_rpt.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Code to support IEC 61850 Reporting as a Server.		*/
/*									*/
/* BUFFERING OF REPORTS:						*/
/* For Buffered Report Controls, buffering is automatic (no user	*/
/* intervention). For each BRCB, all reports are stored in a linked	*/
/* list "rpt_list". Also, "rpt_list_next" points to the next report to	*/
/* send. If all reports have been sent, rpt_list_next==NULL.		*/
/*									*/
/* The maximum number of bytes to store in the buffer is a		*/
/* user-configurable parameter. When the maximum number of bytes is	*/
/* exceeded, the oldest report in the buffer is discarded.		*/
/*									*/
/* The standard SISCO linked list functions are used for buffering.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvl61850_brcb_rpt_save						*/
/*	mvl61850_brcb_rpt_send						*/
/*	mvl61850_brcb_rpt_lists_clean					*/
/*	mvl61850_brcb_client_service					*/
/*	mvl61850_urcb_client_service					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/13/12  JRB	   mvl61850_rpt_dataset_create: del scan added	*/
/*			   10/20/11 (must be done AFTER data mapping).	*/
/* 01/11/12  JRB	   mvl61850_brcb_client_service: chk ResvTms.	*/
/*			   Del mvl61850_mk_dataref, use mvl61850_objref_create*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 10/20/11  JRB	   mvl61850_rpt_dataset_create: start scan to	*/
/*			   init "last_data".				*/
/* 05/25/11  JRB	   buflist_entry_create: alloc only one buffer.	*/
/* 09/20/10  JRB	   Delete incorrect cast.			*/
/* 01/12/10  JRB     11    Fix log messages.				*/
/* 06/30/08  JRB     10    For Ed2, save lastSentTimeOfEntry for BRCB.	*/
/*			   Resync to oldest if all 0 written to EntryID.*/
/* 06/04/08  JRB     09    Start buffering on power-up if DatSet attr	*/
/*			   references existing DataSet (for 61850-7-2 Ed2)*/
/* 05/15/08  JRB     08    Add mvl61850_integrity_timeout for 61850 only*/
/*			   (does NOT scan data, just sends current data)*/
/* 05/06/08  JRB     07    Fix memory leak after "PDU size" error.	*/
/* 02/29/08  JRB     06    Create correct AA_SPEC or VMD_SPEC DatSet.	*/
/*			   Reset cur_va_index, SubSeqNum when all_seg_done*/
/*			   so retransmission starts with first va.	*/
/*			   If dsNvl==NULL, do not service RCB.		*/
/* 02/13/07  JRB     05    mvl61850_get_rcb: del unused net_info arg.	*/
/*			   Use scan_rate to control scan timing.	*/
/*			   Save TimeofEntry in "basrcb".		*/
/* 11/30/06  JRB     04    Fix *_brcb_entryid_init so user can call it.	*/
/* 10/10/06  JRB     03    Add mvl61850_rpt_ctrl_destroy_all.		*/
/* 08/18/06  JRB     02    Disable debug printing. Clean up comments.	*/
/* 08/09/06  JRB     01    New. Moved most 61850 Rpt code to here	*/
/*			   with following changes:			*/
/*			   New RPT functions just for 61850 (not UCA):	*/
/*			    mvl61850_create_rpt_ctrl, mvl61850_free_rpt_ctrl,*/
/*			    mvl61850_rpt_service.			*/
/*			   Chg BRCB code to save "raw" data in buffer	*/
/*			    and encode rpt only when sending.		*/
/*			   Chg to one rpt list with ptr to next to send.*/
/*			   ..urcb_client_service: First chk if connected.*/
/*			   Put start time in first 4 bytes of EntryID.	*/
/*			   When GI rpt buffered, discard old GI rpt.	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_acse.h"	/* need mvl_cfg_info	*/
#include "mvl_uca.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* DEBUG: IEC 61850 is not clear about exactly how to handle "BufOvfl",
 * so the code in this module implements the following:
 * 1. If client writes EntryID & the value can't be found in the buffer,
 *    BufOvfl is set, an error is returned, and the oldest buffer entry will
 *    be sent next (see mvl61850_brcb_rpt_set_entryid).
 * 2. If entry found but discarded before RptEna,
 *    BufOvfl is set, and the oldest buffer entry will
 *    be sent next (see buflist_discard_old).
 * 3. If entries lost while connected, set BufOvfl (see buflist_discard_old).
 * 4. After one report sent with BufOvfl, clear it
 *    (see mvl61850_brcb_client_service).
 */

/************************************************************************/
/*			print macros for debugging			*/
/************************************************************************/
#if 0	/* DEBUG: enable to see queue activity.	*/
/* Use function here (too complex for macro).	*/
ST_VOID print_queues(MVL61850_BRCB_CTRL *brcbCtrl)
  {
  BUFLIST_ENTRY *entry;
  printf ("REPORT BUFFER ('.'=rpt, '+'=next_to_send)");
  for (entry = brcbCtrl->rpt_list;
      entry != NULL;
      entry = (BUFLIST_ENTRY *) list_get_next (brcbCtrl->rpt_list, entry))
   {
   if (entry == brcbCtrl->rpt_list_next)
     putchar('+');
   else
     putchar('.');
   }
  putchar('\n');
  }
#define print_msg0(fmt)		printf (fmt)
#define print_msg1(fmt,a)	printf (fmt,a)
#else
#define print_queues(brcbCtrl)
#define print_msg0(fmt)
#define print_msg1(fmt,a)
#endif

/************************************************************************/
/* 			mvl61850_mk_rptid				*/
/* Construct RptID from info in rptCtrl.				*/
/* RETURNS:	SD_SUCCESS or SD_FAILURE				*/
/*		Also on error, put a NULL string in RptID & log error.	*/
/************************************************************************/
ST_RET mvl61850_mk_rptid (MVLU_RPT_CTRL *rptCtrl, ST_CHAR *RptID, size_t max_len)
  {
ST_RET retcode;

  /* If domname + '/' + basrcb_name will fit in RptID, then write it.	*/
  if (strlen (rptCtrl->dsNvl->nvl_scope.dom->name) + strlen (rptCtrl->basrcb_name) + 1 <= max_len)
    {
    sprintf (RptID, "%s/%s", rptCtrl->dsNvl->nvl_scope.dom->name,
             rptCtrl->basrcb_name);
    retcode = SD_SUCCESS;
    }
  else
    {
    MVL_LOG_ERR0 ("Cannot construct RptID (too long)");	/* should NEVER happen*/
    RptID [0] = '\0';		/* return empty RptID	*/
    retcode = SD_FAILURE;
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl61850_rcb_cleanup				*/
/* Cleanup RPT info to prepare for next report.				*/
/************************************************************************/
ST_VOID mvl61850_rcb_cleanup (MVLU_RPT_CLIENT *rptClient)
  {
MVL_NVLIST_CTRL *dsNvl;
ST_INT incSize;		/* num bytes for inclusion bitstring	*/
  dsNvl = rptClient->rpt_ctrl->dsNvl;
  incSize = BSTR_NUMBITS_TO_NUMBYTES(dsNvl->num_of_entries);
  /* Reset reasons, etc. to prepare for new report triggers.	*/
  memset (rptClient->reasons_data, 0, dsNvl->num_of_entries);
  memset (rptClient->changed_flags, 0, incSize);
  rptClient->numTrgs = 0;
  }

/************************************************************************/
/*			mvl61850_brcb_rpt_set_entryid			*/
/************************************************************************/
ST_RET mvl61850_brcb_rpt_set_entryid (MVLU_RPT_CLIENT *rptClient, ST_UCHAR *EntryID)
  {
BUFLIST_ENTRY *entry;
BUFLIST_ENTRY *foundEntry = NULL;	/* matching entry, assume NULL (not found)*/
MVL61850_BRCB_CTRL *brcbCtrl;
ST_RET retcode = SD_FAILURE;	/* assume failure	*/
ST_UCHAR all_zero[8]={0,0,0,0,0,0,0,0};

  /* Assume this is a BRCB. Should never get here otherwise.	*/
  assert (rptClient->rpt_ctrl->rcb_type == RCB_TYPE_IEC_BRCB);
  brcbCtrl = &rptClient->rpt_ctrl->brcbCtrl;

  /* Is client writing all 0?	*/
  if (memcmp (all_zero, EntryID, 8) == 0)
    {	/* special case. Just resync to oldest rpt in buffer	*/
    brcbCtrl->rpt_list_next = brcbCtrl->rpt_list;	/* oldest	*/
    return (SD_SUCCESS);	/* this can never fail*/
    }

  /* Search "sent" list for this ID.	*/
  /* If "rpt_list_next != NULL" start search there. 			*/
  /* If "rpt_list_next == NULL" start search at end of list.		*/
  if (brcbCtrl->rpt_list_next)
    entry = brcbCtrl->rpt_list_next;
  else
    entry = (BUFLIST_ENTRY *) list_find_last ((DBL_LNK *) brcbCtrl->rpt_list);
  for ( ;
       entry != NULL;
       entry = (BUFLIST_ENTRY *) list_find_prev ((DBL_LNK *) brcbCtrl->rpt_list, (DBL_LNK *) entry))
    {
    if (memcmp (entry->EntryID, EntryID, 8) == 0)
      {
      foundEntry = entry;
      break;
      }
    }
  /* If matching entry found, just point to next one.	*/
  if (foundEntry)
    {
    brcbCtrl->rpt_list_next = list_get_next (brcbCtrl->rpt_list, entry);
    retcode = SD_SUCCESS;
    }
  else
    {	/* not found, point to "oldest" rpt	*/
    brcbCtrl->rpt_list_next = brcbCtrl->rpt_list;	/* oldest	*/
    brcbCtrl->BufOvfl = SD_TRUE;		/* overflow occurred	*/
    }

  return (retcode);
  }

/************************************************************************/
/*			fill_asn1_len_array				*/
/* Compute the ASN.1 encoded len for each var in a rpt dataset, and	*/
/* fill in "bufEntry->asn1_len_array".					*/
/************************************************************************/
static ST_RET fill_asn1_len_array (
	BUFLIST_ENTRY *bufEntry,
	MVL_NVLIST_CTRL *dsNvl)	/* rpt dataset				*/
  {
ST_UCHAR *tmp_asn1_buf;	/* temporary ASN.1 buffer (allocated)	*/
ST_INT tmp_asn1_buf_size = mvl_cfg_info->max_msg_size;	/* reasonable size*/
ST_INT j;
ST_RET retCode=SD_SUCCESS;	/* assume success	*/

  tmp_asn1_buf = chk_malloc (tmp_asn1_buf_size);
  for (j = 0; j < dsNvl->num_of_entries; ++j)
    {
    if (bufEntry->var_data[j].reason_for_incl != 0)
      {		/* this var should be included in rpt	*/
      ST_UCHAR *asn1_ptr;
      MVL_TYPE_CTRL *type_ctrl = dsNvl->entries[j]->type_ctrl;

      retCode = ms_local_to_asn1_2 (type_ctrl->rt, type_ctrl->num_rt,
                bufEntry->var_data[j].data_ptr,	/* CRITICAL: use data from buffer entry*/
                tmp_asn1_buf,
                tmp_asn1_buf_size,
                &asn1_ptr,	/* function sets asn1_ptr (not needed here)*/
                &bufEntry->asn1_len_array[j]);	/* function sets len at this ptr*/
      if (retCode)
        {
	/* Should NEVER fail, but if so, don't continue with other vars.*/
        break;
        }
      }
    }	/* end loop	*/
  chk_free (tmp_asn1_buf);
  return (retCode);
  }

/************************************************************************/
/*			buflist_entry_create				*/
/* Allocate and initialize BUFLIST_ENTRY and add it to list.		*/
/* Also update brcbCtrl->cur_bufsize & brcbCtrl->rpt_count.		*/
/************************************************************************/
BUFLIST_ENTRY *buflist_entry_create (MVLU_RPT_CLIENT *rptClient)
  {
BUFLIST_ENTRY *bufEntry;
MVL_NVLIST_CTRL *dsNvl = rptClient->rpt_ctrl->dsNvl;
ST_INT j;
MVL61850_BRCB_CTRL *brcbCtrl = &rptClient->rpt_ctrl->brcbCtrl;
ST_UINT mem_size;	/* size of one big buffer allocated for this entry*/
ST_CHAR *tmp_ptr;	/* ptr to variable data. Changes as variables added.*/

  /* Compute memory size to allocate for this entry.			*/
  /* Add memory for each variable, but only if the variable is		*/
  /* included in this report (check "reason for inclusion").		*/
  mem_size = sizeof (BUFLIST_ENTRY) + dsNvl->num_of_entries * (sizeof(VAR_DATA) + sizeof (ST_INT));
  for (j = 0; j < dsNvl->num_of_entries; j++)
    {
    /* Check "reason for inclusion".	*/
    if (rptClient->reasons_data[j] != 0)
      {				/* this var must be in the report	*/
      MVL_TYPE_CTRL *type_ctrl = dsNvl->entries[j]->type_ctrl;
      mem_size += type_ctrl->data_size;	/* add space for this variable	*/
      }
    }

  /* Allocate one big buffer to contain everything for this entry.	*/
  bufEntry = (BUFLIST_ENTRY *) chk_calloc (1, mem_size);

  /* Set pointers to information within the big buffer.		*/
  /* Set "var_data" to point immediately after "bufEntry".	*/
  bufEntry->var_data = (VAR_DATA *) (bufEntry + 1);
  /* Set "asn1_len_array" to point immediately after "var_data".	*/
  bufEntry->asn1_len_array = (ST_INT *) (bufEntry->var_data + dsNvl->num_of_entries);
  /* Set "tmp_ptr" to point immediately after "asn1_len_array". Variable*/
  /* data will be saved here. This is incremented as variables added.	*/
  tmp_ptr = (ST_CHAR *) (bufEntry->asn1_len_array + dsNvl->num_of_entries);

  bufEntry->mem_size = mem_size;	/* save for cur_bufsize calculations.*/
  bufEntry->num_var = dsNvl->num_of_entries;

  for (j = 0; j < bufEntry->num_var; j++)
    {
    /* Check "reason for inclusion".	*/
    if (rptClient->reasons_data[j] != 0)
      {				/* this var must be in the report	*/
      ST_INT data_len;
      MVL_VAR_ASSOC *var_assoc = dsNvl->entries[j];
      MVL_TYPE_CTRL *type_ctrl = dsNvl->entries[j]->type_ctrl;
      VAR_DATA *var_data = &bufEntry->var_data[j];

      /* Find data_len	*/
      data_len = type_ctrl->data_size;

      var_data->data_len = data_len;
      var_data->data_ptr = tmp_ptr;	/* set data_ptr for this variable*/
      tmp_ptr += data_len;	/* update tmp_ptr to prepare for next variable*/

      memcpy (var_data->data_ptr, var_assoc->data, data_len);
      /* Save "reason for inclusion".	*/
      var_data->reason_for_incl = rptClient->reasons_data[j];
      }
    }

  /* Fill in other entry data.	*/
  /* Point to last 4 bytes of Ostring, cast to INT32 & increment.*/
  ++(*(ST_INT32 *)(&rptClient->basrcb.EntryID[4]));
  /* Then copy to bufEntry.	*/
  memcpy (bufEntry->EntryID, rptClient->basrcb.EntryID, 8);
  /* Call user function to get report time.	*/
  u_mvlu_rpt_time_get (&bufEntry->TimeOfEntry);
  /* Copy report time to RCB.	*/
  memcpy (&rptClient->basrcb.TimeofEntry, &bufEntry->TimeOfEntry, sizeof (MMS_BTIME6));

  /* Fill in "bufEntry->asn1_len_array", needed later by "chk_seg_needed".	*/
  if (fill_asn1_len_array (bufEntry, dsNvl)!=SD_SUCCESS)
    {
    /* NOTE: This should NEVER fail, but if it does, just log it.	*/
    /*       Real encode will almost certainly fail later.		*/
    MVL_LOG_ERR0 ("Cannot calculate ASN.1 length for Buffered Report.");
    }

  /* Add entry to list.	*/
  list_add_last (&brcbCtrl->rpt_list, bufEntry);
  brcbCtrl->rpt_count++;	/* Update list count	*/

  /* If next ptr is NULL (all old rpts sent), point to this new entry.*/
  if (brcbCtrl->rpt_list_next == NULL)
    brcbCtrl->rpt_list_next = bufEntry;

  brcbCtrl->cur_bufsize += bufEntry->mem_size;	/* update buffer size*/
  return (bufEntry);
  }

/************************************************************************/
/*			buflist_entry_destroy				*/
/* Remove BUFLIST_ENTRY from list and free it.				*/
/* Also update brcbCtrl->cur_bufsize & brcbCtrl->rpt_count.		*/
/************************************************************************/
ST_VOID buflist_entry_destroy (MVL61850_BRCB_CTRL *brcbCtrl, BUFLIST_ENTRY *entry)
  {
  if (entry == brcbCtrl->rpt_list_next)
    {
    /* Deleting next rpt to send.			*/
    /* CRITICAL: must fix up next to send.		*/
    brcbCtrl->rpt_list_next = list_get_next (brcbCtrl->rpt_list, entry);
    }

  list_unlink (&brcbCtrl->rpt_list, entry);	/* remove rpt from list	*/
  brcbCtrl->rpt_count--;			/* update list count	*/

  brcbCtrl->cur_bufsize -= entry->mem_size;	/* update buffer size*/
  chk_free (entry);
  }

/************************************************************************/
/*			buflist_discard_old_gi				*/
/* Search for old GI report in buffer, and delete it.			*/
/************************************************************************/
ST_VOID buflist_discard_old_gi (MVL61850_BRCB_CTRL *brcbCtrl)
  {
BUFLIST_ENTRY *entry;
  for (entry = brcbCtrl->rpt_list;
       entry != NULL;
       entry = list_get_next (brcbCtrl->rpt_list, entry))
    {
    /* For GI rpt, "reason_for_incl" for all vars is MVLU_TRGOPS_GI,	*/
    /* so just check reason for first var.				*/
    if (entry->var_data[0].reason_for_incl == MVLU_TRGOPS_GI)
      {
      /* Remove entry from list and free it.	*/
      buflist_entry_destroy (brcbCtrl, entry);
      break;	/* never more than one GI, so stop looping now	*/
      }
    }
  }

/************************************************************************/
/*			buflist_discard_old				*/
/* If memory usage exceeds buffer size, remove oldest reports from list	*/
/* until memory usage is reduced.					*/
/************************************************************************/
ST_VOID buflist_discard_old (MVL61850_BRCB_CTRL *brcbCtrl)
  {
BUFLIST_ENTRY *entry;
  while (brcbCtrl->cur_bufsize > brcbCtrl->brcb_bufsize)
    {
    entry = brcbCtrl->rpt_list;	/* find first on list	*/
    if (entry != NULL)
      {
      if (entry == brcbCtrl->rpt_list_next)
        {
        /* Deleting next rpt to send (i.e. rpts are being lost).	*/
        /* Set BufOvfl flag.	*/
        brcbCtrl->BufOvfl = SD_TRUE;
        brcbCtrl->rpts_lost_count++;	/* keep a count of lost reports	*/
        }
      /* Remove entry from list and free it.	*/
      buflist_entry_destroy (brcbCtrl, entry);
      }
    else
      {		/* This error should never happen. Buffer is much too small.*/
      MVL_LOG_ERR0 ("Report buffer size too small to store any reports.");
      }
    }	/* end loop	*/
  }

/************************************************************************/
/*			mvl61850_brcb_rpt_lists_clean			*/
/* Delete and destroy all entries on the buffered report linked list	*/
/* for this BRCB.							*/
/************************************************************************/
ST_VOID mvl61850_brcb_rpt_lists_clean (MVL61850_BRCB_CTRL *brcbCtrl)
  {
BUFLIST_ENTRY *entry;
  /* Loop removing first entry from list.	*/
  while ((entry = brcbCtrl->rpt_list) != NULL)
    buflist_entry_destroy (brcbCtrl, entry);	/* removes it from list*/

  assert (brcbCtrl->rpt_list_next == NULL);	/* if list empty, this must be NULL*/

  if (brcbCtrl->rpt_count != 0 || brcbCtrl->cur_bufsize != 0)
    {
    MVL_LOG_ERR2 ("ERROR removing buffered reports from list (count=%d, bufsize=%d). Possible memory leak.",
                brcbCtrl->rpt_count, brcbCtrl->cur_bufsize);
    brcbCtrl->rpt_count = 0;		/* reset count		*/
    brcbCtrl->cur_bufsize = 0;		/* reset buffer size	*/
    }
  brcbCtrl->BufOvfl = SD_FALSE;		/* Clear BufOvfl flag	*/
  brcbCtrl->rpt_list_next = NULL;	/* CRITICAL: must not point to deleted entry*/
  }

/************************************************************************/
/*			mvl61850_brcb_rpt_save				*/
/* For BRCB, just buffer the raw data in a linked list to be encoded	*/
/* and sent later (see mvl61850_brcb_rpt_send).				*/
/************************************************************************/
ST_RET mvl61850_brcb_rpt_save (MVLU_RPT_CLIENT *rptClient)
  {
  /* If this is GI rpt, discard previous GI rpt from buffer before	*/
  /* saving new one in buffer.						*/
  if (rptClient->reasons_data[0] == MVLU_TRGOPS_GI)
    buflist_discard_old_gi (&rptClient->rpt_ctrl->brcbCtrl);

  buflist_entry_create (rptClient);	/* add rpt to buffer		*/

  /* If buffer is full, discard oldest reports until not full.	*/
  buflist_discard_old (&rptClient->rpt_ctrl->brcbCtrl);
  mvl61850_rcb_cleanup (rptClient);	/* prepare for next RPT.	*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvl61850_brcb_rpt_send				*/
/* Encode and send a Buffered Report (possibly in multiple segments).	*/
/* NOTE: This is similar to mvl61850_urcb_rpt_send. The main difference	*/
/*       is that it gets most data from the BUFLIST_ENTRY struct.	*/
/* NOTE: DOES NOT use rptClient->reasons_data, rptClient->changed_flags,*/
/*       rptClient->numTrgs. Use "reason_for_incl" from BUFLIST_ENTRY	*/
/*       to construct inclusion bitstring.				*/
/************************************************************************/
ST_RET mvl61850_brcb_rpt_send (MVLU_RPT_CTRL *rptCtrl,
	MVLU_RPT_CLIENT *rptClient,
	BUFLIST_ENTRY *bufEntry,
	ST_BOOLEAN *all_seg_done)
  {
MVL_NVLIST_CTRL *dsNvl;
MVL_NVLIST_CTRL *rptNvl;
MVL_VAR_ASSOC *va;
ST_INT j;		/* loop counter	*/
ST_RET retCode;
ST_INT sendIndex;
ST_INT sendIndexSave;	/* save index before data, compare after data	*/
ST_UINT8 *optFlds;
MVLU_BASRCB *basrcb;
ST_INT incSize;		/* num bytes for inclusion bitstring	*/
MVL_VAR_ASSOC *tmp_va_arr;	/* alloc array of structs	*/
ST_INT tmp_va_arr_size;		/* num of entries in tmp_va_arr	*/
MVL_VAR_ASSOC *tmp_va;		/* current entry in tmp_va_arr	*/
/* For Segmented reports, all segments are generated by this function,
 * so these parameters can be local variables.
 */
ST_INT next_va_index;		/* index into dsNvl va array	*/
ST_BOOLEAN segNeeded;		/* If TRUE, segmenting needed	*/
ST_BOOLEAN MoreSegmentsFollow;
ST_CHAR *tmp_dataref_buf;
ST_CHAR tmpRptID [MVL61850_MAX_RPTID_LEN+1];


  retCode = SD_SUCCESS;		/* assume success. Change if error detected.*/
  *all_seg_done = SD_FALSE;	/* initialize flag			*/

  basrcb = &rptClient->basrcb;

  optFlds = basrcb->OptFlds.data_1;
  dsNvl = rptCtrl->dsNvl;
  rptNvl = &rptCtrl->rptNvl;

  incSize = BSTR_NUMBITS_TO_NUMBYTES(dsNvl->num_of_entries);

  MVLU_LOG_FLOW1 ("Building IEC-61850 BRCB Report, MVL_NET_INFO " S_FMT_PTR, rptClient->netInfo);

  /* Need tmp va's for options, array of data_refs, array of dataset vars,*/
  /* array of reason_for_incl.						*/
  tmp_va_arr_size = MVLU_MAX_RPT_OPTS + (dsNvl->num_of_entries * 3);
  tmp_va_arr = M_CALLOC (MSMEM_GEN, sizeof (MVL_VAR_ASSOC), tmp_va_arr_size);

  /* Need tmp buffer for datarefs. One buffer for all.		*/
  /* Allow max len plus NULL for each dataref.			*/
  tmp_dataref_buf = M_CALLOC (MSMEM_GEN, (MVL61850_MAX_OBJREF_LEN+1), dsNvl->num_of_entries);

  assert (rptCtrl->dsNvl->num_of_entries);	/* must be >0 entries	*/

  /* MAIN LOOP	*/
  /* Start with "bufEntry->cur_va_index" and "bufEntry->SubSeqNum".*/
  /* Usually they are both 0, but if segmenting occurs and buffers were	*/
  /* not available to send all segments, they indicate what is next.	*/
  for (next_va_index = bufEntry->cur_va_index;
       next_va_index < rptCtrl->dsNvl->num_of_entries;
       bufEntry->cur_va_index = next_va_index, bufEntry->SubSeqNum++)
    {	/* BEGIN MAIN LOOP	*/
    if (retCode)
      break;	/* something failed in this loop or earlier loop, so stop now.*/

    /* If buffers not available, break out of loop.			*/
    /* Use cur_va_index, SubSeqNum in bufEntry as starting point	*/
    /* next time this function is called.				*/
    if (mvl_ureq_bufs_avail (rptClient->netInfo) <= 0)	/* stack bufs avail to send rpt	*/
      break;

    tmp_va = tmp_va_arr;	/* start loop pointing to first entry	*/
    sendIndex = 0;		/* start with first rptNvl->entries.	*/

    /* Prepare a Report NVL to be sent, based on the			*/
    /* options, data, and inclusion bitstring in the rptCtrl.		*/

    /* CRITICAL: asn1_len_array must be filled in before call to chk_seg_needed.*/
    segNeeded = chk_seg_needed (rptClient, bufEntry->asn1_len_array, bufEntry->cur_va_index, &next_va_index);

    if (next_va_index - bufEntry->cur_va_index == 0)
      {
      MVL_LOG_ERR0 ("MMS PDU size too small to fit ANY variables in IEC-61850 Report. Report not sent.");
      retCode = SD_FAILURE;
      break;	/* break out of loop so normal cleanup occurs.	*/
      }

    /* We will create a NVL to send, using the dsNvl as a model ...	*/

    /* RptID and OptFlds are always the first 2 entries in the Rpt NVL.	*/

    /* Get RptID from RCB (OK because changing it causes PurgeBuf).	*/
    tmp_va->type_id = rptCtrl->rpt_typeids.vstring65;
    if (basrcb->RptID [0] == '\0')
      {		/* RptID not set in RCB, so construct it.	*/
      mvl61850_mk_rptid (rptCtrl, tmpRptID, MVL61850_MAX_RPTID_LEN);
      tmp_va->data = tmpRptID;
      }
    else
      tmp_va->data = basrcb->RptID;
    MVLU_LOG_CFLOW1 ("  RptID=%s", (ST_CHAR *)tmp_va->data);	/* log before tmp_va++*/
    rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/

    /* Get OptFlds from RCB (OK because changing it causes PurgeBuf).	*/
    tmp_va->type_id = rptCtrl->rpt_typeids.bvstring10;
    tmp_va->data = &basrcb->OptFlds;
    rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
    MVLU_LOG_CFLOW2 ("  OptFld = 0x%02x 0x%02x", basrcb->OptFlds.data_1[0],
                     basrcb->OptFlds.data_1[1]);	/* 10 bit bstr (2 bytes)	*/

    /* Add optional RCB vars to NVL, depending on OptFlds.	*/

    /* Get SqNum from RCB.	*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_SQNUM))
      {
      assert (rptCtrl->rcb_type == RCB_TYPE_IEC_BRCB);
      /* SqNum in BRCB is INT16U	*/
      tmp_va->type_id = rptCtrl->rpt_typeids.int16u;
      tmp_va->data = &basrcb->SqNumInt16u;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      MVLU_LOG_CFLOW1 ("    SqNum : %u", (unsigned) basrcb->SqNumInt16u);
      }

    /* Get TimeofEntry from BUFLIST_ENTRY.	*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_TIMESTAMP))
      {
      MVLU_LOG_CFLOW2 ("    RptTim : %ld ms, %ld days",
              (ST_LONG)bufEntry->TimeOfEntry.ms, (ST_LONG)bufEntry->TimeOfEntry.day);

      tmp_va->type_id = rptCtrl->rpt_typeids.btime6;
      tmp_va->data = &bufEntry->TimeOfEntry;	/* use time saved in buffer*/
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      }

    /* Get DatSet from RCB (OK because changing it causes PurgeBuf).	*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_DATSETNAME))
      {
      /* UCA called it OutDat instead of DatSet. Value is same.	*/
      MVLU_LOG_CFLOW1 ("    DatSet : %s", basrcb->DatSetNa);
      tmp_va->type_id = rptCtrl->rpt_typeids.vstring129; /*ObjectReference*/
      tmp_va->data = basrcb->DatSetNa;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      }

    /* Get BufOvfl.	*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_BUFOVFL))
      {
      tmp_va->type_id = rptCtrl->rpt_typeids.mmsbool;
      tmp_va->data = &rptCtrl->brcbCtrl.BufOvfl;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      }

    /* Get EntryID from BUFLIST_ENTRY.	*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_ENTRYID))
      {
      /* NOTE: EntryID only incremented when RPT buffered.		*/
      /*       Copied to bufEntry->EntryID after incrementing.	*/
      tmp_va->type_id = rptCtrl->rpt_typeids.ostring8;
      tmp_va->data = bufEntry->EntryID;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      }

    /* Add "ConfRev" if enabled by "OptFlds".	*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_CONFREV))
      {
      tmp_va->type_id = rptCtrl->rpt_typeids.int32u;
      tmp_va->data = &basrcb->ConfRev;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      }

    if (segNeeded)
      {
      /* Set appropriate bit in OptFlds.			*/
      /* NOTE: this bit only for output. Never used as input.	*/
      BSTR_BIT_SET_ON (optFlds,OPTFLD_BITNUM_SUBSEQNUM);

      tmp_va->type_id = rptCtrl->rpt_typeids.int16u;
      tmp_va->data = &bufEntry->SubSeqNum;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/

      if (next_va_index < rptCtrl->dsNvl->num_of_entries)
        MoreSegmentsFollow = SD_TRUE;
      else
        MoreSegmentsFollow = SD_FALSE;

      tmp_va->type_id = rptCtrl->rpt_typeids.mmsbool;
      tmp_va->data = &MoreSegmentsFollow;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      }
    else
      {
      BSTR_BIT_SET_OFF (optFlds,OPTFLD_BITNUM_SUBSEQNUM);
      }

    /* Construct inclusion bitstring using "reason_for_incl" in BUFLIST_ENTRY.	*/
    tmp_va->type_id = rptCtrl->inclusion_typeid;
    /* just set the bits included in this segment.	*/
    memset (rptClient->segmented_inclusion, 0, incSize);	/* start clean	*/
    for (j = bufEntry->cur_va_index; j < next_va_index; ++j)
      {
      if (bufEntry->var_data[j].reason_for_incl)
        BSTR_BIT_SET_ON (rptClient->segmented_inclusion, j);
      }
    tmp_va->data = rptClient->segmented_inclusion;
    rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/

    /* If data-Ref enabled, go through inclusion_data to decide what to send*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_DATAREF))
      {
      for (j = bufEntry->cur_va_index; j < next_va_index; ++j)
        {
        if (bufEntry->var_data[j].reason_for_incl)
          {	/* include dataRef for this variable	*/
          tmp_va->type_id = rptCtrl->rpt_typeids.vstring129; /*ObjectReference*/
          /* point to right part of tmp buffer & construct dataref	*/
          tmp_va->data = &tmp_dataref_buf [j * (MVL61850_MAX_OBJREF_LEN+1)];
          mvl61850_objref_create (dsNvl->entries[j]->name, &dsNvl->va_scope[j],
                               tmp_va->data);
          rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
          }
        }
      }

    /* HERE'S DATA: Go through inclusion_data to decide what to send	*/
    sendIndexSave = sendIndex;	/* save index before data included	*/
    for (j = bufEntry->cur_va_index; j < next_va_index; ++j)
      {
      if (bufEntry->var_data[j].reason_for_incl)
        {	/* include this variable	*/
        va = dsNvl->entries[j];
        tmp_va->data = bufEntry->var_data[j].data_ptr;	/* buffered data*/
        tmp_va->type_id = va->type_id;
        MVLU_LOG_CFLOW3 ("  Including variable %d ('%s'), reason_for_incl=0x%02X",
                        j, va->name, bufEntry->var_data[j].reason_for_incl);
        rptNvl->entries[sendIndex++] = tmp_va++;
        }
      }
    assert (sendIndex>sendIndexSave);	/* make sure SOME data included	*/

    /* If "reason" enabled, go through inclusion_data to decide what to send*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_REASON))
      {
      for (j = bufEntry->cur_va_index; j < next_va_index; ++j)
        {
        if (bufEntry->var_data[j].reason_for_incl)
          {	/* include reason for this variable	*/
          /* Don't need BVSTR here, because size is fixed.	*/
          tmp_va->type_id = rptCtrl->rpt_typeids.bstr6;
          tmp_va->data = &bufEntry->var_data[j].reason_for_incl;
          rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
          }
        }
      }
    rptNvl->num_of_entries = sendIndex;
    assert (rptNvl->num_of_entries <= rptCtrl->maxNumRptVars);	/* past end of array?*/
    assert ((tmp_va-tmp_va_arr) <= tmp_va_arr_size);	/* past end of array?*/

    /* Encode the InformationReport and send it.	*/
    retCode = mvl_info_variables (rptClient->netInfo,
                        rptNvl,
                        SD_FALSE);	/* FALSE means "send as NVL"	*/
    }	/* END MAIN LOOP	*/

  /* Did we send all segments of this report?	*/
  if (retCode == SD_SUCCESS && bufEntry->cur_va_index == rptCtrl->dsNvl->num_of_entries)
    {
    *all_seg_done = SD_TRUE;	/* YES: Set flag	*/
    /* CRITICAL: Reset these to be ready for retransmit later.	*/
    bufEntry->cur_va_index = 0;
    bufEntry->SubSeqNum = 0;
    }

  M_FREE (MSMEM_GEN, tmp_va_arr);
  M_FREE (MSMEM_GEN, tmp_dataref_buf);
  return (retCode);
  }

/************************************************************************/
/*			mvl61850_brcb_enable_all			*/
/* Start buffering all buffered reports.				*/
/************************************************************************/
ST_VOID mvl61850_brcb_enable_all ()
  {
MVLU_RPT_CTRL *rptCtrl;

  for (rptCtrl = mvl61850_rpt_ctrl_list;
       rptCtrl != NULL;
       rptCtrl = (MVLU_RPT_CTRL *) list_get_next (mvl61850_rpt_ctrl_list, rptCtrl))
    {
    if (rptCtrl->rcb_type == RCB_TYPE_IEC_BRCB)
      rptCtrl->brcbCtrl.enabled_once = SD_TRUE;
    }
  }

/************************************************************************/
/*			mvl61850_brcb_entryid_init			*/
/* Set initial value of EntryID for this BRCB. EntryID must point to	*/
/* an 8 byte octet string.						*/
/************************************************************************/
ST_VOID mvl61850_brcb_entryid_init (MVLU_RPT_CTRL *rptCtrl, ST_UINT8 *EntryID)
  {
  memcpy (rptCtrl->only_client.basrcb.EntryID, EntryID, 8);
  }

/************************************************************************/
/*			mvl61850_rpt_change_scan_va_done			*/
/* Used by code that scans for dchg, qchg, dupd changes.		*/
/* This function is called by 'mvlu_rd_prim_done' when all "leaf"	*/
/* functions for a VA have been completed (i.e. data ready for this VA).*/
/************************************************************************/
ST_VOID mvl61850_rpt_change_scan_va_done (MVL_IND_PEND *indCtrl,
					MVL_VAR_ASSOC *va)
  {
  /* If "rpt_reason" set by ANY leaf function, call mvlu_rpt_va_change.	*/
  if (va->rpt_reason)
    mvlu_rpt_va_change (va, va->rpt_reason, NULL);
  }

/************************************************************************/
/*			mvl61850_rpt_change_scan_done			*/
/* Used by code that scans for dchg, qchg, dupd changes.		*/
/* This function is called when the scan is complete.			*/
/* (i.e. 'mvlu_rd_prim_done' has been called for all "leafs" of all	*/
/* variables in the report). Everything is ready to build a report,	*/
/* so build it now.							*/
/* CRITICAL: when all done, this function must free the temporary	*/
/*   MVL_IND_PEND struct by calling "mvlu_integrity_scan_destroy".	*/
/************************************************************************/
ST_RET mvl61850_rpt_change_scan_done (MVL_IND_PEND *indCtrl)
  {
ST_RET retCode = SD_SUCCESS;	/* nothing to fail here	*/

  /* NOTE: not much to do here. DO NOT send reports.			*/
  /*       mvlu_rpt_service chks numTrgs & BufTim & sends when needed.	*/

  /* CRITICAL: free the temporary MVL_IND_PEND struct.	*/
  mvlu_integrity_scan_destroy (indCtrl);
  return (retCode);
  }

/************************************************************************/
/*			mvl61850_rpt_change_scan			*/
/* Scan all IEC 61850 Report Datasets for dchg, qchg, dupd.		*/
/************************************************************************/
ST_VOID mvl61850_rpt_change_scan (/*ST_VOID*/ST_BOOLEAN bScanForce)
  {
MVLU_RPT_CTRL *rptCtrl;
MVL_NVLIST_CTRL *dsNvl;
ST_INT j;
ST_DOUBLE timeNow;

  /* Reset "already_scanned=SD_FALSE" for each NVL.				*/
  /* Reset it for every NVL, even if not enabled (faster).			*/
  for (rptCtrl = mvl61850_rpt_ctrl_list;
       rptCtrl != NULL;
       rptCtrl = (MVLU_RPT_CTRL *) list_get_next (mvl61850_rpt_ctrl_list, rptCtrl))
    {
    dsNvl = rptCtrl->dsNvl;
    if (dsNvl)	/* dsNvl might be NULL	*/
      dsNvl->already_scanned = SD_FALSE;
    }

  timeNow = sGetMsTime ();	/* get time just once. Use it in loop.*/

  /* Loop through all Report NVLs to scan for data changes.		*/
  /* Set already_scanned=SD_TRUE for each NVL. If same NVL used in another	*/
  /* Report, do NOT rescan it.						*/
  for (rptCtrl = mvl61850_rpt_ctrl_list;
       rptCtrl != NULL;
       rptCtrl = (MVLU_RPT_CTRL *) list_get_next (mvl61850_rpt_ctrl_list, rptCtrl))
    {
    /* If scan_rate > 0, check timer. If not, go ahead and scan now.	*/
    /* NOTE: if scan_rate is too small, there may be extra delay between scans.*/
    if (rptCtrl->scan_rate > 0)
      {	/* check timer to see if it's time to scan	*/
      if (bScanForce==SD_FALSE && timeNow < rptCtrl->next_scan_start)
        continue;	/* DO NOT scan now. Skip to next rptCtrl	*/
      else
        rptCtrl->next_scan_start = timeNow + rptCtrl->scan_rate;
      }

    dsNvl = rptCtrl->dsNvl;
    if (dsNvl!=NULL && dsNvl->already_scanned==SD_FALSE)	/* dsNvl might be NULL*/
      {		/* This NVL not yet scanned.	*/
      /* If URCB enabled or BRCB enabled ONCE, start scan.*/
      if (rptCtrl->only_client.basrcb.RptEna
          || (rptCtrl->rcb_type == RCB_TYPE_IEC_BRCB
              && rptCtrl->brcbCtrl.enabled_once))
        {
        /* Initialize "rpt_reason=0" for all variables. May be changed	*/
        /* in leaf functions. Checked in mvl61850_rpt_change_scan_va_done.	*/
        for (j = 0; j < dsNvl->num_of_entries; j++)
          dsNvl->entries[j]->rpt_reason = 0;

        /* This is NOT an Integrity scan, but mvlu_integrity_scan_read	*/
        /* does what we need if the right callback functions passed to it.*/
        /* Scan for dchg, qchg, dupd.					*/
        /* This function "begins" the scan. When the scan is		*/
        /* complete, "mvl61850_rpt_change_scan_done" is called to build the	*/
        /* the report. If ALL "leaf" functions are synchronous,		*/
        /* "mvl61850_rpt_change_scan_done" is called BEFORE this function returns.*/
        /* CRITICAL: this function allocates a temporary MVL_IND_PEND	*/
        /*   which must be freed by mvl61850_rpt_change_scan_done.		*/
        mvlu_integrity_scan_read (&rptCtrl->only_client,
                mvl61850_rpt_change_scan_va_done,
                mvl61850_rpt_change_scan_done);
        dsNvl->already_scanned = SD_TRUE;	/* this NVL scanned. Don't scan again for another RCB*/
        }
      }
    }
  }

/************************************************************************/
/*			mvl61850_get_rcb				*/
/* Find IEC 61850 RCB (URCB or BRCB).					*/
/* NOTE: Caller can access MVLU_RPT_CTRL, if needed, by using "rpt_ctrl"*/
/* member of MVLU_RPT_CLIENT.						*/
/************************************************************************/
MVLU_BASRCB *mvl61850_get_rcb (MVL_VAR_ASSOC *baseVa,
	RUNTIME_TYPE *rt,
	MVLU_RPT_CLIENT **rptClientOut)
  {
MVLU_RPT_CTRL *rptCtrl;
MVLU_BASRCB *rcb = NULL;	/* ptr to be returned	*/

   /* Find the associated Report Control, using this RT as a key */
  for (rptCtrl = mvl61850_rpt_ctrl_list;
       rptCtrl != NULL;
       rptCtrl = (MVLU_RPT_CTRL *) list_get_next (mvl61850_rpt_ctrl_list, rptCtrl))
    {
    if (rptCtrl->base_va == baseVa &&
        (rt > rptCtrl->rcbRtHead) &&
        (rt <= rptCtrl->rcbRtHead + rptCtrl->rcbRtHead->u.str.num_rt_blks))
      {
      if (rptCtrl->rcb_type != RCB_TYPE_UCA)
        {
        rcb = &rptCtrl->only_client.basrcb;
        if (rptClientOut != NULL)		/* set optional ptr	*/
          *rptClientOut = &rptCtrl->only_client;	/* always EXACTLY one client*/
        }
      break;	/* rptCtrl is set. Just need to return it.	*/
      }
    }
  /* rcb will be NULL if rptCtrl NOT found or NOT 61850	*/
  return (rcb);
  }

/************************************************************************/
/*			mvl61850_rpt_dataset_create			*/
/* Create IEC 61850 Report Control Block resources for a dataset.	*/
/* Assumes EXACTLY one client per rptCtrl (see rptCtrl->only_client).	*/
/************************************************************************/
ST_RET mvl61850_rpt_dataset_create (MVLU_RPT_CTRL *rptCtrl,
			MVL_NVLIST_CTRL *dsNvl)
  {
RUNTIME_TYPE *incRt;
ST_RET rc;
ST_INT numDsVars;
ST_INT changed_flags_size;
ST_INT allocSize;

  if (rptCtrl->dsNvl)
    {	/* dsNvl already set. Must call mvl61850_rpt_dataset_destroy first*/
    MVL_LOG_ERR0 ("Report Dataset already exists. Must destroy it before creating new one.");
    return (SD_FAILURE);
    }

  if (dsNvl == NULL)
    return (SD_SUCCESS);	/* this is allowed, but no need to do anything*/

  numDsVars = dsNvl->num_of_entries;

  /* NOTE: RptID (in MVLU_BASRCB struct) might be empty (0 length string).*/
  /*   If so, a string will be constructed each time a report is built	*/
  /*   (see "mvlu_mk_rptid" function).				*/

/* Alloc the entries table; allow for RptID, OptFlds, SqNum, RptTim, 	*/
/* DatSet, BufOvfl, EntryID, ConfRev,					*/
/* SubSeqNum, MoreSegmentsFollow, & InclusionBitstring plus ARRAYS of	*/
/* dataRefs, values, and Reasons.					*/
/* "assert" later if "maxNumRptVars" exceeded.				*/
/* Allow (3 * numDsVars) to handle arrays of dataRef, value, and reason.*/
  rptCtrl->maxNumRptVars = MVLU_MAX_RPT_OPTS + (3 * numDsVars);

  rptCtrl->rptNvl.entries = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_GEN, rptCtrl->maxNumRptVars,
					sizeof (MVL_VAR_ASSOC *));

  /* NOTE: The "rptNvl->entries" array is not filled in and
   *       and "rptNvl->num_of_entries" is not set
   *       until a Report is being built.
   */

  /* We need to create a Runtime Type for the inclusion bitstring */
  incRt = &rptCtrl->incRt;
  incRt->el_tag = RT_BIT_STRING;
  incRt->el_size = BSTR_NUMBITS_TO_NUMBYTES(numDsVars);
  incRt->offset_to_last = incRt->el_size;
  incRt->u.p.el_len = numDsVars;
  rc = mvlu_add_rt_type (&rptCtrl->incRt, 1, &rptCtrl->inclusion_typeid);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR0 ("Error - could not add temp RT type");
    return (SD_FAILURE);
    }

/* That's all we can do ahead of time for the RPT variable associations	*/

/* Set a bit of the data for the BASRCB */

  /* Initialize the "DatSet" attribute of the RCB.	*/
  mvl61850_objref_create (dsNvl->name, &dsNvl->nvl_scope, rptCtrl->only_client.basrcb.DatSetNa);

  /* Alloc one buf for reasons_data, changed_flags, segmented_inclusion.*/
  /* Free this buf in mvl61850_rpt_dataset_destroy.				*/
  changed_flags_size = BSTR_NUMBITS_TO_NUMBYTES(numDsVars);
  allocSize = (numDsVars * sizeof (ST_UINT8)) +	  /* reasons_data     	*/
	(2 * changed_flags_size);	/* changed_flags & segmented_inclusion*/
  rptCtrl->only_client.reasons_data = chk_calloc (1, allocSize);
  rptCtrl->only_client.changed_flags = rptCtrl->only_client.reasons_data + numDsVars;
  rptCtrl->only_client.segmented_inclusion = rptCtrl->only_client.changed_flags + changed_flags_size;

  rptCtrl->dsNvl = dsNvl;	/* now save new dataset	*/

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvl61850_rpt_dataset_destroy			*/
/* Destroy resources created by mvl61850_rpt_dataset_create.		*/
/************************************************************************/
ST_VOID mvl61850_rpt_dataset_destroy (MVLU_RPT_CTRL *rptCtrl)
  {
  if (rptCtrl->dsNvl != NULL)
    {		/* if dsNvl is already NULL, do nothing	*/
    chk_free (rptCtrl->only_client.reasons_data);
    rptCtrl->only_client.basrcb.DatSetNa[0] = '\0';	/* clear DatSet	*/

    rptCtrl->maxNumRptVars = 0;

    M_FREE (MSMEM_GEN, rptCtrl->rptNvl.entries);

    /* Free the Runtime Type for the inclusion bitstring */
    mvlu_free_rt_type (rptCtrl->inclusion_typeid);
    rptCtrl->dsNvl = NULL;
    }
  return;
  }

/************************************************************************/
/*                       mvl61850_create_rpt_ctrl			*/
/* Create IEC 61850 Report Control Block.				*/
/* Similar to UCA function "mvlu_create_rpt_ctrl", but just for 61850.	*/
/* Only one client per RCB (see "only_client" param).			*/
/* The "dsNvl" arg may be NULL. If so, the RCB may not be enabled until	*/
/* it is set.								*/
/* NOTE: Does not support UCA Reports (see assert checking "rcb_type").	*/
/************************************************************************/
MVLU_RPT_CTRL *mvl61850_create_rpt_ctrl (ST_CHAR *basrcbName,
			MVL_NVLIST_CTRL *dsNvl,	/* may be NULL		*/
			MVL_VAR_ASSOC *base_va,
			ST_INT rcb_type,
			ST_INT buftim_action,	/* MVLU_RPT_BUFTIM_*	*/
			ST_INT brcb_bufsize,	/* for BRCB only	*/
			ST_UINT32 ConfRev)	/* for BRCB/URCB only	*/

  {
MVLU_RPT_CTRL *rptCtrl;
size_t rptCtrlSize;
ST_INT numRt;
ST_RET status;	/* SD_SUCCESS if everything worked	*/

  assert (rcb_type != RCB_TYPE_UCA);	/* DO NOT use this funct for UCA rpts*/

  /* If any rpt_ctrl created, disconnect function ptr must be set.	*/
  _mvlu_rpt_disconnect_rcvd_fun = _mvlu_rpt_disconnect_rcvd;

  /* Allocate MVLU_RPT_CTRL struct. Allow extra space after struct to	*/
  /* store basrcbName.							*/
  rptCtrlSize = sizeof (MVLU_RPT_CTRL) + strlen (basrcbName) + 1;
  rptCtrl = (MVLU_RPT_CTRL *) M_CALLOC (MSMEM_GEN, 1, rptCtrlSize);

  rptCtrl->basrcb_name = (ST_CHAR *) (rptCtrl + 1);	/*point after struct*/
  strcpy (rptCtrl->basrcb_name, basrcbName);

  /* Initialize structure members.	*/
  rptCtrl->base_va = base_va;
  rptCtrl->rcb_type = rcb_type;
  rptCtrl->buftim_action = buftim_action;

  rptCtrl->brcbCtrl.brcb_bufsize = brcb_bufsize;	/* ONLY for IEC BRCB*/

  rptCtrl->rptNvl.name = "RPT";	/* Report NVL name ALWAYS = "RPT"*/

  rptCtrl->only_client.basrcb.ConfRev = ConfRev;
  rptCtrl->only_client.basrcb.OptFlds.len_1 = 10;	/* bitstring len*/
  rptCtrl->only_client.basrcb.TrgOps.len = 6;		/* bitstring len*/

  rptCtrl->only_client.rpt_ctrl = rptCtrl;	/* client struct needs ptr back to rptCtrl*/

  /* For BRCB, set the initial "EntryID" value.	*/
  /* Put current time in first 4 bytes, 0 in last 4 bytes.	*/
  if (rcb_type == RCB_TYPE_IEC_BRCB)
    {
    ST_INT32 tmp[2];
    tmp [0] = (ST_INT32) time (NULL);
    tmp [1] = 0;
    mvl61850_brcb_entryid_init (rptCtrl, (ST_UINT8 *) tmp);
    }

  /* The following initializations could fail, so we set & check "status".*/

  /* Find the runtime type of the RCB and save it in rptCtrl->rcbRtHead.*/
  /* Used by leaf functions to find the correct rptCtrl.		*/
  /* If successful, this function sets "rptCtrl->rcbRtHead" & "numRt".	*/
  status = mvlu_rpt_rcb_type_find (base_va->type_id, basrcbName, &rptCtrl->rcbRtHead, &numRt);

  if (status == SD_SUCCESS)
    {
    if (mvlu_rpt_find_typeids (&rptCtrl->rpt_typeids))
      {
      MVL_LOG_ERR0 ("Cannot find Report Control Block types");
      status = SD_FAILURE;
      }
    }

  /* mvl61850_rpt_dataset_create initializes everything related to dsNvl.	*/
  if (status == SD_SUCCESS)
    status = mvl61850_rpt_dataset_create (rptCtrl, dsNvl);

  if (status == SD_SUCCESS)
    {
#if 1	/* IEC 61850-7-2 Ed2 says to start buffering on power-up if	*/
	/* DatSet attribute references an existing DataSet.		*/
    if (dsNvl != NULL && rptCtrl->rcb_type == RCB_TYPE_IEC_BRCB)
      rptCtrl->brcbCtrl.enabled_once = SD_TRUE;	/* starts buffering	*/
#endif
    list_add_last((ST_VOID **) &mvl61850_rpt_ctrl_list, (ST_VOID *) rptCtrl);
    }
  else
    {
    M_FREE (MSMEM_GEN, rptCtrl);	/* clean up	*/
    rptCtrl = NULL;			/* set error return value	*/
    }
  return (rptCtrl);
  }

/************************************************************************/
/*                       mvl61850_free_rpt_ctrl				*/
/* Free a MVLU_RPT_CTRL allocated by 'mvl61850_create_rpt_ctrl'.	*/
/************************************************************************/

ST_VOID mvl61850_free_rpt_ctrl (MVLU_RPT_CTRL *rptCtrl)
  {
  assert (rptCtrl->rcb_type != RCB_TYPE_UCA);

/* Take it off the report control list */
  list_unlink ((ST_VOID **) &mvl61850_rpt_ctrl_list, (ST_VOID *) rptCtrl);

  mvl61850_rpt_dataset_destroy (rptCtrl);

/* BRCB may contain large lists of reports sent or queued.	*/
  if (rptCtrl->rcb_type == RCB_TYPE_IEC_BRCB)
    mvl61850_brcb_rpt_lists_clean (&rptCtrl->brcbCtrl);

  M_FREE (MSMEM_GEN, rptCtrl);
  }

/************************************************************************/
/*			mvl61850_brcb_client_service			*/
/* Check the BRCB state and perform appropriate actions.		*/
/************************************************************************/
ST_VOID mvl61850_brcb_client_service (MVLU_RPT_CTRL *rptCtrl,
			MVLU_RPT_CLIENT *rptClient,
			ST_DOUBLE timeNow)
  {
BUFLIST_ENTRY *entry;
ST_BOOLEAN all_seg_done;	/* SD_TRUE if done sending all segments	*/
ST_RET retCode;

  /* Check "ResvTmsExpireTime". It is set to non-zero value only when the*/
  /* reserved client disconnects. If it is non-zero, check for timeout	*/
  /* or check to see if the reserved client has reconnected.		*/
  /* NOTE: ResvTms is only supported in Edition 2. For Edition 1,	*/
  /*       ResvTmsExpireTime is always 0, so this code never executes.	*/
  if (rptClient->ResvTmsExpireTime != 0)	/* disconnected & Resv timing out*/
    {
    /* NOTE: Owner should always be set at this point.			*/
    if (timeNow > rptClient->ResvTmsExpireTime)
      {
      /* BRCB reservation expired.	*/
      rptClient->basrcb.ResvTms = 0;		/* unreserve BRCB	*/
      rptClient->ResvTmsExpireTime = 0;		/* stop timer		*/
      *(ST_UINT32 *) rptClient->basrcb.Owner = 0;	/* clear "Owner"*/
      }
    else
      {
      /* While in this state, look for reconnect by the "reserved" client.*/
      /* Must loop through all active connections.	*/
      ST_INT j;
      for (j = 0; j < mvl_cfg_info->num_called; ++j)
        {
        MVL_NET_INFO *net_info = &mvl_called_conn_ctrl[j];
        if (net_info->conn_active &&
            net_info->ass_ind_info.calling_paddr.netAddr.ip == *(ST_UINT32 *) rptClient->basrcb.Owner)
          {
          /* The BRCB "Owner" has reconnected.				*/
          /* Re-reserve BRCB by just stopping timer.			*/
          rptClient->ResvTmsExpireTime = 0;	/* stop timer	*/

          /* Just in case this client has multiple connections, we DO NOT save netInfo now.*/
          /* Instead, we wait for client to write to BRCB (see _rcb_writable).*/
          /* This way first conn from correct IP to write to BRCB after	*/
          /* disconnect gets control.					*/

          break;	/* stop on first match	*/
          }
        }
      }
    }

  /* Check BRBC state and, if necessary, encode AND queue up report.	*/
  mvl61850_rcb_chk_state (rptCtrl, rptClient, timeNow);

  /* Now, examine BRCB state and send report if necessary.
   * If connected, reports are enabled, reports are queued, AND stack buffers
   * are available, send a report.
   */
  if (rptClient->netInfo  &&			/* connected	*/
      rptClient->basrcb.RptEna  &&		/* rpts enabled	*/
      rptCtrl->brcbCtrl.rpt_list_next)		/* rpts queued & not all sent*/
    {
    if (mvl_ureq_bufs_avail (rptClient->netInfo) > 0)	/* stack bufs avail to send rpt	*/
      {
      /* Encode and send next buffered report.	*/
      entry = rptCtrl->brcbCtrl.rpt_list_next;	/* this is next one to send*/
      retCode = mvl61850_brcb_rpt_send (rptCtrl, rptClient, entry, &all_seg_done);
      if (retCode != SD_SUCCESS)
        MVL_LOG_ERR1 ("Sending IEC-61850 Buffered Report failed: err=0x%X", retCode);

      /* If error OR done sending all segments of this rpt,		*/
      /* point to next rpt on list (NULL if all sent) & update SqNum	*/
      if (retCode != SD_SUCCESS ||
          all_seg_done)	/* done sending all segments of this report.	*/
        {	/* point to next rpt on the list (NULL if all sent)	*/
        rptCtrl->brcbCtrl.rpt_list_next = list_get_next (rptCtrl->brcbCtrl.rpt_list, entry);
        memcpy (rptCtrl->brcbCtrl.lastSentEntryID, entry->EntryID, 8);	/* save last sent EntryID*/
        memcpy (&rptCtrl->brcbCtrl.lastSentTimeOfEntry, &entry->TimeOfEntry, sizeof (MMS_BTIME6));
        print_queues (&rptCtrl->brcbCtrl);		/* just for debug	*/
        /* Increment seq number for next rpt.	*/
        rptClient->basrcb.SqNumInt16u++;	/* used only for BRCB	*/
        /* Only one report should be sent with BufOvfl TRUE; clear it now.*/
        rptCtrl->brcbCtrl.BufOvfl = SD_FALSE;
        }
      }
    else
      print_msg0 ("STACK BUFFERS NOT AVAILABLE TO SEND REPORT\n");
    }
  return;
  }

/************************************************************************/
/*			mvl61850_urcb_client_service			*/
/* Check the URCB state and perform appropriate actions.		*/
/************************************************************************/
ST_VOID mvl61850_urcb_client_service (MVLU_RPT_CTRL *rptCtrl,
			MVLU_RPT_CLIENT *rptClient,
			ST_DOUBLE timeNow)
  {
  /* Must be connected to send Unbuffered reports.	*/
  if (rptClient->netInfo!=NULL)
    {
    /* If no buffers are available to send unbuffered reports, don't waste time
     * checking timers, scanning, etc.
     */
    if (mvl_ureq_bufs_avail (rptClient->netInfo) <= 0)	/* stack bufs avail to send rpt	*/
      print_msg0 ("STACK BUFFERS NOT AVAILABLE TO SEND REPORT\n");
    else
      /* Check URBC state and, if necessary, encode AND send report.	*/
      mvl61850_rcb_chk_state (rptCtrl, rptClient, timeNow);
    }
  return;
  }

/************************************************************************/
/*			mvl61850_rpt_service 				*/
/* Perform servicing for all IEC 61850 Report Control Blocks.		*/
/* Scanning for changes (dchg, qchg, etc) is automatic		  	*/
/* (see mvl61850_rpt_change_scan).					*/
/************************************************************************/

ST_VOID mvl61850_rpt_service (/*ST_VOID*/ST_BOOLEAN bScanForce)
  {
MVLU_RPT_CTRL *rptCtrl;
ST_DOUBLE timeNow;

/* Scan all report data for changes (dchg, qchg, dupd).	*/
  mvl61850_rpt_change_scan (bScanForce);

/* See if it is time to send reports	*/
  timeNow = sGetMsTime ();
  for (rptCtrl = mvl61850_rpt_ctrl_list;
       rptCtrl != NULL;
       rptCtrl = (MVLU_RPT_CTRL *) list_get_next (mvl61850_rpt_ctrl_list, rptCtrl))
    {
    /* NOTE: if DatSet is NULL, don't need to do anything for this rptCtrl.*/
    if (rptCtrl->dsNvl==NULL)
      continue;
    /* Only one client (rptCtrl->only_client).	*/
    if (rptCtrl->rcb_type == RCB_TYPE_IEC_BRCB)
      mvl61850_brcb_client_service (rptCtrl, &rptCtrl->only_client, timeNow);
    else
      {
      assert (rptCtrl->rcb_type == RCB_TYPE_IEC_URCB);	/* must be IEC_URCB*/
      mvl61850_urcb_client_service (rptCtrl, &rptCtrl->only_client, timeNow);
      }
    }
  }

/************************************************************************/
/*			mvl61850_rpt_ctrl_destroy_all			*/
/* Destroy all 61850 report controls.					*/
/************************************************************************/
ST_VOID mvl61850_rpt_ctrl_destroy_all ()
  {
MVLU_RPT_CTRL *rptCtrl;
  /* Use list_find_last to avoid calling list_unlink twice.	*/
  while ((rptCtrl = (MVLU_RPT_CTRL *) list_find_last ((DBL_LNK *)mvl61850_rpt_ctrl_list)) != NULL)
    {
    mvl61850_free_rpt_ctrl (rptCtrl);	/* this also unlinks it from list*/
    }
  }

/************************************************************************/
/*			mvl61850_integrity_timeout			*/
/* NOTE: For IEC 61850 only. For UCA, use mvlu_integrity_timeout.	*/
/* If Integrity period timeout occurred, send Integrity Report.		*/
/* NOTE: data scan already done at start of mvl61850_rpt_service.	*/
/*       No need to rescan data. Just send the data from the data scan.	*/
/* RETURNS: SD_TRUE if IntgPd is set and timeout occurred		*/
/*          SD_FALSE otherwise						*/
/************************************************************************/
ST_RET mvl61850_integrity_timeout (MVLU_RPT_CLIENT *rptClient, ST_DOUBLE timeNow)
  {
ST_RET retVal;
ST_INT j;
ST_RET readyRet;	/* return code from mvlu_rpt_ready	*/

  if (rptClient->basrcb.IntgPd != 0
      && BSTR_BIT_GET (rptClient->basrcb.TrgOps.data, TRGOPS_BITNUM_INTEGRITY)	/* enabled*/
      && timeNow > rptClient->next_integ_rpt_time)	/* timeout occurred*/
    {
    MVLU_LOG_FLOW1 ("IEC 61850 Report Integrity Period timeout for client " S_FMT_PTR,
							rptClient);

    /* Send last buffered entry (due to BufTm buffering) BEFORE this integrity report.*/
    if (rptClient->numTrgs > 0)
      {		/* send or queue report	*/
      mvlu_rpt_ready (rptClient, MVLU_RPT_TYPE_RBE);
      }

    /* Set "reason" for each variable of Dataset now.	*/
    for (j = 0; j < rptClient->rpt_ctrl->dsNvl->num_of_entries; j++)
      rptClient->reasons_data[j] = MVLU_TRGOPS_INTEGRITY;

    /* send or queue rpt	*/
    readyRet = mvlu_rpt_ready (rptClient, MVLU_RPT_TYPE_INTEGRITY_OR_GI);

    if (readyRet != SD_SUCCESS)
      MVLU_LOG_FLOW1 ("IEC 61850 Integrity report could not be sent or queued: err=0x%X", readyRet);

    /* set time for next integrity report	*/
    rptClient->next_integ_rpt_time = sGetMsTime ()
                     + (ST_DOUBLE) rptClient->basrcb.IntgPd;
    retVal = SD_TRUE;
    }
  else
    retVal = SD_FALSE;
  return (retVal);
  }


