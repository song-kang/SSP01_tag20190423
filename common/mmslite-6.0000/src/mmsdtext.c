/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2004 All Rights Reserved					*/
/*									*/
/* MODULE NAME : mmsdtext.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to convert local data to text.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	ms_local_to_text						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/22/10  JRB	   ms_local_to_text: init tmpBuf & arr_loops.	*/
/*			   Add maxsize arg to time functions.		*/
/* 08/26/09  JRB    06     Cast sprintf args to avoid warnings.		*/
/* 09/16/08  LWP    05	   Ported to QNX Neutrino			*/
/* 05/13/08  JRB    04     Fix fraction & add qflags in LocToTextUtc.	*/
/* 09/01/04  JRB    03     Add mmsdefs.h, mms_vvar.h (don't know how	*/
/*			   it compiled before without them).		*/
/* 08/06/04  JRB    02     Cast 1-st arg in call to LocToTextUtf8.	*/
/* 01/23/04  JRB    01     New. Possible replacement for sxaLocalToText.*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "time.h" 
#include "time_str.h"
#include "mem_chk.h"
#include "mms_log.h"
#include "mmslog.h"
#include "mmsdefs.h"
#include "mms_vvar.h"	/* need RUNTIME_TYPE	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#define TEMP_DATA_BUF_SIZE 20000
#define DATA_SEPARATOR	   '\n'		/* newline	*/

