/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_rename.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the rename	*/
/*	service.  It decodes the rename response (indication) and	*/
/*	encodes the rename request.					*/
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
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_rename_rsp					*/
/* NOTE : Since the mms_rename_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/
  

#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rename					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send a rename REQUEST:	  			*/
/************************************************************************/

MMSREQ_PEND *mp_rename (ST_INT chan, RENAME_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_RENAME,
			M_CAST_MK_FUN (_ms_mk_rename_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rename					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE a rename REQUEST:		  			*/
/************************************************************************/

ST_RET mpl_rename (RENAME_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_RENAME,
			M_CAST_MK_FUN (_ms_mk_rename_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rename_req					*/
/* ENCODE a rename REQUEST:						*/
/************************************************************************/

ST_VOID _ms_mk_rename_req (ASN1_ENC_CTXT *aCtx, RENAME_REQ_INFO  *info_ptr)
  {
  asn1r_wr_vstr (aCtx, info_ptr->new_ident); 	/* write the new identifier	*/
  asn1r_fin_prim (aCtx, 2,CTX);			/* context specific tag 2	*/

  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
  _ms_wr_mms_objname (aCtx, &info_ptr->cur_name);	/* write the current name	*/
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context specific tag 1	*/

  asn1r_strt_constr (aCtx);			/* start extendedObjectClass	*/
#ifdef CS_SUPPORT
  if (info_ptr->cs_objclass_pres)  	/* if companion standard	*/
    {
    asn1r_strt_constr (aCtx);			/* start cs ObjectClass	cstr	*/
    asn1r_wr_octstr (aCtx, info_ptr->obj.cs.cs_class,info_ptr->obj.cs.len);
    asn1r_fin_constr (aCtx, 1,CTX,DEF);
    }
  else
    {
    asn1r_wr_i16 (aCtx, info_ptr->obj.mms_class);	/* write the object scope	*/
    asn1r_fin_prim (aCtx, 0,CTX);
    }
#else
  asn1r_wr_i16 (aCtx, info_ptr->obj.mms_class);	/* write the object scope	*/
  asn1r_fin_prim (aCtx, 0,CTX);
#endif
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish the extendedObjClass	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_RENAME,CTX,DEF);		/* complete rename req		*/
  }
