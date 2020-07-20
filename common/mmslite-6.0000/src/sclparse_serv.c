/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2012-2012 All Rights Reserved					*/
/*									*/
/* MODULE NAME : sclparse_serv.c					*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to parse just the "Services" element of an SCL file.	*/
/*	Called from sclparse.c						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			scl_services_init				*/
/*			Services_SEFun					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who           Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/11/12  JRB	   Created.					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "sx_defs.h"
#include "sx_log.h"
#include "str_util.h"
#include "slog.h"
#include "scl.h"	/* SCL file processing structs & functions	*/

/************************************************************************/
/*			scl_services_init				*/
/* Initialize SCL_SERVICES structure.					*/
/************************************************************************/
ST_VOID scl_services_init (SCL_SERVICES *scl_services)
  {
  /*NOTE: This memset initializes all boolean members to 0 (SD_FALSE).	*/
  memset (scl_services, 0, sizeof (SCL_SERVICES));
  strcpy (scl_services->ReportSettings.cbName, "Fix");
  strcpy (scl_services->ReportSettings.datSet, "Fix");
  strcpy (scl_services->ReportSettings.rptID, "Fix");
  strcpy (scl_services->ReportSettings.optFields, "Fix");
  strcpy (scl_services->ReportSettings.bufTime, "Fix");
  strcpy (scl_services->ReportSettings.trgOps, "Fix");
  strcpy (scl_services->ReportSettings.intgPd, "Fix");
#if 0	/* memset above initializes these	*/
  scl_services->ReportSettings.resvTms = SD_FALSE;
  scl_services->ReportSettings.owner = SD_FALSE;
#endif
  }

/************************************************************************/
/*			_GetDirectory_SFun				*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _GetDirectory_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.GetDirectory = SD_TRUE;
  }
/************************************************************************/
/*			_GetDataObjectDefinition_SFun			*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _GetDataObjectDefinition_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.GetDataObjectDefinition = SD_TRUE;
  }
/************************************************************************/
/*			_DataObjectDirectory_SFun			*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _DataObjectDirectory_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.DataObjectDirectory = SD_TRUE;
  }
/************************************************************************/
/*			_GetDataSetValue_SFun				*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _GetDataSetValue_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.GetDataSetValue = SD_TRUE;
  }
/************************************************************************/
/*			_SetDataSetValue_SFun				*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _SetDataSetValue_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.SetDataSetValue = SD_TRUE;
  }
/************************************************************************/
/*			_DataSetDirectory_SFun				*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _DataSetDirectory_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.DataSetDirectory = SD_TRUE;
  }
/************************************************************************/
/*			_ReadWrite_SFun					*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _ReadWrite_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.ReadWrite = SD_TRUE;
  }
/************************************************************************/
/*			_TimerActivatedControl_SFun			*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _TimerActivatedControl_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.TimerActivatedControl = SD_TRUE;
  }
/************************************************************************/
/*			_GetCBValues_SFun				*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _GetCBValues_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.GetCBValues = SD_TRUE;
  }
/************************************************************************/
/*			_GSEDir_SFun					*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _GSEDir_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.GSEDir = SD_TRUE;
  }
/************************************************************************/
/*			_FileHandling_SFun				*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _FileHandling_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.FileHandling = SD_TRUE;
  }
/************************************************************************/
/*			_ConfLdName_SFun				*/
/* Just set boolean flag if this is found.				*/
/************************************************************************/
static ST_VOID _ConfLdName_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclDecCtrl->scl_services.ConfLdName = SD_TRUE;
  }

/************************************************************************/
/*			_ConfLogControl_SFun				*/
/* Fill in structure.							*/
/************************************************************************/
static ST_VOID _ConfLogControl_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
SCL_SERVICE_WITH_MAX *service_with_max;
  
  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  /* Point to right struct and fill it in.	*/
  service_with_max = &sclDecCtrl->scl_services.ConfLogControl;
  service_with_max->enabled = SD_TRUE;
  ret = scl_get_uint_attr (sxDecCtrl, "max", &service_with_max->max, SCL_ATTR_REQUIRED);
  if (ret)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  }
