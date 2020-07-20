/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2001-2005, All Rights Reserved					*/
/*									*/
/* MODULE NAME : glbtimer.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :  Portable Timer functions.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			gs_timer_*					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/19/10  DSF     03    Added SEV_SUPPORT				*/
/* 05/25/05  DSF     02    A bug fix in gs_timer_init ()		*/
/* 05/23/05  JRB     01    Timer code moved from glbsem to here.	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "slog.h"
#include "glbsem.h"
#if defined (SEV_SUPPORT)
#include "sev.h"
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_BOOLEAN	gs_timer_initialized = SD_FALSE;
static ST_ULONG		gs_timer_resolution = 10;	/* in milliseconds */

static GS_TIMER        *gs_timer_lst = NULL;	/* list of user timers		*/
static ST_MUTEX_SEM     gs_timer_lst_mutex;	/* protects access to gs_timer_lst */
/* Note: On UNIX system the gs_timer_lst contains list of active timers.	*/
/* If _WIN32_timers is defined:							*/
/*	 On _WIN32 the gs_timer_lst contain list of pointers to delete with	*/
/*       a delay (after the gs_timer_end function have been called).		*/

static ST_RET   gs_timer_remove (GS_TIMER *timer_id);

#if defined (_WIN32_timers)

#if (_MSC_VER >= 1300)
void CALLBACK gs_timer_callback (UINT uTimerID, UINT uMsg, 
                                 DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
#else
void CALLBACK gs_timer_callback (UINT uTimerID, UINT uMsg, 
                                 DWORD dwUser, DWORD dw1, DWORD dw2);
#endif
static ST_RET gs_timer_check_list (ST_VOID);

#else
/* UNIX & Windows */
	/* Note: On AIX the real-time support is not implemented.	*/
	/*	 Functions such as timer_create, sleep, or nanosleep are*/
	/*	 not available (in AIX version 4.3.2 or 4.3.3).		*/
	/*	 This implementation of timers is using the GLBSEM	*/
	/*	 gs_wait_event_sem function implemented with phtreads	*/
	/*	 to support the timer functionality.			*/
	/* Note: The negatives of this UNIX timers implementations are:	*/
	/*	 If some functions take long time in timer callback_fun	*/
	/*       then the other timers will not be processed on time.	*/
	/*	 This could degrade multithreaded applications.		*/
	/*	 In addition if there are many busy threads in the	*/
	/*	 application then the timer thread may not be called on	*/
	/*	 time because it has the same priority as other		*/
	/*	 application threads.					*/


/* main timer thread */
static ST_THREAD_HANDLE gs_timer_thread_handle = 0;
static ST_THREAD_ID     gs_timer_thread_id = 0;
static ST_BOOLEAN       gs_timer_thread_stop = SD_FALSE;
static ST_EVENT_SEM     gs_timer_event_sem = NULL;  /* use event sem for sleep	*/

/* timer worker threads */
typedef struct gs_timer_worker_tag
  {
  ST_BOOLEAN		used;
  ST_BOOLEAN		stop;
  ST_THREAD_HANDLE	thread_handle;
  ST_THREAD_ID		thread_id;
  } GS_TIMER_WORKER;

static GS_TIMER_WORKER *gs_timer_worker_list = NULL;
static ST_UINT          gs_timer_worker_cnt;
static ST_MUTEX_SEM     gs_timer_worker_mutex;	   /* protects access to gs_timer_worker_list	*/
static ST_EVENT_SEM     gs_timer_worker_event_sem = NULL;  /* event sem for timer worker threads */

ST_UINT  gs_timer_worker_min = GS_TIMER_WORKER_MIN;
ST_UINT  gs_timer_worker_max = GS_TIMER_WORKER_MAX;


static ST_THREAD_RET ST_THREAD_CALL_CONV gs_timer_thread (ST_THREAD_ARG arg);
static ST_THREAD_RET ST_THREAD_CALL_CONV gs_timer_worker_thread (ST_THREAD_ARG arg);

static ST_RET gs_timer_start_worker (ST_VOID);
static ST_RET gs_timer_stop_worker (ST_BOOLEAN all, ST_LONG timeout);

#endif /* defined (_WIN32_timers) */


/************************************************************************/
/*			gs_timer_set_resolution				*/
/*----------------------------------------------------------------------*/
/* Set timer resolution to the lowest value required by an application	*/
/* before calling gs_start_timer. The default value may be already	*/
/* satisfying the application needs.					*/
/* On Windows system if the timer resolution is changed while the 	*/
/* application is running, only timers started afterwards will be	*/
/* affected. On UNIX system all timers will be afected.			*/
/*									*/
/* Parameters:								*/
/*	timer_resolution	On Windows system it could be 0	to have	*/
/*				the resolution with most accurancy.	*/
/* Return:								*/
/* 	SD_SUCCESS if the timer resolution set				*/
/*	SD_FAILURE otherwise						*/
/************************************************************************/
ST_RET gs_timer_set_resolution (ST_ULONG timer_resolution)
{
  gs_timer_resolution = timer_resolution;

  return (SD_SUCCESS);
}

/************************************************************************/
/*			gs_timer_get_resolution				*/
/*----------------------------------------------------------------------*/
/* Get timer resolution returns the current resolution in milliseconds.	*/
/*									*/
/* Parameters:								*/
/*	none								*/
/* Return:								*/
/* 	ST_ULONG	timer resolution				*/
/************************************************************************/
ST_ULONG gs_timer_get_resolution (ST_VOID)
{
  return (gs_timer_resolution);
}


/************************************************************************/
/*			gs_timer_init					*/
/*----------------------------------------------------------------------*/
/* This is timer support initialization function. 			*/
/* It is not neccessary to call this function because it is called by	*/
/* the gs_timer_start function. In multhithreaded application it maybe	*/
/* required to call this functions before any thread calls the		*/
/* gs_timer_start function to avoid initialization problem from multiple*/
/* threads.								*/
/* To properly cleanup resources before application exits the function	*/
/* gs_timer_cleanup should be called.					*/
/* NOTE: the gs_timer_cleanup should not be called from other thread	*/
/*       while this functions is executing.				*/
/* Parameters:								*/
/*	min_workers	Min number of worker threads to start.		*/
/*			If this parameter is 0 the GS_TIMER_WORKER_MIN	*/
/*			value will be used.				*/
/*	max_workers	Max number of worker threads.			*/
/*			If this parameter is 0 the GS_TIMER_WORKER_MAX	*/
/*			value will be used.				*/
/* Return:								*/
/* 	SD_SUCCESS if timer support initialized				*/
/*	SD_FAILURE otherwise						*/
/************************************************************************/
ST_RET   gs_timer_init (ST_UINT min_workers, ST_UINT max_workers)
{
ST_RET  ret;
ST_UINT i;

  if (gs_timer_initialized)
    return (SD_SUCCESS);			/* already initialized	*/

  stimeInit ();				/* we need milliseconds timer	*/

  /* create mutex sem to protect the timer list */
  ret = gs_mutex_create (&gs_timer_lst_mutex);
  if (ret != SD_SUCCESS)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_init error: create mutex failed ret=%d", ret);
    return (ret);
    }

#if !defined (_WIN32_timers)
  /* create mutex sem to protect the worker list */
  ret = gs_mutex_create (&gs_timer_worker_mutex);
  if (ret != SD_SUCCESS)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_init error: create worker mutex failed ret=%d", ret);
    gs_mutex_destroy (&gs_timer_lst_mutex);
    return (ret);
    }

  /* create timer event semaphore */
  gs_timer_event_sem = gs_get_event_sem (SD_FALSE);
  if (gs_timer_event_sem == NULL)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_init error: create timer semaphore failed ret=%d", ret);
    gs_mutex_destroy (&gs_timer_lst_mutex);
    gs_mutex_destroy (&gs_timer_worker_mutex);
    return (SD_FAILURE);
    }

  /* create timer event semaphore for worker threads */
  gs_timer_worker_event_sem = gs_get_event_sem (SD_FALSE);
  if (gs_timer_worker_event_sem == NULL)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_init error: create worker timer semaphore failed ret=%d", ret);
    gs_free_event_sem (gs_timer_event_sem);
    gs_mutex_destroy (&gs_timer_lst_mutex);
    gs_mutex_destroy (&gs_timer_worker_mutex);
    return (SD_FAILURE);
    }

  /* start main timer thread */
  ret = gs_start_thread (gs_timer_thread, (ST_THREAD_ARG) 0, 
			&gs_timer_thread_handle, &gs_timer_thread_id);
  if (ret != SD_SUCCESS)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_init error: start timer thread failed ret=%d", ret);
    gs_free_event_sem (gs_timer_event_sem);
    gs_free_event_sem (gs_timer_worker_event_sem);
    gs_mutex_destroy (&gs_timer_lst_mutex);
    gs_mutex_destroy (&gs_timer_worker_mutex);
    return (ret);
    }
  GLBSEM_LOG_FLOW1 ("GLBSEM gs_timer_init: starting timer thread threadId=0x%08lx", gs_timer_thread_id);

  /* start few timer worker threads */
  gs_timer_worker_min = min_workers ? min_workers : GS_TIMER_WORKER_MIN;
  gs_timer_worker_max = max_workers ? max_workers : GS_TIMER_WORKER_MAX;
  if (gs_timer_worker_max < gs_timer_worker_min)
    gs_timer_worker_max = gs_timer_worker_min;

  gs_timer_worker_list = (GS_TIMER_WORKER *) chk_calloc (gs_timer_worker_max, sizeof(GS_TIMER_WORKER));
  for (i=0; i<gs_timer_worker_min; ++i)
    {
    ret = gs_timer_start_worker ();
    if (ret != SD_SUCCESS)
      break;
    }
  if (ret != SD_SUCCESS)
    {
    gs_timer_cleanup (2*gs_timer_resolution);	/* stop all timer threads	*/
    return (ret);
    }
#endif /* !defined (_WIN32_timers) */

  gs_timer_initialized = SD_TRUE;
  gs_timer_thread_stop = SD_FALSE;

  return (ret);
}

/************************************************************************/
/*			gs_timer_start					*/
/*----------------------------------------------------------------------*/
/* This function will start user timer.					*/
/* If the timer type is SD_TIMER_ONESHOT type then the entry will be	*/
/* deleted from the list of timers after the first time the timer is	*/
/* serviced. If the timer type is periodic the user needs to call the	*/
/* gs_timer_end function to stop the timer and release resources.	*/
/*									*/
/* Parameters:								*/
/*	type		timer type SD_TIMER_ONESHOT or SD_TIMER_PERIODIC*/
/*	interval	num of milliseconds >0 between timer servicing 	*/
/*			Should be multiple of gs_timer_resolution.	*/
/*	callback_fun	pointer to fun that is called when timer expires*/
/*	arg		argument to callback_fun			*/
/* Return:								*/
/*	pointer to the timer structure if function successful		*/
/*	NULL if function failed						*/
/************************************************************************/
GS_TIMER *gs_timer_start (ST_UINT  type,
                          ST_ULONG interval,
                          ST_VOID (*callback_fun)(ST_VOID *),
                          ST_VOID  *arg)
{
GS_TIMER *timer_id = NULL;	/* set initially to invalid timer id	*/

  if (!gs_timer_initialized)
    {
    if (gs_timer_init (GS_TIMER_WORKER_MIN, GS_TIMER_WORKER_MAX) != SD_SUCCESS)
      {
      GLBSEM_LOG_ERR0 ("GLBSEM gs_timer_start error: timer support failed to initialize");
      return (timer_id);
      }
    }

#if !defined (_WIN32_timers)
  if (gs_timer_thread_stop)
    {
    GLBSEM_LOG_NERR0 ("GLBSEM gs_timer_end error: timer thread exited");
    return (timer_id);
    }
#endif  /* !defined (_WIN32_timers) */

  if (type != SD_TIMER_ONESHOT && type != SD_TIMER_PERIODIC)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_start error: invalid type=%u", type);
    return (timer_id);
    }

  if (interval == 0)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_start error: invalid interval=%u", interval);
    return (timer_id);    
    }

  if (callback_fun == NULL)
    {
    GLBSEM_LOG_ERR0 ("GLBSEM gs_timer_start error: invalid callback_fun=NULL");
    return (timer_id);    
    }

  /* allocate struct for the timer */
  timer_id = (GS_TIMER *) chk_calloc (1, sizeof (GS_TIMER));
  if (timer_id == NULL)
    {
    GLBSEM_LOG_ERR0 ("GLBSEM gs_timer_start error: allocate timer struct failed");
    return (timer_id);    
    }

  /* set timer's parameters */
  timer_id->type = type;
  timer_id->callback_fun = callback_fun;
  timer_id->arg = arg;
  timer_id->interval = interval;
  timer_id->expiration = sGetMsTime() + (ST_DOUBLE) timer_id->interval;

