/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 All Rights Reserved					*/
/*									*/
/* MODULE NAME : mvl_ijou.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to automatically build a ReadJournal response,	*/
/*	assuming the Journal is modeled as a linked list of journal	*/
/*	entries.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	u_mvl_start_jread						*/
/*	u_mvl_get_next_jread_entry					*/
/*	u_mvl_free_jread_entry						*/
/*	u_mvl_end_jread							*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 04/13/10  JRB	   Fix last chg (comma used incorrectly in loops)*/
/* 02/25/10  JRB	   Chk je!=NULL in loops (4 places).		*/
/* 02/04/10  JRB     08    Chg all printf to MVL_LOG.., add back thisFil..*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 04/14/00  JRB     06    Lint cleanup.				*/
/* 01/21/00  MDE     05    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 11/16/98  MDE     03    Renamed internal functions (prefix '_')	*/
/* 10/29/98  JRB     02    Delete unnecessary includes.			*/
/* 10/09/98  JRB     01    Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mvl_defs.h"
#include "mvl_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* All the functions below, used for building the ReadJournal response,	*/
/* assume the Journal is modeled as a linked list of journal entries.	*/
/* They use SISCO functions to search the linked list. The user code	*/
/* must use the same linked list functions to add journal entries to	*/
/* the linked list (or remove them from the linked list).		*/
/************************************************************************/

/************************************************************************/
/* Static functions for building the JREAD response.			*/
/************************************************************************/
static MVL_JOURNAL_ENTRY *find_je_start_time_forward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo);
static MVL_JOURNAL_ENTRY *find_je_start_time_backward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo);

static MVL_JOURNAL_ENTRY *find_je_start_entry (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo);

static MVL_JOURNAL_ENTRY *find_je_stop_forward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *start_je);
static MVL_JOURNAL_ENTRY *find_je_stop_backward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *start_je);

static MVL_JOURNAL_ENTRY *find_je_start_after_forward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *start_je,
                             MVL_JOURNAL_ENTRY *stop_je);
static MVL_JOURNAL_ENTRY *find_je_start_after_backward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *start_je,
                             MVL_JOURNAL_ENTRY *stop_je);

static ST_BOOLEAN check_je_list_of_var (JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *je);

/************************************************************************/
/* Global variables for building the JREAD response.			*/
/************************************************************************/
MVL_JOURNAL_ENTRY *je_array [MAX_JREAD_RESP_ENTRIES];	/* array or pointers*/
ST_INT je_array_index;		/* index into "je_array"	*/
ST_INT je_array_num_entries;	/* # of entries in "je_array"	*/
ST_BOOLEAN jread_more_follows;
MVL_JOURNAL_ENTRY *stop_je;	/* journal entry to stop with. 	*/
				/* NOT INCLUDED in response.	*/

