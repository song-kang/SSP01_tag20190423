/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1993-2012, All Rights Reserved 				*/
/*									*/
/* MODULE NAME : slogfil.c						*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/12/12  EJV           slogSetHdr: added slogHdrMaxSize parameter.	*/ 
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/24/10  JRB	   Chk src!=NULL BEFORE calling strlen.		*/
/* 03/27/08  EJV     22    Elim warning from previous changes.		*/
/* 03/27/08  EJV     21    Use S_MAX_PATH instead of MAX_PATH.		*/
/*                         slog_get_index_file_name: added destLen param*/
/*                         and changed to return result.		*/
/* 10/08/07  EJV     20    Moved MAX_PATH define to sysincs.h		*/
/* 01/15/07  EJV     19    Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 02/13/06  DSF     18    Migrate to VS.NET 2005			*/
/* 10/31/05  MDE     17    Tweaked index tile name creation		*/
/* 09/23/05  MDE     16    Tweaked size of idxFileName in     		*/
/* 09/14/05  DSF     15    Increased size of idxFileName in     	*/
/* 08/26/05  MDE     14    Fixed startup w/wo 				*/
/* 08/10/05  MDE     13    Added index file, wrap cleanup, etc.		*/
/* 06/07/04  EJV     12    slogFil: chg state if reopen err (HARD_FLUSH)*/
/* 05/13/04  EJV     11    Moved log header formatting to slogSetHdr.	*/
/* 03/11/04  GLB     10    Remove "thisFileName"                    	*/
/* 02/20/03  JRB     09    Del PSOS code.				*/
/* 10/13/00  EJV     08    Moved glbsem.h below sysincs.h.		*/
/* 08/22/00  KCR     07    Added FIL_CTRL_NO_LOG_HDR			*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 04/14/99  MDE     05    Removed unnecessary include files		*/
/* 10/23/98  MDE     04    No blank line bet. logs if LOG_NO_HEADER_CR	*/
/* 10/16/98  DSF     03    Spelling					*/
/* 10/08/98  MDE     02    Migrated to updated SLOG interface		*/
/* 10/06/97  DSF     01    Added thisFileName				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#if defined (_WIN32)
#pragma warning(disable : 4996)
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include <errno.h>
#if defined(_WIN32)
#include "io.h"      
#endif

#include "slog.h"

/************************************************************************/

/* Do not change w/o testing */
#define OLDEST_STRING  "          OLDEST DATA STARTS BELOW"

/************************************************************************/
/* Local function prototypes						*/
/************************************************************************/

static ST_VOID  printWipe 	      (FILE *fh);
static ST_VOID  printWrap        (FILE *fh);
static ST_VOID  printStart       (FILE *fh);
static ST_VOID  slogFileErrorPrint(ST_CHAR *text, ST_CHAR *fname);
static ST_LONG  findOldestMessage(LOG_CTRL	*lc, FILE *fh);
static ST_INT copy_named_files (LOG_CTRL *lc, SD_CONST ST_CHAR *SD_CONST dest, 
		      SD_CONST ST_CHAR *SD_CONST src);
static ST_INT copy_named_files_ex (LOG_CTRL *lc, SD_CONST ST_CHAR *SD_CONST dest, 
	SD_CONST ST_CHAR *SD_CONST src);
static ST_VOID chk_slog_service (LOG_CTRL	*lc);

/************************************************************************/
/*                              slogFile				*/
/************************************************************************/

