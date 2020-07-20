/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_fclose.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the file close.	*/
/*	It decodes the file close response (confirm) & encodes the	*/
/*	file close request.						*/
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
/*			mms_file_close_rsp				*/
/* NOTE : Since the mms_file_close_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_fclose					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file close request.				*/
/************************************************************************/

MMSREQ_PEND *mp_fclose (ST_INT chan, FCLOSE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_FILE_CLOSE,
			M_CAST_MK_FUN (_ms_mk_fclose_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_fclose					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file close request.				*/
/************************************************************************/

ST_RET mpl_fclose (FCLOSE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_FILE_CLOSE,
			M_CAST_MK_FUN (_ms_mk_fclose_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_fclose_req					*/
/* construct a file close request					*/
/************************************************************************/

ST_VOID _ms_mk_fclose_req (ASN1_ENC_CTXT *aCtx, FCLOSE_REQ_INFO *info)
  {

  asn1r_wr_i32 (aCtx, info->frsmid);		/* write the frsmid		*/

		/* write the context specific tag for this opcode.	*/
  asn1r_fin_prim (aCtx, MMSOP_FILE_CLOSE,CTX);
  }
