/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*                  1986 - 2008 All Rights Reserved                     */
/*                                                                      */
/* MODULE NAME : mloguser.h                                             */
/* PRODUCT(S)  : MMS-EASE 250-001                                       */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      ANSI Protytped Logging Functions for MMS-EASE user code.        */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 03/03/10  JRB	   Replace "ml_log_*" with "mlogl_info" struct.	*/
/*			   Del obsolete ml_mlog_install function.	*/
/* 01/20/10  JRB     17    Chg invokeId to ST_ULONG in "ml_log_*".	*/
/* 03/01/08  JRB     16    Del commented code.				*/
/* 10/30/06  JRB     15    Add mlog_arb_log_ctrl extern.		*/
/* 03/23/05  JRB     14    Del unused m_log_mllp* & m_log_llp* protos.	*/
/* 06/10/04  EJV     13    Removed suicacse.h - lber.h conflict on AIX.	*/
/* 04/05/02  MDE     12    Cleaned up MMS Lite use of MLOG		*/
/* 11/04/98  DSF     11    Added m_get_mms_rej_text ()			*/
/* 08/13/98  MDE     10    Added m_log_tdl				*/
/* 07/16/98  MDE     09    Added ml_log_dec_rslt declaration		*/
/* 06/15/98  MDE     08    Changes to allow compile under C++		*/
/* 03/20/98  JRB     07    Del mmsop_en.h. Only user code needs it now.	*/
/* 09/29/97  MDE     06    Don't need mlog_en.h				*/
/* 08/15/97  MDE     05    BTOD handling changes			*/
/* 08/14/97  MDE     04    Removed unused 'm_log_info_print'		*/
/* 08/04/97  MDE     03    Added 'no log' function declarations		*/
/* 08/04/97  MDE     02    Changes towards breakup			*/
/* 06/09/97  MDE     01    Removed several mmv_v???.h includes		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MLOGUSER_INCLUDED
#define MLOGUSER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mmslog.h"
#include "mms_pcon.h"
#include "mms_pdom.h"
#include "mms_pevn.h"
#include "mms_pfil.h"
#include "mms_pjou.h"
#include "mms_pocs.h"
#include "mms_pprg.h"
#include "mms_psem.h"
#include "mms_pvar.h"
#include "mms_pvmd.h"
#include "mms_vvar.h"
#if defined(MMS_LITE)
#include "mmsintr2.h"
#endif
#include "sx_arb.h"	/* needed for SXD_ARB_DATA_CTRL	*/


/************************************************************************/
/************************************************************************/

extern ST_VOID (*m_req_log_fun_tbl [MAX_MMSOP_DIS+1]) (ST_VOID *v);
extern ST_VOID (*m_resp_log_fun_tbl [MAX_MMSOP_DIS+1]) (ST_VOID *v);