#if defined (_WIN32_timers)
  {
  UINT event;

  /* IMPORTANT: While testing we discovered that the max interval for PERIODIC	*/
  /*		timer is 430000 ms (7 min 10 sec). The timeSetEvent fun will not*/
  /*		return error if the interval is greater than 430000 but will	*/
  /*		compute new interval as follow:					*/
  /*			interval % 430000 + 1000				*/
  /*		Because ONE-SHOT timer works OK we will use this timer instead	*/
  /*		for the case when the timer interval needs to be >= 430000.	*/
  if (type == SD_TIMER_PERIODIC)
    {
    if (interval < 430000)
      {
      event = TIME_PERIODIC;
#if (_MSC_VER >= 1300)
      /* if we use TIME_KILL_SYNCHRONOUS we will assure that periodic	*/
      /* timer will not be called after timeKillEvent is called.	*/
      if (gs_is_win_ver(SD_WIN_VER_XP_AND_LATER) == 1)
        event = TIME_PERIODIC | TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS;
#endif
      }
    else
      event = TIME_ONESHOT;	/* will need to call series of ONE-SHOT timers */
    }
  else
    event = TIME_ONESHOT;

  timer_id->timerID = timeSetEvent ((UINT) interval,
                                    (UINT) gs_timer_resolution,
                                    gs_timer_callback,
#if (_MSC_VER >= 1300)
                                    (DWORD_PTR) timer_id,
#else
                                    (DWORD) timer_id,
#endif
                                    event);
  if (timer_id->timerID == 0)
    {
    GLBSEM_LOG_ERR0 ("GLBSEM gs_timer_start error: timeSetEvent failed");
    chk_free (timer_id);
    return (NULL);
    }
  }
#else
  {
  /* UNIX only */
  /* add new timer to the front of the timer list gs_timer_lst */
  gs_mutex_get (&gs_timer_lst_mutex);
  list_add_first (&gs_timer_lst, timer_id);
  gs_mutex_free (&gs_timer_lst_mutex);
  }
