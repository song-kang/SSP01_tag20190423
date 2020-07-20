/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998-2004, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mvlu_sbo.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 05/06/08  JRB     15    Chg sbo_pool from static to global.		*/
/* 02/26/07  JRB     14    initSboCtrl: add use_ms_timer arg.		*/
/* 01/27/05  JRB     13    Log if createSboName fails.			*/
/* 12/03/04  JRB     12    Move 61850 code to new module mvl61850_ctl.c.*/
/*			   Make initSboCtrl funct global so 61850	*/
/*			   module can use it too.			*/
/*			   Use strrchr to find last '$'.		*/
/* 09/20/04  JRB     11    Replace chk_sbo_select w/ mvlu_sbo_chk_state.*/
/*			   Chg chk_sbo_timeouts to mvlu_sbo_chk_timers.	*/
/*			   Do timeouts ONLY in mvlu_sbo_chk_timers.	*/
/*			   Do "Select" only if SBO is ONLY attribute	*/
/*			   being read (not part of higher level var).	*/
/*			   Add funct mvlu_sbo_ctrl_free.		*/
/*			   Del currTime global variable.		*/
/*			   Pass sboTimeout arg to initSboCtrl.		*/
/* 12/11/02  JRB     10    Use new mvl_uca.h				*/
/* 12/09/02  MDE     09    Changed SBO reference handling		*/
/* 10/25/00  JRB     08    Add #ifdef MVL_UCA.				*/
/* 07/13/00  JRB     07    Add #ifdef USE_RT_TYPE_2.			*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     05    Changed MVL_VA_SCOPE to MVL_SCOPE		*/
/* 12/11/98  MDE     04    Removed scope references from VA		*/
/* 11/16/98  MDE     03    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     02    Minor lint cleanup				*/
/* 09/11/98  MDE     01    New						*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mvl_uca.h"
#include "mvl_log.h"

#if defined(MVL_UCA)	/* This entire module is only valid for UCA.	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Global variables							*/
/************************************************************************/

MVL_SBO_CTRL sbo_pool[MAX_NUM_SBO_PEND];

/************************************************************************/
/* Static functions */

static ST_VOID createSboName (MVL_VAR_ASSOC *va, MVL_SCOPE *va_scope, 
			      ST_CHAR *dest,
			      ST_BOOLEAN select, ST_RTREF primRef);

/************************************************************************/
/************************************************************************/
/*                       mvlu_sbo_select_rd_ind				*/
/* Leaf function for reading UCA "SBO" attribute (i.e. performing	*/
/* Control Model 'Select' Service).					*/ 
/* NOTE: do NOT use this leaf function for IEC 61850 SBO.		*/
/************************************************************************/

ST_VOID mvlu_sbo_select_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
ST_CHAR *dest;
MVL_VAR_ASSOC *va;
MVL_SCOPE *va_scope;
ST_CHAR sboName[MAX_SBO_NAME_SIZE+1];
MVL_SBO_CTRL *sboCtrl;
ST_CHAR *lastdollar;	/* ptr to last '$' in var name	*/

/* First take care of timeout housekeeping  			*/
  mvlu_sbo_chk_timers ();

/* find the name of the attribute being selected		*/
  va = mvluRdVaCtrl->rdVaCtrl->va;

/* Only perform "Select" if SBO is ONLY attribute being read.	*/
/* Check this by seeing if variable name ends with "$SBO".	*/
/* Point to where "$SBO" should be, then see if it's there.	*/
  lastdollar = strrchr (va->name, '$');		/* find last '$'	*/
  if (lastdollar != NULL && strcmp (lastdollar+1, "SBO") == 0)
    {
    va_scope = &mvluRdVaCtrl->rdVaCtrl->va_scope;
    createSboName (va, va_scope, sboName, SD_TRUE, mvluRdVaCtrl->primRef);

/* Get a SBO control element */
    sboCtrl = initSboCtrl (mvluRdVaCtrl->indCtrl->event->net_info,sboName,
              SBO_SELECT_TIMEOUT, SD_FALSE);	/* timeout in seconds*/ 
    if (sboCtrl == NULL)
      {
      mvlu_rd_prim_done (mvluRdVaCtrl, SD_FAILURE);
      return;
      }
    }
  else
    /* Higher level variable read. Allow read, but just return NULL (empty) string.*/
    sboName[0]='\0';

  dest = (ST_CHAR *) mvluRdVaCtrl->primData;
  strcpy (dest, sboName);
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			mvlu_sbo_operate_wr_ind				*/
/* NOTE: this leaf function should be used only for UCA (not for 61850).*/
/************************************************************************/

