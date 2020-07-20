/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*                 1999 - 2007, All Rights Reserved             	*/
/*									*/
/* MODULE NAME : smem.c							*/
/* PRODUCT(S)  : Utilities						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/03/12  JRB           Chg args to size_t like system functs.	*/
/* 01/15/07  EJV    06     Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 01/30/06  GLB    05     Integrated porting changes for VMS           */
/* 06/06/03  JRB    04     Use S_LOCK_RESOURCES (new util mutex).	*/
/* 09/25/01  JRB    03     Check smem_add_pool args.			*/
/* 08/24/01  JRB    02     Add clone support (see autoClone, maxClones).*/
/*			   DON'T add m_mem_overhead, mem_smem.c does it.*/
/*			   Add LOCK in smem_log_state.			*/
/*			   Pass old & new size to smem_realloc.		*/
/*			   smem_realloc NEVER realloc smaller.		*/
/*			   Add args to smem_add_pool.			*/
/*			   Chg args to u_smem_*.			*/
/*			   Call new u_smem_get_buf_detail.		*/
/* 12/09/99  MDE     01	   New						*/
/************************************************************************/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "smem.h"
#include "mem_chk.h"

/************************************************************************/
/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/

ST_UINT smem_debug_sel;

#ifdef DEBUG_SISCO
SD_CONST ST_CHAR *SD_CONST _smem_log_err_logstr = "SMEM_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _smem_log_flow_logstr = "SMEM_LOG_FLOW";
SD_CONST ST_CHAR *SD_CONST _smem_log_nerr_logstr = "SMEM_LOG_NERR";
SD_CONST ST_CHAR *SD_CONST _smem_log_dump_logstr = "SMEM_LOG_DUMP";
SD_CONST ST_CHAR *SD_CONST _smem_log_debug_logstr = "SMEM_LOG_DEBUG";
#endif

/************************************************************************/
/* Static variables.							*/
/************************************************************************/

/************************************************************************/
/* Static function prototypes.						*/
/************************************************************************/
static ST_INT findSmemContextIndex (ST_CHAR *ptr);
static SMEM_POOL_CTRL *clonePool (SMEM_CONTEXT *smemContext, SMEM_POOL_CTRL *sc);

/************************************************************************/

/************************************************************************/
/*			smem_add_pool 					*/
/************************************************************************/

SMEM_POOL_CTRL *smem_add_pool (SMEM_CONTEXT *smemContext, 
			       ST_UINT8 numBuf, size_t bufSize,
			       ST_BOOLEAN autoClone,
			       ST_INT maxClones,
			       ST_CHAR *poolName)
  {
SMEM_POOL_CTRL *newSc;
SMEM_POOL_CTRL *sc;
SMEM_POOL_CTRL **dest;
ST_INT i;

  if (numBuf == 0  ||  bufSize == 0)
    {
    SMEMLOG_ERR0 ("smem_add_pool: buffer size or number of buffers = 0. ILLEGAL.");
    return (NULL);	/* FAILURE	*/
    }

  /* Use calloc for SMEM_POOL_CTRL so struct starts out clean (especially next=NULL)	*/
  newSc = (SMEM_POOL_CTRL *) calloc (1, sizeof (SMEM_POOL_CTRL));
  newSc->availIndexStack = (ST_UINT8 *) malloc (sizeof (ST_UINT8) * numBuf);
  newSc->firstBuf = (ST_CHAR *) malloc (bufSize * numBuf);
  newSc->lastBuf = newSc->firstBuf + (bufSize * (numBuf-1));

  /* Fill in new SMEM_POOL_CTRL struct with data passed to this funct.	*/
  newSc->bufSize = bufSize;
  newSc->numBuf = numBuf;
  newSc->autoClone = autoClone;
  newSc->maxClones = maxClones;
  newSc->poolName = poolName;

  newSc->nextAvailBuf = 0;
#ifdef DEBUG_SISCO
  newSc->maxNumUsed = 0;
  newSc->usedSize = (size_t *) malloc (sizeof (size_t) * numBuf);
#endif
  for (i = 0; i < numBuf; ++i)
    newSc->availIndexStack[i] = i;
    
/* Now insert it into the list ... */
  if (smemContext->smemPoolCtrlList == NULL)
    smemContext->smemPoolCtrlList = newSc;
  else
    {
    dest = &smemContext->smemPoolCtrlList;
    sc = smemContext->smemPoolCtrlList;
    while (SD_TRUE)
      {
      if (sc->bufSize > bufSize) 
        {
	newSc->next = sc;
        *dest = newSc;
        break;
        }
      dest = &sc->next;
      sc = sc->next;
      if (sc == NULL)
        {
        *dest = newSc;
        newSc->next = NULL;
      	break;
        }
      }
    }  

  SMEMLOG_DEBUG3 ("SMEM new pool %08lx, num = %u, size %u",
      newSc, newSc->numBuf, newSc->bufSize);
  return (newSc);
  }

