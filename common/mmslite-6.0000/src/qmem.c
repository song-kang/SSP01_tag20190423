/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1993-2007, All Rights Reserved 				*/
/*									*/
/* MODULE NAME : qmem.c							*/
/* PRODUCT(S)  : Quick Memory Allocator					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/15/07  EJV     08    Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 03/11/04  GLB     07    Remove "thisFileName"                        */
/* 06/06/03  JRB     06    Use S_LOCK_RESOURCES (new util mutex).	*/
/* 04/14/03  JRB     05    Eliminate compiler warnings.			*/
/* 05/24/01  JRB     04    Chg chk_calloc to calloc.  This always compiled*/
/*			   with !DEBUG_SISCO, screws up link.		*/
/* 09/24/99  JRB     03    added:   #include "sysincs.h"                */
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 04/14/99  MDE     01    Removed unnecessary include files		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mem_chk.h"
#include "qmem.h"

#if defined(MSDOS)
#define HUGE _huge
#else
#define HUGE
#define halloc(a,b)	calloc(a,b)
#endif

ST_INT qMemElemSize;		/* should be ((power of 2)-2) 	*/
		    
static ST_INT initialized;
static ST_CHAR HUGE *qMemBuf;
static ST_CHAR *qMemUsed;
static ST_INT16 qMemIndex;
static ST_INT qNumElem;
static ST_INT qShift;

/************************************************************************/
/*			qMemInit					*/
/* Initialization function						*/
/************************************************************************/

ST_RET qMemInit (ST_INT NumElem)
  {
ST_INT size;
ST_RET ret;

  S_LOCK_UTIL_RESOURCES ();
  if (!initialized)
    {
    if (qMemElemSize == 0)
      qMemElemSize = 126;
    qMemBuf = (ST_CHAR HUGE *) halloc ((ST_INT32) NumElem, qMemElemSize + 2);
    qMemUsed = (ST_CHAR *) calloc (NumElem, 1);
    qMemIndex = 0;
    qNumElem = NumElem;

    qShift = -1;	    
    size = qMemElemSize + 2; /* 2 for the header! 			*/
    while (size)	/* determine the size of the buffer (power of 2)*/
      {
      size >>= 1;	/* size = (size/2)				*/
      qShift++;		/* Need to multiply buffer index by 2 later	*/
      }
    initialized = SD_TRUE;
    }

  if (!qMemBuf || !qMemUsed)
    ret = SD_FAILURE;
  else
    ret = SD_SUCCESS;

  S_UNLOCK_UTIL_RESOURCES ();
  return (ret);
  }

/************************************************************************/
/*			qMemAlloc					*/
/* Allocation function							*/
/************************************************************************/

ST_VOID *qMemAlloc (ST_INT size)
  {
ST_INT32 offset;
ST_CHAR HUGE *ptr;
register ST_INT i;

  S_LOCK_UTIL_RESOURCES ();
  if (!initialized)
    qMemInit (qNumElem == 0 ? 1024 : qNumElem);

  if (qMemIndex == -1 || size > qMemElemSize)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return (NULL);
    }

/* qMemIndex is the index into the array of buffers to the one that	*/
/* we will return this time						*/

  offset = ((ST_INT32) qMemIndex) << qShift;  /* multiply by buffer size	*/
  ptr = qMemBuf + offset;
  * (ST_INT16 *) ptr = qMemIndex;		/* save index into used array	*/

  qMemUsed[qMemIndex] = SD_TRUE;		/* mark used			*/

/* Now get ready for next time						*/
  qMemIndex++;
  if (qMemIndex == qNumElem)
    qMemIndex = 0;

  for (i = 0; i < qNumElem; i++)	/* find available buffer	*/
    {
    if (qMemUsed[qMemIndex] == SD_FALSE)
      break;
    else
      {
      qMemIndex++;
      if (qMemIndex == qNumElem)
        qMemIndex = 0;
      }
    }
  if (i == qNumElem)			/* no more buffers		*/
    qMemIndex = -1;			
  
  S_UNLOCK_UTIL_RESOURCES ();
  return ((ST_VOID *) (ptr + 2));
  }


/************************************************************************/
/*			qMemFree					*/
/* Free function							*/
/************************************************************************/

ST_VOID qMemFree (ST_VOID *vp)
  {
ST_INT16 index;
ST_CHAR *ptr;

/* recover the index into the 'used' flag array & set SD_FALSE		*/
  S_LOCK_UTIL_RESOURCES ();
  ptr = (ST_CHAR *) vp;  
  ptr -= 2;
  index = * (ST_INT16 *) ptr;
  qMemUsed[index] = SD_FALSE;

  qMemIndex = index;
  S_UNLOCK_UTIL_RESOURCES ();
  }

