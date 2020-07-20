/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsefun.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the function declarations used by the primitive 	*/
/*	level encode routines (mp_xxxx).				*/
/*									*/
/*	Parameters : pointer to the operation specific data structure.	*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     02	   Convert to use ASN1R.			*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMSEFUN_INCLUDED
#define MMSEFUN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* primitive encode functions						*/
/************************************************************************/

ST_UCHAR *_ms_mk_init (ASN1_ENC_CTXT *ac, ST_UCHAR *buf_ptr, ST_INT buf_len, 
		INIT_INFO *info_ptr, ST_INT pdutype,
		ST_BOOLEAN cs_init);

ST_UCHAR *_ms_mk_conclude (ASN1_ENC_CTXT *ac, ST_UCHAR *buf_ptr,ST_INT buf_len,ST_INT pdutype);

ST_UCHAR *_ms_mk_cancel (ASN1_ENC_CTXT *ac, ST_BOOLEAN request, ST_UCHAR *msg_ptr, 
			 ST_INT buf_len, ST_UINT32 id);

ST_UCHAR *_ms_mk_reject (ASN1_ENC_CTXT *ac, ST_UCHAR *msg_ptr, ST_INT buf_len,
	REJECT_RESP_INFO *info_ptr);

ST_UCHAR *_ms_mk_err_resp (ASN1_ENC_CTXT *ac, ST_UCHAR *buf_ptr, ST_INT buf_len, ST_INT op,
	ST_UINT32 id, ST_INT16 err_class, ST_INT16 code);

ST_VOID _ms_mk_stat_req      (ASN1_ENC_CTXT *ac, STATUS_REQ_INFO *info);
ST_VOID _ms_mk_stat_resp     (ASN1_ENC_CTXT *ac, STATUS_RESP_INFO *info);
ST_VOID _ms_mk_ustat_req     (ASN1_ENC_CTXT *ac, USTATUS_REQ_INFO *info);
ST_VOID _ms_mk_namel_req     (ASN1_ENC_CTXT *ac, NAMELIST_REQ_INFO *info);
ST_VOID _ms_mk_namel_resp    (ASN1_ENC_CTXT *ac, NAMELIST_RESP_INFO *info);
ST_VOID _ms_mk_ident_resp    (ASN1_ENC_CTXT *ac, IDENT_RESP_INFO *info);
ST_VOID _ms_mk_rename_req    (ASN1_ENC_CTXT *ac, RENAME_REQ_INFO *info);
ST_VOID _ms_mk_getcl_req     (ASN1_ENC_CTXT *ac, GETCL_REQ_INFO *info);
ST_VOID _ms_mk_getcl_resp    (ASN1_ENC_CTXT *ac, GETCL_RESP_INFO *info);

ST_VOID _ms_mk_initdown_req  (ASN1_ENC_CTXT *ac, INITDOWN_REQ_INFO *info);
ST_VOID _ms_mk_initdown_resp (ASN1_ENC_CTXT *ac, ST_VOID *dummy_info);
ST_VOID _ms_mk_download_req  (ASN1_ENC_CTXT *ac, DOWNLOAD_REQ_INFO *info);
ST_VOID _ms_mk_download_resp (ASN1_ENC_CTXT *ac, DOWNLOAD_RESP_INFO *info);
ST_VOID _ms_mk_termdown_req  (ASN1_ENC_CTXT *ac, TERMDOWN_REQ_INFO *info);
ST_VOID _ms_mk_initupl_req   (ASN1_ENC_CTXT *ac, INITUPL_REQ_INFO *info);
ST_VOID _ms_mk_initupl_resp  (ASN1_ENC_CTXT *ac, INITUPL_RESP_INFO *info);
ST_VOID _ms_mk_upload_req    (ASN1_ENC_CTXT *ac, UPLOAD_REQ_INFO *info);
ST_VOID _ms_mk_upload_resp   (ASN1_ENC_CTXT *ac, UPLOAD_RESP_INFO *info);
ST_VOID _ms_mk_termupl_req   (ASN1_ENC_CTXT *ac, TERMUPL_REQ_INFO *info);
ST_VOID _ms_mk_rddwn_req     (ASN1_ENC_CTXT *ac, RDDWN_REQ_INFO *info);
ST_VOID _ms_mk_rdupl_req     (ASN1_ENC_CTXT *ac, RDUPL_REQ_INFO *info);
ST_VOID _ms_mk_loaddom_req   (ASN1_ENC_CTXT *ac, LOADDOM_REQ_INFO *info);
ST_VOID _ms_mk_storedom_req  (ASN1_ENC_CTXT *ac, STOREDOM_REQ_INFO *info);
ST_VOID _ms_mk_deldom_req    (ASN1_ENC_CTXT *ac, DELDOM_REQ_INFO *info);
ST_VOID _ms_mk_getdom_req    (ASN1_ENC_CTXT *ac, GETDOM_REQ_INFO *info);
ST_VOID _ms_mk_getdom_resp   (ASN1_ENC_CTXT *ac, GETDOM_RESP_INFO *info);