/************************************************************************/
/*			smem_malloc 					*/
/************************************************************************/

ST_VOID *smem_malloc (SMEM_CONTEXT *smemContext, size_t size)
  {
SMEM_POOL_CTRL *sc;
SMEM_POOL_CTRL *new_sc;
ST_CHAR *buf;
size_t bufSize;
ST_INT bufIndex;

  SMEMLOG_FLOW1 ("SMEM malloc size %u", size);

/* Find the first smemCtrl that is big enough for this request */
  sc = smemContext->smemPoolCtrlList;
  while (SD_TRUE)
    {
    if (sc == NULL)
      {
      SMEMLOG_NERR1 ("SMEM has no buffer pools large enough for size %d", size);
      sc = u_smem_need_buffers (smemContext, 0, size);
      if (sc == NULL)
        {
        SMEMLOG_ERR1 ("Error: no SMEM Pool Control elements for size %d", size);
        return (NULL);
        }
      }
    if (sc->bufSize >= size) 
      break;
    sc = sc->next;
    }

/* OK, found the first, now find one that has buffers available */
  bufSize = sc->bufSize;
  while (SD_TRUE)
    {
    if (sc->nextAvailBuf != sc->numBuf)	/* not all used */
      break;

    if (sc->next && sc->next->bufSize == bufSize)		/* same size */
      sc = sc->next;
    else
      {
      SMEMLOG_NERR1 ("SMEM needs more buffers of size %d", bufSize);
      if (sc->autoClone)
        new_sc = clonePool (smemContext, sc);
      else
        new_sc = u_smem_need_buffers (smemContext, sc->numBuf, sc->bufSize);
      if (new_sc)
        {
        sc = new_sc;
        break;
        }
      else
        {
        SMEMLOG_ERR1 ("User did not supply additional buffers of size %d", bufSize);
        return (NULL);
        }
      }
    }

  assert (sc->bufSize >= size);	/* Make sure right pool found (or created)*/

  /* NOTE: This msg assumes that smemContext is element of m_smem_ctxt array.*/
  SMEMLOG_DEBUG4 ("SMEM malloc: Context Num %d, pool %08lx, stack pos %d, index %d", 
                        smemContext-m_smem_ctxt,
			sc, 
			sc->nextAvailBuf, 
			sc->availIndexStack[sc->nextAvailBuf]);
  if (smemContext->contextName)
    {
    SMEMLOG_CDEBUG1 ("Context Name = %s", smemContext->contextName);
    }

  /* OK, this smemCtrl has buffers available ... */
  bufIndex = sc->availIndexStack[sc->nextAvailBuf++];
  buf = sc->firstBuf + (bufIndex * bufSize);

#ifdef DEBUG_SISCO
  if (sc->nextAvailBuf > sc->maxNumUsed)
    sc->maxNumUsed = sc->nextAvailBuf;

  sc->usedSize[sc->nextAvailBuf-1] = size;
#endif

  SMEMLOG_CFLOW1 ("ptr %08lx",buf);
  return ((ST_VOID *) buf);
  }
  
