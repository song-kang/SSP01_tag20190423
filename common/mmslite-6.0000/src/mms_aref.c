/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*      	  1986 - 2011, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_aref.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :                                                 */
/* This module contains functions for encoding and decoding and the	*/
/* general manipulation of Application References.			*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev      Comments                                    */
/* --------  ---  ------   -------------------------------------------  */
/* 04/02/11  EJV           Re-added suicacse.h (eliminated sun warning).*/
/* 10/28/10  EJV           Del suicacse.h.				*/
/* 12/08/07  EJV     08	   ms_arname_to_appref: chg **appref to *appref.*/ 
/*                         s_find_proc: chg ret and added *appref param.*/
/*			   ms_asn1_to_appref: use usr_info[0],not glbvar*/
/* 04/04/05  EJV     07	   MMSEASE: needs suicscse.h.			*/
/* 02/28/05  EJV     05    Elim Linux warnings in if (a=b)		*/
/* 03/28/05  JRB     05    Del suicacse header.				*/
/* 03/11/04  GLB     04    Cleaned up "thisFileName"                    */
/* 12/20/01  JRB     03    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/29/97  DSF     01    Moved some of this functionality to SUIC	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"

#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pcon.h"
#include "asn1defs.h"

#ifndef MMS_LITE
#include "suicacse.h"  /* for s_find_proc */
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && defined(S_MT_SUPPORT)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif


static ST_VOID ap_title_strt (ASN1_DEC_CTXT *aCtx);
static ST_VOID ap_invoke_strt (ASN1_DEC_CTXT *aCtx);
static ST_VOID ae_qual_strt (ASN1_DEC_CTXT *aCtx);
static ST_VOID ae_invoke_strt (ASN1_DEC_CTXT *aCtx);
static ST_VOID ar_ref_asn1_decode_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID ae_invoke_get (ASN1_DEC_CTXT *aCtx);
static ST_VOID ae_qual_cnstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID ae_qual_get (ASN1_DEC_CTXT *aCtx);
static ST_VOID ap_invoke_cnstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID ap_invoke_get (ASN1_DEC_CTXT *aCtx);
static ST_VOID ap_cnstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID ap_title_get (ASN1_DEC_CTXT *aCtx);
static ST_VOID ar_cnstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID ar_ref_strt (ASN1_DEC_CTXT *aCtx);

/**************************************************************************/
/*			ms_appref_to_asn1				  */
/* Function to convert from an intermediate for of an ApplicationReference*/
/* to ASN.1.  Function only supports FORM_2 of and ApplicationReference	  */
/**************************************************************************/

ST_RET ms_appref_to_asn1 (APP_REF *ar_ref_ptr, ST_UCHAR *build_buf_ptr,
			  ST_INT build_buf_len, ST_UCHAR **ret_ptr, 
			  ST_INT *ret_len)
  {
ST_INT	ret_val;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  if (ar_ref_ptr->form == APP_REF_FORM2)
    {
    asn1r_strt_asn1_bld (aCtx, build_buf_ptr,build_buf_len);
    asn1r_strt_constr (aCtx);

    if (ar_ref_ptr->ae_invoke_pres)
      {
      asn1r_strt_constr (aCtx);
      asn1r_wr_i32 (aCtx, ar_ref_ptr->ae_invoke);
      asn1r_fin_prim (aCtx, INT_CODE,UNI);
      asn1r_fin_constr (aCtx, 3,CTX,DEF);
      }

    if (ar_ref_ptr->ae_qual_pres)
      {
      asn1r_strt_constr (aCtx);
      asn1r_wr_i32 (aCtx, ar_ref_ptr->ae_qual.form_2);
      asn1r_fin_prim (aCtx, INT_CODE,UNI);
      asn1r_fin_constr (aCtx, 2,CTX,DEF);
      }

    if (ar_ref_ptr->ap_invoke_pres)
      {
      asn1r_strt_constr (aCtx);
      asn1r_wr_i32 (aCtx, ar_ref_ptr->ap_invoke);
      asn1r_fin_prim (aCtx, INT_CODE,UNI);
      asn1r_fin_constr (aCtx, 1,CTX,DEF);
      }

    if ((ar_ref_ptr->ap_title.form_2.num_comps) && 
         (ar_ref_ptr->ap_title_pres))
      {
      asn1r_strt_constr (aCtx);
      asn1r_wr_objid (aCtx, &ar_ref_ptr->ap_title.form_2.comps[0],
               ar_ref_ptr->ap_title.form_2.num_comps);
      asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
      asn1r_fin_constr (aCtx, 0,CTX,DEF);
      }

    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);

    *ret_ptr = aCtx->asn1r_field_ptr+1;
    *ret_len = (&build_buf_ptr[build_buf_len] - aCtx->asn1r_field_ptr -1) & 0xffff;
    ret_val = SD_SUCCESS;
    }
  else				/* not form 2				*/
    ret_val = MVE_BAD_APP_REF_FORM;

