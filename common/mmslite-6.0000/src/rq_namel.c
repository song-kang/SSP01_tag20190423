/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_namelist.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the GetNameList	*/
/*	service.  It decodes the GetNameList response (indication) and	*/
/*	encodes the GetNameList request.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
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
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

static  NAMELIST_RESP_INFO	*info;

static ST_CHAR **nptr;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID namelist_get_list_of_ident (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_get_identifier   (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_list_done        (ASN1_DEC_CTXT *aCtx);
static ST_VOID namelist_more_follows     (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_namelist_rsp				*/
/* Function called to decode a GetNameList response PDU after the PDU	*/
/* type has been determined.  Sets up state machine function pointers	*/
/* and returns, letting the ASN1DE tools take over.			*/
/************************************************************************/

ST_VOID mms_namelist_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;
ST_INT max_vstr;

  MLOG_CDEC0 ("Get Named List");

#if defined(FIXED_DECBUF)
  max_vstr = m_cl_max_namel_names;
  rsp_size = mmsl_dec_info_size;
#else

/* Allocate enough memory to hold the namelist_resp_info structure and	*/
/* a list of pointers to the names returned in the GetNameList response.*/

  max_vstr = _mmsdec_msglen/2;
  if (m_cl_max_namel_names && (max_vstr > m_cl_max_namel_names))
    max_vstr = m_cl_max_namel_names;
  rsp_size = sizeof(NAMELIST_RESP_INFO) + (sizeof(ST_CHAR *) * max_vstr);
#endif

/* get storage for request information	*/
  info = (NAMELIST_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,namelist_get_list_of_ident);	/* declare next elmnt expected	*/
  }

/************************************************************************/
/*			    namelist_get_list_of_ident			*/
/* GetNameList response is being decoded: the SEQUENCEOF Identifier has */
/* been encountered.							*/
/************************************************************************/

static ST_VOID namelist_get_list_of_ident (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_list_of_ident");

/* Initialize nptr to just after the namelist_resp_info structure, and	*/
/* initialize the number of names to zero.				*/

  nptr = (ST_CHAR **) (info + 1);
  info -> num_names = 0;
  info -> more_follows = SD_TRUE;		/* set the default value	*/

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = namelist_list_done;
  ASN1R_TAG_ADD (aCtx, UNI,VISTR_CODE,namelist_get_identifier);
  }

/************************************************************************/
/*				namelist_get_identifier			*/
/* GetNameList resp. is being decoded: model name has been encountered. */
/************************************************************************/

static ST_VOID namelist_get_identifier (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_get_identifier");

/* Increment the number of names, assign the pointer to the new name,	*/
/* and read the name into the space in the existing message.		*/

  nptr[info->num_names] = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_identifier (aCtx, nptr[info->num_names]))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  ++info->num_names;

  asn1r_tag_restore (aCtx);
  }

/************************************************************************/
/*			  namelist_list_done 				*/
/* GetNameList response is being decoded: end of name list encountered. */
/************************************************************************/

static ST_VOID namelist_list_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT  resp_size;

/* There are no other data elements in the message, resize the struct.  */

			/* find the exact size of the storage used	*/
  resp_size = sizeof (NAMELIST_RESP_INFO) +
			(info->num_names) * sizeof (ST_CHAR *);

			/* give back unused storage			*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,resp_size);
  info = (NAMELIST_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;


  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ASN1R_TAG_ADD (aCtx, CTX,1,namelist_more_follows);
  }

/************************************************************************/
/*			namelist_more_follows	       			*/
/* GetNameList response is being decoded: the more follows parameter	*/
/* has been encountered.						*/
/************************************************************************/

static ST_VOID namelist_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("namelist_more_follows");

  if (aCtx->asn1r_elmnt_len != 1)
    {					/* If not of length 1 (boolean),*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);		/* stop parse, report error	*/
    return;				/* and return.			*/
    }

  if (asn1r_get_bool (aCtx, &info -> more_follows))	/* Read more_follows boolean.	*/
    {					/* If error reading integer,	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);		/* stop parse, report error	*/
    return;				/* and return.			*/
    }

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_namelist					*/
/* ENCODE and send a GetNameList REQUEST:				*/
/************************************************************************/

MMSREQ_PEND *mp_namelist (ST_INT chan, NAMELIST_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_NAMLIST,
			M_CAST_MK_FUN (_ms_mk_namel_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_namelist					*/
/* ENCODE a GetNameList REQUEST:					*/
/************************************************************************/

ST_RET   mpl_namelist (NAMELIST_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_NAMLIST,
			M_CAST_MK_FUN (_ms_mk_namel_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_namel_req					*/
/* ENCODE a GetNameList REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_namel_req (ASN1_ENC_CTXT *aCtx, NAMELIST_REQ_INFO *info_ptr)
  {

  if (info_ptr->cont_after_pres)
    {
    asn1r_wr_vstr (aCtx, info_ptr->continue_after);	/* write continue-after name	*/
    asn1r_fin_prim (aCtx, 2,CTX);
    }

  asn1r_strt_constr (aCtx);			/* start the object scope cstr	*/

  if (info_ptr->objscope == DOM_SPEC)	/* if domain specific		*/
    asn1r_wr_vstr (aCtx, info_ptr->dname);		/* write domain_name identifier */
  asn1r_fin_prim (aCtx, info_ptr->objscope,CTX);	/* either NULL or identifier 	*/

  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish the object scope cstr	*/

  asn1r_strt_constr (aCtx);			/* start extendedObjectClass	*/
#ifdef CS_SUPPORT
  if (info_ptr->cs_objclass_pres)	/* if companion standard	*/
    {
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
  asn1r_fin_constr (aCtx, MMSOP_GET_NAMLIST,CTX,DEF); /* complete GetNameList req	*/
  }

