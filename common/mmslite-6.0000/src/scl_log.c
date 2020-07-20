/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2004 All Rights Reserved					*/
/*									*/
/* MODULE NAME : scl_log.c						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to log info parsed from SCL file.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			scl_log_all_data_types				*/
/*			scl_log_all_logical_devices			*/
/*			scl_log_all					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/12/12  JRB	   scl_log_all_logical_devices: add server loop.*/
/* 05/08/07  JRB    02     Fix log message.				*/
/* 06/10/04  DWL    01     Initial Revision.				*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "scl.h"
#include "slog.h"
#include "sx_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			scl_log_do					*/
/************************************************************************/
static ST_VOID scl_log_do (SCL_DO *scl_do)
  {
  SXLOG_DEC2 ("SCL LOG: <DO name='%s' type='%s'>",
    	       scl_do->name, scl_do->type); 
  }

/************************************************************************/
/*			scl_log_da					*/
/************************************************************************/
static ST_VOID scl_log_da (SCL_DA *scl_da)
  {
ST_CHAR dchg[6];
ST_CHAR qchg[6];
ST_CHAR dupd[6];

  SXLOG_DEC8 ("SCL LOG: <DA name='%s' desc='%s' sAddr='%s' bType='%s' valKind='%s' type='%s' count='%u' fc='%s'",
    	       scl_da->name, scl_da->desc, scl_da->sAddr, scl_da->bType,
    	       scl_da->valKind, scl_da->type, scl_da->count, scl_da->fc); 

  if (scl_da->dchg == SD_TRUE)
    strncpy (dchg, "true", sizeof(dchg));
  else
    strncpy (dchg, "false", sizeof(dchg));

  if (scl_da->qchg == SD_TRUE)
    strncpy (qchg, "true", sizeof(qchg));
  else
    strncpy (qchg, "false", sizeof(qchg));

  if (scl_da->dupd == SD_TRUE)
    strncpy (dupd, "true", sizeof(dupd));
  else
    strncpy (dupd, "false", sizeof(dupd));

  SXLOG_CDEC3 ("SCL LOG: dchg='%s' qchg='%s' dupd='%s'>",
    	        dchg, qchg, dupd); 
  }

/************************************************************************/
/*			scl_log_sdo					*/
/************************************************************************/
static ST_VOID scl_log_sdo (SCL_DA *scl_da)
  {
  SXLOG_DEC3 ("SCL LOG: <SDO name='%s' desc='%s' type='%s'>",
    	       scl_da->name, scl_da->desc, scl_da->type); 
  }

/************************************************************************/
/*			scl_log_lntype					*/
/************************************************************************/
static ST_VOID scl_log_lntype (SCL_LNTYPE *scl_lntype)
  {
SCL_DO *scl_do;

  SXLOG_DEC2 ("SCL LOG: <LNodeType id='%s' lnClass='%s'>",
    	       scl_lntype->id, scl_lntype->lnClass); 

  for (scl_do = (SCL_DO *) list_find_last ((DBL_LNK *) scl_lntype->doHead);
       scl_do != NULL;
       scl_do = (SCL_DO *) list_find_prev ((DBL_LNK *) scl_lntype->doHead, (DBL_LNK *) scl_do))
    {
    scl_log_do (scl_do);
    }
  }

/************************************************************************/
/*			scl_log_dotype					*/
/************************************************************************/
static ST_VOID scl_log_dotype (SCL_DOTYPE *scl_dotype)
  {
SCL_DA *scl_da;

  SXLOG_DEC2 ("SCL LOG: <DOType id='%s' cdc='%s'>",
    	       scl_dotype->id, scl_dotype->cdc); 

  for (scl_da = (SCL_DA *) list_find_last ((DBL_LNK *) scl_dotype->daHead);
       scl_da != NULL;
       scl_da = (SCL_DA *) list_find_prev ((DBL_LNK *) scl_dotype->daHead, (DBL_LNK *) scl_da))
    {
    if (scl_da->objtype == SCL_OBJTYPE_SDO)
      scl_log_sdo (scl_da);
    else /* Must be SCL_OBJTYPE_DA */
      scl_log_da (scl_da);
    }
  }

/************************************************************************/
/*			scl_log_bda					*/
/************************************************************************/
static ST_VOID scl_log_bda (SCL_BDA *scl_bda)
  {
  SXLOG_DEC7 ("SCL LOG: <BDA name='%s' desc='%s' sAddr='%s' bType='%s' valKind='%s' type='%s' count='%u'>",
    	       scl_bda->name, scl_bda->desc, scl_bda->sAddr,
    	       scl_bda->bType, scl_bda->valKind, scl_bda->type,
    	       scl_bda->count); 
  SXLOG_DEC1 ("SCL LOG: <Val='%s'>", scl_bda->Val); 
  }

