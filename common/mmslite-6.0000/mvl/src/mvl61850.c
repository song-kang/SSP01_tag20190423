/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2002-2006 All Rights Reserved					*/
/*									*/
/* MODULE NAME : mvl61850.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Code to support IEC-61850 Reporting as a Server.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvl61850_rcb_chk_state						*/
/*	mvl61850_urcb_rpt_send						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 03/19/12  JRB	   Fix Klocwork warning: uninitialized local.	*/
/* 01/11/12  JRB	   Add leaf functs for ResvTms, Owner for Ed 2.	*/
/*			   Add mvl61850_objref_to_objname.		*/
/*			   Del mvl61850_mk_dataref, use mvl61850_objref_create*/
/*			   Chk return from mvl61850_get_rcb.		*/
/* 03/08/11  JRB	   mvl61850_objref_create: del '/' from AA-Spec.*/
/* 12/08/10  JRB	   Move mvl61850_beh_stval_rd_ind to user file.	*/
/* 09/21/10  JRB	   Return ..VALUE_INVALID if EntryID not found.	*/
/*			   Return ..VALUE_INVALID if DatSet not found.	*/
/* 01/12/10  JRB     32    Fix log messages.				*/
/* 08/05/08  JRB     31    Increment ConfRev if DatSet is written.	*/
/* 07/22/08  JRB     30    Add mvl61850_objref_create.			*/
/* 06/30/08  JRB     29    For Ed2, return different EntryID/TimeOfEntry*/
/*			   values if RptEna=FALSE or TRUE.		*/
/* 05/15/08  JRB     28    Use NEW mvl61850_integrity_timeout.		*/
/* 05/06/08  JRB     27    Fix memory leak after "PDU size" error.	*/
/* 04/15/08  JRB     26	   Don't call mvlu_rpt_va_change for GI		*/
/*			   (save "reason" later when scan completes).	*/
/* 02/29/08  JRB     25    Allow VMD_SPEC DatSet for URCB or BRCB.	*/
/*			   Allow AA_SPEC DatSet for URCB, but ONLY if	*/
/*			   RCB is reserved (Resv=TRUE).			*/
/*			   Set changed_flags for Integrity BEFORE loop.	*/
/*			   chk_seg_needed: allow longer (129) RptID, etc*/
/* 03/16/07  JRB     24    mvlu_gi_wr_ind: Chk that this client reserved URCB.*/
/* 03/07/07  JRB     23    Use mvlu_get_leaf_val_int_any for Beh$stVal.	*/
/* 02/28/07  JRB     22    mvlu_gi_wr_ind: allow write only if RCB is	*/
/*			   enabled; if TrgOps.GI=FALSE, allow write but	*/
/*			   DO NOT generate a report.			*/
/* 02/13/07  JRB     21    mvl61850_get_rcb: del unused net_info arg.	*/
/*			   Use new _rcb_writable.			*/
/* 12/20/06  JRB     20    PurgeBuf only if DatSet val changed (tissue 322)*/
/*			   Allow Beh$stVal to be INT8 (Enum) or INT32	*/
/*			   (for Tissues 120, 146, 171, 234).		*/
/* 10/30/06  JRB     19    Use new mvl_vmd_* object handling functions.	*/
/* 08/09/06  JRB     18    Moved most 61850 Rpt code to mvl61850_rpt.c	*/
/*			   Chg mvl61850_rcb_build to mvl61850_urcb_rpt_send*/
/*			   & use only for URCB (new functs handle BRCB).*/
/*			   Use new mvlu_rpt_ready, mvl61850_mk_dataref,	*/
/*			    mvl61850_rcb_cleanup.			*/
/*			   Use new mvl61850_mk_rptid to generate RptID	*/
/*			    in RPT if RptID is NULL in RCB.		*/
/*			   Allow GI write only if TrgOp GI bit=1.	*/
/*			   Del mvl_enc_va_data, use new ms_local_to_asn1_2.*/
/*			   Add mvl61850_datset_wr_ind to allow DatSet write.*/
/*			   Store EntryID everywhere as Ostring8.	*/
/*			   Fix type for all ObjectReference (Vstring129).*/
/*			   Use mvl61850_get_rcb to find the RCB.	*/
/* 07/11/05  JRB     17    BUG FIX: Set cur_bufsize=0 after PurgeBuf write.*/
/*			   Set BufOvfl=SD_FALSE after PurgeBuf write	*/
/*			   (not clear in 61850, but passes KEMA test).	*/
/* 07/08/05  JRB     16    For URCB, ignore BUFOVFL, ENTRYID bits in	*/
/*			   OptFlds to conform to 61850-8-1.		*/
/* 06/27/05  JRB     15    mvlu_entryid_rd_ind: return lastSentEntryID.	*/
/*			   Old code returned "last queued" EntryID.	*/
/* 06/27/05  JRB     14    _mvlu_get_rd_rcb: add (MVLU_RPT_CLIENT **) arg*/
/* 05/16/05  JRB     13    Set failure=ARE_TEMP_UNAVAIL when trying to	*/
/*			   write RCB elements while RptEna=TRUE.	*/
/*			   Del mvlu_sqnum_int16u_wr.. (not writable).	*/
/* 05/04/05  JRB     12    Call u_mvlu_rpt_time_get just once BEFORE	*/
/*			   main loop in ..rcb_build so that each	*/
/*			   segmented report has the same time stamp.	*/
/* 01/19/05  JRB     11    Add mvl61850_beh_stval_rd_ind.		*/
/* 08/20/04  JRB     10    Increment EntryID before encoding rpt,	*/
/*			   so EntryID encoded in rpt matches EntryID	*/
/*			   stored in BUFLIST_ENTRY struct.		*/
/* 08/06/04  EJV     09    mvlu_entryid_wr_ind: add typecast.		*/
/* 06/30/04  JRB     08    Add mvlu_sqnum_int16u_rd(wr)_ind_fun & use	*/
/*			   SqNumInt16u instead of SqNum for BRCB.	*/
/*			   Chg RptID type to vstring65.			*/
/*			   Del SCL parsing functions:			*/
/*			    mvl61850_ln_create_start, mvl61850_do_create,*/
/*			    mvl61850_da_create, mvl61850_ln_create_finish,*/
/*			    & mvl61850_ln_destroy. New SCL standard	*/
/*			    makes these obsolete.			*/
/* 03/18/04  JRB     07    Fix dataref in reports by adding scope.	*/
/* 12/17/03  JRB     06    61850-8-1 FDIS changes:			*/
/*			   Add ConfRev to rpt if enabled by OptFlds.	*/
/*			   Move SubSeqNum, MoreSegmentsFollow to just	*/
/*			   before inclusion bitstring.			*/
/*			   Save TimeofEntry in "basrcb".		*/
/*			   Add mvlu_timeofentry_rd_ind.			*/
/* 07/09/03  JRB     05    Del assert in mvlu_gi_scan_done so GI works	*/
/*			   for URCB too.				*/
/* 04/14/03  JRB     04    Eliminate compiler warnings.			*/
/* 04/04/03  JRB     03    Fix integrity/GI scan code so multiple	*/
/*			   concurrent scans don't corrupt one another.	*/
/* 03/13/03  JRB     02    Add URCB support: rename/rearrange some things*/
/*			   Add code for 61850 dynamic type creation.	*/
/*			   Trigger Intg Rpt ONLY by setting IntgPd!=0.	*/
/*			   Trigger GI ONLY by writing GI!=0.		*/
/*			   Move bstrcpy, bvstrcpy to "asn1r.c".		*/
/*			   Fix report segmenting (see chk_seg_needed).	*/
/* 12/15/02  JRB     01    Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_uca.h"
#include "mvl_log.h"
#include "mvl_acse.h"	/* need mvl_cfg_info	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_RET fill_asn1_len_array (MVLU_RPT_CLIENT *rptClient,
	MVL_NVLIST_CTRL *dsNvl,
	ST_INT *asn1_len_array);

/******
 *			mvl61850_rcb_chk_state
 * Check RCB state and build/send report if necessary. This function works for
 * BRCB or URCB.
 */