#endif  /* defined (_WIN32_timers) */

  GLBSEM_LOG_FLOW2 ("GLBSEM gs_timer_start: thread threadId=0x%08lx started timer tm=0x%X",
                    GET_THREAD_ID(), timer_id);

  return (timer_id);
}

#if defined (_WIN32_timers)
/************************************************************************/
/*			gs_timer_callback				*/
/* This function is called by Windows when the timer interval elapses.	*/
/************************************************************************/
#if (_MSC_VER >= 1300)
void CALLBACK gs_timer_callback (UINT uTimerID, UINT uMsg, 
                                 DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
#else
void CALLBACK gs_timer_callback (UINT uTimerID, UINT uMsg, 
                                 DWORD dwUser, DWORD dw1, DWORD dw2)
#endif
{
GS_TIMER *timer_id = (GS_TIMER *) dwUser;

  if (!gs_timer_initialized)
    return;  /* ignore this callback, gs_timer_cleanup may have been called */

  /* if timer processing is already pending or timer was stopped ignore this call */
  if (timer_id->pending || timer_id->delay_start != 0)
    {
    GLBSEM_LOG_FLOW1 ("GLBSEM gs_timer_callback: ignoring timer callback"
                      " for tm=0x%X (previous timer callback processing is still pending or timer stopped)",
                      timer_id);
    return;
    }

  timer_id->pending = SD_TRUE;
  (*timer_id->callback_fun)(timer_id->arg);
  timer_id->pending = SD_FALSE;

  if (timer_id->type == SD_TIMER_ONESHOT)
    {
    /* do not need any more, free  the timer resources */
    gs_timer_remove (timer_id);
    }

  /* if this was ONE-SHOT timer we need to restart it again	*/
  if (timer_id->type == SD_TIMER_PERIODIC && timer_id->interval >= 430000)
    {
    timer_id->timerID = timeSetEvent ((UINT) timer_id->interval,
                                      (UINT) gs_timer_resolution,
                                      gs_timer_callback,
#if (_MSC_VER >= 1300)
                                      (DWORD_PTR) timer_id,
#else
                                      (DWORD) timer_id,
#endif
                                      TIME_ONESHOT);
    if (timer_id->timerID == 0)
      {
      GLBSEM_LOG_ERR0 ("GLBSEM restart timer error: timeSetEvent failed");
      chk_free (timer_id);
      }
    }
}
#endif  /* defined (_WIN32_timers) */

/************************************************************************/
/*			gs_timer_end					*/
/*----------------------------------------------------------------------*/
/* Function to stop the periodic timer and release resources.		*/
/* Oneshot timer will release its resources automatically after the	*/
/* first time it is serviced.						*/
/* Parameters:								*/
/*	timer_id	timer ID returned from gs_timer_start		*/
/* Return:								*/
/* 	SD_SUCCESS if function successful				*/
/*	SD_FAILURE otherwise						*/
/************************************************************************/
ST_RET gs_timer_end (GS_TIMER *timer_id)
{
ST_RET ret;

  if (!gs_timer_initialized)
    {
    GLBSEM_LOG_NERR0 ("GLBSEM gs_timer_end error: timer support not initialized");
    return (SD_FAILURE);
    }

  if (timer_id == NULL)
    {
    GLBSEM_LOG_ERR0 ("GLBSEM gs_timer_end error: invalid timer_id=NULL");
    return (SD_FAILURE);
    }

#if defined (_WIN32_timers)
  {
  if (timer_id->delay_start == 0)
    timer_id->delay_start = 1000;  /* prevents processing callback functions */
  ret = timeKillEvent (timer_id->timerID);
  if (ret != TIMERR_NOERROR)
    {
    GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_end error: timeKillEvent failed error=%d", ret);
    }
  }
#endif /* defined (_WIN32_timers) */

  ret = gs_timer_remove (timer_id);

#if defined (_WIN32_timers)
  /* check if any timer pointers can be deleted from the list */
  gs_timer_check_list ();
#endif /* defined (_WIN32_timers) */

  return (ret);
}

#if defined (_WIN32_timers)
/************************************************************************/
/*			gs_timer_check_list				*/
/*----------------------------------------------------------------------*/
/* On _Win32 check what timer pointers are ready to delete.		*/
/* Parameters:								*/
/*	none								*/
/* Return:								*/
/* 	SD_SUCCESS if function successful				*/
/*	SD_FAILURE otherwise						*/
/************************************************************************/
static ST_RET gs_timer_check_list (ST_VOID)
{
ST_RET    ret = SD_SUCCESS;
GS_TIMER *timer_ptr;
GS_TIMER *timer_next;
ST_DOUBLE time_now;
ST_ULONG  timer_end_delay;

  gs_mutex_get (&gs_timer_lst_mutex);
  /* check if any pointers are ready to delete */
  timer_ptr = gs_timer_lst;
  time_now = sGetMsTime ();
  while (timer_ptr)
    {
    timer_next = list_get_next (gs_timer_lst, timer_ptr);
    /* do not delete ptr when callback is pending */
    if (!timer_ptr->pending)
      {
      /* wait for 10 times the interval (but no longer than 1 sec) before deleting the pointer */
      timer_end_delay = min (timer_ptr->interval * 10, 1000);
      if ((ST_ULONG)(time_now - timer_ptr->delay_start) > timer_end_delay)
        {
        /* delete the timer pointer */
        ret = list_unlink (&gs_timer_lst, timer_ptr);
	if (timer_ptr->event)
	  gs_signal_event_sem (timer_ptr->event);
        GLBSEM_LOG_FLOW2 ("GLBSEM gs_timer_check_list: thread 0x%08lx free timer pointer tm=0x%X",
                          GET_THREAD_ID(), timer_ptr);
        chk_free (timer_ptr);
	}
      }
    timer_ptr = timer_next;
    }
  gs_mutex_free (&gs_timer_lst_mutex);

  return (ret);
}
#endif /* defined (_WIN32_timers) */

/************************************************************************/
/*			gs_timer_remove					*/
/*----------------------------------------------------------------------*/
/* On UNIX this function will delete the timer_id from the list of	*/
/* active timers.							*/
/* On _Win32 the timer_id will be added to list of timers waiting to be	*/
/* destroyed. Windows can call a timer callback function after the 	*/
/* applications called the gs_timer_end(), so we save all the timer	*/
/* pointers on a delete list. The list is checked for timers ready for	*/
/* deletion.								*/
/* Parameters:								*/
/*	timer_id	timer ID returned from gs_timer_start		*/
/* Return:								*/
/* 	SD_SUCCESS if function successful				*/
/*	SD_FAILURE otherwise						*/
/************************************************************************/
static ST_RET gs_timer_remove (GS_TIMER *timer_id)
{
ST_RET     ret = SD_SUCCESS;

#if defined (_WIN32_timers)
  {
  /* add the timer_id to delayed delete list */
  gs_mutex_get (&gs_timer_lst_mutex);
  timer_id->delay_start = sGetMsTime ();
  ret = list_add_last (&gs_timer_lst, timer_id);
  gs_mutex_free (&gs_timer_lst_mutex);
  if (ret == SD_SUCCESS)
    {
    GLBSEM_LOG_FLOW2 ("GLBSEM gs_timer_remove: thread 0x%08lx stopped timer tm=0x%X",
                      GET_THREAD_ID(), timer_id);
    }

  }  
#else
  {   /* UNIX & Windows */
  /* Find the timer_id and delete it from the list */
  gs_mutex_get (&gs_timer_lst_mutex);
  /* if the timer is in callback fun we need to wait before unlinking */
  if (timer_id->pending)
    {
    timer_id->terminate = SD_TRUE;  /* can't delete at this moment, timer is in callback */
    GLBSEM_LOG_FLOW2 ("GLBSEM gs_timer_remove: thread 0x%08lx stopping timer tm=0x%X (terminate=TRUE)",
                      GET_THREAD_ID(), timer_id);
    }
  else
    {
    ret = list_unlink (&gs_timer_lst, timer_id);
    if (ret == SD_SUCCESS)
      {
      if (timer_id->event)
        gs_signal_event_sem (timer_id->event);
      chk_free (timer_id);
      GLBSEM_LOG_FLOW2 ("GLBSEM gs_timer_remove: thread 0x%08lx stopped timer tm=0x%X",
                        GET_THREAD_ID(), timer_id);
      }
    else
      GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_remove error: timer_id=%X not found in list",
    		      timer_id);
    }
  gs_mutex_free (&gs_timer_lst_mutex);
  
  }
#endif /* defined (_WIN32_timers) */

  return (ret);
}

