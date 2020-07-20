/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_ced2.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common primitive encode/decode functions	*/
/*	particular getting and writing filenames.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 03/11/04  GLB     03    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     01    Added SD_CONST modifiers			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"

/************************************************************************/

static ST_VOID fname_get_name  (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/*			global structures & functions			*/
/************************************************************************/

static FILE_NAME        *fname_ptr;


static ST_INT  MAX_FNAMES;
static ST_INT *fname_count;
static ST_RET  ERROR_CODE;


/************************************************************************/
/************************************************************************/
/*			_ms_get_mms_fname					*/
/************************************************************************/

ST_VOID _ms_get_mms_fname (ASN1_DEC_CTXT *aCtx, FILE_NAME *data_ptr, ST_INT ptr_len,
	ST_INT *num_of_names_ptr)
  {
  MLOG_CDEC0 ("_ms_get_mms_fname");

  if (aCtx->mmsdec_rslt->type == MMSRESP)
    ERROR_CODE = RESP_BAD_VALUE;
  else
    ERROR_CODE = REQ_BAD_VALUE;

  fname_ptr   = data_ptr;      		/* save the addr of the array	*/
  MAX_FNAMES  = ptr_len;       		/* save the array upper bounds	*/
  fname_count = num_of_names_ptr;       /* save the ptr to the count	*/
  ASN1R_TAG_ADD (aCtx, UNI, GRAFSTR_CODE, fname_get_name);
  }

/************************************************************************/
/*			fname_get_name					*/
/************************************************************************/

static ST_VOID fname_get_name (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fname_get_name");

  (*fname_count)++;			/* increment # of file names	*/
  if (*fname_count > MAX_FNAMES)
    {
    asn1r_set_dec_err (aCtx, ERROR_CODE);
    return;
    }
				/* save file name pointer & length	*/
  fname_ptr->fn_len = aCtx->asn1r_elmnt_len;
  fname_ptr->fname = (ST_CHAR *) aCtx->asn1r_field_ptr;
  asn1r_get_octstr (aCtx, (ST_UCHAR *) fname_ptr->fname);

  fname_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI, GRAFSTR_CODE, fname_get_name);
  }


/************************************************************************/
/************************************************************************/
/*			_ms_wr_mms_fname					*/
/* encode a list of file names.						*/
/************************************************************************/

ST_VOID _ms_wr_mms_fname (ASN1_ENC_CTXT *aCtx, FILE_NAME *data_ptr, ST_INT len)
  {
ST_INT i;
FILE_NAME 	*fn_ptr;

  fn_ptr = data_ptr;
  fn_ptr += len - 1;			/* point to last file name	*/
  for (i = 0; i < len; i++)
    {
    asn1r_wr_octstr (aCtx, (ST_UCHAR *) fn_ptr->fname,fn_ptr->fn_len);/* file name	*/
    asn1r_fin_prim (aCtx, GRAFSTR_CODE,UNI);		/* universal grafstring	*/
    fn_ptr--;
    }
  }
