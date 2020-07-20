/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_geteea.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the get event	*/
/*	enrollment attributes. It decodes the get event enrollment 	*/
/*	attributes request (indication) and encodes the get event 	*/
/*	enrollment attributes response.			   		*/
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

static  GETEEA_REQ_INFO	*req_info;

static OBJECT_NAME      *obj_ptr;
static ST_INT max_obj_name;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID geteea_get_scope            (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_eelist_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_objname_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_eelist_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_client_app_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_client_app_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_client_app_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_ecname_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_ecname_done          (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_ecname_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_eaname_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_eaname_done          (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_eaname_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_caname_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_caname_done          (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_caname_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteea_dec_done             (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_geteea_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_geteea_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Get Event Enrollment Attributes Request");

#if defined(FIXED_DECBUF)
  max_obj_name = m_sv_max_obj_name;
  req_size = mmsl_dec_info_size;
#else
  max_obj_name = _mmsdec_msglen / 2;	/* find maximum size  */
  if (m_sv_max_obj_name && (max_obj_name > m_sv_max_obj_name))
    max_obj_name = m_sv_max_obj_name;
  req_size = sizeof (GETEEA_REQ_INFO) + (sizeof (OBJECT_NAME) * max_obj_name);
#endif

/* get storage for request information	*/
  req_info = (GETEEA_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);

  req_info->scope_of_req = 1;			/* default is client	*/
/*  req_info->eenames_pres    = SD_FALSE;   	   default not present	*/
/*  req_info->client_app_pres = SD_FALSE;   	   default not present	*/
/*  req_info->evcon_pres      = SD_FALSE;   	   default not present	*/
/*  req_info->evact_pres      = SD_FALSE;   	   default not present	*/
/*  req_info->ca_name_pres    = SD_FALSE;   	   default not present	*/

  ASN1R_TAG_ADD (aCtx, CTX,          0, geteea_get_scope);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, geteea_eelist_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, geteea_client_app_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, geteea_ecname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, geteea_eaname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, geteea_caname_cstr);

  aCtx->asn1r_decode_done_fun = geteea_dec_done;	/* ok to be done		*/
  _ms_set_cs_check2 (aCtx);			/* check for CS info		*/
  }

/************************************************************************/
/*			geteea_get_scope   				*/
/* Decoding get event enrollment attributes request: scope was		*/
/* encountered.								*/
/************************************************************************/

static ST_VOID geteea_get_scope (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_get_scope");
					/* read the scope 		*/
  if (asn1r_get_i16 (aCtx, &(req_info->scope_of_req)))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  else if ((req_info->scope_of_req < 0) || (req_info->scope_of_req > 3))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, geteea_eelist_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, geteea_client_app_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, geteea_ecname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, geteea_eaname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, geteea_caname_cstr);
  _ms_set_cs_check2 (aCtx);			/* check for CS info		*/
  }

/************************************************************************/
/*			geteea_eelist_cstr				*/
/* decoding get event enrollment attributes request: event enrollment	*/
/* name list constructor was encountered.   				*/
/************************************************************************/

static ST_VOID geteea_eelist_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_eelist_cstr");

  req_info->eenames_pres = SD_TRUE;	/* set list of names present	*/
  obj_ptr  = (OBJECT_NAME *) (req_info + 1);

  _ms_get_mms_objname (aCtx, obj_ptr, geteea_objname_done);

					/* set the cstr done function	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteea_eelist_cstr_done;
  }

/************************************************************************/
/*			geteea_objname_done				*/
/* decoding get event enrollment attributes request: object name has	*/
/* been decoded.    							*/
/************************************************************************/

static ST_VOID geteea_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_objname_done");

  req_info->num_of_eenames++;		/* inc # of event enroll names	*/
  if (req_info->num_of_eenames > max_obj_name)
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  else
    {
    obj_ptr++;				/* point to where next will go	*/
    _ms_get_mms_objname (aCtx, obj_ptr, geteea_objname_done);
    }
  }

/************************************************************************/
/*			geteea_eelist_cstr_done				*/
/* decoding get event enrollment attributes request: event enrollment	*/
/* name list constructor done was encountered.  			*/
/************************************************************************/

static ST_VOID geteea_eelist_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, geteea_client_app_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, geteea_ecname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, geteea_eaname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, geteea_caname_cstr);
  _ms_set_cs_check2 (aCtx);			/* check for CS info		*/
  }

/************************************************************************/
/*			geteea_client_app_cstr				*/
/* Decoding get event enrollment attributes request: client application	*/
/* constructor was encountered.	 					*/
/************************************************************************/

static ST_VOID geteea_client_app_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_client_app_cstr");

  req_info->client_app_pres = SD_TRUE;	/* set client app present	*/

  _ms_get_mms_app_ref (aCtx, &req_info->client_app,&req_info->client_app_len,
					 geteea_client_app_done);
  }

