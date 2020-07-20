/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2005, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmslvar.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	All global MMSEASE-LITE variables are defined in this module, 	*/
/*	and declared in LMMSDEFS.H					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del globals _mmsdec_rslt, _mms_dec_info, &	*/
/*			   _mms_dec_info_pres.				*/
/* 07/21/09  MDE     16    Added MID log masks				*/
/* 03/17/08  JRB     15    Del m_use_long_ints flag.			*/
/* 10/04/07  MDE     14    Tweaked LOGCFG_VALUE_GROUP/LOGCFGX_VALUE_MAP	*/
/* 06/22/05  JRB     13    Remove static from usrLog* var.		*/
/* 06/06/05  EJV     12    usrLogMaskMapCtrl not used when FOUNDRY def.	*/
/* 05/23/05  EJV     11    Add xxxLogMaskMapCtrl for parsing logcfg.xml	*/
/* 09/04/03  EJV     10    Chg mms_debug_sel to ST_UINT	(from ST_UINT32)*/
/* 03/13/03  JRB     09    Del u_ml_get_rt_type func ptr, it is func now*/
/* 01/02/03  JRB     08    Add arg to u_ml_get_rt_type.			*/
/* 03/15/01  JRB     07    Added user_debug_sel & _user_*_logstr	*/ 
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 02/23/99  JRB     05    Don't init m_data_algn_tbl, it's a constant.	*/
/* 10/21/98  MDE     04    Removed VMS ifdef				*/
/* 10/08/98  MDE     03    Migrated to updated SLOG interface		*/
/* 11/05/97  MDE     02    Changed mmsl_msg_start to ST_UCHAR *		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_err.h"
#include "mms_pvar.h"
#include "mms_vvar.h"


/************************************************************************/
/************************************************************************/
/* MMSEASE-LITE Global Variables  					*/
/************************************************************************/

ST_INT   mmsl_enc_buf_size;
ST_UCHAR *mmsl_enc_buf;
ST_INT   mmsl_dec_info_size;

#if !defined(NO_GLB_VAR_INIT)
ST_INT16 mmsl_version = 1; 	/* select IS for version		*/
ST_UINT32 mmsl_invoke_id = 1;
#else
ST_INT16 mmsl_version;		/* select IS for version		*/
ST_UINT32 mmsl_invoke_id;
#endif

ST_UCHAR *mmsl_msg_start;
ST_INT mmsl_msg_len;

/************************************************************************/
/* Variables in common with MMS-EASE					*/

#if !defined(NO_GLB_VAR_INIT)
ST_UINT  _mmsdec_ctxt = MMS_PCI;/* decode context type, default to core	*/
#else
ST_UINT  _mmsdec_ctxt;		/* decode context type, default to core	*/
#endif
ST_INT       _mmsdec_msglen; 	/* MMS decode message length		*/
ST_UCHAR    *_mmsdec_msgptr; 	/* MMS decode message ptr		*/

ADTNL_ERR_RESP_INFO  adtnl_err_info;

ST_RET mms_op_err;


/************************************************************************/
/************************************************************************/
/************************************************************************/
/* Some Alternate Access related variables. These are defined in 	*/
/* mmsvar.c for MMS-EASE, but are put here for MMS-LITE			*/

#if !defined(NO_GLB_VAR_INIT)
ST_INT m_max_dec_aa = 50;
ST_INT m_max_rt_aa_ctrl = 1000;
#else
ST_INT m_max_dec_aa;
ST_INT m_max_rt_aa_ctrl;
#endif

ST_INT m_hw_dec_aa;
ST_INT m_hw_rt_aa_ctrl;
ST_BOOLEAN m_alt_acc_packed;

/************************************************************************/
/************************************************************************/
/* This variable can be set to force use of the DIS MMS floating point 	*/
/* type protocol (used in any over the wire and internal ASN.1 type 	*/
/* definitions).							*/
/************************************************************************/

ST_BOOLEAN m_use_dis_float;

/************************************************************************/
/* Global variables for modifier support				*/
/************************************************************************/

#if !defined (NO_GLB_VAR_INIT)
LIST_OF_MODS modifier_list; 	/* used to encode modifier info	*/
ST_INT	m_max_mods = 1;	/* num of modifier structs calloced during dec.	*/
#else
LIST_OF_MODS modifier_list;
ST_INT	m_max_mods;
#endif

/************************************************************************/
/* Global variables for compantion standard (IS)			*/
/************************************************************************/

CSI cs_send;		/* SEND CS info structure		*/
ST_BOOLEAN cs_send_reset_val;

/************************************************************************/
/* Logging variables							*/
/************************************************************************/

#if !defined(NO_GLB_VAR_INIT)
ST_UINT mms_debug_sel = MMS_LOG_ERR;
ST_UINT user_debug_sel = USER_LOG_ERR;
#else
ST_UINT mms_debug_sel;
ST_UINT user_debug_sel;
#endif

