/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_initdown.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of initiate download */
/*	sequence.  It decodes the initiate download sequence response	*/
/*	(confirm) & encodes the initiate download sequence request.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     03    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID init_download_null (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_init_download_rsp				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_init_download_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Initiate Download Sequence Response");

#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
  if (mmsl_version)				/* IS 	*/
#endif
    _mms_null_pdu_dec (aCtx);			/* IS PDU is NULL	*/
  else						/* DIS 	*/
    {
    aCtx->asn1r_decode_method = ASN1_TAG_METHOD; /* Set decode method to TAG */
    ASN1R_TAG_ADD (aCtx, UNI, NULL_CODE, init_download_null);
    }
  }

/************************************************************************/
/*			init_download_null				*/
/* This function is called from MMSDEC when the NULL contents of this	*/
/* response is found. No action required.				*/
/************************************************************************/

static ST_VOID init_download_null (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("init_download_null");

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_initdown					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the initiate download sequence.		*/
/************************************************************************/

MMSREQ_PEND *mp_initdown (ST_INT chan, INITDOWN_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_INIT_DOWNLOAD,
			M_CAST_MK_FUN (_ms_mk_initdown_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_initdown					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the initiate download sequence.			*/
/************************************************************************/

ST_RET  mpl_initdown (INITDOWN_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_INIT_DOWNLOAD,
			M_CAST_MK_FUN (_ms_mk_initdown_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_initdown_req 				*/
/* ENCODE an initiate download sequence REQUEST:			*/
/************************************************************************/

ST_VOID _ms_mk_initdown_req (ASN1_ENC_CTXT *aCtx, INITDOWN_REQ_INFO *info)
  {
ST_INT16	i;
ST_CHAR	**cap_ptr;

  asn1r_wr_bool (aCtx, info->sharable);		/* write 'sharable' element	*/
  asn1r_fin_prim (aCtx, 2,CTX);

/* Write the list of capabilities					*/

  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
  cap_ptr  = (ST_CHAR **) (info + 1);
  cap_ptr += info->num_of_capab - 1;	/* point to last capability	*/
  for (i = 0; i < info->num_of_capab; i++)
    {
    asn1r_wr_vstr (aCtx, *cap_ptr);			/* write the capability 	*/
    asn1r_fin_prim (aCtx, VISTR_CODE,UNI);		/* universal string		*/
    cap_ptr--;
    }
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* ctx tag 1 constructor done	*/

  asn1r_wr_vstr (aCtx, info->dname);		/* write the domain name	*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_INIT_DOWNLOAD,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }

