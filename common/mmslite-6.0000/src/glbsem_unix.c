/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2010, All Rights Reserved					*/
/*									*/
/* MODULE NAME : glbsem_unix.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION : Multi-thread support for UNIX-like systems ONLY.*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/07/12  JRB	   Simplify logging using S_FMT_THREAD_IDX.	*/
/* 12/21/10  EJV           Added _ex versions with file/line parameters	*/
/* 01/22/10  JRB     12    Fix logging of thread Id and Handle (don't	*/
/*			   know type for format so use union).		*/
/* 01/29/08  EJV     11    Use S_FMT_* macros to log pointers & handles.*/
/* 12/12/05  JRB     10    Disable gs_wait_mult_event_sem with #if 0 to	*/
/*			   catch problems at compile time, not run time.*/
/* 06/06/05  EJV     09    Added S_MT_SUPPORT to avoid Linux errors.	*/
/* 11/04/03  JRB     08    Del gs_sleep (use sMsSleep).			*/
/* 10/09/03  JRB     07    Use thisThreadId only if DEBUG_SISCO.	*/
/*			   Use clock_gettime for default system.	*/
/*			   gs_start_thread: don't init threadHandle=NULL*/
/* 08/25/03  JRB     06    gs_sleep: use rqtp.tv_sec if ms >= 1000.	*/
/* 06/20/03  EJV     05    Renamed gs_mutex_get to gs_mutex_get_tm.	*/
/*			   Del param from gs_mutex_get calls.		*/
/* 06/12/03  EJV     04    Redesigned mutex sems implementation.	*/
/* 06/11/03  EJV     03    Added empty functions: gs_named_mutex_xxx	*/
/*			     and gs_get_named_event_sem			*/
/*			   Added thisFileName, del unmatched #if	*/
/* 06/06/03  JRB     02    Del logging in gs_mutex_get, gs_mutex_free.	*/
/* 06/06/03  JRB     01    NEW. Code taken out of glbsem.c		*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "slog.h"
#include "glbsem.h"

#if defined(S_MT_SUPPORT)

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_RET _gs_sig_event_sem (ST_EVENT_SEM es, ST_INT predicate);

/*======================================================================*/
/* UNIX FUNCTIONS complying with POSIX 1003.1c				*/
/*======================================================================*/


	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/
	/*							*/
	/*	MUTEX SEMAPHORE FUNCTIONS for			*/
	/* 	_AIX, __alpha					*/
	/*							*/
	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/


/************************************************************************/
/*                         gs_mutex_create					*/
/*----------------------------------------------------------------------*/
/* Creates and initializes MUTEX seamphore.				*/
/* Parameters:								*/
/*	ms		pointer to mutex sem struct			*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/************************************************************************/
ST_RET gs_mutex_create_ex (ST_MUTEX_SEM *ms, ST_CHAR *srcFile, ST_INT srcLine)
  {
ST_RET               ret;
ST_INT               result;
pthread_mutexattr_t  mutexattr;
#if defined(DEBUG_SISCO)
ST_THREAD_ID threadId = GET_THREAD_ID();
#endif

  ret = SD_FAILURE;

  /* Initialize the mutex attributes variable (needed only for mutex init) */

  result = pthread_mutexattr_init (&mutexattr);
  if ( result != 0 )
    {
    GLBSEM_LOG_ERR3 ("GLBSEM gs_mutex_create error: Thread " S_FMT_THREAD_IDX " failed to initialize"
                     " mutex semaphore ms=" S_FMT_PTR " attributes (error=%d).",
                     threadId, ms, result);
    return (ret);
    }

  /* Set the mutex attributes (mutex type to PTHREAD_MUTEX_RECURSIVE).	*/

  /* The semaphore type could not be PTHREAD_MUTEX_ERRORCHECK because	*/
  /* in the DEC UNIX manual there is a warning that "DECthreads does not*/
  /* currently detect deadlock conditions involving more then one mutex,*/
  /* but may in the future." Because of this restriction and the fact	*/
  /* that ICCP Toolkit is implementing an independent mutex semaphore	*/
  /* (currently PTHREAD_MUTEX_NORMAL may be changed to ERRORCHECK) or 	*/
  /* possibility that user application may want to use the ERRORCHECK	*/
  /* semaphore we decided to use the PTHREAD_MUTEX_RECURSIVE.		*/

  /* If the semaphore type is PTHREAD_MUTEX_ERRORCHECK then it will	*/
  /* return an error if the current owner of the semaphore tries	*/
  /* to lock the mutex again (or thread tries to unlock mutex that it	*/
  /* does not own).							*/

  result = pthread_mutexattr_settype (&mutexattr, PTHREAD_MUTEX_RECURSIVE);
  if ( result != 0 )
    {
    GLBSEM_LOG_ERR3 ("GLBSEM gs_mutex_create error: thread " S_FMT_THREAD_IDX " failed to set mutex"
                     " semaphore ms=" S_FMT_PTR " attributes (error=%d).", 
                     threadId, ms, result);
    switch ( result )
      {
      case EINVAL:
        GLBSEM_LOG_CERR0 ("error=EINVAL (invalid mutex attribute or type).");
      break;
      case ESRCH:
        GLBSEM_LOG_CERR0 ("error=ESRCH (non existing mutex attributes object).");
      break;
      }

    pthread_mutexattr_destroy (&mutexattr);
    return (ret);
    }

  /* Initialize the mutex with specific attributes. The mutex will be	*/
  /* in the unlocked state.						*/

  result = pthread_mutex_init (ms, &mutexattr);
  if ( result != 0 )
    {
    GLBSEM_LOG_ERR3 ("GLBSEM gs_mutex_create error: thread " S_FMT_THREAD_IDX " failed to initialize"
                     " mutex semaphore ms=" S_FMT_PTR " (error=%d).",
                     threadId, ms, result);
    switch ( result )
      {
      case EAGAIN:
        GLBSEM_LOG_CERR0 ("error=EAGAIN (system resouces not available).");
      break;
      case ENOMEM:
        GLBSEM_LOG_CERR0 ("error=ENOMEM (insufficient memory for mutex"
                             " initialization).");
      break;
      case EBUSY:
        GLBSEM_LOG_CERR0 ("error=EBUSY (programs requested mutex"
                             " reinitialization).");
      break;
      case EINVAL:
        GLBSEM_LOG_CERR0 ("error=EINVAL (invalid mutex argument).");
      break;
      case EPERM:
        GLBSEM_LOG_CERR0 ("error=EPERM (insufficient privileges).");
      break;
      }

    pthread_mutexattr_destroy (&mutexattr);
    return (ret);
    }

  /* release the mutex attributes object - not needed anymore */
  pthread_mutexattr_destroy (&mutexattr);

  GLBSEM_LOG_FLOW2 ("GLBSEM gs_mutex_create: thread " S_FMT_THREAD_IDX " initialized "
                    " mutex semaphore ms=" S_FMT_PTR ".", threadId, ms);

  /* initialized the mutex successfully */
  ret = SD_SUCCESS;

  return (ret);
  }

