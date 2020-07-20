/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2010, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mvl61850_ctl.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	IEC 61850 Control Model Server functions.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			mvl61850_sbo_create_sboname			*/
/*			mvl61850_ctl_chk_sbo				*/
/*			mvl61850_ctl_chk_sbow				*/
/*			mvl61850_ctl_chk_state				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 05/04/11  JRB	   mvl61850_ctl_chk_sbow: set correct DAE on errors.*/
/* 04/18/11  JRB	   getSboCtrl: chg to pass UCA test CtlN2,CtlN4.*/
/* 03/24/11  JRB	   Chg default to MVL61850_SBO_SELECT_TIMEOUT.	*/
/* 10/11/10  JRB	   mvl61850_ctl_chk_state: simplify funct & set	*/
/*			   mvluWrVaCtrl->wrVaCtrl->failure on errors.	*/
/* 07/23/08  JRB     17    Fix creation of CntrlObj string again.	*/
/*			   Fix CntrlObj size (129 char) in TDL.		*/
/*			   Send AddCause=3(select-failed) if already selected*/
/* 07/02/08  JRB     16    Repl cmd_executing w/ ctlState (more flexible)*/
/*			   Make sure SBO name <= MVL61850_MAX_OBJREF_LEN.*/
/*			   Fix CntrlObj string.	Del unused var.		*/
/* 05/14/08  JRB     15    mvl61850_ctl_req_done: first chk (va!=NULL).	*/
/* 05/08/08  JRB     14    Handle sboClass=operate-many. For sbo-with-	*/
/*			   enhanced-sec, don't unselect until CmdTerm.	*/
/* 05/08/08  JRB     13    mvl61850_ctl_chk_state: set LastApplError.	*/
/* 05/06/08  JRB     12    Don't allow re-select until after command or	*/
/*			   sboTimeout (use new getSboCtrl function).	*/
/*			   Set default Error/AddCause if not already set*/
/* 03/03/08  JRB     11    Add mvl61850_ctl_req_done & fix it to NOT send*/
/*			   LastApplError for Oper if normal security.	*/
/*			   Find and save sboClass to use later.		*/
/* 02/26/07  JRB     10    initSboCtrl: add use_ms_timer arg.		*/
/* 02/21/07  JRB     09    Fix comment.					*/
/* 11/21/06  JRB     08    Add mvl61850_ctl_lastapplerror_send.		*/
/* 10/30/06  JRB     07    Use new mvl_vmd_* object handling functions.	*/
/*			   Fix check that ALL of Oper and ONLY Oper is	*/
/*			   being written.				*/
/* 04/17/06  JRB     06    Do NOT create LastApplError var at startup,	*/
/*			   but create temporary var only when needed.	*/
/* 09/12/05  JRB     05    Add mvl61850_ctl_chk_sbow.			*/
/*			   Add NERR log if Loc$stVal==TRUE.		*/
/*			   Chk ctlModel before SBO read or SBOw write.	*/
/* 07/11/05  JRB     04    Add mvl61850_ctl_command_termination.	*/
/*			   Add mvl61850_ctl_lastapplerror_create.	*/
/* 03/28/05  JRB     03    Don't allow SBO read if Loc$stVal==TRUE.	*/
/* 01/27/05  JRB     02    Del unused variable.				*/
/* 12/10/04  JRB     01    New. Moved 61850 Control code from mvlu_sbo.c*/
/*			   to this module.				*/
/*			   Add mvl61850_ctl_chk_sbo, mvl61850_ctl_chk_state.*/
/*			   Del leaf funct mvl61850_sbo_select_rd_ind.	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mvl_uca.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif


/************************************************************************/
/*			getSboCtrl					*/
/* RETURNS: pointer to a structure from the global pool "sbo_pool"	*/
/*          (same pool used by "initSboCtrl" for UCA controls).		*/
/* NOTE: On error, this function returns NULL. If LastApplError!=NULL,	*/
/*       it also sets the Error and AddCause members of LastApplError.	*/
/************************************************************************/
static MVL_SBO_CTRL *getSboCtrl (MVL_NET_INFO *net_info,
	ST_CHAR *sbo_name,
	ST_INT8   ctlModel,
	ST_UINT32 sboTimeout,
	ST_INT8   sboClass,
	MVL61850_LAST_APPL_ERROR *LastApplError)
  {
ST_INT i;
MVL_SBO_CTRL *sboCtrl;

  /* Stop now if name is too long (strcpy later would not be safe).	*/
  if (strlen (sbo_name) >= sizeof (sboCtrl->sbo_var))
    {	/* should NEVER happen	*/
    MVL_LOG_ERR1 ("Error finding IEC 61850 Control object '%s'. Name too long",
                  sbo_name);
    return (NULL);
    }

  /* See if the element is already selected.				*/
  sboCtrl = sbo_pool;
  for (i = 0; i < MAX_NUM_SBO_PEND; ++i, ++sboCtrl)
    {
    if (sboCtrl->in_use == SD_TRUE && !strcmp (sboCtrl->sbo_var, sbo_name))
      {
      /* Already selected.	*/
      if (sboCtrl->net_info == net_info)
        {
        /* This is same connection.			*/
        /* UCA Test Procedure Rev2.2 CtlN2 says to:	*/
        /* send Select Response+ (for SBOns),		*/
        /* send SelectWithValue Response+ (for SBOes),	*/
        /* and reset the timeout.			*/
        sboCtrl->expire_time_ms = sGetMsTime () + sboTimeout;	/* reset timeout*/
        return (sboCtrl);
        }
      else
        {
        /* This is different connection.	*/
        /* UCA Test Procedure Rev2.2 CtlN4 says to do this.		*/
        if (LastApplError != NULL)
          {
          LastApplError->Error = 3;	/* Operator Test Not OK		*/
          LastApplError->AddCause = 12;	/* Command-already-in-execution	*/
          }
        return (NULL);
        }
      }
    }

  /* The protected element is not selected, find unused SBO control	*/
  sboCtrl = sbo_pool;
  for (i = 0; i < MAX_NUM_SBO_PEND; ++i, ++sboCtrl)
    {
    if (sboCtrl->in_use == SD_FALSE)
      break;
    }
  if (i >= MAX_NUM_SBO_PEND)
    {
    if (LastApplError != NULL)
      {
      LastApplError->Error = 1;		/* Unknown	*/
      LastApplError->AddCause = 0;	/* Unknown	*/
      }
    return (NULL);
    }

  /* Found available SBO control, set the parameters.	*/
  memset (sboCtrl, 0, sizeof (MVL_SBO_CTRL));	/* start w/ clean struct*/
  sboCtrl->in_use = SD_TRUE;
  sboCtrl->use_ms_timer = SD_TRUE;	/* ALWAYS "ms" timer for 61850	*/
  sboCtrl->expire_time_ms = sGetMsTime () + sboTimeout;
  sboCtrl->net_info = net_info;
  /* NOTE: sbo_name length checked above, so this strcpy is safe.	*/
  strcpy (sboCtrl->sbo_var, sbo_name);

  sboCtrl->ctlModel = ctlModel;
  sboCtrl->sboClass = sboClass;
  return (sboCtrl);
  }

