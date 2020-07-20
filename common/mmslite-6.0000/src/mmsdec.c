/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2010, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsdec.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE				    	*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the initial MMS message decode functions.	*/
/*	Decodes the header portion of MMS PDUs. 			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/05/10  JRB	   Del redundant initialization of rslt_ptr.	*/
/* 02/24/10  JRB	   Del _mmsdec_rslt,  use aCtx->mmsdec_rslt	*/
/*			   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/*			   Del _mms_dec_info_pres, use aCtx->mmsdec_rslt->data_pres*/
/*			   _ms_mms_decode: init aCtx->mmsdec_rslt to all 0*/
/* 01/20/10  JRB     11    Fix log messages.				*/
/* 03/17/08  EJV     10	   Added slog.					*/    
/* 12/19/05  JRB     22    Chg cr to cr_mms, del static to fix warning.*/
/* 09/07/05  JRB     08    Fix backwards client/server error log msgs.	*/
/* 07/24/02  EJV     07    mmsdec_ucreq_opcode: corr err in the check	*/
/*			     opcode due to recent MMSOP_xxx changes.	*/
/* 12/20/01  JRB     06    Converted to use ASN1R (re-entrant ASN1)	*/
/*			   Del mms_err_invoke_fun proto (see mms_dfun.h)*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 04/07/99  MDE     04    Logging improvements (unsupported service)	*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 03/18/98  JRB     01    Fix mmsdec_map_err for new error values.	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_err.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/

static ST_VOID mmsdec_start (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code);
static ST_VOID mmsdec_invoke_id (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code);
#ifdef MOD_SUPPORT 		       /* support modifiers 		*/
static ST_VOID mmsdec_modlist (ASN1_DEC_CTXT *aCtx, ST_UINT16 el_id);
static ST_VOID  mmsdec_modlist_done_fun (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID  mmsdec_opcode (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code);
static ST_VOID  mmsdec_ucreq_opcode (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code);
static ST_RET   mmsdec_chk_req_op (ASN1_DEC_CTXT *aCtx, ST_INT opcode, ST_INT unsupp_code);
static ST_RET   mmsdec_chk_resp_op (ASN1_DEC_CTXT *aCtx, ST_UINT opcode);
static ST_VOID  mmsdec_map_err (ASN1_DEC_CTXT *aCtx);    /* maps ASN.1 tool errors to MMS	*/
#ifndef MMS_LITE 		       /* don't assign op id's 		*/
static ST_VOID  mmsdec_assign_rid (ASN1_DEC_CTXT *aCtx, ST_INT opcode);
#endif

/************************************************************************/
const ST_CHAR cr_mms[] =
 "(c) COPYRIGHT SYSTEMS INTEGRATION SPECIALISTS COMPANY INC., 1986 - 2005.  All Rights Reserved.";
/************************************************************************/
/* Operation Specific Decode Buffer Management				*/

ST_VOID *(*m_calloc_os_fun)(ST_UINT num, ST_UINT size);
ST_VOID *(*m_realloc_os_fun)(ST_VOID *old, ST_UINT new_size);
ST_VOID  (*m_free_os_fun)(ST_VOID *buf);

/************************************************************************/
/* The array below is used to check the constructed state of a PDU	*/

SD_CONST static ST_UINT16 mmspdu_cstr_chk[] =
   {
   CONSTR,		/* CONFIRMED REQUEST IS CONSTRUCTOR		*/
   CONSTR,		/* CONFIRMED RESPONSE				*/
   CONSTR,		/* CONFIRMED ERROR				*/
   CONSTR,		/* UNCONFIRMED PDU				*/
   CONSTR,		/* REJECT					*/
   0,			/* CANCEL REQUEST				*/
   0,			/* CANCEL RESPONSE				*/
   CONSTR,		/* CANCEL ERROR					*/
   CONSTR,		/* INITIATE REQUEST				*/
   CONSTR,		/* INITIATE RESPONSE				*/
   CONSTR,		/* INITIATE ERROR				*/
   0,			/* CONCLUDE REQUEST IS PRIMITIVE		*/
   0,			/* CONCLUDE RESPONSE				*/
   CONSTR		/* CONCLUDE ERROR IS CONSTRUCTOR		*/
   };


/************************************************************************/
/************************************************************************/
/*			_ms_mms_decode					*/
/* Function to setup and initiate MMS PDU decode			*/
/* Only fourteen context specific codes are legal at start		*/
/* Input parameters : pointer to message start, length of message,	*/
/* pointer to result buffer.						*/
/************************************************************************/

ST_RET _ms_mms_decode (ST_UCHAR *msg_ptr, ST_INT len, MMSDEC_INFO *rslt_ptr)
  {
ASN1_DEC_CTXT localDecCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_DEC_CTXT *aCtx = &localDecCtx;

  MLOG_DEC0 ("Decoding MMS PDU");
  memset (aCtx, 0, sizeof (ASN1_DEC_CTXT));	/* CRITICAL: start clean.	*/

  aCtx->asn1r_decode_method = ASN1_CLASS_METHOD; /* select the class method	*/
  aCtx->asn1r_c_id_fun = mmsdec_start;	/* point to mms decode initial function */
  aCtx->asn1r_u_id_fun = asn1r_class_err; 	/* no universals to start		*/
  aCtx->asn1r_a_id_fun = asn1r_class_err; 	/* no applications to start		*/
  aCtx->asn1r_p_id_fun = asn1r_class_err; 	/* no privates to start 		*/

  aCtx->asn1r_decode_done_fun = asn1r_done_err; /* done function = error fun 	*/
  aCtx->asn1r_err_fun = NULL;		   /* set up error detected fun 	*/

  _mmsdec_msglen = len;			/* save decode message length	*/
  _mmsdec_msgptr = msg_ptr; 		/* save decode message ptr	*/

  /* Save result ptr in aCtx & initialize all structure members.	*/
  aCtx->mmsdec_rslt = rslt_ptr;
  memset (rslt_ptr, 0, sizeof (MMSDEC_INFO));	/* CRITICAL: start clean.*/
#if 0	/* These are initialized automatically by calling memset above.	*/
  rslt_ptr->err_code = NO_DECODE_ERR;	/* start with no error		*/
  rslt_ptr->dec_level = 0;		/* start with no usable info	*/
  rslt_ptr->data_pres = SD_FALSE;
  rslt_ptr->mods.info_pres = SD_FALSE;
  rslt_ptr->cs.cs_pres = SD_FALSE;
#endif	/* These are initialized automatically by calling memset above.	*/

/* ASN.1 decode machine is set up for initial MMS decode.		*/
/* Call 'asn1r_decode_asn1' to parse the message.                             */

  asn1r_decode_asn1 (aCtx, msg_ptr,len);		/* do message decode		*/
  mmsdec_map_err (aCtx);			/* write the error code 	*/
  if (aCtx->asn1r_pdu_dec_err == NO_DECODE_ERR)
    return (SD_SUCCESS); 			/* return 'no error' code       */
#ifndef MMS_LITE
  else
    {
    if (aCtx->mmsdec_rslt->mods.info_pres)	/* see if there were any mods	*/
      chk_free (aCtx->mmsdec_rslt->mods.mod_list_ptr);

/* Note that op specific buffer are free'd in _mms_dec_buf_free on error	*/

    }
#endif
  return (MVE_ASN1_DECODE_ERR);		/* return error code		*/
  }

/************************************************************************/
/************************************************************************/
/*			mmsdec_start					*/
/* Initial MMS decode context-specific code service function.  This	*/
/* function is called when the first data element of the message is a	*/
/* context-specific class.						*/
/************************************************************************/

#define MAX_MMS_PDU_TAG		13

static ST_VOID mmsdec_start (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code)
  {
  MLOG_CDEC0 ("mmsdec_start");

/* check for valid MMS PDU type, such as request, response, error, etc. */
/* if valid tag and constructed	state OK				*/
  if (id_code <= MAX_MMS_PDU_TAG &&
			 !(mmspdu_cstr_chk[id_code] ^ aCtx->asn1r_constr_elmnt))
    {				
    aCtx->mmsdec_rslt->dec_level = 1;  /* set flag that PDU type is OK	*/
    aCtx->mmsdec_rslt->type = id_code; /* save the type for the user	*/
    switch (id_code)
      {
      case MMSREQ :			/* Confirmed Request PDU	*/
	aCtx->asn1r_u_id_fun = mmsdec_invoke_id;	/* next must be uni, integer 	*/
					/* invoke ID			*/
	aCtx->asn1r_c_id_fun = asn1r_class_err;		/* can not context specific	*/
	break;

      case MMSRESP :			/* Confirmed Response PDU	*/
	aCtx->asn1r_u_id_fun = mmsdec_invoke_id;	/* next must be uni, integer 	*/
					/* invoke ID			*/
	aCtx->asn1r_c_id_fun = asn1r_class_err;		/* cannot be context specific	*/
	break;

      case MMSERROR :			/* Confirmed Error PDU		*/
	aCtx->asn1r_c_id_fun = mms_err_invoke_fun;	/* must be context specific	*/
	break;		        	/* integer (invoke ID)		*/

      case MMSUNREQ :			/* Unconfirmed PDU		*/
	aCtx->asn1r_c_id_fun = mmsdec_ucreq_opcode;	/* expect context-specific tag	*/
	break;

      case MMSREJECT :			/* Reject PDU			*/
	aCtx->asn1r_c_id_fun = mms_reject;		/* expect context-specific	*/
	break;

      case MMSCANREQ :			/* Cancel Request PDU		*/
        if (!(mmsop_en[MMSOP_CANCEL] & RESP_EN))	/* protocol error if	*/
          asn1r_set_dec_err (aCtx, CANREQ_UNSPECIFIED);	/* unsupported service	*/
        else
          {
          if (asn1r_get_u32 (aCtx, &aCtx->mmsdec_rslt->id))
	    asn1r_set_dec_err (aCtx, CANREQ_INVAL_INVOKE);
          else
            {
            aCtx->mmsdec_rslt->dec_level = 2;
            (*mms_req_decode_fun [MMSOP_CANCEL]) (aCtx);
            }
          }
	break;
		     
      case MMSCANRESP : 		/* Cancel Response PDU		*/
	if (!(mmsop_en[MMSOP_CANCEL] & REQ_EN))	/* protocol error if	*/
	  asn1r_set_dec_err (aCtx, CANRESP_UNSPECIFIED);	/* not supported	*/
        else
	  {
          if (asn1r_get_u32 (aCtx, &aCtx->mmsdec_rslt->id))
	    asn1r_set_dec_err (aCtx, CANRESP_INVAL_INVOKE);
          else
            {
            aCtx->mmsdec_rslt->dec_level = 2;		/* Save inv. ID in rslt */
            (*mms_rsp_decode_fun [MMSOP_CANCEL]) (aCtx);
            }
          }
	break;

      case MMSCANERR :			/* Cancel Error PDU		*/
	if (!(mmsop_en[MMSOP_CANCEL] & REQ_EN))	/* protocol error if	*/
	  asn1r_set_dec_err (aCtx, CANERR_INVAL_SERV);	/* is not supported	*/
        else					/* expect context-spec	*/
	  aCtx->asn1r_c_id_fun = mms_err_invoke_fun;
	break;			      
 
      case MMSINITREQ : 		/* Initiate Request PDU		*/
    	if (!(mmsop_en[MMSOP_INITIATE] & RESP_EN))	/* if not enabled	*/
          asn1r_set_dec_err (aCtx, REQ_UNREC_SERV);		/* as responder		*/
        else
          {				  
#ifndef MMS_LITE
          mmsdec_assign_rid (aCtx, MMSOP_INITIATE);	/* Assign opcode and	*/
						/* reserved Initiate ID	*/
#endif
	  (*mms_req_decode_fun [MMSOP_INITIATE]) (aCtx);
          }
        break;

      case MMSINITRESP :	       /* Initiate Response PDU		*/
    	if (!(mmsop_en[MMSOP_INITIATE] & REQ_EN))	/* if not enabled	*/
          asn1r_set_dec_err (aCtx, RESP_UNREC_SERV);		/* as responder		*/
        else
          {	     
#ifndef MMS_LITE
          mmsdec_assign_rid (aCtx, MMSOP_INITIATE);	/* Assign opcode and	*/
					        /* reserved Initiate ID	*/
#endif
	  (*mms_rsp_decode_fun [MMSOP_INITIATE]) (aCtx);
          }
        break;

      case MMSINITERR : 		/* Initiate Error PDU		*/
	if (!(mmsop_en[MMSOP_INITIATE] & REQ_EN))	/* protocol error if	*/
	  asn1r_set_dec_err (aCtx, ERR_UNREC_SERV);		/* is not supported	*/
        else
	  {
#ifndef MMS_LITE
	  mmsdec_assign_rid (aCtx, MMSOP_INITIATE);	/* Assign opcode and	*/
						/* reserved Initiate ID	*/
#endif
  	  mms_err_rsp (aCtx); 			/* general err function	*/
          }
	break;

      case MMSCNCLREQ : 		/* Conclude Requset PDU 	*/
	if (!(mmsop_en[MMSOP_CONCLUDE] & RESP_EN))	/* protocol error if	*/
	  asn1r_set_dec_err (aCtx, CONREQ_UNSPECIFIED);	/* not supported	*/
        else
	  {
	  if (aCtx->asn1r_elmnt_len)
	    asn1r_set_dec_err (aCtx, CONREQ_INVAL_ARG); 	
          else
	    {
#ifndef MMS_LITE
	    mmsdec_assign_rid (aCtx, MMSOP_CONCLUDE);/* Assign opcode and	*/
						/* reserved Conclude ID */
#endif
            (*mms_req_decode_fun [MMSOP_CONCLUDE]) (aCtx);
   	    }
          }
	break;

      case MMSCNCLRESP :		/* Conclude Response PDU	*/
	if (!(mmsop_en[MMSOP_CONCLUDE] & REQ_EN))	/* protocol error if	*/
	  asn1r_set_dec_err (aCtx, CONRESP_UNSPECIFIED);	/* not supported	*/
        else
	  {
	  if (aCtx->asn1r_elmnt_len)
	     asn1r_set_dec_err (aCtx, CONRESP_INVAL_RSLT);	/* if constructor	*/
          else
	    {
#ifndef MMS_LITE
	    mmsdec_assign_rid (aCtx, MMSOP_CONCLUDE); /* Assign opcode and	*/
		 				  /* reserved Concl ID	*/
#endif
            (*mms_rsp_decode_fun [MMSOP_CONCLUDE]) (aCtx);
 	    }
 	  }
	break;

      case MMSCNCLERR : 		/* Conclude Error PDU		*/
	if (!(mmsop_en[MMSOP_CONCLUDE] & REQ_EN))	/* protocol error if	*/
	  asn1r_set_dec_err (aCtx, CONERR_INVAL_SERV);	/* is not supported	*/
	else
	  {
#ifndef MMS_LITE
	  mmsdec_assign_rid (aCtx, MMSOP_CONCLUDE); /* Assign opcode and	*/
						/* reserved Conclude ID */
#endif
	  mms_err_rsp (aCtx); 			/* general err function	*/
	  }
	break;

      default:
	asn1r_set_dec_err (aCtx, PDU_UNKNOWN_TYPE);	/* Not a legal MMS PDU type.	*/
      }
    }
  else
    asn1r_set_dec_err (aCtx, PDU_UNKNOWN_TYPE);	/* Not a legal MMS PDU type.	*/
  }					/* or constructor error		*/


/************************************************************************/
/************************************************************************/
/*			mmsdec_invoke_id   				*/
/* Universal class service function called when invoke ID is expected	*/
/* (after first data element in most MMS confirmed services has been	*/
/* decoded, and this data element is a universal).			*/
/************************************************************************/

static ST_VOID mmsdec_invoke_id (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code)
  {
  if (aCtx->asn1r_constr_elmnt || id_code != INT_CODE)
    asn1r_set_dec_err (aCtx, PDU_INVALID);	/* not a constr or INTEGER type */
  else
    {
    if (asn1r_get_u32 (aCtx, &aCtx->mmsdec_rslt->id))
      {
      if (aCtx->mmsdec_rslt->type == MMSREQ)
	asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);    /* not a valid 4-byte integer	*/
      else
	asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);   /* not a valid 4-byte integer	*/
      return;
      }
    }

  MLOG_CDEC1 ("Invoke ID = %lu",(ST_ULONG)aCtx->mmsdec_rslt->id);

  aCtx->mmsdec_rslt->dec_level = 2;   /* set flag that Invoke ID is OK	*/
				/* setup function pointers for next	*/
  aCtx->asn1r_c_id_fun = mmsdec_opcode;	/* next is explicit context tag 	*/
