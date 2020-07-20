/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_ced4.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common primitive encode/decode functions	*/
/*	particular to getting and writing variables.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 02/09/09  JRB     04    Allow any value for DataAccessError.		*/
/* 03/11/04  GLB     03    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     01    Added SD_CONST modifiers			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
static ST_VOID va_spec_objname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_spec_vlist_list_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_spec_vlist_name_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_vlist_alt_acc_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_vlist_alt_acc_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_vlist_useq_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_vlist_var_spec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_vlist_useq_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_var_spec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID var_spec_type_spec (ASN1_DEC_CTXT *aCtx);
static ST_VOID var_spec_objname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID var_spec_address_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID var_spec_var_descr_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID var_spec_scat_acc_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID var_spec_get_null (ASN1_DEC_CTXT *aCtx);
static ST_VOID alt_access_get_aa_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID scat_access_get_sa_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID type_spec_get_tspec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID vdata_get_data_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID access_rslt_get_data_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID access_rslt_cstr_parse  (ASN1_DEC_CTXT *aCtx, ST_UINT16 id);
static ST_VOID access_rslt_get_result (ASN1_DEC_CTXT *aCtx, ST_UINT16 id);
static ST_VOID address_unc_addr_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID address_get_num_addr  (ASN1_DEC_CTXT *aCtx);
static ST_VOID address_get_sym_addr  (ASN1_DEC_CTXT *aCtx);
static ST_VOID address_get_unc_addr  (ASN1_DEC_CTXT *aCtx);
static ST_VOID address_unc_addr_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_va_address_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_va_access_rslt_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_va_data_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_va_type_spec_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_va_scat_access_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_va_alt_access_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_va_var_spec_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_va_spec_done) (ASN1_DEC_CTXT *aCtx);
static ST_VOID var_spec_cstr_get_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID va_spec_done (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/*			global structures & functions			*/
/************************************************************************/

static VAR_ACC_ADDR     *addr_ptr;
static ACCESS_RESULT    *acc_rslt_ptr;
static VAR_ACC_DATA     *vdata_ptr;
static VAR_ACC_TSPEC    *tspec_ptr;
static SCATTERED_ACCESS *scat_acc_ptr;
static ALTERNATE_ACCESS *alt_acc_ptr;
static VARIABLE_SPEC    *var_spec_ptr;
static VARIABLE_LIST    *var_list_ptr;
static VAR_ACC_SPEC     *va_spec_ptr;
static VARIABLE_LIST    *va_spec_vlist_ptr;

static ST_INT *NUM_OF_VARS;
static ST_INT  MAX_VARS;
static ST_RET  ERROR_CODE;
static ST_INT  save_dec_method;

/************************************************************************/
/************************************************************************/
/*			_ms_get_va_address					*/
/* Function to get a variable access address from the received message. */
/************************************************************************/

