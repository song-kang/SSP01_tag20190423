/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : copp_enc.c						*/
/* PRODUCT(S)  : MOSI Stack (over TP0 or TP4)				*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements binding and encoding functions	*/
/* 		of the COPP protocol.					*/
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
/*			copp_bind					*/
/*			copp_unbind					*/
/*									*/
/*			copp_con_req					*/
/*			copp_con_rsp_pos				*/
/*			copp_con_rsp_neg				*/
/*									*/
/*			copp_rel_req					*/
/*			copp_rel_rsp_pos				*/
/*									*/
/*			copp_u_abort_req				*/
/*									*/
/*			copp_data_req					*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 11/17/09  JRB    08     Chg "*_bind_id" to (ST_VOID *).		*/
/*			   copp_con_req: pass loc_addr instead of bind_id.*/
/*			   Use S_FMT_PTR for logging all pointers.	*/
/*			   Disable bind code for SNAP_LITE.		*/
/*			   Use "normal" encode functions for selectors.	*/
/* 04/28/08  GLB    07     Removed CALLED_ONLY and CALLING_ONLY         */
/* 01/02/02  JRB    06     Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/17/98  JRB    05     Allow second call to copp_bind if PSEL same.	*/
/*			   Use new error codes.				*/
/* 02/20/98  JRB    04     Cast wr_i8 arg. Chg abort_reason to ST_INT8.	*/
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

ST_BOOLEAN copp_bind_called;	/* flag to tell if copp_bind was called	*/
ST_UCHAR only_loc_psel [1+MAX_PSEL_LEN];	/* Local len & PSEL	*/

#if !defined(SNAP_LITE)		/* SNAP_LITE code in other files	*/
/************************************************************************/
/* copp_bind								*/
/************************************************************************/
ST_RET copp_bind (ST_VOID *user_bind_id, PRES_ADDR *pres_addr, 
	ST_INT sharable, ST_INT max_conns)
  {
ST_INT ret = SD_SUCCESS;

  COPP_LOG_ENC1 ("P-BIND.req:    copp_user_bind_id =" S_FMT_PTR, user_bind_id);

  if (!copp_bind_called)
    {
    /* Make sure Local PSEL len is legal before copying.			*/
    if (pres_addr->psel_len <= sizeof (only_loc_psel)-1)
      {
      /* Save Local PSEL (only do memcpy if len != 0).			*/
      if ((only_loc_psel [0] = (ST_UCHAR) pres_addr->psel_len) != 0)
        memcpy (&only_loc_psel [1], pres_addr->psel, pres_addr->psel_len);
      }
    else
      {
      COPP_LOG_ERR1 ("COPP-ERROR: P-BIND.req: Invalid length=%d of PSEL.",
        pres_addr->psel_len);
      ret = COPP_ERR_INV_PSEL;			/* invalid local PSEL	*/
      }
    }
  else
    {    /* Second Bind. Only allow it if the PSEL is the same.		*/
    if (only_loc_psel [0] != (ST_UCHAR) pres_addr->psel_len  ||
         (pres_addr->psel_len != 0  &&
          memcmp (&only_loc_psel [1], pres_addr->psel, pres_addr->psel_len)))
      {  
      COPP_LOG_ERR0 ("COPP-ERROR: Cannot BIND to different PSEL");
      ret = COPP_ERR_INV_PSEL;			/* invalid local PSEL	*/
      }  
    }

  /* Call cosp_bind.	*/
  if (ret == SD_SUCCESS)
    ret = cosp_bind (user_bind_id, pres_addr, sharable, max_conns);

  /* If successful, set called flag. Otherwise, treat next bind like first.*/
  if (ret == SD_SUCCESS)
    copp_bind_called = SD_TRUE;

  return (ret);
  }

/************************************************************************/
/* copp_unbind								*/
/************************************************************************/
ST_RET copp_unbind (ST_VOID *copp_bind_id)
  {
  COPP_LOG_ENC1 ("P-UNBIND.req:                                 copp_bind_id =" S_FMT_PTR, copp_bind_id);
  return (cosp_unbind (copp_bind_id));
  }
#endif	/* !defined(SNAP_LITE)	*/