/************************************************************************/
/*			scl_log_datype					*/
/************************************************************************/
static ST_VOID scl_log_datype (SCL_DATYPE *scl_datype)
  {
SCL_BDA *scl_bda;

  SXLOG_DEC1 ("SCL LOG: <DAType id='%s'>",
    	       scl_datype->id); 

  for (scl_bda = (SCL_BDA *) list_find_last ((DBL_LNK *) scl_datype->bdaHead);
       scl_bda != NULL;
       scl_bda = (SCL_BDA *) list_find_prev ((DBL_LNK *) scl_datype->bdaHead, (DBL_LNK *) scl_bda))
    {
    scl_log_bda (scl_bda);
    }
  }

/************************************************************************/
/*			scl_log_enumval				*/
/************************************************************************/
static ST_VOID scl_log_enumval (SCL_ENUMVAL *scl_enumval)
  {
  SXLOG_DEC2 ("SCL LOG: <EnumVal ord='%u' EnumVal='%s'>",
    	       scl_enumval->ord, scl_enumval->EnumVal); 
  }

/************************************************************************/
/*			scl_log_enumtype				*/
/************************************************************************/
static ST_VOID scl_log_enumtype (SCL_ENUMTYPE *scl_enumtype)
  {
SCL_ENUMVAL *scl_enumval;

  SXLOG_DEC1 ("SCL LOG: <EnumType id='%s'>",
    	       scl_enumtype->id); 

  for (scl_enumval = (SCL_ENUMVAL *) list_find_last ((DBL_LNK *) scl_enumtype->enumvalHead);
       scl_enumval != NULL;
       scl_enumval = (SCL_ENUMVAL *) list_find_prev ((DBL_LNK *) scl_enumtype->enumvalHead, (DBL_LNK *) scl_enumval))
    {
    scl_log_enumval (scl_enumval);
    }
  }

/************************************************************************/
/*			scl_log_all_data_types				*/
/************************************************************************/
ST_VOID scl_log_all_data_types (SCL_INFO *scl_info)
  {
SCL_LNTYPE *scl_lntype;
SCL_DOTYPE *scl_dotype;
SCL_DATYPE *scl_datype;
SCL_ENUMTYPE *scl_enumtype;

  /* NOTE: linked list is in reverse order from data in SCL file,	*/
  /*     so get off list in reverse order.				*/
  for (scl_lntype = (SCL_LNTYPE *) list_find_last ((DBL_LNK *) scl_info->lnTypeHead);
       scl_lntype != NULL;
       scl_lntype = (SCL_LNTYPE *) list_find_prev ((DBL_LNK *) scl_info->lnTypeHead, (DBL_LNK *) scl_lntype))
    {
    scl_log_lntype (scl_lntype);
    }

  for (scl_dotype = (SCL_DOTYPE *) list_find_last ((DBL_LNK *) scl_info->doTypeHead);
       scl_dotype != NULL;
       scl_dotype = (SCL_DOTYPE *) list_find_prev ((DBL_LNK *) scl_info->doTypeHead, (DBL_LNK *) scl_dotype))
    {
    scl_log_dotype (scl_dotype);
    }

  for (scl_datype = (SCL_DATYPE *) list_find_last ((DBL_LNK *) scl_info->daTypeHead);
       scl_datype != NULL;
       scl_datype = (SCL_DATYPE *) list_find_prev ((DBL_LNK *) scl_info->daTypeHead, (DBL_LNK *) scl_datype))
    {
    scl_log_datype (scl_datype);
    }

  for (scl_enumtype = (SCL_ENUMTYPE *) list_find_last ((DBL_LNK *) scl_info->enumTypeHead);
       scl_enumtype != NULL;
       scl_enumtype = (SCL_ENUMTYPE *) list_find_prev ((DBL_LNK *) scl_info->enumTypeHead, (DBL_LNK *) scl_enumtype))
    {
    scl_log_enumtype (scl_enumtype);
    }
  }

/************************************************************************/
/*			scl_log_fcda					*/
/************************************************************************/
static ST_VOID scl_log_fcda (SCL_FCDA *scl_fcda)
  {
  SXLOG_DEC7 ("SCL LOG: <FCDA ldInst='%s' prefix='%s' lnInst='%s' lnClass='%s' doName='%s' daName='%s' fc='%s'>",
    	       scl_fcda->ldInst, scl_fcda->prefix, scl_fcda->lnInst,
    	       scl_fcda->lnClass, scl_fcda->doName, scl_fcda->daName,
    	       scl_fcda->fc); 
  }

/************************************************************************/
/*			scl_log_dataset				*/
/************************************************************************/
static ST_VOID scl_log_dataset (SCL_DATASET *scl_dataset)
  {
SCL_FCDA *scl_fcda;

  SXLOG_DEC2 ("SCL LOG: <Dataset name='%s' desc='%s'>",
	       scl_dataset->name, scl_dataset->desc); 

  for (scl_fcda = (SCL_FCDA *) list_find_last ((DBL_LNK *) scl_dataset->fcdaHead);
       scl_fcda != NULL;
       scl_fcda = (SCL_FCDA *) list_find_prev ((DBL_LNK *) scl_dataset->fcdaHead, (DBL_LNK *) scl_fcda))
    {
    scl_log_fcda (scl_fcda);
    }
  }