/************************************************************************/
/*                       gs_named_mutex_create				*/
/*----------------------------------------------------------------------*/
/* Create/Open NAMED mutex.						*/
/* For each call to this function the gs_named_mutex_destroy must be	*/
/* called.								*/
/* Parameters:								*/
/*	ms		pointer to mutex object				*/
/*	name	       	pointer to mutex name.				*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/************************************************************************/

#if 0	/* DEBUG: To be implemented later	*/
ST_RET gs_named_mutex_create_ex (ST_MUTEX_SEM *ms, ST_CHAR *name, ST_CHAR *srcFile, ST_INT srcLine)
  {
ST_RET ret = SD_FAILURE;

  /* To be implemented */
  return (ret);
  }
#endif	/* DEBUG: To be implemented later	*/

/************************************************************************/
/*			gs_mutex_get_tm	        			*/
/*----------------------------------------------------------------------*/
/* Lock the MUTEX semaphore ms.						*/
/* Parameters:								*/
/*	ms		pointer to mutex sem struct			*/
/*	timeout		this parameter is ignored in UNIX implementation*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/* CRITICAL: gs_mutex_get_tm must NOT call any slog, stime, or mem_chk	*/
/*	functions. These functions call ge_mutex_get which would cause	*/
/*	an infinite loop.						*/
/************************************************************************/

ST_RET gs_mutex_get_tm_ex (ST_MUTEX_SEM *ms, ST_LONG timeout, ST_CHAR *srcFile, ST_INT srcLine)
  {
ST_INT result;

  if (!gs_already_inited)	/* Make sure gs is initialized.		*/
    gs_init ();

  /* Because the mutex semaphore type is PTHREAD_MUTEX_RECURSIVE it	*/
  /* will allow to relock the mutex by the thread that owns the mutex	*/
  /* without blocking. The lock count is incremented for each recursive	*/
  /* lock within the thread.						*/
  /* If the mutex is locked by another thread, the calling thread will	*/
  /* wait until the mutex become available.				*/

  result = pthread_mutex_lock (ms);
  assert (result==0);	/* otherwise threads will overwrite each other's data	*/

  if (result == 0)
    return (SD_SUCCESS);
  else
    return (SD_FAILURE);
  }

/************************************************************************/
/*			gs_mutex_free		        		*/
/*----------------------------------------------------------------------*/
/* Unlock the MUTEX semaphore ms.					*/
/* Parameters:								*/
/*	ms		pointer to mutex sem struct			*/
/* Return values:  							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/* CRITICAL: gs_mutex_free must NOT call any slog, stime, or mem_chk	*/
/*	functions. These functions call ge_mutex_free which would cause	*/
/*	an infinite loop.						*/
/************************************************************************/
ST_RET gs_mutex_free_ex (ST_MUTEX_SEM *ms, ST_CHAR *srcFile, ST_INT srcLine)
  {
ST_INT       result;

  /* Because the mutex semaphore type is PTHREAD_MUTEX_RECURSIVE 	*/
  /* if the calling thread owns the mutex the lock count is decremented.*/
  /* the mutex remains owned by the calling thread until the count is 0.*/
  /* When the count becomes 0 the mutex is unlocked and can be grabed by*/
  /* next waiting thread.						*/

  result = pthread_mutex_unlock (ms);
  assert (result==0);	/* otherwise threads will overwrite each other's data	*/

  if (result == 0)
    return (SD_SUCCESS);
  else
    return (SD_FAILURE);
  }

/************************************************************************/
/*			gs_mutex_destroy				*/
/*----------------------------------------------------------------------*/
/* Free all resources allocated for the MUTEX semaphore ms.		*/
/* Parameters:								*/
/*	ms		pointer to mutex sem struct			*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/************************************************************************/
ST_RET gs_mutex_destroy (ST_MUTEX_SEM *ms)
  {
ST_RET         ret;
ST_INT         result;
#if defined(DEBUG_SISCO)
ST_THREAD_ID threadId = GET_THREAD_ID();
#endif

  ret = SD_FAILURE;

  /* The pthread_mutex_destroy function deinitializes the ms mutex	*/
  /* semaphore. THe function is successful only if the  mutex is not	*/
  /* referenced or locked.						*/

  result = pthread_mutex_destroy (ms);
  if ( result != 0 )
    {
    /* Log a major error here */
    GLBSEM_LOG_ERR3 ("GLBSEM gs_mutex_destroy error: thread " S_FMT_THREAD_IDX
                     " failed to destroy mutex semaphore ms=" S_FMT_PTR " (error=%d)",
                     threadId, ms, result);
    switch ( result )
      {
      case EBUSY:
        GLBSEM_LOG_CERR0 ("error=EBUSY (semaphore locked or referenced)");
      break;
      case EINVAL:
        GLBSEM_LOG_CERR0 ("error=EINVAL (invalid mutex semaphore)");
      break;
      }

    return (ret);
    }

  GLBSEM_LOG_FLOW2 ("GLBSEM gs_mutex_destroy: thread " S_FMT_THREAD_IDX " destroyed mutex"
                    " semaphore ms=" S_FMT_PTR ".", threadId, ms);

  /* destroy mutex successful */
  ret = SD_SUCCESS;

  return (ret);
  }

	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/
	/*							*/
	/*	USER's EVENT SEMAPHORE FUNCTIONS for		*/
	/*	_AIX, __alpha					*/
	/*							*/
	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/

/************************************************************************/
/*                       gs_get_event_sem				*/
/*----------------------------------------------------------------------*/
/* Creates and initializes an event semaphore.				*/
/* The event semaphore is implemented using PTHREADs Condition Variable.*/
/* Parameters:								*/
/*  ST_BOOLEAN		manualReset	     Type of event sem		*/
/* Return values:							*/
/*  ST_EVENT_SEM	handle (pointer) to event semaphore		*/
/*  NULL		if function failed				*/
/************************************************************************/

ST_EVENT_SEM gs_get_event_sem_ex (ST_BOOLEAN manualReset, ST_CHAR *srcFile, ST_INT srcLine)
  {
ST_INT        result;
ST_EVENT_SEM  es = NULL;		/* pointer to event sem struct	*/
#if defined(DEBUG_SISCO)
ST_THREAD_ID threadId = GET_THREAD_ID();
#endif

  /* allocate the memory for the event sem			*/

  es = (ST_EVENT_SEM) chk_calloc (1, sizeof (GS_EVENT_SEM));
  if ( es == NULL )
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_get_event_sem error: thread " S_FMT_THREAD_IDX
                     " (allocate memory failed).", threadId);
    return (NULL);
    }

  /* crete mutex for the condition variable */
  if ( gs_mutex_create (&es->mutex) != SD_SUCCESS )
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_get_event_sem error: thread " S_FMT_THREAD_IDX
                     " (gs_mutex_create failed).", threadId);

    chk_free (es);
    return (NULL);
    }

  /* Condition Variable attributes are currently not implemented, use NULL */
  result = pthread_cond_init (&es->cond, NULL);
  if ( result != 0 )
    {
    GLBSEM_LOG_ERR2 ("GLBSEM gs_get_event_sem error: thread " S_FMT_THREAD_IDX
                     " (pthread_cond_init failed result=%d),", threadId, result);
    switch ( result )
      {
      case EAGAIN:
        GLBSEM_LOG_CERR0 ("result=EAGAIN (system resouces not available).");
      break;
      case ENOMEM:
        GLBSEM_LOG_CERR0 ("result=ENOMEM (insufficient memory for mutex"
                          " initialization).");
      break;
      case EBUSY:
        GLBSEM_LOG_CERR0 ("result=EBUSY (programs requested condition variable"
                          " reinitialization).");
      break;
      case EINVAL:
        GLBSEM_LOG_CERR0 ("result=EINVAL (invalid attr argument).");
      break;
      }
    gs_mutex_destroy (&es->mutex);
    chk_free (es);
    return (NULL);
    }

  GLBSEM_LOG_FLOW2 ("GLBSEM gs_get_event_sem: thread " S_FMT_THREAD_IDX " initialized event sem es=" S_FMT_PTR ".",
                    threadId, es);

  /* successfully initialized the event semaphore */
  es->manualReset = manualReset;
  return (es);
  }

