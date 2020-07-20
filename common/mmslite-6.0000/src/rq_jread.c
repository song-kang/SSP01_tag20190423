/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_jread.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of read journal.	*/
/*	It decodes the read journal response (confirm) & encodes the	*/
/*	read journal request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     06    Corr spelling for 'receive'.			*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 08/15/97  MDE     01    BTOD handling changes			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pjou.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  JREAD_RESP_INFO	*rsp_info;

static JOURNAL_ENTRY   *je_ptr;
static ST_INT max_jou_ent;
static ST_INT max_ec_vars;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID jread_get_orig_ae (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_jou_entry_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_get_more_follows (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_jou_entry_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_ec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_ec_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_orig_ae_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_orig_ae_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_entry_id_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_entry_id_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_entry_id_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_list_of_je_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_list_of_je_cstr (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_jread_rsp					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_jread_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT  rsp_size;

  MLOG_CDEC0 ("Read Journal Response");

#if defined(FIXED_DECBUF)
  max_jou_ent = m_cl_max_journal_entries;
  max_ec_vars = m_cl_max_entry_content;
  rsp_size = mmsl_dec_info_size;
#else
/* determine maximum storage - this over-estimates the maximum	*/
/* required storage, storage for the maximum number of journal	*/
/* entries would be sufficient. 				*/
  max_jou_ent  = _mmsdec_msglen/10;
  max_ec_vars  = _mmsdec_msglen/4;

  if (m_cl_max_journal_entries && (max_jou_ent > m_cl_max_journal_entries))
    max_jou_ent = m_cl_max_journal_entries;
  if (m_cl_max_entry_content && (max_ec_vars > m_cl_max_entry_content))
    max_ec_vars = m_cl_max_entry_content;
  rsp_size = sizeof (JREAD_RESP_INFO)
	     + (sizeof (JOURNAL_ENTRY) * max_jou_ent)
	     + (sizeof (VAR_INFO)      * max_ec_vars);
#endif

/* get storage for request information	*/
  rsp_info = (JREAD_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  je_ptr   = (JOURNAL_ENTRY *) (rsp_info + 1);
  rsp_info->more_follows = SD_FALSE;	/* set default value		*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, jread_list_of_je_cstr);
  }

/************************************************************************/
/*			jread_list_of_je_cstr				*/
/* read journal RESPONSE (confirm) is being decoded: list of journal	*/
/* entry constructor was encountered.					*/
/************************************************************************/

static ST_VOID jread_list_of_je_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_list_of_je_cstr");

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, jread_jou_entry_cstr);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_list_of_je_cstr_done;
  }

/************************************************************************/
/*			jread_jou_entry_cstr				*/
/* read journal response is being decoded: journal entry cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_jou_entry_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_jou_entry_cstr");

  rsp_info->num_of_jou_entry++; 	/* increment # of journal entry */
  if (rsp_info->num_of_jou_entry > max_jou_ent)
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 	 0, jread_entry_id_prim);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, jread_entry_id_cstr);
  }

/************************************************************************/
/*			jread_entry_id_prim				*/
/* read journal response is being decoded: entry id primitive was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_entry_id_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_entry_id_prim");

				/* decode entry id back onto itself	*/
  je_ptr->entry_id = aCtx->asn1r_field_ptr;

  if (asn1r_get_octstr (aCtx, je_ptr->entry_id))		/* read entry id	*/
    asn1r_set_dec_err (aCtx, RESP_UNSPECIFIED);

  je_ptr->entry_id_len = aCtx->asn1r_octetcount;

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jread_get_orig_ae);
  }

/************************************************************************/
/*			jread_entry_id_cstr				*/
/* read journal response is being decoded: entry id constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_entry_id_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_entry_id_cstr");

			/* decode entry id back onto itself		*/
  je_ptr->entry_id = aCtx->asn1r_field_ptr;

			/* set up done decoding octet cstr destination	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_entry_id_cstr_done;

			/* get entry id from the received message	*/
			/* set maximum length to accept entire message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, je_ptr->entry_id);
  }

/************************************************************************/
/*			jread_entry_id_cstr_done			*/
/* read journal response is being decoded: entry id constructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_entry_id_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  je_ptr->entry_id_len = aCtx->asn1r_octetcount;		/* save entry id len	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jread_get_orig_ae);
  }

/************************************************************************/
/*			jread_get_orig_ae				*/
/* read journal response is being decoded: originating ae cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_get_orig_ae (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_get_orig_ae");

  _ms_get_mms_app_ref (aCtx, &je_ptr->orig_ae, &je_ptr->orig_ae_len, jread_orig_ae_done);
  }

/************************************************************************/
/*			jread_orig_ae_done				*/
/* read journal response is being decoded: originating ae cstr done was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_orig_ae_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = jread_orig_ae_cstr_done;
  }

/************************************************************************/
/*			jread_orig_ae_cstr_done				*/
/* read journal response is being decoded: originating ae cstr done was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_orig_ae_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, jread_ec_cstr);
  }

/************************************************************************/
/*			jread_ec_cstr					*/
/* read journal response is being decoded: entry content constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_ec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_ec_cstr");

  _ms_get_jou_ent_cont (aCtx, &je_ptr->ent_content, max_ec_vars, jread_ec_done);
  }

/************************************************************************/
/*			jread_ec_done					*/
/* read journal response is being decoded: entry content has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID jread_ec_done (ASN1_DEC_CTXT *aCtx)
  {
VAR_INFO *vi_ptr;

  MLOG_CDEC0 ("jread_ec_done");

				  /* point to where next will go	*/
  if (je_ptr->ent_content.entry_form_tag == 2 &&
                       je_ptr->ent_content.ef.data.list_of_var_pres)
    {
    vi_ptr = (VAR_INFO *) (je_ptr + 1);
    je_ptr = (JOURNAL_ENTRY *)
		(vi_ptr + je_ptr->ent_content.ef.data.num_of_var);
    }
  else
    je_ptr++;

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_jou_entry_cstr_done;
  }

