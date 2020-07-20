/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_input.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the input 	*/
/*	operation.  It decodes the input request (indication) and	*/
/*	encodes the input response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pocs.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  INPUT_REQ_INFO	*req_info;

static ST_INT	max_prompt_count;
static ST_CHAR	**prompt_ptr;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID input_prompt_data_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID input_get_timeout (ASN1_DEC_CTXT *aCtx);
static ST_VOID input_get_prompt_data (ASN1_DEC_CTXT *aCtx);
static ST_VOID input_prompt_data_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID input_get_echo (ASN1_DEC_CTXT *aCtx);
static ST_VOID input_get_station_name (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_input_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_input_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Input Request");

#if defined(FIXED_DECBUF)
  max_prompt_count = m_sv_max_prompt_count;
  req_size = mmsl_dec_info_size;
#else
  max_prompt_count = _mmsdec_msglen / 2;  /* find maximum size  */
  if (m_sv_max_prompt_count && (max_prompt_count > m_sv_max_prompt_count))
    max_prompt_count = m_sv_max_prompt_count;
  req_size = sizeof (INPUT_REQ_INFO) + (sizeof (ST_CHAR *) * max_prompt_count);
#endif

/* get storage for request information	*/
  req_info = (INPUT_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  prompt_ptr = (ST_CHAR **) (req_info + 1);
  req_info->echo	 = SD_TRUE;
  req_info->timeout_pres = SD_FALSE;
  req_info->prompt_count = 0;

  ASN1R_TAG_ADD (aCtx, CTX, 0, input_get_station_name);
  }

/************************************************************************/
/*			input_get_station_name				*/
/* input request is being decoded: get the station name.		*/
/************************************************************************/

static ST_VOID input_get_station_name (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("input_get_station_name");

  if (asn1r_get_identifier (aCtx, req_info->station_name))	/* read station name	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 	 1, input_get_echo);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, input_prompt_data_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 	 3, input_get_timeout);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			input_get_echo					*/
/* input request is being decoded: input echo was encountered.		*/
/************************************************************************/

static ST_VOID input_get_echo (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("input_get_echo");

  if (asn1r_get_bool (aCtx, &req_info->echo)) 		/* read input echo	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, input_prompt_data_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 	 3, input_get_timeout);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			input_prompt_data_cstr				*/
/* input request is being decoded: prompt data list constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID input_prompt_data_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("input_prompt_data_cstr");

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, input_get_prompt_data);
  req_info->prompt_pres = SD_TRUE;
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = input_prompt_data_cstr_done;
  }

/************************************************************************/
/*			input_get_prompt_data				*/
/* input request is being decoded: prompt data was encountered. 	*/
/************************************************************************/

static ST_VOID input_get_prompt_data (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("input_get_prompt_dat");

  req_info->prompt_count++;		/* increment prompt data count	*/
  if (req_info->prompt_count > max_prompt_count)
    {					/* too many prompt data strings */
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  *prompt_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr-1;	/* set prompt data pointer	*/
  if (asn1r_get_vstr (aCtx, *prompt_ptr))		/* shift prompt data left	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  prompt_ptr++; 			/* point to where next will go	*/
  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, input_get_prompt_data);
  }

/************************************************************************/
/*			input_prompt_data_cstr_done			*/
/* input request is being decoded: prompt data list constructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID input_prompt_data_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

				/* find actual size for request info	*/
  req_size = sizeof (INPUT_REQ_INFO)
		+ (sizeof (ST_CHAR *) * req_info->prompt_count);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (INPUT_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;
  ASN1R_TAG_ADD (aCtx, CTX, 3, input_get_timeout);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			input_get_timeout				*/
/* input request is being decoded: input timeout was encountered.	*/
/************************************************************************/

static ST_VOID input_get_timeout (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("input_get_timeou");

  req_info->timeout_pres = SD_TRUE;	/* set input timeout present	*/

  if (asn1r_get_u32 (aCtx, &req_info->timeout))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_input_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the input response.			*/
/************************************************************************/

ST_RET mp_input_resp (MMSREQ_IND *indptr, INPUT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_INPUT,indptr,
			M_CAST_MK_FUN (_ms_mk_input_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_input_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the input response.				*/
/************************************************************************/

ST_RET mpl_input_resp (ST_UINT32 invoke_id, INPUT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_INPUT,invoke_id,
			M_CAST_MK_FUN (_ms_mk_input_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_input_resp					*/
/* ENCODE an input RESPONSE:						*/
/************************************************************************/

ST_VOID _ms_mk_input_resp (ASN1_ENC_CTXT *aCtx, INPUT_RESP_INFO *info_ptr)
  {
  asn1r_wr_vstr (aCtx, info_ptr->input_resp);	/* encode the input response	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_INPUT,CTX); 		/* tag = opcode, ctx		*/
  }