/************************************************************************/
/*                       gs_get_named_event_sem				*/
/*----------------------------------------------------------------------*/
/* Create a "manual-reset" or "auto-reset" NAMED event semaphore.	*/
/* Parameters:								*/
/*	name			pointer to semaphore name.		*/
/*	manualReset		SD_TRUE or SD_FALSE			*/
/* Return values:							*/
/*	ST_EVENT_SEM		handle to named event semaphore object	*/
/************************************************************************/

#if 0	/* DEBUG: To be implemented later	*/
ST_EVENT_SEM gs_get_named_event_sem_ex (ST_CHAR *name, ST_BOOLEAN manualReset,
					ST_CHAR *srcFile, ST_INT srcLine)
  {
ST_EVENT_SEM retEventSem = NULL;

  /* To be implemented */
  return (retEventSem);
  }
#endif	/* DEBUG: To be implemented later	*/

/************************************************************************/
/*                       gs_wait_event_sem				*/
/*----------------------------------------------------------------------*/
/* Wait for event semaphore es until it becomes signaled or timeout	*/
/* occurrs.								*/
/* If compiled with DEBUG_SISCO the gs_track variable need to be set to	*/
/* value >0 to enable the timing code. This way logging can be used	*/
/* by an application without degradation of the EVENT sem performance.	*/
/* Parameters:								*/
/*	es		event semaphore object (pointer)		*/
/*	timeout		interval in milliseconds to wait for the es to	*/
/*			be signaled, if -1L then the function will be	*/
/*			blocked indefinitely until the event semaphore	*/
/*			is signaled.					*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/*	SD_TIMEOUT	timeout occurred				*/
/************************************************************************/

ST_RET gs_wait_event_sem_ex (ST_EVENT_SEM es, ST_LONG timeout, ST_CHAR *srcFile, ST_LONG srcLine)
  {
ST_RET     ret;
ST_INT     result;
struct timespec tsDelta;
struct timespec tsAbstime;
#if defined(DEBUG_SISCO)
ST_DOUBLE  startTime;
ST_DOUBLE  endTime;
ST_DOUBLE  elapsedTime;
#endif
#if defined(linux)
struct timeval curtimeval;	/* for gettimeofday	*/
#endif
#if defined(DEBUG_SISCO)
ST_THREAD_ID threadId = GET_THREAD_ID();
#endif

  ret = SD_FAILURE;

  if (!es)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_wait_event_sem error: thread " S_FMT_THREAD_IDX " (invalid es=NULL)",
                     threadId);
    return (ret);
    }

  /* mutex for the specified condition variable must be locked before	*/
  /* going into wait or checking preedicate				*/
  gs_mutex_get (&es->mutex);

  /* Note: according to the docummentation we may not capture the	*/
  /*       signaling of conditional variable if a thread was not waiting*/
  /*       for it, since the signaling of the condition is not held	*/
  /*       (AIX and maybe other UNIX systems).				*/
  /*       Let check the predicate for the conditional variable first to*/
  /*       see if it was set and act accordingly.			*/

  /* To simulate Windows manual-reset semaphore if gs_signal_event_sem	*/
  /* was used then the semaphore remains signaled until it is exlicitly	*/
  /* reset to non-signaled state by calling gs_reset_event_sem.		*/
  /* If it is auto-reset semaphore and ge_signal_event_sem was used but	*/
  /* no thread was waiting then the sem remains signaled until a thread	*/
  /* is released.							*/

  if (es->predicate == 1)
    {
    /* conditional variable was signaled before we entered the wait	*/
    if (es->manualReset == SD_FALSE)
      es->predicate = 0;	/* simulate release of a waiting thread */
    gs_mutex_free (&es->mutex);
    GLBSEM_LOG_FLOW2 ("GLBSEM gs_wait_event_sem: thread " S_FMT_THREAD_IDX 
                      " returned from wait for es=" S_FMT_PTR " (on entry p=1)",
                      threadId, es);
    return (SD_SUCCESS);
    }
  /* If gs_pulse_event_sem was used the state of the semaphore should	*/
  /* be always in non-signaled state when this function is entered.	*/

#if defined(DEBUG_SISCO)
  if (gs_track)
    startTime = sGetMsTime ();