/* Check for encode overrun */
  if (aCtx->asn1r_encode_overrun)
    ret_val = ME_ASN1_ENCODE_OVERRUN;

  S_UNLOCK_COMMON_RESOURCES ();
  return (ret_val);
  }


/************************************************************************/
/*                DECODE FUNCTIONS FOR APPLICATION REFERENCE		*/
/* 			    ms_asn1_to_appref				*/
/* Function to convert an ASN.1 encoded ApplicationReference to an	*/
/* intermediate form.  These decode functions only support FORM_2	*/
/************************************************************************/

ST_RET ms_asn1_to_appref (APP_REF *ar_ref_ptr, ST_UCHAR *buf_ptr, ST_INT buf_len)
  {
ST_RET ret;
ASN1_DEC_CTXT localDecCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_DEC_CTXT *aCtx = &localDecCtx;

  S_LOCK_COMMON_RESOURCES ();
  memset (aCtx, 0, sizeof (ASN1_DEC_CTXT));	/* CRITICAL: start clean.	*/

  memset (ar_ref_ptr, 0, sizeof (APP_REF));
  if (*buf_ptr == (CONSTR | SEQ_CODE))
    ar_ref_ptr->form = APP_REF_FORM2;
  else
    ar_ref_ptr->form = APP_REF_FORM1;
  ar_ref_ptr->ap_title.form_2.num_comps = 0;
  ar_ref_ptr->ap_invoke_pres = SD_FALSE;
  ar_ref_ptr->ae_qual_pres = SD_FALSE;
  ar_ref_ptr->ae_invoke_pres = SD_FALSE;

  aCtx->usr_info[0] = ar_ref_ptr;

  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;
  ASN1R_TAG_ADD (aCtx, UNI|CONSTR,SEQ_CODE,ar_ref_strt);
  aCtx->asn1r_decode_done_fun = asn1r_done_err;
  aCtx->asn1r_err_fun = NULL;
  asn1r_decode_asn1 (aCtx, buf_ptr,buf_len);

  if (aCtx->asn1r_pdu_dec_err == NO_DECODE_ERR)
    ret = SD_SUCCESS;
  else
    ret = SD_FAILURE;
  S_UNLOCK_COMMON_RESOURCES ();

  return (ret);
  }


static ST_VOID ar_ref_strt (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,ap_title_strt);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,ap_invoke_strt);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,ae_qual_strt);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,3,ae_invoke_strt);
  aCtx->asn1r_decode_done_fun = ar_ref_asn1_decode_done;
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ar_cnstr_done;
  }
 
static ST_VOID ar_cnstr_done (ASN1_DEC_CTXT *aCtx)
  {
  }
 
static ST_VOID ap_title_strt (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, UNI,OBJ_ID_CODE,ap_title_get);
  aCtx->asn1r_decode_done_fun = asn1r_done_err;
  }

static ST_VOID ap_title_get (ASN1_DEC_CTXT *aCtx)		    
  {
APP_REF *ar_fill_ptr = (APP_REF *) aCtx->usr_info[0];

  asn1r_get_objid (aCtx, &ar_fill_ptr->ap_title.form_2.comps[0],
            &ar_fill_ptr->ap_title.form_2.num_comps);
  if (ar_fill_ptr->ap_title.form_2.num_comps)
    ar_fill_ptr->ap_title_pres = SD_TRUE;
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ap_cnstr_done;
  }

static ST_VOID ap_cnstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,ap_invoke_strt);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,ae_qual_strt);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,3,ae_invoke_strt);
  aCtx->asn1r_decode_done_fun = ar_ref_asn1_decode_done;
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ar_cnstr_done;
  }

static ST_VOID ap_invoke_strt (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, UNI,INT_CODE,ap_invoke_get);
  aCtx->asn1r_decode_done_fun = asn1r_done_err;
  }

static ST_VOID ap_invoke_get (ASN1_DEC_CTXT *aCtx)
  {
APP_REF *ar_fill_ptr = (APP_REF *) aCtx->usr_info[0];

  if (!asn1r_get_i32 (aCtx, &ar_fill_ptr->ap_invoke))
    ar_fill_ptr->ap_invoke_pres = SD_TRUE; 
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ap_invoke_cnstr_done;
  }

static ST_VOID ap_invoke_cnstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,ae_qual_strt);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,3,ae_invoke_strt);
  aCtx->asn1r_decode_done_fun = ar_ref_asn1_decode_done;
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ar_cnstr_done;
  }

static ST_VOID ae_qual_strt (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, UNI,INT_CODE,ae_qual_get);
  aCtx->asn1r_decode_done_fun = asn1r_done_err;
  }