#ifdef DEBUG_SISCO
SD_CONST ST_CHAR *SD_CONST _mms_log_dec_logstr = "MMS_LOG_DEC";
SD_CONST ST_CHAR *SD_CONST _mms_log_enc_logstr = "MMS_LOG_ENC";
SD_CONST ST_CHAR *SD_CONST _mms_log_acse_logstr = "MMS_LOG_ACSE";
SD_CONST ST_CHAR *SD_CONST _mms_log_llc_logstr = "MMS_LOG_LLC";
SD_CONST ST_CHAR *SD_CONST _mms_log_ique_logstr = "MMS_LOG_IQUE";
SD_CONST ST_CHAR *SD_CONST _mms_log_rque_logstr = "MMS_LOG_RQUE";
SD_CONST ST_CHAR *SD_CONST _mms_log_ind_logstr = "MMS_LOG_IND";
SD_CONST ST_CHAR *SD_CONST _mms_log_conf_logstr = "MMS_LOG_CONF";
SD_CONST ST_CHAR *SD_CONST _mms_log_vm_logstr = "MMS_LOG_VM";
SD_CONST ST_CHAR *SD_CONST _mms_log_err_logstr = "MMS_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _mms_log_nerr_logstr = "MMS_LOG_NERR";
SD_CONST ST_CHAR *SD_CONST _mms_log_pdu_logstr = "MMS_LOG_PDU";
SD_CONST ST_CHAR *SD_CONST _mms_log_config_logstr = "MMS_LOG_CONFIG";
SD_CONST ST_CHAR *SD_CONST _mms_log_always_logstr = "MMS_LOG_ALWAYS";
SD_CONST ST_CHAR *SD_CONST _mms_log_midd_logstr = "MMS_LOG_MIDD";
SD_CONST ST_CHAR *SD_CONST _mms_log_mide_logstr = "MMS_LOG_MIDE";

SD_CONST ST_CHAR *SD_CONST _user_err_logstr = "USER_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _user_client_logstr = "USER_LOG_CLIENT";
SD_CONST ST_CHAR *SD_CONST _user_server_logstr = "USER_LOG_SERVER";

LOGCFGX_VALUE_MAP mmsLogMaskMaps[] =
  {
    {"MMS_LOG_ERR",	MMS_LOG_ERR,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"MMS_LOG_NERR",	MMS_LOG_NERR,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"MMS_LOG_DEC",	MMS_LOG_DEC,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Decode"},
    {"MMS_LOG_ENC",	MMS_LOG_ENC,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Encode"},
    {"MMS_LOG_MIDD",	MMS_LOG_MIDD,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Data Decode"},
    {"MMS_LOG_MIDE",	MMS_LOG_MIDE,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Data Encode"},

    {"MMS_LOG_CLIENT",	MMS_LOG_REQ|MMS_LOG_CONF, &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Client"},
    {"MMS_LOG_SERVER",	MMS_LOG_IND|MMS_LOG_RESP, &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Server"},

    {"MMS_LOG_REQ",	MMS_LOG_REQ,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Request"},
    {"MMS_LOG_RESP",	MMS_LOG_RESP,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Response"},
    {"MMS_LOG_IND",	MMS_LOG_IND,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Indication"},
    {"MMS_LOG_CONF",	MMS_LOG_CONF,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Confirm"},
    {"MMS_LOG_RT",	MMS_LOG_RT,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "RuntimeType"},
    {"MMS_LOG_RTAA",	MMS_LOG_RTAA,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "RuntimeAlternateAccess"},
    {"MMS_LOG_AA",	MMS_LOG_AA,	  &mms_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "AlternateAccess"},
  };

LOGCFG_VALUE_GROUP mmsLogMaskMapCtrl =
  {
  {NULL,NULL},
  "MmsLogMasks",      /* Parent Tag */
  sizeof(mmsLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  mmsLogMaskMaps
  };

LOGCFGX_VALUE_MAP usrLogMaskMaps[] =
  {
    {"USER_LOG_ERR",	USER_LOG_ERR,	 &user_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"USER_LOG_CLIENT",	USER_LOG_CLIENT, &user_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Client"},
    {"USER_LOG_SERVER",	USER_LOG_SERVER, &user_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Server"}
  };

LOGCFG_VALUE_GROUP usrLogMaskMapCtrl =
  {
  {NULL,NULL},
  "UserLogMasks",	     /* Parent Tag */
  sizeof(usrLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  usrLogMaskMaps
  };
#endif /* DEBUG_SISCO */

/************************************************************************/
/*			      m_init_glb_vars				*/
/************************************************************************/

#if defined(NO_GLB_VAR_INIT)
ST_VOID a_init_glb_vars (ST_VOID);

ST_VOID m_init_glb_vars (ST_VOID)
  {
  a_init_glb_vars ();
  mmsl_version = 1;
  mmsl_invoke_id = 1;
  _mmsdec_ctxt = MMS_PCI;
  m_max_mods = 1;
  mms_debug_sel = MMS_LOG_ERR;
#if defined(NO_GLB_VAR_INIT)
  if (!m_data_algn_tbl)
    m_data_algn_tbl = m_packed_data_algn_tbl;
#endif
   m_max_dec_aa = 50;
   m_max_rt_aa_ctrl = 1000;
  }

#endif