/************************************************************************/
/*			_GOOSE_SFun					*/
/* Fill in structure.							*/
/************************************************************************/
static ST_VOID _GOOSE_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
SCL_SERVICE_WITH_MAX *service_with_max;
  
  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  /* Point to right struct and fill it in.	*/
  service_with_max = &sclDecCtrl->scl_services.GOOSE;
  service_with_max->enabled = SD_TRUE;
  ret = scl_get_uint_attr (sxDecCtrl, "max", &service_with_max->max, SCL_ATTR_REQUIRED);
  if (ret)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  }
/************************************************************************/
/*			_GSSE_SFun					*/
/* Fill in structure.							*/
/************************************************************************/
static ST_VOID _GSSE_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
SCL_SERVICE_WITH_MAX *service_with_max;
  
  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  /* Point to right struct and fill it in.	*/
  service_with_max = &sclDecCtrl->scl_services.GSSE;
  service_with_max->enabled = SD_TRUE;
  ret = scl_get_uint_attr (sxDecCtrl, "max", &service_with_max->max, SCL_ATTR_REQUIRED);
  if (ret)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  }
/************************************************************************/
/*			_SMVsc_SFun					*/
/* Fill in structure.							*/
/************************************************************************/
static ST_VOID _SMVsc_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
SCL_SERVICE_WITH_MAX *service_with_max;
  
  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  /* Point to right struct and fill it in.	*/
  service_with_max = &sclDecCtrl->scl_services.SMVsc;
  service_with_max->enabled = SD_TRUE;
  ret = scl_get_uint_attr (sxDecCtrl, "max", &service_with_max->max, SCL_ATTR_REQUIRED);
  if (ret)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  }
/************************************************************************/
/*			_SupSubscription_SFun				*/
/* Fill in structure.							*/
/************************************************************************/
static ST_VOID _SupSubscription_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
SCL_SERVICE_WITH_MAX *service_with_max;
  
  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  /* Point to right struct and fill it in.	*/
  service_with_max = &sclDecCtrl->scl_services.SupSubscription;
  service_with_max->enabled = SD_TRUE;
  ret = scl_get_uint_attr (sxDecCtrl, "max", &service_with_max->max, SCL_ATTR_REQUIRED);
  if (ret)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  }
/************************************************************************/
/*			_ConfSigRef_SFun				*/
/* Fill in structure.							*/
/************************************************************************/
static ST_VOID _ConfSigRef_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
SCL_SERVICE_WITH_MAX *service_with_max;
  
  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  /* Point to right struct and fill it in.	*/
  service_with_max = &sclDecCtrl->scl_services.ConfSigRef;
  service_with_max->enabled = SD_TRUE;
  ret = scl_get_uint_attr (sxDecCtrl, "max", &service_with_max->max, SCL_ATTR_REQUIRED);
  if (ret)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  }
