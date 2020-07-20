/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995-2009, All Rights Reserved					*/
/*									*/
/* MODULE NAME : acse2dec.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	ACSE Decode Functions.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    62     Use new decoder (acse_dec_*), avoids use	*/
/*			   of global variables.				*/
/* 11/01/09  EJV    61     Merge these MMS-EASE changes:		*/
/*			   Use S_FMT_PTR macro to log pointers.		*/
/*			   Merged some continuation slogs into one slog.*/
/*			   Chg xxx_bind_id from ST_LONG to ST_VOID*	*/
/*                           to support all 64-bit systems.		*/
/*			   Made MOSI the default stack code.		*/
/*			   Del REDUCED_STACK and TRIM7 code.		*/
/* 02/26/09  JRB    60     Fix to handle PDV len=0 or user_info len=0.	*/
/* 04/28/08  GLB    59     Removed CALLED_ONLY and CALLING_ONLY         */
/* 01/30/06  GLB    58     Integrated porting changes for VMS           */
/* 07/06/05  EJV    57     Log "IP ADDR:" on the same line as the ip.	*/
/* 05/19/05  JRB    56     Chg "unrecognized tag" logs from ERR to DEC.	*/
/* 03/31/05  EJV    55     a_get_rem_ip_addr: added defined(linux)	*/
/* 09/20/04  JRB    54     u_copp_con_cnf: don't access acse_conn after	*/
/*			   copp_u_abort_req (it may have been freed).	*/
/* 08/06/04  EJV    53     _a_dec_authentication: chg param to ST_UCHAR.*/
/* 03/15/04  GLB    51     Delete "iso_mms_is_ASO_context_name"         */
/* 03/04/04  JRB    51     Use ASN1 decode log macros (ALOG_DEC*)	*/
/*			   Del logging of A-DATA.ind (use MVL or COPP)	*/
/* 02/06/04  EJV    50	   Use MACE_MECH_ID define.			*/
/* 01/08/04  EJV    49     Ported funcs to sun.				*/
/* 12/12/03  JRB    48     Fix asn1r_get_bitstr calls, init data=0 first*/
/* 10/24/03  JRB    47     Chg #if ..TP0.. to avoid compile errors.	*/
/* 07/08/03  EJV    46     Del old password auth (OBSOLETE_ACSE_AUTH);	*/
/*			   ACSE_AUTH_ENABLED code wo/define.		*/
/*			   Fix the ACSE Auth decode.			*/
/* 03/31/03  JRB    45     asn1r_get_bitstr: add max_bits arg.		*/
/* 11/08/02  JRB    44     asn1_skip_elmnt ret error if indef len.	*/
/* 10/29/02  JRB    43     Fix a_auth_password_dec ret if pwd too long.	*/
/* 10/22/02  ASK    42	   Wrap ifdef TP0_ENABLED around inet_ntoa calls*/
/* 10/14/02  ASK    41     Added decode of abort diagnostic.		*/
/* 10/03/02  ASK    40     Now send abort when AARQ fails to decode	*/
/* 09/27/02  ASK    39     Added ACSE auth decode to AARE, moved 	*/
/*			   bitstring defines into acse2.h		*/	
/* 06/26/02  JRB    38     con_ind: Log calling/called address.		*/
/* 06/20/02  JRB    37     con_ind: Copy calling/called PADDR to AARQ.	*/
/* 01/10/02  EJV    36     Converted to use ASN1R for TRIM7		*/
/* 01/02/02  JRB    35     Converted to use ASN1R (re-entrant ASN1)	*/
/* 10/09/01  JRB    34     Log if unrecognized tag received.		*/
/* 06/22/01  JRB    33     Fix u_copp_rel_cnf_pos & tp4_data_ind	*/
/*			   handling of STATE_RESP_COLL			*/
/*			   state per ISO 8650 (was kluged before).	*/
/* 03/15/01  JRB    32     Use new SMEM allocation functions.		*/
/* 01/02/01  EJV    31     Ported to _AIX				*/
/* 04/12/00  JRB    30     Lint cleanup.				*/
/* 09/13/99  MDE    29     Added SD_CONST modifiers			*/
/* 07/14/99  JRB    28     Add ACSE Password Authentication.		*/
/* 07/14/99  JRB    27     Add functions to get remote NSAP or IP Addr.	*/
/* 08/13/98  JRB    26     Lint cleanup.				*/
/* 08/11/98  JRB    25     BUG_FIX: MOSI: only save user_info if PCI=MMS*/
/* 03/18/98  JRB    24     Use defines (not variables) for ASN1 errors.	*/
/* 11/07/97  JRB    23     Can't use // for comments.			*/
/* 07/13/97  EJV    22     Commented out call to exit() in tp4_except.	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/19/97  JRB    21     Add MOSI "CALLING" support.			*/
/* 03/19/97  JRB    20     tp4_connect_cnf send AARQ (already encoded).	*/
/*			   Use acse_free_conn to free conn ctrl struct.	*/
/*			   Improve logging.				*/
/*			   Improve release collision handling.		*/
/* 03/07/97  JRB    19     Only pass abort to user if not in IDLE state.*/
/* 03/07/97  JRB    18     Don't call acse_decode unless class==APP.	*/
/* 03/07/97  JRB    17     Use new asn1_skip_elmnt funct instead of	*/
/*			   inline code and "parse_next" calls.		*/
/* 03/07/97  JRB    16     Added MOSI support.				*/
/* 08/12/96  JRB    15     Handle decode error gracefully in		*/
/*			   tp4_data_ind_aarqind & tp4_data_ind_aarqcnf.	*/
/* 07/15/96  JRB    14     Only free acse_conn->aarq_apdu.user_info.ptr	*/
/*			   if len != 0 (wasn't alloc'd otherwise).	*/
/* 07/08/96  JRB    13     Add tp4_except for LEAN_T.			*/
/* 06/25/96  JRB    12     Fix return codes for tp4_data_ind_aarq*	*/
/* 05/02/96  JRB    11     Add STATE_TP4CONN_WAIT.			*/
/* 04/25/96  JRB    10     Set acse_conn->initiator = SD_TRUE in		*/
/*			   tp4_data_ind_aarqcnf (for release collisions)*/
/* 04/18/96  JRB    09     if (acse_conn->state == STATE_AARE_ABANDON)	*/
/*			   in tp4_connect_cnf or tp4_data_ind, call	*/
/*			   tp4_disconnect and free acse_conn.		*/
/* 04/16/96  JRB    08     Copy rcv_data and rcv_data_len to local	*/
/*			   variables in tp4_data_ind for safe freeing.	*/
/* 03/26/96  MDE    07     Added REDUCED_STACK support			*/
/* 03/11/96  JRB    06     Move acse_msgsize to acse2enc.c		*/
/* 03/08/96  JRB    05     Define thisFileName even if !DEBUG_MMS.	*/
/* 03/07/96  JRB    04     Set default for acse_msgsize in case		*/
/*			   a_set_msgsize not called by user.		*/
/* 02/16/96  JRB    03     Clean up asn1_set_dec_err codes.			*/
/* 12/29/95  JRB    02     Broke up tp4_data_ind into subfunctions.	*/
/* 10/30/95  JRB    01     Created					*/
/************************************************************************/
static char *thisFileName = __FILE__;

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "asn1defs.h"
#include "asn1log.h"	/* for ASN.1 decode logging	*/
#include "acseauth.h"
#include "acse2.h"
#include "copp_usr.h"
#include "tp4api.h"
#include "tp4.h"	/* need MIN_TP0_CONN_ID	definition	*/

