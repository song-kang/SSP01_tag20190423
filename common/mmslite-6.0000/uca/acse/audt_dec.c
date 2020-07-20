/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995-2003, All Rights Reserved.			        	*/
/*									*/
/* MODULE NAME : audt_dec.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	A-Unit-Data decode						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 06/22/10  JRB	   Del clpp_event (use new subnet_serve).	*/
/* 12/03/03  JRB    19     Fix asn1r_get_bitstr calls, init data=0 first*/
/* 03/31/03  JRB    18     asn1r_get_bitstr: add max_bits arg.		*/
/* 06/20/02  JRB    17     Chg to use new netAddr union in PRES_ADDR.	*/
/* 03/04/02  JRB    16     Fix thisFile..				*/
/* 01/10/02  JRB    15     Del all binding.				*/
/*			   Del clnp_data_ind (decode & call ACSE ind).	*/
/*			   Add cltp_decode_nsdu_2 (decode only) & chg	*/
/*			   cltp_decode_nsdu to use it.			*/
/*			   Pass (AUDT_APDU *) to clpp_decode.		*/
/*			   Eliminate ALL globals by saving decode info	*/
/*			   in usr_info member of ASN1_DEC_CTXT.		*/
/*			   Chg asn1r_objidcmp to asn1_objidcmp.		*/
/* 03/12/01  JRB    14     Use new SMEM allocation functions.		*/
/* 08/05/99  JRB    13     Support "multithreading" using ASN1R		*/
/*			   and "clnp_read_fun_cl".			*/
/*			   DON'T disable clpp_event if TP4_ENABLED.	*/
/* 12/07/98  JRB    12     Save loc_nsap, loc_mac, rem_mac on AUDT-Ind.	*/
/*			   Don't check CALLED_PADDR. Let user check it.	*/
/*			   Del audt_debug_sel. Use acse_debug_sel.	*/
/* 08/13/98  JRB    11     Lint cleanup.				*/
/* 08/03/98  JRB    10     Allow NULL PSEL, SSEL, TSEL.			*/
/* 07/23/98  JRB    09     Delete Marben support. Use Marben version of	*/
/*			   clpp_find_pbind. Chk clpp_find_pbind return.	*/
/*			   Use asn1r_objidcmp.				*/
/* 03/18/98  JRB    08     Use defines (not variables) for ASN1 errors.	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 12/16/96  JRB    07     Use audt_debug_sel via "AUDTLOG_*" macros.	*/
/* 11/22/96  JRB    06     Add cltp_decode_nsdu for LEAN_T. This can be	*/
/*			   called from COTP to process a CLTP PDU.	*/
/* 10/04/96  JRB    05     Add LEAN_T support.  Move Marben specific	*/
/*			   code out of clnp_data_ind.			*/
/* 05/17/96  JRB    04     Change globals to static (only used here).	*/
/* 03/08/96  JRB    03     Define thisFileName even if !DEBUG_MMS.	*/
/* 02/16/96  JRB    02     Clean up asn1r_set_dec_err codes.			*/
/* 10/30/95  JRB    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "acse2.h"	/* this includes "asn1defs.h"	*/
#include "tp4api.h"
#include "tp4.h"
#include "tp4_encd.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

/* Structure for tracking this decode.		*/
typedef struct
  {
  ST_INT8 pcimms;
  ST_INT8 pciacse;
  ST_INT8 pcisave;		/* PCI rcvd in P-context-def-list	*/  

  MMS_OBJ_ID *my_obj_id;
  ST_INT32 *my_i32;
  } DECODE_TRACK;

/************************************************************************/
/* Local function prototypes						*/
/************************************************************************/

static ST_RET clpp_decode (ST_UCHAR *msg_ptr, ST_INT len, AUDT_APDU *audt);
static ST_VOID clnp_get_osi_addr (ST_UCHAR *buf, PRES_ADDR *pres_addr);

