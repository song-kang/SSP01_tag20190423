/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_fopen.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the file open.	*/
/*	It decodes the file open request (indication) and encodes	*/
/*	the file open response. 					*/
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
#include "mms_pfil.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  FOPEN_REQ_INFO	*fop_info;

static	ST_INT	max_fnames;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID fopen_fname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID fopen_fname_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID fopen_get_init_pos    (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/*			mms_file_open_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_file_open_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("File Open Request");

#if defined(FIXED_DECBUF)
  max_fnames = m_sv_max_file_names;
  req_size = mmsl_dec_info_size;
#else
  max_fnames = (_mmsdec_msglen/2);
  if (m_sv_max_file_names && (max_fnames > m_sv_max_file_names))
    max_fnames = m_sv_max_file_names;
  req_size = sizeof (FOPEN_REQ_INFO) + (max_fnames * (sizeof (FILE_NAME)));
#endif

/* get storage for request information	*/
  fop_info = (FOPEN_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,fopen_fname_cstr);
  }

/************************************************************************/
/*			fopen_fname_cstr				*/
/* file open request is being decoded: filename cstr was encountered. 	*/
/************************************************************************/

static ST_VOID fopen_fname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fopen_fname_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = fopen_fname_cstr_done;
  _ms_get_mms_fname (aCtx, (FILE_NAME *)(fop_info + 1), max_fnames, 
					&fop_info->num_of_fname);
  }

/************************************************************************/
/*			fopen_fname_cstr_done				*/
/* file open request is being decoded: file name list constructor done  */
/* was encountered.							*/
/************************************************************************/

static ST_VOID fopen_fname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, fopen_get_init_pos);
  }

/************************************************************************/
/*			fopen_get_init_pos				*/
/* file open request is being decoded: initial position was encountered */
/* this must be all.							*/
/************************************************************************/

static ST_VOID fopen_get_init_pos (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

  MLOG_CDEC0 ("fopen_get_init_pos");

  if (asn1r_get_u32 (aCtx, &(fop_info->init_pos))) /* read the initial position	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

				/* find actual size for request info	*/
  req_size = sizeof (FOPEN_REQ_INFO)
		+ (sizeof (FILE_NAME) * fop_info->num_of_fname);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  fop_info = (FOPEN_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_fopen_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file open response.				*/
/************************************************************************/

ST_RET mp_fopen_resp (MMSREQ_IND *indptr, FOPEN_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_FILE_OPEN,indptr,
			M_CAST_MK_FUN (_ms_mk_fopen_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_fopen_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file open response.				*/
/************************************************************************/

ST_RET mpl_fopen_resp (ST_UINT32 invoke_id, FOPEN_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_FILE_OPEN,invoke_id,
			M_CAST_MK_FUN (_ms_mk_fopen_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_fopen_resp					*/
/* ENCODE a file open RESPONSE: 					*/
/************************************************************************/

ST_VOID _ms_mk_fopen_resp (ASN1_ENC_CTXT *aCtx, FOPEN_RESP_INFO *info_ptr)
  {
  asn1r_strt_constr (aCtx);			/* start the file attr constr	*/
  if (info_ptr->ent.mtimpres)		/* if data is to be included -	*/
    {
    asn1r_wr_time (aCtx, info_ptr->ent.mtime);	/* write the last modified time */
    asn1r_fin_prim (aCtx, 1,CTX);
    }

  asn1r_wr_u32 (aCtx, info_ptr->ent.fsize); 	/* write the file size		*/
  asn1r_fin_prim (aCtx, 0,CTX);

  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* close file attr constructor	*/

  asn1r_wr_i32 (aCtx, info_ptr->frsmid);		/* write the frsmid		*/
  asn1r_fin_prim (aCtx, 0,CTX);

		/* write the context specific tag for this opcode.	*/
  asn1r_fin_constr (aCtx, MMSOP_FILE_OPEN,CTX,DEF);
  }
