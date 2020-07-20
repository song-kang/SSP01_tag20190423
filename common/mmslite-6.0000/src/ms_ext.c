/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ms_ext.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 03/11/04  GLB     04    Cleaned up "thisFileName"                    */
/* 03/31/03  JRB     03    asn1r_get_bitstr: add max_bits arg.		*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     01    Added SD_CONST modifiers			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && defined(S_MT_SUPPORT)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/

ST_VOID ms_get_extern (ASN1_DEC_CTXT *aCtx, EXTERN_INFO *dest);

static EXTERN_INFO *ext_dest;

static ST_VOID ext_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID ext_cstr_done (ASN1_DEC_CTXT *aCtx);

static ST_INT save_method; 	/* storage for decode method		*/

/************************************************************************/
/*			ms_decode_extern				*/
/************************************************************************/

ST_RET ms_decode_extern (ST_UCHAR *asn1ptr, ST_INT asn1len, 
			   EXTERN_INFO *dest)
  {
ST_RET ret;
ASN1_DEC_CTXT localDecCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_DEC_CTXT *aCtx = &localDecCtx;

  memset (aCtx, 0, sizeof (ASN1_DEC_CTXT));	/* CRITICAL: start clean.	*/
  S_LOCK_COMMON_RESOURCES ();
  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;		/* select tag method			*/
  aCtx->asn1r_decode_done_fun = asn1r_done_err;/* not legal to be done at this time	*/
  aCtx->asn1r_err_fun = NULL;

  ext_dest = dest;

  ASN1R_TAG_ADD (aCtx, UNI|CONSTR, EXTERN_CODE, ext_cstr);
  asn1r_decode_asn1 (aCtx, asn1ptr,asn1len);/* decode entire ASN.1 'Data' entity    */

  if (aCtx->asn1r_pdu_dec_err == NO_DECODE_ERR)	/* check for sucess or return error	*/
    ret = SD_SUCCESS;
  else
    ret = MVE_DATA_CONVERT;

  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

/************************************************************************/
/*			   ext_cstr					*/
/* This function is called when an 'EXTERNAL' data element has been	*/
/* found. It will put info into the selected structure.			*/
/************************************************************************/

static ST_VOID ext_cstr (ASN1_DEC_CTXT *aCtx)
  {
  ms_get_extern (aCtx, ext_dest);
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = ext_cstr_done;
  }


static ST_VOID ext_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_decode_done = SD_TRUE;
  aCtx->asn1r_decode_done_fun = NULL;
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/

static EXTERN_INFO *ext_info_ptr;
static ST_VOID get_dir_ref (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_indir_ref (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_objdesc (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_sngl_asn1 (ASN1_DEC_CTXT *aCtx);
static ST_VOID sngl_parse_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID sngl_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_oct_alignd_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_oct_alignd_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID oct_alignd_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_arb_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_arb_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID arb_done (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			   ms_get_extern					*/
/* This function is called when an 'EXTERNAL' data element has been	*/
/* found. It will put info into the selected structure.			*/
/************************************************************************/

ST_VOID ms_get_extern (ASN1_DEC_CTXT *aCtx, EXTERN_INFO *dest)
  {
  save_method = aCtx->asn1r_decode_method;			/* use class method	*/
  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;

  ext_info_ptr = dest;				/* save the destination	*/
  ext_info_ptr -> dir_ref_pres = SD_FALSE;         /* clear pres flag      */
  ext_info_ptr -> indir_ref_pres = SD_FALSE;       /* clear pres flag      */
  ext_info_ptr -> dv_descr_pres = SD_FALSE;	/* clear present flag   */

  ASN1R_TAG_ADD (aCtx, UNI,OBJ_ID_CODE,get_dir_ref);	/* transfer syntax name	*/
  ASN1R_TAG_ADD (aCtx, UNI,INT_CODE,get_indir_ref);		/* PCI			*/
  ASN1R_TAG_ADD (aCtx, UNI,OBJDSCR_CODE,get_objdesc);	/* object descripter    */
						/* Data Types		*/
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,get_sngl_asn1);		/* single asn1 type 	*/
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,get_oct_alignd_cstr);	/* oct alignd, cstr	*/
  ASN1R_TAG_ADD (aCtx, CTX,1,get_oct_alignd_prim);		/* oct alignd, prim	*/
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,get_arb_cstr);		/* arbitrary, cstr	*/
  ASN1R_TAG_ADD (aCtx, CTX,2,get_arb_prim);			/* arbitrary, prim	*/
  }

/************************************************************************/
/*			   get_dir_ref					*/
/* This is the function called when :					*/
/************************************************************************/

static ST_VOID get_dir_ref (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_dir_ref");

  ext_info_ptr->dir_ref_pres = SD_TRUE;		/* mark present		*/

  if (asn1r_get_objid (aCtx, ext_info_ptr->dir_ref.comps,
		 &ext_info_ptr->dir_ref.num_comps))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE); 
    return;                          
    }

  ASN1R_TAG_ADD (aCtx, UNI,INT_CODE,get_indir_ref);		/* indir ref maybe	*/
  ASN1R_TAG_ADD (aCtx, UNI,OBJDSCR_CODE,get_objdesc);	/* object descripter    */
						/* Data Types		*/
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,get_sngl_asn1);		/* single asn1 type 	*/
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,get_oct_alignd_cstr);	/* oct alignd, cstr	*/
  ASN1R_TAG_ADD (aCtx, CTX,1,get_oct_alignd_prim);		/* oct alignd, prim	*/
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,get_arb_cstr);		/* arbitrary, cstr	*/
  ASN1R_TAG_ADD (aCtx, CTX,2,get_arb_prim);			/* arbitrary, prim	*/
  }