static ST_VOID decode_p_unit_data (ASN1_DEC_CTXT *ac);
static ST_VOID decode_version (ASN1_DEC_CTXT *ac);
static ST_VOID decode_calling_psel (ASN1_DEC_CTXT *ac);
static ST_VOID decode_called_psel (ASN1_DEC_CTXT *ac);
static ST_VOID decode_pc_deflist (ASN1_DEC_CTXT *ac);
static ST_VOID decode_context (ASN1_DEC_CTXT *ac);
static ST_VOID decode_pci (ASN1_DEC_CTXT *ac);
static ST_VOID decode_asn (ASN1_DEC_CTXT *ac);
static ST_VOID decode_tsn_list (ASN1_DEC_CTXT *ac);
static ST_VOID decode_tsn (ASN1_DEC_CTXT *ac);
static ST_VOID decode_tsn_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_context_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_pc_deflist_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_p_user_data (ASN1_DEC_CTXT *ac);
static ST_VOID decode_pdv_list (ASN1_DEC_CTXT *ac);
static ST_VOID decode_acse_tsn (ASN1_DEC_CTXT *ac);
static ST_VOID decode_acse_pci (ASN1_DEC_CTXT *ac);
static ST_VOID decode_pdv (ASN1_DEC_CTXT *ac);
static ST_VOID decode_audt (ASN1_DEC_CTXT *ac);
static ST_VOID decode_audt_main (ASN1_DEC_CTXT *ac, ST_UINT16 id_code);
static ST_VOID decode_external (ASN1_DEC_CTXT *ac);
static ST_VOID decode_mms_tsn (ASN1_DEC_CTXT *ac);
static ST_VOID decode_mms_pci (ASN1_DEC_CTXT *ac);
static ST_VOID decode_mms_data (ASN1_DEC_CTXT *ac);
static ST_VOID decode_constr_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_mms_data_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_external_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_audt_user_info_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_audt_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_pdv_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_pdv_list_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_p_user_data_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_p_unit_data_done (ASN1_DEC_CTXT *ac);
static ST_VOID decode_clpp_done (ASN1_DEC_CTXT *ac);

static ST_VOID decode_obj_id (ASN1_DEC_CTXT *ac);
static ST_VOID decode_i32 (ASN1_DEC_CTXT *ac);

/************************************************************************/
/*			cltp_decode_nsdu (LEAN-T Only)			*/
/* This decodes the NSDU. It is always called				*/
/* by clpp_event, but it may also be called from COTP (i.e. TP4).	*/
/* If the decode is successful, it calls the ACSE indication function.	*/
/************************************************************************/
ST_VOID cltp_decode_nsdu (N_UNITDATA *pNudt)
  {
AUDT_APDU audt;		/* filled in by decode	*/
ST_RET rc;
  /* Call decode function. If successful, call ACSE user indication function.*/
  /* If it fails, do nothing (error should already be logged).		*/
  if ((rc = cltp_decode_nsdu_2 (pNudt, &audt)) == SD_SUCCESS)
    u_a_unit_data_ind (&audt);
  }


