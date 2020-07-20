/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_fdir.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of file directory.	*/
/*	It decodes the file directory response (confirm) & encodes the	*/
/*	file directory request. 					*/
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
#include "mms_pfil.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  FDIR_RESP_INFO	*fop_info;

static FDIR_DIR_ENT   *ent_ptr;
static FDIR_DIR_ENT   *dir_ptr;
static ST_INT max_dir_ents;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID fdir_dir_entry       (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_more_follows    (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_get_fil_mtim    (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_dir_ent_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_get_fil_siz     (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_get_fil_attr    (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_fname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_fname_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_dir_list_done   (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_dir_seq         (ASN1_DEC_CTXT *aCtx);
static ST_VOID fdir_dir_list        (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_file_dir_rsp				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_file_dir_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("File Directory Response");


#if defined(FIXED_DECBUF)
  max_dir_ents = m_cl_max_file_directory;
  rsp_size = mmsl_dec_info_size;
#else
  max_dir_ents = _mmsdec_msglen/8;	/* determine maximum size	*/
  if (m_cl_max_file_directory && (max_dir_ents > m_cl_max_file_directory))
    max_dir_ents = m_cl_max_file_directory;

/* get storage for resp info	*/
  rsp_size    = sizeof (FDIR_RESP_INFO) 
		 + (max_dir_ents * (sizeof (FDIR_DIR_ENT)))
		 + (max_dir_ents * (sizeof (FILE_NAME)));
#endif

/* get storage for request information	*/
  fop_info = (FDIR_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  fop_info->num_dir_ent  = 0;		   /* set default values	*/
  fop_info->more_follows = SD_FALSE;
  ent_ptr = (FDIR_DIR_ENT *) (fop_info + 1);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, fdir_dir_list);
  }

/************************************************************************/
/*			fdir_dir_list					*/
/* file directory response is being decoded: directory list constructor */
/* was encountered.							*/
/************************************************************************/

static ST_VOID fdir_dir_list (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_dir_list");

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, fdir_dir_seq);
  }

/************************************************************************/
/*			fdir_dir_seq					*/
/* file directory response is being decoded: directory entry constr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID fdir_dir_seq (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_dir_seq");

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, fdir_dir_entry);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level - 1] = fdir_dir_list_done;
  }

/************************************************************************/
/*			fdir_dir_entry					*/
/* file directory response is being decoded: directory sequence was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID fdir_dir_entry (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_dir_entry");

  fop_info->num_dir_ent++;	/* increment directory entry count	*/
  if (fop_info->num_dir_ent > max_dir_ents)
    asn1r_set_dec_err (aCtx, RESP_UNSPECIFIED);

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,0,fdir_fname_cstr);
  }

/************************************************************************/
/*			fdir_fname_cstr				*/
/* request domain download request is being decoded: filename cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID fdir_fname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_fname_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = fdir_fname_cstr_done;
  _ms_get_mms_fname (aCtx, (FILE_NAME *)(ent_ptr + 1), 
		max_dir_ents, &ent_ptr->num_of_fname);
  }

/************************************************************************/
/*			fdir_fname_cstr_done				*/
/* request domain download request is being decoded: file name	list	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID fdir_fname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, fdir_get_fil_attr);
  }

/************************************************************************/
/*			fdir_get_fil_attr				*/
/* file directory response is being decoded: file attributes constr was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID fdir_get_fil_attr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_get_fil_attr");

  ASN1R_TAG_ADD (aCtx, CTX, 0, fdir_get_fil_siz);
  }

/************************************************************************/
/*			fdir_get_fil_siz				*/
/* file directory response is being decoded: file size was found,	*/
/* modified time can be next, or can be done.				*/
/************************************************************************/

static ST_VOID fdir_get_fil_siz (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_get_fil_si");

  if (asn1r_get_u32 (aCtx, &(ent_ptr->fsize)))	/* read the file size		*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX,1,fdir_get_fil_mtim);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;	/* ok for file attr to be done	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level-1] = fdir_dir_ent_done;
  }

/************************************************************************/
/*			fdir_get_fil_mtim				*/
/* file directory response is being decoded: directory entry was	*/
/* encountered, and modified time was found; creator can be next.	*/
/************************************************************************/

static ST_VOID fdir_get_fil_mtim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_get_fil_mtim");

  ent_ptr->mtimpres = SD_TRUE;		/* set modify time present	*/
  if (asn1r_get_time (aCtx, &ent_ptr->mtime))	/* read the last modified time	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  }

/************************************************************************/
/*			fdir_dir_ent_done				*/
/* DECODING file directory response: directory entry was completed.	*/
/* Re-initialize the valid states.					*/
/************************************************************************/

static ST_VOID fdir_dir_ent_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_dir_ent_done");

/* point to where the next directory entry will start			*/

  ent_ptr = (FDIR_DIR_ENT *)(((FILE_NAME *)(ent_ptr + 1))
            + (ent_ptr -> num_of_fname));

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, fdir_dir_entry);
  }

/************************************************************************/
/*			fdir_dir_list_done				*/
/* DECODING file directory response: directory list was completed.	*/
/* Re-initialize the valid states.					*/
/************************************************************************/

static ST_VOID fdir_dir_list_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	resp_size;
ST_INT	i;
ST_INT	fname_count;

  MLOG_CDEC0 ("fdir_dir_list_done");

  fname_count = 0;
  ASN1R_TAG_ADD (aCtx, CTX, 1, fdir_more_follows);

  			/* find the exact size of the storage used	*/

  dir_ptr = (FDIR_DIR_ENT *)(fop_info + 1);
  for (i=0; i < fop_info->num_dir_ent; ++i)
    {
    fname_count += dir_ptr->num_of_fname;
    dir_ptr = (FDIR_DIR_ENT *)(((FILE_NAME *)(dir_ptr + 1))
              + (dir_ptr -> num_of_fname));
    }
			/* give back unused storage			*/
  resp_size = sizeof (FDIR_RESP_INFO) 
	      + ((fop_info->num_dir_ent) * sizeof (FDIR_DIR_ENT))
	      + (fname_count * sizeof (FILE_NAME));
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,resp_size);
  fop_info = (FDIR_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			fdir_more_follows				*/
/* file directory response is being decoded:  more follows indicator	*/
/* has been encountered.						*/
/************************************************************************/

static ST_VOID fdir_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("fdir_more_follows");

  if (asn1r_get_bool (aCtx, &(fop_info->more_follows)))	/* read more follows	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_fdir 					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file directory request.			*/
/************************************************************************/

MMSREQ_PEND *mp_fdir (ST_INT chan, FDIR_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_FILE_DIR,
			M_CAST_MK_FUN (_ms_mk_fdir_req),
			chan,(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_fdir 					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the file directory request.			*/
/************************************************************************/

ST_RET  mpl_fdir (FDIR_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_FILE_DIR,
			M_CAST_MK_FUN (_ms_mk_fdir_req),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_fdir_req					*/
/* ENCODE a file directory REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_fdir_req (ASN1_ENC_CTXT *aCtx, FDIR_REQ_INFO *info)
  {
FILE_NAME	*fname_idx;

  if (info->cont_after_pres)
    {					/* write continue after name	*/
    asn1r_strt_constr (aCtx);
    fname_idx = (FILE_NAME *)(info + 1);
    fname_idx += info->num_of_fs_fname;
    _ms_wr_mms_fname (aCtx, fname_idx, info->num_of_ca_fname);
    asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context specific tag 1	*/
    }

  if (info->filespec_pres)
    {					/* write the file specification */
    asn1r_strt_constr (aCtx);
    fname_idx = (FILE_NAME *)(info + 1);
    _ms_wr_mms_fname (aCtx, fname_idx, info->num_of_fs_fname);
    asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/
    }

		/* write the context specific tag for this opcode	*/
  asn1r_fin_constr (aCtx, MMSOP_FILE_DIR,CTX,DEF);
  }
