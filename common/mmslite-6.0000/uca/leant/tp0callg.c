/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0callg.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions for handling of "Calling" side of connections.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	tp0_connect							*/
/*	tp0_process_cc							*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 11/17/09  JRB    16     tp0_connect: pass loc_addr instead of bind_id.*/
/*			   Chg user_conn_id from ST_LONG to (ST_VOID *).*/
/* 11/12/08  MDE    15     Added tpdu_cc->sock_info->user_conn_id check */
/* 07/30/08  JRB    14     Use DR reason codes valid for TP0.		*/
/* 05/06/05  EJV    13     Added port param to np_connect_req call.	*/
/* 03/29/05  EJV    12     Added MMSEASE_MOSI support.			*/
/* 09/17/04  JRB    11     Chg tp0_connect to return connection ID.	*/
/* 02/02/04  JRB    10     tp0_connect: fix tp_type, bind_id checks.	*/
/* 07/30/03  EJV    09     Added encrypt_ctrl to tp0_connect.		*/
/* 04/07/03  JRB    08     Add SPDU_RX_NO_PREALLOCATE option.		*/
/* 06/20/02  JRB    07     Pass ip addr as ulong to np_connect_req.	*/
/* 06/08/99  JRB    06     Chg to support 1024 connections.		*/
/* 08/13/98  JRB    05     Lint cleanup.				*/
/* 06/17/98  JRB    04     Increment loc_ref_offset before encoding CR	*/
/*			   to avoid sending duplicate CR.		*/
/* 12/04/97  JRB    03     Don't chk TSAPs on CC (may not be sent).	*/
/* 09/25/97  JRB    02     Don't generate exception for invalid state.	*/
/* 08/01/97  JRB    01     Created (compatible with MMS-EASE 7.0).	*/
/************************************************************************/
static char *thisFileName = __FILE__;	/* For TP4_EXCEPT and slog macros*/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4.h"		/* Internal definitions for tp4	*/
#include "tp4_encd.h"		/* Encode/decode defs for tp4	*/
#include "tp4_log.h"

/************************************************************************/
/*			tp0_connect					*/
/* This function is called by the user to initiate a connect request to	*/
/* a remote node							*/
/* RETURNS: connection ID if successful, (-1) on error			*/
/* CRITICAL: connection ID = (conn_id+MIN_TP0_CONN_ID). Remember that	*/
/*	this is hoe we distinguish between TP0 and TP4 connections.	*/
/************************************************************************/