/************************************************************************/
/*			geteea_client_app_done     		       	*/
/* Decoding get event enrollment attributes request: client application */
/* has been obtained.							*/
/************************************************************************/

static ST_VOID geteea_client_app_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteea_client_app_cstr_done;
  }

/************************************************************************/
/*			geteea_client_app_cstr_done 			*/
/* Decoding get event enrollment attributes request: client application */
/* constructor done was encountered.  					*/
/************************************************************************/

static ST_VOID geteea_client_app_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, geteea_ecname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, geteea_eaname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, geteea_caname_cstr);
  _ms_set_cs_check2 (aCtx);			/* check for CS info		*/
  }

/************************************************************************/
/*			geteea_ecname_cstr				*/
/* decoding get event enrollment attributes request: event condition	*/
/* name constructor was encountered    					*/
/************************************************************************/

static ST_VOID geteea_ecname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_ecname_cstr");

  req_info->evcon_name_pres = SD_TRUE;  	/* event condition name present	*/

  _ms_get_mms_objname (aCtx, &req_info->evcon_name, geteea_ecname_done);
  }

/************************************************************************/
/*			geteea_ecname_done	     			*/
/* decoding get event enrollment attributes request: event condition	*/
/* name has been obtained.	 					*/
/************************************************************************/

static ST_VOID geteea_ecname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_ecname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteea_ecname_cstr_done;
  }

/************************************************************************/
/*			geteea_ecname_cstr_done				*/
/* decoding get event enrollment attributes request: event condition 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID geteea_ecname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, geteea_eaname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, geteea_caname_cstr);
  _ms_set_cs_check2 (aCtx);			/* check for CS info		*/
  }

/************************************************************************/
/*			geteea_eaname_cstr 				*/
/* decoding get event enrollment attributes request: event action	*/
/* name constructor was encountered    					*/
/************************************************************************/

static ST_VOID geteea_eaname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_eaname_cstr");

  req_info->evact_name_pres = SD_TRUE;  	/* event action name present	*/

  _ms_get_mms_objname (aCtx, &req_info->evact_name, geteea_eaname_done);
  }

/************************************************************************/
/*			geteea_eaname_done	     			*/
/* decoding get event enrollment attributes request: event action	*/
/* name has been obtained. 	    					*/
/************************************************************************/

static ST_VOID geteea_eaname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_eaname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteea_eaname_cstr_done;
  }

/************************************************************************/
/*			geteea_eaname_cstr_done				*/
/* decoding get event enrollment attributes request: event condition	*/
/* name constructor done was encountered.				*/
/************************************************************************/

static ST_VOID geteea_eaname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, geteea_caname_cstr);
  _ms_set_cs_check2 (aCtx);			/* check for CS info		*/
  }

/************************************************************************/
/*			geteea_caname_cstr 				*/
/* decoding get event enrollment attributes request: continue after	*/
/* name constructor was encountered    					*/
/************************************************************************/

static ST_VOID geteea_caname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_caname_cstr");

  req_info->ca_name_pres = SD_TRUE;  	/* continue after name present	*/

  _ms_get_mms_objname (aCtx, &req_info->ca_name, geteea_caname_done);
  }

/************************************************************************/
/*			geteea_caname_done	     			*/
/* decoding get event enrollment attributes request: continue after	*/
/* name has been obtained. 	    					*/
/************************************************************************/

static ST_VOID geteea_caname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteea_caname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteea_caname_cstr_done;
  }

/************************************************************************/
/*			geteea_caname_cstr_done				*/
/* decoding get event enrollment attributes request: continue after	*/
/* name constructor done was encountered.				*/
/************************************************************************/

static ST_VOID geteea_caname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check2 (aCtx);			/* check for CS info		*/
  }

/************************************************************************/
/*			geteea_dec_done			   		*/
/* decode done, realloc the structure to desired size			*/
/************************************************************************/

