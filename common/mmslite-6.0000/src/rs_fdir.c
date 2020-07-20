/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_fdir.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of file directory.	*/
/*	It decodes the file directory request (indication) and encodes	*/
/*	the file directory response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pfil.h"
#include "asn1defs.h"
#include "mem_chk.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  FDIR_REQ_INFO	*fop_info;

static	ST_INT	max_fnames;
static FILE_NAME	*fname_idx;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID fdir_fs_fname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_ca_fname_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_ca_fname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_fs_fname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_dec_done_ok  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_file_dir_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_file_dir_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT req_size;

  MLOG_CDEC0 ("File Directory Request");

#if defined(FIXED_DECBUF)
  max_fnames = m_sv_max_file_names;
  req_size = mmsl_dec_info_size;
#else
  max_fnames = (_mmsdec_msglen/2);
  if (m_sv_max_file_names && (max_fnames > m_sv_max_file_names))
    max_fnames = m_sv_max_file_names;
  req_size = sizeof (FDIR_REQ_INFO) + (max_fnames * sizeof (FILE_NAME));
#endif

/* get storage for request information	*/
  fop_info = (FDIR_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  if (aCtx->asn1r_elmnt_len > 0)	/* a constructor is present			*/
    {
    fname_idx = (FILE_NAME *)(fop_info + 1);
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 0, fdir_fs_fname_cstr);
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 1, fdir_ca_fname_cstr);
    }
  else			/* all optional constructors were excluded	*/
    {
    fop_info->filespec_pres   = SD_FALSE;	/* reset filespec present	*/
    fop_info->cont_after_pres = SD_FALSE;	/* reset continue after present */
    }
  aCtx->asn1r_decode_done_fun   = fdir_dec_done_ok;
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			fdir_fs_fname_cstr				*/
/* file directory request is being decoded: file specification filename */
/* cstr was encountered. 						*/
/************************************************************************/

static ST_VOID fdir_fs_fname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_fs_fname_cstr");

  fop_info->filespec_pres = SD_TRUE;	/* set file spec present	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = fdir_fs_fname_cstr_done;
  _ms_get_mms_fname (aCtx, fname_idx, max_fnames, &fop_info->num_of_fs_fname);
  }

/************************************************************************/
/*			fdir_fs_fname_cstr_done				*/
/* file directory request is being decoded: file specification file     */
/* name	list  constructor done was encountered.				*/
/************************************************************************/

static ST_VOID fdir_fs_fname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 1, fdir_ca_fname_cstr);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			fdir_ca_fname_cstr				*/
/* file directory request is being decoded: continue after filename     */
/* cstr was encountered. 						*/
/************************************************************************/

static ST_VOID fdir_ca_fname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_ca_fname_cstr");

  fop_info->cont_after_pres = SD_TRUE;	/* set continue name present	*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = fdir_ca_fname_cstr_done;
  fname_idx += fop_info -> num_of_fs_fname;
  _ms_get_mms_fname (aCtx, fname_idx, max_fnames, &fop_info->num_of_ca_fname);
  }

/************************************************************************/
/*			fdir_ca_fname_cstr_done				*/
/* file directory request is being decoded: continue after file name    */
/* list	constructor done was encountered.				*/
/************************************************************************/

static ST_VOID fdir_ca_fname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_ca_fname_cstr_done");

  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			fdir_dec_done_ok				*/
/* file directory request is completely done.  Any extra storage is	*/
/* given back.								*/
/************************************************************************/

