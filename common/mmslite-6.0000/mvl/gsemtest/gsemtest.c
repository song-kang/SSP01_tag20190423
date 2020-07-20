/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*                  1997-2008, All Rights Reserved                   	*/
/*									*/
/* MODULE NAME : gsemtest.c						*/
/* PRODUCT(S)  : Multi-thread support for old code			*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/01/08  EJV     25    Use S_FMT_HANDLE macro in logging.		*/
/*			   Rem systen includes (see sysincs.h).		*/
/* 02/24/05  EJV     24    testTimerX, timerThreadX: chg %d to %ld	*/
/*			   Chg in printf 0x%X to 0x%lX and typecast	*/
/*			     from (ST_UINT) to (ST_ULONG).		*/
/* 07/02/04  EJV     23    Added code to test timers in 2 threads	*/
/* 05/10/04  EJV     22    Added testTimer2 to test timer worker threads*/
/* 04/16/04  EJV     21    Added gs_init, replaced time(NULL).		*/
/* 11/18/03  EJV     20    Added logging to timer test.			*/
/* 11/04/03  JRB     19    Replace gs_sleep calls with sMsSleep.	*/
/* 09/29/03  JRB     18    Use _AIX code for "default" system.		*/
/*			   Enable named mutex test ONLY on Windows.	*/
/*			   Add casts on printf calls.			*/
/* 06/20/03  EJV     17    Del param from gs_mutex_get, gs_get_semx.	*/
/* 06/12/03  EJV     16    Reworked mutex implementation.		*/
/* 06/10/03  EJV     15    Added testing for Named Mutexes.		*/
/* 06/06/03  JRB     14    Chg GLBSEM_LOG_ALWAYS* to SLOGALWAYS*.	*/
/* 07/16/02  EJV     13    Reworked thread wait code for pulsing.	*/
/* 11/02/01  EJV     12    Added call to gs_close_thread.		*/
/*			   testEvent: now has manualReset parameter.	*/
/*			   Added: eventThreadX, testSetEventXThreads and*/
/*			   testPulseEventXThreads.			*/
/* 07/24/01  EJV     11    _WIN32: added support for gs_timer.		*/
/* 03/23/01  EJV     10    Added __alpha to test timer code.		*/
/* 03/14/01  EJV     09    Added check for S_MT_SUPPORT defined.	*/
/*			   Deleted S_MT_SUPPORT defined in this module.	*/
/*			   Changed testEvent to sync with glbsem changes*/
/* 02/16/01  EJV     08    _AIX:GS_SLEEP chg (nanosleep not implemented)*/
/*			   _AIX: added test for timer functionality.	*/
/* 12/20/00  EJV     07    Ported to AIX				*/
/* 12/07/00  EJV     06    Added arg to gs_get_event_sem.		*/
/* 10/12/00  EJV     05    Ported to DEC UNIX (__alpha)			*/
/* 03/13/00  JRB     04    Del obsolete gs_install call.		*/
/* 11/04/98  RKR     03    cleanup 					*/
/* 04/11/98  MDE     02    Minor cleanup for use w/MMS-LITE		*/
/* 03/28/97  MDE     01    First revision				*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "slog.h"
#include "glbsem.h"
#include "stime.h"

/* The GS_SLEEP macro takes sleep time in milliseconds.		*/
#if defined(_WIN32)

#define GS_SLEEP(t) Sleep(t)

#else

ST_EVENT_SEM  gs_sleep_event_sem;

#define GS_SLEEP(t) \
  { \
  ST_RET ret; \
  ret = gs_wait_event_sem (gs_sleep_event_sem, (ST_LONG) t); \
  if (ret != SD_TIMEOUT) printf("\nGS_SLEEP does not work properly"); \
  }   
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;

/************************************************************************/

ST_VOID testMutex (ST_VOID);
ST_THREAD_RET ST_THREAD_CALL_CONV mutexThread1 (ST_THREAD_ARG ta);
ST_THREAD_RET ST_THREAD_CALL_CONV mutexThread2 (ST_THREAD_ARG ta);
ST_VOID mutexSubFun (ST_VOID);

ST_INT mySharedArray[2000];
ST_BOOLEAN stopMutexTest;
ST_BOOLEAN stopTimerTest;

ST_VOID testEvent (ST_BOOLEAN manualReset);
ST_VOID testSetEventXThreads (ST_BOOLEAN manualReset);
ST_VOID testPulseEventXThreads (ST_BOOLEAN manualReset);
ST_VOID testTimer (ST_VOID);
ST_VOID testTimer2 (ST_VOID);
ST_VOID timer_callback_fun(ST_VOID *arg);

ST_VOID testTimersInThread (ST_VOID);
ST_THREAD_RET ST_THREAD_CALL_CONV timerThread1 (ST_THREAD_ARG ta); 
ST_THREAD_RET ST_THREAD_CALL_CONV timerThread2 (ST_THREAD_ARG ta); 

ST_THREAD_RET ST_THREAD_CALL_CONV eventThread (ST_THREAD_ARG ta);
ST_THREAD_RET ST_THREAD_CALL_CONV eventThreadX (ST_THREAD_ARG ta);
#if defined(_WIN32)	/* DEBUG: named mutex only supported on Windows	*/
ST_VOID testNamedMutex (ST_VOID);
#endif

ST_EVENT_SEM threadEventSem;
ST_EVENT_SEM mainEventSem;
ST_EVENT_SEM threadContEventSem;
ST_EVENT_SEM threadExitEventSem1;
ST_EVENT_SEM threadExitEventSem2;
ST_EVENT_SEM threadExitEventSem3;
ST_BOOLEAN verbose = SD_FALSE;
ST_BOOLEAN errorsDetected = SD_FALSE;

ST_VOID m_set_log_cfg (ST_VOID);



/************************************************************************/
/*				main					*/
/************************************************************************/

int main (int argc, char **argv)
  {
  setbuf (stdout, NULL);	/* no buffering */

#if defined(S_MT_SUPPORT)
  printf ("\n Multithread support enabled: S_MT_SUPPORT defined");
#else
  printf ("\n ERROR: Multithread support disabled: S_MT_SUPPORT not defined");
#endif

  /* init glbsem explicitly to avoid auto-initialization calls from multiple threads */
  if (gs_init() != SD_SUCCESS)
    {
    printf ("gs_init() failed");
    exit(1);
    }
  /* init stime explicitly to avoid auto-initialization calls from multiple threads */
  if (stimeInit() != SD_SUCCESS)
    {
    printf ("stimeInit() failed");
    exit(1);
    }

  m_set_log_cfg ();
/* gs_debug_sel |=  GS_LOG_FLOW; */

#if !defined (_WIN32)
  {
  gs_sleep_event_sem = gs_get_event_sem (SD_FALSE);
  if (gs_sleep_event_sem == NULL)
    {
    printf ("\nCreate sleep semaphore failed, exiting...");
    exit(1);
    }
  }
#endif

#if defined(_WIN32)	/* DEBUG: named mutex only supported on Windows	*/
  testNamedMutex ();			/* test named mutex 		*/
#else
  printf ("\n\n Named mutex semaphore NOT SUPPORTED on this platform");
#endif

  /* test timers, use default gs_timer_resolution=1000ms */
  gs_timer_set_resolution (10);
  SLOGALWAYS1("TIMER Resolution=%ld", gs_timer_get_resolution ());

  testTimer ();
  testTimer2 ();
  testTimersInThread ();


  printf("\n...testing mutex...");
  testMutex ();

  /* test event semaphores */
  testEvent (SD_FALSE);			/* test   auto-reset event	*/
  testEvent (SD_TRUE);			/* test manual-reset event	*/

  testSetEventXThreads (SD_TRUE); 	/* test manual-reset event	*/
  testSetEventXThreads (SD_FALSE);	/* test   auto-reset event	*/
  testPulseEventXThreads (SD_TRUE); 	/* test manual-reset event	*/
  testPulseEventXThreads (SD_FALSE);	/* test   auto-reset event	*/

  if (errorsDetected == SD_TRUE)
    printf ("\n\n All tests complete: ERRORS DETECTED!\n\n");
  else
    printf ("\n\n All tests complete: ALL TESTS SUCCESSFUL!\n\n");

  GS_SLEEP (5000);

#if !defined (_WIN32)
  gs_free_event_sem (gs_sleep_event_sem);
#endif
  gs_timer_cleanup (2000);
  return (0);
  }

time_t tmStartTime;
/************************************************************************/
/*                       testTimer					*/
/************************************************************************/

ST_VOID testTimer (ST_VOID)
  {
  ST_RET ret;
  ST_ULONG interval1 = 3000;  /* 300000 ms */
  ST_ULONG interval2 = 80000; /* 800000 ms */
  ST_ULONG interval3 = 43000; /* 430000 ms */
  GS_TIMER *timerId1, *timerId2, *timerId3;

  printf ("\ntimer1=%ld, timer2=%ld, timer3=%ld sec",   interval1/1000, interval2/1000, interval3/1000);
  SLOGALWAYS3("timer1=%ld, timer2=%ld, timer3=%ld sec", interval1/1000, interval2/1000, interval3/1000);
  tmStartTime = time (NULL);

  timerId1 = gs_timer_start (SD_TIMER_PERIODIC, interval1, &timer_callback_fun, (ST_VOID *)1);
  if (timerId1 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval1);  
    return;
    }
  else
    printf("\n Started timerId1=0x%0lX", (ST_ULONG) timerId1);

  timerId2 = gs_timer_start (SD_TIMER_ONESHOT, interval2, &timer_callback_fun, (ST_VOID *)2);
  if (timerId2 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval2);  
    ret = gs_timer_end (timerId1);
    return;
    }
  else
    printf("\n Started timerId2=0x%0lX", (ST_ULONG) timerId2);

  timerId3 = gs_timer_start (SD_TIMER_PERIODIC, interval3, &timer_callback_fun, (ST_VOID *)3);
  if (timerId3 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval3);  
    ret = gs_timer_end (timerId1);
    ret = gs_timer_end (timerId2);
    return;
    }
  else
    printf("\n Started timerId3=0x%0lX", (ST_ULONG) timerId3);

#if 1
  GS_SLEEP(90000);	/* 90 sec */
#else
  /* for testing longer timers */
  GS_SLEEP(900000);	/* 900 sec */
#endif

  ret = gs_timer_end (timerId1);
  if (ret == SD_SUCCESS)
    printf("\n Ended timerId1=0x%0lX", (ST_ULONG) timerId1);
  else
    printf("\n Failed to End timerId1=0x%0lX", (ST_ULONG) timerId1);

  /* not need to  end timer, SD_TIMER_ONESHOT timer is auto-deleted */
  printf("\n Ended timerId2=0x%0lX", (ST_ULONG) timerId2);

  ret = gs_timer_end (timerId3);
  if (ret == SD_SUCCESS)
    printf("\n Ended timerId3=0x%0lX", (ST_ULONG) timerId3);
  else
    printf("\n Failed to End timerId3=0x%0lX", (ST_ULONG) timerId3);
  }

/************************************************************************/
/*                       testTimer2					*/
/* More stressful timers test.						*/
/************************************************************************/

ST_VOID testTimer2 (ST_VOID)
  {
  ST_RET ret;
  ST_ULONG interval = 1000;		/* ms */
#define GS_MAX_TEST_TIMERS	20
  GS_TIMER *timerId[GS_MAX_TEST_TIMERS];
  GS_TIMER *timerId2;
  ST_INT i;

  printf ("\ntimer interval=%ld ms, number of timers=%d",   interval, GS_MAX_TEST_TIMERS);
  SLOGALWAYS2("timer interval=%ld ms, number of timers=%d",   interval, GS_MAX_TEST_TIMERS);
  tmStartTime = time (NULL);

  /* clear all IDs */
  for (i=0; i<GS_MAX_TEST_TIMERS; ++i)
    timerId[i] = 0;

  /* start timers */
  for (i=0; i<GS_MAX_TEST_TIMERS; i++)
    {
    timerId[i] = gs_timer_start (SD_TIMER_PERIODIC, interval, &timer_callback_fun, (ST_VOID *) i);
    if (timerId[i] == NULL)
      {
      errorsDetected = SD_TRUE;
      printf("\n Failed to start timer i=%d", i);
      return;
      }
    else
      printf("\n Started timerId=0x%0lX i=%d", (ST_ULONG) timerId[i], i);
    }

  /* let the timers to expire for some period of time */
  GS_SLEEP(20000);

  /* end some of timers (some worker threads should exit)*/
  for (i=0; i<GS_MAX_TEST_TIMERS-2; i++)
    {
    if (timerId[i])
      {
      ret = gs_timer_end (timerId[i]);
      if (ret == SD_SUCCESS)
        {
        printf("\n Ended timerId=0x%0lX", (ST_ULONG) timerId[i]);
	timerId[i] = 0;
	}
      else
        printf("\n Failed to End timerId=0x%0lX", (ST_ULONG) timerId[i]);
      }
    }

  /* check if we still processing ONESHOT timers OK */
  timerId2 = gs_timer_start (SD_TIMER_ONESHOT, 10000, &timer_callback_fun, (ST_VOID *)101);
  if (timerId2 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start ONESHOT timer");  
    }
  else
    printf("\n Started timerId2=0x%0lX", (ST_ULONG) timerId2);

  /* let the remaining timers to expire for some period of time */
  GS_SLEEP(20000);

  /* end rest of timers */
  for (i=0; i<GS_MAX_TEST_TIMERS; ++i)
    {
    if (timerId[i])
      {
      ret = gs_timer_end (timerId[i]);
      if (ret == SD_SUCCESS)
        {
        printf("\n Ended timerId=0x%0lX", (ST_ULONG) timerId[i]);
	timerId[i] = 0;
	}
      else
        printf("\n Failed to End timerId=0x%0lX", (ST_ULONG) timerId[i]);
      }
    }
  }