ST_VOID mvl61850_rcb_chk_state (MVLU_RPT_CTRL *rptCtrl,
			MVLU_RPT_CLIENT *rptClient,
			ST_DOUBLE timeNow)
  {
  /* If URCB enabled or BRCB enabled ONCE, chk Integrity period & triggers.*/
  if (rptClient->basrcb.RptEna
      || (rptClient->rpt_ctrl->rcb_type == RCB_TYPE_IEC_BRCB
          && rptClient->rpt_ctrl->brcbCtrl.enabled_once))
    {
    /* Chk for Integrity Period timeout.	*/
    /* NOTE: use mvl61850_integrity_timeout (mvlu_integrity_timeout is	*/
    /*       only for UCA).						*/
    mvl61850_integrity_timeout (rptClient, timeNow);

    /* Chk for other triggers.	*/
    /* NOTE: numTrgs ONLY incremented if changes matched the TrgOps.	*/
    if (rptClient->numTrgs > 0)
      {
      /* If BufTim disabled (=0) OR BufTim expired, send report now.	*/
      if (rptClient->basrcb.BufTim == 0 || timeNow > rptClient->buf_time_done)
        {
        MVLU_LOG_FLOW1 ("Non-Integrity report for client " S_FMT_PTR, rptClient);
        mvlu_rpt_ready (rptClient, MVLU_RPT_TYPE_RBE);	/* send or queue rpt*/
        }
      }
    }	/* end if rpt enabled at least once	*/
  return;
  }

/************************************************************************/
/*			mvlu_gi_scan_va_done				*/
/* This function is called by 'mvlu_rd_prim_done' when all "leaf"	*/
/* functions for a VA have been completed (i.e. data ready for this VA).*/
/************************************************************************/

ST_VOID mvlu_gi_scan_va_done (MVL_IND_PEND *indCtrl,
					MVL_VAR_ASSOC *va)
  {
  ST_INT data_size;
  /* CRITICAL: DO NOT call mvlu_rpt_va_change. rptClient->reasons_data	*/
  /*     will be set in mvlu_gi_scan_done when scan completes.		*/
  data_size = mvl_type_ctrl[va->type_id].data_size;
  memcpy (va->last_data, va->data, data_size);
  }

/************************************************************************/
/*			mvlu_gi_scan_done				*/
/* Same as "_mvlu_rpt_scan_done" except arg is (MVL_IND_PEND *)		*/
/* instead of (MVLU_RPT_SCAN_CTRL *).					*/
/*									*/
/* This function is called when the gi scan is complete.		*/
/* (i.e. 'mvlu_rd_prim_done' has been called for all "leafs" of all	*/
/* variables in the report). Everything is ready to build a report,	*/
/* so build it now.							*/
/************************************************************************/

ST_RET mvlu_gi_scan_done (MVL_IND_PEND *indCtrl)
  {
ST_RET retCode;
MVLU_RPT_CLIENT *rptClient;
ST_INT j;

  /* Get "rptClient", saved in "indCtrl" when scan initialized.	*/
  rptClient = (MVLU_RPT_CLIENT *) indCtrl->usr_ind_ctrl;

  /* Set "reason" for each variable of Dataset now.	*/
  for (j = 0; j < rptClient->rpt_ctrl->dsNvl->num_of_entries; j++)
    rptClient->reasons_data[j] = MVLU_TRGOPS_GI;

  /* Assume this is a BRCB or URCB. Should never get here otherwise.	*/

  /* Build the report.	*/
  retCode = mvlu_rpt_ready (rptClient, MVLU_RPT_TYPE_INTEGRITY_OR_GI);

  if (retCode != SD_SUCCESS)
    MVLU_LOG_FLOW1 ("GI report send failed: err=0x%X", retCode);

  mvlu_integrity_scan_destroy (indCtrl);	/* destroy temporary struct*/
  return (retCode);
  }

