/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_frename.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the file rename.	*/
/*	It decodes the file rename response (confirm) & encodes the	*/
/*	file rename request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pfil.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/************************************************************************/
/*			mms_file_rename_rsp				*/
/* NOTE : Since the mms_file_rename_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_frename					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file rename request.				*/
/************************************************************************/

MMSREQ_PEND *mp_frename (ST_INT chan, FRENAME_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_FILE_RENAME,
			M_CAST_MK_FUN (_ms_mk_frename_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_frename					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file rename request.				*/
/************************************************************************/

ST_RET  mpl_frename (FRENAME_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_FILE_RENAME,
			M_CAST_MK_FUN (_ms_mk_frename_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_frename_req					*/
/* construct a file rename request					*/
/************************************************************************/

ST_VOID _ms_mk_frename_req (ASN1_ENC_CTXT *aCtx, FRENAME_REQ_INFO *info)
  {
FILE_NAME *fname_idx;

					/* write the new file name	*/
  asn1r_strt_constr (aCtx);
  fname_idx = (FILE_NAME *)(info + 1);
  fname_idx += info->num_of_cur_fname;
  _ms_wr_mms_fname (aCtx, fname_idx, info -> num_of_new_fname);
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context specific tag 1	*/

					/* write the current file name	*/
  asn1r_strt_constr (aCtx);
  fname_idx = (FILE_NAME *)(info + 1);
  _ms_wr_mms_fname (aCtx, fname_idx, info -> num_of_cur_fname);
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

		/* write the context specific tag for this opcode.	*/
  asn1r_fin_constr (aCtx, MMSOP_FILE_RENAME,CTX,DEF);
  }
