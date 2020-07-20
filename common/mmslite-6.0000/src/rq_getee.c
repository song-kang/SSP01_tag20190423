/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_geteea.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the get event	*/
/*	enrollment attributes.  It decodes the get event enrollment	*/
/*	attributes response (confirm) & encodes the get event		*/
/*	enrollment attributes request. 	   				*/
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

static  GETEEA_RESP_INFO	*rsp_info;

static EVENT_ENROLLMENT      *ee_ptr;
static ST_INT max_ev_enrolls;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID _g_eelist_cstr              (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_evenroll_cstr            (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eename_cstr              (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eename_done              (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eename_cstr_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_ecname_cstr              (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_ecname_objname           (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_ecname_objname_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_ecname_objname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_ecname_undefined         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_ecname_cstr_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eaname_cstr              (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eaname_objname           (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eaname_objname_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eaname_objname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eaname_undefined         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eaname_cstr_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_client_app_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_app_ref_done             (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_client_app_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_get_mms_deletable        (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_get_ee_class             (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_get_duration             (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_get_invoke               (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_get_rem_acc_delay        (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID _g_addl_detail_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_addl_detail_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_addl_detail_cstr_done    (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID _g_aecname_cstr             (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_aecname_objname          (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_aecname_objname_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_aecname_objname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_aecname_undefined        (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_aecname_cstr_done        (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_evenroll_cstr_done       (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_eelist_cstr_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_get_more_follows         (ASN1_DEC_CTXT *aCtx);
static ST_VOID _g_dec_done                 (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_geteea_rsp 					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_geteea_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT  rsp_size;

  MLOG_CDEC0 ("Get Event Enrollment Attributes Response");

#if defined(FIXED_DECBUF)
  max_ev_enrolls = m_cl_max_event_enrollment;
  rsp_size = mmsl_dec_info_size;
#else
  max_ev_enrolls = _mmsdec_msglen / 2; /* find maximum size for response info	*/
  if (m_cl_max_event_enrollment && (max_ev_enrolls > m_cl_max_event_enrollment))
    max_ev_enrolls = m_cl_max_event_enrollment;

/* get storage for request information	*/
  rsp_size = sizeof (GETEEA_RESP_INFO)
		+ (sizeof (EVENT_ENROLLMENT) * max_ev_enrolls);
#endif

/* get storage for request information	*/
  rsp_info = (GETEEA_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  ee_ptr  = (EVENT_ENROLLMENT *) (rsp_info + 1);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, _g_eelist_cstr);
  }

/************************************************************************/
/*			_g_eelist_cstr				*/
/* Decoding get event enrollment attributes response: list of event	*/
/* enrollment names constructor was encountered.  			*/
/************************************************************************/

static ST_VOID _g_eelist_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_eelist_cstr");

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, _g_evenroll_cstr);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_eelist_cstr_done;
  }

/************************************************************************/
/*			_g_evenroll_cstr	  			*/
/* decoding get event enrollment attributes response: event enrollment	*/
/* constructor was encountered.						*/
/************************************************************************/

static ST_VOID _g_evenroll_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_evenroll_cstr");

  rsp_info->num_of_evenroll++;		/* inc # of event enrollments	*/
  if (rsp_info->num_of_evenroll > max_ev_enrolls)
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  else
    ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, _g_eename_cstr);
  }

/************************************************************************/
/*			_g_eename_cstr				*/
/* decoding get event enrollment attributes response: event enrollment	*/
/* name constructor was encountered.   					*/
/************************************************************************/

static ST_VOID _g_eename_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_eename_cstr");

  _ms_get_mms_objname (aCtx, &ee_ptr->evenroll_name, _g_eename_done);
  }

/************************************************************************/
/*			_g_eename_done				*/
/* decoding get event enrollment attributes response: event enrollment	*/
/* name has been decoded.	    					*/
/************************************************************************/

static ST_VOID _g_eename_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_eename_done");
					/* set the cstr done function	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_eename_cstr_done;
  }

/************************************************************************/
/*			_g_eename_cstr_done				*/
/* decoding get event enrollment attributes response: event enrollment	*/
/* name constructor done was encountered.  				*/
/************************************************************************/

static ST_VOID _g_eename_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, _g_ecname_cstr);
  }

/************************************************************************/
/*			_g_ecname_cstr				*/
/* decoding get event enrollment attributes response: event condition	*/
/* name constructor was encountered    					*/
/************************************************************************/

static ST_VOID _g_ecname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_ecname_cstr");

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, _g_ecname_objname);
  ASN1R_TAG_ADD (aCtx, CTX,          1, _g_ecname_undefined);
  }

/************************************************************************/
/*			_g_ecname_objname	  			*/
/* decoding get event enrollment attributes response: event condition	*/
/* name object name constructor was encountered.   			*/
/************************************************************************/

static ST_VOID _g_ecname_objname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_ecname_objname");

  ee_ptr->evcon_name_tag = 0;		/* set tag to event condition	*/

  _ms_get_mms_objname (aCtx, &ee_ptr->evcon_name, _g_ecname_objname_done);
  }

/************************************************************************/
/*			_g_ecname_objname_done     			*/
/* decoding get event enrollment attributes response: event condition	*/
/* name object name has been obtained.	 				*/
/************************************************************************/

static ST_VOID _g_ecname_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_ecname_objname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_ecname_objname_cstr_done;
  }

/************************************************************************/
/*			_g_ecname_objname_cstr_done   		*/
/* decoding get event enrollment attributes response: event condition	*/
/* name object name constructor done was encountered.			*/
/************************************************************************/

static ST_VOID _g_ecname_objname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_ecname_cstr_done;
  }

/************************************************************************/
/*			_g_ecname_undefined	  			*/
/* decoding get event enrollment attributes response: event condition	*/
/* name undefined primitive was encountered. 	  			*/
/************************************************************************/

static ST_VOID _g_ecname_undefined (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_ecname_undefined");

  ee_ptr->evcon_name_tag = 1;		/* set tag to undefined		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_ecname_cstr_done;
  }

/************************************************************************/
/*			_g_ecname_cstr_done				*/
/* decoding get event enrollment attributes response: event condition 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID _g_ecname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, _g_eaname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, _g_client_app_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          4, _g_get_mms_deletable);
  ASN1R_TAG_ADD (aCtx, CTX,          5, _g_get_ee_class);
  }

/************************************************************************/
/*			_g_eaname_cstr 				*/
/* decoding get event enrollment attributes response: event action	*/
/* name constructor was encountered    					*/
/************************************************************************/

static ST_VOID _g_eaname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_eaname_cstr");

  ee_ptr->evact_name_pres = SD_TRUE;	/* event action name present	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, _g_eaname_objname);
  ASN1R_TAG_ADD (aCtx, CTX,          1, _g_eaname_undefined);
  }

/************************************************************************/
/*			_g_eaname_objname	  			*/
/* decoding get event enrollment attributes response: event action	*/
/* name object name constructor was encountered.   			*/
/************************************************************************/

static ST_VOID _g_eaname_objname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_eaname_objname");

  ee_ptr->evact_name_tag = 0;		/* set tag to event action	*/

  _ms_get_mms_objname (aCtx, &ee_ptr->evact_name, _g_eaname_objname_done);
  }

/************************************************************************/
/*			_g_eaname_objname_done     			*/
/* decoding get event enrollment attributes response: event action	*/
/* name object name has been obtained.	 				*/
/************************************************************************/

static ST_VOID _g_eaname_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_eaname_objname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_eaname_objname_cstr_done;
  }

/************************************************************************/
/*			_g_eaname_objname_cstr_done   		*/
/* decoding get event enrollment attributes response: event action	*/
/* name object name constructor done was encountered.			*/
/************************************************************************/

static ST_VOID _g_eaname_objname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_eaname_cstr_done;
  }

/************************************************************************/
/*			_g_eaname_undefined	  			*/
/* decoding get event enrollment attributes response: event action	*/
/* name undefined primitive was encountered. 	  			*/
/************************************************************************/

static ST_VOID _g_eaname_undefined (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_eaname_undefined");

  ee_ptr->evact_name_tag = 1;		/* set tag to undefined		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_eaname_cstr_done;
  }

/************************************************************************/
/*			_g_eaname_cstr_done				*/
/* decoding get event enrollment attributes response: event action 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID _g_eaname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, _g_client_app_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          4, _g_get_mms_deletable);
  ASN1R_TAG_ADD (aCtx, CTX,          5, _g_get_ee_class);
  }

/************************************************************************/
/*			_g_client_app_cstr				*/
/* Decoding get event enrollment attributes response: client application*/
/* constructor was encountered.	 					*/
/************************************************************************/

static ST_VOID _g_client_app_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_client_app_cstr");

  ee_ptr->client_app_pres = SD_TRUE;	/* set client app present	*/

  _ms_get_mms_app_ref (aCtx, &ee_ptr->client_app,&ee_ptr->client_app_len,
					 _g_app_ref_done);
  }

/************************************************************************/
/*			_g_app_ref_done     		       	*/
/* Decoding get event enrollment attributes response: client application*/
/* has been obtained.							*/
/************************************************************************/

static ST_VOID _g_app_ref_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_client_app_cstr_done;
  }

/************************************************************************/
/*			_g_client_app_cstr_done 			*/
/* Decoding get event enrollment attributes response: client application*/
/* constructor done was encountered.  					*/
/************************************************************************/

static ST_VOID _g_client_app_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX, 4, _g_get_mms_deletable);
  ASN1R_TAG_ADD (aCtx, CTX, 5, _g_get_ee_class);
  }

/************************************************************************/
/*			_g_get_mms_deletable			*/
/* Decoding get event enrollment attributes response: mms deletable was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID _g_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_get_mms_deletable");
					/* read mms deletable 		*/
  if (asn1r_get_bool (aCtx, &(ee_ptr->mms_deletable)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 5, _g_get_ee_class);
  }

/************************************************************************/
/*			_g_get_ee_class				*/
/* Decoding get event enrollment attributes response: event enrollment	*/
/* class was encountered.   						*/
/************************************************************************/

static ST_VOID _g_get_ee_class (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_get_ee_class");
					/* read event condition class	*/
  if (asn1r_get_i16 (aCtx, &(ee_ptr->ee_class)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((ee_ptr->ee_class < 0) || (ee_ptr->ee_class > 1))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX,          6, _g_get_duration);
  ASN1R_TAG_ADD (aCtx, CTX,          7, _g_get_invoke);
  ASN1R_TAG_ADD (aCtx, CTX,          8, _g_get_rem_acc_delay);
#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 9, _g_addl_detail_cstr);
#endif
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,10, _g_aecname_cstr);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_evenroll_cstr_done;
  }

/************************************************************************/
/*			_g_get_duration				*/
/* Decoding get event enrollment attributes response: duration was	*/
/* encountered.   							*/
/************************************************************************/

static ST_VOID _g_get_duration (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_get_duration");
					/* read the duration		*/
  if (asn1r_get_i16 (aCtx, &(ee_ptr->duration)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((ee_ptr->duration < 0) || (ee_ptr->duration > 1))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX,          7, _g_get_invoke);
  ASN1R_TAG_ADD (aCtx, CTX,          8, _g_get_rem_acc_delay);
#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 9, _g_addl_detail_cstr);
#endif
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,10, _g_aecname_cstr);
  }

/************************************************************************/
/*			_g_get_invoke				*/
/* Decoding get event enrollment attributes response: invoke was	*/
/* encountered.   							*/
/************************************************************************/

static ST_VOID _g_get_invoke (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_get_invoke");
  
  ee_ptr->invoke_id_pres = SD_TRUE;	/* set invoke id present	*/

					/* read the invoke id		*/
  if (asn1r_get_u32 (aCtx, &(ee_ptr->invoke_id)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX,          8, _g_get_rem_acc_delay);
#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 9, _g_addl_detail_cstr);
#endif
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,10, _g_aecname_cstr);
  }

/************************************************************************/
/*			_g_get_rem_acc_delay     			*/
/* Decoding get event enrollment attributes response: remaining		*/
/* acceptable delay was	encountered. 					*/
/************************************************************************/

static ST_VOID _g_get_rem_acc_delay (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_get_rem_acc_delay");
  
  ee_ptr->rem_acc_delay_pres = SD_TRUE;	/* set rem acc delay  present	*/

					/* read the rem acc delay 	*/
  if (asn1r_get_u32 (aCtx, &(ee_ptr->rem_acc_delay)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 9, _g_addl_detail_cstr);
#endif
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,10, _g_aecname_cstr);
  }
		       
#ifdef CS_SUPPORT
/************************************************************************/
/*			_g_addl_detail_cstr				*/
/* Decoding get event enrollment attributes response: additional detail	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID _g_addl_detail_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_addl_detail_cstr");

  ee_ptr->addl_detail_pres = SD_TRUE;		/* set flag		*/
  ee_ptr->addl_detail      = aCtx->asn1r_field_ptr;

  asn1r_parse_next (aCtx, _g_addl_detail_done);		/* parse the contents	*/
  }

/************************************************************************/
/*			_g_addl_detail_done				*/
/************************************************************************/

static ST_VOID _g_addl_detail_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_addl_detail_done");
  
  ee_ptr->addl_detail_len = aCtx->asn1r_octetcount;	/* save length		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_addl_detail_cstr_done;
  }

/************************************************************************/
/*			_g_addl_detail_cstr_done 			*/
/************************************************************************/

static ST_VOID _g_addl_detail_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,10, _g_aecname_cstr);
  }
