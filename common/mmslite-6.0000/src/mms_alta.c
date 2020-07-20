/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2006, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_alta.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/* Function to decode ASN.1 type definitions into Runtime typedefs	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 01/20/10  JRB     21    Fix log messages.				*/
/* 08/09/06  JRB     20    Add ms_aa_to_asn1_2.				*/
/* 02/22/05  JRB     19    Make "sp" local to avoid VXWORKS conflict.	*/
/* 02/06/02  JRB     18    Move ms_mk_rt_aa* to new mms_rtaa.c.		*/
/* 12/20/01  JRB     17    Converted to use ASN1R (re-entrant ASN1)	*/
/* 11/15/01  EJV     16    Added support for new MMS type UtcTime:	*/
/*			   find_named_comp: added case for RT_UTC_TIME;	*/
/*			   _ms_log_runtime_aa: add case for RT_UTC_TIME;*/
/* 09/26/00  JRB     15    Add SD_CONST on ms_mk_rt_aa*			*/
/* 07/28/00  JRB     14    Use ms_comp_name_find for everyone.		*/
/* 07/13/00  JRB     13    Use new ms_comp_name_find for MMS LITE.	*/
/* 04/28/00  JRB     12    Lint cleanup					*/
/* 09/13/99  RKR     11    Respaced a macro for the install utility     */
/* 09/13/99  MDE     10    Added SD_CONST modifiers			*/
/* 07/30/99  MDE     09    Replaced CSTR_DONE_FUN macro use		*/
/* 09/21/98  MDE     08    Minor lint cleanup				*/
/* 09/10/98  MDE     07    Changes to work better with MMS-LITE		*/
/* 06/15/98  MDE     06    Changes to allow compile under C++		*/
/* 03/12/98  MDE     05    '_ms_m_get_rt_info' use change		*/
/* 03/11/98  MDE     04    Removed NEST_RT_TYPES			*/
/* 02/10/98  MDE     03    No longer use runtime type 'loops' element	*/
/*			   rearranged for MMS-LITE (MMS_ALTA_DATA)	*/
/* 12/29/97  MDE     02    Corrected LOCK/UNLOCK problems		*/
/* 06/09/97  MDE     01    Modified Runtime Type handling		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/

static ST_RET wr_altAcc (ASN1_ENC_CTXT *aCtx, ALT_ACC_EL **aap, ALT_ACC_EL *last_aa);
static ST_RET wr_altAccSel (ASN1_ENC_CTXT *aCtx, ALT_ACC_EL **aap, ALT_ACC_EL *last_aa);
static ALT_ACC_EL *find_start_aa_nest (ALT_ACC_EL *aa, ALT_ACC_EL *last_aa);
static ST_VOID alt_acc_seq_start (ASN1_DEC_CTXT *aCtx);
static ST_VOID alt_acc_seq_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID add_alt_acc_tags (ASN1_DEC_CTXT *aCtx);
static ST_VOID named_start (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_component_name (ASN1_DEC_CTXT *aCtx);
static ST_VOID named_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_acc_comp (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_acc_index (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_acc_index_range (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_acc_ir_low (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_acc_ir_num (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_acc_ir_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_acc_all (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc_comp (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc_index (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc_index_range (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc_ir_low (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc_ir_num (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc_ir_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID sel_alt_acc_all (ASN1_DEC_CTXT *aCtx);
static ST_VOID alt_acc_dec_done_ok (ASN1_DEC_CTXT *aCtx);
static ST_VOID inc_dest_aa (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/* Static internal variables, used during decode			*/

static ALT_ACC_EL *dest_aa;
static ALT_ACC_EL *end_dest_aa;

/************************************************************************/
/************************************************************************/
/*                       ms_asn1_to_aa					*/
/* Convert an ASN.1 encoded AA to an array of 'ALT_ACC_EL'		*/
/* Allocates an array of 'm_max_dec_aa' elements, using chk_calloc,	*/
/* does not realloc to correct size.					*/
/************************************************************************/