/************************************************************************/
/************************************************************************/
ST_RET u_mvl_start_jread (MVL_JOURNAL_CTRL *jCtrl, 
			  JREAD_REQ_INFO *reqInfo, 
			  MVL_JREAD_BUF_CTRL *bufCtrl,
			  ST_VOID **usrHandleOut)
  {
MVL_JOURNAL_ENTRY *je;
MVL_JOURNAL_ENTRY *start_je =jCtrl->entries;	/* First entry that passes*/
						/* "Range Start Spec."	*/
ST_BOOLEAN backwards = SD_FALSE;
ST_INT k;

  /* Init global variables.	*/
  je_array_index = 0;
  je_array_num_entries = 0;
  jread_more_follows = SD_TRUE;		/* Assume "more_follows".	*/

  if (reqInfo->range_start_pres
      &&  reqInfo->start_tag == 0	/* Use Starting time	*/
      &&  reqInfo->start_time.form != MMS_BTOD6)
     {
     MVL_LOG_ERR0 ("ReadJournal request Starting Time illegal. Must be Btime6.");
     return (SD_FAILURE);
     }
  if (reqInfo->range_stop_pres
      &&  reqInfo->stop_tag == 0	/* Use Stop time	*/
      &&  reqInfo->end_time.form != MMS_BTOD6)
    {
    MVL_LOG_ERR0 ("ReadJournal request End Time illegal. Must be Btime6.");
    return (SD_FAILURE);
    }
  if (reqInfo->sa_entry_pres
      &&  reqInfo->time_spec.form != MMS_BTOD6)
    {
    MVL_LOG_ERR0 ("ReadJournal request Time Specification illegal. Must be Btime6.");
    return (SD_FAILURE);
    }

  /* If "Number of Entries" is used and is negative, do reverse order.	*/
  if (reqInfo->range_stop_pres
      &&  reqInfo->stop_tag != 0	/* Use "Number of Entries"	*/
      &&  reqInfo->num_of_entries < 0)	/* Negative			*/
    backwards = SD_TRUE;	/* return entries in reverse order	*/

  /* Process "Range Start Specification"	*/
  if (reqInfo->range_start_pres)
    {
    if (reqInfo->start_tag == 0)	/* Use Starting time	*/
      {
      if (reqInfo->range_stop_pres && reqInfo->stop_tag == 0)
        {		/* Use "Ending Time"	*/
        if (reqInfo->start_time.day > reqInfo->end_time.day  ||
            (reqInfo->start_time.day == reqInfo->end_time.day &&
             reqInfo->start_time.ms > reqInfo->end_time.ms) )
          {		/* Start time > End time	*/
          backwards = SD_TRUE;	/* return entries in reverse order	*/
          }
        }

      if (backwards)
        {
        /* Set "start_je" to the first entry with time <= start time.	*/
        /* Could be NULL if no entry found.				*/
        start_je = find_je_start_time_backward (jCtrl, reqInfo);
        }
      else
        {
        /* Set "start_je" to the first entry with time >= start time.	*/
        /* Could be NULL if no entry found.				*/
        start_je = find_je_start_time_forward (jCtrl, reqInfo);
        }
      if (start_je == NULL)
        return (SD_SUCCESS);	/* None found. Return 0 entries	*/
      }
    else				/* Use "Starting Entry"	*/
      {
      start_je = find_je_start_entry (jCtrl, reqInfo);
      if (start_je == NULL)
        return (SD_FAILURE);	/* If no match, must send Negative resp.*/

      if (reqInfo->range_stop_pres && reqInfo->stop_tag == 0)
        {		/* Use "Ending Time"	*/
        if (start_je->occur_time.day > reqInfo->end_time.day  ||
            (start_je->occur_time.day == reqInfo->end_time.day &&
             start_je->occur_time.ms > reqInfo->end_time.ms) )
          {		/* Entry time > End time	*/
          backwards = SD_TRUE;	/* return entries in reverse order	*/
          }
        }
      }
    }	/* End processing "Range Start Specification".	*/
  else
    start_je = jCtrl->entries;


  /* Process "Range Stop Specification"	*/
  /* Set "stop_je" pointing to first entry past the "stop spec".	*/
  if (backwards)
    {			/* Search BACKWARD thru linked list	*/
    stop_je = find_je_stop_backward (jCtrl, reqInfo, start_je);
    }
  else
    {			/* Search FORWARD thru linked list	*/
    stop_je = find_je_stop_forward (jCtrl, reqInfo, start_je);
    }

  /* Process "Entry to Start After"	*/
  if (reqInfo->sa_entry_pres)
    {		/* "Entry to Start After" specified.	*/
    /* Scan linked list for "Start After Entry" and "Start After Time",	*/
    /* up to "Range Stop", if specified.				*/
    /* Adjust "start_entry" accordingly.				*/
    if (backwards)
      {			/* Search BACKWARD thru linked list	*/
      start_je = find_je_start_after_backward (jCtrl, reqInfo, start_je, stop_je);
      }
    else
      {			/* Search FORWARD thru linked list	*/
      start_je = find_je_start_after_forward (jCtrl, reqInfo, start_je, stop_je);
      }			/* End "Search FORWARD thru linked list"	*/
    }
  else
    ;		/* "Entry to Start After" NOT specified. Do nothing.	*/
  /* End processing of "Entry to Start After"	*/
    
  /* Now find all entries that pass the "List of Variables" filter	*/
  /* and add them to an array of Journal entries to send in response.	*/
  if (backwards)
    {
    for (je = start_je; 
         je != stop_je && je != NULL;
         je = (MVL_JOURNAL_ENTRY *) list_find_prev ((DBL_LNK *) jCtrl->entries, (DBL_LNK *) je))
      {
      if (je_array_num_entries >= MAX_JREAD_RESP_ENTRIES)
        break;				/* Assume still more entries.	*/

      /* If "List of Variables" specified, check if this entry matches.*/
      if (check_je_list_of_var (reqInfo, je))
        je_array[je_array_num_entries++] = je;	/* Matched.	*/
      }	/* end "for"	*/
    }
  else
    {	/* "!backwards"	*/
    for (je = start_je; 
         je != stop_je && je != NULL;
         je = (MVL_JOURNAL_ENTRY *) list_get_next (jCtrl->entries, je))
      {
      if (je_array_num_entries >= MAX_JREAD_RESP_ENTRIES)
        break;				/* Assume still more entries.	*/

      /* If "List of Variables" specified, check if this entry matches.*/
      if (check_je_list_of_var (reqInfo, je))
        je_array[je_array_num_entries++] = je;	/* Matched.	*/
      }	/* end "for"	*/
    }	/* End "!backwards"	*/
  if (je == stop_je)		/* End of linked list.	*/
    jread_more_follows = SD_FALSE;	/* No more entries	*/

/* We need to tell MVL how many entries and variables we have */
  bufCtrl->numVarInfo = 0;
  bufCtrl->numUsrEntries = je_array_num_entries;
  for (k = 0; k < je_array_num_entries; k++)
    {
    je = je_array [k];
    if (je->entry_form_tag == JE_FORM_DATA && je->ef.data.list_of_var_pres)
      {		/* entry form is 'data' */
      bufCtrl->numVarInfo += je->ef.data.num_of_var;
      }
    }

/* By default, MVL assumes "u_mvl_get_next_jread_entry" returns a	*/
/* pointer (*mvlJeDest) to persistent data.				*/
/* This is a valid assumption for the linked list implementation	*/
/* in this sample code. If you cannot point to persistent data,		*/
/* you must compile the MVL library and this code with			*/
/* MVL_JREAD_*_DYNAMIC defined and adjust the allocation to allow	*/
/* MVL enough space to copy the data.					*/ 

#if defined (MVL_JREAD_ENTRYID_DYNAMIC)
  bufCtrl->entryIdBuf = M_MALLOC (MSMEM_GEN, 1000);
#endif
#if defined (MVL_JREAD_VARTAG_DYNAMIC)
  bufCtrl->varTagBuf = M_MALLOC (MSMEM_GEN, 1000);
#endif
#if defined (MVL_JREAD_ANNOTATION_DYNAMIC)
  bufCtrl->annotationBuf = M_MALLOC (MSMEM_GEN, 1000);
#endif

  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
ST_RET u_mvl_get_next_jread_entry (MVL_JOURNAL_CTRL *jCtrl,
				   ST_VOID *usrHandle, 
				   ST_BOOLEAN *moreFollowsOut, 
				   MVL_JOURNAL_ENTRY **mvlJeDest)
  {
  if (je_array_index >= je_array_num_entries)
    return (SD_FAILURE);	/* Should never happen.	*/

  /* If last in "je_array", use "real more_follows value".	*/
  if (je_array_index + 1 == je_array_num_entries)
    *moreFollowsOut = jread_more_follows;
  else
    *moreFollowsOut = SD_TRUE;	/* Not last in array, so always more.	*/

  *mvlJeDest = je_array[je_array_index];
  je_array_index++;		/* Increment array index to next entry.	*/
  return (SD_SUCCESS);
  }

/************************************************************************/

ST_VOID u_mvl_free_jread_entry (MVL_JOURNAL_CTRL *jCtrl, 
				ST_VOID *usrHandle, 
				MVL_JOURNAL_ENTRY *mvlJe)
  {
  }

/************************************************************************/

ST_VOID u_mvl_end_jread (MVL_JOURNAL_CTRL *jCtrl,
			 ST_VOID *usrHandle, 
			 MVL_JREAD_BUF_CTRL *bufCtrl,
			 JREAD_REQ_INFO *reqInfo)
  {

/* Free up buffers allocated in "u_mvl_start_jread".			*/
#if defined (MVL_JREAD_ENTRYID_DYNAMIC)
  M_FREE (MSMEM_GEN, bufCtrl->entryIdBuf);
#endif
#if defined (MVL_JREAD_VARTAG_DYNAMIC)
  M_FREE (MSMEM_GEN, bufCtrl->varTagBuf);
#endif
#if defined (MVL_JREAD_ANNOTATION_DYNAMIC)
  M_FREE (MSMEM_GEN, bufCtrl->annotationBuf);
#endif
  }

/************************************************************************/
/************************************************************************/
static MVL_JOURNAL_ENTRY *find_je_start_time_forward (MVL_JOURNAL_CTRL *jCtrl, 
			  JREAD_REQ_INFO *reqInfo)
  {
MVL_JOURNAL_ENTRY *je;

  for (je = jCtrl->entries;
       je != NULL;
       je = (MVL_JOURNAL_ENTRY *) list_get_next (jCtrl->entries, je))
    {		/* Search entire linked list	*/
    if (je->occur_time.day > reqInfo->start_time.day  ||
        (je->occur_time.day == reqInfo->start_time.day &&
         je->occur_time.ms >= reqInfo->start_time.ms) )
      break;	/* entry time >= start time. Start here.	*/
    }	/* End "for" loop for time spec	*/
  return (je);		/* May be NULL if not found.	*/
  }

/************************************************************************/
/************************************************************************/
static MVL_JOURNAL_ENTRY *find_je_start_time_backward (MVL_JOURNAL_CTRL *jCtrl, 
			  JREAD_REQ_INFO *reqInfo)
  {
MVL_JOURNAL_ENTRY *je;

  for (je = (MVL_JOURNAL_ENTRY *) list_find_last ((DBL_LNK *) jCtrl->entries);
       je != NULL;
       je = (MVL_JOURNAL_ENTRY *) list_find_prev ((DBL_LNK *) jCtrl->entries, (DBL_LNK *) je))
    {		/* Search entire linked list	*/
    if (je->occur_time.day < reqInfo->start_time.day  ||
        (je->occur_time.day == reqInfo->start_time.day &&
         je->occur_time.ms <= reqInfo->start_time.ms) )
      break;	/* entry time <= start time. Start here.	*/
    }	/* End "for" loop for time spec	*/
  return (je);		/* May be NULL if not found.		*/
  }

/************************************************************************/
/************************************************************************/
static MVL_JOURNAL_ENTRY *find_je_start_entry (MVL_JOURNAL_CTRL *jCtrl, 
			  JREAD_REQ_INFO *reqInfo)
  {
MVL_JOURNAL_ENTRY *je;
  for (je = jCtrl->entries;
       je != NULL;
       je = (MVL_JOURNAL_ENTRY *) list_get_next (jCtrl->entries, je))
    {
    if (je->entry_id_len == reqInfo->start_entry_len &&
        !memcmp (je->entry_id, reqInfo->start_entry, je->entry_id_len))
      return (je);	/* entry id == Starting Entry. Start here.	*/
    }	/* End "for" loop for "Starting Entry"	*/
  return (NULL);
  }

/************************************************************************/
/************************************************************************/
static MVL_JOURNAL_ENTRY *find_je_stop_forward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *start_je)
  {
MVL_JOURNAL_ENTRY *je;
MVL_JOURNAL_ENTRY *stop_je;
ST_INT num_entries;


  if (reqInfo->range_stop_pres)
    {
    for (je = start_je, num_entries = 0; 
         je != NULL;
         je = (MVL_JOURNAL_ENTRY *) list_get_next (jCtrl->entries, je),
         num_entries++)
      {
      if (reqInfo->stop_tag == 0)
        {		/* Use "Ending Time"	*/
        if (je->occur_time.day > reqInfo->end_time.day  ||
            (je->occur_time.day == reqInfo->end_time.day &&
             je->occur_time.ms > reqInfo->end_time.ms) )
          {		/* Entry time > End time	*/
          break;	/* Found it. Done.	*/
          }
        }
      else
        {		/* Use "Number of entries"	*/
        if (num_entries >= reqInfo->num_of_entries)
          break;	/* Found it. Done.	*/
        }
      }	/* end "for"	*/
    stop_je = je;	/* May be NULL if not found.	*/
    }
  else
    stop_je = NULL;	/* No stop spec. Set to NULL (i.e. end of list)	*/

  return (stop_je);
  }

