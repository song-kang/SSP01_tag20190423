/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_geteca.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the get event	*/
/*	condition attributes. It decodes the get event condition	*/
/*	attributes request (indication) and encodes the get event	*/
/*	condition attributes response.					*/
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
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETECA_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID geteca_objname_done  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_geteca_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_geteca_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Event Condition Attributes Request");
/* get storage for request information	*/
  req_info = (GETECA_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETECA_REQ_INFO));
  _ms_get_mms_objname (aCtx, &req_info->evcon_name, geteca_objname_done);
  }

/************************************************************************/
/*			geteca_objname_done				*/
/* Decoding get event condition attributes request: event condition 	*/
/* name has been obtained						*/
/************************************************************************/

static ST_VOID geteca_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_geteca_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get event condition attributes	*/
/* response.								*/
/************************************************************************/

ST_RET mp_geteca_resp (MMSREQ_IND *indptr, GETECA_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_EC_ATTR,indptr,
			M_CAST_MK_FUN (_ms_mk_geteca_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_geteca_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get event condition attributes	response.	*/
/************************************************************************/

ST_RET mpl_geteca_resp (ST_UINT32 invoke_id, GETECA_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_EC_ATTR,invoke_id,
			M_CAST_MK_FUN (_ms_mk_geteca_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_geteca_resp					*/
/* Construct a get event condition attributes response.			*/
/************************************************************************/

ST_VOID _ms_mk_geteca_resp (ASN1_ENC_CTXT *aCtx, GETECA_RESP_INFO *info)
{
  if (info->eval_int_pres)
    {
    asn1r_wr_u32 (aCtx, info->eval_interval); 	/* write evaluation interval	*/
    asn1r_fin_prim (aCtx, 7, CTX);			/* ctx tag 7			*/
    }

  if (info->mon_var_pres)
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 6 constructor	*/
    if (info->mon_var_tag == SD_TRUE)	/* write undefined (NULL)	*/
      asn1r_fin_prim (aCtx, 1, CTX);		/* ctx tag 1			*/
    else
      {					/* write variable reference	*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
      _ms_wr_va_var_spec (aCtx, &info->var_ref);
      asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish ctx tag 0		*/
      }
    asn1r_fin_constr (aCtx, 6, CTX, DEF);		/* finish ctx tag 6		*/
    }

  if (info->as_reports)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->as_reports); 		/* write alarm summary reports	*/
    asn1r_fin_prim (aCtx, 4, CTX);			/* ctx tag 4			*/
    }

  if (info->severity != MMS_NORM_SEVERITY)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->severity);
    asn1r_fin_prim (aCtx, 3, CTX);
    }

  if (info->priority != MMS_NORM_PRIORITY)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->priority);
    asn1r_fin_prim (aCtx, 2, CTX);
    }

  asn1r_wr_i16 (aCtx, info->eclass);
  asn1r_fin_prim (aCtx, 1, CTX);

  if (info->mms_deletable)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->mms_deletable); 	/* write mms deletable indicator*/
    asn1r_fin_prim (aCtx, 0, CTX);			/* ctx tag 0			*/
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_EC_ATTR,CTX,DEF);	/* tag = opcode, ctx		*/
  }
