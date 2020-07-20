#ifdef DEBUG_SISCO
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 All Rights Reserved					*/
/*									*/
/* MODULE NAME : log_cfg.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	read_log_cfg_file						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/05/01  MDE    11     Allow to compile with Marben			*/
/* 03/09/00  MDE    10     Set mvl_debug_sel MVLLOG_ERR			*/
/* 01/04/00  JRB    09     Add ifdef around whole module.		*/
/* 04/01/99  RKR    08     Added ICCP_LITE				*/
/* 03/18/99  MDE    07     Added control for 'MemHwDump'		*/
/* 03/08/99  MDE    06     Added ifdef around ADLC header files		*/
/* 02/02/99  JRB    05     Add COSP and COPP for MOSI.			*/
/* 10/20/98  MDE    04     Spelling					*/
/* 10/15/98  MDE    03     Added new LogCtrl flags, MVLU_FLOW 		*/
/* 10/13/98  MDE    02     Removed unused MVL log flags			*/
/* 10/09/98  JRB    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#include "slog.h"
#include "asn1defs.h"
#include "mem_chk.h"
#include "cfg_util.h"
#include "mms_log.h"
#include "mvl_log.h"
#include "gen_list.h"
#include "acse2usr.h"

#if defined (ICCP_LITE) 
#include "mi_log.h"
#endif

#if (defined (LEAN_T) || defined (REDUCED_STACK))
#include "tp4api.h"
#include "clnp_usr.h"
#include "smp_usr.h"
#endif	/* (LEAN_T || REDUCED_STACK)	*/

#if (defined (ADLC_SLAVE) || defined (ADLC_MASTER))
#include "adlc_usr.h"
#include "adlc_log.h"		/* for "ADLC_SET_LOG_MASK" macro	*/
#endif

/* Control bit set/clear macros		*/

#define M_SET_CTRL(a)      {tmp_sLogCtrl.logCtrl |=  (a);}
#define M_CLR_CTRL(a)      {tmp_sLogCtrl.logCtrl &= ~(a);}

#define M_SET_FILE_CTRL(a) {tmp_sLogCtrl.fc.ctrl |=  (a);}
#define M_CLR_FILE_CTRL(a) {tmp_sLogCtrl.fc.ctrl &= ~(a);}

#define M_SET_MEM_CTRL(a)  {tmp_sLogCtrl.mc.ctrl |=  (a);}
#define M_CLR_MEM_CTRL(a)  {tmp_sLogCtrl.mc.ctrl &= ~(a);}

/*----- Internal functions prototypes for handling state tables -----*/

static ST_VOID  cfg_begin (ST_VOID);

static ST_VOID  cfg_lc_file_log_en (ST_VOID);
static ST_VOID  cfg_lc_mem_log_en (ST_VOID);

static ST_VOID  cfg_lc_log_file_size (ST_VOID);
static ST_VOID  cfg_lc_log_file_name (ST_VOID);

static ST_VOID  cfg_lc_log_mem_items (ST_VOID);
static ST_VOID  cfg_lc_dump_file_name (ST_VOID);
static ST_VOID  cfg_lc_mem_auto_dump (ST_VOID);
static ST_VOID  cfg_lc_mem_hw_dump (ST_VOID);

static ST_VOID  cfg_lc_time_enable (ST_VOID);
static ST_VOID  cfg_lc_hard_flush (ST_VOID);
static ST_VOID  cfg_lc_no_append (ST_VOID);
static ST_VOID  cfg_lc_no_msg_header (ST_VOID);
static ST_VOID  cfg_lc_no_wipe (ST_VOID);
static ST_VOID  cfg_lc_no_wrap (ST_VOID);
static ST_VOID  cfg_lc_setbuf (ST_VOID);
static ST_VOID  cfg_lc_no_header_cr (ST_VOID);
static ST_VOID  cfg_lc_suppress_filename (ST_VOID);
static ST_VOID  cfg_lc_suppress_logtype (ST_VOID);
static ST_VOID  cfg_lc_end (ST_VOID);

static ST_VOID  cfg_mem_log_calloc (ST_VOID);
static ST_VOID  cfg_mem_log_malloc (ST_VOID);
static ST_VOID  cfg_mem_log_realloc (ST_VOID);
static ST_VOID  cfg_mem_log_free (ST_VOID);

static ST_VOID  cfg_asn1_log_dec (ST_VOID);
static ST_VOID  cfg_asn1_log_enc (ST_VOID);
static ST_VOID  cfg_asn1_log_nerr (ST_VOID);

static ST_VOID  cfg_mms_log_dec (ST_VOID);
static ST_VOID  cfg_mms_log_enc (ST_VOID);
static ST_VOID  cfg_mms_log_nerr (ST_VOID);
static ST_VOID  cfg_mms_log_rt (ST_VOID);
static ST_VOID  cfg_mms_log_rtaa (ST_VOID);
static ST_VOID  cfg_mms_log_aa (ST_VOID);

static ST_VOID  cfg_milog_err (ST_VOID);
static ST_VOID  cfg_mi_log_nerr (ST_VOID);
static ST_VOID  cfg_milog_flow (ST_VOID);

static ST_VOID  cfg_mvllog_nerr (ST_VOID);
static ST_VOID  cfg_mvllog_acse (ST_VOID);
static ST_VOID  cfg_mvllog_acsedata (ST_VOID);
static ST_VOID  cfg_mvlulog_flow (ST_VOID);

static ST_VOID  cfg_acse_log_dec (ST_VOID);
static ST_VOID  cfg_acse_log_enc (ST_VOID);

static ST_VOID  cfg_copp_log_dec (ST_VOID);
static ST_VOID  cfg_copp_log_dec_hex (ST_VOID);
static ST_VOID  cfg_copp_log_enc (ST_VOID);
static ST_VOID  cfg_copp_log_enc_hex (ST_VOID);

static ST_VOID  cfg_cosp_log_dec (ST_VOID);
static ST_VOID  cfg_cosp_log_dec_hex (ST_VOID);
static ST_VOID  cfg_cosp_log_enc (ST_VOID);
static ST_VOID  cfg_cosp_log_enc_hex (ST_VOID);

static ST_VOID  cfg_tp4_log_flowdown (ST_VOID);
static ST_VOID  cfg_tp4_log_flowup (ST_VOID);

static ST_VOID  cfg_clnp_log_nerr (ST_VOID);
static ST_VOID  cfg_clnp_log_req (ST_VOID);
static ST_VOID  cfg_clnp_log_ind (ST_VOID);
static ST_VOID  cfg_clnp_log_enc_dec (ST_VOID);
static ST_VOID  cfg_clnp_log_llc_enc_dec (ST_VOID);
static ST_VOID  cfg_clsns_log_req (ST_VOID);
static ST_VOID  cfg_clsns_log_ind (ST_VOID);

static ST_VOID  cfg_adlc_log_nerr (ST_VOID);
static ST_VOID  cfg_adlc_log_flow (ST_VOID);
static ST_VOID  cfg_adlc_log_io (ST_VOID);
static ST_VOID  cfg_adlc_log_hexio (ST_VOID);
static ST_VOID  cfg_adlc_log_sm (ST_VOID);
static ST_VOID  cfg_adlc_log_hexsm (ST_VOID);
static ST_VOID  cfg_adlc_log_user(ST_VOID);
static ST_VOID  cfg_adlc_log_hexuser (ST_VOID);

static ST_VOID  cfg_smp_log_req (ST_VOID);
static ST_VOID  cfg_smp_log_ind (ST_VOID);

static ST_VOID  cfg_m_fill_en (ST_VOID);
static ST_VOID  cfg_m_heap_check_enable (ST_VOID);
static ST_VOID  cfg_m_check_list_enable (ST_VOID);
static ST_VOID  cfg_m_find_node_enable (ST_VOID);
static ST_VOID  cfg_m_track_prev_free (ST_VOID);
extern ST_BOOLEAN m_track_prev_free;	/* so secret it's not in any header file*/
static ST_VOID  cfg_list_debug_sel (ST_VOID);
static ST_VOID  cfg_m_no_realloc_smaller (ST_VOID);

static ST_VOID  cfg_end (ST_VOID);

/************************************************************************/
/* Buffers to save file names.						*/
/************************************************************************/
static ST_CHAR	log_file_name [256];
static ST_CHAR	memlog_file_name [256];
static LOG_CTRL tmp_sLogCtrl;

/************************************************************************/
/* Uncomment this define to compile in a bunch of printf statments	*/
/* that show up when a keyword is encountered				*/
/*#define S_TABLE_DEBUG*/

struct cfg_kw_el mlog_base_kwtbl[] =
  {
    {"%LogBegin",	cfg_begin,		NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el mlog_settings_kwtbl[] =
  {
    {"FileLogEn",	cfg_lc_file_log_en,	NULL },
    {"MemLogEn",	cfg_lc_mem_log_en,	NULL },

    /* Log File Attributes	*/
    {"LogFileSize",	cfg_lc_log_file_size,	NULL },
    {"LogFileName",	cfg_lc_log_file_name,	NULL },

    /* Memory Resident Logging Attributes	*/
    {"LogMemItems",	cfg_lc_log_mem_items,	NULL },
    {"DumpFileName",	cfg_lc_dump_file_name,	NULL },
    {"MemAutoDump",	cfg_lc_mem_auto_dump,	NULL },
    {"MemHwDump",	cfg_lc_mem_hw_dump,	NULL },

    /* Misc. Control Flags	*/
    {"TimeEnable",	cfg_lc_time_enable,	NULL },
    {"HardFlush",	cfg_lc_hard_flush,	NULL },
    {"NoAppend",	cfg_lc_no_append,	NULL },
    {"NoMsgheader",	cfg_lc_no_msg_header,	NULL },
    {"NoWipe",		cfg_lc_no_wipe,		NULL },
    {"NoWrap",		cfg_lc_no_wrap,		NULL },
    {"Setbuf",		cfg_lc_setbuf,		NULL },
    {"NoHeaderCr",	cfg_lc_no_header_cr,   	NULL },
    {"SuppressFilename",cfg_lc_suppress_filename,NULL },
    {"SuppressLogtype",	cfg_lc_suppress_logtype, NULL },

    /* Memory use logging	*/
    {"MEM_LOG_CALLOC",	cfg_mem_log_calloc,	NULL },
    {"MEM_LOG_MALLOC",	cfg_mem_log_malloc,	NULL },
    {"MEM_LOG_REALLOC",	cfg_mem_log_realloc,	NULL },
    {"MEM_LOG_FREE",	cfg_mem_log_free,	NULL },

    /* ASN.1 Log masks	*/
    {"ASN1_LOG_DEC",	cfg_asn1_log_dec,	NULL },
    {"ASN1_LOG_ENC",	cfg_asn1_log_enc,	NULL },
    {"ASN1_LOG_NERR",	cfg_asn1_log_nerr,	NULL },

    /* MMS Demo Log masks	*/
    {"MMS_LOG_DEC",	cfg_mms_log_dec,	NULL },
    {"MMS_LOG_ENC",	cfg_mms_log_enc,	NULL },

    {"MMS_LOG_NERR",	cfg_mms_log_nerr,	NULL },

    {"MMS_LOG_RT",	cfg_mms_log_rt,		NULL },
    {"MMS_LOG_RTAA",	cfg_mms_log_rtaa,	NULL },
    {"MMS_LOG_AA",	cfg_mms_log_aa,		NULL },

    {"MVLLOG_NERR",		cfg_mvllog_nerr,	NULL },
    {"MVLLOG_ACSE",		cfg_mvllog_acse,	NULL },
    {"MVLLOG_ACSEDATA",		cfg_mvllog_acsedata,	NULL },
    {"MVLULOG_FLOW",		cfg_mvlulog_flow,	NULL },

    {"ACSE_LOG_DEC",		cfg_acse_log_dec,	NULL },
    {"ACSE_LOG_ENC",		cfg_acse_log_enc,	NULL },

#if defined (ICCP_LITE)
    {"MILOG_ERR",		cfg_milog_err,		NULL },
    {"MI_LOG_NERR",		cfg_mi_log_nerr,	NULL },
    {"MILOG_FLOW",		cfg_milog_flow,		NULL },
#endif

#if defined (MOSI)
    {"COPP_LOG_DEC",		cfg_copp_log_dec,	NULL },
    {"COPP_LOG_DEC_HEX",	cfg_copp_log_dec_hex,	NULL },
    {"COPP_LOG_ENC",		cfg_copp_log_enc,	NULL },
    {"COPP_LOG_ENC_HEX",	cfg_copp_log_enc_hex,	NULL },

    {"COSP_LOG_DEC",		cfg_cosp_log_dec,	NULL },
    {"COSP_LOG_DEC_HEX",	cfg_cosp_log_dec_hex,	NULL },
    {"COSP_LOG_ENC",		cfg_cosp_log_enc,	NULL },
    {"COSP_LOG_ENC_HEX",	cfg_cosp_log_enc_hex,	NULL },
#endif	/* MOSI	*/

    {"TP4_LOG_FLOWDOWN",	cfg_tp4_log_flowdown,	NULL },
    {"TP4_LOG_FLOWUP",		cfg_tp4_log_flowup,	NULL },

#if defined (UCA_SMP)
    {"SMP_LOG_REQ",		cfg_smp_log_req,	NULL },
    {"SMP_LOG_IND",		cfg_smp_log_ind,	NULL },
#endif

#if defined (LEAN_T)
    {"CLNP_LOG_NERR",		cfg_clnp_log_nerr,		NULL },
    {"CLNP_LOG_REQ",		cfg_clnp_log_req,		NULL },
    {"CLNP_LOG_IND",		cfg_clnp_log_ind,		NULL },
    {"CLNP_LOG_ENC_DEC",	cfg_clnp_log_enc_dec,		NULL },
    {"CLNP_LOG_LLC_ENC_DEC",	cfg_clnp_log_llc_enc_dec,	NULL },
    {"CLSNS_LOG_REQ",		cfg_clsns_log_req,		NULL },
    {"CLSNS_LOG_IND",		cfg_clsns_log_ind,		NULL },
#endif

#if (defined (ADLC_SLAVE) || defined (ADLC_MASTER))
    {"ADLC_LOG_NERR",		cfg_adlc_log_nerr,	NULL },
    {"ADLC_LOG_FLOW",		cfg_adlc_log_flow,	NULL },
    {"ADLC_LOG_IO",		cfg_adlc_log_io,	NULL },
    {"ADLC_LOG_HEXIO",		cfg_adlc_log_hexio,	NULL },
    {"ADLC_LOG_SM",		cfg_adlc_log_sm,	NULL },
    {"ADLC_LOG_HEXSM",		cfg_adlc_log_hexsm,	NULL },
    {"ADLC_LOG_USER",		cfg_adlc_log_user,	NULL },
    {"ADLC_LOG_HEXUSER",	cfg_adlc_log_hexuser,	NULL },
#endif

    /* Integrity checking on memory allocation.				*/
    /* Not really related to logging, but no better place to put them.	*/
    {"m_fill_en",		cfg_m_fill_en,			NULL },
    {"m_heap_check_enable",	cfg_m_heap_check_enable,	NULL },
    {"m_check_list_enable",	cfg_m_check_list_enable,	NULL },
    {"m_find_node_enable",	cfg_m_find_node_enable,		NULL },
    {"m_track_prev_free",	cfg_m_track_prev_free,		NULL },
    {"list_debug_sel",		cfg_list_debug_sel,		NULL },
    {"m_no_realloc_smaller",	cfg_m_no_realloc_smaller,	NULL },

    {"%LogEnd",			cfg_end,			NULL},
    { NULL, NULL, NULL}
  };

/************************************************************************/
/*			read_log_cfg_file				*/
/************************************************************************/

ST_RET read_log_cfg_file (ST_CHAR *filename)
  {
ST_RET  ret_val;
ST_UINT cfg_log_mask_save;

  mvl_debug_sel |= MVLLOG_ERR;

  /* Copy original "sLogCtrl" to temporary struct which may be modified	*/
  /* by the configuration.						*/
  /* This prevents logging control changes while there may be logging.	*/
  memcpy (&tmp_sLogCtrl, sLogCtrl, sizeof (LOG_CTRL));

  ret_val = cfg_process_file (filename, mlog_base_kwtbl);

  if (ret_val == SD_SUCCESS)
    {
    /* Config successful: Save new settings in "sLogCtrl".		*/
    memcpy (sLogCtrl, &tmp_sLogCtrl, sizeof (LOG_CTRL));

    /* Config can't change "state" or "fp", so if log file was open,	*/
    /* close it and clear state.					*/
    if (sLogCtrl->fc.state & FIL_STATE_OPEN)
      {					/* log file already opened	*/
      fclose (sLogCtrl->fc.fp);			/* close it		*/
      sLogCtrl->fc.state &= ~(FIL_STATE_OPEN);	/* clear state		*/
      }
    }
  else
    {
    /* Config failed: Do not modify "sLogCtrl".				*/
    /* Turn on cfg logging and parse file again.				*/
    cfg_log_mask_save = cfg_log_mask;		/* Save old mask	*/
    cfg_log_mask = CFG_LOG_ERR | CFG_LOG_FLOW;
    cfg_process_file (filename, mlog_base_kwtbl);
    cfg_log_mask = cfg_log_mask_save;		/* Restore old mask	*/
    }
  
  return (ret_val);
  }

/*------------- Internal functions for handling state tables -----------*/

/************************************************************************/
/*			cfg_begin					*/
/************************************************************************/
static ST_VOID  cfg_begin (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_begin: entered.");
#endif
  if (cfg_tbl_push (mlog_settings_kwtbl) != SD_SUCCESS)
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfg_lc_file_log_en				*/
/************************************************************************/
static ST_VOID  cfg_lc_file_log_en (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_file_log_en: entered.");
#endif
  /* Found 'FileLogEn' flag						*/
  M_SET_CTRL (LOG_FILE_EN);
  }

/************************************************************************/
/*			cfg_lc_mem_log_en				*/
/************************************************************************/
static ST_VOID  cfg_lc_mem_log_en (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_mem_log_en: entered.");
#endif
  /* Found 'MemLogEn' flag						*/
  M_SET_CTRL (LOG_MEM_EN);
  }

/************************************************************************/
/*			cfg_lc_log_file_size				*/
/************************************************************************/
ST_VOID  cfg_lc_log_file_size (ST_VOID)
  {
  ST_ULONG lval;

#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_log_file_size: entered.");
#endif
  /* Found 'LogFileSize' keyword, get the value				*/
  if (cfg_get_ulong (&lval) == SD_SUCCESS)
    {
    tmp_sLogCtrl.fc.maxSize = lval;    /* Size of the circular log file	*/
    }
  else
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfg_lc_log_file_name				*/
/************************************************************************/
ST_VOID  cfg_lc_log_file_name (ST_VOID)
  {
  ST_CHAR  *fname;

#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_log_file_name: entered.");
#endif
  /* Found 'LogFileName' keyword, read the user defined log file name	*/
  if ((fname = cfg_get_string_ptr ()) != NULL)
    {						/* Save name	*/
    if (strlen (fname) < sizeof (log_file_name))
      {
      strcpy (log_file_name, fname);
      tmp_sLogCtrl.fc.fileName = log_file_name;
      }
    else
      cfg_set_config_err ();	/* Would overflow buffer	*/
    }
  else
    cfg_set_config_err ();
}

/************************************************************************/
/*			cfg_lc_log_mem_items				*/
/************************************************************************/
ST_VOID  cfg_lc_log_mem_items (ST_VOID)
  {
  ST_UINT val;

#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_log_mem_items: entered.");
#endif
  /* Found 'LogMemItems' keyword, get the value				*/
  if (cfg_get_uint (&val) == SD_SUCCESS)
    {
    tmp_sLogCtrl.mc.maxItems = val;
    }
  else
    {
    cfg_set_config_err ();
    }
  }

/************************************************************************/
/*			cfg_lc_dump_file_name				*/
/************************************************************************/
ST_VOID  cfg_lc_dump_file_name (ST_VOID)
  {
  ST_CHAR  *fname;

#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_dump_file_name: entered.");
#endif

  /* Found 'DumpFileName' keyword, read the user defined log file name	*/

  if ((fname = cfg_get_string_ptr ()) != NULL)
    {						/* Save file name	*/
    if (strlen (fname) < sizeof (memlog_file_name))
      {
      strcpy (memlog_file_name, fname);
      tmp_sLogCtrl.mc.dumpFileName = memlog_file_name;
      }
    else
      cfg_set_config_err ();	/* Would overflow buffer	*/
    }
  else
    {
    cfg_set_config_err ();
    }
  }

/************************************************************************/
/*			cfg_lc_mem_auto_dump				*/
/************************************************************************/
ST_VOID  cfg_lc_mem_auto_dump (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_mem_auto_dump: entered.");
#endif
  /* Found 'MemAutoDump' flag						*/
  M_SET_MEM_CTRL (MEM_CTRL_AUTODUMP_EN);
  }

/************************************************************************/
/*			cfg_lc_mem_hw_dump				*/
/************************************************************************/
ST_VOID  cfg_lc_mem_hw_dump (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_mem_hw_dump: entered.");
#endif
  m_auto_hw_log = SD_TRUE;
  }

/************************************************************************/
/*			cfg_lc_time_enable				*/
/************************************************************************/
static ST_VOID  cfg_lc_time_enable (ST_VOID)
  {
ST_UINT val;

#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_time_enable: entered.");
#endif
  /* Found 'TimeEnable' keyword, get the value				*/

  if (cfg_get_uint (&val) == SD_SUCCESS)
    {
    if (val == 1)
      {
      M_SET_CTRL (LOG_TIME_EN);		/* Set Time/Date logging	*/
      }
    else if (val == 2)
      {
      M_SET_CTRL (LOG_TIME_EN);		/* Set Time/Date logging	*/
      }
    else
      cfg_set_config_err ();
    }
  else
    cfg_set_config_err ();
  }

/************************************************************************/
/*			cfg_lc_hard_flush				*/
/************************************************************************/
static ST_VOID  cfg_lc_hard_flush (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_hard_flush: entered.");
#endif
  /* Found 'HardFlush' flag						*/
  M_SET_FILE_CTRL (FIL_CTRL_HARD_FLUSH);  /* Hard flush is requested	*/

  }

/************************************************************************/
/*			cfg_lc_no_append				*/
/************************************************************************/
static ST_VOID  cfg_lc_no_append (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_no_append: entered.");
#endif
  /* Found 'NoAppend' flag						*/
  M_SET_FILE_CTRL (FIL_CTRL_NO_APPEND);  /* overwrite is requested	*/
  }

/************************************************************************/
/*			cfg_lc_no_msg_header				*/
/************************************************************************/
static ST_VOID  cfg_lc_no_msg_header (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_no_msg_header: entered.");
#endif
  /* Found 'NoMsgheader' flag						*/
  M_CLR_FILE_CTRL (FIL_CTRL_MSG_HDR_EN);
  M_CLR_MEM_CTRL (MEM_CTRL_MSG_HDR_EN);
  }

/************************************************************************/
/*			cfg_lc_no_wipe					*/
/************************************************************************/
static ST_VOID  cfg_lc_no_wipe (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_no_wipe: entered.");
#endif
  /* Found 'NoWipe' flag						*/
  M_CLR_FILE_CTRL (FIL_CTRL_WIPE_EN);
  }

/************************************************************************/
/*			cfg_lc_no_wrap					*/
/************************************************************************/
static ST_VOID  cfg_lc_no_wrap (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_no_wrap: entered.");
#endif
  /* Found 'NoWrap' flag						*/
  M_CLR_FILE_CTRL (FIL_CTRL_WRAP_EN);
  }

/************************************************************************/
/*			cfg_lc_setbuf					*/
/************************************************************************/
static ST_VOID  cfg_lc_setbuf (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_setbuf: entered.");
#endif
  /* Found 'Setbuf' flag						*/
  M_SET_FILE_CTRL (FIL_CTRL_SETBUF_EN);  /* Setbuf is requested		*/
  }

/************************************************************************/
/*			cfg_lc_no_header_cr					*/
/************************************************************************/
static ST_VOID  cfg_lc_no_header_cr (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_no_header_cr: entered.");
#endif
  M_SET_CTRL (LOG_NO_HEADER_CR);
  }

/************************************************************************/
/*			cfg_lc_suppress_filename					*/
/************************************************************************/
static ST_VOID  cfg_lc_suppress_filename (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_suppress_filename: entered.");
#endif
  M_SET_CTRL (LOG_FILENAME_SUPPRESS);
  }

/************************************************************************/
/*			cfg_lc_suppress_logtype					*/
/************************************************************************/
static ST_VOID  cfg_lc_suppress_logtype (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_lc_suppress_logtype: entered.");
#endif
  M_SET_CTRL (LOG_LOGTYPE_SUPPRESS);
  }

/************************************************************************/
/*			cfg_mem_log_calloc				*/
/************************************************************************/

static ST_VOID  cfg_mem_log_calloc (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mem_log_calloc: entered.");
#endif
  chk_debug_en |= MEM_LOG_CALLOC;
  }

/************************************************************************/
/*			cfg_mem_log_malloc				*/
/************************************************************************/

static ST_VOID  cfg_mem_log_malloc (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mem_log_malloc: entered.");
#endif
  chk_debug_en |= MEM_LOG_MALLOC;
  }

/************************************************************************/
/*			cfg_mem_log_realloc				*/
/************************************************************************/
static ST_VOID  cfg_mem_log_realloc (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mem_log_realloc: entered.");
#endif
  chk_debug_en |= MEM_LOG_REALLOC;
  }

/************************************************************************/
/*			cfg_mem_log_free				*/
/************************************************************************/
static ST_VOID  cfg_mem_log_free (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mem_log_free: entered.");
#endif
  chk_debug_en |= MEM_LOG_FREE;
  }

/************************************************************************/
/*			cfg_m_fill_en					*/
/************************************************************************/
static ST_VOID  cfg_m_fill_en (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_m_fill_en: entered.");
#endif
  m_fill_en = SD_TRUE;
  }

/************************************************************************/
/*			cfg_m_heap_check_enable				*/
/************************************************************************/
static ST_VOID  cfg_m_heap_check_enable (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_m_heap_check_enable: entered.");
#endif
  m_heap_check_enable = SD_TRUE;
  }

/************************************************************************/
/*			cfg_m_check_list_enable				*/
/************************************************************************/
static ST_VOID  cfg_m_check_list_enable (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_m_check_list_enable: entered.");
#endif
  m_check_list_enable = SD_TRUE;
  }

/************************************************************************/
/*			cfg_m_find_node_enable				*/
/************************************************************************/
static ST_VOID  cfg_m_find_node_enable (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_m_find_node_enable: entered.");
#endif
  m_find_node_enable = SD_TRUE;
  }

/************************************************************************/
/*			cfg_m_track_prev_free				*/
/************************************************************************/
static ST_VOID  cfg_m_track_prev_free (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_m_track_prev_free: entered.");
#endif
  m_track_prev_free = SD_TRUE;
  }

/************************************************************************/
/*			cfg_list_debug_sel				*/
/************************************************************************/
static ST_VOID  cfg_list_debug_sel (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_list_debug_sel: entered.");
#endif
  list_debug_sel = SD_TRUE;
  }

/************************************************************************/
/*			cfg_m_no_realloc_smaller			*/
/************************************************************************/
static ST_VOID  cfg_m_no_realloc_smaller (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_m_no_realloc_smaller: entered.");
#endif
  m_no_realloc_smaller = SD_TRUE;
  }

/************************************************************************/
/*			cfg_asn1_log_dec				*/
/************************************************************************/
static ST_VOID  cfg_asn1_log_dec (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_asn1_log_dec: entered.");
#endif
  asn1_debug_sel |= ASN1_LOG_DEC;
  }

/************************************************************************/
/*			cfg_asn1_log_enc				*/
/************************************************************************/
static ST_VOID  cfg_asn1_log_enc (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_asn1_log_enc: entered.");
#endif
  asn1_debug_sel |= ASN1_LOG_ENC;
  }

/************************************************************************/
/*			cfg_asn1_log_nerr				*/
/************************************************************************/
static ST_VOID  cfg_asn1_log_nerr (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_asn1_log_nerr: entered.");
#endif
  asn1_debug_sel |= ASN1_LOG_NERR;
  }

/************************************************************************/
/*			cfg_mms_log_dec					*/
/************************************************************************/
static ST_VOID  cfg_mms_log_dec (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mms_log_dec: entered.");
#endif
  mms_debug_sel |= MMS_LOG_DEC;
  }

/************************************************************************/
/*			cfg_mms_log_enc					*/
/************************************************************************/
static ST_VOID  cfg_mms_log_enc (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mms_log_enc: entered.");
#endif
  mms_debug_sel |= MMS_LOG_ENC;
  }

/************************************************************************/
/*			cfg_mms_log_nerr				*/
/************************************************************************/
static ST_VOID  cfg_mms_log_nerr (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mms_log_nerr: entered.");
#endif
  mms_debug_sel |= MMS_LOG_NERR;
  }

/************************************************************************/
/*			cfg_mms_log_rt					*/
/************************************************************************/
static ST_VOID  cfg_mms_log_rt (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mms_log_rt: entered.");
#endif
  mms_debug_sel |= MMS_LOG_RT;
  }

/************************************************************************/
/*			cfg_mms_log_rtaa				*/
/************************************************************************/
static ST_VOID  cfg_mms_log_rtaa (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mms_log_rtaa: entered.");
#endif
  mms_debug_sel |= MMS_LOG_RTAA;
  }

/************************************************************************/
/*			cfg_mms_log_aa					*/
/************************************************************************/
static ST_VOID  cfg_mms_log_aa (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mms_log_aa: entered.");
#endif
  mms_debug_sel |= MMS_LOG_AA;
  }

/************************************************************************/
/*			cfg_mvllog_nerr					*/
/************************************************************************/
static ST_VOID  cfg_mvllog_nerr (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mvllog_nerr: entered.");
#endif
  mvl_debug_sel |= MVLLOG_NERR;
  }

/************************************************************************/
/*			cfg_mvllog_acse					*/
/************************************************************************/
static ST_VOID  cfg_mvllog_acse (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mvllog_acse: entered.");
#endif
  mvl_debug_sel |= MVLLOG_ACSE;
  }

/************************************************************************/
/*			cfg_mvllog_acsedata				*/
/************************************************************************/
static ST_VOID  cfg_mvllog_acsedata (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mvllog_acsedata: entered.");
#endif
  mvl_debug_sel |= MVLLOG_ACSEDATA;
  }


/************************************************************************/
/*			cfg_mvlulog_flow					*/
/************************************************************************/
static ST_VOID  cfg_mvlulog_flow (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mvlulog_flow: entered.");
#endif
  mvl_debug_sel |= MVLULOG_FLOW;
  }

/************************************************************************/
/*			cfg_acse_log_dec				*/
/************************************************************************/
static ST_VOID  cfg_acse_log_dec (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_acse_log_dec: entered.");
#endif
  acse_debug_sel |= ACSE_LOG_DEC;
  }

/************************************************************************/
/*			cfg_acse_log_enc				*/
/************************************************************************/
static ST_VOID  cfg_acse_log_enc (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_acse_log_enc: entered.");
#endif
  acse_debug_sel |= ACSE_LOG_ENC;
  }

#if defined (ICCP_LITE)
/************************************************************************/
/*			cfg_milog_err					*/
/************************************************************************/
static ST_VOID  cfg_milog_err (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_milog_err: entered.");
#endif
  mi_debug_sel |= MILOG_ERR;
  }

/************************************************************************/
/*			cfg_mi_log_nerr					*/
/************************************************************************/
static ST_VOID  cfg_mi_log_nerr (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_mi_log_nerr: entered.");
#endif
  mi_debug_sel |= MI_LOG_NERR;
  }

/************************************************************************/
/*			cfg_milog_flow					*/
/************************************************************************/
static ST_VOID  cfg_milog_flow (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_milog_flow: entered.");
#endif
  mi_debug_sel |= MILOG_FLOW;
  }
#endif

#if defined (MOSI)	/* COPP & COSP logging only for MOSI.		*/

/************************************************************************/
/*			cfg_copp_log_dec				*/
/************************************************************************/
static ST_VOID cfg_copp_log_dec (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_copp_log_dec: entered.");
#endif
  acse_debug_sel |= COPP_LOG_DEC;
  }

/************************************************************************/
/*			cfg_copp_log_dec_hex				*/
/************************************************************************/
static ST_VOID cfg_copp_log_dec_hex (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_copp_log_dec_hex: entered.");
#endif
  acse_debug_sel |= COPP_LOG_DEC_HEX;
  }

/************************************************************************/
/*			cfg_copp_log_enc				*/
/************************************************************************/
static ST_VOID cfg_copp_log_enc (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_copp_log_enc: entered.");
#endif
  acse_debug_sel |= COPP_LOG_ENC;
  }

/************************************************************************/
/*			cfg_copp_log_enc_hex				*/
/************************************************************************/
static ST_VOID cfg_copp_log_enc_hex (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_copp_log_enc_hex: entered.");
#endif
  acse_debug_sel |= COPP_LOG_ENC_HEX;
  }

/************************************************************************/
/*			cfg_cosp_log_dec				*/
/************************************************************************/
static ST_VOID cfg_cosp_log_dec (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_cosp_log_dec: entered.");
#endif
  acse_debug_sel |= COSP_LOG_DEC;
  }

/************************************************************************/
/*			cfg_cosp_log_dec_hex				*/
/************************************************************************/
static ST_VOID cfg_cosp_log_dec_hex (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_cosp_log_dec_hex: entered.");
#endif
  acse_debug_sel |= COSP_LOG_DEC_HEX;
  }

/************************************************************************/
/*			cfg_cosp_log_enc				*/
/************************************************************************/
static ST_VOID cfg_cosp_log_enc (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_cosp_log_enc: entered.");
#endif
  acse_debug_sel |= COSP_LOG_ENC;
  }

/************************************************************************/
/*			cfg_cosp_log_enc_hex				*/
/************************************************************************/
static ST_VOID cfg_cosp_log_enc_hex (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_cosp_log_enc_hex: entered.");
#endif
  acse_debug_sel |= COSP_LOG_ENC_HEX;
  }
#endif	/* MOSI	*/

/************************************************************************/
/*			cfg_tp4_log_flowdown				*/
/************************************************************************/
static ST_VOID  cfg_tp4_log_flowdown (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_tp4_log_flowdown: entered.");
#endif
#if !defined(MAP30_ACSE)
  tp4_debug_sel |= TP4_LOG_FLOWDOWN;
#endif
  }

/************************************************************************/
/*			cfg_tp4_log_flowup				*/
/************************************************************************/
static ST_VOID  cfg_tp4_log_flowup (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_tp4_log_flowup: entered.");
#endif
#if !defined(MAP30_ACSE)
  tp4_debug_sel |= TP4_LOG_FLOWUP;
#endif
  }

#if defined (UCA_SMP)
/************************************************************************/
/*			cfg_smp_log_req					*/
/************************************************************************/
static ST_VOID  cfg_smp_log_req (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_smp_log_req: entered.");
#endif
  smp_debug_sel |= SMP_LOG_REQ;
  }

/************************************************************************/
/*			cfg_smp_log_ind					*/
/************************************************************************/
static ST_VOID  cfg_smp_log_ind (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_smp_log_ind: entered.");
#endif
  smp_debug_sel |= SMP_LOG_IND;
  }
#endif	/* defined (UCA_SMP)	*/

#if defined (LEAN_T)
/************************************************************************/
/*			cfg_clnp_log_nerr				*/
/************************************************************************/
static ST_VOID  cfg_clnp_log_nerr (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_clnp_log_nerr: entered.");
#endif
#if !defined(MAP30_ACSE)
  clnp_debug_sel |= CLNP_LOG_NERR;
#endif
  }

/************************************************************************/
/*			cfg_clnp_log_req				*/
/************************************************************************/
static ST_VOID  cfg_clnp_log_req (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_clnp_log_req: entered.");
#endif
#if !defined(MAP30_ACSE)
  clnp_debug_sel |= CLNP_LOG_REQ;
#endif
  }

/************************************************************************/
/*			cfg_clnp_log_ind				*/
/************************************************************************/
static ST_VOID  cfg_clnp_log_ind (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_clnp_log_ind: entered.");
#endif
#if !defined(MAP30_ACSE)
  clnp_debug_sel |= CLNP_LOG_IND;
#endif
  }

/************************************************************************/
/*			cfg_clnp_log_enc_dec				*/
/************************************************************************/
static ST_VOID  cfg_clnp_log_enc_dec (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_clnp_log_enc_dec: entered.");
#endif
#if !defined(MAP30_ACSE)
  clnp_debug_sel |= CLNP_LOG_ENC_DEC;
#endif
  }

/************************************************************************/
/*			cfg_clnp_log_llc_enc_dec			*/
/************************************************************************/
static ST_VOID  cfg_clnp_log_llc_enc_dec (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_clnp_log_llc_enc_dec: entered.");
#endif
#if !defined(MAP30_ACSE)
  clnp_debug_sel |= CLNP_LOG_LLC_ENC_DEC;
#endif
  }

/************************************************************************/
/*			cfg_clsns_log_req				*/
/************************************************************************/
static ST_VOID  cfg_clsns_log_req (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_clsns_log_req: entered.");
#endif
#if !defined(MAP30_ACSE)
  clnp_debug_sel |= CLSNS_LOG_REQ;
#endif
  }

/************************************************************************/
/*			cfg_clsns_log_ind				*/
/************************************************************************/
static ST_VOID  cfg_clsns_log_ind (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_clsns_log_ind: entered.");
#endif
#if !defined(MAP30_ACSE)
  clnp_debug_sel |= CLSNS_LOG_IND;
#endif
  }
#endif	/* defined (LEAN_T)	*/

#if (defined (ADLC_SLAVE) || defined (ADLC_MASTER))
/************************************************************************/
/*			cfg_adlc_log_nerr				*/
/* Found 'ADLC_LOG_NERR' keyword					*/
/************************************************************************/
static ST_VOID  cfg_adlc_log_nerr (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_adlc_log_nerr: entered.");
#endif
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_NERR);
  }

/************************************************************************/
/*			cfg_adlc_log_flow				*/
/* Found 'ADLC_LOG_FLOW' keyword					*/
/************************************************************************/
static ST_VOID  cfg_adlc_log_flow (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_adlc_log_flow: entered.");
#endif
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_FLOW);
  }

/************************************************************************/
/*			cfg_adlc_log_io					*/
/* Found 'ADLC_LOG_IO' keyword						*/
/************************************************************************/
static ST_VOID  cfg_adlc_log_io (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_adlc_log_io: entered.");
#endif
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_IO);
  }

/************************************************************************/
/*			cfg_adlc_log_hexio				*/
/* Found 'ADLC_LOG_HEXIO' keyword					*/
/************************************************************************/
static ST_VOID  cfg_adlc_log_hexio (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_adlc_log_hexio: entered.");
#endif
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_HEXIO);
  }

/************************************************************************/
/*			cfg_adlc_log_sm					*/
/* Found 'ADLC_LOG_SM' keyword						*/
/************************************************************************/
static ST_VOID  cfg_adlc_log_sm (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_adlc_log_sm: entered.");
#endif
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_SM);
  }
/************************************************************************/
/*			cfg_adlc_log_hexsm				*/
/* Found 'ADLC_LOG_HEXSM' keyword					*/
/************************************************************************/
static ST_VOID  cfg_adlc_log_hexsm (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_adlc_log_hexsm: entered.");
#endif
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_HEXSM);
  }
/************************************************************************/
/*			cfg_adlc_log_user				*/
/* Found 'ADLC_LOG_USER' keyword					*/
/************************************************************************/
static ST_VOID  cfg_adlc_log_user (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_adlc_log_user: entered.");
#endif
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_USER);
  }
/************************************************************************/
/*			cfg_adlc_log_hexuser				*/
/* Found 'ADLC_LOG_HEXUSER' keyword					*/
/************************************************************************/
static ST_VOID  cfg_adlc_log_hexuser (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_adlc_log_hexuser: entered.");
#endif
  ADLC_SET_LOG_MASK (ADLC_MASK_LOG_HEXUSER);
  }
#endif	/* (defined (ADLC_SLAVE) || defined (ADLC_MASTER))	*/

/************************************************************************/
/*				cfg_end					*/
/************************************************************************/

static ST_VOID  cfg_end (ST_VOID)
  {
#ifdef S_TABLE_DEBUG
  printf ("\ncfg_end: entered.");
#endif
  /* Pop the table from the stack	*/
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();

  cfg_set_endfile_ok ();
  }

#endif	/* DEBUG_SISCO	*/