/************************************************************************/
/*			mvl61850_urcb_rpt_send				*/
/* Set up all info needed to build an Information Report for a URCB,	*/
/* then call "mvl_info_variables" to encode/send it.			*/
/************************************************************************/
ST_RET mvl61850_urcb_rpt_send (MVLU_RPT_CTRL *rptCtrl,
			MVLU_RPT_CLIENT *rptClient,
			ST_INT rpt_type)
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
ST_INT cur_va_index, next_va_index;	/* indices into dsNvl va array	*/
ST_BOOLEAN segNeeded;		/* If TRUE, segmenting needed	*/
ST_UINT16 SubSeqNum = 0;
ST_BOOLEAN MoreSegmentsFollow;
ST_CHAR *tmp_dataref_buf;
ST_CHAR tmpRptID [MVL61850_MAX_RPTID_LEN+1];
ST_INT *asn1_len_array;	/* array of ASN.1 lengths, one for each var (allocated)*/

  retCode = SD_FAILURE; 	/* assume error. This changes if rpt sent.*/
  basrcb = &rptClient->basrcb;

  optFlds = basrcb->OptFlds.data_1;
  dsNvl = rptCtrl->dsNvl;
  rptNvl = &rptCtrl->rptNvl;

  incSize = BSTR_NUMBITS_TO_NUMBYTES(dsNvl->num_of_entries);

  MVLU_LOG_FLOW1 ("Building IEC-61850 URCB Report, MVL_NET_INFO " S_FMT_PTR, rptClient->netInfo);

  /* Need tmp va's for options, array of data_refs, array of reasons_data.*/
  tmp_va_arr_size = MVLU_MAX_RPT_OPTS + (dsNvl->num_of_entries * 2);
  tmp_va_arr = M_CALLOC (MSMEM_GEN, sizeof (MVL_VAR_ASSOC), tmp_va_arr_size);

  /* Need tmp buffer for datarefs. One buffer for all.		*/
  /* Allow max len plus NULL for each dataref.			*/
  tmp_dataref_buf = M_CALLOC (MSMEM_GEN, (MVL61850_MAX_OBJREF_LEN+1), dsNvl->num_of_entries);

  asn1_len_array = M_CALLOC (MSMEM_GEN, rptCtrl->dsNvl->num_of_entries, sizeof(ST_INT));

  assert (rptCtrl->dsNvl->num_of_entries);	/* must be >0 entries	*/

  /* Call user function to get report time. Called here before loop	*/
  /* so that all segmented reports get the same time stamp.		*/
  u_mvlu_rpt_time_get (&basrcb->TimeofEntry);

  /* Fill in "asn1_len_array", needed later by "chk_seg_needed".	*/
  if (fill_asn1_len_array (rptClient, rptCtrl->dsNvl, asn1_len_array)!=SD_SUCCESS)
    {
    /* NOTE: This should NEVER fail, but if it does, just log it.	*/
    /*       Real encode will almost certainly fail later.		*/
    MVL_LOG_ERR0 ("Cannot calculate ASN.1 length for Unbuffered Report.");
    }

  /* If this is Integrity (or GI) report, just set all inclusion bits.*/
  if (rpt_type == MVLU_RPT_TYPE_INTEGRITY_OR_GI)
    memset (rptClient->changed_flags, 0xff, incSize);

  for (cur_va_index = 0, next_va_index = 0;
       next_va_index < rptCtrl->dsNvl->num_of_entries;
       cur_va_index = next_va_index, SubSeqNum++)
    {	/* BEGIN MAIN LOOP	*/
    tmp_va = tmp_va_arr;	/* start loop pointing to first entry	*/
    sendIndex = 0;		/* start with first rptNvl->entries.	*/

    /* Prepares a UCA Report NVL to be sent, based on the		*/
    /* options, data, and inclusion bitstring in the rptCtrl. After this*/
    /* function completes, the rptCtrl->rptNvl is ready to be sent.	*/

    /* CRITICAL: asn1_len_array must be filled in before call to chk_seg_needed.*/
    segNeeded = chk_seg_needed (rptClient, asn1_len_array, cur_va_index, &next_va_index);

    if (next_va_index - cur_va_index == 0)
      {
      MVL_LOG_ERR0 ("MMS PDU size too small to fit ANY variables in IEC-61850 Report. Report not sent.");
      retCode = SD_FAILURE;
      break;	/* break out of loop so normal cleanup occurs.	*/
      }

    /* We will create a NVL to send, using the dsNvl as a model ...	*/

    /* RptID and OptFlds are always the first 2 entries in the Rpt NVL.	*/

    /* Add RptID to Report.						*/
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

    /* Add OptFlds to Report.						*/
    tmp_va->type_id = rptCtrl->rpt_typeids.bvstring10;
    tmp_va->data = &basrcb->OptFlds;
    rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
    MVLU_LOG_CFLOW2 ("  OptFld = 0x%02x 0x%02x", basrcb->OptFlds.data_1[0],
                     basrcb->OptFlds.data_1[1]);	/* 10 bit bstr (2 bytes)	*/

    /* Add optional RCB vars to NVL, depending on OptFlds.	*/

    /* The following optional vars are controlled by the same OptFlds bits,
     * are of the same type, and are in the same order as in UCA, so that
     * UCA clients can decode reports if only these options are used.
     */

    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_SQNUM))
      {
      /* NOTE: 61850-7-2 says SqNum in Report is INT16U, but we encode
       *       INT8U from URCB. ASN.1 encodes same either way, so client
       *       may decode as INT16U if needed.
       */
      tmp_va->type_id = rptCtrl->rpt_typeids.int8u;
      tmp_va->data = &basrcb->SqNum;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      MVLU_LOG_CFLOW1 ("    SqNum : %u", (unsigned) basrcb->SqNum);
      }
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_TIMESTAMP))
      {
      MVLU_LOG_CFLOW2 ("    RptTim : %ld ms, %ld days",
              (ST_LONG)basrcb->TimeofEntry.ms, (ST_LONG)basrcb->TimeofEntry.day);

      tmp_va->type_id = rptCtrl->rpt_typeids.btime6;
      tmp_va->data = &basrcb->TimeofEntry;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      }
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_DATSETNAME))
      {
      /* UCA called it OutDat instead of DataSetName. Value is same.	*/
      MVLU_LOG_CFLOW1 ("    DataSetName : %s", basrcb->DatSetNa);
      tmp_va->type_id = rptCtrl->rpt_typeids.vstring129; /*ObjectReference*/
      tmp_va->data = basrcb->DatSetNa;
      rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
      }

    /* The following optional vars are NOT supported by UCA. The OptFlds bits
     * must never be set by UCA clients, and the corresponding data
     * must NOT be sent to UCA clients.
     */

    /* For URCB, ignore BUFOVFL bit in OptFlds.	*/
    /* For URCB, ignore ENTRYID bit in OptFlds.	*/

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
      tmp_va->data = &SubSeqNum;
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

    /* Add inclusion bitstring.	*/
    tmp_va->type_id = rptCtrl->inclusion_typeid;
    if (segNeeded)
      {
      /* just set the bits included in this segment.	*/
      memset (rptClient->segmented_inclusion, 0, incSize);	/* start clean	*/
      for (j = cur_va_index; j < next_va_index; ++j)
        {
        if (BSTR_BIT_GET (rptClient->changed_flags, j))
          BSTR_BIT_SET_ON (rptClient->segmented_inclusion, j);
        }
      tmp_va->data = rptClient->segmented_inclusion;
      }
    else
      tmp_va->data = rptClient->changed_flags;
    rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/

    /* If data-Ref enabled, go through inclusion_data to decide what to send*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_DATAREF))
      {
      for (j = cur_va_index; j < next_va_index; ++j)
        {
        if (BSTR_BIT_GET (rptClient->changed_flags, j))
          {
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
    for (j = cur_va_index; j < next_va_index; ++j)
      {
      if (BSTR_BIT_GET (rptClient->changed_flags, j))
        {
        va = dsNvl->entries[j];
        MVLU_LOG_CFLOW3 ("  Including variable %d ('%s'), reason=0x%02X",
                        j, va->name, rptClient->reasons_data[j]);
        rptNvl->entries[sendIndex++] = va;
        /* va->data already points to latest data. No need to copy data.*/
        }
      }
    assert (sendIndex>sendIndexSave);	/* make sure SOME data included	*/

    /* If "reason" enabled, go through inclusion_data to decide what to send*/
    if (BSTR_BIT_GET(optFlds, OPTFLD_BITNUM_REASON))
      {
      for (j = cur_va_index; j < next_va_index; ++j)
        {
        if (BSTR_BIT_GET (rptClient->changed_flags, j))
          {
          /* Don't need BVSTR here, because size is fixed.	*/
          tmp_va->type_id = rptCtrl->rpt_typeids.bstr6;
          tmp_va->data = &rptClient->reasons_data[j];
          rptNvl->entries[sendIndex++] = tmp_va++;	/* set entry & point to next*/
          }
        }
      }
    rptNvl->num_of_entries = sendIndex;
    assert (rptNvl->num_of_entries <= rptCtrl->maxNumRptVars);	/* past end of array?*/
    assert ((tmp_va-tmp_va_arr) <= tmp_va_arr_size);	/* past end of array?*/

    /* Encode the InformationReport and send it.	*/
    retCode = mvl_info_variables (rptClient->netInfo,	/* URCB: encode & send*/
                        &rptClient->rpt_ctrl->rptNvl,
                        SD_FALSE);	/* FALSE means "send as NVL"	*/
    /* If send fails, just log it. Try to send other segments anyway.*/
    if (retCode)
      MVL_LOG_ERR1 ("Sending IEC-61850 Unbuffered Report failed: err=0x%X", retCode);
    }	/* END MAIN LOOP	*/

  /* Increment seq number for next rpt.	*/
  basrcb->SqNum++;		/* used only for URCB	*/

  /* Reset reasons, etc. to prepare for new report triggers.	*/
  mvl61850_rcb_cleanup (rptClient);	/* prepare for next RPT.	*/
  if (retCode != SD_SUCCESS)
    MVLU_LOG_FLOW1 ("Report Build failed: err=0x%X", retCode);
  M_FREE (MSMEM_GEN, tmp_va_arr);
  M_FREE (MSMEM_GEN, tmp_dataref_buf);
  M_FREE (MSMEM_GEN, asn1_len_array);
  return (retCode);
  }