/************************************************************************/
/*			timer_callback_fun				*/
/************************************************************************/
ST_VOID timer_callback_fun(ST_VOID *arg)
  {
  ST_UINT timerNum = (ST_UINT) arg;

  printf ("\n threadId=0x%08lx Timer %d expired time=%ld sec", (long) GET_THREAD_ID(), timerNum, time(NULL) - tmStartTime);

  SLOGALWAYS3("threadId=0x%08lx Timer %d expired time=%d sec", GET_THREAD_ID(), timerNum, time(NULL) - tmStartTime);
 
  if (timerNum % 5 == 0)
    GS_SLEEP(5000);	/* simulate long time processing for some timers	*/
  }

/************************************************************************/
/*                       testTimersInThread				*/
/************************************************************************/

ST_VOID testTimersInThread (ST_VOID)
  {
ST_DOUBLE startTime;
ST_RET ret;
ST_THREAD_HANDLE thread1Handle;
ST_THREAD_ID thread1Id;
ST_THREAD_HANDLE thread2Handle;
ST_THREAD_ID thread2Id;

/* This is the MUTEX test code 						*/
  stopTimerTest = SD_FALSE;

/* Start a couple of threads that will fight for the same resource */
  ret = gs_start_thread (&timerThread1, (ST_THREAD_ARG)1, 
			 &thread1Handle, &thread1Id);
  ret |= gs_start_thread (&timerThread2, (ST_THREAD_ARG)2, 
			 &thread2Handle, &thread2Id);
  tmStartTime = time (NULL);

  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: Thread start");
    errorsDetected = SD_TRUE;
    }

/* Run the test for a while ... */
  startTime = sGetMsTime();
  while (sGetMsTime() - startTime < (ST_DOUBLE) (60*1000))
    GS_SLEEP (100);

  stopTimerTest = SD_TRUE;
  ret = gs_wait_thread (thread1Handle, thread1Id, 1000);
  if (ret == SD_SUCCESS)
    {
    if ((ret = gs_close_thread (thread1Handle)) != SD_SUCCESS)
      printf ("\n ERROR: Thread stop (gs_close_thread does not work)");
    }
  else
    printf ("\n ERROR: Thread stop (gs_wait_thread does not work)");

  ret |= gs_wait_thread (thread2Handle, thread2Id, 1000);
  if (ret == SD_SUCCESS)
    {
    if ( (ret = gs_close_thread (thread2Handle)) != SD_SUCCESS)
      printf ("\n ERROR: Thread stop (gs_close_thread does not work)");
    }
  else
    printf ("\n ERROR: Thread stop (gs_wait_thread does not work)");

  if (ret != SD_SUCCESS)
    errorsDetected = SD_TRUE;
  }

/************************************************************************/
/*                       timerThread1					*/
/************************************************************************/

ST_THREAD_RET ST_THREAD_CALL_CONV timerThread1 (ST_THREAD_ARG ta) 
  {
ST_INT i;
ST_LONG loopCount;
ST_LONG longArg;

ST_RET ret;
ST_ULONG interval1 = 1000;
ST_ULONG interval2 = 1000;
ST_ULONG interval3 = 1000;
GS_TIMER *timerId1, *timerId2, *timerId3;

  longArg = (ST_LONG) ta;
  loopCount = 0;

  printf ("\ntimer1=%ld, timer2=%ld, timer3=%ld sec",   interval1/1000, interval2/1000, interval3/1000);
  SLOGALWAYS3("timer1=%ld, timer2=%ld, timer3=%ld sec", interval1/1000, interval2/1000, interval3/1000);

  timerId1 = gs_timer_start (SD_TIMER_PERIODIC, interval1, &timer_callback_fun, (ST_VOID *)21);
  if (timerId1 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval1);  
    return ST_THREAD_RET_VAL;
    }
  else
    printf("\n Started timerId1=0x%0lX", (ST_ULONG) timerId1);

  timerId2 = gs_timer_start (SD_TIMER_ONESHOT, interval2, &timer_callback_fun, (ST_VOID *)22);
  if (timerId2 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval2);  
    ret = gs_timer_end (timerId1);
    return ST_THREAD_RET_VAL;
    }
  else
    printf("\n Started timerId2=0x%0lX", (ST_ULONG) timerId2);

  timerId3 = gs_timer_start (SD_TIMER_PERIODIC, interval3, &timer_callback_fun, (ST_VOID *)23);
  if (timerId3 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval3);  
    ret = gs_timer_end (timerId1);
    ret = gs_timer_end (timerId2);
    return ST_THREAD_RET_VAL;
    }
  else
    printf("\n Started timerId3=0x%0lX", (ST_ULONG) timerId3);


  while (stopTimerTest == SD_FALSE)
    {
    S_LOCK_COMMON_RESOURCES (); 

    for (i = 0; i < 1000; ++i)
      mySharedArray[i] = i;

    GS_SLEEP (10);

    for (i = 0; i < 1000; ++i)
      {
      if (mySharedArray[i] != i)
        {
        printf ("\n Error, the MUTEX semaphore is not working");
        errorsDetected = SD_TRUE;
	break;
        }
      }

  /* run another shared test */
    mutexSubFun ();
    S_UNLOCK_COMMON_RESOURCES ();
    ++loopCount;
    GS_SLEEP (1);
    }
  printf ("\n Thread 1 had the semaphore %ld times", loopCount);
  printf ("\n Thread 1 argument was %ld, should be 1", longArg);

  ret = gs_timer_end (timerId1);
  if (ret == SD_SUCCESS)
    printf("\n Ended timerId1=0x%0lX", (ST_ULONG) timerId1);
  else
    printf("\n Failed to End timerId1=0x%0lX", (ST_ULONG) timerId1);

  /* not need to  end timer, SD_TIMER_ONESHOT timer is auto-deleted */
  printf("\n Ended timerId2=0x%0lX", (ST_ULONG) timerId2);

  ret = gs_timer_end (timerId3);
  if (ret == SD_SUCCESS)
    printf("\n Ended timerId3=0x%0lX", (ST_ULONG) timerId3);
  else
    printf("\n Failed to End timerId3=0x%0lX", (ST_ULONG) timerId3);

  return ST_THREAD_RET_VAL;
  }     