/************************************************************************/
/************************************************************************/
static MVL_JOURNAL_ENTRY *find_je_stop_backward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *start_je)
  {
MVL_JOURNAL_ENTRY *je;
MVL_JOURNAL_ENTRY *stop_je;
ST_INT num_entries;

  if (reqInfo->range_stop_pres)
    {
    for (je = start_je, num_entries = 0; 
         je != NULL;
         je = (MVL_JOURNAL_ENTRY *) list_find_prev ((DBL_LNK *) jCtrl->entries, (DBL_LNK *) je),
         num_entries++)
      {
      if (reqInfo->stop_tag == 0)
        {		/* Use "Ending Time"	*/
        if (je->occur_time.day < reqInfo->end_time.day  ||
            (je->occur_time.day == reqInfo->end_time.day &&
             je->occur_time.ms < reqInfo->end_time.ms) )
          {		/* Entry time < End time	*/
          break;	/* Found it. Done.	*/
          }
        }
      else
        {	/* Use "Number of entries" ("0 -" converts neg to pos)	*/
        if (num_entries >= (0 - reqInfo->num_of_entries) )
          break;	/* Found it. Done.	*/
        }
      }	/* end "for"	*/
    stop_je = je;	/* May be NULL if not found.	*/
    }
  else
    stop_je = NULL;	/* No stop spec. Set to NULL (i.e. end of list)	*/

  return (stop_je);
  }

