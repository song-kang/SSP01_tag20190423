/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_fopen.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the file open.	*/
/*	It decodes the file open response (confirm) & encodes the	*/
/*	file open request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pfil.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  FOPEN_RESP_INFO	*fop_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID fopen_fil_attr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID fopen_get_fil_mtim  (ASN1_DEC_CTXT *aCtx);
static ST_VOID fopen_get_fil_siz   (ASN1_DEC_CTXT *aCtx);
static ST_VOID fopen_get_fil_attr	(ASN1_DEC_CTXT *aCtx);
static ST_VOID fopen_get_frsmid    (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_file_open_rsp				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_file_open_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("File Open Response");


/* get storage for request information	*/
  fop_info = (FOPEN_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (FOPEN_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX, 0, fopen_get_frsmid);
  }

/************************************************************************/
/*			fopen_get_frsmid				*/
/* file open response is being decoded: file read state machine id was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID fopen_get_frsmid (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fopen_get_frsmid");

  if (asn1r_get_i32 (aCtx, &(fop_info->frsmid)))	/* read the frsmid		*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, fopen_get_fil_attr);
  }

/************************************************************************/
/*			fopen_get_fil_attr				*/
/* file open response is being decoded: file attributes constr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID fopen_get_fil_attr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fopen_get_fil_attr");

  ASN1R_TAG_ADD (aCtx, CTX, 0, fopen_get_fil_siz);
  }

/************************************************************************/
/*			fopen_get_fil_siz				*/
/* file open response is being decoded: file size was found, last	*/
/* modified time can be next, or can be done.				*/
/************************************************************************/

static ST_VOID fopen_get_fil_siz (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fopen_get_fil_siz");

  if (asn1r_get_u32 (aCtx, &(fop_info->ent.fsize)))	/* read the file size	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX,1,fopen_get_fil_mtim);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = fopen_fil_attr_done;
  }

/************************************************************************/
/*			fopen_get_fil_mtim				*/
/* file open response is being decoded: modified time was encountered,	*/
/* must be done now.							*/
/************************************************************************/

static ST_VOID fopen_get_fil_mtim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fopen_get_fil_mtim");

  fop_info->ent.mtimpres = SD_TRUE;	/* set modify time present	*/
  if (asn1r_get_time (aCtx, &fop_info->ent.mtime))	/* read the last modified time	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  }

/************************************************************************/
/*			fopen_fil_attr_done				*/
/* DECODING file open response: file attribute constructor completed.	*/
/* Set the valid state to done. 					*/
/************************************************************************/

static ST_VOID fopen_fil_attr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fopen_fil_attr_done");

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_fopen					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file open request.				*/
/************************************************************************/

MMSREQ_PEND *mp_fopen (ST_INT chan, FOPEN_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_FILE_OPEN,
			M_CAST_MK_FUN (_ms_mk_fopen_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_fopen					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file open request.				*/
/************************************************************************/

ST_RET mpl_fopen (FOPEN_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_FILE_OPEN,
			M_CAST_MK_FUN (_ms_mk_fopen_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_fopen_req					*/
/* ENCODE a file openectory REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_fopen_req (ASN1_ENC_CTXT *aCtx, FOPEN_REQ_INFO *info)
  {

  asn1r_wr_u32 (aCtx, info->init_pos);		/* write the initial position	*/
  asn1r_fin_prim (aCtx, 1,CTX);			/* context specific tag 1	*/

  asn1r_strt_constr (aCtx);
  _ms_wr_mms_fname (aCtx, (FILE_NAME *)(info + 1),
                 info -> num_of_fname);
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

		/* write the context specific tag for this opcode	*/
  asn1r_fin_constr (aCtx, MMSOP_FILE_OPEN,CTX,DEF);
  }
