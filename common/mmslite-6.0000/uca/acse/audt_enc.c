/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995-2010, All Rights Reserved.			        	*/
/*									*/
/* MODULE NAME : audt_enc.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	A-Unit-Data encode						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 06/16/10  JRB	   Del del clpp_init.. clpp_term.. audt_set_msgsize*/
/*			   Repl audt_buf, audt_msgsize globals w/ locals*/
/* 01/24/05  JRB    33     Move acse_debug_sel & "*_logstr" to lean_var.c*/
/* 10/29/02  JRB    32     a_unit_data_req check remote tp_type.	*/
/* 06/20/02  JRB    31     Chg to use new netAddr union in PRES_ADDR.	*/
/* 03/04/02  JRB    30     Del unneeded <process.h>. Fix warnings.	*/
/* 01/10/02  JRB    29     Del all binding, don't pass bind id to	*/
/*			   a_unit_data_req but set audt_apdu->calling_paddr.*/
/*			   Chg ASN1_CTXT to ASN1_ENC_CTXT & skip memset.*/
/* 03/12/01  JRB    28     Use new SMEM allocation functions.		*/
/* 07/17/00  JRB    27     clpp_term..: free the global encode buffer.	*/
/* 12/10/99  JRB    26     Chk for encode_overrun.			*/
/* 07/30/99  JRB    25     Support "multithreading" using ASN1R.	*/
/* 01/28/99  JRB    24     Set cltp_decode_nsdu_fun funct ptr if needed.*/
/* 12/07/98  JRB    23	   Ignore NSAP on clpp_bind. User never sets it,*/
/*			   and LEAN_T never uses it (Marben code used it).*/
/*			   Only allow 1 call to clpp_bind.		*/
/*			   Del audt_debug_sel. Use acse_debug_sel.	*/
/* 10/08/98  MDE    22     Migrated to updated SLOG interface		*/
/* 08/13/98  JRB    21     Lint cleanup.				*/
/* 08/03/98  JRB    20     Allow NULL PSEL, SSEL, TSEL.			*/
/* 07/23/98  JRB    19     Delete Marben support.			*/
/* 07/10/98  JRB    18     Del *_obj_id, use acse2var.c			*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 01/09/97  MDE    17     Allow user to specify destination MAC	*/
/* 12/16/96  JRB    16     Chg acse_debug_sel to audt_debug_sel so it	*/
/*			   doesn't conflict with COACSE.		*/
/*			   Use audt_debug_sel via "AUDTLOG_*" macros.	*/
/* 12/02/96  JRB    15     Just set rem_mac_valid = SD_FALSE for LEAN_T	*/
/*			   in a_unit_data_req.				*/
/* 11/18/96  JRB    14     Del acse_dummyfun call. Funct is gone now.	*/
/* 11/15/96  JRB    13     Don't initialize globals.			*/
/*			   Move acse_debug_sel global here.		*/
/* 10/04/96  JRB    09     Add LEAN_T support.				*/
/* 05/20/96  JRB    08     Pass NSAP to stack on bind & add RG_RFAD	*/
/*			   flag. a_unit_data_req set nudfrna[0] = 0 so	*/
/*			   stack uses NSAP that was passed on bind.	*/
/* 05/17/96  JRB    07     Added <process.h> for getpid proto.		*/
/* 04/24/96  JRB    06     Use QUEUE_NAME & CLIENT_QUEUE_NAME for	*/
/*			   mailbox names (see portdef.h).		*/
/* 04/16/96  JRB    05     Add clpp_read_event_sem = clpp_loc.usr_sem	*/
/* 03/11/96  JRB    04     Add audt_msgsize & use for audt_buf.		*/
/* 03/07/96  JRB    03     Chg name to audt_dummyfuncall for link to COT7.*/
/* 10/30/95  JRB    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "asn1r.h"	/* MUST be BEFORE acse2.h so overrides asn1defs.h*/
#include "acse2.h"
#include "clnp_usr.h"

#ifdef TP4_ENABLED
#include "tp4api.h"		/* for cltp_decode_nsdu_fun funct ptr	*/
#endif
/************************************************************************/
/* Global Variables							*/
/************************************************************************/
#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif
static ST_CHAR pcimms = 1;	/* use fixed value	*/
static ST_CHAR pciacse = 3;	/* use fixed value	*/