/************************************************************************/
/*			cltp_decode_nsdu_2				*/
/* This function ONLY decodes the NSDU up through the ACSE layer.	*/
/* All results are placed in the caller's structure pointed to by the	*/
/* "pAudt" argument.							*/
/* The caller must call the appropriate ACSE user indication function	*/
/* (e.g. u_a_unit_data_ind) to process the decoded info.		*/
/************************************************************************/
ST_RET cltp_decode_nsdu_2 (N_UNITDATA *pNudt, AUDT_APDU *pAudt)
  {
ST_UCHAR *curptr;			/* CRITICAL: must be unsigned			*/
ST_UCHAR head_len;
ST_UCHAR *head_start;
ST_UCHAR param_len;

  memset (pAudt, 0, sizeof (AUDT_APDU));		/* init struct	*/

  /* Get the NSAPS and copy to pAudt.	*/
  clnp_get_osi_addr (pNudt->rem_nsap, &pAudt->calling_paddr);
  clnp_get_osi_addr (pNudt->loc_nsap, &pAudt->called_paddr);

  /* Get the MACS and copy to pAudt.	*/
  memcpy (pAudt->rem_mac, pNudt->rem_mac, CLNP_MAX_LEN_MAC);
  memcpy (pAudt->loc_mac, pNudt->loc_mac, CLNP_MAX_LEN_MAC);

  /* Decode the N-Unitdata and fill in rest of pAudt.		*/
  curptr = pNudt->data_buf;
  head_len = *curptr++;
  head_start = curptr;
  if (head_len == 255)
    {	/* DEBUG: if this ever happens, add code to handle extended length*/
    ACSELOG_ERR0 ("CLNP: Extended header length not supported");
    return (SD_FAILURE);
    }
  if (*curptr++ != 0x40)		/* Not T-Unit-Data PDU		*/
    {
    ACSELOG_ERR0 ("CLTP: Not T-UNITDATA PDU");
    return (SD_FAILURE);
    }
  while (curptr < head_start + head_len)
    {
    switch (*curptr++)
      {
      case 0xC1:			/* Calling TSEL			*/
      pAudt->calling_paddr.tsel_len = (ST_UINT) *curptr++;
      memcpy (pAudt->calling_paddr.tsel, curptr, pAudt->calling_paddr.tsel_len);
      curptr += pAudt->calling_paddr.tsel_len;
      break;

      case 0xC2:			/* Called TSEL			*/
      pAudt->called_paddr.tsel_len = (ST_UINT) *curptr++;
      memcpy (pAudt->called_paddr.tsel, curptr, pAudt->called_paddr.tsel_len);
      curptr += pAudt->called_paddr.tsel_len;
      break;

      default:				/* Checksum	*/
      param_len = *curptr++;
      curptr += param_len;		/* Throw it away.		*/
      break;
      }	/* end "switch"	*/
    }	/* end "while"	*/
  if (curptr != head_start + head_len)
    {
    ACSELOG_ERR0 ("CLTP: header length incorrect");
    return (SD_FAILURE);
    }
  /* Should be at start of Transport User Data which contains the	*/
  /* Session Unit Data SPDU.						*/
  if (*curptr++ != 64)			/* SI = Session Unit Data PDU	*/
    {
    ACSELOG_ERR0 ("CLSP: Not S-UNITDATA PDU");
    return (SD_FAILURE);
    }
  head_len = *curptr++;
  head_start = curptr;
  if (head_len == 255)
    {	/* DEBUG: if this ever happens, add code to handle extended length*/
    ACSELOG_ERR0 ("CLSP: Extended header length not supported");
    return (SD_FAILURE);
    }
  while (curptr < head_start + head_len)
    {
    switch (*curptr++)
      {
      case 51:				/* Calling SSEL			*/
      pAudt->calling_paddr.ssel_len = (ST_UINT) *curptr++;
      memcpy (pAudt->calling_paddr.ssel, curptr, pAudt->calling_paddr.ssel_len);
      curptr += pAudt->calling_paddr.ssel_len;
      break;

      case 52:				/* Called SSEL			*/
      pAudt->called_paddr.ssel_len = (ST_UINT) *curptr++;
      memcpy (pAudt->called_paddr.ssel, curptr, pAudt->called_paddr.ssel_len);
      curptr += pAudt->called_paddr.ssel_len;
      break;

      default:				/* Version#	*/
      param_len = *curptr++;
      curptr += param_len;		/* Throw it away.		*/
      break;
      }	/* end "switch"	*/
    }	/* end "while"	*/
  if (curptr != head_start + head_len)
    {
    ACSELOG_ERR0 ("CLSP: header length incorrect");
    return (SD_FAILURE);
    }
  /* Should be at start of Session User Information field which contains*/
  /* the Presentation Unit Data PDU.						*/

  if (clpp_decode (curptr, pNudt->data_buf+pNudt->data_len-curptr, pAudt))
    {
    ACSELOG_ERR0 ("CLPP decode failed");
    return (SD_FAILURE);
    }

  ACSELOG_DEC0 ("A-UNIT-DATA.ind:");
  ACSELOG_DEC0C ("  DST PSEL:");
  ACSELOG_DECH  (pAudt->called_paddr.psel_len, pAudt->called_paddr.psel);
  ACSELOG_DEC0C ("  DST SSEL:");
  ACSELOG_DECH  (pAudt->called_paddr.ssel_len, pAudt->called_paddr.ssel);
  ACSELOG_DEC0C ("  DST TSEL:");
  ACSELOG_DECH  (pAudt->called_paddr.tsel_len, pAudt->called_paddr.tsel);
  ACSELOG_DEC0C ("  DST NSAP:");
  ACSELOG_DECH  (pAudt->called_paddr.nsap_len, pAudt->called_paddr.netAddr.nsap);
  ACSELOG_DEC0C ("  DST MAC:");
  ACSELOG_DECH  (CLNP_MAX_LEN_MAC, pAudt->loc_mac);

  ACSELOG_DEC0C ("  SRC PSEL:");
  ACSELOG_DECH  (pAudt->calling_paddr.psel_len, pAudt->calling_paddr.psel);
  ACSELOG_DEC0C ("  SRC SSEL:");
  ACSELOG_DECH  (pAudt->calling_paddr.ssel_len, pAudt->calling_paddr.ssel);
  ACSELOG_DEC0C ("  SRC TSEL:");
  ACSELOG_DECH  (pAudt->calling_paddr.tsel_len, pAudt->calling_paddr.tsel);
  ACSELOG_DEC0C ("  SRC NSAP:");
  ACSELOG_DECH  (pAudt->calling_paddr.nsap_len, pAudt->calling_paddr.netAddr.nsap);
  ACSELOG_DEC0C ("  SRC MAC:");
  ACSELOG_DECH  (CLNP_MAX_LEN_MAC, pAudt->rem_mac);

  ACSELOG_DEC1C ("  Data length: %u", pAudt->user_info.len);
  ACSELOG_DECH  (pAudt->user_info.len, pAudt->user_info.ptr);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			clpp_decode					*/
/* Input parameters : pointer to message start, length of message,	*/
/* pointer to result buffer.						*/
/************************************************************************/

static ST_RET clpp_decode (ST_UCHAR *msg_ptr, ST_INT len, AUDT_APDU *pAudt)
  {
ASN1_DEC_CTXT ac;
DECODE_TRACK decode_track;	/* temporary decode info		*/

  memset (&ac, 0, sizeof (ASN1_DEC_CTXT));
  ac.usr_info[0] = pAudt;		/* Save in ASN1 Decode Context	*/
  ac.usr_info[1] = &decode_track;	/* Save in ASN1 Decode Context	*/
  ac.asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. 	*/
  ASN1R_TAG_ADD ((&ac), UNI|CONSTR,SEQ_CODE,decode_p_unit_data);

  ac.asn1r_decode_done_fun = asn1r_done_err; /* done function = error fun 	*/
  ac.asn1r_err_fun = NULL;		   /* set up error detected fun 	*/

  /* Clean out old stuff from structure.				*/
  memset (&decode_track, 0, sizeof (decode_track));

/* Call 'asn1r_decode_asn1' to parse the message.                             */

  asn1r_decode_asn1 (&ac,msg_ptr,len);		/* do message decode		*/
  if (ac.asn1r_pdu_dec_err == NO_DECODE_ERR)
    return (SD_SUCCESS); 			/* return 'no error' code       */
  ACSELOG_ERR1 ("Error decoding P-UNIT-DATA = %d", ac.asn1r_pdu_dec_err); 
  return (ac.asn1r_pdu_dec_err);			/* return error code		*/
  }

/************************************************************************/
/* decode_p_unit_data							*/
/************************************************************************/
static ST_VOID decode_p_unit_data (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac, CTX,0,decode_version);
  ASN1R_TAG_ADD (ac, CTX,1,decode_calling_psel);
  ASN1R_TAG_ADD (ac, CTX,2,decode_called_psel);
  ASN1R_TAG_ADD (ac, CTX|CONSTR,4,decode_pc_deflist);
  ASN1R_TAG_ADD (ac, APP|CONSTR,1,
          decode_p_user_data);	/* User-data (must be fully-encoded)	*/
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_p_unit_data_done;
  }