/************************************************************************/
/*			smem_calloc 					*/
/************************************************************************/

ST_VOID *smem_calloc (SMEM_CONTEXT *smemContext, size_t num, size_t size)
  {
ST_CHAR *buf;
size_t bufSize;

  bufSize = num * size;
  buf = (ST_CHAR *) smem_malloc (smemContext, bufSize);
  if (buf == NULL)
    return (NULL);

  memset (buf, 0, bufSize);
  return ((ST_VOID *) buf);
  }


/************************************************************************/
/*			smem_realloc 					*/
/************************************************************************/

ST_VOID *smem_realloc (SMEM_CONTEXT *smemContext, ST_VOID *oldptr,
                       size_t oldsize, size_t newsize)
  {
ST_VOID *buf;

  /* NEVER realloc smaller. With SMEM, this would require twice as many	*/
  /* buffers in this context.						*/
  if (newsize > oldsize)
    {
    buf = smem_malloc (smemContext, newsize);
    memcpy (buf, oldptr, min(oldsize,newsize)); 
    smem_free (smemContext, oldptr);
    }
  else
    buf = oldptr;
  return (buf);
  }


/************************************************************************/
/*			smem_free 					*/
/************************************************************************/


ST_VOID smem_free (SMEM_CONTEXT *smemContext, ST_VOID *ptr)
  {
SMEM_POOL_CTRL *sc;
ST_INT bufIndex;
ST_INT ContextIndex;	/* Index into array of SMEM contexts	*/

/* Find the smemCtrl that this buffer belongs to */
  sc = smemContext->smemPoolCtrlList;
  while (SD_TRUE)
    {
    if (sc == NULL)
      {
      SMEMLOG_ERR1 ("SMEM free could not find SMEM Pool Control for ptr %08lx",
			ptr);
      if ((ContextIndex = findSmemContextIndex ((ST_CHAR *) ptr)) >= 0)
        {
        SMEMLOG_CERR2 ("ptr %08lx found in SMEM Context number %d", ptr, ContextIndex);
        }
      else
        {
        SMEMLOG_CERR1 ("ptr %08lx not found in other SMEM Contexts either", ptr);
        }

      return;
      }
    if ((ST_CHAR *) ptr >= sc->firstBuf && (ST_CHAR *) ptr <= sc->lastBuf)
      break;

    sc = sc->next;
    }

#ifdef DEBUG_SISCO
  if (sc->nextAvailBuf <= 0)
    {
    SMEMLOG_ERR0 ("Error: sc->nextAvailBuf <= 0");
    return;
    }
#endif

  bufIndex = (ST_INT)(((ST_CHAR *) ptr - sc->firstBuf)/sc->bufSize);


  sc->availIndexStack[--sc->nextAvailBuf] = bufIndex;
  /* NOTE: This msg assumes that smemContext is element of m_smem_ctxt array.*/
  SMEMLOG_DEBUG3 ("SMEM free: Context Num %d, pool %08lx, ptr %08lx",
                  smemContext-m_smem_ctxt, sc, ptr);
  if (smemContext->contextName)
    {
    SMEMLOG_CDEBUG1 ("Context Name = %s", smemContext->contextName);
    }
  SMEMLOG_CDEBUG2 ("stack pos %d, index %d", sc->nextAvailBuf, bufIndex);
  }

/************************************************************************/
/************************************************************************/