/************************************************************************/
/* copp_con_req								*/
/************************************************************************/
ST_RET copp_con_req (ACSE_CONN *acse_conn, PRES_ADDR *loc_addr, PRES_ADDR *rem_addr)
  {
  /* WARNING: this hardcoding assumes that ACSE pci always equals 1	*/
  /*       and MMS pci always equals 3 when we send P-CONNECT.req.	*/
  /*       These must be set in a_associate_req before any encoding.	*/
static ST_UCHAR hc_definition_list [] =
  {
  0x30, 0x0f, 0x02, 0x01, 0x01, 0x06, 0x04, 0x52, 0x01, 0x00, 0x01,
  0x30, 0x04, 0x06, 0x02, 0x51, 0x01,
  0x30, 0x10, 0x02, 0x01, 0x03, 0x06, 0x05, 0x28, 0xca, 0x22, 0x02, 0x01,
  0x30, 0x04, 0x06, 0x02, 0x51, 0x01
  };
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_UTIL_RESOURCES ();
  COPP_LOG_ENC3 ("P-CONNECT.req:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %d", acse_conn, acse_conn->user_conn_id, acse_conn->apdu_len);
  COPP_LOG_ENCH (acse_conn->apdu_len, acse_conn->apdu_ptr);
  S_UNLOCK_UTIL_RESOURCES ();

  if (acse_conn->copp_state != COPP_STATE_IDLE)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot send P-CONNECT.req (Illegal state=%d)",
      acse_conn->copp_state);
    return (COPP_ERR_INV_STATE);
    }
  acse_conn->cr = SD_FALSE;
  acse_conn->rl = SD_FALSE;

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf, cosp_msgsize);

  asn1r_strt_constr (aCtx);			/* start CP SET	*/
  asn1r_strt_constr (aCtx);			/* start enclosed SEQUENCE	*/
  asn1r_strt_constr (aCtx);		/* start P-User-data (fully-encoded)	*/
  asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
  asn1r_strt_constr (aCtx);			/* start single-ASN1-type	*/

  /* Fake out ACSE encoding.  Already encoded into cosp_buf.		*/
  aCtx->asn1r_field_ptr -= acse_conn->apdu_len;

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish single-ASN1-type	*/
  asn1r_wr_i8 (aCtx, acse_conn->pciacse);		/* PCI for ACSE			*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  /* Do not encode Transfer syntax name.  We know it is ASN1.		*/
  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
  asn1r_fin_constr (aCtx, 1,APP,DEF);	/* finish P-User-data (fully-encoded)	*/

  /* Hardcode Presentation-requirements (kernel only).			*/
  *aCtx->asn1r_field_ptr-- = 0x00;
  *aCtx->asn1r_field_ptr-- = 0x06;
  *aCtx->asn1r_field_ptr-- = 0x02;
  *aCtx->asn1r_field_ptr-- = 0x88;

  /* 2 P-contexts: ACSE and MMS, and they both have a TSN of ASN1,	*/
  /* so we can hardcode this.						*/
  /* WARNING: this hardcoding assumes that ACSE pci always equals 1	*/
  /*       and MMS pci always equals 3 when we send P-CONNECT.req.	*/
  /*       These must be set in a_associate_req before any encoding.	*/
  asn1r_strt_constr (aCtx);		/* start P-context-def-list.	*/
  aCtx->asn1r_field_ptr -= sizeof (hc_definition_list);
  memcpy (aCtx->asn1r_field_ptr + 1, hc_definition_list, sizeof (hc_definition_list));
  asn1r_fin_constr (aCtx, 4,CTX,DEF);	/* finish P-context-def-list.	*/

  /* Encode Called-p-selector.					*/
  asn1r_wr_octstr (aCtx, rem_addr->psel, rem_addr->psel_len);
  asn1r_fin_prim (aCtx, 2, CTX);	/* finish Called-PSEL	*/

  /* Encode Calling-p-selector.					*/
  asn1r_wr_octstr (aCtx, loc_addr->psel, loc_addr->psel_len);
  asn1r_fin_prim (aCtx, 1, CTX);	/* finish Calling-PSEL	*/

  /* Protocol-version defaults to version-1, so don't encode.		*/

  asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish enclosed SEQUENCE	*/
  /* Hard-code Mode.  Always "normal-mode".	*/
  *aCtx->asn1r_field_ptr-- = 0x01;
  *aCtx->asn1r_field_ptr-- = 0x01;
  *aCtx->asn1r_field_ptr-- = 0x80;
  *aCtx->asn1r_field_ptr-- = 0x03;
  *aCtx->asn1r_field_ptr-- = 0xA0;
  asn1r_fin_constr (aCtx, SET_CODE,UNI,DEF);	/* finish CPA SET	*/
  acse_conn->ppdu_ptr = aCtx->asn1r_field_ptr + 1;
  acse_conn->ppdu_len = (unsigned int) (cosp_buf + cosp_msgsize - acse_conn->ppdu_ptr);
  /* Pointing to beginning of Session User Information Field now.	*/

  acse_conn->copp_state = COPP_STATE_AWAIT_CPA;
  /* Pass to COSP.							*/
  return (cosp_con_req (acse_conn, loc_addr, rem_addr));
  }
