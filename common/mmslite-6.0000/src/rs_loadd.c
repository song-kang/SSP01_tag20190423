/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_loaddom.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of load domain	*/
/*	content.  It decodes the load domain content request		*/
/*	(indication) & encodes the load domain content response.	*/
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
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
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

static  LOADDOM_REQ_INFO	*req_info;

static ST_CHAR	**cap_ptr;
static	ST_INT	max_capabilities;
static	ST_INT	max_fnames;
static ST_UCHAR   *ar_start;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/
static ST_VOID loaddom_fname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID loaddom_fname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID loaddom_get_sharable (ASN1_DEC_CTXT *aCtx);
static ST_VOID loaddom_get_cap  (ASN1_DEC_CTXT *aCtx);
static ST_VOID loaddom_cap_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID loaddom_cap_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID loaddom_get_dname  (ASN1_DEC_CTXT *aCtx);
static ST_VOID loaddom_third_pty_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID third_pty_cstr_done (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			mms_load_domain_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_load_domain_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

  MLOG_CDEC0 ("Load Domain Content Request");


#if defined(FIXED_DECBUF)
  max_fnames = m_sv_max_file_names;
  max_capabilities = m_sv_max_loadd_cap;
  req_size = mmsl_dec_info_size;
#else
  max_capabilities = (_mmsdec_msglen/2);
  max_fnames = (_mmsdec_msglen/2);
  if (m_sv_max_loadd_cap && (max_capabilities > m_sv_max_loadd_cap))
    max_capabilities = m_sv_max_loadd_cap;
  if (m_sv_max_file_names && (max_fnames > m_sv_max_file_names))
    max_fnames = m_sv_max_file_names;
  req_size = sizeof(LOADDOM_REQ_INFO) + 
	     	(max_fnames * sizeof(FILE_NAME)) +
  		(max_capabilities * sizeof(ST_CHAR *));
#endif

/* get storage for request information	*/
  req_info = (LOADDOM_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  cap_ptr  = (ST_CHAR **) (req_info + 1);
  ASN1R_TAG_ADD (aCtx, CTX, 0,loaddom_get_dname);
  }

/************************************************************************/
/*			loaddom_get_dname				*/
/* load domain content request is being decoded: domain name was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID loaddom_get_dname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("loaddom_get_dname");

  if (asn1r_get_identifier (aCtx, req_info->dname)) /* read the domain name 	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1,loaddom_cap_cstr);
  }

/************************************************************************/
/*			loaddom_cap_cstr				*/
/* load domain content request is being decoded: capability list	*/
/* constructor was encountered. 					*/
/************************************************************************/

static ST_VOID loaddom_cap_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("loaddom_cap_cstr");

  ASN1R_TAG_ADD (aCtx, UNI,VISTR_CODE,loaddom_get_cap);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = loaddom_cap_cstr_done;
  }

/************************************************************************/
/*			loaddom_get_cap 				*/
/* load domain content request is being decoded: capability string	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID loaddom_get_cap (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("loaddom_get_cap");

  req_info->num_of_capab++;		/* increment # of capabilities	*/
  if (req_info->num_of_capab > max_capabilities)
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

	/* copy capability back onto itself shifted by one character	*/
  *cap_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_vstr (aCtx, *cap_ptr))
    {					/* capability is too ST_INT32	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  cap_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI,VISTR_CODE,loaddom_get_cap);
  }

/************************************************************************/
/*			loaddom_cap_cstr_done				*/
/* load domain content request is being decoded: capability list	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID loaddom_cap_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX,2,loaddom_get_sharable);
  }

/************************************************************************/
/*			loaddom_get_sharable				*/
/* load domain content request is being decoded: sharable was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID loaddom_get_sharable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("loaddom_get_sharable");
			/* read sharable from the received message	*/
  if (asn1r_get_bool (aCtx, &(req_info->sharable)))
    {					/* sharable is too ST_INT32 	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,4,loaddom_fname_cstr);
  }

/************************************************************************/
/*			loaddom_fname_cstr				*/
/* load domain content request is being decoded: filename cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID loaddom_fname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("loaddom_fname_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = loaddom_fname_cstr_done;

/* In the case of a request domain download request the file names are	*/
/* put right after the cabability string pointers.  The max number of 	*/
/* these file names is the same worst case as the capability strings.	*/

  _ms_get_mms_fname (aCtx, (FILE_NAME *) cap_ptr, max_fnames, &req_info->num_of_fname);
  }

/************************************************************************/
/*			loaddom_fname_cstr_done				*/
/* load domain content request is being decoded: file name	list	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID loaddom_fname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

				/* find actual size for request info	*/
  req_size = sizeof (LOADDOM_REQ_INFO)
		+ (sizeof (ST_CHAR *) * req_info->num_of_capab)
		+ (sizeof (FILE_NAME) * req_info->num_of_fname);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr, req_size);
  req_info = (LOADDOM_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, loaddom_third_pty_cstr);
  ar_start = aCtx->asn1r_field_ptr;		/* save in case third party rcvd	*/
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			loaddom_third_pty_cstr 				*/
/* load domain content request is being decoded: third party		*/
/* cstr was encountered.						*/
/************************************************************************/

static ST_VOID loaddom_third_pty_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("loaddom_third_pty_cstr");

  req_info->third_pty_pres = SD_TRUE;		/* set third party	*/
  req_info->third_pty = ar_start;		/* save_start		*/
  *ar_start = '\x30';				/* change tag		*/

  if (aCtx->asn1r_elmnt_len)
    asn1r_parse_cstr_contents (aCtx, third_pty_cstr_done);
  else
    aCtx->asn1r_c_done_fun[ aCtx->asn1r_msg_level ] = third_pty_cstr_done;
  }

/************************************************************************/
/*			third_pty_cstr_done				*/
/* load domain content request is being decoded: third party		*/
/* was encountered and read into req structure. 			*/
/************************************************************************/

static ST_VOID third_pty_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("third_pty_cstr_done");

  req_info->third_pty_len = (int) (aCtx->asn1r_field_ptr - ar_start);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_loaddom_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the load domain content response. 	*/
/************************************************************************/

ST_RET mp_loaddom_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_LOAD_DOMAIN));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_loaddom_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the load domain content response. 			*/
/************************************************************************/

ST_RET mpl_loaddom_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_LOAD_DOMAIN));
  }
#endif
