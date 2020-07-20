/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_defea.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of define event	*/
/*	action.  It decodes the define event action request 		*/
/*	(indication) & encodes the define event action response.   	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 04/28/00  JRB     04    Lint cleanup					*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
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

static  DEFEA_REQ_INFO	*req_info;

#ifdef MOD_SUPPORT
static MODIFIER      *mod_ptr;
static ST_INT max_modifiers;
#endif

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/* static functions with multiple references                            */
static ST_VOID defea_name_cstr           (ASN1_DEC_CTXT *aCtx);
static ST_VOID defea_name_done           (ASN1_DEC_CTXT *aCtx);
static ST_VOID defea_name_cstr_done      (ASN1_DEC_CTXT *aCtx);
#ifdef MOD_SUPPORT
static ST_VOID defea_get_modlist         (ASN1_DEC_CTXT *aCtx);
static ST_VOID defea_modlist_done        (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID defea_get_conf_serv_req   (ASN1_DEC_CTXT *aCtx);
static ST_VOID defea_conf_serv_req_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID defea_conf_serv_cstr_done (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID defea_cs_rdetail_cstr     (ASN1_DEC_CTXT *aCtx);
static ST_VOID defea_cs_rdetail_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID defea_cs_rdetail_cstr_done (ASN1_DEC_CTXT *aCtx);
#endif

/************************************************************************/
/************************************************************************/
/*			mms_defea_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_defea_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT  req_size;

  MLOG_CDEC0 ("Define Event Action Request");
				/* find maximum size for request info	*/

#if defined(FIXED_DECBUF)
  max_modifiers = 0;
  req_size = sizeof (DEFEA_REQ_INFO);
#else
#ifdef MOD_SUPPORT
  max_modifiers = _mmsdec_msglen / 2;
  req_size = sizeof (DEFEA_REQ_INFO)
		+ (sizeof (MODIFIER) * max_modifiers);
#else
  req_size = sizeof (DEFEA_REQ_INFO);
#endif
#endif

/* get storage for request information	*/
  req_info = (DEFEA_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);


/*  req_info->modlist_pres  = SD_FALSE;  		set default values	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, defea_name_cstr);
  }

/************************************************************************/
/*			defea_name_cstr					*/
/* define event action request is being decoded: event action name	*/
/* constructor was encountered. 					*/
/************************************************************************/

static ST_VOID defea_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defea_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evact_name, defea_name_done);
  }

/************************************************************************/
/*			defea_name_done					*/
/* define event action request is being decoded: event action name	*/
/* has been obtained.		 					*/
/************************************************************************/

static ST_VOID defea_name_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defea_name_cstr_done;
  }

/************************************************************************/
/*			defea_name_cstr_done				*/
/* define event action request is being decoded: event action name	*/
/* constructor done was encountered. 					*/
/************************************************************************/

static ST_VOID defea_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

#ifdef MOD_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, defea_get_modlist);
#endif
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, defea_get_conf_serv_req);
#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,79, defea_cs_rdetail_cstr);
#endif

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#ifdef MOD_SUPPORT
/************************************************************************/
/*			defea_get_modlist				*/
/* Decoding define event action request: list of modifiers constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID defea_get_modlist (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defea_get_modlist");

  req_info->modlist_pres = SD_TRUE;   	/* list of modifiers present	*/

  mod_ptr  = (MODIFIER *) (req_info + 1);

  _ms_get_mms_modlist (aCtx, mod_ptr, &req_info->num_of_modifiers,
  			max_modifiers, defea_modlist_done);
  }

/************************************************************************/
/*			defea_modlist_done				*/
/* Decoding define event action request: list of modifiers constructor	*/
/* done was encountered. 						*/
/************************************************************************/

static ST_VOID defea_modlist_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defea_modlist_done");

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, defea_get_conf_serv_req);
#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,79, defea_cs_rdetail_cstr);
#endif

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }
#endif

/************************************************************************/
/*			defea_get_conf_serv_req				*/
/* Decoding define event action request: confirmed service request 	*/
/* constructor was encountered.	     					*/
/************************************************************************/

static ST_VOID defea_get_conf_serv_req (ASN1_DEC_CTXT *aCtx)
  {
#ifdef MOD_SUPPORT
ST_INT req_size;
#endif

  MLOG_CDEC0 ("defea_get_conf_serv_req");
				/* find actual size for request info	*/
#ifdef MOD_SUPPORT
  req_size = sizeof (DEFEA_REQ_INFO)
		+ (sizeof (MODIFIER)*req_info->num_of_modifiers);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr, req_size);
  req_info = (DEFEA_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;
#endif

				/* get the confirmed service request	*/
  req_info->conf_serv_req = aCtx->asn1r_field_ptr;

  asn1r_parse_next (aCtx, defea_conf_serv_req_done);	/* parse the contents	*/
  }

/************************************************************************/
/*			defea_conf_serv_req_done    			*/
/* Decoding define event action request: confirmed service request 	*/
/* has been obtained.		     					*/
/************************************************************************/

static ST_VOID defea_conf_serv_req_done (ASN1_DEC_CTXT *aCtx)
  {
  req_info->conf_serv_req_len = aCtx->asn1r_octetcount;	/* save length		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defea_conf_serv_cstr_done;
  }

/************************************************************************/
/*			defea_conf_serv_cstr_done			*/
/* Decoding define event action request: confirmed service request 	*/
/* constructor done has been encountered.				*/
/************************************************************************/

static ST_VOID defea_conf_serv_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
#ifdef CS_SUPPORT
	    		/* check for Companion Standard request detail	*/
    ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 79, defea_cs_rdetail_cstr);
#endif

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#ifdef CS_SUPPORT
/************************************************************************/
/*			defea_cs_rdetail_cstr				*/
/* Decoding define event action request: companion standard request	*/
/* detail constructor has been encountered.				*/
/************************************************************************/

static ST_VOID defea_cs_rdetail_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defea_cs_rdetail_cstr");

  req_info->cs_rdetail_pres = SD_TRUE;		/* set the present flag	*/

		/* leave CS response detail in place; just parse it. 	*/
  req_info->cs_rdetail = aCtx->asn1r_field_ptr;
  asn1r_parse_next (aCtx, defea_cs_rdetail_done);
  }

/************************************************************************/
/*			defea_cs_rdetail_done				*/
/* event notification request is being decoded: action result success:	*/
/* companion standard response detail has been parsed.  		*/
/************************************************************************/

static ST_VOID defea_cs_rdetail_done (ASN1_DEC_CTXT *aCtx)
  {
  req_info->cs_rdetail_len = aCtx->asn1r_octetcount;	/* save length		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defea_cs_rdetail_cstr_done;
  }

/************************************************************************/
/*			defea_cs_rdetail_cstr_done			*/
/* event notification request is being decoded: action result success:	*/
/* confirmed service response cstr done has been encountered.		*/
/************************************************************************/

static ST_VOID defea_cs_rdetail_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }
#endif


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defea_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the define event action response.    	*/
/************************************************************************/

ST_RET mp_defea_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DEF_EA));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defea_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define event action response.    		*/
/************************************************************************/

ST_RET mpl_defea_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DEF_EA));
  }
#endif