/************************************************************************/
/*			gs_timer_cleanup				*/
/*----------------------------------------------------------------------*/
/* Call this function before exiting application to stop timer thread	*/
/* and release all resources used by user timers.			*/
/* NOTE: the gs_timer_init should not be called from other thread	*/
/*       while this functions is executing.				*/
/* Parameters:								*/
/*	timeout		max time in milliseconds to wait for thread to	*/
/*			terminate.					*/
/*			  -1L wait function will not return until thread*/
/*			      terminates.				*/
/*			   0L wait function will return immediately.	*/
/*			On UNIX the timeout parameter is ignored, wait	*/
/*			function will return when the thread terminates.*/
/* Return:								*/
/* 	SD_SUCCESS if function successful				*/
/*	SD_FAILURE otherwise						*/
/************************************************************************/
ST_RET gs_timer_cleanup (ST_LONG timeout)
{
ST_RET     ret = SD_SUCCESS;
GS_TIMER  *timer_ptr;

  if (!gs_timer_initialized)
    return (SD_SUCCESS);

  /* prevent new timers being added */
  gs_timer_initialized = SD_FALSE;

#if !defined (_WIN32_timers)
  {
  /* UNIX & Windows */

  /* wake all worker threads and wait for termination */
  ret = gs_timer_stop_worker (SD_TRUE, timeout);

  /* If there is problem in joining the worker threads then do not 	*/
  /* release the event or mutex semaphores.				*/

  if (ret == SD_SUCCESS)
    {
    gs_free_event_sem (gs_timer_worker_event_sem);
    gs_mutex_destroy (&gs_timer_worker_mutex);
    }

  if (gs_timer_worker_list)
    {
    chk_free (gs_timer_worker_list);
    gs_timer_worker_list = NULL;
    }

  /* stop timer threads */
  gs_timer_thread_stop = SD_TRUE;		/* tell threads to exit */
  gs_signal_event_sem (gs_timer_event_sem);	/* wake the main thread	*/

  /* now take care of the main timer thread */
  ret = gs_wait_thread (gs_timer_thread_handle, gs_timer_thread_id, timeout);

  /* If there is problem in joining the thread then do not release the	*/
  /* event or mutex semaphores.						*/

  if ( ret == SD_SUCCESS)
    /* release timer event sem */
    gs_free_event_sem (gs_timer_event_sem);
  }
#endif /* !defined (_WIN32_timers) */

  /* release the timer list */
  gs_mutex_get (&gs_timer_lst_mutex);
  while (gs_timer_lst)
    {
    timer_ptr = list_get_first (&gs_timer_lst);
    chk_free (timer_ptr);
    }  
  gs_mutex_free (&gs_timer_lst_mutex);

  /* release timer table mutex */
  if ( ret == SD_SUCCESS)
    gs_mutex_destroy (&gs_timer_lst_mutex);

  return (ret);
}

