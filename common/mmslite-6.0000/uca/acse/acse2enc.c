/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995-2010, All Rights Reserved					*/
/*									*/
/* MODULE NAME : acse2enc.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	ACSE Encode Functions.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 09/04/10  EJV	   a_associate_req: save local auth_info.	*/
/*			   a_get_security_info: set local auth_info.	*/
/* 06/22/10  JRB	   Del a_get_event_handles* (do from user code).*/
/* 11/17/09  JRB    38     a_associate_req: don't pass bind_id and	*/
/*			   use aarq_apdu->calling_paddr instead;	*/
/*			   set PCI's before encoding, even if		*/
/*			   (user_info.len == 0).			*/
/* 11/01/09  EJV    37     Merge these MMS-EASE changes:		*/
/*			   Use S_FMT_PTR macro to log pointers.		*/
/*			   Chg xxx_bind_id from ST_LONG to ST_VOID*	*/
/*                           to support all 64-bit systems.		*/
/*			   Made MOSI the default stack code.		*/
/*			   Del REDUCED_STACK and TRIM7 code.		*/
/* 03/13/09  JRB    36     Fix return value on a_get_event_handles.	*/
/* 02/27/08  EJV    35     a_associate_rsp: do not free acse_conn.	*/
/* 11/07/06  EJV    34     MMSEASE_MOSI:elim HPUX warn when logging PSEL*/
/* 07/06/05  EJV    33     a_associate_req: log "IP ADDR: ip" same line,*/
/*			     MMSEASE_MOSI: log local PSEL,SSEL,TSEL.	*/
/* 05/06/05  EJV    32     Log PORT if configured.			*/
/* 01/24/05  JRB    31     Move acse_debug_sel & "*_logstr" to lean_var.c*/
/* 09/17/04  JRB    30     a_abort_req: don't access acse_conn after	*/
/*			   copp_u_abort_req (it may have been freed).	*/
/* 08/06/04  EJV    29     Added typecast (ST_UCHAR *) ...password.	*/
/* 03/04/04  JRB    28     Mov thisFil.. after incs, they may define DEBUG_..*/
/*			   Del logging of A-DATA.req (use MVL or COPP)	*/
/* 12/18/03  JRB    27	   If MOSI, use cosp_buf, DON't use acse_buf.	*/
/*			   Use ASN1_ENC_PTR, ASN1_ENC_LEN macros.	*/
/* 10/21/03  JRB    26     Add a_get_event_handles_unix.		*/
/* 08/20/03  EJV    25	   Chg cosp_msgsize to fit larger User Data	*/
/*  			   ACSE_AUTH_ENABLED code wo/define.		*/
/*			   Added security functions.			*/
/*			   Added param to tp4_connect .			*/
/*			   Fix the ACSE Auth encode.			*/
/* 06/17/03  EJV    24	   Added _acse_dib_logstr.			*/
/* 10/22/02  ASK    23	   Wrap ifdef TP0_ENABLED around inet_ntoa calls*/
/* 10/14/02  ASK    22     Added encoding for abort diagnostic.		*/
/* 08/26/02  ASK    21	   Added password mechanism encoding in		*/ 
/*			   a_associate_req and a_associate rsp.		*/
/* 07/16/02  JRB    20     Add tp0_sock.h for sockets/IP defs.		*/
/* 06/26/02  JRB    19     associate_req: Log calling/called address.	*/
/* 01/22/02  JRB    18     Add args to copp_initialize (same as MAP30..)*/
/*			   Del a_set_msgsize (no longer needed).	*/
/* 01/02/02  JRB    17     Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/22/01  JRB    16     Fix a_release_rsp handling of STATE_RESP_COLL*/
/*			   state per ISO 8650 (was kluged before).	*/
/* 03/15/01  JRB    15     Use new SMEM allocation functions.		*/
/* 01/04/01  EJV    14     a_get_event_handles used only on _WIN32	*/
/* 09/24/99  JRB    13     a_get_event_handles not used on		*/
/*                         DOS, QNX or VXWORKS				*/
/* 09/13/99  MDE    12     Added SD_CONST modifiers			*/
/* 08/05/99  JRB    11     Add "a_get_event_handles" function for MOSI.	*/
/* 10/08/98  MDE    10     Migrated to updated SLOG interface		*/
/* 08/13/98  JRB    09     Lint cleanup.				*/
/* 07/10/98  JRB    08     Del *_obj_id, use acse2var.c			*/
/* 03/23/98  NAV    07     Free buffers on termination			*/
/* 02/16/98  JRB    06     Del or "#if out" unused locals.		*/
/* 11/10/97  JRB    05     Chg cosp_buf to (ST_UCHAR *).		*/
/* 09/22/97  JRB    04     acse_msgsize MUST be set before copp_init.	*/
/*			   a_data_req (MOSI): check for buffer overflow	*/
/* 09/04/97  JRB    03     Pass SPDU msgsize arg to tp4_initialize.	*/ 
/* 07/31/97  JRB    02     copp_bind & a_associate_req set "tp_type"	*/
/*			   in tp4_addr. New TP4 needs it.		*/
/* 06/09/97  JRB    01     If !MOSI, on A-ASSOCIATE.rsp-, just set	*/
/*			   state=STATE_IDLE. Don't free as did on 3/7/97*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/02/97  JRB    24     wr_* functs now ret void, so don't check.	*/
/* 04/09/97  JRB    23     a_associate_req return (0L) for any error.	*/
/* 03/19/97  JRB    22     Add MOSI "CALLING" support.			*/
/* 03/19/97  JRB    21     Encode AARQ in a_associate_req: need for MOSI*/
/*			   In a_release_req, chg handling if send fails.*/
/*			   Use acse_free_conn to free conn ctrl struct.	*/
/*			   Improve logging.				*/
/*			   Improve release collision handling.		*/
/* 03/07/97  JRB    20     Always send result-source-diag on AARE.	*/
/*			   Rechecked spec, and this is MANDATORY.	*/
/* 03/07/97  JRB    19     If !MOSI, free acse_conn on negative		*/
/*			   A-Associate.rsp (MOSI frees in COSP).	*/
/* 03/07/97  JRB    18     Use new "acse_free_con" to free acse_conn.	*/
/* 03/07/97  JRB    17     Added MOSI support.				*/
/* 02/11/97  JRB    16     In a_release_rsp, if send fails, don't free	*/
/*			   acse_conn and don't change state.		*/
/* 01/23/97  JRB    15     Init acse_debug_sel in copp_initialize.	*/
/* 11/18/96  JRB    14     Del acse_dummyfun call. Funct is gone now.	*/
/* 11/15/96  JRB    13     Don't initialize globals.			*/
/*			   Move acse_debug_sel global here.		*/
/* 08/19/96  MDE    12     Add dummy a_buffers_avail for Reduced Stack	*/
/* 08/14/96  JRB    11     Add a_buffers_avail function.		*/
/* 07/19/96  JRB    10     Change DEBUG_MMS to DEBUG_SISCO.		*/
/* 07/15/96  JRB    09     Only free acse_conn->aarq_apdu.user_info.ptr	*/
/*			   if len != 0 (wasn't alloc'd otherwise).	*/
/* 05/02/96  JRB    08     Add STATE_TP4CONN_WAIT.			*/
/* 04/23/96  MDE    07     Fixed minor warning              		*/
/* 04/18/96  JRB    06     a_associate_req return acse_conn_id or NULL	*/
/*			   on error, instead of just SD_SUCCESS or error.	*/
/*			   if (acse_conn->state == STATE_AARE_WAIT)	*/
/*			   in a_abort_req, set to STATE_AARE_ABANDON.	*/
/* 03/26/96  MDE    05     Added REDUCED_STACK support			*/
/* 03/11/96  JRB    04     Move acse_msgsize here & use for acse_buf.	*/
/* 03/07/96  JRB    03     Handle aarq_apdu->user_info.len = 0.		*/
/* 01/19/96  JRB    02     Cast constant chars to ST_UCHAR.		*/
/* 10/30/95  JRB    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "asn1defs.h"
#include "acse2.h"
#include "cosp_usr.h"
#include "copp_usr.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

ST_UINT acse_msgsize;


/* Buffer for building init, release, and abort reqests.	*/
ST_UINT cosp_msgsize;
ST_UCHAR *cosp_buf;	/* Buffer containing "acse_buf" + COSP&COPP headers.*/
static ST_VOID acse_encode_user_info (ASN1_ENC_CTXT *aCtx, ACSE_CONN *acse_conn, BUFFER *user_info);

