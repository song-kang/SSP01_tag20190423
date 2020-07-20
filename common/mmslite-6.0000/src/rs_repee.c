/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_repees.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the report event	*/
/*	enrollment status. It decodes the report event enrollment	*/
/*	status request (indication) and encodes the report event 	*/
/*	enrollment status response.    					*/
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

static  REPEES_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID repees_objname_done  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_repees_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_repees_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Event Enrollment Status Request");
  req_info = (REPEES_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (REPEES_REQ_INFO));
  _ms_get_mms_objname (aCtx, &req_info->evenroll_name, repees_objname_done);
  }

/************************************************************************/
/*			repees_objname_done				*/
/* Decoding report event enrollment status request: event enrollment  	*/
/* name has been obtained.   						*/
/************************************************************************/

static ST_VOID repees_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_repees_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the report event enrollment status response	*/
/************************************************************************/

ST_RET mp_repees_resp (MMSREQ_IND *indptr, REPEES_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_REP_EE_STAT,indptr,
			M_CAST_MK_FUN (_ms_mk_repees_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_repees_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report event enrollment status response.	*/
/************************************************************************/

ST_RET mpl_repees_resp (ST_UINT32 invoke_id, REPEES_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_REP_EE_STAT,invoke_id,
			M_CAST_MK_FUN (_ms_mk_repees_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_repees_resp					*/
/* Construct a report event enrollment status response.			*/
/************************************************************************/

ST_VOID _ms_mk_repees_resp (ASN1_ENC_CTXT *aCtx, REPEES_RESP_INFO *info)
  {
  asn1r_wr_i16 (aCtx, info->cur_state); 		/* write current state		*/
  asn1r_fin_prim (aCtx, 4, CTX);			/* ctx tag 4			*/

  if (info->alarm_ack_rule_pres)
    {
    asn1r_wr_i16 (aCtx, info->alarm_ack_rule);   	/* write alarm ack rule		*/
    asn1r_fin_prim (aCtx, 3, CTX);			/* ctx tag 3			*/
    }

  asn1r_wr_i16 (aCtx, info->duration); 		/* write the duration 		*/
  asn1r_fin_prim (aCtx, 2, CTX);			/* ctx tag 2			*/

  if (info->not_lost)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->not_lost);	 	/* write notification lost	*/
    asn1r_fin_prim (aCtx, 1, CTX);			/* ctx tag 1			*/
    }

  asn1r_wr_bitstr (aCtx, &info->ec_transitions, 7);
  asn1r_fin_prim (aCtx, 0, CTX);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_EE_STAT,CTX,DEF);	/* tag = opcode, ctx		*/
  }
