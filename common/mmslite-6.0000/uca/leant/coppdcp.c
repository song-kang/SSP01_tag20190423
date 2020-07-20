/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : coppdcp.c						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements decoding of CP PPDU.		*/
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
/*			u_cosp_con_ind 					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/17/09  JRB    19     Use new decoder (copp_dec_cp), avoids use	*/
/*			   of global variables.				*/
/*			   Moved u_cosp_con_cnf_pos to "coppdcpa.c".	*/
/* 11/01/09  JRB    18     Chg user_bind_id to (ST_VOID *).		*/
/*			   Use S_FMT_PTR macro to log pointers.		*/
/*			   Init acse_conn->loc_psel before decode.	*/
/* 02/26/09  JRB    17     Fix to handle PDV len=0 or User-data len=0.	*/
/* 10/23/08  JRB    16     Del unused variable.				*/
/* 10/08/08  JRB    15     Fix PSEL decode (didn't work if len > 127).	*/
/* 04/28/08  GLB    14     Removed CALLED_ONLY and CALLING_ONLY         */
/* 01/30/06  GLB    13     Integrated porting changes for VMS           */
/* 08/01/03  JRB    12     Elim compiler warning.			*/
/* 06/24/03  JRB    11     Init psels before decode to avoid crash	*/
/*			   when sender does not send psels.		*/
/* 03/31/03  JRB    10     asn1r_get_bitstr: add max_bits arg.		*/
/* 01/02/02  JRB    09     Converted to use ASN1R (re-entrant ASN1)	*/
/* 10/10/01  JRB    08     asn1_skip_elmnt doesn't work for indef len	*/
/*			   encoding. Use "parse_next" instead.		*/
/* 05/09/01  JRB    07     On P-CONNECT.ind, save PSELs in ACSE_CONN.	*/
/* 02/02/99  JRB    06     BUG FIX: Decode "prov_reas" in "Result-list"	*/
/*			   even if no TSN (see decode_result).		*/
/* 03/18/98  JRB    05     Use defines (not variables) for ASN1 errors.	*/
/* 02/20/98  JRB    04     Chg abort_reason & event_id to ST_INT8.	*/
/* 12/22/97  JRB    03     Use ST_INT8.					*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/19/97  JRB    02     Add "CALLING" support.  General cleanup.	*/
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

#if defined(SNAP_LITE)
#error Do not use this module with SNAP_LITE. Use 'coppdcp_snap.c' instead.
#endif
/************************************************************************/
/*			u_cosp_con_ind					*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* remote node wishes to establish a connection.			*/
/* 									*/
/* If all decoding and all parameter checks succeed, then important	*/
/* parameters are saved in "acse_conn", and "u_copp_con_ind" is called.	*/
/*									*/
/* Parameters:								*/
/*  ST_VOID     *user_bind_id		SS-User's id for this binding.	*/
/*  ACSE_CONN	*con			Pointer to connection info	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_con_ind (ST_VOID *user_bind_id, ACSE_CONN *acse_conn)
  {
ST_INT8 abort_reason;
ST_INT8 event_id;
COPP_DEC_CP decode_cp;	/* decoded CP data		*/
ST_RET dec_ret;		/* decoder return value		*/

  if (acse_conn->copp_state != COPP_STATE_IDLE)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot process S-CONNECT.ind (Illegal state=%d)",
      acse_conn->copp_state);
    return;
    }

  /* CRITICAL: Init APDU ptr & len. Changed only if APDU found in msg.	*/
  acse_conn->apdu_ptr = NULL;
  acse_conn->apdu_len = 0;

  /* Set the local PSEL to NULL selector since we may send a negative	*/
  /* response (copp_con_rsp_neg) before this PSEL is decoded.		*/
  /* This is used as the "Responding PSEL" in the response.		*/
  acse_conn->loc_psel[0] = 0;	/* NULL selector	*/

  /* This clears "decode_cp", then fills it in with decoded data.	*/
  dec_ret = copp_dec_cp (acse_conn->ppdu_ptr, acse_conn->ppdu_len, &decode_cp);
  if (dec_ret)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: ASN.1 decode of S-Connect.ind failed, err=%d", dec_ret);
    abort_reason = 1;			/* unrecognized-ppdu	*/
    event_id = -1;			/* Unlisted	*/
    /* Encode and send ARP PPDU.					*/
    copp_p_abort_req (acse_conn, abort_reason, event_id);
    /* DON'T pass P-P-Abort.ind to COPP-user. User doesn't know about conn.*/
    }
  else if (decode_cp.send_negative_resp)
    {
    /* Flag was set during decode to trigger negative response, so send it.*/
    copp_con_rsp_neg (acse_conn, decode_cp.negative_resp_reason);
    }
  else if (decode_cp.version_bit_str != 0x80)
    {		/* Must be version1:  Bit 0 of bitstr (high bit) set	*/
    COPP_LOG_ERR0 ("COPP-ERROR: P-CONNECT.ind contains illegal P-version.");
    copp_con_rsp_neg (acse_conn, PROV_REASON_VERSION);
    }
  else if (decode_cp.def_ctxt_proposed)
    {
    COPP_LOG_ERR0 ("COPP-ERROR: P-CONNECT.ind contains unsupported P-default-context.");
    copp_con_rsp_neg (acse_conn, PROV_REASON_DEFAULT_CONTEXT);
    }
  else if (memcmp (decode_cp.called_psel, only_loc_psel, only_loc_psel [0]+1))
    {		/* Called PSEL must be one that is bound.		*/
    COPP_LOG_ERR0 ("COPP-ERROR: P-CONNECT.ind contains invalid Called-P-address.");
    copp_con_rsp_neg (acse_conn, PROV_REASON_CALLED_PRES_ADDR);
    }
  else if (decode_cp.bad_pc_deflist)
    {    /* Something we can't handle in pc_deflist.		*/
    COPP_LOG_ERR0 ("COPP-ERROR: P-CONNECT.ind contains illegal PC-deflist.");
    copp_con_rsp_neg (acse_conn, PROV_REASON_LOCAL_LIMIT);
    }
  else
    {
    /* Save critical info in "acse_conn"				*/
    /* NOTE: ptr=NULL & len=0 if APDU is not encoded in the message.	*/
    acse_conn->apdu_ptr = decode_cp.pdv_info.pdv_data_ptr;
    acse_conn->apdu_len = decode_cp.pdv_info.pdv_data_len;
    acse_conn->pciacse  = decode_cp.pciacse;
    acse_conn->pcimms   = decode_cp.pcimms;
    acse_conn->num_ctxt = decode_cp.num_ctxt;

    /* Save called_psel, calling_psel in acse_conn.			*/
    /* Will NEVER be too long, because len checked above.		*/
    memcpy (acse_conn->loc_psel, decode_cp.called_psel, decode_cp.called_psel [0]+1);
    memcpy (acse_conn->rem_psel, decode_cp.calling_psel, decode_cp.calling_psel [0]+1);
    
    COPP_LOG_DEC1 ("P-CONNECT.ind:   acse_conn_id =" S_FMT_PTR ".", acse_conn);
    COPP_LOG_DECC1 ("User data length: %d", acse_conn->apdu_len);
    COPP_LOG_DECH (acse_conn->apdu_len, acse_conn->apdu_ptr);

    acse_conn->copp_state = COPP_STATE_AWAIT_CON_RSP;

    u_copp_con_ind (user_bind_id, acse_conn);
    }
  }

