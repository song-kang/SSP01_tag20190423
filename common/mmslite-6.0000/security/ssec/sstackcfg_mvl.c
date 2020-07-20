/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2010-2010, All Rights Reserved					*/
/*									*/
/* MODULE NAME : sStackCfg_mvl.c					*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION : SISCO Stack Configuration (SSC)			*/
/*									*/
/*	Functions to read the SISCO Stack Configuration and fill in 	*/
/*	parameters used by the MVL layer.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			sscConfigMvl					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments    			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/21/10  JRB    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "slog.h"
#include "tp4api.h"
#include "mvl_acse.h"
#include "mvl_defs.h"
#include "mvl_log.h"

#include "acse2.h"
#include "sstackcfg.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/
#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif
/************************************************************************/
/*			sscConfigMvl					*/
/* Read SISCO Stack Configuration and fill in MVL_CFG_INFO structure.	*/
/* Initialize "tp0_cfg.max_tpdu_len" (global).				*/
/* Save ALL configured Local AR Names in "cfg_info->bind_ctrl" array.	*/
/* Save first Local AR Name in "cfg_info->local_ar_name" (use as default).*/
/* WARNING: do not access "sscStackCfg" after sscReleaseStackCfg called.*/
/************************************************************************/
ST_RET sscConfigMvl (
	ST_CHAR *appName,	/* must match appName in configuration.	*/
	MVL_CFG_INFO *cfg_info)	/* caller structure to fill in		*/
{
ST_RET retcode;
SSC_APP_AR_NAME *arname;	/* current entry being processed	*/
ST_INT bc_idx;			/* index to current bind_ctrl entry	*/
SSC_STACK_CFG *sscStackCfg;

  memset (cfg_info, 0, sizeof(MVL_CFG_INFO));	/* CRITICAL: start clean*/
  cfg_info->use_bind_ctrl = SD_TRUE;	/* always set this flag		*/
  /*cfg_info->num_bind_ctrl = 0;*/	/* memset above does this	*/

  /* Read the SISCO Stack Configuration.				*/
  /* This fills in a global structure. Must call "sscAccessStackCfg"	*/
  /* and "sscReleaseStackCfg" to access the global.			*/
  /* NOTE: "appName" arg must match a name read from config (stored	*/
  /*       in "appName" in one entry of sscStackCfg->appNetworkList).	*/
  retcode = sscConfigAppNetwork (appName);
  if (retcode != SD_SUCCESS)
    {
    MVL_LOG_ERR0("SISCO Stack Configuration failed");
    return (retcode);
    }
  /* Copy parameters we need from Stack Config struct.			*/
  sscStackCfg = sscAccessStackCfg ();	/* protect access to global	*/
  /* Save TP0 parameters in global struct.	*/
  tp0_cfg.max_tpdu_len = sscStackCfg->appNetwork->leantProfile->tcpMaxTpduLen;

  /* Loop through AR Names once just to count Local AR Names.	*/
  for (arname = sscStackCfg->appNetwork->appArNameList;
       arname != NULL;
       arname = list_get_next (sscStackCfg->appNetwork->appArNameList, arname))
    {  
    if (arname->role == SSC_AR_NAME_ROLE_LOCAL)	/* local	*/
      {
      cfg_info->num_bind_ctrl++;
      }
    }
 
  cfg_info->max_bind_ctrl = cfg_info->num_bind_ctrl;	/* max same as count*/

  /* Allocate array of NET_BIND_CTRL structures (freed by "mvl_end_acse").*/
  /* DEBUG: If you need to add more bind_ctrl later, you must increase	*/
  /*        "max_bind_ctrl" before this to allocate extra space.	*/
  cfg_info->bind_ctrl = (NET_BIND_CTRL *) chk_calloc (1,
                        cfg_info->max_bind_ctrl * sizeof(NET_BIND_CTRL));

  /* Loop again to copy each Local AR Name to one of the NET_BIND_CTRL.	*/
  bc_idx = 0;
  for (arname = sscStackCfg->appNetwork->appArNameList;
       arname != NULL;
       arname = list_get_next (sscStackCfg->appNetwork->appArNameList, arname))
    {  
    if (arname->role == SSC_AR_NAME_ROLE_LOCAL)	/* local	*/
      {
      /* Save EVERY Local AR Name in cfg_info->bind_ctrl array.		*/
      strcpy (cfg_info->bind_ctrl[bc_idx].ar_name, arname->arName);
      /* Save first (default) Local AR Name in cfg_info->local_ar_name.	*/
      /* DEBUG: This is redundant, but many old apps use this.		*/
      if (cfg_info->local_ar_name[0] == '\0')
        strcpy (cfg_info->local_ar_name, arname->arName);
      bc_idx++;
      }
    }

  /* Save other parameters in "cfg_info".	*/
  cfg_info->max_msg_size = sscStackCfg->appNetwork->maxMmsPduLength;
  cfg_info->num_calling  = sscStackCfg->appNetwork->maxCallingConns;
  cfg_info->num_called   = sscStackCfg->appNetwork->maxCalledConns;

  sscReleaseStackCfg ();		/* protect access to global	*/
  return (SD_SUCCESS);
}

