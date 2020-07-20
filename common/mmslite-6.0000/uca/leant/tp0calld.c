/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 - 2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0calld.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions for handling of "Called" side of connections.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	tp0_accept ()							*/
/*	tp0_process_cr ()						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    09     Chg user_conn_id to (ST_VOID *).		*/
/*			   Del MMSEASE_MOSI support.			*/
/*			   Fail compile if SNAP_LITE defined.		*/
/* 07/30/08  JRB    08     Use DR reason codes valid for TP0.		*/
/* 03/29/05  EJV    07     Added MMSEASE_MOSI support.			*/
/* 06/20/02  JRB    06     Chg addr args to tp4_connect_ind.		*/
/* 05/09/01  JRB    05     On T-CONNECT.ind, save TSAPs in TP0_CONN.	*/
/* 04/25/00  JRB    04     Lint cleanup.				*/
/* 06/08/99  JRB    03     Chg to support 1024 connections.		*/
/* 08/13/98  JRB    02     Lint cleanup.				*/
/* 08/01/97  JRB    01     Created (compatible with MMS-EASE 7.0).	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4.h"		/* Internal definitions for tp4	*/
#include "tp4_encd.h"		/* Encode/decode defs for tp4	*/
#include "tp4_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#if defined(SNAP_LITE)
#error Do not use this module with SNAP_LITE. Use 'tp0calld2.c' instead.
#endif

/*======================================================================*/
/*======================================================================*/
/* FUNCTIONS BELOW CALLED BY USER TO ENCODE AND SEND TPDUs.		*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			tp0_accept					*/
/* This function is called by the user to accept a connect request from	*/
/* a remote node							*/
/************************************************************************/

ST_RET tp0_accept (ST_LONG tp0_conn_id, ST_VOID *user_conn_id,
		    ST_INT conndata_len,
		    char *conndata)
  {
TP0_CONN *tp_conn;

  if ((ST_ULONG) (tp0_conn_id-MIN_TP0_CONN_ID) >= tp0_cfg.max_num_conns)
    return (TP4E_BADCONN);

  tp_conn = &tp0_conn_arr [tp0_conn_id-MIN_TP0_CONN_ID];
  if (tp_conn->state != TP_STATE_WFTRESP)
    return (TP4E_BADCONN);

  tp_conn->user_conn_id = user_conn_id;		/* Save User's ID	*/

  /* Save User Data in tp_conn for retransmission.	*/
  /* DEBUG: Can we blow this off to save space?		*/
  tp_conn->tpdu_cx.udata_len = min ((ST_UINT16) conndata_len, TP_MAX_CONN_UDATA);
  memcpy (tp_conn->tpdu_cx.udata_buf, conndata, tp_conn->tpdu_cx.udata_len);

  tp_conn->state = TP_STATE_OPEN;
  tp0_send_cc (&tp_conn->tpdu_cx);
  return (SD_SUCCESS);
  }

