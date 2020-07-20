/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 - 2001 All Rights Reserved					*/
/*									*/
/* MODULE NAME : readhex.c						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*                                                                      */
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/12/10  JRB     15    Fix sscanf arg.				*/
/* 01/30/06  GLB     14    Integrated porting changes for VMS           */
/* 08/10/05  JRB     13    Fix comment.					*/
/* 10/26/04  MDE     12    Support new time format			*/
/* 01/02/01  EJV     11    Added few castings to eliminate AIX warnings.*/
/* 09/27/99  JRB     10    change #if HEX_DEBUG to #ifdef HEX_DEBUG     */
/* 02/03/99  DWL     09    Write header lines to resulting log file	*/
/* 11/12/98  JRB     08    Instead of copying lineBuf to tempBuf,	*/
/*			   use tmpPtr to point within lineBuf.		*/
/*			   Use MAX_LINE_LEN define for line length.	*/
/* 11/04/98  DWL     07    Fixed minor bugs, changed declarations of	*/
/*                         slog header variables			*/
/* 10/16/98  DWL     06    Added support for new Slog format		*/
/* 08/12/98  MDE     05    Fixed Slog header copy overwrite		*/
/* 07/15/98  DWL     04    Added header support.			*/
/* 04/13/98  DWL     03    Fixed SLOG bug.				*/
/* 06/13/97  DWL     02	   Added functionality for SLOG, Network	*/
/*			   Analyzer, Netsight log files			*/
/* 02/18/97  MDE     01    Created from lean_cfg.c			*/
/************************************************************************/

#ifdef DEBUG_SISCO
static char *thisFileName = __FILE__;
#endif

#include "glbtypes.h"
#include "ctype.h"
#include "mem_chk.h"
#include "readhex.h"
#include <stdlib.h>
#include <string.h>


/************************************************************************/
/************************************************************************/
ST_RET parseHex (unsigned char *srcData, int destBufLen, 
   	      int *lenOut, unsigned char *selDestdestBuf, ST_INT ftype);

ST_RET mult_count(ST_INT multiplier);

ST_VOID log_hex_bytes (FILE *dest, ST_UCHAR *ptr, ST_INT len);

ST_INT numSlogHeaderLines;
ST_CHAR *slogHeaderLines[MAX_NUM_SLOG_HEADER_LINES];
ST_BOOLEAN checkIfHeader (ST_CHAR *data);

/************************************************************************/
/************************************************************************/

