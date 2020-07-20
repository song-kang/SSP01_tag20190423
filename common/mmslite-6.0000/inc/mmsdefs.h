/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2004, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsdefs.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This include file is for MMS-EASE or SMALL-MMS programmer use,	*/
/*	and calls out all needed include files.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED :						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 05/09/07  JRB     14    Add forward references.			*/
/* 04/09/07  MDE     13    Enhanced filtered logging 			*/
/* 09/17/04  EJV     12    Added mms_chk_resource.			*/
/* 10/13/03  EJV     11    Moved MMSEASE_VERSION define to mms_defs.h.	*/
/* 10/07/03  EJV     10    Added MMSEASE_VERSION define.		*/
/* 12/12/02  JRB     09    Add ASN1_ENC_CTXT arg to _mms_unconf_req_fin	*/ 
/* 01/18/02  JRB     08    Replace _ms_fin_pdu_debug with _ms_fin_pdu	*/
/*			   and _ms_fin_pdu_log.				*/
/* 12/20/01  JRB     07	   Convert to use ASN1R.			*/
/* 09/26/00  JRB     06    Add SD_CONST on ms_mk_rt_aa*			*/
/* 06/01/00  RKR     05    Removed prot mms_null_pdu_dec use mms_dfun.h	*/
/* 03/23/99  MDE     04    Changes to decode buffer allocation scheme   */
/* 06/15/98  MDE     03    More changes to allow compile under C++	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 03/20/98  JRB     01    Del mmsop_en.h. Only user code needs it now.	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMSDEFS_INCLUDED
#define MMSDEFS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "glberror.h"

#ifndef MMS_LITE
#include "gen_list.h"
#include "mem_chk.h"
#include "mms_defs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mmsintrn.h"
#include "mms_mp.h"
#include "mms_mv.h"
#include "mms_err.h"
#else
#include "mem_chk.h"
#include "mms_def2.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mmsintr2.h"
#include "mms_mp.h"
#include "mmsldefs.h"
#endif

#include "mms_dfun.h"
#include "mms_perr.h"
#include "mms_log.h"
#include "mmslog.h"

#include "mms_pcon.h"
#include "mms_pdom.h"
#include "mms_pevn.h"
#include "mms_pfil.h"
#include "mms_pjou.h"
#include "mms_pocs.h"
#include "mms_pprg.h"
#include "mms_psem.h"
#include "mms_pvmd.h"

/************************************************************************/
/* For use when global variables cannot be initialized at compile time	*/

#if defined (NO_GLB_VAR_INIT)
ST_VOID m_init_glb_vars (ST_VOID);
#endif

/************************************************************************/
ST_VOID _ms_wr_service_err (ASN1_ENC_CTXT *ac, ST_INT16 eclass, ST_INT16 code, 
			ADTNL_ERR_RESP_INFO *adtnl_info);

/************************************************************************/
/* Variable Access Functions.						*/
/************************************************************************/