#ifdef MOD_SUPPORT
  if (aCtx->mmsdec_rslt->type == MMSREQ)/* if MMSREQ a modifier could be next */
    aCtx->asn1r_u_id_fun = mmsdec_modlist; 	/* modifiers are tagged universal	*/
  else
#endif
    aCtx->asn1r_u_id_fun = asn1r_class_err; 	/* no more universals			*/
  }

#ifdef MOD_SUPPORT
/************************************************************************/
/************************************************************************/
/*			mmsdec_modlist					*/
/* An MMS request has been received with a modifier list present.  We	*/
/* change the asn1_decode_method to TAG so _ms_get_mms_modlist can decode it.	*/
/* During the decode the modifier list will be attached to aCtx->mmsdec_rslt.*/
/* Upon completion of the decode we see if we can give back any unused	*/
/* memory and change the decod_method back to CLASS.			*/
/************************************************************************/

static ST_VOID mmsdec_modlist (ASN1_DEC_CTXT *aCtx, ST_UINT16 el_id)
  {
#ifdef MMS_LITE
extern ST_CHAR mms_dec_modbuf [];
#endif

  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;		/* select TAG method for get_mms_modifer*/
  
  aCtx->mmsdec_rslt->mods.num_of_mods = 0;
#ifndef MMS_LITE
  aCtx->mmsdec_rslt->mods.mod_list_ptr = (MODIFIER *)
                                   chk_calloc (m_max_mods,
                                               sizeof (MODIFIER));
#else
  aCtx->mmsdec_rslt->mods.mod_list_ptr = (MODIFIER *) mms_dec_modbuf;
#endif
  _ms_get_mms_modlist (aCtx, aCtx->mmsdec_rslt->mods.mod_list_ptr,
                   &(aCtx->mmsdec_rslt->mods.num_of_mods),
                   m_max_mods,
		   mmsdec_modlist_done_fun);
  }

