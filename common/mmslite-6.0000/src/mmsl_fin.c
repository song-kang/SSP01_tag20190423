/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsl_fin.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common functions used to finish various 	*/
/*	types of MMS PDU's						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	ST_INT16 mms_fin_null_resp  (invoke_id,opcode)			*/
/*	ST_INT16 mms_fin_null_req   (opcode) 				*/
/*	ST_INT16 mms_resp_fin       (opcode,invoke_id,mk_fun,info_ptr) 	*/
/*	ST_INT16 mms_req_fin        (opcode,mk_ptr,info_ptr)	    	*/
/*	ST_INT16 mms_unconf_req_fin (opcode,mk_ptr,info_ptr)		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/03/10  JRB	   Replace "ml_log_*" with "mlogl_info" struct.	*/
/* 01/20/10  JRB     14    Chg invokeId to ST_ULONG in "ml_log_*".	*/
/* 12/18/07  MDE     13    Fixed request log mask		 	*/
/* 11/12/07  MDE     12    Pass MMSOP_READ to logging when usr handled 	*/
/* 04/09/07  MDE     11    Enhanced filtered logging 			*/
/* 03/11/04  GLB     10    Remove "thisFileName"                    	*/
/* 12/12/02  JRB     09    _mms_unconf_req_fin: Add ASN1_ENC_CTXT arg,	*/ 
/*			   do NOT call asn1r_strt_asn1_bld, do NOT set	*/
/*			   global vars "mmsl_msg_*".			*/
/* 04/08/02  MDE     08    Added log function pointers 			*/
/* 04/05/02  MDE     07    Cleaned up MMS Lite use of MLOG		*/
/* 01/18/02  JRB     06    Del _ms_fin_pdu_debug. Use new _ms_fin_pdu	*/
/*			   and _ms_fin_pdu_log (in mms_ced.c).		*/
/* 01/02/02  JRB     05    Converted to use ASN1R.			*/
/*			   Add ASN1_DEC_CTXT to (*mk_fun) calls in	*/
/*			   mms_req_fin, mms_resp_fin, mms_unconf_req_fin*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 03/20/98  JRB     02    Don't need mmsop_en.h anymore.		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mloguser.h"
#include "mvl_defs.h"
#include "mvl_log.h"
#include "asn1defs.h"

/************************************************************************/
/************************************************************************/
/* GENERAL PDU FINISH FUNCTIONS						*/
/************************************************************************/
/*			mms_fin_null_resp				*/
/* general NULL response function, used for prim NULL responses		*/
/************************************************************************/

ST_RET _mms_fin_null_resp (ST_UINT32 invoke_id, ST_INT opcode)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  asn1r_strt_asn1_bld (aCtx, mmsl_enc_buf,mmsl_enc_buf_size);	/* init the builder     */

#ifdef CS_SUPPORT			/* Check for CS information	*/
  if (cs_send.cs_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_delmnt (aCtx, cs_send.cs_ptr,cs_send.cs_len);
    cs_send.cs_pres = cs_send_reset_val;
    asn1r_fin_constr (aCtx, 79,CTX|CONSTR,DEF);
    }
#endif

/* write the context specific explicit asn1_tag for this opcode.		*/
  asn1r_fin_prim (aCtx, (ST_INT16)opcode,CTX); 	/* asn1_tag = opcode, ctx prim	*/

  asn1r_wr_u32 (aCtx, invoke_id);			/* write the invoke id		*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);

  mmsl_msg_start = _ms_fin_pdu (aCtx, MMSRESP, &mmsl_msg_len);
#ifdef DEBUG_SISCO
  _ms_fin_pdu_log (MMSRESP, mms_op_string[opcode],mmsl_msg_start,mmsl_msg_len);
#endif

  if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
    return (ME_ASN1_ENCODE_OVERRUN);

  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*	      		mms_fin_null_req				*/
/* general NULL request function, used for prim NULL requests		*/
/************************************************************************/

ST_RET _mms_fin_null_req (ST_INT opcode)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  asn1r_strt_asn1_bld (aCtx, mmsl_enc_buf,mmsl_enc_buf_size);	/* init the builder 	*/

