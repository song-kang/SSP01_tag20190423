/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_delvlist.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of delete named	*/
/*	variable list.  It decodes the delete named variable list 	*/
/*	request (indication) and encodes the delete named variable	*/
/*	list response.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_del_vlist_req 				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_del_vlist_req (ASN1_DEC_CTXT *aCtx)
  {
  mms_del_type_req (aCtx);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_delvlist_resp		   		*/
/* Create and send a delete named variable list response.  		*/
/************************************************************************/

ST_RET mp_delvlist_resp (MMSREQ_IND *indptr, DELVLIST_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_DEL_VLIST,indptr,
			M_CAST_MK_FUN (_ms_mk_delvlist_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_delvlist_resp		   		*/
/* Create a delete named variable list response.  			*/
/************************************************************************/

ST_RET mpl_delvlist_resp (ST_UINT32  invoke_id, 
			  DELVLIST_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_DEL_VLIST,invoke_id,
			M_CAST_MK_FUN (_ms_mk_delvlist_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_delvlist_resp  				*/
/* construct a delete named variable list response. 			*/
/************************************************************************/

ST_VOID _ms_mk_delvlist_resp (ASN1_ENC_CTXT *aCtx, DELVLIST_RESP_INFO *info)
  {
  asn1r_wr_u32 (aCtx, info->num_deleted);		/* write # deleted		*/
  asn1r_fin_prim (aCtx, 1,CTX);

  asn1r_wr_u32 (aCtx, info->num_matched);		/* write # matched		*/
  asn1r_fin_prim (aCtx, 0,CTX);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEL_VLIST,CTX,DEF);     	/* tag = opcode, ctx constr	*/
  }