/************************************************************************/
/* copp_rel_req								*/
/************************************************************************/
ST_RET copp_rel_req (ACSE_CONN *acse_conn)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_UTIL_RESOURCES ();
  COPP_LOG_ENC3 ("P-RELEASE.req:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %d", acse_conn, acse_conn->user_conn_id, acse_conn->apdu_len);
  COPP_LOG_ENCH (acse_conn->apdu_len, acse_conn->apdu_ptr);
  S_UNLOCK_UTIL_RESOURCES ();

  if (acse_conn->copp_state != COPP_STATE_CONNECTED)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot send P-RELEASE.req (Illegal state=%d)",
      acse_conn->copp_state);
    return (COPP_ERR_INV_STATE);
    }
  /* Set release collision flags.	*/
  if (acse_conn->rl)
    acse_conn->cr = SD_TRUE;
  acse_conn->rl = SD_TRUE;

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf, cosp_msgsize);

  asn1r_strt_constr (aCtx);		/* start P-User-data (fully-encoded)	*/
  asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
  asn1r_strt_constr (aCtx);			/* start single-ASN1-type	*/

  /* Fake out ACSE encoding.  Already encoded into cosp_buf.		*/
  aCtx->asn1r_field_ptr -= acse_conn->apdu_len;

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish single-ASN1-type	*/
  asn1r_wr_i8 (aCtx, acse_conn->pciacse);		/* PCI for ACSE			*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  /* Do not encode Transfer syntax name.  We know it is ASN1.		*/
  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
  asn1r_fin_constr (aCtx, 1,APP,DEF);	/* finish P-User-data (fully-encoded)	*/

  acse_conn->ppdu_ptr = aCtx->asn1r_field_ptr + 1;
  acse_conn->ppdu_len = (unsigned int) (cosp_buf + cosp_msgsize - acse_conn->ppdu_ptr);
  /* Pointing to beginning of Session User Information Field now.	*/

  return (cosp_rel_req (acse_conn));
  }