#ifdef CS_SUPPORT			/* Check for CS information	*/
  if (cs_send.cs_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_delmnt (aCtx, cs_send.cs_ptr,cs_send.cs_len);
    cs_send.cs_pres = cs_send_reset_val;
    asn1r_fin_constr (aCtx, 79,CTX|CONSTR,DEF);
    }
#endif

/* Write the context specific explicit asn1_tag for this opcode on a NULL	*/
/* data element.							*/

  asn1r_fin_prim (aCtx, (ST_INT16)opcode, CTX);	/* asn1_tag = opcode, context	*/

#ifdef MOD_SUPPORT			/* check for modifier info	*/
  if ( modifier_list.info_pres )
    {
    asn1r_strt_constr (aCtx);
    wr_mms_modlist( modifier_list.mod_list_ptr, 
                    modifier_list.num_of_mods );
    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	
    modifier_list.info_pres = SD_FALSE;
    }
#endif

  asn1r_wr_u32 (aCtx, mmsl_invoke_id++);		/* write the invoke id		*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);

/* Complete the outer ASN.1 element					*/
  mmsl_msg_start = _ms_fin_pdu (aCtx, MMSREQ, &mmsl_msg_len);
#ifdef DEBUG_SISCO
  _ms_fin_pdu_log (MMSREQ, mms_op_string[opcode],mmsl_msg_start,mmsl_msg_len);
#endif

  if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun	*/
    {
    return (ME_ASN1_ENCODE_OVERRUN);
    }

/* OK, message has been build successfully				*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			mms_resp_fin					*/
/* general mp_xxxxxx response function, used for prim responses		*/
/************************************************************************/

ST_RET _mms_resp_fin (ST_INT opcode, ST_UINT32 invoke_id, 
		      ST_VOID (*mk_fun)(ASN1_ENC_CTXT *ac, ST_CHAR *info), ST_CHAR *info_ptr)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  asn1r_strt_asn1_bld (aCtx, mmsl_enc_buf,mmsl_enc_buf_size);	/* init the builder	*/

#ifdef CS_SUPPORT			/* Check for CS information	*/
  if (cs_send.cs_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_delmnt (aCtx, cs_send.cs_ptr,cs_send.cs_len);
    cs_send.cs_pres = cs_send_reset_val;
    asn1r_fin_constr (aCtx, 79,CTX|CONSTR,DEF);
    }
#endif

  (*mk_fun) (aCtx, info_ptr);
  
  asn1r_wr_u32 (aCtx, invoke_id);	      	/* write the invoke id		*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);

  mmsl_msg_start = _ms_fin_pdu (aCtx, MMSRESP, &mmsl_msg_len);
#ifdef DEBUG_SISCO
  _ms_fin_pdu_log (MMSRESP, mms_op_string[opcode],mmsl_msg_start,mmsl_msg_len);
#endif

  if (aCtx->asn1r_encode_overrun)    	/* Check for encode overrun 	*/
    return (ME_ASN1_ENCODE_OVERRUN);

  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*	      		mms_req_fin					*/
/* general mp_xxxxx request function, used for prim requests		*/
/************************************************************************/

ST_RET _mms_req_fin (ST_INT opcode, ST_VOID (*mk_ptr)(ASN1_ENC_CTXT *ac, ST_CHAR *info), 
		     ST_CHAR *info_ptr)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  asn1r_strt_asn1_bld (aCtx, mmsl_enc_buf,mmsl_enc_buf_size);	/* init the builder		*/

#ifdef CS_SUPPORT			/* Check for CS information	*/
  if (cs_send.cs_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_delmnt (aCtx, cs_send.cs_ptr,cs_send.cs_len);
    cs_send.cs_pres = cs_send_reset_val;
    asn1r_fin_constr (aCtx, 79,CTX|CONSTR,DEF);
    }
#endif

  (*mk_ptr)(aCtx, info_ptr); 			/* write the PDU		*/

#ifdef MOD_SUPPORT			/* check for modifier info	*/
  if ( modifier_list.info_pres )
    {
    asn1r_strt_constr (aCtx);
    wr_mms_modlist( modifier_list.mod_list_ptr, 
                    modifier_list.num_of_mods );
    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	
    modifier_list.info_pres = SD_FALSE;
    }
