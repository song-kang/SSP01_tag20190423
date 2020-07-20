/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_rsstat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of report semaphore	*/
/*	status.  It decodes the report semaphore status request 	*/
/*	(indication) and encodes the report semaphore status response.	*/
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
#include "mms_psem.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  RSSTAT_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID rsstat_objname_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_rsstat_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_rsstat_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Semaphore Status Request");


				/* get storage for request information	*/
  req_info = (RSSTAT_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (RSSTAT_REQ_INFO));
  _ms_get_mms_objname (aCtx, &req_info->sem_name, rsstat_objname_done);
  }

/************************************************************************/
/*			rsstat_objname_done				*/
/* Decoding report semaphore status request: semaphore name obtained	*/
/************************************************************************/

static ST_VOID rsstat_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rsstat_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report semaphore status response.	*/
/************************************************************************/

ST_RET mp_rsstat_resp (MMSREQ_IND *indptr, RSSTAT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_REP_SEMSTAT,indptr,
			M_CAST_MK_FUN (_ms_mk_rsstat_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rsstat_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report semaphore status response.		*/
/************************************************************************/

ST_RET mpl_rsstat_resp (ST_UINT32 invoke_id, RSSTAT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_REP_SEMSTAT,invoke_id,
			M_CAST_MK_FUN (_ms_mk_rsstat_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rsstat_resp					*/
/* Construct a report semaphore status response.			*/
/************************************************************************/

ST_VOID _ms_mk_rsstat_resp (ASN1_ENC_CTXT *aCtx, RSSTAT_RESP_INFO *info)
  {
  asn1r_strt_constr (aCtx);			/* start the sequence constr	*/

  asn1r_wr_u16 (aCtx, info->num_of_hung);		/* write number of hung tokens	*/
  asn1r_fin_prim (aCtx, 4,CTX);

  asn1r_wr_u16 (aCtx, info->num_of_owned);		/* write number of owned tokens */
  asn1r_fin_prim (aCtx, 3,CTX);

  asn1r_wr_u16 (aCtx, info->num_of_tokens); 	/* write number of tokens	*/
  asn1r_fin_prim (aCtx, 2,CTX);

  asn1r_wr_i16 (aCtx, info->tclass); 		/* write the class		*/
  asn1r_fin_prim (aCtx, 1,CTX);

  asn1r_wr_bool (aCtx, info->mms_deletable);   	/* write the mms deletable	*/
  asn1r_fin_prim (aCtx, 0,CTX);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_SEMSTAT,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