/************************************************************************/
/* decode_version							*/
/************************************************************************/
static ST_VOID decode_version (ASN1_DEC_CTXT *ac)
  {
ST_UCHAR version1_bit_str;
  /* CRITICAL: asn1r_get_bitstr doesn't set unused bits, so init all bits=0*/
  version1_bit_str = 0;
  asn1r_get_bitstr (ac, &version1_bit_str, 8);
  if ((version1_bit_str & 0x80) == 0)	/* Bit 0 of bitstr (high bit) set	*/
    {
    asn1r_set_dec_err (ac, ASN1E_INVALID_BITSTR);	/* Illegal ACSE version		*/
    return;
    }
  ASN1R_TAG_ADD (ac,CTX,1,decode_calling_psel);
  ASN1R_TAG_ADD (ac,CTX,2,decode_called_psel);
  ASN1R_TAG_ADD (ac,CTX|CONSTR,4,decode_pc_deflist);
  ASN1R_TAG_ADD (ac,APP|CONSTR,1,
          decode_p_user_data);	/* User-data (must be fully-encoded)	*/
  }

/************************************************************************/
/* decode_calling_psel							*/
/************************************************************************/
static ST_VOID decode_calling_psel (ASN1_DEC_CTXT *ac)
  {
AUDT_APDU *pAudt = (AUDT_APDU *)ac->usr_info[0];

  if (asn1r_get_octstr (ac, pAudt->calling_paddr.psel))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN); 
    return;                          
    }
  pAudt->calling_paddr.psel_len = ac->asn1r_octetcount;
  ASN1R_TAG_ADD (ac,CTX,2,decode_called_psel);
  ASN1R_TAG_ADD (ac,CTX|CONSTR,4,decode_pc_deflist);
  ASN1R_TAG_ADD (ac,APP|CONSTR,1,
          decode_p_user_data);	/* User-data (must be fully-encoded)	*/
  }

