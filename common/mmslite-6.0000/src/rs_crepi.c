/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_crepi.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of create program	*/
/*	invocation.  It decodes the create program invocation request	*/
/*	(indication) & encodes the create program invocation response.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pprg.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  CREPI_REQ_INFO	*req_info;

static ST_CHAR	**dom_ptr;
static ST_INT	max_vstr;
static ST_INT	num_vstr;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID crepi_get_reusable (ASN1_DEC_CTXT *aCtx);
static ST_VOID crepi_get_monitor (ASN1_DEC_CTXT *aCtx);
static ST_VOID crepi_get_domain_name (ASN1_DEC_CTXT *aCtx);
static ST_VOID crepi_domain_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID crepi_domain_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID crepi_get_piname (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_create_pi_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_create_pi_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Create Program Invocation Request");

#if defined(FIXED_DECBUF)
  max_vstr = m_sv_max_vstr;
  req_size = mmsl_dec_info_size;
#else
  max_vstr = _mmsdec_msglen/2;	    /* determine maximum size	*/	
  if (m_sv_max_vstr && (max_vstr > m_sv_max_vstr))
    max_vstr = m_sv_max_vstr;
					
  req_size = sizeof (CREPI_REQ_INFO) + (sizeof (ST_CHAR *)*max_vstr);
#endif

/* get storage for request information	*/
  req_info = (CREPI_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  dom_ptr  = (ST_CHAR **) (req_info + 1);	/* point to where first will go */
  num_vstr = 0;
  ASN1R_TAG_ADD (aCtx, CTX, 0, crepi_get_piname); 	/* pi name is next	*/
  }

/************************************************************************/
/*			crepi_get_piname				*/
/* create program invocation REQUEST (indication) is being decoded:	*/
/* decode the state.							*/
/************************************************************************/

static ST_VOID crepi_get_piname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("crepi_get_piname");

  if (asn1r_get_identifier (aCtx, req_info->piname))/* read the pgm invocation name */
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, crepi_domain_cstr);
  }

/************************************************************************/
/*			crepi_domain_cstr				*/
/* create program invocation REQUEST (indication) is being decoded:	*/
/* domain name list constructor was encountered.			*/
/************************************************************************/

static ST_VOID crepi_domain_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("crepi_domain_cstr");

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, crepi_get_domain_name);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = crepi_domain_cstr_done;
  }

/************************************************************************/
/*			crepi_get_domain_name				*/
/* create program invocation REQUEST (indication) is being decoded:	*/
/* domain name was encountered. 					*/
/************************************************************************/

static ST_VOID crepi_get_domain_name (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("crepi_get_domain_name");

  req_info->num_of_dnames++;		/* increment # of domain names	*/

  num_vstr++;				/* increment number of vstr	*/
  if (num_vstr > max_vstr)
    {					/* too many visible strings	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

	/* copy domain name back onto itself shifted by one character	*/
  *dom_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_identifier (aCtx, *dom_ptr))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  dom_ptr++;				/* point to where next will go	*/

  asn1r_tag_restore (aCtx);
  }

/************************************************************************/
/*			crepi_domain_cstr_done				*/
/* create program invocation REQUEST (indication) is being decoded:	*/
/* domain name list constructor done was encountered.			*/
/************************************************************************/

static ST_VOID crepi_domain_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

  req_info->reusable = SD_TRUE;	/* SD_TRUE us default value		*/
  ASN1R_TAG_ADD (aCtx, CTX, 	 2, crepi_get_reusable);
  ASN1R_TAG_ADD (aCtx, CTX, 	 3, crepi_get_monitor);

  /* find actual size for request info	*/
  req_size = (int) ((ST_CHAR *)dom_ptr - ((ST_CHAR *) req_info) + 1);
				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (CREPI_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


/************************************************************************/
/*			crepi_get_reusable				*/
/* create program invocation REQUEST (indication) is being decoded:	*/
/* decode the reusable flag.						*/
/************************************************************************/

static ST_VOID crepi_get_reusable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("crepi_get_reusable");

  if (asn1r_get_bool (aCtx, &(req_info->reusable)))  /* read reusable from message	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  ASN1R_TAG_ADD (aCtx, CTX, 	 3, crepi_get_monitor);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			crepi_get_monitor				*/
/* create program invocation REQUEST (indication) is being decoded:	*/
/* decode the monitor flag.						*/
/************************************************************************/

static ST_VOID crepi_get_monitor (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("crepi_get_monitor");

  req_info->monitor_pres = SD_TRUE;	/* set monitor present		*/

  if (asn1r_get_bool (aCtx, &(req_info->monitor)))	/* read monitor from message	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_crepi_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the create program invocation response.	*/
/************************************************************************/

ST_RET mp_crepi_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_CREATE_PI));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_crepi_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the create program invocation response.		*/
/************************************************************************/

ST_RET mpl_crepi_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_CREATE_PI));
  }
#endif
