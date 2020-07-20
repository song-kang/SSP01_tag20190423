/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_getcl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of get capability	*/
/*	list.  It decodes the get capability list request (indication)	*/
/*	& encodes the get capability list response.			*/
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
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETCL_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getcl_get_cont_after (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_getcl_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_getcl_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Capability List Request");


/* get storage for request information	*/
  req_info = (GETCL_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETCL_REQ_INFO));
  req_info->cont_after_pres = SD_FALSE;

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getcl_get_cont_after);
  asn1r_set_all_cstr_done (aCtx); 	/* clear cstr done functions		*/
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getcl_get_cont_after				*/
/* get capability list REQUEST (indication) is being decoded:		*/
/* continue after was encountered.					*/
/************************************************************************/

static ST_VOID getcl_get_cont_after (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getcl_get_cont_after");

  req_info->cont_after_pres = SD_TRUE;

/* copy continue after back onto itself, shifted by one character	*/
  req_info->continue_after = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_vstr (aCtx, req_info->continue_after))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getcl_resp 					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get capability list response.		*/
/************************************************************************/

ST_RET mp_getcl_resp (MMSREQ_IND *indptr, GETCL_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_CAP_LIST,indptr,
			M_CAST_MK_FUN (_ms_mk_getcl_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getcl_resp 					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get capability list response.			*/
/************************************************************************/

ST_RET  mpl_getcl_resp (ST_UINT32 invoke_id, GETCL_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_CAP_LIST,invoke_id,
			M_CAST_MK_FUN (_ms_mk_getcl_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getcl_resp 					*/
/* ENCODE a get capability list RESPONSE: 				*/
/************************************************************************/

ST_VOID _ms_mk_getcl_resp (ASN1_ENC_CTXT *aCtx, GETCL_RESP_INFO *info)
  {
ST_INT	 i;
ST_CHAR	**cap_ptr;

  if (!info->more_follows)		/* not the default of SD_TRUE	*/
    {					/* write more follows not pres	*/
    asn1r_wr_bool (aCtx, info->more_follows);
    asn1r_fin_prim (aCtx, 1,CTX);
    }

  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/

  cap_ptr  = (ST_CHAR **) (info + 1);
  cap_ptr += (info->num_of_capab - 1);	/* point to last capability	*/
  for (i = 0; i < info->num_of_capab; i++)
    {
    asn1r_wr_vstr (aCtx, *cap_ptr);
    asn1r_fin_prim (aCtx, VISTR_CODE, UNI);
    cap_ptr--;
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* ctx tag 1 constructor	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_CAP_LIST,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
