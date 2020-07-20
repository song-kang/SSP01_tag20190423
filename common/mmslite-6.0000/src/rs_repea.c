/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_repeas.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the report event	*/
/*	action status. It decodes the report event action status	*/
/*	request (indication) and encodes the report event action  	*/
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

static  REPEAS_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID repeas_objname_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_repeas_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_repeas_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Event Action Status Request");

/* get storage for request information	*/
  req_info = (REPEAS_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (REPEAS_REQ_INFO));
  aCtx->asn1r_err_fun = _mms_dec_buf_free;
  _ms_get_mms_objname (aCtx, &req_info->evact_name, repeas_objname_done);
  }

/************************************************************************/
/*			repeas_objname_done				*/
/* Decoding get event action attributes request: event action name 	*/
/* has been obtained.							*/
/************************************************************************/

static ST_VOID repeas_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_repeas_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report event action status response	*/
/************************************************************************/

ST_RET mp_repeas_resp (MMSREQ_IND *indptr, REPEAS_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_REP_EA_STAT,indptr,
			M_CAST_MK_FUN (_ms_mk_repeas_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_repeas_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report event action status response.		*/
/************************************************************************/

ST_RET mpl_repeas_resp (ST_UINT32 invoke_id, REPEAS_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_REP_EA_STAT,invoke_id,
			M_CAST_MK_FUN (_ms_mk_repeas_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_repeas_resp					*/
/* Construct a get event action attributes response.			*/
/************************************************************************/

ST_VOID _ms_mk_repeas_resp (ASN1_ENC_CTXT *aCtx, REPEAS_RESP_INFO *info)
  {
  asn1r_wr_u32 (aCtx, info->num_of_ev_enroll); 	/* write # of event enrollments */

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_REP_EA_STAT,CTX);		/* tag = opcode, ctx		*/
  }