ST_VOID _ms_get_va_address (ASN1_DEC_CTXT *aCtx, VAR_ACC_ADDR *data_ptr, ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {

			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    ERROR_CODE = RESP_BAD_VALUE;
  else
    ERROR_CODE = REQ_BAD_VALUE;

  addr_ptr = data_ptr;
  get_va_address_done = done_fun;

  ASN1R_TAG_ADD (aCtx, CTX, 	 0, address_get_num_addr);
  ASN1R_TAG_ADD (aCtx, CTX, 	 1, address_get_sym_addr);
  ASN1R_TAG_ADD (aCtx, CTX, 	 2, address_get_unc_addr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, address_unc_addr_cstr);
  }

/************************************************************************/
/*			address_get_num_addr				*/
/* numeric address was encountered.					*/
/************************************************************************/

static ST_VOID address_get_num_addr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_num_addr");

  addr_ptr->addr_tag = 0;		/* set tag to numeric address	*/

					/* read numeric address 	*/
  if (asn1r_get_u32 (aCtx, &addr_ptr->addr.num_addr))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  (*get_va_address_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/*			address_get_sym_addr				*/
/* symbolic address was encountered.					*/
/************************************************************************/

static ST_VOID address_get_sym_addr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("address_get_sym_addr");

  addr_ptr->addr_tag = 1;		/* set tag to symbolic address	*/

	/* decode symbolic address onto itself, shifted by 1 ST_CHAR	*/
  addr_ptr->addr.sym_addr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
					/* read symbolic address	*/
  if (asn1r_get_vstr (aCtx, addr_ptr->addr.sym_addr))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  (*get_va_address_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/*			address_get_unc_addr				*/
/* unconstrained address primitive was encountered.			*/
/************************************************************************/

static ST_VOID address_get_unc_addr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("address_get_unc_addr");

  addr_ptr->addr_tag = 2;		/* set tag to unconstrained	*/

	/* decode unconstrained address onto itself, shifted by 1 ST_CHAR	*/
  addr_ptr->addr.unc_addr.unc_ptr = aCtx->asn1r_field_ptr - 1;
					/* read unconstrained address	*/
  if (asn1r_get_octstr (aCtx, addr_ptr->addr.unc_addr.unc_ptr))
    asn1r_set_dec_err (aCtx, ERROR_CODE);
					/* save the address length	*/
  addr_ptr->addr.unc_addr.unc_len = aCtx->asn1r_octetcount;

  (*get_va_address_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/*			address_unc_addr_cstr				*/
/* unconstrained address constructor was encountered.			*/
/************************************************************************/

static ST_VOID address_unc_addr_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("address_unc_addr_cstr");

  addr_ptr->addr_tag = 2;		/* set tag to unconstrained	*/

	/* decode unconstrained address onto itself, shifted by 1 ST_CHAR	*/
  addr_ptr->addr.unc_addr.unc_ptr = aCtx->asn1r_field_ptr - 1;

			/* set up where to go when done decoding cstr	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = address_unc_addr_cstr_done;

			/* read unconstrained address from message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, addr_ptr->addr.unc_addr.unc_ptr);
  }

/************************************************************************/
/*			address_unc_addr_cstr_done			*/
/* unconstrained address constructor done was encountered.		*/
/************************************************************************/

static ST_VOID address_unc_addr_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
					/* save the address length	*/
  addr_ptr->addr.unc_addr.unc_len = aCtx->asn1r_octetcount;

  (*get_va_address_done) (aCtx);		/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_get_va_access_rslt				*/
/* Function to get a variable access access result from the received	*/
/* message.								*/
/************************************************************************/

ST_VOID _ms_get_va_access_rslt (ASN1_DEC_CTXT *aCtx, ACCESS_RESULT *data_ptr,
	ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {
			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    ERROR_CODE = RESP_BAD_VALUE;
  else
    ERROR_CODE = REQ_BAD_VALUE;

  acc_rslt_ptr = data_ptr;
  get_va_access_rslt_done = done_fun;

  acc_rslt_ptr->va_data.data = aCtx->asn1r_field_ptr;	/* set data pointer	*/

  save_dec_method = aCtx->asn1r_decode_method;
  aCtx->asn1r_decode_method   = ASN1_CLASS_METHOD;

  aCtx->asn1r_c_id_fun = access_rslt_get_result;		/* decode access result */
  aCtx->asn1r_u_id_fun = asn1r_class_err; 			/* no universals	*/
  aCtx->asn1r_a_id_fun = asn1r_class_err; 			/* no applications	*/
  aCtx->asn1r_p_id_fun = asn1r_class_err; 			/* no privates		*/
  }

/************************************************************************/
/*			access_rslt_get_result				*/
/* get access result.							*/
/************************************************************************/

static ST_VOID access_rslt_get_result (ASN1_DEC_CTXT *aCtx, ST_UINT16 id)
  {
  MLOG_CDEC0 ("access_rslt_get_result");

  if (aCtx->asn1r_constr_elmnt)
    {					/* constructor, success 	*/
    acc_rslt_ptr->acc_rslt_tag = ACC_RSLT_SUCCESS;
    aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = access_rslt_get_data_done;
    aCtx->asn1r_c_id_fun = access_rslt_cstr_parse;	/* decode access result */
    }
  else
    {					/* primitive			*/
    if (id != 0)
      { 				/* success			*/
      acc_rslt_ptr->acc_rslt_tag = ACC_RSLT_SUCCESS;
      aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;		/* move past data element	*/
      access_rslt_get_data_done (aCtx);
      }
    else
      { 				  /* failure			*/
      acc_rslt_ptr->acc_rslt_tag = ACC_RSLT_FAILURE;
					  /* read data access error	*/
      if (asn1r_get_i16 (aCtx, &acc_rslt_ptr->failure))
	asn1r_set_dec_err (aCtx, ERROR_CODE);
      /* NOTE: DataAccessError should be [0-11] but allow any val here.	*/

      aCtx->asn1r_decode_method = save_dec_method;	/* restore original dec method	*/
      (*get_va_access_rslt_done) (aCtx);	/* all done, call user set fun	*/
      }
    }
  }

/************************************************************************/
/*			access_rslt_cstr_parse				*/
/* in the middle of a result constructor, just accept the elements	*/
/************************************************************************/

static ST_VOID access_rslt_cstr_parse (ASN1_DEC_CTXT *aCtx, ST_UINT16 id)
  {
  MLOG_CDEC0 ("access_rslt_cstr_parse");

  if (!aCtx->asn1r_constr_elmnt)			/* if primitive, need to move	*/
    aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;		/* aCtx->asn1r_field_ptr			*/
  else					/* if cstr, need to set cstr	*/
    aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = NULL;	/* done function == safe	*/
  }

/************************************************************************/
/*			access_rslt_get_data_done			*/
/* variable data has been obtianed.					*/
/************************************************************************/

static ST_VOID access_rslt_get_data_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("access_rslt_get_data_done");
					/* set the variable data length */
  acc_rslt_ptr->va_data.len = 
			(int) (aCtx->asn1r_field_ptr - acc_rslt_ptr->va_data.data);

  aCtx->asn1r_decode_method = save_dec_method;	/* restore original dec method	*/

  (*get_va_access_rslt_done) (aCtx);	/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_get_va_data					*/
/* Function to get a variable access data from the received message.	*/
/************************************************************************/

ST_VOID _ms_get_va_data (ASN1_DEC_CTXT *aCtx, VAR_ACC_DATA *data_ptr, ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {

  vdata_ptr = data_ptr;
  get_va_data_done = done_fun;

  vdata_ptr->data = aCtx->asn1r_field_ptr;			/* set data pointer	*/

  asn1r_parse_next (aCtx, vdata_get_data_done);		/* parse entire element */
  }

/************************************************************************/
/*			vdata_get_data_done				*/
/* variable data has been parsed.					*/
/************************************************************************/

static ST_VOID vdata_get_data_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("vdata_get_data_done");
					/* set the variable data length */
  vdata_ptr->len = (int) (aCtx->asn1r_field_ptr - vdata_ptr->data);

  (*get_va_data_done) (aCtx);		/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_get_va_type_spec				*/
/* Function to get a variable access type specification from the	*/
/* received message.							*/
/************************************************************************/

ST_VOID _ms_get_va_type_spec (ASN1_DEC_CTXT *aCtx, VAR_ACC_TSPEC *data_ptr,
	ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {

  tspec_ptr = data_ptr;
  get_va_type_spec_done = done_fun;

  tspec_ptr->data = aCtx->asn1r_field_ptr;			/* set data pointer	*/

  asn1r_parse_next (aCtx, type_spec_get_tspec_done);	/* parse entire element */
  }

/************************************************************************/
/*			type_spec_get_tspec_done			*/
/* valid type specification was encountered.				*/
/************************************************************************/

static ST_VOID type_spec_get_tspec_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("type_spec_get_tspec_done");
					/* set type specification len	*/
  tspec_ptr->len = (int) (aCtx->asn1r_field_ptr - tspec_ptr->data);

  (*get_va_type_spec_done) (aCtx);		/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_get_va_scat_access				*/
/* Function to get a variable access scattered access from the received */
/* message.								*/
/************************************************************************/

ST_VOID _ms_get_va_scat_access (ASN1_DEC_CTXT *aCtx, SCATTERED_ACCESS *data_ptr,
	ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {

  scat_acc_ptr = data_ptr;
  get_va_scat_access_done = done_fun;

  scat_acc_ptr->data = aCtx->asn1r_field_ptr;		/* set data pointer	*/

  asn1r_parse_next (aCtx, scat_access_get_sa_done); 	/* parse entire element */
  }

/************************************************************************/
/*			scat_access_get_sa_done 			*/
/* scattered access universal sequence constructor was encountered.	*/
/************************************************************************/

static ST_VOID scat_access_get_sa_done (ASN1_DEC_CTXT *aCtx)
  {
					/* set scattered access len	*/
  scat_acc_ptr->len = (int) (aCtx->asn1r_field_ptr - scat_acc_ptr->data);

  (*get_va_scat_access_done) (aCtx);	/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_get_va_alt_access				*/
/* Function to get a variable access alternate access from the received */
/* message.								*/
/************************************************************************/

ST_VOID _ms_get_va_alt_access (ASN1_DEC_CTXT *aCtx, ALTERNATE_ACCESS *data_ptr,
	ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {
  MLOG_CDEC0 ("_ms_get_va_alt_access");

  alt_acc_ptr = data_ptr;
  get_va_alt_access_done = done_fun;

  alt_acc_ptr->data = aCtx->asn1r_field_ptr;		/* set data pointer	*/

  asn1r_parse_cstr_contents (aCtx, alt_access_get_aa_done); /* parse entire element */
  }

/************************************************************************/
/*			alt_access_get_aa_done				*/
/* scattered access was encountered.					*/
/************************************************************************/

static ST_VOID alt_access_get_aa_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("alt_access_get_aa_done");
					/* set type specification len	*/
  alt_acc_ptr->len = (int) (aCtx->asn1r_field_ptr - alt_acc_ptr->data);

  (*get_va_alt_access_done) (aCtx); 	/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_get_va_var_spec 				*/
/* Function to get a variable access variable specification from the	*/
/* received message.							*/
/************************************************************************/

ST_VOID _ms_get_va_var_spec (ASN1_DEC_CTXT *aCtx, VARIABLE_SPEC *data_ptr,
	ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {
			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    ERROR_CODE = RESP_BAD_VALUE;
  else
    ERROR_CODE = REQ_BAD_VALUE;

  var_spec_ptr = data_ptr;
  get_va_var_spec_done = done_fun;

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,  0, var_spec_objname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,  1, var_spec_address_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,  2, var_spec_var_descr_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,  3, var_spec_scat_acc_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 	  4, var_spec_get_null);
  }

/************************************************************************/
/*			var_spec_objname_cstr				*/
/* variable specification object name constructor was encountered.	*/
/************************************************************************/

static ST_VOID var_spec_objname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("var_spec_objname_cstr");

  var_spec_ptr->var_spec_tag = 0;	/* set tag to name		*/

  _ms_get_mms_objname (aCtx, &var_spec_ptr->vs.name, var_spec_cstr_get_done);
  }

/************************************************************************/
/*			var_spec_address_cstr				*/
/* variable specification address was encountered.			*/
/************************************************************************/

static ST_VOID var_spec_address_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("var_spec_address_cstr");

  var_spec_ptr->var_spec_tag = 1;	/* set tag to address		*/

  _ms_get_va_address (aCtx, &var_spec_ptr->vs.address, var_spec_cstr_get_done);
  }

/************************************************************************/
/*			var_spec_var_descr_cstr 			*/
/* variable specification variable description constructor was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID var_spec_var_descr_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("var_spec_var_descr_cstr");

  var_spec_ptr->var_spec_tag = 2;	/* set tag to variable descr	*/

  _ms_get_va_address (aCtx, &var_spec_ptr->vs.var_descr.address, var_spec_type_spec);
  }

/************************************************************************/
/*			var_spec_type_spec				*/
/* variable specification address is done, get the type specification.	*/
/************************************************************************/

static ST_VOID var_spec_type_spec (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("var_spec_type_spec");

  _ms_get_va_type_spec (aCtx, &var_spec_ptr->vs.var_descr.type, var_spec_cstr_get_done);
  }

/************************************************************************/
/*			var_spec_scat_acc_cstr				*/
/* variable specification scattered access constructor was encountered. */
/************************************************************************/

static ST_VOID var_spec_scat_acc_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("var_spec_scat_acc_cstr");

  var_spec_ptr->var_spec_tag = 3;	/* set tag to scattered access	*/

  _ms_get_va_scat_access (aCtx, &var_spec_ptr->vs.sa_descr, var_spec_cstr_get_done);
  }

/************************************************************************/
/*			var_spec_get_null				*/
/* variable specification null was encountered. 			*/
/************************************************************************/

static ST_VOID var_spec_get_null (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("var_spec_get_null");

  var_spec_ptr->var_spec_tag = 4;	/* set tag to variable descr	*/

  if (aCtx->asn1r_elmnt_len != 0)
    {
    asn1r_set_dec_err (aCtx, ERROR_CODE);
    return;
    }

  (*get_va_var_spec_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/*			var_spec_cstr_get_done				*/
/* variable specification object name constructor done was encountered. */
/************************************************************************/

static ST_VOID var_spec_cstr_get_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = va_var_spec_done;
  }

/************************************************************************/
/*			va_var_spec_done				*/
/* variable specification has been obtained, return to set-up function. */
/************************************************************************/

static ST_VOID va_var_spec_done (ASN1_DEC_CTXT *aCtx)
  {
  (*get_va_var_spec_done) (aCtx);		/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_get_va_var_list					*/
/* Function to get a variable access variable list from the received	*/
/* message.								*/
/************************************************************************/

ST_VOID _ms_get_va_var_list (ASN1_DEC_CTXT *aCtx, VARIABLE_LIST *data_ptr, ST_INT *num_of_vars,
	ST_INT max_vars, ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {
			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    ERROR_CODE = RESP_BAD_VALUE;
  else
    ERROR_CODE = REQ_BAD_VALUE;

  var_list_ptr = data_ptr;
  NUM_OF_VARS  = num_of_vars;
  MAX_VARS     = max_vars;
  *NUM_OF_VARS = 0;	  		/* initialize # of variables	*/

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, va_vlist_useq_cstr);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = done_fun;		/* can be empty list	*/
  }

/************************************************************************/
/*			va_vlist_useq_cstr				*/
/* variable access variable list universal sequence was encountered.	*/
/************************************************************************/

static ST_VOID va_vlist_useq_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("va_vlist_useq_cstr");

  (*NUM_OF_VARS)++;			/* increment # of variables	*/
  if (*NUM_OF_VARS > MAX_VARS)
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  var_list_ptr->alt_access_pres = SD_FALSE;

  _ms_get_va_var_spec (aCtx, &var_list_ptr->var_spec, va_vlist_var_spec_done);
  }

/************************************************************************/
/*			va_vlist_var_spec_done				*/
/* variable access variable specification done, get alternate access.	*/
/************************************************************************/

static ST_VOID va_vlist_var_spec_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("va_vlist_var_spec_done");

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,  5, va_vlist_alt_acc_cstr);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = va_vlist_useq_cstr_done;
  }

/************************************************************************/
/*			va_vlist_alt_acc_cstr				*/
/* variable access variable list alternate access constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID va_vlist_alt_acc_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("va_vlist_alt_acc_cstr");

  var_list_ptr->alt_access_pres = SD_TRUE; /* set alternate access present */

  _ms_get_va_alt_access (aCtx, &var_list_ptr->alt_access, va_vlist_alt_acc_done);
  }

/************************************************************************/
/*			va_vlist_alt_acc_done				*/
/* variable specification alternate access has been obtained.		*/
/************************************************************************/

static ST_VOID va_vlist_alt_acc_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("va_vlist_alt_acc_done");
  }


/************************************************************************/
/*			va_vlist_useq_cstr_done 			*/
/* variable access variable list universal sequence constructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID va_vlist_useq_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("va_vlist_useq_cstr_done");
  var_list_ptr++;			/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, va_vlist_useq_cstr);
  }


/************************************************************************/
/************************************************************************/
/*			_ms_get_va_spec					*/
/* Function to get a variable access specification from the received	*/
/* message.								*/
/************************************************************************/

ST_VOID _ms_get_va_spec (ASN1_DEC_CTXT *aCtx, VAR_ACC_SPEC *data_ptr, ST_INT max_vars,
	ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {

			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    ERROR_CODE = RESP_BAD_VALUE;
  else
    ERROR_CODE = REQ_BAD_VALUE;

  va_spec_ptr = data_ptr;
  va_spec_vlist_ptr = (VARIABLE_LIST *) (va_spec_ptr + 1);
  MAX_VARS    = max_vars;
  get_va_spec_done = done_fun;

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,  0, va_spec_vlist_list_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,  1, va_spec_vlist_name_cstr);
  }

/************************************************************************/
/*			va_spec_vlist_list_cstr 			*/
/* variable access specification list of variables constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID va_spec_vlist_list_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("va_spec_vlist_list_cstr");

  va_spec_ptr->var_acc_tag = 0; 	/* set tag to list of variables	*/

  _ms_get_va_var_list (aCtx, va_spec_vlist_ptr, &va_spec_ptr->num_of_variables,
  			MAX_VARS, va_spec_done);
  }

/************************************************************************/
/*			va_spec_vlist_name_cstr 			*/
/* variable access specification variable list name constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID va_spec_vlist_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("va_spec_vlist_name_cstr");

  va_spec_ptr->var_acc_tag = 1; 	/* set tag to var list name	*/

  _ms_get_mms_objname (aCtx, &va_spec_ptr->vl_name, va_spec_objname_done);
  }

/************************************************************************/
/*			va_spec_objname_done				*/
/* variable access specification variable list object name is complete. */
/************************************************************************/

static ST_VOID va_spec_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = va_spec_done;
  }