/************************************************************************/
/*			u_copp_con_cnf					*/
/* P-CONNECT.cnf (Connect Confirm).					*/
/* Parameters:								*/
/*	acse_conn	Pointer to connection info			*/
/*	result		P_CON_RESULT_ACCEPT, P_CON_RESULT_USER_REJ or	*/
/*			P_CON_RESULT_PROVIDER_REJ			*/
/*	reason		Provider-reason (if result=P_CON_RESULT_PROVIDER_REJ)*/
/************************************************************************/
ST_VOID u_copp_con_cnf (ACSE_CONN *acse_conn, ST_INT result, ST_INT reason)
  {
ST_RET dec_ret;		/* decoder return value	*/
AARE_APDU aare_apdu;	/* AARE APDU decoded	*/
ABRT_APDU abrt_apdu;	/* Abort APDU to pass to u_a_abort_ind (on err)	*/

  if (acse_conn->state != STATE_AARE_WAIT)
    {
    ACSELOG_ERR0 ("ACSE-ERROR: Ignoring unexpected P-CONNECT.cnf");
    return;
    }
  if (result == P_CON_RESULT_PROVIDER_REJ)
    {	/* No AARE to decode.	*/
    ACSELOG_DEC3 ("A-ASSOCIATE.cnf-: acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                  "  Rejected by PS-provider. Provider-reason = %d. No P-user-data.",
                  acse_conn, acse_conn->user_conn_id, reason);

    /* ACSE spec says to pass "A-ASSOCIATE.cnf-" primitive to		*/
    /* ACSE-user, but ACSE-user may not handle it with no AARE info, so	*/
    /* pass "A-P-ABORT.ind" primitive instead.  The result is the same.	*/
    abrt_apdu.source = ACSE_SERVICE_PROVIDER;
    abrt_apdu.user_info.len = 0;
    u_a_abort_ind (acse_conn->user_conn_id, &abrt_apdu);
    acse_conn->state = STATE_IDLE;
    /* DON'T free acse_conn. COSP will free it when T-DISCONNECT done.	*/
    return;
    }

  /* result != P_CON_RESULT_PROVIDER_REJ.  Decode AARE.			*/
  /* Store decoded data in the "aare_apdu" structure.			*/
  dec_ret = acse_dec_aare (acse_conn->apdu_ptr,acse_conn->apdu_len, acse_conn->pcimms, &aare_apdu);
  if (dec_ret == SD_SUCCESS)
    {						/* Handle AARE.		*/
    if (aare_apdu.result == 0)
      {
      acse_conn->initiator = SD_TRUE;
      acse_conn->state = STATE_ASSOCIATED;
      ACSELOG_DEC2 ("A-ASSOCIATE.cnf+: acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
        acse_conn, acse_conn->user_conn_id);
      }
    else
      {
      acse_conn->state = STATE_IDLE;
      ACSELOG_DEC2 ("A-ASSOCIATE.cnf-: acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
        acse_conn, acse_conn->user_conn_id);
      }

    /* save partner's security info to the ACSE_CONN for user's anytime examination */
    acse_conn->part_auth_info = aare_apdu.auth_info;
    if (aare_apdu.auth_info.mech_type == ACSE_AUTH_MECH_OTHER)
      {
      /* allocate buffer for the Authentication Value */
      acse_conn->part_auth_info.u.other_auth.auth_value.buf = acse_conn->part_auth_info.u.other_auth.auth_value.ptr =
           (ST_UCHAR *) chk_calloc (aare_apdu.auth_info.u.other_auth.auth_value.len, sizeof(ST_CHAR));
      memcpy (acse_conn->part_auth_info.u.other_auth.auth_value.ptr, aare_apdu.auth_info.u.other_auth.auth_value.ptr,
              aare_apdu.auth_info.u.other_auth.auth_value.len);
      }
    aare_apdu.encrypt_ctrl = acse_conn->encrypt_ctrl;

    u_a_associate_cnf (acse_conn, acse_conn->user_conn_id,
             &aare_apdu);
    }
  else
    {					/* Handle ASN1 decode error.	*/
    ACSELOG_ERR0 ("ACSE-ERROR: AARE decode failed. Aborting.");
    /* Pass up A-P-ABORT.ind	*/
    abrt_apdu.source = ACSE_SERVICE_PROVIDER;
    abrt_apdu.user_info.len = 0;
    u_a_abort_ind (acse_conn->user_conn_id, &abrt_apdu);

    acse_conn->state = STATE_IDLE;

    /* NOTE: Send copp_u_abort_req ONLY IF COPP accepted conn.		*/
    if (result == P_CON_RESULT_ACCEPT)
      copp_u_abort_req (acse_conn);
    /* CRITICAL: DO NOT access acse_conn after abort. It may have been	*/
    /*     freed by cosp_u_abort_req (called from copp_u_abort_req).	*/
    }
  return;
  }

/************************************************************************/
/*			u_copp_rel_cnf_pos				*/
/* P-RELEASE.cnf+ (POSITIVE Release Confirm).				*/
/* Must be RLRE APDU.  Decode it and call u_a_release_cnf.		*/
/************************************************************************/
ST_VOID u_copp_rel_cnf_pos (ACSE_CONN *acse_conn)
  {
ST_RET dec_ret;		/* decoder return value	*/
RLRE_APDU rlre_apdu;	/* RLRE APDU decoded	*/
ABRT_APDU abrt_apdu;	/* Abort APDU to pass to u_a_abort_ind (on err)	*/

  /* Decode the RLRE APDU & store data in the "rlre_apdu" structure.	*/
  dec_ret = acse_dec_rlre (acse_conn->apdu_ptr,acse_conn->apdu_len, acse_conn->pcimms, &rlre_apdu);
  if (dec_ret == SD_SUCCESS)
    {
    /* Handle RLRE.		*/
    switch (acse_conn->state)
      {
      case STATE_RLRE_WAIT:
      acse_conn->state = STATE_IDLE;
      ACSELOG_DEC2 ("A-RELEASE.cnf+:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
        acse_conn, acse_conn->user_conn_id);
      u_a_release_cnf (acse_conn->user_conn_id, &rlre_apdu);
      break;

      case STATE_RESP_COLL:
      acse_conn->state = STATE_ARLSRSP_WAIT;
      ACSELOG_DEC2 ("A-RELEASE.cnf+:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
        acse_conn, acse_conn->user_conn_id);
      u_a_release_cnf (acse_conn->user_conn_id, &rlre_apdu);
      break;

      default:
      u_a_except (acse_conn->user_conn_id, EX_ACSE_INVALID_STATE, 
              thisFileName, __LINE__);
      break;
      }	/* end "switch"	*/
    }
  else
    {
    /* Handle ASN1 decode error.	*/
    ACSELOG_ERR0 ("ACSE-ERROR: RLRE decode failed. Aborting.");
    /* Pass up A-P-ABORT.ind	*/
    abrt_apdu.source = ACSE_SERVICE_PROVIDER;
    abrt_apdu.user_info.len = 0;
    u_a_abort_ind (acse_conn->user_conn_id, &abrt_apdu);

    acse_conn->state = STATE_IDLE;
    }
  }