/************************************************************************/
/*			   get_indir_ref					*/
/* This is the function called when the PCI element of a PDV is found	*/
/************************************************************************/

static ST_VOID get_indir_ref (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_indir_ref");

  if (asn1r_get_i32 (aCtx, &ext_info_ptr->indir_ref))   /* If out of range for us	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  else
    {
    ext_info_ptr->indir_ref_pres = SD_TRUE;
    ASN1R_TAG_ADD (aCtx, UNI,OBJDSCR_CODE,get_objdesc);	/* object descripter    */
						/* Data Types		*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,get_sngl_asn1);	/* single asn1 type 	*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,get_oct_alignd_cstr);	/* oct alignd, cstr	*/
    ASN1R_TAG_ADD (aCtx, CTX,1,get_oct_alignd_prim);	/* oct alignd, prim	*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,get_arb_cstr);	/* arbitrary, cstr	*/
    ASN1R_TAG_ADD (aCtx, CTX,2,get_arb_prim);		/* arbitrary, prim	*/
    }
  }

/************************************************************************/
/*			   get_objdesc					*/
/* This is the function called when the PCI element of a PDV is found	*/
/************************************************************************/

static ST_VOID get_objdesc (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_objdesc");

  ext_info_ptr->dv_descr_pres = SD_TRUE;		/* mark present		*/
  ext_info_ptr->dv_descr_len = aCtx->asn1r_elmnt_len;	/* len of string	*/
  ext_info_ptr->dv_descr = aCtx->asn1r_field_ptr;		/* leave it lie		*/
  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;			/* skip over		*/

/* set up to get presentation data values - choice of three		*/

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,get_sngl_asn1);	/* single asn1 type 	*/
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,get_oct_alignd_cstr);	/* oct alignd, cstr	*/
  ASN1R_TAG_ADD (aCtx, CTX,1,get_oct_alignd_prim);	/* oct alignd, prim	*/
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,get_arb_cstr);	/* arbitrary, cstr	*/
  ASN1R_TAG_ADD (aCtx, CTX,2,get_arb_prim);		/* arbitrary, prim	*/
  }

/************************************************************************/
/*			   get_sngl_asn1				*/
/* This is the function called when :					*/
/************************************************************************/

static ST_VOID get_sngl_asn1 (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_sngl_asn1");

/* aCtx->asn1r_field_ptr points to 'any', elmnt is length of data			*/
  ext_info_ptr->encoding_tag = 0;		/* write tag		*/
  ext_info_ptr->data_len = aCtx->asn1r_elmnt_len;		/* len of ANY		*/
  ext_info_ptr->data_ptr = aCtx->asn1r_field_ptr;		/* leave it lie		*/

  asn1r_parse_next (aCtx, sngl_parse_done);
  }


/************************************************************************/
/*			   sngl_parse_done				*/
/*									*/
/************************************************************************/

static ST_VOID sngl_parse_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sngl_parse_done");

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = sngl_done;
  }

/************************************************************************/
/*			   sngl_done 					*/
/* This is the cstr done function called when :				*/
/************************************************************************/

static ST_VOID sngl_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("sngl_done");

  aCtx->asn1r_decode_method = save_method;		/* this extern is now done	*/
  }


/************************************************************************/
/*			   get_oct_alignd_prim				*/
/* This is the function called when :					*/
/************************************************************************/

static ST_VOID get_oct_alignd_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_oct_alignd_prim");

/* aCtx->asn1r_elmnt_len == # octects */

  ext_info_ptr->encoding_tag = 1;		/* write tag		*/
  ext_info_ptr->data_len = aCtx->asn1r_elmnt_len;		/* len of ANY		*/
  ext_info_ptr->data_ptr = aCtx->asn1r_field_ptr;		/* leave it lie		*/

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;			/* skip over data	*/

  aCtx->asn1r_decode_method = save_method;		/* this extern is now done	*/
  }

/************************************************************************/
/*			   get_oct_alignd_cstr				*/
/* This is the function called when :					*/
/************************************************************************/

