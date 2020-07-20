/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_ced6.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common primitive encode/decode functions	*/
/*	particular event time						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     03    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 08/15/97  MDE     01    BTOD handling changes			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"

/************************************************************************/
static ST_VOID evtime_get_time_of_day  (ASN1_DEC_CTXT *aCtx);
static ST_VOID evtime_get_time_seq_id (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_mms_evtime_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID evtime_get_undefined (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/*			global structures & functions			*/
/************************************************************************/

static EVENT_TIME       *ev_time_ptr;

static ST_RET _error_code;


/************************************************************************/
/************************************************************************/
/*			_ms_get_mms_evtime					*/
/* Function to get an Event Time from the received message.		*/
/************************************************************************/

ST_VOID _ms_get_mms_evtime (ASN1_DEC_CTXT *aCtx, EVENT_TIME *data_ptr, ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {
			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    _error_code = RESP_BAD_VALUE;
  else
    _error_code = REQ_BAD_VALUE;

  ev_time_ptr = data_ptr;
  get_mms_evtime_done = done_fun;

  ASN1R_TAG_ADD (aCtx, CTX, 0, evtime_get_time_of_day);
  ASN1R_TAG_ADD (aCtx, CTX, 1, evtime_get_time_seq_id);
#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
  if (mmsl_version)				/* IS 	*/
#endif
    ASN1R_TAG_ADD (aCtx, CTX, 2, evtime_get_undefined);
  }

/************************************************************************/
/*			evtime_get_time_of_day				*/
/* time of day was encountered. 					*/
/************************************************************************/

static ST_VOID evtime_get_time_of_day (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evtime_get_time_of_day");

  ev_time_ptr->evtime_tag = 0;		/* set tag to time of day	*/

					/* read time from message	*/
  if (asn1r_get_btod (aCtx, &ev_time_ptr->evtime.time_of_day))
    asn1r_set_dec_err (aCtx, _error_code);

  (*get_mms_evtime_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/*			evtime_get_time_seq_id				*/
/* time sequence identifier was encountered.				*/
/************************************************************************/

static ST_VOID evtime_get_time_seq_id (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evtime_get_time_seq_id");

  ev_time_ptr->evtime_tag = 1;		/* set tag to time seq id	*/

					/* read time from message	*/
  if (asn1r_get_u32 (aCtx, &ev_time_ptr->evtime.time_seq_id))
    asn1r_set_dec_err (aCtx, _error_code);

  (*get_mms_evtime_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/*			evtime_get_undefined 				*/
/* undefined was encountered.                				*/
/************************************************************************/

static ST_VOID evtime_get_undefined (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evtime_get_undefined");

  ev_time_ptr->evtime_tag = 2;		/* set tag to undefined      	*/

  (*get_mms_evtime_done) (aCtx);		/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_wr_mms_evtime					*/
/* Function to write an Event Time to the message.			*/
/************************************************************************/

ST_VOID _ms_wr_mms_evtime (ASN1_ENC_CTXT *aCtx, EVENT_TIME *data_ptr)
  {
  if (data_ptr->evtime_tag == 0)
    {					/* write the time of day	*/
    asn1r_wr_btod (aCtx, &data_ptr->evtime.time_of_day);
    asn1r_fin_prim (aCtx, 0, CTX);			/* context tag 0		*/
    }
  else if (data_ptr->evtime_tag == 1)
    {		  			/* write time sequence id	*/
    asn1r_wr_u32 (aCtx, data_ptr->evtime.time_seq_id);
    asn1r_fin_prim (aCtx, 1, CTX);			/* context tag 1		*/
    }
  else
    {					/* IS - write undefined (null)	*/
    asn1r_fin_prim (aCtx, 2, CTX);                  /* context tag 2                */
    }
  }

