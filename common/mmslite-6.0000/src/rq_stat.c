/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_stat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the functions to decode a status response	*/
/*	and encode a status request message.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     03    Converted to use ASN1R (re-entrant ASN1)	*/
/*			   Deleted mms_status proto (see mms_dfun.h)	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/************************************************************************/
/*			   mms_status_rsp				*/
/* This function is called from mmsdec to decode a Status response PDU	*/
/* after the PDU type has been determined.  It simply calls mms_status	*/
/* in the module mmsstat.c.						*/
/************************************************************************/

ST_VOID mms_status_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("decode status response");

  mms_status (aCtx);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_status					*/
/* This function is called from the user to ENCODE and send the 	*/
/* Status request PDU.							*/
/************************************************************************/

MMSREQ_PEND *mp_status (ST_INT chan, STATUS_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_STATUS,
			M_CAST_MK_FUN (_ms_mk_stat_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_status					*/
/* This function is called by the user to ENCODE the Status request	*/
/* PDU.									*/
/************************************************************************/

ST_RET mpl_status (STATUS_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_STATUS,
			M_CAST_MK_FUN (_ms_mk_stat_req),
			(ST_CHAR *)info_ptr));
  } 
#endif

/************************************************************************/
/************************************************************************/
/*			_ms_mk_stat_req					*/
/* ENCODE an Status REQUEST:						*/
/************************************************************************/

ST_VOID _ms_mk_stat_req (ASN1_ENC_CTXT *aCtx, STATUS_REQ_INFO *info_ptr)
  {

/* Write the ST_BOOLEAN value represented by the "extended" component of   */
/* the status_req_info structure (0 means false, nonzero means true).	*/

  if (info_ptr -> extended)
    asn1r_wr_bool (aCtx, 0x7F);
  else
    asn1r_wr_bool (aCtx, 0x00);

/* Write the context specific explicit tag for this opcode on the	*/
/* ST_BOOLEAN data element just written.					*/

  asn1r_fin_prim (aCtx, MMSOP_STATUS,CTX);		/* tag = opcode, context	*/
  }