/************************************************************************/
/*			mvl61850_sbo_create_sboname			*/
/* Creates a name to return when an IEC 61850 SBO attribute is read.	*/
/* sboName must point to buffer of at least [MVL61850_MAX_OBJREF_LEN+1] char*/
/* NOTE: this name may also be passed to "mvlu_sbo_chk_state"		*/
/*	to find the correct MVL_SBO_CTRL struct.			*/
/************************************************************************/
ST_VOID mvl61850_sbo_create_sboname (MVL_VAR_ASSOC *va, MVL_SCOPE *va_scope, 
		      ST_CHAR *sboName)
  {
ST_CHAR *tmp_ptr;	/* use to find last '$' in var name	*/

/* Create ObjectReference for this variable.	*/
  if (mvl61850_objref_create (va->name, va_scope, sboName) != SD_SUCCESS)
    {	/* should NEVER happen	*/
    MVL_LOG_ERR0 ("Can't generate SBO ObjectReference");
    sboName [0] = '\0';	/* return empty string	*/
    return;
    }

/* Replace end of var name with name of protected element (Oper).	*/
/* Var name may end with Oper, SBO, Cancel, etc. Replace with "Oper".	*/
/* This is a waste if it already ends with "Oper", but it takes more	*/
/* code to check.							*/
  if ((tmp_ptr = strrchr (sboName, '$')) != NULL &&	/* find last '$'*/
      tmp_ptr - sboName + 5 <= MVL61850_MAX_OBJREF_LEN)	/* room for Oper*/
    strcpy (tmp_ptr+1, "Oper");	/* Write name of protected element (Oper) AFTER it.*/
  else
    {	/* should NEVER happen	*/
    MVL_LOG_ERR1 ("Can't generate SBO ObjectReference from '%s' (too long to replace last comp with 'Oper')", sboName);
    sboName [0] = '\0';	/* return empty string	*/
    }
  }