/************************************************************************/
/*			scl_log_rcb					*/
/************************************************************************/
static ST_VOID scl_log_rcb (SCL_RCB *scl_rcb)
  {
ST_CHAR buffered[6];

  if (scl_rcb->buffered == SD_TRUE)
    strncpy (buffered, "true", sizeof (buffered));
  else
    strncpy (buffered, "false", sizeof (buffered));

  SXLOG_DEC8 ("SCL LOG: <ReportControl name='%s' desc='%s' datSet='%s' intgPd='%u' rptID='%s' confRev='%u' buffered='%s' bufTime='%u'", 
		scl_rcb->name, scl_rcb->desc, scl_rcb->datSet, 
		scl_rcb->intgPd, scl_rcb->rptID, scl_rcb->confRev,
		buffered, scl_rcb->bufTime);

  /* These are bitstrings. Just log as hex.	*/
  SXLOG_CDEC3 ("SCL LOG: TrgOps=0x%02X, OptFlds=0x%02X%02X",
		(ST_UINT) scl_rcb->TrgOps [0],
                (ST_UINT) scl_rcb->OptFlds[0],
                (ST_UINT) scl_rcb->OptFlds[1] );
  }

/************************************************************************/
/*			scl_log_ln					*/
/************************************************************************/
static ST_VOID scl_log_ln (SCL_LN *scl_ln)
  {
SCL_DATASET *scl_dataset;
SCL_RCB *scl_rcb;

  SXLOG_DEC5 ("SCL LOG: <LNode desc='%s' lnType='%s' lnClass='%s' inst='%s' prefix='%s'>", 
      	 	scl_ln->desc, scl_ln->lnType, scl_ln->lnClass,
      	 	scl_ln->inst, scl_ln->prefix);

  /* Log Datasets							*/
  for (scl_dataset = (SCL_DATASET *) list_find_last ((DBL_LNK *) scl_ln->datasetHead);
       scl_dataset != NULL;
       scl_dataset = (SCL_DATASET *) list_find_prev ((DBL_LNK *) scl_ln->datasetHead, (DBL_LNK *) scl_dataset))
    {
    scl_log_dataset (scl_dataset);
    }

  /* Log RCBs								*/
  for (scl_rcb = (SCL_RCB *) list_find_last ((DBL_LNK *) scl_ln->rcbHead);
       scl_rcb != NULL;
       scl_rcb = (SCL_RCB *) list_find_prev ((DBL_LNK *) scl_ln->rcbHead, (DBL_LNK *) scl_rcb))
    {
    scl_log_rcb (scl_rcb);
    }
  }

/************************************************************************/
/*			scl_log_ld					*/
/************************************************************************/
static ST_VOID scl_log_ld (
	SCL_INFO *scl_info,	/* main struct where all SCL info stored*/
	SCL_LD *scl_ld)
  {
SCL_LN *scl_ln;

  SXLOG_DEC2 ("SCL LOG: <LDevice desc='%s' inst='%s'>", 
  	       scl_ld->desc, scl_ld->inst);

  /* Log Logical nodes							*/
  /* NOTE: linked lists are in reverse order from data in SCL file,	*/
  /*       so get off list in reverse order.				*/
  for (scl_ln = (SCL_LN *) list_find_last ((DBL_LNK *) scl_ld->lnHead);
       scl_ln != NULL;
       scl_ln = (SCL_LN *) list_find_prev ((DBL_LNK *) scl_ld->lnHead, (DBL_LNK *) scl_ln))
    {
    scl_log_ln (scl_ln);
    }
  }

/************************************************************************/
/*			scl_log_all_logical_devices			*/
/* Log all Logical Devices from SCL info saved in "scl_info".		*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
ST_VOID scl_log_all_logical_devices (SCL_INFO *scl_info)
  {
SCL_SERVER *scl_server;
SCL_LD *scl_ld;

  for (scl_server = (SCL_SERVER *) list_find_last ((DBL_LNK *) scl_info->serverHead);
       scl_server != NULL;
       scl_server = (SCL_SERVER *) list_find_prev ((DBL_LNK *) scl_info->serverHead, (DBL_LNK *) scl_server))
    {
    for (scl_ld = (SCL_LD *) list_find_last ((DBL_LNK *) scl_server->ldHead);
         scl_ld != NULL;
         scl_ld = (SCL_LD *) list_find_prev ((DBL_LNK *) scl_server->ldHead, (DBL_LNK *) scl_ld))
      {
      scl_log_ld (scl_info, scl_ld);
      }
    }
  }

/************************************************************************/
/*			scl_log_all					*/
/* Log everything from SCL info saved in "scl_info".			*/
/************************************************************************/
ST_VOID scl_log_all (SCL_INFO *scl_info)
  {
    scl_log_all_logical_devices (scl_info);
    scl_log_all_data_types (scl_info);
  }
