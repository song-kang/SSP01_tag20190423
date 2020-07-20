/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_jinit.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requestor portion of initialize	*/
/*	journal.  It decodes the initialize journal response (confirm)	*/
/*	and encodes the initialize journal request.			*/
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
/* 08/15/97  MDE     02    BTOD handling changes			*/
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

static  JINIT_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_jinit_rsp					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_jinit_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Initialize Journal Response");


/* get storage for request information	*/
  rsp_info = (JINIT_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (JINIT_RESP_INFO));
  if (asn1r_get_u32 (aCtx, &rsp_info->del_entries))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jinit					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the initialize journal request.		*/
/************************************************************************/

MMSREQ_PEND *mp_jinit (ST_INT chan, JINIT_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_INIT_JOURNAL,
			M_CAST_MK_FUN (_ms_mk_jinit_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jinit					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the initialize journal request.			*/
/************************************************************************/

ST_RET  mpl_jinit (JINIT_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_INIT_JOURNAL,
			M_CAST_MK_FUN (_ms_mk_jinit_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_jinit_req					*/
/* ENCODE an initialize journal REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_jinit_req (ASN1_ENC_CTXT *aCtx, JINIT_REQ_INFO *info)
  {

  if (info->limit_spec_pres)
    {					/* write limit specification	*/
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    if (info->limit_entry_pres)
      {					/* write limiting entry		*/
      asn1r_wr_octstr (aCtx, info->limit_entry, info->limit_entry_len);
      asn1r_fin_prim (aCtx, 1,CTX);
      }
    asn1r_wr_btod (aCtx, &info->limit_time);
    asn1r_fin_prim (aCtx, 0,CTX);
    asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish context tag 1 cstr	*/
    }

  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  _ms_wr_mms_objname (aCtx, &info->jou_name);	/* write the journal name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish context tag 0 cstr	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_INIT_JOURNAL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
