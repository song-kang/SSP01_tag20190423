/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_ced3.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common primitive encode/decode functions	*/
/*	particular to getting and writing object names.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 12/02/03  MDE     04    Added log for invalid object name tag	*/
/* 12/20/01  JRB     03    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/09/97  MDE     01    Fixed object name decode for COMPACT		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"

#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/

static ST_VOID objname_get_item_id (ASN1_DEC_CTXT *aCtx);
static ST_VOID objname_get_domain_id (ASN1_DEC_CTXT *aCtx);
static ST_VOID objname_get_vmd_spec  (ASN1_DEC_CTXT *aCtx);
static ST_VOID objname_dom_spec_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID objname_dom_spec_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID objname_get_aa_spec   (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/*			global structures & functions			*/
/************************************************************************/

static ST_VOID (*get_mms_objname_done) (ASN1_DEC_CTXT *aCtx);

static OBJECT_NAME      *obj_ptr;

static ST_RET _error_code;

/************************************************************************/
/************************************************************************/
/*			_ms_get_mms_objname 				*/
/* Function to get an MMS object name from the received message.	*/
/************************************************************************/

ST_VOID  _ms_get_mms_objname (ASN1_DEC_CTXT *aCtx, OBJECT_NAME *data_ptr, ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {
			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    _error_code = RESP_BAD_VALUE;
  else
    _error_code = REQ_BAD_VALUE;

  obj_ptr = data_ptr;
  get_mms_objname_done = done_fun;

  ASN1R_TAG_ADD (aCtx, CTX, 	 0, objname_get_vmd_spec);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, objname_dom_spec_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 	 2, objname_get_aa_spec);
  }

/************************************************************************/
/*			objname_get_vmd_spec				*/
/* vmd-specific name was encountered.					*/
/************************************************************************/

static ST_VOID objname_get_vmd_spec (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("objname_get_vmd_spec");

  obj_ptr->object_tag = VMD_SPEC;	/* set tag to vmd specific	*/

#if defined(USE_COMPACT_MMS_STRUCTS)
  obj_ptr->obj_name.vmd_spec = (ST_CHAR *) (aCtx->asn1r_field_ptr - 1);
#endif
					/* read name from message	*/
  if (asn1r_get_identifier (aCtx, obj_ptr->obj_name.vmd_spec))
    asn1r_set_dec_err (aCtx, _error_code);

  (*get_mms_objname_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/*			objname_dom_spec_cstr				*/
/* domain specific constructor was encountered. 			*/
/************************************************************************/

static ST_VOID objname_dom_spec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("objname_dom_spec_cstr");

  obj_ptr->object_tag = DOM_SPEC;		/* set tag to domain specific	*/

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, objname_get_domain_id);
  }

/************************************************************************/
/*			objname_get_domain_id				*/
/* domain id was encountered.						*/
/************************************************************************/

static ST_VOID objname_get_domain_id (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("objname_get_domain_id");

#if defined(USE_COMPACT_MMS_STRUCTS)
  obj_ptr->domain_id = (ST_CHAR *) (aCtx->asn1r_field_ptr - 1);
#endif
					/* read name from message	*/
  if (asn1r_get_identifier (aCtx, obj_ptr->domain_id))
    asn1r_set_dec_err (aCtx, _error_code);

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, objname_get_item_id);
  }

/************************************************************************/
/*			objname_get_item_id				*/
/* item id was encountered.						*/
/************************************************************************/

static ST_VOID objname_get_item_id (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("objname_get_item_id");

#if defined(USE_COMPACT_MMS_STRUCTS)
  obj_ptr->obj_name.item_id = (ST_CHAR *) (aCtx->asn1r_field_ptr - 1);
#endif
					/* read name from message	*/
  if (asn1r_get_identifier (aCtx, obj_ptr->obj_name.item_id))
    asn1r_set_dec_err (aCtx, _error_code);
					/* set constructor done fun	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = objname_dom_spec_cstr_done;
  }

/************************************************************************/
/*			objname_dom_spec_cstr_done			*/
/* domain specific constructor done was encountered.			*/
/************************************************************************/

static ST_VOID objname_dom_spec_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  (*get_mms_objname_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/*			objname_get_aa_spec				*/
/* aa-specific name was encountered.					*/
/************************************************************************/

static ST_VOID objname_get_aa_spec (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("objname_get_aa_spec");

  obj_ptr->object_tag = AA_SPEC;		/* set tag to aa specific	*/

#if defined(USE_COMPACT_MMS_STRUCTS)
  obj_ptr->obj_name.aa_spec = (ST_CHAR *) (aCtx->asn1r_field_ptr - 1);
#endif
					/* read name from message	*/
  if (asn1r_get_identifier (aCtx, obj_ptr->obj_name.aa_spec))
    asn1r_set_dec_err (aCtx, _error_code);

  (*get_mms_objname_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/************************************************************************/
/*			_ms_wr_mms_objname					*/
/* Function to write an MMS object name to the message. 		*/
/************************************************************************/

ST_VOID _ms_wr_mms_objname (ASN1_ENC_CTXT *aCtx, OBJECT_NAME *data_ptr)
  {
  switch (data_ptr->object_tag)
    {
    case (VMD_SPEC)  : 		/* write the vmd specific name	*/
      asn1r_wr_vstr (aCtx, data_ptr->obj_name.vmd_spec);
      asn1r_fin_prim (aCtx, 0, CTX);	/* context tag 0		*/
      break;

    case (DOM_SPEC)  : 		/* write domain specific name	*/
      asn1r_strt_constr (aCtx);
      asn1r_wr_vstr (aCtx, data_ptr->obj_name.item_id);
      asn1r_fin_prim (aCtx, VISTR_CODE, UNI);	/* universal string		*/
      asn1r_wr_vstr (aCtx, data_ptr->domain_id);
      asn1r_fin_prim (aCtx, VISTR_CODE, UNI);	/* universal string		*/
      asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context tag 1		*/
      break;

    case (AA_SPEC)  : 			/* write the aa specific name	*/
      asn1r_wr_vstr (aCtx, data_ptr->obj_name.aa_spec);
      asn1r_fin_prim (aCtx, 2,CTX); 		/* context tag 2		*/
      break;

    default:
      MLOG_NERR1 ("Invalid object_tag: %d", data_ptr->object_tag);
      break;
    }
  }

