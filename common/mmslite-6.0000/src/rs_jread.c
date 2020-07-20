/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_jread.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of read journal.	*/
/*	It decodes the read journal request (indication) & encodes	*/
/*	the read journal response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 08/28/03  EJV     07    Corr spelling for 'receive'.			*/
/* 12/20/01  JRB     06    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     04    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 08/15/97  MDE     02    BTOD handling changes			*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pjou.h"
#include "asn1defs.h"
#include "mem_chk.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  JREAD_REQ_INFO	*req_info;

static	ST_CHAR  **var_ptr;
static	ST_INT	max_vars;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID jread_range_stop_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_list_of_var_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_sa_entry_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_range_start_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_range_stop_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_sa_entry_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_entry_spec_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_entry_spec_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_get_time_spec (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_get_var (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_list_of_var_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_get_stop_time (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_get_num_of_ent (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_start_entry_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_get_start_time (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_start_entry_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_start_entry_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_range_start_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_jname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_jname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_jname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_entry_spec_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jread_dec_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_jread_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_jread_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Read Journal Request");

#if defined(FIXED_DECBUF)
  max_vars = m_sv_max_jread_vars;
  req_size = mmsl_dec_info_size;
#else
  max_vars = (_mmsdec_msglen/2);
  if (m_sv_max_jread_vars && (max_vars > m_sv_max_jread_vars))
    max_vars = m_sv_max_jread_vars;

  req_size = sizeof (JREAD_REQ_INFO) + (max_vars * sizeof (ST_CHAR *));
#endif

/* get storage for request information	*/
  req_info = (JREAD_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  var_ptr  = (ST_CHAR **) (req_info + 1);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, jread_jname_cstr);
  }

/************************************************************************/
/*			jread_jname_cstr				*/
/* read journal request is being decoded: journal name constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_jname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_jname_cstr");

  _ms_get_mms_objname (aCtx, &req_info->jou_name, jread_jname_done);
  }

/************************************************************************/
/*			jread_jname_done				*/
/* read journal request is being decoded: journal name has been 	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID jread_jname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_jname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_jname_cstr_done;
  }

/************************************************************************/
/*			jread_jname_cstr_done				*/
/* read journal request is being decoded: journal name constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_jname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jread_range_start_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, jread_range_stop_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, jread_list_of_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, jread_sa_entry_cstr);

  aCtx->asn1r_decode_done_fun   = jread_dec_done;	/* ok to be done, set done fun	*/
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jread_range_start_cstr				*/
/* read journal request is being decoded: range start constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_range_start_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_range_start_cstr");

  req_info->range_start_pres = SD_TRUE;

  ASN1R_TAG_ADD (aCtx, CTX, 	 0, jread_get_start_time);
  ASN1R_TAG_ADD (aCtx, CTX, 	 1, jread_start_entry_prim);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jread_start_entry_cstr);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jread_get_start_time				*/
/* initialize journal request is being decoded: start time was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_get_start_time (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_get_start_tim");

  req_info->start_tag = SD_SUCCESS;		/* set start time	*/

  if (asn1r_get_btod (aCtx, &req_info->start_time))		/* get start time	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_range_start_cstr_done;
  }

/************************************************************************/
/*			jread_start_entry_prim				*/
/* read journal request is being decoded: start entry primitive was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_start_entry_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_start_entry_prim");

  req_info->start_tag = SD_TRUE;			/* set start entry	*/

				/* decode start entry back onto itself	*/
  req_info->start_entry = aCtx->asn1r_field_ptr;

  if (asn1r_get_octstr (aCtx, req_info->start_entry))	/* read start entry	*/
    asn1r_set_dec_err (aCtx, REQ_UNSPECIFIED);

  req_info->start_entry_len = aCtx->asn1r_octetcount;

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_range_start_cstr_done;
  }

