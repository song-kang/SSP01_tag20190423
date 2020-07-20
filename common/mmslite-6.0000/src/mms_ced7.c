/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_ced7.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common primitive encode/decode functions	*/
/*	particular to getting and writing a list of modifiers.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     04    Corr spelling for 'receive'.			*/
/* 03/31/03  JRB     03    asn1r_get_bitstr: add max_bits arg.		*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     01    Added SD_CONST modifiers			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"


#ifdef MOD_SUPPORT
/************************************************************************/
/************************************************************************/
/*			global structures & functions			*/
/************************************************************************/

static ST_VOID modlist_attach_to_ec  (ASN1_DEC_CTXT *aCtx);
static ST_VOID modlist_attach_to_sem (ASN1_DEC_CTXT *aCtx);
static ST_VOID modlist_modifier_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID modlist_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_get_priority      (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_get_acc_delay     (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_get_ctrl_timeout  (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_get_abrt_timeout  (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_get_rel_conn_lost (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_get_ct_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_get_named_token   (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_objname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID atsem_sem_name_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_get_acc_delay (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_get_ct_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_get_ct_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_ecname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_ecname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_ecname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_eename_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_eename_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID atec_eename_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID (*get_mms_modlist_done) (ASN1_DEC_CTXT *aCtx);


static MODIFIER  *mod_ptr;
static ST_INT *NUM_OF_MODS;
static ST_INT  MAX_MODS;
static ST_RET  ERROR_CODE;


/************************************************************************/
/************************************************************************/
/*			_ms_get_mms_modlist 				*/
/* Function to get an MMS list of modifiers from the received message.	*/
/************************************************************************/

ST_VOID _ms_get_mms_modlist (ASN1_DEC_CTXT *aCtx, MODIFIER *data_ptr, ST_INT *num_ptr,
	ST_INT max_mods, ST_VOID (*done_fun) (ASN1_DEC_CTXT *ac))
  {
			/* set decode error for request or response	*/
  if (aCtx->mmsdec_rslt->type == MMSRESP)
    ERROR_CODE = RESP_BAD_VALUE;
  else
    ERROR_CODE = REQ_BAD_VALUE;

  mod_ptr = data_ptr;
  NUM_OF_MODS  = num_ptr;
  MAX_MODS     = max_mods;
  get_mms_modlist_done = done_fun;

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, modlist_attach_to_ec);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, modlist_attach_to_sem);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = modlist_done;
  }

/************************************************************************/
/************************************************************************/
/*			modlist_attach_to_ec				*/
/* decoding list of modifiers: attach to event condition modifier cstr	*/
/* was encountered. 							*/
/************************************************************************/

static ST_VOID modlist_attach_to_ec (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("modlist_attach_to_ec");

  (*NUM_OF_MODS)++;			/* increment # of modifiers	*/
  if (*NUM_OF_MODS > MAX_MODS)
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  mod_ptr->modifier_tag = 0;		/* set tag to attach to ec	*/

  ASN1R_TAG_ADD (aCtx, CTX |CONSTR, 0, atec_eename_cstr);
  }

/************************************************************************/
/*			atec_eename_cstr				*/
/* decoding attach to event condition modifier: event enrollment name	*/
/* constructor was encountered.	 					*/
/************************************************************************/

static ST_VOID atec_eename_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atec_eename_cstr");

  _ms_get_mms_objname (aCtx,&mod_ptr->mod.atec.evenroll_name, atec_eename_done);
  }

/************************************************************************/
/*			atec_eename_done				*/
/* decoding attach to event condition modifier: event enrollment name	*/
/* has been obtained.		 					*/
/************************************************************************/

static ST_VOID atec_eename_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = atec_eename_cstr_done;
  }

/************************************************************************/
/*			atec_eename_cstr_done				*/
/* decoding attach to event condition modifier: event enrollment name	*/
/* constructor done was encountered.	    				*/
/************************************************************************/

static ST_VOID atec_eename_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX |CONSTR, 1, atec_ecname_cstr);
  }

/************************************************************************/
/*			atec_ecname_cstr				*/
/* decoding attach to event condition modifier: event condition name	*/
/* constructor was encountered.	 					*/
/************************************************************************/

static ST_VOID atec_ecname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atec_ecname_cstr");

  _ms_get_mms_objname (aCtx, &mod_ptr->mod.atec.evcon_name, atec_ecname_done);
  }