static ST_VOID get_oct_alignd_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_oct_alignd_cstr");

  ext_info_ptr->encoding_tag = 1;		/* write tag		*/
  ext_info_ptr->data_ptr = aCtx->asn1r_field_ptr;		/* leave it lie		*/
  asn1r_get_octstr_cstr (aCtx, ASN1_MAX_PDU, aCtx->asn1r_field_ptr);
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = oct_alignd_done;
  }

/************************************************************************/
/*			   oct_alignd_done 				*/
/* This is the cstr done function called when :				*/
/************************************************************************/

static ST_VOID oct_alignd_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("oct_alignd_done");

  ext_info_ptr->data_len = aCtx->asn1r_octetcount;		/* len of octet string  */
  aCtx->asn1r_decode_method = save_method;		/* this extern is now done	*/
  }

/************************************************************************/
/*			   get_arb_prim					*/
/* This is the function called when :					*/
/************************************************************************/

static ST_VOID get_arb_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_arb_prim");

/* aCtx->asn1r_bitcount = bits */

  ext_info_ptr->encoding_tag = 2;		/* write tag		*/
  ext_info_ptr->data_len = aCtx->asn1r_elmnt_len-1;	/* len of ANY		*/
  ext_info_ptr->data_ptr = aCtx->asn1r_field_ptr;		/* leave it lie		*/
  asn1r_get_bitstr (aCtx, aCtx->asn1r_field_ptr,0);	/* max_bits=0 (no limit)*/
  ext_info_ptr->num_bits = aCtx->asn1r_bitcount;
  aCtx->asn1r_decode_method = save_method;		/* this extern is now done	*/
  }

/************************************************************************/
/*			   get_arb_cstr					*/
/* This is the function called when :					*/
/************************************************************************/

static ST_VOID get_arb_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_arb_cstr");

  ext_info_ptr->encoding_tag = 2;		/* write tag		*/
  ext_info_ptr->data_ptr = aCtx->asn1r_field_ptr;		/* leave it lie		*/

  asn1r_get_bitstr_cstr (aCtx, ASN1_MAX_PDU, aCtx->asn1r_field_ptr);

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = arb_done;
  }


/************************************************************************/
/*			   arb_done 					*/
/* This is the cstr done function called when :				*/
/************************************************************************/

static ST_VOID arb_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("arb_done");

  ext_info_ptr->num_bits = aCtx->asn1r_bitcount;
  ext_info_ptr->data_len = CALC_BIT_LEN (aCtx->asn1r_bitcount);
  aCtx->asn1r_decode_method = save_method;		/* this extern is now done	*/
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			 ms_encode_extern 				*/
/************************************************************************/

ST_RET ms_encode_extern (EXTERN_INFO *ext, ST_UCHAR *dest, ST_INT dest_len,
	ST_INT *ext_len, ST_UCHAR **ext_ptr)
  {
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  asn1r_strt_asn1_bld (aCtx, dest,dest_len);	/* init the builder     */
  wr_external (aCtx, ext);

  *ext_ptr = aCtx->asn1r_field_ptr + 1;
  *ext_len = (int) ((dest+dest_len) - *ext_ptr);

  if (!aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
    ret = SD_SUCCESS;
  else
    ret = ME_ASN1_ENCODE_OVERRUN;

  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

/************************************************************************/
/*			 wr_external 					*/
/************************************************************************/

ST_VOID wr_external (ASN1_ENC_CTXT *aCtx, EXTERN_INFO *ext)
  {
  asn1r_strt_constr (aCtx);		/* start EXTERNAL			*/

  if (ext->encoding_tag == 0)	/* ANY */
    {
    asn1r_wr_octstr (aCtx, ext->data_ptr,ext->data_len);
    asn1r_fin_constr (aCtx, 0,CTX,DEF);   
    }
  else if (ext->encoding_tag == 1)	/* Octet Aligned */
    {
    asn1r_wr_octstr (aCtx, ext->data_ptr,ext->data_len);
    asn1r_fin_prim (aCtx, 1,CTX);		/* finish octet-aligned string		*/
    }
  else if (ext->encoding_tag == 2)	/* Arbitrary			*/
    {
    asn1r_wr_bitstr (aCtx, ext->data_ptr,ext->num_bits);
    asn1r_fin_prim (aCtx, 2,CTX);		
    }

  if (ext->dv_descr_pres)
    {
    asn1r_wr_octstr (aCtx, ext->dv_descr,ext->dv_descr_len);
    asn1r_fin_prim (aCtx, OBJDSCR_CODE,UNI);	/* object descripter    */
    }

  if (ext->indir_ref_pres)
    {
    asn1r_wr_i32 (aCtx, ext->indir_ref);
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    }

  if (ext->dir_ref_pres)
    {
    asn1r_wr_objid (aCtx, ext->dir_ref.comps,ext->dir_ref.num_comps);
    asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
    }

  asn1r_fin_constr (aCtx, EXTERN_CODE,UNI,DEF);		/* finish EXTERNAL	*/
  }