#if !defined (_WIN32_timers)
/************************************************************************/
/*			gs_timer_thread					*/
/*----------------------------------------------------------------------*/
/* This thread check for expired timers and set event for worker threads*/
/* to process the timer expiration.					*/
/* The gs_timer_resolution will be used as sleep time in the thread.	*/
/* The resolution should be the smallest timer interval in user's	*/
/* application and can be changed while the application is running.	*/
/* Parameters:								*/
/*	arg	       not used						*/
/* Return:								*/
/*	none								*/
/************************************************************************/
static ST_THREAD_RET ST_THREAD_CALL_CONV gs_timer_thread (ST_THREAD_ARG arg)
{
int       ret = 0;
GS_TIMER *timer_ptr;
ST_UINT   signaled_cnt;
ST_UINT   pending_cnt;
ST_UINT   pending_checks = 0;
ST_UINT   idle_checks = 0;
ST_DOUBLE curr_time;

/* this define will limit the number of times we start/stop worker threads	*/
#define   GS_MAX_CHECKS		5	/* if we encounter all worker threads	*/
					/* busy/idle for this number of times	*/
					/* then we will start/stop worker thread*/

#if defined (SEV_SUPPORT)
    sevStartThisThreadWatch (0, "Main Timer Thread", __FILE__, __LINE__);
#endif

  GLBSEM_LOG_FLOW1 ("GLBSEM timer thread threadId=0x%08lx started", GET_THREAD_ID());

#if defined (_WIN32)
  gs_set_thread_name (GET_THREAD_ID (), "MainTimerThread");
#endif

  while (!gs_timer_thread_stop)
    {
    ret = gs_wait_event_sem (gs_timer_event_sem, gs_timer_resolution);
    if (ret == SD_SUCCESS)
      {
      /* event was posted from gs_timer_cleanup, exit the thread */
      break;
      }
    else if (ret != SD_TIMEOUT)
      {
      GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_thread error: gs_wait_event_sem failed ret=%d", ret);
      break;			/* exit thread, something is wrong */
      }

    /* process expired timers */
    gs_mutex_get (&gs_timer_lst_mutex);
    timer_ptr = gs_timer_lst;
    pending_cnt = 0;
    signaled_cnt = 0;
    curr_time = sGetMsTime();
    while (timer_ptr && !gs_timer_thread_stop)
      {
      if (curr_time >= timer_ptr->expiration)
        {
        if (timer_ptr->pending)
          ++pending_cnt;	/* timer processing already pending on this timer	*/
	else
	  {
	  ++signaled_cnt;
          gs_signal_event_sem (gs_timer_worker_event_sem);  /* wake up worker thread	*/
	  sMsSleep(0);	  /* this will allow worker thread to start processing timer	*/
	  }
        }
      timer_ptr = list_get_next (gs_timer_lst, timer_ptr);
      }  /* end while loop */
    gs_mutex_free (&gs_timer_lst_mutex);

    /* check if need for timers processing and number of worker threads is balanced	*/
    if (pending_cnt + signaled_cnt > gs_timer_worker_cnt)
      {
      ++pending_checks;		/* getting busy	*/
      idle_checks = 0;
      }
    else if (pending_cnt + signaled_cnt < gs_timer_worker_cnt)
      {
      pending_checks = 0;
      ++idle_checks;		/* getting idle */
      }
    else
      {
      pending_checks = 0;	/* just right	*/
      idle_checks = 0;
      }

    /* check if we need to adjust the number of worker threads	*/
    if (pending_checks >= GS_MAX_CHECKS && gs_timer_worker_cnt < gs_timer_worker_max)
      {
      /* all threads are busy for a while processing callbacks, start new worker thread */
      ret = gs_timer_start_worker ();
      if (ret == SD_SUCCESS)
        {
        pending_checks = 0;
        idle_checks = 0;
	}
      }
    else if (idle_checks >= GS_MAX_CHECKS && gs_timer_worker_cnt > gs_timer_worker_min)
      {
      /* threads are not so busy processing callbacks, stop one worker thread */
      ret = gs_timer_stop_worker (SD_FALSE, gs_timer_resolution/2);
      if (ret == SD_SUCCESS)
        {
        pending_checks = 0;
        idle_checks = 0;
	}
      }
    } /* end while loop */

  gs_timer_thread_stop = SD_TRUE;   /* thread exiting, set the flag in	*/
				    /* case we need to check thread status */

  GLBSEM_LOG_FLOW1 ("GLBSEM timer thread threadId=0x%08lx exiting", GET_THREAD_ID());
  
  #if defined (SEV_SUPPORT)
    sevStopThisThreadWatch ();
#endif

  return ST_THREAD_RET_VAL;
}