/* Request / Response Data Logging Functions				*/
ST_VOID m_log_ackevnot_req (ACKEVNOT_REQ_INFO *info);
ST_VOID m_log_altecm_req (ALTECM_REQ_INFO *info);
ST_VOID m_log_altee_resp (ALTEE_RESP_INFO *info);
ST_VOID m_log_altee_req (ALTEE_REQ_INFO *info);
ST_VOID m_log_crepi_req (CREPI_REQ_INFO *info);
ST_VOID m_log_cancel_req (ST_VOID);
ST_VOID m_log_cancel_resp (ST_VOID);
ST_VOID m_log_defea_req (DEFEA_REQ_INFO *info);
ST_VOID m_log_defec_req (DEFEC_REQ_INFO *info);
ST_VOID m_log_defee_req (DEFEE_REQ_INFO *info);
ST_VOID m_log_defscat_req (DEFSCAT_REQ_INFO *info);
ST_VOID m_log_defsem_req (DEFSEM_REQ_INFO *info);
ST_VOID m_log_deftype_req (DEFTYPE_REQ_INFO *info);
ST_VOID m_log_defvar_req (DEFVAR_REQ_INFO *info);
ST_VOID m_log_defvlist_req (DEFVLIST_REQ_INFO *info);
ST_VOID m_log_deldom_req (DELDOM_REQ_INFO *info);
ST_VOID m_log_delea_resp (DELEA_RESP_INFO *info);
ST_VOID m_log_delea_req (DELEA_REQ_INFO *info);
ST_VOID m_log_delec_req (DELEC_REQ_INFO *info);
ST_VOID m_log_delec_resp (DELEC_RESP_INFO *info);
ST_VOID m_log_delee_resp (DELEE_RESP_INFO *info);
ST_VOID m_log_delee_req (DELEE_REQ_INFO *info);
ST_VOID m_log_delpi_req (DELPI_REQ_INFO *info);
ST_VOID m_log_delsem_req (DELSEM_REQ_INFO *info);
ST_VOID m_log_deltype_resp (DELTYPE_RESP_INFO *info);
ST_VOID m_log_deltype_req (DELTYPE_REQ_INFO *info);
ST_VOID m_log_delvar_resp (DELVAR_RESP_INFO *info);
ST_VOID m_log_delvar_req (DELVAR_REQ_INFO *info);
ST_VOID m_log_delvlist_resp (DELVLIST_RESP_INFO *info);
ST_VOID m_log_delvlist_req (DELVLIST_REQ_INFO *info);
ST_VOID m_log_download_resp (DOWNLOAD_RESP_INFO *info);
ST_VOID m_log_download_req (DOWNLOAD_REQ_INFO *info);
ST_VOID m_log_evnot_req (EVNOT_REQ_INFO *info);
ST_VOID m_log_fclose_req (FCLOSE_REQ_INFO *info);
ST_VOID m_log_fdelete_req (FDELETE_REQ_INFO *info);
ST_VOID m_log_fdir_resp (FDIR_RESP_INFO *info);
ST_VOID m_log_fdir_req (FDIR_REQ_INFO *info);
ST_VOID m_log_fopen_resp (FOPEN_RESP_INFO *info);
ST_VOID m_log_fopen_req (FOPEN_REQ_INFO *info);
ST_VOID m_log_fread_resp (FREAD_RESP_INFO *info);
ST_VOID m_log_fread_req (FREAD_REQ_INFO *info);
ST_VOID m_log_frename_req (FRENAME_REQ_INFO *info);
ST_VOID m_log_getaes_resp (GETAES_RESP_INFO *info);
ST_VOID m_log_getaes_req (GETAES_REQ_INFO *info);
ST_VOID m_log_getas_resp (GETAS_RESP_INFO *info);
ST_VOID m_log_getas_req (GETAS_REQ_INFO *info);
ST_VOID m_log_getcl_resp (GETCL_RESP_INFO *info);
ST_VOID m_log_getcl_req (GETCL_REQ_INFO *info);
ST_VOID m_log_getdom_resp (GETDOM_RESP_INFO *info);
ST_VOID m_log_getdom_req (GETDOM_REQ_INFO *info);
ST_VOID m_log_geteaa_resp (GETEAA_RESP_INFO *info);
ST_VOID m_log_geteaa_req (GETEAA_REQ_INFO *info);
ST_VOID m_log_geteca_resp (GETECA_RESP_INFO *info);
ST_VOID m_log_geteca_req (GETECA_REQ_INFO *info);
ST_VOID m_log_geteea_resp (GETEEA_RESP_INFO *info);
ST_VOID m_log_geteea_req (GETEEA_REQ_INFO *info);
ST_VOID m_log_getpi_resp (GETPI_RESP_INFO *info);
ST_VOID m_log_getpi_req (GETPI_REQ_INFO *info);
ST_VOID m_log_getscat_resp (GETSCAT_RESP_INFO *info);
ST_VOID m_log_getscat_req (GETSCAT_REQ_INFO *info);
ST_VOID m_log_gettype_resp (GETTYPE_RESP_INFO *info);
ST_VOID m_log_gettype_req (GETTYPE_REQ_INFO *info);
ST_VOID m_log_getvar_resp (GETVAR_RESP_INFO *info);
ST_VOID m_log_getvar_req (GETVAR_REQ_INFO *info);
ST_VOID m_log_getvlist_resp (GETVLIST_RESP_INFO *info);
ST_VOID m_log_getvlist_req (GETVLIST_REQ_INFO *info);
ST_VOID m_log_ident_resp (IDENT_RESP_INFO *info);
ST_VOID m_log_info_req (INFO_REQ_INFO *info);
ST_VOID m_log_initdown_req (INITDOWN_REQ_INFO *info);
ST_VOID m_log_initupl_resp (INITUPL_RESP_INFO *info);
ST_VOID m_log_initupl_req (INITUPL_REQ_INFO *info);
ST_VOID m_log_init_info (INIT_INFO *info);
ST_VOID m_log_input_resp (INPUT_RESP_INFO *info);
ST_VOID m_log_input_req (INPUT_REQ_INFO *info);
ST_VOID m_log_jcreate_req (JCREATE_REQ_INFO *info);
ST_VOID m_log_jdelete_req (JDELETE_REQ_INFO *info);
ST_VOID m_log_jinit_resp (JINIT_RESP_INFO *info);
ST_VOID m_log_jinit_req (JINIT_REQ_INFO *info);
ST_VOID m_log_jread_resp (JREAD_RESP_INFO *info);
ST_VOID m_log_jread_req (JREAD_REQ_INFO *info);
ST_VOID m_log_jstat_resp (JSTAT_RESP_INFO *info);
ST_VOID m_log_jstat_req (JSTAT_REQ_INFO *info);
ST_VOID m_log_jwrite_req (JWRITE_REQ_INFO *info);
ST_VOID m_log_kill_req (KILL_REQ_INFO *info);
ST_VOID m_log_loaddom_req (LOADDOM_REQ_INFO *info);
ST_VOID m_log_namelist_resp (NAMELIST_RESP_INFO *info);
ST_VOID m_log_namelist_req (NAMELIST_REQ_INFO *info);
ST_VOID m_log_obtfile_req (OBTFILE_REQ_INFO *info);
ST_VOID m_log_output_req (OUTPUT_REQ_INFO *info);
ST_VOID m_log_rddwn_req (RDDWN_REQ_INFO *info);
ST_VOID m_log_rdupl_req (RDUPL_REQ_INFO *info);
ST_VOID m_log_read_resp (READ_RESP_INFO *info);
ST_VOID m_log_read_req (READ_REQ_INFO *info);
ST_VOID m_log_relctrl_req (RELCTRL_REQ_INFO *info);
ST_VOID m_log_rename_req (RENAME_REQ_INFO *info);
ST_VOID m_log_repeas_resp (REPEAS_RESP_INFO *info);
ST_VOID m_log_repeas_req (REPEAS_REQ_INFO *info);
ST_VOID m_log_repecs_resp (REPECS_RESP_INFO *info);
ST_VOID m_log_repecs_req (REPECS_REQ_INFO *info);
ST_VOID m_log_repees_resp (REPEES_RESP_INFO *info);
ST_VOID m_log_repees_req (REPEES_REQ_INFO *info);
ST_VOID m_log_reset_req (RESET_REQ_INFO *info);
ST_VOID m_log_resume_req (RESUME_REQ_INFO *info);
ST_VOID m_log_rsentry_resp (RSENTRY_RESP_INFO *info);
ST_VOID m_log_rsentry_req (RSENTRY_REQ_INFO *info);
ST_VOID m_log_rspool_resp (RSPOOL_RESP_INFO *info);
ST_VOID m_log_rspool_req (RSPOOL_REQ_INFO *info);
ST_VOID m_log_rsstat_resp (RSSTAT_RESP_INFO *info);
ST_VOID m_log_rsstat_req (RSSTAT_REQ_INFO *info);
ST_VOID m_log_start_req (START_REQ_INFO *info);
ST_VOID m_log_status_resp (STATUS_RESP_INFO *info);
ST_VOID m_log_status_req (STATUS_REQ_INFO *info);
ST_VOID m_log_stop_req (STOP_REQ_INFO *info);
ST_VOID m_log_storedom_req (STOREDOM_REQ_INFO *info);
ST_VOID m_log_takectrl_resp (TAKECTRL_RESP_INFO *info);
ST_VOID m_log_takectrl_req (TAKECTRL_REQ_INFO *info);
ST_VOID m_log_termdown_req (TERMDOWN_REQ_INFO *info);
ST_VOID m_log_termupl_req (TERMUPL_REQ_INFO *info);
ST_VOID m_log_trige_req (TRIGE_REQ_INFO *info);
ST_VOID m_log_upload_resp (UPLOAD_RESP_INFO *info);
ST_VOID m_log_upload_req (UPLOAD_REQ_INFO *info);
ST_VOID m_log_ustatus_req (USTATUS_REQ_INFO *info);
ST_VOID m_log_write_resp (WRITE_RESP_INFO *info);
ST_VOID m_log_write_req (WRITE_REQ_INFO *info);

