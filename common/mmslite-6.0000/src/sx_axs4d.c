/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	           1994 - 2008 All Rights Reserved			*/
/*									*/
/* MODULE NAME : SX_AXS4D.C  						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION :                                                 */
/*	Functions to convert local data to text and vice versa.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*			sxaLocalToText					*/
/*			sxaTextToLocal					*/
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date    Who  Rev    Comments                                    	*/
/* -------- --- ------  ----------------------------------------------- */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/23/10 JRB		Add "maxsize" arg on "*ToString" calls.		*/
/* 08/26/09 JRB    23   Chg sscanf arg type to avoid warning.		*/
/* 09/16/08 LWP	   22	Ported to QNX Neutrino				*/
/* 01/30/08 EJV    21   sprintf,sscanf: use S_FMT_* macros for ST_INT64	*/
/* 10/24/06 JRB    20   sxaTextToLocal: do not alloc and copy text	*/
/*			 buffer (caused memory leak).			*/
/*			sxaLocalToText: do not alloc text buf (caused	*/
/*			 memory leak). Go back to using global buffer.	*/
/*			Add thread-safe sxaLocalToText2.		*/
/*			Del unused sxaTextToLoc.			*/
/*			sxaAddString: DO NOT write past end of buffer.	*/
/*			sxaCheckOctetString: handle empty input string.	*/
/* 10/17/06 MDE    19    Added support for unsigned hex ints (32 bit)	*/
/* 04/04/06 CRM    18   Added SXD_CONTEXT to eliminate global variables */
/* 01/30/06 GLB    17   Integrated porting changes for VMS              */
/* 09/01/04 EJV    16   Added new include "sx_arb.h"			*/
/* 03/16/04 EJV    15   Added ST_(U)LONG typecast to sprintf,on some sys*/
/*			     ST_(U)INT32 can be (unsigned) long or int.	*/
/*			Use SISCO types in place of C types.		*/
/* 01/23/04 JRB    14   Add LYNX support.				*/
/* 01/08/04 EJV    13   Ported 64-bit funcs to sun.			*/
/* 10/17/03 JRB    12   Add SD_CONST.					*/
/* 10/13/03 EJV    11   Replaced defined(QNX) with defined(__QNX__).	*/
/* 07/09/03 JRB    10   Add module description.				*/
/* 06/11/03 EJV    09   __alpha: eliminated warning in sxaTextToLocInt16*/
/* 04/14/03 JRB    08   Add linux support. Use #error.			*/
/* 08/16/02 EJV    07   Ported 64-bit funcs to UNIX.			*/
/* 07/07/02 GLB    06   Remove unreferenced sxaLocToTextUtc             */
/*                      Remove unreferenced sxaTextToLocUtc             */
/* 04/24/02 EJV    05   Replaced ltoa with sprintf for !(_WIN32 || QNX).*/
/*			Replaced gcvt with sprintf for !(_WIN32	|| QNX).*/
/*			Added DEBUG_SISCO, SD_CONST for thisFileName.	*/
/*			sxaCheckOctetString: initialized nibble.	*/
/*			Added empty sxaLocToTextUtc and sxaTextToLocUtc	*/
/*			Corrected 2-nd param name (static fun protos)	*/
/* 04/18/02 EJV    04   Added NULL to sxaTextToLocTbl for UTC Time fun.	*/
/* 06/08/01 EJV    03   Changed 2nd param to time_t in sxaLocToTextGtime*/
/*			sxaTextToLocGtime.				*/
/* 05/21/01 MDE    02   Fixups for QNX					*/
/* 03/23/01 MDE    01   New, derived from axs4data.c			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "time.h" 
#include "time_str.h"
#include "sx_defs.h"
#include "sx_arb.h"
#include "sx_log.h"
#include "mem_chk.h"

/************************************************************************/
/* Define for SLOG debugging mechanism                                  */
/************************************************************************/
/************************************************************************/
/* Use a static pointer to avoid duplication of __FILE__ strings.	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
#define MAX_DDE_DATA_SIZE  30000
#define TEMP_DATA_BUF_SIZE 20000
/*#define DATA_SEPARATOR	   0x7c	 a pipe '|'	*/
#define DATA_SEPARATOR1	   0x0d		/* carraige return */
#define DATA_SEPARATOR2	   0x0a		/* line feed 	*/
/************************************************************************/

typedef struct 
  {
  ST_INT  nStrLen;       /* Current string array total length 	*/
  ST_INT  nDataIdx;      /* Current index in work buffer		*/
  ST_CHAR *sTmpData;     /* Temporary work buffer			*/
  ST_INT  nStringCount;  /* count strings added to buffer		*/
  ST_BOOLEAN textDataErrorDetected;
  ST_CHAR *sxaTextWorkBuf;
  ST_UINT workBufLen;	/* length of work buffer	*/
  } SXD_CONTEXT;

/************************************************************************/
/* Global variables							*/
/************************************************************************/
static ST_CHAR gTextBuf [MAX_DDE_DATA_SIZE];	/* sxaLocalToText encodes here*/

/************************************************************************/

static ST_VOID sxaAddString (ST_VOID *usr, ST_CHAR *str);
static ST_INT sxaGetNextString (ST_VOID *usr);
static ST_VOID leftTrim (ST_CHAR *pString);

static ST_INT sxaCheckIntString (ST_CHAR*);
static ST_INT sxaCheckUintString (ST_CHAR*);
static ST_INT sxaCheckFloatString (ST_CHAR*);
static ST_VOID SkipWhiteSpace (ST_CHAR**);
static ST_INT sxaCheckOctetString (ST_CHAR*, ST_CHAR*, ST_INT*);
static ST_INT sxaCheckBitString (ST_CHAR*, ST_CHAR*, ST_INT*);


/************************************************************************/
/************************************************************************/
/* LOCAL TO TEXT DATA CONVERSION					*/
/************************************************************************/
/************************************************************************/
/* This table is used as input to the MMS-EASE arbitrary data handling	*/
/* function.  All functions return SUCCESS or FAILURE			*/