ST_VOID  _mms_get_service_err (ASN1_DEC_CTXT *ac, ERR_INFO  *dest);
ST_VOID  _ms_get_mms_fname (ASN1_DEC_CTXT *ac, FILE_NAME *dest, ST_INT max_fnames, ST_INT *num_out);
ST_VOID  _ms_get_mms_app_ref (ASN1_DEC_CTXT *ac, ST_UCHAR **dest, ST_INT *dest_size, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_mms_evtime	(ASN1_DEC_CTXT *ac, EVENT_TIME *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_mms_modlist (ASN1_DEC_CTXT *ac, MODIFIER *dest, ST_INT *num_out, ST_INT max_mods, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_mms_objname (ASN1_DEC_CTXT *ac, OBJECT_NAME *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_jou_ent_cont (ASN1_DEC_CTXT *ac, ENTRY_CONTENT *dest, ST_INT dest_size, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_address	(ASN1_DEC_CTXT *ac, VAR_ACC_ADDR *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_access_rslt (ASN1_DEC_CTXT *ac, ACCESS_RESULT *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_data (ASN1_DEC_CTXT *ac, VAR_ACC_DATA *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_type_spec (ASN1_DEC_CTXT *ac, VAR_ACC_TSPEC *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_scat_access (ASN1_DEC_CTXT *ac, SCATTERED_ACCESS *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_alt_access (ASN1_DEC_CTXT *ac, ALTERNATE_ACCESS *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_var_spec (ASN1_DEC_CTXT *ac, VARIABLE_SPEC *dest, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_var_list (ASN1_DEC_CTXT *ac, VARIABLE_LIST *dest, ST_INT *num_out, ST_INT max_num, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID  _ms_get_va_spec (ASN1_DEC_CTXT *ac, VAR_ACC_SPEC *dest, ST_INT max_vars, ST_VOID (*done)(ASN1_DEC_CTXT *ac));

ST_VOID  _ms_wr_mms_fname      (ASN1_ENC_CTXT *ac, FILE_NAME *src, ST_INT num);
ST_VOID  _ms_wr_mms_evtime     (ASN1_ENC_CTXT *ac, EVENT_TIME *src);
ST_VOID  _ms_wr_mms_modlist    (ASN1_ENC_CTXT *ac, MODIFIER *src, ST_INT num);
ST_VOID  _ms_wr_mms_objname    (ASN1_ENC_CTXT *ac, OBJECT_NAME *src);
ST_VOID  _ms_wr_jou_ent_cont   (ASN1_ENC_CTXT *ac, ENTRY_CONTENT *src);
ST_VOID  _ms_wr_va_address     (ASN1_ENC_CTXT *ac, VAR_ACC_ADDR *src);
ST_VOID  _ms_wr_va_access_rslt (ASN1_ENC_CTXT *ac, ACCESS_RESULT *src);
ST_VOID  _ms_wr_va_data        (ASN1_ENC_CTXT *ac, VAR_ACC_DATA *src);
ST_VOID  _ms_wr_va_type_spec   (ASN1_ENC_CTXT *ac, VAR_ACC_TSPEC *src);
ST_VOID  _ms_wr_va_scat_access (ASN1_ENC_CTXT *ac, SCATTERED_ACCESS *src);
ST_VOID  _ms_wr_va_alt_access  (ASN1_ENC_CTXT *ac, ALTERNATE_ACCESS *src);
ST_VOID  _ms_wr_va_var_spec    (ASN1_ENC_CTXT *ac, VARIABLE_SPEC *src);
ST_VOID  _ms_wr_va_var_list    (ASN1_ENC_CTXT *ac, VARIABLE_LIST *src, ST_INT num);
ST_VOID  _ms_wr_va_spec        (ASN1_ENC_CTXT *ac, VAR_ACC_SPEC *src);

ST_VOID _mms_init_dec (ASN1_DEC_CTXT *ac, ST_BOOLEAN req);

/************************************************************************/
/* Alternate Access Functions						*/
/************************************************************************/
ST_VOID _ms_log_runtime_aa (RT_AA_CTRL *rt_aa, ST_INT num_aa);

ST_RET _ms_mk_rt_aa_all (SD_CONST RUNTIME_TYPE *rt_head,  ST_INT rt_num, 
                       RT_AA_CTRL **rt_aa_out, ST_INT *num_aa_out);

ST_RET _ms_mk_rt_aa (SD_CONST RUNTIME_TYPE *rt,  ST_INT rt_num, 
                   ALT_ACCESS *alt_acc,
                   RT_AA_CTRL **rt_aa_out, ST_INT *num_aa_out);

ST_INT _ms_get_rtaa_data_size (RT_AA_CTRL *runtime_aa, ST_INT num_rt_aa);

/************************************************************************/
/* Special ASN.1 buffer handling functions for VM operations where	*/
/* a series of ASN.1 encoded AA must be created.			*/

ST_UCHAR *_m_get_aa_asn1_buf (ST_INT *size_out);
ST_VOID _m_free_aa_asn1_buf (ST_VOID);
ST_VOID _m_set_aa_asn1_buf_end (ST_UCHAR *new_end);

/************************************************************************/
/************************************************************************/
/* General function declarations used by the mp_xxx series.		*/
/* These are not of general use to the application programmer.		*/
/************************************************************************/

ST_VOID _ms_chk_for_extern (ASN1_DEC_CTXT *ac, ST_INT *len_ptr, ST_UCHAR **buf_ptr, ST_VOID (*done)(ASN1_DEC_CTXT *ac));
ST_VOID _ms_set_cs_check (ASN1_DEC_CTXT *ac);
ST_VOID _ms_set_cs_check2 (ASN1_DEC_CTXT *ac);
ST_VOID _mms_dec_done_ok (ASN1_DEC_CTXT *ac);

ST_UCHAR *_ms_fin_pdu (ASN1_ENC_CTXT *aCtx, ST_INT type, ST_INT *pPduLen);
#ifdef DEBUG_SISCO
ST_VOID _ms_fin_pdu_log (ST_INT type, ST_CHAR *txt,
			ST_UCHAR *pduPtr, ST_INT pduLen);
#endif

ST_VOID  _mms_dec_buf_free (ASN1_DEC_CTXT *ac, ST_RET err_code);
#ifndef MMS_LITE
ST_RET _mms_send_null_resp (MMSREQ_IND *indptr, ST_INT opcode);
MMSREQ_PEND *_mms_send_null_req (ST_INT chan, ST_INT opcode);

MMSREQ_PEND *_mms_req_send (ST_INT opcode, 
				ST_VOID (*mk_ptr)(ASN1_ENC_CTXT *, ST_CHAR *), 
				ST_INT chan,
				ST_CHAR *info_ptr);

ST_INT _mms_chk_resource (ST_INT chan);
ST_RET _mms_unconf_req_send (ST_INT opcode, 
				ST_VOID (*mk_ptr)(ASN1_ENC_CTXT *, ST_CHAR *), 
				ST_INT chan, 
				ST_CHAR *info_ptr);

ST_RET _mms_resp_send (ST_INT opcode, 
				MMSREQ_IND *indptr, 
				ST_VOID (*mk_fun)(ASN1_ENC_CTXT *, ST_CHAR *), 
				ST_CHAR *info_ptr);

#else
ST_RET _mms_fin_null_resp (ST_UINT32 invoke_id, ST_INT opcode);
ST_RET _mms_fin_null_req  (ST_INT opcode);

ST_RET _mms_resp_fin       (ST_INT opcode, ST_UINT32 invoke_id,
   	 	      	    ST_VOID (*mk_fun)(ASN1_ENC_CTXT *, ST_CHAR *), 
		      	    ST_CHAR *info_ptr);

ST_RET _mms_req_fin	   (ST_INT opcode, 
		      	    ST_VOID (*mk_ptr)(ASN1_ENC_CTXT *, ST_CHAR *), 
		      	    ST_CHAR *info_ptr);

ST_RET _mms_unconf_req_fin (ASN1_ENC_CTXT *aCtx, ST_INT opcode, 
			    ST_VOID (*mk_ptr)(ASN1_ENC_CTXT *, ST_CHAR *), 
			    ST_CHAR *info_ptr);

struct mvl_req_pend;	/* forward reference	*/
struct mvl_ind_pend;	/* forward reference	*/
struct mvl_net_info;	/* forward reference	*/

ST_VOID mpl_req_log (struct mvl_req_pend *reqCtrl, ST_VOID *reqInfo);
ST_VOID mpl_resp_log (struct mvl_ind_pend *indCtrl, ST_VOID *respInfo);
ST_VOID mpl_unsol_req_log (struct mvl_net_info *netInfo, ST_INT op, ST_VOID *reqInfo);

#endif

/* A casting aid ... */
#define M_CAST_MK_FUN(x)  ((ST_VOID(*)(ASN1_ENC_CTXT *, ST_CHAR *)) x)


#ifndef MMS_LITE
#include "mms_llp.h" 
#include "mmsllp.h"
#endif

#include "mmsefun.h"	/* Bruder moved this here	*/
#ifdef __cplusplus
}
#endif

#endif		/* MMSDEFS_INCLUDED */