ST_VOID _ms_mk_crepi_req     (ASN1_ENC_CTXT *ac, CREPI_REQ_INFO *info);
ST_VOID _ms_mk_delpi_req     (ASN1_ENC_CTXT *ac, DELPI_REQ_INFO *info);
ST_VOID _ms_mk_start_req     (ASN1_ENC_CTXT *ac, START_REQ_INFO *info);
ST_VOID _ms_mk_stop_req      (ASN1_ENC_CTXT *ac, STOP_REQ_INFO *info);
ST_VOID _ms_mk_resume_req    (ASN1_ENC_CTXT *ac, RESUME_REQ_INFO *info);
ST_VOID _ms_mk_reset_req     (ASN1_ENC_CTXT *ac, RESET_REQ_INFO *info);
ST_VOID _ms_mk_kill_req      (ASN1_ENC_CTXT *ac, KILL_REQ_INFO *info);
ST_VOID _ms_mk_getpi_req     (ASN1_ENC_CTXT *ac, GETPI_REQ_INFO *info);
ST_VOID _ms_mk_getpi_resp    (ASN1_ENC_CTXT *ac, GETPI_RESP_INFO *info);

ST_VOID _ms_mk_read_req      (ASN1_ENC_CTXT *ac, READ_REQ_INFO *info);
ST_VOID _ms_mk_read_resp     (ASN1_ENC_CTXT *ac, READ_RESP_INFO *info);
ST_VOID _ms_mk_write_req     (ASN1_ENC_CTXT *ac, WRITE_REQ_INFO *info);
ST_VOID _ms_mk_write_resp    (ASN1_ENC_CTXT *ac, WRITE_RESP_INFO *info);
ST_VOID _ms_mk_info_req      (ASN1_ENC_CTXT *ac, INFO_REQ_INFO *info);
ST_VOID _ms_mk_getvar_req    (ASN1_ENC_CTXT *ac, GETVAR_REQ_INFO *info);
ST_VOID _ms_mk_getvar_resp   (ASN1_ENC_CTXT *ac, GETVAR_RESP_INFO *info);
ST_VOID _ms_mk_defvar_req    (ASN1_ENC_CTXT *ac, DEFVAR_REQ_INFO *info);
ST_VOID _ms_mk_delvar_req    (ASN1_ENC_CTXT *ac, DELVAR_REQ_INFO *info);
ST_VOID _ms_mk_delvar_resp   (ASN1_ENC_CTXT *ac, DELVAR_RESP_INFO *info);
ST_VOID _ms_mk_defscat_req   (ASN1_ENC_CTXT *ac, DEFSCAT_REQ_INFO *info);
ST_VOID _ms_mk_getscat_req   (ASN1_ENC_CTXT *ac, GETSCAT_REQ_INFO *info);
ST_VOID _ms_mk_getscat_resp  (ASN1_ENC_CTXT *ac, GETSCAT_RESP_INFO *info);
ST_VOID _ms_mk_defvlist_req  (ASN1_ENC_CTXT *ac, DEFVLIST_REQ_INFO *info);
ST_VOID _ms_mk_getvlist_req  (ASN1_ENC_CTXT *ac, GETVLIST_REQ_INFO *info);
ST_VOID _ms_mk_getvlist_resp (ASN1_ENC_CTXT *ac, GETVLIST_RESP_INFO *info);
ST_VOID _ms_mk_delvlist_req  (ASN1_ENC_CTXT *ac, DELVLIST_REQ_INFO *info);
ST_VOID _ms_mk_delvlist_resp (ASN1_ENC_CTXT *ac, DELVLIST_RESP_INFO *info);
ST_VOID _ms_mk_gettype_req   (ASN1_ENC_CTXT *ac, GETTYPE_REQ_INFO *info);
ST_VOID _ms_mk_gettype_resp  (ASN1_ENC_CTXT *ac, GETTYPE_RESP_INFO *info);
ST_VOID _ms_mk_deftype_req   (ASN1_ENC_CTXT *ac, DEFTYPE_REQ_INFO *info);
ST_VOID _ms_mk_deltype_req   (ASN1_ENC_CTXT *ac, DELTYPE_REQ_INFO *info);
ST_VOID _ms_mk_deltype_resp  (ASN1_ENC_CTXT *ac, DELTYPE_RESP_INFO *info);