/************************************************************************/
/* decode_called_psel							*/
/************************************************************************/
static ST_VOID decode_called_psel (ASN1_DEC_CTXT *ac)
  {
AUDT_APDU *pAudt = (AUDT_APDU *)ac->usr_info[0];
  if (asn1r_get_octstr (ac,pAudt->called_paddr.psel))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN); 
    return;                          
    }
  pAudt->called_paddr.psel_len = ac->asn1r_octetcount;
  ASN1R_TAG_ADD (ac,CTX|CONSTR,4,decode_pc_deflist);
  ASN1R_TAG_ADD (ac,APP|CONSTR,1,
          decode_p_user_data);	/* User-data (must be fully-encoded)	*/
  }

/************************************************************************/
/* decode_pc_deflist							*/
/************************************************************************/
static ST_VOID decode_pc_deflist (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac,UNI|CONSTR,SEQ_CODE,decode_context);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_pc_deflist_done;
  }

/************************************************************************/
/* decode_context							*/
/************************************************************************/
static ST_VOID decode_context (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_pci);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_context_done;
  }

/************************************************************************/
/* decode_pci								*/
/* This function stores decoded PCI in DECODE_TRACK.			*/
/************************************************************************/
static ST_VOID decode_pci (ASN1_DEC_CTXT *ac)
  {
DECODE_TRACK *decode_track = (DECODE_TRACK *)ac->usr_info[1];
  if (asn1r_get_i8 (ac,&decode_track->pcisave))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN); 
    return;                          
    }
  ASN1R_TAG_ADD (ac,UNI,OBJ_ID_CODE,decode_asn);
  }
/************************************************************************/
/* decode_asn								*/
/************************************************************************/
static ST_VOID decode_asn (ASN1_DEC_CTXT *ac)
  {
MMS_OBJ_ID asn;
DECODE_TRACK *decode_track = (DECODE_TRACK *)ac->usr_info[1];

  if (asn1r_get_objid (ac,asn.comps,&asn.num_comps))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN);
    return;
    }
  if (asn1_objidcmp (&asn, &mms_obj_id) == 0)
    decode_track->pcimms = decode_track->pcisave;
  else if (asn1_objidcmp (&asn, &acse_obj_id) == 0)
    decode_track->pciacse = decode_track->pcisave;
  else
    {	/* Unrecognized Abstract Syntax.  Must be "ACSE" or "ISO MMS".	*/
    asn1r_set_dec_err (ac,ASN1E_INVALID_BITSTR);
    return;
    }

  ASN1R_TAG_ADD (ac,UNI|CONSTR,SEQ_CODE,decode_tsn_list);
  }
/************************************************************************/
/* decode_tsn_list							*/
/************************************************************************/
static ST_VOID decode_tsn_list (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac,UNI,OBJ_ID_CODE,decode_tsn);

  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_tsn_done;
  }

/************************************************************************/
/* decode_tsn								*/
/************************************************************************/
static ST_VOID decode_tsn (ASN1_DEC_CTXT *ac)
  {
MMS_OBJ_ID tsn;

  if (asn1r_get_objid (ac,tsn.comps,&tsn.num_comps))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN);
    return;
    }
  if (asn1_objidcmp (&tsn, &asn1_obj_id) != 0)
    {					/* TSN better be ASN1		*/
    asn1r_set_dec_err (ac,ASN1E_INVALID_BITSTR);
    return;
    }
  /* DEBUG: we don't support multiple TSN so no ASN1R_TAG_ADD here.	*/
  /* Should end up in decode_tsn_done after this.		*/
  }

