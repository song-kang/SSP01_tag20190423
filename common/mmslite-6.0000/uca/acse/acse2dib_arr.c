/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2003-2009, All Rights Reserved					*/
/*									*/
/* MODULE NAME : acse2dib_arr.c						*/
/* PRODUCT(S)  : MMS-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Address matching functions using global DIB_ENTRY arrays	*/
/*	(loc_dib_table and rem_dib_table).				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			dib_match_remote_ar				*/
/*			dib_match_local_ar				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/26/09  JRB     01    Created from code moved here from acse2dib.c	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#include "acse2.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
static ST_CHAR *thisFileName = __FILE__;
#endif

/************************************************************************/
/*			dib_match_remote_ar				*/
/************************************************************************/

ST_RET dib_match_remote_ar (DIB_MATCH_CTRL *matchCtrl, 
	AARQ_APDU *aarq,
	DIB_ENTRY **dib_entry_out)
  {
ST_INT i;
DIB_ENTRY *de;
DIB_ENTRY *closeDe;
ST_INT remMatch;

  ACSELOG_DIB0 ("Matching Remote AR");
  closeDe = NULL;
  for (i = 0; i < num_rem_dib_entries; ++i)
    {
    de = &rem_dib_table[i];
    ACSELOG_DIB1C ("      Remote AR '%s'", de->name);
    remMatch = dib_cmp_remote_addr (matchCtrl, aarq, de);
    if (remMatch == DIB_MATCH_EXACT)
      {
      *dib_entry_out = de;
      ACSELOG_DIB1C ("Exact Match: '%s'", de->name);
      return (SD_SUCCESS);
      }
    if (remMatch == DIB_MATCH_CLOSE && closeDe == NULL)
      closeDe = de;
    }

  if (closeDe != NULL)
    {
    *dib_entry_out = closeDe;
    ACSELOG_DIB1C ("Close Match: '%s'", closeDe->name);
    return (SD_SUCCESS);
    }
  
  return (SD_FAILURE);
  }

/************************************************************************/
/*			dib_match_local_ar				*/
/************************************************************************/

ST_RET dib_match_local_ar (DIB_MATCH_CTRL *matchCtrl, 
	AARQ_APDU *aarq,
	DIB_ENTRY **dib_entry_out)
  {
ST_INT i;
DIB_ENTRY *de;
DIB_ENTRY *closeDe;
ST_INT locMatch;

  ACSELOG_DIB0 ("Matching Local AR");
  closeDe = NULL;
  for (i = 0; i < num_loc_dib_entries; ++i)
    {
    de = &loc_dib_table[i];
    ACSELOG_DIB1C ("      Local AR '%s'", de->name);
    locMatch = dib_cmp_local_addr (matchCtrl,aarq, de);
    if (locMatch == DIB_MATCH_EXACT)
      {
      ACSELOG_DIB1C ("Exact Match: '%s'", de->name);
      *dib_entry_out = de;
      return (SD_SUCCESS);
      }

    if (locMatch == DIB_MATCH_CLOSE && closeDe == NULL)
      closeDe = de;
    }

  if (closeDe != NULL)
    {
    ACSELOG_DIB1C ("Close Match: '%s'", de->name);
    *dib_entry_out = closeDe;
    return (SD_SUCCESS);
    }
  
  return (SD_FAILURE);
  }