ST_RET ms_asn1_to_aa (ST_UCHAR *asn1ptr, ST_INT asn1len, ALT_ACCESS *alt_acc_out)
  {
ALT_ACC_EL *dest_aa_head;
ASN1_DEC_CTXT localDecCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_DEC_CTXT *aCtx = &localDecCtx;

  memset (aCtx, 0, sizeof (ASN1_DEC_CTXT));	/* CRITICAL: start clean.	*/

  S_LOCK_COMMON_RESOURCES ();

/* Allocate the output AA array						*/
  dest_aa = (ALT_ACC_EL *) chk_calloc (m_max_dec_aa, sizeof (ALT_ACC_EL));
  end_dest_aa = dest_aa + (m_max_dec_aa -1);
  dest_aa_head = dest_aa;

/* Set up ASN.1 decode tools						*/
  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;		/* select tag method			*/
  aCtx->asn1r_decode_done_fun = asn1r_done_err;/* not legal to be done at this time	*/
  aCtx->asn1r_err_fun = NULL;

/* We are using an alternate entry point into the ASN.1 decode, where	*/
/* the outer tag is not required.					*/

  add_alt_acc_tags (aCtx);
  aCtx->asn1r_c_done_fun[0] = NULL;
  aCtx->asn1r_decode_done_fun = alt_acc_dec_done_ok;
  asn1r_decode_asn1_seq (aCtx, asn1ptr, asn1len); /* decode ASN.1 'Data' entity  */

  if (aCtx->asn1r_pdu_dec_err != NO_DECODE_ERR)	/* check for sucess or return error	*/
    {
    chk_free (dest_aa_head);
    MLOG_NERR0 ("ASN.1 to AA Decode Error");
    S_UNLOCK_COMMON_RESOURCES ();
    return (MVE_DATA_CONVERT);
    }

  alt_acc_out->aa = dest_aa_head;
  alt_acc_out->num_aa = (int) (dest_aa - dest_aa_head);

/* Log the alternate access, if selected				*/
  if (mms_debug_sel & MMS_LOG_AA)
    ms_log_alt_access (alt_acc_out);

/* save the high water mark						*/
  if (alt_acc_out->num_aa > m_hw_dec_aa)
    m_hw_dec_aa = alt_acc_out->num_aa;

  S_UNLOCK_COMMON_RESOURCES ();
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			alt_acc_seq_start				*/
/* This function is called whe the outer UNI SEQ constructor of the	*/
/* AlternateAcess is found. Used only for continuing the decode for a	*/
/* nested AA spec, as the outer constructor is stripped 		*/
/************************************************************************/

static ST_VOID alt_acc_seq_start (ASN1_DEC_CTXT *aCtx)
  {
  add_alt_acc_tags (aCtx);
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = alt_acc_seq_cstr_done;
  }


/************************************************************************/
/*			alt_acc_seq_cstr_done				*/
/************************************************************************/

static ST_VOID alt_acc_seq_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("alt_acc_seq_cstr_done");
  }

/************************************************************************/
/************************************************************************/
/*			add_alt_acc_tags				*/
/* Add appropriate tags for the outer 'AlternateAccess'			*/
/************************************************************************/

static ST_VOID  add_alt_acc_tags (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, sel_alt_acc);
  ASN1R_TAG_ADD (aCtx, CTX,          1, sel_acc_comp);
  ASN1R_TAG_ADD (aCtx, CTX,          2, sel_acc_index);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, sel_acc_index_range);
  ASN1R_TAG_ADD (aCtx, CTX,          4, sel_acc_all);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, named_start);
  }

/************************************************************************/
/*			named_start					*/
/* Constructor for the 'named' element of the 'AlternateAccess'		*/
/************************************************************************/

static ST_VOID named_start (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("named_start");
  ASN1R_TAG_ADD (aCtx, CTX, 0, get_component_name);
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = named_cstr_done;
  }

/************************************************************************/
/*			get_component_name 				*/
/* The 'componentName' element of the 'named' element			*/
/************************************************************************/

static ST_VOID get_component_name (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_component_name");
  if (asn1r_get_identifier (aCtx, dest_aa->comp_name))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    dest_aa->comp_name_pres = SD_TRUE;
    ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, sel_alt_acc);
    ASN1R_TAG_ADD (aCtx, CTX,          1, sel_acc_comp);
    ASN1R_TAG_ADD (aCtx, CTX,          2, sel_acc_index);
    ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, sel_acc_index_range);
    ASN1R_TAG_ADD (aCtx, CTX,          4, sel_acc_all);
    }
  }

/************************************************************************/
/*			named_cstr_done					*/
/* End of 'named' constructor						*/
/************************************************************************/

