/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_getvlist.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the get named	*/
/*	variable list attributes.  It decodes the get named variable	*/
/*	list attributes response (confirm) & encodes the get named 	*/
/*	variable list attributes request.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
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
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETVLIST_RESP_INFO	*rsp_info;

static VARIABLE_LIST  *vl_ptr;
static ST_INT max_vars;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getvlist_list_of_var_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvlist_get_varlist (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvlist_get_mms_deletable (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_get_vlist_rsp 				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_vlist_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("Get Named Variable List Attributes Response");

#if defined(FIXED_DECBUF)
  max_vars = m_cl_max_getvla_vars;
  rsp_size = mmsl_dec_info_size;
#else
  /* find maximum storage required	*/
  max_vars     = _mmsdec_msglen / 4;
  if (m_cl_max_getvla_vars && (max_vars > m_cl_max_getvla_vars))
    max_vars = m_cl_max_getvla_vars;

  rsp_size = sizeof (GETVLIST_RESP_INFO)
	     + (sizeof (VARIABLE_LIST) * max_vars);
#endif

/* get storage for request information	*/
  rsp_info = (GETVLIST_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  vl_ptr   = (VARIABLE_LIST *) (rsp_info + 1);

  ASN1R_TAG_ADD (aCtx, CTX, 0, getvlist_get_mms_deletable);
  }

/************************************************************************/
/*			getvlist_get_mms_deletable			*/
/* Decoding get named variable list attributes response: mms deletable	*/
/* was encountered. 							*/
/************************************************************************/

static ST_VOID getvlist_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvlist_get_mms_deletable");
					/* read mms deletable		*/
  if (asn1r_get_bool (aCtx, &(rsp_info->mms_deletable)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, getvlist_get_varlist);
  }

/************************************************************************/
/*			getvlist_get_varlist				*/
/* Decoding get named variable list attributes response: list of 	*/
/* variables constructor was encountered.   				*/
/************************************************************************/

static ST_VOID getvlist_get_varlist (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvlist_get_varlist");

  _ms_get_va_var_list (aCtx, vl_ptr, &rsp_info->num_of_variables,
  			max_vars, getvlist_list_of_var_done);
  }

/************************************************************************/
/*			getvlist_list_of_var_done			*/
/* get named variable list attributes request: list of variables has	*/
/* been obtained. 							*/
/************************************************************************/

static ST_VOID getvlist_list_of_var_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 rsp_size;

  MLOG_CDEC0 ("getvlist_list_of_var_done");
				/* find actual size for request info	*/
  rsp_size = sizeof (GETVLIST_RESP_INFO)
	     + (sizeof (VARIABLE_LIST) * rsp_info->num_of_variables);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (GETVLIST_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getvlist					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the get named variable list attributes	*/
/* request.								*/
/************************************************************************/

MMSREQ_PEND *mp_getvlist (ST_INT chan, GETVLIST_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_VLIST,
			M_CAST_MK_FUN (_ms_mk_getvlist_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getvlist					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get named variable list attributes request.	*/
/************************************************************************/

ST_RET mpl_getvlist (GETVLIST_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_VLIST,
			M_CAST_MK_FUN (_ms_mk_getvlist_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getvlist_req					*/
/* construct a get named variable list attributes request		*/
/************************************************************************/

ST_VOID _ms_mk_getvlist_req (ASN1_ENC_CTXT *aCtx, GETVLIST_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->vl_name);	/* write named var list name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_VLIST,CTX,DEF); 	/* tag = opcode, ctx		*/
  }
