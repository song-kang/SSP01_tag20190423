/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : coppdaru.c						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements decoding of U-ABORT indication.	*/
/*									*/
/*  For information see the:						*/
/*	ISO 8822 "Information processing systems - Open Systems		*/
/*	  Interconnection - Connection oriented presentation service	*/
/*	  definition.							*/
/*	ISO 8823 "Information processing systems - Open Systems		*/
/*	  Interconnection - Connection oriented presentation protocol	*/
/*	  specification.						*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			u_cosp_u_abort_ind 				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/17/09  JRB    08     Use new decoder (copp_dec_arx), avoids use	*/
/*			   of global variables.				*/
/* 02/26/09  JRB    07     Fix to handle PDV len=0 or User-data len=0.	*/
/* 01/30/06  GLB    06     Integrated porting changes for VMS           */
/* 01/02/02  JRB    05     Converted to use ASN1R (re-entrant ASN1)	*/
/* 03/18/98  JRB    04     Use defines (not variables) for ASN1 errors.	*/
/* 12/22/97  JRB    03     Use ST_INT8.					*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/19/97  JRB    02     General cleanup.				*/
/* 02/27/97  JRB    01     Created					*/
/************************************************************************/
#ifdef DEBUG_SISCO
static char *thisFileName = __FILE__;
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "asn1defs.h"
#include "acse2.h"

#include "copp.h"
#include "cosp_usr.h"

/************************************************************************/
/*			u_cosp_u_abort_ind 				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* connection has been aborted by remote SS-user.			*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*acse_conn		Pointer to connection info	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_u_abort_ind (ACSE_CONN *acse_conn)
  {
COPP_DEC_ARX arx_info;
ST_RET dec_ret;		/* decoder return value		*/

  if (acse_conn->copp_state == COPP_STATE_IDLE)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot process S-U-ABORT.ind (Illegal state=%d)",
      acse_conn->copp_state);
    return;
    }

  /* CRITICAL: Init APDU ptr & len. Changed only if APDU found in msg.	*/
  acse_conn->apdu_ptr = NULL;
  acse_conn->apdu_len = 0;

  dec_ret = copp_dec_arx (acse_conn->ppdu_ptr, acse_conn->ppdu_len,
            acse_conn->pciacse,	/* expected PCI	*/
            &arx_info);		/* decoded data	*/
  if (dec_ret)
    {		/* DECODE FAILED	*/
    /* Couldn't decode ARP or ARU, so pass up as P-P-Abort.ind with	*/
    /* reason = unrecognized-ppdu					*/
    arx_info.abort_reason = 1;	/* unrecognized-ppdu.		*/
    COPP_LOG_ERR1 ("COPP-ERROR: ASN.1 decode of S-U-Abort.ind failed, err=%d", dec_ret);
    COPP_LOG_DEC3 ("P-P-ABORT.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "  reason =%d",
        acse_conn, acse_conn->user_conn_id, arx_info.abort_reason);
    u_copp_p_abort_ind (acse_conn, arx_info.abort_reason);
    }
  else
    {		/* DECODE SUCCESSFUL	*/
    /* Pass up APDU (if present) to ACSE layer.				*/
    /* NOTE: ptr=NULL & len=0 if APDU is not encoded in the message.	*/
    acse_conn->apdu_ptr = arx_info.pdv_info.pdv_data_ptr;
    acse_conn->apdu_len = arx_info.pdv_info.pdv_data_len;
    if (arx_info.abort_type == COPP_U_ABORT)
      {		/* This is P-U-Abort	*/
      COPP_LOG_DEC3 ("P-U-ABORT.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                     "  User data length: %d", acse_conn, acse_conn->user_conn_id, acse_conn->apdu_len);
      COPP_LOG_DECH (acse_conn->apdu_len, acse_conn->apdu_ptr);
      u_copp_u_abort_ind (acse_conn);
      }
    else
      {		/* This is P-P-Abort	*/
      COPP_LOG_DEC4 ("P-P-ABORT.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "  reason =%d, event_id =%d",
        acse_conn, acse_conn->user_conn_id, arx_info.abort_reason, arx_info.event_id);
      u_copp_p_abort_ind (acse_conn, arx_info.abort_reason);
      }
    }
  acse_conn->copp_state = COPP_STATE_IDLE;	/* No matter what.	*/
  }