#endif

  /* set the absolute wait time */
  if ( timeout >= 0 )
    {
    tsDelta.tv_sec =   timeout / 1000L;
    tsDelta.tv_nsec = (timeout % 1000L) * 1000000L;  /* nanoseconds */

#if defined(_AIX) || (defined(__alpha) && !defined(__VMS))
    /* obtain abosolute time for wakeup */
    result = pthread_get_expiration_np (&tsDelta, &tsAbstime);
    if ( result != 0 )
      {
      GLBSEM_LOG_ERR3 ("GLBSEM gs_wait_event_sem error: thread " S_FMT_THREAD_IDX ", es=" S_FMT_PTR
                       " (pthread_get_expiration_np failed result=%d)", 
                       threadId, es, result);
      gs_mutex_free (&es->mutex);
      return (ret);
      }
#else	/* all other systems	*/
    /* Get current time & add timeout to it.	*/
#if defined(linux)
    /* LINUX linker can't find "clock_gettime" so use "gettimeofday".*/
    gettimeofday (&curtimeval,NULL);
    tsAbstime.tv_sec  = curtimeval.tv_sec;		/* convert timeval to timespec	*/
    tsAbstime.tv_nsec = curtimeval.tv_usec*1000;	
#else
    clock_gettime (CLOCK_REALTIME, &tsAbstime); /* gives timespec, which we want*/
#endif
    tsAbstime.tv_sec  += tsDelta.tv_sec;
    tsAbstime.tv_nsec += tsDelta.tv_nsec;	
    /* adjust if (nsec) > 1000000000	*/
    if (tsAbstime.tv_nsec / 1000000000 > 0)
      {		/* in this case, should be EXACTLY = 1	*/
      tsAbstime.tv_sec++;	/* add 1 second	*/
      tsAbstime.tv_nsec = tsAbstime.tv_nsec % 1000000000;
      }
#endif	/* all other systems	*/
    }

  /* wait until condition signaled or timeout				*/
  es->predicate = 0;
  while ( !es->predicate )
    {
    if ( timeout < 0 )
      /* wait indefinitely	*/
      result = pthread_cond_wait (&es->cond, &es->mutex);
    else
      /* wait for a period of time, if timeout=0 then fun returns immediatelly */
      result = pthread_cond_timedwait (&es->cond, &es->mutex, &tsAbstime);

    if ( result == 0 )
      {
      if (es->predicate)
        {
        /* if gs_signal_event_sem used then state of predicate will	*/
        /* remain set until exlicitly reset by gs_reset_event_sem.	*/
        if (es->manualReset == SD_TRUE && es->predicate == 1)
          {
          /* do not reset the predicate */
          }
        else
          es->predicate = 0;
        ret = SD_SUCCESS;  		/* cond var was signaled	*/
        GLBSEM_LOG_FLOW2 ("GLBSEM gs_wait_event_sem: thread " S_FMT_THREAD_IDX
                          " returning from wait for es=" S_FMT_PTR " (success)",
                          threadId, es);
        break;				/* exit while 			*/
        }
      else
        {
        if (es->manualReset == SD_TRUE)
          {
          /* On Windows manual-reset semaphore calling PulseEvent allows*/
          /* to wake up all waiting threads. We will simulate this	*/
          /* behavior here by ignoring the predicate=0 because other	*/
          /* thread probably did reset it already.			*/
          ret = SD_SUCCESS;  		/* cond var was signaled	*/
          GLBSEM_LOG_FLOW2 ("GLBSEM gs_wait_event_sem: thread " S_FMT_THREAD_IDX 
                            " returning from wait for es=" S_FMT_PTR ".",
                            threadId, es);
          break;				/* exit while 		*/
          }
        else
          {
          /* On Windows the auto-reset semaphore will allow to wake	*/
          /* only one thread, so we have to go back to waiting state	*/
          /* for this thread.						*/
          
          GLBSEM_LOG_FLOW2 ("GLBSEM gs_wait_event_sem: thread " S_FMT_THREAD_IDX
                            " spurious wake up for es=" S_FMT_PTR ".",
                            threadId, es);
          continue;
          }
        }
      }
    else if (result == ETIMEDOUT)
      {
      /* it is possible that the predicate was set shortly before timeout */
      if (es->predicate)
        {
        ret = SD_SUCCESS;		/* cond variable signaled	*/
        GLBSEM_LOG_FLOW2 ("GLBSEM gs_wait_event_sem: thread " S_FMT_THREAD_IDX
                          " returned from wait for es=" S_FMT_PTR " (timeout but p=1).",
                          threadId, es);
        }
      else
        {
        ret = SD_TIMEOUT; 		/* timeout			*/
        GLBSEM_LOG_FLOW3 ("GLBSEM gs_wait_event_sem: thread " S_FMT_THREAD_IDX ", es=" S_FMT_PTR
                          " (timeout of %ld ms)", threadId, es, timeout);
        }
      es->predicate = 0;		/* clear for next time */
      break;				/* exit while 			*/
      }
    else
      {
      GLBSEM_LOG_ERR3 ("GLBSEM gs_wait_event_sem error: thread " S_FMT_THREAD_IDX ", es=" S_FMT_PTR
                       " (pthread_cond_(timed)wait failed result=%d)", 
                       threadId, es, result);
      es->predicate = 0;		/* clear for next time */
      break;				/* exit while 			*/
      }
    }  /* end while */

  /* release the mutex semaphore for the conditional variable	*/
  gs_mutex_free (&es->mutex);

