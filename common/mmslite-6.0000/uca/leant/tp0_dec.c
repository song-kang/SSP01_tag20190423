/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_dec.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 decode functions.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	np_data_ind ()							*/
/*	np_disconnect_ind ()						*/
/*	np_connect_cnf_pos ()						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    07     Chg all conn_id to (ST_VOID *).		*/
/* 10/22/08  JRB    06     TPDU must be at least 2 bytes to decode.	*/
/*			   Chg tp_decode_er to really decode ER TPDU.	*/
/* 04/07/03  JRB    05     Add SPDU_RX_NO_PREALLOCATE option.		*/
/* 04/13/00  JRB    04     Lint cleanup.				*/
/* 08/13/98  JRB    03     Lint cleanup.				*/
/* 09/25/97  JRB    02     Don't generate exception for invalid state.	*/
/* 08/01/97  JRB    01     Created (compatible with MMS-EASE 7.0).	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4.h"		/* Internal definitions for tp4	*/
#include "tp4_encd.h"
#include "tp4_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			np_data_ind					*/
/* This "np_data_ind" funct is similar to "tp_decode_nsdu" used for TP4.*/
/* It calls one function to decode the PDU, then another function to	*/
/* process it.  The "decode" functions are shared with TP4.		*/
/************************************************************************/
ST_VOID np_data_ind (SOCK_INFO *sock_info, ST_UCHAR *pdu_ptr, ST_UINT16 pdu_len)
  {
ST_UCHAR	type;  
ST_UINT16	len;		/* len of processed TPDU by a dec func	*/
TPDU_CX		 tpdu_cx_rx;	/* tpdu_cr_rx or tpdu_cc_rx		*/
TPDU_DR		 tpdu_dr_rx;
TPDU_DT		 tpdu_dt_rx;
TPDU_ER          tpdu_er_rx;

  /* TP0 TPDU starts after TPKT hdr (RFC1006_HEAD_LEN bytes).		*/
  /* TPDU must be at least 2 bytes (len and TPDU code) to safely decode.*/
  if (pdu_len < RFC1006_HEAD_LEN + 2)
    {
    TP_LOG_ERR0 ("TP0 TPDU too short to decode. Ignored.");
    return;
    }

  /* Strip off TPKT header.		*/
  pdu_len -= RFC1006_HEAD_LEN;
  pdu_ptr += RFC1006_HEAD_LEN;

    /* get the TPDU type (from the second octet, bits 4-7) */
    type = pdu_ptr [1] & TP_PDU_MASK_CDT;

    /* Based on type of TPDU (CR, CC, DT, etc.) call appropriate decode	*/
    /* subfunction.							*/

    switch (type)
      {
      case TP_PDU_TYPE_CR:
      case TP_PDU_TYPE_CC:
        if ((len = tp_decode_cx (&tpdu_cx_rx, pdu_ptr, pdu_len, type,
                  TP_PDU_MAX_SIZE_65531)) != 0)	/* Default TPDU size=65531*/
          {
          tpdu_cx_rx.sock_info = sock_info;

          tp_log_tsdu ((ST_VOID *) &tpdu_cx_rx, type, TP_IND);
          /* This is the only step different in processing CR or CC.	*/
          if (type == TP_PDU_TYPE_CR)
            tp0_process_cr (&tpdu_cx_rx);
          else
            tp0_process_cc (&tpdu_cx_rx);
          }
      break;

      case TP_PDU_TYPE_DR:
        if ((len = tp_decode_dr (&tpdu_dr_rx, pdu_ptr, pdu_len)) != 0)
          {
          tpdu_dr_rx.sock_info = sock_info;

          tp_log_tsdu ((ST_VOID *) &tpdu_dr_rx, TP_PDU_TYPE_DR, TP_IND);
          tp0_process_dr (&tpdu_dr_rx);
          }
      break;

      case TP_PDU_TYPE_DT:
        if ((len = tp0_decode_dt (&tpdu_dt_rx, pdu_ptr, pdu_len)) != 0)
          {
          tpdu_dt_rx.sock_info = sock_info;

          tp_log_tsdu ((ST_VOID *) &tpdu_dt_rx, TP_PDU_TYPE_DT, TP_IND);
          tp0_process_dt (&tpdu_dt_rx);
          }
      break;

      case TP_PDU_TYPE_ER:
        /* NOTE: the last arg indicates this is Transport Class 0.	*/
        if ((len = tp_decode_er (&tpdu_er_rx, pdu_ptr, pdu_len, 0)) > 0)
          {
          /* decode succeeded but we ignore ER TPDU. Log so user knows.*/
          TP_LOG_ERR0 ("DEBUG: processing of TPDU-ER.ind not implemented yet.");
          }
      break;

      case TP_PDU_TYPE_ED:	/* we do not support expedited data	*/
      case TP_PDU_TYPE_EA:
        TP_LOG_ERR0 ("TP-ERROR: decode ED or EA TPDU failed (not supported)");
      break;		/* Do nothing					*/

      default:		/* type unknown, we should exit	decoding	*/
        TP_LOG_ERR1 ("TP-ERROR: decode TPDU failed (invalid type 0x%2.2X)",
	  (unsigned) type);
      break;
      }	/* end "switch"	*/
  }
/************************************************************************/
/*			np_disconnect_ind				*/
/************************************************************************/
ST_VOID np_disconnect_ind (SOCK_INFO *sock_info)
  {
TP0_CONN *tp_conn;
ST_INT reason;		/* Transport Disconnect reason	*/

  if (sock_info->user_conn_id == INVALID_CONN_ID)
    return;	/* Transport doesn't know about connection yet. IGNORE.	*/

  tp_conn = (TP0_CONN *) sock_info->user_conn_id;
  switch (tp_conn->state)
    {
    case TP_STATE_WFNC:
    case TP_STATE_WFCC:
    case TP_STATE_OPEN:
    case TP_STATE_WFTRESP:
      reason = 0;			/* Normal disconnection	*/
      /* NOTE: To save memory, user data is never passed up to user.	*/
      tp4_disconnect_ind (tp_conn->user_conn_id, reason,
                          0, NULL);	/* No user data passed up.	*/
      tp_conn->state = TP_STATE_CLOSED;
#if defined(SPDU_RX_NO_PREALLOCATE)
      M_FREE (MSMEM_STARTUP, tp_conn->spdu_rx.spdu_ptr);
#endif
      break;
    default:
      TP_LOG_ERR1 ("TP-ERROR: invalid state (%d) for N-DISCONNECT.ind. Ignored.",
        tp_conn->state);
      break;
    }
  }
/************************************************************************/
/*			np_connect_cnf_pos				*/
/************************************************************************/
ST_VOID np_connect_cnf_pos (SOCK_INFO *sock_info, ST_VOID *user_conn_id)
  {
TP0_CONN *tp_conn = (TP0_CONN *) user_conn_id;

  /* Save sock_info ptr in tp_conn.	*/
  tp_conn->sock_info = sock_info;

  /* CRITICAL: Save sock_info in tp_conn->tpdu_cx before calling tp0_send_cr*/
  tp_conn->tpdu_cx.sock_info = tp_conn->sock_info;

  /* Send T-CONNECT.req		*/
  tp0_send_cr (&tp_conn->tpdu_cx);
  tp_conn->state = TP_STATE_WFCC;
  }