/* common component logging */
ST_VOID m_log_app_ref (APP_REF *info);
ST_VOID m_log_asn1_app_ref (ST_UCHAR *buf_ptr, ST_INT buf_len);
ST_VOID m_log_modifier (MODIFIER *info);
ST_VOID m_log_tdl (ST_UCHAR *asn1, ST_INT asn1_len);

ST_VOID m_log_error_info (ERR_INFO *err_ptr);
#if !defined(MMS_LITE)
ST_VOID m_log_err_info (MMSREQ_PEND *req_ptr);
#endif

ST_VOID m_no_log_resp (ST_VOID *resp);
ST_VOID m_no_log_req  (ST_VOID *req);

/************************************************************************/

ST_VOID m_get_mms_err_text (ST_INT eclass, ST_INT code, 
	ST_CHAR *dest, ST_INT dest_len);

ST_VOID m_get_mms_rej_text (ST_INT rclass, ST_INT code, 
	ST_CHAR *dest, ST_INT dest_len);

ST_VOID m_log_reject_info (REJECT_RESP_INFO *rej_ptr);

ST_VOID m_log_evtime (EVENT_TIME *info);
ST_VOID m_log_evt_tod (MMS_BTOD *info);
ST_VOID m_log_external (EXTERN_INFO *info);