/************************************************************************/
/************************************************************************/
/*			mmsdec_modlist_done_fun				*/
/* This function is called when the modifier list has been decoded.	*/
/************************************************************************/

static ST_VOID mmsdec_modlist_done_fun (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_decode_method = ASN1_CLASS_METHOD; /* restore the class method	*/
  aCtx->mmsdec_rslt->mods.info_pres = SD_TRUE;
  
  }
#endif  

/************************************************************************/
/************************************************************************/
/*			mmsdec_opcode					*/
/* An MMS request with good invoke ID has been detected. Next field	*/
/* should be an implicit context tag which represents the opcode.	*/
/* This function is called if a context class is decoded.		*/
/************************************************************************/

static ST_VOID mmsdec_opcode (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code)
  {
ST_INT operr;

#ifndef MMS_LITE
  if (id_code > MAX_MMS_OPCODE)
    {				/* Must be a legal MMS opcode		*/
    if (aCtx->mmsdec_rslt->type == MMSREQ)
	asn1r_set_dec_err (aCtx, REQ_UNREC_SERV);
    else
	asn1r_set_dec_err (aCtx, RESP_UNREC_SERV);
    return;
    }
#endif

  aCtx->mmsdec_rslt->dec_level = 3;    /* set flag that opcode is read OK	*/
  aCtx->mmsdec_rslt->op = id_code;     /* save the opcode for the user	*/

  MLOG_CDEC1 ("MMS Opcode = %d",aCtx->mmsdec_rslt->op);

/* Opcode has been saved.  Check that the service corresponding to this */
/* opcode is supported and there are no parse errors.			*/

  if (aCtx->mmsdec_rslt->type == MMSREQ)
    {
    						/* pass err code	*/
    operr = mmsdec_chk_req_op (aCtx, aCtx->mmsdec_rslt->op,REQ_UNREC_SERV);
    if (!operr)
      (*mms_req_decode_fun [aCtx->mmsdec_rslt->op]) (aCtx);
    }
  else
    {
    operr = mmsdec_chk_resp_op (aCtx, aCtx->mmsdec_rslt->op);
    if (!operr)
      (*mms_rsp_decode_fun [aCtx->mmsdec_rslt->op]) (aCtx);
    }
  }