ST_LONG tp0_connect (ST_VOID *user_conn_id,
		     TP4_ADDR *loc_tp4_addr,
		     TP4_ADDR *rem_tp4_addr,
		     ST_INT conndata_len,
		     char *conndata,
                     S_SEC_ENCRYPT_CTRL *encrypt_ctrl)
  {
ST_UINT16 conn_id;		/* index into "tp0_conn_arr" array.		*/
TP0_CONN *tp_conn;
TPDU_CX *tpdu_cr;

  if (rem_tp4_addr->tp_type != TP_TYPE_TCP)
    {
    TP_LOG_ERR0 ("tp0_connect: invalid transport type");
    return (-1);
    }

  if (rem_tp4_addr->tp4_sel_len > MAX_TSEL_LEN)
    {
    TP4_EXCEPT ();	/* If this happens, user code is invalid.	*/
    return (-1);
    }

  /* Look for available connection.	*/
  tp_conn = &tp0_conn_arr [0];
  for (conn_id = 0;  conn_id < tp0_cfg.max_num_conns;  conn_id++, tp_conn++)
    {
    if (tp_conn->state == TP_STATE_CLOSED)
      break;		/* Found one!!  "tp_conn" points to it now.	*/
    }	/* end "for"	*/
  if (conn_id >= tp0_cfg.max_num_conns)
    {			/* Didn't find one!!	*/
    TP_LOG_ERR0 ("TP-ERROR: tp0_connect: all connections in use");
    return (-1);
    }
  /* "tp_conn" now points to a connection in TP_STATE_CLOSED state.	*/    
  tp0_conn_clean (tp_conn);	/* Good time to clean up conn struct.	*/

  /* Increment "loc_ref_offset" to avoid duplicate CR.			*/
  inc_loc_ref_offset (tp_conn->loc_ref_offset);
  
  /* Set "tpdu_cr" pointing to "tp_conn->tpdu_cx" and fill in.		*/
  /* Info stored in "tp_conn" in case retransmissions needed.		*/
  tpdu_cr = &tp_conn->tpdu_cx;
  /* Copy local and remote TSEL to the CR TPDU.	*/
  tpdu_cr->loc_tsap [0] = (ST_UCHAR) loc_tp4_addr->tp4_sel_len;
  memcpy (&tpdu_cr->loc_tsap [1], loc_tp4_addr->tp4_sel, loc_tp4_addr->tp4_sel_len);
  tpdu_cr->rem_tsap [0] = (ST_UCHAR) rem_tp4_addr->tp4_sel_len;
  memcpy (&tpdu_cr->rem_tsap [1], rem_tp4_addr->tp4_sel, rem_tp4_addr->tp4_sel_len);

  tpdu_cr->loc_ref   = calc_loc_ref (conn_id, tp_conn->loc_ref_offset);
  tpdu_cr->rem_ref   = 0;
  tpdu_cr->max_tpdu_len_enc = tp0_cfg.max_tpdu_len_enc;
  tpdu_cr->preferred_class = 0;
  tpdu_cr->cdt       = 0;
  tpdu_cr->udata_len = min ((ST_UINT16) conndata_len, TP_MAX_CONN_UDATA);
  memcpy (tpdu_cr->udata_buf, conndata, tpdu_cr->udata_len);

  /* Set state before calling np_connect_req (may complete immediately).*/
  tp_conn->user_conn_id = user_conn_id;	/* Save User's ID for confirm.	*/
  tp_conn->state = TP_STATE_WFNC;
  if ((tp_conn->sock_info = np_connect_req (tp_conn, rem_tp4_addr->netAddr.ip,
                                            rem_tp4_addr->port, encrypt_ctrl)) == NULL)
    {
    tp_conn->state = TP_STATE_CLOSED;	/* CRITICAL: so tp_conn can be reused.*/
#if defined(SPDU_RX_NO_PREALLOCATE)
    M_FREE (MSMEM_STARTUP, tp_conn->spdu_rx.spdu_ptr);
#endif
    return (-1);	/* Network connect failed immediately.	*/
    }

  /* CRITICAL: For TP0, connection ID = (conn_id+MIN_TP0_CONN_ID).	*/
  return (conn_id+MIN_TP0_CONN_ID);
  }

