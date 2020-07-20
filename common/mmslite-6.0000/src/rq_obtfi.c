/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_obtfile.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the obtain file.	*/
/*	It decodes the obtain file response (confirm) & encodes the	*/
/*	obtain file request.						*/
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
/*			mms_obtain_file_rsp				*/
/* NOTE : Since the mms_obtain_file_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_obtfile					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the obtain file request.				*/
/************************************************************************/

MMSREQ_PEND *mp_obtfile (ST_INT chan, OBTFILE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_OBTAIN_FILE,
			M_CAST_MK_FUN (_ms_mk_obtfile_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_obtfile					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the obtain file request.				*/
/************************************************************************/

ST_RET  mpl_obtfile (OBTFILE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_OBTAIN_FILE,
			M_CAST_MK_FUN (_ms_mk_obtfile_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_obtfile_req					*/
/* construct an obtain file request					*/
/************************************************************************/

ST_VOID _ms_mk_obtfile_req (ASN1_ENC_CTXT *aCtx, OBTFILE_REQ_INFO *info)
  {
FILE_NAME	*fname_idx;

					/* write the destinaion file	*/
  asn1r_strt_constr (aCtx);
  fname_idx = (FILE_NAME *)(info + 1);
  fname_idx +=  info->num_of_src_fname;
  _ms_wr_mms_fname (aCtx, fname_idx, info->num_of_dest_fname);
  asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* context specific tag 2	*/

					/* write the source file	*/
  asn1r_strt_constr (aCtx);
  fname_idx = (FILE_NAME *)(info + 1);
  _ms_wr_mms_fname (aCtx, fname_idx, info->num_of_src_fname);
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context specific tag 1	*/

  if (info->ar_title_pres)
    {
    asn1r_wr_delmnt (aCtx, info->ar_title, info->ar_len);
    *(aCtx->asn1r_field_ptr+1) = (ST_UCHAR) 0xA0; 
    }

/* Obtain file request contents complete, finish the sequence by	*/
/* writing the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_OBTAIN_FILE,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