#if defined(DEBUG_SISCO)
  if (gs_track)
    {
    endTime = sGetMsTime ();
    elapsedTime = endTime - startTime;
    if (elapsedTime > gs_hwEventTime)
      gs_hwEventTime = elapsedTime;
    }
#endif  /* defined(DEBUG_SISCO) */

  return (ret);
  }

/************************************************************************/
/*                       gs_wait_mult_event_sem				*/
/*----------------------------------------------------------------------*/
/* NOT IMPLEMENTED.							*/
/* Wait for event semaphore es until it becomes signaled or timeout	*/
/* occurrs.								*/
/* Parameters:								*/
/*	numEvents	number of event semaphores to wait for		*/
/*	esTable		pointer to table of event semaphore objects	*/
/*	activity	pointer to table where this function will mark	*/
/*			proper index entry with SD_TRUE for the event	*/
/*			semaphore that have been signaled		*/
/*	timeout		interval in milliseconds to wait for the es to	*/
/*			be signaled, if -1L then the function will be	*/
/*			blocked indefinitely until the event semaphore	*/
/*			is signaled.					*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/*      SD_TIMEOUT      timeout						*/
/************************************************************************/
#if 0	/* DEBUG: Not implemented on this platform. Is it needed?	*/
ST_RET gs_wait_mult_event_sem_ex (ST_INT numEvents, ST_EVENT_SEM *esTable, 
		ST_BOOLEAN *activity, ST_LONG timeout, ST_CHAR *srcFile, ST_LONG srcLine)
  {
ST_RET ret = SD_FAILURE;
  return (ret);
  }
#endif

/************************************************************************/
/*                       _gs_sig_event_sem				*/
/*----------------------------------------------------------------------*/
/* Signal event semaphore es.						*/
/* pthread_cond_signal wakes up at least one thread that is currently	*/
/* blocked on the specified condition. The higher priority thread will	*/
/* wake up first. This is like Windows auto-reset semaphore.		*/
/* pthread_cond_broadcast wakes up all waiting threads on specified	*/
/* condition. This is like Windows manual-reset semaphore.		*/
/* Parameters:								*/
/*	es		event semaphore object	(pointer)		*/
/*	predicate	value for the predicate to be set to.		*/
/* Return values:							*/
/*	none								*/
/************************************************************************/

static ST_RET _gs_sig_event_sem (ST_EVENT_SEM es, ST_INT predicate)
  {
ST_INT        result = SD_FAILURE;
#if defined(DEBUG_SISCO)
ST_THREAD_ID threadId = GET_THREAD_ID();
#endif

  if (!es)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM _gs_sig_event_sem error: thread " S_FMT_THREAD_IDX
                     " (invalid es=NULL)", threadId);
    return (result);
    }

  /* mutex for the specified condition variable must be locked before	*/
  /* going changing predicate and signaling 				*/
  gs_mutex_get (&es->mutex);

  es->predicate = predicate;
  if (es->manualReset == SD_TRUE)
    result = pthread_cond_broadcast (&es->cond);
  else
    result = pthread_cond_signal (&es->cond);

  /* release the mutex semaphore for the conditional variable	*/
  gs_mutex_free (&es->mutex);

  if (result != 0)
    {
    GLBSEM_LOG_ERR3 ("GLBSEM _gs_sig_event_sem error: thread " S_FMT_THREAD_IDX ", es=" S_FMT_PTR
                     " (pthread_cond_signal(broadcast) failed result=%d)",
                     threadId, es, result);
    }

  return (result);
  }

/************************************************************************/
/*                       gs_signal_event_sem				*/
/*----------------------------------------------------------------------*/
/* Signal event semaphore es.						*/
/* This behaviour is similiar to Windows auto-reset event semaphore	*/
/* when SetEvent is used to signal.					*/
/* The condition's predicate will be reset by the woken	thread, 	*/
/* see gs_wait_event_sem. 						*/
/* If no thread is waiting the predicate remains set but the signaling	*/
/* is not held.	If new thread calls gs_wait_event_sem it will return	*/
/* without going to wait state because the predicate value is 1.	*/
/* Parameters:								*/
/*	es		event semaphore object	(pointer)		*/
/* Return values:							*/
/*	none								*/
/************************************************************************/

ST_VOID gs_signal_event_sem (ST_EVENT_SEM es)
  {
  ST_INT result;
  ST_INT predicate = 1;
#if defined(DEBUG_SISCO)
ST_THREAD_ID threadId = GET_THREAD_ID();
#endif
  
  result = _gs_sig_event_sem (es, predicate);

  if (result == 0)
    {
    GLBSEM_LOG_FLOW2 ("GLBSEM gs_signal_event_sem: thread " S_FMT_THREAD_IDX " signaled es=" S_FMT_PTR ".",
                      threadId, es);
    }
  }

