#if defined(UCA_SMP)
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1997 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smp_dec.c						*/
/* PRODUCT(S)  : UCA Station Management Protocol (UCA SMP).		*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module implements the decoding of the UCA Station		*/
/*	Management Protocol PDUs.					*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/21/03  JRB    09     Del dummy func if !defined(UCA_SMP).		*/
/* 04/24/03  JRB    08     smp_dec_uca_pdu: fix uninitialized struct.	*/
/* 01/09/02  EJV    07     Changed to use ASN1R (re-entrant ASN1)	*/
/* 04/27/00  MDE    06     Lint cleanup					*/
/* 09/13/99  MDE    05     Added SD_CONST modifiers			*/
/* 08/13/98  JRB    04     Lint cleanup.				*/
/* 03/18/98  JRB    03     Use defines (not variables) for ASN1 errors.	*/
/* 11/14/97  EJV    02     Changed the decoding of Sync-ResponsePDU	*/
/*			   from IMPLICIT INTEGER to INTEGERS		*/
/* 09/19/97  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#include "asn1defs.h"
#include "smp_log.h"
#include "smp_usr.h"
#include "smp.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif


	/*----------------------------------------------*/
	/* decoded fields from UCA PDU 			*/
	/*----------------------------------------------*/

ST_UCHAR smp_uca_pdu_type;

	/*----------------------------------------------*/
	/* decoded fields from ErrorPDU 		*/
	/*----------------------------------------------*/

ST_UCHAR smp_err_code;

	/*----------------------------------------------*/
	/* decoded fields from TimeSyncPDU 		*/
	/*----------------------------------------------*/

ST_UCHAR smp_timesync_pdu_type;

	/*----------------------------------------------*/
	/* decoded fields from Sync-RequestPDU		*/
	/*----------------------------------------------*/

SMP_TIME smp_time_req;
SMP_TIME smp_time_rsp;
ST_UCHAR smp_auth [SMP_SIZE_OF_AUTH];	/* authentication */

	/*----------------------------------------------*/
	/* decoded fields from Sync-ResponsePDU		*/
	/*----------------------------------------------*/

SMP_TIME smp_delta_time;

	/*----------------------------------------------*/
	/* decoded fields from Sync-ErrorPDU 		*/
	/*----------------------------------------------*/

ST_UCHAR smp_sync_err_code;

	/*----------------------------------------------*/
	/* Local vars and functions			*/
	/*----------------------------------------------*/

static ST_UINT smp_asn1_dec_err;

static ST_VOID smp_dec_asn1_err (ASN1_DEC_CTXT *aCtx, ST_RET asn1_err);

