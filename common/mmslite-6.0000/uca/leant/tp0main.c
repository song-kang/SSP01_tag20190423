/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0main.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Main processsing functions for TP0.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	tp0_initialize ()						*/
/*	tp0_terminate ()						*/
/*	tp0_bind ()							*/
/*	tp0_unbind ()							*/
/*	tp0_session_timer ()						*/
/*	tp0_timer_tick ()						*/
/*	tp0_buffers_avail ()						*/
/*	tp0_event ()							*/
/*	tp0_data ()							*/
/*	tp0_disconnect ()						*/
/*	tp0_process_dt ()						*/
/*	tp0_process_dr ()						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 11/17/09  JRB    28     Chg *_bind_id to (ST_VOID *).		*/
/*			   Del MMSEASE_MOSI code.			*/
/* 07/30/08  JRB    27     Use DR reason codes valid for TP0.		*/
/* 06/23/06  JRB    26     Fix calc_loc_ref problem (conn_id wrong type)*/
/* 06/06/05  EJV    25     MMSEASE_MOSI: add rfc1006_listener_* funcs.	*/
/* 04/06/05  EJV    24     Added MMSEASE_MOSI support.			*/
/*			   tp0_terminate: check for valid tp0_conn_arr.	*/
/* 03/11/05  JRB    23     Ignore DT.ind or DR.ind if waiting for CR.ind*/
/* 02/22/05  JRB    22     tp0_data: fix return value.			*/
/* 01/24/05  JRB    21     Move tp4_debug_sel & "*_logstr" to lean_var.c*/
/* 09/17/04  JRB    20     tp0_session_timer: chk state only if starting*/
/*			   Use session_cfg.disconnect_timeout.		*/
/* 01/13/04  EJV    19     tp0_initialize: set tp0_cfg.max_spdu_outst.	*/
/*			   Changed tp0_send_... funcs to return value.	*/
/* 			   np_data_req: added eot param.		*/
/* 04/07/03  JRB    18     Add SPDU_RX_NO_PREALLOCATE option.		*/
/* 07/10/02  JRB    17     tp0_buffers_avail return INT_MAX (no limit).	*/
/* 06/20/02  JRB    16     Add tp0_convert_ip function.			*/
/* 05/08/02  JRB    15     Add tp0_connect_outstanding_count function.	*/
/* 01/22/02  JRB    14     Add max_conns arg to tp0_initialize.		*/
/* 05/25/01  JRB    13     Fix thisFile.. warning.			*/
/* 03/15/00  JRB    12     Alloc tp_conn->spdu_rx.spdu_ptr once at	*/
/*			   startup for each conn. Free before exit.	*/
/* 03/15/00  JRB    11     If spdu_rx exceeds max, disconnect.		*/
/* 01/05/01  EJV    10     tp0_initialize: corrected logging.		*/
/* 03/14/00  JRB    09     tp0_terminate: fake DR.ind so stack cleans up*/
/* 09/13/99  MDE    08     Added SD_CONST modifiers			*/
/* 06/08/99  JRB    07     Chg to support 1024 connections.		*/
/* 08/13/98  JRB    06     Lint cleanup.				*/
/* 03/23/98  NAV    05     Free g_tpkt_enc_buf on termination		*/
/* 02/16/98  JRB    04     Cast args.					*/
/* 09/25/97  JRB    03     Don't generate exception for invalid state.	*/
/* 09/23/97  JRB    02     tp0_terminate: disconnect all TP0 conns.	*/
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
/* Local function Prototypes						*/
/************************************************************************/

/************************************************************************/
/* Global Configuration Structure					*/
/* This structure must be filled in by user BEFORE tp0_initialize ().	*/
/* It MUST NOT change after tp0_initialize ().				*/
/* DEBUG: should tp0_initialize make it's own copy of this struct	*/
/*        so that the user CAN NOT screw it up?????			*/
/************************************************************************/
TP0_CFG tp0_cfg;

/************************************************************************/
/* Global Variables initialized by tp0_initialize ().			*/
/* Not accessible to user.						*/
/************************************************************************/
TP0_CONN *tp0_conn_arr;	/* ptr to array of "max_num_conns" structs	*/
ST_UCHAR *g_tpkt_enc_buf;		/* Buffer for encoding TPKTs.	*/
ST_UINT g_tp0_max_spdu_len;	/* Max SPDU size to send or receive	*/
				/* Was static but now used in other files*/