/************************************************************************/
/*                       timerThread2					*/
/************************************************************************/

ST_THREAD_RET ST_THREAD_CALL_CONV timerThread2 (ST_THREAD_ARG ta) 
  {
ST_INT i;
ST_LONG loopCount;
ST_LONG longArg;

ST_RET ret;
ST_ULONG interval1 = 1000;
ST_ULONG interval2 = 1000;
ST_ULONG interval3 = 1000;
GS_TIMER *timerId1, *timerId2, *timerId3;

  longArg = (ST_LONG) ta;
  loopCount = 0;

  printf ("\ntimer1=%ld, timer2=%ld, timer3=%ld sec",   interval1/1000, interval2/1000, interval3/1000);
  SLOGALWAYS3("timer1=%ld, timer2=%ld, timer3=%ld sec", interval1/1000, interval2/1000, interval3/1000);

  timerId1 = gs_timer_start (SD_TIMER_PERIODIC, interval1, &timer_callback_fun, (ST_VOID *)24);
  if (timerId1 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval1);  
    return ST_THREAD_RET_VAL;
    }
  else
    printf("\n Started timerId1=0x%0lX", (ST_ULONG) timerId1);

  timerId2 = gs_timer_start (SD_TIMER_ONESHOT, interval2, &timer_callback_fun, (ST_VOID *)26); /* not 25 ! */
  if (timerId2 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval2);  
    ret = gs_timer_end (timerId1);
    return ST_THREAD_RET_VAL;
    }
  else
    printf("\n Started timerId2=0x%0lX", (ST_ULONG) timerId2);

  timerId3 = gs_timer_start (SD_TIMER_PERIODIC, interval3, &timer_callback_fun, (ST_VOID *)27);
  if (timerId3 == NULL)
    {
    errorsDetected = SD_TRUE;
    printf("\n Failed to start %lu sec timer", interval3);  
    ret = gs_timer_end (timerId1);
    ret = gs_timer_end (timerId2);
    return ST_THREAD_RET_VAL;
    }
  else
    printf("\n Started timerId3=0x%0lX", (ST_ULONG) timerId3);

  while (stopTimerTest == SD_FALSE)
    {
    S_LOCK_COMMON_RESOURCES (); 

    for (i = 0; i < 1000; ++i)
      mySharedArray[i] = 1000-i;

    GS_SLEEP (10);

    for (i = 0; i < 1000; ++i)
      {
      if (mySharedArray[i] != 1000-i)
        {
        printf ("\n Error, the MUTEX semaphore is not working");
        errorsDetected = SD_TRUE;
	break;
        }
      }

  /* run another shared test */
    mutexSubFun ();
    S_UNLOCK_COMMON_RESOURCES ();
    ++loopCount;
    GS_SLEEP (1);
    }
  printf ("\n Thread 2 had the semaphore %ld times", loopCount);
  printf ("\n Thread 2 argument was %ld, should be 2", longArg);

  ret = gs_timer_end (timerId1);
  if (ret == SD_SUCCESS)
    printf("\n Ended timerId1=0x%0lX", (ST_ULONG) timerId1);
  else
    printf("\n Failed to End timerId1=0x%0lX", (ST_ULONG) timerId1);

  /* not need to  end timer, SD_TIMER_ONESHOT timer is auto-deleted */
  printf("\n Ended timerId2=0x%0lX", (ST_ULONG) timerId2);

  ret = gs_timer_end (timerId3);
  if (ret == SD_SUCCESS)
    printf("\n Ended timerId3=0x%0lX", (ST_ULONG) timerId3);
  else
    printf("\n Failed to End timerId3=0x%0lX", (ST_ULONG) timerId3);

  return ST_THREAD_RET_VAL;
  }     


/************************************************************************/
/*                       testMutex					*/
/************************************************************************/

ST_VOID testMutex (ST_VOID)
  {
ST_DOUBLE startTime;
ST_RET ret;
ST_THREAD_HANDLE thread1Handle;
ST_THREAD_ID thread1Id;
ST_THREAD_HANDLE thread2Handle;
ST_THREAD_ID thread2Id;

/* This is the MUTEX test code 						*/
  stopMutexTest = SD_FALSE;

/* Start a couple of threads that will fight for the same resource */
  ret = gs_start_thread (&mutexThread1, (ST_THREAD_ARG)1, 
			 &thread1Handle, &thread1Id);
  ret |= gs_start_thread (&mutexThread2, (ST_THREAD_ARG)2, 
			 &thread2Handle, &thread2Id);

  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: Thread start");
    errorsDetected = SD_TRUE;
    }

/* Run the test for a while ... */
  startTime = sGetMsTime();
  while (sGetMsTime() - startTime < (ST_DOUBLE) (60*1000))
    GS_SLEEP (100);

  stopMutexTest = SD_TRUE;
  ret = gs_wait_thread (thread1Handle, thread1Id, 1000);
  if (ret == SD_SUCCESS)
    {
    if ((ret = gs_close_thread (thread1Handle)) != SD_SUCCESS)
      printf ("\n ERROR: Thread stop (gs_close_thread does not work)");
    }
  else
    printf ("\n ERROR: Thread stop (gs_wait_thread does not work)");

  ret |= gs_wait_thread (thread2Handle, thread2Id, 1000);
  if (ret == SD_SUCCESS)
    {
    if ( (ret = gs_close_thread (thread2Handle)) != SD_SUCCESS)
      printf ("\n ERROR: Thread stop (gs_close_thread does not work)");
    }
  else
    printf ("\n ERROR: Thread stop (gs_wait_thread does not work)");

  if (ret != SD_SUCCESS)
    errorsDetected = SD_TRUE;
  }