/************************************************************************/
/*			va_spec_done					*/
/* variable access specification has been obtained, return to set-up	*/
/* function.								*/
/************************************************************************/

static ST_VOID va_spec_done (ASN1_DEC_CTXT *aCtx)
  {
  (*get_va_spec_done) (aCtx);		/* all done, call user set fun	*/
  }

/************************************************************************/
/************************************************************************/
/*			_ms_wr_va_address					*/
/* Function to write a variable access address to the message.		*/
/************************************************************************/

ST_VOID _ms_wr_va_address (ASN1_ENC_CTXT *aCtx, VAR_ACC_ADDR *data_ptr)
  {
  switch (data_ptr->addr_tag)
    {
    case (0)  : 			/* write the numeric address	*/
      asn1r_wr_u32 (aCtx, data_ptr->addr.num_addr);
      asn1r_fin_prim (aCtx, 0, CTX);		/* context tag 0		*/
      break;

    case (1)  : 			/* write the symbolic address	*/
      asn1r_wr_vstr (aCtx, data_ptr->addr.sym_addr);
      asn1r_fin_prim (aCtx, 1,CTX); 		/* context tag 1		*/
      break;

    case (2)  : 			/* write the unconstrained addr */
      asn1r_wr_octstr (aCtx, data_ptr->addr.unc_addr.unc_ptr,
		 data_ptr->addr.unc_addr.unc_len);
      asn1r_fin_prim (aCtx, 2,CTX); 		/* context tag 2		*/
      break;
    }
  }