/************************************************************************/
/*			jread_jou_entry_cstr_done			*/
/* read journal response is being decoded: journal entry constructor	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jread_jou_entry_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, jread_jou_entry_cstr);
  }

/************************************************************************/
/*			jread_list_of_je_cstr_done			*/
/* read journal response is being decoded: list of journal entry cstr	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jread_list_of_je_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  ASN1R_TAG_ADD (aCtx, CTX, 1, jread_get_more_follows);
				/* find actual size for response info	*/
  rsp_size = (int) ((char *) je_ptr - (char *) rsp_info);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (JREAD_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jread_get_more_follows				*/
/* read journal response is being decoded: more follows indicator	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_get_more_follows");

  if (asn1r_get_bool (aCtx, &rsp_info->more_follows)) 	/* read more follows	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jread					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the read journal request. 		*/
/************************************************************************/

MMSREQ_PEND *mp_jread (ST_INT chan, JREAD_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_READ_JOURNAL,
			M_CAST_MK_FUN (_ms_mk_jread_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jread					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the read journal request. 				*/
/************************************************************************/

ST_RET  mpl_jread (JREAD_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_READ_JOURNAL,
			M_CAST_MK_FUN (_ms_mk_jread_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_jread_req					*/
/* ENCODE a read journal REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_jread_req (ASN1_ENC_CTXT *aCtx, JREAD_REQ_INFO *info)
  {
ST_INT	i;
ST_CHAR  **var_ptr;

  asn1r_strt_constr (aCtx);			/* start sequence constructor	*/

  if (info->sa_entry_pres)
    {					/* write start after entry	*/
    asn1r_strt_constr (aCtx);			/* start context tag 5 cstr	*/
    asn1r_wr_octstr (aCtx, info->entry_spec, info->entry_spec_len);
    asn1r_fin_prim (aCtx, 1,CTX);
    asn1r_wr_btod (aCtx, &info->time_spec);
    asn1r_fin_prim (aCtx, 0,CTX);
    asn1r_fin_constr (aCtx, 5,CTX,DEF);		/* finish context tag 5 cstr	*/
    }

  if (info->list_of_var_pres)
    {					/* write list of variables	*/
    asn1r_strt_constr (aCtx);			/* start context tag 4 cstr	*/
    var_ptr  = (ST_CHAR **) (info + 1);
    var_ptr += info->num_of_var - 1;	/* point to last entry		*/
    for (i = 0; i < info->num_of_var; i++)
      {
      asn1r_wr_vstr (aCtx, *var_ptr);		/* write the variable		*/
      asn1r_fin_prim (aCtx, VISTR_CODE,UNI);	/* universal string		*/
      var_ptr--;
      }
    asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* finish context tag 4 cstr	*/
    }

  if (info->range_stop_pres)
    {					/* write range stop		*/
    asn1r_strt_constr (aCtx);			/* start context tag 2 cstr	*/
    if (info->stop_tag == SD_TRUE)
      { 				/* write the number of entries	*/
      asn1r_wr_i32 (aCtx, info->num_of_entries);
      asn1r_fin_prim (aCtx, 1,CTX);
      }
    else
      { 				/* write the ending time	*/
      asn1r_wr_btod (aCtx, &info->end_time);
      asn1r_fin_prim (aCtx, 0,CTX);
      }
    asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish context tag 2 cstr	*/
    }

  if (info->range_start_pres)
    {					/* write range start		*/
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    if (info->start_tag == 1)
      { 				/* write the starting entry	*/
      asn1r_wr_octstr (aCtx, info->start_entry, info->start_entry_len);
      asn1r_fin_prim (aCtx, 1,CTX);
      }
    else
      { 				/* write the starting time	*/
      asn1r_wr_btod (aCtx, &info->start_time);
      asn1r_fin_prim (aCtx, 0,CTX);
      }
    asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish context tag 1 cstr	*/
    }

  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  _ms_wr_mms_objname (aCtx, &info->jou_name);	/* write the journal name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish context tag 0 cstr	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_READ_JOURNAL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