/******
 *			chk_seg_needed
 * Check if segmenting is needed.
 * Set (*next_va_index) = first va to send in "next" report.
 * RETURN: SD_FALSE if segmenting NOT needed.
 *         SD_TRUE if segmenting IS needed.
 */
ST_BOOLEAN chk_seg_needed (MVLU_RPT_CLIENT *rptClient,
			ST_INT *asn1_len_array,
			ST_INT cur_va_index,	/* input arg	*/
			ST_INT *next_va_index)	/* output arg	*/
  {
ST_INT negotiatedPduSize;
ST_INT unusedSize;
ST_INT num_entries;
ST_INT loopIndex;
ST_BOOLEAN segNeeded;

  negotiatedPduSize = rptClient->netInfo->max_pdu_size;

  num_entries = rptClient->rpt_ctrl->dsNvl->num_of_entries;

  /* Subtract overhead from max "negotiated" PDU size.	*/
  /* Very verbose, but most data are constants so compiler should simplify.*/
  unusedSize = negotiatedPduSize
		- 32	/* InfoRpt overhead (ROUGH estimate)	*/
		- 131	/* RptID	(Vstring129)	*/
		- 5	/* OptFlds	(Bstring10)	*/
		- 4	/* SqNum	(Int16)		*/
		- 131	/* DataSetNam	(Vstring129)	*/
		- 3	/* BufOvfl	(Boolean)	*/
		- 8	/* TimeOfEntry	(Btime6)	*/
		- 4	/* SubSeqNum	(Int16)		*/
		- 3	/* MoreSegFoll	(Boolean)	*/
		-10	/* EntryID	(Ostring8)	*/
		-(3+num_entries/8)	/* Inclusion Bitstring	*/
		;

  /* In a loop, check if each var fits in the Report.	*/
  for (loopIndex = cur_va_index; loopIndex < num_entries; ++loopIndex)
    {
    /* Subtract space needed for this var.	*/
    /* If len==0, this var must not be included in the report, so skip it.*/
    if (asn1_len_array[loopIndex])
      {	/* this var should be included in rpt. Check for space.	*/
      unusedSize -= 131;		/* DataRef	(Vstring129)	*/
      unusedSize -= 4;		/* ReasonForInclusion	(Bstring4?)	*/
      unusedSize -= asn1_len_array[loopIndex];	/* Data		*/
      }
    if (unusedSize < 0)
      break;	/* cannot fit this va. Break out of loop.	*/
    }
  /* When loop ends, "loopIndex" is index to first variable of "next" report.*/

  *next_va_index = loopIndex;
  if (cur_va_index == 0 && (*next_va_index) == num_entries)
    segNeeded = SD_FALSE;	/* All fit in one PDU (no segmenting)	*/
  else
    segNeeded = SD_TRUE;
  return (segNeeded);
  }

/************************************************************************/
/*			mvlu_confrev_rd_ind				*/
/* NOTE: confrev write funct should be something like u_no_write_allowed*/
/************************************************************************/
ST_VOID mvlu_confrev_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
ST_UINT32 *dest;
MVLU_BASRCB *rcb;
ST_RET rc;

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluRdVaCtrl->rdVaCtrl->va->base_va,
	    mvluRdVaCtrl->rt,
            NULL);
  if (rcb != NULL)
    {
    dest = (ST_UINT32 *) mvluRdVaCtrl->primData;
    *dest = rcb->ConfRev;
    rc = SD_SUCCESS;
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, rc);
  }