ST_VOID slogFile (LOG_CTRL *lc, SD_CONST ST_INT logType, 
		SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		SD_CONST ST_CHAR *SD_CONST sourceFile, 
		SD_CONST ST_INT lineNum, 
		SD_CONST ST_INT bufLen, SD_CONST ST_CHAR *buf)

  {
FILE *fh;
struct stat fileStat;
ST_LONG seekPos;
ST_LONG currPos;
ST_ULONG fileSize;
ST_CHAR slogHdr[SLOG_MAX_HDR];  
/*ST_INT wrap;								*/

  if (!(lc->fc.state & FIL_STATE_OPEN))	/* if not already opened	*/
    {
  /* Need to open the file						*/
    if (lc->fc.ctrl & FIL_CTRL_NO_APPEND)  /* If overwrite is requested	*/
      fh = NULL;
    else
      lc->fc.fp = fh = fopen (lc->fc.fileName,"r+");

    if (!fh)		/* File must not exist, or overwrite requested	*/
      {			/* try creating it				*/
      lc->fc.fp = fh = fopen (lc->fc.fileName,"w+");
      if (!fh)
        {
        slogFileErrorPrint ("OPEN",lc->fc.fileName);
        return;
        }
      }

  /* We have successfully opened the log file */
    lc->fc.state = FIL_STATE_OPEN;
    if (lc->fc.ctrl & FIL_CTRL_SETBUF_EN)  /* If setbuf desired		*/
      setbuf (fh,NULL);  		  /* no buffering, please	*/

  /* If appending, need to get to the start write position */
    if ((lc->fc.ctrl & FIL_CTRL_NO_APPEND) == 0)
      {
  /* Check the current file size, see if we can just append */
      if (fstat (fileno(fh), &fileStat))
        {
        slogFileErrorPrint ("SEEK",lc->fc.fileName);
        return;
	}
      fileSize = fileStat.st_size;
      if (fileSize < lc->fc.maxSize)
        seekPos = (ST_LONG) fileSize;
      else
        {
        seekPos = findOldestMessage (lc, fh);
        lc->fc.state |= FIL_STATE_NEED_WIPE; /* flag to show we wrapped */
	}
      if (fseek (fh,seekPos,SEEK_SET))
        {
        slogFileErrorPrint ("SEEK",lc->fc.fileName);
        return;
        }
      }

/* Print the 'started' message						*/
    if (!(lc->fc.ctrl & FIL_CTRL_NO_LOG_HDR)) /* log file header */
      printStart (fh);
    }/* File not opened							*/
  else 
    { 	/* The file has already been opened for logging			*/
    fh = lc->fc.fp;
    if (lc->fc.ctrl & FIL_CTRL_WRAP_EN) /* if wrap allowed		*/
      {    

/* Check to see the position of the file to see if we need to wrap it	*/

      currPos = ftell (fh);
      if (currPos == -1L)
        {
        slogFileErrorPrint ("TELL",lc->fc.fileName);
        return;
        }

/* Check to see if we need to wrap - currPos is where we would write	*/

      if (((ST_ULONG) currPos > lc->fc.maxSize))
        {

/* Yep, need to wrap - cut off the rest of the file to avoid trailing 	*/

#if defined(MSDOS) || defined(__OS2__) || defined(_WIN32)
        if (chsize (fileno(fh), currPos))
  	  {
          slogFileErrorPrint ("CHSIZE",lc->fc.fileName);
          }
#else
        if (ftruncate (fileno(fh), currPos))
	  {
          slogFileErrorPrint ("FTRUNCATE",lc->fc.fileName);
	  }
#endif

/* Seek to start of file						*/
        if (fseek (fh,0L,SEEK_SET))
          {
          slogFileErrorPrint ("SEEK",lc->fc.fileName);
          return;
          }

/* Print wrap message at top						*/

        seekPos = 0;
        printWrap (fh);
        lc->fc.state |= FIL_STATE_NEED_WIPE; /* flag to show we wrapped */
        } /* need to wrap */
      } /* Wrap not disabled */
    } /* File already opened	*/

/* Ok, the file is open and the pointer set to where we log the next	*/
/* message, any wrap has been printed - time to log the info		*/

/* Now print the message header						*/
  slogSetHdr (lc, logType, logTypeStr, sourceFile, lineNum, slogHdr, sizeof(slogHdr), "\n");
  fprintf (fh,"%s", slogHdr);

/* Now print the message buffer						*/
  fprintf (fh,"%s", buf);

/* Now print 'wipe bar', if file has wrapped some time in the past	*/
/* and if we are supposed to do the wipe				*/

  if (lc->fc.state & FIL_STATE_NEED_WIPE && 
      lc->fc.ctrl & FIL_CTRL_WIPE_EN)
    {
    currPos = ftell (fh);
    if (currPos == -1L)
      {
      slogFileErrorPrint ("TELL",lc->fc.fileName);
      return;
      }

/* Print the wipe bar							*/
    lc->fc.wipeFilePos = currPos;
    printWipe (fh);

/* Seek to the current write position					*/

    if (fseek (fh,currPos,SEEK_SET))
      {
      slogFileErrorPrint ("SEEK",lc->fc.fileName);
      return;
      }
    } /* Need to print wipe bar						*/

/* Try to not lose log data						*/

  fflush (fh);

/* Check for paranoid user - wants to close && reopen			*/

  if (lc->fc.ctrl & FIL_CTRL_HARD_FLUSH)
    {
    currPos = ftell (fh);
    if (currPos == -1L)
      {
      slogFileErrorPrint ("TELL",lc->fc.fileName);
      return;
      }

/* Close the file							*/

    fclose (fh);

/* Re-open the file							*/

    lc->fc.fp = fh = fopen (lc->fc.fileName,"r+");
    if (!fh)		/* better exist					*/
      {		
      slogFileErrorPrint ("OPEN",lc->fc.fileName);
      lc->fc.state &= ~FIL_STATE_OPEN;
      return;
      }

    if (lc->fc.ctrl & FIL_CTRL_SETBUF_EN)  /* If setbuf desired		*/
      setbuf (fh,NULL);  		  /* no buffering, please	*/

/* Seek to current write position					*/

    if (fseek (fh,currPos,SEEK_SET))
      {
      slogFileErrorPrint ("SEEK",lc->fc.fileName);
      return;
      }
    } /* if hard flush enabled						*/
  }

