/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_ced.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common primitive encode/decode functions.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 06/11/10  MDE           set data_pres == SD_FALSE to fix dup free    */  
/* 02/24/10  JRB	   Del _mmsdec_rslt,  use aCtx->mmsdec_rslt	*/
/*			   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/*			   Del _mms_dec_info_pres, use aCtx->mmsdec_rslt->data_pres*/
/* 01/22/02  JRB     05    Add _ms_fin_pdu & _ms_fin_pdu_log (these	*/
/*			   replace functionality of _ms_fin_pdu_debug).	*/
/*			   Use new "asn1r_buf_end" instead of globals.	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 04/28/00  JRB     03    Lint cleanup					*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     01    Changes to decode buffer allocation scheme   */
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"

/************************************************************************/
/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif


/************************************************************************/
/* COMPANION STANDARD INFO DECODE FUNCTIONS 				*/
/************************************************************************/

#ifdef CS_SUPPORT
static ST_VOID mms_cs_start (ASN1_DEC_CTXT *aCtx);
static ST_VOID mms_cs_cstr_done (ASN1_DEC_CTXT *aCtx);
#endif

/************************************************************************/
/************************************************************************/
/*			 _ms_set_cs_check 					*/
/* This function is used to check for CS info after rest of PDU has 	*/
/* been decoded.							*/
/************************************************************************/

ST_VOID _ms_set_cs_check (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_decode_done_fun = _mms_dec_done_ok;
  _ms_set_cs_check2 (aCtx);
  }

/************************************************************************/
/*			 _ms_set_cs_check2 				*/
/* use this function to avoid setting the aCtx->asn1r_decode_done_fun	*/
/************************************************************************/

ST_VOID _ms_set_cs_check2 (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;  		   /* need to use TAG method	*/

#ifdef CS_SUPPORT
#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)	   /* IS only			*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,79,mms_cs_start);  /* need to check for CS	*/
#else
  if (mmsl_version)			   /* IS only			*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,79,mms_cs_start);  /* need to check for CS	*/
#endif
#endif
					   /* in case this is cstr done	*/
  if (!aCtx->asn1r_msg_level)
    aCtx->asn1r_c_done_fun[1] = NULL;		   /* OK for all to be complete	*/
  else				/* need to go through all upper levels	*/
    aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = _ms_set_cs_check2;   
  }


#ifdef CS_SUPPORT
/************************************************************************/
/*			 mms_cs_start 					*/
/************************************************************************/

static ST_VOID mms_cs_start (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->mmsdec_rslt->cs.cs_ptr = aCtx->asn1r_field_ptr;	/* start of CS info*/
  asn1r_set_all_cstr_done (aCtx);		   /* clear up all cstr done's	*/
  asn1r_parse_cstr_contents (aCtx, mms_cs_cstr_done);  /* verify ASN.1		*/
  }

/************************************************************************/
/*			 mms_cs_cstr_done 				*/
/************************************************************************/

static ST_VOID mms_cs_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->mmsdec_rslt->cs.cs_pres = SD_TRUE;	/* CS info was OK	*/
  aCtx->mmsdec_rslt->cs.cs_len = (ST_INT) (aCtx->asn1r_field_ptr - aCtx->mmsdec_rslt->cs.cs_ptr);
  aCtx->asn1r_decode_done = SD_TRUE;
  }
#endif

/************************************************************************/
/************************************************************************/
/* EXTERNAL decode functions						*/
/************************************************************************/
/************************************************************************/
/*			   _ms_chk_for_extern				*/
/************************************************************************/
/* This function is called when adding tags when an EXTERNAL may be	*/
/* encountered. Pass in the destination and the constructor done fun	*/
/* Currently do not parse EXTERNAL itself, allow user to do this.	*/

static ST_VOID (*ext_done_fun)(ASN1_DEC_CTXT *aCtx);

static ST_VOID ext_seq_start (ASN1_DEC_CTXT *aCtx);

static ST_INT *ext_len_ptr;		/* save in case EXTERNAL found	*/
static ST_UCHAR **ext_buf_ptr_ptr;

ST_VOID _ms_chk_for_extern (ASN1_DEC_CTXT *aCtx, ST_INT *len_ptr, ST_UCHAR **buf_ptr_ptr, 
		     ST_VOID (*done_fun)(ASN1_DEC_CTXT *aCtx))
  {
  ASN1R_TAG_ADD (aCtx, UNI|CONSTR,EXTERN_CODE,ext_seq_start); 	/* get the EXT seq*/
  ext_done_fun = done_fun;
  ext_len_ptr = len_ptr;
  ext_buf_ptr_ptr = buf_ptr_ptr;
  }