/************************************************************************/
/*			jread_start_entry_cstr				*/
/* read journal request is being decoded: start entry constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_start_entry_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_start_entry_cstr");

  req_info->start_tag = SD_TRUE;			/* set start entry	*/

			/* decode start entry back onto itself		*/
  req_info->start_entry = aCtx->asn1r_field_ptr;

			/* set up done decoding octet cstr destination	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_start_entry_cstr_done;

			/* get start entry from the received message	*/
			/* set maximum length to accept entire message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, req_info->start_entry);
  }

/************************************************************************/
/*			jread_start_entry_cstr_done			*/
/* read journal request is being decoded: start entry constructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_start_entry_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  req_info->start_entry_len = aCtx->asn1r_octetcount;	/* save start entry len */

			/* set up done decoding start spec cstr dest	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_range_start_cstr_done;
  }

/************************************************************************/
/*			jread_range_start_cstr_done			*/
/* read journal request is being decoded: range start constructor	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jread_range_start_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, jread_range_stop_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, jread_list_of_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, jread_sa_entry_cstr);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jread_range_stop_cstr				*/
/* read journal request is being decoded: range stop constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_range_stop_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_range_stop_cstr");

  req_info->range_stop_pres = SD_TRUE;

  ASN1R_TAG_ADD (aCtx, CTX, 0, jread_get_stop_time);
  ASN1R_TAG_ADD (aCtx, CTX, 1, jread_get_num_of_ent);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jread_get_stop_time				*/
/* initialize journal request is being decoded: ending time was 	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_get_stop_time (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_get_stop_tim");

  req_info->stop_tag = SD_SUCCESS;			/* set ending time	*/

  if (asn1r_get_btod (aCtx, &req_info->end_time))		/* get ending time	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_range_stop_cstr_done;
  }

/************************************************************************/
/*			jread_get_num_of_ent				*/
/* read journal request is being decoded: number of entries was 	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_get_num_of_ent (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_get_num_of_ent");

  req_info->stop_tag = SD_TRUE;			/* set num of entries	*/

  if (asn1r_get_i32 (aCtx, &req_info->num_of_entries))	/* read number of entry */
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_range_stop_cstr_done;
  }

/************************************************************************/
/*			jread_range_stop_cstr_done			*/
/* read journal request is being decoded: range stop constructor	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jread_range_stop_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, jread_list_of_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, jread_sa_entry_cstr);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jread_list_of_var_cstr				*/
/* read journal request is being decoded: list of variables constructor */
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_list_of_var_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_list_of_var_cstr");

  req_info->list_of_var_pres = SD_TRUE;

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, jread_get_var);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_list_of_var_cstr_done;
  }

/************************************************************************/
/*			jread_get_var					*/
/* read journal request is being decoded: variable was encountered.	*/
/************************************************************************/

static ST_VOID jread_get_var (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_get_var");

  req_info->num_of_var++;			/* increment # of var	*/
  if (req_info->num_of_var > max_vars)
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

	/* copy variable back onto itself shifted by one character	*/
  *var_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_vstr (aCtx, *var_ptr))
    {						/* variable is too ST_INT32 */
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  var_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, jread_get_var);
  }

/************************************************************************/
/*			jread_list_of_var_cstr_done			*/
/* read journal request is being decoded: list of variable constructor	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jread_list_of_var_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, jread_sa_entry_cstr);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jread_sa_entry_cstr				*/
/* read journal request is being decoded: start after entry constructor */
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_sa_entry_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_sa_entry_cstr");

  req_info->sa_entry_pres = SD_TRUE;

  ASN1R_TAG_ADD (aCtx, CTX, 0, jread_get_time_spec);
  }

/************************************************************************/
/*			jread_get_time_spec				*/
/* initialize journal request is being decoded: time specification was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_get_time_spec (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_get_time_spec");

  if (asn1r_get_btod (aCtx, &req_info->time_spec))		/* get time spec	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 	 1, jread_entry_spec_prim);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jread_entry_spec_cstr);
  }

/************************************************************************/
/*			jread_entry_spec_prim				*/
/* read journal request is being decoded: entry specification prim was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jread_entry_spec_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_entry_spec_prim");

				/* decode entry spec back onto itself	*/
  req_info->entry_spec = aCtx->asn1r_field_ptr;

  if (asn1r_get_octstr (aCtx, req_info->entry_spec))	/* read entry spec	*/
    asn1r_set_dec_err (aCtx, REQ_UNSPECIFIED);

  req_info->entry_spec_len = aCtx->asn1r_octetcount;

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_sa_entry_cstr_done;
  }

