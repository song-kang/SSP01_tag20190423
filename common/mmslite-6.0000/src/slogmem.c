/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1993-2007, All Rights Reserved 				*/
/*									*/
/* MODULE NAME : slogmem.c						*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/15/07  EJV     16    Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 07/13/05  JRB     15    slogGetMemMsg: skip timestamp on continuation*/
/*			   logs & indent 4 chars; indent hex logs.	*/
/*			   getMsgBuffer: make size based on max msg.	*/
/* 02/25/05  MDE     14    Added \n 					*/
/* 01/10/05  MDE     13    Cleanup dump a bit 				*/
/* 07/09/04  EJV     12    All systems: one time/date format in slog hdr*/
/* 03/12/04  GLB     11    Removed "thisFileName"                  	*/
/* 02/17/03  JRB     10    Del system includes (using sysincs.h).	*/
/* 05/24/01  JRB     09    Chg chk_calloc to calloc.  This always compiled*/
/*			   with !DEBUG_SISCO, screws up link.		*/
/* 10/13/00  EJV     08    Moved glbsem.h below sysincs.h.		*/
/* 09/13/99  MDE     07    Added SD_CONST modifiers			*/
/* 02/09/99  DSF     06    Thread-safe slogMem ()			*/
/*			   Remove extraneous newline in slogMemDump()	*/
/* 10/23/98  MDE     05    No blank line bet. logs if LOG_NO_HEADER_CR	*/
/* 10/16/98  DSF     04    Spelling					*/
/* 10/08/98  MDE     03    Migrated to updated SLOG interface		*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 09/18/97  DSF     01    In slogMemInit (), clear chk_debug_en before	*/
/*			   calling chk_calloc () and restore it after.	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mem_chk.h"
#include "slog.h"
#include "qmem.h"

/************************************************************************/
/* Some mysterious looking function pointers				*/
/************************************************************************/

ST_VOID *(*slogAllocBuf) (ST_INT size) = qMemAlloc;
ST_VOID (*slogFreeBuf) (ST_VOID *buf) = qMemFree;

/************************************************************************/
/*                               slogMem				*/
/************************************************************************/