/************************************************************************/
/*			mvlu_purgebuf_rd_ind				*/
/************************************************************************/
ST_VOID mvlu_purgebuf_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
ST_BOOLEAN *dest;
MVLU_BASRCB *rcb;
ST_RET rc;

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluRdVaCtrl->rdVaCtrl->va->base_va,
	    mvluRdVaCtrl->rt,
            NULL);
  if (rcb != NULL)
    {
    dest = (ST_BOOLEAN *) mvluRdVaCtrl->primData;
    *dest = 0;		/* no data stored. ALWAYS return 0	*/
    rc = SD_SUCCESS;
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, rc);
  }

/************************************************************************/
/*			mvlu_gi_rd_ind					*/
/************************************************************************/
ST_VOID mvlu_gi_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
ST_BOOLEAN *dest;
MVLU_BASRCB *rcb;
ST_RET rc;

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluRdVaCtrl->rdVaCtrl->va->base_va,
	    mvluRdVaCtrl->rt,
            NULL);
  if (rcb != NULL)
    {
    dest = (ST_BOOLEAN *) mvluRdVaCtrl->primData;
    *dest = 0;		/* no data stored. ALWAYS return 0	*/
    rc = SD_SUCCESS;
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, rc);
  }


/************************************************************************/
/*			mvlu_entryid_rd_ind				*/
/************************************************************************/
ST_VOID mvlu_entryid_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVLU_BASRCB *rcb;
ST_RET rc;
MVLU_RPT_CLIENT *rptClient;	/* can access all rpt info from this struct.*/
BUFLIST_ENTRY *entry;

  rc = SD_FAILURE;

  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluRdVaCtrl->rdVaCtrl->va->base_va,
	    mvluRdVaCtrl->rt,
            &rptClient);
  if (rcb != NULL)
    {
#if 1	/* NOTE: Ed2 says to return different value if RptEna=FALSE or TRUE.*/
    if (rptClient->basrcb.RptEna==0)
      {
      entry = (BUFLIST_ENTRY *) list_find_last ((DBL_LNK *) rptClient->rpt_ctrl->brcbCtrl.rpt_list);
      if (entry == NULL)	/* empty list, return all 0	*/
        memset (mvluRdVaCtrl->primData, 0, 8);
      else
        memcpy (mvluRdVaCtrl->primData, entry->EntryID, 8);
      }
    else
      memcpy (mvluRdVaCtrl->primData, rptClient->rpt_ctrl->brcbCtrl.lastSentEntryID, 8);
#else	/* old way (before Ed2)	*/
    memcpy (mvluRdVaCtrl->primData, rptClient->rpt_ctrl->brcbCtrl.lastSentEntryID, 8);
#endif
    rc = SD_SUCCESS;
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, rc);
  }

/************************************************************************/
/*			mvlu_timeofentry_rd_ind				*/
/************************************************************************/

ST_VOID mvlu_timeofentry_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVLU_BASRCB *rcb;
ST_RET rc;
MVLU_RPT_CLIENT *rptClient;	/* can access all rpt info from this struct.*/
BUFLIST_ENTRY *entry;

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluRdVaCtrl->rdVaCtrl->va->base_va,
	    mvluRdVaCtrl->rt,
            &rptClient);
  if (rcb != NULL)
    {
#if 1	/* NOTE: Ed2 says to return different value if RptEna=FALSE or TRUE.*/
    if (rptClient->basrcb.RptEna==0)
      {
      entry = (BUFLIST_ENTRY *) list_find_last ((DBL_LNK *) rptClient->rpt_ctrl->brcbCtrl.rpt_list);
      if (entry == NULL)	/* empty list	*/
        {	/* Empty buffer. Return all 0.	*/
        MMS_BTIME6 *btime = (MMS_BTIME6 *)mvluRdVaCtrl->primData;
        btime->ms = 0;
        btime->day = 0;
        }
      else
        memcpy (mvluRdVaCtrl->primData, &entry->TimeOfEntry, sizeof (MMS_BTIME6));
      }
    else
      memcpy (mvluRdVaCtrl->primData, &rptClient->rpt_ctrl->brcbCtrl.lastSentTimeOfEntry, sizeof (MMS_BTIME6));
#else	/* old way (before Ed2)	*/
    memcpy (mvluRdVaCtrl->primData, &rcb->TimeofEntry, sizeof (MMS_BTIME6));
#endif
    rc = SD_SUCCESS;
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, rc);
  }

/************************************************************************/
/*			mvl61850_brcb_resvtms_rd_ind			*/
/* Read "ResvTms" attribute of BRCB.					*/
/* NOTE: should only be called for Edition 2 server.			*/
/************************************************************************/
ST_VOID mvl61850_brcb_resvtms_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
ST_INT16 *dest;
MVLU_BASRCB *rcb;
ST_RET rc;
MVLU_RPT_CLIENT *rptClient;	/* can access all rpt info from this struct.*/

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluRdVaCtrl->rdVaCtrl->va->base_va,
	    mvluRdVaCtrl->rt,
            &rptClient);
  if (rcb != NULL)
    {
    dest = (ST_INT16 *) mvluRdVaCtrl->primData;
    *dest = rcb->ResvTms;
    rc = SD_SUCCESS;
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, rc);
  }

/************************************************************************/
/*			mvl61850_owner_rd_ind				*/
/* Read "Owner" attribute of BRCB or URCB.				*/
/* NOTE: "Owner" is set when RCB is enabled OR when ResvTms is set to	*/
/*   positive value. "Owner" is cleared when ResvTms expires OR when	*/
/*   (ResvTms==0 and BRCB is disabled).*/
/************************************************************************/
ST_VOID mvl61850_owner_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVLU_BASRCB *rcb;
ST_RET rc;
MVLU_RPT_CLIENT *rptClient;	/* can access all rpt info from this struct.*/

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluRdVaCtrl->rdVaCtrl->va->base_va,
	    mvluRdVaCtrl->rt,
            &rptClient);
  if (rcb != NULL)
    {
    memcpy (mvluRdVaCtrl->primData, rcb->Owner, 64);
    rc = SD_SUCCESS;
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, rc);
  }