/************************************************************************/
/*			_ms_wr_va_access_rslt				*/
/* Function to write a variable access access result to the message.	*/
/************************************************************************/

ST_VOID _ms_wr_va_access_rslt (ASN1_ENC_CTXT *aCtx, ACCESS_RESULT *data_ptr)
  {
  if (data_ptr->acc_rslt_tag == ACC_RSLT_SUCCESS)
    _ms_wr_va_data (aCtx, &data_ptr->va_data);	/* success, write data		*/
  else
    {					/* failure, write failure value */
    asn1r_wr_i16 (aCtx, data_ptr->failure);
    asn1r_fin_prim (aCtx, 0, CTX);			/* context tag 0		*/
    }
  }

/************************************************************************/
/*			_ms_wr_va_data					*/
/* Function to write a variable access data to the message.		*/
/************************************************************************/

ST_VOID _ms_wr_va_data (ASN1_ENC_CTXT *aCtx, VAR_ACC_DATA *data_ptr)
  {
  asn1r_wr_delmnt (aCtx, data_ptr->data, data_ptr->len);
  }

/************************************************************************/
/*			_ms_wr_va_type_spec 				*/
/* Function to write a variable access type specification to the	*/
/* message.								*/
/************************************************************************/

ST_VOID _ms_wr_va_type_spec (ASN1_ENC_CTXT *aCtx, VAR_ACC_TSPEC *data_ptr)
  {
  asn1r_wr_delmnt (aCtx, data_ptr->data, data_ptr->len);
  }

