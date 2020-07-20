/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_ced1.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common primitive encode/decode functions	*/
/*	particular to get and write journal service.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 04/28/00  JRB     03    Lint cleanup					*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
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
/*			global structures & functions			*/
/************************************************************************/

static ENTRY_CONTENT    *ent_cont_ptr;

static ST_VOID (*get_jou_ent_cont_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_get_data_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_get_annotation (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_list_of_var_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_vlist_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_vlist_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_vs_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_value_spec_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_get_var_tag (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_event_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_get_state (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_evcon_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_ecn_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_evcon_name_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_event_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_data_cstr_done (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID jec_addl_detail_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_addl_detail_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jec_get_addl_detail (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID jec_get_occur_time (ASN1_DEC_CTXT *aCtx);

static VAR_INFO	       *var_info_ptr;

static ST_INT MAX_DATA_VARS;
static ST_RET ERROR_CODE;


/************************************************************************/
/************************************************************************/
/*			_ms_get_jou_ent_cont				*/
/* Function to get a Journal Entry Content from the received message.	*/
/************************************************************************/

ST_VOID _ms_get_jou_ent_cont (ASN1_DEC_CTXT *aCtx, ENTRY_CONTENT *data_ptr,
			  ST_INT max_vars, 
			  ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {
			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    ERROR_CODE = RESP_BAD_VALUE;
  else
    ERROR_CODE = REQ_BAD_VALUE;

  ent_cont_ptr	= data_ptr;
  MAX_DATA_VARS = max_vars;
  get_jou_ent_cont_done = done_fun;

  ent_cont_ptr->addl_detail_pres = SD_FALSE;	/* set default values	*/
  ent_cont_ptr->entry_form_tag = 0;
  ASN1R_TAG_ADD (aCtx, CTX,  0, jec_get_occur_time);
  }

/************************************************************************/
/*			jec_get_occur_time				*/
/* journal entry content is being decoded: occurrence time was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jec_get_occur_time (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_get_occur_time");
					/* read time from message	*/
  if (asn1r_get_btod (aCtx, &ent_cont_ptr->occur_time))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jec_get_addl_detail);
#endif
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, jec_get_data_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 	  	3, jec_get_annotation);

					/* set cstr done function	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = get_jou_ent_cont_done;
  }

#ifdef CS_SUPPORT
/************************************************************************/
/*			jec_get_addl_detail				*/
/* journal entry content is being decoded: additional detail was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jec_get_addl_detail (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_get_addl_detail");

  ent_cont_ptr->addl_detail_pres = SD_TRUE;	/* set addl detail pres */

		/* leave additional detail in place; just parse it.	*/
  ent_cont_ptr->addl_detail = aCtx->asn1r_field_ptr;
  asn1r_parse_next (aCtx, jec_addl_detail_done);
  }

/************************************************************************/
/*			jec_addl_detail_done				*/
/* journal entry content is being decoded: additional detail has been	*/
/* parsed.								*/
/************************************************************************/

static ST_VOID jec_addl_detail_done (ASN1_DEC_CTXT *aCtx)
  {

  ent_cont_ptr->addl_detail_len = aCtx->asn1r_octetcount;		/* save length	*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jec_addl_detail_cstr_done;
  }

/************************************************************************/
/*			jec_addl_detail_cstr_done			*/
/* journal entry content is being decoded: additional detail cstr done	*/
/* has been encountered.						*/
/************************************************************************/

static ST_VOID jec_addl_detail_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, jec_get_data_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 	  	3, jec_get_annotation);
  }
#endif

/************************************************************************/
/*			jec_get_data_cstr				*/
/* journal entry content is being decoded: data constructor was 	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jec_get_data_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_get_data_cstr");

  ent_cont_ptr->entry_form_tag = 2;		/* set data form tag	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, jec_event_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jec_list_of_var_cstr);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jec_data_cstr_done;
  }

/************************************************************************/
/*			jec_event_cstr					*/
/* journal entry content is being decoded: event cstr was encountered.	*/
/************************************************************************/

static ST_VOID jec_event_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_event_cstr");

  ent_cont_ptr->ef.data.event_pres = SD_TRUE;		/* set event present	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, jec_evcon_name_cstr);
  }

/************************************************************************/
/*			jec_evcon_name_cstr				*/
/* journal entry content is being decoded: event condition name cstr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jec_evcon_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_evcon_name_cstr");

  _ms_get_mms_objname (aCtx, &ent_cont_ptr->ef.data.evcon_name, jec_ecn_done);
  }

/************************************************************************/
/*			jec_ecn_done					*/
/* journal entry content is being decoded: event condition name has	*/
/* been obtained.							*/
/************************************************************************/

static ST_VOID jec_ecn_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_ecn_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jec_evcon_name_cstr_done;
  }

/************************************************************************/
/*			jec_evcon_name_cstr_done			*/
/* journal entry content is being decoded: event condition name cstr	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jec_evcon_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, jec_get_state);
  }

/************************************************************************/
/*			jec_get_state					*/
/* journal entry content is being decoded: current state was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jec_get_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_get_state");
					/* read time from message	*/
  if (asn1r_get_i16 (aCtx, &ent_cont_ptr->ef.data.cur_state) ||
     (ent_cont_ptr->ef.data.cur_state < 0) ||
     (ent_cont_ptr->ef.data.cur_state > 2))
    asn1r_set_dec_err (aCtx, UNCONF_BAD_VALUE);
					/* set cstr done function	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jec_event_cstr_done;
  }

/************************************************************************/
/*			jec_event_cstr_done				*/
/* journal entry content is being decoded: event cstr done was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jec_event_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jec_list_of_var_cstr);
  }

/************************************************************************/
/*			jec_list_of_var_cstr				*/
/* journal entry content is being decoded: list of variables cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jec_list_of_var_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_list_of_var_cstr");

  ent_cont_ptr->ef.data.list_of_var_pres = SD_TRUE;

  ent_cont_ptr->ef.data.num_of_var = 0;	  /* initialize # of variables	*/
					  /* initialize var list pointer*/
  var_info_ptr = (VAR_INFO *) (ent_cont_ptr + 1);

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, jec_vlist_cstr);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;
  }

