/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_obtfile.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the obtain file.	*/
/*	It decodes the obtain file request (indication) and encodes	*/
/*	the obtain file response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
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
#include "mms_pfil.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  OBTFILE_REQ_INFO	*reqinfo;

static	ST_INT	max_fnames;
static FILE_NAME	*fname_idx;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/* static functions with multiple references                            */
static ST_VOID obtfile_src_fname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID ar_title_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID obtfile_dest_fname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID obtfile_dest_fname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID obtfile_src_fname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID obtfile_ar_title_cstr (ASN1_DEC_CTXT *aCtx);

static ST_UCHAR *ar_start;


/************************************************************************/
/************************************************************************/
/*			mms_obtain_file_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_obtain_file_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Obtain File Request");

#if defined(FIXED_DECBUF)
  max_fnames = m_sv_max_file_names;
  req_size = mmsl_dec_info_size;
#else
  max_fnames = (_mmsdec_msglen/2);
  if (m_sv_max_file_names && (max_fnames > m_sv_max_file_names))
    max_fnames = m_sv_max_file_names;
  req_size = sizeof (OBTFILE_REQ_INFO) + (max_fnames * sizeof (FILE_NAME));
#endif

/* get storage for request information	*/
  reqinfo = (OBTFILE_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  reqinfo->ar_title_pres = SD_FALSE;	/* set default value		*/
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, obtfile_ar_title_cstr);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 1, obtfile_src_fname_cstr);

  ar_start = aCtx->asn1r_field_ptr;			/* save start of AR constructor	*/
  }

/************************************************************************/
/*			obtfile_ar_title_cstr				*/
/* obtain file request is being decoded : application title constructor */
/* was encountered.							*/
/************************************************************************/

static ST_VOID obtfile_ar_title_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("obtfile_ar_title_cstr");

  reqinfo->ar_title_pres = SD_TRUE;	/* set ap title present 	*/
  reqinfo->ar_title = ar_start;		/* save start			*/
  *ar_start = '\x30';			/* change tag			*/

  if (aCtx->asn1r_elmnt_len)
    asn1r_parse_cstr_contents (aCtx, ar_title_cstr_done);
  else				/* no contents, call user done function	*/
    aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = ar_title_cstr_done;
  }

/************************************************************************/
/*			ar_title_cstr_done				*/
/* obtain file request is being decoded : application title primitive	*/
/* was encountered and read.						*/
/************************************************************************/

static ST_VOID ar_title_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ar_title_cstr_done");

  reqinfo->ar_len = (int) (aCtx->asn1r_field_ptr - ar_start);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,obtfile_src_fname_cstr);
  }

/************************************************************************/
/*			obtfile_src_fname_cstr				*/
/* obtain file request is being decoded: source filename cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID obtfile_src_fname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("obtfile_src_fname_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = obtfile_src_fname_cstr_done;
  fname_idx = (FILE_NAME *)(reqinfo + 1);
  _ms_get_mms_fname (aCtx, fname_idx, max_fnames, &reqinfo->num_of_src_fname);
  }

/************************************************************************/
/*			obtfile_src_fname_cstr_done    			*/
/* obtain file request is being decoded: source file name	list	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID obtfile_src_fname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,obtfile_dest_fname_cstr);
  }

/************************************************************************/
/*			obtfile_dest_fname_cstr				*/
/* obtain file request is being decoded: destination filename cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID obtfile_dest_fname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("obtfile_dest_fname_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = obtfile_dest_fname_cstr_done;
  fname_idx += reqinfo -> num_of_src_fname;
  _ms_get_mms_fname (aCtx, fname_idx, max_fnames, &reqinfo->num_of_dest_fname);
  }

/************************************************************************/
/*			obtfile_dest_fname_cstr_done    			*/
/* obtain file request is being decoded: destination file name	list	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID obtfile_dest_fname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

				/* find actual size for request info	*/
  req_size = sizeof (OBTFILE_REQ_INFO)
             + (sizeof (FILE_NAME) * reqinfo->num_of_src_fname)
	     + (sizeof (FILE_NAME) * reqinfo->num_of_dest_fname);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  reqinfo = (OBTFILE_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_obtfile_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the obtain file response.				*/
/************************************************************************/

ST_RET mp_obtfile_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_OBTAIN_FILE));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_obtfile_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the obtain file response.				*/
/************************************************************************/

ST_RET mpl_obtfile_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_OBTAIN_FILE));
  }
#endif
