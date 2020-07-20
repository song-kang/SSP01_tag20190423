/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_getdom.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of get domain	*/
/*	attribute.  It decodes the get domain attribute request 	*/
/*	(indication) & encodes the get domain attribute response.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     06    Converted to use ASN1R (re-entrant ASN1)	*/
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
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETDOM_REQ_INFO	*req_info;


/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_get_dom_attr_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_dom_attr_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Domain Attribute Request");


/* get storage for request information	*/
  req_info = (GETDOM_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETDOM_REQ_INFO));
  if (asn1r_get_identifier (aCtx, req_info->dname)) 	/* read the domain name */
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getdom_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get domain attribute response.	*/
/************************************************************************/
 
ST_RET mp_getdom_resp (MMSREQ_IND *indptr, GETDOM_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_DOM_ATTR,indptr,
			M_CAST_MK_FUN (_ms_mk_getdom_resp),
			(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getdom_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get domain attribute response.			*/
/************************************************************************/
 
ST_RET mpl_getdom_resp (ST_UINT32 invoke_id, GETDOM_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_DOM_ATTR,invoke_id,
			M_CAST_MK_FUN (_ms_mk_getdom_resp),
			(ST_CHAR *)info_ptr));
  } 
#endif

  
/************************************************************************/
/************************************************************************/
/*			_ms_mk_getdom_resp					*/
/* ENCODE a get domain attribute RESPONSE:				*/
/************************************************************************/

ST_VOID _ms_mk_getdom_resp (ASN1_ENC_CTXT *aCtx, GETDOM_RESP_INFO *info)
  {
ST_INT	 i;
ST_CHAR	**cap_ptr;
ST_CHAR	**pin_ptr;

  asn1r_wr_i8 (aCtx, info->upload_in_progress);	/* write upload in progress	*/
  asn1r_fin_prim (aCtx, 5,CTX);			/* ctx tag 5			*/

  asn1r_strt_constr (aCtx);			/* start ctx tag 4 constructor	*/
  cap_ptr  = (ST_CHAR **) (info + 1);
  cap_ptr += (info->num_of_capab - 1);	/* point to last capability	*/
  pin_ptr  = cap_ptr + 1;
  pin_ptr += (info->num_of_pinames-1);	/* point to last pi name	*/
  for (i = 0; i < info->num_of_pinames; i++)
    {
    asn1r_wr_vstr (aCtx, *pin_ptr);
    asn1r_fin_prim (aCtx, VISTR_CODE, UNI);
    pin_ptr--;
    }
  asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* ctx tag 4 constructor	*/

  asn1r_wr_bool (aCtx, info->sharable);		/* write sharable		*/
  asn1r_fin_prim (aCtx, 3,CTX);			/* ctx tag 3			*/

  asn1r_wr_bool (aCtx, info->mms_deletable);	/* write mms deletable		*/
  asn1r_fin_prim (aCtx, 2,CTX);			/* ctx tag 2			*/

  asn1r_wr_i16 (aCtx, info->state); 		/* write state			*/
  asn1r_fin_prim (aCtx, 1,CTX);			/* ctx tag 1			*/

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  for (i = 0; i < info->num_of_capab; i++)
    {
    asn1r_wr_vstr (aCtx, *cap_ptr);
    asn1r_fin_prim (aCtx, VISTR_CODE, UNI);
    cap_ptr--;
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* ctx tag 0 constructor	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_DOM_ATTR,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