ST_VOID mvlu_sbo_operate_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
MVL_SBO_CTRL *sboSelect;
MVL_SCOPE *va_scope;
ST_CHAR sboName[MAX_SBO_NAME_SIZE+1];

/* First take care of timeout housekeeping  			*/
  mvlu_sbo_chk_timers ();

  va_scope = &mvluWrVaCtrl->wrVaCtrl->va_scope;
  createSboName (mvluWrVaCtrl->wrVaCtrl->va, &mvluWrVaCtrl->wrVaCtrl->va_scope, sboName, SD_FALSE, (ST_RTREF) 0);
  sboSelect = mvlu_sbo_chk_state (sboName,
                        mvluWrVaCtrl->indCtrl->event->net_info);
  if (sboSelect != NULL)
    {
    u_mvl_sbo_operate (sboSelect, mvluWrVaCtrl);
    sboSelect->in_use = SD_FALSE;
    }
  else	/* The select was not in place, cannot operate ... */
    {
    mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);
    }
  }

/************************************************************************/
/*                       mvlu_clr_pend_sbo				*/
/************************************************************************/

ST_VOID mvlu_clr_pend_sbo (MVL_NET_INFO *net_info)
  {
ST_INT i;
  
  for (i = 0; i < MAX_NUM_SBO_PEND; ++i)
    {
    if (sbo_pool[i].net_info == net_info)
      sbo_pool[i].in_use = SD_FALSE;
    }
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			mvlu_sbo_chk_timers				*/
/************************************************************************/

ST_VOID mvlu_sbo_chk_timers ()
  {
ST_INT i;
MVL_SBO_CTRL *sboCtrl;
time_t currTime;
ST_DOUBLE currTimeMs;	/* current time in milliseconds	*/

  currTime = time (NULL);          	/* PORT ISSUE: time() */
  currTimeMs = sGetMsTime ();
  sboCtrl = sbo_pool;
  for (i = 0; i < MAX_NUM_SBO_PEND; ++i, ++sboCtrl)
    {
    if (sboCtrl->in_use == SD_TRUE)
      {
      if (sboCtrl->use_ms_timer)
        {
        if (sboCtrl->expire_time_ms < currTimeMs)
          sboCtrl->in_use = SD_FALSE;	/* SBO timer expired	*/
        }
      else	/* use timer in seconds	*/
        {
        if  (sboCtrl->expire_time < currTime)
          sboCtrl->in_use = SD_FALSE;	/* SBO timer expired	*/
        }
      }
    }
  }

/************************************************************************/
/*			initSboCtrl					*/
/************************************************************************/

MVL_SBO_CTRL *initSboCtrl (MVL_NET_INFO *net_info,
	ST_CHAR *sbo_name,
	ST_UINT32 sboTimeout,
	ST_BOOLEAN use_ms_timer)
  {
ST_INT i;
MVL_SBO_CTRL *sboCtrl;
time_t currTime = time (NULL);

/* OK, now see if the element is already selected			*/
  sboCtrl = sbo_pool;
  for (i = 0; i < MAX_NUM_SBO_PEND; ++i, ++sboCtrl)
    {
    if (sboCtrl->in_use == SD_TRUE && !strcmp (sboCtrl->sbo_var, sbo_name))
      {
    /* OK, already selected, make sure it is us then reset the timeout	*/
      if (sboCtrl->net_info != net_info)
        return (NULL);

      if (sboCtrl->use_ms_timer)
        sboCtrl->expire_time_ms = sGetMsTime () + sboTimeout;
      else
        sboCtrl->expire_time = currTime + sboTimeout;
      return (sboCtrl);
      }
    }

/* The protected element is not selected, find a unused SBO control 	*/
  sboCtrl = sbo_pool;
  for (i = 0; i < MAX_NUM_SBO_PEND; ++i, ++sboCtrl)
    {
    if (sboCtrl->in_use == SD_FALSE)
      break;
    }
  if (i >= MAX_NUM_SBO_PEND)
    return (NULL);

/* OK, we have a newly selected SBO control, set the parameters		*/
  sboCtrl->in_use = SD_TRUE;
  sboCtrl->use_ms_timer = use_ms_timer;
  if (sboCtrl->use_ms_timer)
    sboCtrl->expire_time_ms = sGetMsTime () + sboTimeout;
  else
    sboCtrl->expire_time = currTime + sboTimeout;
  sboCtrl->net_info = net_info;
  strcpy (sboCtrl->sbo_var, sbo_name);
  return (sboCtrl);
  }

/************************************************************************/
/*                       mvlu_sbo_chk_state				*/
/* Find the MVL_SBO_CTRL struct for this "sboName".			*/
/* Call createSboName (for UCA), mvl61850_sbo_create_sboname (for 61850)*/
/* to generate the "sboName" string to pass to this function.		*/
/************************************************************************/

MVL_SBO_CTRL *mvlu_sbo_chk_state (ST_CHAR *sboName,
				     MVL_NET_INFO *net_info)
  {
ST_INT i;
MVL_SBO_CTRL *sboCtrl;

/* See if we have selected the element					*/
  sboCtrl = sbo_pool;
  for (i = 0; i < MAX_NUM_SBO_PEND; ++i, ++sboCtrl)
    {
    if (sboCtrl->in_use == SD_TRUE && 
        !strcmp (sboCtrl->sbo_var, sboName) &&
        sboCtrl->net_info == net_info)
      {
      return (sboCtrl);
      }
    }
  return (NULL);
  }

/************************************************************************/
/************************************************************************/
/*                       createSboName					*/
/* Creates a name to return when an UCA SBO attribute is read.		*/
/* NOTE: this name may also be passed to "mvlu_sbo_chk_state"		*/
/*	to find the correct MVL_SBO_CTRL struct.			*/
/************************************************************************/
static ST_VOID createSboName (MVL_VAR_ASSOC *va, MVL_SCOPE *va_scope, 
		      ST_CHAR *dest, ST_BOOLEAN select, ST_RTREF primRef)
  {
ST_INT len;

/* if primRef is NULL, then don't perform the string table lookup	*/
  switch (va_scope->scope)
    {
    case VMD_SPEC: 
      strcpy (dest, "/");
    break;

    case DOM_SPEC: 
      strcpy (dest, va_scope->dom->name);
      strcat (dest, "/");
    break;

    case AA_SPEC:  
      strcpy (dest,"@/");
    break;
    }

/* Scope in place, now append the name of variable 			*/
  strcat (dest, va->name);

/* Now, "SBO" is the end of the name, and the reference is the index 	*/
/* to the string for the component that is being selected. So, if we 	*/
/* want the name of the protected element we just replace the "SBO" 	*/
/* with the element name.  						*/
  if (select == SD_TRUE)
    {
    len = (ST_INT) (strlen (dest) - 3);

    if (primRef != 0)
      /* primRef must be a pointer to a string; name of protected element */
      strcpy (&dest[len], (ST_CHAR *) primRef);
    else
      MVL_LOG_ERR1 ("SBO name generated '%s' is invalid", dest);
      /* DEBUG: should this function return SD_FAILURE in this case?	*/
    }
  }
/************************************************************************/
/*			mvlu_sbo_ctrl_free				*/
/* Frees the MVL_SBO_CTRL struct by setting in_use=SD_FALSE.		*/
/* Client must "Select" (i.e. read SBO) again before next "Operate".	*/
/************************************************************************/
ST_VOID mvlu_sbo_ctrl_free (MVL_SBO_CTRL *sboCtrl)
  {
  /* When writing non-SBO variables, sboCtrl == NULL.	*/
  if (sboCtrl)
    sboCtrl->in_use = SD_FALSE;	
  return;      
  }
#endif	/* defined(MVL_UCA)	*/