/************************************************************************/
/************************************************************************/
static MVL_JOURNAL_ENTRY *find_je_start_after_forward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *start_je,
                             MVL_JOURNAL_ENTRY *stop_je)
  {
MVL_JOURNAL_ENTRY *je;
MVL_JOURNAL_ENTRY *start_after_entry_je=NULL;	/* First entry that passes*/
						/* "Start After Entry Spec."*/
MVL_JOURNAL_ENTRY *start_after_time_je=NULL;	/* First entry that passes*/
						/* "Start After Time Spec."*/

  /* The variables "start_after_entry_je" and/or "start_after_time_je"*/
  /* will be set to non-NULL values if they are found.		*/
  /* If one of them is found, adjust "start_entry" accordingly.	*/
  for (je = start_je; 
       je != stop_je && je != NULL;
       je = (MVL_JOURNAL_ENTRY *) list_get_next (jCtrl->entries, je))
    {
    /* Check for matching "entry_spec"	*/
    if (je->entry_id_len == reqInfo->entry_spec_len &&
        !memcmp (je->entry_id, reqInfo->entry_spec, je->entry_id_len))
      {	/* entry spec matches	*/
      start_after_entry_je = je;
      break;	/* Done checking.	*/
      }

    /* If haven't found matching "entry_spec" yet, check "time_spec".	*/
    if (start_after_time_je == NULL)
      {				/* "time_spec" match not found yet.	*/
      if (je->occur_time.day > reqInfo->time_spec.day  ||
          (je->occur_time.day == reqInfo->time_spec.day &&
           je->occur_time.ms > reqInfo->time_spec.ms) )
        {	/* entry time > time spec.	*/
        /* Save this but keep checking for matching "entry_spec".	*/
        start_after_time_je = je;	/* DO NOT "break"		*/
        }
      }
    }	/* end "for"	*/

  /* If found match on Start After Entry, start with NEXT entry.	*/
  /* If only found match on Start After Time, start with THAT entry.*/
  if (start_after_entry_je)
    start_je = (MVL_JOURNAL_ENTRY *) list_get_next (jCtrl->entries,
               start_after_entry_je);
  else if (start_after_time_je)
    start_je = start_after_time_je;
  else
    start_je = NULL;	/* Didn't find either. Return 0 entries.*/
  return (start_je);
  }

