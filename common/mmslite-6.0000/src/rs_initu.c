/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_initupl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of initiate upload	*/
/*	sequence.  It decodes the initiate upload sequence request	*/
/*	(indication) & encodes the initiate upload sequence response.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
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

static  INITUPL_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_init_upload_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_init_upload_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Initiate Upload Sequence Request");


/* get storage for request information	*/
  req_info = (INITUPL_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (INITUPL_REQ_INFO));
  aCtx->asn1r_err_fun  = _mms_dec_buf_free;

  if (asn1r_get_identifier (aCtx, req_info->dname))       /* read the domain name	      */
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_initupl_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the initiate upload sequence response.	*/
/************************************************************************/

ST_RET mp_initupl_resp (MMSREQ_IND *indptr, INITUPL_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_INIT_UPLOAD,indptr,
			M_CAST_MK_FUN (_ms_mk_initupl_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_initupl_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the initiate upload sequence response.		*/
/************************************************************************/

ST_RET mpl_initupl_resp (ST_UINT32 invoke_id, INITUPL_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_INIT_UPLOAD,invoke_id,
			M_CAST_MK_FUN (_ms_mk_initupl_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_initupl_resp 				*/
/* ENCODE an initiate upload sequence RESPONSE: 			*/
/************************************************************************/

ST_VOID _ms_mk_initupl_resp (ASN1_ENC_CTXT *aCtx, INITUPL_RESP_INFO *info)
  {
ST_INT	 i;
ST_CHAR	**cap_ptr;

  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/

  cap_ptr  = (ST_CHAR **) (info + 1);
  cap_ptr += (info->num_of_capab - 1);	/* point to last capability	*/
  for (i = 0; i < info->num_of_capab; i++)
    {
    asn1r_wr_vstr (aCtx, *cap_ptr);
    asn1r_fin_prim (aCtx, VISTR_CODE, UNI);
    cap_ptr--;
    }
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* ctx tag 1 constructor	*/

  asn1r_wr_i32 (aCtx, info->ulsmid);		/* write upload state machine id*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* ctx tag 0			*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_INIT_UPLOAD,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