#if !defined(SNAP_LITE)		/* SNAP_LITE code in other files	*/
/************************************************************************/
/* copp_initialize							*/
/* Save "msg_size". Pass "max_conns" to tp4_initialize.			*/
/************************************************************************/
ST_RET copp_initialize (ST_INT max_conns, ST_UINT msg_size)
  {
ST_RET ret;

  acse_msgsize = msg_size;	/* Save msg size in global.		*/

  /* Always enable error logging.	*/
#if defined(DEBUG_SISCO)
  acse_debug_sel |= (ACSE_LOG_ERR | COPP_LOG_ERR | COSP_LOG_ERR);
#endif

  if (acse_msgsize == 0)
    {			/* ACSE user must call a_set_msgsize first.	*/
    ACSELOG_ERR0 ("ACSE-ERROR: msg size not set");
    return (E_ACSE_INVALID_PARAM);	/* Invalid Parameter		*/
    }
  cosp_msgsize = max(acse_msgsize, COSP_MAX_UDATA_CON) + MAX_COSP_COPP_HEADER;
  cosp_buf = (ST_UCHAR *) M_MALLOC (MSMEM_STARTUP, cosp_msgsize);

  ret = tp4_initialize (max_conns, cosp_msgsize);
  return (ret);
  }

/************************************************************************/
/* copp_terminate							*/
/************************************************************************/
ST_RET copp_terminate (ST_VOID)
  {

  if (cosp_buf)
    M_FREE (MSMEM_STARTUP, cosp_buf);

  return (tp4_terminate ());
  }
#endif	/* !defined(SNAP_LITE)	*/

/************************************************************************/
/* copp_event								*/
/************************************************************************/
ST_BOOLEAN copp_event (ST_VOID)
  {
  return (tp4_event ());
  }