/************************************************************************/
/*			atec_ecname_done				*/
/* decoding attach to event condition modifier: event condition name	*/
/* has been obtained.		 					*/
/************************************************************************/

static ST_VOID atec_ecname_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = atec_ecname_cstr_done;
  }

/************************************************************************/
/*			atec_ecname_cstr_done				*/
/* decoding attach to event condition modifier: event condition name	*/
/* constructor done was encountered.	    				*/
/************************************************************************/

static ST_VOID atec_ecname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX,       2,atec_get_ct_prim);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,atec_get_ct_cstr);
  }

/************************************************************************/
/*			atec_get_ct_prim				*/
/* decoding attach to event condition modifier: causing transitions	*/
/* primitive was encountered. 	 					*/
/************************************************************************/

static ST_VOID atec_get_ct_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atec_get_ct_prim");
  if (!asn1r_get_bitstr (aCtx, &mod_ptr->mod.atec.causing_transitions, 7))
    atec_get_ct_done (aCtx);      	/* set up for the next data element	*/
  else
    asn1r_set_dec_err (aCtx, ERROR_CODE);
  }

/************************************************************************/
/*			atec_get_ct_cstr    				*/
/* decoding attach to event condition modifier: causing transitions	*/
/* constructor was encountered. 					*/
/************************************************************************/

static ST_VOID atec_get_ct_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atec_get_ct_cstr");

	  			/* set up bitstring cstr done function	*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = atec_get_ct_done;

  asn1r_get_bitstr_cstr (aCtx, 7,&mod_ptr->mod.atec.causing_transitions);
  }

/************************************************************************/
/*			atec_get_ct_done			      	*/
/* decoding attach to event condition modifier: causing transitions	*/
/* constructor done was encountered. 					*/
/************************************************************************/

static ST_VOID atec_get_ct_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atec_get_ct_done");

				/* Check aCtx->asn1r_bitcount for correct # of bits	*/
  if (aCtx->asn1r_bitcount != 7)			/* should be 7 bits		*/
    asn1r_set_dec_err (aCtx, ERROR_CODE);
  else					/* now get acceptable delay	*/
    {
    ASN1R_TAG_ADD (aCtx, CTX,3,atec_get_acc_delay);
    aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = modlist_modifier_done;
    }
  }

/************************************************************************/
/*			atec_get_acc_delay				*/
/* decoding attach to event condition modifier: decode acceptable delay	*/
/************************************************************************/

static ST_VOID atec_get_acc_delay (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atec_get_acc_dela");

  mod_ptr->mod.atec.acc_delay_pres = SD_TRUE;	 	/* accept delay present */

				/* read acceptable delay from message	*/
  if (asn1r_get_u32 (aCtx, &(mod_ptr->mod.atec.acc_delay)))
    asn1r_set_dec_err (aCtx, ERROR_CODE);
  }

/************************************************************************/
/************************************************************************/
/*			modlist_attach_to_sem				*/
/* decoding list of modifiers: attach to semaphore modifier constructor	*/
/* was encountered. 							*/
/************************************************************************/

static ST_VOID modlist_attach_to_sem (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("modlist_attach_to_sem");

  (*NUM_OF_MODS)++;			/* increment # of modifiers	*/
  if (*NUM_OF_MODS > MAX_MODS)
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  mod_ptr->modifier_tag = 1;		/* set tag to attach to sem	*/

  ASN1R_TAG_ADD (aCtx, CTX |CONSTR, 0, atsem_sem_name_cstr);
  }

/************************************************************************/
/*			atsem_sem_name_cstr				*/
/* decoding attach to semaphore modifier: semaphore name constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID atsem_sem_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atsem_sem_name_cstr");

  mod_ptr->mod.atsem.priority      = MMS_NORM_PRIORITY;
  mod_ptr->mod.atsem.rel_conn_lost = SD_TRUE;

  _ms_get_mms_objname (aCtx, &mod_ptr->mod.atsem.sem_name, atsem_objname_done);
  }

/************************************************************************/
/*			atsem_objname_done				*/
/* decoding attach to semaphore modifier: semaphore name has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID atsem_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = atsem_sem_name_cstr_done;
  }

/************************************************************************/
/*			atsem_sem_name_cstr_done			*/
/* decoding attach to semaphore modifier: semaphore name constructor	*/
/* done was encountered.       						*/
/************************************************************************/