/************************************************************************/
/* copp_con_rsp_pos							*/
/* Encode and send CPA PPDU.						*/
/************************************************************************/
ST_RET copp_con_rsp_pos (ACSE_CONN *acse_conn)
  {
static ST_UCHAR hc_result_list [] =
{
0x30, 0x07, 0x80, 0x01, 0x00, 0x81, 0x02, 0x51, 0x01,
0x30, 0x07, 0x80, 0x01, 0x00, 0x81, 0x02, 0x51, 0x01
};
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_UTIL_RESOURCES ();
  COPP_LOG_ENC3 ("P-CONNECT.rsp+:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %d", acse_conn, acse_conn->user_conn_id, acse_conn->apdu_len);
  COPP_LOG_ENCH (acse_conn->apdu_len, acse_conn->apdu_ptr);
  S_UNLOCK_UTIL_RESOURCES ();

  if (acse_conn->copp_state != COPP_STATE_AWAIT_CON_RSP)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot send P-CONNECT.rsp+ (Illegal state=%d)",
      acse_conn->copp_state);
    return (COPP_ERR_INV_STATE);
    }

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf, cosp_msgsize);

  asn1r_strt_constr (aCtx);			/* start CPA SET	*/
  asn1r_strt_constr (aCtx);			/* start enclosed SEQUENCE	*/
  asn1r_strt_constr (aCtx);		/* start P-User-data (fully-encoded)	*/
  asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
  asn1r_strt_constr (aCtx);			/* start single-ASN1-type	*/

  /* Fake out ACSE encoding.  Already encoded into cosp_buf.		*/
  aCtx->asn1r_field_ptr -= acse_conn->apdu_len;

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish single-ASN1-type	*/
  asn1r_wr_i8 (aCtx, acse_conn->pciacse);		/* PCI for ACSE			*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  /* Do not encode Transfer syntax name.  We know it is ASN1.		*/
  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
  asn1r_fin_constr (aCtx, 1,APP,DEF);	/* finish P-User-data (fully-encoded)	*/

  /* Hardcode Presentation-requirements (kernel only).			*/
  *aCtx->asn1r_field_ptr-- = 0x00;
  *aCtx->asn1r_field_ptr-- = 0x06;
  *aCtx->asn1r_field_ptr-- = 0x02;
  *aCtx->asn1r_field_ptr-- = 0x88;

  /* Hey, I like this.  We only accept connections if there are		*/
  /* 2 P-contexts: ACSE and MMS, and they both have a TSN of ASN1,	*/
  /* so we can hardcode this.						*/
  asn1r_strt_constr (aCtx);		/* start P-context-def-result-list.	*/
  aCtx->asn1r_field_ptr -= sizeof (hc_result_list);
  memcpy (aCtx->asn1r_field_ptr + 1, hc_result_list, sizeof (hc_result_list));
  asn1r_fin_constr (aCtx, 5,CTX,DEF);	/* finish P-context-def-result-list.	*/

  /* Encode Responding-p-selector, if known. Saved in acse_conn if	*/
  /* decoded from request (length=0 if not present or not decoded).	*/
  if (acse_conn->loc_psel[0])		/* first byte is length	*/
    {
    asn1r_wr_octstr (aCtx, &acse_conn->loc_psel[1], acse_conn->loc_psel[0]);
    asn1r_fin_prim (aCtx, 3, CTX);	/* finish Responding-PSEL	*/
    }

  /* Protocol-version defaults to version-1, so don't encode.		*/

  asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish enclosed SEQUENCE	*/
  /* Hard-code Mode.  Always "normal-mode".	*/
  *aCtx->asn1r_field_ptr-- = 0x01;
  *aCtx->asn1r_field_ptr-- = 0x01;
  *aCtx->asn1r_field_ptr-- = 0x80;
  *aCtx->asn1r_field_ptr-- = 0x03;
  *aCtx->asn1r_field_ptr-- = 0xA0;
  asn1r_fin_constr (aCtx, SET_CODE,UNI,DEF);	/* finish CPA SET	*/
  acse_conn->ppdu_ptr = aCtx->asn1r_field_ptr + 1;
  acse_conn->ppdu_len = (unsigned int) (cosp_buf + cosp_msgsize - acse_conn->ppdu_ptr);
  /* Pointing to beginning of Session User Information Field now.	*/

  acse_conn->copp_state = COPP_STATE_CONNECTED;
  /* Pass to COSP.							*/
  return (cosp_con_rsp_pos (acse_conn));
  }