/************************************************************************/
/*			jec_vlist_cstr					*/
/* journal entry content is being decoded: variable list constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jec_vlist_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_vlist_cstr");

  ent_cont_ptr->ef.data.num_of_var++;		/* increment # of variables	*/
  if (ent_cont_ptr->ef.data.num_of_var > MAX_DATA_VARS)
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  ASN1R_TAG_ADD (aCtx, CTX, 0, jec_get_var_tag);
  }

/************************************************************************/
/*			jec_get_var_tag 				*/
/* journal entry content is being decoded: variable tag was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jec_get_var_tag (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_get_var_tag");

		/* copy variable tag back onto itself shifted by 1 char */
  var_info_ptr->var_tag = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;

				/* read variable tag from message	*/
  if (asn1r_get_vstr (aCtx, var_info_ptr->var_tag))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jec_value_spec_cstr);
  }

/************************************************************************/
/*			jec_value_spec_cstr				*/
/* journal entry content is being decoded: value specification cstr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jec_value_spec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_value_spec_cstr");

  _ms_get_va_data (aCtx, &var_info_ptr->value_spec, jec_vs_done);
  }

/************************************************************************/
/*			jec_vs_done					*/
/* journal entry content is being decoded: value specification has been */
/* obtained.								*/
/************************************************************************/

static ST_VOID jec_vs_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_vs_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level]   = NULL;
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level-1] = jec_vlist_cstr_done;
  }

