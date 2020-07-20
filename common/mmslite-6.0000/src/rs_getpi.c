/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_getpi.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of get program	*/
/*	invocation attribute.  It decodes the get program invocation	*/
/*	attribute request (indication) & encodes the get program	*/
/*	invocation attribute response.					*/
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
#include "mms_pprg.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETPI_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_get_pi_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_pi_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Program Invocation Attribute Request");


/* get storage for request information	*/
  req_info = (GETPI_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETPI_REQ_INFO));
  if (asn1r_get_identifier (aCtx, req_info->piname))/* read program invocation name */
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getpi_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get program invocation attribute	*/
/* response.								*/
/************************************************************************/

ST_RET mp_getpi_resp (MMSREQ_IND *indptr, GETPI_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_PI_ATTR,indptr,
			M_CAST_MK_FUN (_ms_mk_getpi_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getpi_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get program invocation attribute response.	*/
/************************************************************************/

ST_RET mpl_getpi_resp (ST_UINT32 invoke_id, GETPI_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_PI_ATTR,invoke_id,
			M_CAST_MK_FUN (_ms_mk_getpi_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getpi_resp					*/
/* ENCODE a get program invocation attribute RESPONSE:			*/
/************************************************************************/

ST_VOID _ms_mk_getpi_resp (ASN1_ENC_CTXT *aCtx, GETPI_RESP_INFO *info)
{
ST_INT 	i;
ST_CHAR	**dom_ptr;

  if (info->start_arg_type == ARG_TYPE_SIMPLE)
    {
    asn1r_wr_vstr (aCtx, (ST_CHAR *)info->start_arg);	/* write the start argument	*/
    asn1r_fin_prim (aCtx, 5,CTX);			/* ctx tag 5			*/
    }
  else		/* ENCODED data, write as complete ASN.1 EXTERNAL     	*/
    asn1r_wr_delmnt (aCtx, info->start_arg,info->start_arg_len);

  asn1r_wr_bool (aCtx, info->monitor);		/* write the monitor flag	*/
  asn1r_fin_prim (aCtx, 4,CTX);			/* ctx tag 4			*/

  asn1r_wr_bool (aCtx, info->reusable);		/* write the reusable flag	*/
  asn1r_fin_prim (aCtx, 3,CTX);			/* ctx tag 3			*/

  asn1r_wr_bool (aCtx, info->mms_deletable);	/* write mms deletable flag	*/
  asn1r_fin_prim (aCtx, 2,CTX);			/* ctx tag 2			*/

  asn1r_strt_constr (aCtx);			/* start ctx tag 2 constructor	*/
  dom_ptr  = (ST_CHAR **) (info + 1);
  dom_ptr += (info->num_of_dnames - 1); /* point to last domain name	*/
  for (i = 0; i < info->num_of_dnames; i++)
    {
    asn1r_wr_vstr (aCtx, *dom_ptr);
    asn1r_fin_prim (aCtx, VISTR_CODE, UNI);
    dom_ptr--;
    }
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* ctx tag 1 constructor	*/

  asn1r_wr_i16 (aCtx, info->state); 		/* write the state		*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* ctx tag 0			*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_PI_ATTR,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