/************************************************************************/
/*			jread_entry_spec_cstr				*/
/* read journal request is being decoded: entry specification cstr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_entry_spec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jread_entry_spec_cstr");

			/* decode entry spec back onto itself		*/
  req_info->entry_spec = aCtx->asn1r_field_ptr;

			/* set up done decoding octet cstr destination	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_entry_spec_cstr_done;

			/* get entry spec from the received message	*/
			/* set maximum length to accept entire message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, req_info->entry_spec);
  }

/************************************************************************/
/*			jread_entry_spec_cstr_done			*/
/* read journal request is being decoded: entry spec constructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jread_entry_spec_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  req_info->entry_spec_len = aCtx->asn1r_octetcount;	/* save entry spec len	*/

			/* set up done decoding entry spec cstr dest	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jread_sa_entry_cstr_done;
  }

/************************************************************************/
/*			jread_sa_entry_cstr_done			*/
/* read journal request is being decoded: start after entry constructor */
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jread_sa_entry_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jread_dec_done					*/
/* read journal request is completely done.  Any extra storage is	*/
/* given back.								*/
/************************************************************************/

static ST_VOID jread_dec_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

  				/* find actual size for request info	*/
  req_size = sizeof (JREAD_REQ_INFO)
		+ (sizeof (ST_CHAR *) * req_info->num_of_var);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (JREAD_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _mms_dec_done_ok (aCtx);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jread_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the read journal response.		*/
/************************************************************************/

ST_RET mp_jread_resp (MMSREQ_IND *indptr, JREAD_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_READ_JOURNAL,indptr,
			M_CAST_MK_FUN (_ms_mk_jread_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jread_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the read journal response.				*/
/************************************************************************/

ST_RET mpl_jread_resp (ST_UINT32 invoke_id, JREAD_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_READ_JOURNAL,invoke_id,
			M_CAST_MK_FUN (_ms_mk_jread_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_jread_resp					*/
/* ENCODE a read journal RESPONSE:					*/
/************************************************************************/

ST_VOID _ms_mk_jread_resp (ASN1_ENC_CTXT *aCtx, JREAD_RESP_INFO *info_ptr)
  {
ST_INT	i;
ST_INT	jel_size;
JOURNAL_ENTRY **je_list;
JOURNAL_ENTRY **je_list_save;
JOURNAL_ENTRY  *je_ptr;
VAR_INFO       *vi_ptr;

  if (info_ptr->more_follows)
    {					/* not the default		*/
    asn1r_wr_bool (aCtx, info_ptr->more_follows);
    asn1r_fin_prim (aCtx, 1,CTX);			/* context tag 1		*/
    }
					/* allocate storage for ec ptrs */
  jel_size = sizeof (JOURNAL_ENTRY *) * (info_ptr->num_of_jou_entry+1);
  je_list  = (JOURNAL_ENTRY **) chk_calloc (1, jel_size);
  je_list_save = je_list;		/* save the ptr list pointer	*/
					/* find first journal entry	*/
  je_ptr     = (JOURNAL_ENTRY *) (info_ptr + 1);
  *(je_list) = je_ptr;
  for (i = 1; i < info_ptr->num_of_jou_entry; i++)
    {					/* find ptrs to journal entries */
    if (je_ptr->ent_content.entry_form_tag == 2 &&
        je_ptr->ent_content.ef.data.list_of_var_pres)
      {
      vi_ptr  = (VAR_INFO *) (je_ptr + 1);
      vi_ptr += je_ptr->ent_content.ef.data.num_of_var;
      je_ptr = (JOURNAL_ENTRY *) vi_ptr;
      }
    else
     je_ptr++;

    *(++je_list) = je_ptr;
    }

  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  for (i=0; i < info_ptr->num_of_jou_entry; i++)
    {
    asn1r_strt_constr (aCtx);			/* start journal entry cstr	*/
    je_ptr   = *(je_list--);
    asn1r_strt_constr (aCtx);			/* start context tag 2 cstr	*/
    _ms_wr_jou_ent_cont (aCtx, &je_ptr->ent_content);
    asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish context tag 2 cstr	*/
    asn1r_strt_constr (aCtx);			/* start originating ae cstr	*/
    asn1r_wr_delmnt (aCtx, je_ptr->orig_ae,je_ptr->orig_ae_len);
    asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish context tag 1 cstr	*/
    asn1r_wr_octstr (aCtx, je_ptr->entry_id, je_ptr->entry_id_len);
    asn1r_fin_prim (aCtx, 0,CTX);			/* finish context tag 0 	*/
    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish journal entry cstr	*/
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish context tag 0 cstr	*/
  chk_free (je_list_save);		/* free the list pointer	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_READ_JOURNAL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