ST_VOID slogMem (LOG_CTRL *lc, 
		SD_CONST ST_INT logType, 
		SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		SD_CONST ST_CHAR *SD_CONST sourceFile, 
		SD_CONST ST_INT lineNum, 
		ST_INT bufLen, SD_CONST ST_CHAR *buf)
  {
LOGMEM_ITEM *thisItem;
MEM_LOG_CTRL *mc;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

/* get pointer to memory control structure				*/

  mc = &lc->mc;

/* Check for initialization						*/

  if (!(mc->state & MEM_STATE_INIT))
    {
    slogMemInit (lc);
    }

/* Get pointer to the destination item and reset the 'next put' index	*/

  thisItem = &mc->item[mc->nextPut];

  mc->nextPut++;
  if (mc->nextPut >= mc->maxItems)	/* check for wraparound		*/
    {
    mc->nextPut = 0;

/* Check for autodump && clean						*/

    if (mc->ctrl & MEM_CTRL_AUTODUMP_EN)
      {
      slogDumpMem (lc);		/* Dump memory to log file 		*/
      slogResetMem (lc);	/* Delete all contents			*/
      }
    }

/* Check to see if we need to free the string buffer			*/

  if (thisItem->flags & LMF_USED)
    {
    (*slogFreeBuf) (thisItem->string);
    }
    
/* Set flags for this item						*/

  if (mc->ctrl & MEM_CTRL_MSG_HDR_EN)
    {
    thisItem->flags = LMF_HEADER | LMF_USED;
    thisItem->sourceFile = sourceFile;	/* save source information	*/
    thisItem->lineNum = lineNum;
    }
  else
    thisItem->flags = LMF_USED;

/* If this is a HEX log, remember that it is				*/

  if (mc->ctrl & MEM_CTRL_HEX_LOG)
    {
    thisItem->flags |= LMF_HEX;
    }

/* Save the log type information					*/

  thisItem->logType = logType;
  thisItem->logTypeStr = logTypeStr;

/* If time stamping is enabled, need to save the time			*/

  if (lc->logCtrl & LOG_TIME_EN)
    strcpy (thisItem->slogTimeText, slogTimeText);

/* Limit the string size  (bufLen includes the null)			*/

  if (bufLen > SLOG_MEM_BUF_SIZE)
    {
    bufLen = SLOG_MEM_BUF_SIZE;
    thisItem->string = (ST_CHAR *) (*slogAllocBuf) (bufLen+1);
    strncpy (thisItem->string, buf, SLOG_MEM_BUF_SIZE -1);
    thisItem->string[SLOG_MEM_BUF_SIZE-1] = 0;
    }
  else			/* no buffer size adjustments req'd		*/
    {
    thisItem->string = (ST_CHAR *) (*slogAllocBuf) (bufLen+1);
    strcpy (thisItem->string, buf);
    }

  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*	                         slogDumpMem				*/
/************************************************************************/

ST_VOID slogDumpMem (LOG_CTRL *lc)
  {
LOGMEM_ITEM 	*item;
LOGMEM_ITEM 	*tooFar;
ST_INT 	count;
FILE 		*fh;
time_t 		t;
ST_CHAR 		fname[SLOG_MAX_FNAME + 1];
MEM_LOG_CTRL 	*mc;
ST_INT 	i;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

/* get pointer to memory control structure				*/

  mc = &lc->mc;

  if (!(mc->state & MEM_STATE_INIT))
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

  fh = fopen(mc->dumpFileName, "a+");
  if (!fh)
    fh = fopen(mc->dumpFileName, "w+");

  if (fh)
    {

/* Print the dump header						*/

    t = time(NULL);
    fprintf (fh,"\n\n***********************************************************");
    fprintf (fh,"\n          MEMORY LOG LIST %s", ctime(&t));
    fprintf (fh,"***********************************************************");

    count = mc->maxItems;	/* just in case, for loop escape	*/

/* start at the last logged item, which would be 'next put'		*/
/* if the list has wrapped, otherwise somewhere down the list		*/

    item = &mc->item[mc->nextPut]; 	/* Item to log			*/
    tooFar = &mc->item[mc->maxItems];	/* 1 past end of array		*/

    for (i = 0; i < count; ++i,++item) 	/* for each possible item	*/
      { 
      if (item >= tooFar)		/* check for wrap		*/
        item = mc->item;		/* go to top			*/

      if (item->flags & LMF_USED)	/* if used ..			*/
        { 
        if (item->flags & LMF_HEADER)
          {		/* header info is to be used	*/
          if (!(lc->logCtrl & LOG_NO_HEADER_CR))
            fprintf (fh,"\n");

          if (item->logType != SLOG_CONT)
            {
            if (lc->logCtrl & LOG_TIME_EN)  /* user wants time stamp	*/
              fprintf (fh,"\n%s ", item->slogTimeText);

            if (!(lc->logCtrl & LOG_LOGTYPE_SUPPRESS))
              {
              if (item->logTypeStr != NULL)
                fprintf (fh,"%s ", item->logTypeStr);
              else
                fprintf (fh,"LogType:% 2d ", item->logType);
	      }

            if (item->sourceFile && !(lc->logCtrl & LOG_FILENAME_SUPPRESS))
              {
              slogTrimFileName (fname,item->sourceFile);
              fprintf (fh,"(%s %d) ", fname, item->lineNum);
              }
            if (!(lc->logCtrl & LOG_NO_HEADER_CR))
              fprintf (fh,"\n  ");
            }
          }
        if (item->logType == SLOG_CONT)
          fprintf (fh,"  ");

        fprintf(fh,"%s",item->string);
        } /* end if used */
      }   /* end for each possible item	*/
    fclose(fh);
    }
  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*                            slogResetMem				*/
/************************************************************************/

ST_VOID slogResetMem (LOG_CTRL *lc)
  {
LOGMEM_ITEM 	*item;
ST_INT 	i;
MEM_LOG_CTRL 	*mc;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

/* get pointer to memory control structure				*/
  mc = &lc->mc;

  if (!(mc->state & MEM_STATE_INIT))
    return;

/* just go through the list, clear the used flag && free the str buf	*/

  item = mc->item;
  for (i = 0; i < mc->maxItems; ++i, ++item)
    {
    if (item->flags & LMF_USED)
      {
      item->flags &= ~LMF_USED;
      (*slogFreeBuf) (item->string);
      }
    }  
  mc->nextPut = 0;
  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*                              slogMemInit				*/
/************************************************************************/

#define SLOG_MAX_ITEMS	(0xFF00/sizeof(LOGMEM_ITEM))

ST_VOID slogMemInit (LOG_CTRL *lc)
  {
MEM_LOG_CTRL 	*mc;
ST_UINT save_debug_en;

  mc = &lc->mc;
  if (!mc->maxItems)
    {
    fprintf (stderr,"\n Memory Logging Problem - 0 Items");
    return;
    }

  if (mc->maxItems > SLOG_MAX_ITEMS)
    {
    fprintf (stderr,"\n Memory Logging Problem - Too Many Items");
    mc->maxItems = SLOG_MAX_ITEMS;
    }

  if (qMemInit (mc->maxItems))
    {
    fprintf (stderr,"\n QMEM Initialization Problem");
    mc->maxItems = 0;
    lc->logCtrl &= ~LOG_MEM_EN;
    }
  else
    {
/* Allocate a zero'd memory block for the item control list		*/
    save_debug_en = chk_debug_en;
    chk_debug_en = 0;
    mc->item = 
         (LOGMEM_ITEM *) calloc (mc->maxItems, sizeof (LOGMEM_ITEM));
    chk_debug_en = save_debug_en;
    }

  mc->state |= MEM_STATE_INIT;
  }

/************************************************************************/
/*		               slogGetMemMsg				*/
/************************************************************************/

/* Need buffer big enough for max msg plus extra space for timestamp.	*/
/* 50 should leave plenty of safety margin.				*/
static ST_CHAR getMsgBuffer[SLOG_MEM_BUF_SIZE + 50];

ST_CHAR *slogGetMemMsg (LOG_CTRL *lc, SD_CONST ST_INT msgNum)
  {
ST_INT 	i;
MEM_LOG_CTRL 	*mc;
ST_INT 	index;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

/* get pointer to memory control structure				*/

  mc = &lc->mc;
  if (!(mc->state & MEM_STATE_INIT) || msgNum > mc->maxItems)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return (NULL);
    }

/* FInd the start index in the item array				*/

  index = mc->nextPut;	       /* if wrapped, next put is oldest item	*/
  if (!(mc->item[index].flags & LMF_USED)) /* but if not wrapped	*/
    index = 0;				   /* oldest is item 0		*/

/* Find the 'msgNum' item index (msgNum == 0 means oldest item)		*/

  for (i = 0; i < msgNum; ++i)
    { 
    ++index;
    if (index >= mc->maxItems)	/* check for wraparound			*/
      index = 0;
    } 

  if ((mc->item[index].flags & LMF_USED)) /* if used ... 		*/
    {
/* too easy! return (mc->item[index].string);  				*/

    if (lc->logCtrl & LOG_TIME_EN)
      {
      if (mc->item[index].flags & LMF_HEADER)
        {
        if (mc->item[index].logType == SLOG_CONT)
          sprintf (getMsgBuffer,"    %s", mc->item[index].string);	/* indent 4 char*/
        else
          sprintf (getMsgBuffer,"%s: %s", &mc->item[index].slogTimeText[11],
                                         mc->item[index].string);
        }
      else if (mc->item[index].flags & LMF_HEX)
        sprintf (getMsgBuffer,"    %s", mc->item[index].string);	/* indent 4 char*/
      else 
        sprintf (getMsgBuffer,"             %s", mc->item[index].string);
      }
    else
      {
      if (mc->item[index].logType == SLOG_CONT)
        sprintf (getMsgBuffer,"    %s", mc->item[index].string);	/* indent 4 char*/
      else
        sprintf (getMsgBuffer,"%s", mc->item[index].string);
      }

    S_UNLOCK_UTIL_RESOURCES ();
    return (getMsgBuffer);
    }

  S_UNLOCK_UTIL_RESOURCES ();
  return (NULL);
  }

/************************************************************************/
/*		               slogGetMemCount				*/
/************************************************************************/

ST_INT slogGetMemCount (LOG_CTRL *lc)
  {
MEM_LOG_CTRL 	*mc;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

/* get pointer to memory control structure				*/

  mc = &lc->mc;
  if (!(mc->state & MEM_STATE_INIT))
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return (0);
    }

  if (mc->item[mc->nextPut].flags & LMF_USED) /* if wrapped,		*/
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return (mc->maxItems);
    }
  else
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return (mc->nextPut);
    }
  }