/************************************************************************/
/*                       mutexThread1					*/
/************************************************************************/

ST_THREAD_RET ST_THREAD_CALL_CONV mutexThread1 (ST_THREAD_ARG ta) 
  {
ST_INT i;
ST_LONG loopCount;
ST_LONG longArg;

  longArg = (ST_LONG) ta;
  loopCount = 0;
  while (stopMutexTest == SD_FALSE)
    {
    S_LOCK_COMMON_RESOURCES (); 

    for (i = 0; i < 1000; ++i)
      mySharedArray[i] = i;

    GS_SLEEP (10);

    for (i = 0; i < 1000; ++i)
      {
      if (mySharedArray[i] != i)
        {
        printf ("\n Error, the MUTEX semaphore is not working");
        errorsDetected = SD_TRUE;
	break;
        }
      }

  /* run another shared test */
    mutexSubFun ();
    S_UNLOCK_COMMON_RESOURCES ();
    ++loopCount;
    GS_SLEEP (1);
    }
  printf ("\n Thread 1 had the semaphore %ld times", loopCount);
  printf ("\n Thread 1 argument was %ld, should be 1", longArg);

  return ST_THREAD_RET_VAL;
  }     

/************************************************************************/
/*                       mutexThread2					*/
/************************************************************************/

ST_THREAD_RET ST_THREAD_CALL_CONV mutexThread2 (ST_THREAD_ARG ta) 
  {
ST_INT i;
ST_LONG loopCount;
ST_LONG longArg;

  longArg = (ST_LONG) ta;
  loopCount = 0;
  while (stopMutexTest == SD_FALSE)
    {
    S_LOCK_COMMON_RESOURCES (); 

    for (i = 0; i < 1000; ++i)
      mySharedArray[i] = 1000-i;

    GS_SLEEP (10);

    for (i = 0; i < 1000; ++i)
      {
      if (mySharedArray[i] != 1000-i)
        {
        printf ("\n Error, the MUTEX semaphore is not working");
        errorsDetected = SD_TRUE;
	break;
        }
      }

  /* run another shared test */
    mutexSubFun ();
    S_UNLOCK_COMMON_RESOURCES ();
    ++loopCount;
    GS_SLEEP (1);
    }
  printf ("\n Thread 2 had the semaphore %ld times", loopCount);
  printf ("\n Thread 2 argument was %ld, should be 2", longArg);

  return ST_THREAD_RET_VAL;
  }     

/************************************************************************/
/*                       mutexSubFun					*/
/************************************************************************/

ST_VOID mutexSubFun () 
  {
ST_INT i;

  S_LOCK_COMMON_RESOURCES (); 

  for (i = 0; i < 1000; ++i)
    mySharedArray[i] = 666-i;

  GS_SLEEP (10);

  for (i = 0; i < 1000; ++i)
    {
    if (mySharedArray[i] != 666-i)
      {
      printf ("\n Error, the MUTEX semaphore is not working");
      errorsDetected = SD_TRUE;
      break;
      }
    }

  S_UNLOCK_COMMON_RESOURCES ();
  }     

/************************************************************************/
/*                       testEvent					*/
/************************************************************************/

