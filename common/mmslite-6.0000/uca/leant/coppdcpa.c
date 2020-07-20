/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2009-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : coppdcpa.c						*/
/* PRODUCT(S)  : MOSI Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements decoding of CPA PPDU.		*/
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
/*			u_cosp_con_cnf_pos				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/02/09  JRB    01     Moved function from "coppdcp.c" to here &	*/
/*			   changed it to use new decoder (copp_dec_cpa).*/
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
/*			u_cosp_con_cnf_pos				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that	*/
/* the connection has been established.					*/
/* 									*/
/* Parameters:								*/
/*  acse_conn			Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_con_cnf_pos (ACSE_CONN *acse_conn)
  {
ST_INT8 abort_reason;
ST_INT8 event_id;
COPP_DEC_CPA cpa_info;
ST_RET dec_ret;		/* decoder return value		*/

  if (acse_conn->copp_state != COPP_STATE_AWAIT_CPA)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot process S-CONNECT.cnf+ (Illegal state=%d)",
      acse_conn->copp_state);
    return;
    }

  /* CRITICAL: Init APDU ptr & len. Changed only if APDU found in msg.	*/
  acse_conn->apdu_ptr = NULL;
  acse_conn->apdu_len = 0;

  /* This clears "cpa_info", then fills it in with decoded data.	*/
  dec_ret = copp_dec_cpa (acse_conn->ppdu_ptr, acse_conn->ppdu_len,
            acse_conn->pciacse,	/* expected PCI	*/
            &cpa_info);		/* decoded data	*/
  if (dec_ret)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: ASN.1 decode of S-Connect.cnf+ failed, err=%d", dec_ret);
    acse_conn->copp_state = COPP_STATE_IDLE;
    abort_reason = 1;			/* unrecognized-ppdu	*/
    event_id = -1;			/* Unlisted	*/
    /* Encode and send ARP PPDU.					*/
    copp_p_abort_req (acse_conn, abort_reason, event_id);
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else if (cpa_info.version_bit_str != 0x80)
    {		/* Must be version1:  Bit 0 of bitstr (high bit) set	*/
    COPP_LOG_ERR1 ("COPP-ERROR: Unsupported version in CPA received for user_conn_id =" S_FMT_PTR ".",
      acse_conn->user_conn_id);
    acse_conn->copp_state = COPP_STATE_IDLE;
    abort_reason = 6;			/* invalid-ppdu-parameter-value	*/
    event_id = 1;			/* cpa-PPDU	*/
    /* Encode and send ARP PPDU.					*/
    copp_p_abort_req (acse_conn, abort_reason, event_id);
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else if (cpa_info.pc_def_result_list.bad_pc_deflist)
    {
    COPP_LOG_ERR0 ("COPP-ERROR: CPA received with invalid P-context-definition-result-list");
    acse_conn->copp_state = COPP_STATE_IDLE;
    abort_reason = 6;			/* invalid-ppdu-parameter-value	*/
    event_id = 1;			/* cpa-PPDU	*/
    /* Encode and send ARP PPDU.					*/
    copp_p_abort_req (acse_conn, abort_reason, event_id);
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else
    {
    /* NOTE: "cpa_info.pc_def_result_list.num_ctxt" should be 0 if not	*/
    /*       encoded (it's optional) or 2 (we always propose 2).	*/

    /* Save critical info in "acse_conn"				*/
    /* NOTE: ptr=NULL & len=0 if APDU is not encoded in the message.	*/
    acse_conn->apdu_ptr = cpa_info.pdv_info.pdv_data_ptr;
    acse_conn->apdu_len = cpa_info.pdv_info.pdv_data_len;

    COPP_LOG_DEC2 ("P-CONNECT.cnf+:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
      acse_conn, acse_conn->user_conn_id);
    COPP_LOG_DECC1 ("User data length: %d", acse_conn->apdu_len);
    COPP_LOG_DECH (acse_conn->apdu_len, acse_conn->apdu_ptr);

    acse_conn->copp_state = COPP_STATE_CONNECTED;
    u_copp_con_cnf (acse_conn, P_CON_RESULT_ACCEPT, 0);
    }
  }

