/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_dfun.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the function declarations required to interface 	*/
/*	with the primitive level decode routines.			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     01    Convert to use ASN1R.			*/
/*			   Add mms_err_invoke_fun proto.			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_DFUN_INCLUDED
#define MMS_DFUN_INCLUDED

#include "asn1r.h"	/* need ASN1_DEC_CTXT	*/

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/************************************************************************/
/* MMS service decode functions						*/
/************************************************************************/

ST_VOID mms_reject 		(ASN1_DEC_CTXT *ac, ST_UINT16 id_code);
ST_VOID mms_err_invoke_fun (ASN1_DEC_CTXT *aCtx, ST_UINT16);
ST_VOID mms_status_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_status_rsp   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_status		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_ustatus_req  	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_namelist_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_namelist_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_identify_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_identify_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rename_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_getcl_req  		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_getcl_rsp  		(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_init_download_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_init_download_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_download_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_download_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_term_download_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_term_download_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_init_upload_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_init_upload_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_upload_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_upload_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_term_upload_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_term_upload_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rddwn_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rddwn_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rdupl_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rdupl_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_load_domain_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_load_domain_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_store_domain_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_store_domain_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delete_domain_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delete_domain_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_dom_attr_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_dom_attr_rsp 	(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_create_pi_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_create_pi_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delete_pi_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delete_pi_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_start_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_stop_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_stop_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_resume_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_resume_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_reset_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_reset_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_kill_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_kill_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_pi_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_pi_rsp 		(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_read_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_read_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_write_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_write_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_info_rpt_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_var_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_var_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_def_var_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_def_var_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_del_var_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_del_var_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_def_scat_req 	(ASN1_DEC_CTXT *aCtx);      	
ST_VOID mms_get_scat_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_scat_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_def_vlist_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_vlist_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_vlist_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_del_vlist_req 	(ASN1_DEC_CTXT *aCtx);     	
ST_VOID mms_del_vlist_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_type_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_get_type_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_def_type_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_def_type_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_del_type_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_del_type_rsp 	(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_takectrl_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_takectrl_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_relctrl_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_relctrl_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rsstat_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rsstat_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rspool_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rspool_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rsentry_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_rsentry_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_defsem_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_defsem_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delsem_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delsem_rsp 		(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_output_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_output_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_input_req  		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_input_rsp  		(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_defec_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delec_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delec_rsp    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_geteca_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_geteca_rsp   	(ASN1_DEC_CTXT *aCtx);      	
ST_VOID mms_repecs_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_repecs_rsp   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_altecm_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_trige_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_defea_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delea_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delea_rsp    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_geteaa_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_geteaa_rsp   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_repeas_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_repeas_rsp   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_defee_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delee_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_delee_rsp    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_altee_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_altee_rsp    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_repees_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_repees_rsp   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_geteea_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_geteea_rsp   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_evnot_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_ackevnot_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_ackevnot_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_getas_req    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_getas_rsp    	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_getaes_req   	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_getaes_rsp   	(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_jwrite_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jwrite_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jread_req  		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jread_rsp  		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jinit_req  		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jinit_rsp  		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jstat_req  		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jstat_rsp  		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jcreate_req  	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_jdelete_req  	(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_obtain_file_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_obtain_file_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_open_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_open_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_read_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_read_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_close_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_close_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_rename_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_rename_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_delete_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_delete_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_dir_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_file_dir_rsp 	(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_conclude_req 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_conclude_rsp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_cancel_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_cancel_rsp 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_init_req 		(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_init_rsp 		(ASN1_DEC_CTXT *aCtx);

ST_VOID mms_rsp_not_supp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID mms_req_not_supp 	(ASN1_DEC_CTXT *aCtx);
ST_VOID _mms_null_pdu_dec 	(ASN1_DEC_CTXT *aCtx);

#ifdef __cplusplus
}
#endif

#endif   /* MMS_DFUN_INCLUDED */
