/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_geteaa.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the get event	*/
/*	action attributes. It decodes the get event action attributes	*/
/*	request (indication) and encodes the get event action 		*/
/*	attributes response.						*/
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

static  GETEAA_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID geteaa_objname_done  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_geteaa_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_geteaa_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Event Action Attributes Request");

/* get storage for request information	*/
  req_info = (GETEAA_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETEAA_REQ_INFO));
  _ms_get_mms_objname (aCtx, &req_info->evact_name, geteaa_objname_done);
  }

/************************************************************************/
/*			geteaa_objname_done				*/
/* Decoding get event action attributes request: event action name 	*/
/* has been obtained.							*/
/************************************************************************/

static ST_VOID geteaa_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_geteaa_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get event action attributes response	*/
/************************************************************************/

ST_RET mp_geteaa_resp (MMSREQ_IND *indptr, GETEAA_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_EA_ATTR,indptr,
			M_CAST_MK_FUN (_ms_mk_geteaa_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_geteaa_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get event action attributes response.		*/
/************************************************************************/

ST_RET mpl_geteaa_resp (ST_UINT32 invoke_id, GETEAA_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_EA_ATTR,invoke_id,
			M_CAST_MK_FUN (_ms_mk_geteaa_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_geteaa_resp					*/
/* Construct a get event action attributes response.			*/
/************************************************************************/

ST_VOID _ms_mk_geteaa_resp (ASN1_ENC_CTXT *aCtx, GETEAA_RESP_INFO *info)
  {
#ifdef MOD_SUPPORT
MODIFIER *mod_ptr;

  mod_ptr = (MODIFIER *) (info + 1);
#endif

#ifdef CS_SUPPORT		/* if companion standards are supported	*/
  if (info->cs_rdetail_pres)	/*   check for CS request detail	*/
    {		       
    asn1r_strt_constr (aCtx);			/* start ctx tag 79 constructor	*/
    asn1r_wr_delmnt (aCtx, info->cs_rdetail, info->cs_rdetail_len);
    asn1r_fin_constr (aCtx, 79, CTX, DEF);		/* CS is cstr 79		*/
    }
#endif

  asn1r_strt_constr (aCtx);			/* start ctx tag 2 constructor	*/
  asn1r_wr_delmnt (aCtx, info->conf_serv_req,info->conf_serv_req_len);
  asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish ctx tag 2 constructor */

  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
#ifdef MOD_SUPPORT			/* don't write if not supported	*/
  _ms_wr_mms_modlist (aCtx, mod_ptr, info->num_of_modifiers);
#endif
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish ctx tag 1 constructor */

  if (info->mms_deletable)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->mms_deletable); 	/* write mms deletable indicator*/
    asn1r_fin_prim (aCtx, 0, CTX);			/* ctx tag 0			*/
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_EA_ATTR,CTX,DEF);	/* tag = opcode, ctx		*/
  }