/************************************************************************/
/*			gs_timer_start_worker				*/
/*----------------------------------------------------------------------*/
/* This function will start timer worker thread.			*/
/* Parameters:								*/
/*	none								*/
/* Return:								*/
/* 	SD_SUCCESS if function successful				*/
/*	SD_FAILURE otherwise						*/
/************************************************************************/
static ST_RET gs_timer_start_worker (ST_VOID)
{
ST_RET           ret = SD_FAILURE;
GS_TIMER_WORKER *worker = NULL;
ST_UINT          i;

  gs_mutex_get (&gs_timer_worker_mutex);

  /* find unused entry */
  for (i=0; i<gs_timer_worker_max; ++i)
    {
    if (!gs_timer_worker_list[i].used)
      {
      worker = &gs_timer_worker_list[i];
      worker->used = SD_TRUE;
      worker->stop = SD_FALSE;
      break;
      }
    }
  if (worker)
    {
    ret = gs_start_thread (gs_timer_worker_thread, (ST_THREAD_ARG) worker, 
                           &worker->thread_handle, &worker->thread_id);
    if (ret == SD_SUCCESS)
      {
      ++gs_timer_worker_cnt;
      GLBSEM_LOG_FLOW1 ("GLBSEM: starting timer worker thread threadId=0x%08lx",
                        worker->thread_id);
      }
    else
      {
      worker->used = SD_FALSE;
      GLBSEM_LOG_ERR1 ("GLBSEM: start of timer worker thread failed ret=%d", ret);
      }
    }

  gs_mutex_free (&gs_timer_worker_mutex);

  return (ret);
}

/************************************************************************/
/*			gs_timer_stop_worker				*/
/*----------------------------------------------------------------------*/
/* This function will stop timer worker thread(s).			*/
/* Parameters:								*/
/*	all	SD_TRUE will stop all worker threads, stop one otherwise*/
/*	timeout	how long we want to wait for each thread to exit	*/
/* Return:								*/
/* 	SD_SUCCESS if function successful				*/
/*	SD_FAILURE otherwise						*/
/************************************************************************/
static ST_RET gs_timer_stop_worker (ST_BOOLEAN all, ST_LONG timeout)
{
ST_RET  ret = SD_SUCCESS;
ST_RET  ret2;
ST_UINT i;

  gs_mutex_get (&gs_timer_worker_mutex);

  /* set the stop flag and wake one/all worker threads */
  for (i=0; i<gs_timer_worker_max; ++i)
    {
    if (gs_timer_worker_list[i].used)
      {
      gs_timer_worker_list[i].stop = SD_TRUE;
      gs_signal_event_sem (gs_timer_worker_event_sem);  /* wake worker threads	*/
      if (!all)
        break;
      }
    }

  /* wait until worker thread(s) exit */
  for (i=0; i<gs_timer_worker_max; ++i)
    {
    if (gs_timer_worker_list[i].used && gs_timer_worker_list[i].stop)
      {
      /* it is possible that the thread will take longer than timeout to terminate, in	*/
      /* that case this code will succeed on next call to gs_timer_stop_worker		*/
      ret2 = gs_wait_thread (gs_timer_worker_list[i].thread_handle, gs_timer_worker_list[i].thread_id, timeout);
      if (ret2 == SD_SUCCESS)
        {
        gs_timer_worker_list[i].used = SD_FALSE;
        gs_timer_worker_list[i].stop = SD_FALSE;
        --gs_timer_worker_cnt;
        GLBSEM_LOG_FLOW2 ("GLBSEM: stopped timer worker thread threadId=0x%08lx (%d worker threads left)",
                        gs_timer_worker_list[i].thread_id, gs_timer_worker_cnt);
	}
      ret |= ret2;
      }
    }

  gs_mutex_free (&gs_timer_worker_mutex);

  return (ret);
}

