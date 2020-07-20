/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_input.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the input 	*/
/*	operation.  It decodes the input request (indication) and	*/
/*	encodes the input response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
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

#include "mmsdefs.h"
#include "mms_pocs.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  INPUT_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_input_rsp					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_input_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Input Response");


/* get storage for request information	*/
  rsp_info = (INPUT_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (INPUT_RESP_INFO));
  rsp_info->input_resp = (ST_CHAR *) aCtx->asn1r_field_ptr - 1; /* set ptr to in rsp*/
  if (asn1r_get_vstr (aCtx, rsp_info->input_resp))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_input					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the input request.			*/
/************************************************************************/

MMSREQ_PEND *mp_input (ST_INT chan, INPUT_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_INPUT,
			M_CAST_MK_FUN (_ms_mk_input_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_input					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the input request.					*/
/************************************************************************/

ST_RET  mpl_input (INPUT_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_INPUT,
			M_CAST_MK_FUN (_ms_mk_input_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_input_req					*/
/* ENCODE an input REQUEST:						*/
/************************************************************************/

ST_VOID _ms_mk_input_req (ASN1_ENC_CTXT *aCtx, INPUT_REQ_INFO *info)
  {
ST_INT	i;
ST_CHAR	**prompt_data;

  asn1r_strt_constr (aCtx);			/* start sequence constructor	*/

  if (info->timeout_pres)
    {
    asn1r_wr_u32 (aCtx, info->timeout);		/* write input timeout		*/
    asn1r_fin_prim (aCtx, 3,CTX);			/* context specific tag 3	*/
    }
  
  if (info->prompt_pres)
    {
    asn1r_strt_constr (aCtx);			/* start prompt data list cstr	*/
    prompt_data  = (ST_CHAR **) (info + 1);/* point to first prompt data	*/
    prompt_data += info->prompt_count -1; /* point to last prompt data	*/
    for (i = 0; i < info->prompt_count; i++)
      {
      asn1r_wr_vstr (aCtx, *prompt_data);		/* write the prompt data	*/
      asn1r_fin_prim (aCtx, VISTR_CODE,UNI);	/* universal string		*/
      prompt_data--;
      }
    asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* context specific tag 2	*/
    }

  if (!info->echo)
    {					/* different from default	*/
    asn1r_wr_bool (aCtx, info->echo); 		/* write the echo		*/
    asn1r_fin_prim (aCtx, 1,CTX);			/* context tag 1		*/
    }

  asn1r_wr_vstr (aCtx, info->station_name); 	/* write the station name	*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_INPUT,CTX,DEF);		/* tag = opcode, ctx constr	*/
  }