static ST_VOID       smp_dec_err_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID START_smp_dec_time_sync_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_measure_req_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_measure_rsp_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID START_smp_dec_sync_req_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_req_request_time (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_req_response_time (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_req_request_usec (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_req_response_usec (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_req_authentication (ASN1_DEC_CTXT *aCtx);
static ST_VOID   END_smp_dec_sync_req_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID START_smp_dec_sync_rsp_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_rsp_delta_days (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_rsp_delta_msec (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_rsp_delta_usec (ASN1_DEC_CTXT *aCtx);
static ST_VOID   END_smp_dec_sync_rsp_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_sync_err_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID       smp_dec_prepare_pdu (ASN1_DEC_CTXT *aCtx);
static ST_VOID   END_smp_dec_time_sync_pdu (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/*			smp_dec_uca_pdu					*/
/*----------------------------------------------------------------------*/
/* Decode a UCA Station Management Protocol PDU.			*/
/************************************************************************/
ST_RET smp_dec_uca_pdu (ST_UCHAR *pdu_buf, ST_UINT16  pdu_len)
{
ASN1_DEC_CTXT aLocalCtx = {0};		/* CRITICAL: start clean	*/
ASN1_DEC_CTXT *aCtx = &aLocalCtx;

  /* init dec output variables */
  smp_uca_pdu_type = (ST_UCHAR) SMP_PDU_INVALID_TYPE;
  smp_timesync_pdu_type = (ST_UCHAR) SMP_PDU_INVALID_TYPE;
  smp_asn1_dec_err = 0;

  /* decode received PDU */
  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;
  aCtx->asn1r_decode_done_fun = asn1r_done_err;	/* decoding not done		*/
  aCtx->asn1r_err_fun = smp_dec_asn1_err;	/* set up error detected fun 	*/

  ASN1R_TAG_ADD (aCtx, CTX         , 0,       smp_dec_err_pdu);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, START_smp_dec_time_sync_pdu);
  asn1r_decode_asn1 (aCtx, pdu_buf, (ST_UINT) pdu_len);

  /* PDU decoded ? */
  if (smp_asn1_dec_err)
    {
    SMP_LOG_ERR1 ("SMP-ERROR: ASN1 decoding failed (error=%u)", smp_asn1_dec_err);

    if (smp_asn1_dec_err == (ST_UINT) ASN1E_UNEXPECTED_TAG)
      /* unrecognized UCA-MgmtPDU or TimeSyncPDU */
      smp_err_code = (ST_UCHAR) SMP_ERR_UNRECOGNIZED_SERVICE;
    else
      /* PDU not encoded properly */ 
      smp_err_code = (ST_UCHAR) SMP_ERR_PDU_ERROR;	
    return (SD_FAILURE);
    }
  else
    return (SD_SUCCESS);
}


/************************************************************************/
/*			smp_dec_asn1_err				*/
/*----------------------------------------------------------------------*/
/* ASN1 decoding of received UCA PDU failed, send ErrorPDU.		*/
/************************************************************************/
static ST_VOID smp_dec_asn1_err (ASN1_DEC_CTXT *aCtx, ST_RET asn1_err)
{
  smp_asn1_dec_err = (ST_UINT) asn1_err;
}


/************************************************************************/
/*			smp_dec_err_pdu					*/
/*----------------------------------------------------------------------*/
/* UCA Error PDU received.						*/
/************************************************************************/
static ST_VOID smp_dec_err_pdu (ASN1_DEC_CTXT *aCtx)
{

  smp_uca_pdu_type = SMP_UCA_PDU_ERROR;
  smp_err_code = SMP_ERR_NO_ERROR;	/* init the value */

  /* decode error code */
  if (asn1r_get_u8 (aCtx, &smp_err_code))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;
    }

  /* asn1 decoding done */
  aCtx->asn1r_decode_done = SD_TRUE;
  aCtx->asn1r_decode_done_fun = NULL;
}


/************************************************************************/
/*			START_smp_dec_time_sync_pdu			*/
/*----------------------------------------------------------------------*/
/* UCA Time Synchronization PDU received.				*/
/************************************************************************/
static ST_VOID START_smp_dec_time_sync_pdu (ASN1_DEC_CTXT *aCtx)
{
  smp_uca_pdu_type = SMP_UCA_PDU_TIMESYNC;

  /* decode received TimeSyncPDU */
  ASN1R_TAG_ADD (aCtx, CTX,          0,       smp_dec_measure_req_pdu);
  ASN1R_TAG_ADD (aCtx, CTX,          1,       smp_dec_measure_rsp_pdu);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, START_smp_dec_sync_req_pdu);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, START_smp_dec_sync_rsp_pdu);
  ASN1R_TAG_ADD (aCtx, CTX,          4,       smp_dec_sync_err_pdu);
  ASN1R_TAG_ADD (aCtx, CTX,          5,       smp_dec_prepare_pdu);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = END_smp_dec_time_sync_pdu;
}


/************************************************************************/
/*			smp_dec_measure_req_pdu				*/
/*----------------------------------------------------------------------*/
/* Time Synchronization: Measure-Request PDU received.			*/
/************************************************************************/
static ST_VOID smp_dec_measure_req_pdu (ASN1_DEC_CTXT *aCtx)
{
  smp_timesync_pdu_type = SMP_TIMESYNC_PDU_MEASURE_REQ;
}


/************************************************************************/
/*			smp_dec_measure_rsp_pdu				*/
/*----------------------------------------------------------------------*/
/* Time Synchronization: Measure-Response PDU received.			*/
/************************************************************************/
static ST_VOID smp_dec_measure_rsp_pdu (ASN1_DEC_CTXT *aCtx)
{
  smp_timesync_pdu_type = SMP_TIMESYNC_PDU_MEASURE_RSP;
}


/************************************************************************/
/*			START_smp_dec_sync_req_pdu			*/
/*----------------------------------------------------------------------*/
/* Time Synchronization: Sync-Request PDU received.			*/
/************************************************************************/
static ST_VOID START_smp_dec_sync_req_pdu (ASN1_DEC_CTXT *aCtx)
{
  smp_timesync_pdu_type = SMP_TIMESYNC_PDU_SYNC_REQ;
  memset (&smp_time_req, 0, sizeof (SMP_TIME));
  memset (&smp_time_rsp, 0, sizeof (SMP_TIME));
  memset (smp_auth, 0 ,sizeof (smp_auth));

  ASN1R_TAG_ADD (aCtx, CTX, 0, smp_dec_sync_req_request_time);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = END_smp_dec_sync_req_pdu;
}


/************************************************************************/
/*			smp_dec_sync_req_request_time			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-RequestPDU, decoding request-time param		*/
/************************************************************************/
static ST_VOID smp_dec_sync_req_request_time (ASN1_DEC_CTXT *aCtx)
{
MMS_BTOD tim;

  if (asn1r_get_btod (aCtx, &tim))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_BOOLEAN);
    return;                          
    }

  if (tim.form == MMS_BTOD6)
    {
    smp_time_req.days = tim.day;
    smp_time_req.msec = tim.ms;
    smp_time_req.usec = 0;
    }
  else
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 1, smp_dec_sync_req_response_time);
}