ST_RET readHex (ST_CHAR *fileName, ST_CHAR *dest, ST_INT destSize, 
		ST_INT ftype)
  {
#define MAX_LINE_LEN	256
FILE *old;
static ST_CHAR lineBuf[MAX_LINE_LEN];
ST_CHAR *tempPtr;	/* Current pointer into lineBuf	*/
ST_INT pduLen;
ST_INT lenOut;
ST_INT done;
ST_INT i;
ST_INT lineNum;
static ST_INT prevLineNum = -1;
ST_CHAR lineNumBuf[10];
ST_BOOLEAN valid_data;
ST_BOOLEAN end_pdu;
ST_BOOLEAN go_ahead;
ST_BOOLEAN readHeaderInfo = SD_FALSE;
ST_RET ret;

/* First we calloc room for header lines				*/
  for (i=0; i<MAX_NUM_SLOG_HEADER_LINES; i++)
    slogHeaderLines[i] = (ST_CHAR *) chk_calloc (1, MAX_LINE_LEN);

  if (!(old = fopen (fileName,"r")))
    {
    printf ("\n\n File '%s' Not Found", fileName);
    return (SD_FAILURE);
    }

  ret = SD_SUCCESS;
  mult_count(0);   /* Only matters if data type is NETWORK ANALYZER or  NETSIGHT */
  done = SD_FALSE;
  pduLen = 0;
  end_pdu = SD_FALSE;
  go_ahead = SD_TRUE;
  strcpy (lineBuf, "");

/* Begin parsing PDU's	*/
  while (!done)
    {
    if ((strcmp (lineBuf, "") == 0 && ftype == SLOG_FILE) || 
        (ftype != SLOG_FILE))
      {
      if (fgets (lineBuf, MAX_LINE_LEN, old) == NULL)
	go_ahead = SD_FALSE;
      else
	go_ahead = SD_TRUE;
      }

    if (go_ahead)
      {
      valid_data = SD_FALSE;
/* This loop tests the current line in the log file to see if it contains any characters */
      for (i = 0; i < MAX_LINE_LEN; i++)
        {
	if ((lineBuf[i] != SPACE) && (lineBuf[i] != LF) && (lineBuf[i] != 0)) 	
	  {
	  valid_data = SD_TRUE;
	  break;
	  }
	else if (lineBuf[i] == LF)
	  break;
	}

/* Here we want to extract any header lines in case they are needed later	*/
      if (checkIfHeader (lineBuf) == SD_TRUE)	/* we are seeing header info	*/
        {
        readHeaderInfo = SD_TRUE;
	numSlogHeaderLines = 0;				/* Init	*/
	}
      if (strstr (lineBuf, "00000") != NULL)		/* back to data			*/
        readHeaderInfo = SD_FALSE;
      if (readHeaderInfo)
        {
        strcpy (slogHeaderLines[numSlogHeaderLines], 
		 &lineBuf[0]); 

        slogHeaderLines[numSlogHeaderLines][strlen(lineBuf)-1] = 0;

	++numSlogHeaderLines;
	if (numSlogHeaderLines > MAX_NUM_SLOG_HEADER_LINES - 1)
	  /* Cut off before memory overwrite happens, only 10 header lines allowed	*/
	  readHeaderInfo = SD_FALSE;
	}

/* If Slog file, we want to test if there is valid data on this line before continuing  */
/* because there are not always empty lines between PDU's in that type of file	*/
      if (ftype == SLOG_FILE)
	{
	if (strncmp (lineBuf, " 0", 2) != 0 && strncmp (lineBuf, "0", 1) != 0 &&
	    strncmp (lineBuf, "  0", 3) != 0)
	  valid_data = SD_FALSE;
	else
	  {
          tempPtr = lineBuf;		/* tempPtr point to start of buffer.*/
	  while (!isalnum (*tempPtr))
	    ++tempPtr;
	  strncpy (lineNumBuf, tempPtr, 5);
	  lineNumBuf[5] = 0;
	  for (i=0; i<5; i++)
	    {
	    if (!isxdigit (lineNumBuf[i]))
	      {
	      valid_data = SD_FALSE;
	      break;
	      }
	    }
	  if (valid_data == SD_TRUE)
	    {
	    lineNum = -1;
	    sscanf (lineNumBuf, "%x", (ST_UINT *)&lineNum);
	    if (lineNum <= prevLineNum)
	      valid_data = SD_FALSE;
	    prevLineNum = lineNum;
	    }
	  }
	}
      
      /* If there are characters on this line, call parseHex */
      /* May or may not be parseable data, that is decided later, except for SLOG, decided above */
      /* End of PDU signified by blank line or eof */
      if (valid_data == SD_TRUE)
        {
	end_pdu = SD_FALSE;
        ret = parseHex ((unsigned char *) lineBuf, destSize-pduLen, &lenOut, (unsigned char *) &dest[pduLen], ftype);
        if (ret != SD_SUCCESS)
          return(SD_FAILURE);
        pduLen += lenOut;
	strcpy (lineBuf, "");
	}
      else					/* end of current PDU  			*/	
        {
	if (!end_pdu && pduLen > 0)
	  {
	  u_readhex_pdu ((ST_UCHAR *)dest, pduLen);
	  
          end_pdu = SD_TRUE;
	  mult_count(0);   /* Only matters if data type is NETWORK ANALYZER or  NETSIGHT */
	  pduLen = 0;
	  prevLineNum = -1;
	  }
	else
	  if (ftype == SLOG_FILE)
	    strcpy (lineBuf, "");
	}
      }
    else
      { 		 			/* end of file, end of last PDU		*/
      if (!end_pdu && pduLen > 0)
        {
        u_readhex_pdu ((ST_UCHAR *)dest, pduLen);

        end_pdu = SD_TRUE;
	mult_count(0);   /* Only matters if data type is NETWORK ANALYZER or  NETSIGHT */
	pduLen = 0;
	}
      done = SD_TRUE;   	/* no more to read			*/
      }
    }
  fclose (old);
  
  /* Free allocated header space      */
  for (i=0; i<MAX_NUM_SLOG_HEADER_LINES; i++)
    chk_free (slogHeaderLines[i]);

  return (ret);
  }


/************************************************************************/
/*			parseHex					*/
/************************************************************************/

