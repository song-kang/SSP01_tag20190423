/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1993, 1994, 1995, All Rights Reserved			*/
/*									*/
/* MODULE NAME : rs_cfg.c						*/
/* PRODUCT(S)  : Reduced Stack 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	DIB handling functions.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*                                                                      */
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 05/03/05  JRB     02    Replace suicacse with rs_a.h.		*/
/* 08/13/98  JRB     01    Lint cleanup.				*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 01/09/97  RKR     07    Used DEBUG_SISCO to compile out ref sLogCtrl */
/* 12/17/96  JRB     06    Imposed DEBUG_SISCO				*/
/* 11/19/96  MDE     05    Now handle 0 dib entries w/o mem error	*/
/* 09/23/96  KCR     04    Added s_reread_remote_ar_info()              */
/* 05/29/96  MDE     03    Handle change in DIB_ENTRY (name now ptr)	*/
/* 05/17/96  MDE     02    Fixed 'table push' problem			*/
/* 04/09/96  MDE     01    Created 					*/
/************************************************************************/

#ifdef DEBUG_SISCO
static char *thisFileName = __FILE__;
#endif

#include "glbtypes.h"
#include "gen_list.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "rs_a.h"
#include "cfg_util.h"
#include "cfglog.h"
#include "tp4api.h"
#include "rs_defs.h"

#undef local


/************************************************************************/
/************************************************************************/
/* Functions & Keyword tables                                           */
/************************************************************************/
static void cfg_begin_master_addr (ST_VOID);
static void cfg_begin_local (ST_VOID);
static void cfg_begin_remote (ST_VOID);
static void cfg_common_name (ST_VOID);
static void cfg_ap_title  (ST_VOID);
static void cfg_ae_qualifier (ST_VOID);
static void cfg_adlcAddr (ST_VOID);
static void cfg_addr_base (ST_VOID);
static void cfg_addr_num (ST_VOID);
static void cfg_end_local (ST_VOID);
static void cfg_end_remote (ST_VOID);
static void cfg_end_master_addr (ST_VOID);
static ST_RET parseObjId (MMS_OBJ_ID *o, ST_CHAR *buff);


/* Root Keyword Table - 2 sections                                      */
struct cfg_kw_el rs_base_kwtbl[] =
  {
    {"Begin_Local", 		cfg_begin_local, NULL },
    {"Begin_Remote", 		cfg_begin_remote, NULL },
    {"Begin_Master_Addr",	cfg_begin_master_addr, NULL },
    { NULL, NULL, NULL}
  };

struct cfg_kw_el rs_addr_kwtbl[] =
  {
    {"Base", 			cfg_addr_base,  NULL },
    {"Number",	 		cfg_addr_num,  NULL },
    {"End_Master_Addr", 	cfg_end_remote,   NULL },
    { NULL, NULL, NULL}	   
  };

/* Looking for Device parameters                                      	*/
struct cfg_kw_el rs_internal_kwtbl[] =
  {
    {"Common_Name", 	cfg_common_name,  NULL },
    {"AP_Title", 	cfg_ap_title, 	  NULL },
    {"AE_Qualifier", 	cfg_ae_qualifier, NULL },
    {"AdlcAddr", 	cfg_adlcAddr, 	  NULL },
    {"End_Local", 	cfg_end_local,    NULL },
    {"End_Remote", 	cfg_end_remote,   NULL },
    { NULL, NULL, NULL}	   
  };

#define CFG_MAX_AR_NAME		64
#define CFG_MAX_AP_TITLE	80
#define CFG_MAX_AE_QUAL		32

typedef struct _tagTransDevInfo
  {
  DBL_LNK l;
  ST_CHAR name[CFG_MAX_AR_NAME +1];
  ST_CHAR apTitle[CFG_MAX_AP_TITLE +1];
  ST_CHAR aeQual[CFG_MAX_AE_QUAL +1];
  ST_UINT16 adlcAddr;
  } TRANS_DEV_INFO;

static TRANS_DEV_INFO *localTransList;
static TRANS_DEV_INFO *remoteTransList;
static TRANS_DEV_INFO *currTransEl;

/************************************************************************/
/*                       read_rs_config					*/
/************************************************************************/