static ST_VOID ae_qual_get (ASN1_DEC_CTXT *aCtx)
  {
APP_REF *ar_fill_ptr = (APP_REF *) aCtx->usr_info[0];

  if (!asn1r_get_i32 (aCtx, &ar_fill_ptr->ae_qual.form_2))
    ar_fill_ptr->ae_qual_pres = SD_TRUE; 
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ae_qual_cnstr_done;
  }

static ST_VOID ae_qual_cnstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,3,ae_invoke_strt);
  aCtx->asn1r_decode_done_fun = ar_ref_asn1_decode_done;
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ar_cnstr_done;
  }

static ST_VOID ae_invoke_strt (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, UNI,INT_CODE,ae_invoke_get);
  aCtx->asn1r_decode_done_fun = asn1r_done_err;
  }

static ST_VOID ae_invoke_get (ASN1_DEC_CTXT *aCtx)
  {
APP_REF *ar_fill_ptr = (APP_REF *) aCtx->usr_info[0];

  if (!asn1r_get_i32 (aCtx, &ar_fill_ptr->ae_invoke))
	ar_fill_ptr->ae_invoke_pres = SD_TRUE; 
  aCtx->asn1r_decode_done_fun = ar_ref_asn1_decode_done;
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ar_cnstr_done;
  }

static ST_VOID ar_ref_asn1_decode_done (ASN1_DEC_CTXT *aCtx)
  {
  }

/************************************************************************/
#ifndef MMS_LITE
/************************************************************************/

/************************************************************************/
/*			ms_arname_to_asn1				*/
/* Function to convert from an arname to the ASN.1 encoded equivalent of*/
/* the ApplicationReference that corresponds to it.  Returns SD_SUCCESS or	*/
/* ERROR CODE.								*/
/************************************************************************/

ST_RET ms_arname_to_asn1 (ST_CHAR *arname, ST_UCHAR *buf_ptr, 
			  ST_INT buf_len, ST_UCHAR **ret_ptr_addr, 
			  ST_INT *ret_len_addr)
  {
ST_RET	ret_val;
APP_REF	app_reference;		/* memset to 0 in s_find_proc 	*/

  S_LOCK_COMMON_RESOURCES ();
  ret_val = MVE_DATA_CONVERT;
  if ((ret_val = ms_arname_to_appref (arname, &app_reference)) == SD_SUCCESS)
    {
    ret_val = ms_appref_to_asn1(&app_reference, buf_ptr, buf_len, 
    				 ret_ptr_addr, ret_len_addr);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret_val);
  }

/************************************************************************/
/*			ms_asn1_to_arname				*/
/* Function to convert from an ASN.1 encoded application reference to 	*/	
/* the arname that corresponds to it.  Return SD_SUCCESS or ERROR CODE.	*/
/************************************************************************/

ST_RET ms_asn1_to_arname (ST_CHAR *arname, ST_UCHAR *asn1_ptr, ST_INT asn1_len)
  {
ST_RET	ret_val;
APP_REF	app_reference;

  S_LOCK_COMMON_RESOURCES ();
  ret_val = MVE_ASN1_DECODE_ERR;
  if (!(ret_val = ms_asn1_to_appref (&app_reference, asn1_ptr, asn1_len)))
    {
    ret_val = ms_appref_to_arname (arname, &app_reference);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret_val);
  }

/************************************************************************/
/*			ms_arname_to_appref				*/
/* Function to convert from the C representation of an arname to the C	*/
/* representation of an ApplicationReference.  Returns SD_SUCCESS or 	*/
/* ERROR CODE.								*/
/************************************************************************/

ST_RET ms_arname_to_appref (ST_CHAR *arname, APP_REF *appref)
  {
ST_RET	ret_val;

  S_LOCK_COMMON_RESOURCES ();
  ret_val = MVE_DATA_CONVERT;
  if (arname && appref)
    {
    if (s_find_proc (arname, appref) == SD_SUCCESS)
      ret_val = SD_SUCCESS;
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret_val);
  }

/************************************************************************/
/*			ms_appref_to_arname				*/
/* Function to convert from the C representation of an ApplicationRef.	*/
/* to the C representation of an arname.  Returns SD_SUCCESS or ERROR CODE.*/
/* The argument arname has to be a pointer to a place big enough to	*/
/* hold the arname (MAX_AR_LEN+1).					*/
/************************************************************************/

ST_RET ms_appref_to_arname (ST_CHAR *arname, APP_REF *appref_addr)
  {
ST_RET ret;

  S_LOCK_COMMON_RESOURCES ();
  ret = MVE_DATA_CONVERT;
  if (arname  && appref_addr) 
    {				      /* match the AE-Qualifier		*/
    if (s_find_arname (appref_addr, appref_addr->ae_qual_pres, arname))
      ret = SD_SUCCESS;
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

/************************************************************************/
#endif 	/* end ifndef MMS_LITE */
/************************************************************************/