static ST_VOID atsem_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, atsem_get_named_token);
  ASN1R_TAG_ADD (aCtx, CTX, 2, atsem_get_priority   );
  ASN1R_TAG_ADD (aCtx, CTX, 3, atsem_get_acc_delay  );
  ASN1R_TAG_ADD (aCtx, CTX, 4, atsem_get_ctrl_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 5, atsem_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 6, atsem_get_rel_conn_lost);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = modlist_modifier_done;
  }

/************************************************************************/
/*			atsem_get_named_token				*/
/* decoding attach to semaphore modifier: named token name was		*/
/* encountered. 	      						*/
/************************************************************************/

static ST_VOID atsem_get_named_token (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atsem_get_named_token");

  mod_ptr->mod.atsem.named_token_pres = SD_TRUE;	/* named token present	*/

			/* read named token from received message	*/
  if (asn1r_get_identifier (aCtx, mod_ptr->mod.atsem.named_token))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  ASN1R_TAG_ADD (aCtx, CTX, 2, atsem_get_priority   );
  ASN1R_TAG_ADD (aCtx, CTX, 3, atsem_get_acc_delay  );
  ASN1R_TAG_ADD (aCtx, CTX, 4, atsem_get_ctrl_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 5, atsem_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 6, atsem_get_rel_conn_lost);
  }

/************************************************************************/
/*			atsem_get_priority				*/
/* decoding attach to semaphore modifier: decode the priority.		*/
/************************************************************************/

static ST_VOID atsem_get_priority (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atsem_get_priorit");
				/* read priority from received message	*/
  if (asn1r_get_u8 (aCtx, &(mod_ptr->mod.atsem.priority)))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  ASN1R_TAG_ADD (aCtx, CTX, 3, atsem_get_acc_delay  );
  ASN1R_TAG_ADD (aCtx, CTX, 4, atsem_get_ctrl_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 5, atsem_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 6, atsem_get_rel_conn_lost);
  }

/************************************************************************/
/*			atsem_get_acc_delay				*/
/* decoding attach to semaphore modifier: decode acceptable delay 	*/
/************************************************************************/

static ST_VOID atsem_get_acc_delay (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atsem_get_acc_dela");

  mod_ptr->mod.atsem.acc_delay_pres = SD_TRUE; 	/* accept delay present */

				/* read acceptable delay from message	*/
  if (asn1r_get_u32 (aCtx, &(mod_ptr->mod.atsem.acc_delay)))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  ASN1R_TAG_ADD (aCtx, CTX, 4, atsem_get_ctrl_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 5, atsem_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 6, atsem_get_rel_conn_lost);
  }

/************************************************************************/
/*			atsem_get_ctrl_timeout				*/
/* decoding attach to semaphore modifier: decode control timeout. 	*/
/************************************************************************/

static ST_VOID atsem_get_ctrl_timeout (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atsem_get_ctrl_timeou");

  mod_ptr->mod.atsem.ctrl_timeout_pres = SD_TRUE;	/* ctrl timeout pres	*/

				/* read control timeout from message	*/
  if (asn1r_get_u32 (aCtx, &(mod_ptr->mod.atsem.ctrl_timeout)))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  ASN1R_TAG_ADD (aCtx, CTX, 5, atsem_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 6, atsem_get_rel_conn_lost);
  }

/************************************************************************/
/*			atsem_get_abrt_timeout				*/
/* decoding attach to semaphore modifier: decode abort timeout. 	*/
/************************************************************************/

static ST_VOID atsem_get_abrt_timeout (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atsem_get_abrt_timeout");

  mod_ptr->mod.atsem.abrt_on_timeout_pres = SD_TRUE;

  if (asn1r_get_bool (aCtx, &(mod_ptr->mod.atsem.abrt_on_timeout)))
    asn1r_set_dec_err (aCtx, ERROR_CODE);

  ASN1R_TAG_ADD (aCtx, CTX, 6, atsem_get_rel_conn_lost);
  }

/************************************************************************/
/*			atsem_get_rel_conn_lost				*/
/* decoding attach to semaphore modifier: decode relinquish if 		*/
/* connection lost.							*/
/************************************************************************/