/************************************************************************/
/* decode_tsn_done							*/
/************************************************************************/
static ST_VOID decode_tsn_done (ASN1_DEC_CTXT *ac)
  {
  return;
  }

/************************************************************************/
/* decode_context_done							*/
/************************************************************************/
static ST_VOID decode_context_done (ASN1_DEC_CTXT *ac)
  {
  /* May be another context.	*/
  ASN1R_TAG_ADD (ac,UNI|CONSTR,SEQ_CODE,decode_context);
  }

/************************************************************************/
/* decode_pc_deflist_done						*/
/************************************************************************/
static ST_VOID decode_pc_deflist_done (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac,APP|CONSTR,1,
          decode_p_user_data);	/* User-data (must be fully-encoded)	*/
  }

/************************************************************************/
/* decode_p_user_data							*/
/************************************************************************/
static ST_VOID decode_p_user_data (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac,UNI|CONSTR,SEQ_CODE,decode_pdv_list);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_p_user_data_done;
  }

/************************************************************************/
/* decode_pdv_list							*/
/************************************************************************/
static ST_VOID decode_pdv_list (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac,UNI,OBJ_ID_CODE,decode_acse_tsn);
  ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_acse_pci);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_pdv_list_done;
  }
/************************************************************************/
/* decode_acse_tsn							*/
/************************************************************************/
static ST_VOID decode_acse_tsn (ASN1_DEC_CTXT *ac)
  {
MMS_OBJ_ID tsn;

  if (asn1r_get_objid (ac,tsn.comps,&tsn.num_comps))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN);
    return;
    }
  if (asn1_objidcmp (&tsn, &asn1_obj_id) != 0)
    {					/* TSN better be ASN1		*/
    asn1r_set_dec_err (ac,ASN1E_INVALID_BITSTR);
    return;
    }
  ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_acse_pci);
  }

/************************************************************************/
/* decode_acse_pci							*/
/************************************************************************/
static ST_VOID decode_acse_pci (ASN1_DEC_CTXT *ac)
  {
ST_INT8 pci;
DECODE_TRACK *decode_track = (DECODE_TRACK *)ac->usr_info[1];

  if (asn1r_get_i8 (ac,&pci))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN); 
    return;                          
    }
  if (pci != decode_track->pciacse)
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BITSTR); 
    return;                          
    }
  ASN1R_TAG_ADD (ac,CTX|CONSTR,0,decode_pdv);	/* Must be single-ASN1-type	*/
  }

/************************************************************************/
/* decode_pdv								*/
/************************************************************************/
static ST_VOID decode_pdv (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac,APP|CONSTR,TAG_AUDT,decode_audt);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_pdv_done;
  }

/************************************************************************/
/* decode_audt								*/
/************************************************************************/
static ST_VOID decode_audt (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_decode_method = ASN1_CLASS_METHOD;	/* Use ASN1_CLASS_METHOD method to decode.		*/
  ac->asn1r_c_id_fun = decode_audt_main;
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_audt_done;
  ac->asn1r_u_id_fun = asn1r_class_err; 	/* no universals to start		*/
  ac->asn1r_a_id_fun = asn1r_class_err; 	/* no applications to start		*/
  ac->asn1r_p_id_fun = asn1r_class_err; 	/* no privates to start 		*/
  }


