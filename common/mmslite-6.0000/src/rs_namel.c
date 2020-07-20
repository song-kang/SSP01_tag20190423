/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_namel.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the GetNameList	*/
/*	service.  It decodes the GetNameList request (indication) and	*/
/*	encodes the GetNameList response.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     10    Remove "thisFileName"                    	*/
/* 07/01/02  EJV     09    NLV: support namelist for dom specific jou.	*/ 
/* 12/20/01  JRB     08    Converted to use ASN1R (re-entrant ASN1)	*/
/* 05/05/00  JRB     07    Lint cleanup					*/
/* 04/07/00  RKR     06    Added code to check for illegal scope & class*/
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
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  NAMELIST_REQ_INFO	*info;

/* static functions 							*/
static ST_VOID namelist_get_extobjclass   (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_get_objclass      (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID namelist_get_cs_objclass   (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_cs_objclass_done  (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID namelist_extobj_class_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_get_objscope      (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_get_vmdscope      (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_get_domscope      (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_get_aascope       (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_objscope_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_get_continue_after (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_namelist_req				*/
/* This function is called from MMSDEC when the opcode of a request	*/
/* PDU for this operation is decoded.  Setup state machine function	*/
/* pointers, etc.							*/
/************************************************************************/

ST_VOID mms_namelist_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("GetNameList Request");


/* get storage for request information	*/
  info = (NAMELIST_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (NAMELIST_REQ_INFO));

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,namelist_get_extobjclass);
  }

/************************************************************************/
/*			    namelist_get_extobjclass			*/
/* GetNameList response is being decoded: object class parameter has	*/
/* been encountered.							*/
/************************************************************************/

static ST_VOID namelist_get_extobjclass (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_extobjclass");

  ASN1R_TAG_ADD (aCtx, CTX,0,namelist_get_objclass);
#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,namelist_get_cs_objclass);
#endif
  }

/************************************************************************/
/*			    namelist_get_objclass			*/
/* GetNameList response is being decoded: object class parameter has	*/
/* been encountered.							*/
/************************************************************************/

static ST_VOID namelist_get_objclass (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_objclass");
					/* read the object class	*/
  if (asn1r_get_i16 (aCtx, &info->obj.mms_class) ||
      info->obj.mms_class < 0 ||
      info->obj.mms_class > 11)
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = namelist_extobj_class_done;
  }

#ifdef CS_SUPPORT
/************************************************************************/
/*			    namelist_get_cs_objclass			*/
/* GetNameList response is being decoded: CS object class parameter has	*/
/* been encountered. The contents here are a ANY, defined by CS.	*/
/************************************************************************/

static ST_VOID namelist_get_cs_objclass (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_cs_objclass");

  info->cs_objclass_pres = SD_TRUE;		/* set flag for CS object	*/
  info->obj.cs.len = aCtx->asn1r_elmnt_len;		/* save pointer and len		*/
  info->obj.cs.cs_class = aCtx->asn1r_field_ptr;

  asn1r_parse_next (aCtx, namelist_cs_objclass_done);	/* ensure valid ASN.1 CS data	*/
  }

/************************************************************************/
/*			    namelist_cs_objclass_done			*/
/************************************************************************/

static ST_VOID namelist_cs_objclass_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_cs_objclass_done");
			/* OK for extended object class to be done	*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = namelist_extobj_class_done;
  }
#endif

/************************************************************************/
/*			    namelist_extobj_class_done			*/
/* extendedObjectClass constructor done function. Set up to get Scope	*/
/************************************************************************/

static ST_VOID namelist_extobj_class_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_extobj_class_done");

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,namelist_get_objscope);	/* get Object Scope		*/
  }

/************************************************************************/
/*			namelist_get_objscope				*/
/* GetNameList response is being decoded: object scope constructor has	*/
/* been encountered, set up to get scope				*/
/************************************************************************/

static ST_VOID namelist_get_objscope (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_objscope");

/* This constructor contains one of three data elements, where the tag	*/
/* is the scope identifier.						*/

  ASN1R_TAG_ADD (aCtx, CTX,0,namelist_get_vmdscope);
  ASN1R_TAG_ADD (aCtx, CTX,1,namelist_get_domscope);
  ASN1R_TAG_ADD (aCtx, CTX,2,namelist_get_aascope);
  }

/************************************************************************/
/*			namelist_get_vmdscope				*/
/* VMD scope, must be null						*/
/************************************************************************/

static ST_VOID namelist_get_vmdscope (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_vmdscope");

  if (aCtx->asn1r_elmnt_len) 			/* if not NULL			*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  
  info->objscope = VMD_SPEC;		/* set scope 			*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = namelist_objscope_done;
  }

/************************************************************************/
/*			namelist_get_domscope				*/
/* VMD scope, must be null						*/
/************************************************************************/

static ST_VOID namelist_get_domscope (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_domscope");

  if ((asn1r_get_identifier (aCtx, info->dname)) ||		/* read the domain name */
      (info->obj.mms_class == MMS_CLASS_DOM) ||
      (info->obj.mms_class == MMS_CLASS_PI) ||
      (info->obj.mms_class == MMS_CLASS_OPER_STA))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  info->objscope = DOM_SPEC;		/* set scope 			*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = namelist_objscope_done;
  }

/************************************************************************/
/*			namelist_get_aascope				*/
/* VMD scope, must be null						*/
/************************************************************************/

static ST_VOID namelist_get_aascope (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_aascope");

  if ((aCtx->asn1r_elmnt_len) ||		/* if not NULL			*/
      (info->obj.mms_class == MMS_CLASS_DOM) ||
      (info->obj.mms_class == MMS_CLASS_PI) ||
      (info->obj.mms_class == MMS_CLASS_OPER_STA) ||
      (info->obj.mms_class == MMS_CLASS_SEM))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  info->objscope = AA_SPEC;		/* set scope 			*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = namelist_objscope_done;
  }

/************************************************************************/
/*			    namelist_objscope_done			*/
/* object scope constructor complete, now the message could be done.	*/
/************************************************************************/

static ST_VOID namelist_objscope_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_objscope_done");

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ASN1R_TAG_ADD (aCtx, CTX,2,namelist_get_continue_after);
  }

/************************************************************************/
/*			namelist_get_continue_after			*/
/* GetNameList response is being decoded: continue after parameter was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID namelist_get_continue_after (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_continue_after");

  info->cont_after_pres = SD_TRUE; 		/* read the continue	*/
  if (asn1r_get_identifier (aCtx, info->continue_after))	/* after parameter	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

/* There are no other data elements in the message.  Indicate that the	*/
/* end of the message should have been reached, and that all construc-	*/
/* tors should have ended at this point.				*/

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_namelist_resp				*/
/* This function is called from the user to ENCODE and send the 	*/
/* GetNameList response PDU.						*/
/************************************************************************/

ST_RET mp_namelist_resp (MMSREQ_IND *indptr, NAMELIST_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_NAMLIST,indptr,
			M_CAST_MK_FUN (_ms_mk_namel_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_namelist_resp				*/
/* This function is called by the user to ENCODE a GetNameList response	*/
/* PDU.									*/
/************************************************************************/

ST_RET mpl_namelist_resp (ST_UINT32 invoke_id, NAMELIST_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_NAMLIST,invoke_id,
			M_CAST_MK_FUN (_ms_mk_namel_resp),
			(ST_CHAR *)info_ptr));
  }
#endif

/************************************************************************/
/************************************************************************/
/*			_ms_mk_namel_resp					*/
/* Construct a GetNameList RESPONSE:					*/
/************************************************************************/

ST_VOID _ms_mk_namel_resp (ASN1_ENC_CTXT *aCtx, NAMELIST_RESP_INFO *info_ptr)
  {
ST_CHAR	**nptr; 			/* Ptr to array of name ptrs	*/
ST_INT i;

  if (!info_ptr -> more_follows)	/* if not default value	(SD_TRUE)	*/
    {
    asn1r_wr_bool (aCtx, info_ptr -> more_follows);	/* write end of list boolean	*/
    asn1r_fin_prim (aCtx, 1,CTX);
    }

  asn1r_strt_constr (aCtx);			/* start the 'seqof' names cstr */

/* Calculate the address of the first name pointer.			*/

  nptr = (ST_CHAR **)(info_ptr + 1);
  nptr += (info_ptr->num_names - 1);	/* point to last name		*/

  for (i=0;i < info_ptr -> num_names;i++)
    {
    asn1r_wr_vstr (aCtx, *nptr);			/* write the list of names	*/
    asn1r_fin_prim (aCtx, VISTR_CODE,UNI);
    nptr--;
    }

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* complete sequence of names	*/
  asn1r_fin_constr (aCtx, MMSOP_GET_NAMLIST,CTX,DEF); /* complete GetNameList resp	*/
  }
