/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_download.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of download segment. */
/*	It decodes the download segment request (indication) & encodes	*/
/*	the download segment response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     08    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     07    Corr spelling for 'receive'.			*/
/* 12/20/01  JRB     06    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     04    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 07/03/97  MDE     02    Op-specific info buffer handling changes	*/
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

static  DOWNLOAD_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_download_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_download_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Download Segment Request");

/* get storage for request information	*/
  req_info = (DOWNLOAD_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (DOWNLOAD_REQ_INFO));
			/* read domain name from the received message	*/
  if (asn1r_get_identifier (aCtx, req_info->dname))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_download_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the download segment response.		*/
/************************************************************************/

ST_RET mp_download_resp (MMSREQ_IND *indptr,DOWNLOAD_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_DOWN_LOAD,indptr,
			M_CAST_MK_FUN (_ms_mk_download_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_download_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the download segment response.			*/
/************************************************************************/

ST_RET mpl_download_resp (ST_UINT32 invoke_id, 
			  DOWNLOAD_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_DOWN_LOAD,invoke_id,
			M_CAST_MK_FUN (_ms_mk_download_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_download_resp				*/
/* Construct a download segment response.				*/
/************************************************************************/

ST_VOID _ms_mk_download_resp (ASN1_ENC_CTXT *aCtx, DOWNLOAD_RESP_INFO *info)
{
  asn1r_strt_constr (aCtx);			/* start the sequence constr	*/
  if (!info->more_follows)
    {					/* NOT the default (SD_TRUE)	*/
    asn1r_wr_bool (aCtx, info->more_follows); 	/* write more follows indicator */
    asn1r_fin_prim (aCtx, 1,CTX);			/* context tag 1		*/
    }

  if (info->load_data_type == LOAD_DATA_NON_CODED)
    {
    asn1r_wr_octstr (aCtx, info->load_data,info->load_data_len);
    asn1r_fin_prim (aCtx, 0,CTX);			/* context tag 0		*/
    }
  else		/* CODED data, write as complete ASN.1 EXTERNAL		*/
    asn1r_wr_delmnt (aCtx, info->load_data,info->load_data_len);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DOWN_LOAD,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }

