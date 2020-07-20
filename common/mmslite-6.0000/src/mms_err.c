/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_err.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the MMS error functions for decoding	*/
/*	& encoding the error response PDUs :				*/
/*		confirmed-ErrorPDU					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 03/03/10  JRB	   Replace "ml_log_*" with "mlogl_info" struct.	*/
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 01/20/10  JRB     09    Fix log messages.				*/
/* 11/01/07  EJV     08    Rem obsolete LLC30... code.			*/
/* 04/05/02  MDE     07    Added MLOG logging				*/
/* 01/23/02  JRB     06    Fix "Response built" msg (was missing header)*/
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
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pcon.h"
#include "asn1defs.h"
#include "mloguser.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* variables global to the decode :					*/

static ERR_INFO *err_info_ptr;


#ifdef MOD_SUPPORT
static ST_VOID err_mod_pos (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID err_rsp2 (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_class (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_get_cac (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID err_get_cs (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_cs_done (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID err_class_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_get_add_code  (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_get_add_descr (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_add_ssi_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_get_add_ssi_i16 (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_get_add_ssi_u32 (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_get_dee_obj_name (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_dee_obj_name_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_get_add_serv_err (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_adtnl_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_ssi_done (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			mms_err_invoke_fun				*/
/* This function is called from mmsdec.c when invoke ID is expected	*/
/* in a CONFIRMED or CANCEL ERROR PDU.					*/
/************************************************************************/

ST_VOID mms_err_invoke_fun (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code)
  {
ST_INT pdutype;

  pdutype = aCtx->mmsdec_rslt->type;
  if (aCtx->asn1r_constr_elmnt || id_code != 0)
    {
    asn1r_set_dec_err (aCtx, PDU_INVALID);   		/* wrong code or constructed int*/
    return;
    }
  if (asn1r_get_u32 (aCtx, &aCtx->mmsdec_rslt->id))
    {
    if (pdutype == MMSCANERR)
      asn1r_set_dec_err (aCtx, CANERR_INVAL_INVOKE); /* not a valid integer contents	*/
    else
      asn1r_set_dec_err (aCtx, ERR_INVAL_INVOKE);   /* not a valid integer contents	*/
    return;
    }

  aCtx->mmsdec_rslt->dec_level = 2;   /* set flag that Invoke ID is OK	*/

  MLOG_CDEC1 ("Invoke ID present, = %lu",(ST_ULONG)aCtx->mmsdec_rslt->id);
  err_info_ptr = (ERR_INFO *) _m_get_dec_buf (aCtx, sizeof (ERR_INFO));
  if (pdutype == MMSCANERR)		/* Cancel Error PDU, next must 	*/
    ASN1R_TAG_ADD (aCtx, CTX | CONSTR,1,err_rsp2);	/* be context tag 1 cstr	*/
  else
    {					/* Confirmed Error PDU, next 	*/
#ifdef MOD_SUPPORT			
    ASN1R_TAG_ADD (aCtx, CTX ,1,err_mod_pos); 	  	/* Context tag 1 (opt)	*/
#endif
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,err_rsp2);	 	/* Context tag 2 cstr	*/
    }
  }

#ifdef MOD_SUPPORT
/************************************************************************/
/*			err_mod_pos					*/
/* This function is called when a modifier position is detected in a	*/
/* confirmed_request_error PDU.  The only thing which may follow is a	*/
/* service error description.						*/
/************************************************************************/

static ST_VOID err_mod_pos (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_mod_pos");
  
  if (asn1r_get_i32 (aCtx, &(err_info_ptr->adtnl.mod_pos)))	/* read mod position	*/
    asn1r_set_dec_err (aCtx, ERR_UNSPECIFIED);
  err_info_ptr -> adtnl.mod_pos_pres = SD_TRUE;
  
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,2,err_rsp2);	 	/* Context tag 2 cstr */
  }
#endif


/************************************************************************/
/************************************************************************/
/*			mms_err_rsp					*/
/* This function is called from mmsdec.c when an Error Response PDU is	*/
/* being decoded for initiate or conclude error ONLY			*/
/* When this is called directly, we need to allocate the buffer		*/
/************************************************************************/

ST_VOID mms_err_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("mms_err_rsp");

  err_info_ptr = (ERR_INFO *) _m_get_dec_buf (aCtx, sizeof (ERR_INFO));
  aCtx->asn1r_err_fun      = _mms_dec_buf_free;
  err_rsp2 (aCtx);				/* set tags			*/
  }


/************************************************************************/
/************************************************************************/
/*			_mms_get_service_err 				*/
/* This function is called from rs_termd.c when IS is used and the	*/
/* 'discarded' parameter is send (service error) -or- from rs_evnot.c	*/
/* when the action result is failure.					*/
/************************************************************************/

ST_VOID _mms_get_service_err (ASN1_DEC_CTXT *aCtx, ERR_INFO *err_ptr)
  {
  err_info_ptr = err_ptr;		/* use caller's storage		*/
  err_rsp2 (aCtx);				/* set tags			*/
  }


/************************************************************************/
/************************************************************************/
/*			err_rsp2					*/
/* This function is called when an Error Response PDU is being decoded	*/
/* for confirmed error.							*/
/************************************************************************/

static ST_VOID err_rsp2 (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_rsp2");
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,err_class);
  }

/************************************************************************/
/*			err_class					*/
/* This function is invoked when the ServiceError is being decoded, and	*/
/* the errorClass choice is encountered.				*/
/************************************************************************/

static ST_VOID err_class (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_class");

  ASN1R_TAG_ADD (aCtx, CTX, 0, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 1, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 2, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 3, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 4, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 5, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 6, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 7, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 8, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX, 9, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX,10, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX,11, err_get_cac);
  ASN1R_TAG_ADD (aCtx, CTX,12, err_get_cac);
#ifdef CS_SUPPORT
#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
  if (mmsl_version)				/* IS 	*/
#endif
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,13, err_get_cs); 	/* CS error		*/
#endif
  }

/************************************************************************/
/*			err_get_cac					*/
/* error response is being decoded:  context tagged error class has	*/
/* been encountered, get the error class and error code.		*/
/************************************************************************/

static ST_VOID err_get_cac (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_get_cac");

  err_info_ptr->eclass = aCtx->asn1r_elmnt_id;		/* save the error class */

  if (asn1r_get_i16 (aCtx, &(err_info_ptr->code))) 	/* read the error code	*/
    asn1r_set_dec_err (aCtx, ERR_UNSPECIFIED);

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = err_class_cstr_done;	/* choice cstr done	*/
  }

#ifdef CS_SUPPORT
/************************************************************************/
/*			err_get_cs					*/
/* error response is being decoded:  context tagged error class 13 has	*/
/* been encountered, get the CS service error.				*/
/************************************************************************/

static ST_VOID err_get_cs (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_get_cs");

  err_info_ptr->adtnl.ss_error_data= aCtx->asn1r_field_ptr;	/* save ptr to field	*/
  err_info_ptr->adtnl.ss_error_len = aCtx->asn1r_elmnt_len;	/* save len of field	*/
  asn1r_parse_next (aCtx, err_cs_done);  	 		/* parse the contents	*/
  }

/************************************************************************/
/*			err_cs_done					*/
/* Companion Standard service error has been parsed OK.			*/
/************************************************************************/

static ST_VOID err_cs_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = err_class_cstr_done;	/* choice cstr done	*/
  }
#endif

/************************************************************************/
/*			err_class_cstr_done					*/
/* errorClass constructor (choice) is complete				*/
/* set up to get additional (optional) information			*/
/************************************************************************/

static ST_VOID err_class_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_class_cstr_done");

				/* add tags for optional information	*/
  ASN1R_TAG_ADD (aCtx, CTX, 1, err_get_add_code);
  ASN1R_TAG_ADD (aCtx, CTX, 2, err_get_add_descr);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 3, err_add_ssi_cstr);

  aCtx->asn1r_decode_done_fun = _mms_dec_done_ok;

  if ((aCtx->mmsdec_rslt->type == MMSREQ) ||	/* terminate download request*/
      (aCtx->mmsdec_rslt->type == MMSUNREQ))	/* or Event Notification*/
  	/* Terminate Download req or Event Notification unconfirmed	*/
    	/* request is being decoded, decode can be done, but check for	*/
    	/* companion standard information.				*/
    _ms_set_cs_check (aCtx);
  else
    asn1r_set_all_cstr_done (aCtx);		/* clear all cstr done funtions */
  }

/************************************************************************/
/*			err_get_add_code				*/
/* error response is being decoded:  additional error code has been	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID err_get_add_code (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_get_add_code");

  err_info_ptr->adtnl.info_pres = SD_TRUE;		/* set info present	*/
  err_info_ptr->adtnl.code_pres = SD_TRUE;		/* set code present	*/

			/* read the additional error code		*/
  if (asn1r_get_i32 (aCtx, &(err_info_ptr->adtnl.code)))
    asn1r_set_dec_err (aCtx, ERR_UNSPECIFIED);

  ASN1R_TAG_ADD (aCtx, CTX, 2, err_get_add_descr);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 3, err_add_ssi_cstr);

  if ((aCtx->mmsdec_rslt->type == MMSREQ) ||	/* terminate download request*/
      (aCtx->mmsdec_rslt->type == MMSUNREQ))	/* or Event Notification*/
  	/* Terminate Download req or Event Notification unconfirmed	*/
    	/* request is being decoded, decode can be done, but check for	*/
    	/* companion standard information.				*/
    _ms_set_cs_check (aCtx);
  else
    asn1r_set_all_cstr_done (aCtx);		/* clear all cstr done funtions */
  }

/************************************************************************/
/*			err_get_add_descr				*/
/* error response is being decoded:  additional error description has	*/
/* been encountered.							*/
/************************************************************************/

static ST_VOID err_get_add_descr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_get_add_descr");

  err_info_ptr->adtnl.info_pres = SD_TRUE;		/* set info present	*/
  err_info_ptr->adtnl.descr_pres = SD_TRUE;	/* set descr present	*/

			/* read the additional error description	*/
/* copy error description back onto itself shifted by one character	*/
  err_info_ptr->adtnl.descr = (ST_CHAR *)aCtx->asn1r_field_ptr -1;
  if (asn1r_get_vstr (aCtx, err_info_ptr->adtnl.descr))
    asn1r_set_dec_err (aCtx, ERR_UNSPECIFIED);

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 3, err_add_ssi_cstr);

  if ((aCtx->mmsdec_rslt->type == MMSREQ) ||	/* terminate download request*/
      (aCtx->mmsdec_rslt->type == MMSUNREQ))	/* or Event Notification*/
  	/* Terminate Download req or Event Notification unconfirmed	*/
    	/* request is being decoded, decode can be done, but check for	*/
    	/* companion standard information.				*/
    _ms_set_cs_check (aCtx);
  else
    asn1r_set_all_cstr_done (aCtx);		/* clear all cstr done funtions */
  }

/************************************************************************/
/*			err_add_ssi_cstr 				*/
/* error response is being decoded:  additional service specific error	*/
/* information constructor has been encountered.			*/
/************************************************************************/

static ST_VOID err_add_ssi_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_get_ssi_cstr");

					/* add tags for all choices	*/
  ASN1R_TAG_ADD (aCtx, CTX, 0, err_get_add_ssi_i16);	
  ASN1R_TAG_ADD (aCtx, CTX, 1, err_get_add_ssi_i16);	
  ASN1R_TAG_ADD (aCtx, CTX, 2, err_get_add_ssi_i16);	
  ASN1R_TAG_ADD (aCtx, CTX, 3, err_get_add_ssi_i16);	
  ASN1R_TAG_ADD (aCtx, CTX, 4, err_get_add_ssi_i16);	
  ASN1R_TAG_ADD (aCtx, CTX, 5, err_get_add_ssi_u32);	
  ASN1R_TAG_ADD (aCtx, CTX, 6, err_get_add_ssi_u32);	
  ASN1R_TAG_ADD (aCtx, CTX, 7, err_get_add_ssi_u32);	
#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
  if (mmsl_version)				/* IS 	*/
#endif
    {
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 8, err_get_dee_obj_name);	/* def ev enroll*/
    ASN1R_TAG_ADD (aCtx, CTX,        9, err_get_add_ssi_i16);	/* file rename	*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,10, err_get_add_serv_err);	/* adtnl service*/
    }
  else
    {
    ASN1R_TAG_ADD (aCtx, CTX,        8, err_get_add_ssi_i16);	/* file rename	*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 9, err_get_add_serv_err);	/* adtnl service*/
    }
  }

/************************************************************************/
/*			err_get_add_ssi_i16 				*/
/* error response is being decoded:  additional service specific error	*/
/* information has been encountered for obtain file, start, stop, 	*/
/* resume, reset or file rename.					*/
/************************************************************************/
					
static ST_VOID err_get_add_ssi_i16 (ASN1_DEC_CTXT *aCtx)	
  {
ST_INT16  ssi_error;

  MLOG_CDEC0 ("err_get_add_ssi_i16");

  err_info_ptr->adtnl.info_pres = SD_TRUE;	/* set info present indicator	*/
  err_info_ptr->adtnl.ssi_pres  = SD_TRUE;	/* set additional service pres	*/

  err_info_ptr->adtnl.service = aCtx->asn1r_elmnt_id;	/* save the service id	*/

	    			/* read the additional service error	*/
  if (asn1r_get_i16 (aCtx, &ssi_error))
    asn1r_set_dec_err (aCtx, ERR_UNSPECIFIED);
  else
    err_info_ptr->adtnl.ss_error_val = (ST_UINT32) ssi_error;
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = err_ssi_done;
  }

/************************************************************************/
/*			err_get_add_ssi_u32 				*/
/* error response is being decoded:  additional service specific error	*/
/* information has been encountered for delete varaible access, delete	*/
/* named variable list or delete named type.			 	*/
/************************************************************************/
					
static ST_VOID err_get_add_ssi_u32 (ASN1_DEC_CTXT *aCtx)	
  {
  MLOG_CDEC0 ("err_get_add_ssi_u32");

  err_info_ptr->adtnl.info_pres = SD_TRUE;	/* set info present indicator	*/
  err_info_ptr->adtnl.ssi_pres  = SD_TRUE;	/* set additional service pres	*/

  err_info_ptr->adtnl.service = aCtx->asn1r_elmnt_id;	/* save the service id	*/

	    			/* read the additional service error	*/
  if (asn1r_get_u32 (aCtx, &(err_info_ptr->adtnl.ss_error_val)))
    asn1r_set_dec_err (aCtx, ERR_UNSPECIFIED);
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = err_ssi_done;
  }

/************************************************************************/
/*			err_get_dee_obj_name 				*/
/* error response is being decoded:  additional service specific error	*/
/* information has been encountered for define event enrollment error	*/
/* get the object name.						 	*/
/************************************************************************/
					
static ST_VOID err_get_dee_obj_name (ASN1_DEC_CTXT *aCtx)	
  {
  MLOG_CDEC0 ("err_get_dee_obj_name");

  err_info_ptr->adtnl.info_pres = SD_TRUE;	/* set info present indicator	*/
  err_info_ptr->adtnl.ssi_pres  = SD_TRUE;	/* set additional service pres	*/

  err_info_ptr->adtnl.service = aCtx->asn1r_elmnt_id;	/* save the service id	*/

			    			/* read the object name	*/
  _ms_get_mms_objname (aCtx, &err_info_ptr->adtnl.ss_error_oname, 
			err_dee_obj_name_done);
  }