static ST_VOID atsem_get_rel_conn_lost (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("atsem_get_rel_conn_lost");

  if (asn1r_get_bool (aCtx, &(mod_ptr->mod.atsem.rel_conn_lost)))
    asn1r_set_dec_err (aCtx, ERROR_CODE);
  }

/************************************************************************/
/*			modlist_modifier_done				*/
/* decoding attach to semaphore modifier: modifier constructor done	*/
/* was encountered					 		*/
/************************************************************************/

static ST_VOID modlist_modifier_done (ASN1_DEC_CTXT *aCtx)
  {
  mod_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, modlist_attach_to_ec);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, modlist_attach_to_sem);
  }

/************************************************************************/
/*			modlist_done					*/
/* decoding list of modifiers: list is done				*/
/************************************************************************/

static ST_VOID modlist_done (ASN1_DEC_CTXT *aCtx)
  {
  (*get_mms_modlist_done) (aCtx);	/* all done, call user set fun	*/
  }


/************************************************************************/
/************************************************************************/
/*			_ms_wr_mms_modlist					*/
/* Function to write an MMS list of modifiers to the message. 		*/
/************************************************************************/

ST_VOID _ms_wr_mms_modlist (ASN1_ENC_CTXT *aCtx, MODIFIER *data_ptr, ST_INT num_of_mods)
  {
MODIFIER *modPtr;
ST_INT i;

  modPtr = data_ptr;
  modPtr += num_of_mods - 1; 		/* point to last in the list	*/

  for (i = 0; i < num_of_mods; i++)
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 0 or 1 cstr	*/
    switch (modPtr->modifier_tag)
      {
      case (0)  : 			/* write attach to event cond 	*/
        if (modPtr->mod.atec.acc_delay_pres)
	  {
	  asn1r_wr_u32 (aCtx, modPtr->mod.atec.acc_delay);
	  asn1r_fin_prim (aCtx, 3, CTX);
	  }

	asn1r_wr_bitstr (aCtx, &modPtr->mod.atec.causing_transitions, 7);
        asn1r_fin_prim (aCtx, 2, CTX);
        
        asn1r_strt_constr (aCtx);
        _ms_wr_mms_objname (aCtx, &modPtr->mod.atec.evcon_name);
        asn1r_fin_constr (aCtx, 1, CTX, DEF);

	asn1r_strt_constr (aCtx);
        _ms_wr_mms_objname (aCtx, &modPtr->mod.atec.evenroll_name);
        asn1r_fin_constr (aCtx, 0, CTX, DEF);
        break;

      case (1)  : 			/* write attach to semaphore	*/
        asn1r_wr_bool (aCtx, modPtr->mod.atsem.rel_conn_lost);
	asn1r_fin_prim (aCtx, 6, CTX);

        if (modPtr->mod.atsem.abrt_on_timeout_pres)
	  {
	  asn1r_wr_bool (aCtx, modPtr->mod.atsem.abrt_on_timeout);
          asn1r_fin_prim (aCtx, 5, CTX);
	  }

        if (modPtr->mod.atsem.ctrl_timeout_pres)
	  {
	  asn1r_wr_u32 (aCtx, modPtr->mod.atsem.ctrl_timeout);
          asn1r_fin_prim (aCtx, 4, CTX);
	  }

        if (modPtr->mod.atsem.acc_delay_pres)
	  {
	  asn1r_wr_u32 (aCtx, modPtr->mod.atsem.acc_delay);
	  asn1r_fin_prim (aCtx, 3, CTX);
	  }

        if (modPtr->mod.atsem.priority != MMS_NORM_PRIORITY)
	  {
	  asn1r_wr_u8 (aCtx, modPtr->mod.atsem.priority);
	  asn1r_fin_prim (aCtx, 2, CTX);
	  }

        if (modPtr->mod.atsem.named_token_pres)
	  {
	  asn1r_wr_vstr (aCtx, modPtr->mod.atsem.named_token);
	  asn1r_fin_prim (aCtx, 1, CTX);
	  }
        
	asn1r_strt_constr (aCtx);
        _ms_wr_mms_objname (aCtx, &modPtr->mod.atsem.sem_name);
        asn1r_fin_constr (aCtx, 0, CTX, DEF);
        break;
      } 
    asn1r_fin_constr (aCtx, (ST_UINT16) modPtr->modifier_tag, CTX, DEF);
    modPtr--; 				/* point to previous modifier	*/
    }
  }
#endif