ST_VOID _ms_mk_takectrl_req  (ASN1_ENC_CTXT *ac, TAKECTRL_REQ_INFO *info);
ST_VOID _ms_mk_takectrl_resp (ASN1_ENC_CTXT *ac, TAKECTRL_RESP_INFO *info);
ST_VOID _ms_mk_relctrl_req   (ASN1_ENC_CTXT *ac, RELCTRL_REQ_INFO *info);
ST_VOID _ms_mk_rsstat_req    (ASN1_ENC_CTXT *ac, RSSTAT_REQ_INFO *info);
ST_VOID _ms_mk_rsstat_resp   (ASN1_ENC_CTXT *ac, RSSTAT_RESP_INFO *info);
ST_VOID _ms_mk_rspool_req    (ASN1_ENC_CTXT *ac, RSPOOL_REQ_INFO *info);
ST_VOID _ms_mk_rspool_resp   (ASN1_ENC_CTXT *ac, RSPOOL_RESP_INFO *info);
ST_VOID _ms_mk_rsentry_req   (ASN1_ENC_CTXT *ac, RSENTRY_REQ_INFO *info);
ST_VOID _ms_mk_rsentry_resp  (ASN1_ENC_CTXT *ac, RSENTRY_RESP_INFO *info);
ST_VOID _ms_mk_defsem_req    (ASN1_ENC_CTXT *ac, DEFSEM_REQ_INFO *info);
ST_VOID _ms_mk_delsem_req    (ASN1_ENC_CTXT *ac, DELSEM_REQ_INFO *info);

ST_VOID _ms_mk_output_req    (ASN1_ENC_CTXT *ac, OUTPUT_REQ_INFO *info);
ST_VOID _ms_mk_input_req     (ASN1_ENC_CTXT *ac, INPUT_REQ_INFO *info);
ST_VOID _ms_mk_input_resp    (ASN1_ENC_CTXT *ac, INPUT_RESP_INFO *info);

ST_VOID _ms_mk_defec_req     (ASN1_ENC_CTXT *ac, DEFEC_REQ_INFO *info);
ST_VOID _ms_mk_delec_req     (ASN1_ENC_CTXT *ac, DELEC_REQ_INFO *info);
ST_VOID _ms_mk_delec_resp    (ASN1_ENC_CTXT *ac, DELEC_RESP_INFO *info);
ST_VOID _ms_mk_geteca_req    (ASN1_ENC_CTXT *ac, GETECA_REQ_INFO *info);
ST_VOID _ms_mk_geteca_resp   (ASN1_ENC_CTXT *ac, GETECA_RESP_INFO *info);
ST_VOID _ms_mk_repecs_req    (ASN1_ENC_CTXT *ac, REPECS_REQ_INFO *info);
ST_VOID _ms_mk_repecs_resp   (ASN1_ENC_CTXT *ac, REPECS_RESP_INFO *info);
ST_VOID _ms_mk_altecm_req    (ASN1_ENC_CTXT *ac, ALTECM_REQ_INFO *info);
ST_VOID _ms_mk_trige_req     (ASN1_ENC_CTXT *ac, TRIGE_REQ_INFO *info);
ST_VOID _ms_mk_defea_req     (ASN1_ENC_CTXT *ac, DEFEA_REQ_INFO *info);
ST_VOID _ms_mk_delea_req     (ASN1_ENC_CTXT *ac, DELEA_REQ_INFO *info);
ST_VOID _ms_mk_delea_resp    (ASN1_ENC_CTXT *ac, DELEA_RESP_INFO *info);
ST_VOID _ms_mk_geteaa_req    (ASN1_ENC_CTXT *ac, GETEAA_REQ_INFO *info);
ST_VOID _ms_mk_geteaa_resp   (ASN1_ENC_CTXT *ac, GETEAA_RESP_INFO *info);
ST_VOID _ms_mk_repeas_req    (ASN1_ENC_CTXT *ac, REPEAS_REQ_INFO *info);
ST_VOID _ms_mk_repeas_resp   (ASN1_ENC_CTXT *ac, REPEAS_RESP_INFO *info);
ST_VOID _ms_mk_defee_req     (ASN1_ENC_CTXT *ac, DEFEE_REQ_INFO *info);
ST_VOID _ms_mk_delee_req     (ASN1_ENC_CTXT *ac, DELEE_REQ_INFO *info);
ST_VOID _ms_mk_delee_resp    (ASN1_ENC_CTXT *ac, DELEE_RESP_INFO *info);
ST_VOID _ms_mk_altee_req     (ASN1_ENC_CTXT *ac, ALTEE_REQ_INFO *info);
ST_VOID _ms_mk_altee_resp    (ASN1_ENC_CTXT *ac, ALTEE_RESP_INFO *info);
ST_VOID _ms_mk_repees_req    (ASN1_ENC_CTXT *ac, REPEES_REQ_INFO *info);
ST_VOID _ms_mk_repees_resp   (ASN1_ENC_CTXT *ac, REPEES_RESP_INFO *info);
ST_VOID _ms_mk_geteea_req    (ASN1_ENC_CTXT *ac, GETEEA_REQ_INFO *info);
ST_VOID _ms_mk_geteea_resp   (ASN1_ENC_CTXT *ac, GETEEA_RESP_INFO *info);
ST_VOID _ms_mk_evnot_req     (ASN1_ENC_CTXT *ac, EVNOT_REQ_INFO *info);
ST_VOID _ms_mk_ackevnot_req  (ASN1_ENC_CTXT *ac, ACKEVNOT_REQ_INFO *info);
ST_VOID _ms_mk_getas_req     (ASN1_ENC_CTXT *ac, GETAS_REQ_INFO *info);
ST_VOID _ms_mk_getas_resp    (ASN1_ENC_CTXT *ac, GETAS_RESP_INFO *info);
ST_VOID _ms_mk_getaes_req    (ASN1_ENC_CTXT *ac, GETAES_REQ_INFO *info);
ST_VOID _ms_mk_getaes_resp   (ASN1_ENC_CTXT *ac, GETAES_RESP_INFO *info);

