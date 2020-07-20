/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1986-2001 All Rights Reserved                   	*/
/*									*/
/* MODULE NAME : fkeydefs.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains user application definitions and is not	*/
/*	directly a part of MMSEASE.  It is only used for the sample	*/
/*	application.							*/
/* NOTE:  If the ?key.c module is compiled with define UTIL_LIB only	*/
/*	  following functions from wkey.c are exposed:			*/
/*		nlfn ()							*/
/*		log_hex_bytes ()					*/
/*		wait_debug_log ()					*/
/*		wait_msg ()						*/
/*		flush_keys ()						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/18/03  JRB     05    Fix QNX #ifdef.				*/
/* 02/17/03  CRM     04    Added "defined(linux)" code. 		*/
/* 02/17/03  JRB     03	   Stop using SYSTEM_SEL define.		*/
/* 05/16/01  EJV     02    Added proto for term_init, term_rest.	*/
/* 01/22/98  NAV     01    Added funct_menu proto.			*/ 
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef FKEYDEFS_INCLUDED
#define FKEYDEFS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#if defined (_WIN32)
#include <conio.h>
#endif

/************************************************************************/
/* NOTE:								*/
/* For QUICK WIN application the call to kbhit () or getch () functions	*/
/* will install our keyboard handling hook function to intercept the	*/
/* keyboard input.							*/
/* It is not necessary to call strt_Hook function, because the kbhit	*/
/* and getch functions are checking for the hook beeing in place.	*/
/* It will be wise to call the end_Hook function to clear the Windows	*/
/* from our keyboard handling if not needed anymore.			*/
/************************************************************************/
int strt_Hook(ST_VOID);
int end_Hook(ST_VOID);

/*======================================================================
  Basic functions keyboard functions:
    kbhit ()      checks if keyboard was pressed.  It installs also our
		  keyboard handling hook function for the QUICK WIN apps.
    fkey_kbhit () returns SD_FALSE most of the times. Only every KBHIT_PERIOD
    		  (50-th time when called) it will return the return
    		  from the call to the kbhit () function.
    		  This function may be absolete.
    getch ()      returns the ASCII value of a pressed key.  The kbhit ()
    		  function should be called first to see if the keyboard
    		  was pressed.  The function will install our keyboard
    		  handling hook function for the QUICK WIN applications,
    		  if not already installed.
    flush_keys () flushes the keyboard character buffer.
    get_a_char () will wait for the keyboard hit and return the ascii
		  code for the character.  In addition the function will
		  echo the character to screen.
======================================================================*/
#if !defined (_WIN32)
#if defined(__VMS)
/* VMS uses a short return type, and is prototyped in a lot of places */
ST_INT   kbhit      (ST_VOID);
ST_INT   getch      (ST_VOID);
#else
int  kbhit      (ST_VOID);
int  getch      (ST_VOID);
#endif
#else
int  nt_kbhit (ST_VOID);
#define kbhit	nt_kbhit
#endif

ST_RET fkey_kbhit (ST_VOID);
ST_VOID  flush_keys (ST_VOID);
ST_CHAR  get_a_char (ST_VOID);

#define KBHIT_PERIOD	50
#define KBHIT() fkey_kbhit ()


/*=======================================================================
  Formatting output to stdout (or FILE *) functions: 
    log_hex_bytes () outputs formatted hex data to a dest (may be stdout).
    		  It will format each line (16 hex numbers) of the output
    		  as follow:
		  "\noffs  hh hh hh hh hh hh hh hh  hh hh hh hh hh hh hh hh   *tttttttttttttttt*"
    list_bytes () outputs formatted bytes to stdout. It will format each
                  line (16 hex numbers) of the output as follow:
		  "\n hh hh hh hh hh hh hh hh  hh hh hh hh hh hh hh hh "
    list_ascii () outputs the ascii data to stdout. The ascii data will
                  be split on more then one line if required. The data in
                  the ptr do not have to be NULL terminated.
    list_words () outputs the formatted words to stdout.  The format is
    		  as follow:
    		  "hhhh hhhh hhhh ...."
========================================================================*/
ST_VOID log_hex_bytes (FILE *dest, ST_UCHAR *ptr, ST_INT len);
ST_VOID list_bytes    (ST_UCHAR *ptr, ST_INT len);
ST_VOID list_ascii    (ST_CHAR *ptr, ST_INT len);
ST_VOID list_words    (ST_UINT *ptr, ST_INT len);


