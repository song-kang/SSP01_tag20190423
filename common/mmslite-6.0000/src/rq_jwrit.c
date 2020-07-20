/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_jwrite.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of write journal.	*/
/*	It decodes the write journal response (confirm) & encodes the 	*/
/*	write journal request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     03    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pjou.h"
#include "asn1defs.h"
#include "mem_chk.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/************************************************************************/
/*			mms_jwrite_rsp					*/
/* NOTE : Since the mms_jwrite_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jwrite					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the write journal request.		*/
/************************************************************************/

MMSREQ_PEND *mp_jwrite (ST_INT chan, JWRITE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_WRITE_JOURNAL,
			M_CAST_MK_FUN (_ms_mk_jwrite_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jwrite					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the write journal request.				*/
/************************************************************************/

ST_RET  mpl_jwrite (JWRITE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_WRITE_JOURNAL,
			M_CAST_MK_FUN (_ms_mk_jwrite_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_jwrite_req					*/
/* ENCODE a write journal REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_jwrite_req (ASN1_ENC_CTXT *aCtx, JWRITE_REQ_INFO *info)
  {
ST_INT	 i;
ENTRY_CONTENT **ec_list;
ENTRY_CONTENT	*ec_ptr;
VAR_INFO	*vi_ptr;
ST_INT	 ecl_size;

  ecl_size = sizeof (ENTRY_CONTENT *) * info->num_of_jou_entry;
  ec_list  = (ENTRY_CONTENT **) chk_calloc (1, ecl_size);

					/* find first entry content	*/
  ec_ptr       = (ENTRY_CONTENT *) (info + 1);
  for (i = 0; i < info->num_of_jou_entry; i++)
    { 					/* find ptrs to entry contents 	*/
					/* if variable list follows	*/
    *(ec_list++) = ec_ptr;
    if (ec_ptr->entry_form_tag == 2 && ec_ptr->ef.data.list_of_var_pres)
      {
      vi_ptr  = (VAR_INFO *) (ec_ptr + 1);
      vi_ptr += ec_ptr->ef.data.num_of_var;
      ec_ptr = (ENTRY_CONTENT *) vi_ptr;
      }
    else
      ec_ptr++;				/* annoatation or no var list	*/

    }

  asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
  for (i = 0; i < info->num_of_jou_entry; i++)
    {
    ec_ptr   = *(--ec_list);
    asn1r_strt_constr (aCtx);			/* start universal seq cstr	*/
    _ms_wr_jou_ent_cont (aCtx, ec_ptr); 		/* write the entry content 	*/
    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF); 	/* finish universal seq cstr	*/
    }
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish context tag 1 cstr	*/
  chk_free (ec_list);

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  _ms_wr_mms_objname (aCtx, &info->jou_name);	/* write the journal name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_WRITE_JOURNAL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