/************************************************************************/
/* u_copp_con_ind							*/
/* P-Connect Indication.						*/
/* Must be AARQ APDU.  Decode it and call u_a_associate_ind.		*/
/************************************************************************/
ST_VOID u_copp_con_ind (ST_VOID *user_bind_id, ACSE_CONN *acse_conn)
  {
ST_RET dec_ret;		/* decoder return value	*/
AARQ_APDU aarq_apdu;	/* AARQ APDU decoded	*/
ST_INT j;

  /* Decode the AARQ APDU & store data in the "aarq_apdu" structure.	*/
  dec_ret = acse_dec_aarq (acse_conn->apdu_ptr,acse_conn->apdu_len, acse_conn->pcimms, &aarq_apdu);
  if (dec_ret == SD_SUCCESS)
    {
    /* Handle AARQ.		*/
    acse_conn->state = STATE_AASCRSP_WAIT;

    /* Copy "calling" (remote) address to aarq_apdu.			*/
    if ((aarq_apdu.calling_paddr.psel_len = acse_conn->rem_psel[0])!=0)
      memcpy (aarq_apdu.calling_paddr.psel, &acse_conn->rem_psel[1],
              acse_conn->rem_psel[0]);
    if ((aarq_apdu.calling_paddr.ssel_len = acse_conn->rem_ssel[0])!=0)
      memcpy (aarq_apdu.calling_paddr.ssel, &acse_conn->rem_ssel[1],
              acse_conn->rem_ssel[0]);
    if ((aarq_apdu.calling_paddr.tsel_len = acse_conn->rem_tsel[0])!=0)
      memcpy (aarq_apdu.calling_paddr.tsel, &acse_conn->rem_tsel[1],
              acse_conn->rem_tsel[0]);

    if (acse_conn->tp4_conn_id >= MIN_TP0_CONN_ID)	/* This is TP0 conn. Save IP addr*/
      {
      aarq_apdu.calling_paddr.tp_type = TP_TYPE_TCP;
      aarq_apdu.calling_paddr.netAddr.ip = acse_conn->remNetAddr.ip;
      }
    else				/* This is TP4 conn. Save NSAP	*/
      {
      aarq_apdu.calling_paddr.tp_type = TP_TYPE_TP4;
      if ((aarq_apdu.calling_paddr.nsap_len = acse_conn->remNetAddr.nsap[0])!=0)
        memcpy (aarq_apdu.calling_paddr.netAddr.nsap, &acse_conn->remNetAddr.nsap[1],
                acse_conn->remNetAddr.nsap[0]);
      }

    /* Copy "called" (local) address (except NSAP) to aarq_apdu.	*/
    /* NOTE: "called" tp_type must be same as "calling".	*/
    aarq_apdu.called_paddr.tp_type = aarq_apdu.calling_paddr.tp_type;
    if ((aarq_apdu.called_paddr.psel_len = acse_conn->loc_psel[0])!=0)
      memcpy (aarq_apdu.called_paddr.psel, &acse_conn->loc_psel[1],
              acse_conn->loc_psel[0]);
    if ((aarq_apdu.called_paddr.ssel_len = acse_conn->loc_ssel[0])!=0)
      memcpy (aarq_apdu.called_paddr.ssel, &acse_conn->loc_ssel[1],
              acse_conn->loc_ssel[0]);
    if ((aarq_apdu.called_paddr.tsel_len = acse_conn->loc_tsel[0])!=0)
      memcpy (aarq_apdu.called_paddr.tsel, &acse_conn->loc_tsel[1],
              acse_conn->loc_tsel[0]);

    ACSELOG_DEC1 ("A-ASSOCIATE.ind:  acse_conn_id =" S_FMT_PTR "\n"
                 "  DST (CALLED) ADDRESS:", acse_conn);
    if (aarq_apdu.called_ae_title.AP_title_pres)
      {
      for (j = 0; j < aarq_apdu.called_ae_title.AP_title.num_comps; ++j)
        {			/* Verbose but gets the job done.	*/
        ACSELOG_DEC2C ("  AP_title.comps[%d] = %d",
          j, aarq_apdu.called_ae_title.AP_title.comps[j]);
        }
      }
    if (aarq_apdu.called_ae_title.AE_qual_pres)
      {
      ACSELOG_DEC1C ("  AE_qual = %ld", (ST_LONG) aarq_apdu.called_ae_title.AE_qual);
      }
    ACSELOG_DEC0C ("  PSEL:");
    ACSELOG_DECH  (aarq_apdu.called_paddr.psel_len, aarq_apdu.called_paddr.psel);
    ACSELOG_DEC0C ("  SSEL:");
    ACSELOG_DECH  (aarq_apdu.called_paddr.ssel_len, aarq_apdu.called_paddr.ssel);
    ACSELOG_DEC0C ("  TSEL:");
    ACSELOG_DECH  (aarq_apdu.called_paddr.tsel_len, aarq_apdu.called_paddr.tsel);

    ACSELOG_DEC0C ("SRC (CALLING) ADDRESS:");
    if (aarq_apdu.calling_ae_title.AP_title_pres)
      {
      for (j = 0; j < aarq_apdu.calling_ae_title.AP_title.num_comps; ++j)
        {			/* Verbose but gets the job done.	*/
        ACSELOG_DEC2C ("  AP_title.comps[%d] = %d",
          j, aarq_apdu.calling_ae_title.AP_title.comps[j]);
        }
      }
    if (aarq_apdu.calling_ae_title.AE_qual_pres)
      {
      ACSELOG_DEC1C ("  AE_qual = %ld", (ST_LONG) aarq_apdu.calling_ae_title.AE_qual);
      }
    ACSELOG_DEC0C ("  PSEL:");
    ACSELOG_DECH  (aarq_apdu.calling_paddr.psel_len, aarq_apdu.calling_paddr.psel);
    ACSELOG_DEC0C ("  SSEL:");
    ACSELOG_DECH  (aarq_apdu.calling_paddr.ssel_len, aarq_apdu.calling_paddr.ssel);
    ACSELOG_DEC0C ("  TSEL:");
    ACSELOG_DECH  (aarq_apdu.calling_paddr.tsel_len, aarq_apdu.calling_paddr.tsel);
    if (aarq_apdu.calling_paddr.tp_type == TP_TYPE_TCP)
      {
#if defined(TP0_ENABLED)
      struct in_addr sin_addr;	/* inet_ntoa needs this addr format*/
      sin_addr.s_addr = aarq_apdu.calling_paddr.netAddr.ip;
      ACSELOG_DEC1C ("  IP ADDR:  %s", inet_ntoa (sin_addr));
#else
      ACSELOG_ERR0 ("Received TCP conn ind. TCP not supported");
#endif
      }
    else
      {
      ACSELOG_DEC0C ("  NSAP:");
      ACSELOG_DECH  (aarq_apdu.calling_paddr.nsap_len, aarq_apdu.calling_paddr.netAddr.nsap);
      }

    /* save partner's security info to the ACSE_CONN for user's anytime examination */
    acse_conn->part_auth_info = aarq_apdu.auth_info;
    if (aarq_apdu.auth_info.mech_type == ACSE_AUTH_MECH_OTHER)
      {
      /* allocate buffer for the Authentication Value */
      acse_conn->part_auth_info.u.other_auth.auth_value.buf = acse_conn->part_auth_info.u.other_auth.auth_value.ptr =
           (ST_UCHAR *) chk_calloc (aarq_apdu.auth_info.u.other_auth.auth_value.len, sizeof(ST_CHAR));
      memcpy (acse_conn->part_auth_info.u.other_auth.auth_value.ptr, aarq_apdu.auth_info.u.other_auth.auth_value.ptr,
              aarq_apdu.auth_info.u.other_auth.auth_value.len);
      }
    aarq_apdu.encrypt_ctrl = acse_conn->encrypt_ctrl;

    u_a_associate_ind (user_bind_id, acse_conn, &aarq_apdu);
    }
  else
    {
    /* Handle ASN1 decode error.  					*/
    /* According to the ACSE spec section 7.3.3.4 (pg 20) we should 	*/
    /* be sending an Abort PDU upon protocol error.			*/
    ACSELOG_ERR0 ("ACSE-ERROR: AARQ decode failed");
    copp_u_abort_req (acse_conn);
    acse_conn->state = STATE_IDLE;
    }
  }

