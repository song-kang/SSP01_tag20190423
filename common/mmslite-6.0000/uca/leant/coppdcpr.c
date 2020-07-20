/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : coppdcpr.c						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements decoding of CPR PPDU.		*/
/*									*/
/*  For information see the:						*/
/*	ISO 8822 "Information processing systems - Open Systems		*/
/*	  Interconnection - Connection oriented presentation service	*/
/*	  definition.							*/
/*	ISO 8823 "Information processing systems - Open Systems		*/
/*	  Interconnection - Connection oriented presentation protocol	*/
/*	  specification.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			u_cosp_con_cnf_neg				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/17/09  JRB    12     Use new decoder (copp_dec_cpr), avoids use	*/
/*			   of global variables.				*/
/*			   Use S_FMT_PTR macro to log pointers.		*/
/* 02/26/09  JRB    11     Fix to handle PDV len=0 or User-data len=0.	*/
/* 10/08/08  JRB    10     Fix PSEL decode (didn't work if len > 127).	*/
/* 04/28/08  GLB    09     Removed CALLED_ONLY.				*/
/* 03/31/03  JRB    08     asn1r_get_bitstr: add max_bits arg.		*/
/* 01/02/02  JRB    07     Converted to use ASN1R (re-entrant ASN1)	*/
/* 10/09/01  JRB    06     asn1_skip_elmnt doesn't work for indef len	*/
/*			   encoding. Use "parse_next" instead.		*/
/* 02/02/99  JRB    05     BUG FIX: Decode "prov_reas" in "Result-list"	*/
/*			   even if no TSN (see decode_result).		*/
/* 02/02/99  JRB    04	   Allow User-data with Prov-reason but ignore.	*/
/* 03/18/98  JRB    03     Use defines (not variables) for ASN1 errors.	*/
/* 12/22/97  JRB    02     Use ST_INT8.					*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/19/97  JRB    01     Created					*/
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
/*			u_cosp_con_cnf_neg				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate the	*/
/* connection has been rejected by remote.				*/
/* 									*/
/* Parameters:								*/
/*	acse_conn		Pointer to connection info		*/
/*	result			see def results COSP_CON_CNF_... above	*/
/*	reason			see def reasons COSP_CON_CNF_... above	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_con_cnf_neg (ACSE_CONN *acse_conn, ST_INT result, ST_INT reason)
  {
ST_INT8 abort_reason;
COPP_DEC_CPR cpr_info;
ST_RET dec_ret;		/* decoder return value		*/

  if (acse_conn->copp_state != COPP_STATE_AWAIT_CPA)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot process S-CONNECT.cnf- (Illegal state=%d)",
      acse_conn->copp_state);
    return;
    }

  /* CRITICAL: Init APDU ptr & len. Changed only if APDU found in msg.	*/
  acse_conn->apdu_ptr = NULL;
  acse_conn->apdu_len = 0;

  dec_ret = copp_dec_cpr (acse_conn->ppdu_ptr, acse_conn->ppdu_len,
            acse_conn->pciacse,	/* expected PCI	*/
            &cpr_info);		/* decoded data	*/
  if (dec_ret)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: ASN.1 decode of S-Connect.cnf- failed, err=%d", dec_ret);
    abort_reason = 1;			/* unrecognized-ppdu	*/
    /* Don't send ARP PPDU.  Remote node is already rejecting connect.	*/
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else if (cpr_info.version_bit_str != 0x80)
    {		/* Must be version1:  Bit 0 of bitstr (high bit) set	*/
    COPP_LOG_ERR1 ("COPP-ERROR: Unsupported version in CPR received for user_conn_id =" S_FMT_PTR ".",
      acse_conn->user_conn_id);
    abort_reason = 6;			/* invalid-ppdu-parameter-value	*/
    /* Don't send ARP PPDU.  Remote node is already rejecting connect.	*/
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else if (cpr_info.provider_reason_present)
    {		/* Provider reject	*/
    COPP_LOG_DEC3 ("P-CONNECT.cnf-:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR
                   "  Provider-reason = %d. No P-user-data.",
      acse_conn, acse_conn->user_conn_id, cpr_info.provider_reason);
    /* NOTE: there is no User-data with this one.			*/
    u_copp_con_cnf (acse_conn, P_CON_RESULT_PROVIDER_REJ,
      cpr_info.provider_reason);
    }
  else if (cpr_info.pc_def_result_list.bad_pc_deflist)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Unsupported P-context in CPR received for user_conn_id =" S_FMT_PTR ".",
      acse_conn->user_conn_id);
    abort_reason = 6;			/* invalid-ppdu-parameter-value	*/
    /* Don't send ARP PPDU.  Remote node is already rejecting connect.	*/
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else
    {		/* User reject		*/
    /* Connection rejected, but still must pass up APDU to ACSE layer.	*/
    /* NOTE: ptr=NULL & len=0 if APDU is not encoded in the message.	*/
    acse_conn->apdu_ptr = cpr_info.pdv_info.pdv_data_ptr;
    acse_conn->apdu_len = cpr_info.pdv_info.pdv_data_len;
    COPP_LOG_DEC3 ("P-CONNECT.cnf-:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                   "  User data length: %d", acse_conn, acse_conn->user_conn_id, acse_conn->apdu_len);
    COPP_LOG_DECH (acse_conn->apdu_len, acse_conn->apdu_ptr);

    u_copp_con_cnf (acse_conn, P_CON_RESULT_USER_REJ, 0);
    }
  acse_conn->copp_state = COPP_STATE_IDLE;	/* No matter what	*/
  }