/************************************************************************/
/* Global Variables initialized by tp0_bind ().				*/
/* Not accessible to user.						*/
/************************************************************************/
#if !defined(SNAP_LITE)		/* SNAP_LITE code in other files	*/
/* DEBUG: should we check tp0_bind_called before any calls to user?	*/
static ST_INT tp0_bind_called;	/* Flag to prevent multiple bind calls.	*/
#endif
/* CRITICAL: only_tp0_bind_id must be different from only_tp4_bind_id,	*/
/*           because either may be passed up to user in tp4_bind_cnf.	*/
ST_VOID *only_tp0_bind_id = (ST_VOID *)199;	/* Only one tp0_bind_id.*/
ST_VOID *only_tp0_user_bind_id;		/* Only one user_bind_id.	*/
ST_UCHAR only_tp0_loc_tsap [1+MAX_TSEL_LEN];	/* Local len & TSAP addr*/

/*======================================================================*/
/*======================================================================*/
/* INITIALIZATION AND TERMINATION FUNCTIONS				*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/* tp0_initialize							*/
/* Initialize the TP0 service provider.					*/
/* NOTE:  the configuration parameters in the "tp0_cfg" struct must be	*/
/*        set BEFORE calling this function.				*/
/* WARNING: tp0_cfg MUST NOT be modified after this function called.	*/
/* NOTE:  the value of tp0_cfg.max_num_conns is overwritten with the	*/
/* 	  value passed in the argument max_conns.			*/
/* Return:								*/
/*	SD_SUCCESS or							*/
/*	error code (TP4E_* or error code returned from np_init)		*/
/************************************************************************/
ST_RET tp0_initialize (ST_UINT max_conns, ST_UINT max_spdu_len)
  {
ST_RET errcode;
ST_UINT16 max_tpkt_len;			/* Max len of RFC1006 TPKT.	*/
ST_UINT j;

  if (max_conns > TP_MAX_NUM_CONNS)
    {
    TP_LOG_ERR1 ("TP-ERROR: number of requested TCP connections (%d) exceeds max",
                 max_conns);
    return (TP4E_INVAL_NUM_CONNS);
    }
  tp0_cfg.max_num_conns = max_conns;	/* overwrite configured value	*/

#if !defined (TP4_ENABLED)	/* Otherwise, done in tp4_initialize	*/
#if defined(DEBUG_SISCO)
  tp4_debug_sel |= TP4_LOG_ERR;		/* Always enable error logging.	*/
#endif
  tp4_init_timer ();
#endif	/* !TP4_ENABLED	*/

  /* Save arg in global for later use.		*/
  g_tp0_max_spdu_len = max_spdu_len;

  /* Check that all configured parameter values are legal.		*/

  /* init the max number of SPDUs that can be queued in gensock2	*/
  if (tp0_cfg.max_spdu_outst == 0)
    tp0_cfg.max_spdu_outst = 50;

  /* Check max_tpdu_len. If legal, save binary encoding in global NOW.	*/
  switch (tp0_cfg.max_tpdu_len)
    {					/* code for PDU size	*/
    case   128:	tp0_cfg.max_tpdu_len_enc = TP_PDU_MAX_SIZE_128  ; break;
    case   256:	tp0_cfg.max_tpdu_len_enc = TP_PDU_MAX_SIZE_256  ; break;
    case   512:	tp0_cfg.max_tpdu_len_enc = TP_PDU_MAX_SIZE_512  ; break;
    case  1024:	tp0_cfg.max_tpdu_len_enc = TP_PDU_MAX_SIZE_1024 ; break;
    case  2048:	tp0_cfg.max_tpdu_len_enc = TP_PDU_MAX_SIZE_2048 ; break;
    case  4096:	tp0_cfg.max_tpdu_len_enc = TP_PDU_MAX_SIZE_4096 ; break;
    case  8192:	tp0_cfg.max_tpdu_len_enc = TP_PDU_MAX_SIZE_8192 ; break;
    case 65531:	tp0_cfg.max_tpdu_len_enc = TP_PDU_MAX_SIZE_65531; break;
    default:
      TP_LOG_ERR1 ("TP-ERROR: configured rfc1006_max_tpdu_len (%d) illegal",
                 tp0_cfg.max_tpdu_len);
      return (TP4E_INVAL_TPDU_LEN);
    }

  /* Compute Max TPKT len & allocate TPKT encode buffer.	*/
  max_tpkt_len = tp0_cfg.max_tpdu_len + RFC1006_HEAD_LEN;
  g_tpkt_enc_buf = (ST_UCHAR *) M_MALLOC (MSMEM_STARTUP, max_tpkt_len);

  /* Initialize the Network layer (i.e. RFC1006).			*/
  if ((errcode = np_init (max_conns)) != SD_SUCCESS)
    return (errcode);

  tp0_conn_arr = (TP0_CONN *) M_CALLOC (MSMEM_STARTUP, max_conns, sizeof (TP0_CONN));
  for (j=0; j<max_conns; j++)
    {
#if !defined(SPDU_RX_NO_PREALLOCATE)
    tp0_conn_arr [j].spdu_rx.spdu_ptr = (ST_CHAR *) M_MALLOC (MSMEM_STARTUP, g_tp0_max_spdu_len);
#endif
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/* tp0_terminate							*/
/************************************************************************/
ST_RET tp0_terminate (ST_VOID)
  {
ST_RET ret = SD_SUCCESS;
ST_UINT j;

  if (tp0_conn_arr)
    {
    for (j = 0;  j < tp0_cfg.max_num_conns;  j++)
      {
      if (tp0_conn_arr [j].state != TP_STATE_CLOSED)
        {
        /* Fake disconnect indication so Upper Layers can clean up.	*/
        tp4_disconnect_ind (tp0_conn_arr[j].user_conn_id, TP_DR_NO_REASON,
                            0, NULL);
        /* Send disconnect request.					*/
        tp0_disconnect (j + MIN_TP0_CONN_ID, 0, NULL);
        }
#if !defined(SPDU_RX_NO_PREALLOCATE)
      M_FREE (MSMEM_STARTUP, tp0_conn_arr [j].spdu_rx.spdu_ptr);
#endif
      }
    M_FREE (MSMEM_STARTUP, tp0_conn_arr);
    tp0_conn_arr = NULL;
    }
  if (g_tpkt_enc_buf)
    M_FREE (MSMEM_STARTUP, g_tpkt_enc_buf);

  ret = np_end ();
  return (ret);
  }
  
#if !defined(SNAP_LITE)		/* SNAP_LITE code in other files	*/
/************************************************************************/
/*			tp0_bind					*/
/* This function substitutes for "tp4_bind" when TP0/RFC1006 is being	*/
/* used.  See "tp4_bind" header for more info.				*/
/************************************************************************/
ST_RET tp0_bind (ST_VOID *user_bind_id, 
		  TP4_ADDR *tp4_addr, 
		  ST_INT sharable,
		  ST_INT max_conns)
  {
  if (tp4_addr->tp_type != TP_TYPE_TCP)
    {						/* Not for TP0	*/
    /* No other TP provider to pass to. This is error.	*/
    TP_LOG_ERR0 ("TP-ERROR: tp0_bind: invalid transport type");
    return (SD_FAILURE);
    }

  if (tp0_bind_called)
    {
    TP_LOG_ERR0 ("TP-ERROR: too many binds");
    return (TP4E_TOO_MANY_BINDS);
    }

  if (max_conns > tp0_cfg.max_num_conns)
    {
    TP_LOG_ERR2 ("TP-ERROR: requested TP conns (%d) greater than number configured (%d)",
                 max_conns, tp0_cfg.max_num_conns);
    return (TP4E_INVAL_NUM_CONNS);
    }

  /* Save user_bind_id in Global. Use in all calls to user functions.	*/
  only_tp0_user_bind_id = user_bind_id;

  /* Make sure Local TSAP len is legal before copying.			*/
  if (tp4_addr->tp4_sel_len > MAX_TSEL_LEN)
    {
    TP_LOG_ERR0 ("TP-ERROR: TSEL too long");
    return (TP4E_INVAL_TSEL);
    }

  /* Save Local TSAP (only do memcpy if len != 0).			*/
  if ((only_tp0_loc_tsap [0] = (ST_UCHAR) tp4_addr->tp4_sel_len) != 0)
    memcpy (&only_tp0_loc_tsap [1], tp4_addr->tp4_sel, tp4_addr->tp4_sel_len);
  tp4_bind_cnf (user_bind_id, only_tp0_bind_id, SD_SUCCESS);
  tp0_bind_called = SD_TRUE;	/* Only set if successful.		*/
  return (SD_SUCCESS);
  }
  
/************************************************************************/
/* tp0_unbind								*/
/************************************************************************/
ST_RET tp0_unbind (ST_VOID *tp0_bind_id)
  {
  if (tp0_bind_id != only_tp0_bind_id)
    return (SD_FAILURE);		/* Invalid bind id.		*/
  if (tp0_bind_called == SD_FALSE)
    return (SD_FAILURE);	/* Bind never made. Cannot unbind.	*/
  tp0_bind_called = SD_FALSE;	/* Clear flag so user can bind again.	*/
  tp4_unbind_cnf (only_tp0_user_bind_id);
  return (SD_SUCCESS);
  }
#endif	/* !defined(SNAP_LITE)	*/

/*======================================================================*/
/*======================================================================*/
/* TIMER FUNCTIONS							*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*                      tp0_session_timer				*/
/*----------------------------------------------------------------------*/
/* This function starts the session timer (for MOSI implementation).	*/
/* The user function tp4_session_timer_expired will be called if the	*/
/* timer expires.							*/
/*									*/
/* Parameters:								*/
/*  ST_LONG	tp_conn_id	TP0 connection id for this connect.	*/
/*  ST_BOOLEAN	start		SD_TRUE if timer should be started,	*/
/*				SD_FALSE if timer should be stopped.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		if not successful			*/
/************************************************************************/
ST_RET tp0_session_timer (ST_LONG tp_conn_id, ST_BOOLEAN start)
{
TP0_CONN *tp_conn = &tp0_conn_arr [tp_conn_id-MIN_TP0_CONN_ID];

  if (start)
    {
    if (tp_conn->state != TP_STATE_OPEN)
      {
      TP_LOG_ERR1 ("TP-ERROR: illegal connection state = %d", tp_conn->state);
      return (TP4E_CONN_STATE);
      }

    /* If configured, use configured val. It not, use reasonable val.*/
    if (session_cfg.disconnect_timeout)
      tp_conn->session_timer = session_cfg.disconnect_timeout;
    else
      tp_conn->session_timer = 60;
    }
  else
    tp_conn->session_timer = 0;		/* stopping the session timer	*/

  return (SD_SUCCESS);
}

/************************************************************************/
/*			tp0_timer_tick					*/
/*----------------------------------------------------------------------*/
/* This function is called on 1 second intervals by the TP0-user	*/
/* When this function is called, TP0 decrements all of the timers	*/
/* for all outstanding transport connections.				*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none				       	*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID tp0_timer_tick ()
  {
ST_UINT conn_id;
TP0_CONN *tp_conn;

  /* Because this function is called often and the entire tp0_conn_arr	*/
  /* must be checked, it must be very efficient.  Therefore, tp_conn	*/
  /* is used to point to each element of array (instead of		*/
  /* tp0_conn_arr[conn_id]).						*/
  for (conn_id = 0, tp_conn = &tp0_conn_arr[0];
       conn_id < tp0_cfg.max_num_conns;
       conn_id++, tp_conn++)
    {
    /* Handle "session timer" (MOSI implementation only)	*/
    /* Only decrement it if it's not 0 and state is legal.	*/
    if (tp_conn->state == TP_STATE_OPEN &&
        tp_conn->session_timer)
      {
      if (--tp_conn->session_timer == 0)
        tp4_session_timer_expired (tp_conn->user_conn_id);
      }		/* End "session timer" handling	*/
    }		/* End "for" loop	*/
  }

/************************************************************************/
/* tp0_buffers_avail							*/
/*----------------------------------------------------------------------*/
/* This function returns the number of buffers available for sending	*/
/* SPDUs.  This may be used, for example, when sending Information	*/
/* Reports, to make sure there are always enough buffers for sending	*/
/* responses to incoming requests.					*/
/*									*/
/* Parameters:								*/
/*	ST_LONG tp_conn_id		Transport Connection ID		*/
/*									*/
/* Return:								*/
/*	ST_INT			# of SPDU transmit buffers available	*/
/************************************************************************/
ST_INT tp0_buffers_avail (ST_LONG tp_conn_id)
  {
TP0_CONN *tp_conn = &tp0_conn_arr [tp_conn_id-MIN_TP0_CONN_ID];
ST_UINT spdu_que_cnt;

  spdu_que_cnt = np_get_tx_queue_cnt (tp_conn->sock_info);
  return (tp0_cfg.max_spdu_outst - spdu_que_cnt);
  }

/************************************************************************/
/* tp0_event								*/
/* Receive a n_unitdata NSDU, decode it, process it, and free it.	*/
/************************************************************************/
ST_BOOLEAN tp0_event ()
  {
  tp4_check_timer ();

  return (np_event ());
  }
  
/*======================================================================*/
/*======================================================================*/
/* FUNCTIONS BELOW CALLED BY USER TO ENCODE AND SEND TPDUs.		*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/* tp0_data								*/
/* RETURN CODES:							*/
/*	SD_SUCCESS	(if SPDU successfully queued for sending)	*/
/*      TP4E_CONN_STATE (connection not in proper state for this)	*/
/*	OR error returned from tp0_send_dt.				*/
/************************************************************************/
ST_RET tp0_data (ST_LONG tp_conn_id, ST_INT eot, ST_UINT spdu_len, ST_CHAR *spdu_ptr)
  {
ST_RET ret = SD_SUCCESS;
TP0_CONN *tp_conn = &tp0_conn_arr [tp_conn_id-MIN_TP0_CONN_ID];
ST_UINT spdu_offset;
TPDU_DT tpdu_dt_tx;		/* ptr to TPDU of interest.	*/

  if (tp_conn->state != TP_STATE_OPEN)
    {
    TP_LOG_ERR1 ("TP-ERROR: illegal connection state = %d", tp_conn->state);
    return (TP4E_CONN_STATE);
    }

  tpdu_dt_tx.dst_ref = tp_conn->rem_ref;
  tpdu_dt_tx.sock_info = tp_conn->sock_info;

  /* Set up TPDU user data by pointing into SPDU.	*/
  for (spdu_offset = 0;  spdu_offset < spdu_len;  )
    {
    tpdu_dt_tx.udata_ptr = spdu_ptr + spdu_offset;
    if (spdu_len - spdu_offset <= (ST_UINT)(tp_conn->max_tpdu_len - TP0_HEAD_LEN_DT))
      {
      tpdu_dt_tx.udata_len = spdu_len - spdu_offset;
      tpdu_dt_tx.eot = SD_TRUE;
      }
    else
      {
      TP_LOG_REQ0 ("TP-INFO: SPDU being segmented. Should increase TPDU size.");
      /* Send largest allowed TPDU, taking header size into account.	*/
      tpdu_dt_tx.udata_len = (tp_conn->max_tpdu_len - TP0_HEAD_LEN_DT);
      tpdu_dt_tx.eot = SD_FALSE;
      }
    spdu_offset += tpdu_dt_tx.udata_len;

    ret = tp0_send_dt (&tpdu_dt_tx);
    if (ret != SD_SUCCESS)
      break;
    }	/* end "for"	*/

  return (ret);
  }

/************************************************************************/
/*			tp0_disconnect					*/
/* This function is called by the user to break connection		*/
/************************************************************************/
ST_RET tp0_disconnect (ST_LONG tp0_conn_id,
		        ST_INT conndata_len,
		        char *conndata)
  {
TP0_CONN *tp_conn;	/* ptr to struct in "tp0_conn_arr" array.	*/
TPDU_DR tpdu_dr;
ST_UINT16 conn_id = (ST_UINT16) tp0_conn_id - MIN_TP0_CONN_ID;

  if (conn_id >= tp0_cfg.max_num_conns)
    return (TP4E_BADCONN);

  tp_conn = &tp0_conn_arr [conn_id];

  /* Fill in TPDU_DR struct in case DR needs to be sent.		*/
  tpdu_dr.loc_ref = calc_loc_ref (conn_id, tp_conn->loc_ref_offset);
  tpdu_dr.rem_ref = tp_conn->rem_ref;
  tpdu_dr.sock_info = tp_conn->sock_info;
  tpdu_dr.reason = TP_DR_NO_REASON;

  switch (tp_conn->state)
    {
    case TP_STATE_OPEN:
    case TP_STATE_WFCC:
    case TP_STATE_WFNC:
      np_disconnect_req (tp_conn->sock_info);
      tp_conn->state = TP_STATE_CLOSED;
#if defined(SPDU_RX_NO_PREALLOCATE)
      M_FREE (MSMEM_STARTUP, tp_conn->spdu_rx.spdu_ptr);
#endif
      break;
    case TP_STATE_WFTRESP:
      /* Overwrite assumed "loc_ref". Not valid in this state.	*/
      tpdu_dr.loc_ref = 0;
      tp0_send_dr (&tpdu_dr);
      tp_conn->state = TP_STATE_CLOSED;
#if defined(SPDU_RX_NO_PREALLOCATE)
      M_FREE (MSMEM_STARTUP, tp_conn->spdu_rx.spdu_ptr);
#endif
      break;
    default:
      TP_LOG_ERR1 ("TP-ERROR: invalid state (%d) for sending DR. Not sent.",
        tp_conn->state);
      return (SD_FAILURE);
    }	/* end "switch"	*/
  return (SD_SUCCESS);
  }
    
/*======================================================================*/
/*======================================================================*/
/* FUNCTIONS BELOW CALLED BY TP0 TO PROCESS TPDUs RECEIVED		*/
/* (TPDUs ALREADY DECODED BY TP0).					*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/* tp0_process_dt							*/
/* Only one SPDU per connection is ever being processed.  As soon as	*/
/* the EOT is received, the SPDU is passed up to the Transport-user.	*/
/************************************************************************/
ST_VOID tp0_process_dt (TPDU_DT *tpdu_dt_rx)
  {
TP0_CONN *tp_conn;

  if (tpdu_dt_rx->sock_info->user_conn_id == INVALID_CONN_ID)
    {
    TP_LOG_ERR0 ("TP-ERROR: unexpected TPDU-DT.ind ignored. Waiting for TPDU-CR.ind.");
    return;
    }

  tp_conn = (TP0_CONN *) tpdu_dt_rx->sock_info->user_conn_id;
  if (tp_conn - tp0_conn_arr > tp0_cfg.max_num_conns)	/* check "conn_id"*/
    {
    TP_LOG_ERR0 ("TP-ERROR: illegal connection id");
    return;	/* conn_id is invalid.  Ignore this TPDU.		*/
    }
  
  switch (tp_conn->state)
    {
    case TP_STATE_CLOSED:
      return;			/* Ignore this DT.			*/
    case TP_STATE_OPEN:
      break;			/* Continue				*/
    default:
      TP_LOG_ERR1 ("TP-ERROR: invalid state (%d) for received DT. Ignored.",
        tp_conn->state);
      return;
    }
    
  /* If this is beginning of SPDU, initialize buffer for it.		*/
  if (tp_conn->spdu_rx_start)	/* init when CR rcvd or CR sent 	*/
    {		/* Last TPDU contained EOT. This TPDU starts new SPDU.	*/
    /* We can freely overwrite "spdu_rx.spdu_ptr" because the user	*/
    /* is now responsible for the last one.				*/
    tp_conn->spdu_rx.spdu_len = 0;
    }

  if ((ST_UINT) (tp_conn->spdu_rx.spdu_len + tpdu_dt_rx->udata_len)
      > g_tp0_max_spdu_len)
    {
    /* Would exceed max SPDU size. CAN'T DO memcpy. WON'T FIT IN BUFFER.*/
    /* No good recovery, so just disconnect.				*/
    /* NOTE: This is really a Session problem, but we handle it here.	*/

    TP_LOG_ERR1 ("Received msg exceeds max_spdu_len (%d)", g_tp0_max_spdu_len);

    /* Pass up disconnect indication.	*/
    tp4_disconnect_ind (tp_conn->user_conn_id, TP_DR_NO_REASON,
                        0, NULL);
    /* Send disconnect request (compute conn_id from tp_conn.	*/
    tp0_disconnect ((ST_LONG) (tp_conn - tp0_conn_arr + MIN_TP0_CONN_ID), 0, NULL);
    return;					/* DO NOT CONTINUE	*/
    }

  memcpy (tp_conn->spdu_rx.spdu_ptr + tp_conn->spdu_rx.spdu_len, tpdu_dt_rx->udata_ptr,
          tpdu_dt_rx->udata_len);
  tp_conn->spdu_rx.spdu_len += tpdu_dt_rx->udata_len;

  if (tpdu_dt_rx->eot)
    {		/* This TPDU contains EOT. This TPDU ends SPDU.		*/
    tp_conn->spdu_rx_start = SD_TRUE;		/* Next TPDU will begin a SPDU.	*/

    /* Pass up SPDU to user. Note that we always pass up complete SPDU	*/
    /* so eot is always SD_TRUE.					*/
    /* "tp4_data_ind" must completely process the data at spdu_ptr.	*/
    /* When it returns, the buffer may be reused.			*/

    tp4_data_ind (tp_conn->user_conn_id, tpdu_dt_rx->eot, tp_conn->spdu_rx.spdu_len,
                  tp_conn->spdu_rx.spdu_ptr);
    }
  else
    tp_conn->spdu_rx_start = SD_FALSE;	/* Next TPDU is part of this SPDU*/
  }

/************************************************************************/
/* tp0_process_dr							*/
/* Process a DR (Disconnect Request) TPDU.				*/
/************************************************************************/
ST_VOID tp0_process_dr (TPDU_DR *tpdu_dr)
  {
TP0_CONN *tp_conn;
ST_INT reason;

  if (tpdu_dr->sock_info->user_conn_id == INVALID_CONN_ID)
    {
    TP_LOG_ERR0 ("TP-ERROR: unexpected TPDU-DR.ind ignored. Waiting for TPDU-CR.ind.");
    return;
    }

  /* tp_conn was saved in "sock_info" (as user_conn_id) when CR sent/rcvd*/
  tp_conn = (TP0_CONN *) tpdu_dr->sock_info->user_conn_id;

  switch (tp_conn->state)
    {
    case TP_STATE_CLOSED:
      break;
    case TP_STATE_WFCC:
      reason = 0;			/* Normal disconnection	*/
      /* NOTE: To save memory, user data is never passed up to user.	*/
      tp4_disconnect_ind (tp_conn->user_conn_id, reason,
                          0, NULL);	/* No user data passed up.	*/
      np_disconnect_req (tp_conn->sock_info);	/* Disconnect network conn.*/
      tp_conn->state = TP_STATE_CLOSED;	/* Skip REFWAIT state.		*/
#if defined(SPDU_RX_NO_PREALLOCATE)
      M_FREE (MSMEM_STARTUP, tp_conn->spdu_rx.spdu_ptr);
#endif
      break;
    case TP_STATE_OPEN:
      TP_LOG_ERR0 ("TP-ERROR: Received TP0 DR in OPEN state. Ignoring.");
      break;
    default:
      TP_LOG_ERR1 ("TP-ERROR: invalid state (%d) for received DR. Ignored.",
        tp_conn->state);
      break;
    }
  return;
  }

/*======================================================================*/
/*======================================================================*/
/* INTERNAL TP0 FUNCTIONS						*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/* tp0_conn_clean							*/
/* Initialize connection struct for a new connection.			*/
/************************************************************************/
ST_VOID tp0_conn_clean (TP0_CONN *tp_conn)
  {
  tp_conn->spdu_rx_start = SD_TRUE;	/* CRITICAL: Initialize flag.	*/
#if defined(SPDU_RX_NO_PREALLOCATE)
  tp_conn->spdu_rx.spdu_ptr = (ST_CHAR *) M_MALLOC (MSMEM_STARTUP, g_tp0_max_spdu_len);
#endif
  return;
  }

/************************************************************************/
/* tp0_send_dt								*/
/* Send a DT (Data) TPDU.						*/
/************************************************************************/
ST_RET tp0_send_dt (TPDU_DT *tpdu_dt)
  {
ST_RET ret = SD_FAILURE;
ST_UINT16 data_len;

  tp_log_tsdu ((ST_VOID *) tpdu_dt, (ST_UCHAR) TP_PDU_TYPE_DT, TP_REQ);

  /* Fill in g_tpkt_enc_buf with TPDU					*/
  data_len = tp0_encode_dt (g_tpkt_enc_buf + RFC1006_HEAD_LEN, tpdu_dt);
  if (data_len > 0)
    {
    /* Fill in g_tpkt_enc_buf HEADER					*/
    data_len += RFC1006_HEAD_LEN;	/* include header in len	*/
    g_tpkt_enc_buf [0] = 3;				/* vrsn		*/
    g_tpkt_enc_buf [1] = 0;				/* reserved	*/
    g_tpkt_enc_buf [2] = (ST_UCHAR) (data_len >> 8);	/* high byte	*/
    g_tpkt_enc_buf [3] = (ST_UCHAR) (data_len & 0xff);	/* low byte	*/
    ret = np_data_req (tpdu_dt->sock_info, data_len, g_tpkt_enc_buf, tpdu_dt->eot);
    }
  return (ret);
  }

/************************************************************************/
/* tp0_send_cr								*/
/* Send a CR (Connect Request) TPDU.					*/
/************************************************************************/
ST_RET tp0_send_cr (TPDU_CX *tpdu_cr)
  {
ST_RET ret = SD_FAILURE;
ST_UINT16 data_len;

  tp_log_tsdu ((ST_VOID *) tpdu_cr, (ST_UCHAR) TP_PDU_TYPE_CR, TP_REQ);

  /* Fill in g_tpkt_enc_buf with TPDU					*/
  data_len = tp_encode_cx (g_tpkt_enc_buf + RFC1006_HEAD_LEN, tpdu_cr,
  				       TP_PDU_TYPE_CR);
  if (data_len > 0)
    {
    /* Fill in g_tpkt_enc_buf HEADER					*/
    data_len += RFC1006_HEAD_LEN;	/* include header in len	*/
    g_tpkt_enc_buf [0] = 3;				/* vrsn		*/
    g_tpkt_enc_buf [1] = 0;				/* reserved	*/
    g_tpkt_enc_buf [2] = (ST_UCHAR) (data_len >> 8);	/* high byte	*/
    g_tpkt_enc_buf [3] = (ST_UCHAR) (data_len & 0xff);	/* low byte	*/
    ret = np_data_req (tpdu_cr->sock_info, data_len, g_tpkt_enc_buf, SD_TRUE);
    }
  return (ret);
  }

/************************************************************************/
/* tp0_send_cc								*/
/* Send a CC (Connect Confirm) TPDU.					*/
/************************************************************************/
ST_RET tp0_send_cc (TPDU_CX *tpdu_cc)
  {
ST_RET ret = SD_FAILURE;
ST_UINT16 data_len;

  tp_log_tsdu ((ST_VOID *) tpdu_cc, (ST_UCHAR) TP_PDU_TYPE_CC, TP_REQ);

  /* Fill in g_tpkt_enc_buf with TPDU					*/
  data_len = tp_encode_cx (g_tpkt_enc_buf + RFC1006_HEAD_LEN, tpdu_cc,
  				       TP_PDU_TYPE_CC);
  if (data_len > 0)
    {
    /* Fill in g_tpkt_enc_buf HEADER					*/
    data_len += RFC1006_HEAD_LEN;	/* include header in len	*/
    g_tpkt_enc_buf [0] = 3;				/* vrsn		*/
    g_tpkt_enc_buf [1] = 0;				/* reserved	*/
    g_tpkt_enc_buf [2] = (ST_UCHAR) (data_len >> 8);	/* high byte	*/
    g_tpkt_enc_buf [3] = (ST_UCHAR) (data_len & 0xff);	/* low byte	*/
    ret = np_data_req (tpdu_cc->sock_info, data_len, g_tpkt_enc_buf, SD_TRUE);
    }
  return (ret);
  }

/************************************************************************/
/* tp0_send_dr								*/
/* Send a DR (Disconnect Request) TPDU.					*/
/************************************************************************/
ST_RET tp0_send_dr (TPDU_DR *tpdu_dr)
  {
ST_RET ret = SD_FAILURE;
ST_UINT16 data_len;

  tp_log_tsdu ((ST_VOID *) tpdu_dr, TP_PDU_TYPE_DR, TP_REQ);

  /* Fill in g_tpkt_enc_buf with TPDU					*/
  data_len = tp_encode_dr (g_tpkt_enc_buf + RFC1006_HEAD_LEN, tpdu_dr);
  if (data_len > 0)
    {
    /* Fill in g_tpkt_enc_buf HEADER					*/
    data_len += RFC1006_HEAD_LEN;	/* include header in len	*/
    g_tpkt_enc_buf [0] = 3;				/* vrsn		*/
    g_tpkt_enc_buf [1] = 0;				/* reserved	*/
    g_tpkt_enc_buf [2] = (ST_UCHAR) (data_len >> 8);	/* high byte	*/
    g_tpkt_enc_buf [3] = (ST_UCHAR) (data_len & 0xff);	/* low byte	*/
    ret = np_data_req (tpdu_dr->sock_info, data_len, g_tpkt_enc_buf, SD_TRUE);
    }
  return (ret);
  }

/************************************************************************/
/*		tp0_connect_outstanding_count				*/
/* NOTE: can't presently use tp4Addr for anything, but in the future	*/
/* we may use it to check only for outstanding connections to the same	*/
/* IP Address.								*/
/************************************************************************/
ST_INT tp0_connect_outstanding_count (TP4_ADDR *tp4Addr)
  {
ST_INT connectOutstandingCount = 0;
ST_INT j;

  for (j=0; j<tp0_cfg.max_num_conns; j++)
    {
    /* Waiting for Network connect (i.e., socket connect) to complete?	*/
    if (tp0_conn_arr [j].state == TP_STATE_WFNC)
      connectOutstandingCount++;		/* YES, increment count	*/
    }
  return (connectOutstandingCount);
  }

/************************************************************************/
/*			tp0_convert_ip					*/
/* RETURNS: IP Address in network byte order OR				*/
/*          "htonl(INADDR_NONE)" if conversion fails.			*/
/************************************************************************/
ST_ULONG tp0_convert_ip (ST_CHAR *pHostName)
  {
ST_ULONG ipAddr;

  ipAddr = inet_addr(pHostName);
  if (ipAddr == htonl(INADDR_NONE))
    {
#if defined (VXWORKS)		/* gethostbyname not supported	*/
    TP_LOG_ERR1 ("Invalid IP address %s", pHostName);
#else	/* !VXWORKS	*/
HOSTENT	*pHostEnt;		/* host database entry for remote host	*/
    pHostEnt = gethostbyname(pHostName);
    if (pHostEnt == NULL)
      {
      TP_LOG_ERR1 ("Can't get IP address for host %s", pHostName);
      }
    else
      ipAddr = *(ST_UINT32 *)(pHostEnt->h_addr);
#endif	/* !VXWORKS	*/
    }
  return (ipAddr);
  }