static ST_VOID named_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("named_cstr_done");
  add_alt_acc_tags (aCtx);
  }


/************************************************************************/
/*			sel_acc_comp					*/
/* The 'component' of the 'selectAccess' has been encountered		*/
/************************************************************************/

static ST_VOID sel_acc_comp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_acc_comp");
  if (asn1r_get_identifier (aCtx, dest_aa->u.component))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    dest_aa->sel_type = AA_COMP;
    inc_dest_aa (aCtx);
    add_alt_acc_tags (aCtx);
    }
  }

/************************************************************************/
/*			sel_acc_index					*/
/* The 'index' of the 'selectAccess' has been encountered		*/
/************************************************************************/

static ST_VOID sel_acc_index (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_acc_index");
  if (asn1r_get_u32 (aCtx, &dest_aa->u.index))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    dest_aa->sel_type = AA_INDEX;
    inc_dest_aa (aCtx);
    add_alt_acc_tags (aCtx);
    }
  }

/************************************************************************/
/*			sel_acc_index_range				*/
/* The 'indexRange' of the 'selectAccess' has been encountered		*/
/************************************************************************/

static ST_VOID sel_acc_index_range (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_acc_index_range");

  dest_aa->sel_type = AA_INDEX_RANGE;
  ASN1R_TAG_ADD (aCtx, CTX,0, sel_acc_ir_low);
  }


/************************************************************************/
/*			sel_acc_ir_low					*/
/* The 'low_index' of the 'indexRange' has been encountered		*/
/************************************************************************/

static ST_VOID sel_acc_ir_low (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_acc_ir_low");
  if (asn1r_get_u32 (aCtx, &dest_aa->u.ir.low_index))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    ASN1R_TAG_ADD (aCtx, CTX, 1, sel_acc_ir_num);
    }
  }

/************************************************************************/
/*			sel_acc_ir_num					*/
/* The 'numberOfElements' of the 'indexRange' has been encountered 	*/
/************************************************************************/

static ST_VOID sel_acc_ir_num (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_acc_ir_num");
  if (asn1r_get_u32 (aCtx, &dest_aa->u.ir.num_elmnts))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = sel_acc_ir_cstr_done;
    }
  }

/************************************************************************/
/*			sel_acc_ir_cstr_done				*/
/* End of the 'indexRange' constructor					*/
/************************************************************************/

static ST_VOID sel_acc_ir_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_acc_ir_cstr_done");
  inc_dest_aa (aCtx);
  add_alt_acc_tags (aCtx);
  }


/************************************************************************/
/*			sel_acc_all					*/
/* The 'allElements' of the 'selectAccess' has been encountered		*/
/************************************************************************/

static ST_VOID sel_acc_all (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_acc_all");
  dest_aa->sel_type = AA_ALL;
  inc_dest_aa (aCtx);
  add_alt_acc_tags (aCtx);
  }


/************************************************************************/
/************************************************************************/
/*			sel_alt_acc					*/
/* The 'selectAlternateAccess' constructor has been encountered		*/
/************************************************************************/

static ST_VOID sel_alt_acc (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc");

  ASN1R_TAG_ADD (aCtx, CTX,          0, sel_alt_acc_comp);
  ASN1R_TAG_ADD (aCtx, CTX,          1, sel_alt_acc_index);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, sel_alt_acc_index_range);
  ASN1R_TAG_ADD (aCtx, CTX,          3, sel_alt_acc_all);
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = sel_alt_acc_cstr_done;
  }


/************************************************************************/
/*			sel_alt_acc_cstr_done				*/
/* End of the 'selectAlternateAccess' constructor			*/
/************************************************************************/

static ST_VOID sel_alt_acc_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc_cstr_done");
  dest_aa->sel_type = AA_END_NEST;
  inc_dest_aa (aCtx);
  add_alt_acc_tags (aCtx);
  }

/************************************************************************/
/*			sel_alt_acc_comp				*/
/* The 'component' of the 'selectAlternateAccesss' has been encountered	*/
/************************************************************************/

static ST_VOID sel_alt_acc_comp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc_comp");
  if (asn1r_get_identifier (aCtx, dest_aa->u.component))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    dest_aa->sel_type = AA_COMP_NEST;
    inc_dest_aa (aCtx);
    ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, alt_acc_seq_start);
    }
  }