static ST_VOID fdir_dec_done_ok (ASN1_DEC_CTXT *aCtx)
  {
ST_INT  req_size;
				/* find actual size for request info	*/
  req_size = sizeof (FDIR_REQ_INFO)
             + (sizeof (FILE_NAME) * fop_info->num_of_fs_fname)
	     + (sizeof (FILE_NAME) * fop_info->num_of_ca_fname);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  fop_info = (FDIR_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _mms_dec_done_ok (aCtx);

  MLOG_PAUSEDEC (NULL);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_fdir_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file directory response.			*/
/************************************************************************/
 
ST_RET mp_fdir_resp (MMSREQ_IND *indptr, FDIR_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_FILE_DIR,indptr,
			M_CAST_MK_FUN (_ms_mk_fdir_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_fdir_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file directory response.			*/
/************************************************************************/
 
ST_RET mpl_fdir_resp (ST_UINT32 invoke_id, FDIR_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_FILE_DIR,invoke_id,
			M_CAST_MK_FUN (_ms_mk_fdir_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_fdir_resp					*/
/* ENCODE a file directory RESPONSE:					*/
/************************************************************************/

ST_VOID _ms_mk_fdir_resp (ASN1_ENC_CTXT *aCtx, FDIR_RESP_INFO *info_ptr)
  {
FDIR_DIR_ENT *ent_ptr;
ST_INT	i;
ST_INT	tab_size;
FDIR_DIR_ENT	**dir_ent_tab;
FDIR_DIR_ENT	*dir_ptr;
ST_INT	dir_ent_idx;

/*  allocate an array of fdir_dir_ent pointers				*/
  
  tab_size =  info_ptr->num_dir_ent * sizeof (FDIR_DIR_ENT *);
  tab_size++; 			/* make sure not 0			*/

  dir_ent_tab = (FDIR_DIR_ENT **)chk_calloc (1, tab_size);

  dir_ptr = (FDIR_DIR_ENT *)(info_ptr + 1);

  for (i=0; i < info_ptr->num_dir_ent; ++i)
    {
    dir_ent_tab[ i ] = dir_ptr;
    dir_ptr = (FDIR_DIR_ENT *) 
       ((FILE_NAME *) (dir_ptr + 1) + dir_ptr -> num_of_fname);
    }

  asn1r_strt_constr (aCtx);			/* start the fdir seq constr	*/

  if (info_ptr->more_follows)
    {					/* NOT the default value -	*/
    asn1r_wr_bool (aCtx, info_ptr->more_follows);	/* encode more follows		*/
    asn1r_fin_prim (aCtx, 1,CTX);			/* finish the primitive 	*/
    }

  asn1r_strt_constr (aCtx);			/* start list of dir ent constr */
  asn1r_strt_constr (aCtx);			/* start seq of dir ent constr	*/

  if (info_ptr->num_dir_ent > 0)	/* if data is to be included -	*/
    {
					/* point to last dir entry	*/
    dir_ent_idx = info_ptr->num_dir_ent - 1;
    ent_ptr = dir_ent_tab[ dir_ent_idx ];
    for (i = 0; i < info_ptr->num_dir_ent; ++i)
      {
      asn1r_strt_constr (aCtx);			/* start the dir entry constr	*/

      asn1r_strt_constr (aCtx);			/* start the file attr constr	*/
      if (ent_ptr->mtimpres)
	{
	asn1r_wr_time (aCtx, ent_ptr->mtime);	/* write the last modified time */
	asn1r_fin_prim (aCtx, 1,CTX);
	}
      asn1r_wr_u32 (aCtx, ent_ptr->fsize);		/* write the file size		*/
      asn1r_fin_prim (aCtx, 0,CTX);
      asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* close file attr constructor	*/

					/* write the file name		*/
      asn1r_strt_constr (aCtx);
      _ms_wr_mms_fname (aCtx, (FILE_NAME *)(ent_ptr + 1), 
                    ent_ptr->num_of_fname);
      asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

      asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* close dir entry constructor	*/
      dir_ent_idx--;
      ent_ptr = dir_ent_tab[ dir_ent_idx ];
      }
    }

  asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish seq of dir ents cstr	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish list of dir ents cstr */

/* write the context specific explicit tag for this opcode.		*/

  asn1r_fin_constr (aCtx, MMSOP_FILE_DIR,CTX,DEF);	/* tag = opcode, ctx constr	*/
  chk_free (dir_ent_tab);
  }
