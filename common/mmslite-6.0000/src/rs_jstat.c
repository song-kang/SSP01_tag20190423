/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_jstat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of report journal	*/
/*	status. It decodes the report journal status request		*/
/*	(indication) & encodes the report journal status response.	*/
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
#include "mms_pjou.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  JSTAT_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID jstat_jn_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_jstat_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_jstat_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Journal Status Request");


/* get storage for request information	*/
  req_info = (JSTAT_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (JSTAT_REQ_INFO));
  _ms_get_mms_objname (aCtx, &req_info->jou_name, jstat_jn_done);
  }

/************************************************************************/
/*			jstat_jn_done					*/
/* report journal status request is being decoded: journal name has	*/
/* been obtained.							*/
/************************************************************************/

static ST_VOID jstat_jn_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jstat_jn_done");

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jstat_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report journal status response.	*/
/************************************************************************/

ST_RET mp_jstat_resp (MMSREQ_IND *indptr, JSTAT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_STAT_JOURNAL,indptr,
			M_CAST_MK_FUN (_ms_mk_jstat_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jstat_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report journal status response.		*/
/************************************************************************/

ST_RET mpl_jstat_resp (ST_UINT32 invoke_id, JSTAT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_STAT_JOURNAL,invoke_id,
			M_CAST_MK_FUN (_ms_mk_jstat_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_jstat_resp					*/
/* ENCODE a report journal status RESPONSE:				*/
/************************************************************************/

ST_VOID _ms_mk_jstat_resp (ASN1_ENC_CTXT *aCtx, JSTAT_RESP_INFO *info_ptr)
  {
  asn1r_wr_bool (aCtx, info_ptr->mms_deletable);   	/* write the deletable param	*/
  asn1r_fin_prim (aCtx, 1,CTX);

  asn1r_wr_u32 (aCtx, info_ptr->cur_entries);	/* encode current entries	*/
  asn1r_fin_prim (aCtx, 0,CTX);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_STAT_JOURNAL,CTX,DEF);	/* tag = opcode, ctx		*/
  }