/************************************************************************/
/*			err_dee_obj_name_done	 			*/
/* define event enrollment error is being decoded: object name has	*/
/* been obtained.		 					*/
/************************************************************************/

static ST_VOID err_dee_obj_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("err_dee_obj_name_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = err_adtnl_done;
  }

/************************************************************************/
/*			err_get_add_serv_err 				*/
/* error response is being decoded:  additional service specific error	*/
/* information has been encountered for additional service error.	*/
/************************************************************************/
					
static ST_VOID err_get_add_serv_err (ASN1_DEC_CTXT *aCtx)	
  {
  MLOG_CDEC0 ("err_get_add_serv_err");

  err_info_ptr->adtnl.info_pres = SD_TRUE;	/* set info present indicator	*/
  err_info_ptr->adtnl.ssi_pres  = SD_TRUE;	/* set additional service pres	*/

  err_info_ptr->adtnl.service = aCtx->asn1r_elmnt_id;	/* save the service id	*/

	        	/* Companion Standard additional service error	*/
  err_info_ptr->adtnl.ss_error_data = aCtx->asn1r_field_ptr;  /* save ptr to field	*/
  err_info_ptr->adtnl.ss_error_len  = aCtx->asn1r_elmnt_len;  /* save len of field	*/
  asn1r_parse_next (aCtx, err_adtnl_done);   	          /* parse contents	*/
  }

/************************************************************************/
/*			err_adtnl_done					*/
/* IS: define event enrollment, object name has been obtained -OR-	*/
/* Companion Standard ANY has been parsed OK.  Message is now done.	*/
/************************************************************************/

static ST_VOID err_adtnl_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = err_ssi_done;
  }

/************************************************************************/
/*			err_ssi_done					*/
/* Service specific info constructor done				*/
/************************************************************************/

static ST_VOID err_ssi_done (ASN1_DEC_CTXT *aCtx)
  {
  if ((aCtx->mmsdec_rslt->type == MMSREQ) ||	/* terminate download request*/
      (aCtx->mmsdec_rslt->type == MMSUNREQ))	/* or Event Notification*/
  	/* Terminate Download req or Event Notification unconfirmed	*/
    	/* request is being decoded, decode can be done, but check for	*/
    	/* companion standard information.				*/
    _ms_set_cs_check (aCtx);
  else
    aCtx->asn1r_decode_done = SD_TRUE;		      /*   just terminate the parse	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_err_resp					*/
/* Create and send a general error response				*/
/************************************************************************/

ST_RET mp_err_resp (MMSREQ_IND *indptr, ST_INT16 class, ST_INT16 code)
  {
ST_UCHAR	*msg_ptr;			/* pointer to start of message	*/
ST_INT	msg_len;			/* length of message built	*/
ST_RET	ret;				/* function return value	*/
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  if (!(ret = _ms_check_ind_actv (indptr)))
    {
    msg_ptr = _ms_mk_err_resp (aCtx, m_build_buf,mms_max_msgsize,indptr->op,indptr->id,
				class,code);
    msg_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr);

    if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
      ret = ME_ASN1_ENCODE_OVERRUN;
    else
      ret = _mms_send_data (indptr->chan, msg_len, msg_ptr, 
		indptr->context, indptr->add_addr_info);

  /* If a response logging function is installed, call it		*/
    if (!ret && m_log_error_resp_fun && (mms_debug_sel & MMS_LOG_RESP))
      (*m_log_error_resp_fun) (indptr, class, code);

    _ms_ind_serve_done (indptr);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_err_resp					*/
/* Create and send a general error response				*/
/* Initiate, Conclude, Confirmed Service				*/ 
/************************************************************************/

ST_RET mpl_err_resp (ST_INT opcode, ST_UINT32 invoke, 
		     ST_INT16 err_class, ST_INT16 code)
  {
ST_INT ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;
ERR_INFO 	err_info;

  S_LOCK_COMMON_RESOURCES ();
  mmsl_msg_start = _ms_mk_err_resp (aCtx, mmsl_enc_buf,mmsl_enc_buf_size,opcode,invoke,
  			err_class,code);
  mmsl_msg_len = (ST_INT)((mmsl_enc_buf + mmsl_enc_buf_size) - mmsl_msg_start);

  if (!aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
    ret = SD_SUCCESS;
  else
    ret = ME_ASN1_ENCODE_OVERRUN;

  if ((mms_debug_sel & MMS_LOG_RESP) && ret == SD_SUCCESS && mlogl_info.error_resp_fun)
    {
    memset (&err_info, 0, sizeof (err_info));
    err_info.code = code;
    err_info.eclass = err_class;
    (*mlogl_info.error_resp_fun) (invoke, &err_info);
    }

  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_err_resp					*/
/* This function is called to build a general error response.		*/
/* adtnl_err_info is a global variable containing additional error 	*/
/* information.								*/
/************************************************************************/

ST_UCHAR *_ms_mk_err_resp (ASN1_ENC_CTXT *aCtx, ST_UCHAR *buf_ptr, ST_INT buf_len, ST_INT op,
	ST_UINT32 id, ST_INT16 err_class, ST_INT16 code)
  {
ST_INT	pdu_type;			/* PDU type to go in error PDU. */
#ifdef DEBUG_SISCO
ST_UCHAR	*msg_start;			/* These variables used for	*/
ST_INT		msg_len;			/* debug print only		*/
#endif

  asn1r_strt_asn1_bld (aCtx, buf_ptr,buf_len);	/* init the ASN.1 tools 	*/
  _ms_wr_service_err (aCtx, err_class,code,&adtnl_err_info);

/* Write SEQUENCE header and invoke id, if this opcode is NOT initiate	*/
/* or conclude. 							*/
  if ((op == MMSOP_INITIATE) || (op == MMSOP_CONCLUDE))
    ;
  else
    {				/* Finish the serviceError sequence.	*/
    if (op == MMSOP_CANCEL)
      asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context tag 1 for cancel	*/
    else
      asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* context 2 for confirmed err	*/

#ifdef MOD_SUPPORT
    if (adtnl_err_info.mod_pos_pres)	/* send the modifier position	*/
      {
      asn1r_wr_u32 (aCtx, adtnl_err_info.mod_pos);
      asn1r_fin_prim (aCtx, 1,CTX);
      adtnl_err_info.mod_pos_pres = SD_FALSE;
      }
#endif

    asn1r_wr_u32 (aCtx, id);		/* Write the invoke ID. 		*/
    asn1r_fin_prim (aCtx, 0,CTX); 		/* conf and cancl both use CTX 0 tag	*/
    }

  				/* Reset add'l info structure to empty	*/
  adtnl_err_info.info_pres = SD_FALSE;  

  switch (op)
    {
    case MMSOP_INITIATE :
      pdu_type = MMSINITERR;
      break;
    case MMSOP_CONCLUDE :
      pdu_type = MMSCNCLERR;
      break;
    case MMSOP_CANCEL :
      pdu_type = MMSCANERR;
      break;
    default:
      pdu_type = MMSERROR;
    }

#ifdef DEBUG_SISCO
  asn1r_fin_constr (aCtx, (ST_UINT16) pdu_type,CTX,DEF);
  msg_start = aCtx->asn1r_field_ptr + 1;
  msg_len = (ST_INT)((buf_ptr + buf_len) - msg_start);
  MLOG_ENC1 ("ERROR Response built : len = %d ",msg_len);
  MLOG_ENCH (msg_len,msg_start);
  MLOG_PAUSEENC (NULL);
  return (msg_start);
#else
  asn1r_fin_constr (aCtx, (ST_UINT16) pdu_type,CTX,DEF);
  return (aCtx->asn1r_field_ptr + 1);
#endif
  }


/************************************************************************/
/************************************************************************/
/*			wr_service_err					*/
/************************************************************************/

ST_VOID _ms_wr_service_err (ASN1_ENC_CTXT *aCtx, ST_INT16 err_class, ST_INT16 code,
			    ADTNL_ERR_RESP_INFO *adtnl_info)
  {
ST_INT16 ssi_error;

  asn1r_strt_constr (aCtx);			/* start sequence constructor	*/
  if (adtnl_info->info_pres)
    {				/* write additional info if present	*/
    if (adtnl_info->ssi_pres)
      { 			/* write the service specific info	*/
      asn1r_strt_constr (aCtx);	
      switch (adtnl_info->service)
        {
        case 0 :	/* obtain file	*/
        case 1 :	/* start	*/
        case 2 :	/* stop		*/
        case 3 :	/* resume	*/
        case 4 :	/* reset	*/
	  ssi_error = (ST_INT16) adtnl_info->ss_error_val;
          asn1r_wr_i16 (aCtx, ssi_error);
	  asn1r_fin_prim (aCtx, adtnl_info->service,CTX);
          break;

        case 5 :	/* delete variable access	*/
        case 6 :	/* delete named variable list	*/
        case 7 :	/* delete named type		*/
          asn1r_wr_u32 (aCtx, adtnl_info->ss_error_val);
	  asn1r_fin_prim (aCtx, adtnl_info->service,CTX);
          break;

        case 8 :
#ifndef MMS_LITE
          if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
          if (mmsl_version)				/* IS 	*/
#endif
	    {		 		/* IS, define event enrollment	*/
	    asn1r_strt_constr (aCtx);
	    _ms_wr_mms_objname (aCtx, &adtnl_info->ss_error_oname);
	    asn1r_fin_constr (aCtx, 8,CTX,DEF);
	    }
	  else
	    {				/* DIS, file rename 		*/
	    ssi_error = (ST_INT16) adtnl_info->ss_error_val;
            asn1r_wr_i16 (aCtx, ssi_error);
	    asn1r_fin_prim (aCtx, adtnl_info->service,CTX);
	    }
	  break;

        case 9 :
#ifndef MMS_LITE
          if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
          if (mmsl_version)				/* IS 	*/
#endif
	    {				/* IS, file rename 		*/		     
	    ssi_error = (ST_INT16) adtnl_info->ss_error_val;
            asn1r_wr_i16 (aCtx, ssi_error);
	    asn1r_fin_prim (aCtx, adtnl_info->service,CTX);
	    }		     
	  else
	    {				/* DIS, additional service err	*/		     
 	    asn1r_strt_constr (aCtx);
	    asn1r_wr_delmnt (aCtx, adtnl_info->ss_error_data,adtnl_info->ss_error_len);
	    asn1r_fin_constr (aCtx, 9,CTX,DEF);
	    }		     
	  break;

        default:			/* IS, additional service error	*/
	  asn1r_strt_constr (aCtx);
	  asn1r_wr_delmnt (aCtx, adtnl_info->ss_error_data,adtnl_info->ss_error_len);
	  asn1r_fin_constr (aCtx, 10,CTX,DEF);
	}
      asn1r_fin_constr (aCtx, 3,CTX,DEF);
      }

    if (adtnl_info->descr_pres)
      { 			/* write the additional description	*/
      asn1r_wr_vstr (aCtx, adtnl_info->descr);
      asn1r_fin_prim (aCtx, 2,CTX);
      }
    if (adtnl_info->code_pres)
      { 			/* write the service specific info	*/
      asn1r_wr_i32 (aCtx, adtnl_info->code);
      asn1r_fin_prim (aCtx, 1,CTX);
      }
    adtnl_info->info_pres = SD_FALSE;
    }

  asn1r_strt_constr (aCtx);			/* start choice constructor	*/
  asn1r_wr_i16 (aCtx, code);			/* write the value		*/
  asn1r_fin_prim (aCtx, err_class,CTX); 		/* finish the contents		*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context 0 cstr		*/
  }

