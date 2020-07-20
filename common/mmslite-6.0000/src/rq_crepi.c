/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_crepi.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of create program	*/
/*	invocation.  It decodes the create program invocation response	*/
/*	(confirm) & encodes the create program invocation request.	*/
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
#include "mms_pprg.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/************************************************************************/
/*			mms_create_pi_rsp				*/
/* NOTE: Since the mms_create_pi_rsp is a NULL PDU, the decode is 	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode 	*/
/* done fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_crepi					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the create program invocation request.	*/
/************************************************************************/

MMSREQ_PEND *mp_crepi (ST_INT chan, CREPI_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_CREATE_PI,
			M_CAST_MK_FUN (_ms_mk_crepi_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_crepi					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the create program invocation request.		*/
/************************************************************************/

ST_RET  mpl_crepi (CREPI_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_CREATE_PI,
			M_CAST_MK_FUN (_ms_mk_crepi_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_crepi_req					*/
/* construct a create program invocation request			*/
/************************************************************************/

ST_VOID _ms_mk_crepi_req (ASN1_ENC_CTXT *aCtx, CREPI_REQ_INFO *info)
  {
ST_INT	i;
ST_CHAR	**dom_ptr;

  if (info->monitor_pres)
    {
    asn1r_wr_bool (aCtx, info->monitor);		/* write the monitor flag	*/
    asn1r_fin_prim (aCtx, 3,CTX);			/* ctx tag 3			*/
    }

  if (!info->reusable)		/* SD_TRUE is default		*/
    {
    asn1r_wr_bool (aCtx, info->reusable);		/* write the re-usable flag	*/
    asn1r_fin_prim (aCtx, 2,CTX);			/* ctx tag 2			*/
    }
					/* Write list of Domain Names	*/
  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
  dom_ptr  = (ST_CHAR **) (info + 1);
  dom_ptr += (info->num_of_dnames - 1); /* point to last domain name	*/
  for (i = 0; i < info->num_of_dnames; i++)
    {
    asn1r_wr_vstr (aCtx, *dom_ptr);
    asn1r_fin_prim (aCtx, VISTR_CODE, UNI);
    dom_ptr--;
    }
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* ctx tag 1 constructor	*/

  asn1r_wr_vstr (aCtx, info->piname);		/* write pgm invocation name	*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* ctx tag 0			*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_CREATE_PI,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