/************************************************************************/
/* tp0_process_cc							*/
/************************************************************************/
ST_VOID tp0_process_cc (TPDU_CX *tpdu_cc)
  {
ST_ULONG conn_id;	/* index into "tp0_conn_arr" array (Unsigned so	*/
		/* no sign-extension on compare to tp0_cfg.max_num_conns).*/
TP0_CONN *tp_conn;	/* ptr to struct in "tp0_conn_arr" array.	*/
TPDU_DR tpdu_dr;
ST_INT unacceptable_cc = SD_FALSE;
ST_INT reason;


  /* Fill in TPDU_DR struct in case DR needs to be sent.		*/
  /* Do not fill in "reason" yet.					*/
  tpdu_dr.loc_ref = tpdu_cc->loc_ref;
  tpdu_dr.rem_ref = tpdu_cc->rem_ref;
  tpdu_dr.sock_info = tpdu_cc->sock_info;

  /* Check connection ID.	*/
  if (tpdu_cc->sock_info->user_conn_id == INVALID_CONN_ID)
    {
    /* Invalid ID means CR has not been sent, so this CC is unexpected.	*/
    /* CC most likely received from "abnormal" application.		*/
    TP_LOG_ERR0 ("TP-ERROR: tp0_process_cc: Invalid tp_conn");
    /* May be intentional attack, so disconnect immediately.		*/
    np_disconnect_req (tpdu_cc->sock_info);  
    return;  
    }

  /* Find the correct "tp_conn". It should have been saved in sock_info.*/
  tp_conn = (TP0_CONN *) tpdu_cc->sock_info->user_conn_id;


  conn_id = (ST_ULONG) (tp_conn - tp0_conn_arr);	/* conn_id = index into array	*/

  switch (tp_conn->state)
    {
    case TP_STATE_CLOSED:
      tpdu_dr.reason = TP_DR_NO_SESSION;	/* We're not trying to connect*/
      tp0_send_dr (&tpdu_dr);
      break;
    case TP_STATE_WFCC:
      /* Check for Transport Class 0.  Don't allow any other.		*/
      if (tpdu_cc->preferred_class != 0)
        {
        unacceptable_cc = SD_TRUE;
        }
      /* Save negotiated TPDU len in tp_conn->max_tpdu_len.	*/
      switch (tpdu_cc->max_tpdu_len_enc)
        {
        case TP_PDU_MAX_SIZE_128  :	tp_conn->max_tpdu_len =   128; break;
        case TP_PDU_MAX_SIZE_256  :	tp_conn->max_tpdu_len =   256; break;
        case TP_PDU_MAX_SIZE_512  :	tp_conn->max_tpdu_len =   512; break;
        case TP_PDU_MAX_SIZE_1024 :	tp_conn->max_tpdu_len =  1024; break;
        case TP_PDU_MAX_SIZE_2048 :	tp_conn->max_tpdu_len =  2048; break;
        case TP_PDU_MAX_SIZE_4096 :	tp_conn->max_tpdu_len =  4096; break;
        case TP_PDU_MAX_SIZE_8192 :	tp_conn->max_tpdu_len =  8192; break;
        case TP_PDU_MAX_SIZE_65531:	tp_conn->max_tpdu_len = 65531; break;
        /* Default to ILLEGAL value (i.e. 0)	*/
        default:			tp_conn->max_tpdu_len =     0; break;
        }	/* end "switch"	*/
      /* If max_tpdu_len ILLEGAL or ">" proposed, this is protocol error.*/
      if (tp_conn->max_tpdu_len == 0  ||		/* ILLEGAL	*/
          tp_conn->max_tpdu_len > tp0_cfg.max_tpdu_len)	/* ">" proposed	*/
        {
        tpdu_dr.reason = TP_DR_NO_REASON;
        unacceptable_cc = SD_TRUE;
        }

      if (unacceptable_cc)
        {	/* UN-ACCEPTABLE CLASS 0 CC TPDU.	*/
        reason = 0xff;			/* Local user error	*/
        tp4_disconnect_ind (tp_conn->user_conn_id, reason,
                            0, NULL);	/* No user data passed up.	*/
        np_disconnect_req (tp_conn->sock_info);
        tp_conn->state = TP_STATE_CLOSED;
#if defined(SPDU_RX_NO_PREALLOCATE)
        M_FREE (MSMEM_STARTUP, tp_conn->spdu_rx.spdu_ptr);
#endif
        }
      else
        {	/* ACCEPTABLE CLASS 0 CC TPDU.	*/
        /* Save "rem_ref" in "tp_conn".			*/
        tp_conn->rem_ref = tpdu_cc->rem_ref;

        /* CRITICAL: set state before tp4_connect_cnf because may	*/
        /* send first Transport data from within tp4_connect_cnf.	*/
        tp_conn->state = TP_STATE_OPEN;
        tp4_connect_cnf (tp_conn->user_conn_id, conn_id+MIN_TP0_CONN_ID, 
                        (ST_UINT) tpdu_cc->udata_len, tpdu_cc->udata_buf);
        tp_conn->state = TP_STATE_OPEN;
        }
      break;

    default:
      TP_LOG_ERR1 ("TP-ERROR: invalid state (%d) for received CC. Ignored.",
        tp_conn->state);
      break;
    }		/* end "switch"	*/
  return;
  }

