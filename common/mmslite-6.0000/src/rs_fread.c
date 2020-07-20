/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_fread.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the file read.	*/
/*	It decodes the file read request (indication) and encodes	*/
/*	the file read response. 					*/
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

static  FREAD_REQ_INFO	*fop_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_file_read_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_file_read_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("File Read Request");

/* get storage for request information	*/
  fop_info = (FREAD_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (FREAD_REQ_INFO));
  if (asn1r_get_i32 (aCtx, &(fop_info->frsmid)))	/* read the frsmid		*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_fread_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file read response.				*/
/************************************************************************/

ST_RET mp_fread_resp (MMSREQ_IND *indptr, FREAD_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_FILE_READ,indptr,
			M_CAST_MK_FUN (_ms_mk_fread_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_fread_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file read response.				*/
/************************************************************************/

ST_RET mpl_fread_resp (ST_UINT32 invoke_id, FREAD_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_FILE_READ,invoke_id,
			M_CAST_MK_FUN (_ms_mk_fread_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_fread_resp					*/
/* ENCODE a file read RESPONSE: 					*/
/************************************************************************/

ST_VOID _ms_mk_fread_resp (ASN1_ENC_CTXT *aCtx, FREAD_RESP_INFO *info_ptr)
  {

  if (!info_ptr->more_follows)		/* if data is to be included -	*/
    {					/* different from the default	*/
    asn1r_wr_bool (aCtx, info_ptr->more_follows);	/* write more follows indicator */
    asn1r_fin_prim (aCtx, 1,CTX);
    }

					/* write the file data octets	*/
  asn1r_wr_octstr (aCtx, info_ptr->filedata, info_ptr->fd_len);
  asn1r_fin_prim (aCtx, 0,CTX);

		/* write the context specific tag for this opcode.	*/
  asn1r_fin_constr (aCtx, MMSOP_FILE_READ,CTX,DEF);
  }