ST_VOID _ms_mk_jwrite_req    (ASN1_ENC_CTXT *ac, JWRITE_REQ_INFO *info);
ST_VOID _ms_mk_jread_req     (ASN1_ENC_CTXT *ac, JREAD_REQ_INFO *info);
ST_VOID _ms_mk_jread_resp    (ASN1_ENC_CTXT *ac, JREAD_RESP_INFO *info);
ST_VOID _ms_mk_jinit_req     (ASN1_ENC_CTXT *ac, JINIT_REQ_INFO *info);
ST_VOID _ms_mk_jinit_resp    (ASN1_ENC_CTXT *ac, JINIT_RESP_INFO *info);
ST_VOID _ms_mk_jstat_req     (ASN1_ENC_CTXT *ac, JSTAT_REQ_INFO *info);
ST_VOID _ms_mk_jstat_resp    (ASN1_ENC_CTXT *ac, JSTAT_RESP_INFO *info);
ST_VOID _ms_mk_jcreate_req   (ASN1_ENC_CTXT *ac, JCREATE_REQ_INFO *info);
ST_VOID _ms_mk_jdelete_req   (ASN1_ENC_CTXT *ac, JDELETE_REQ_INFO *info);

ST_VOID _ms_mk_obtfile_req   (ASN1_ENC_CTXT *ac, OBTFILE_REQ_INFO *info);
ST_VOID _ms_mk_fopen_req     (ASN1_ENC_CTXT *ac, FOPEN_REQ_INFO *info);
ST_VOID _ms_mk_fopen_resp    (ASN1_ENC_CTXT *ac, FOPEN_RESP_INFO *info);
ST_VOID _ms_mk_fread_req     (ASN1_ENC_CTXT *ac, FREAD_REQ_INFO *info);
ST_VOID _ms_mk_fread_resp    (ASN1_ENC_CTXT *ac, FREAD_RESP_INFO *info);
ST_VOID _ms_mk_fclose_req    (ASN1_ENC_CTXT *ac, FCLOSE_REQ_INFO *info);
ST_VOID _ms_mk_frename_req   (ASN1_ENC_CTXT *ac, FRENAME_REQ_INFO *info);
ST_VOID _ms_mk_fdelete_req   (ASN1_ENC_CTXT *ac, FDELETE_REQ_INFO *info);
ST_VOID _ms_mk_fdir_req      (ASN1_ENC_CTXT *ac, FDIR_REQ_INFO *info);
ST_VOID _ms_mk_fdir_resp     (ASN1_ENC_CTXT *ac, FDIR_RESP_INFO *info);

#ifdef __cplusplus
}
#endif

#endif		/* MMSEFUN_INCLUDED */