/************************************************************************/
/* u_copp_rel_ind							*/
/* P-Release Indication.						*/
/* Must be RLRQ APDU.  Decode it and call u_a_release_ind.		*/
/************************************************************************/
ST_VOID u_copp_rel_ind (ACSE_CONN *acse_conn)
  {
ST_RET dec_ret;		/* decoder return value	*/
RLRQ_APDU rlrq_apdu;	/* RLRQ APDU decoded	*/

  /* Decode the RLRQ APDU & store data in the "rlrq_apdu" structure.	*/
  dec_ret = acse_dec_rlrq (acse_conn->apdu_ptr,acse_conn->apdu_len, acse_conn->pcimms, &rlrq_apdu);
  if (dec_ret == SD_SUCCESS)
    {
    /* Handle RLRQ.		*/
    switch (acse_conn->state)
      {
      case STATE_ASSOCIATED:
      acse_conn->state = STATE_ARLSRSP_WAIT;
      ACSELOG_DEC2 ("A-RELEASE.ind:    acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
        acse_conn, acse_conn->user_conn_id);
      u_a_release_ind (acse_conn->user_conn_id, &rlrq_apdu);
      break;

      case STATE_RLRE_WAIT:
      if (acse_conn->initiator)
        acse_conn->state = STATE_INIT_COLL;
      else
        acse_conn->state = STATE_RESP_COLL;
      ACSELOG_DEC2 ("A-RELEASE.ind:    acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
        acse_conn, acse_conn->user_conn_id);
      u_a_release_ind (acse_conn->user_conn_id, &rlrq_apdu);
      break;

      default:
      u_a_except (acse_conn->user_conn_id, EX_ACSE_INVALID_STATE, 
              thisFileName, __LINE__);
      break;
      }	/* end "switch"	*/
    }
  else
    {
    /* Handle ASN1 decode error.	*/
    ACSELOG_ERR0 ("ACSE-ERROR: RLRQ decode failed");
    }
  }