/************************************************************************/
/************************************************************************/
/*			mmsdec_ucreq_opcode				*/
/* An MMS unconfirmed request has been detected.  Next field should be	*/
/* an implicit context tag which represents the opcode.  This function	*/
/* is called if a context class is decoded.				*/
/************************************************************************/

static ST_VOID mmsdec_ucreq_opcode (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code)
  {
  if (id_code == 0)
    aCtx->mmsdec_rslt->op = MMSOP_INFO_RPT;
  else if (id_code == 1)
    aCtx->mmsdec_rslt->op = MMSOP_USTATUS;
  else if (id_code == 2)
    aCtx->mmsdec_rslt->op = MMSOP_EVENT_NOT;
  else
    {				/* Must be a legal MMS opcode		*/
    asn1r_set_dec_err (aCtx, PDU_INVALID);
    return;
    }

  MLOG_CDEC1 ("Unconfirmed request opcode = %d",aCtx->mmsdec_rslt->op);

  aCtx->mmsdec_rslt->dec_level = 3;    /* set flag that opcode is read OK	*/
  aCtx->mmsdec_rslt->id = UNREQ_INVOKE;

/* Opcode has been saved.  Check that the service corresponding to this */
/* opcode is supported and there are no parse errors.			*/

  if (!mmsdec_chk_req_op (aCtx, aCtx->mmsdec_rslt->op,UNCONF_UNREC_SERV))
    (*mms_req_decode_fun [aCtx->mmsdec_rslt->op]) (aCtx);	/* continue decode	*/
  }