/************************************************************************/
/*                       mvlu_purgebuf_wr_ind				*/
/************************************************************************/
ST_VOID mvlu_purgebuf_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_BOOLEAN newval;	/* val to write to RptEna	*/
MVLU_BASRCB *rcb;
MVLU_RPT_CLIENT *rptClient;
ST_RET rc;

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluWrVaCtrl->wrVaCtrl->va->base_va,
	    mvluWrVaCtrl->rt,
            &rptClient);
  if (rcb && _rcb_writable (rcb, rptClient, mvluWrVaCtrl))
    {					/* rcb found and NOT enabled	*/
    rc = SD_SUCCESS;			/* allow write		*/
    newval = *(ST_BOOLEAN *) mvluWrVaCtrl->primData;

    /* If the client is enabling the Purge, dump rpts_sent & rpts_queued */
    if (newval != 0)	/* any non-zero val means "enable"	*/
      {
      /* this function does just what we want.		*/
      mvl61850_brcb_rpt_lists_clean (&rptClient->rpt_ctrl->brcbCtrl);
      }

    /* NOTE: don't actually change any value because PurgeBuf is	*/
    /* automatically set back to 0 when Purge is complete.		*/
    }
  else
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_TEMP_UNAVAIL;
  mvlu_wr_prim_done (mvluWrVaCtrl, rc);
  }

/************************************************************************/
/*                       mvlu_gi_wr_ind					*/
/************************************************************************/
ST_VOID mvlu_gi_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_BOOLEAN newval;	/* val to write to RptEna	*/
MVLU_BASRCB *rcb;
MVLU_RPT_CLIENT *rptClient;
ST_RET rc;

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluWrVaCtrl->wrVaCtrl->va->base_va,
	    mvluWrVaCtrl->rt,
            &rptClient);

  /* Check all condtions. If all conditions right, set rc=SD_SUCCESS to allow write.*/
  if (rcb != NULL)
    {
    /* RCB must be enabled.	*/
    /* NOTE: this is only RCB attribute that is writable when RCB enabled.*/
    if (rptClient->basrcb.RptEna)
      {
      if (rptClient->rpt_ctrl->rcb_type == RCB_TYPE_IEC_URCB)
        {
        /* URCB must be reserved by this clint. RCB is enabled, so it	*/
        /* must already be reserved. Just make sure reserved by this client.*/
        if (rptClient->netInfo == mvluWrVaCtrl->indCtrl->event->net_info)
          rc = SD_SUCCESS;			/* allow write		*/
        else
          MVLU_LOG_FLOW1 ("Write of GI for RptID='%s' FAILED because RCB reserved by another client",
              rcb->RptID);
        }
      else					/* RCB_TYPE_IEC_BRCB	*/
        rc = SD_SUCCESS;			/* allow write		*/
      }
    else
      MVLU_LOG_FLOW1 ("Write of GI for RptID='%s' FAILED because RCB not enabled",
           rcb->RptID);
    }

  if (rc == SD_SUCCESS)
    {
    newval = *(ST_BOOLEAN *) mvluWrVaCtrl->primData;

    /* If client is enabling the GI & TrgOps.GI=TRUE, start GI scan.*/
    if (newval != 0	/* any non-zero val means "enable"	*/
        && BSTR_BIT_GET(rcb->TrgOps.data, TRGOPS_BITNUM_GENERAL_INTERROGATION) != 0)
      mvlu_integrity_scan_read (rptClient,
                mvlu_gi_scan_va_done,
                mvlu_gi_scan_done); /* begin GI scan*/

    /* NOTE: don't actually change any value because GI is automatically*/
    /* set back to 0 when GI is complete.				*/
    }
  else		/* for any error, return TEMP_UNAVAIL	*/
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_TEMP_UNAVAIL;
  mvlu_wr_prim_done (mvluWrVaCtrl, rc);
  }

/************************************************************************/
/*                      mvlu_entryid_wr_ind				*/
/************************************************************************/
ST_VOID mvlu_entryid_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
MVLU_BASRCB *rcb;
MVLU_RPT_CLIENT *rptClient;
ST_RET rc;

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluWrVaCtrl->wrVaCtrl->va->base_va,
	    mvluWrVaCtrl->rt,
            &rptClient);
  if (rcb && _rcb_writable (rcb, rptClient, mvluWrVaCtrl))
    {
    /* NOTE: primData points to EntryID as Ostring8.	*/
    rc = mvl61850_brcb_rpt_set_entryid (rptClient, (ST_UCHAR *) mvluWrVaCtrl->primData);
    if (rc)
      {
      /* IEC 61850-7-2 Ed 2 says to return "parameter-value-inappropriate"*/
      /* if this fails. Not explicitly mapped to MMS error in 61850-8-1	*/
      /* but this seems like the appropriate mapping.			*/
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      }
    }
  else
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_TEMP_UNAVAIL;
  mvlu_wr_prim_done (mvluWrVaCtrl, rc);
  }

/************************************************************************/
/*			mvlu_sqnum_int16u_rd_ind_fun			*/
/* Same as mvlu_sqnum_rd_ind_fun except uses SqNumInt16u		*/
/* (UINT16 instead of UINT8).						*/
/************************************************************************/

ST_VOID mvlu_sqnum_int16u_rd_ind_fun (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
ST_UINT16 *dest;
MVLU_BASRCB *rcb;
ST_RET rc;

  rc = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluRdVaCtrl->rdVaCtrl->va->base_va,
	    mvluRdVaCtrl->rt,
            NULL);
  if (rcb != NULL)
    {
    dest = (ST_UINT16 *) mvluRdVaCtrl->primData;
    *dest = rcb->SqNumInt16u;
    rc = SD_SUCCESS;
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, rc);
  }