ST_VOID smem_range_check (SMEM_CONTEXT *smemContext, 
			  size_t size, ST_BOOLEAN add)
  {
#ifdef DEBUG_SISCO
ST_INT i;
SMEM_RANGE_TRACK *rt;
SMEM_RT_CTRL *smemRangeTrack;

  smemRangeTrack = smemContext->smemRangeTrack;
  if (smemRangeTrack != NULL)
    {
    for (i = 0; i < smemRangeTrack->numRanges; ++i)
      {
      rt = &smemRangeTrack->rt[i];
      if (size >= rt->lowLimit && size <= rt->highLimit)
        {
        if (add == SD_TRUE)
          {
          ++rt->currNum;
          if (rt->currNum > rt->maxNum)
            rt->maxNum = rt->currNum;
          }
        else
          --rt->currNum;
        if (rt->currNum < 0)
          {
          SMEMLOG_ERR0 ("smem_range_check: count of buffers is negative. Buffer probably freed using wrong context.");
          }
        }
      }
    }
#endif
  }


/************************************************************************/
/*			smem_log_state 					*/
/************************************************************************/

ST_VOID smem_log_state (SMEM_CONTEXT *smemContext)
  {
#ifdef DEBUG_SISCO
SMEM_POOL_CTRL *sc;
SMEM_RT_CTRL *smemRangeTrack;
ST_INT i, j;
ST_LONG sumNumBuf;
size_t sumMemory;
ST_LONG sumMaxNumBuf;
size_t sumMaxMemory;
SMEM_RANGE_TRACK *rt;
size_t realBufSize;
ST_CHAR *bufDetail;	/* additional buffer information from upper layer*/

  S_LOCK_UTIL_RESOURCES ();	/* don't want any allocs while doing this*/


  /* NOTE: This msg assumes that smemContext is element of m_smem_ctxt array.*/
  SMEMLOG_DUMP1 ("SMEM Current State for Context Index: %d",
                 smemContext - m_smem_ctxt);
  if (smemContext->contextName)
    {
    SMEMLOG_CDUMP1 ("Context Name:  %s", smemContext->contextName);
    }

  sumNumBuf = 0;
  sumMemory = 0;
  sumMaxNumBuf = 0;
  sumMaxMemory = 0;
  sc = smemContext->smemPoolCtrlList;
  i = 0;
  while (sc)
    {
    realBufSize =  sc->bufSize;
    SMEMLOG_CDUMP0 ("");
    SMEMLOG_CDUMP2 ("Pool %d : %08lx", i, sc);
    SMEMLOG_CDUMP1 ("  Buffer Size:         %d", realBufSize);
    SMEMLOG_CDUMP1 ("  Number Buffers:      %d", sc->numBuf);
    SMEMLOG_CDUMP1 ("  Current number used: %d", sc->nextAvailBuf);
    SMEMLOG_CDUMP1 ("  Current memory used: %d", sc->nextAvailBuf * realBufSize);
    SMEMLOG_CDUMP1 ("  Max number used:     %d", sc->maxNumUsed);
    SMEMLOG_CDUMP1 ("  Max memory used:     %d", sc->maxNumUsed * realBufSize);
    sumNumBuf += sc->nextAvailBuf;
    sumMemory += sc->nextAvailBuf * realBufSize;
    sumMaxNumBuf += sc->maxNumUsed;
    sumMaxMemory += sc->maxNumUsed * realBufSize;

    SMEMLOG_CDUMP1 ("  nextAvailBuf:        %d", sc->nextAvailBuf);
    SMEMLOG_CDUMP1 ("  firstBuf:            %08lx", sc->firstBuf);
    SMEMLOG_CDUMP1 ("  lastBuf:             %08lx", sc->lastBuf);

    if (sc->maxNumUsed > 0)
      {
      SMEMLOG_CDUMP0 ("  Buffers used");
      /* Only log the indices that were used.	*/
      for (j = 0; j < sc->maxNumUsed; ++j)
        {
	/* Get additional buffer info from upper layer.	*/
        bufDetail = u_smem_get_buf_detail (sc->firstBuf + (j * sc->bufSize),
                         sc->usedSize[j]);
        SMEMLOG_CDUMP3 ("    % 3d) %d, %s", j, sc->availIndexStack[j],
                                bufDetail);
        }
      }

    sc = sc->next;
    ++i;
    }


  SMEMLOG_CDUMP0 ("");
  SMEMLOG_CDUMP1 ("Sum current number used  %d", sumNumBuf);
  SMEMLOG_CDUMP1 ("Sum current memory used  %d", sumMemory);
  SMEMLOG_CDUMP1 ("Sum max number used  %d", sumMaxNumBuf);
  SMEMLOG_CDUMP1 ("Sum max memory used  %d", sumMaxMemory);


  smemRangeTrack = smemContext->smemRangeTrack;
  if (smemRangeTrack != NULL)
    {
    SMEMLOG_CDUMP0 ("");
    SMEMLOG_CDUMP0 ("SMEM Range Tracking");
    for (i = 0; i < smemRangeTrack->numRanges; ++i)
      {
      rt = &smemRangeTrack->rt[i];
      SMEMLOG_CDUMP3 ("  Range: % 5u - % 5u : High Water: %ld",
      				rt->lowLimit, rt->highLimit, rt->maxNum);
      }
    }
  S_UNLOCK_UTIL_RESOURCES ();

#endif
  }

