/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_repec.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the report event	*/
/*	condition status. It decodes the report event condition	status	*/
/*	request (indication) and encodes the report event condition 	*/
/*	status response.						*/
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

static  REPECS_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID repecs_objname_done  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_repecs_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_repecs_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Event Condition Status Request");
				/* get storage for request information	*/
  req_info = (REPECS_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (REPECS_REQ_INFO));
  _ms_get_mms_objname (aCtx, &req_info->evcon_name, repecs_objname_done);
  }

/************************************************************************/
/*			repecs_objname_done				*/
/* Decoding report event condition status request: event condition 	*/
/* name has been obtained						*/
/************************************************************************/

static ST_VOID repecs_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_repecs_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report event condition status		*/
/************************************************************************/

ST_RET mp_repecs_resp (MMSREQ_IND *indptr, REPECS_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_REP_EC_STAT,indptr,
			M_CAST_MK_FUN (_ms_mk_repecs_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_repecs_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report event condition status.			*/
/************************************************************************/

ST_RET mpl_repecs_resp (ST_UINT32 invoke_id, REPECS_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_REP_EC_STAT,invoke_id,
			M_CAST_MK_FUN (_ms_mk_repecs_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_repecs_resp					*/
/* Construct a report event condition status response.			*/
/************************************************************************/

ST_VOID _ms_mk_repecs_resp (ASN1_ENC_CTXT *aCtx, REPECS_RESP_INFO *info)
  {
  if (info->tti_time_pres)
    { 					/* write tran to idle time	*/
    asn1r_strt_constr (aCtx);			/* start ctx tag 4 constructor	*/
    _ms_wr_mms_evtime (aCtx, &info->tti_time);
    asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* finish ctx tag 4 constructor */
    }

  if (info->tta_time_pres)
    { 					/* write tran to active time	*/
    asn1r_strt_constr (aCtx);			/* start ctx tag 3 constructor	*/
    _ms_wr_mms_evtime (aCtx, &info->tta_time);
    asn1r_fin_constr (aCtx, 3,CTX,DEF);		/* finish ctx tag 3 constructor */
    }

  if (info->enabled_pres)
    { 					/* write enabled indicator	*/
    asn1r_wr_bool (aCtx, info->enabled);
    asn1r_fin_prim (aCtx, 2,CTX);			/* finish ctx tag 2 		*/
    }

  asn1r_wr_u32 (aCtx, info->num_of_ev_enroll); 	/* write # of event enrollments	*/
  asn1r_fin_prim (aCtx, 1, CTX);			/* ctx tag 1			*/

  asn1r_wr_i16 (aCtx, info->cur_state);
  asn1r_fin_prim (aCtx, 0, CTX);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_EC_STAT,CTX,DEF);	/* tag = opcode, ctx		*/
  }
