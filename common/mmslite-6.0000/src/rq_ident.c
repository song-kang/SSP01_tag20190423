/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_ident.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the Identify	*/
/*	service.  It decodes the Identify response (indication) and	*/
/*	encodes the Identify request.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

static  IDENT_RESP_INFO		*info;
static ST_CHAR *vstr;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID ident_get_model   (ASN1_DEC_CTXT *aCtx);
static ST_VOID ident_get_vendor  (ASN1_DEC_CTXT *aCtx);
static ST_VOID ident_get_rev     (ASN1_DEC_CTXT *aCtx);
static ST_VOID ident_start_as    (ASN1_DEC_CTXT *aCtx);
static ST_VOID ident_get_as      (ASN1_DEC_CTXT *aCtx);
static ST_VOID ident_as_seq_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_identify_rsp				*/
/* Function called to decode an Identify response PDU after the PDU	*/
/* type has been determined.  Sets up state machine function pointers	*/
/* and returns, letting the ASN1DE tools take over.			*/
/************************************************************************/

ST_VOID mms_identify_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Identify Response");


/* get storage for request information	*/
  info = (IDENT_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (IDENT_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX,0,ident_get_vendor);
  }

/************************************************************************/
/*			ident_get_vendor				*/
/* Identify response is being decoded: vendor name has been encountered.*/
/************************************************************************/

static ST_VOID ident_get_vendor (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ident_get_vendor");

/* copy vendor name back onto itself shifted by one character	        */

  vstr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_vstr (aCtx, vstr))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
    return;                          
    }                                
  
  if (strlen (vstr) < MAX_VEND_LEN)
    strcpy (info->vend, vstr);
  else
    memcpy (info->vend, vstr, MAX_VEND_LEN); /* only take up to MAX_VEND_LEN  	*/
  info->vend[ MAX_VEND_LEN ] = 0x00;        /* NULL term. end just in case  */

  ASN1R_TAG_ADD (aCtx, CTX,1,ident_get_model);
  }


/************************************************************************/
/*			ident_get_model 				*/
/* Identify response is being decoded: model name has been encountered. */
/************************************************************************/

static ST_VOID ident_get_model (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ident_get_model");

  vstr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_vstr (aCtx, vstr))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
    return;                          
    }

  if (strlen (vstr) < MAX_MOD_LEN)
    strcpy (info->model, vstr);
  else
    memcpy (info->model, vstr, MAX_MOD_LEN); /* only take up to MAX_MOD_LEN  	*/
  info->model[ MAX_MOD_LEN ] = 0x00;        /* NULL term. end just in case  */

  ASN1R_TAG_ADD (aCtx, CTX,2,ident_get_rev);
  }

/************************************************************************/
/*			ident_get_rev					*/
/* Identify response is being decoded: revision name was encountered.	*/
/************************************************************************/

static ST_VOID ident_get_rev (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ident_get_rev");
	      
  vstr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_vstr (aCtx, vstr))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
    return;                          
    }

  if (strlen (vstr) < MAX_REV_LEN)
    strcpy (info->rev, vstr);
  else
    memcpy (info->rev, vstr, MAX_REV_LEN); /* only take up to MAX_REV_LEN  	*/
  info->rev[ MAX_REV_LEN ] = 0x00;        /* NULL term. end just in case   */


  _ms_set_cs_check (aCtx);			/* check for CS info		*/

/* if using IS, could have a list of abstract syntax's following	*/

#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,3,ident_start_as);
#else
  if (mmsl_version)				/* IS 	*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,3,ident_start_as);
#endif
  }

/************************************************************************/
/*			ident_start_as					*/
/* Identify response is being decoded: AS sequence detected		*/
/************************************************************************/

static ST_VOID ident_start_as (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ident_start_as");

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ident_as_seq_done;

  ASN1R_TAG_ADD (aCtx, UNI,OBJ_ID_CODE,ident_get_as);
  }

/************************************************************************/
/*			ident_get_as					*/
/* Identify response is being decoded: AS obj id found			*/
/************************************************************************/

static ST_VOID ident_get_as (ASN1_DEC_CTXT *aCtx)
  {
ST_INT new_size;
struct mms_obj_id *obj;

  MLOG_CDEC0 ("ident_get_as");

  new_size = sizeof (IDENT_RESP_INFO) + 
		(sizeof (struct mms_obj_id) * (info->num_as+1));

  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,new_size);
  info = (IDENT_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;
  obj = (struct mms_obj_id *) (info +1);	/* point to first	*/
  obj += info->num_as;				/* point to next spot	*/


  if (asn1r_get_objid (aCtx, obj->comps,&obj->num_comps))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE); 
    return;                          
    }

/* get ready for another in the sequence				*/
  ASN1R_TAG_ADD (aCtx, UNI,OBJ_ID_CODE,ident_get_as);
  info->num_as++;
  }

/************************************************************************/
/*			ident_as_seq_done				*/
/* Identify response is being decoded: AS sequence complete		*/
/************************************************************************/

static ST_VOID ident_as_seq_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ident_as_seq_done");

  _ms_set_cs_check (aCtx);			/* check for CS info		*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_ident					*/
/* This function is called from the user to ENCODE and send the 	*/
/* Identify request PDU.						*/
/************************************************************************/

MMSREQ_PEND *mp_ident (ST_INT chan)
  {
  return (_mms_send_null_req (chan,MMSOP_IDENTIFY));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_ident					*/
/* This function is called from the user to ENCODE and send the 	*/
/* Identify request PDU.						*/
/************************************************************************/

ST_RET mpl_ident (ST_VOID)
  {
  return (_mms_fin_null_req (MMSOP_IDENTIFY));
  }

#endif