/************************************************************************/
/*                       gs_pulse_event_sem				*/
/*----------------------------------------------------------------------*/
/* Signal event semaphore es.						*/
/* This behaviour is similiar to Windows auto-reset event semaphore	*/
/* when PulseEvent is used to signal. The condition's predicate will be	*/
/* reset by the woken thread, see gs_wait_event_sem.			*/
/* If no thread is waiting the predicate remains set but the signaling	*/
/* is not held.	If a thread calls the gs_wait_event_sem it will go	*/
/* into wait state because the predicate value is 2.			*/
/* Parameters:								*/
/*	es		event semaphore object	(pointer)		*/
/* Return values:							*/
/*	none								*/
/************************************************************************/

ST_VOID gs_pulse_event_sem (ST_EVENT_SEM es)
  {
  ST_INT result;
  ST_INT predicate = 2;
#if defined(DEBUG_SISCO)
ST_THREAD_ID threadId = GET_THREAD_ID();
#endif

  result = _gs_sig_event_sem (es, predicate);

  if (result == 0)
    {
    GLBSEM_LOG_FLOW2 ("GLBSEM gs_pulse_event_sem: thread " S_FMT_THREAD_IDX " signaled es=" S_FMT_PTR ".",
                      threadId, es);
    }
  }

/************************************************************************/
/*                       gs_reset_event_sem				*/
/*----------------------------------------------------------------------*/
/* Reset event semaphore es.						*/
/* Parameters:								*/
/*	es		event semaphore object (pointer)		*/
/* Return values:							*/
/*	none								*/
/************************************************************************/

ST_VOID gs_reset_event_sem (ST_EVENT_SEM es)
  {
  /* mutex for the specified condition variable must be locked before	*/
  /* going changing predicate and signaling 				*/
  gs_mutex_get (&es->mutex);

  es->predicate = 0;

   /* release the mutex semaphore for the conditional variable	*/
  gs_mutex_free (&es->mutex);
  }

/************************************************************************/
/*                       gs_free_event_sem				*/
/*----------------------------------------------------------------------*/
/* Release resources taken by event semaphore es.			*/
/* Parameters:								*/
/*	es		event semaphore object (pointer)		*/
/* Return values:  none							*/
/*	none								*/
/************************************************************************/

ST_VOID gs_free_event_sem (ST_EVENT_SEM es)
  {
ST_RET ret;
ST_INT result;
#if defined(DEBUG_SISCO)
ST_THREAD_ID threadId = GET_THREAD_ID();
#endif

  if (!es)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_free_event_sem error: thread " S_FMT_THREAD_IDX
                     " (invalid es=NULL)", threadId);
    return;
    }

  /* free mutex	*/
  ret = gs_mutex_destroy (&es->mutex);
  if ( ret != SD_SUCCESS)
    return;  /* mutex probably busy, a thread is in the gs_wait_event_sem */

  /* free the conditional variable	*/
  result = pthread_cond_destroy (&es->cond);
  if (result != 0)
    {
    GLBSEM_LOG_ERR3 ("GLBSEM gs_free_event_sem error: thread " S_FMT_THREAD_IDX ", es=" S_FMT_PTR
                     " (pthread_cond_destroy failed result=%d)",
                     threadId, es, result);
    switch ( result )
      {
      case EBUSY:
        GLBSEM_LOG_CERR0 ("result=EBUSY (program requested to destroy"
                          " referenced condition variable).");
      break;
      case EINVAL:
        GLBSEM_LOG_CERR0 ("result=EINVAL (invalid cond argument).");
      break;
      }
    }
  else
    {
    GLBSEM_LOG_FLOW2 ("GLBSEM gs_free_event_sem: thread " S_FMT_THREAD_IDX
                      " released event sem es=" S_FMT_PTR ".", threadId, es);
    }

  /* free this allocated pointer */
  chk_free (es);
  }


	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/
	/*							*/
	/*	USER's THREAD FUNCTIONS for _AIX, __alpha	*/
	/*							*/
	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/

/************************************************************************/
/*                       gs_start_thread				*/
/*----------------------------------------------------------------------*/
/* Start a new thread.							*/
/* Parameters:								*/
/*	threadFunc		pointer to thread function to run	*/
/*	threadArg		thread function argument		*/
/*	threadHandleOut		pointer where to return thread handle	*/
/*	threadIdOut		pointer where to return thread ID	*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/************************************************************************/

