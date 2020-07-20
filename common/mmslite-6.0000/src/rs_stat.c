/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_stat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the functions to decode a status request	*/
/*	and encode a status response message.				*/
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
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/

static  STATUS_REQ_INFO	*info;


/************************************************************************/
/************************************************************************/
/*			   mms_status_req				*/
/* This function is called from mmsdec to decode a Status request PDU	*/
/* after the PDU type has been determined.  It sets up the parse of	*/
/* the next data element and returns, letting the ASN.1 tools take over.*/
/************************************************************************/

ST_VOID mms_status_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("decode status request");

  info = (STATUS_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (STATUS_REQ_INFO));
  if (asn1r_get_bool (aCtx, &info -> extended))	/* Read extended flag.		*/
    {					/* If error reading boolean,	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);		/* stop parse, report error	*/
    return;				/* and return.			*/
    }

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_status_resp					*/
/* This function is called from the user to ENCODE and send the 	*/
/* Status response PDU.							*/
/************************************************************************/

ST_RET mp_status_resp (MMSREQ_IND *indptr, STATUS_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_STATUS,indptr,
			M_CAST_MK_FUN (_ms_mk_stat_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_status_resp					*/
/* This function is called by the user to ENCODE the Status response	*/
/* PDU.									*/
/************************************************************************/

ST_RET mpl_status_resp (ST_UINT32 invoke_id, STATUS_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_STATUS,invoke_id,
			M_CAST_MK_FUN (_ms_mk_stat_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_stat_req					*/
/* ENCODE a Status RESPONSE:						*/
/************************************************************************/

ST_VOID _ms_mk_stat_resp (ASN1_ENC_CTXT *aCtx, STATUS_RESP_INFO *info_ptr)
  {
  if (info_ptr->local_detail_pres)     	/* if present, write the local	*/
    {					/* detail parameter		*/
    asn1r_wr_bitstr (aCtx, info_ptr->local_detail,info_ptr->local_detail_len);
    asn1r_fin_prim (aCtx, 2,CTX);
    }

  asn1r_wr_i16 (aCtx, info_ptr->physical_stat); 	/* write the physical status	*/
  asn1r_fin_prim (aCtx, 1,CTX);

  asn1r_wr_i16 (aCtx, info_ptr->logical_stat);     	/* write the logical status	*/
  asn1r_fin_prim (aCtx, 0,CTX);

/* Write the context-specific tag for this opcode.			*/
  asn1r_fin_constr (aCtx, MMSOP_STATUS,CTX,DEF);
  }
