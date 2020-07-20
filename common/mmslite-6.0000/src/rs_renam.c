/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_rename.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the Rename	*/
/*	service.  It decodes the Rename request (indication) and	*/
/*	encodes the Rename response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 04/28/00  JRB     04    Lint cleanup					*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  RENAME_REQ_INFO	*info;

static ST_VOID rename_ext_objclass      (ASN1_DEC_CTXT *aCtx);
static ST_VOID rename_get_objclass      (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID rename_get_cs_objclass   (ASN1_DEC_CTXT *aCtx);
static ST_VOID rename_cs_objclass_done  (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID rename_ext_objclass_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID rename_get_curname       (ASN1_DEC_CTXT *aCtx);
static ST_VOID rename_curname_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID rename_curname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID rename_get_new_ident     (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_rename_req					*/
/* This function is called from MMSDEC when the opcode of a request	*/
/* PDU for this operation is decoded.  Setup state machine function	*/
/* pointers, etc.							*/
/************************************************************************/

ST_VOID mms_rename_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Rename Request");

  info = (RENAME_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (RENAME_REQ_INFO));
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,rename_ext_objclass);
  }

/************************************************************************/
/*			    rename_ext_objclass				*/
/* Rename response is being decoded: object class parameter has		*/
/* been encountered.							*/
/************************************************************************/

static ST_VOID rename_ext_objclass (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_ext_objclass");

  ASN1R_TAG_ADD (aCtx, CTX,       0,rename_get_objclass);
#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,rename_get_cs_objclass);
#endif
  }

/************************************************************************/
/*			    rename_get_objclass				*/
/* Rename response is being decoded: object class parameter has		*/
/* been encountered.							*/
/************************************************************************/

static ST_VOID rename_get_objclass (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_get_objclass");
					/* read the object class	*/
  if (asn1r_get_i16 (aCtx, &info->obj.mms_class) ||
      info->obj.mms_class < 0 ||
      info->obj.mms_class > 11)
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = rename_ext_objclass_done;
  }

#ifdef CS_SUPPORT
/************************************************************************/
/*			    rename_get_cs_objclass  			*/
/* Rename response is being decoded: CS object class parameter has	*/
/* been encountered. The contents here are a ANY, defined by CS.	*/
/************************************************************************/

static ST_VOID rename_get_cs_objclass (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_get_cs_objclass");

  info->cs_objclass_pres = SD_TRUE;	/* set flag for CS object	*/
  info->obj.cs.len = aCtx->asn1r_elmnt_len;		/* save pointer and len		*/
  info->obj.cs.cs_class = aCtx->asn1r_field_ptr;

  asn1r_parse_next (aCtx, rename_cs_objclass_done);	/* ensure valid ASN.1 CS data	*/
  }

/************************************************************************/
/*			    rename_cs_objclass_done			*/
/************************************************************************/

static ST_VOID rename_cs_objclass_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_cs_objclass_done");
			/* OK for extended object class to be done	*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = rename_ext_objclass_done;
  }
#endif

/************************************************************************/
/*			    rename_ext_objclass_done  			*/
/* extendedObjectClass constructor done function. Set up to get Scope	*/
/************************************************************************/

static ST_VOID rename_ext_objclass_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_extobj_class_done");

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,rename_get_curname);	/* get current Name 	*/
  }

/************************************************************************/
/*			rename_get_curname 				*/
/* Rename response is being decoded: object name constructor has	*/
/* been encountered, set up to get the current name			*/
/************************************************************************/

static ST_VOID rename_get_curname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_get_curname");

  _ms_get_mms_objname (aCtx, &info->cur_name, rename_curname_done);
  }

/************************************************************************/
/*			rename_curname_done				*/
/* rename request is being decoded: current name has been obtained.	*/
/************************************************************************/

static ST_VOID rename_curname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_curname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rename_curname_cstr_done;
  }

/************************************************************************/
/*			rename_curname_cstr_done			*/
/* rename request is being decoded: current name constr done was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID rename_curname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_curname_cstr_done");

  ASN1R_TAG_ADD (aCtx, CTX,2,rename_get_new_ident);
  }

/************************************************************************/
/*			rename_get_new_ident				*/
/* Rename response is being decoded: new identifier parameter was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID rename_get_new_ident (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rename_get_new_ident");

  if (asn1r_get_identifier (aCtx, info->new_ident))		/* new identifier	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rename_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the rename response.			*/
/************************************************************************/

ST_RET mp_rename_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_RENAME));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rename_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the rename response.				*/
/************************************************************************/

ST_RET mpl_rename_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_RENAME));
  }
#endif
