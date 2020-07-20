/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_uploa.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of upload segment.	*/
/*	It decodes the upload segment request (indication) and encodes	*/
/*	the upload segment response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     06    Corr spelling for 'receive'.			*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  UPLOAD_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_upload_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_upload_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Upload Segment Request");

/* get storage for request information	*/
  req_info = (UPLOAD_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (UPLOAD_REQ_INFO));

		/* read upload state machine id from received message	*/
  if (asn1r_get_i32 (aCtx, &(req_info->ulsmid)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_upload_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the upload segment response.		*/
/************************************************************************/

ST_RET mp_upload_resp (MMSREQ_IND *indptr, UPLOAD_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_UP_LOAD,indptr,
			M_CAST_MK_FUN (_ms_mk_upload_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_upload_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the upload segment response.			*/
/************************************************************************/

ST_RET mpl_upload_resp (ST_UINT32 invoke_id, UPLOAD_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_UP_LOAD,invoke_id,
			M_CAST_MK_FUN (_ms_mk_upload_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_upload_resp					*/
/* Construct an upload segment response.				*/
/************************************************************************/

ST_VOID _ms_mk_upload_resp (ASN1_ENC_CTXT *aCtx, UPLOAD_RESP_INFO *info)
  {
  if (!info->more_follows)
    {					/* NOT the default (SD_TRUE)	*/
    asn1r_wr_bool (aCtx, info->more_follows); 	/* write more follows flag	*/
    asn1r_fin_prim (aCtx, 1, CTX);
    }

  if (info->load_data_type != LOAD_DATA_CODED)
    {
    asn1r_wr_octstr (aCtx, info->load_data,info->load_data_len);
    asn1r_fin_prim (aCtx, 0,CTX);			/* context tag 0		*/
    }
  else		/* CODED data, write as complete ASN.1 EXTERNAL		*/
    asn1r_wr_delmnt (aCtx, info->load_data,info->load_data_len);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_UP_LOAD,CTX,DEF); 	/* tag = opcode, ctx constr	*/
  }
