/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2003-2009, All Rights Reserved					*/
/*									*/
/* MODULE NAME : acse2dib_ll.c						*/
/* PRODUCT(S)  : MMS-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Address matching functions using global DIB_ENTRY linked list	*/
/*	(sscStackCfg->appNetwork->dibEntryList).			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			dib_match_remote_ar_ll				*/
/*				        				*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/22/09  JRB     01    Created.					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#include "acse2.h"
#include "sstackcfg.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
static ST_CHAR *thisFileName = __FILE__;
#endif


/************************************************************************/
/*			dib_match_remote_ar_ll				*/
/* Loop through "sscStackCfg->appNetwork->dibEntryList" to find a	*/
/* DIB_ENTRY matching aarq->calling_paddr & aarq->calling_ae_title.	*/
/************************************************************************/

ST_RET dib_match_remote_ar_ll (DIB_MATCH_CTRL *matchCtrl, 
	AARQ_APDU *aarq,
	DIB_ENTRY **dib_entry_out)
  {
DIB_ENTRY *de;
DIB_ENTRY *closeDe;
ST_INT match;
DIB_ENTRY    *de_list = NULL;
SSC_STACK_CFG *sscStackCfg = NULL;
ST_RET retcode;

  ACSELOG_DIB0 ("Matching Remote AR");
  closeDe = NULL;

  /* get the pointers to SISCO Stack Configuration */
  sscStackCfg = sscAccessStackCfg ();
  if (!sscStackCfg || !sscStackCfg->appNetwork)
    {
    sscReleaseStackCfg ();
    return (SD_FAILURE);
    }
  de_list = sscStackCfg->appNetwork->dibEntryList;

  retcode = SD_FAILURE;	/* assume failure, chg if match found*/
  for (de = de_list;
       de != NULL; 
       de = list_get_next (de_list, de))
    {
    ACSELOG_DIB1C ("      Remote AR '%s'", de->name);
    match = dib_cmp_remote_addr (matchCtrl, aarq, de);
    if (match == DIB_MATCH_EXACT)
      {
      *dib_entry_out = de;
      ACSELOG_DIB1C ("Exact Match: '%s'", de->name);
      retcode = SD_SUCCESS;
      break;	/* found a match, stop looping	*/
      }
    else if (match == DIB_MATCH_CLOSE && closeDe == NULL)
      closeDe = de;	/* save close de, but keep looping for exact match*/
    }

  if (closeDe != NULL)
    {
    *dib_entry_out = closeDe;
    ACSELOG_DIB1C ("Close Match: '%s'", closeDe->name);
    retcode = SD_SUCCESS;
    }
  sscReleaseStackCfg ();
  
  return (retcode);
  }

