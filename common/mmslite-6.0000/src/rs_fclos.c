/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_fclose.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the file close.	*/
/*	It decodes the file close request (indication) and encodes	*/
/*	the file close response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	ST_VOID mms_file_close_req ()					*/
/*	ST_INT16 mp_fclose_resp (indptr)				*/
/*	ST_INT16 mpl_fclose_resp (invoke_id)   	- MMSEASE-LITE		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     06    Corr spelling for 'receive'.			*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     02    Op-specific info buffer handling changes	*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
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

static  FCLOSE_REQ_INFO	*fop_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_file_close_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_file_close_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("File Close Request");


/* get storage for request information	*/
  fop_info = (FCLOSE_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (FCLOSE_REQ_INFO));

			/* read frsmid from the received message	*/
  if (asn1r_get_i32 (aCtx, &(fop_info->frsmid)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_fclose_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file close response.				*/
/************************************************************************/

ST_RET mp_fclose_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_FILE_CLOSE));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_fclose_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file close response.				*/
/************************************************************************/

ST_RET mpl_fclose_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_FILE_CLOSE));
  }
#endif