/************************************************************************/
/* a_associate_req							*/
/************************************************************************/
ST_VOID *a_associate_req (
	ST_VOID *user_conn_id,	/* USER's connection ID			*/
	AARQ_APDU *aarq_apdu)	/* AARQ_APDU info			*/
  {
ACSE_CONN *acse_conn;
AE_TITLE *ae_title;
ST_UCHAR version1_bit_str = BIT_VERSION1;	/* Bit 0 of bitstr (high bit) set	*/
ST_UCHAR auth_bit_str = BIT_AUTHENTICATION;		

ST_UCHAR *msg_ptr;
ST_UINT msg_len;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;
ST_INT j;

  /* Allocate connection tracking structure "acse_conn".		*/
  acse_conn = (ACSE_CONN *) M_CALLOC (MSMEM_ACSE_CONN, 1, sizeof (ACSE_CONN));

  /* Save USER's connection ID. Use it in calls to user functions.	*/
  acse_conn->user_conn_id = user_conn_id;

  /* save the encryption info */
  acse_conn->encrypt_ctrl = aarq_apdu->encrypt_ctrl;

  ACSELOG_ENC2 ("A-ASSOCIATE.req:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
    acse_conn, acse_conn->user_conn_id);
  ACSELOG_ENC0C ("DST (CALLED) ADDRESS:");
  if (aarq_apdu->called_ae_title.AP_title_pres)
    {
    for (j = 0; j < aarq_apdu->called_ae_title.AP_title.num_comps; ++j)
      {			/* Verbose but gets the job done.	*/
      ACSELOG_ENC2C ("  AP_title.comps[%d] = %d",
        j, aarq_apdu->called_ae_title.AP_title.comps[j]);
      }
    }
  if (aarq_apdu->called_ae_title.AE_qual_pres)
    {
    ACSELOG_ENC1C ("  AE_qual = %ld", (ST_LONG) aarq_apdu->called_ae_title.AE_qual);
    }
  ACSELOG_ENC0C ("  PSEL:");
  ACSELOG_ENCH  (aarq_apdu->called_paddr.psel_len, aarq_apdu->called_paddr.psel);
  ACSELOG_ENC0C ("  SSEL:");
  ACSELOG_ENCH  (aarq_apdu->called_paddr.ssel_len, aarq_apdu->called_paddr.ssel);
  ACSELOG_ENC0C ("  TSEL:");
  ACSELOG_ENCH  (aarq_apdu->called_paddr.tsel_len, aarq_apdu->called_paddr.tsel);
  if (aarq_apdu->called_paddr.tp_type == TP_TYPE_TCP)
    {
    struct in_addr sin_addr;	/* inet_ntoa needs this addr format*/
    sin_addr.s_addr = aarq_apdu->called_paddr.netAddr.ip;
#if defined(TP0_ENABLED)
    ACSELOG_ENC1C ("  IP ADDR:  %s", inet_ntoa (sin_addr));
    if (aarq_apdu->encrypt_ctrl.encryptMode == S_SEC_ENCRYPT_SSL)
      {
      if (aarq_apdu->encrypt_ctrl.u.ssl.port == 0)
        ACSELOG_ENC0C ("  Encryption: SSL, port= using default SSL Port");
      else	
        ACSELOG_ENC1C ("  Encryption: SSL, port=%u", (ST_UINT) aarq_apdu->encrypt_ctrl.u.ssl.port);
      }
    else
      {
      if (aarq_apdu->called_paddr.port == 0)
        ACSELOG_ENC0C ("  PORT:  using default RFC1006 Port (102)");
      else
        ACSELOG_ENC1C ("  PORT:  %u", (ST_UINT) aarq_apdu->called_paddr.port);
      ACSELOG_ENC0C ("  Encryption: None");
      }
#endif
    if (aarq_apdu->auth_info.auth_pres)
      {
      if (aarq_apdu->auth_info.mech_type == ACSE_AUTH_MECH_MACE_CERT)
        ACSELOG_ENC0C ("  Authentication: MACE");
      else if (aarq_apdu->auth_info.mech_type == ACSE_AUTH_MECH_PASSWORD)
        ACSELOG_ENC0C ("  Authentication: password");
      else
        ACSELOG_ENC0C ("  Authentication: other");
      }
    else
      ACSELOG_ENC0C ("  Authentication: None");
    }
  else
    {
    ACSELOG_ENC0C ("  NSAP:");
    ACSELOG_ENCH  (aarq_apdu->called_paddr.nsap_len, aarq_apdu->called_paddr.netAddr.nsap);
    }

  ACSELOG_ENC0C ("SRC (CALLING) ADDRESS:");
  if (aarq_apdu->calling_ae_title.AP_title_pres)
    {
    for (j = 0; j < aarq_apdu->calling_ae_title.AP_title.num_comps; ++j)
      {			/* Verbose but gets the job done.	*/
      ACSELOG_ENC2C ("  AP_title.comps[%d] = %d",
        j, aarq_apdu->calling_ae_title.AP_title.comps[j]);
      }
    }
  if (aarq_apdu->calling_ae_title.AE_qual_pres)
    {
    ACSELOG_ENC1C ("  AE_qual = %ld", (ST_LONG) aarq_apdu->calling_ae_title.AE_qual);
    }
  ACSELOG_ENC0C ("  PSEL:");
  ACSELOG_ENCH  (aarq_apdu->calling_paddr.psel_len, aarq_apdu->calling_paddr.psel);
  ACSELOG_ENC0C ("  SSEL:");
  ACSELOG_ENCH  (aarq_apdu->calling_paddr.ssel_len, aarq_apdu->calling_paddr.ssel);
  ACSELOG_ENC0C ("  TSEL:");
  ACSELOG_ENCH  (aarq_apdu->calling_paddr.tsel_len, aarq_apdu->calling_paddr.tsel);

  /* Begin encoding request.						*/
  /* PCI's must be set before any encoding, even if user_info.len == 0	*/
  /* (i.e. caller did not encode any "user_info").			*/
  acse_conn->pciacse = 1;
  acse_conn->pcimms = 3;
  asn1r_strt_asn1_bld (aCtx, cosp_buf,cosp_msgsize);

  asn1r_strt_constr (aCtx);			/* Start AARQ APDU constructor	*/
  /* Write user_info.							*/
  if (aarq_apdu->user_info.len)
    {
    asn1r_strt_constr (aCtx);
    acse_encode_user_info (aCtx, acse_conn, &aarq_apdu->user_info);
    asn1r_fin_constr (aCtx, AARQ_user_info,CTX,DEF);
    }
  
  if(aarq_apdu->auth_info.auth_pres == SD_TRUE)
    {
    if(aarq_apdu->auth_info.mech_type == ACSE_AUTH_MECH_PASSWORD)
      {
      ACSELOG_ENC0C ("  Authentication: password");
      /* Write the password value */
      asn1r_strt_constr (aCtx);	  
      asn1r_wr_octstr (aCtx, (ST_UCHAR *) aarq_apdu->auth_info.u.pw_auth.password, 
      		     (int) strlen(aarq_apdu->auth_info.u.pw_auth.password)); /* Strip off the NULL */
      asn1r_fin_prim (aCtx, 0,CTX); 
      asn1r_fin_constr (aCtx, AARQ_CALLING_auth_value,CTX,DEF);
  
      /* Write the mechanism name (password in this case) */
      asn1r_wr_objid (aCtx, a_auth_password_obj_id.comps, a_auth_password_obj_id.num_comps);
      asn1r_fin_prim (aCtx, AARQ_mechanism_name, CTX);
      }
    else
      {
      if (aarq_apdu->auth_info.mech_type == ACSE_AUTH_MECH_MACE_CERT)
        ACSELOG_ENC0C ("Authentication: MACE");
      else
        ACSELOG_ENC0C ("Authentication: other");
      asn1r_strt_constr (aCtx);	  
      asn1r_strt_constr (aCtx);				/* Authentication-value	*/
      asn1r_strt_constr (aCtx);				/* Single ASN.1 Type 	*/
      asn1r_wr_octstr (aCtx, aarq_apdu->auth_info.u.other_auth.auth_value.ptr, 
      		     	     aarq_apdu->auth_info.u.other_auth.auth_value.len);
      asn1r_fin_constr (aCtx, 0, CTX, SD_FALSE); 	/* Single ASN.1 Type 	*/
      asn1r_fin_constr (aCtx, 2, CTX, SD_FALSE); 	/* Authentication-value	*/
      asn1r_fin_constr (aCtx, AARQ_CALLING_auth_value,CTX,DEF);
  
      /* Write the mechanism name */
      asn1r_wr_objid (aCtx, aarq_apdu->auth_info.u.other_auth.mech_id.comps, aarq_apdu->auth_info.u.other_auth.mech_id.num_comps);
      asn1r_fin_prim (aCtx, AARQ_mechanism_name, CTX);
      }
  
    /* Let's encode the authentication present bit to enable security */
    asn1r_wr_bitstr (aCtx, &auth_bit_str, 1);
    asn1r_fin_prim (aCtx, AARQ_acse_requirements, CTX);
    } 

  /* Write CALLING AE Title (i.e. AP_title, AE_qual, AP_inv_id, AE_inv_id).*/
  ae_title = &aarq_apdu->calling_ae_title;

  if (ae_title->AE_inv_id_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AE_inv_id);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_CALLING_AE_inv_id,CTX,DEF);
    }
    
  if (ae_title->AP_inv_id_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AP_inv_id);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_CALLING_AP_inv_id,CTX,DEF);
    }
    
  if (ae_title->AE_qual_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AE_qual);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_CALLING_AE_qual,CTX,DEF);
    }
    
  if (ae_title->AP_title_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_objid (aCtx, ae_title->AP_title.comps, ae_title->AP_title.num_comps);
    asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_CALLING_AP_title,CTX,DEF);
    }


  /* Write CALLED AE Title (i.e. AP_title, AE_qual, AP_inv_id, AE_inv_id).*/
  ae_title = &aarq_apdu->called_ae_title;

  if (ae_title->AE_inv_id_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AE_inv_id);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_CALLED_AE_inv_id,CTX,DEF);
    }
    
  if (ae_title->AP_inv_id_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AP_inv_id);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_CALLED_AP_inv_id,CTX,DEF);
    }
    
  if (ae_title->AE_qual_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AE_qual);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_CALLED_AE_qual,CTX,DEF);
    }
    
  if (ae_title->AP_title_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_objid (aCtx, ae_title->AP_title.comps, ae_title->AP_title.num_comps);
    asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_CALLED_AP_title,CTX,DEF);
    }

  /* Write ASO Context Name.						*/
  if (aarq_apdu->ASO_context_name_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_objid (aCtx, aarq_apdu->ASO_context_name.comps, aarq_apdu->ASO_context_name.num_comps);
    asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
    asn1r_fin_constr (aCtx, AARQ_ASO_context_name,CTX,DEF);
    }

  /* Write protocol_version.						*/
  asn1r_wr_bitstr (aCtx, &version1_bit_str, 1);
  asn1r_fin_prim (aCtx, AARQ_protocol_version, CTX);

  asn1r_fin_constr (aCtx, TAG_AARQ,APP,DEF);	/* Finish AARQ APDU	*/

  msg_ptr = ASN1_ENC_PTR (aCtx);	/* point to beginning of PDU.	*/
  msg_len = (unsigned int) ASN1_ENC_LEN (aCtx);

  if (msg_len > acse_msgsize)
    {
    ACSELOG_ERR1 ("ACSE-ERROR: acse_msgsize=%u is too small to encode A-ASSOCIATE.req (increase MMS msg size).",
                  acse_msgsize);
    M_FREE (MSMEM_ACSE_CONN, acse_conn);
    return (NULL);
    }

  acse_conn->apdu_ptr = msg_ptr;
  acse_conn->apdu_len = msg_len;
  /* Send AARQ APDU via P-CONNECT.req					*/
  if (copp_con_req (acse_conn, &aarq_apdu->calling_paddr, &aarq_apdu->called_paddr) != SD_SUCCESS)
    {
    acse_free_con (acse_conn);		/* Dump conn tracking struct	*/
    return (NULL);
    }
  acse_conn->state = STATE_AARE_WAIT;	/* Waiting for AARE-apdu	*/

  /* save local auth_info */
  acse_conn->auth_info = aarq_apdu->auth_info; 
  /* copied only if returning acse_conn != NULL, otherwise the calling	*/
  /* function should free data in auth_info				*/

  return (acse_conn);
  }
