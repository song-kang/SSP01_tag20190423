/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsstat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the common functions to perform the decode */
/*	of a status response or an unsolicited status request, which	*/
/*	have identical syntax.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 03/31/03  JRB     05    asn1r_get_bitstr: add max_bits arg.		*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/

static STATUS_RESP_INFO *info;

static ST_VOID local_detail_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_local_detail_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_local_detail_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_physical_stat (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_logical_stat (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			     mms_status 				*/
/* This function is called from mms_status_rsp (in rq_stat.c) or from	*/
/* mms_ustatus_req (in rs_ustat.c) to decode either a Status response	*/
/* PDU or an UnsolicitedStatus request PDU after the PDU type has been	*/
/* determined.	It sets up the parse of the next data element and	*/
/* returns, letting the ASN.1 tools take over.				*/
/************************************************************************/

ST_VOID mms_status (ASN1_DEC_CTXT *aCtx)
  {
/* Create status_resp_info structure data area to put decoded info into.*/
  info = (STATUS_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (STATUS_RESP_INFO));

/* (NOTE: A structure of typp_info is excactly the same as	*/
/* a structure of type ustat so this logic works with both	*/

  ASN1R_TAG_ADD (aCtx, CTX,0,get_logical_stat);/* Set up to parse 1st param.*/
  }


/************************************************************************/
/*			get_logical_stat				*/
/* The logical status parameter has been encountered.  Read its value.	*/
/************************************************************************/

static ST_VOID get_logical_stat (ASN1_DEC_CTXT *aCtx)
  {
ST_INT high_lim;

  MLOG_CDEC0 ("get_logical_stat");

  if (asn1r_get_i16 (aCtx, &info->logical_stat))	/* Read logical status. 	*/
    {					/* If error reading integer,	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);		/* stop parse, report error	*/
    return;				/* and return.			*/
    }

#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
  if (mmsl_version)		/* IS 	*/
#endif
    high_lim = 3;
  else			     			/* DIS	*/
    high_lim = 2;
  
  if ((info->logical_stat < 0) || (info->logical_stat > high_lim))
    {					/* validate range of values 	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

/* The next data element expected is the physical status.		*/
  ASN1R_TAG_ADD (aCtx, CTX,1,get_physical_stat);
  }


/************************************************************************/
/*			get_physical_stat				*/
/* The physical status parameter has been encountered.	Read its value. */
/************************************************************************/

static ST_VOID get_physical_stat (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_physical_stat");

/* Get the value of the physical status parameter.			*/

  if (asn1r_get_i16 (aCtx, &info->physical_stat))	/* Read physical status.	*/
    {					/* If error reading integer,	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);		/* stop parse, report error	*/
    return;				/* and return.			*/
    }

  if ((info->physical_stat < 0) || (info->physical_stat > 3))
    {					/* validate range [0..3]	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

/* The next data element is either the local detail parameter or the	*/
/* additional detail parameter, or there may be no other data elements. */

  ASN1R_TAG_ADD (aCtx, CTX,2,get_local_detail_prim);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,get_local_detail_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


/************************************************************************/
/*		      get_local_detail_prim				*/
/* The local detail parameter has been encountered as a primitive data	*/
/* element.  Read its value.						*/
/************************************************************************/

static ST_VOID get_local_detail_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_local_detail_prim");

/* Get the value of the local detail parameter. 			*/

  if (aCtx->asn1r_elmnt_len <= MAX_IDENT_LEN)/* check length before moving data	*/
    {
    if (!asn1r_get_bitstr (aCtx, info->local_detail, sizeof(info->local_detail)*8))
      { 			  /* If everything is kosher,		*/
      local_detail_done (aCtx);	  /* set up for the next data element.	*/
      return;			  /* Return success and continue parse. */
      }
    }

/* Problem getting value - stop parse and report error. 		*/

  asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  return;
  }

/************************************************************************/
/*		     get_local_detail_cstr				*/
/* The local detail parameter has been encountered as a constructor	*/
/* data element.  Read its value.					*/
/************************************************************************/

static ST_VOID get_local_detail_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_local_detail_cstr");

/* Set up to call this function when done with constructor bitstring.	*/

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = local_detail_done;

/* Get the value of the local detail parameter.  For constructor	*/
/* bitstrings, this requires returning and letting the ASN.1 tools	*/
/* parse it to any nesting level.					*/

  asn1r_get_bitstr_cstr (aCtx, 16,info->local_detail);
  }


/************************************************************************/
/*			local_detail_done				*/
/* Function called after the local_detail parameter has been parsed.	*/
/************************************************************************/

static ST_VOID local_detail_done (ASN1_DEC_CTXT *aCtx)
  {

  info -> local_detail_pres = SD_TRUE;
  info -> local_detail_len = aCtx->asn1r_bitcount;

/* The next data element, if there is one, is the additional detail	*/
/* parameter.								*/

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }
