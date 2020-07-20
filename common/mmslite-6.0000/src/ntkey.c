/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1995 All Rights Reserved 				*/
/*									*/
/* MODULE NAME : ntkey.c 						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains function key handling functions, and	*/
/*	other misc. key handling functions.   This module is necessary	*/
/* 	when the application links to the C MTDLL library on NT.  The 	*/
/*	reason for this entire module is that the C MT DLL version of	*/
/* 	function 'kbhit' causes memory violations.  			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	static ST_INT nlfn ()						*/
/*	ST_VOID   bad_key ()						*/
/*	ST_INT  check_key ()						*/
/*	ST_VOID   do_fun ()						*/
/*	ST_VOID   fun_null ()						*/
/*	ST_VOID   key_err (msg)						*/
/*	ST_VOID   wait_msg (str)					*/
/*	ST_VOID   err_msg (str)						*/
/*	ST_VOID   wait_key ()						*/
/*	ST_VOID   fun_exe (key)						*/
/*	ST_VOID   flush_keys ()						*/
/*	ST_VOID   list_bytes (ptr,len)					*/
/*	ST_VOID   list_words (ptr,len)					*/
/*	ST_VOID   list_ascii (ptr,len)					*/
/*	ST_BOOLEAN ask (question, default_ans)				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/27/07  JRB    08     Del windows.h, etc (already in sysincs.h).	*/
/* 05/15/02  nav    07     Merge iccp/sample with util/otheros          */
/* 06/25/97  EJV    06     Added define NO_F_KEY to allow function key	*/
/*                         to be 1,2,3,... Telnet do not like Fn keys.	*/
/* 05/14/97  EJV    05     Added back simulate_press_key function	*/
/* 05/12/97  IKE    04     Changed ST_INT to ST_RET in check_key	*/
/* 01/31/97  NAV    03     Allow input of number rather than funkey	*/
/* 03/13/96  DSF    02     Remove CPU_AWARE ifdef's			*/
/* 12/07/95  DSF    01     Initial release				*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#include "fkeydefs.h"
#include "gvaldefs.h"
#include "scrndefs.h"

#ifdef EXPOSE_LOGGING_FUN_PTRS
/************************************************************************/
/* initialize the user utility function pointer table			*/
/************************************************************************/

ST_VOID (*usr_util_fun [USR_UTIL_FUNS]) () =
  {
  wait_debug_log,			/* 00 WAIT_DEBUG_LOG		*/
  log_hex_bytes				/* 01 LOG_HEX_ST_CHARS		*/
  };
#endif

ST_VOID (*funct_1) (ST_VOID);
ST_VOID (*funct_2) (ST_VOID);
ST_VOID (*funct_3) (ST_VOID);
ST_VOID (*funct_4) (ST_VOID);
ST_VOID (*funct_5) (ST_VOID);
ST_VOID (*funct_6) (ST_VOID);
ST_VOID (*funct_7) (ST_VOID);
ST_VOID (*funct_8) (ST_VOID);
ST_VOID (*funct_9) (ST_VOID);
ST_VOID (*funct_10) (ST_VOID);
ST_VOID (*funct_menu) ();

/* We always need a Keyboard handle					*/


static 	HANDLE	hStdin;

/************************************************************************/
/* Keyboard Event and Mutex semaphores					*/
/************************************************************************/
HANDLE 	kbdEvent;
HANDLE	kbdBufferMtx;

/* Keyboard buffer and control						*/

#define NUM_BUFFERED_KEYS 200
static char keyboard_buffer[NUM_BUFFERED_KEYS];
static int curr_put = 0;
static int curr_get = 0;
static int next_put;
static int next_get;

ST_VOID kbdThread (ST_VOID *arg);

ST_VOID fun_exe (ST_INT x);

/************************************************************************/
/* A table of function pointers is used to select the action		*/
/* performed when a function key is pressed. The fun_exe () routine	*/
/* selects the function based on the key code. The fun_null can be	*/
/* used to set all fuunction key pointers to the "Illegal Key"          */
/* display function.							*/
/************************************************************************/

#if defined(NO_F_KEY)
#define F1  0x31	   /* 1 */
#define F2  0x32	   /* 2 */
#define F3  0x33	   /* 3 */
#define F4  0x34	   /* 4 */
#define F5  0x35	   /* 5 */
#define F6  0x36	   /* 6 */
#define F7  0x37	   /* 7 */
#define F8  0x38	   /* 8 */
#define F9  0x39	   /* 9 */
#define F10 0x30	   /* 10 */
#else
#define F1  0x3B	   /* F1 */
#define F2  0x3C	   /* F2 */
#define F3  0x3D	   /* F3 */
#define F4  0x3E	   /* F4 */
#define F5  0x3F	   /* F5 */
#define F6  0x40	   /* F6 */
#define F7  0x41	   /* F7 */
#define F8  0x42	   /* F8 */
#define F9  0x43	   /* F9 */
#define F10 0x44	   /* F10 */
#endif

#define MENU_KEY	0x1B	/* the escape key	*/

/************************************************************************/
/************************************************************************/
/*			nlfn						*/
/* function to just return.  Used when need to set service routine to	*/
/* a dummy function							*/
/************************************************************************/

static ST_INT nlfn (ST_VOID)
  {
  return (0);
  }
ST_RET (*servefun)(ST_VOID) = nlfn;	    /* pointer to function to be called */

ST_VOID (*key_wait_service_fun)(ST_VOID);

/************************************************************************/
/************************************************************************/
/*			term_init					*/
/************************************************************************/


ST_RET term_init (ST_VOID)
  {
DWORD	fMode;

  hStdin = GetStdHandle (STD_INPUT_HANDLE);
  GetConsoleMode (hStdin, &fMode);
  fMode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  SetConsoleMode (hStdin, fMode);

  kbdEvent = CreateEvent (NULL, SD_TRUE, SD_FALSE, NULL);
  if (kbdEvent == NULL)
    {
    printf ("ERROR: CreateEvent (), rc = %d", GetLastError ());
    return (SD_FAILURE);
    }
	
  kbdBufferMtx = CreateMutex (NULL, SD_FALSE, NULL);
  if (kbdBufferMtx == NULL)
    {
    printf ("ERROR: CreateMutex (), rc = %d", GetLastError ());
    return (SD_FAILURE);
    }
	
  _beginthread (kbdThread, 0, 0);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			term_rest					*/
/************************************************************************/

ST_RET term_rest (ST_VOID)
  {

  return (SD_SUCCESS);
  }

/************************************************************************/
/* 	                    keyboard_thread				*/
/************************************************************************/
/* This thread loops forever, retrieving kbd characters and buffering	*/
/* it.									*/

ST_VOID kbdThread (ST_VOID *arg)
  {
ST_CHAR achar;
INPUT_RECORD ir;
KEY_EVENT_RECORD key;
DWORD irsRead;
DWORD rc;

  while (SD_TRUE)
    {
    if (ReadConsoleInput (hStdin, &ir, 1, &irsRead) == SD_FALSE)
      continue;

    if (irsRead == 0)
      continue;

    switch (ir.EventType)
      {
      case KEY_EVENT:
        key = ir.Event.KeyEvent;
	if (key.bKeyDown == SD_FALSE)
	  continue;

#if defined(NO_F_KEY)
	if (key.wVirtualKeyCode >= F10 && key.wVirtualKeyCode <= F9)
	  achar = (ST_CHAR) key.wVirtualKeyCode;
	else if (key.wVirtualKeyCode >= VK_NUMPAD0 && key.wVirtualKeyCode <= VK_NUMPAD9)
	  achar = key.wVirtualKeyCode - VK_NUMPAD0 + F10;
#else
	if (key.wVirtualKeyCode >= VK_F1 && key.wVirtualKeyCode <= VK_F10)
	  achar = key.wVirtualKeyCode - VK_F1 + F1;
#endif
	else
	  achar = key.uChar.AsciiChar;
      break;

      default:
        continue;
      break;
      }
    
  /* We got a character, get control of the keyboard buffer and save it	*/
  /* if there is room in the buffer. If not,just drop it		*/

    next_put = curr_put + 1;
    if (next_put == NUM_BUFFERED_KEYS) 
      next_put = 0;

    if (next_put == curr_get)
      continue;

  /* We got a character, get control of the keyboard buffer and save it	*/
    rc = WaitForSingleObject (kbdBufferMtx, INFINITE);
    if (rc == 0xFFFFFFFF)
      {
      printf ("ERROR: Request Mutex for Kbd Buffer, rc = %d", GetLastError ());
      return;
      }

    keyboard_buffer[curr_put] = achar;
    curr_put = next_put;

  /* Ok, we can release control of the kbd buffer, then post the kbd	*/
  /* event semaphore to let others know that something is up		*/
    if (ReleaseMutex (kbdBufferMtx) == SD_FALSE)
      printf ("ERROR: Release Mutex for Kbd Buffer, rc = %d", GetLastError ());

    SetEvent (kbdEvent);
    }
  }

/************************************************************************/
/*			getkch						*/
/************************************************************************/

ST_CHAR  getkch (ST_VOID)
  {
DWORD	rc;
char	achar;

/* first just check to see if there are any characters waiting for us	*/
  if (curr_get == curr_put)
    return (EOF);

/* OK, characters are available, return the oldest one			*/
/* Get control of the keyboard buffer					*/
  rc = WaitForSingleObject (kbdBufferMtx, INFINITE);
  if (rc == 0xFFFFFFFF)
    {
    printf ("ERROR: Request Mutex for Kbd Buffer, rc = %d", GetLastError ());
    return (EOF);
    }

/* retrieve the oldest character					*/
  achar = keyboard_buffer[curr_get];

/* take care of the get position					*/
  curr_get++;
  if (curr_get == NUM_BUFFERED_KEYS) 
    curr_get = 0;

/* If there are no more characters, go ahead and reset the event sem	*/  
  if (curr_get == curr_put)
    {
    if (ResetEvent (kbdEvent) == SD_FALSE)
      printf ("ERROR: Reset Kbd Event, rc = %d", GetLastError ());
    }

/* Release control of the keyboard buffer				*/
  if (ReleaseMutex (kbdBufferMtx) == SD_FALSE)
    printf ("ERROR: Release Mutex for Kbd Buffer, rc = %d", GetLastError ());

  return (achar);
  }

/************************************************************************/
/*                        do_key_wait_service				*/
/************************************************************************/


ST_VOID do_key_wait_service (void)
  {
DWORD	rc;


/* If there is a character already available, just return		*/
  if (curr_get != curr_put)
    return;

/* If the 'servefun' has been set, we need to call it.			*/
/* If not, we can wait on the keyboard semaphore			*/
  if (servefun == nlfn)
    {		/* We can just block on the keyboard semaphore		*/
    rc = WaitForSingleObject (kbdEvent, INFINITE);
    if (rc == 0xFFFFFFFF)
      printf ("ERROR: WaitForSingleObject failed on kbdEvent, rc = %d", GetLastError ());
    }
  else		
    {		/* We need to do the poll thing, and burn the CPU	*/
    while (curr_get == curr_put)
      (*servefun) ();
    }
  }

/************************************************************************/
/*			flush_keys					*/
/* Function to flush the keyboard character buffer			*/
/************************************************************************/

ST_VOID flush_keys (ST_VOID)
  {
DWORD	rc;

  rc = WaitForSingleObject (kbdBufferMtx, INFINITE);
  if (rc == 0xFFFFFFFF)
    {
    printf ("ERROR: Request Mutex for Kbd Buffer (flush_keys), rc = %d", GetLastError ());
    return;
    }
  curr_get = curr_put = 0;

  if (ResetEvent (kbdEvent) == SD_FALSE)
    printf ("ERROR: Reset Kbd Event, rc = %d", GetLastError ());

  if (ReleaseMutex (kbdBufferMtx) == SD_FALSE)
    printf ("ERROR: Release Mutex for Kbd Buffer (getkch), rc = %d", GetLastError ());
  }

/************************************************************************/
/*			kbhit						*/
/************************************************************************/

ST_INT	kbhit (ST_VOID)
  {

  if (curr_get != curr_put)
    return (SD_TRUE);

  return (SD_FALSE);
  }

/************************************************************************/
/*			wait_debug_log					*/
/************************************************************************/

ST_VOID wait_debug_log (FILE *fptr, ST_CHAR *str)
  {
  flush_keys ();
  fprintf (fptr,"\n %s",str);
  fflush (fptr);
  if (fptr == stdout)
    do_key_wait_service ();

  flush_keys ();
  }

/************************************************************************/
/*			wait_msg					*/
/* Function to display a message and wait for a key hit 		*/
/************************************************************************/


ST_VOID wait_msg (ST_CHAR *str)
  {
  flush_keys ();
  printf ("\n %s",str);
  do_key_wait_service ();
  flush_keys ();
  }

/****************************************************************************/
/*                           log_hex_bytes				    */
/* This function provides a hex dump facility.				    */
/****************************************************************************/

#define cvt_nibble(a)	((a) > 9 ? (a) + 'A' - 10 : (a) + '0')
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
    curlen = min (len,16); 		/* # char's in current line	*/
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
      hex = cvt_nibble (hex);
      debug_buf[hex_index++] = hex;
      hex = ch & 0x0f;
      hex = cvt_nibble (hex);
      debug_buf[hex_index++] = hex;

      debug_buf[hex_index++] = ' ';

      if (isprint (ch))			/* print ASCII portion		*/
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

/************************************************************************/
/* Functions unused by the MMS provider.                                */
/************************************************************************/

#ifndef UTIL_LIB

/************************************************************************/
/*			check_key					*/
/* Check for key pressed, if not just return.				*/
/* If so, check to see if function key, if not: illegal key.		*/
/* If function key, execute the function.				*/
/************************************************************************/

ST_RET check_key (ST_VOID)
  {
ST_INT key;			/* key gets character code		     */

  if (kbhit ())			/* Key Pressed ?			*/
    {
    key = (ST_INT) getkch ();
    if (key == MENU_KEY)
      {
      if (funct_menu)
        (*funct_menu) ();
      return 0;
      }
#if defined(NO_F_KEY)
    else if (key < F10 || key > F9 )
#else
    else if (key > F10 || key < F1 )
#endif
      return (0);
    else
      {
      if ( key == F10 )
        return ( 1 );
      else
        {
	fun_exe ( key );
	return (0);
	}
      }
    }
  else
    return (0);
  }

/************************************************************************/
/*			bad_key 					*/
/* Illegal Key routine, need to Beep Here				*/
/************************************************************************/

ST_VOID bad_key (ST_VOID)
  {
  flush_keys ();
  }


/************************************************************************/
/*			fun_null					*/
/* Function to make all function keys illegal				*/
/************************************************************************/

ST_VOID fun_null (ST_VOID)
  {
  funct_1  = bad_key;
  funct_2  = bad_key;
  funct_3  = bad_key;
  funct_4  = bad_key;
  funct_5  = bad_key;
  funct_6  = bad_key;
  funct_7  = bad_key;
  funct_8  = bad_key;
  funct_9  = bad_key;
  funct_10 = bad_key;
  }

/************************************************************************/
/*			key_err 					*/
/* Message display and wait function, used to display error messages	*/
/************************************************************************/

ST_VOID key_err (ST_CHAR *msg)
  {

  CLEARSCR;
  printf ("\n\n\n\n\n");
  printf ("%s",msg);
  wait_key ();
  }

/************************************************************************/
/*			wait_key					*/
/************************************************************************/

ST_VOID wait_key (ST_VOID)
  {
  do_key_wait_service ();
  flush_keys ();
  }

/************************************************************************/
/*			fun_exe 					*/
/* Function Key Function Select Routine, Select the key function by	*/
/* using the function pointer						*/
/************************************************************************/

ST_VOID fun_exe (ST_INT key)/* key code is passed to this routine	     */
  {			/* This must be the second ST_CHAR key sequence */
  switch (key)
     {
     case F1:		/* F1 */
       (*funct_1) ();
     break;

     case F2:		/* F2 */
       (*funct_2) ();
     break;

     case F3:		/* F3 */
       (*funct_3) ();
     break;

     case F4:		/* F4 */
       (*funct_4) ();
     break;

     case F5:		/* F5 */
       (*funct_5) ();
     break;

     case F6:		/* F6 */
       (*funct_6) ();
     break;

     case F7:		/* F7 */
       (*funct_7) ();
     break;

     case F8:		/* F8 */
       (*funct_8) ();
     break;

     case F9:		/* F9 */
	(*funct_9) ();
     break;

     case F10:		/* F10 */
       (*funct_10) ();
     break;

     default:		 /* Not a Function Key */
       bad_key ();
     break;
     }
  }

/************************************************************************/
/*			list_bytes					*/
/************************************************************************/

ST_VOID list_bytes (ST_UCHAR *ptr, ST_INT len)
  {
  CLEAREOL;
  while (len)
    {
    printf (" %02X",*(ptr++) & 0xFF);
    len--;
    }
  }


/************************************************************************/
/*			list_ascii					*/
/************************************************************************/

ST_VOID list_ascii (ST_CHAR *ptr, ST_INT len)
  {
  CLEAREOL;
  while (len)
    {
    printf ("%c",*(ptr++) );
    len--;
    }
  }


/************************************************************************/
/*			ask						*/
/* Ask a yes/no question and return the answer. 			*/
/************************************************************************/

ST_BOOLEAN ask (ST_CHAR *question, ST_BOOLEAN default_ans)
  {
ST_CHAR  ans [20];
ST_BOOLEAN ret_ans;

  printf ("%s", question);
  flush_keys ();
  ans [0] = '?';
  strget (ans);
  if (ans [0] == 'n' || ans [0] == 'N')
    ret_ans = SD_FALSE;
  else
    {
    if (ans [0] == 'y' || ans [0] == 'Y')
      ret_ans = SD_TRUE;
    else
      ret_ans = default_ans;
    }

  return (ret_ans);
  }


/************************************************************************/
/*				get_a_char				*/
/************************************************************************/

ST_CHAR get_a_char (ST_VOID)
  {
  do_key_wait_service ();		/* Wait for key or other event	*/
  return (getkch ());
  }

/************************************************************************/
/*			err_msg 					*/
/************************************************************************/


ST_VOID err_msg (ST_CHAR *str)
  {
  wait_msg (str);
  }

/************************************************************************/
/*			list_words					*/
/************************************************************************/

ST_VOID list_words (ST_UINT *ptr, ST_INT len)
  {
  CLEAREOL;
  while (len)
    {
    printf (" %04X",*(ptr++));
    len--;
    }
  }

/************************************************************************/
/*                      simulate_press_key                              */
/*----------------------------------------------------------------------*/
/* This function will simulate a key being pressed by user.		*/
/************************************************************************/
ST_VOID simulate_press_key (ST_CHAR achar)
{
DWORD rc;

  /* Get control of the keyboard buffer and save it. */

  next_put = curr_put + 1;
  if (next_put == NUM_BUFFERED_KEYS) 
    next_put = 0;

  if (next_put == curr_get)
    return;

  /* We got a character, get control of the keyboard buffer and save it	*/
  rc = WaitForSingleObject (kbdBufferMtx, INFINITE);
  if (rc == 0xFFFFFFFFL)
    {
    printf ("ERROR: Request Mutex for Kbd Buffer, rc = %d", GetLastError ());
    return;
    }

  keyboard_buffer[curr_put] = achar;
  curr_put = next_put;

  /* Ok, we can release control of the kbd buffer, then post the kbd	*/
  /* event semaphore to let others know that something is up		*/
  if (ReleaseMutex (kbdBufferMtx) == FALSE)
    printf ("ERROR: Release Mutex for Kbd Buffer, rc = %d", GetLastError ());

  SetEvent (kbdEvent);
}

#endif
