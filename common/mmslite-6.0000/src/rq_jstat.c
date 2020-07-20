/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_jstat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requestor portion of report journal	*/
/*	status. It decodes the report journal status response (confirm) */
/*	and encodes the report journal status request.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
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

static  JSTAT_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID jstat_get_deletable (ASN1_DEC_CTXT *aCtx);
static ST_VOID jstat_get_cur_entries (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_jstat_rsp					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_jstat_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Journal Status Response");

/* get storage for request information	*/
  rsp_info = (JSTAT_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (JSTAT_RESP_INFO));

  ASN1R_TAG_ADD (aCtx, CTX, 0, jstat_get_cur_entries);
  }

/************************************************************************/
/*			jstat_get_cur_entries				*/
/* report journal status response is being decoded:  current entries	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jstat_get_cur_entries (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report journal status response");

  if (asn1r_get_u32 (aCtx, &rsp_info->cur_entries))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 1, jstat_get_deletable); /* deletable is next		*/
  }

/************************************************************************/
/*			jstat_get_deletable				*/
/* report journal status response is being decoded:  current entries	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jstat_get_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jstat_get_deletable");

  if (asn1r_get_bool (aCtx, &rsp_info->mms_deletable))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jstat					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report journal status request.	*/
/************************************************************************/

MMSREQ_PEND *mp_jstat (ST_INT chan, JSTAT_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_STAT_JOURNAL,
			M_CAST_MK_FUN (_ms_mk_jstat_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jstat					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report journal status request.			*/
/************************************************************************/

ST_RET  mpl_jstat (JSTAT_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_STAT_JOURNAL,
			M_CAST_MK_FUN (_ms_mk_jstat_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_jstat_req					*/
/* ENCODE a report journal status REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_jstat_req (ASN1_ENC_CTXT *aCtx, JSTAT_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->jou_name);	/* write the journal name	*/
/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_STAT_JOURNAL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