ST_VOID testEvent (ST_BOOLEAN manualReset)
  {
ST_RET ret;
ST_INT j;
ST_INT i;
ST_EVENT_SEM eSem;
ST_DOUBLE startTime;
ST_DOUBLE endTime;
ST_DOUBLE elapsedTime;
ST_DOUBLE deltaTime;
ST_THREAD_HANDLE threadHandle;
ST_THREAD_ID threadId;

  printf ("\n\n Testing event semaphores\n");

  /* !!! manualReset param is ignored on UNIX, but the gs_wait_event_sem */
  /* is acting as if it was Windows auto-reset or manual-reset semaphore */
  eSem = gs_get_event_sem (manualReset);
  if (!eSem)
    {
    printf ("\n\n Error! Could not get event semaphores");
    errorsDetected = SD_TRUE;
    return;
    }
 
  /*------------------------------------*/
  /* Check wait function timeout	*/
  /*------------------------------------*/
  startTime = sGetMsTime ();
  ret = gs_wait_event_sem (eSem, 1000);
  endTime = sGetMsTime ();
  elapsedTime = endTime - startTime;  

  if (ret == SD_SUCCESS || ret == SD_FAILURE)
    {
    printf ("\n ERROR: gs_wait_event_sem should have returned an error (timeout)");  
    errorsDetected = SD_TRUE;
    }
  deltaTime = elapsedTime - 1000.0;
  if (deltaTime < 0.0)
    deltaTime *= -1.0;
  if (deltaTime > 20.0)
    {
    printf ("\n ERROR: Elapsed time was %.0f, should have been 1000.0",  
						elapsedTime);  
    errorsDetected = SD_TRUE;
    }

  /*------------------------------------*/
  /* Check signal/wait functions order	*/
  /*------------------------------------*/
  gs_signal_event_sem (eSem);
  startTime = sGetMsTime ();
  ret = gs_wait_event_sem (eSem, 1000);
  endTime = sGetMsTime ();
  elapsedTime = endTime - startTime;  

  /* manual-reset event: event should remain in signaled state until	*/
  /* explicitly reset by call to gs_reset_event_sem. Calling		*/
  /* gs_wait_event_sem should return shortly after call with SD_SUCCESS.*/

  /* auto-reset event: event should remain in signaled state if no	*/
  /* thread is waiting for it. Calling gs_wait_event_sem should return	*/
  /* shortly after call with SD_SUCCESS.				*/

  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: gs_wait_event_sem should have returned success");  
    errorsDetected = SD_TRUE;
    }
  else
    printf ("\n Tested signal holding: OK");  
  if (elapsedTime > 10.0)
    {
    printf ("\n ERROR: Elapsed time was %.0f, should have been ~0.0", 
            elapsedTime);  
    errorsDetected = SD_TRUE;
    }
  if (manualReset == SD_TRUE)
    gs_reset_event_sem (eSem);	/* manual-reset sem need explicit reset */

  gs_free_event_sem (eSem);	/* done with this event sem */

  /*------------------------------------*/
  /* test event semaphores in 2 threads	*/
  /*------------------------------------*/
  for (j = 0; j < 2; ++j)
    {
  /* Allocate two event semaphores, 1 for each thread to wait on the sem */
    threadEventSem = gs_get_event_sem (manualReset);
    mainEventSem = gs_get_event_sem (manualReset);

    if (!threadEventSem || !mainEventSem)
      {
      printf ("\n\n Error! Could not get event semaphores");
      errorsDetected = SD_TRUE;
      return;
      }
    else
      {
    /* Start the second thread, which will wait for us to set it's	*/
    /* event semaphore before continuing.				*/
      ret = gs_start_thread (&eventThread, (ST_THREAD_ARG)&manualReset,
                             &threadHandle, &threadId);
      
      for (i = 0; i < 1000; ++i)
        mySharedArray[i] = i;
      GS_SLEEP (22);
      for (i = 0; i < 1000; ++i)
        {
        if (mySharedArray[i] != i)
          {
          printf ("\n Error, the EVENT semaphore is not working");
          errorsDetected = SD_TRUE;
     	  break;
          }
        }
  
    /* Now tell the event thread it can continue */
      gs_signal_event_sem (threadEventSem);
  
    /* Now wait for the thread to set our event sem .. */
      ret = gs_wait_event_sem (mainEventSem, -1L);
      if (ret != SD_SUCCESS)
	{
        printf ("\n Event Semaphore Error!");
	errorsDetected = SD_TRUE;
	}
      else if (verbose)
        printf ("\n Main thread got event ...");

      if (manualReset)
        gs_reset_event_sem (mainEventSem);


    /* at this point is we call gs_wait_evant semaphore, we should	*/
    /* timeout since the event is in non-signaled state			*/
      ret = gs_wait_event_sem (mainEventSem, 1000);
      if (ret != SD_TIMEOUT)
        {
        printf ("\n ERROR: gs_wait_event_sem should have returned an error (timeout)");  
        errorsDetected = SD_TRUE;
        }

    /* Free the event semaphores */  
      gs_free_event_sem (threadEventSem);
      gs_free_event_sem (mainEventSem);

    /* cleanup after exited thread */
      ret = gs_wait_thread (threadHandle, threadId, 5000);
      if (ret == SD_SUCCESS)
        {
        if ((ret = gs_close_thread (threadHandle)) == SD_SUCCESS)
          {
          threadHandle = 0;
          threadId = 0;
          }
        else
          printf ("\n ERROR: Thread stop (gs_close_thread does not work)");
        }
      else
        printf ("\n ERROR: Thread stop (gs_wait_thread does not work)");
      if (ret != SD_SUCCESS)
        errorsDetected = SD_TRUE;
      }
    }
  }
   
/************************************************************************/
/*                       eventThread					*/
/************************************************************************/

ST_THREAD_RET ST_THREAD_CALL_CONV eventThread (ST_THREAD_ARG ta) 
  {
ST_RET ret;
ST_INT i;
ST_BOOLEAN *manualReset = (ST_BOOLEAN *) ta;

  /* We will wait for the main thread to set our semaphore */
  ret = gs_wait_event_sem (threadEventSem, -1L);
  if (ret != SD_SUCCESS)
    {
    printf ("\n Event Semaphore Error!");
    errorsDetected = SD_TRUE;
    }
  else
    {
    if (*manualReset)
      gs_reset_event_sem (threadEventSem);

    if (verbose)
      printf ("\n Event thread got event ...");

    for (i = 0; i < 1000; ++i)
      mySharedArray[i] = 1000-i;
    GS_SLEEP (22);
    for (i = 0; i < 1000; ++i)
      {
      if (mySharedArray[i] != 1000-i)
        {
        printf ("\n Error, the EVENT semaphore is not working");
        errorsDetected = SD_TRUE;
        break;
        }
      }
    }
  
  /* Now tell the main loop it can continue */
  gs_signal_event_sem (mainEventSem);

  return ST_THREAD_RET_VAL;
  }     

typedef struct thread_arg
  {
  ST_BOOLEAN	manualReset;
  ST_CHAR	ch;
  } THREAD_ARG;

/************************************************************************/
/*                       testSetEventXThreads				*/
/************************************************************************/

