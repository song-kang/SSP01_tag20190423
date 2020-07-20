/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1993-2007, All Rights Reserved 				*/
/*									*/
/* MODULE NAME : sloghex.c						*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/19/10  MDE     17    Added lineCountLimit				*/
/* 08/26/09  MDE     16    Cut '%' from hex log, to protect 'dyn' users	*/
/* 01/15/07  EJV     15    Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 08/18/06  JRB     14    Chk _slog_remote_fun along with other ptrs.	*/
/* 05/23/05  EJV     13    doSlog corr: replaced LOG_IPC_EN with 	*/
/*			     LOG_IPC_LISTEN_EN || LOG_IPC_CALL_EN	*/
/* 11/05/04  EJV     12    Added check for LOG_IPC_EN.			*/
/* 05/18/04  MDE     11    Removed LOG_IPC_SUPPORT #ifdef's		*/
/* 03/11/04  GLB     10    Remove "thisFileName"                    	*/
/* 02/17/03  JRB     09    Del system includes (using sysincs.h).	*/
/* 03/27/01  MDE     08    Added _slogStr				*/
/* 09/13/99  MDE     07    Added SD_CONST modifiers			*/
/* 11/11/98  DSF     06    Minor changes to _slog_dyn_log_fun		*/
/* 10/09/98  MDE     05    Put back center space per JB request		*/
/* 10/08/98  MDE     04    Migrated to updated SLOG interface		*/
/* 04/07/98  MDE     03    Minor warning cleanup			*/
/* 10/06/97  DSF     02    Added thisFileName				*/
/* 05/28/97  DSF     01    Added IPC logging capability			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "slog.h"

#define cvt_nibble(a)	((ST_UCHAR) ((a) > 9 ? (a) + 'A' - 10 : (a) + '0'))
#define DEBUG_BUF_LEN 100

#define CHAR_PER_LINE  75

/************************************************************************/
/*                             slogHex  				*/
/* Main HEX logging function, typically called via macro		*/
/* Create the HEX strings && pass to memory & file logging		*/
/* functions if enabled							*/
/************************************************************************/

ST_VOID slogHex (LOG_CTRL *lc, 
		 SD_CONST ST_INT logType, 
		 SD_CONST ST_CHAR *SD_CONST fileName, 
		 SD_CONST ST_INT lineNum,
	  	 SD_CONST ST_INT numBytes, 
		 SD_CONST ST_VOID *hexData)
  {
  _slogHex (lc, numBytes, hexData);
  }