/************************************************************************/
/* a_associate_rsp							*/
/************************************************************************/
ST_RET a_associate_rsp (
	ST_VOID *acse_conn_id,	/* ACSE's connection ID			*/
	ST_VOID *user_conn_id,	/* USER's connection ID			*/
	AARE_APDU *info)	/* AARE_APDU info			*/
  {
AE_TITLE *ae_title;
ST_UCHAR version1_bit_str = BIT_VERSION1;	/* Bit 0 of bitstr (high bit) set	*/
ST_UCHAR auth_bit_str = BIT_AUTHENTICATION;		

ST_UCHAR *msg_ptr;
ST_UINT msg_len;
ACSE_CONN *acse_conn;
ST_RET retcode;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  ACSELOG_ENC3 ("A-ASSOCIATE.rsp%c:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
    (info->result ? '-' : '+'), acse_conn_id, user_conn_id);

  acse_conn = (ACSE_CONN *) acse_conn_id;

  if (acse_conn == NULL)
    return (E_ACSE_INVALID_CONN_ID);	/* Invalid acse_conn_id		*/

  if (acse_conn->state != STATE_AASCRSP_WAIT)
    return (E_ACSE_INVALID_STATE);	/* Invalid state for this operation*/

  /* Save USER's connection ID. Use it in calls to user functions.	*/
  acse_conn->user_conn_id = user_conn_id;

  /* Everything looks OK. Go ahead and encode response and send it.	*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf,cosp_msgsize);
    
  asn1r_strt_constr (aCtx);			/* Start AARE APDU constructor	*/
  /* Write user_info.							*/
  if (info->user_info.len)
    {
    asn1r_strt_constr (aCtx);
    acse_encode_user_info (aCtx, acse_conn, &info->user_info);
    asn1r_fin_constr (aCtx, AARE_user_info,CTX,DEF);
    }

  if(info->auth_info.auth_pres == SD_TRUE)
    {
    if(info->auth_info.mech_type == ACSE_AUTH_MECH_PASSWORD)
      {
      /* Write the password value */
      asn1r_strt_constr (aCtx);	  
      asn1r_wr_octstr (aCtx, (ST_UCHAR *) info->auth_info.u.pw_auth.password, 
      		     (int) strlen(info->auth_info.u.pw_auth.password)); /* Strip off the NULL */
      asn1r_fin_prim (aCtx, 0,CTX); 
      asn1r_fin_constr (aCtx, AARE_RESPONDING_auth_value,CTX,DEF);
  
      /* Write the mechanism name (password in this case) */
      asn1r_wr_objid (aCtx, a_auth_password_obj_id.comps, a_auth_password_obj_id.num_comps);
      asn1r_fin_prim (aCtx, AARE_mechanism_name, CTX);
      }
    else
      {
      if (info->auth_info.mech_type == ACSE_AUTH_MECH_MACE_CERT)
        ACSELOG_ENC0C ("Authentication: MACE");
      else
        ACSELOG_ENC0C ("Authentication: other");
      asn1r_strt_constr (aCtx);	  
      asn1r_strt_constr (aCtx);				/* Authentication-value	*/
      asn1r_strt_constr (aCtx);				/* Single ASN.1 Type 	*/
      asn1r_wr_octstr (aCtx, info->auth_info.u.other_auth.auth_value.ptr, 
      		     	     info->auth_info.u.other_auth.auth_value.len);
      asn1r_fin_constr (aCtx, 0, CTX, SD_FALSE); 	/* Single ASN.1 Type 	*/
      asn1r_fin_constr (aCtx, 2, CTX, SD_FALSE); 	/* Authentication-value	*/
      asn1r_fin_constr (aCtx, AARE_RESPONDING_auth_value,CTX,DEF);
  
      /* Write the mechanism name */
      asn1r_wr_objid (aCtx, info->auth_info.u.other_auth.mech_id.comps, 
      			    info->auth_info.u.other_auth.mech_id.num_comps);
      asn1r_fin_prim (aCtx, AARE_mechanism_name, CTX);
      }
  
    /* Let's encode the authentication present bit to enable security */
    asn1r_wr_bitstr (aCtx, &auth_bit_str, 1);
    asn1r_fin_prim (aCtx, AARE_RESPONDER_acse_requirements, CTX);
    } 

  /* Write RESP AE Title (i.e. AP_title, AE_qual, AP_inv_id, AE_inv_id)	*/
  /* fields of AARE APDU.						*/
  ae_title = &info->responding_ae_title;

  if (ae_title->AE_inv_id_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AE_inv_id);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARE_RESPONDING_AE_inv_id,CTX,DEF);
    }
    
  if (ae_title->AP_inv_id_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AP_inv_id);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARE_RESPONDING_AP_inv_id,CTX,DEF);
    }
    
  if (ae_title->AE_qual_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_i32 (aCtx, ae_title->AE_qual);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    asn1r_fin_constr (aCtx, AARE_RESPONDING_AE_qual,CTX,DEF);
    }
    
  if (ae_title->AP_title_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_objid (aCtx, ae_title->AP_title.comps, ae_title->AP_title.num_comps);
    asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
    asn1r_fin_constr (aCtx, AARE_RESPONDING_AP_title,CTX,DEF);
    }

  /* Write result-source-diagnostic.					*/
  /* DEBUG: If user required to set "result_source" and "result_diag",	*/
  /* "result_source_diag_pres" would no longer be needed.		*/
  if (info->result_source_diag_pres == 0)
    {		/* User probably didn't set values, so set to default.	*/
    info->result_source = 1;	/* Source = acse-service-user.	*/
    info->result_diag = 0;	/* Diag = null.			*/
    }
  asn1r_strt_constr (aCtx);
  asn1r_strt_constr (aCtx);
  asn1r_wr_i16 (aCtx, info->result_diag);
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  if (info->result_source != 1  &&  info->result_source != 2)
    return (E_ACSE_ENC_ERR);		/* Make sure source legal.	*/
  asn1r_fin_constr (aCtx, info->result_source,CTX,DEF);
  asn1r_fin_constr (aCtx, AARE_result_source_diagnostic,CTX,DEF);

  /* Write result							*/
  asn1r_strt_constr (aCtx);
  asn1r_wr_i16 (aCtx, info->result);
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  asn1r_fin_constr (aCtx, AARE_result,CTX,DEF);

  /* Write ASO Context Name.						*/
  if (info->ASO_context_name_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_objid (aCtx, info->ASO_context_name.comps, info->ASO_context_name.num_comps);
    asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
    asn1r_fin_constr (aCtx, AARE_ASO_context_name,CTX,DEF);
    }

  /* Write protocol_version.						*/
  asn1r_wr_bitstr (aCtx, &version1_bit_str, 1);
  asn1r_fin_prim (aCtx, AARE_protocol_version, CTX);

  asn1r_fin_constr (aCtx, TAG_AARE,APP,DEF);	/* Finish AARE APDU	*/

  msg_ptr = ASN1_ENC_PTR (aCtx);	/* point to beginning of PDU.	*/
  msg_len = (unsigned int) ASN1_ENC_LEN (aCtx);

  if (msg_len > acse_msgsize)
    return (E_ACSE_BUFFER_OVERFLOW);

  acse_conn->apdu_ptr = msg_ptr;
  acse_conn->apdu_len = msg_len;
  if (info->result)
    /* NOTE:  COSP will free "acse_conn" after T-disconnect.	*/
    retcode = copp_con_rsp_neg (acse_conn, -1);	/* -1 means "from user"	*/
  else
    {
    retcode = copp_con_rsp_pos (acse_conn);
    if (retcode == SD_SUCCESS)
      acse_conn->state = STATE_ASSOCIATED;
    }
  if (retcode != SD_SUCCESS)
    {
    #if 0  
    /* EJV 2/27/2008: if copp_con_rsp_* fails then the most probable cause is	*/
    /*                that the socket was disconencted. In this case the	*/
    /*		      the tp0_disconnect_ind will free the acse_conn.		*/
    /*		      If it was other error the application will issue an abort	*/
    /*		      to cleanup the connection.				*/
    acse_free_con (acse_conn);		/* Dump conn tracking struct	*/
    #endif  /* 0 */ 
    ACSELOG_ENC0 ("ACSE-ERROR: failed to send A-ASSOCIATE.rsp (disconnected socket or other error).");
    return (E_ACSE_SEND_ERR);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/* a_release_req							*/
/************************************************************************/
ST_RET a_release_req (
	ST_VOID *acse_conn_id,	/* ACSE's connection ID			*/
	RLRQ_APDU *info)	/* RLRQ_APDU info			*/
  {
ST_UCHAR *msg_ptr;
ST_UINT msg_len;
ACSE_CONN *acse_conn;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  acse_conn = (ACSE_CONN *)acse_conn_id;

  ACSELOG_ENC2 ("A-RELEASE.req:    acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
    acse_conn, acse_conn->user_conn_id);

  if (acse_conn == NULL)
    return (E_ACSE_INVALID_CONN_ID);	/* Invalid acse_conn_id		*/

  if (acse_conn->state != STATE_ASSOCIATED)
    return (E_ACSE_INVALID_STATE);	/* Invalid state for this operation*/

  /* Everything looks OK. Go ahead and encode request and send it.	*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf,cosp_msgsize);
    
  asn1r_strt_constr (aCtx);		/* Start RLRQ APDU constructor		*/
  /* Write user_info.							*/
  if (info->user_info.len)
    {
    asn1r_strt_constr (aCtx);
    acse_encode_user_info (aCtx, acse_conn, &info->user_info);
    asn1r_fin_constr (aCtx, RLRQ_user_info,CTX,DEF);
    }
  
  /* Write Release Reason.						*/
  if (info->reason_pres)
    {
    asn1r_wr_i16 (aCtx, info->reason);
    asn1r_fin_prim (aCtx, RLRQ_reason,CTX);
    }
    
  asn1r_fin_constr (aCtx, TAG_RLRQ,APP,DEF);	/* Finish RLRQ APDU	*/
  msg_ptr = ASN1_ENC_PTR (aCtx);	/* point to beginning of PDU.	*/
  msg_len = (unsigned int) ASN1_ENC_LEN (aCtx);
  if (msg_len > acse_msgsize)
    return (E_ACSE_BUFFER_OVERFLOW);

  acse_conn->apdu_ptr = msg_ptr;
  acse_conn->apdu_len = msg_len;
  /* Send RLRQ APDU via P-RELEASE.req					*/
  if (copp_rel_req (acse_conn) != SD_SUCCESS)
    return (E_ACSE_SEND_ERR);
  acse_conn->state = STATE_RLRE_WAIT;
  
  return (SD_SUCCESS);
  }

/************************************************************************/
/* a_release_rsp							*/
/************************************************************************/
ST_RET a_release_rsp (
	ST_VOID *acse_conn_id,	/* ACSE's connection ID			*/
	RLRE_APDU *info)	/* RLRE_APDU info			*/
  {
ST_UCHAR *msg_ptr;
ST_UINT msg_len;
ACSE_CONN *acse_conn;
ST_RET retcode;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  acse_conn = (ACSE_CONN *)acse_conn_id;

  ACSELOG_ENC2 ("A-RELEASE.rsp:    acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
    acse_conn, acse_conn->user_conn_id);

  if (acse_conn == 0)
    return (E_ACSE_INVALID_CONN_ID);	/* Invalid acse_conn_id		*/

  /* This state may happen, but ACSE state machine says do nothing.	*/
  if (acse_conn->state == STATE_RESP_COLL)
    {
    ACSELOG_ENC0C ("A-RELEASE.rsp not sent because this is Responder side of RELEASE collision");
    return (SD_SUCCESS);
    }

  if (acse_conn->state != STATE_ARLSRSP_WAIT  &&
      acse_conn->state != STATE_INIT_COLL)
    return (E_ACSE_INVALID_STATE);	/* Invalid state for this operation*/

  /* Everything looks OK. Go ahead and encode response and send it.	*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf,cosp_msgsize);
    
  asn1r_strt_constr (aCtx);		/* Start RLRE APDU constructor		*/
  /* Write user_info.							*/
  if (info->user_info.len)
    {
    asn1r_strt_constr (aCtx);
    acse_encode_user_info (aCtx, acse_conn, &info->user_info);
    asn1r_fin_constr (aCtx, RLRE_user_info,CTX,DEF);
    }
  
  /* Write Release Reason.						*/
  if (info->reason_pres)
    {
    asn1r_wr_i16 (aCtx, info->reason);
    asn1r_fin_prim (aCtx, RLRE_reason,CTX);
    }
    
  asn1r_fin_constr (aCtx, TAG_RLRE,APP,DEF);	/* Finish RLRE APDU	*/
  msg_ptr = ASN1_ENC_PTR (aCtx);	/* point to beginning of PDU.	*/
  msg_len = (unsigned int) ASN1_ENC_LEN (aCtx);
  if (msg_len > acse_msgsize)
    return (E_ACSE_BUFFER_OVERFLOW);

  acse_conn->apdu_ptr = msg_ptr;
  acse_conn->apdu_len = msg_len;
  /* NOTE: Only "affirmative" allowed because "Negotiated Release" not	*/
  /* supported by Session.						*/
  retcode = copp_rel_rsp_pos (acse_conn);
  if (retcode != SD_SUCCESS)
    return (E_ACSE_SEND_ERR);
  
  if (acse_conn->state == STATE_ARLSRSP_WAIT)
    acse_conn->state = STATE_IDLE;
  else					/* must be STATE_INIT_COLL	*/
    acse_conn->state = STATE_RLRE_WAIT;

  return (SD_SUCCESS);
  }

/************************************************************************/
/* a_abort_req								*/
/************************************************************************/
ST_RET a_abort_req (
	ST_VOID *acse_conn_id,	/* ACSE's connection ID			*/
	ABRT_APDU *info)	/* ABRT_APDU info			*/
  {
ST_UCHAR *msg_ptr;
ST_UINT msg_len;
ACSE_CONN *acse_conn;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  acse_conn = (ACSE_CONN *)acse_conn_id;

  ACSELOG_ENC2 ("A-ABORT.req:      acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR ".",
    acse_conn, acse_conn->user_conn_id);

  if (acse_conn == NULL)
    return (E_ACSE_INVALID_CONN_ID);	/* Invalid acse_conn_id		*/

  if (acse_conn->state == STATE_IDLE)	/* Waiting for T-DISCONNECT	*/
    return (E_ACSE_INVALID_STATE);	/* Invalid state for this operation*/

  /* Everything looks OK. Go ahead and encode request and send it.	*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf,cosp_msgsize);
    
  asn1r_strt_constr (aCtx);		/* Start ABRT APDU constructor		*/
  /* Write user_info.							*/
  if (info->user_info.len)
    {
    asn1r_strt_constr (aCtx);
    acse_encode_user_info (aCtx, acse_conn, &info->user_info);
    asn1r_fin_constr (aCtx, ABRT_user_info,CTX,DEF);
    }
  
  /* Write Abort Diagnostic.						*/
  if(info->diagnostic_pres == SD_TRUE)
    {
    asn1r_wr_i16 (aCtx, info->diagnostic);
    asn1r_fin_prim (aCtx, ABRT_diagnostic,CTX);
    }

  /* Write Abort Source.						*/
  asn1r_wr_i16 (aCtx, info->source);
  asn1r_fin_prim (aCtx, ABRT_source,CTX);
    
  asn1r_fin_constr (aCtx, TAG_ABRT,APP,DEF);	/* Finish ABRT APDU	*/
  msg_ptr = ASN1_ENC_PTR (aCtx);	/* point to beginning of PDU.	*/
  msg_len = (unsigned int) ASN1_ENC_LEN (aCtx);
  if (msg_len > acse_msgsize)
    return (E_ACSE_BUFFER_OVERFLOW);

  /* Send P-U-Abort Request.						*/
  acse_conn->apdu_ptr = msg_ptr;
  acse_conn->apdu_len = msg_len;
  acse_conn->state = STATE_IDLE;	/* COSP will free acse_conn.	*/
  if (copp_u_abort_req (acse_conn) != SD_SUCCESS)
    return (E_ACSE_SEND_ERR);
  /* CRITICAL: DO NOT access acse_conn after abort. It may have been	*/
  /*     freed by cosp_u_abort_req (called from copp_u_abort_req).	*/
  
  return (SD_SUCCESS);
  }

/************************************************************************/
/* a_data_req - Send A-Data req.					*/
/************************************************************************/
ST_RET a_data_req (
	ST_VOID *acse_conn_id,	/* ACSE's ID for this connection	*/
	ST_UINT data_len,
	ST_UCHAR *data)
  {
ACSE_CONN *acse_conn;

  acse_conn = (ACSE_CONN *)acse_conn_id;
  
  if (acse_conn == NULL)
    return (E_ACSE_INVALID_CONN_ID);	/* Invalid acse_conn_id		*/

  if (data_len > acse_msgsize)
    {		/* Should never happen if ACSE user code is correct.	*/
    ACSELOG_ERR2 ("ACSE-ERROR: A-DATA.req: data len (%u) > max (%u)",
      data_len, acse_msgsize);
    return (E_ACSE_BUFFER_OVERFLOW);
    }
  /* Just copy data to end of "cosp_buf" & set apdu_ptr, apdu_len.	*/
  /* copp_data_req will add COPP encoding in front of this data.	*/
  acse_conn->apdu_ptr = cosp_buf + cosp_msgsize - data_len;
  acse_conn->apdu_len = data_len;
  memcpy (acse_conn->apdu_ptr, data, data_len);

  return (copp_data_req (acse_conn));
  }

/************************************************************************/
/* acse_encode_user_info						*/
/************************************************************************/
static ST_VOID acse_encode_user_info (ASN1_ENC_CTXT *aCtx, ACSE_CONN *acse_conn, BUFFER *user_info)
  {
  asn1r_strt_constr (aCtx);			/* start EXTERNAL		*/
  asn1r_strt_constr (aCtx);			/* start single-ASN1-type	*/
  asn1r_wr_octstr (aCtx, user_info->ptr, user_info->len);
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish single-ASN1-type	*/
  asn1r_wr_i8 (aCtx, acse_conn->pcimms);		/* PCI for MMS		*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);

  /* NOTE:  Transfer-syntax-name (OPTIONAL) not sent. Saves few bytes.	*/

  asn1r_fin_constr (aCtx, EXTERN_CODE,UNI,DEF);	/* finish EXTERNAL		*/
  return;
  }

/************************************************************************/
/* a_buffers_avail - Buffers available for sending A-Data req.		*/
/* This function returns the number of buffers available for sending	*/
/* A-Data.  This may be used, for example, when sending Information	*/
/* Reports, to make sure there are always enough buffers for sending	*/
/* responses to incoming requests.					*/
/*									*/
/* Parameters:								*/
/*	ST_VOID *acse_conn_id	ACSE Connection ID			*/
/*									*/
/* Return:								*/
/*	ST_INT			# of ACSE transmit buffers available	*/
/************************************************************************/

ST_INT a_buffers_avail (
	ST_VOID *acse_conn_id)	/* ACSE's ID for this connection	*/
  {
ACSE_CONN *acse_conn;

  acse_conn = (ACSE_CONN *)acse_conn_id;
  
  if (acse_conn == NULL)
    return (0);		/* Invalid acse_conn_id		*/

  return (tp4_buffers_avail (acse_conn->tp4_conn_id));
  }

	/*------------------------------------------------------*/
	/* 		Security functions 			*/
	/*------------------------------------------------------*/

/********************************************************************************/
/*			a_get_security_info					*/
/*------------------------------------------------------------------------------*/
/* Returns pointers to encryption control and partner's authentication info	*/
/* from the specified channel info. This functions should be called after	*/
/* connection indication was received to retrieve the security informations.	*/
/* After the connection terminates call the s_free_part_security_info() to free	*/
/* the allocated auth_value buf and cleanup the channel info.			*/
/* Parameters:									*/
/*   ACSE_CONN		 *acse_conn	indirect ptr to access security info	*/
/*   S_SEC_PARAMS        *secParams	pointer where to  ret security info.	*/
/* Return:									*/
/*   ST_RET		SD_SUCCESS	if get operation succeeded, otherwise	*/
/*			SD_FAILURE	or other error.				*/
/********************************************************************************/
ST_RET a_get_security_info (ACSE_CONN *acse_conn, S_SEC_SPARAMS *secParams)
{
ST_RET     ret = SD_SUCCESS;

  secParams->authInfo = NULL;		/* local auth info not stored		*/
  secParams->partAuthInfo = NULL;	/* partner auth info			*/
  secParams->encryptCtrl = NULL;	/* connection encryption info		*/

  if (acse_conn)
    {
    secParams->authInfo = &acse_conn->auth_info;
    secParams->partAuthInfo = &acse_conn->part_auth_info;
    secParams->encryptCtrl = &acse_conn->encrypt_ctrl;
    }
  else
    ret = SD_FAILURE;			/* cc should be valid in this call	*/

  return (ret);  
}

/********************************************************************************/
/*			a_free_security_info					*/
/*------------------------------------------------------------------------------*/
/* Releases authentication info resources.					*/
/* Parameters:									*/
/*   ACSE_AUTH_INFO	*authInfo	ptr to free the security resources	*/
/* Return:									*/
/*   ST_RET		SD_SUCCESS	if free operation succeeded		*/
/*			SD_FAILURE      otherwise				*/
/********************************************************************************/
ST_RET a_free_security_info (ACSE_AUTH_INFO *authInfo)
{
ST_RET ret = SD_SUCCESS;

  if (authInfo)
    {
    /* clear authentication info */
    /* if there is an existing auth_value buf pointer then we have to free it	*/
    if (authInfo->auth_pres && authInfo->mech_type != ACSE_AUTH_MECH_PASSWORD)
      if (authInfo->u.other_auth.auth_value.buf)
        {
        chk_free (authInfo->u.other_auth.auth_value.buf);
        authInfo->u.other_auth.auth_value.buf = NULL;
        authInfo->u.other_auth.auth_value.ptr = NULL;
        authInfo->u.other_auth.auth_value.len = 0;
        }
    authInfo->auth_pres = SD_FALSE;
    }
  
  return (ret);  
}


/********************************************************************************/
/*			a_free_part_security_info				*/
/*------------------------------------------------------------------------------*/
/* Releases partner authentication resources and resets the field in ACSE_CONN.	*/
/* Parameters:									*/
/*   ACSE_CONN		*acse_conn	indirect ptr to access security info	*/
/* Return:									*/
/*   ST_RET		SD_SUCCESS	if free operation succeeded		*/
/*			SD_FAILURE      otherwise				*/
/********************************************************************************/
ST_RET a_free_part_security_info (ACSE_CONN *acse_conn)
{
ST_RET     ret = SD_SUCCESS;

  if (acse_conn)
    ret = a_free_security_info (&acse_conn->part_auth_info);
  return (ret);  
}

