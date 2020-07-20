/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : copp_dec.c						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements decoding of most confirm/indication*/
/*		functions.						*/
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
/*			u_cosp_bind_cnf					*/
/*			u_cosp_unbind_cnf				*/
/*									*/
/*			u_cosp_rel_cnf_pos				*/
/*			u_cosp_rel_ind					*/
/*			u_cosp_p_abort_ind 				*/
/*			u_cosp_data_ind 				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/17/09  JRB    13     Use new decoder (copp_dec_user..), avoids use*/
/*			   of global variables.				*/
/*			   Chg all bind_id, conn_id to (ST_VOID *).	*/
/*			   Use S_FMT_PTR for logging pointers.		*/
/* 02/26/09  JRB    12     Fix to handle PDV len=0 or User-data len=0.	*/
/* 04/28/08  GLB    11     Removed CALLED_ONLY and CALLING_ONLY         */
/* 01/02/02  JRB    10     Converted to use ASN1R (re-entrant ASN1)	*/
/* 04/24/01  JRB    09     Allow octet-aligned PDV. Don't think it's	*/
/*			   legal but some stacks (ONE?) may use it.	*/
/* 02/21/01  JRB    08     Fix previous change. Was checking wrong byte	*/
/*			   in PDU for Indef Len. Use global flag instead*/  
/* 12/20/00  JRB    07     asn1_skip_elmnt doesn't work for indef len	*/
/*			   encoding. Use "parse_next" instead.		*/
/* 08/13/98  JRB    06     Lint cleanup.				*/
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

/************************************************************************/
/*			u_cosp_bind_cnf					*/
/************************************************************************/
ST_VOID u_cosp_bind_cnf (ST_VOID *user_bind_id, ST_VOID *cosp_bind_id, ST_RET result)
  {
  COPP_LOG_DEC3 ("P-BIND.cnf:    copp_user_bind_id =" S_FMT_PTR "  copp_bind_id =" S_FMT_PTR "  result =%d",
    user_bind_id, cosp_bind_id, result);

  u_copp_bind_cnf (user_bind_id, cosp_bind_id, result);
  }

/************************************************************************/
/*			u_cosp_unbind_cnf				*/
/************************************************************************/
ST_VOID u_cosp_unbind_cnf (ST_VOID *user_bind_id)
  {
  COPP_LOG_DEC1 ("P-UNBIND.cnf:  copp_user_bind_id =" S_FMT_PTR "", user_bind_id);

  u_copp_unbind_cnf (user_bind_id);
  }

/* See "coppdcp.c" for "u_cosp_con_cnf_pos" function.			*/
/* See "coppdcpr.c" for "u_cosp_con_cnf_neg" function.			*/

/************************************************************************/
/*			u_cosp_rel_cnf_pos				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate positive	*/
/* result of the release operation.					*/
/* 									*/
/* Parameters:								*/
/*	acse_conn		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_rel_cnf_pos (ACSE_CONN *acse_conn)
  {
ST_INT8 abort_reason;
PDV_INFO pdv_info;	/* PDV info set by copp_dec_user_data_only	*/
ST_RET dec_ret;		/* decoder return value		*/

  if (acse_conn->copp_state != COPP_STATE_CONNECTED)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot process S-RELEASE.cnf+ (Illegal state=%d)",
      acse_conn->copp_state);
    return;
    }

  dec_ret = copp_dec_user_data_only (acse_conn->ppdu_ptr, acse_conn->ppdu_len,
            acse_conn->pciacse,	/* expected PCI (Release is ACSE)	*/
            &pdv_info);		/* decoded data				*/
  if (dec_ret)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: ASN.1 decode of S-Release.cnf failed, err=%d", dec_ret);
    abort_reason = 1;			/* unrecognized-ppdu	*/
    /* Don't send ARP PPDU.  COSP already thinks we're released.	*/
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else
    {
    /* Save ptr and len in acse_conn & call "release.cnf" function.	*/
    /* NOTE: ptr=NULL & len=0 if APDU is not encoded in the message.	*/
    acse_conn->apdu_ptr = pdv_info.pdv_data_ptr;
    acse_conn->apdu_len = pdv_info.pdv_data_len; 
    COPP_LOG_DEC2 ("P-RELEASE.cnf+:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
      acse_conn, acse_conn->user_conn_id);
    COPP_LOG_DECC1 ("User data length: %d", acse_conn->apdu_len);
    COPP_LOG_DECH (acse_conn->apdu_len, acse_conn->apdu_ptr);

    if (acse_conn->cr)
      {
      acse_conn->cr = SD_FALSE;
      acse_conn->rl = SD_FALSE;	/* and stay in COPP_STATE_CONNECTED	*/
      }
    else
      acse_conn->copp_state = COPP_STATE_IDLE;

    u_copp_rel_cnf_pos (acse_conn);
    }
  }

/* See "coppdcp.c" for "u_cosp_con_ind" function.			*/

