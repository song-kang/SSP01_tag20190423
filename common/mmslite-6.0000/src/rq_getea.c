/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_geteaa.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the get event	*/
/*	action attributes.  It decodes the get event action attributes	*/
/*	response (confirm) & encodes the get event action attributes 	*/
/*	request. 							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     06    Converted to use ASN1R (re-entrant ASN1)	*/
/* 04/28/00  JRB     05    Lint cleanup					*/
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
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETEAA_RESP_INFO	*rsp_info;

#ifdef MOD_SUPPORT
static MODIFIER      *mod_ptr;
static ST_INT max_modifiers;
#endif

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID geteaa_get_mms_deletable       (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteaa_get_modlist             (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteaa_modlist_done            (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteaa_conf_serv_req_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteaa_conf_serv_req_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteaa_conf_serv_req_cstr_done (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID geteaa_cs_rdetail_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteaa_cs_rdetail_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteaa_cs_rdetail_cstr_done    (ASN1_DEC_CTXT *aCtx);
#endif


/************************************************************************/
/************************************************************************/
/*			mms_geteaa_rsp 					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_geteaa_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("Get Event Action Attributes Response");
				/* find maximum size for response info	*/

#if defined(FIXED_DECBUF)
  rsp_size = sizeof (GETEAA_RESP_INFO);
#else
#ifdef MOD_SUPPORT
  max_modifiers = _mmsdec_msglen / 2;
  rsp_size = sizeof (GETEAA_RESP_INFO)
		+ (sizeof (MODIFIER) * max_modifiers);
  mod_ptr  = (MODIFIER *) (rsp_info + 1);
#else
  rsp_size = sizeof (GETEAA_RESP_INFO);
#endif
#endif

/* get storage for request information	*/
  rsp_info = (GETEAA_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);

  ASN1R_TAG_ADD (aCtx, CTX,          0, geteaa_get_mms_deletable);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, geteaa_get_modlist);
  }

/************************************************************************/
/*			geteaa_get_mms_deletable			*/
/* Decoding get event action attributes response: mms deletable was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID geteaa_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteaa_get_mms_deletable");
					/* read mms deletable		*/
  if (asn1r_get_bool (aCtx, &(rsp_info->mms_deletable)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, geteaa_get_modlist);
  }

/************************************************************************/
/*			geteaa_get_modlist				*/
/* Decoding get event action attributes response: list of modifiers	*/
/* constructor was encountered.	       					*/
/************************************************************************/

static ST_VOID geteaa_get_modlist (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteaa_get_modlist");

#ifdef MOD_SUPPORT
  _ms_get_mms_modlist (aCtx, mod_ptr, &rsp_info->num_of_modifiers,
  			max_modifiers, geteaa_modlist_done);
#else				/* must be empty if not supported	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteaa_modlist_done;
#endif
  }

/************************************************************************/
/*			geteaa_modlist_done				*/
/* Decoding get event action attributes response: list of modifiers 	*/
/* constructor done was encountered. 	  				*/
/************************************************************************/

static ST_VOID geteaa_modlist_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteaa_modlist_done");

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, geteaa_conf_serv_req_cstr);
  }

/************************************************************************/
/*			geteaa_conf_serv_req_cstr  			*/
/* Decoding get event action attributes response: confirmed service 	*/
/* request constructor was encountered.	  				*/
/************************************************************************/

static ST_VOID geteaa_conf_serv_req_cstr (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("geteaa_conf_serv_req_cstr");
				/* find actual size for response info	*/
  rsp_size = sizeof (GETEAA_RESP_INFO)
		+ (sizeof (MODIFIER)*rsp_info->num_of_modifiers);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr, rsp_size);
  rsp_info = (GETEAA_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;

				/* get the confirmed service request	*/
  rsp_info->conf_serv_req = aCtx->asn1r_field_ptr;

  asn1r_parse_next (aCtx, geteaa_conf_serv_req_done);	/* parse the contents	*/
  }

/************************************************************************/
/*			geteaa_conf_serv_req_done    			*/
/* Decoding get event action attributes response: confirmed service  	*/
/* request has been obtained.	       					*/
/************************************************************************/

static ST_VOID geteaa_conf_serv_req_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteaa_conf_serv_req_done");

  rsp_info->conf_serv_req_len = aCtx->asn1r_octetcount;	/* save the length	*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteaa_conf_serv_req_cstr_done;
  }

/************************************************************************/
/*			geteaa_conf_serv_req_cstr_done      		*/
/* Decoding get event action attributes response: confirmed service  	*/
/* request constructor done was encountered.				*/
/************************************************************************/

static ST_VOID geteaa_conf_serv_req_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 79, geteaa_cs_rdetail_cstr);
#endif

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#ifdef CS_SUPPORT
/************************************************************************/
/*			geteaa_cs_rdetail_cstr				*/
/* Decoding get event action attributes response: companion standard	*/
/* request detail was encountered.					*/
/************************************************************************/

static ST_VOID geteaa_cs_rdetail_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteaa_cs_rdetail_cstr");

  rsp_info->cs_rdetail_pres = SD_TRUE;		/* set flag		*/

  rsp_info->cs_rdetail = aCtx->asn1r_field_ptr;	/* get the CS request detail	*/
  asn1r_parse_next (aCtx, geteaa_cs_rdetail_done);	/* parse the contents		*/
  }

/************************************************************************/
/*			geteaa_cs_rdetail_done				*/
/************************************************************************/

static ST_VOID geteaa_cs_rdetail_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteaa_cs_rdetail_done");

  rsp_info->cs_rdetail_len = aCtx->asn1r_octetcount;	/* save the length 	*/
  
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteaa_cs_rdetail_cstr_done;
  }

/************************************************************************/
/*			geteaa_cs_rdetail_cstr_done			*/
/* event notification request is being decoded: action result success:	*/
/* confirmed service response cstr done has been encountered.		*/
/************************************************************************/

static ST_VOID geteaa_cs_rdetail_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }
#endif


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_geteaa					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the get event action attributes request	*/
/************************************************************************/

MMSREQ_PEND *mp_geteaa (ST_INT chan, GETEAA_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_EA_ATTR,
			M_CAST_MK_FUN (_ms_mk_geteaa_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_geteaa					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get event action attributes request.		*/
/************************************************************************/

ST_RET  mpl_geteaa (GETEAA_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_EA_ATTR,
			M_CAST_MK_FUN (_ms_mk_geteaa_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_geteaa_req					*/
/* construct a get event action attributes request			*/
/************************************************************************/

ST_VOID _ms_mk_geteaa_req (ASN1_ENC_CTXT *aCtx, GETEAA_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->evact_name);	/* write event action name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_EA_ATTR,CTX,DEF); 	/* tag = opcode, ctx		*/
  }