ST_VOID _slogHex (LOG_CTRL *lc, 
		  ST_INT numBytes, 
		  SD_CONST ST_VOID *hexData)
  {
ST_INT i;
ST_INT ascii_index;
ST_INT hex_index;
ST_CHAR debug_buf[DEBUG_BUF_LEN+1]; /* output buffer			*/
ST_INT curlen;
ST_UCHAR ch;
ST_UCHAR hex;
ST_LONG addr;
ST_UINT filCtrlSave;
ST_UINT memCtrlSave;
ST_UCHAR *buf;
ST_INT lineCount;
ST_BOOLEAN firstDrop;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

  addr = 0;
 
/* Make the compiler happy						*/
  buf = (ST_UCHAR *) hexData;

/* Make sure the LOG_CTRL pointer is not NULL				*/
  if (!lc)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

/* Check to see if any logging is enabled AND dynamic user logging	*/
/* function pointer not set						*/
  if (!(lc->logCtrl & (LOG_FILE_EN | LOG_MEM_EN | LOG_IPC_EN)) && 
      !slog_dyn_log_fun && !_slog_dyn_log_fun &&
      !slog_remote_fun && !_slog_remote_fun)	   
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

/* Remember the state of the wipe/wrap/header enable flags		*/
/* so that we can restore them later					*/

  filCtrlSave = lc->fc.ctrl & (FIL_CTRL_WIPE_EN | 
			    FIL_CTRL_WRAP_EN |
                            FIL_CTRL_MSG_HDR_EN);

  memCtrlSave = lc->mc.ctrl & MEM_CTRL_MSG_HDR_EN;

/* No hdr or wipe during a hex dump (allow wrap for first line if req'd)*/
  lc->fc.ctrl &= ~(FIL_CTRL_WIPE_EN | FIL_CTRL_MSG_HDR_EN);
  lc->mc.ctrl &= ~MEM_CTRL_MSG_HDR_EN;

/* Also turn on HEX flag bit for memory logging				*/
  lc->mc.ctrl |= MEM_CTRL_HEX_LOG;

/* OK, now start the dump, line by line					*/
  lineCount = 0;
  firstDrop = SD_TRUE;

  while (numBytes)
    {
    curlen = min(numBytes,16); 		/* # char's in current line	*/
    for (i = 0; i < DEBUG_BUF_LEN; ++i)
      debug_buf[i] = ' ';
    sprintf (debug_buf,"%05lX  ",addr);/* data offset 			*/
    hex_index = 7;			/* where to put hex data 	*/
    ascii_index = 57;			/* where to put ascii conversion */
    debug_buf[ascii_index++] = '*';

    ++lineCount;
    for (i = 0; i < curlen; i++)	/* for each byte in this line	*/
      {
      ch = *buf++;  			/* get next character		*/

      hex = (ch >> 4) & (ST_UCHAR) 0x0f;/* write HEX chars for the byte	*/
      hex = cvt_nibble(hex);
      debug_buf[hex_index++] = hex;
      hex = ch & (ST_UCHAR) 0x0f;
      hex = cvt_nibble(hex);
      debug_buf[hex_index++] = hex;
      ++hex_index;			/* space between bytes */
      if (i == 7)
        ++hex_index;

    /* Note: We will not put a '%' in the ASCII section, to help 	*/
    /* protect external slog_dyn_log_fun users from vsprintf problems	*/
      if (isprint(ch) && ch != '%') 	/* print ASCII portion		*/
	debug_buf[ascii_index] = ch;
      else
	debug_buf[ascii_index] = '.';	/* just put a '.' there 	*/
      ascii_index++;
      } 				

    debug_buf[ascii_index++] = '*';
    debug_buf[ascii_index++] = 0;

    addr += 16;				/* prepare for next line	*/
    numBytes -= curlen;

/* debug_buf now contains the hex/ascii line to be logged		*/

/* for the last line to be logged, turn wipe back on			*/
    if (!numBytes)
      {
      lc->fc.ctrl |= (filCtrlSave & FIL_CTRL_WIPE_EN);
      }

    if (lc->logCtrl & LOG_FILE_EN)	/* File Logging enabled		*/
      {
      slogFile (lc, SLOG_CONT, NULL, NULL, 0, (int) strlen(debug_buf), debug_buf);
      }

    if (lc->logCtrl & LOG_MEM_EN)	/* Memory Logging enabled	*/
      {
      slogMem (lc, SLOG_CONT, NULL, NULL, 0, (int) strlen(debug_buf), debug_buf);
      }

    if ((lc->logCtrl & LOG_IPC_LISTEN_EN) || (lc->logCtrl & LOG_IPC_CALL_EN))
      /* IPC Logging enabled (listen, calling, or both modes) */
      {
      if ((lc->ipc.lineCountLimit == 0) || (lineCount <= lc->ipc.lineCountLimit) || !numBytes)
        {
        slogIpc (lc, SLOG_CONT, NULL, NULL, 0, (int) strlen(debug_buf), debug_buf);
  	}
      else if (firstDrop == SD_TRUE)
        {
        firstDrop = SD_FALSE;
        slogIpc (lc, SLOG_CONT, NULL, NULL, 0, 5, "  ...");
    	}
      }

/* If the user has set up a dynamic log display function, call it	*/
    if (slog_dyn_log_fun)
      {
      (*slog_dyn_log_fun)(lc, SLOG_CONT, NULL, 0, 
                                      (int) strlen(debug_buf), debug_buf);
      }
    if (_slog_dyn_log_fun)
      {
      (*_slog_dyn_log_fun)(lc, slogTimeText, SLOG_CONT, NULL, NULL, 0, 
                                      (int)strlen(debug_buf), debug_buf);
      }

/* If the user has set up a remote logging function, call it		*/
    if (slog_remote_fun)
      {
      (*slog_remote_fun)(slogRemoteFlags, lc, SLOG_CONT,  
				NULL, 0, 
				(int) strlen(debug_buf), debug_buf);
      }
    if (_slog_remote_fun)
      {
      (*_slog_remote_fun)(slogRemoteFlags, lc, SLOG_CONT, NULL,  
				NULL, 0, 
				(int) strlen(debug_buf), debug_buf);
      }

/* Don't want to wrap the file in the middle of a hex dump		*/
    lc->fc.ctrl &= ~FIL_CTRL_WRAP_EN;
    }

/* All done, restore the flag state bits				*/

  lc->fc.ctrl |= filCtrlSave;
  lc->mc.ctrl |= memCtrlSave;
  lc->mc.ctrl &= ~MEM_CTRL_HEX_LOG;

  S_UNLOCK_UTIL_RESOURCES ();
  }


/************************************************************************/
/*                             slogStr  				*/
/************************************************************************/