/************************************************************************/
/*			u_cosp_rel_ind					*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* remote node wishes to release a connection.				*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*acse_conn		Pointer to connection info	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_rel_ind (ACSE_CONN *acse_conn)
  {
ST_INT8 abort_reason;
ST_INT8 event_id;
PDV_INFO pdv_info;	/* PDV info set by copp_dec_user_data_only	*/
ST_RET dec_ret;		/* decoder return value		*/

  if (acse_conn->copp_state != COPP_STATE_CONNECTED)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot process S-RELEASE.ind (Illegal state=%d)",
      acse_conn->copp_state);
    return;
    }
  /* Set release collision flags.	*/
  if (acse_conn->rl)
    acse_conn->cr = SD_TRUE;
  acse_conn->rl = SD_TRUE;

  dec_ret = copp_dec_user_data_only (acse_conn->ppdu_ptr, acse_conn->ppdu_len,
            acse_conn->pciacse,	/* expected PCI (Release is ACSE)	*/
            &pdv_info);		/* decoded data				*/
  if (dec_ret)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: ASN.1 decode of S-Release.ind failed, err=%d", dec_ret);
    acse_conn->copp_state = COPP_STATE_IDLE;
    abort_reason = 1;			/* unrecognized-ppdu	*/
    event_id = 14;			/* s-release-indication	*/
    /* Encode and send ARP PPDU.					*/
    copp_p_abort_req (acse_conn, abort_reason, event_id);
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else
    {
    /* Save ptr and len in acse_conn & call "release.ind" function.	*/
    /* NOTE: ptr=NULL & len=0 if APDU is not encoded in the message.	*/
    acse_conn->apdu_ptr = pdv_info.pdv_data_ptr;
    acse_conn->apdu_len = pdv_info.pdv_data_len;

    COPP_LOG_DEC2 ("P-RELEASE.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
      acse_conn, acse_conn->user_conn_id);
    COPP_LOG_DECC1 ("User data length: %d", acse_conn->apdu_len);
    COPP_LOG_DECH (acse_conn->apdu_len, acse_conn->apdu_ptr);

    u_copp_rel_ind (acse_conn);
    }
  }

/************************************************************************/
/*			u_cosp_p_abort_ind 				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* connection has been aborted by SS-provider (local or remote).	*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*acse_con	Pointer to connection info		*/
/*  ST_INT	reason		see def reasons COSP_P_AB_IND_... above	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_p_abort_ind (ACSE_CONN *acse_conn, ST_INT reason)
  {
  if (acse_conn->copp_state == COPP_STATE_IDLE)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot process S-P-ABORT.ind (Illegal state=%d)",
      acse_conn->copp_state);
    return;
    }

  COPP_LOG_DEC3 ("P-P-ABORT.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR 
    "  reason =%d",
    acse_conn, acse_conn->user_conn_id, reason + 0x100);

  acse_conn->copp_state = COPP_STATE_IDLE;
  u_copp_p_abort_ind (acse_conn, reason + 0x100);
  }

/************************************************************************/
/*			u_cosp_data_ind 				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* data SPDU has been received.						*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*acse_conn		Pointer to connection info	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_data_ind (ACSE_CONN *acse_conn)
  {
ST_INT8 abort_reason;
ST_INT8 event_id;
PDV_INFO pdv_info;	/* PDV info set by copp_dec_user_data_only	*/
ST_RET dec_ret;		/* decoder return value		*/

  if (acse_conn->copp_state != COPP_STATE_CONNECTED)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot process S-DATA.ind (Illegal state=%d)",
      acse_conn->copp_state);
    return;
    }

  dec_ret = copp_dec_user_data_only (acse_conn->ppdu_ptr, acse_conn->ppdu_len,
            acse_conn->pcimms,	/* expected PCI (Data is MMS)		*/
            &pdv_info);		/* decoded data				*/
  if (dec_ret)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: ASN.1 decode of S-Data.ind failed, err=%d", dec_ret);
    abort_reason = 1;			/* unrecognized-ppdu	*/
    event_id = -1;			/* Unlisted	*/
    /* Encode and send ARP PPDU.					*/
    copp_p_abort_req (acse_conn, abort_reason, event_id);
    /* Pass P-P-Abort.ind to COPP-user.					*/
    u_copp_p_abort_ind (acse_conn, abort_reason);
    }
  else
    {
    /* Save ptr and len in acse_conn, & call data indication function.	*/
    /* NOTE: ptr=NULL & len=0 if APDU is not encoded in the message.	*/
    acse_conn->apdu_ptr = pdv_info.pdv_data_ptr;
    acse_conn->apdu_len = pdv_info.pdv_data_len; 
    COPP_LOG_DEC2 ("P-DATA.ind:      acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
      acse_conn, acse_conn->user_conn_id);
    COPP_LOG_DECC1 ("User data length: %d", acse_conn->apdu_len);
    COPP_LOG_DECH (acse_conn->apdu_len, acse_conn->apdu_ptr);

    u_copp_data_ind (acse_conn);
    }
  }