ST_VOID m_log_objname (OBJECT_NAME *info);
ST_VOID m_log_pi_state (ST_INT state);
ST_VOID m_log_address (VAR_ACC_ADDR *info);
ST_VOID m_log_var_list (VARIABLE_LIST *vl_ptr, ST_INT num_of_vars);
ST_VOID m_log_var_spec (VARIABLE_SPEC *info);
ST_VOID m_log_vaspec (VAR_ACC_SPEC *info);
ST_VOID m_log_data (ST_INT asn1_data_len, ST_UCHAR *asn1_data);

/************************************************************************/
/************************************************************************/
#if defined(MMS_LITE)
/************************************************************************/

ST_RET ml_log_dec_rslt (MMSDEC_INFO *rslt);

ST_VOID _ml_log_req_info (ST_ULONG invokeId, ST_INT op, ST_VOID *info);
ST_VOID _ml_log_ind_info (ST_ULONG invokeId, ST_INT op, ST_VOID *info);
ST_VOID _ml_log_resp_info (ST_ULONG invokeId, ST_INT op, ST_VOID *info);
ST_VOID _ml_log_conf_info (ST_ULONG invokeId, ST_INT op, ST_VOID *info);
ST_VOID _ml_log_unsol_req_info (ST_INT op, ST_VOID *info);
ST_VOID _ml_log_unsol_ind_info (ST_INT op, ST_VOID *info);
ST_VOID _ml_log_error_resp (ST_ULONG invokeId, ST_VOID *info);
ST_VOID _ml_log_error_conf (ST_ULONG invokeId, ST_VOID *info);
ST_VOID _ml_log_reject_send (ST_VOID *info);
ST_VOID _ml_log_reject_recv (ST_VOID *info);


/* MLOG integration */
typedef struct
  {
  /* Function pointers set by "mmsop_en.c" if MLOG_ENABLE defined.	*/
  ST_VOID (*req_info_fun)  (ST_ULONG invokeId, ST_INT op, ST_VOID *info);
  ST_VOID (*ind_info_fun)  (ST_ULONG invokeId, ST_INT op, ST_VOID *info);

  ST_VOID (*resp_info_fun) (ST_ULONG invokeId, ST_INT op, ST_VOID *info);
  ST_VOID (*conf_info_fun) (ST_ULONG invokeId, ST_INT op, ST_VOID *info);

  ST_VOID (*unsol_req_info_fun) (ST_INT op, ST_VOID *info);
  ST_VOID (*unsol_ind_info_fun) (ST_INT op, ST_VOID *info);

  ST_VOID (*error_conf_fun) (ST_ULONG invokeId, ST_VOID *info);
  ST_VOID (*error_resp_fun) (ST_ULONG invokeId, ST_VOID *info);

  ST_VOID (*reject_send_fun) (ST_VOID *info);
  ST_VOID (*reject_recv_fun) (ST_VOID *info);
  } MLOGL_INFO;

extern MLOGL_INFO mlogl_info;	/* global MLOGL control structure	*/
				/* initialized at compile time		*/

/* OBSOLETE ml_mlog_install function replaced with this macro (does nothing).*/
#define ml_mlog_install()	/* for backward compatibility only	*/

/************************************************************************/
#endif /* MMS_LITE */
/************************************************************************/

/************************************************************************/
#if !defined(MMS_LITE)
/************************************************************************/
ST_VOID m_log_ureq_info (ST_INT chan, ST_INT op, ST_VOID *info);
ST_VOID m_log_req_info  (MMSREQ_PEND *req, ST_VOID *info);
ST_VOID m_log_resp_info (MMSREQ_IND *ind, ST_VOID *info);
ST_VOID m_log_ind_info  (MMSREQ_IND *ind);
ST_VOID m_log_conf_info (MMSREQ_PEND *conf);
ST_VOID m_log_error_resp (MMSREQ_IND *ind, ST_INT16 eclass, ST_INT16 code);
ST_RET  m_mlog_install (ST_VOID);
#endif

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
extern SXD_ARB_DATA_CTRL mlog_arb_log_ctrl;

/************************************************************************/
#ifdef __cplusplus
}
#endif
/************************************************************************/


#endif  /* MLOGUSER_INCLUDED */