/************************************************************************/
/*			AddString					*/
/* Copy "Src" string to end of the "Dst" string.			*/
/************************************************************************/
static ST_RET AddString (ST_CHAR *Src, ST_CHAR *Dst, ST_UINT DstSize, ST_UINT *pDstLen)
  {
ST_UINT tmpDstLen = *pDstLen;
  /* Watch out for overrun						*/
  if ((tmpDstLen + strlen (Src) + 1) > DstSize)
    {
    MLOG_NERR0 ("Local->Text buffer overrun");
    return (SD_FAILURE);
    }

  /* Add separator after each string. Later strip off last separator.	*/
  strcpy (&Dst[tmpDstLen],Src);
  tmpDstLen += (ST_INT) strlen (Src);
  Dst[tmpDstLen++] = DATA_SEPARATOR;
  *pDstLen = tmpDstLen;	/* update caller's buf len	*/
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextInt8 ()				*/
/************************************************************************/
static ST_RET LocToTextInt8 (ST_INT8 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"%d", (int) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextInt16 ()				*/
/************************************************************************/
static ST_RET LocToTextInt16 (ST_INT16 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)		
  {
  sprintf (text,"%d",(int) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextInt32 ()				*/
/************************************************************************/
static ST_RET LocToTextInt32 (ST_INT32 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"%ld", (long) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextInt64 ()				*/
/************************************************************************/
#if defined(INT64_SUPPORT)
static ST_RET LocToTextInt64 (ST_INT64 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
#ifdef _WIN32
  sprintf (text,"%I64d",*pSrc);
#elif defined(_AIX) || defined(__hpux) || defined(linux) || defined(sun) || defined(__LYNX)
  sprintf (text,"%lld",*pSrc);
#elif (defined(__alpha) && !defined(__VMS)) 
  sprintf (text,"%ld",*pSrc);
#else
  #error Missing INT64 code for this platform.
#endif
  return (SD_SUCCESS);
  }
#endif
/************************************************************************/
/*			LocToTextUint8 ()				*/
/************************************************************************/
static ST_RET LocToTextUint8 (ST_UINT8 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"%u",(unsigned) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextUint16 ()				*/
/************************************************************************/
static ST_RET LocToTextUint16 (ST_UINT16 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"%u",(unsigned) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextUint32 ()				*/
/************************************************************************/
static ST_RET LocToTextUint32 (ST_UINT32 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"%lu",(unsigned long) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextUint64 ()				*/
/************************************************************************/
#if defined(INT64_SUPPORT)
static ST_RET LocToTextUint64 (ST_UINT64 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
#ifdef _WIN32
  sprintf (text,"%I64u", *pSrc);
#elif defined(_AIX) || defined(__hpux) || defined(linux) || defined(sun) || defined(__LYNX)
  sprintf (text,"%llu", *pSrc);
#elif (defined(__alpha) && !defined(__VMS)) 
  sprintf (text,"%lu", *pSrc);
#else
  #error Missing INT64 code for this platform.
#endif
  return (SD_SUCCESS);
  }
#endif
/************************************************************************/
/*			LocToTextBcd1 ()			 	*/
/************************************************************************/
static ST_RET LocToTextBcd1 (ST_INT8 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"%d", (int) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextBcd2 ()				*/
/************************************************************************/
static ST_RET LocToTextBcd2 (ST_INT16 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)		
  {
  sprintf (text,"%d",(int) *pSrc);
  return (SD_SUCCESS);
  }	     
/************************************************************************/
/*			LocToTextBcd4 ()				*/
/************************************************************************/
static ST_RET LocToTextBcd4 (ST_INT32 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"%ld", (long) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextBool ()		      		*/
/************************************************************************/
static ST_RET LocToTextBool (ST_BOOLEAN *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {     
  sprintf (text,"%u",(unsigned) *pSrc);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextFlt ()				*/
/************************************************************************/
static ST_RET LocToTextFlt (ST_FLOAT *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
#if defined(_WIN32) || (defined(__QNX__) && !defined(__QNXNTO__))
  /* 7 significant digits */
  gcvt((ST_DOUBLE) *pSrc, 7, text);
#else
  sprintf (text, "%.7g", (ST_DOUBLE) (*pSrc));
#endif
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextDbl ()		     		*/
/************************************************************************/
static ST_RET LocToTextDbl (ST_DOUBLE *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
#if defined(_WIN32) || (defined(__QNX__) && !defined(__QNXNTO__))
  /* 16 significant digits */
  gcvt(*pSrc, 16, text);
#else
  sprintf (text, "%.16g", *pSrc);
#endif
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextOct				*/
/************************************************************************/
static ST_RET LocToTextOct (ST_UCHAR *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
int i, k;
int numBytes;
ST_INT16 *sp;
ST_CHAR *destBuf;

  text[0] = 0;
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
    MLOG_NERR1 ("Octet String (%d bytes) too long to encode", numBytes);
    return SD_FAILURE;
    }

  destBuf = text;
  for (i = 0; i < numBytes; ++i, ++k)
    {
    sprintf (destBuf, "%02x ", (unsigned int) pSrc[k]);
    destBuf += 3;
    }

  /* Eliminate the trailing space					*/
  *(destBuf - 1) = 0;
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextBs					*/
/************************************************************************/
static ST_RET LocToTextBs  (ST_UCHAR *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
int i;
int j;
int k;
int numBits;
ST_INT16 *sp;
ST_CHAR *destBuf;
ST_UCHAR mask;

  text[0] = 0;
  numBits = rt->u.p.el_len;

/* We take 1 dest byte per bit, make sure it fits				*/
  if (numBits > TEMP_DATA_BUF_SIZE-1)
    {
    MLOG_NERR1 ("Bit String (%d bits) too long to encode", numBits);
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

  destBuf = text;
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
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextBtime4				*/
/************************************************************************/
static ST_RET LocToTextBtime4 (ST_INT32 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text, size_t maxsize)
  {
  if (Btime4ValsToString (text, maxsize, *pSrc))
    {
    MLOG_NERR0 ("Btime4 String conversion error.");
    return (SD_FAILURE);
    }

  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextBtime6				*/
/************************************************************************/
static ST_RET LocToTextBtime6 (ST_INT32 *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text, size_t maxsize)
  {
ST_INT32 *pDays, *pMsec;
  
  pMsec = pSrc;
  pDays = (pSrc+1);
  if (Btime6ValsToString (text, maxsize, *pDays, *pMsec))
    {
    MLOG_NERR0 ("Btime6 String conversion error.");
    return (SD_FAILURE);
    }

  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextGtime				*/
/************************************************************************/
static ST_RET LocToTextGtime (time_t *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text, size_t maxsize)
  {
  if (tstrTimeToString (*pSrc, text, maxsize))
    {
    MLOG_NERR0 ("Time to String conversion error");
    return (SD_FAILURE);
    }

  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextUtc					*/
/************************************************************************/
static ST_RET LocToTextUtc (MMS_UTC_TIME *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"UTC TIME seconds=%lu, fraction=%lu, qflags=%lu",
           (long) pSrc->secs, (long) pSrc->fraction, (long) pSrc->qflags);
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			LocToTextUtf8					*/
/************************************************************************/
static ST_RET LocToTextUtf8 (ST_UCHAR *pSrc, RUNTIME_TYPE *rt, ST_CHAR *text)
  {
  sprintf (text,"UTF8string: can't display");
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			ms_local_to_text				*/
/* NOTE: tmpBuf is passed to most "LocToText.." static functions. These	*/
/*	functions must NOT write past end of tmpBuf.			*/
/************************************************************************/

ST_CHAR *ms_local_to_text (ST_CHAR *datptr, SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
	ST_CHAR *textBuf, ST_UINT textBufSize)
  {
RUNTIME_TYPE *rt_ptr;
RUNTIME_TYPE *rt_end;
ST_RET uDataRet;
ST_INT arr_loop_level;
ST_INT arr_loops[ASN1_MAX_LEVEL];
ST_CHAR *ret_ptr;
ST_CHAR tmpBuf[TEMP_DATA_BUF_SIZE] = "";	/* init to empty string	*/
ST_UINT  nStrLen = 0;	/* Current text total length 	*/

  memset (arr_loops, 0, sizeof (arr_loops));	/* CRITICAL: start clean.*/

  nStrLen = 0;

  arr_loop_level = 0;

  rt_ptr = (RUNTIME_TYPE *) rt_head;  	/* point to head rt_block	*/
  rt_end = rt_ptr + rt_num; 	/* done when pointer is here		*/

  uDataRet = SD_SUCCESS;
  while (rt_ptr < rt_end && uDataRet == SD_SUCCESS)
    {
    if (rt_ptr->el_tag == RT_ARR_END)	/* treat case of array ending	*/
      {
      if (--arr_loops[arr_loop_level] > 0) /* if need to do next ar elmnt	*/
	rt_ptr -= rt_ptr->u.arr.num_rt_blks; /* mv rt_ptr to start of arr	*/
      else
        --arr_loop_level;
      }
    if (rt_ptr->el_tag == RT_ARR_START)	/* treat case of array starting	*/
      {
      /* initialize the loop counter for the array			*/
      ++arr_loop_level;
      arr_loops[arr_loop_level] = rt_ptr->u.arr.num_elmnts;
      }

    switch (rt_ptr->el_tag)
      {
      case RT_ARR_START :
      break;			/* do nothing	*/

      case RT_ARR_END :			/* array done			*/
      break;			/* do nothing	*/

      case RT_STR_START :
        uDataRet = AddString ("{", textBuf, textBufSize, &nStrLen);
      break;

      case RT_STR_END :			/* structure done		*/
        uDataRet = AddString ("}", textBuf, textBufSize, &nStrLen);
      break;

      case RT_BOOL :
        uDataRet = LocToTextBool ((ST_BOOLEAN *) datptr, rt_ptr, tmpBuf);
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;

      case RT_BIT_STRING :			
        uDataRet = LocToTextBs ((ST_UCHAR *) datptr, rt_ptr, tmpBuf);
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;

      case RT_INTEGER :			
        switch (rt_ptr->u.p.el_len)	/* determine length		*/
       	  {
          case 1 :			/* one byte int 		*/
            uDataRet = LocToTextInt8 ((ST_INT8 *) datptr, rt_ptr, tmpBuf);
     	  break;
     
     	  case 2 :			/* two byte int 		*/
            uDataRet = LocToTextInt16 ((ST_INT16 *) datptr, rt_ptr, tmpBuf);
     	  break;
     	  
     	  case 4 :			/* four byte integer		*/
            uDataRet = LocToTextInt32 ((ST_INT32 *) datptr, rt_ptr, tmpBuf);
     	  break;
     	  
#ifdef INT64_SUPPORT
     	  case 8 :			/* eight byte integer		*/
            uDataRet = LocToTextInt64 ((ST_INT64 *) datptr, rt_ptr, tmpBuf);
     	  break;
#endif
          default:
            uDataRet = SD_FAILURE;
          break;
          }
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;

      case RT_UNSIGNED : 			
        switch (rt_ptr->u.p.el_len)	/* determine length		*/
       	  {
          case 1 :			/* one byte int 		*/
            uDataRet = LocToTextUint8 ((ST_UCHAR *) datptr, rt_ptr, tmpBuf);
     	  break;
     
     	  case 2 :			/* two byte int 		*/
            uDataRet = LocToTextUint16 ((ST_UINT16 *) datptr, rt_ptr, tmpBuf);
     	  break;
     	  
     	  case 4 :			/* four byte integer		*/
            uDataRet = LocToTextUint32 ((ST_UINT32 *) datptr, rt_ptr, tmpBuf);
     	  break;
     	  
#ifdef INT64_SUPPORT
     	  case 8 :			/* eight byte integer		*/
            uDataRet = LocToTextUint64 ((ST_UINT64 *) datptr, rt_ptr, tmpBuf);
     	  break;
#endif 	/* INT64_SUPPORT	*/
          default:
            uDataRet = SD_FAILURE;
          break;
     	  }
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;

#ifdef FLOAT_DATA_SUPPORT
      case RT_FLOATING_POINT : 		
        if (rt_ptr->u.p.el_len != sizeof (ST_FLOAT))
          uDataRet = LocToTextDbl ((ST_DOUBLE *) datptr, rt_ptr, tmpBuf);
        else
          uDataRet = LocToTextFlt ((ST_FLOAT *) datptr, rt_ptr, tmpBuf);
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;
#endif

      case RT_OCTET_STRING : 		
        uDataRet = LocToTextOct ((ST_UCHAR *) datptr, rt_ptr, tmpBuf);
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;

      case RT_VISIBLE_STRING : 	/* No conversion needed. Just add to working text*/
        uDataRet = AddString (datptr, textBuf, textBufSize, &nStrLen);
      break;

#ifdef TIME_DATA_SUPPORT
      case RT_GENERAL_TIME : 		
        uDataRet = LocToTextGtime ((time_t *) datptr, rt_ptr, tmpBuf, sizeof(tmpBuf));
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;
#endif

#ifdef BTOD_DATA_SUPPORT
      case RT_BINARY_TIME : 		
        switch (rt_ptr->u.p.el_len)	/* determine length		*/
	  {
	  case 4:
            uDataRet = LocToTextBtime4 ((ST_INT32 *) datptr, rt_ptr, tmpBuf, sizeof(tmpBuf));
          break;
	  case 6:
            uDataRet = LocToTextBtime6 ((ST_INT32 *) datptr, rt_ptr, tmpBuf, sizeof(tmpBuf));
          break;
          default:
            uDataRet = SD_FAILURE;
          break;
	  }
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;
#endif

      case RT_BCD : 			
        if (rt_ptr->u.p.el_len <= 2)
          uDataRet = LocToTextBcd1 ((ST_INT8 *) datptr, rt_ptr, tmpBuf);
        else if (rt_ptr->u.p.el_len <= 4)
          uDataRet = LocToTextBcd2 ((ST_INT16 *) datptr, rt_ptr, tmpBuf);
        else if (rt_ptr->u.p.el_len <= 8)
          uDataRet = LocToTextBcd4 ((ST_INT32 *) datptr, rt_ptr, tmpBuf);
        else
          uDataRet = SD_FAILURE;
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;

      case RT_UTC_TIME :
        uDataRet = LocToTextUtc ((MMS_UTC_TIME *) datptr, rt_ptr, tmpBuf);
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;

      case RT_UTF8_STRING :
        uDataRet = LocToTextUtf8 ((ST_UCHAR *) datptr, rt_ptr, tmpBuf);
        if (uDataRet==SD_SUCCESS)	/* If converted OK, add to working text*/
          uDataRet = AddString (tmpBuf, textBuf, textBufSize, &nStrLen);
      break;

      default : 			/* should not be any other tag	*/
        MLOG_ERR1 ("Invalid tag: %d", (int) rt_ptr->el_tag);
	uDataRet = SD_FAILURE;
      break;
      }

    assert (strlen (tmpBuf) < sizeof (tmpBuf));	/* Must not exceed buffer*/

    datptr += rt_ptr->el_size; 	/* Adjust data pointer		*/
    rt_ptr++;  			/* point to next rt element	*/
    }

  assert (nStrLen <= textBufSize);	/* checked in AddString so this should never fail*/
  if (uDataRet)
    ret_ptr = NULL;
  else
    {
    textBuf [--nStrLen] = '\0';	/* replace last '\n' with '\0'	*/
    ret_ptr = textBuf;
    }
  return (ret_ptr);
  }