/************************************************************************/
/*			sel_alt_acc_index				*/
/* The 'index' of the 'selectAlternateAccesss' has been encountered   	*/
/************************************************************************/

static ST_VOID sel_alt_acc_index (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc_index");
  if (asn1r_get_u32 (aCtx, &dest_aa->u.index))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    dest_aa->sel_type = AA_INDEX_NEST;
    inc_dest_aa (aCtx);
    ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, alt_acc_seq_start);
    }
  }

/************************************************************************/
/*			sel_alt_acc_index_range				*/
/* The 'indexRange' of 'selectAlternateAccesss' has been encountered	*/
/************************************************************************/

static ST_VOID sel_alt_acc_index_range (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc_index_range");
  dest_aa->sel_type = AA_INDEX_RANGE_NEST;
  ASN1R_TAG_ADD (aCtx, CTX,0, sel_alt_acc_ir_low);
  }


/************************************************************************/
/*			sel_alt_acc_ir_low				*/
/* The 'low_index' of the 'indexRange' has been encountered		*/
/************************************************************************/

static ST_VOID sel_alt_acc_ir_low (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc_ir_low");
  if (asn1r_get_u32 (aCtx, &dest_aa->u.ir.low_index))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    ASN1R_TAG_ADD (aCtx, CTX, 1, sel_alt_acc_ir_num);
    }
  }

/************************************************************************/
/*			sel_alt_acc_ir_num				*/
/* The 'numberOfElements' of the 'indexRange' has been encountered 	*/
/************************************************************************/

static ST_VOID sel_alt_acc_ir_num (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc_ir_num");
  if (asn1r_get_u32 (aCtx, &dest_aa->u.ir.num_elmnts))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  else
    {
    aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = sel_alt_acc_ir_cstr_done;
    }
  }

/************************************************************************/
/*			sel_alt_acc_ir_cstr_done			*/
/* End of the 'indexRange' constructor					*/
/************************************************************************/

static ST_VOID sel_alt_acc_ir_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc_ir_cstr_done");
  inc_dest_aa (aCtx);
  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, alt_acc_seq_start);
  }


/************************************************************************/
/*			sel_alt_acc_all					*/
/* The 'allElements' of 'selectAlternateAccesss' has been encountered	*/
/************************************************************************/

static ST_VOID sel_alt_acc_all (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sel_alt_acc_all");
  dest_aa->sel_type = AA_ALL_NEST;
  inc_dest_aa (aCtx);
  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, alt_acc_seq_start);
  }


/************************************************************************/
/*			inc_dest_aa					*/
/* Increment the destination AA pointer, verify no overrun		*/
/************************************************************************/

static ST_VOID inc_dest_aa (ASN1_DEC_CTXT *aCtx)
  {
  if (++dest_aa > end_dest_aa)
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);      
  }

/************************************************************************/
/*			alt_acc_dec_done_ok				*/
/* 'decode done fun' for the ASN.1 to AA decode				*/
/************************************************************************/

