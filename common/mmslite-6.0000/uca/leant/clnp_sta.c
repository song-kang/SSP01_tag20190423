/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_sta.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements the access to statistics for	*/
/*		Connectionless-mode Network Protocol (CLNP).		*/
/*									*/
/*	Note:	Compiled the whole project with CLNP_STAT define to	*/
/*		keep statistics of CLNP operation.			*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			clnp_stats_reset				*/
/*			clnp_stats_get					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/23/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "clnp_sta.h"

#if defined(CLNP_STAT)

CLNP_STATS	clnp_stats;	/* statistics recorded by the CLNP	*/

/************************************************************************/
/*			clnp_stats_reset				*/
/*----------------------------------------------------------------------*/
/* This function will clear the statistics structure.			*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if reset successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_stats_reset (ST_VOID)
{
ST_RET ret;

  ret = SD_SUCCESS;

  memset (&clnp_stats, 0, CLNP_STATS_LEN);

  return (ret);
}


/************************************************************************/
/*			clnp_stats_get					*/
/*----------------------------------------------------------------------*/
/* This function will return pointer to the statistics structure.	*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	CLNP_STAT *		pointer to stats struct			*/
/************************************************************************/
CLNP_STATS *clnp_stats_get (ST_VOID)
{
  return (&clnp_stats);
}

#else
  /* functions if CLNP_STAT is not defined */

ST_RET clnp_stats_reset (ST_VOID)
{
  return ((ST_RET) SD_SUCCESS);
}


CLNP_STATS *clnp_stats_get (ST_VOID)
{
  return ((CLNP_STATS *) NULL);
}

#endif /* record statistics defined */