/************************************************************************/
/*			smp_dec_sync_req_response_time			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-RequestPDU, decoding response-time param		*/
/************************************************************************/
static ST_VOID smp_dec_sync_req_response_time (ASN1_DEC_CTXT *aCtx)
{
MMS_BTOD tim;

  if (asn1r_get_btod (aCtx, &tim))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_BOOLEAN);
    return;                          
    }

  if (tim.form == MMS_BTOD6)
    {
    smp_time_rsp.days = tim.day;
    smp_time_rsp.msec = tim.ms;
    }
  else
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 2, smp_dec_sync_req_request_usec);
  ASN1R_TAG_ADD (aCtx, CTX, 3, smp_dec_sync_req_response_usec);
  ASN1R_TAG_ADD (aCtx, CTX, 4, smp_dec_sync_req_authentication);
}


/************************************************************************/
/*			smp_dec_sync_req_request_usec			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-RequestPDU, decoding request-usec param		*/
/************************************************************************/
static ST_VOID smp_dec_sync_req_request_usec (ASN1_DEC_CTXT *aCtx)
{

  if (asn1r_get_i32 (aCtx, &smp_time_req.usec))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;                          
    }

  ASN1R_TAG_ADD (aCtx, CTX, 3, smp_dec_sync_req_response_usec);
  ASN1R_TAG_ADD (aCtx, CTX, 4, smp_dec_sync_req_authentication);
}


/************************************************************************/
/*			smp_dec_sync_req_response_usec			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-RequestPDU, decoding response-usec param		*/
/************************************************************************/
static ST_VOID smp_dec_sync_req_response_usec (ASN1_DEC_CTXT *aCtx)
{

  if (asn1r_get_i32 (aCtx, &smp_time_rsp.usec))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;                          
    }

  ASN1R_TAG_ADD (aCtx, CTX, 4, smp_dec_sync_req_authentication);
}


/************************************************************************/
/*			smp_dec_sync_req_authentication			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-RequestPDU, decoding authentication param		*/
/************************************************************************/
static ST_VOID smp_dec_sync_req_authentication (ASN1_DEC_CTXT *aCtx)
{

  /* set max, the buffer is limited */
  if (aCtx->asn1r_elmnt_len > SMP_SIZE_OF_AUTH)
    {
    asn1r_set_dec_err (aCtx, ASN1E_OCTSTR_TOO_BIG);
    return;
    }

  if (asn1r_get_octstr (aCtx, smp_auth))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_BOOLEAN);
    return;                          
    }
}