static ST_VOID geteea_dec_done (ASN1_DEC_CTXT *aCtx)
  {  
ST_INT	req_size;
  				/* find actual size for request info	*/
  req_size = sizeof (GETEEA_REQ_INFO)
		+ (sizeof (OBJECT_NAME) * req_info->num_of_eenames);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (GETEEA_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _mms_dec_done_ok (aCtx); 
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_geteea_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get event enrollment attributes	*/
/* response.								*/
/************************************************************************/

ST_RET mp_geteea_resp (MMSREQ_IND *indptr, GETEEA_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_EE_ATTR,indptr,
			M_CAST_MK_FUN (_ms_mk_geteea_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_geteea_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get event enrollment attributes response.	*/
/************************************************************************/

ST_RET mpl_geteea_resp (ST_UINT32 invoke_id, GETEEA_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_EE_ATTR,invoke_id,
			M_CAST_MK_FUN (_ms_mk_geteea_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_geteea_resp					*/
/* Construct a get event enrollment attributes attributes response.  	*/
/************************************************************************/

ST_VOID _ms_mk_geteea_resp (ASN1_ENC_CTXT *aCtx, GETEEA_RESP_INFO *info)
  {
ST_INT	i;
EVENT_ENROLLMENT *ee_ptr;

  if (info->more_follows)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->more_follows); 	/* write more follows indicator	*/
    asn1r_fin_prim (aCtx, 1, CTX);			/* ctx tag 1			*/
    }

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  ee_ptr = (EVENT_ENROLLMENT *) (info + 1);
  ee_ptr +=info->num_of_evenroll-1;	/* point to last event enroll	*/
  for (i = 0; i < info->num_of_evenroll; i++)
    {
    asn1r_strt_constr (aCtx);			/* start uni seq constructor	*/
    if (ee_ptr->ackec_name_pres)
      {
      asn1r_strt_constr (aCtx);			/* start ctx tag 10 constructor	*/
      if (ee_ptr->ackec_name_tag == 1)
        asn1r_fin_prim (aCtx, 1, CTX);		/* event action name undefined	*/
      else
        { 				/* write event action name	*/
        asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
        _ms_wr_mms_objname (aCtx, &ee_ptr->ackec_name);
        asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx tag 0 constructor */
        }
      asn1r_fin_constr (aCtx, 10,CTX,DEF);		/* finish ctx tag 10 constructor */
      }

#ifdef CS_SUPPORT
    if (ee_ptr->addl_detail_pres)
      {
      asn1r_strt_constr (aCtx);			/* start ctx tag 9 constructor	*/
      asn1r_wr_delmnt (aCtx, ee_ptr->addl_detail,ee_ptr->addl_detail_len);
      asn1r_fin_constr (aCtx, 9,CTX,DEF);		/* finish ctx tag 9 constructor */
      }
#endif

    if (ee_ptr->rem_acc_delay_pres)
      {
      asn1r_wr_u32 (aCtx, ee_ptr->rem_acc_delay); 	/* write remaining acc delay 	*/
      asn1r_fin_prim (aCtx, 8, CTX);		/* ctx tag 8			*/
      }

    if (ee_ptr->invoke_id_pres)
      {
      asn1r_wr_u32 (aCtx, ee_ptr->invoke_id); 	/* write the invoke id  	*/
      asn1r_fin_prim (aCtx, 7, CTX);		/* ctx tag 7			*/
      }

    if (ee_ptr->duration != 0)
      {		 			/* not the default of current	*/
      asn1r_wr_i16 (aCtx, ee_ptr->duration);		/* write the duration		*/
      asn1r_fin_prim (aCtx, 6, CTX);		/* ctx tag 6			*/
      }
    
    asn1r_wr_i16 (aCtx, ee_ptr->ee_class);		/* write event enrollment class	*/
    asn1r_fin_prim (aCtx, 5, CTX);			/* ctx tag 5			*/
    
    if (ee_ptr->mms_deletable)
      {		 			/* not the default of SD_FALSE	*/
      asn1r_wr_bool (aCtx, ee_ptr->mms_deletable); 	/* write mms deletable		*/
      asn1r_fin_prim (aCtx, 4, CTX);		/* ctx tag 4			*/
      }

    if (ee_ptr->client_app_pres)
      {
      asn1r_strt_constr (aCtx);			/* start ctx tag 3 constructor	*/
      asn1r_wr_delmnt (aCtx, ee_ptr->client_app,ee_ptr->client_app_len);
      asn1r_fin_constr (aCtx, 3,CTX,DEF);		/* finish ctx tag 3 constructor */
      }

    if (ee_ptr->evact_name_pres)
      {
      asn1r_strt_constr (aCtx);			/* start ctx tag 2 constructor	*/
      if (ee_ptr->evact_name_tag == 1)
        asn1r_fin_prim (aCtx, 1, CTX);		/* event action name undefined	*/
      else
        { 				/* write event action name	*/
        asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
        _ms_wr_mms_objname (aCtx, &ee_ptr->evact_name);
        asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx tag 0 constructor */
        }
      asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish ctx tag 2 constructor */
      }

    asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
    if (ee_ptr->evcon_name_tag == 1)
      asn1r_fin_prim (aCtx, 1, CTX);		/* event condition name undef	*/
    else
      {					/* write event condition name	*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
      _ms_wr_mms_objname (aCtx, &ee_ptr->evcon_name);
      asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx tag 0 constructor */
      }
    asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish ctx tag 1 constructor */

    asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
    _ms_wr_mms_objname (aCtx, &ee_ptr->evenroll_name);
    asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx tag 0 constructor */
    
    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF); 	/* finish uni seq constructor	*/
    ee_ptr--;
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx tag 0 constructor */

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_EE_ATTR,CTX,DEF);	/* tag = opcode, ctx		*/
  }