/************************************************************************/
/*			     mmsdec_chk_req_op				*/
/*			     mmsdec_chk_resp_op				*/
/* This function checks whether the given opcode is supported as	*/
/* request or response (or both), if the data element containing the	*/
/* opcode is of the proper form (either primitive or constructor),	*/
/* and if it is a NULL when it is supposed to be.  The mmsop_en table	*/
/* contains the opcode support information.				*/
/* Returns 0 for success, else non-zero error code			*/
/************************************************************************/

static ST_RET mmsdec_chk_req_op (ASN1_DEC_CTXT *aCtx, ST_INT opcode, ST_INT unsupp_code)
  {
ST_UCHAR op_en;

/* First check that opcode range, then whether it is enabled		*/

  if ((opcode > MAX_MMSOP_DIS) || ! ((op_en = mmsop_en[opcode]) & RESP_EN))
    {
    if (opcode <= MAX_MMSOP_DIS)
      {
      MLOG_NERR1 ("MMS service %s not supported as server (cannot decode request)", 
					mms_op_string[opcode]);
      }
    else
      {
      MLOG_NERR1 ("MMS service opcode %d out of range", opcode);
      }
    asn1r_set_dec_err (aCtx, unsupp_code);			/* req not supported	*/
    return (MVE_REQ_NOT_SUPP);
    }
  else
    {
    if ((aCtx->asn1r_constr_elmnt && !(op_en & 0x04)) ||
	 (!aCtx->asn1r_constr_elmnt && (op_en & 0x04)) ||
         (aCtx->asn1r_elmnt_len > 0 && (op_en & 0x10)))
      {
      asn1r_set_dec_err (aCtx, PDU_INVALID);
      return (MVE_INVALID_PDU);
      }
    return (SD_SUCCESS);			/* return success code			*/
    }
  }