ST_RET parseHex (unsigned char *srcData, int destBufLen, 
   	      int *lenOut, unsigned char *selDestdestBuf, ST_INT ftype)
  {
ST_INT i;
ST_INT16 digit;
ST_INT16 nibble;
ST_INT lineNumber;
ST_INT numDigits;		/* read only NUM_OCTETS_PER_LINE hex digits per line	*/
ST_INT numSpaces;		/* if > 3, this is partial last line of PDU	*/

  switch(ftype)
    {
    case SLOG_FILE:
      while (isspace(*srcData))    /* Pass over initial space */
	++srcData;
      if (*srcData != '0')
	{
	*lenOut = 0;
	return(SD_SUCCESS);
	}
      while (!isspace (*srcData))  /* pass over line numbers in file */
        ++srcData;
    break;

    case NETWORK_ANALYZER:
    case NETSIGHT:
      while (isspace (*srcData)) /* pass over spaces in file		*/
        ++srcData;
      lineNumber = 0;
      while (!isspace (*srcData))
        {
        lineNumber = lineNumber + (*srcData++ - '0');
	if (!isspace (*srcData))
	  lineNumber = lineNumber * 10;
	}
      if (lineNumber != mult_count(2) * NUM_OCTETS_PER_LINE)
        {
	*lenOut = 0;
	return (SD_SUCCESS);	/* no hex data on this line		*/
	}
      break;

    case PLAIN_HEX:
        while (isspace (*srcData))
	  ++srcData;
        if (!isxdigit(*srcData))
	  {
	  *lenOut = 0;
	  return (SD_SUCCESS);
	  }
      break;
    }
  mult_count(1);   /* Only matters if data type is NETWORK ANALYZER or  NETSIGHT */
  while (!isxdigit (*srcData))
    {
    if (*srcData == '\n')
      {
      *lenOut = 0;
      return (SD_SUCCESS);
      }
    ++srcData;
    }

  i = numDigits = numSpaces = 0;
  nibble = SD_FALSE;
  while (isxdigit (*srcData))	/* get hex number 		*/
    {
    digit = *srcData++;		/* separate nibbles			*/
    digit = isdigit (digit) ? digit - '0' : 10 + (toupper (digit) - 'A'); 
    if (nibble)
      {
      nibble = SD_FALSE;
      selDestdestBuf[i] = (selDestdestBuf[i] | (unsigned char) digit);	/* set low nibble */
      numDigits++;
      i++;

      numSpaces = 0;
      while (isspace (*srcData) && *srcData != '\n')
        {
        ++srcData;
	numSpaces++;
	if (numSpaces > 2)
	  break;
	}
      }
    else
      {
      nibble = SD_TRUE;
      selDestdestBuf[i] = (unsigned char) digit << 4;		/* set high nibble */
      }
    }

/* set length	*/
  selDestdestBuf[i] = 0;
  if (i == 0 && nibble == SD_FALSE)
    *lenOut = 0;
  else
    {
    if (nibble == SD_FALSE)
      *lenOut = i;
    else
      *lenOut = i + 1;
    }
  return (SD_SUCCESS);
  }

/****************************************************************/
/*                              mult_count			*/
/*  Multiplier maintenance function	  			*/
/* This routine keeps track of line numbers in Network Analyzer */
/* and Netsight files. 						*/
/* Line numbers increment like 0, 16, 32, etc.			*/
/* Line number is calculated by (mult * NUM_OCTETS_PER_LINE) 	*/
/* If the current line number isn't right, ie. 32 comes after 	*/
/* 16, we determine there is no more hex data for the given PDU	*/
/****************************************************************/

ST_RET mult_count(ST_INT multiplier)
  {
static ST_INT mult;

  switch (multiplier)
    {
    case 0:
      mult = 0;
      break;
    case 1:
      ++mult;
      break;
    default:
      /* 2, do nothing */
      break;
    }
  
  return (mult);
  }
  