/************************************************************************/
/*			   ext_seq_start				*/
/* External encountered							*/
/************************************************************************/

static ST_VOID ext_seq_start (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ext_seq_start");

/* save pointer and length of the external data element			*/
  *ext_len_ptr = (int) (aCtx->asn1r_elmnt_len + (aCtx->asn1r_field_ptr - aCtx->asn1r_field_start));
  *ext_buf_ptr_ptr = aCtx->asn1r_field_start;

  asn1r_parse_cstr_contents (aCtx, ext_done_fun);
  }


/************************************************************************/
/************************************************************************/
/* GENERAL DECODE FUNCTIONS						*/
/************************************************************************/
/*			mms_req_not_supp				*/
/************************************************************************/

ST_VOID mms_req_not_supp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("mms_req_not_supp");

  if (aCtx->mmsdec_rslt->op == MMSOP_INFO_RPT ||
      aCtx->mmsdec_rslt->op == MMSOP_USTATUS ||
      aCtx->mmsdec_rslt->op == MMSOP_EVENT_NOT)
    asn1r_set_dec_err (aCtx, UNCONF_UNREC_SERV);
  else
    asn1r_set_dec_err (aCtx, REQ_UNREC_SERV);
  }

/************************************************************************/
/*			mms_rsp_not_supp				*/
/************************************************************************/

ST_VOID mms_rsp_not_supp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("mms_rsp_not_supp ");

  asn1r_set_dec_err (aCtx, RESP_UNREC_SERV);
  }

/************************************************************************/
/************************************************************************/
/*			_mms_null_pdu_dec				*/
/* This function is called from MMSDEC when the opcode for a NULL PDU 	*/
/* is being decoded.							*/
/************************************************************************/

ST_VOID _mms_null_pdu_dec (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Decoding Null PDU (examine opcode for service)");

  _ms_set_cs_check (aCtx);			/* check to see of CS info pres	*/
  }

/************************************************************************/
/*			_mms_dec_done_ok					*/
/* general decode done function, used for most primitive decodes	*/
/************************************************************************/

ST_VOID _mms_dec_done_ok (ASN1_DEC_CTXT *aCtx)
  {
  /* Do nothing here ("aCtx->mmsdec_rslt->data_pres" &			*/
  /* "aCtx->mmsdec_rslt->data_ptr" were set during decode).		*/
  }

/************************************************************************/
/*			_mms_dec_buf_free				*/
/* error decoding the request: free allocated storage.			*/
/************************************************************************/

ST_VOID _mms_dec_buf_free (ASN1_DEC_CTXT *aCtx, ST_RET err)
  {
  (*m_free_os_fun)(aCtx->mmsdec_rslt->data_ptr);
   aCtx->mmsdec_rslt->data_pres = SD_FALSE;
  }

/************************************************************************/
/*				_ms_fin_pdu				*/
/* User passes ptr to PDU len pPduLen, this function sets (*pPduLen).	*/
/* RETURNS: pointer to MMS PDU						*/
/************************************************************************/

ST_UCHAR *_ms_fin_pdu (ASN1_ENC_CTXT *aCtx, ST_INT type, ST_INT *pPduLen)
  {
  asn1r_fin_constr (aCtx, (ST_INT16)type, CTX,DEF);
  *pPduLen = (int)(aCtx->asn1r_buf_end - aCtx->asn1r_field_ptr);
  return (aCtx->asn1r_field_ptr + 1);
  }

#ifdef DEBUG_SISCO
/************************************************************************/
/*				_ms_fin_pdu_log				*/
/* Write complete MMS PDU to log.					*/
/************************************************************************/

ST_VOID _ms_fin_pdu_log (ST_INT type, ST_CHAR *msg_ptr,
			ST_UCHAR *pduPtr, ST_INT pduLen)
  {
/* Do PDU Encode logging						*/
  if (type == MMSRESP)
    {
    MLOG_ENC2 ("%s Response built : len = %d",msg_ptr, pduLen);
    }
  else
    {
    MLOG_ENC2 ("%s Request built : len = %d",msg_ptr, pduLen);
    }
  MLOG_ENCH (pduLen,pduPtr);

  return;
  }
#endif	/* DEBUG_SISCO	*/