/************************************************************************/
/*			gs_timer_worker_thread				*/
/*----------------------------------------------------------------------*/
/* This thread will wait for a gs_timer_worker_event_sem and if woken up*/
/* it will call user callback_fun function to process pending timer.	*/
/* The gs_timer_resolution will be used as sleep time in the thread.	*/
/* The resolution should be the smallest timer interval in user's	*/
/* application and can be changed while the application is running.	*/
/* Parameters:								*/
/*	arg	       not used						*/
/* Return:								*/
/*	none								*/
/************************************************************************/
static ST_THREAD_RET ST_THREAD_CALL_CONV gs_timer_worker_thread (ST_THREAD_ARG arg)
{
GS_TIMER_WORKER *worker = (GS_TIMER_WORKER *) arg;
int         ret = 0;
int         ret2 = 0;
ST_DOUBLE   time_start, time_end;
GS_TIMER   *timer_ptr = NULL;
ST_BOOLEAN  timer_found = SD_FALSE;
ST_DOUBLE   curr_time;

#if defined (SEV_SUPPORT)
  sevStartThisThreadWatch (0, "Worker Timer Thread", __FILE__, __LINE__);
#endif

  GLBSEM_LOG_FLOW1 ("GLBSEM timer worker thread threadId=0x%08lx started", GET_THREAD_ID());

#if defined (_WIN32)
  gs_set_thread_name (GET_THREAD_ID (), "TimerWorkerThread");
#endif

  while (!(worker->stop || gs_timer_thread_stop))
    {
    if (!timer_found)
      /* go to sleep for specific period of time */
      ret = gs_wait_event_sem (gs_timer_worker_event_sem, gs_timer_resolution);
    else
      ret = SD_SUCCESS;

    if (ret == SD_SUCCESS)
      {
      /* event signaled to process timer or to terminate		*/
      if (worker->stop || gs_timer_thread_stop)
        break;				/* thread needs to terminate	*/

      /* find timer that need servicing */
      gs_mutex_get (&gs_timer_lst_mutex);
      curr_time = sGetMsTime();
      timer_found = SD_FALSE;
      /* try process timer that is back on the list since it waited longest */
      if (timer_ptr)
        {
	/* if this timer is still valid find next timer in list */
        if (list_find_node (gs_timer_lst, timer_ptr) == SD_SUCCESS)
          timer_ptr = list_get_next (gs_timer_lst, timer_ptr);
	else
	  timer_ptr = NULL;
	}
      while (timer_ptr)
        {
        if (!timer_ptr->pending &&  curr_time >= timer_ptr->expiration)
          {
          timer_found = SD_TRUE;
          /* set next expiration time */
          timer_ptr->expiration = sGetMsTime() + (ST_DOUBLE) timer_ptr->interval;
	  timer_ptr->pending = SD_TRUE;
	  break;
          }
        timer_ptr = list_get_next (gs_timer_lst, timer_ptr);
        }

      /* if timer not found, process timer that is at the beginning of list */
      if (!timer_found)
        {
        timer_ptr = gs_timer_lst;
        while (timer_ptr)
          {
          if (!timer_ptr->pending && curr_time >= timer_ptr->expiration)
            {
            timer_found = SD_TRUE;
            /* set next expiration time */
            timer_ptr->expiration = sGetMsTime() + (ST_DOUBLE) timer_ptr->interval;
            timer_ptr->pending = SD_TRUE;
	    break;
            }
          timer_ptr = list_get_next (gs_timer_lst, timer_ptr);
          }
        }
      gs_mutex_free (&gs_timer_lst_mutex);

      if (timer_found)
        {
	/* do not lock the gs_timer_lst_mutex while in callback function !	*/
        time_start = sGetMsTime ();	/* to compute callback_fun proc time	*/
        if (!timer_ptr->terminate)
          (*timer_ptr->callback_fun)(timer_ptr->arg);
        time_end = sGetMsTime ();

        gs_mutex_get (&gs_timer_lst_mutex);
        timer_ptr->pending = SD_FALSE;
        if (timer_ptr->type == SD_TIMER_ONESHOT)
          /* delete the timer from the list */
          ret2 = gs_timer_remove (timer_ptr);
	else
	  {
	  /* periodic timer */
          if (timer_ptr->terminate)
	    {
	    /* timer terminate flag was set while it was in callback function	*/
            ret2 = gs_timer_remove (timer_ptr);
	    timer_ptr = NULL;			/* so we do not use it above	*/ 
	    }
	  else if ( (ST_ULONG) (time_end - time_start) >= timer_ptr->interval)
            timer_ptr->expiration = sGetMsTime() + (ST_DOUBLE) timer_ptr->interval;
            /* start new time count since callback took so long */
          }
        gs_mutex_free (&gs_timer_lst_mutex);
        }
      }
    else if (ret != SD_TIMEOUT)
      {
      GLBSEM_LOG_ERR1 ("GLBSEM gs_timer_worker_thread error: gs_wait_event_sem failed ret=%d", ret);
      break;			/* exit thread, something is wrong */
      }
    } /* end while loop */

  GLBSEM_LOG_FLOW1 ("GLBSEM timer worker thread threadId=0x%08lx exiting", GET_THREAD_ID());
  
#if defined (SEV_SUPPORT)
  sevStopThisThreadWatch ();
#endif

  return ST_THREAD_RET_VAL;
}

#endif /* !defined (_WIN32_timers) */