/************************************************************************/
/*			_ReportSettings_SFun				*/
/* CRITICAL: default values set earlier by scl_services_init.		*/
/************************************************************************/
static ST_VOID _ReportSettings_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
SCL_REPORTSETTINGS *ReportSettings;
ST_CHAR *str;	/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  ReportSettings = &sclDecCtrl->scl_services.ReportSettings;

  /* start optional attributes */
  /* These must be "Dyn", "Conf", or "Fix", so we allow max of 4 characters.*/
  ret = scl_get_attr_copy (sxDecCtrl, "cbName", ReportSettings->cbName, 4, SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "datSet", ReportSettings->datSet, 4, SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "rptID", ReportSettings->rptID, 4, SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "optFields", ReportSettings->optFields, 4, SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "bufTime", ReportSettings->bufTime, 4, SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "trgOps", ReportSettings->trgOps, 4, SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "intgPd", ReportSettings->intgPd, 4, SCL_ATTR_OPTIONAL);

  ret = scl_get_attr_ptr (sxDecCtrl, "resvTms", &str, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
    {
    ReportSettings->resvTms = SD_TRUE;	/* set boolean value	*/
    }
  /* "owner" attribute proposed in Tissue 807. If Tissue 807 is not	*/
  /* approved, this is useless, but it shouldn't hurt to check for it.	*/
  ret = scl_get_attr_ptr (sxDecCtrl, "owner", &str, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
    {
    ReportSettings->owner = SD_TRUE;	/* set boolean value	*/
    }
  /* end optional attributes */
  }

/************************************************************************/
/* Table used by Services_SEFun below.					*/
/************************************************************************/
SX_ELEMENT ServicesElements[] = 
{
  /* These entries for "tServiceYesNo".	*/
  {"GetDirectory",		SX_ELF_CSTART|SX_ELF_OPT,	_GetDirectory_SFun, NULL, 0},
  {"GetDataObjectDefinition",	SX_ELF_CSTART|SX_ELF_OPT,	_GetDataObjectDefinition_SFun, NULL, 0},
  {"DataObjectDirectory",	SX_ELF_CSTART|SX_ELF_OPT,	_DataObjectDirectory_SFun, NULL, 0},
  {"GetDataSetValue",		SX_ELF_CSTART|SX_ELF_OPT,	_GetDataSetValue_SFun, NULL, 0},
  {"SetDataSetValue",		SX_ELF_CSTART|SX_ELF_OPT,	_SetDataSetValue_SFun, NULL, 0},
  {"DataSetDirectory",		SX_ELF_CSTART|SX_ELF_OPT,	_DataSetDirectory_SFun, NULL, 0},
  {"ReadWrite",			SX_ELF_CSTART|SX_ELF_OPT,	_ReadWrite_SFun, NULL, 0},
  {"TimerActivatedControl",	SX_ELF_CSTART|SX_ELF_OPT,	_TimerActivatedControl_SFun, NULL, 0},
  {"GetCBValues",		SX_ELF_CSTART|SX_ELF_OPT,	_GetCBValues_SFun, NULL, 0},
  {"GSEDir",			SX_ELF_CSTART|SX_ELF_OPT,	_GSEDir_SFun, NULL, 0},
  {"FileHandling",		SX_ELF_CSTART|SX_ELF_OPT,	_FileHandling_SFun, NULL, 0},
  {"ConfLdName",		SX_ELF_CSTART|SX_ELF_OPT,	_ConfLdName_SFun, NULL, 0},
  /* These entries for "tServiceWithMax".	*/
  {"ConfLogControl",		SX_ELF_CSTART|SX_ELF_OPT,	_ConfLogControl_SFun, NULL, 0},
  {"GOOSE",			SX_ELF_CSTART|SX_ELF_OPT,	_GOOSE_SFun, NULL, 0},
  {"GSSE",			SX_ELF_CSTART|SX_ELF_OPT,	_GSSE_SFun, NULL, 0},
  {"SMVsc",			SX_ELF_CSTART|SX_ELF_OPT,	_SMVsc_SFun, NULL, 0},
  {"SupSubscription",		SX_ELF_CSTART|SX_ELF_OPT,	_SupSubscription_SFun, NULL, 0},
  {"ConfSigRef",		SX_ELF_CSTART|SX_ELF_OPT,	_ConfSigRef_SFun, NULL, 0},
  /* More complex entries.	*/
  /* DEBUG: TO DO: Add entries for DynAssociation, SettingGroups, ConfDataSet,*/
  /*        DynDataSet, LogSettings, GSESettings, SMVSettings, ConfLNs.*/
  {"ReportSettings",		SX_ELF_CSTART|SX_ELF_OPT,	_ReportSettings_SFun, NULL, 0}
};

/************************************************************************/
/*			Services_SEFun					*/
/* CRITICAL: default values were set automatically earlier. This element*/
/* is optional in SCL.							*/
/************************************************************************/
ST_VOID Services_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
SCL_INFO *scl_info;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_info = sclDecCtrl->sclInfo;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* If optional "nameLength" attribute is present, ignore it.	*/
    /* NOTE: don't need to call an "add" function. Info will be saved in sclDecCtrl->scl_services.*/
    SXLOG_CDEC0 ("SCL PARSE: Services section found");
    sx_push (sxDecCtrl, sizeof(ServicesElements)/sizeof(SX_ELEMENT), ServicesElements, SD_FALSE);
    }
  else /* reason = SX_ELEMENT_END */
    {
    sx_pop (sxDecCtrl);
    }
  }