/************************************************************************/
/*			_ms_wr_va_scat_access				*/
/* Function to write a variable access scattered access to the message. */
/************************************************************************/

ST_VOID _ms_wr_va_scat_access (ASN1_ENC_CTXT *aCtx, SCATTERED_ACCESS *data_ptr)
  {
  asn1r_wr_delmnt (aCtx, data_ptr->data, data_ptr->len);
  }

/************************************************************************/
/*			_ms_wr_va_alt_access				*/
/* Function to write a variable access alternate access to the message. */
/************************************************************************/

ST_VOID _ms_wr_va_alt_access (ASN1_ENC_CTXT *aCtx, ALTERNATE_ACCESS *data_ptr)
  {
  asn1r_wr_delmnt (aCtx, data_ptr->data, data_ptr->len);
  }

/************************************************************************/
/*			_ms_wr_va_var_spec					*/
/* Function to write a variable access variable specification to the	*/
/* message.								*/
/************************************************************************/

ST_VOID _ms_wr_va_var_spec (ASN1_ENC_CTXT *aCtx, VARIABLE_SPEC *data_ptr)
  {
  switch (data_ptr->var_spec_tag)
    {
    case   0 :
      asn1r_strt_constr (aCtx);
      _ms_wr_mms_objname (aCtx, &data_ptr->vs.name);
      asn1r_fin_constr (aCtx, 0, CTX, DEF);
      break;

    case   1 :
      asn1r_strt_constr (aCtx);
      _ms_wr_va_address (aCtx, &data_ptr->vs.address);
      asn1r_fin_constr (aCtx, 1, CTX, DEF);
      break;

    case   2 :
      asn1r_strt_constr (aCtx);
      _ms_wr_va_type_spec (aCtx, &data_ptr->vs.var_descr.type);
      _ms_wr_va_address (aCtx, &data_ptr->vs.var_descr.address);
      asn1r_fin_constr (aCtx, 2, CTX, DEF);
      break;

    case   3 :
      asn1r_strt_constr (aCtx);
      _ms_wr_va_scat_access (aCtx, &data_ptr->vs.sa_descr);
      asn1r_fin_constr (aCtx, 3, CTX, DEF);
      break;

    case   4 :
      asn1r_fin_prim (aCtx, 4, CTX);
      break;
    }
  }