ST_VOID testSetEventXThreads (ST_BOOLEAN manualReset)
  {
ST_RET ret;
ST_THREAD_HANDLE thread1Handle;
ST_THREAD_ID thread1Id;
ST_THREAD_HANDLE thread2Handle;
ST_THREAD_ID thread2Id;
ST_THREAD_HANDLE thread3Handle;
ST_THREAD_ID thread3Id;
THREAD_ARG ta1;
THREAD_ARG ta2;
THREAD_ARG ta3;

  printf ("\n\n---> Testing event semaphores in multiple threads\n"
          "---> manualReset=%s, using SetEvent", manualReset ? "TRUE":"FALSE");
  printf ("\n---> Expected result(all +N): +1+2+3...\n");

  /* This function will test if all threads start running when signaled	*/
  /* by the main thread							*/

  /* Allocate event semaphores */
  threadContEventSem = gs_get_event_sem (manualReset);
  threadExitEventSem1 = gs_get_event_sem (manualReset);
  threadExitEventSem2 = gs_get_event_sem (manualReset);
  threadExitEventSem3 = gs_get_event_sem (manualReset);

  if (!threadContEventSem ||
      !threadExitEventSem1 || !threadExitEventSem2 || !threadExitEventSem3)
    {
    printf ("\n\n Error! Could not get event semaphores");
    errorsDetected = SD_TRUE;
    return;
    }

  /* DIFFERENT: SetEvent and PulseEvent:				*/
  /* If we signal the event before any thread is waiting for it then	*/
  /* the state remains signaled (manual-reset and auto-reset sem)	*/
  gs_signal_event_sem (threadContEventSem);

  /* Start the second, third,... thread, which will wait for us to set	*/
  /* it's event semaphore for exit.					*/
  ta1.manualReset = manualReset;
  ta1.ch = '1';
  ret = gs_start_thread (&eventThreadX, (ST_THREAD_ARG)&ta1,
                             &thread1Handle, &thread1Id);
      
  ta2.manualReset = manualReset;
  ta2.ch = '2';
  ret = gs_start_thread (&eventThreadX, (ST_THREAD_ARG)&ta2,
                             &thread2Handle, &thread2Id);

  ta3.manualReset = manualReset;
  ta3.ch = '3';
  ret = gs_start_thread (&eventThreadX, (ST_THREAD_ARG)&ta3,
                             &thread3Handle, &thread3Id);


  /* Now tell thread(s) they can continue.				*/
  /* If auto-reset sem then only 1 thread will continue per 1 signaling.*/

  /* At this point one thread should be already running since we	*/
  /* signaled the sem above. Should see some +1 +2 +3			*/

  GS_SLEEP (1000);
  GS_SLEEP (1000);
  GS_SLEEP (1000);
  if (manualReset)
    printf ("\n! If you did'n see any +1+2+3... then holding signal is not working\n");
  else
    printf ("\n! If you did'n see any +1 or +2 or +3 then holding signal is not working\n");

  if (!manualReset)
    {
    /* release next thread */
    GS_SLEEP (3000);
    gs_signal_event_sem (threadContEventSem);
    /* release last thread */
    GS_SLEEP (3000);
    gs_signal_event_sem (threadContEventSem);
    }
  GS_SLEEP (3000);

  if (manualReset)
    /* by now all thread should be running, reset the event sem */
    gs_reset_event_sem (threadContEventSem);

  /* cleanup after thread exiting */

  printf ("\nWaiting for thread threadId=0x%08x to terminate", (ST_UINT) thread1Id);
  gs_signal_event_sem (threadExitEventSem1);	/* if manual-reset will terminate all threads */
  ret = gs_wait_thread (thread1Handle, thread1Id, 5000);
  ret |= gs_close_thread (thread1Handle);
  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: Thread stop (gs_wait_thread or gs_close_thread returned error)");
    errorsDetected = SD_TRUE;
    }

  printf ("\nWaiting for thread threadId=0x%08x to terminate", (ST_UINT) thread2Id);
  gs_signal_event_sem (threadExitEventSem2);
  ret = gs_wait_thread (thread2Handle, thread2Id, 5000);
  ret |= gs_close_thread (thread2Handle);
  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: Thread stop (gs_wait_thread or gs_close_thread returned error)");
    errorsDetected = SD_TRUE;
    }

  printf ("\nWaiting for thread threadId=0x%08x to terminate", (ST_UINT) thread3Id);
  gs_signal_event_sem (threadExitEventSem3);
  ret = gs_wait_thread (thread3Handle, thread3Id, 5000);
  ret |= gs_close_thread (thread3Handle);
  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: Thread stop (gs_wait_thread or gs_close_thread returned error)");
    errorsDetected = SD_TRUE;
    }

  /* Free the event semaphores */  
  gs_free_event_sem (threadContEventSem);
  gs_free_event_sem (threadExitEventSem1);
  gs_free_event_sem (threadExitEventSem2);
  gs_free_event_sem (threadExitEventSem3);

  }
 
/************************************************************************/
/*                       testPulseEventXThreads				*/
/************************************************************************/

ST_VOID testPulseEventXThreads (ST_BOOLEAN manualReset)
  {
ST_RET ret;
ST_THREAD_HANDLE thread1Handle;
ST_THREAD_ID thread1Id;
ST_THREAD_HANDLE thread2Handle;
ST_THREAD_ID thread2Id;
ST_THREAD_HANDLE thread3Handle;
ST_THREAD_ID thread3Id;
THREAD_ARG ta1;
THREAD_ARG ta2;
THREAD_ARG ta3;

  printf ("\n\n---> Testing event semaphores in multiple threads\n"
          "---> manualReset=%s, using PulseEvent", manualReset ? "TRUE":"FALSE");
  printf ("\n---> Expected result(all +N): +1+2+3...\n");

  /* This function will test if all threads start running when signaled	*/
  /* by the main thread							*/

  /* Allocate event semaphores */
  threadContEventSem = gs_get_event_sem (manualReset);
  threadExitEventSem1 = gs_get_event_sem (manualReset);
  threadExitEventSem2 = gs_get_event_sem (manualReset);
  threadExitEventSem3 = gs_get_event_sem (manualReset);

  if (!threadContEventSem ||
      !threadExitEventSem1 || !threadExitEventSem2 || !threadExitEventSem3)
    {
    printf ("\n\n Error! Could not get event semaphores");
    errorsDetected = SD_TRUE;
    return;
    }

  /* DIFFERENT: SetEvent and PulseEvent:				*/
  /* If we pulse the event before any thread is waiting for it then	*/
  /* the state will be non-signaled after function returns (manual-reset*/
  /* and auto-reset sem)						*/
  gs_pulse_event_sem (threadContEventSem);

  /* Start the second, third,... thread, which will wait for us to set	*/
  /* it's event semaphore for exit.					*/
  ta1.manualReset = manualReset;
  ta1.ch = '1';
  ret = gs_start_thread (&eventThreadX, (ST_THREAD_ARG)&ta1,
                             &thread1Handle, &thread1Id);
      
  ta2.manualReset = manualReset;
  ta2.ch = '2';
  ret = gs_start_thread (&eventThreadX, (ST_THREAD_ARG)&ta2,
                             &thread2Handle, &thread2Id);

  ta3.manualReset = manualReset;
  ta3.ch = '3';
  ret = gs_start_thread (&eventThreadX, (ST_THREAD_ARG)&ta3,
                             &thread3Handle, &thread3Id);

  GS_SLEEP (1000);
  GS_SLEEP (1000);
  GS_SLEEP (1000);
  printf ("\n ! If you see by now any +N printed then the PULSING is not working\n");

  /* Now tell thread(s) they can continue, if auto-reset sem only one	*/
  /* thread will continue per one event posting.			*/
  gs_pulse_event_sem (threadContEventSem);
  if (manualReset == SD_FALSE)
    {
    /* release next thread */
    GS_SLEEP (3000);
    gs_pulse_event_sem (threadContEventSem);
    /* release last thread */
    GS_SLEEP (3000);
    gs_pulse_event_sem (threadContEventSem);
    }  
  GS_SLEEP (3000);

  /* manual-reset sem sem already reset, explicit call not required	*/

  /* cleanup after exiting threads */

  printf ("\nWaiting for thread threadId=0x%08x to terminate", (ST_UINT) thread1Id);
  ret = 1;
  while (ret != 0)
    {
    /* Now tell the event thread(s) they can exit */
    gs_pulse_event_sem (threadExitEventSem1);
    ret = gs_wait_thread (thread1Handle, thread1Id, 500);
    printf(" +1exiting ");
    }
  ret |= gs_close_thread (thread1Handle);
  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: Thread stop (gs_wait_thread or gs_close_thread returned error)");
    errorsDetected = SD_TRUE;
    }

  printf ("\nWaiting for thread threadId=0x%08x to terminate", (ST_UINT) thread2Id);
  ret = 1;
  while (ret != 0)
    {
    /* Now tell the event thread(s) they can exit */
    gs_pulse_event_sem (threadExitEventSem2);
    ret = gs_wait_thread (thread2Handle, thread2Id, 500);
    printf(" +2exiting ");
    }
  ret |= gs_close_thread (thread2Handle);
  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: Thread stop (gs_wait_thread or gs_close_thread returned error)");
    errorsDetected = SD_TRUE;
    }

  printf ("\nWaiting for thread threadId=0x%08x to terminate", (ST_UINT) thread3Id);
  ret = 1;
  while (ret != 0)
    {
    /* Now tell the event thread(s) they can exit */
    gs_pulse_event_sem (threadExitEventSem3);
    ret = gs_wait_thread (thread3Handle, thread3Id, 500);
    printf(" +3exiting ");
    }
  ret |= gs_close_thread (thread3Handle);
  if (ret != SD_SUCCESS)
    {
    printf ("\n ERROR: Thread stop (gs_wait_thread or gs_close_thread returned error)");
    errorsDetected = SD_TRUE;
    }

  /* Free the event semaphores */  
  gs_free_event_sem (threadContEventSem);
  gs_free_event_sem (threadExitEventSem1);
  gs_free_event_sem (threadExitEventSem2);
  gs_free_event_sem (threadExitEventSem3);

  }


