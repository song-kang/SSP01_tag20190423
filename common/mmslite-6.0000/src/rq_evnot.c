/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2004, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_evnot.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the event 	*/
/*	notification service.  It encodes the event notification	*/
/*	request.  There is no response (confirm).			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/28/05  JRB     05    Incl suicacse if MMS_LITE not defined.	*/
/* 09/17/04  EJV     04    Use new mms_chk_resource.			*/
/* 12/12/02  JRB     03    Add ASN1_ENC_CTXT arg to mpl_evnot		*/ 
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

#ifndef MMS_LITE
#include "mms_pevn.h"
#include "suicacse.h"	/* for SE_QUE_FULL only	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/*			mp_evnot					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the event notification request.		*/
/************************************************************************/

ST_RET mp_evnot (ST_INT chan, EVNOT_REQ_INFO *info_ptr)
  {
ST_RET rc;
ST_INT avail;

  /* locking here will assure that _mms_chk_resource will work in	*/
  /* multiple threads when mp_* functions are called to send		*/
  /* unconfirmed requests						*/
  S_LOCK_COMMON_RESOURCES ();

  avail = _mms_chk_resource (chan);
  if (avail > 0)
    rc = _mms_unconf_req_send (MMSOP_EVENT_NOT,
			M_CAST_MK_FUN (_ms_mk_evnot_req),
			chan,(ST_CHAR *)info_ptr);
  else
    rc = SE_QUE_FULL;

  S_UNLOCK_COMMON_RESOURCES ();

  return (rc);
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_evnot					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the event notification request.			*/
/************************************************************************/

ST_RET mpl_evnot (ASN1_ENC_CTXT *aCtx, EVNOT_REQ_INFO *info_ptr)
  {
  return (_mms_unconf_req_fin (aCtx, MMSOP_EVENT_NOT,
			M_CAST_MK_FUN (_ms_mk_evnot_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_evnot_req					*/
/* construct an event notification request.				*/
/************************************************************************/

ST_VOID _ms_mk_evnot_req (ASN1_ENC_CTXT *aCtx, EVNOT_REQ_INFO *info)
  {
  if (info->evact_result_pres)
    {					/* write event action result	*/
    asn1r_strt_constr (aCtx);			/* start context tag 8 cstr	*/
    asn1r_strt_constr (aCtx);			/* start ctx tag 0 or 1 cstr	*/

    if (info->evact_result_tag == 1)	/* failure, write service error	*/
      {					/*  then check for modifier pos	*/
      _ms_wr_service_err (aCtx, (info->serv_err)->eclass,(info->serv_err)->code,
				&(info->serv_err)->adtnl);
      asn1r_fin_constr (aCtx, 1, CTX, DEF);

#ifdef MOD_SUPPORT
      if (info->mod_pos_pres)
        {
        asn1r_wr_u32 (aCtx, info->mod_pos);
        asn1r_fin_prim (aCtx, 0, CTX);
        }
#endif
      }
    else			/* SD_SUCCESS	*/
      { 	
#ifdef CS_SUPPORT		/* first check for CS response detail	*/
      if (info->cs_rdetail_pres)
        {		       
        asn1r_strt_constr (aCtx);			/* start context tag 79 cstr	*/
        asn1r_wr_delmnt (aCtx, info->cs_rdetail, info->cs_rdetail_len);
        asn1r_fin_constr (aCtx, 79, CTX, DEF);		/* CS is cstr 79	*/
        }
#endif
      asn1r_wr_delmnt (aCtx, info->conf_serv_resp, info->conf_serv_resp_len);
      }
    asn1r_fin_constr (aCtx, info->evact_result_tag, CTX, DEF);	/* cstr 0 or 1	*/
    _ms_wr_mms_objname (aCtx, &info->evact_name); /* write event action name	*/
    asn1r_fin_constr (aCtx, 8, CTX, DEF);		  /* finish context tag 8 cstr	*/
    }

  if (info->alarm_ack_rule_pres)
    {					/* write alarm ack rule 	*/
    asn1r_wr_i16 (aCtx, info->alarm_ack_rule);
    asn1r_fin_prim (aCtx, 7, CTX);
    }

  if (info->not_lost)
    {					/* not the defalut		*/
    asn1r_wr_bool (aCtx, info->not_lost);		/* write notification lost	*/
    asn1r_fin_prim (aCtx, 6, CTX);
    }

  asn1r_strt_constr (aCtx);			/* start context tag 4 cstr	*/
  _ms_wr_mms_evtime (aCtx, &info->trans_time);	/* write event time		*/
  asn1r_fin_constr (aCtx, 4, CTX, DEF);		/* finish context tag 4 cstr	*/

  if (info->cur_state_pres)
    {
    asn1r_wr_i16 (aCtx, info->cur_state);		/* write the current state	*/
    asn1r_fin_prim (aCtx, 3, CTX);
    }

  asn1r_wr_u8 (aCtx, info->severity);		/* write the severity		*/
  asn1r_fin_prim (aCtx, 2, CTX);

  asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
  _ms_wr_mms_objname (aCtx, &info->evcon_name);	/* write event condition name	*/
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish the ev con name cstr	*/

  asn1r_strt_constr (aCtx);			/* start event enrollment name  */
  _ms_wr_mms_objname (aCtx, &info->evenroll_name);/* write event enrollment name	*/
  asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish eventenrollment cstr	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* tag = opcode, ctx		*/
  }