/************************************************************************/
/*			mvl61850_mkname_ctlmodel			*/
/* Generate the "flattened" name of the "ctlModel" attribute that	*/
/* corresponds to the attribute passed as "var_name" arg.		*/
/* The var_name arg should always be of the following form:		*/
/*	LNodeName$CO$yyy$Oper						*/
/*	LNodeName$CO$yyy$SBO, etc.					*/
/* This funct generates a name like this ("LNodeName$" stripped off):	*/
/*	CF$yyy$ctlModel							*/
/************************************************************************/
ST_RET mvl61850_mkname_ctlmodel (ST_CHAR *var_name, ST_CHAR *flatname, size_t flatname_len)
  {
ST_RET retcode = SD_FAILURE;
ST_CHAR *firstdollar;	/* ptr to first '$' in var name	*/
ST_CHAR *lastdollar;	/* ptr to last '$' in var name	*/
ST_INT prefixlen;	/* len of first half of generated string	*/
ST_CHAR *suffix = "ctlModel";

  firstdollar = strchr (var_name, '$');		/* find first '$'	*/
  lastdollar = strrchr (var_name, '$');		/* find last '$'	*/
  if (firstdollar && lastdollar)
    {
    prefixlen = (int) (lastdollar - firstdollar);
    if (prefixlen + strlen (suffix) <= flatname_len)
      {
      strcpy (flatname, firstdollar+1);	/* Copy text after first '$'*/
      strncpy (flatname, "CF", 2);	/* Replace CO at start with CF	*/
      strcpy (flatname + prefixlen, suffix);	/* Write new suffix after last '$'*/
      retcode = SD_SUCCESS;
      }
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl61850_mkname_sbotimeout			*/
/* Generate the "flattened" name of the "sboTimeout" attribute that	*/
/* corresponds to the attribute passed as "var_name" arg.		*/
/* The var_name arg should always be of the following form:		*/
/*	LNodeName$CO$yyy$SBO						*/
/* This funct generates a name like this ("LNodeName$" stripped off):	*/
/*	CF$yyy$sboTimeout						*/
/************************************************************************/
ST_RET mvl61850_mkname_sbotimeout (ST_CHAR *var_name, ST_CHAR *flatname, size_t flatname_len)
  {
ST_CHAR *firstdollar;	/* ptr to first '$' in var name	*/
ST_CHAR *lastdollar;	/* ptr to last '$' in var name	*/
ST_INT prefixlen;	/* len of first half of generated string	*/
ST_CHAR *suffix = "sboTimeout";	/* replacement text to put after last '$'*/
ST_RET retcode = SD_FAILURE;

  firstdollar = strchr (var_name, '$');		/* find first '$'	*/
  lastdollar = strrchr (var_name, '$');		/* find last '$'	*/
  if (firstdollar && lastdollar)
    {
    prefixlen = (int) (lastdollar - firstdollar);
    if (prefixlen +  strlen (suffix) <= flatname_len)	/* room to replace*/
      {
      strcpy (flatname, firstdollar+1);	/* Copy text after first '$'*/
      strncpy (flatname, "CF", 2);	/* Replace CO at start with CF	*/
      strcpy (flatname + prefixlen, suffix);	/* Write new suffix after last '$'*/
      retcode = SD_SUCCESS;
      }
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl61850_mkname_sboclass			*/
/* Generate the "flattened" name of the "sboClass" attribute that	*/
/* corresponds to the attribute passed as "var_name" arg.		*/
/* The var_name arg should always be of the following form:		*/
/*	LNodeName$CO$yyy$SBO						*/
/* This funct generates a name like this ("LNodeName$" stripped off):	*/
/*	CF$yyy$sboClass						*/
/************************************************************************/
ST_RET mvl61850_mkname_sboclass (ST_CHAR *var_name, ST_CHAR *flatname, size_t flatname_len)
  {
ST_CHAR *firstdollar;	/* ptr to first '$' in var name	*/
ST_CHAR *lastdollar;	/* ptr to last '$' in var name	*/
ST_INT prefixlen;	/* len of first half of generated string	*/
ST_CHAR *suffix = "sboClass";	/* replacement text to put after last '$'*/
ST_RET retcode = SD_FAILURE;

  firstdollar = strchr (var_name, '$');		/* find first '$'	*/
  lastdollar = strrchr (var_name, '$');		/* find last '$'	*/
  if (firstdollar && lastdollar)
    {
    prefixlen = (int) (lastdollar - firstdollar);
    if (prefixlen + strlen (suffix) <= flatname_len)	/* room to replace*/
      {
      strcpy (flatname, firstdollar+1);	/* Copy text after first '$'*/
      strncpy (flatname, "CF", 2);	/* Replace CO at start with CF	*/
      strcpy (flatname + prefixlen, suffix);	/* Write new suffix after last '$'*/
      retcode = SD_SUCCESS;
      }
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl61850_sboclass_find				*/
/* Find the corresponding 'sboClass' attribute for this variable and 	*/
/* read its current value.						*/
/************************************************************************/
static ST_INT8 mvl61850_sboclass_find (MVL_VAR_ASSOC *va)
  {
ST_CHAR sboClassName [MAX_IDENT_LEN+1];		/* "CF$...$sboClass"	*/
ST_INT8 sboClass;
  if (mvl61850_mkname_sboclass (va->name, sboClassName, MAX_IDENT_LEN) != SD_SUCCESS ||
      mvlu_get_leaf_val_int8 (va->base_va, sboClassName, &sboClass) != SD_SUCCESS)
    sboClass = MVL61850_SBOCLASS_OPERATE_ONCE;	/* set default value	*/
  return (sboClass);
  }

/************************************************************************/
/*			mvl61850_ctl_chk_sbo				*/
/* This function should be called from "leaf" function when IEC 61850	*/
/* "SBO" attribute is being read (i.e. performing Control Model 'Select'*/
/* Service). It checks if client is allowed to perform 'Select'.	*/
/* If 'Select' allowed, it reserves a MVL_SBO_CTRL struct and returns	*/
/* a ptr to it.								*/
/* RETURNS:  ptr to struct if successful (sbo_var member of struct	*/
/*           contains name to send in read response).			*/
/*	     NULL if failed.						*/
/* CRITICAL: if return is NOT NULL & caller decides not to allow	*/
/*           'Select', must pass this ptr to mvlu_sbo_ctrl_free.	*/
/************************************************************************/
MVL_SBO_CTRL *mvl61850_ctl_chk_sbo (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_VAR_ASSOC *va;
ST_CHAR sboName[MVL61850_MAX_OBJREF_LEN+1];
MVL_SBO_CTRL *sboCtrl = NULL;
ST_CHAR *lastdollar;	/* ptr to last '$' in var name	*/
/* Init sboTimeout=0. If ..$CF$..$sboTimeout==0, or not found, set to default val.*/
ST_UINT32 sboTimeout = 0;	/* SBO timeout value.	*/
ST_CHAR sboTimeoutName [MAX_IDENT_LEN+1];	/* "CF$...$sboTimeout"	*/
ST_BOOLEAN loc_stval;
ST_INT8 sboClass;

/* First take care of timeout housekeeping  			*/
  mvlu_sbo_chk_timers ();

/* find the name of the attribute being selected		*/
  va = mvluRdVaCtrl->rdVaCtrl->va;

/* Check value of "ST$Loc$stVal".	*/
  if (mvlu_get_leaf_val_boolean (va->base_va, "ST$Loc$stVal", &loc_stval)!=SD_SUCCESS)
    loc_stval = SD_FALSE;	/* if can't read ST$Loc$stVal, assume FALSE*/
  if (loc_stval)
    {
    MVL_LOG_NERR1 ("Reading of '%s' not allowed: Local mode is set.", va->name);
    return (NULL);	/* error return	*/
    }

/* Only perform "Select" if SBO is ONLY attribute being read.	*/
/* Check this by seeing if variable name ends with "$SBO".	*/
/* Point to where "$SBO" should be, then see if it's there.	*/
  lastdollar = strrchr (va->name, '$');		/* find last '$'	*/
  if (lastdollar != NULL && strcmp (lastdollar+1, "SBO") == 0)
    {
    ST_INT8 ctlModel;
    ST_CHAR flatname [MAX_IDENT_LEN + 1];	/* attribute to find	*/

    /* Generate "ctlModel" leaf name and try to read its value.	*/
    /* CRITICAL: this only works if va->name checked first (i.e. it ends in "$SBO")*/
    if (mvl61850_mkname_ctlmodel (va->name, flatname, MAX_IDENT_LEN) != SD_SUCCESS
        || mvlu_get_leaf_val_int8 (va->base_va, flatname, &ctlModel) != SD_SUCCESS)
      ctlModel = MVL61850_CTLMODEL_STATUS_ONLY;	/* can't read ctlModel. assume status-only*/

    if (ctlModel != MVL61850_CTLMODEL_SBO_NORMAL)
      {
      MVL_LOG_NERR3 ("Reading of '%s' not allowed: ctlModel=%d, expected ctlModel=%d (sbo-with-normal-security)",
                     va->name, ctlModel, MVL61850_CTLMODEL_SBO_NORMAL);
      return (NULL);	/* error return	*/
      }

/* Create sboName from var name.	*/
    mvl61850_sbo_create_sboname (va, &mvluRdVaCtrl->rdVaCtrl->va_scope, sboName);

/* Find "sboTimeout" value.	*/
  if (mvl61850_mkname_sbotimeout (va->name, sboTimeoutName, MAX_IDENT_LEN) != SD_SUCCESS ||
      mvlu_get_leaf_val_uint32 (va->base_va, sboTimeoutName, &sboTimeout) != SD_SUCCESS ||
      sboTimeout==0)		/* ..$CF$..$sboTimeout==0, or not found	*/
    sboTimeout = MVL61850_SBO_SELECT_TIMEOUT;	/* set timeout to default val*/

/* Find "sboClass" value.	*/
    sboClass = mvl61850_sboclass_find (va);

/* Get a SBO control element */
    sboCtrl = getSboCtrl (mvluRdVaCtrl->indCtrl->event->net_info,sboName,
              ctlModel, sboTimeout, sboClass,
              NULL);	/* LastApplError not used*/
    }

  return (sboCtrl);
  }

/************************************************************************/
/*			mvl61850_ctl_chk_sbow				*/
/* This function should be called from "leaf" function when IEC 61850	*/
/* an attribute inside the "SBOw" structure is being written		*/
/* (i.e. performing Control Model 'Select' Service for			*/
/* sbo-with-enhanced-security).						*/
/* It checks if client is allowed to perform 'Select'.			*/
/* If 'Select' allowed, it reserves a MVL_SBO_CTRL struct and returns	*/
/* a ptr to it.								*/
/* RETURNS:  ptr to struct if successful (sbo_var member of struct	*/
/*           contains name to send in read response).			*/
/*	     NULL if failed.						*/
/* CRITICAL: if return is NOT NULL & caller decides not to allow	*/
/*           'Select', caller must pass this ptr to mvlu_sbo_ctrl_free.	*/
/* NOTE: this is very similar to mvl61850_ctl_chk_sbo, but it is used	*/
/*       when writing "SBOw" instead of when reading "SBO".		*/
/************************************************************************/
MVL_SBO_CTRL *mvl61850_ctl_chk_sbow (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
MVL_VAR_ASSOC *va;
ST_CHAR sboName[MVL61850_MAX_OBJREF_LEN+1];
MVL_SBO_CTRL *sboCtrl = NULL;
ST_CHAR *lastdollar;	/* ptr to last '$' in var name	*/
/* Init sboTimeout=0. If ..$CF$..$sboTimeout==0, or not found, set to default val.*/
ST_UINT32 sboTimeout = 0;	/* SBO timeout value.	*/
ST_CHAR sboTimeoutName [MAX_IDENT_LEN+1];	/* "CF$...$sboTimeout"	*/
ST_BOOLEAN loc_stval;
ST_INT8 sboClass;

/* First take care of timeout housekeeping  			*/
  mvlu_sbo_chk_timers ();

  va = mvluWrVaCtrl->wrVaCtrl->va;

/* Check value of "ST$Loc$stVal".	*/
  if (mvlu_get_leaf_val_boolean (va->base_va, "ST$Loc$stVal", &loc_stval)!=SD_SUCCESS)
    loc_stval = SD_FALSE;	/* if can't read ST$Loc$stVal, assume FALSE*/
  if (loc_stval)
    {
    MVL_LOG_NERR1 ("Writing of '%s' not allowed: Local mode is set.", va->name);
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_TEMP_UNAVAIL;
    return (NULL);	/* error return	*/
    }

/* Only perform "Select" if entire SBOw is written & nothing else.*/
/* Check this by seeing if variable name ends with "$SBOw".	*/
/* Point to where "$SBOw" should be, then see if it's there.	*/
  lastdollar = strrchr (va->name, '$');		/* find last '$'	*/
  if (lastdollar != NULL && strcmp (lastdollar+1, "SBOw") == 0)
    {
    ST_INT8 ctlModel;
    ST_CHAR flatname [MAX_IDENT_LEN + 1];	/* attribute to find	*/

    /* Generate "ctlModel" leaf name and try to read its value.		*/
    /* CRITICAL: this only works if va->name checked first (i.e. it ends in "$SBOw")*/
    if (mvl61850_mkname_ctlmodel (va->name, flatname, MAX_IDENT_LEN) != SD_SUCCESS
        || mvlu_get_leaf_val_int8 (va->base_va, flatname, &ctlModel) != SD_SUCCESS)
      ctlModel = MVL61850_CTLMODEL_STATUS_ONLY;	/* can't read ctlModel. assume status-only*/

    if (ctlModel != MVL61850_CTLMODEL_SBO_ENHANCED)
      {
      MVL_LOG_NERR3 ("Writing of '%s' not allowed: ctlModel=%d, expected ctlModel=%d (sbo-with-enhanced-security)",
                     va->name, ctlModel, MVL61850_CTLMODEL_SBO_ENHANCED);
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;
      return (NULL);
      }

    /* Create sboName from var name.	*/
    /* NOTE: the sboName is not really used for "SBOw", but the "initSboCtrl"*/
    /*   function (originally written only for "SBO") needs it.		*/
    mvl61850_sbo_create_sboname (va, &mvluWrVaCtrl->wrVaCtrl->va_scope, sboName);

    /* Find "sboTimeout" value.	*/
    if (mvl61850_mkname_sbotimeout (va->name, sboTimeoutName, MAX_IDENT_LEN) != SD_SUCCESS ||
        mvlu_get_leaf_val_uint32 (va->base_va, sboTimeoutName, &sboTimeout) != SD_SUCCESS ||
        sboTimeout==0)		/* ..$CF$..$sboTimeout==0, or not found	*/
      sboTimeout = MVL61850_SBO_SELECT_TIMEOUT;	/* set timeout to default val*/

    /* Find "sboClass" value.	*/
    sboClass = mvl61850_sboclass_find (va);

    /* Get a SBO control element */
    sboCtrl = getSboCtrl (mvluWrVaCtrl->indCtrl->event->net_info, sboName, 
              ctlModel, sboTimeout, sboClass,
              &mvluWrVaCtrl->wrVaCtrl->LastApplError);

    if (sboCtrl == NULL)
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_TEMP_UNAVAIL;
    }
  else
    {	/* not writing whole "SBOw" structure. Not allowed.	*/
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;
    }

  return (sboCtrl);
  }

/************************************************************************/
/*			mvl61850_ctl_chk_state				*/
/* Check ctlModel, etc. to determine if control is right type &		*/
/* in right state.							*/
/* RETURNS: SD_SUCCESS if OK to perform Operate command.		*/
/*          SD_FAILURE if NOT OK (also sets appropriate Error and	*/
/*          AddCause members of LastApplError).				*/
/************************************************************************/
ST_RET mvl61850_ctl_chk_state (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
MVL_VAR_ASSOC *base_var = mvluWrVaCtrl->wrVaCtrl->va->base_va;
ST_INT8 ctlModel;
ST_BOOLEAN loc_stval;
ST_CHAR flatname [MAX_IDENT_LEN + 1];	/* attribute to find	*/
ST_CHAR *lastdollar;	/* ptr to last '$' in var name	*/
MVL61850_LAST_APPL_ERROR *LastApplError;

  /* Send this DAE in the MMS Write response on any error. If the Write	*/
  /* succeeds, this value is ignored (i.e. not sent).			*/
  mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;

  LastApplError = &mvluWrVaCtrl->wrVaCtrl->LastApplError;

  /* Only allow "Operate" if writing ALL of "Oper" and ONLY "Oper".	*/
  /* Check this by seeing if variable name ends with "$Oper".		*/
  /* Point to where "$Oper" should be, then see if it's there.		*/
  lastdollar = strrchr (mvluWrVaCtrl->wrVaCtrl->va->name, '$');	/* find last '$'*/
  if (lastdollar == NULL || strcmp (lastdollar+1, "Oper") != 0)
    {
    /* LastApplError not sent in this case, so don't bother setting it.	*/
    return (SD_FAILURE);	/* ERROR: not writing ALL of Oper and ONLY Oper.*/
    }

  /* Check value of "ST$Loc$stVal".	*/
  if (mvlu_get_leaf_val_boolean (base_var, "ST$Loc$stVal", &loc_stval)!=SD_SUCCESS)
    loc_stval = SD_FALSE;	/* if can't read ST$Loc$stVal, assume FALSE*/
  if (loc_stval == SD_FALSE)
    {
    /* Generate "ctlModel" leaf name and try to read its value.	*/	
    if (mvl61850_mkname_ctlmodel (mvluWrVaCtrl->wrVaCtrl->va->name, flatname, MAX_IDENT_LEN) != SD_SUCCESS
        || mvlu_get_leaf_val_int8 (base_var, flatname, &ctlModel) != SD_SUCCESS)
      ctlModel = MVL61850_CTLMODEL_STATUS_ONLY;	/* can't read ctlModel. assume status-only*/

    if (ctlModel == MVL61850_CTLMODEL_DIRECT_NORMAL  ||
        ctlModel == MVL61850_CTLMODEL_DIRECT_ENHANCED)
      {	/* This is Direct control.	*/
      /* OK. Nothing else to check. Continue to successful return.	*/
      }
    else if (ctlModel == MVL61850_CTLMODEL_SBO_NORMAL)
      {	/* sbo-with-normal-security	*/
      /* Chk sboCtrl (set by startElWrites if client writing "Oper").	*/
      if (!mvluWrVaCtrl->wrVaCtrl->sboCtrl)
        {
        /* LastApplError not sent in this case, so don't bother setting it.	*/
        return (SD_FAILURE);
        }
      }
    else if (ctlModel == MVL61850_CTLMODEL_SBO_ENHANCED)
      {	/* sbo-with-enhanced-security	*/
      /* Chk sboCtrl (set by startElWrites if client writing "Oper").	*/
      if (!mvluWrVaCtrl->wrVaCtrl->sboCtrl)
        {
        LastApplError->Error = 1;	/* Unknown*/
        LastApplError->AddCause = 3;	/* Select-failed*/
        return (SD_FAILURE);
        }
      }
    else
      {
      /* ctlModel="status-only" or invalid.	*/
      LastApplError->Error = 1;		/* Unknown*/
      LastApplError->AddCause = 8;	/* Blocked-by-mode*/
      return (SD_FAILURE);
      }
    }	/* end "if (loc_stval == SD_FALSE)"  (i.e. "remote" mode)	*/
  else
    {	/* loc_stval == SD_TRUE (i.e. "local" mode)	*/
    LastApplError->Error = 1;		/* Unknown*/
    LastApplError->AddCause = 8;	/* Blocked-by-mode*/
    return (SD_FAILURE);
    }
  /* Passed all checks. Allow the control operation.	*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvl61850_ctl_command_termination		*/
/* Called by user to send "Command Termination" request for controls	*/
/* with "Enhanced Security".						*/
/************************************************************************/
ST_RET mvl61850_ctl_command_termination (MVL_NET_INFO *net_info, ST_CHAR *oper_ref,
            ST_RET status, MVL61850_LAST_APPL_ERROR *last_appl_error)
  {
OBJECT_NAME nvl_obj;	/* NVL object name	*/
MVL_NVLIST_CTRL *nvlist_ctrl;
ST_INT num_var;
OBJECT_NAME var_obj[2];	/* array of var object names (1 or 2 depending on status)*/
MVL_VAR_ASSOC *last_appl_var = NULL;	/* var assoc for "LastApplError"*/
ST_RET ret;
ST_CHAR oper_ref_local[MAX_IDENT_LEN+1];	/* local copy of oper_ref arg	*/
MVL_SBO_CTRL *sboCtrl;		/* ptr to SBO control if this is SBO	*/

  ret = SD_SUCCESS;	/* assume success	*/

  /* Create tmp NVL used only for building this rpt.	*/
  nvl_obj.object_tag = VMD_SPEC;
  nvl_obj.obj_name.vmd_spec = "tmp_nvl_for_rpt";

  /* Fill in var_obj array.	*/
  num_var = 0;
  if (status != SD_SUCCESS)
    {			/* On error, include "LastApplError" in report.	*/
    ST_INT type_id;
    /* Create temporary VMD_SPEC "LastApplError" variable.	*/
    /* If this fails, mvl_vmd_nvl_add will also fail later.	*/ 
    type_id = mvl_typename_to_typeid ("LastApplErrorType");
    if (type_id >= 0)
      {
      var_obj[num_var].object_tag = VMD_SPEC;
      var_obj[num_var].obj_name.vmd_spec = "LastApplError";
      /* NOTE: last_appl_error points to temporary data. This var must be deleted before returning*/
      last_appl_var = mvl_vmd_var_add (&mvl_vmd, &var_obj[num_var],
            NULL,	/* (MVL_NET_INFO *)	*/
            type_id,
            last_appl_error,	/* data ptr (saved in var->data)	*/
            NULL,	/* (MVL_VAR_PROC *)	*/
            SD_FALSE);	/* DO NOT copy the var name	*/
      }
    num_var++;
    }
  /* Add this control object (LN$FC$xxx$Oper) to "var_obj".	*/
  var_obj[num_var].object_tag = DOM_SPEC;	/* ALWAYS Domain specific*/
  /* strtok modifies input buffer, so copy oper_ref to local buffer first.	*/
  strcpy (oper_ref_local, oper_ref);
  var_obj[num_var].domain_id = strtok (oper_ref_local, "/");	/*extract dom name*/
  var_obj[num_var].obj_name.vmd_spec = strtok (NULL, "");
  if (var_obj[num_var].obj_name.vmd_spec == NULL)
    {
    MVL_LOG_ERR1 ("Invalid ControlObjectReference '%s', cannot send CommandTermination", oper_ref);
    ret = SD_FAILURE;
    }
  else
    {
    num_var++;

    nvlist_ctrl = mvl_vmd_nvl_add (&mvl_vmd, &nvl_obj, NULL,	/* net_info: not needed*/
                   num_var, var_obj, SD_TRUE);	/* copy name	*/

    /* Now send report	*/
    if (nvlist_ctrl)
      {
      ret = mvl_info_variables (net_info, nvlist_ctrl, SD_TRUE);	/* listOfVariables*/
      mvl_vmd_nvl_remove (&mvl_vmd, &nvl_obj, NULL);	/* done with tmp NVL	*/
      }
    else
      ret = SD_FAILURE;
    }
  /* If temporary "LastApplError" var created, destroy it now.	*/
  if (last_appl_var)
    mvl_vmd_var_remove (&mvl_vmd, &var_obj[0], NULL);	/* always first in "var_obj" array*/
    
  sboCtrl = mvlu_sbo_chk_state (oper_ref, net_info);
  if (sboCtrl != NULL
      && sboCtrl->ctlModel == MVL61850_CTLMODEL_SBO_ENHANCED)
    {
    /* ctlModel=sbo-with-enhanced-security. If sboClass=operate-once,	*/
    /* "unselect" now (i.e. free sboCtrl), else go back to READY state.	*/
    if (sboCtrl->sboClass == MVL61850_SBOCLASS_OPERATE_ONCE)
      mvlu_sbo_ctrl_free (sboCtrl);	/* like "UNSELECTED" state*/
    else
      sboCtrl->ctlState = MVL61850_CTLSTATE_READY;
    }
  return (ret);
  }

/************************************************************************/
/*			mvl61850_ctl_lastapplerror_create		*/
/* Create type to use later when "temporary" LastApplError var created.	*/
/************************************************************************/
ST_RET mvl61850_ctl_lastapplerror_create ()
  {
ST_RET retcode = SD_SUCCESS;
ST_CHAR *LastApplErrorTdl = "{\
    (CntrlObj)Vstring129,\
    (Error)Byte,\
    (Origin){(orCat)Byte,(orIdent)OVstring64},\
    (ctlNum)Ubyte,\
    (AddCause)Byte}";

  /* Maybe this type was already created. If it does NOT exist, create it.*/
  if (mvl_typename_to_typeid ("LastApplErrorType") < 0)
    {
    if (mvl_type_id_create_from_tdl ("LastApplErrorType",LastApplErrorTdl) < 0)
      retcode = SD_FAILURE;	/* create failed	*/
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl61850_ctl_lastapplerror_send			*/
/* Called by user to send "LastApplError" information report.		*/
/************************************************************************/
ST_RET mvl61850_ctl_lastapplerror_send (MVL_NET_INFO *net_info,
	MVL61850_LAST_APPL_ERROR *last_appl_error)
  {
OBJECT_NAME nvl_obj;	/* NVL object name	*/
MVL_NVLIST_CTRL *nvlist_ctrl;
ST_INT num_var;
OBJECT_NAME var_obj;			/* temporary var object name	*/
MVL_VAR_ASSOC *last_appl_var = NULL;	/* var assoc for "LastApplError"*/
ST_RET ret;
ST_INT type_id;

  ret = SD_SUCCESS;	/* assume success	*/

  /* Create tmp NVL used only for building this rpt.	*/
  nvl_obj.object_tag = VMD_SPEC;
  nvl_obj.obj_name.vmd_spec = "tmp_nvl_for_rpt";

  /* Fill in var_obj.	*/
  num_var = 1;
  /* Create temporary VMD_SPEC "LastApplError" variable.	*/
  /* If this fails, mvl_vmd_nvl_add will also fail later.		*/ 
  type_id = mvl_typename_to_typeid ("LastApplErrorType");
  if (type_id >= 0)
    {
    var_obj.object_tag = VMD_SPEC;
    var_obj.obj_name.vmd_spec = "LastApplError";
    /* NOTE: last_appl_error points to temporary data. This var must be deleted before returning*/
    last_appl_var = mvl_vmd_var_add (&mvl_vmd, &var_obj,
            NULL,	/* (MVL_NET_INFO *)	*/
            type_id,
            last_appl_error,	/* data ptr (saved in var->data)	*/
            NULL,	/* (MVL_VAR_PROC *)	*/
            SD_FALSE);	/* DO NOT copy the var name	*/

    nvlist_ctrl = mvl_vmd_nvl_add (&mvl_vmd, &nvl_obj, NULL,	/* net_info: not needed*/
                   num_var, &var_obj, SD_TRUE);	/* copy name	*/

    /* Now send report	*/
    if (nvlist_ctrl)
      {
      ret = mvl_info_variables (net_info, nvlist_ctrl, SD_TRUE);	/* listOfVariables*/
      mvl_vmd_nvl_remove (&mvl_vmd, &nvl_obj, NULL);	/* done with tmp NVL	*/
      }
    else
      ret = SD_FAILURE;
    }
  else
    ret = SD_FAILURE;

  /* If temporary "LastApplError" var created, destroy it now.	*/
  if (last_appl_var)
    mvl_vmd_var_remove (&mvl_vmd, &var_obj, NULL);
    
  if (ret != SD_SUCCESS)
    MVL_LOG_ERR0 ("Error sending LastApplError");
  return (ret);
  }

/************************************************************************/
/*			mvl61850_ctl_req_done				*/
/* Do some necessary cleanup IF the MMS Write request represents an IEC	*/
/* 61850 control request.						*/
/* NOTE: For any other MMS Write request, this function does nothing.	*/
/************************************************************************/
ST_VOID mvl61850_ctl_req_done (MVLAS_WR_VA_CTRL *wrVaCtrl, MVL_NET_INFO *net_info)

  {
ST_INT8 ctlModel;
ST_BOOLEAN send_lastappl = SD_FALSE;
ST_CHAR flatname [MAX_IDENT_LEN+1];
ST_CHAR *lastdollar;		/* ptr to last '$' in variable name	*/
MVL_SBO_CTRL *sboCtrl;		/* ptr to SBO control if this is SBO	*/

  /* CRITICAL: make sure variable was found before doing anything else.	*/
  if (wrVaCtrl->va == NULL)
    return;	/* var not found. Do nothing.	*/

  lastdollar = strrchr (wrVaCtrl->va->name, '$');	/* find last '$'*/

  /* If Write failed, and Var is IEC 61850 Oper, Cancel, or SBOw,	*/
  /* must also send InformationReport containing LastApplError.		*/
  if (wrVaCtrl->resp_tag == WR_RSLT_FAILURE)
    {
    /* Var name must end in $Oper, $Cancel, or $SBOw.			*/
    if (lastdollar != NULL)
      {
      /* Send LastApplError if writing Cancel or SBOw.	*/
      if (strcmp (lastdollar+1, "Cancel") == 0 ||
          strcmp (lastdollar+1, "SBOw") == 0)
        send_lastappl = SD_TRUE;
      /* Send LastApplError if writing Oper AND Enhanced security.	*/
      else if (strcmp (lastdollar+1, "Oper") == 0)
        {
        /* Generate "ctlModel" leaf name like "CF$...$ctlModel" from	*/
        /* variable name "...$Oper" and try to read its value.		*/
        if (mvl61850_mkname_ctlmodel (wrVaCtrl->va->name, flatname, MAX_IDENT_LEN) != SD_SUCCESS
            || mvlu_get_leaf_val_int8 (wrVaCtrl->va->base_va, flatname, &ctlModel) != SD_SUCCESS)
          ctlModel = MVL61850_CTLMODEL_STATUS_ONLY;	/* can't read ctlModel. assume status-only*/

        if (ctlModel == MVL61850_CTLMODEL_DIRECT_ENHANCED  ||
            ctlModel == MVL61850_CTLMODEL_SBO_ENHANCED)
          send_lastappl = SD_TRUE;
        }
      if (send_lastappl)
        {
        /* Construct & send LastApplError info report.*/

        /* Fill in "LastApplError" structure.	*/
        /* This function builds a string in "CntrlObj".	*/
        /* If it fails, the string is empty.		*/
#if 1
        mvl61850_objref_create (wrVaCtrl->va->name, &wrVaCtrl->va_scope, wrVaCtrl->LastApplError.CntrlObj);
#else	/* OLD WAY (similar but CntrlObj always ended in '$Oper').	*/
        mvl61850_sbo_create_sboname (wrVaCtrl->va, &wrVaCtrl->va_scope, wrVaCtrl->LastApplError.CntrlObj);
#endif
        /* Other members (Error, AddCause, Origin, ctlNum) already set by leaf functions.*/

        /* If Error/AddCause not set yet, set default values.	*/
        if (wrVaCtrl->LastApplError.Error == 0)
          {
          wrVaCtrl->LastApplError.Error = 1;	/* Unknown	*/        
          wrVaCtrl->LastApplError.AddCause = 0;	/* Unknown	*/
          }
        mvl61850_ctl_lastapplerror_send (net_info, &wrVaCtrl->LastApplError);
        }
      }
    }
  /* Do special cleanup for IEC 61850 SBO controls.			*/
  /* If wrVaCtrl->sboCtrl != NULL, this var is SBO Oper or Cancel	*/
  /* (see startElWrites), so do appropriate SBO cleanup.		*/
  /* If this is a Cancel request, or if ctlModel=sbo-with-normal-security*/
  /* AND sboClass=operate-once, we must "unselect" the control		*/
  /* (i.e. free "sboCtrl").						*/
  sboCtrl = wrVaCtrl->sboCtrl;
  if (sboCtrl != NULL)
    {
    if (strcmp (lastdollar+1, "Cancel") == 0
        || (sboCtrl->ctlModel == MVL61850_CTLMODEL_SBO_NORMAL
            && sboCtrl->sboClass == MVL61850_SBOCLASS_OPERATE_ONCE) )
      {
      /* NOTE: for sbo-with-enhanced-security, this is done AFTER CommandTerm.*/
      mvlu_sbo_ctrl_free (sboCtrl);
      wrVaCtrl->sboCtrl = NULL;	/* reset this so free not done twice.*/
      }
    }
  }