/************************************************************************/
static ST_RET mmsdec_chk_resp_op (ASN1_DEC_CTXT *aCtx, ST_UINT opcode)
  {
ST_UCHAR op_en;

/* First check that opcode range, then whether it is enabled		*/

  if ((opcode > MAX_MMSOP_DIS) || ! ((op_en = mmsop_en[opcode]) & REQ_EN))
    {
    if (opcode <= MAX_MMSOP_DIS)
      {
      MLOG_NERR1 ("MMS service %s not supported as client (cannot decode response)", 
					mms_op_string[opcode]);
      }
    else
      {
      MLOG_NERR1 ("MMS service opcode %d out of range", opcode);
      }

    asn1r_set_dec_err (aCtx, RESP_UNREC_SERV);		/* resp not supported	*/
    return (MVE_RESP_NOT_SUPP);
    }
  else
    {
    if (opcode == MMSOP_INIT_DOWNLOAD)
      {
#ifndef MMS_LITE
      if (mms_chan_info[_mmsdechan].version)	/* IS 	*/
#else
      if (mmsl_version)				/* IS 	*/
#endif
        {
        if (aCtx->asn1r_constr_elmnt || aCtx->asn1r_elmnt_len)
          {
          asn1r_set_dec_err (aCtx, PDU_INVALID);
          return (MVE_INVALID_PDU);
          }
        }
      else					/* DIS	*/
        {
        if (!aCtx->asn1r_constr_elmnt || !aCtx->asn1r_elmnt_len)
          {
          asn1r_set_dec_err (aCtx, PDU_INVALID);
          return (MVE_INVALID_PDU);
          }
        }
      }
    else
      {
      if ((aCtx->asn1r_constr_elmnt && !(op_en & 0x08)) ||
  	  (!aCtx->asn1r_constr_elmnt && (op_en & 0x08)) ||
          (aCtx->asn1r_elmnt_len > 0 && (op_en & 0x20)))
        {
        asn1r_set_dec_err (aCtx, PDU_INVALID);
        return (MVE_INVALID_PDU);
        }
      }

    return (SD_SUCCESS);			/* return success code 		*/
    }
  }