/************************************************************************/
/************************************************************************/
static MVL_JOURNAL_ENTRY *find_je_start_after_backward (MVL_JOURNAL_CTRL *jCtrl, 
			     JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *start_je,
                             MVL_JOURNAL_ENTRY *stop_je)
  {
MVL_JOURNAL_ENTRY *je;
MVL_JOURNAL_ENTRY *start_after_entry_je=NULL;	/* First entry that passes*/
						/* "Start After Entry Spec."*/
MVL_JOURNAL_ENTRY *start_after_time_je=NULL;	/* First entry that passes*/
						/* "Start After Time Spec."*/

  /* The variables "start_after_entry_je" and/or "start_after_time_je"	*/
  /* will be set to non-NULL values if they are found.			*/
  /* If one of them is found, adjust "start_entry" accordingly.		*/
  for (je = start_je; 
       je != stop_je && je != NULL;
       je = (MVL_JOURNAL_ENTRY *) list_find_prev ((DBL_LNK *) jCtrl->entries, (DBL_LNK *) je))
    {
    /* Check for matching "entry_spec"	*/
    if (je->entry_id_len == reqInfo->entry_spec_len &&
        !memcmp (je->entry_id, reqInfo->entry_spec, je->entry_id_len))
      {	/* entry spec matches	*/
      start_after_entry_je = je;
      break;	/* Done checking.	*/
      }

    /* If haven't found matching "entry_spec" yet, check "time_spec".	*/
    if (start_after_time_je == NULL)
      {				/* "time_spec" match not found yet.	*/
      if (je->occur_time.day < reqInfo->time_spec.day  ||
          (je->occur_time.day == reqInfo->time_spec.day &&
           je->occur_time.ms < reqInfo->time_spec.ms) )
        {	/* entry time < time spec.	*/
        /* Save this but keep checking for matching "entry_spec".	*/
        start_after_time_je = je;	/* DO NOT "break"		*/
        }
      }
    }	/* end "for"	*/

  /* If found match on Start After Entry, start with NEXT entry.	*/
  /* If only found match on Start After Time, start with THAT entry.*/
  if (start_after_entry_je)
    start_je = (MVL_JOURNAL_ENTRY *) list_find_prev ((DBL_LNK *) jCtrl->entries,
               (DBL_LNK *) start_after_entry_je);
  else if (start_after_time_je)
    start_je = start_after_time_je;
  else
    start_je = NULL;	/* Didn't find either. Return 0 entries.*/
  return (start_je);
  }

/************************************************************************/
/************************************************************************/
static ST_BOOLEAN check_je_list_of_var (JREAD_REQ_INFO *reqInfo, 
                             MVL_JOURNAL_ENTRY *je)
  {
ST_BOOLEAN retcode = SD_FALSE;	/* No match.	*/
ST_INT j, k;
ST_CHAR **list_of_var;

  if (reqInfo->list_of_var_pres)
    {
    list_of_var = (ST_CHAR **) (reqInfo + 1);
    
    if (je->entry_form_tag == JE_FORM_DATA
        && je->ef.data.list_of_var_pres)
      {
      for (j = 0; j < reqInfo->num_of_var;  j++)
        {
        for (k = 0; k < je->ef.data.num_of_var; k++)
          {
          if (!strcmp (je->ef.data.list_of_var[k].var_tag, list_of_var [j]))
            retcode = SD_TRUE;	/* Matched.	*/
          }	/* end "for" loop	*/
        }	/* end "for" loop	*/
      }
    }	/* End "if (reqInfo->list_of_var_pres)"	*/
  else
    {	/* No "List of Variables" to check. This one must be OK.*/
    retcode = SD_TRUE;		/* Just as good as a match.	*/
    }
  return (retcode);
  }