#endif

/************************************************************************/
/*			_g_aecname_cstr 				*/
/* decoding get event enrollment attributes response: acknowledgement	*/
/* event condition name constructor was encountered    			*/
/************************************************************************/

static ST_VOID _g_aecname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_aecname_cstr");

  ee_ptr->ackec_name_pres = SD_TRUE;	/* event action name present	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, _g_aecname_objname);
  ASN1R_TAG_ADD (aCtx, CTX,          1, _g_aecname_undefined);
  }

/************************************************************************/
/*			_g_aecname_objname	  			*/
/* decoding get event enrollment attributes response: acknowledgement	*/
/* event condition name object name was encountered.   			*/
/************************************************************************/

static ST_VOID _g_aecname_objname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_aecname_objname");

  ee_ptr->ackec_name_tag = 0;		/* set tag to event action	*/

  _ms_get_mms_objname (aCtx, &ee_ptr->ackec_name, _g_aecname_objname_done);
  }

/************************************************************************/
/*			_g_aecname_objname_done    			*/
/* decoding get event enrollment attributes response: acknowledgement	*/
/* event condition name object name has been obtained. 			*/
/************************************************************************/

static ST_VOID _g_aecname_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_aecname_objname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_aecname_objname_cstr_done;
  }

/************************************************************************/
/*			_g_aecname_objname_cstr_done   		*/
/* decoding get event enrollment attributes response: acknowledgement	*/
/* event condition name object name constructor done was encountered.	*/
/************************************************************************/