/************************************************************************/
/*			mmsdec_map_err					*/
/* Function to map the error codes supplied by the generic ASN.1 tools	*/
/* to the MMS error codes specified for the Reject service.  Inputs are */
/*	aCtx->asn1r_pdu_dec_err;					*/
/*	aCtx->mmsdec_rslt;						*/
/*									*/
/* (A aCtx->asn1r_pdu_dec_err value of 0 should not occur and maps to UNSPECIFIED.) */
/************************************************************************/
static ST_VOID mmsdec_map_err (ASN1_DEC_CTXT *aCtx)
  {
ST_INT level;

  level = 0;

/* asn1_pdu_dec_err codes of NO_DECODE_ERR are OK, and codes of > 15 have been	*/
/* set by the MMS decode logic and so are OK.  15 is a special number	*/
/* because the lowest reject code is a 0x10(REQ_UNSPECIFIED)		*/

  if (aCtx->asn1r_pdu_dec_err == NO_DECODE_ERR || aCtx->asn1r_pdu_dec_err >= REQ_UNSPECIFIED)
    {
    aCtx->mmsdec_rslt->err_code = aCtx->asn1r_pdu_dec_err;
    }

/* We started to decode some type of PDU and it wasn't valid.  Here we	*/
/* try to determine how much of the PDU we parsed and generate a 	*/
/* pertainent error code						*/

  else
    {
    level = aCtx->mmsdec_rslt -> dec_level;
    if (level == 0)			/* did we determine it's type ?	*/
      aCtx->mmsdec_rslt->err_code = PDU_INVALID;
    else				/* if we knew it's type we 	*/
      {					/* switch to figure out more	*/
      switch (aCtx->mmsdec_rslt->type)
        {

/* About the only time we get here and have to do this is when the MMS	*/
/* decoder temporarily turned the decode over the the ASN.1 tools (via	*/
/* parse_next, parse_cstr_contents, etc) and an error was discovered.	*/
/* The error the ASN.1 tools will report is <= 15.  Some, most or all	*/
/* of the PDU was decoded.  It's not fair to just say we couldn't tell	*/
/* anything at all about the PDU.					*/
/* These are the rules for determining what reject class and code get	*/
/* returned for the rejected PDU.  The protocol for the begining of	*/
/* each MMSpdu is different so almost each type has to dealt with on an	*/
/* individual basis.							*/

        case MMSREQ :		/* request for most confirmed services	*/
          if (level >= 2)
            aCtx->mmsdec_rslt->err_code = REQ_INVAL_ARG;
          else
            aCtx->mmsdec_rslt->err_code = REQ_UNSPECIFIED;
 	  break;

        case MMSRESP :		/* response for most confirmed services */
          if (level >= 2)
            aCtx->mmsdec_rslt->err_code = RESP_INVAL_RSLT;
          else
            aCtx->mmsdec_rslt->err_code = RESP_UNSPECIFIED;
	  break;

        case MMSERROR :		/* error for most confirmed services	*/
          aCtx->mmsdec_rslt->err_code = ERR_UNSPECIFIED;
          break;

        case MMSUNREQ :		/* request for unconfirmed services	*/
          aCtx->mmsdec_rslt->err_code = UNCONF_UNSPECIFIED;
          break;

        case MMSREJECT :	/* reject				*/
          aCtx->mmsdec_rslt->err_code = PDU_UNKNOWN_TYPE;
          break;

        case MMSCANREQ :	/* request for the Cancel service	*/
          aCtx->mmsdec_rslt->err_code = CANREQ_UNSPECIFIED;
          break;

        case MMSCANRESP : 	/* response for the Cancel service	*/
          aCtx->mmsdec_rslt->err_code = CANRESP_UNSPECIFIED;
          break;

        case MMSCANERR :	/* error for Cancel service	*/
          aCtx->mmsdec_rslt->err_code = CANERR_UNSPECIFIED;
          break;

        case MMSINITREQ : 	/* request for Initiate service */
          aCtx->mmsdec_rslt->err_code = REQ_BAD_VALUE;
          break;

        case MMSINITRESP :	/* response for Initiate service */
          aCtx->mmsdec_rslt->err_code = RESP_BAD_VALUE;
          break;

        case MMSINITERR : 	/* error for Initiate service	*/
          aCtx->mmsdec_rslt->err_code = ERR_BAD_VALUE;
          break;

        case MMSCNCLREQ : 	/* request for Conclude service 	*/
          aCtx->mmsdec_rslt->err_code = CONREQ_UNSPECIFIED;
          break;

        case MMSCNCLRESP :	/* response for Conclude service	*/
          aCtx->mmsdec_rslt->err_code = CONRESP_UNSPECIFIED;
          break;

        case MMSCNCLERR : 	/* error for Conclude service	*/
          aCtx->mmsdec_rslt->err_code = CONERR_UNSPECIFIED;
          break;

        default:
          aCtx->mmsdec_rslt->err_code = PDU_INVALID;
          break;
        }
      }
    }
  }