/************************************************************************/
/* decode_audt_main								*/
/************************************************************************/
static ST_VOID decode_audt_main (ASN1_DEC_CTXT *ac, ST_UINT16 id_code)
  {
ST_UCHAR version1_bit_str;
AUDT_APDU *pAudt = (AUDT_APDU *)ac->usr_info[0];
DECODE_TRACK *decode_track = (DECODE_TRACK *)ac->usr_info[1];

  switch (id_code)
    {
    case AUDT_protocol_version:
      if (ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* constructor not allowed	*/
        return;
        }
      /* CRITICAL: asn1r_get_bitstr doesn't set unused bits, so init all bits=0*/
      version1_bit_str = 0;
      asn1r_get_bitstr (ac,&version1_bit_str, 8);
      if ((version1_bit_str & 0x80) == 0)	/* Bit 0 of bitstr (high bit) set*/
        {
        asn1r_set_dec_err (ac,ASN1E_INVALID_BITSTR);	/* Illegal ACSE version		*/
        return;
        }
      break;

    case AUDT_ASO_context_name:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->ASO_context_name_pres = SD_TRUE;
      decode_track->my_obj_id = &pAudt->ASO_context_name;
      ASN1R_TAG_ADD (ac,UNI,OBJ_ID_CODE,decode_obj_id);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_CALLED_AP_title:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->called_ae_title.AP_title_pres = SD_TRUE;
      decode_track->my_obj_id = &pAudt->called_ae_title.AP_title;
      ASN1R_TAG_ADD (ac,UNI,OBJ_ID_CODE,decode_obj_id);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_CALLED_AE_qual:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->called_ae_title.AE_qual_pres = SD_TRUE;
      decode_track->my_i32 = &pAudt->called_ae_title.AE_qual;
      ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_i32);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_CALLED_AP_inv_id:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->called_ae_title.AP_inv_id_pres = SD_TRUE;
      decode_track->my_i32 = &pAudt->called_ae_title.AP_inv_id;
      ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_i32);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_CALLED_AE_inv_id:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->called_ae_title.AE_inv_id_pres = SD_TRUE;
      decode_track->my_i32 = &pAudt->called_ae_title.AE_inv_id;
      ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_i32);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_CALLING_AP_title:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->calling_ae_title.AP_title_pres = SD_TRUE;
      decode_track->my_obj_id = &pAudt->calling_ae_title.AP_title;
      ASN1R_TAG_ADD (ac,UNI,OBJ_ID_CODE,decode_obj_id);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_CALLING_AE_qual:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->calling_ae_title.AE_qual_pres = SD_TRUE;
      decode_track->my_i32 = &pAudt->calling_ae_title.AE_qual;
      ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_i32);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_CALLING_AP_inv_id:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->calling_ae_title.AP_inv_id_pres = SD_TRUE;
      decode_track->my_i32 = &pAudt->calling_ae_title.AP_inv_id;
      ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_i32);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_CALLING_AE_inv_id:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      pAudt->calling_ae_title.AE_inv_id_pres = SD_TRUE;
      decode_track->my_i32 = &pAudt->calling_ae_title.AE_inv_id;
      ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_i32);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_constr_done;
      break;

    case AUDT_user_info:
      if (!ac->asn1r_constr_elmnt)
        {
        asn1r_set_dec_err (ac,ASN1E_UNEXPECTED_FORM);	/* Must be constructor	*/
        return;
        }
      ac->asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. */
      ASN1R_TAG_ADD (ac,UNI|CONSTR,EXTERN_CODE,decode_external);
      ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_audt_user_info_done;
      break;
      
    default:				/* Don't know how to decode.	*/
      /* DEBUG: this is a very kluged up way to handle this but it works.*/
      if (ac->asn1r_constr_elmnt)
        {
        ac->asn1r_constr_elmnt = 0;	/* Make ASN.1 treat it like primitive.	*/
        --ac->asn1r_msg_level;		/* Already incremented for constructor	*/
        }			/* so decrement to act like primitive.	*/
      ac->asn1r_field_ptr += ac->asn1r_elmnt_len;
      break;
    }	/* End "switch (id_code)"	*/
  }

/************************************************************************/
/* decode_external							*/
/************************************************************************/
static ST_VOID decode_external (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac,UNI,OBJ_ID_CODE,decode_mms_tsn);
  ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_mms_pci);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_external_done;
  }

/************************************************************************/
/* decode_mms_tsn							*/
/************************************************************************/
static ST_VOID decode_mms_tsn (ASN1_DEC_CTXT *ac)
  {
MMS_OBJ_ID tsn;

  if (asn1r_get_objid (ac,tsn.comps,&tsn.num_comps))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN);
    return;
    }
  if (asn1_objidcmp (&tsn, &asn1_obj_id) != 0)
    {					/* TSN better be ASN1		*/
    asn1r_set_dec_err (ac,ASN1E_INVALID_BITSTR);
    return;
    }
  ASN1R_TAG_ADD (ac,UNI,INT_CODE,decode_mms_pci);
  }

/************************************************************************/
/* decode_mms_pci							*/
/************************************************************************/
static ST_VOID decode_mms_pci (ASN1_DEC_CTXT *ac)
  {
ST_INT8 pci;
DECODE_TRACK *decode_track = (DECODE_TRACK *)ac->usr_info[1];

  if (asn1r_get_i8 (ac,&pci))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN); 
    return;                          
    }
  if (pci != decode_track->pcimms)
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BITSTR); 
    return;                          
    }
  ASN1R_TAG_ADD (ac,CTX|CONSTR,0,decode_mms_data);	/* Must be single-ASN1-type	*/
  }