static ST_VOID _g_aecname_objname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_aecname_cstr_done;
  }

/************************************************************************/
/*			_g_aecname_undefined	  	     	*/
/* decoding get event enrollment attributes response: acknowledgement	*/
/* event condition name undefined primitive was encountered.		*/
/************************************************************************/

static ST_VOID _g_aecname_undefined (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_aecname_undefined");

  ee_ptr->ackec_name_tag = 1;		/* set tag to undefined		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_aecname_cstr_done;
  }

/************************************************************************/
/*			_g_aecname_cstr_done	    		*/
/* decoding get event enrollment attributes response: acknowledgement 	*/
/* event condition name constructor done was encountered. 		*/
/************************************************************************/

static ST_VOID _g_aecname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = _g_evenroll_cstr_done;
  }

/************************************************************************/
/*			_g_evenroll_cstr_done 			*/
/* Decoding get event enrollment attributes response: event enrollment 	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID _g_evenroll_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ee_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, _g_evenroll_cstr);
  }

/************************************************************************/
/*			_g_eelist_cstr_done				*/
/* Decoding get event enrollment attributes response: list of event 	*/
/* enrollments constructor done was encountered.			*/
/************************************************************************/

static ST_VOID _g_eelist_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, _g_get_more_follows);

  aCtx->asn1r_decode_done_fun = _g_dec_done;	/* ok to be done		*/
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }
  