/************************************************************************/
/* copp_con_rsp_neg							*/
/* Encode and send CPR PPDU.						*/
/* If "provider_reason" < 0, this is "user-reject", else this is	*/
/* "provider-reject" and the value of "provider_reason" will be		*/
/* encoded in the "Provider-reason" parameter of the PPDU.		*/
/************************************************************************/
ST_RET copp_con_rsp_neg (ACSE_CONN *acse_conn, ST_INT provider_reason)
  {
ST_INT j;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  COPP_LOG_ENC3 ("P-CONNECT.rsp-:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "  reason =%d",
    acse_conn, acse_conn->user_conn_id, provider_reason);
  if (provider_reason < 0)
    {				/* user-reject:  must have User-data	*/
    COPP_LOG_ENCC1 ("User data length: %d", acse_conn->apdu_len);
    COPP_LOG_ENCH (acse_conn->apdu_len, acse_conn->apdu_ptr);
    }

  if (provider_reason < 0 && acse_conn->copp_state != COPP_STATE_AWAIT_CON_RSP)
    {	/* If from user, must be in "await P-CONrsp" state.	*/
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot send P-CONNECT.rsp- (Illegal state=%d)",
      acse_conn->copp_state);
    return (COPP_ERR_INV_STATE);
    }

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf, cosp_msgsize);

  asn1r_strt_constr (aCtx);			/* start CPR SEQUENCE	*/
  if (provider_reason < 0)
    {				/* user-reject:  encode P-User-data	*/
    asn1r_strt_constr (aCtx);		/* start P-User-data (fully-encoded)	*/
    asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
    asn1r_strt_constr (aCtx);			/* start single-ASN1-type	*/

    /* Fake out ACSE encoding.  Already encoded into cosp_buf.		*/
    aCtx->asn1r_field_ptr -= acse_conn->apdu_len;

    asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish single-ASN1-type	*/
    asn1r_wr_i8 (aCtx, acse_conn->pciacse);		/* PCI for ACSE			*/
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    /* Do not encode Transfer syntax name.  We know it is ASN1.		*/
    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
    asn1r_fin_constr (aCtx, 1,APP,DEF);	/* finish P-User-data (fully-encoded)	*/
    }	/* end "if (provider_reason < 0)"	*/

  if (provider_reason >= 0)
    {			/* provider-reject:  encode Provider-reason	*/
    asn1r_wr_i8 (aCtx, (ST_INT8) provider_reason);		/* Provider-reason	*/
    asn1r_fin_prim (aCtx, 10,CTX);
    }

  if (provider_reason == PROV_REASON_DEFAULT_CONTEXT)
    {	/* Hardcode "Default-context-result = 2" (provider rejection)	*/
    *aCtx->asn1r_field_ptr-- = 0x02;
    *aCtx->asn1r_field_ptr-- = 0x01;
    *aCtx->asn1r_field_ptr-- = 0x87;
    }
    
  /* If P-contexts proposed, just accept all of them.		*/
  /* We're rejecting the connect anyway, so it doesn't matter.	*/
  if (acse_conn->num_ctxt)
    {
    asn1r_strt_constr (aCtx);		/* start P-context-def-result-list.	*/
    for (j = 0;  j < acse_conn->num_ctxt;  j++)
      {
      asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
      /* DEBUG: would be better to encode same TSN they sent.		*/
      *aCtx->asn1r_field_ptr-- = 0x01;	/* Hardcode Transfer-syntax-name = ASN.1*/
      *aCtx->asn1r_field_ptr-- = 0x51;
      *aCtx->asn1r_field_ptr-- = 0x02;
      *aCtx->asn1r_field_ptr-- = 0x81;
      *aCtx->asn1r_field_ptr-- = 0x00;	/* Hardcode "Result=0" (acceptance)	*/
      *aCtx->asn1r_field_ptr-- = 0x01;
      *aCtx->asn1r_field_ptr-- = 0x80;
      asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
      }
    asn1r_fin_constr (aCtx, 5,CTX,DEF);	/* finish P-context-def-result-list.	*/
    }
    
  /* Encode Responding-p-selector, if known. Saved in acse_conn if	*/
  /* decoded from request (length=0 if not present or not decoded).	*/
  if (acse_conn->loc_psel[0])		/* first byte is length	*/
    {
    asn1r_wr_octstr (aCtx, &acse_conn->loc_psel[1], acse_conn->loc_psel[0]);
    asn1r_fin_prim (aCtx, 3, CTX);	/* finish Responding-PSEL	*/
    }

  /* Protocol-version defaults to version-1, so don't encode.		*/

  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);		/* finish CPR SEQUENCE	*/
  acse_conn->ppdu_ptr = aCtx->asn1r_field_ptr + 1;
  acse_conn->ppdu_len = (unsigned int) (cosp_buf + cosp_msgsize - acse_conn->ppdu_ptr);
  /* Pointing to beginning of Session User Information Field now.	*/
  
  acse_conn->copp_state = COPP_STATE_IDLE;
  /* Pass to COSP.							*/
  return (cosp_con_rsp_neg (acse_conn, COSP_CON_RSP_U_IN_UDATA));
  }