/************************************************************************/
/* u_copp_p_abort_ind							*/
/* P-P-Abort Indication.						*/
/* ACSE doesn't need to decode anything.  Just pass up A-P-ABORT.ind	*/
/************************************************************************/
ST_VOID u_copp_p_abort_ind (ACSE_CONN *acse_conn, ST_INT reason)
  {
ABRT_APDU abrt_apdu;	/* Abort APDU to pass to u_a_abort_ind	*/
  acse_conn->state = STATE_IDLE;
  ACSELOG_DEC3 ("A-P-ABORT.ind:    acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                "  Provider-reason = %d", acse_conn, acse_conn->user_conn_id, reason);
  /* Pass up A-P-ABORT.ind	*/
  abrt_apdu.source = ACSE_SERVICE_PROVIDER;
  abrt_apdu.user_info.len = 0;
  u_a_abort_ind (acse_conn->user_conn_id, &abrt_apdu);
  }

/************************************************************************/
/* u_copp_u_abort_ind							*/
/* P-U-Abort Indication.						*/
/* Must be ABRT APDU.  Decode it and call u_a_abort_ind.		*/
/************************************************************************/
ST_VOID u_copp_u_abort_ind (ACSE_CONN *acse_conn)
  {
ST_RET dec_ret;		/* decoder return value	*/
ABRT_APDU abrt_apdu;	/* Abort APDU decoded	*/

  /* Decode the ABRT APDU & store data in the "abrt_apdu" structure.	*/
  dec_ret = acse_dec_abrt (acse_conn->apdu_ptr,acse_conn->apdu_len, acse_conn->pcimms, &abrt_apdu);
  if (dec_ret != SD_SUCCESS)
    {
    ACSELOG_ERR1 ("ACSE-ERROR: ABRT decode failed (code = %d). Call u_a_abort_ind anyway.",
                  dec_ret);
    /* Fill "abrt_apdu" with defaults (possibly corrupted during decode).*/
    abrt_apdu.source = ACSE_SERVICE_USER;
    abrt_apdu.user_info.len = 0;
    }

  acse_conn->state = STATE_IDLE;
  ACSELOG_DEC2 ("A-ABORT.ind:      acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
    acse_conn, acse_conn->user_conn_id);
  u_a_abort_ind (acse_conn->user_conn_id, &abrt_apdu);
  }

/************************************************************************/
/* u_copp_data_ind							*/
/* P-Data Indication.							*/
/* ACSE doesn't need to decode anything.  Just call u_a_data_ind.	*/
/************************************************************************/
ST_VOID u_copp_data_ind (ACSE_CONN *acse_conn)
  {
  u_a_data_ind (acse_conn->user_conn_id, acse_conn->apdu_len, acse_conn->apdu_ptr);
  }

/************************************************************************/
/* tp4_except								*/
/************************************************************************/
ST_VOID tp4_except (
	ST_CHAR *filename,		/* Source code file name		*/
	ST_INT line)		/* source code line number		*/
  {
  /* DEBUG: need better way to deal with all exceptions.		*/
  /* How about one exception function, pass component name and errcode	*/
  ACSELOG_ERR2 ("ERROR: TP4 exception:  File = %s, Line = %d", filename, line);
  /* DEBUG: create new errcode, or SKIP IT and use only filename/line??	*/
  /* NOTE: first arg is user_conn_id. Don't know it, so pass NULL.	*/
  u_a_except (NULL, EX_ACSE_DECODE, filename, line);
  }

#if defined (TP4_ENABLED)
/************************************************************************/
/*			a_get_rem_nsap					*/
/* RETURN: pointer to NSAP. First byte is length.			*/
/************************************************************************/
#include "tp4.h"			/* Need internal TP4 defines	*/
ST_UCHAR *a_get_rem_nsap (ST_VOID *acse_conn_id)
  {
ACSE_CONN *acse_conn;
ST_LONG tp_conn_id;
TP_CONN *tp_conn;

  acse_conn = (ACSE_CONN *) acse_conn_id;
  tp_conn_id = acse_conn->tp4_conn_id;
  if (tp_conn_id < 0  ||  tp_conn_id >= tp_cfg.max_num_conns)
    return (NULL);	/* Invalid conn_id (could be TP0 conn id	*/
			/* which starts at MIN_TP0_CONN_ID).		*/
  tp_conn = &tp_conn_arr [tp_conn_id];
  return (tp_conn->rem_nsap);
  }
#endif	/* defined (TP4_ENABLED)	*/

#if defined (TP0_ENABLED)
/************************************************************************/
/*			a_get_rem_ip_addr				*/
/* This function returns the remote IP Address as an unsigned long in	*/
/* "network byte order", just like the "standard sockets" function	*/
/* "inet_addr" does. If there is an error, it returns			*/
/* "htonl (INADDR_NONE)", just like "inet_addr" does.			*/
/*   The return value can be stored in the appropriate union member	*/
/* in the structure "in_addr", which can be passed to "inet_ntoa".	*/
/************************************************************************/
#include "tp4.h"			/* Need internal TP4 defines	*/
#include "tp0_sock.h"			/* Need "sockets" defines	*/
ST_ULONG a_get_rem_ip_addr (ST_VOID *acse_conn_id)
  {
ACSE_CONN *acse_conn;
ST_LONG tp_conn_id;
TP0_CONN *tp0_conn;
ST_INT ret;
SOCKADDR_IN sockaddr_in;
SOCK_ADDRLEN addr_len;

  acse_conn = (ACSE_CONN *) acse_conn_id;
  tp_conn_id = acse_conn->tp4_conn_id;
  if (tp_conn_id >= MIN_TP0_CONN_ID  &&
      tp_conn_id < tp0_cfg.max_num_conns + MIN_TP0_CONN_ID)
    {				/* Conn id is a legal TP0 conn id	*/
    tp0_conn = &tp0_conn_arr [tp_conn_id - MIN_TP0_CONN_ID];

    addr_len = sizeof (SOCKADDR_IN);	/* CRITICAL: set to expected len.*/
    ret = getpeername (tp0_conn->sock_info->hSock, (SOCKADDR *) &sockaddr_in, &addr_len); 
    if (ret == 0)
      return (sockaddr_in.sin_addr.s_addr);
    }
   
  return (htonl (INADDR_NONE));		/* something failed	*/
  }
#endif	/* defined (TP0_ENABLED)	*/