ST_VOID _slogStr (LOG_CTRL *lc, ST_CHAR *strData)
  {
ST_INT i;
ST_INT ascii_index;
ST_CHAR debug_buf[DEBUG_BUF_LEN+1]; /* output buffer			*/
ST_INT curlen;
ST_UCHAR ch;
ST_UINT filCtrlSave;
ST_UINT memCtrlSave;
ST_INT numBytes;
ST_INT lineCount;
ST_BOOLEAN firstDrop;

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

/* Make sure the LOG_CTRL pointer is not NULL				*/
  if (!lc)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

/* Check to see if any logging is enabled AND dynamic user logging	*/
/* function pointer not set						*/
  if (!(lc->logCtrl & (LOG_FILE_EN | LOG_MEM_EN | LOG_IPC_EN)) && 
      !slog_dyn_log_fun && !_slog_dyn_log_fun &&
      !slog_remote_fun && !_slog_remote_fun)	   
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

/* Remember the state of the wipe/wrap/header enable flags		*/
/* so that we can restore them later					*/

  filCtrlSave = lc->fc.ctrl & (FIL_CTRL_WIPE_EN | 
			    FIL_CTRL_WRAP_EN |
                            FIL_CTRL_MSG_HDR_EN);

  memCtrlSave = lc->mc.ctrl & MEM_CTRL_MSG_HDR_EN;

/* No hdr or wipe during a hex dump (allow wrap for first line if req'd)*/
  lc->fc.ctrl &= ~(FIL_CTRL_WIPE_EN | FIL_CTRL_MSG_HDR_EN);
  lc->mc.ctrl &= ~MEM_CTRL_MSG_HDR_EN;

/* OK, now start the dump, line by line					*/
  lineCount = 0;
  firstDrop = SD_TRUE;

  numBytes = (int) strlen (strData);
  while (numBytes)
    {
    curlen = min (numBytes,CHAR_PER_LINE); 		/* # char's in current line	*/
    ascii_index = 0;			/* where to put ascii conversion */

    ++lineCount;
    for (i = 0; i < curlen; i++)	/* for each byte in this line	*/
      {
      ch = *strData++;  		/* get next character		*/
      if (isprint(ch)) 			/* print ASCII portion		*/
	debug_buf[ascii_index] = ch;
      else if (ch == '\n')
        {
	debug_buf[ascii_index] = ch;
        ++i;
        break;
        }				/* end of this line */
      else if (ch == '\t')
	debug_buf[ascii_index] = ch;
      else
	debug_buf[ascii_index] = '.';	/* just put a '.' there 	*/
      ascii_index++;
      } 				
    debug_buf[ascii_index++] = 0;

    numBytes -= i;

/* debug_buf now contains the ascii line to be logged		*/
/* for the last line to be logged, turn wipe back on			*/
    if (!numBytes)
      {
      lc->fc.ctrl |= (filCtrlSave & FIL_CTRL_WIPE_EN);
      }

    if (lc->logCtrl & LOG_FILE_EN)	/* File Logging enabled		*/
      {
      slogFile (lc, SLOG_CONT, NULL, NULL, 0, (int) strlen(debug_buf), debug_buf);
      }

    if (lc->logCtrl & LOG_MEM_EN)	/* Memory Logging enabled	*/
      {
      slogMem (lc, SLOG_CONT, NULL, NULL, 0, (int) strlen(debug_buf), debug_buf);
      }

    if ((lc->logCtrl & LOG_IPC_LISTEN_EN) || (lc->logCtrl & LOG_IPC_CALL_EN))
      /* IPC Logging enabled (listen, calling, or both modes) */
      {
      if ((lc->ipc.lineCountLimit == 0) || (lineCount <= lc->ipc.lineCountLimit) || !numBytes)
        {
        slogIpc (lc, SLOG_CONT, NULL, NULL, 0, (int) strlen(debug_buf), debug_buf);
        }
      else if (firstDrop == SD_TRUE)
        {
        firstDrop = SD_FALSE;
        slogIpc (lc, SLOG_CONT, NULL, NULL, 0, 5, "  ...");
    	}
      }

/* If the user has set up a dynamic log display function, call it	*/
    if (slog_dyn_log_fun)
      {
      (*slog_dyn_log_fun)(lc, SLOG_CONT, NULL, 0, 
                                      (int) strlen(debug_buf), debug_buf);
      }
    if (_slog_dyn_log_fun)
      {
      (*_slog_dyn_log_fun)(lc, slogTimeText, SLOG_CONT, NULL, NULL, 0, 
                                      (int) strlen(debug_buf), debug_buf);
      }

/* If the user has set up a remote logging function, call it		*/
    if (slog_remote_fun)
      {
      (*slog_remote_fun)(slogRemoteFlags, lc, SLOG_CONT,  
				NULL, 0, 
				(int) strlen(debug_buf), debug_buf);
      }
    if (_slog_remote_fun)
      {
      (*_slog_remote_fun)(slogRemoteFlags, lc, SLOG_CONT, NULL,  
				NULL, 0, 
				(int) strlen(debug_buf), debug_buf);
      }

/* Don't want to wrap the file in the middle of a hex dump		*/
    lc->fc.ctrl &= ~FIL_CTRL_WRAP_EN;
    }

/* All done, restore the flag state bits				*/

  lc->fc.ctrl |= filCtrlSave;
  lc->mc.ctrl |= memCtrlSave;
  lc->mc.ctrl &= ~MEM_CTRL_HEX_LOG;

  S_UNLOCK_UTIL_RESOURCES ();
  }