/************************************************************************/
/*			_g_get_more_follows				*/
/* Decoding get event enrollment attributes response: more follows was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID _g_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("_g_get_more_follows");
					/* read more follows 		*/
  if (asn1r_get_bool (aCtx, &(rsp_info->more_follows)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			_g_dec_done			   		*/
/* decode done, realloc the structure to desired size			*/
/************************************************************************/

static ST_VOID _g_dec_done (ASN1_DEC_CTXT *aCtx)
  {  
ST_INT	rsp_size;
  				/* find actual size for request info	*/
  rsp_size = sizeof (GETEEA_RESP_INFO)
	      + (sizeof (EVENT_ENROLLMENT) * rsp_info->num_of_evenroll);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (GETEEA_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _mms_dec_done_ok (aCtx);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_geteea					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the get event enrollment attributes request */
/************************************************************************/

MMSREQ_PEND *mp_geteea (ST_INT chan, GETEEA_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_EE_ATTR,
			M_CAST_MK_FUN (_ms_mk_geteea_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_geteea					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get event enrollment attributes request.	*/
/************************************************************************/

ST_RET  mpl_geteea (GETEEA_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_EE_ATTR,
			M_CAST_MK_FUN (_ms_mk_geteea_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_geteea_req					*/
/* construct a get event enrollment attributes request			*/
/************************************************************************/

ST_VOID _ms_mk_geteea_req (ASN1_ENC_CTXT *aCtx, GETEEA_REQ_INFO *info)
  {
ST_INT	i;
OBJECT_NAME *obj_ptr;

  if (info->ca_name_pres)
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 5 constructor	*/
    _ms_wr_mms_objname (aCtx, &info->ca_name);	/* write continue after name	*/
    asn1r_fin_constr (aCtx, 5,CTX,DEF);		/* finish ctx tag 5 constructor */
    }

  if (info->evact_name_pres)
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 4 constructor	*/
    _ms_wr_mms_objname (aCtx, &info->evact_name);	/* write event enrollment name	*/
    asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* finish ctx tag 4 constructor */
    }

  if (info->evcon_name_pres)
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 3 constructor	*/
    _ms_wr_mms_objname (aCtx, &info->evcon_name);	/* write event condition name	*/
    asn1r_fin_constr (aCtx, 3,CTX,DEF);		/* finish ctx tag 3 constructor */
    }

  if (info->client_app_pres)
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 2 constructor	*/
    asn1r_wr_delmnt (aCtx, info->client_app,info->client_app_len);
    asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish ctx tag 2 constructor */
    }

  if (info->eenames_pres)
    {
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    obj_ptr  = (OBJECT_NAME *) (info + 1);
    obj_ptr +=info->num_of_eenames-1;	/* point to last object name	*/
    for (i = 0; i < info->num_of_eenames; i++)
      {
      _ms_wr_mms_objname (aCtx, obj_ptr);
      obj_ptr--;
      }
    asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish context tag 1 cstr	*/
    }

  if (info->scope_of_req != 1)
    {					/* not the default (1 = client)	*/
    asn1r_wr_i16 (aCtx, info->scope_of_req);  	/* write the scope of request	*/
    asn1r_fin_prim (aCtx, 0, CTX);			/* ctx tag 0			*/
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_EE_ATTR,CTX,DEF); 	/* tag = opcode, ctx		*/
  }