/************************************************************************/
/* Local function prototypes						*/
/************************************************************************/
static ST_VOID clnp_bld_osi_addr (ST_UCHAR *buf, PRES_ADDR *pres_addr);
static ST_UCHAR *a_unit_data_enc (AUDT_APDU *audt_apdu,
	ST_UCHAR *audt_buf,	/* buffer in which to encode	*/
	ST_UINT audt_msgsize);	/* size of encode buffer	*/

/************************************************************************/
/*			a_unit_data_req					*/
/* This function is called by the user to send data			*/
/* Caller must set "audt_apdu->calling_paddr" (previous version used	*/
/* address from bind).							*/
/************************************************************************/
ST_RET a_unit_data_req (
	AUDT_APDU *audt_apdu)
  {
N_UNITDATA *n_unitdata;
ST_INT status;
ST_UCHAR *curptr;	/* CRITICAL: ST_UCHAR so constants not sign extended	*/
ST_UINT pdulen;
ST_UCHAR audt_buf [1024];	/* big enough for any IEC 61850 GSSE msg*/
ST_UINT audt_msgsize = sizeof (audt_buf);

  if (audt_apdu->called_paddr.tp_type != TP_TYPE_TP4)	/* must be TP4 (need NSAP)	*/
    {
    ACSELOG_ERR0 ("A-UNIT_DATA.req: Remote addr Transport type illegal");
    return (E_ACSE_ENC_ERR);
    }
  ACSELOG_ENC0 ("A-UNIT-DATA.req:");
  ACSELOG_ENC0C ("  DST PSEL:");
  ACSELOG_ENCH  (audt_apdu->called_paddr.psel_len, audt_apdu->called_paddr.psel);
  ACSELOG_ENC0C ("  DST SSEL:");
  ACSELOG_ENCH  (audt_apdu->called_paddr.ssel_len, audt_apdu->called_paddr.ssel);
  ACSELOG_ENC0C ("  DST TSEL:");
  ACSELOG_ENCH  (audt_apdu->called_paddr.tsel_len, audt_apdu->called_paddr.tsel);
  ACSELOG_ENC0C ("  DST NSAP:");
  ACSELOG_ENCH  (audt_apdu->called_paddr.nsap_len, audt_apdu->called_paddr.netAddr.nsap);
  ACSELOG_ENC0C ("  DST MAC:");
  if (audt_apdu->rem_mac_valid)
    {
    ACSELOG_ENCH  (CLNP_MAX_LEN_MAC, audt_apdu->rem_mac);
    }
  else
    {
    ACSELOG_ENC0C ("    UNKNOWN");
    }

  ACSELOG_ENC0C ("  SRC PSEL:");
  ACSELOG_ENCH  (audt_apdu->calling_paddr.psel_len, audt_apdu->calling_paddr.psel);
  ACSELOG_ENC0C ("  SRC SSEL:");
  ACSELOG_ENCH  (audt_apdu->calling_paddr.ssel_len, audt_apdu->calling_paddr.ssel);
  ACSELOG_ENC0C ("  SRC TSEL:");
  ACSELOG_ENCH  (audt_apdu->calling_paddr.tsel_len, audt_apdu->calling_paddr.tsel);

  ACSELOG_ENC1C ("  Data length: %u", audt_apdu->user_info.len);
  ACSELOG_ENCH  (audt_apdu->user_info.len, audt_apdu->user_info.ptr);

  /* Encode everything up to Network layer.	*/
  curptr = a_unit_data_enc (audt_apdu, audt_buf, audt_msgsize);

  if (curptr == NULL)
    return (E_ACSE_ENC_ERR);

  /* Compute the PDU length	*/
  pdulen = (unsigned int) (audt_buf+audt_msgsize-curptr);

  /* a_unit_data_enc should not allow overflow, but just in case, chk len.*/
  if (pdulen > audt_msgsize)
    return (E_ACSE_BUFFER_OVERFLOW);
 
  /* NOTE: User must pass remote MAC in audt_apdu because Lean-T	*/
  /*       doesn't keep NSAP/MAC tables.				*/
  if ((n_unitdata = clnp_alloc (pdulen)) == NULL)
    {
    ACSELOG_ERR0 ("clnp_alloc () returned NULL!");
    return (E_ACSE_MEMORY_ALLOC);
    }
  n_unitdata->data_len = pdulen;
  memcpy (n_unitdata->data_buf, curptr, pdulen);
  clnp_bld_osi_addr (n_unitdata->rem_nsap, &audt_apdu->called_paddr);
#if defined (MCGW)
  *(ST_UINT16 *) n_unitdata->rem_mac = audt_apdu->called_paddr.adlcAddr;
  n_unitdata->rem_mac_valid = SD_TRUE;
#else		/* !defined (MCGW)	*/

  if (audt_apdu->rem_mac_valid)
    {  
    n_unitdata->rem_mac_valid = SD_TRUE;
    memcpy (n_unitdata->rem_mac, audt_apdu->rem_mac, CLNP_MAX_LEN_MAC);
    }
  else
    n_unitdata->rem_mac_valid = SD_FALSE;	/* Let ES-IS figure it out.	*/
#endif		/* !defined (MCGW)	*/
  if ((status = clnp_write (n_unitdata)))	/* NOTE: clnp frees n_unitdata	*/
    {
    ACSELOG_ERR1 ("clnp_write () failed, retcode = %d.", status);
    return (E_ACSE_SEND_ERR);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			clnp_bld_osi_addr				*/
/* This function takes the NSAP in "pres_addr" in our format,		*/
/* and puts it in "buf" in Marben format.				*/
/************************************************************************/

static ST_VOID clnp_bld_osi_addr (ST_UCHAR *buf, PRES_ADDR *pres_addr)
  {
ST_UINT j = 0;

  buf[j++] = (ST_UCHAR) pres_addr->nsap_len;
  memcpy (&buf[j], pres_addr->netAddr.nsap, pres_addr->nsap_len);
  }

/************************************************************************/
/*			a_unit_data_enc					*/
/* RETURNS: ptr to encoded data OR					*/
/*	    NULL if can't fit in buffer (i.e. audt_msgsize).		*/
/************************************************************************/
static ST_UCHAR *a_unit_data_enc (AUDT_APDU *audt_apdu,
	ST_UCHAR *audt_buf,	/* buffer in which to encode	*/
	ST_UINT audt_msgsize)	/* size of encode buffer	*/
  {
ST_UCHAR *curptr;	/* CRITICAL: ST_UCHAR so constants not sign extended	*/
AE_TITLE *ae_title;
ST_UCHAR version1_bit_str = 0x80;	/* Bit 0 of bitstr (high bit) set	*/
ST_UCHAR s_header_len;			/* Session header length	*/
ST_UCHAR t_header_len;			/* Transport header length	*/
ASN1_ENC_CTXT ac;

  /* Begin encoding request.						*/
  asn1r_strt_asn1_bld (&ac,audt_buf, audt_msgsize);

  asn1r_strt_constr (&ac);			/* start P-Unit-Data SEQUENCE	*/
  asn1r_strt_constr (&ac);		/* start P-User-data (fully-encoded)	*/
  asn1r_strt_constr (&ac);			/* start SEQUENCE	*/
  asn1r_strt_constr (&ac);			/* start single-ASN1-type	*/

  /* Write AUDT APDU.							*/
  asn1r_strt_constr (&ac);			/* Start AUDT APDU		*/
  if (audt_apdu->user_info.len)
    {
    asn1r_strt_constr (&ac);			/* start User Info		*/
    asn1r_strt_constr (&ac);			/* start EXTERNAL		*/
    asn1r_strt_constr (&ac);			/* start single-ASN1-type	*/
    asn1r_wr_octstr (&ac,audt_apdu->user_info.ptr, audt_apdu->user_info.len);
    asn1r_fin_constr (&ac,0,CTX,DEF);		/* finish single-ASN1-type	*/
    asn1r_wr_i8 (&ac,pcimms);			/* PCI for MMS			*/
    asn1r_fin_prim (&ac,INT_CODE,UNI);
    asn1r_wr_objid (&ac,asn1_obj_id.comps,
              asn1_obj_id.num_comps);	/* Transfer syntax name (i.e. ASN1)*/
    asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
    asn1r_fin_constr (&ac,EXTERN_CODE,UNI,DEF);	/* finish EXTERNAL		*/
    asn1r_fin_constr (&ac,AUDT_user_info,CTX,DEF);	/* finish User Info		*/
    }

  /* Write CALLING AE Title (i.e. AP_title, AE_qual, AP_inv_id, AE_inv_id).*/
  ae_title = &audt_apdu->calling_ae_title;

  if (ae_title->AE_inv_id_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_i32 (&ac,ae_title->AE_inv_id);
    asn1r_fin_prim (&ac,INT_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_CALLING_AE_inv_id,CTX,DEF);
    }
    
  if (ae_title->AP_inv_id_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_i32 (&ac,ae_title->AP_inv_id);
    asn1r_fin_prim (&ac,INT_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_CALLING_AP_inv_id,CTX,DEF);
    }
    
  if (ae_title->AE_qual_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_i32 (&ac,ae_title->AE_qual);
    asn1r_fin_prim (&ac,INT_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_CALLING_AE_qual,CTX,DEF);
    }
    
  if (ae_title->AP_title_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_objid (&ac,ae_title->AP_title.comps, ae_title->AP_title.num_comps);
    asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_CALLING_AP_title,CTX,DEF);
    }


  /* Write CALLED AE Title (i.e. AP_title, AE_qual, AP_inv_id, AE_inv_id).*/
  ae_title = &audt_apdu->called_ae_title;

  if (ae_title->AE_inv_id_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_i32 (&ac,ae_title->AE_inv_id);
    asn1r_fin_prim (&ac,INT_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_CALLED_AE_inv_id,CTX,DEF);
    }
    
  if (ae_title->AP_inv_id_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_i32 (&ac,ae_title->AP_inv_id);
    asn1r_fin_prim (&ac,INT_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_CALLED_AP_inv_id,CTX,DEF);
    }
    
  if (ae_title->AE_qual_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_i32 (&ac,ae_title->AE_qual);
    asn1r_fin_prim (&ac,INT_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_CALLED_AE_qual,CTX,DEF);
    }
    
  if (ae_title->AP_title_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_objid (&ac,ae_title->AP_title.comps, ae_title->AP_title.num_comps);
    asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_CALLED_AP_title,CTX,DEF);
    }

  /* Write ASO Context Name.						*/
  if (audt_apdu->ASO_context_name_pres)
    {
    asn1r_strt_constr (&ac);
    asn1r_wr_objid (&ac,audt_apdu->ASO_context_name.comps, audt_apdu->ASO_context_name.num_comps);
    asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
    asn1r_fin_constr (&ac,AUDT_ASO_context_name,CTX,DEF);
    }

  /* Write protocol_version.						*/
  asn1r_wr_bitstr (&ac,&version1_bit_str, 1);
  asn1r_fin_prim (&ac,AUDT_protocol_version, CTX);
  asn1r_fin_constr (&ac,TAG_AUDT,APP,DEF);	/* Finish AUDT APDU		*/

  asn1r_fin_constr (&ac,0,CTX,DEF);		/* finish single-ASN1-type const*/
  asn1r_wr_i8 (&ac,pciacse);			/* PCI for ACSE			*/
  asn1r_fin_prim (&ac,INT_CODE,UNI);
  asn1r_wr_objid (&ac,asn1_obj_id.comps,
            asn1_obj_id.num_comps);	/* Transfer syntax name (i.e. ASN1)*/
  asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
  asn1r_fin_constr (&ac,SEQ_CODE,UNI,DEF);	/* finish SEQUENCE		*/
  asn1r_fin_constr (&ac,1,APP,DEF);	/* finish P-User-data (fully-encoded)	*/

  asn1r_strt_constr (&ac);			/* start P-Context-list SEQUENCE*/
  asn1r_strt_constr (&ac);			/* start ACSE Context SEQUENCE	*/
  asn1r_strt_constr (&ac);			/* start TSN SEQUENCE		*/
  asn1r_wr_objid (&ac,asn1_obj_id.comps,
            asn1_obj_id.num_comps);	/* Transfer syntax name (i.e. ASN1)*/
  asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
  asn1r_fin_constr (&ac,SEQ_CODE,UNI,DEF);	/* finish TSN SEQUENCE		*/

  asn1r_wr_objid (&ac,acse_obj_id.comps,
            acse_obj_id.num_comps);	/* Abstract syntax name (i.e. ACSE)*/
  asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
  asn1r_wr_i8 (&ac,pciacse);			/* PCI for ACSE			*/
  asn1r_fin_prim (&ac,INT_CODE,UNI);
  asn1r_fin_constr (&ac,SEQ_CODE,UNI,DEF);	/* finish ACSE Context SEQUENCE	*/

  asn1r_strt_constr (&ac);			/* start MMS Context SEQUENCE	*/
  asn1r_strt_constr (&ac);			/* start TSN SEQUENCE		*/
  asn1r_wr_objid (&ac,asn1_obj_id.comps,
            asn1_obj_id.num_comps);	/* Transfer syntax name (i.e. ASN1)*/
  asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
  asn1r_fin_constr (&ac,SEQ_CODE,UNI,DEF);	/* finish TSN SEQUENCE		*/

  asn1r_wr_objid (&ac,mms_obj_id.comps,
            mms_obj_id.num_comps);	/* Abstract syntax name (i.e. MMS)*/
  asn1r_fin_prim (&ac,OBJ_ID_CODE,UNI);
  asn1r_wr_i8 (&ac,pcimms);			/* PCI for MMS			*/
  asn1r_fin_prim (&ac,INT_CODE,UNI);
  asn1r_fin_constr (&ac,SEQ_CODE,UNI,DEF);	/* finish MMS Context SEQUENCE	*/
  asn1r_fin_constr (&ac,4,CTX,DEF);		/* finish P-Context-list SEQUENCE*/


  if (audt_apdu->called_paddr.psel_len)
    {
    asn1r_wr_octstr (&ac,audt_apdu->called_paddr.psel, audt_apdu->called_paddr.psel_len);
    asn1r_fin_prim (&ac,2,CTX);
    }
  if (audt_apdu->calling_paddr.psel_len)
    {
    asn1r_wr_octstr (&ac,audt_apdu->calling_paddr.psel, audt_apdu->calling_paddr.psel_len);
    asn1r_fin_prim (&ac,1,CTX);
    }
  asn1r_fin_constr (&ac,SEQ_CODE,UNI,DEF);	/* finish P-Unit-Data SEQUENCE	*/
  curptr = ac.asn1r_field_ptr + 1;
  /* Pointing to beginning of Session User Information Field now.	*/

  /* Make sure there is still room for rest of encoding.	*/
  if (ac.asn1r_encode_overrun  ||
      curptr
      - audt_apdu->called_paddr.ssel_len
      - audt_apdu->calling_paddr.ssel_len
      - audt_apdu->called_paddr.tsel_len
      - audt_apdu->calling_paddr.tsel_len
      - 12
      < audt_buf)
    return (NULL);		/* Would Overflow, so give up now	*/

  s_header_len = 0;
  /* Called Session Selector	*/
  if (audt_apdu->called_paddr.ssel_len)
    {
    curptr -= audt_apdu->called_paddr.ssel_len;
    memcpy (curptr, audt_apdu->called_paddr.ssel, audt_apdu->called_paddr.ssel_len);
    *(--curptr) = (ST_UCHAR) audt_apdu->called_paddr.ssel_len;
    *(--curptr) = 52;			/* PI = Called Session Sel	*/
    s_header_len += (audt_apdu->called_paddr.ssel_len + 2);
    }
  /* Calling Session Selector	*/
  if (audt_apdu->calling_paddr.ssel_len)
    {
    curptr -= audt_apdu->calling_paddr.ssel_len;
    memcpy (curptr, audt_apdu->calling_paddr.ssel, audt_apdu->calling_paddr.ssel_len);
    *(--curptr) = (ST_UCHAR) audt_apdu->calling_paddr.ssel_len;
    *(--curptr) = 51;			/* PI = Calling Session Sel	*/
    s_header_len += (audt_apdu->calling_paddr.ssel_len + 2);
    }
  /* NOTE: this len not same as Connection Oriented Session.	*/
  *(--curptr) = s_header_len;
  *(--curptr) = 64;			/* SI = Session Unit Data PDU	*/
  
  t_header_len = 0;
  /* Called Transport Selector	*/
  if (audt_apdu->called_paddr.tsel_len)
    {
    curptr -= audt_apdu->called_paddr.tsel_len;
    memcpy (curptr, audt_apdu->called_paddr.tsel, audt_apdu->called_paddr.tsel_len);
    *(--curptr) = (ST_UCHAR) audt_apdu->called_paddr.tsel_len;
    *(--curptr) = 0xC2;			/* PI = Called Transport Sel	*/
    t_header_len += (audt_apdu->called_paddr.tsel_len + 2);
    }
  /* Calling Transport Selector	*/
  if (audt_apdu->calling_paddr.tsel_len)
    {
    curptr -= audt_apdu->calling_paddr.tsel_len;
    memcpy (curptr, audt_apdu->calling_paddr.tsel, audt_apdu->calling_paddr.tsel_len);
    *(--curptr) = (ST_UCHAR) audt_apdu->calling_paddr.tsel_len;
    *(--curptr) = 0xC1;			/* PI = Calling Transport Sel	*/
    t_header_len += (audt_apdu->calling_paddr.tsel_len + 2);
    }
  *(--curptr) = 0x40;			/* Transport Unit Data PDU	*/
  t_header_len++;
  *(--curptr) = t_header_len;		/* Transport header len.	*/
  return (curptr);
  }

