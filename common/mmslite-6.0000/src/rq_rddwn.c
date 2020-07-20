/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_rddwn.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of request domain	*/
/*	download.  It decodes the request domain download response	*/
/*	(confirm) & encodes the request domain download request.	*/
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
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_rddwn_rsp					*/
/* NOTE : Since the mms_rddwn_rsp is a NULL PDU, the decode is continued*/
/* by '_mms_null_pdu_dec', which simply sets decode done = 1 and sets the*/
/* decode done function to point to the general decode done fun.  See   */
/* mmsdec.c for the general functions. 					*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rddwn					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the request domain download request.	*/
/************************************************************************/

MMSREQ_PEND *mp_rddwn (ST_INT chan, RDDWN_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REQ_DOM_DOWN,
			M_CAST_MK_FUN (_ms_mk_rddwn_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rddwn					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the request domain download request.		*/
/************************************************************************/

ST_RET  mpl_rddwn (RDDWN_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REQ_DOM_DOWN,
			M_CAST_MK_FUN (_ms_mk_rddwn_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rddwn_req 					*/
/* ENCODE a request domain download REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_rddwn_req (ASN1_ENC_CTXT *aCtx, RDDWN_REQ_INFO *info)
  {
ST_INT	i;
ST_CHAR	**cap_ptr;
FILE_NAME	*fname_idx;

  asn1r_strt_constr (aCtx);
  cap_ptr  = (ST_CHAR **) (info + 1);
  cap_ptr += info->num_of_capab;	/* point to first file_name	*/
  fname_idx = (FILE_NAME *)(cap_ptr);
  _ms_wr_mms_fname (aCtx, fname_idx, info->num_of_fname);
  asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* context specific tag 4	*/

  asn1r_wr_bool (aCtx, info->sharable);		/* write 'sharable' element	*/
  asn1r_fin_prim (aCtx, 2,CTX);

/* Write the list of capabilities					*/

  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
  cap_ptr--; 				/* point to last capability	*/
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
  asn1r_fin_constr (aCtx, MMSOP_REQ_DOM_DOWN,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }

