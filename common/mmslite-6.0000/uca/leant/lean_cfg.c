/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2002 All Rights Reserved					*/
/*									*/
/* MODULE NAME : lean_cfg.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements parsing of the Lean-T Stack	*/
/*		configuration file.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*                                                                      */
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 06/27/02  JRB     28    Add tp0_sock.h.				*/
/* 06/20/02  JRB     27    Chg to use new netAddr union in PRES_ADDR.	*/
/* 07/17/00  JRB     26    Set DriverID only if TP4_ENABLED defined.	*/
/* 06/15/00  JRB     25    Set DriverID only if MOSI defined.		*/
/* 05/02/00  JRB     24    Added DriverID for WIN32.			*/
/* 01/25/00  MDE     23    Fixed compile problem for DEBUG version	*/
/* 12/10/99  JRB     22    Fix save_dib_table when AP Title or AE Qual	*/
/*			   NOT CONFIGURED or AP Title parse fails.	*/
/*			   Fix parseObjId for invalid data.		*/
/* 06/08/99  JRB     21    Chg max_num_conns to ST_UINT16.		*/
/* 09/18/98  JRB     20    Del all logging cfg. Use "log_cfg.c" instead.*/
/* 08/13/98  JRB     19    Lint cleanup.				*/
/* 08/06/98  JRB     18    Use new cfg_get_octet_string function &	*/
/*			   delete parseSelector.			*/
/* 06/19/98  JRB     17    Allow TP_TYPE_TPX for multiple stack support.*/
/* 03/11/98  JRB     16    Del "max_spdu_length" param from tp4c_kwtbl..*/
/*			   Add "rfc1006_keepalive" param to tp4c_kwtbl..*/
/* 02/16/98  JRB     15    Clean up some #ifs.				*/
/* 08/26/97  JRB     14    RFC1006 changes:				*/
/*			   - Use new DIB_ENTRY with embedded PRES_ADDR.	*/
/*			   - Include "lean_a.h" instead of "suicacse.h"	*/
/*			   - Add "rfc1006_*" params to "tp4c_kwtbl".	*/
/*			   - Add "tranport", "ip_addr" to "dibc_kwtbl.."*/
/*			   - If !TP4_ENABLED, ignore TP4/CLNP config	*/
/*			   - If TP0_ENABLED, save TP0 config		*/
/*			   - parseSelector: chg arg back to (ST_INT *)	*/
/*			   - parseSelector: don't ignore maxSelLen arg	*/
/*			   - parseObjId: add default "return" statement.*/
/* 07/09/97  JRB     13    Deleted obsolete clnp_err.h include.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/03/97  EJV     12    Removed CLNP_LOG_ALWAYS			*/
/* 11/19/96  MDE     11    Now handle 0 dib entries w/o mem error	*/
/* 11/13/96  JRB     10    Don't init globals unless they are constant.	*/
/* 10/04/96  JRB     09    Add ak_delay to TP4 config.			*/
/* 10/04/96  JRB     08    Chg parseSelector arg to (ST_INT16 *).		*/
/* 10/04/96  JRB     07    Chg funct name to "save_loc_rem_dib_tables"	*/
/*			   and modify for reconfig.			*/
/* 10/04/96  JRB     06    Add init_dirser_called flag for reconfig.	*/
/* 10/04/96  JRB     05    If MCGW defined, don't config TP4, and add	*/
/*			   adlcAddr to DIB config.			*/
/* 09/04/96  EJV     04    Fixed keyword table (_ENC_ instead of _END_)	*/
/* 07/24/96  EJV     03    Combined CLNP, TP4 and DIB configuration	*/
/*			   files into one.				*/
/* 07/22/96  JRB     02    Fixed up names, etc. for Lean-T.		*/
/* 07/22/96  MDE     01    Created from rs_cfg.c			*/
/************************************************************************/
#ifdef DEBUG_SISCO
static char *thisFileName = __FILE__;
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "gen_list.h"
#include "cfg_util.h"
#include "cfglog.h"


#if defined (MCGW)
/* For the MCGW, the following global variables and function names	*/
/* are renamed by macro so they don't conflict with names in OSI SUIC.	*/
#define num_loc_dib_entries	num_loc_dib_entries_clt7
#define num_rem_dib_entries	num_rem_dib_entries_clt7
#define loc_dib_table		loc_dib_table_clt7
#define rem_dib_table		rem_dib_table_clt7
#define init_dirser		init_dirser_clt7
#endif		/* defined (MCGW)	*/

#include "clnp_usr.h"
#include "tp4api.h"
#include "tp0_sock.h"		/* portable "sockets" interface		*/
#include "lean_a.h"		/* DIB_ENTRY definition.		*/
#include "acse2log.h"

#if defined (_WIN32)
#include "ethsub.h"
#endif

/************************************************************************/
/* Local variables in this module					*/
/************************************************************************/

static ST_CHAR *cfg_file_name = "lean.cfg";
static ST_BOOLEAN init_dirser_called;

static ST_RET save_loc_rem_dib_tables (void);

/*----------------------------------------------------------------------*/
/*  Base configuration keyword table for major sections	in cfg file.	*/
/*  Functions and keyword tables related to parsing specific section	*/
/*  are defined in respective part of this module.			*/
/*----------------------------------------------------------------------*/
static void clnpc_begin (void);
static void  tp4c_begin (void);
static void  dibc_begin (void);

static CFG_KW_EL cfg_base_kwtbl [] =
  {
    {"%Clnp_Begin",		clnpc_begin,	NULL },
#if !defined (MCGW)
    { "%Tp4_Begin",		 tp4c_begin, 	NULL },
#endif		/* !defined (MCGW)	*/
    { "%Dib_Begin",		 dibc_begin, 	NULL },
    { NULL, NULL, NULL}
  };

/************************************************************************/
/*                       init_dirser					*/
/*----------------------------------------------------------------------*/
/* This function will initialize the LEAN-T Stack parameters from	*/
/* configuration file.							*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if configuration successful		*/
/*	SD_FAILURE (1)		otherwise				*/
/************************************************************************/

ST_RET init_dirser (ST_VOID)
  {
ST_RET ret;

  ret = SD_FAILURE;

  /* This function will initialize the state table for reading the	*/
  /* configuration file and parse the file until EOF or end flag is set	*/

  /* Read the configuration file */
  ret = cfg_process_file (cfg_file_name, cfg_base_kwtbl);

  if (ret == SD_SUCCESS)
    {
    save_loc_rem_dib_tables ();
    }
  else
    {
    ST_UINT cfg_log_mask_save = cfg_log_mask;

    /* parsing cfg file failed, turn on cfg logging and parse file again */
    cfg_log_mask = CFG_LOG_ERR | CFG_LOG_FLOW;
    cfg_process_file (cfg_file_name, cfg_base_kwtbl);
    cfg_log_mask = cfg_log_mask_save;
    }

  /* Set flag to prevent reconfiguring of critical parameters.		*/
  init_dirser_called = SD_TRUE;
  return (ret);
  }


/*======================================================================*/
/************************************************************************/
/*======================================================================*/
/*									*/
/*	Functions and keyword tables for parsing the CLNP section	*/
/*	from configuration file.					*/
/*									*/
/*======================================================================*/
/************************************************************************/
/*======================================================================*/

#if defined(_WIN32) && defined(MOSI) && defined(TP4_ENABLED)
static void clnpc_driverid (void);
#endif
static void clnpc_lifetime (void);
static void clnpc_lifetime_dec (void);
static void clnpc_cfg_timer (void);
static void clnpc_esh_delay (void);
static void clnpc_loc_mac (void);
static void clnpc_loc_nsap (void);

static void clnpc_end (void);


/* CLNP section keyword tables	*/

static CFG_KW_EL clnpc_kwtbl [] =
  {
#if defined(_WIN32) && defined(MOSI) && defined(TP4_ENABLED)
    {"DriverID",		clnpc_driverid,		NULL },
#endif
    {"Lifetime",		clnpc_lifetime,		NULL },
    {"Lifetime_Decrement",	clnpc_lifetime_dec,	NULL },
    {"Cfg_Timer",		clnpc_cfg_timer,    	NULL },
    {"Esh_Delay",		clnpc_esh_delay,    	NULL },
    {"Local_MAC",		clnpc_loc_mac,		NULL },
    {"Local_NSAP",		clnpc_loc_nsap,		NULL },
    {"%Clnp_End",		clnpc_end,      	NULL },
    { NULL, NULL, NULL}
  };

/*----------------------------------------------------------------------*/
/*			clnpc_begin					*/
/* keyword '%Clnp_Begin'							*/
/*----------------------------------------------------------------------*/
static void  clnpc_begin (void)
  {
  if (cfg_tbl_push (clnpc_kwtbl) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/*----------------------------------------------------------------------*/
/*			clnpc_end					*/
/* keyword '%Clnp_End'							*/
/*----------------------------------------------------------------------*/
static void  clnpc_end (void)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();
  }

#if defined(_WIN32) && defined(MOSI) && defined(TP4_ENABLED)
/*----------------------------------------------------------------------*/
/*			clnpc_driverid					*/
/* keyword 'driverid'							*/
/*----------------------------------------------------------------------*/
static void clnpc_driverid (void)
  {
ST_INT iVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_int (&iVal) == SD_SUCCESS)
    {
    DriverID = iVal;
    }
  else
    cfg_set_config_err();
  }
#endif	/* _WIN32 && MOSI && TP4_ENALBED	*/

/*----------------------------------------------------------------------*/
/*			clnpc_lifetime					*/
/* keyword 'Lifetime'							*/
/*----------------------------------------------------------------------*/
static void clnpc_lifetime (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    clnp_param.pdu_lifetime = (ST_UCHAR) sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			clnpc_lifetime_dec				*/
/* keyword 'Lifetime_Decrement'						*/
/*----------------------------------------------------------------------*/
static void clnpc_lifetime_dec (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    clnp_param.pdu_lifetime_dec = (ST_UCHAR) sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			clnpc_cfg_timer					*/
/* keyword 'Cfg_Timer'							*/
/*----------------------------------------------------------------------*/
static void clnpc_cfg_timer (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    clnp_param.esh_cfg_timer = sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			clnpc_esh_delay					*/
/* keyword 'Esh_Delay'							*/
/*----------------------------------------------------------------------*/
static void clnpc_esh_delay (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    clnp_param.esh_delay = sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			clnpc_loc_mac					*/
/* keyword 'Local_MAC'							*/
/*----------------------------------------------------------------------*/
static void clnpc_loc_mac (void)
  {
#if defined (TP4_ENABLED)
ST_UINT mac_len;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_octet_string (clnp_param.loc_mac, &mac_len, CLNP_MAX_LEN_MAC))
    cfg_set_config_err();
#else	/* !TP4_ENABLED	*/
  cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
#endif	/* !TP4_ENABLED	*/
  }

/*----------------------------------------------------------------------*/
/*			clnpc_loc_nsap					*/
/* keyword 'Local_NSAP'							*/
/*----------------------------------------------------------------------*/
static void clnpc_loc_nsap (void)
  {
#if defined (TP4_ENABLED)
ST_UINT nsap_len;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_octet_string (&clnp_param.loc_nsap[1], &nsap_len, CLNP_MAX_LEN_NSAP)
      == SD_SUCCESS)
    {
    if (nsap_len > 0)
      clnp_param.loc_nsap[0] = (ST_UCHAR) nsap_len;        
    }
  else
    cfg_set_config_err();
#else	/* !TP4_ENABLED	*/
  cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
#endif	/* !TP4_ENABLED	*/
  }


#if !defined (MCGW)
/*======================================================================*/
/************************************************************************/
/*======================================================================*/
/*									*/
/*	Functions and keyword tables for parsing the TP4 section	*/
/*	from configuration file.					*/
/*									*/
/*======================================================================*/
/************************************************************************/
/*======================================================================*/

static void tp4c_end (void);

static void tp4c_max_tpdu_len (void);
static void tp4c_max_rem_cdt (void);
static void tp4c_loc_cdt (void);
static void tp4c_max_num_conns (void);
static void tp4c_max_spdu_outst (void);
static void tp4c_window_time (void);
static void tp4c_inact_time (void);
static void tp4c_retrans_time (void);
static void tp4c_max_trans (void);
static void tp4c_ak_delay (void);

static void rfc1006_max_tpdu_len (void);
static void rfc1006_max_num_conns (void);
static void rfc1006_keepalive (void);

/* TP4 section keyword tables	*/

static CFG_KW_EL tp4c_kwtbl [] =
  {
    {"Max_Tpdu_Length",		tp4c_max_tpdu_len,	NULL },
    {"Max_Remote_Cdt",		tp4c_max_rem_cdt,	NULL },
    {"Local_Cdt",		tp4c_loc_cdt,    	NULL },
    {"Max_Num_Connections",	tp4c_max_num_conns,    	NULL },
    {"Max_Spdu_Outstanding",	tp4c_max_spdu_outst,	NULL },
    {"Window_Time",		tp4c_window_time,    	NULL },
    {"Inactivity_Time",		tp4c_inact_time,    	NULL },
    {"Retransmission_Time",	tp4c_retrans_time,    	NULL },
    {"Max_Transmissions",	tp4c_max_trans,    	NULL },
    {"ak_delay", 		tp4c_ak_delay,		NULL },
    {"RFC1006_Max_Tpdu_Len",	rfc1006_max_tpdu_len,	NULL },
    {"RFC1006_Max_Num_Conns",	rfc1006_max_num_conns,	NULL },
    {"rfc1006_keepalive",	rfc1006_keepalive,	NULL },
    {"%Tp4_End", 		tp4c_end,      		NULL },
    { NULL, NULL, NULL}
  };

/*----------------------------------------------------------------------*/
/*			tp4c_begin					*/
/* keyword '%Tp4_Begin'							*/
/*----------------------------------------------------------------------*/
static void  tp4c_begin (void)
  {
  if (cfg_tbl_push (tp4c_kwtbl) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_end					*/
/* keyword '%Tp4_End'							*/
/*----------------------------------------------------------------------*/
static void  tp4c_end (void)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_max_tpdu_len				*/
/* keyword 'Max_Tpdu_Length' 						*/
/*----------------------------------------------------------------------*/
static void tp4c_max_tpdu_len (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.max_tpdu_len = sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_max_rem_cdt				*/
/* keyword 'Max_Remote_Cdt'						*/
/*----------------------------------------------------------------------*/
static void tp4c_max_rem_cdt (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.max_rem_cdt = (ST_UCHAR) sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_loc_cdt					*/
/* keyword 'Local_Cdt'							*/
/*----------------------------------------------------------------------*/
static void tp4c_loc_cdt (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.loc_cdt = (ST_UCHAR) sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_max_num_conns				*/
/* keyword 'Max_Num_Connections'					*/
/*----------------------------------------------------------------------*/
static void tp4c_max_num_conns (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.max_num_conns = sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_max_spdu_outst				*/
/* keyword 'Max_Spdu_Outstanding'					*/
/*----------------------------------------------------------------------*/
static void tp4c_max_spdu_outst (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.max_spdu_outst = (ST_UCHAR) sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_window_time				*/
/* keyword 'Window_Time'							*/
/*----------------------------------------------------------------------*/
static void tp4c_window_time (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.window_time = sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_inact_time					*/
/* keyword 'Inactivity_Time' 						*/
/*----------------------------------------------------------------------*/
static void tp4c_inact_time (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.inact_time = sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_retrans_time				*/
/* keyword 'Retransmission_Time'					*/
/*----------------------------------------------------------------------*/
static void tp4c_retrans_time (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.retrans_time = sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_max_trans					*/
/* keyword 'Max_Transmissions'						*/
/*----------------------------------------------------------------------*/
static void tp4c_max_trans (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.max_trans = (ST_UCHAR) sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			tp4c_ak_delay					*/
/* keyword 'ak_delay'							*/
/*----------------------------------------------------------------------*/
static void tp4c_ak_delay (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP4_ENABLED)	/* Just ignore if disabled.		*/
    tp_cfg.ak_delay = (ST_UCHAR) sVal;
#endif	/* TP4_ENABLED	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			rfc1006_max_tpdu_len				*/
/* keyword 'RFC1006_Max_Tpdu_Len' 						*/
/*----------------------------------------------------------------------*/
static void rfc1006_max_tpdu_len (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP0_ENABLED)	/* Just ignore if not enabled.		*/
    tp0_cfg.max_tpdu_len = sVal;
#endif	/* defined (TP0_ENABLED)	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			rfc1006_max_num_conns				*/
/* keyword 'RFC1006_Max_Num_Conns'					*/
/*----------------------------------------------------------------------*/
static void rfc1006_max_num_conns (void)
  {
ST_UINT16	sVal;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_ushort (&sVal) == SD_SUCCESS)
    {
#if defined (TP0_ENABLED)	/* Just ignore if not enabled.		*/
    tp0_cfg.max_num_conns = sVal;
#endif	/* defined (TP0_ENABLED)	*/
    }
  else
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
/*			rfc1006_keepalive				*/
/* keyword 'rfc1006_keepalive'						*/
/*----------------------------------------------------------------------*/
static void rfc1006_keepalive (void)
  {
#if defined (TP0_ENABLED)	/* Just ignore if not enabled.		*/
  tp0_cfg.keepalive = SD_TRUE;
#endif	/* defined (TP0_ENABLED)	*/
  }

#endif	/* !defined (MCGW)	*/


/*======================================================================*/
/************************************************************************/
/*======================================================================*/
/*									*/
/*	Functions and keyword tables for parsing the Local and		*/
/*	Remote DIB sections from configuration file.			*/
/*									*/
/*======================================================================*/
/************************************************************************/
/*======================================================================*/


#define CFG_MAX_AR_NAME		64
#define CFG_MAX_AP_TITLE	80
#define CFG_MAX_AE_QUAL		32

typedef struct _tagTransDevInfo
  {
  DBL_LNK l;
  ST_CHAR name[CFG_MAX_AR_NAME +1];
  ST_CHAR apTitle[CFG_MAX_AP_TITLE +1];
  ST_CHAR aeQual[CFG_MAX_AE_QUAL +1];
  PRES_ADDR pres_addr;
  ST_CHAR ip_addr [MAX_IP_ADDR_LEN];	/* Overwrite pres_addr->nsap	*/
					/* with this if tp_type = TCP	*/
  } TRANS_DEV_INFO;

static void dibc_end (void);

static void dibc_begin_local (void);
static void dibc_begin_remote (void);
static void dibc_common_name (void);
static void dibc_ap_title  (void);
static void dibc_ae_qualifier (void);
static void dibc_psel (void);
static void dibc_ssel (void);
static void dibc_tsel (void);
static void dibc_nsap (void);
static void dibc_tp_type (void);
static void dibc_ip_addr (void);
#if defined (MCGW)
static void dibc_adlcaddr (void);
#endif			/* defined (MCGW)	*/
static void dibc_end_local (void);
static void dibc_end_remote (void);

static ST_RET parseObjId (MMS_OBJ_ID *o, ST_CHAR *buff);
static ST_VOID save_dib_table (TRANS_DEV_INFO *, DIB_ENTRY *);



static CFG_KW_EL dibc_kwtbl[] =
  {
    {"Begin_Local", 		dibc_begin_local,  NULL },
    {"Begin_Remote", 		dibc_begin_remote, NULL },
    {"%Dib_End",		dibc_end,	   NULL },
    { NULL, NULL, NULL}
  };

static CFG_KW_EL dibc_kwtbl_param[] =
  {
    {"Common_Name", 	dibc_common_name,  NULL },
    {"AP_Title", 	dibc_ap_title, 	  NULL },
    {"AE_Qualifier", 	dibc_ae_qualifier, NULL },
    {"Psel",	 	dibc_psel, 	  NULL },
    {"Ssel",	 	dibc_ssel, 	  NULL },
    {"Tsel",	 	dibc_tsel, 	  NULL },
    {"NSAP",	 	dibc_nsap, 	  NULL },
    {"Transport", 	dibc_tp_type, 	  NULL },
    {"IP_Addr",	 	dibc_ip_addr, 	  NULL },
#if defined (MCGW)
    {"AdlcAddr", 	dibc_adlcaddr, 	  NULL },
#endif	/* defined (MCGW)	*/
    {"End_Local", 	dibc_end_local,    NULL },
    {"End_Remote", 	dibc_end_remote,   NULL },
    { NULL, NULL, NULL}	   
  };

static TRANS_DEV_INFO *localTransList;
static TRANS_DEV_INFO *remoteTransList;
static TRANS_DEV_INFO *currTransEl;

static ST_BOOLEAN rem_section_flag;

/*----------------------------------------------------------------------*/
/*			dibc_begin					*/
/* keyword '%Dib_Begin'							*/
/*----------------------------------------------------------------------*/
static void  dibc_begin (void)
  {
  /* CRITICAL: Always start with empty temporary DIB Table linked lists	*/
  /* (may be reconfiguring).						*/
  localTransList = NULL;
  remoteTransList = NULL;

  if (cfg_tbl_push (dibc_kwtbl) != SD_SUCCESS) 
    cfg_set_config_err ();
  }

/*----------------------------------------------------------------------*/
/*			dibc_end					*/
/* keyword '%Dib_End'							*/
/*----------------------------------------------------------------------*/
static void  dibc_end (void)
  {
  if (cfg_tbl_pop (1) != SD_SUCCESS)
    cfg_set_config_err ();

  cfg_set_endfile_ok ();	/* this is the end of configuration	*/
  }

/*----------------------------------------------------------------------*/
/*			dibc_begin_local				*/
/* keyword 'Begin_Local'						*/
/*----------------------------------------------------------------------*/

static void dibc_begin_local (void)
  {
  if (!init_dirser_called)
    {			/* Don't alloc if reconfiguring.		*/
    currTransEl = (TRANS_DEV_INFO *) chk_calloc (1, sizeof(TRANS_DEV_INFO));
    currTransEl->pres_addr.tp_type = TP_TYPE_TP4;	/* Default=TP4	*/
    list_add_last ((void**) &localTransList, currTransEl);
    }

  cfg_tbl_push (dibc_kwtbl_param);
  }
		    
/*----------------------------------------------------------------------*/
/*			dibc_begin_remote				*/
/* keyword 'Begin_Remote'						*/
/*----------------------------------------------------------------------*/
static void dibc_begin_remote (void)
  {
  currTransEl = (TRANS_DEV_INFO *) chk_calloc (1, sizeof(TRANS_DEV_INFO));
  currTransEl->pres_addr.tp_type = TP_TYPE_TP4;	/* Default=TP4	*/
  list_add_last ((void**) &remoteTransList, currTransEl);
  cfg_tbl_push (dibc_kwtbl_param);
  rem_section_flag = SD_TRUE;
  }

/*----------------------------------------------------------------------*/
/*	Processing of keywords found in one of sub-section:		*/
/*		Local_Begin ... Local_End				*/
/*		Remote_Begin ... Remote_End				*/
/*----------------------------------------------------------------------*/

static void dibc_common_name (void)
  {
ST_CHAR *p;

  if (init_dirser_called && rem_section_flag == SD_FALSE)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  p = cfg_get_string_ptr ();
  if (!p)
    cfg_set_config_err();
  else if (strlen (p) > CFG_MAX_AR_NAME)
    cfg_set_config_err();
  else
    strcpy (currTransEl->name, p);
  }

/*----------------------------------------------------------------------*/
static void dibc_ap_title  (void)
  {
ST_CHAR *p;

  if (init_dirser_called && rem_section_flag == SD_FALSE)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  p = cfg_get_string_ptr ();
  if (!p)
    cfg_set_config_err();
  else if (strlen (p) > CFG_MAX_AP_TITLE)
    cfg_set_config_err();
  else
    strcpy (currTransEl->apTitle, p);
  }

/*----------------------------------------------------------------------*/
static void dibc_ae_qualifier (void)
  {
ST_CHAR *p;

  if (init_dirser_called && rem_section_flag == SD_FALSE)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  p = cfg_get_string_ptr ();
  if (!p)
    cfg_set_config_err();
  else if (strlen (p) > CFG_MAX_AE_QUAL)
    cfg_set_config_err();
  else
    strcpy (currTransEl->aeQual, p);
  }

/*----------------------------------------------------------------------*/
static void dibc_psel (void)
  {
  if (init_dirser_called && rem_section_flag == SD_FALSE)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_octet_string (currTransEl->pres_addr.psel,
          &currTransEl->pres_addr.psel_len, MAX_PSEL_LEN) != SD_SUCCESS)
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
static void dibc_ssel (void)
  {
  if (init_dirser_called && rem_section_flag == SD_FALSE)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_octet_string (currTransEl->pres_addr.ssel,
          &currTransEl->pres_addr.ssel_len, MAX_SSEL_LEN) != SD_SUCCESS)
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
static void dibc_tsel (void)
  {
  if (init_dirser_called && rem_section_flag == SD_FALSE)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  if (cfg_get_octet_string (currTransEl->pres_addr.tsel,
          &currTransEl->pres_addr.tsel_len, MAX_TSEL_LEN) != SD_SUCCESS)
    cfg_set_config_err();
  }

/*----------------------------------------------------------------------*/
static void dibc_nsap (void)
  {
  if (rem_section_flag)
    {
    /* NSAP only in remote DIB is valid */
    /* NOTE: array size is MAX_IP_ADDR_LEN. Must be >= CLNP_MAX_LEN_NSAP.*/
    if (cfg_get_octet_string (currTransEl->pres_addr.netAddr.nsap,
            &currTransEl->pres_addr.nsap_len, CLNP_MAX_LEN_NSAP) != SD_SUCCESS)
      cfg_set_config_err();
    }
  else
    {
    cfg_set_config_err();
    }
  }

/*----------------------------------------------------------------------*/
static void dibc_tp_type (void)
  {
ST_CHAR *p;

  p = cfg_get_string_ptr ();
  if (!p)
    cfg_set_config_err();
  else
    {
    if (strcmp (p, "TCP") == 0)
      currTransEl->pres_addr.tp_type = TP_TYPE_TCP;
    else if (strcmp (p, "TP4") == 0)
      currTransEl->pres_addr.tp_type = TP_TYPE_TP4;
    else if (strcmp (p, "TPX") == 0)
      currTransEl->pres_addr.tp_type = TP_TYPE_TPX;	/* Both. Only for Local*/
    else
      cfg_set_config_err();		/* too long to fit.	*/
    }
  }

/*----------------------------------------------------------------------*/
static void dibc_ip_addr (void)
  {
ST_CHAR *p;

  if (rem_section_flag)
    {
    /* IP Addr only valid in remote DIB entry */
    p = cfg_get_string_ptr ();
    if (!p)
      cfg_set_config_err();
    else
      {
      if (strlen (p) < MAX_IP_ADDR_LEN)	/* must fit with NULL terminator*/
        strcpy (currTransEl->ip_addr, p);
      else
        cfg_set_config_err();		/* too long to fit.	*/
      }
    }
  else
    {
    cfg_set_config_err();
    }
  }

#if defined (MCGW)
/*----------------------------------------------------------------------*/
static void dibc_adlcaddr (void)
  {
ST_CHAR *p;
ST_LONG longval;

  if (init_dirser_called)
    {
    cfg_get_string_ptr ();	/* Get rest of line and ignore it.	*/
    return;			/* Don't allow reconfiguring.		*/
    }

  p = cfg_get_string_ptr ();
  if (!p)
    cfg_set_config_err();
  else
    {
    longval = atol (p);
    /* Make sure it's a legal ST_UINT16 before casting it.		*/
    if (longval < 0 || longval > 0xffff)
      cfg_set_config_err();
    else
      currTransEl->pres_addr.adlcAddr = (ST_UINT16) longval;
    }
  }
#endif	/* defined (MCGW)	*/

/*----------------------------------------------------------------------*/
/*			dibc_end_local					*/
/*  keyword 'Local_End'							*/
/*----------------------------------------------------------------------*/
static void dibc_end_local (void)
  {
  cfg_tbl_pop (1);
  }

/*----------------------------------------------------------------------*/
/*			dibc_end_remote					*/
/*  keyword 'Remote_End'						*/
/*----------------------------------------------------------------------*/
static void dibc_end_remote (void)
  {
  if (currTransEl->pres_addr.tp_type == TP_TYPE_TCP)
    {
    if ((currTransEl->pres_addr.netAddr.ip = tp0_convert_ip (currTransEl->ip_addr))
        == htonl(INADDR_NONE))
      cfg_set_config_err();		/* IP Addr invalid.	*/
    }
    
  cfg_tbl_pop (1);
  rem_section_flag = SD_FALSE;
  }


/************************************************************************/
/*			parseObjId					*/
/*----------------------------------------------------------------------*/
/* Parses components of the 'ApTitle'.					*/
/************************************************************************/

static ST_RET parseObjId (MMS_OBJ_ID *objId, ST_CHAR *buff)
  {
char    *numStr;

  /* Found keyword 'ApTitle', read the components                       */
  objId->num_comps = 0;
  numStr = strtok (buff, " \t");
  while (numStr && strlen (numStr) > 0)
    {
    if (objId->num_comps >= MAX_OBJID_COMPONENTS)
      {
      return (SD_FAILURE);
      }
    objId->comps[objId->num_comps] = (ST_INT16) atoi (numStr);
    numStr = strtok (NULL, " \t");
    ++objId->num_comps;
    }
  if (objId->num_comps == 0)	/* couldn't parse anything	*/
    return (SD_FAILURE);
  return (SD_SUCCESS);
  }
  
/************************************************************************/
/*                       save_loc_rem_dib_tables				*/
/*----------------------------------------------------------------------*/
/* This function will initialize local and remote dib entries tables.	*/
/*									*/
/* Parameters:								*/
/*	void			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful.		*/
/*	SD_FAILURE (1)		otherwise				*/
/* NOTES:  Does not allow reconfiguration of loc_dib_table.		*/
/*         Allows reconfiguration of rem_dib_table, but only after	*/
/*         freeing the old table.					*/
/************************************************************************/

static ST_RET save_loc_rem_dib_tables (void)
  {
ST_RET ret;
ST_INT j;

  ret = SD_SUCCESS;

  if (!init_dirser_called)		/* Don't allow reconfiguring.	*/
    {
    num_loc_dib_entries = list_get_sizeof (localTransList);
    if (num_loc_dib_entries)
      {
      loc_dib_table = (DIB_ENTRY *) chk_calloc (num_loc_dib_entries, sizeof (DIB_ENTRY));
      save_dib_table (localTransList, loc_dib_table);
      }
    }

  /* If Remote DIB table already configured, free old before allocating new.*/
  if (init_dirser_called)
    {
    for (j = 0; j < num_rem_dib_entries;  j++)
      chk_free (rem_dib_table [j].name);
    chk_free (rem_dib_table);
    }

  num_rem_dib_entries = list_get_sizeof (remoteTransList);
  if (num_rem_dib_entries)
    {
    rem_dib_table = (DIB_ENTRY *) chk_calloc (num_rem_dib_entries, sizeof (DIB_ENTRY));
    save_dib_table (remoteTransList, rem_dib_table);
    }
  return (ret);
  }


/************************************************************************/
/*                       save_dib_table					*/
/*----------------------------------------------------------------------*/
/* This function copies the temporary DIB Table to the permanent	*/
/* DIB Table.  It may be used for the Local as well as the Remote DIB	*/
/* Tables.								*/
/*									*/
/* Parameters:								*/
/*	TRANS_DEV_INFO *	temporary table (linked list of structs)*/
/*	DIB_ENTRY *		permanent table (array of structs)	*/
/*									*/
/************************************************************************/

static ST_VOID save_dib_table (TRANS_DEV_INFO *TransList, DIB_ENTRY *de)
  {
TRANS_DEV_INFO *toFree;
TRANS_DEV_INFO *d;

  d = TransList;	/* Point to first in linked list	*/
  while (d)
    {
    de->name = (ST_CHAR *) chk_malloc (strlen(d->name) + 1);
    strcpy (de->name, d->name);
    if (strlen (d->apTitle))
      {
      if (parseObjId (&de->AP_title, d->apTitle))
        {
        ACSELOG_ERR1 ("Invalid AP Title '%s'. Ignored.", d->apTitle);
        }
      else
        de->AP_title_pres = SD_TRUE;	/* defaults to FALSE	*/
      }

    if (strlen (d->aeQual))
      {
      de->AE_qual_pres = SD_TRUE;	/* defaults to FALSE	*/
      de->AE_qual = atol (d->aeQual);
      }

    de->AP_inv_id_pres = SD_FALSE;
    de->AE_inv_id_pres = SD_FALSE;

    memcpy (&de->pres_addr, &d->pres_addr, sizeof (PRES_ADDR));

    toFree = d;
    d = (TRANS_DEV_INFO *) list_get_next (TransList, d);	/* Point to next TRANS_DEV_INFO	*/
    chk_free (toFree);
    ++de;				/* Point to next DIB_ENTRY	*/
    }		/* End "while"	*/

  TransList = NULL;	/* CRITICAL: clean up in case reconfigure later.*/
  }