/************************************************************************/
/* 		              slogCloseFile				*/
/************************************************************************/

ST_VOID slogCloseFile (LOG_CTRL *lc)
  {

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

  if (lc->fc.state & FIL_STATE_OPEN)
    {
    if (fclose (lc->fc.fp))
      {
      fprintf (stderr,"\n *** LOG FILE CLOSE PROBLEM (%s) : ",
      	lc->fc.fileName);
      return;
      }
    lc->fc.state &= ~FIL_STATE_OPEN;
    }
  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*		               slogCloneFile				*/
/************************************************************************/

ST_VOID slogCloneFile (LOG_CTRL *lc, SD_CONST ST_CHAR *newFile)
  {
ST_LONG 		currPos;
FILE 		*fh;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

  fh = lc->fc.fp;
  if (lc->fc.state & FIL_STATE_OPEN)
    {
    currPos = ftell (fh);
    if (currPos == -1L)
      {
      slogFileErrorPrint ("TELL",lc->fc.fileName);
      S_UNLOCK_UTIL_RESOURCES ();
      return;
      }

/* Close the file							*/
    fclose (fh);

/* Copy the log file							*/
   copy_named_files (lc, newFile, lc->fc.fileName);
   
/* Re-open the file at the old position					*/
    lc->fc.fp = fh = fopen (lc->fc.fileName,"r+");
    if (!fh)		/* better exist					*/
      {		
      slogFileErrorPrint ("OPEN",lc->fc.fileName);
      S_UNLOCK_UTIL_RESOURCES ();
      return;
      }

    if (lc->fc.ctrl & FIL_CTRL_SETBUF_EN)  /* If setbuf desired		*/
      setbuf (fh,NULL);  		  /* no buffering, please	*/

/* Seek to current write position					*/
    if (fseek (fh,currPos,SEEK_SET))
      {
      slogFileErrorPrint ("SEEK",lc->fc.fileName);
      S_UNLOCK_UTIL_RESOURCES ();
      return;
      }
    }
  else	/* log file not opened, just copy it				*/
    copy_named_files (lc, newFile, lc->fc.fileName);

  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*		               slogCloneFileEx				*/
/************************************************************************/

ST_VOID slogCloneFileEx (LOG_CTRL *lc, SD_CONST ST_CHAR *newFile)
  {
ST_LONG 		currPos;
FILE 		*fh;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

  fh = lc->fc.fp;
  if (lc->fc.state & FIL_STATE_OPEN)
    {
    currPos = ftell (fh);
    if (currPos == -1L)
      {
      slogFileErrorPrint ("TELL",lc->fc.fileName);
      S_UNLOCK_UTIL_RESOURCES ();
      return;
      }

/* Close the file							*/
    fclose (fh);

/* Copy the log file							*/
   copy_named_files_ex (lc, newFile, lc->fc.fileName);
   
/* Re-open the file at the old position					*/
    lc->fc.fp = fh = fopen (lc->fc.fileName,"r+");
    if (!fh)		/* better exist					*/
      {		
      slogFileErrorPrint ("OPEN",lc->fc.fileName);
      S_UNLOCK_UTIL_RESOURCES ();
      return;
      }

    if (lc->fc.ctrl & FIL_CTRL_SETBUF_EN)  /* If setbuf desired		*/
      setbuf (fh,NULL);  		  /* no buffering, please	*/

/* Seek to current write position					*/
    if (fseek (fh,currPos,SEEK_SET))
      {
      slogFileErrorPrint ("SEEK",lc->fc.fileName);
      S_UNLOCK_UTIL_RESOURCES ();
      return;
      }
    }
  else	/* log file not opened, just copy it				*/
    copy_named_files_ex (lc, newFile, lc->fc.fileName);

  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*		              slogDeleteFile				*/
/************************************************************************/

ST_VOID slogDeleteFile (LOG_CTRL *lc)
  {

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

/* first close the log file						*/

  slogCloseFile (lc);

/* Now delete it							*/

  remove (lc->fc.fileName);

  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*			       findOldestMessage			*/
/************************************************************************/

static ST_LONG findOldestMessage (LOG_CTRL *lc, FILE *fh)
  {
ST_CHAR idxFileName[S_MAX_PATH];
FILE *idxFh;
ST_CHAR buf[200];
ST_LONG startPos;
ST_LONG currPos;
ST_INT count;
ST_INT cmpLen;

  cmpLen = (ST_INT) strlen(OLDEST_STRING);

/* Need to find the start position by looking for the 'OLDEST' message. */
/* See if we have an index file and seek position ... */
  if (slog_get_index_file_name (lc, idxFileName, (int) sizeof(idxFileName)) == SD_SUCCESS)
    {
    idxFh = fopen (idxFileName,"r");
    if (idxFh)
      {
      count = fscanf (idxFh, "%ld", &startPos);
      fclose (idxFh);
      if (count == 1)
        {
        if (fseek (fh, startPos, SEEK_SET) == 0)
          {
          fgets (buf,200,fh);
          fgets (buf,200,fh);
          fgets (buf,200,fh);
          if (fgets (buf,200,fh))
            {
            if (buf[10] == 'O' && !strncmp (buf,OLDEST_STRING, cmpLen))
              return (startPos);
            }
          }
        }
      }
    }

/* Darn, need to start at the top and find it */
  fseek (fh, 0, SEEK_SET);
  while (SD_TRUE)
    {
    currPos = ftell (fh);
    if (!fgets (buf,200,fh))
      break; 

    chk_slog_service (lc);

  /* Is this the position with the marker string? */
    if (buf[10] == 'O' && !strncmp (buf,OLDEST_STRING, cmpLen))
      return (currPos);
    }

/* No oldest found, go to the start of the file */
  fseek (fh, 0, SEEK_END);
  currPos = ftell (fh);
  return (currPos);
  }

/************************************************************************/
/*			slog_get_index_file_name			*/
/************************************************************************/

ST_RET slog_get_index_file_name (LOG_CTRL *lc, ST_CHAR *dest, ST_INT destLen)
  {
ST_CHAR *src;
ST_INT   srcLen;

  src = lc->fc.fileName;

  if (!src || (srcLen = (int) strlen(src)) == 0)
    return (SD_FAILURE);
  if (!dest || destLen < (srcLen+4))	/* +4 for len of ".sli"	*/
    return (SD_FAILURE);

  while (SD_TRUE)
    {
    *dest = *src;
    if (*dest == 0)	/* End of string ... */
      break;
      
    if (*dest == '.') 
      *dest = '_';

    ++src; 
    ++dest;
    }

  strcat (dest, ".sli");
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                              printStart				*/
/************************************************************************/

static ST_VOID  printStart (FILE *fh)
  {
time_t t;

  t = time(NULL);
  fprintf (fh,"\n\n***********************************************************");
  fprintf (fh,"\n          LOGGING STARTED %s", ctime(&t));
  fprintf (fh,"***********************************************************");
  }

/************************************************************************/
/*                               printWrap                              */
/************************************************************************/

static ST_VOID  printWrap (FILE *fh)
  {
time_t t;

  t = time(NULL);
  fprintf (fh,"\n***********************************************************");
  fprintf (fh,"\n          FILE WRAPPED %s", ctime(&t));
  fprintf (fh,"***********************************************************");
  }

/************************************************************************/
/*                                printWipe				*/
/************************************************************************/

static ST_VOID  printWipe (FILE *fh)
  {
  fprintf (fh,"\n\n***********************************************************");
  fprintf (fh,"\n%s",OLDEST_STRING);;
  fprintf (fh,"\n***********************************************************\n\n");
  }

/************************************************************************/
/*			      slogFileErrorPrint			*/
/************************************************************************/

static ST_VOID slogFileErrorPrint (ST_CHAR *text, ST_CHAR *fname)
  {
  fprintf (stderr,"\n*** LOG FILE %s ERROR (%s) : ",text,fname);
  fprintf (stderr,"%s",strerror(errno));
  }

/************************************************************************/
/*                       copy_named_files					*/
/************************************************************************/

#define FILE_READ_SIZE 	1000

static ST_INT copy_named_files (LOG_CTRL *lc, 
      SD_CONST ST_CHAR *SD_CONST dest, SD_CONST ST_CHAR *SD_CONST src)
  {
FILE *dest_fp;
FILE *src_fp;
ST_INT ret;
ST_CHAR buf[FILE_READ_SIZE+1];
ST_INT bytes_read;
ST_INT bytes_written;

  if (!(src_fp = fopen (src,"rb")))
    return (SD_FAILURE);

  if (!(dest_fp = fopen (dest,"wb")))
    {
    fclose (src_fp);
    return (SD_FAILURE);
    }

  ret = SD_SUCCESS;
  while (SD_TRUE)
    {
    if (!(bytes_read = (ST_INT) fread (buf,1,FILE_READ_SIZE,src_fp)))
      break;

    bytes_written = (ST_INT) fwrite (buf,1,bytes_read,dest_fp);
    if (bytes_read != bytes_written)
      {
      ret = SD_FAILURE;
      break;
      }    

    chk_slog_service (lc);
    }    

  if (fclose (src_fp))
    ret = SD_FAILURE;

  if (fclose (dest_fp))
    ret = SD_FAILURE;

  return (ret);
  }


/************************************************************************/
/*                       copy_named_files_ex				*/
/************************************************************************/

static ST_INT copy_named_files_ex (LOG_CTRL *lc, SD_CONST ST_CHAR *SD_CONST dest, 
		SD_CONST ST_CHAR *SD_CONST src)
  {
FILE *dest_fp;
FILE *src_fp;
ST_INT ret;
ST_CHAR buf[200+1];
ST_LONG  currPos;
ST_LONG  oldestPos;

  if (!(src_fp = fopen (src,"rb")))
    return (SD_FAILURE);

  oldestPos = findOldestMessage (lc, src_fp);
  if (fseek (src_fp, oldestPos, SEEK_SET))
    {
    slogFileErrorPrint ("SEEK",lc->fc.fileName);
    fclose (src_fp);
    return (SD_FAILURE);
    }

  if (!(dest_fp = fopen (dest,"wb")))
    {
    fclose (src_fp);
    return (SD_FAILURE);
    }

  ret = SD_SUCCESS;
  while (SD_TRUE)
    {
    if (!fgets (buf,200,src_fp))
      break;

    fputs (buf,dest_fp);

    chk_slog_service (lc);
    }    

  if (fclose (src_fp))
    ret = SD_FAILURE;

  if (ret == SD_SUCCESS)
    {
    if (oldestPos > 0)
      {
      if (!(src_fp = fopen (src,"rb")))
        {
        fclose (dest_fp);
        return (SD_FAILURE);
	}

      currPos = 0;
      while (currPos < oldestPos)
        {
        if (!fgets (buf,200,src_fp))
          break;

        fputs (buf,dest_fp);

        currPos = ftell (src_fp);
        if (currPos == -1L)
          {
          slogFileErrorPrint ("TELL",lc->fc.fileName);
          fclose (src_fp);
          fclose (dest_fp);
          return (SD_FAILURE);
	  }

        chk_slog_service (lc);
        }
      }
    }

  if (fclose (src_fp))
    ret = SD_FAILURE;

  if (fclose (dest_fp))
    ret = SD_FAILURE;

  return (ret);
  }


/************************************************************************/
/*			chk_slog_service 				*/
/************************************************************************/

static ST_INT slog_service_count;

static ST_VOID chk_slog_service (LOG_CTRL *lc)
  {
ST_UINT32 logCtrlSave;

  /* Allow the application to do it's thing during a slow operation	*/
  if (slog_service_fun)
    {
    if (++slog_service_count > 10)
      {
      slog_service_count = 0;

  /* do not allow file logging during this operation		*/
      logCtrlSave = lc->logCtrl;
      lc->logCtrl &= ~LOG_FILE_EN;
      (*slog_service_fun) ();
      lc->logCtrl = logCtrlSave;
      } 
    } 
  }