/************************************************************************/
/*			mmsdec_assign_rid 				*/
/* Assign a reserved invoke ID for the given service on this channel	*/
/* and store it in the mmsdec_rslt structure.  Also, set the opcode for */
/* the specified service, which must be either Initiate or Conclude	*/
/* (or their error responses).						*/
/************************************************************************/

#ifndef MMS_LITE

static ST_VOID mmsdec_assign_rid (ASN1_DEC_CTXT *aCtx, ST_INT opcode)
  {
  					/* assign reserved invoke id	*/
  if (opcode == MMSOP_INITIATE)		
    aCtx->mmsdec_rslt->id = INIT_INVOKE_ID + _mmsdechan;	/* for Initiate	*/
  else					
    aCtx->mmsdec_rslt->id = CONCL_INVOKE_ID + _mmsdechan;	/* for Conclude	*/

  aCtx->mmsdec_rslt->op = opcode;		/* save the opcode  		*/
  aCtx->mmsdec_rslt->dec_level = 3;		/* opcode & invoke ID are valid	*/
  }

#endif

/************************************************************************/
/************************************************************************/
/*			_m_get_dec_buf 					*/
/************************************************************************/

ST_VOID *_m_get_dec_buf (ASN1_DEC_CTXT *aCtx, ST_UINT size)
  {
  aCtx->mmsdec_rslt->data_ptr = (*m_calloc_os_fun)(1, size);
  aCtx->asn1r_err_fun = _mms_dec_buf_free;
  aCtx->mmsdec_rslt->data_pres = SD_TRUE;
  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;		/* select TAG method for get_mms_modifer*/
  return (aCtx->mmsdec_rslt->data_ptr);
  }
