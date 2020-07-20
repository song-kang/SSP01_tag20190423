/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_output.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the output	*/
/*	operation.  It decodes the output response (confirm) & encodes	*/
/*	the output request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pocs.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/************************************************************************/
/*			  mms_output_rsp			        */
/* NOTE : Since the mms_output_rsp is a NULL PDU, the decode is		*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_output					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the output request.			*/
/************************************************************************/

MMSREQ_PEND *mp_output (ST_INT chan, OUTPUT_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_OUTPUT,
			M_CAST_MK_FUN (_ms_mk_output_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_output					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the output request.				*/
/************************************************************************/

ST_RET  mpl_output (OUTPUT_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_OUTPUT,
			M_CAST_MK_FUN (_ms_mk_output_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_output_req					*/
/* construct an output request						*/
/************************************************************************/

ST_VOID _ms_mk_output_req (ASN1_ENC_CTXT *aCtx, OUTPUT_REQ_INFO *info)
  {
ST_INT	i;
ST_CHAR	**output_data;

  asn1r_strt_constr (aCtx);			/* start sequence constr	*/
  output_data  = (ST_CHAR **) (info + 1);	/* point to first output string */
  output_data += info->data_count - 1;	/* point to last output string	*/
  for (i = 0; i < info->data_count; i++)
    {
    asn1r_wr_vstr (aCtx, *output_data);		/* write the output data	*/
    asn1r_fin_prim (aCtx, VISTR_CODE,UNI);		/* universal string		*/
    output_data--;			/* point to previous string	*/
    }
  asn1r_fin_constr (aCtx, 1, CTX, DEF);		/* context tag 1		*/

  asn1r_wr_vstr (aCtx, info->station_name); 	/* write the station name	*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* universal string		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_OUTPUT,CTX,DEF);		/* tag = opcode, ctx constr	*/
  }