/************************************************************************/
/*			END_smp_dec_sync_req_pdu			*/
/*----------------------------------------------------------------------*/
/* End of decoding Sync-RequestPDU.					*/
/************************************************************************/
static ST_VOID END_smp_dec_sync_req_pdu (ASN1_DEC_CTXT *aCtx)
{
  return;
}


/************************************************************************/
/*			START_smp_dec_sync_rsp_pdu			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-ResponsePDU received.				*/
/************************************************************************/
static ST_VOID START_smp_dec_sync_rsp_pdu (ASN1_DEC_CTXT *aCtx)
{
  smp_timesync_pdu_type = SMP_TIMESYNC_PDU_SYNC_RSP;
  memset (&smp_delta_time, 0, sizeof (SMP_TIME));

  ASN1R_TAG_ADD (aCtx, UNI, INT_CODE, smp_dec_sync_rsp_delta_days);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = END_smp_dec_sync_rsp_pdu;
}


/************************************************************************/
/*			smp_dec_sync_rsp_delta_days			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-ResponsePDU, decoding delta_days parameter.	*/
/************************************************************************/
static ST_VOID smp_dec_sync_rsp_delta_days (ASN1_DEC_CTXT *aCtx)
{
  if (asn1r_get_i32 (aCtx, &smp_delta_time.days))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;                          
    }
  ASN1R_TAG_ADD (aCtx, UNI, INT_CODE, smp_dec_sync_rsp_delta_msec);
}


/************************************************************************/
/*			smp_dec_sync_rsp_delta_msec			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-ResponsePDU, decoding delta_msec parameter.	*/
/************************************************************************/
static ST_VOID smp_dec_sync_rsp_delta_msec (ASN1_DEC_CTXT *aCtx)
{
  if (asn1r_get_i32 (aCtx, &smp_delta_time.msec))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;                          
    }

  ASN1R_TAG_ADD (aCtx, UNI, INT_CODE, smp_dec_sync_rsp_delta_usec);
}


/************************************************************************/
/*			smp_dec_sync_rsp_delta_usec			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-ResponsePDU, decoding delta_usec parameter.	*/
/************************************************************************/
static ST_VOID smp_dec_sync_rsp_delta_usec (ASN1_DEC_CTXT *aCtx)
{
  if (asn1r_get_i32 (aCtx, &smp_delta_time.usec))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;                          
    }
}


/************************************************************************/
/*			END_smp_dec_sync_rsp_pdu			*/
/*----------------------------------------------------------------------*/
/* End of decoding Sync-ResponsePDU.					*/
/************************************************************************/
static ST_VOID END_smp_dec_sync_rsp_pdu (ASN1_DEC_CTXT *aCtx)
{
  return;
}


/************************************************************************/
/*			smp_dec_sync_err_pdu				*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: SyncErrorPDU received.					*/
/************************************************************************/
static ST_VOID smp_dec_sync_err_pdu (ASN1_DEC_CTXT *aCtx)
{
  smp_timesync_pdu_type = SMP_TIMESYNC_PDU_ERROR;
  smp_sync_err_code = SMP_SYNC_ERR_NO_ERROR;	/* set initial value */

  if (asn1r_get_u8 (aCtx, &smp_sync_err_code))
    {
    asn1r_set_dec_err (aCtx, ASN1E_INVALID_LENGTH);
    return;                          
    }
}


/************************************************************************/
/*			smp_dec_prepare_pdu				*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Prepare-SyncPDU received.				*/
/************************************************************************/
static ST_VOID smp_dec_prepare_pdu (ASN1_DEC_CTXT *aCtx)
{
  smp_timesync_pdu_type = SMP_TIMESYNC_PDU_PREPARE;
}

/************************************************************************/
/*			END_smp_dec_time_sync_pdu			*/
/*----------------------------------------------------------------------*/
/* End of decoding Time-SyncPDU.					*/
/************************************************************************/
static ST_VOID END_smp_dec_time_sync_pdu (ASN1_DEC_CTXT *aCtx)
{
  aCtx->asn1r_decode_done = SD_TRUE;
  aCtx->asn1r_decode_done_fun = NULL;
}
#endif  /* UCA_SMP */