/************************************************************************/
/*			jec_vlist_cstr_done				*/
/* journal entry content is being decoded: variable list constructor	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jec_vlist_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  var_info_ptr++;			/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, jec_vlist_cstr);
  }

/************************************************************************/
/*			jec_data_cstr_done				*/
/* journal entry content is being decoded: data constructor done was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jec_data_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  }

/************************************************************************/
/*			jec_get_annotation				*/
/* journal entry content is being decoded: annotation was encountered.	*/
/************************************************************************/

static ST_VOID jec_get_annotation (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jec_get_annotation");

		/* copy annotation back onto itself shifted by 1 char	*/

  ent_cont_ptr->entry_form_tag = 3;
  ent_cont_ptr->ef.annotation = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;

  if (asn1r_get_vstr (aCtx, ent_cont_ptr->ef.annotation))	/* read annotation	*/
    asn1r_set_dec_err (aCtx, ERROR_CODE);
  }


/************************************************************************/
/************************************************************************/
/*			_ms_wr_jou_ent_cont 				*/
/* Function to write a Journal Entry Content to the message.		*/
/************************************************************************/

ST_VOID _ms_wr_jou_ent_cont (ASN1_ENC_CTXT *aCtx, ENTRY_CONTENT *data_ptr)
  {
VAR_INFO *vi_ptr;
ST_INT  i;

  if (data_ptr->entry_form_tag == 3)
    {					/* write annotation		*/
    asn1r_wr_vstr (aCtx, data_ptr->ef.annotation);
    asn1r_fin_prim (aCtx, 3, CTX);			/* context tag 3		*/
    }
  else
    {
    asn1r_strt_constr (aCtx);			/* start context tag 2 cstr	*/
    if (data_ptr->ef.data.list_of_var_pres)
      {
      asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
      vi_ptr = (VAR_INFO *) (data_ptr + 1);
      vi_ptr += data_ptr->ef.data.num_of_var - 1;
      for (i = 0; i < data_ptr->ef.data.num_of_var; i++)
	{
	asn1r_strt_constr (aCtx); 		/* start universal sequece cstr */
	asn1r_strt_constr (aCtx); 		/* start context tag 1 cstr	*/
	_ms_wr_va_data (aCtx, &vi_ptr->value_spec);
	asn1r_fin_constr (aCtx, 1, CTX, DEF);	/* finish context tag 1 cstr	*/
	asn1r_wr_vstr (aCtx, vi_ptr->var_tag);
	asn1r_fin_prim (aCtx, 0, CTX);
	asn1r_fin_constr (aCtx, SEQ_CODE, UNI, DEF);
	vi_ptr--;			/* point to previous var name	*/
	}
      asn1r_fin_constr (aCtx, 1, CTX, DEF); 	/* finish context tag 1 cstr	*/
      }
    if (data_ptr->ef.data.event_pres)
      {
      asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
      asn1r_wr_i16 (aCtx, data_ptr->ef.data.cur_state);/* write the current state	*/
      asn1r_fin_prim (aCtx, 1,CTX); 		/* context tag 1		*/
      asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
      _ms_wr_mms_objname (aCtx, &data_ptr->ef.data.evcon_name);
      asn1r_fin_constr (aCtx, 0, CTX, DEF); 	/* finish context tag 0 cstr	*/
      asn1r_fin_constr (aCtx, 0, CTX, DEF); 	/* finish context tag 0 cstr	*/
      }
    asn1r_fin_constr (aCtx, 2, CTX, DEF);		/* finish context tag 2 cstr	*/
    }

  if (data_ptr->addl_detail_pres)
    {
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    asn1r_wr_delmnt (aCtx, data_ptr->addl_detail,data_ptr->addl_detail_len);
    asn1r_fin_constr (aCtx, 1, CTX, DEF);		/* context tag 1		*/
    }
				/* select short form if not present	*/
  asn1r_wr_btod (aCtx, &data_ptr->occur_time);
  asn1r_fin_prim (aCtx, 0, CTX);			/* context tag 0		*/
  }