/************************************************************************/
/*			copp_rel_rsp_pos				*/
/************************************************************************/
ST_RET copp_rel_rsp_pos (ACSE_CONN *acse_conn)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  COPP_LOG_ENC3 ("P-RELEASE.rsp+:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %d", acse_conn, acse_conn->user_conn_id, acse_conn->apdu_len);
  COPP_LOG_ENCH (acse_conn->apdu_len, acse_conn->apdu_ptr);

  if (acse_conn->copp_state != COPP_STATE_CONNECTED)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot send P-RELEASE.rsp+ (Illegal state=%d)",
      acse_conn->copp_state);
    return (COPP_ERR_INV_STATE);
    }

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf, cosp_msgsize);

  asn1r_strt_constr (aCtx);		/* start P-User-data (fully-encoded)	*/
  asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
  asn1r_strt_constr (aCtx);			/* start single-ASN1-type	*/

  /* Fake out ACSE encoding.  Already encoded into cosp_buf.		*/
  aCtx->asn1r_field_ptr -= acse_conn->apdu_len;

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish single-ASN1-type	*/
  asn1r_wr_i8 (aCtx, acse_conn->pciacse);		/* PCI for ACSE			*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  /* Do not encode Transfer syntax name.  We know it is ASN1.		*/
  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
  asn1r_fin_constr (aCtx, 1,APP,DEF);	/* finish P-User-data (fully-encoded)	*/

  acse_conn->ppdu_ptr = aCtx->asn1r_field_ptr + 1;
  acse_conn->ppdu_len = (unsigned int) (cosp_buf + cosp_msgsize - acse_conn->ppdu_ptr);
  /* Pointing to beginning of Session User Information Field now.	*/

  if (acse_conn->cr)
    {
    acse_conn->cr = SD_FALSE;
    acse_conn->rl = SD_FALSE;	/* and stay in COPP_STATE_CONNECTED	*/
    }
  else
    acse_conn->copp_state = COPP_STATE_IDLE;

  return (cosp_rel_rsp_pos (acse_conn));
  }

/************************************************************************/
/* copp_u_abort_req							*/
/************************************************************************/
ST_RET copp_u_abort_req (ACSE_CONN *acse_conn)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  COPP_LOG_ENC3 ("P-U-ABORT.req:    acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %d", acse_conn, acse_conn->user_conn_id, acse_conn->apdu_len);
  COPP_LOG_ENCH (acse_conn->apdu_len, acse_conn->apdu_ptr);

  if (acse_conn->copp_state == COPP_STATE_IDLE)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot send P-U-ABORT.req (Illegal state=%d)",
      acse_conn->copp_state);
    return (COPP_ERR_INV_STATE);
    }

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf, cosp_msgsize);

  asn1r_strt_constr (aCtx);			/* start ARU			*/
  asn1r_strt_constr (aCtx);		/* start P-User-data (fully-encoded)	*/
  asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
  asn1r_strt_constr (aCtx);			/* start single-ASN1-type	*/

  /* Fake out ACSE encoding.  Already encoded into cosp_buf.		*/
  aCtx->asn1r_field_ptr -= acse_conn->apdu_len;

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish single-ASN1-type	*/
  asn1r_wr_i8 (aCtx, acse_conn->pciacse);		/* PCI for ACSE			*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  /* Do not encode Transfer syntax name.  We know it is ASN1.		*/
  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
  asn1r_fin_constr (aCtx, 1,APP,DEF);	/* finish P-User-data (fully-encoded)	*/

  asn1r_strt_constr (aCtx);		/* start P-context-identifier-list	*/
  asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
  asn1r_wr_objid (aCtx, asn1_obj_id.comps,
            asn1_obj_id.num_comps);	/* Transfer syntax name (i.e. ASN1)*/
  asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
  asn1r_wr_i8 (aCtx, acse_conn->pciacse);		/* PCI for ACSE			*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);	/* finish P-context-identifier-list	*/

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ARU			*/

  acse_conn->ppdu_ptr = aCtx->asn1r_field_ptr + 1;
  acse_conn->ppdu_len = (unsigned int) (cosp_buf + cosp_msgsize - acse_conn->ppdu_ptr);
  /* Pointing to beginning of Session User Information Field now.	*/

  acse_conn->copp_state = COPP_STATE_IDLE;
  return (cosp_u_abort_req (acse_conn));
  }

