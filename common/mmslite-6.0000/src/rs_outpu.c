/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_output.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the output	*/
/*	operation.  It decodes the output request (indication) and	*/
/*	encodes the output response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
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

static  OUTPUT_REQ_INFO	*req_info;

static ST_INT	max_data_count;
static ST_CHAR	**data_ptr;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID output_get_data (ASN1_DEC_CTXT *aCtx);
static ST_VOID output_data_list_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID output_get_data_list_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID output_get_station_name (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_output_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_output_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Output Request");

#if defined(FIXED_DECBUF)
  req_size = mmsl_dec_info_size;
  max_data_count = m_sv_max_data_count;
#else
  max_data_count = _mmsdec_msglen / 2;  /* find maximum size  */
  if (m_sv_max_data_count && (max_data_count > m_sv_max_data_count))
    max_data_count = m_sv_max_data_count;
  req_size = sizeof (OUTPUT_REQ_INFO) + (sizeof (ST_CHAR *) * max_data_count);
#endif

/* get storage for request information	*/
  req_info = (OUTPUT_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  data_ptr = (ST_CHAR **) (req_info + 1);
  req_info->data_count = 0;

  ASN1R_TAG_ADD (aCtx, CTX, 0, output_get_station_name);
  }

/************************************************************************/
/*			output_get_station_name 			*/
/* Output request: decode the operator station name			*/
/************************************************************************/

static ST_VOID output_get_station_name (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("output_get_station_name");
				/* read station name from message	*/
  if (asn1r_get_identifier (aCtx, req_info->station_name))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, output_get_data_list_cstr);
  }

/************************************************************************/
/*			output_get_data_list_cstr			*/
/* Output request: decode the output data constructor			*/
/************************************************************************/

static ST_VOID output_get_data_list_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("output_get_data_list_cstr");

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, output_get_data);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = output_data_list_cstr_done;
  }

/************************************************************************/
/*			output_get_data 				*/
/* Output request: decode the output data				*/
/************************************************************************/

static ST_VOID output_get_data (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("output_get_data");

  req_info->data_count++;		/* increment output data count	*/
  if (req_info->data_count > max_data_count)
    {					/* too many output data strings */
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  *data_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr-1; 	/* set output data pointer	*/
  if (asn1r_get_vstr (aCtx, *data_ptr))		/* shift output data left	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  data_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, output_get_data);
  }

/************************************************************************/
/*			output_data_list_cstr_done			*/
/* Output request: output data list constructor done			*/
/************************************************************************/

static ST_VOID output_data_list_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

				/* find actual size for request info	*/
  req_size = sizeof (OUTPUT_REQ_INFO)
		+ (sizeof (ST_CHAR *) * req_info->data_count);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (OUTPUT_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_output_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the output response.			*/
/************************************************************************/

ST_RET mp_output_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_OUTPUT));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_output_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the output response.				*/
/************************************************************************/

ST_RET mpl_output_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_OUTPUT));
  }
#endif