/************************************************************************/
/* decode_mms_data							*/
/************************************************************************/
static ST_VOID decode_mms_data (ASN1_DEC_CTXT *ac)
  {
AUDT_APDU *pAudt = (AUDT_APDU *)ac->usr_info[0];
  pAudt->user_info.ptr = ac->asn1r_field_ptr;
  pAudt->user_info.len = ac->asn1r_elmnt_len;
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = decode_mms_data_done;
  asn1r_parse_next (ac,decode_constr_done);	/* First thing in MMS PDU is always constr*/
  }

/************************************************************************/
/* decode_constr_done							*/
/************************************************************************/
static ST_VOID decode_constr_done (ASN1_DEC_CTXT *ac)
  {
  }

/************************************************************************/
/* decode_mms_data_done							*/
/************************************************************************/
static ST_VOID decode_mms_data_done (ASN1_DEC_CTXT *ac)
  {
  }

/************************************************************************/
/* decode_external_done							*/
/************************************************************************/
static ST_VOID decode_external_done (ASN1_DEC_CTXT *ac)
  {
  }

/************************************************************************/
/* decode_audt_user_info_done						*/
/************************************************************************/
static ST_VOID decode_audt_user_info_done (ASN1_DEC_CTXT *ac)
  {
  }

/************************************************************************/
/* decode_audt_done							*/
/************************************************************************/
static ST_VOID decode_audt_done (ASN1_DEC_CTXT *ac)
  {
  }

/************************************************************************/
/* decode_pdv_done							*/
/************************************************************************/
static ST_VOID decode_pdv_done (ASN1_DEC_CTXT *ac)
  {
  }
/************************************************************************/
/* decode_pdv_list_done							*/
/************************************************************************/
static ST_VOID decode_pdv_list_done (ASN1_DEC_CTXT *ac)
  {
  }

/************************************************************************/
/* decode_p_user_data_done						*/
/************************************************************************/
static ST_VOID decode_p_user_data_done (ASN1_DEC_CTXT *ac)
  {
  }

/************************************************************************/
/* decode_p_unit_data_done						*/
/************************************************************************/
static ST_VOID decode_p_unit_data_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_decode_done_fun = decode_clpp_done;
  }

/************************************************************************/
/* decode_clpp_done							*/
/************************************************************************/
static ST_VOID decode_clpp_done (ASN1_DEC_CTXT *ac)
  {
  return;
  }

/************************************************************************/
/* decode_obj_id								*/
/************************************************************************/
static ST_VOID decode_obj_id (ASN1_DEC_CTXT *ac)
  {
DECODE_TRACK *decode_track = (DECODE_TRACK *)ac->usr_info[1];
  if (asn1r_get_objid (ac,decode_track->my_obj_id->comps,&decode_track->my_obj_id->num_comps))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN); 
    return;                          
    }
  /* Go back to ASN1_CLASS_METHOD mode. "ac->asn1r_c_id_fun" should still = "decode_????_main"*/
  ac->asn1r_decode_method = ASN1_CLASS_METHOD;
  return;
  }
  
/************************************************************************/
/* decode_i32								*/
/************************************************************************/
static ST_VOID decode_i32 (ASN1_DEC_CTXT *ac)
  {
DECODE_TRACK *decode_track = (DECODE_TRACK *)ac->usr_info[1];
  if (asn1r_get_i32 (ac,decode_track->my_i32))
    {
    asn1r_set_dec_err (ac,ASN1E_INVALID_BOOLEAN); 
    return;                          
    }
  /* Go back to ASN1_CLASS_METHOD mode. "ac->asn1r_c_id_fun" should still = "decode_????_main"*/
  ac->asn1r_decode_method = ASN1_CLASS_METHOD;
  return;
  }
  
/************************************************************************/
/*			clnp_get_osi_addr				*/
/* This function takes the NSAP in "buf" in Marben format,		*/
/* and puts it in "pres_addr" in our format.				*/
/************************************************************************/

static ST_VOID clnp_get_osi_addr (ST_UCHAR *buf, PRES_ADDR *pres_addr)
  {
ST_UINT j = 0;

  pres_addr->nsap_len = buf[j++];
  memcpy (pres_addr->netAddr.nsap, &buf[j], pres_addr->nsap_len);
  return;
  }