/************************************************************************/
/*			mvl61850_datset_wr_ind				*/
/* Try to change IEC 61850 Report Dataset.				*/
/* NOTE: for IEC 61850 only (not for UCA).				*/
/************************************************************************/
ST_VOID mvl61850_datset_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
MVLU_BASRCB *rcb;
ST_RET retCode;
ST_CHAR tmpbuf [MVL61850_MAX_OBJREF_LEN+1];
OBJECT_NAME nvl_oname;
MVLU_RPT_CLIENT *rptClient;
MVL_NVLIST_CTRL *nvl;
ST_BOOLEAN do_purge = SD_FALSE;	/* Chg to SD_TRUE if Purge Buffer required*/
ST_INT16 failure = ARE_TEMP_UNAVAIL;	/* use on err if no other value set*/

  retCode = SD_FAILURE;
  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	*/
  rcb = mvl61850_get_rcb (mvluWrVaCtrl->wrVaCtrl->va->base_va,
	    mvluWrVaCtrl->rt,
            &rptClient);
  if (rcb && _rcb_writable (rcb, rptClient, mvluWrVaCtrl))
    {
    if (strcmp (rcb->DatSetNa, mvluWrVaCtrl->primData) == 0)
      {
      retCode = SD_SUCCESS;	/* Same val written. Nothing to do but allow it*/
      }
    else if (mvluWrVaCtrl->primData[0] == '\0')
      {	/* client wrote empty string, and it was NOT already empty	*/
      mvl61850_rpt_dataset_destroy (rptClient->rpt_ctrl);
      rcb->ConfRev++;	/* DatSet changed. Must increment ConfRev.	*/
      retCode = SD_SUCCESS;
      }
    else if (strlen ((ST_CHAR *) mvluWrVaCtrl->primData) > MVL61850_MAX_OBJREF_LEN)
      {
      failure = ARE_OBJ_VALUE_INVALID;
      }
    else
      {
      /* strtok modifies input buffer, so copy to local buffer first.	*/
      strcpy (tmpbuf, (ST_CHAR *) mvluWrVaCtrl->primData);
      if (tmpbuf[0] == '@')
        {	/* AA-Specific Dataset	*/
        nvl_oname.obj_name.vmd_spec = &tmpbuf[1];	/*extract NVL name*/
        nvl_oname.object_tag = AA_SPEC;
        }
      else if (tmpbuf[0] == '/')
        {	/* VMD-Specific Dataset	*/
        nvl_oname.obj_name.vmd_spec = &tmpbuf[1];	/*extract NVL name*/
        nvl_oname.object_tag = VMD_SPEC;
        }
      else
        {	/* must be Domain-Specific Dataset	*/
        nvl_oname.domain_id = strtok (tmpbuf, "/");	/*extract dom name*/
        nvl_oname.obj_name.vmd_spec = strtok (NULL, "");	/*extract NVL name*/
        nvl_oname.object_tag = DOM_SPEC;
        }

      /* If DatSet name is illegal, fail.*/
      if (nvl_oname.obj_name.vmd_spec == NULL)	/* strtok failed*/
        {
        MVL_LOG_ERR1 ("DatSet name '%s' is invalid. Cannot write it to RCB.",
            (ST_CHAR *) mvluWrVaCtrl->primData);
        failure = ARE_OBJ_VALUE_INVALID;
        }
      else if (nvl_oname.object_tag == AA_SPEC &&
               rptClient->rpt_ctrl->rcb_type == RCB_TYPE_IEC_BRCB)
        {
        MVL_LOG_ERR1 ("Writing AA-Specific DatSet '%s' to BRCB is NEVER allowed.",
            (ST_CHAR *) mvluWrVaCtrl->primData);
        failure = ARE_OBJ_VALUE_INVALID;
        }
      else if (nvl_oname.object_tag == AA_SPEC &&
               rptClient->rpt_ctrl->rcb_type == RCB_TYPE_IEC_URCB &&
               rptClient->basrcb.Resv == 0)
        {
        MVL_LOG_ERR1 ("Writing AA-Specific DatSet '%s' to URCB not allowed because URCB is not reserved.",
            (ST_CHAR *) mvluWrVaCtrl->primData);
        failure = ARE_OBJ_VALUE_INVALID;
        }
      else
        {
        nvl = mvl_vmd_find_nvl (&mvl_vmd, &nvl_oname,
                  mvluWrVaCtrl->indCtrl->event->net_info);	/* for AA_SPEC*/
        if (nvl)
          {
          if (rptClient->rpt_ctrl->rcb_type == RCB_TYPE_IEC_BRCB)
            do_purge = SD_TRUE;	/* BRCB & val changed, must purge buffer*/
          /* Replace "dataset".	*/
          /* NOTE: If dataset was empty, destroy function does nothing.	*/
          mvl61850_rpt_dataset_destroy (rptClient->rpt_ctrl);
          retCode = mvl61850_rpt_dataset_create (rptClient->rpt_ctrl, nvl);
          /* WARNING: if create fails, old dataset is lost.*/
          if (retCode)
            {
            MVL_LOG_ERR1 ("DatSet change failed. RCB DatSet '%s' is no longer valid.", rcb->DatSetNa);
            /* Old DatSet is no longer valid, so clear it.*/
            rcb->DatSetNa[0] = '\0';
            }
          /* Even if create failed, DatSet changed (to NULL) so inc ConfRev.*/
          rcb->ConfRev++;	/* DatSet changed. Must increment ConfRev.*/
          }
        else	/* NVL cannot be found	*/
          {
          /* return this to pass IEC 61850 certification tests RpN4 and BrN4.*/
          failure = ARE_OBJ_VALUE_INVALID;
          }
        }
      }
    }

  if (do_purge)
    mvl61850_brcb_rpt_lists_clean (&rptClient->rpt_ctrl->brcbCtrl);

  if (retCode != SD_SUCCESS)
    mvluWrVaCtrl->wrVaCtrl->failure = failure;

  mvlu_wr_prim_done (mvluWrVaCtrl, retCode);
  }

/************************************************************************/
/*                       mvl61850_brcb_resvtms_wr_ind			*/
/* NOTE: should only be called for Edition 2 server.			*/
/************************************************************************/
ST_VOID mvl61850_brcb_resvtms_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_INT16 newval;	/* val to write to ResvTms	*/
MVLU_BASRCB *rcb;
MVLU_RPT_CLIENT *rptClient;
ST_RET retcode;

  /* Start out assuming success (retcode = SD_SUCCESS). On any error,	*/
  /* change it to SD_FAILURE & set "mvluWrVaCtrl->wrVaCtrl->failure".	*/
  retcode = SD_SUCCESS;

  newval = *(ST_INT16 *) mvluWrVaCtrl->primData;

  /* Only for 61850, so use mvl61850_get_rcb to find the RCB.	 */
  rcb = mvl61850_get_rcb (mvluWrVaCtrl->wrVaCtrl->va->base_va,
	    mvluWrVaCtrl->rt,
            &rptClient);
  if (rcb && _rcb_writable (rcb, rptClient, mvluWrVaCtrl))
    {
    /* Check current value first.	*/
    if (rcb->ResvTms < 0)
      {
      retcode = SD_FAILURE;
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;
      }
    else if (rcb->ResvTms == 0)
      {			/* not reserved	*/
      /* no error yet	*/
      }
    else	/* rcb->ResvTms > 0 (reserved)	*/
      {
      if (*(ST_UINT32 *) rcb->Owner != mvluWrVaCtrl->indCtrl->event->net_info->ass_ind_info.calling_paddr.netAddr.ip)
        {			/* reserved by another client	*/
        retcode = SD_FAILURE;
        mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;
        }
      }

    if (retcode == SD_SUCCESS)
      {
      /* Current value is OK. Now check the new value.	*/
      if (newval < 0)
        {
        retcode = SD_FAILURE;
        mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
        }
      else if (newval == 0)
        {
	/* unreserve BRCB now	*/
        /* Assume last 60 bytes of "Owner" are always 0.		*/    
        *(ST_UINT32 *) rcb->Owner = 0;
        }
      else	/* newval > 0	*/
        {
        /* reserve BRCB	now	*/
        /* Save Owner IP address. This indicates the BRCB is reserved.	*/
        /* Assume bytes after the first 4 of "Owner" are always 0.	*/    
        *(ST_UINT32 *) rcb->Owner = mvluWrVaCtrl->indCtrl->event->net_info->ass_ind_info.calling_paddr.netAddr.ip;

        /* We know we're connected, so save connection id. This owner on this connection now.*/
        /* Don't allow BRCB access on any other connection.	*/
        rptClient->netInfo = mvluWrVaCtrl->indCtrl->event->net_info;	/* reserve BRCB	*/
        }
      }
    }
  else
    {	/* BRCB NOT writable.*/
    retcode = SD_FAILURE;
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_TEMP_UNAVAIL;
    }

  if (retcode == SD_SUCCESS)
    rcb->ResvTms = newval;	/* no error. Finally write the data.	*/
  mvlu_wr_prim_done (mvluWrVaCtrl, retcode);
  }