ST_RET init_dirser (ST_VOID)
  {
TRANS_DEV_INFO *d;
TRANS_DEV_INFO *toFree;
ST_RET ret;
DIB_ENTRY *de;
ST_INT i;
MMS_OBJ_ID objId;

  ret = cfg_process_file ("rsdib.cfg", rs_base_kwtbl);
  if (ret == SD_SUCCESS)
    {
    num_loc_dib_entries = list_get_sizeof (localTransList);
    if (num_loc_dib_entries)
      loc_dib_table = (DIB_ENTRY *) chk_calloc (num_loc_dib_entries, sizeof (DIB_ENTRY));

    d = localTransList;
    de = loc_dib_table;
    while (d)
      {
      de->name = (ST_CHAR *) chk_calloc (1, strlen(d->name) + 1);
      strcpy (de->name, d->name);
      de->local = SD_TRUE;
      de->AP_title_pres = SD_TRUE;
      parseObjId (&objId, d->apTitle);

      de->AP_title.num_comps = objId.num_comps;
      for (i = 0; i < objId.num_comps; ++i)
        de->AP_title.comps[i] = objId.comps[i];
      de->AE_qual_pres = SD_TRUE;
      de->AE_qual = atol (d->aeQual);

      de->AP_inv_id_pres = SD_FALSE;
      de->AE_inv_id_pres = SD_FALSE;
      de->adlcAddr = d->adlcAddr;
      
      toFree = d;
      d = (TRANS_DEV_INFO *) list_get_next (localTransList, d);
      chk_free (toFree);
      ++de;
      }

    num_rem_dib_entries = list_get_sizeof (remoteTransList);
    if (num_rem_dib_entries)
      rem_dib_table = (DIB_ENTRY *) chk_calloc (num_rem_dib_entries, sizeof (DIB_ENTRY));

    d = remoteTransList;
    de = rem_dib_table;
    while (d)
      {
      de->name = (ST_CHAR *) chk_calloc (1, strlen(d->name) + 1);
      strcpy (de->name, d->name);
      de->local = SD_FALSE;
      de->AP_title_pres = SD_TRUE;
      parseObjId (&objId, d->apTitle);

      de->AP_title.num_comps = objId.num_comps;
      for (i = 0; i < objId.num_comps; ++i)
        de->AP_title.comps[i] = objId.comps[i];
      de->AE_qual_pres = SD_TRUE;
      de->AE_qual = atol (d->aeQual);

      de->AP_inv_id_pres = SD_FALSE;
      de->AE_inv_id_pres = SD_FALSE;
      de->adlcAddr = d->adlcAddr;
      
      toFree = d;
      d = (TRANS_DEV_INFO *) list_get_next (remoteTransList, d);
      chk_free (toFree);
      ++de;
      }
    }
  else
    {
    /* Error encoutered in configuration file, reparse the file and	*/
    /* output configuration error messages to a log file		*/

#if defined(DEBUG_SISCO)
    if (!cfg_sLogCtrl)	/* if cfg slog not set, use SUIC		*/
      cfg_sLogCtrl = sLogCtrl;

    if (cfg_sLogCtrl)	/* If this is a useful thing to do ... 		*/
      {
      cfg_log_mask = CFG_LOG_ERR | CFG_LOG_FLOW;
      cfg_process_file ("rsdib.cfg", rs_base_kwtbl);
      cfg_log_mask = CFG_LOG_ERR;
      }
#endif
    }
  return (ret);
  }

/************************************************************************/
/*			s_reread_remote_ar_info				*/
/* This function builds DIB table					*/
/************************************************************************/

ST_RET s_reread_remote_ar_info (ST_VOID)
  {	     
TRANS_DEV_INFO *d;
TRANS_DEV_INFO *toFree;
ST_RET ret;
DIB_ENTRY *de;
ST_INT i;
MMS_OBJ_ID objId;


  localTransList = NULL;
  remoteTransList = NULL;
  currTransEl = NULL;

  if (ret = cfg_process_file ("rsdib.cfg", rs_base_kwtbl))
    return (ret);

  /* Free the original table */
  if (rem_dib_table)
    {
    for (i = 0; i < num_rem_dib_entries; i++)
      chk_free (rem_dib_table[i].name);
    chk_free (rem_dib_table);
    }

  /* free new local information */
  d = localTransList;
  while (d)
    {
    toFree = d;
    d = (TRANS_DEV_INFO *) list_get_next (localTransList, d);
    chk_free (toFree);
    }

    /* reallocate the remote DIB entries */
  num_rem_dib_entries = list_get_sizeof (remoteTransList);
  rem_dib_table = (DIB_ENTRY *) chk_calloc (num_rem_dib_entries, sizeof (DIB_ENTRY));

  d = remoteTransList;
  de = rem_dib_table;
  while (d)
    {
    de->name = (ST_CHAR *) chk_calloc (1, strlen(d->name) + 1);
    strcpy (de->name, d->name);
    de->local = SD_FALSE;
    de->AP_title_pres = SD_TRUE;
    parseObjId (&objId, d->apTitle);

    de->AP_title.num_comps = objId.num_comps;
    for (i = 0; i < objId.num_comps; ++i)
      de->AP_title.comps[i] = objId.comps[i];
    de->AE_qual_pres = SD_TRUE;
    de->AE_qual = atol (d->aeQual);

    de->AP_inv_id_pres = SD_FALSE;
    de->AE_inv_id_pres = SD_FALSE;
    de->adlcAddr = d->adlcAddr;
    
    toFree = d;
    d = (TRANS_DEV_INFO *) list_get_next (remoteTransList, d);
    chk_free (toFree);
    ++de;
    }
  return (SD_SUCCESS);
}



/************************************************************************/
/************************************************************************/
/* Configuration state functions                                        */
/************************************************************************/
static void cfg_begin_master_addr (ST_VOID)
  {
  cfg_tbl_push (rs_addr_kwtbl);
  }

/************************************************************************/

static void cfg_begin_local (ST_VOID)
  {
  currTransEl = (TRANS_DEV_INFO *) chk_calloc (1, sizeof(TRANS_DEV_INFO));
  list_add_last ((ST_VOID**) &localTransList, currTransEl);
  config_eof = SD_FAILURE;
  cfg_tbl_push (rs_internal_kwtbl);
  }
		    
/************************************************************************/

static void cfg_begin_remote (ST_VOID)
  {
  currTransEl = (TRANS_DEV_INFO *) chk_calloc (1, sizeof(TRANS_DEV_INFO));
  list_add_last ((ST_VOID**) &remoteTransList, currTransEl);
  config_eof = SD_FAILURE;
  cfg_tbl_push (rs_internal_kwtbl);
  }

/************************************************************************/

static void cfg_addr_base (ST_VOID)
  {
  cfg_get_hex_ushort (&rsBaseSrcAdlcAddr);
  }

/************************************************************************/

static void cfg_addr_num (ST_VOID)
  {
  cfg_get_hex_ushort (&rsNumSrcAdlcAddr);
  }


/************************************************************************/
static void cfg_common_name (ST_VOID)
  {
ST_CHAR *p;

  p = cfg_get_string_ptr ();
  if (!p)
    cfg_set_config_err();
  else if (strlen (p) > CFG_MAX_AR_NAME)
    cfg_set_config_err();
  else
    strcpy (currTransEl->name, p);
  }

/************************************************************************/
static void cfg_ap_title  (ST_VOID)
  {
ST_CHAR *p;

  p = cfg_get_string_ptr ();
  if (!p)
    cfg_set_config_err();
  else if (strlen (p) > CFG_MAX_AP_TITLE)
    cfg_set_config_err();
  else
    strcpy (currTransEl->apTitle, p);
  }

/************************************************************************/
static void cfg_ae_qualifier (ST_VOID)
  {
ST_CHAR *p;

  p = cfg_get_string_ptr ();
  if (!p)
    cfg_set_config_err();
  else if (strlen (p) > CFG_MAX_AE_QUAL)
    cfg_set_config_err();
  else
    strcpy (currTransEl->aeQual, p);
  }

/************************************************************************/
static void cfg_adlcAddr (ST_VOID)
  {
  cfg_get_ushort (&currTransEl->adlcAddr);
  }

/************************************************************************/
static void cfg_end_local (ST_VOID)
  {
  config_eof = SD_SUCCESS;
  cfg_tbl_pop (1);
  }

/************************************************************************/
static void cfg_end_remote (ST_VOID)
  {
  config_eof = SD_SUCCESS;
  cfg_tbl_pop (1);
  }


/************************************************************************/

static ST_RET parseObjId (MMS_OBJ_ID *o, ST_CHAR *buff)
  {
int     idx;
char    *numStr;

  /* Found keyword 'ApTitle', read the components                       */
  idx = 0;
  numStr = strtok (buff, " \t");
  while (numStr && strlen (numStr) > 0)
    {
    if (idx >= MAX_OBJID_COMPONENTS)
      {
      return (SD_FAILURE);
      break;
      }
    o->comps[idx] = (ST_INT16) atoi (numStr);
    numStr = strtok (NULL, " \t");
    ++idx;
    }
  if (idx > 0 && idx < MAX_OBJID_COMPONENTS)
    {
    o->num_comps = idx;
    }
  return (SD_SUCCESS);
  }

