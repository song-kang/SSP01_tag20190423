/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_repeas.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the report event	*/
/*	action status.  It decodes the report event action status	*/
/*	response (confirm) & encodes the report event action status 	*/
/*	request. 							*/
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
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  REPEAS_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_repeas_rsp 					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_repeas_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Event Action Status Response");

/* get storage for request information	*/
  rsp_info = (REPEAS_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (REPEAS_RESP_INFO));

  if (asn1r_get_u32 (aCtx, &rsp_info->num_of_ev_enroll))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  else
    {
    _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
    }
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_repeas					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the report event action status request.	*/
/************************************************************************/

MMSREQ_PEND *mp_repeas (ST_INT chan, REPEAS_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REP_EA_STAT,
			M_CAST_MK_FUN (_ms_mk_repeas_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_repeas					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report event action status request.		*/
/************************************************************************/

ST_RET  mpl_repeas (REPEAS_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REP_EA_STAT,
			M_CAST_MK_FUN (_ms_mk_repeas_req),
			(ST_CHAR *)info_ptr));
  } 

#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_repeas_req					*/
/* construct a get event action attributes request			*/
/************************************************************************/

ST_VOID _ms_mk_repeas_req (ASN1_ENC_CTXT *aCtx, REPEAS_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->evact_name);	/* write event action name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_EA_STAT,CTX,DEF); 	/* tag = opcode, ctx		*/
  }
