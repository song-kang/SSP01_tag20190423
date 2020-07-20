/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_fread.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the file read.	*/
/*	It decodes the file read response (confirm) & encodes the	*/
/*	file read request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     06    Corr spelling for 'receive'.			*/
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

static  FREAD_RESP_INFO	*fop_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/* static functions with multiple references                            */

static ST_VOID fread_file_data_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID fread_get_fd_prim     (ASN1_DEC_CTXT *aCtx);
static ST_VOID fread_get_fd_cstr     (ASN1_DEC_CTXT *aCtx);
static ST_VOID fread_get_more_follows (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/*			mms_file_read_rsp				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_file_read_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("File Read Response");


/* get storage for request information	*/
  fop_info = (FREAD_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (FREAD_RESP_INFO));
  fop_info->more_follows = SD_TRUE;		/* set default value	*/
  ASN1R_TAG_ADD (aCtx, CTX,0, fread_get_fd_prim);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 0, fread_get_fd_cstr);
  }


/************************************************************************/
/*			fread_get_fd_prim				*/
/* file read response is being decoded: file data was encountered.	*/
/************************************************************************/

static ST_VOID fread_get_fd_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fread_get_fd_prim");

				/* decode file data back onto itself	*/
  fop_info->filedata = aCtx->asn1r_field_ptr;

  if (asn1r_get_octstr (aCtx, fop_info->filedata))	/* read file data from message	*/
    asn1r_set_dec_err (aCtx, RESP_UNSPECIFIED);

  fread_file_data_done (aCtx);	/* save octet count, setup to continue	*/
  }

/************************************************************************/
/*			fread_get_fd_cstr				*/
/* file read response is being decoded: file data was encountered.	*/
/************************************************************************/

static ST_VOID fread_get_fd_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fread_get_fd_cstr");

			/* decode file data back onto itself		*/
  fop_info->filedata = aCtx->asn1r_field_ptr;

			/* set up done decoding octet cstr destination	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = fread_file_data_done;

			/* get file data from the received message	*/
			/* set maximum length to accept entire message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, fop_info->filedata);
  }

/************************************************************************/
/*			fread_file_data_done				*/
/* file read response is being decoded: end of file data constructor	*/
/* This is called directly from prim octet or from c_done fun.		*/
/************************************************************************/

static ST_VOID fread_file_data_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("data_don");

  fop_info->fd_len = aCtx->asn1r_octetcount;	/* save the file data length	*/
			/* set up done decoding file data cstr dest	*/

  ASN1R_TAG_ADD (aCtx, CTX, 1, fread_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			fread_get_more_follows				*/
/* file read response is being decoded: more data follows indicator was */
/* encountered, must be done now.					*/
/************************************************************************/

static ST_VOID fread_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fread_get_more_follows");

  if (asn1r_get_bool (aCtx, &(fop_info->more_follows)))	/* read more follows	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_fread					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file read request.				*/
/************************************************************************/

MMSREQ_PEND *mp_fread (ST_INT chan, FREAD_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_FILE_READ,
			M_CAST_MK_FUN (_ms_mk_fread_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_fread					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file read request.				*/
/************************************************************************/

ST_RET  mpl_fread (FREAD_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_FILE_READ,
			M_CAST_MK_FUN (_ms_mk_fread_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_fread_req					*/
/* ENCODE a file readectory REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_fread_req (ASN1_ENC_CTXT *aCtx, FREAD_REQ_INFO *info)
  {
  asn1r_wr_i32 (aCtx, info->frsmid);		/* write the frsmid		*/
  /* write the context specific tag for this opcode	*/
  asn1r_fin_prim (aCtx, MMSOP_FILE_READ,CTX);
  }