static ST_VOID alt_acc_dec_done_ok (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("alt_dec_done_ok");
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
/*                       ms_aa_to_asn1					*/
/* Encode Alternate Access, assume that the encode tools have been 	*/
/* initalized								*/
/* NOTE: The higher level function "ms_aa_to_asn1_2" is more convenient.*/
/*   Call this directly only if "AA" encode is in the middle of a	*/
/*   larger encode.							*/
/************************************************************************/

static ST_INT encodeDone;

ST_RET ms_aa_to_asn1 (ASN1_ENC_CTXT *aCtx, ALT_ACCESS *alt_acc)
  {
ALT_ACC_EL *aa;
ALT_ACC_EL *last_aa;
ST_RET ret;

  S_LOCK_COMMON_RESOURCES ();
  aa = alt_acc->aa;
  last_aa = aa;				/* save pointer to aa[0]	*/
  aa += (alt_acc->num_aa-1);		/* go from bottom to top	*/

/* Write the AlternateAccess 'Seq Of Choice' elements until we have 	*/
/* gone through all AA elements						*/
  ret = SD_SUCCESS;
  encodeDone = SD_FALSE;
  while (!encodeDone && ret == SD_SUCCESS)
    {
    if (aa == last_aa)
      encodeDone = SD_TRUE;
    ret = wr_altAcc (aCtx, &aa, last_aa);
    }

/* Verify no asn.1 encode buffer overrun				*/
  if (aCtx->asn1r_encode_overrun)
    {
    MLOG_NERR0 ("AA to ASN.1 : buffer overrun");
    ret = ME_ASN1_ENCODE_OVERRUN;
    }

  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

/************************************************************************/
/*			 	wr_altAcc 				*/
/************************************************************************/

/* NOTE : This function is entered recursively to encode parts of the	*/
/* AlternateAccess							*/

static ST_RET wr_altAcc (ASN1_ENC_CTXT *aCtx, ALT_ACC_EL **aap, ALT_ACC_EL *last_aa)
  {
ALT_ACC_EL *aa;
ALT_ACC_EL *aa_start_nest;
ST_RET ret;

/* get a working pointer						*/
  aa = *aap;

/* Write the AlternateAccess sequence of choice element			*/

/* First need to see if this is the start of a named component, so 	*/
/* we can start the constructor if so					*/
  
/* If this is a nested AA element, need to find the start element 	*/
  if (aa->sel_type == AA_END_NEST)
    {
    aa_start_nest = find_start_aa_nest (aa, last_aa);
    if (!aa_start_nest)
      {
      MLOG_NERR0 ("AA to ASN.1 : Bad AA");
      return (MVE_AA_SELECT);
      }

    if (aa_start_nest->comp_name_pres)
      asn1r_strt_constr (aCtx);			/* Start 'named' seq cstr 	*/
    }
  else					/* not the start of a nested	*/
    {
    if (aa->comp_name_pres)		
      asn1r_strt_constr (aCtx);			/* Start 'named' seq cstr 	*/
    }

/* OK, if this was a named component, the constructor has been started	*/
/* Go ahead and write the AlternateAccessSelection element		*/

  ret = wr_altAccSel (aCtx, &aa, last_aa); 	/* write the altAccSel		*/

  *aap = aa;
  return (ret);
  }


/************************************************************************/
/*			wr_altAccSel					*/
/************************************************************************/

/* NOTE : This function is entered recursively to encode parts of the	*/
/* AlternateAccess							*/

static ST_RET wr_altAccSel (ASN1_ENC_CTXT *aCtx, ALT_ACC_EL **aap, ALT_ACC_EL *last_aa)
  {
ALT_ACC_EL *aa;
ALT_ACC_EL *aa_start_nest;
ST_RET ret;

  ret = SD_SUCCESS;
  aa = *aap;				/* get working pointer		*/
  switch (aa->sel_type)
    {
    case AA_COMP :
      asn1r_wr_vstr (aCtx, aa->u.component);
      asn1r_fin_prim (aCtx, 1,CTX);
    break;
  
    case AA_INDEX :
      asn1r_wr_u32 (aCtx, aa->u.index);
      asn1r_fin_prim (aCtx, 2,CTX);
    break;
  
    case AA_INDEX_RANGE :
      asn1r_strt_constr (aCtx);			
      asn1r_wr_u32 (aCtx, aa->u.ir.num_elmnts);
      asn1r_fin_prim (aCtx, 1,CTX);
      asn1r_wr_u32 (aCtx, aa->u.ir.low_index);
      asn1r_fin_prim (aCtx, 0,CTX);
      asn1r_fin_constr (aCtx, 3,CTX,DEF);
    break;
  
    case AA_ALL :
      asn1r_fin_prim (aCtx, 4,CTX);
    break;
  
    case AA_END_NEST :

    /* First we need to find the NEST START element so that we can	*/
    /* encode all elements on this level before returning, so that we 	*/
    /* can add the outer constructor					*/

      aa_start_nest = find_start_aa_nest (aa, last_aa);
      if (!aa_start_nest)
        {
        MLOG_NERR0 ("AA to ASN.1 : Bad AA");
        return (MVE_AA_SELECT);
        }
      
      asn1r_strt_constr (aCtx); 	/* Start SelectAlternateAccess constructor	*/
      asn1r_strt_constr (aCtx); 	/* Start outer AlternateAccess constructor	*/
      --aa;		/* Begin with next element			*/
      while (aa != aa_start_nest)
        {		
        ret = wr_altAcc (aCtx, &aa, last_aa);	/* wr_altAcc will decrement aa	*/
        if (aa == last_aa)
          encodeDone = SD_TRUE;

        if (ret != SD_SUCCESS)
          return (ret);
        }
      asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF); /* Outer AlternateAccess cstr	*/

    /* Now write the accessSelection					*/
      switch (aa->sel_type)
        {
        case AA_COMP_NEST :
          asn1r_wr_vstr (aCtx, aa->u.component);
          asn1r_fin_prim (aCtx, 0,CTX);
        break;
      
        case AA_INDEX_NEST :
          asn1r_wr_u32 (aCtx, aa->u.index);
          asn1r_fin_prim (aCtx, 1,CTX);
        break;
      
        case AA_INDEX_RANGE_NEST :
          asn1r_strt_constr (aCtx);			
          asn1r_wr_u32 (aCtx, aa->u.ir.num_elmnts);
          asn1r_fin_prim (aCtx, 1,CTX);
          asn1r_wr_u32 (aCtx, aa->u.ir.low_index);
          asn1r_fin_prim (aCtx, 0,CTX);
          asn1r_fin_constr (aCtx, 2,CTX,DEF);
        break;	   
      
        case AA_ALL_NEST :
          asn1r_fin_prim (aCtx, 3,CTX);
        break;	
        }

    /* Finish the accessSelection constructor				*/
      asn1r_fin_constr (aCtx, 0,CTX,DEF);
    break;

  /* These should never be encountered at this level			*/
    case AA_COMP_NEST :
    case AA_INDEX_NEST :
    case AA_INDEX_RANGE_NEST :
    case AA_ALL_NEST :
    default :
      MLOG_NERR0 ("AA to ASN.1 : Bad AA sel_type value");
      return SD_FAILURE;
    break;
    }


/* For the elements that are not 'end nest', add the 'named' element 	*/
/* as required								*/

  if (aa->sel_type != AA_END_NEST)
    {
    if (aa->comp_name_pres)
      {
      asn1r_wr_vstr (aCtx, aa->comp_name);		/* Write the name 		*/
      asn1r_fin_prim (aCtx, 0,CTX);
      asn1r_fin_constr (aCtx, 5,CTX,DEF);		/* Finish the 'named' cstr	*/
      }
    }

  if (aa > last_aa)
    --aa;					
  *aap = aa;
  return (ret);
  }


/************************************************************************/
/*			 find_start_aa_nest				*/
/************************************************************************/

static ALT_ACC_EL *find_start_aa_nest (ALT_ACC_EL *aa, 
			   	       	     ALT_ACC_EL *last_aa)
  {
ALT_ACC_EL *aa2;
ST_INT nest;

  aa2 = aa-1;     		/* Start with the element above		*/
  nest = 0;			/* keep track of nesting level		*/
  while (SD_TRUE)
    {
    if (aa2->sel_type == AA_COMP_NEST ||
        aa2->sel_type == AA_INDEX_NEST ||
        aa2->sel_type == AA_INDEX_RANGE_NEST ||
        aa2->sel_type == AA_ALL_NEST)
      {
      if (nest == 0)		/* we found the matching start nest	*/
        break;   
    
      --nest;			/* point to the next above element	*/
      if (nest < 0)		/* sanity check - users make this tbl	*/
        {
        return (NULL);
        }
      }
    else 			/* not a start nest element, check to	*/
      {			/* see if we are nesting further	*/
      if (aa2->sel_type == AA_END_NEST)
        nest++;			
      }
    
  /* OK, we did not find the start element for the end nest, look 	*/
  /* some more								*/
    if (aa2 == last_aa)		/* sanity check - users make this tbl	*/
      {
      MLOG_NERR0 ("AA to ASN.1 : Bad AA");
      return (NULL);
      }
    --aa2;
    }
  return (aa2);
  }

/************************************************************************/
/*			ms_log_alt_access  				*/
/************************************************************************/

ST_VOID ms_log_alt_access (ALT_ACCESS *alt_acc)
  {
#ifdef DEBUG_SISCO
ALT_ACC_EL *aa;
ST_INT num_aa;
ST_INT i;
ST_INT indent;
/* Use these strings to keep logging aligned and indented as desired	*/
#define MAX_INDENT 5
SD_CONST ST_CHAR *sp[MAX_INDENT] =
  {
  "  ",
  "    ",
  "      ",
  "        ",
  "          ",
  };
SD_CONST ST_CHAR *sp2[MAX_INDENT] =
  {
  "        ",
  "      ",
  "    ",
  "  ",
  "",
  };

  S_LOCK_COMMON_RESOURCES ();
  aa = alt_acc->aa;
  num_aa = alt_acc->num_aa;
  indent = 0;
  MLOG_ALWAYS1 ("Alternate Access : %d elements", num_aa);

  for (i = 0; i < num_aa; ++i, ++aa)
    {
    if (aa->comp_name_pres)
      {
      MLOG_CALWAYS3   ("%sNamed      %s: %s", 
					sp[indent], 
                                        sp2[indent],
					aa->comp_name);
      }

    switch (aa->sel_type)
      {
      case AA_COMP :
	MLOG_CALWAYS3 ("%sComp       %s: %s", 
					sp[indent], 
                                        sp2[indent],
					aa->u.component);
      break;
      case AA_INDEX :
	MLOG_CALWAYS3 ("%sIndex      %s: %lu", 
					sp[indent], 
                                        sp2[indent],
					(ST_ULONG) aa->u.index);
        if (--indent < 0)
          indent++;
      break;
      case AA_INDEX_RANGE :
	MLOG_CALWAYS4 ("%sIndex Range%s: Low = %lu, Num = %lu", 
					sp[indent], 
                                        sp2[indent],
					(ST_ULONG) aa->u.ir.low_index,
					(ST_ULONG) aa->u.ir.num_elmnts);
        if (--indent < 0)
          indent++;
      break;
      case AA_ALL :
	MLOG_CALWAYS2 ("%sAll        %s:", 
					sp[indent], 
                                        sp2[indent]);
        if (--indent < 0)
          indent++;
      break;
      case AA_ALL_NEST :
	MLOG_CALWAYS2 ("%sAll Nest   %s:", 
					sp[indent], 
                                        sp2[indent]);
        if (++indent >= MAX_INDENT)
          indent--;
      break;
      case AA_COMP_NEST :
        MLOG_CALWAYS3 ("%sComp Nest  %s: %s", 
					sp[indent], 
                                        sp2[indent],
					aa->u.component);
        if (++indent >= MAX_INDENT)
          indent--;
      break;
      case AA_INDEX_NEST :
	MLOG_CALWAYS3 ("%sIndex Nest %s: %lu", 
					sp[indent], 
                                        sp2[indent],
					(ST_ULONG) aa->u.index);
        if (++indent >= MAX_INDENT)
          indent--;
      break;
      case AA_INDEX_RANGE_NEST :
	MLOG_CALWAYS4 ("%sRange Nest %s: Low = %lu, Num = %lu", 
					sp[indent], 
                                        sp2[indent],
					(ST_ULONG) aa->u.ir.low_index,
					(ST_ULONG) aa->u.ir.num_elmnts);
        if (++indent >= MAX_INDENT)
          indent--;
      break;
      case AA_END_NEST :
	MLOG_CALWAYS2 ("%sEnd Nest   %s:", 
					sp[indent], 
                                        sp2[indent]);
        if (--indent < 0)
          indent++;
      break;

      default : 			/* should not be any other tag	*/
	MLOG_CALWAYS1 (" **** UNKNOWN ELEMENT %02d ",i);
      break;
      }
    }
  S_UNLOCK_COMMON_RESOURCES ();
#endif
  }

/************************************************************************/
/*			ms_aa_to_asn1_2					*/
/************************************************************************/
ST_RET ms_aa_to_asn1_2 (ALT_ACCESS *alt_acc,	/* info to be encoded	*/
	ST_UCHAR *buf_ptr,		/* buffer in which to encode	*/
	ST_INT buf_len,			/* buffer length		*/
	ST_UCHAR **asn1_ptr_out,	/* ptr to ptr to encoded data	*/
	ST_INT *asn1_len_out)		/* ptr to encoded length	*/
  {
ASN1_ENC_CTXT aCtx;
ST_RET retcode;

  asn1r_strt_asn1_bld (&aCtx, buf_ptr, buf_len);	/* start build*/
  retcode = ms_aa_to_asn1 (&aCtx, alt_acc);
  if (retcode == SD_SUCCESS)
    {
    *asn1_ptr_out = ASN1_ENC_PTR(&aCtx);
    *asn1_len_out = (int)ASN1_ENC_LEN(&aCtx);
    }
  return (retcode);
  }