/*=======================================================================
  Functions to display a message and wait for user to hit a key:
    wait_debug_log () outputs the str to a fptr (which may be stdout).
    		  If the output is stdout the function will wait for user
    		  to hit a key.  During waiting the function will call
    		  the servefun () to execute background processing.
    wait_msg ()   outputs the str to stdout.  The function will wait for
    		  user to hit a key.  During waiting the function will call
    		  the servefun () to execute background processing.
    wait_key ()   waits for user to hit a key.  During waiting the function
    		  will call the servefun () to execute background processing.
    err_msg ()    outputs the str to stdout.  The function will wait for
    		  user to hit a key.  Note that this func will NOT call
    		  the servefun () while waiting for the keyboard input.
    ask ()        asks a yes/no question and return the answer.
    		  SD_TRUE will be returned if the answer is 'y' or 'Y';
		  SD_FALSE will be returned if the answer is 'n' or 'N';
		  if user presses the ENTER key the default answer will
		  be returned.
    key_err ()    displays a message and returns shortly after, used to
    		  display error	messages without waiting for user to hit
    		  a key to continue.
========================================================================*/
ST_VOID  wait_debug_log (FILE *fptr, ST_CHAR *str);
ST_VOID  wait_msg	     (ST_CHAR *msg);
ST_VOID  wait_key	     (ST_VOID);
ST_VOID  err_msg 	     (ST_CHAR *str);
ST_BOOLEAN ask (ST_CHAR *question, ST_BOOLEAN default_ans);
ST_VOID  key_err  (ST_CHAR *msg);

/*========================================================================
  Pointer to USER's background function called while waiting for keyboard
  input in following functions:
	wait_key ()
	wait_msg ()
	wait_debug_log ()
	do_fun ()
  This function is initialized to a nlfn () which returns 0 if called.
========================================================================*/
extern ST_RET (*servefun) (ST_VOID);

/*========================================================================
  Pointers to USER's functions handling the actions for key F1 - F10
  This pointers will be initialized at program startup to bad_key ()
  function, which flushes the keyboard buffer.
========================================================================*/
extern ST_VOID  (*funct_1) (ST_VOID);
extern ST_VOID  (*funct_2) (ST_VOID);
extern ST_VOID  (*funct_3) (ST_VOID);
extern ST_VOID  (*funct_4) (ST_VOID);
extern ST_VOID  (*funct_5) (ST_VOID);
extern ST_VOID  (*funct_6) (ST_VOID);
extern ST_VOID  (*funct_7) (ST_VOID);
extern ST_VOID  (*funct_8) (ST_VOID);
extern ST_VOID  (*funct_9) (ST_VOID);
extern ST_VOID  (*funct_10) (ST_VOID);
extern ST_VOID  (*funct_menu) (ST_VOID);	/* may be used to refresh menu */

ST_VOID  fun_null	(ST_VOID);		/* make all function keys F1-F10 illegal*/
ST_VOID  do_fun (ST_VOID);		/* execute functions assigned to funct_n*/
				/* pointers until F10 is pressed. 	*/
ST_INT check_key	(ST_VOID);		/* Check for pressed function key and	*/
				/* execute an action for that key.	*/
				/* Returns 1 if F10 key was pressed,	*/
				/* 0 otherwise.				*/

/* functions implemented in ?key.c */
#if defined(_AIX) || defined(sun) || defined(__hpux) || defined(linux) \
    || (defined(__alpha) && !defined(__VMS)) \
    || defined(VXWORKS) || defined(__QNX__) || defined(__VMS)
ST_VOID	term_init ();
ST_VOID	term_rest ();
#elif defined (__OS2__)
int	term_init (void);
int	term_rest (void);
#endif

#if defined (_WIN32)
ST_RET	term_init (ST_VOID);
ST_INT	term_rest (ST_VOID);
#endif


#ifdef __cplusplus
}
#endif

#endif /* FKEYDEFS_INCLUDED */
