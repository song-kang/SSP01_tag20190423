/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_ustat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the functions to encode an unsolicited	*/
/*	status request. 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/28/05  JRB     05    Incl suicacse if MMS_LITE not defined.	*/
/* 09/17/04  EJV     04    Use new mms_chk_resource.			*/
/* 12/12/02  JRB     03    Add ASN1_ENC_CTXT arg to mpl_ustatus.	*/ 
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"

#ifndef MMS_LITE
#include "mms_pvmd.h"
#include "suicacse.h"	/* for SE_QUE_FULL only	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/*			  mp_ustatus					*/
/* This function is called from the user to ENCODE and send the 	*/
/* Unsolicited Status request PDU.					*/
/************************************************************************/

ST_RET mp_ustatus (ST_INT chan, USTATUS_REQ_INFO *info_ptr)
  {
ST_RET rc;
ST_INT avail;

  /* locking here will assure that _mms_chk_resource will work in	*/
  /* multiple threads when mp_* functions are called to send		*/
  /* unconfirmed requests						*/
  S_LOCK_COMMON_RESOURCES ();

  avail = _mms_chk_resource (chan);
  if (avail > 0)
    rc = _mms_unconf_req_send (MMSOP_USTATUS,
			M_CAST_MK_FUN (_ms_mk_ustat_req),
			chan,(ST_CHAR *)info_ptr);
  else
    rc = SE_QUE_FULL;

  S_UNLOCK_COMMON_RESOURCES ();

  return (rc);
  } 

#else
/************************************************************************/
/************************************************************************/
/*			  mpl_ustatus					*/
/* This function is called by the user to ENCODE an Unsolicited Status	*/
/* request PDU.								*/
/************************************************************************/

ST_RET mpl_ustatus (ASN1_ENC_CTXT *aCtx, USTATUS_REQ_INFO *info_ptr)
  {
  return (_mms_unconf_req_fin (aCtx, MMSOP_USTATUS,
			M_CAST_MK_FUN (_ms_mk_ustat_req),
			(ST_CHAR *)info_ptr));
  } 
#endif

/************************************************************************/
/************************************************************************/
/*			_ms_mk_ustat_req					*/
/* ENCODE an Status RESPONSE:						*/
/************************************************************************/

ST_VOID _ms_mk_ustat_req (ASN1_ENC_CTXT *aCtx, USTATUS_REQ_INFO *info)
  {

  if (info->local_detail_pres)		/* if present, write the local	*/
    {					/* detail parameter		*/
    asn1r_wr_bitstr (aCtx, info->local_detail,info->local_detail_len);
    asn1r_fin_prim (aCtx, 2,CTX);
    }

  asn1r_wr_i16 (aCtx, info->physical_stat); 	/* write the physical status	*/
  asn1r_fin_prim (aCtx, 1,CTX);

  asn1r_wr_i16 (aCtx, info->logical_stat);		/* write the logical status	*/
  asn1r_fin_prim (aCtx, 0,CTX);

/* Write the context-specific tag for this unconfirmed request service. */
  asn1r_fin_constr (aCtx, 1,CTX,DEF);
  }