/************************************************************************/
/*                       eventThreadX					*/
/************************************************************************/

ST_THREAD_RET ST_THREAD_CALL_CONV eventThreadX (ST_THREAD_ARG ta) 
  {
ST_RET ret;
THREAD_ARG *targ = (THREAD_ARG *) ta;

  printf ("\n Thread threadId=0x%08lx running ...\n", (long) GET_THREAD_ID());

  /* We will wait for the main thread to set our semaphore */
  ret = gs_wait_event_sem (threadContEventSem, -1);
  if (ret != SD_SUCCESS)
    {
    printf ("\n Event Semaphore Error!");
    errorsDetected = SD_TRUE;
    }
  else
    {
    /* We will wait for the main thread to set our semaphore */
    printf ("\n");
    switch (targ->ch)
      {
      /* Note: the printf may cause the wait function to miss the semaphore posting */
      case '1': while ( gs_wait_event_sem (threadExitEventSem1, 500) == SD_TIMEOUT)
                printf ("+%c", targ->ch);
              break;
      case '2': while ( gs_wait_event_sem (threadExitEventSem2, 500) == SD_TIMEOUT)
                printf ("+%c", targ->ch);
              break;
      case '3': while ( gs_wait_event_sem (threadExitEventSem3, 500) == SD_TIMEOUT)
                printf ("+%c", targ->ch);
              break;
      }
    }

  return ST_THREAD_RET_VAL;
  }     

#if defined(_WIN32)	/* DEBUG: named mutex only supported on Windows	*/
/************************************************************************/
/*			testNamedMutex					*/
/* This test (gsemtest) should be run from two DOS boxes at about the	*/
/* same time to see if the the mutex works properly.			*/
/************************************************************************/
ST_VOID testNamedMutex (ST_VOID)
{
ST_RET         ret;
ST_CHAR        mutexName[80];
ST_MUTEX_SEM   ms;
ST_LONG        timeout = 50000;	/* milliseconds */


  printf ("\n -->>>>>>");
  printf ("\n To test the Named Mutex run gsemtest in 2 DOS windows!");
  printf ("\n    wait timeout=%ld sec, owning time=%ld sec", timeout/1000, timeout/2000);

  strcpy (mutexName, "mutexA");
  ret = gs_named_mutex_create (&ms, mutexName);
  if (ret != SD_SUCCESS)
    {
    printf ("\n    Named mutex '%s' create error", mutexName);
    errorsDetected = SD_TRUE;
    return;
    }

#if defined(_WIN32)
  printf ("\n    Named mutex '%s' created/opened hMutex="S_FMT_HANDLE ".", mutexName, ms.u.hMutex);
#endif

  printf ("\n    Named mutex '%s' getting", mutexName);
  ret = gs_mutex_get (&ms);
  if (ret == SD_SUCCESS)
    {
    printf ("\n    Named mutex '%s' owning", mutexName);
    /* hold the mutex for a whole and release */
    sMsSleep (timeout/2);
    ret = gs_mutex_free (&ms);
    printf ("\n    Named mutex '%s' released", mutexName);
    }
  else if (ret == SD_TIMEOUT)
    printf ("\n    Named mutex '%s' get failed (timeout)", mutexName);
  else
    printf ("\n    Named mutex '%s' get failed (error)", mutexName);

  printf ("\n ->>>>>>");
  ret = gs_mutex_destroy (&ms);
}
#endif	/* DEBUG: named mutex only supported on Windows	*/

/************************************************************************/
/*			m_set_log_cfg					*/
/* sLogCtrl points to a pre-allocated LOG_CTRL structure so only have	*/
/* to fill it in.							*/
/************************************************************************/

ST_VOID m_set_log_cfg (ST_VOID)
  {
/* Use File logging							*/
  sLogCtrl->logCtrl = LOG_FILE_EN;

/* Use time/date time log						*/
  sLogCtrl->logCtrl |= LOG_TIME_EN;

/* File Logging Control defaults 					*/
  sLogCtrl->fc.fileName = "test.log";
  sLogCtrl->fc.maxSize = 1000000L;
  sLogCtrl->fc.ctrl = (FIL_CTRL_WIPE_EN | 
                 FIL_CTRL_WRAP_EN | 
                 FIL_CTRL_MSG_HDR_EN |
		 FIL_CTRL_NO_APPEND);

  }