/************************************************************************/
/*			findSmemContextIndex				*/
/************************************************************************/

static ST_INT findSmemContextIndex (ST_CHAR *ptr)
  {
SMEM_POOL_CTRL *sc;
ST_INT bufIndex;
ST_INT ContextIndex;	/* Index into array of SMEM contexts	*/
SMEM_CONTEXT *smemContext;

  for (ContextIndex = 0; ContextIndex < M_SMEM_MAX_CONTEXT; ContextIndex++)
    {
    smemContext = &m_smem_ctxt [ContextIndex];

    /* Find the smem Pool that this buffer belongs to */
    sc = smemContext->smemPoolCtrlList;
    while (sc)
      {
      if (ptr >= sc->firstBuf && ptr <= sc->lastBuf)
        break;

      sc = sc->next;
      }
    if (sc != NULL)	/* it was found	*/
      {
      /* ptr is in right range, but let's make sure it is on correct boundary*/
      if ((ptr - sc->firstBuf) % sc->bufSize)
        {
        SMEMLOG_DEBUG2 ("findSmemContextIndex: found context but ptr not on even boundary: index = %d, pool bufsize = %d",
                       ContextIndex, sc->bufSize);
        return (-1);
        }
      else
        {
        bufIndex = (ST_INT)((ptr - sc->firstBuf) / sc->bufSize);
        SMEMLOG_DEBUG3 ("findSmemContextIndex: found context: index = %d, pool bufsize = %d, pool index = %d", 
                       ContextIndex, sc->bufSize, bufIndex);
        }
      return (ContextIndex);
      }
    }
  return (-1);	/* not found in ANY context	*/
  }


/************************************************************************/
/*			clonePool					*/
/* NOTE: each clone comes from last clone, so cloneCount keeps going up.*/
/************************************************************************/
static SMEM_POOL_CTRL *clonePool (SMEM_CONTEXT *smemContext, SMEM_POOL_CTRL *oldPool)
  {
SMEM_POOL_CTRL *newPool;
  /* maxClones = 0 means NO limit.					*/
  /* Each clone contains a clone number incremented with each clone.	*/
  if (oldPool->maxClones == 0  ||			/* No limit	*/
      oldPool->cloneCount < oldPool->maxClones)		/* Within limit	*/
    {
    newPool = smem_add_pool (smemContext,oldPool->numBuf,oldPool->bufSize,
                             oldPool->autoClone, oldPool->maxClones, NULL); 
    if (newPool != NULL)
      {
      /* Make clone same as original except cloneCount.			*/
      /* DO NOT use memcpy because don't want tracking info copied.	*/
      newPool->cloneCount = oldPool->cloneCount + 1;
      }
    }
  else
    newPool = NULL;	/* exceeded max number of clones	*/
  return (newPool);
  }

    
    