/*======================================================================*/
/*======================================================================*/
/* FUNCTIONS BELOW CALLED BY TP0 TO PROCESS TPDUs RECEIVED		*/
/* (TPDUs ALREADY DECODED BY TP0).					*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/* tp0_process_cr							*/
/* Process a CR (Connection Request) TPDU.				*/
/************************************************************************/
ST_VOID tp0_process_cr (TPDU_CX *tpdu_cr_rx)    
  {
ST_UINT16 conn_id;	/* index into "tp0_conn_arr" array (Unsigned so	*/
		/* no sign-extension on compare to tp0_cfg.max_num_conns).*/
TP0_CONN *tp_conn;	/* ptr to struct in "tp0_conn_arr" array.	*/
TPDU_DR tpdu_dr;
ST_VOID *tp0_user_bind_id;

  /* Fill in TPDU_DR struct in case DR needs to be sent.		*/
  /* Do not fill in "reason" yet.					*/
  tpdu_dr.loc_ref = 0;				/* src-ref = 0 (always)	*/
  tpdu_dr.rem_ref = tpdu_cr_rx->rem_ref;
  tpdu_dr.sock_info = tpdu_cr_rx->sock_info;

  /* Check if Local TSAP is correct.		*/
  tp0_user_bind_id = only_tp0_user_bind_id;
  if (memcmp (tpdu_cr_rx->loc_tsap, only_tp0_loc_tsap, only_tp0_loc_tsap[0]+1))
    {
    TP_LOG_ERR0 ("TP-ERROR: received CR with invalid TSAP. Sending DR.");
    TP_LOGH_ERR (tpdu_cr_rx->loc_tsap[0], &tpdu_cr_rx->loc_tsap[1]);
    tpdu_dr.reason = TP_DR_ADDR_UNKNOWN;
    tp0_send_dr (&tpdu_dr);
    return;
    }

  /* Check for Transport Class 0.  Don't allow any other.		*/
  if (tpdu_cr_rx->preferred_class != 0)
    {
    TP_LOG_ERR1 ("TP-ERROR: received CR with invalid class %d. Sending DR.",
      tpdu_cr_rx->preferred_class);
    /* NOTE: this DR "reason" is not legal for Class 0, but caller	*/
    /* proposed different class, so they should recognize this reason.	*/
    tpdu_dr.reason = TP_DR_CLASS_UNSUPPORTED;	/* illegal for TP0 but OK here*/
    tp0_send_dr (&tpdu_dr);
    return;
    }

  /* Local Ref must equal 0.  If not, it is protocol error.		*/
  if (tpdu_cr_rx->loc_ref != 0)
    {
    TP_LOG_ERR0 ("TP-ERROR: received CR with invalid reference. Sending DR.");
    tpdu_dr.reason = TP_DR_NO_REASON;
    tp0_send_dr (&tpdu_dr);
    return;
    }

  /* Look for a position in "tp0_conn_arr" where state = TP_STATE_CLOSED	*/
  tp_conn = &tp0_conn_arr [0];
  for (conn_id = 0;  conn_id < tp0_cfg.max_num_conns;  conn_id++, tp_conn++)
    {
    if (tp_conn->state == TP_STATE_CLOSED)
      {
      tp0_conn_clean (tp_conn);	/* Good time to clean up conn struct.	*/
      inc_loc_ref_offset (tp_conn->loc_ref_offset);

      /* Save sock_info in "tp_conn".				*/
      tp_conn->sock_info = tpdu_cr_rx->sock_info;
      /* Save tp_conn (as user_conn_id) in "sock_info".				*/
      tp_conn->sock_info->user_conn_id = tp_conn;

      /* Copy the CR info to the tp_conn struct and modify it for the CC.*/
      /* Most parameters we send back exactly as received (all except	*/
      /* max_tpdu_len, loc_ref, cdt, udata_buf/len).			*/
      /* NOTE: udata_buf/len not changed until tp0_accept.		*/

      memcpy (&tp_conn->tpdu_cx, tpdu_cr_rx, sizeof (TPDU_CX));
      tp_conn->tpdu_cx.max_tpdu_len_enc = min (tp0_cfg.max_tpdu_len_enc,
                                               tpdu_cr_rx->max_tpdu_len_enc);
      /* Save negotiated TPDU len in tp_conn.	*/
      switch (tp_conn->tpdu_cx.max_tpdu_len_enc)
        {					/* code for PDU size	*/
        case TP_PDU_MAX_SIZE_128  :	tp_conn->max_tpdu_len =   128; break;
        case TP_PDU_MAX_SIZE_256  :	tp_conn->max_tpdu_len =   256; break;
        case TP_PDU_MAX_SIZE_512  :	tp_conn->max_tpdu_len =   512; break;
        case TP_PDU_MAX_SIZE_1024 :	tp_conn->max_tpdu_len =  1024; break;
        case TP_PDU_MAX_SIZE_2048 :	tp_conn->max_tpdu_len =  2048; break;
        case TP_PDU_MAX_SIZE_4096 :	tp_conn->max_tpdu_len =  4096; break;
        case TP_PDU_MAX_SIZE_8192 :	tp_conn->max_tpdu_len =  8192; break;
        case TP_PDU_MAX_SIZE_65531:	tp_conn->max_tpdu_len = 65531; break;
        default:
          TP_LOG_ERR0 ("TP-ERROR: received CR with illegal TPDU size. Sending DR.");
          tpdu_dr.reason = TP_DR_NO_REASON;
          tp0_send_dr (&tpdu_dr);
          return;
        }	/* end "switch"	*/

      tp_conn->tpdu_cx.loc_ref = calc_loc_ref (conn_id, tp_conn->loc_ref_offset);
      tp_conn->tpdu_cx.cdt = 0;	/* CDT always 0 for Class 0	*/

      /* Set "tp_conn" parameters for new connection.			*/
      tp_conn->rem_ref = tpdu_cr_rx->rem_ref;	/* need for sending AKs	*/
      tp_conn->state = TP_STATE_WFTRESP;

      /* Save TSAPs in TP0_CONN struct.					*/
      memcpy (tp_conn->loc_tsap, tpdu_cr_rx->loc_tsap, tpdu_cr_rx->loc_tsap[0]+1);
      memcpy (tp_conn->rem_tsap, tpdu_cr_rx->rem_tsap, tpdu_cr_rx->rem_tsap[0]+1);

      tp4_connect_ind (tp0_user_bind_id, conn_id+MIN_TP0_CONN_ID,
                tpdu_cr_rx->loc_tsap,
                tpdu_cr_rx->rem_tsap,
                NULL,			/* NSAP: not used	*/
                tp_conn->sock_info->ip_addr,
                (ST_INT) tpdu_cr_rx->udata_len,
                tpdu_cr_rx->udata_buf);
      break;		/* BREAK OUT OF LOOP. Only want 1 connection!!!	*/
      }
    }		/* end "for" loop	*/
  if (conn_id >= tp0_cfg.max_num_conns)
    {				/* Did not find an available conn_id.	*/
    TP_LOG_ERR0 ("TP-ERROR: not enough resources for received CR. Sending DR.");
    tpdu_dr.reason = TP_DR_CONGESTION_AT_TSAP;	/* legal for TP0	*/
    tp0_send_dr (&tpdu_dr);
    return;
    }
  return;
  }