/************************************************************************/
/* copp_data_req							*/
/************************************************************************/
ST_RET copp_data_req (ACSE_CONN *acse_conn)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  COPP_LOG_ENC3 ("P-DATA.req:      acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %d", acse_conn, acse_conn->user_conn_id, acse_conn->apdu_len);
  COPP_LOG_ENCH (acse_conn->apdu_len, acse_conn->apdu_ptr);

  if (acse_conn->copp_state != COPP_STATE_CONNECTED)
    {
    COPP_LOG_ERR1 ("COPP-ERROR: Cannot send P-DATA.req (Illegal state=%d)",
      acse_conn->copp_state);
    return (COPP_ERR_INV_STATE);
    }

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf, cosp_msgsize);

  asn1r_strt_constr (aCtx);		/* start P-User-data (fully-encoded)	*/
  asn1r_strt_constr (aCtx);			/* start SEQUENCE	*/
  asn1r_strt_constr (aCtx);			/* start single-ASN1-type	*/

  /* Fake out ACSE encoding.  Already encoded into cosp_buf.		*/
  aCtx->asn1r_field_ptr -= acse_conn->apdu_len;

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish single-ASN1-type	*/
  asn1r_wr_i8 (aCtx, acse_conn->pcimms);		/* PCI for MMS			*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  /* Do not encode Transfer syntax name.  We know it is ASN1.		*/
  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
  asn1r_fin_constr (aCtx, 1,APP,DEF);	/* finish P-User-data (fully-encoded)	*/

  acse_conn->ppdu_ptr = aCtx->asn1r_field_ptr + 1;
  acse_conn->ppdu_len = (unsigned int) (cosp_buf + cosp_msgsize - acse_conn->ppdu_ptr);
  /* Pointing to beginning of Session User Information Field now.	*/

  return (cosp_data_req (acse_conn));
  }

/************************************************************************/
/* copp_p_abort_req							*/
/* Encode and send ARP PPDU (i.e. P-P-Abort.req).			*/
/* This function only called by the COPP-provider (not by COPP-user).	*/
/* Parameters:								*/
/*  ACSE_CONN	*acse_conn		Pointer to connection info	*/
/*  ST_INT8	abort_reason		Abort-reason to send in ARP	*/
/*  ST_INT8	event_id		Event-identifier to send in ARP	*/
/*					If -1, don't send it.		*/
/************************************************************************/
ST_VOID copp_p_abort_req (ACSE_CONN *acse_conn, ST_INT8 abort_reason, ST_INT8 event_id)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  COPP_LOG_ENC4 ("P-P-ABORT.req:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR 
                 "  reason =%d  event_id =%d",
    acse_conn, acse_conn->user_conn_id, (ST_INT) abort_reason, (ST_INT) event_id);

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (aCtx, cosp_buf, cosp_msgsize);

  asn1r_strt_constr (aCtx);			/* start ARP SEQUENCE	*/

  if (event_id != -1)
    {
    asn1r_wr_i8 (aCtx, event_id);		/* Event-identifier	*/
    asn1r_fin_prim (aCtx, 1,CTX);
    }

  asn1r_wr_i8 (aCtx, abort_reason);		/* Abort-reason		*/
  asn1r_fin_prim (aCtx, 0,CTX);

  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish ARP SEQUENCE	*/
  acse_conn->ppdu_ptr = aCtx->asn1r_field_ptr + 1;
  acse_conn->ppdu_len = (unsigned int) (cosp_buf + cosp_msgsize - acse_conn->ppdu_ptr);
  /* Pointing to beginning of Session User Information Field now.	*/
  
  /* Pass ARP to COSP.							*/
  cosp_u_abort_req (acse_conn);
  }