/************************************************************************/
/*                           checkIfHeader				*/
/************************************************************************/
ST_BOOLEAN checkIfHeader (ST_CHAR *data)
  {
ST_CHAR *ptr;
ST_CHAR *tptr;


  ptr = data;  

  /* New format starts with year. Good to year 3000 ... */
  if (*ptr == '2' && isdigit(*(ptr+1)) && isdigit (*(ptr+2)) && isdigit (*(ptr+3)))
    return (SD_TRUE);

  /* Time only format */
  if ((tptr = strstr (ptr, ".")) != NULL)
    {
    if (isdigit (*(tptr+1)) && isdigit (*(tptr+2)) && isdigit (*(tptr+3)) &&
        strncmp ((tptr+4), "s", 1) == 0)
      return (SD_TRUE);
    }
  /* Time and date format */
  if (((strncmp (ptr, "Sun", 3) == 0) ||
       (strncmp (ptr, "Mon", 3) == 0) ||
       (strncmp (ptr, "Tue", 3) == 0) ||
       (strncmp (ptr, "Wed", 3) == 0) ||
       (strncmp (ptr, "Thu", 3) == 0) ||
       (strncmp (ptr, "Fri", 3) == 0) ||
       (strncmp (ptr, "Sat", 3) == 0)) &&
       (strlen (ptr) >= 25)) /* length of date and time */
    {
    if (isalpha (*(ptr)) && isalpha (*(ptr+1)) && isalpha (*(ptr+2))) /* Day */
      {
      ptr +=4;
      if (isalpha (*(ptr)) && isalpha (*(ptr+1)) && isalpha (*(ptr+2))) /* Month */
        {
	ptr += 4;
	if (isdigit (*(ptr)) && isdigit (*(ptr+1))) /* Date */
	  {
	  ptr += 3;
	  if (isdigit (*(ptr)) && isdigit (*(ptr+1)) && strncmp (ptr+2, ":", 1) == 0 &&
	      isdigit (*(ptr+3)) && isdigit (*(ptr+4)) && strncmp (ptr+5, ":", 1) == 0 &&
	      isdigit (*(ptr+6)) && isdigit (*(ptr+7))) /* Time */
	    {
	    ptr += 9;
	    if (isdigit (*(ptr)) && isdigit (*(ptr+1)) &&
	        isdigit (*(ptr+2)) && isdigit (*(ptr+3))) /* Year */
	      {
	      return (SD_TRUE);
	      }
	    }
	  }
	}
      }
    }
  /* Old Slog format */
  if (strstr (ptr, "*** Log Type") != NULL)
    return (SD_TRUE);

  return (SD_FALSE);
  }


/************************************************************************/
/************************************************************************/
#ifdef HEX_DEBUG
/************************************************************************/
/*                           log_hex_bytes				*/
/* This function provides a hex dump facility.				*/
/* Imported from fkey.c							*/
/* This function is DEBUG only.  It logs the parsed PDU back into a	*/
/* file for comparison with the original unparsed file.			*/
/************************************************************************/

#define cvt_nibble(a)		((a) > 9 ? (a) + 'A' - 10 : (a) + '0')
#define DEBUG_BUF_LEN 100

ST_VOID log_hex_bytes (FILE *dest, ST_UCHAR *ptr, ST_INT len)
{
ST_INT i;
ST_INT ascii_index;
ST_INT hex_index;
ST_CHAR debug_buf[DEBUG_BUF_LEN+1]; /* output buffer			*/
ST_INT curlen;
ST_CHAR ch;
ST_UCHAR hex;
ST_INT addr = 0;

  /* format hex data							*/

  while (len)
    {
    curlen = min(len,16); 		/* # char's in current line	*/
    sprintf (debug_buf," %04X  ",addr);	/* data offset 			*/
    hex_index = 7;			/* where to put hex data 	*/
    ascii_index = 58;			/* where to put ascii conversion */
    debug_buf[ascii_index++] = '*';

    for (i = 0; i < DEBUG_BUF_LEN; ++i)
      debug_buf[i] = ' ';
    fprintf (dest,"%s","\n");

    for (i = 0; i < curlen; i++)	/* for each byte in this line	*/			    	
      {	  		    		
      ch = *ptr++;  			/* get next character		*/

      hex = (ch >> 4) & 0x0f;		/* write HEX chars for the byte	*/
      hex = cvt_nibble(hex);
      debug_buf[hex_index++] = hex;
      hex = ch & 0x0f;
      hex = cvt_nibble(hex);
      debug_buf[hex_index++] = hex;

      debug_buf[hex_index++] = ' ';

      if (isprint(ch))			/* print ASCII portion		*/
        debug_buf[ascii_index] = ch;
      else
        debug_buf[ascii_index] = '.';	/* just put a '.' there 	*/
      ascii_index++;

      if (i == 7)			/* half way through line	*/
        debug_buf[hex_index++] = ' ';   /* put an extra space here 	*/
      } 				/* end of line			*/

    debug_buf[ascii_index++] = '*';
    debug_buf[ascii_index++] = 0;
    for (i = hex_index; i < 58; i++)
      debug_buf[i] = ' ';  /* clear the buffer between hex and ascii data */

					/* write out the line		*/
    fprintf (dest,"%s",debug_buf);

    addr += 16;				/* prepare for next line	*/
    len -= curlen;
    }
  }


#endif