#endif

  asn1r_wr_u32 (aCtx, mmsl_invoke_id++); 		/* write the invoke id		*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);

  mmsl_msg_start = _ms_fin_pdu (aCtx, MMSREQ, &mmsl_msg_len);
#ifdef DEBUG_SISCO
  _ms_fin_pdu_log (MMSREQ, mms_op_string[opcode],mmsl_msg_start,mmsl_msg_len);
#endif

  if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun	*/
    {
    return (ME_ASN1_ENCODE_OVERRUN);
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*	      		mms_unconf_req_fin					*/
/* general mp_xxxxx request function, for unconfirmed requests		*/
/************************************************************************/

ST_RET _mms_unconf_req_fin (ASN1_ENC_CTXT *aCtx, ST_INT opcode, ST_VOID (*mk_ptr)(ASN1_ENC_CTXT *ac, ST_CHAR *info),
			   ST_CHAR *info_ptr)
  {
ST_UCHAR *msg_ptr;
ST_INT msg_len;

/* Make sure that "asn1r_strt_asn1_bld" was called for this context.	*/
  assert (aCtx->asn1r_magic == ASN1_ENC_MAGIC_NUMBER);

#ifdef CS_SUPPORT			/* Check for CS information	*/
  if (cs_send.cs_pres)
    {
    asn1r_strt_constr (aCtx);
    asn1r_wr_delmnt (aCtx, cs_send.cs_ptr,cs_send.cs_len);
    cs_send.cs_pres = cs_send_reset_val;
    asn1r_fin_constr (aCtx, 79,CTX|CONSTR,DEF);
    }
#endif

  (*mk_ptr)(aCtx, info_ptr); 			/* write the PDU		*/

  msg_ptr = _ms_fin_pdu (aCtx, MMSUNREQ, &msg_len);
#ifdef DEBUG_SISCO
  _ms_fin_pdu_log (MMSUNREQ, mms_op_string[opcode],msg_ptr,msg_len);
#endif

  if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
    return (ME_ASN1_ENCODE_OVERRUN);

  return (SD_SUCCESS);
  }



/************************************************************************/
/************************************************************************/
/*			mpl_resp_log					*/
/************************************************************************/

ST_VOID mpl_resp_log (MVL_IND_PEND *indCtrl, ST_VOID *respInfo) 
  {
  if ((mms_debug_sel & MMS_LOG_RESP) && mlogl_info.resp_info_fun != NULL)
    {
    if (mvl_conn_filtered_logging == SD_FALSE || indCtrl->event->net_info->log_enable)
      {
      (*mlogl_info.resp_info_fun) (indCtrl->event->u.mms.dec_rslt.id, 
     			       indCtrl->op == MMSOP_RD_USR_HANDLED ? MMSOP_READ : indCtrl->op, 
			       respInfo);
      }
    }
  }


/************************************************************************/
/*			mpl_req_log					*/
/************************************************************************/

ST_VOID mpl_req_log (MVL_REQ_PEND *reqCtrl, ST_VOID *reqInfo) 
  {
  if ((mms_debug_sel & MMS_LOG_REQ) && mlogl_info.req_info_fun != NULL)
    {
    if (mvl_conn_filtered_logging == SD_FALSE || reqCtrl->net_info->log_enable)
      {
      (*mlogl_info.req_info_fun) (reqCtrl->invoke_id, reqCtrl->op, reqInfo);
      }
    }
  }


/************************************************************************/
/*			mpl_unsol_req_log				*/
/************************************************************************/

ST_VOID mpl_unsol_req_log (MVL_NET_INFO *netInfo, ST_INT op, ST_VOID *reqInfo) 
  {
  if ((mms_debug_sel & MMS_LOG_REQ) && mlogl_info.unsol_req_info_fun != NULL)
    {
    if (mvl_conn_filtered_logging == SD_FALSE || netInfo->log_enable)
      {
      (*mlogl_info.unsol_req_info_fun) (op, reqInfo);
      }
    }
  }