/************************************************************************/
/*			fill_asn1_len_array				*/
/* Compute the ASN.1 encoded len for each var in a rpt dataset.		*/
/************************************************************************/
static ST_RET fill_asn1_len_array (MVLU_RPT_CLIENT *rptClient,
	MVL_NVLIST_CTRL *dsNvl,	/* rpt dataset				*/
	ST_INT *asn1_len_array)	/* array of ASN.1 lengths, one for each var*/
				/* array filled in by this function	*/
  {
ST_UCHAR *tmp_asn1_buf;	/* temporary ASN.1 buffer (allocated)	*/
ST_INT tmp_asn1_buf_size = mvl_cfg_info->max_msg_size;	/* reasonable size*/
ST_INT j;
ST_RET retCode=SD_SUCCESS;	/* assume success	*/

  /* Alloc tmp ASN.1 buffer. Use it for each encode in this loop.	*/
  tmp_asn1_buf = chk_malloc (tmp_asn1_buf_size);
  for (j = 0; j < dsNvl->num_of_entries; ++j)
    {
    if (rptClient->reasons_data[j])	/* this var should be included in rpt	*/
      {
      ST_UCHAR *asn1_ptr;
      MVL_TYPE_CTRL *type_ctrl = dsNvl->entries[j]->type_ctrl;

      retCode = ms_local_to_asn1_2 (type_ctrl->rt, type_ctrl->num_rt,
              dsNvl->entries[j]->data,	/* CRITICAL: use data from var*/
              tmp_asn1_buf,
              tmp_asn1_buf_size,
              &asn1_ptr,	/* function sets asn1_ptr (not needed here)*/
              &asn1_len_array[j]);	/* function sets len at this ptr*/
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
/*			mvl61850_objref_create				*/
/* Create the ObjectReference for this Object.				*/
/* objRef must point to buffer of at least [MVL61850_MAX_OBJREF_LEN+1] char*/
/************************************************************************/
ST_RET mvl61850_objref_create (ST_CHAR *objName,
	MVL_SCOPE *objScope, 
	ST_CHAR *objRef)	/* ptr to ObjectReference		*/
  {
  switch (objScope->scope)
    {
    case VMD_SPEC: 
      strcpy (objRef, "/");
    break;

    case DOM_SPEC: 
      strcpy (objRef, objScope->dom->name);
      strcat (objRef, "/");
    break;

    case AA_SPEC:  
      strcpy (objRef,"@");
    break;
    }

  /* Scope in place, now append the name of the Object.		*/
  if (strlen(objRef) + strlen(objName) > MVL61850_MAX_OBJREF_LEN)
    {	/* should NEVER happen	*/
    MVL_LOG_ERR0 ("Can't generate ObjectReference (too long)");
    objRef [0] = '\0';	/* return empty string	*/
    return (SD_FAILURE);
    }
  strcat (objRef, objName);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvl61850_objref_to_objname			*/
/* Convert IEC 61850 ObjectReference to MMS OBJECT_NAME.		*/
/* NOTE: This allocates OBJECT_NAME. Caller must call chk_free to free it.*/
/* RETURNS:	pointer to allocated OBJECT_NAME (NULL on error).	*/
/************************************************************************/
OBJECT_NAME *mvl61850_objref_to_objname (
	ST_CHAR *objref)	/* ObjectReference	*/
  {
ST_CHAR *string_buf;		/* copy of objref to be manipulated	*/
ST_CHAR *slash;			/* ptr to first '/' in (copy of) objref	*/
OBJECT_NAME *object_name;	/* struct allocated and filled in here	*/

  /* Allocate struct plus space for strings.*/
  object_name = chk_calloc (1, sizeof(OBJECT_NAME) + strlen (objref) + 1);

  /* Set string_buf to point immediately after the struct.	*/
  string_buf = (ST_CHAR *) (object_name + 1);
  /* Copy objref there.	*/
  strcpy (string_buf, objref);

  /* NOTE: strtok may modify string_buf.	*/
  /* Look for '/' in the ObjectReference.	*/
  slash = strchr(string_buf, '/');
  if (slash)
    {	/* found '/' in objref	*/
    if (slash == string_buf)
      {		/* '/' is first char. This is VMD_SPEC	*/
      object_name->object_tag = VMD_SPEC;
      object_name->obj_name.vmd_spec = &string_buf[1];	/*extract NVL name*/
      }
    else
      {		/* '/' is NOT first char. This is DOM_SPEC	*/
      object_name->object_tag = DOM_SPEC;
      object_name->domain_id = strtok (string_buf, "/");/*extract dom name*/
      object_name->obj_name.item_id = strtok (NULL, "");/*extract NVL name*/
      }
    }
  else if (string_buf[0] == '@')
    {	/* This is AA_SPEC	*/
    object_name->object_tag = AA_SPEC;
    object_name->obj_name.aa_spec = &string_buf[1];	/*extract NVL name*/
    }
  else
    {	/* objref is NOT a valid ObjectReference	*/
    chk_free (object_name);
    return (NULL);	/* ERROR	*/
    }
  return (object_name);
  }