ST_RET gs_start_thread (ST_THREAD_RET (*threadFunc) (ST_THREAD_ARG),
			ST_THREAD_ARG     threadArg, 
			ST_THREAD_HANDLE *threadHandleOut,
			ST_THREAD_ID     *threadIdOut)
  {
ST_RET           ret;
ST_INT           result;
ST_THREAD_HANDLE threadHandle;
pthread_attr_t   init_attr;

  /* set the thread parameters just in case the default are not set as	*/
  /* we	expecting							*/
  result = pthread_attr_init (&init_attr);
  if (result != 0 )
    {
    ret = SD_FAILURE;

    GLBSEM_LOG_ERR1 ("GLBSEM error: gs_start_thread pthread_attr_init failed"
                     " (error=%d)", result);
    switch ( result )
      {
      case EINVAL:
          GLBSEM_LOG_CERR0 ("errno=EINVAL (invalid attr argument)");
      break;
      case ENOMEM:
          GLBSEM_LOG_CERR0 ("errno=ENOMEM (insufficient memory to create thread)");
      break;
      }
    return (ret);
    }
  result = pthread_attr_setdetachstate (&init_attr, PTHREAD_CREATE_JOINABLE);
  if (result != 0 )
    {
    ret = SD_FAILURE;

    GLBSEM_LOG_ERR1 ("GLBSEM error: gs_start_thread pthread_attr_setdetachstate failed"
                     " (error=%d)", result);
    switch ( result )
      {
      case EINVAL:
          GLBSEM_LOG_CERR0 ("errno=EINVAL (invalid detachstate)");
      break;
      }
    return (ret);
    }

  /* The thread is created with the default attribute PTHREAD_CREATE_JOINABLE	*/
  /* that allows us to wait until the thread terminates in the gs_wait_thread	*/ 
  /* function.									*/

  result = pthread_create (&threadHandle,  /* pointer for thread pointer	*/
                           &init_attr,     /* if NULL, use default thread attr	*/
                           threadFunc,	   /* thread funct to be executed	*/
                           threadArg);	   /* thread funct argument		*/
  if (result == 0 )
    {
    /* Thread created successfully */
    ret = SD_SUCCESS;

    if (threadHandleOut != NULL)
      *threadHandleOut = threadHandle;

    if (threadIdOut != NULL)
      *threadIdOut = threadHandle;	/* use thread handle as id	*/

    GLBSEM_LOG_FLOW1 ("GLBSEM: gs_start_thread created thread threadHandle=" S_FMT_THREAD_IDX ".",
                      threadHandle);
    }
  else
    {
    ret = SD_FAILURE;

    GLBSEM_LOG_ERR1 ("GLBSEM error: gs_start_thread create thread failed"
                     " (error=%d)", result);
    switch ( result )
      {
      case EAGAIN:
          GLBSEM_LOG_CERR0 ("errno=EAGAIN (insufficient resources to create"
                            " thread, limit exceeded)");
      break;
      case EINVAL:
          GLBSEM_LOG_CERR0 ("errno=EINVAL (invalid attr argument)");
      break;
      case ENOMEM:
          GLBSEM_LOG_CERR0 ("errno=ENOMEM (insufficient memory to create thread)");
      break;
      case EPERM:
          GLBSEM_LOG_CERR0 ("errno=EPERM (insufficient permission to create thread)");
      break;
      }
    }

  return (ret);
  }

/************************************************************************/
/*			gs_wait_thread					*/
/*----------------------------------------------------------------------*/
/* Wait until thread with threadHandle terminates or timeout occurrs.	*/
/* On UNIX systems the is no option for timed wait. This function will	*/
/* wait until the thread is terminated.					*/
/* Parameters:								*/
/*	threadHandle	thread handle returned from gs_start_thread	*/
/*	threadId	thread ID returned from gs_start_thread		*/
/*	timeout		max time in milliseconds to wait for thread to	*/
/*			terminate. IGNORED.				*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/************************************************************************/
ST_RET gs_wait_thread (ST_THREAD_HANDLE threadHandle, 
		       ST_THREAD_ID threadId, ST_LONG timeout)
  {
ST_RET ret;
ST_INT result;

  result = pthread_join (threadHandle, NULL);
  if ( result == 0 )
    {
    /* thread terminated successfully */
    ret = SD_SUCCESS;
    GLBSEM_LOG_FLOW1 ("GLBSEM: pthread_join SUCCESS for thread threadHandle=" S_FMT_THREAD_IDX ".",
                      threadHandle);
    }
  else
    {
    ret = SD_FAILURE;
    GLBSEM_LOG_ERR2 ("GLBSEM error: pthread_join failed for thread " S_FMT_THREAD_IDX "."
                     " (error=%d)", threadHandle, result);
    switch ( result )
      {
      case EINVAL:
          GLBSEM_LOG_CERR0 ("errno=EINVAL (thread is not joinable)");
      break;
      case ESRCH:
          GLBSEM_LOG_CERR0 ("errno=ESRCH (invalid thread)");
      break;
      case EDEADLK:
          GLBSEM_LOG_CERR0 ("errno=EDEADLK (deadlock, or thread specifies"
                            " the calling thread)");
      break;
      }
    }

  return (ret);
  }

/************************************************************************/
/*                       gs_close_thread				*/
/*----------------------------------------------------------------------*/
/* Cleanup after thread terminated.					*/
/* Parameters:								*/
/*	threadHandle		thread handle ret from gs_start_thread()*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/************************************************************************/

ST_RET gs_close_thread (ST_THREAD_HANDLE threadHandle)
  {
ST_RET ret = SD_SUCCESS;

  /* nothing to clean up here */
  return (ret);
  }

#endif /* defined(S_MT_SUPPORT) */
