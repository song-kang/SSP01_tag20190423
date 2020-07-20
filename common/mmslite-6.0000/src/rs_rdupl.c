/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_rdupl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of request domain 	*/
/*	upload.  It decodes the request domain upload request		*/
/*	(indication) & encodes the request domain upload response. 	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   ------------------------------------------	*/
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
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  RDUPL_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion :     			*/
static	ST_INT	max_fnames;

static ST_VOID rdupl_fname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID rdupl_get_dname (ASN1_DEC_CTXT *aCtx);
static ST_VOID rdupl_fname_cstr_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_rdupl_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_rdupl_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Request Domain Upload Request");

#if defined(FIXED_DECBUF)
  max_fnames = m_sv_max_file_names;
  req_size = mmsl_dec_info_size;
#else
  max_fnames = (_mmsdec_msglen/2);
  if (m_sv_max_file_names && (max_fnames > m_sv_max_file_names))
    max_fnames = m_sv_max_file_names;
  req_size = sizeof (RDUPL_REQ_INFO) + (max_fnames * sizeof (FILE_NAME));
#endif

/* get storage for request information	*/
  req_info = (RDUPL_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  ASN1R_TAG_ADD (aCtx, CTX, 0,rdupl_get_dname);
  }

/************************************************************************/
/*			rdupl_get_dname					*/
/* request domain upload request is being decoded: domain name was	*/
/* encountered.								*/
/************************************************************************/

static ST_VOID rdupl_get_dname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rdupl_get_dname");

  if (asn1r_get_identifier (aCtx, req_info->dname))       /* read the domain name	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,rdupl_fname_cstr);
  }

/************************************************************************/
/*			rdupl_fname_cstr				*/
/* request domain upload request is being decoded: filename cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID rdupl_fname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rdupl_fname_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rdupl_fname_cstr_done;
  _ms_get_mms_fname (aCtx, (FILE_NAME *)(req_info + 1), max_fnames, 
                 &req_info->num_of_fname);
  }

/************************************************************************/
/*			rdupl_fname_cstr_done				*/
/* request domain upload request is being decoded: file name	list	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID rdupl_fname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;
				/* find actual size for request info	*/
  req_size = sizeof (RDUPL_REQ_INFO)
		+ (sizeof (FILE_NAME) * req_info->num_of_fname);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (RDUPL_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rdupl_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the request domain upload response.	*/
/************************************************************************/

ST_RET mp_rdupl_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_REQ_DOM_UPL));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rdupl_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the request domain upload response.		*/
/************************************************************************/

ST_RET mpl_rdupl_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_REQ_DOM_UPL));
  }
#endif