/************************************************************************/
/*			_ms_wr_va_var_list					*/
/* Function to write a variable access variable list to the message.	*/
/************************************************************************/

ST_VOID _ms_wr_va_var_list (ASN1_ENC_CTXT *aCtx, VARIABLE_LIST *data_ptr, ST_INT num_of_vars)
  {
VARIABLE_LIST *vl_ptr;
ST_INT i;

  vl_ptr = data_ptr;
  vl_ptr += num_of_vars - 1; 		/* point to last in the list	*/
  for (i = 0; i < num_of_vars; i++)
    {
    asn1r_strt_constr (aCtx);			/* start universal sequece cstr */
    if (vl_ptr->alt_access_pres)
      {
      asn1r_strt_constr (aCtx); 			/* start context tag 5 cstr	*/
      _ms_wr_va_alt_access (aCtx, &vl_ptr->alt_access);
      asn1r_fin_constr (aCtx, 5, CTX, DEF);
      }
    _ms_wr_va_var_spec (aCtx, &vl_ptr->var_spec);
    asn1r_fin_constr (aCtx, SEQ_CODE, UNI, DEF);
    vl_ptr--; 				/* point to previous var name	*/
    }
  }

/************************************************************************/
/*			_ms_wr_va_spec					*/
/* Function to write a variable access specification to the message.	*/
/************************************************************************/

ST_VOID _ms_wr_va_spec (ASN1_ENC_CTXT *aCtx, VAR_ACC_SPEC *data_ptr)
  {
VARIABLE_LIST *vl_ptr;

  if (data_ptr->var_acc_tag == 1)
    {
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    _ms_wr_mms_objname (aCtx, &data_ptr->vl_name);
    asn1r_fin_constr (aCtx, 1, CTX, DEF);
    }
  else
    {
    vl_ptr = (VARIABLE_LIST *) (data_ptr + 1);
    asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
    _ms_wr_va_var_list (aCtx, vl_ptr, data_ptr->num_of_variables);
    asn1r_fin_constr (aCtx, 0, CTX, DEF);
    }
  }