static ST_RET sxaLocToTextBool (ST_VOID *usr, ST_BOOLEAN *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextInt8 (ST_VOID *usr, ST_INT8 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextInt16 (ST_VOID *usr, ST_INT16 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextInt32 (ST_VOID *usr, ST_INT32 *pSrc, RUNTIME_TYPE *rt);
#if defined(INT64_SUPPORT)
static ST_RET sxaLocToTextInt64 (ST_VOID *usr, ST_INT64 *pSrc, RUNTIME_TYPE *rt);
#endif
static ST_RET sxaLocToTextUint8 (ST_VOID *usr, ST_UINT8 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextUint16 (ST_VOID *usr, ST_UINT16 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextUint32 (ST_VOID *usr, ST_UINT32 *pSrc, RUNTIME_TYPE *rt);
#if defined(INT64_SUPPORT)
static ST_RET sxaLocToTextUint64 (ST_VOID *usr, ST_UINT64 *pSrc, RUNTIME_TYPE *rt);
#endif
static ST_RET sxaLocToTextBcd1 (ST_VOID *usr, ST_INT8 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextBcd2 (ST_VOID *usr, ST_INT16 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextBcd4 (ST_VOID *usr, ST_INT32 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextFlt (ST_VOID *usr, ST_FLOAT *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextDbl (ST_VOID *usr, ST_DOUBLE *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextStr (ST_VOID *usr, ST_CHAR *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextOct (ST_VOID *usr, ST_UCHAR *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextBs  (ST_VOID *usr, ST_UCHAR *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextBtime4 (ST_VOID *usr, ST_INT32 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextBtime6 (ST_VOID *usr, ST_INT32 *pSrc, RUNTIME_TYPE *rt);
static ST_RET sxaLocToTextGtime (ST_VOID *usr, time_t *pSrc, RUNTIME_TYPE *rt);
#if 0
static ST_RET sxaLocToTextUtc   (ST_VOID *usr, MMS_UTC_TIME *pSrc, RUNTIME_TYPE *rt);
#endif
static SXD_ARB_DATA_CTRL sxaLocToTextTbl =
  {
  NULL,				/* arrStart 	*/
  NULL,				/* arrEnd    	*/
  NULL,				/* strStart  	*/
  NULL,				/* strEnd    	*/
  sxaLocToTextInt8,  		/* int8      	*/
  sxaLocToTextInt16,		/* int16     	*/
  sxaLocToTextInt32, 		/* int32     	*/
#if defined(INT64_SUPPORT)
  sxaLocToTextInt64,		/* int64	*/
#endif
  sxaLocToTextUint8, 		/* uint8     	*/
  sxaLocToTextUint16,		/* uint16    	*/
  sxaLocToTextUint32,		/* uint32    	*/
#if defined(INT64_SUPPORT)
  sxaLocToTextUint64,		/* uint64	*/
#endif
  sxaLocToTextFlt,   		/* flt       	*/
  sxaLocToTextDbl,   		/* dbl       	*/
  sxaLocToTextOct,		/* oct       	*/
  sxaLocToTextBool,		/* bool      	*/
  sxaLocToTextBcd1, 	    	/* bcd1      	*/
  sxaLocToTextBcd2, 	    	/* bcd2      	*/
  sxaLocToTextBcd4, 	    	/* bcd4      	*/
  sxaLocToTextBs,		/* bs	       	*/
  sxaLocToTextStr,		/* visible str	*/
  sxaLocToTextBtime4,		/* Bin Time    	*/
  sxaLocToTextBtime6,		/* Bin Time    	*/
  sxaLocToTextGtime,		/* Gen Time    	*/
  NULL 				/* UTC Time	*/
  };				

/************************************************************************/
/*			sxaLocalToText					*/
/* WARNING: this function is not thread-safe because it uses the global	*/
/*          buffer "gTextBuf". Better to call sxaLocalToText2 directly.	*/
/* NOTE: "ms_local_to_text" function produces better text for logging,	*/
/*       but this function produces text compatible with sxaTextToLocal.*/ 
/************************************************************************/
/* This is the main entry point for LOCAL->TEXT data conversion 	*/

ST_CHAR *sxaLocalToText (ST_VOID *dataPtr, SD_CONST RUNTIME_TYPE *rtHead, ST_INT numRt)
  {
  return (sxaLocalToText2 (dataPtr, rtHead, numRt,
	  gTextBuf,			/* Use global text buffer	*/
	  sizeof (gTextBuf)));		/* size of global text buffer 	*/
  }

/************************************************************************/
/*			sxaLocalToText2					*/
/* This function should be thread-safe as long as caller's "textBuf"	*/
/* is not accessed by other threads.					*/
/* NOTE: "ms_local_to_text" function produces better text for logging,	*/
/*       but this function produces text compatible with sxaTextToLocal.*/ 
/************************************************************************/
ST_CHAR *sxaLocalToText2 (ST_VOID *dataPtr, SD_CONST RUNTIME_TYPE *rtHead, ST_INT numRt,
	ST_CHAR *textBuf,	/* User buffer in which to write text	*/
	ST_UINT textBufSize)	/* size of user buffer			*/
  {
SXD_CONTEXT sxdContext;
ST_CHAR tempDataBuf[TEMP_DATA_BUF_SIZE];
ST_RET rtnVal;

/* Make a global temporary buffer available (easier than calloc..)	*/
  sxdContext.sTmpData = (ST_CHAR *) tempDataBuf;
  sxdContext.sxaTextWorkBuf = textBuf;	/* save user buffer pointer	*/
  sxdContext.workBufLen = textBufSize;	/* save user buffer size	*/

  sxdContext.nStrLen = 0;
  sxdContext.nDataIdx = 0;	     
  sxdContext.nStringCount = 0;

  sxdContext.textDataErrorDetected = SD_FALSE;

  rtnVal = sxd_process_arb_data ((ST_CHAR *) dataPtr, rtHead, numRt, &sxdContext, 
			         &sxaLocToTextTbl, NULL);

  if (rtnVal != SD_SUCCESS)
    {
    return (NULL);
    }

  if (sxdContext.textDataErrorDetected)
    {
    return (NULL);
    }

  /* Null terminate the string						*/
  sxdContext.sxaTextWorkBuf[sxdContext.nStrLen + 1] = 0;

  return (sxdContext.sxaTextWorkBuf);
  }

/************************************************************************/
/*			sxaLocToTextInt8 ()				*/
/************************************************************************/

static ST_RET sxaLocToTextInt8 (ST_VOID *usr, ST_INT8 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%d", (ST_INT) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sxaLocToTextInt16 ()				*/
/************************************************************************/

static ST_RET sxaLocToTextInt16 (ST_VOID *usr, ST_INT16 *pSrc, RUNTIME_TYPE *rt)		
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%d",(ST_INT) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sxaLocToTextInt32 ()				*/
/************************************************************************/

static ST_RET sxaLocToTextInt32 (ST_VOID *usr, ST_INT32 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%ld", (ST_LONG) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sxaLocToTextInt64 ()				*/
/************************************************************************/

#if defined(INT64_SUPPORT)
static ST_RET sxaLocToTextInt64 (ST_VOID *usr, ST_INT64 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData, S_FMT_INT64, *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }
#endif

/************************************************************************/
/*			sxaLocToTextUint8 ()				*/
/************************************************************************/

static ST_RET sxaLocToTextUint8 (ST_VOID *usr, ST_UINT8 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%u",(ST_UINT) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextUint16 ()				*/
/************************************************************************/

static ST_RET sxaLocToTextUint16 (ST_VOID *usr, ST_UINT16 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%u",(ST_UINT) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextUint32 ()				*/
/************************************************************************/

static ST_RET sxaLocToTextUint32 (ST_VOID *usr, ST_UINT32 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%lu", (ST_ULONG) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextUint64 ()				*/
/************************************************************************/

#if defined(INT64_SUPPORT)
static ST_RET sxaLocToTextUint64 (ST_VOID *usr, ST_UINT64 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData, S_FMT_UINT64, *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }
#endif

/************************************************************************/
/*			sxaLocToTextBcd1 ()			 	*/
/************************************************************************/

static ST_RET sxaLocToTextBcd1 (ST_VOID *usr, ST_INT8 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%d", (ST_INT) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextBcd2 ()				*/
/************************************************************************/

static ST_RET sxaLocToTextBcd2 (ST_VOID *usr, ST_INT16 *pSrc, RUNTIME_TYPE *rt)		
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%d",(ST_INT) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }	     

/************************************************************************/
/*			sxaLocToTextBcd4 ()				*/
/************************************************************************/

static ST_RET sxaLocToTextBcd4 (ST_VOID *usr, ST_INT32 *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%ld", (ST_LONG) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextBool ()		      		*/
/************************************************************************/

static ST_RET sxaLocToTextBool (ST_VOID *usr, ST_BOOLEAN *pSrc, RUNTIME_TYPE *rt)
  {     
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
  sprintf (sxdContext->sTmpData,"%u",(ST_UINT) *pSrc);
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextFlt ()				*/
/************************************************************************/

static ST_RET sxaLocToTextFlt (ST_VOID *usr, ST_FLOAT *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
#if defined(_WIN32) || (defined(__QNX__) && !defined(__QNXNTO__))
  /* 7 significant digits */
  gcvt((ST_DOUBLE) *pSrc, 7, sxdContext->sTmpData);
#else
  sprintf (sxdContext->sTmpData, "%.7g", (ST_DOUBLE) (*pSrc));
#endif
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextDbl ()		     		*/
/************************************************************************/

static ST_RET sxaLocToTextDbl (ST_VOID *usr, ST_DOUBLE *pSrc, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;
#if defined(_WIN32) || (defined(__QNX__) && !defined(__QNXNTO__))
  /* 16 significant digits */
  gcvt(*pSrc, 16, sxdContext->sTmpData);
#else
  sprintf (sxdContext->sTmpData, "%.16g", *pSrc);
#endif
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextStr ()		      		*/
/************************************************************************/

static ST_RET sxaLocToTextStr (ST_VOID *usr, ST_CHAR *pSrc, RUNTIME_TYPE *rt)
  {
  sxaAddString (usr, pSrc);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextOct				*/
/************************************************************************/

static ST_RET sxaLocToTextOct (ST_VOID *usr, ST_UCHAR *pSrc, RUNTIME_TYPE *rt)
  {
ST_INT i, k;
ST_INT numBytes;
ST_INT16 *sp;
ST_CHAR *destBuf;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

  sxdContext->sTmpData[0] = 0;
  numBytes = rt->u.p.el_len;

  if (numBytes < 0)		/* a variable length octet string	*/
    {
    sp = (ST_INT16 *) pSrc;
    numBytes = *sp;
    k=2;
    }
  else
    k=0;

  /* We take 3 bytes per octet, make sure it fits			*/
  if (numBytes > TEMP_DATA_BUF_SIZE/3)
    {
    SXLOG_NERR0 ("Octet String to encode too long");
    return SD_FAILURE;
    }

  destBuf = sxdContext->sTmpData;
  for (i = 0; i < numBytes; ++i, ++k)
    {
    sprintf (destBuf, "%02x ", (ST_UINT) pSrc[k]);
    destBuf += 3;
    }

  /* Eliminate the trailing space					*/
  *(destBuf - 1) = 0;
  sxaAddString (usr, sxdContext->sTmpData);
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaLocToTextBs					*/
/************************************************************************/

static ST_RET sxaLocToTextBs  (ST_VOID *usr, ST_UCHAR *pSrc, RUNTIME_TYPE *rt)
  {
ST_INT i;
ST_INT j;
ST_INT k;
ST_INT numBits;
ST_INT16 *sp;
ST_CHAR *destBuf;
ST_UCHAR mask;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

  sxdContext->sTmpData[0] = 0;
  numBits = rt->u.p.el_len;

/* We take 1 dest byte per bit, make sure it fits				*/
  if (numBits > TEMP_DATA_BUF_SIZE-1)
    {
    SXLOG_NERR0 ("Bit String to encode too long");
    return (SD_FAILURE);
    }

  if (numBits < 0)		/* a variable length bit string	*/
    {
    sp = (ST_INT16 *) pSrc;
    numBits = *sp;
    k=2;
    }
  else 
    k=0;

  destBuf = sxdContext->sTmpData;
  for (i = 0; i < numBits; ++k)	/* for each byte, while bits remain	*/
    {
    mask = 0x80;
    for (j = 0; j < 8 && i < numBits; ++i, ++j)
      {
      if (pSrc[k] & mask)
        destBuf[i] = '1';
      else
        destBuf[i] = '0';
      mask >>= 1;
      }
    }
  destBuf[i] = 0;
  sxaAddString (usr, sxdContext->sTmpData);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sxaLocToTextBtime4				*/
/************************************************************************/

static ST_RET sxaLocToTextBtime4 (ST_VOID *usr, ST_INT32 *pSrc, RUNTIME_TYPE *rt)
  {
ST_CHAR timeString[MAX_TIME_STRING_LEN];
  
  if (Btime4ValsToString (timeString, sizeof(timeString), *pSrc))
    {
    SXLOG_NERR0 ("Btime4 String conversion error.");
    return (SD_FAILURE);
    }

  sxaAddString (usr, timeString);
  return (SD_SUCCESS);
  }


/************************************************************************/
/*			sxaLocToTextBtime6				*/
/************************************************************************/

static ST_RET sxaLocToTextBtime6 (ST_VOID *usr, ST_INT32 *pSrc, RUNTIME_TYPE *rt)
  {
ST_INT32 *pDays, *pMsec;
ST_CHAR timeString[MAX_TIME_STRING_LEN];
  
  pMsec = pSrc;
  pDays = (pSrc+1);
  if (Btime6ValsToString (timeString, sizeof(timeString), *pDays, *pMsec))
    {
    SXLOG_NERR0 ("Btime6 String conversion error.");
    return (SD_FAILURE);
    }

  sxaAddString (usr, timeString);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sxaLocToTextGtime				*/
/************************************************************************/

static ST_RET sxaLocToTextGtime (ST_VOID *usr, time_t *pSrc, RUNTIME_TYPE *rt)
  {
ST_CHAR timeString[MAX_TIME_STRING_LEN];

  if (tstrTimeToString (*pSrc, timeString, sizeof(timeString)))
    {
    SXLOG_NERR0 ("Time to String conversion error");
    return (SD_FAILURE);
    }

  sxaAddString (usr, timeString);
  return (SD_SUCCESS);
  }
#if 0
/************************************************************************/
/*			sxaLocToTextUtc					*/
/************************************************************************/

static ST_RET sxaLocToTextUtc (ST_VOID *usr, MMS_UTC_TIME *pSrc, RUNTIME_TYPE *rt)
  {
    SXLOG_ERR0 ("sxaLocToTextUtc error: function not implemented");
    return (SD_FAILURE);
  }
#endif
/************************************************************************/
/*			sxaAddString					*/
/************************************************************************/
/* This function is used to add a string to the working string buffer	*/

static ST_VOID sxaAddString (ST_VOID *usr, ST_CHAR *str)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

  /* Watch out for overrun						*/
  if ((sxdContext->nStrLen + strlen (str) + 3) >= sxdContext->workBufLen)
    {
    SXLOG_NERR0 ("Local->Text buffer overrun");
    sxdContext->textDataErrorDetected = SD_TRUE;
    return;	/* STOP: DO NOT write any more data to buffer	*/
    }

  /* If not the first one, need to add seperators			*/
  if (sxdContext->nStringCount > 0)
    {
/*    sxaTextWorkBuf[nStrLen++] = DATA_SEPARATOR;			*/
    sxdContext->sxaTextWorkBuf[sxdContext->nStrLen++] = DATA_SEPARATOR1;
    sxdContext->sxaTextWorkBuf[sxdContext->nStrLen++] = DATA_SEPARATOR2;
    sxdContext->sxaTextWorkBuf[sxdContext->nStrLen] = 0x00;
    strcat (sxdContext->sxaTextWorkBuf,str);
    }
  else
    strcpy (sxdContext->sxaTextWorkBuf,str);

  sxdContext->nStringCount++;
  sxdContext->nStrLen += (int) strlen (str);
  }

/************************************************************************/
/************************************************************************/
/* TEXT TO LOCAL DATA CONVERSION					*/
/************************************************************************/
/************************************************************************/
/* This table is used as input to the MMS-EASE arbitrary data handling	*/
/* function.  All functions return SUCCESS or FAILURE			*/

static ST_RET sxaTextToLocBool (ST_VOID *usr, ST_BOOLEAN *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocInt8 (ST_VOID *usr, ST_INT8 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocInt16 (ST_VOID *usr, ST_INT16 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocInt32 (ST_VOID *usr, ST_INT32 *pDest, RUNTIME_TYPE *rt);
#if defined(INT64_SUPPORT)
static ST_RET sxaTextToLocInt64 (ST_VOID *usr, ST_INT64 *pDest, RUNTIME_TYPE *rt);
#endif
static ST_RET sxaTextToLocUint8 (ST_VOID *usr, ST_UINT8 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocUint16 (ST_VOID *usr, ST_UINT16 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocUint32 (ST_VOID *usr, ST_UINT32 *pDest, RUNTIME_TYPE *rt);
#if defined(INT64_SUPPORT)
static ST_RET sxaTextToLocUint64 (ST_VOID *usr, ST_UINT64 *pDest, RUNTIME_TYPE *rt);
#endif
static ST_RET sxaTextToLocBcd1 (ST_VOID *usr, ST_INT8 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocBcd2 (ST_VOID *usr, ST_INT16 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocBcd4 (ST_VOID *usr, ST_INT32 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocFlt (ST_VOID *usr, ST_FLOAT *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocDbl (ST_VOID *usr, ST_DOUBLE *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocStr (ST_VOID *usr, ST_CHAR *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocOct (ST_VOID *usr, ST_UCHAR *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocBs  (ST_VOID *usr, ST_UCHAR *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocBtime4 (ST_VOID *usr, ST_INT32 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocBtime6 (ST_VOID *usr, ST_INT32 *pDest, RUNTIME_TYPE *rt);
static ST_RET sxaTextToLocGtime (ST_VOID *usr, time_t *pDest, RUNTIME_TYPE *rt);
#if 0
static ST_RET sxaTextToLocUtc   (ST_VOID *usr, MMS_UTC_TIME *pDest, RUNTIME_TYPE *rt);
#endif

static SXD_ARB_DATA_CTRL sxaTextToLocTbl =
  {
  NULL,				/* arrStart 	*/
  NULL,				/* arrEnd    	*/
  NULL,				/* strStart  	*/
  NULL,				/* strEnd    	*/
  sxaTextToLocInt8,  		/* int8      	*/
  sxaTextToLocInt16,		/* int16     	*/
  sxaTextToLocInt32, 		/* int32     	*/
#if defined(INT64_SUPPORT)
  sxaTextToLocInt64,		/* int64	*/
#endif
  sxaTextToLocUint8, 		/* uint8     	*/
  sxaTextToLocUint16,		/* uint16    	*/
  sxaTextToLocUint32,		/* uint32    	*/
#if defined(INT64_SUPPORT)
  sxaTextToLocUint64,		/* uint64	*/
#endif
  sxaTextToLocFlt,   		/* flt       	*/
  sxaTextToLocDbl,   		/* dbl       	*/
  sxaTextToLocOct,		/* oct       	*/
  sxaTextToLocBool,		/* bool      	*/
  sxaTextToLocBcd1, 	    	/* bcd1      	*/
  sxaTextToLocBcd2, 	    	/* bcd2      	*/
  sxaTextToLocBcd4, 	    	/* bcd4      	*/
  sxaTextToLocBs,		/* bs	       	*/
  sxaTextToLocStr,		/* visible str	*/
  sxaTextToLocBtime4,		/* Bin Time    	*/
  sxaTextToLocBtime6,		/* Bin Time    	*/
  sxaTextToLocGtime,		/* Gen Time    	*/
  NULL 				/* UTC Time	*/
  };				
		
/************************************************************************/
/*			sxaTextToLocal					*/
/************************************************************************/
/* This is the main entry point for TEXT->LOCAL data conversion 	*/

ST_RET sxaTextToLocal (ST_CHAR *pSource, ST_VOID *pDest, ST_INT numRt, SD_CONST RUNTIME_TYPE *rtHead)
  {
SXD_CONTEXT sxdContext;
ST_CHAR tempDataBuf[TEMP_DATA_BUF_SIZE];
ST_RET rtnVal;

/* Make a global temporary buffer available (easier than calloc..)	*/
  sxdContext.sTmpData = tempDataBuf;
  sxdContext.sxaTextWorkBuf = pSource;	/* point to input string	*/
  sxdContext.nStrLen = (int) strlen (sxdContext.sxaTextWorkBuf);
  sxdContext.nDataIdx = 0;	     
  sxdContext.nStringCount = 0;

  sxdContext.textDataErrorDetected = SD_FALSE;

  rtnVal = sxd_process_arb_data ((ST_CHAR *) pDest, rtHead, numRt, &sxdContext, 
			         &sxaTextToLocTbl, NULL);
  if (sxdContext.textDataErrorDetected)
    rtnVal = SD_FAILURE;

  return (rtnVal);
  }

/************************************************************************/
/*			sxaTextToLocBool ()					*/
/************************************************************************/

static ST_RET sxaTextToLocBool (ST_VOID *usr, ST_BOOLEAN *pDest, RUNTIME_TYPE *rt)
  {
ST_INT nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string from the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckIntString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%d", &nDest) == 1) &&
      (nDest >= 0 && nDest <= 255))
    {
    *pDest = (ST_UCHAR) nDest;
    }
  else
    {	   
    SXLOG_NERR1 ("Text '%s' to Local error for type Boolean",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocInt8 ()			 	*/
/************************************************************************/

static ST_RET sxaTextToLocInt8 (ST_VOID *usr, ST_INT8 *pDest, RUNTIME_TYPE *rt)
  {
ST_INT nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckIntString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%d", &nDest) == 1) &&
      (nDest >= -128 && nDest <= 127))
    {
    *pDest = (ST_CHAR) nDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Integer8",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocInt16 ()		      		*/
/************************************************************************/

static ST_RET sxaTextToLocInt16 (ST_VOID *usr, ST_INT16 *pDest, RUNTIME_TYPE *rt)
  {
ST_LONG nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckIntString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%ld", &nDest) == 1) &&
      (nDest >= -32768 && nDest <= 32767))
    {
    *pDest = (ST_INT16) nDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Integer16",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  } 

/************************************************************************/
/*			sxaTextToLocInt32 ()				*/
/************************************************************************/

static ST_RET sxaTextToLocInt32 (ST_VOID *usr, ST_INT32 *pDest, RUNTIME_TYPE *rt)
  {
ST_LONG nDest;
ST_CHAR temp[35];
ST_CHAR *tempPtr;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckIntString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%ld", &nDest) == 1))
    {
#if defined(_WIN32) || defined(__QNX__)
    ltoa(nDest, temp, 10);
#else
    sprintf (temp, "%ld", nDest);
#endif

    /* look out for leading zero's					*/
    if (nDest != 0L)					
      {
      tempPtr = sxdContext->sTmpData;
      while (tempPtr[0] == '0')
        tempPtr++;
      sxdContext->sTmpData = tempPtr;
      }

    if (strcmp(sxdContext->sTmpData, temp))
      {
      SXLOG_NERR1 ("Text '%s' to Local error for type Integer32",
                      sxdContext->sTmpData);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      } 
    else  
      *pDest = nDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Integer32",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }

  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocInt64 ()				*/
/************************************************************************/

#if defined(INT64_SUPPORT)
static ST_RET sxaTextToLocInt64 (ST_VOID *usr, ST_INT64 *pDest, RUNTIME_TYPE *rt)
  {
ST_INT64 nDest;
ST_CHAR temp[35];
ST_CHAR *tempPtr;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckIntString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, S_FMT_INT64, &nDest) == 1))
    {
#ifdef _WIN32
    _i64toa(nDest, temp, 10);
#else
    sprintf (temp, S_FMT_INT64, nDest);
#endif

    /* look out for leading zero's					*/
    if (nDest != 0L)					
      {
      tempPtr = sxdContext->sTmpData;
      while (tempPtr[0] == '0')
        tempPtr++;
      sxdContext->sTmpData = tempPtr;
      }

    if (strcmp(sxdContext->sTmpData, temp))
      {
      SXLOG_NERR1 ("Text '%s' to Local error for type Integer64",
                      sxdContext->sTmpData);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      } 
    else  
      *pDest = nDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Integer64",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  } 
#endif

/************************************************************************/
/*			sxaTextToLocUint8 ()				*/
/************************************************************************/

static ST_RET sxaTextToLocUint8 (ST_VOID *usr, ST_UINT8 *pDest, RUNTIME_TYPE *rt)
  {
ST_UINT nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckUintString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%d", &nDest) == 1) &&
      (nDest <= 255))
    {
    *pDest = (ST_UCHAR) nDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Unsigned8",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocUint16 ()				*/
/************************************************************************/

static ST_RET sxaTextToLocUint16 (ST_VOID *usr, ST_UINT16 *pDest, RUNTIME_TYPE *rt)
  {
ST_ULONG nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckUintString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%lu", &nDest) == 1) &&
      (nDest <= 65535))
    {
    *pDest = (ST_UINT16) nDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Unsigned16",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  } 

/************************************************************************/
/*			sxaTextToLocUint32 ()				*/
/************************************************************************/

static ST_RET sxaTextToLocUint32 (ST_VOID *usr, ST_UINT32 *pDest, RUNTIME_TYPE *rt)
  {
ST_ULONG nDest;
ST_CHAR temp[35];
ST_CHAR *tempPtr;
SXD_CONTEXT *sxdContext;


  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* Check for hex format */
  if (sxdContext->sTmpData[0] == '0' && sxdContext->sTmpData[1] == 'x')
    {
    if (sscanf (sxdContext->sTmpData, "0x%lx", &nDest) == 1)
      {  
      *pDest = nDest;
      return SD_SUCCESS;
      }    
    }    

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckUintString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%lu", &nDest) == 1))
    {
#if defined(_WIN32) || defined(__QNX__)
    ultoa(nDest, temp, 10);
#else
    sprintf (temp, "%lu", nDest);
#endif

    /* look out for leading zero's					*/
    if (nDest != 0L)					
      {
      tempPtr = sxdContext->sTmpData;
      while (tempPtr[0] == '0')
        tempPtr++;
      sxdContext->sTmpData = tempPtr;
      }

    if (strcmp(sxdContext->sTmpData, temp))
      {
      SXLOG_NERR1 ("Text '%s' to Local error for type Unsigned32",
                      sxdContext->sTmpData);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      } 
    else 
      *pDest = nDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Unsigned32",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }


/************************************************************************/
/*			sxaTextToLocUint64 ()				*/
/************************************************************************/

#if defined(INT64_SUPPORT)
static ST_RET sxaTextToLocUint64 (ST_VOID *usr, ST_UINT64 *pDest, RUNTIME_TYPE *rt)
  {
ST_UINT64 nDest;
ST_CHAR temp[35];
ST_CHAR *tempPtr;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckUintString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, S_FMT_UINT64, &nDest) == 1))
    {
#ifdef _WIN32
    _ui64toa(nDest, temp, 10);
#else
    sprintf (temp,  S_FMT_UINT64, nDest);
#endif

    /* look out for leading zero's					*/
    if (nDest != 0L)					
      {
      tempPtr = sxdContext->sTmpData;
      while (tempPtr[0] == '0')
        tempPtr++;
      sxdContext->sTmpData = tempPtr;
      }

    if (strcmp(sxdContext->sTmpData, temp))
      {
      SXLOG_NERR1 ("Text '%s' to Local error for type Unsigned64",
                      sxdContext->sTmpData);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      } 
    else 
      *pDest = nDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Unsigned64",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }
#endif

/************************************************************************/
/*			sxaTextToLocBcd1 ()				*/
/************************************************************************/

static ST_RET sxaTextToLocBcd1 (ST_VOID *usr, ST_INT8 *pDest, RUNTIME_TYPE *rt)
  {
ST_INT nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckIntString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%d", &nDest) == 1))
    {
    if ( (rt->u.p.el_len == 1) && (nDest >= 0 && nDest <= 9) )
      *pDest = (ST_CHAR) nDest;
    else if ( (rt->u.p.el_len == 2) && (nDest >= 0 && nDest <= 99) )
      *pDest = (ST_CHAR) nDest;
    else
      {
      SXLOG_NERR2 ("Text '%s' to Local error for type BCD%d",
                      sxdContext->sTmpData, rt->u.p.el_len);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      }
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type BCD1",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocBcd2 ()					*/
/************************************************************************/

static ST_RET sxaTextToLocBcd2 (ST_VOID *usr, ST_INT16 *pDest, RUNTIME_TYPE *rt)
  {
ST_UINT nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckIntString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%u", &nDest) == 1))
    {
    if ( (rt->u.p.el_len == 3) && (nDest <= 999) )
      *pDest = (ST_INT16) nDest;
    else if ( (rt->u.p.el_len == 4) && (nDest <= 9999) )
      *pDest = (ST_INT16) nDest;
    else
      {
      SXLOG_NERR2 ("Text '%s' to Local error for type BCD%d",
                      sxdContext->sTmpData, rt->u.p.el_len);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      }
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type BCD2",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocBcd4 ()				*/
/************************************************************************/


static ST_RET sxaTextToLocBcd4 (ST_VOID *usr, ST_INT32 *pDest, RUNTIME_TYPE *rt)
  {
ST_LONG nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckIntString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%ld", &nDest) == 1))
    {
    if ( (rt->u.p.el_len == 5) && (nDest >= 0 && nDest <= 99999L) )
      *pDest = nDest;
    else if ( (rt->u.p.el_len == 6) && (nDest >= 0 && nDest <= 999999L) )
      *pDest = nDest;
    else if ( (rt->u.p.el_len == 7) && 
              (nDest >= 0 && (ST_ULONG) nDest <= 9999999L) )
      *pDest = nDest;
    else if ( (rt->u.p.el_len == 8) &&
              ((ST_ULONG) nDest <= 99999999L) )
      *pDest = nDest;
    else
      {
      SXLOG_NERR2 ("Text '%s' to Local error for type BCD%d",
                      sxdContext->sTmpData, rt->u.p.el_len);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      }
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type BCD4",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocFlt ()				*/
/************************************************************************/

static ST_RET sxaTextToLocFlt (ST_VOID *usr, ST_FLOAT *pDest, RUNTIME_TYPE *rt)
  {
ST_DOUBLE dDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;


/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/
  if (sxaCheckFloatString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%lf", &dDest) == 1))
    { 

    *pDest = (ST_FLOAT) dDest;
    
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Float",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocDbl ()					*/
/************************************************************************/
  
static ST_RET sxaTextToLocDbl (ST_VOID *usr, ST_DOUBLE *pDest, RUNTIME_TYPE *rt)
  {
ST_DOUBLE dDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* trim any leading blanks						*/
  leftTrim (sxdContext->sTmpData);

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckFloatString (sxdContext->sTmpData) == SD_SUCCESS && 
      (sscanf (sxdContext->sTmpData, "%lf", &dDest) == 1))
    {
    *pDest = dDest;
    }
  else
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Double",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocStr ()		       		*/
/************************************************************************/

static ST_RET sxaTextToLocStr (ST_VOID *usr, ST_CHAR *pDest, RUNTIME_TYPE *rt)
  {
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    {
    sxdContext->sTmpData[0] = 0x00;
    }
  

/* Verify that the string will fit into the destination			*/
  if (strlen (sxdContext->sTmpData) > (ST_UINT) (rt->el_size - 1))
    {
    sxdContext->textDataErrorDetected = SD_TRUE;
    SXLOG_NERR2 ("String too long (%d max) : '%s'", (ST_INT) (rt->el_size - 1),
    						       sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }

  strcpy (pDest, sxdContext->sTmpData);
  return SD_SUCCESS;
  }


/************************************************************************/
/*			sxaTextToLocOct					*/
/************************************************************************/

static ST_RET sxaTextToLocOct (ST_VOID *usr, ST_UCHAR *pDest, RUNTIME_TYPE *rt)
  {
ST_CHAR outData[TEMP_DATA_BUF_SIZE];
ST_INT outDataLen;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/

  if (sxaCheckOctetString (sxdContext->sTmpData, outData, &outDataLen) != SD_SUCCESS)
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Octet String",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }

  if (rt->u.p.el_len <0)
    {
    if (outDataLen > (ST_INT) abs(rt->u.p.el_len))
      {
      SXLOG_NERR2 ("Octet String too long (%d max) : '%s'", 
    		       abs (rt->u.p.el_len), sxdContext->sTmpData);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      }
    *((ST_INT16 *) pDest) = (ST_INT16) outDataLen;
    memcpy (pDest+2, outData, outDataLen);
    }
  else
    {
    if (outDataLen != (ST_INT) rt->u.p.el_len)
      {
      SXLOG_NERR2 ("Invalid Length Octet String (required length=%d octets) : '%s'",
       		      rt->u.p.el_len, sxdContext->sTmpData);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      }
    memcpy (pDest, outData, outDataLen);
    }

  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocBs					*/
/************************************************************************/

static ST_RET sxaTextToLocBs  (ST_VOID *usr, ST_UCHAR *pDest, RUNTIME_TYPE *rt)
  {
ST_CHAR outData[TEMP_DATA_BUF_SIZE];
ST_INT outDataLen, bstringLength;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

/* Verify the string contents are correct for the data type		*/
/* then convert to local format						*/
  
  if (sxaCheckBitString (sxdContext->sTmpData, outData, &outDataLen) != SD_SUCCESS)
    {
    SXLOG_NERR1 ("Text '%s' to Local error for type Bit String",
                      sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }

  bstringLength = (int) strlen(sxdContext->sTmpData);
  if (rt->u.p.el_len <0)		/* variable length bit string 	*/
    {
    if (bstringLength > abs(rt->u.p.el_len))
      {
      SXLOG_NERR2 ("Variable Length Bit String too long (%d bits max) : '%s'",
        	      abs(rt->u.p.el_len), sxdContext->sTmpData);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      }
    *((ST_INT16 *) pDest) = (ST_INT16) bstringLength;
    memcpy (pDest+2, outData, outDataLen);
    }
  else
    {
    if (bstringLength != rt->u.p.el_len)
      {
      SXLOG_NERR2 ("Invlid Length Bit String  (required length=%d bits) : '%s'", 
                      rt->u.p.el_len, sxdContext->sTmpData);
      sxdContext->textDataErrorDetected = SD_TRUE;
      return SD_FAILURE;
      }
    memcpy (pDest, outData, outDataLen);
    }

  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocBtime4				*/
/************************************************************************/

static ST_RET sxaTextToLocBtime4 (ST_VOID *usr, ST_INT32 *pDest, RUNTIME_TYPE *rt)
  {
ST_INT32 nDest;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

  if (Btime4StringToVals (sxdContext->sTmpData, &nDest))
    {
    SXLOG_NERR1 ("Local Btime4 conversion error '%s'", sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }

  memcpy(pDest, &nDest, sizeof(ST_INT32));
  return SD_SUCCESS;
  }


/************************************************************************/
/*			sxaTextToLocBtime6				*/
/************************************************************************/

static ST_RET sxaTextToLocBtime6 (ST_VOID *usr, ST_INT32 *pDest, RUNTIME_TYPE *rt)
  {
ST_INT32 nDest[2];
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

  if (Btime6StringToVals (sxdContext->sTmpData, &nDest[1], &nDest[0]))
    {
    SXLOG_NERR1 ("Local Btime6 conversion error '%s'", sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }

  memcpy(pDest, nDest, 2*sizeof(ST_INT32));
  return SD_SUCCESS;
  }

/************************************************************************/
/*			sxaTextToLocGtime					*/
/************************************************************************/

static ST_RET sxaTextToLocGtime (ST_VOID *usr, time_t *pDest, RUNTIME_TYPE *rt)
  {
time_t t;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Get the next sub-string frm the source string into 'sTmpData'	*/
  if (sxaGetNextString (usr) != SD_SUCCESS)
    return SD_FAILURE;

  if (tstrStringToTime (sxdContext->sTmpData, &t))
    {
    SXLOG_NERR1 ("Local time conversion error '%s'", sxdContext->sTmpData);
    sxdContext->textDataErrorDetected = SD_TRUE;
    return SD_FAILURE;
    }

  *pDest = t;
  return SD_SUCCESS;
  }

#if 0
/************************************************************************/
/*			sxaTextToLocUtc					*/
/************************************************************************/

static ST_RET sxaTextToLocUtc (ST_VOID *usr, MMS_UTC_TIME *pDest, RUNTIME_TYPE *rt)
  {
    SXLOG_ERR0 ("sxaTextToLocUtc error: function not implemented");
    return (SD_FAILURE);
  }
#endif

/************************************************************************/
/************************************************************************/
/*			sxaGetNextString				*/
/************************************************************************/
/* Get the next sub-string frm the source string into 'sTmpData'	*/

static ST_INT sxaGetNextString (ST_VOID *usr)
  {
ST_INT  nCount;
ST_INT  nIdx;
ST_CHAR c;
SXD_CONTEXT *sxdContext;

  sxdContext = (SXD_CONTEXT *) usr;

/* Copy the data from data buffer to temporary data buffer.		*/
/* Start at the current position, strip off learing delimiters, 	*/
/* accept characters until the next delimiter				*/
/*									*/
/* CF_TEXT:  Each line ends with a carriage return/linefeed combination.*/
/*	     A null character signals the end of the data.		*/
  nCount = 0;
  nIdx = sxdContext->nDataIdx;
  while (sxdContext->nStrLen > 0)
    {
    sxdContext->nStrLen--;
    c = sxdContext->sxaTextWorkBuf[nIdx++];
    if (c == 0x00 || c == 0x0d)
      {
      break;	/* This is the end of this component of the string	*/
      }

    sxdContext->sTmpData[nCount++] = c;
    }

  while (sxdContext->nStrLen > 0)
    {
    c = sxdContext->sxaTextWorkBuf[nIdx];
    if (c != 0x00 && c != 0x0a )
      {
      break;	/* This is strips out the linefeed			*/
      }
    nIdx++;
    sxdContext->nStrLen--;
    }

  sxdContext->nDataIdx = nIdx;
  sxdContext->sTmpData[nCount] = 0x00;
  return (SD_SUCCESS);
  }

/************************************************************************/
/* leftTrim:  remove leading spaces from text string			*/
/************************************************************************/

static ST_VOID leftTrim (ST_CHAR *pString)
  {
ST_CHAR outData[TEMP_DATA_BUF_SIZE];
ST_INT len, startPos=0, i=0;

  len = (int) (strlen (pString));
  if (!len)
    return;

  strcpy (outData, pString);
  while (i<len && outData[i] == ' ')
    {
    startPos++;
    i++;
    }

  strcpy (pString, &outData[startPos]);
  }

/************************************************************************/
/************************************************************************/
/*			sxaCheckIntString				*/
/************************************************************************/
/* Verify that this is valid integer data				*/

static ST_INT sxaCheckIntString (ST_CHAR *pC)
  {
ST_INT slen;
ST_INT i;

  slen = (int) strlen (pC);

/* Make sure there are some characers to look at			*/
  if (!slen)
    return (SD_FAILURE);

/* Look at each character in the string and verify it is valid		*/
//   for (i = 0; i < slen; ++i) 
//     {
//     if (isdigit(pC[i]))
//       continue;
//     if (i == 0  && (pC[i] == '-'))
//       continue;
// 
//     return (SD_FAILURE);
//     }
  return (SD_SUCCESS);
  }


/************************************************************************/
/*			sxaCheckUintString				*/
/************************************************************************/
/* Verify that this is valid unsigned integer data			*/

static ST_INT sxaCheckUintString (ST_CHAR *pC)
  {
ST_INT slen;
ST_INT i;

  slen = (int) strlen (pC);

/* Make sure there are some characters to look at			*/
  if (!slen)
    return (SD_FAILURE);

/* Look at each character in the string and verify it is valid		*/
  for (i = 0; i < slen; ++i) 
    {
    if (isdigit(pC[i]))
      continue;

    return (SD_FAILURE);
    }
  return (SD_SUCCESS);
  }


/************************************************************************/
/*			sxaCheckFloatString				*/
/************************************************************************/
/* Verify that this is valid floating point data			*/

static ST_INT sxaCheckFloatString (ST_CHAR *pC)
  {
ST_INT slen;
ST_INT i;

  slen = (int) strlen (pC);

/* Make sure there are some characers to look at			*/
  if (!slen)
    return (SD_FAILURE);

/* Look at each character in the string and verify it is valid		*/
  for (i = 0; i < slen; ++i) 
    {
    if (isdigit(pC[i]))
      continue;

    if (pC[i] == 'e' ||
        pC[i] == 'E' ||
        pC[i] == '+' ||
        pC[i] == '-' ||
        pC[i] == '.')
      {
      continue;
      }
    return (SD_FAILURE);
    }
  return (SD_SUCCESS);
  }


/************************************************************************/
/*			sxaCheckOctetString				*/
/************************************************************************/
/* Verify that this is valid octet string data				*/

static ST_VOID SkipWhiteSpace (ST_CHAR **p)
  {
ST_CHAR *r;

  r = *p;

  while (isspace (*r) && *r != '\0')
    r++;

  *p = r;
  }


static ST_INT sxaCheckOctetString (ST_CHAR *pC, ST_CHAR *pOutData, ST_INT *pOutLen)
  {
ST_CHAR *p = pC;
ST_BOOLEAN done = SD_FALSE;
ST_INT i = 0;
ST_INT nibble = 1;		/* init to error case				*/
ST_INT digit;

  SkipWhiteSpace (&p);	/* skip white space after delimiter		*/

  if (*p == '\0')
    {	/* input string is empty. Return len = 0.	*/
    *pOutLen = 0;
    return (SD_SUCCESS);
    }

  while (!done)
    {
    if (*p == '\0')
      done = SD_TRUE;
    else
      {
      if (!isxdigit (*p))		/* bad number			*/
        return (SD_FAILURE);

      i = 0;
      nibble = 0;
      while (isxdigit (*p))		/* while hex digits		*/
	{
	if (i > 2000)
	  return (SD_FAILURE);		/* length too long		*/
	digit = *p++;			/* separate nibbles		*/
	digit = isdigit (digit) ? digit - '0' : 10 + (toupper (digit) - 'A'); 
	if (nibble)
	  {
	  nibble = 0;
					/* set low byte			*/
	  pOutData[i] = (pOutData[i] | (ST_CHAR) digit);
	  i++;
          SkipWhiteSpace (&p);
	  }
	else
	  {
	  nibble = 1;
	  pOutData[i] = (ST_CHAR) digit << 4;	/* set high byte*/
	  }
	}
      }
    }

  /* set data length */
  if (nibble == 0)
    *pOutLen = i;
  else
    return (SD_FAILURE);

  return (SD_SUCCESS);
  }


/************************************************************************/
/*			sxaCheckBitString				*/
/************************************************************************/
/* Verify that this is valid bitstring data				*/

static ST_INT sxaCheckBitString (ST_CHAR *pC, ST_CHAR *pOutData, ST_INT *pOutLen)
  {
ST_CHAR *p = pC;
ST_INT i, j;
ST_UCHAR mask;

  i = 0;
  j = 0;
  mask = 0x80;
  while (*p != '\0')
    {

    switch (*p)
      {
      case '0':
        pOutData[i] &= ~mask;
      break;

      case '1':
        pOutData[i] |= mask;
      break;

      default:
        return (SD_FAILURE);
      break;
      }

    j++;		/* check next bit				*/
    mask >>= 1;

    if (j == 8)
      {
      i++;		/* do next byte					*/
      j = 0;
      mask = 0x80;
      }

    p++;
    }

  if (j > 0) 
    i++;
  *pOutLen = i;

  return (SD_SUCCESS);
  }

