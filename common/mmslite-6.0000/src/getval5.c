/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1986-2012 All Rights Reserved             		*/
/*									*/
/* MODULE NAME : getval5.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains routines to get input from the user.	*/
/*	Each routine accepts a pointer to an appropriate variable type	*/
/*	and writes the variable if the user enters appropriate data at	*/
/*	the console.  The routines return 1 if successful, else 0.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/08/12  EJV           Revised code to eliminate Klocwork warnings. */
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 03/31/03  EJV     08    Removed _WINDOWS code.			*/
/* 03/18/03  JRB     07    Fix QNX #ifdefs.				*/
/* 02/17/03  CRM     06    Added "defined(linux)" code. 		*/
/* 02/17/03  JRB     05	   Stop using SYSTEM_SEL define.		*/
/*			   Del PSOS code.				*/
/* 06/23/02  GLB     04    Remove unreferenced nlfn                     */
/* 12/03/99  GLB     03    Cleanup 					*/
/* 04/14/99  MDE     02    Updated to new SYSTEM_SEL defines		*/
/* 06/10/98  NAV     01    input_hex  - make temp_word ST_INT16 	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"

#include "sysincs.h"

#if defined(_WIN32) || defined(MSDOS) || defined(__MSDOS__) || defined(__OS2__)
#include <math.h>
#endif

#if defined(_AIX) || defined(sun) || defined(__hpux) || defined(linux) \
    || (defined(__alpha) && !defined(__VMS))
#include <math.h>
#endif

#if defined(__VMS)
#include <math.h>
ST_INT16 suspend_till_key (ST_INT16);
#endif

#include "gvaldefs.h"
#include "scrndefs.h"
#include "fkeydefs.h"

/* Every MMS-EASE demo on a reasonable OS needs a function to return 	*/
/* the raw key.								*/
ST_CHAR getkch (ST_VOID);

#if defined(__OS2__) || defined (_WIN32)
ST_VOID do_key_wait_service (ST_VOID);
#endif

/************************************************************************/
/*				strget					*/
/* Function to get a character string.	WARNING: Input string must be	*/
/* 200 characters long!! (Otherwise, there is the possibility of the	*/
/* user writing over adjacent data space.)				*/
/************************************************************************/

ST_BOOLEAN strget (ST_CHAR *str)
  {
ST_CHAR line[STRGETSTRINGSIZE];

  SAVESCR;
  entry1(STRGETSTRINGSIZE,line);
  if (strlen (line))
    {
    strcpy (str,line);
    return (SD_TRUE);
    }
  else
    return (SD_FALSE);
  }

/************************************************************************/
/*				intget					*/
/* Function to get an integer value from the user. Works perfectly well */
/* for inputting unsigned integers also, where the input data is simply */
/* cast as a int in the call. 						*/
/************************************************************************/

ST_BOOLEAN intget (ST_INT *in)
  {
ST_INT   i,len;
ST_INT x;
ST_CHAR tmp[50];

  SAVESCR;

  entry1(50,tmp);
  len = (ST_INT) strlen (tmp);			    /* check for numeric string  */
  for (i = 0; i <= len && len && (isdigit (tmp[i]) || tmp[i] == '-'); ++i)
    ;
  if (i && (i == len))
    {
    x = (ST_INT) atoi (tmp);
    *in = x;
    return (SD_TRUE);
    }
  else
    return (SD_FALSE);
  }

/************************************************************************/
/*			  longget					*/
/* Function for inputting long integer values.	Works perfectly well	*/
/* for inputting unsigned longs also, where the input data is simply	*/
/* cast as a long in the call.						*/
/************************************************************************/

ST_BOOLEAN longget (ST_LONG *in)
  {
ST_LONG x;
ST_INT i,len;
ST_CHAR tmp[50];

  SAVESCR;

  entry1(50,tmp);
  len = (ST_INT) strlen (tmp);			    /* check for numeric string  */
  for (i = 0; i <= len && len && (isdigit (tmp[i]) || tmp[i] == '-'); ++i)
    ;
  if (i && (i == len))
    {
    x = (ST_LONG) atol (tmp);
    *in = x;
    return (SD_TRUE);
    }
  else
    return (SD_FALSE);
  }

/************************************************************************/
/*				hexget					*/
/* Function to get a positive hex integer value from the user.          */
/************************************************************************/

ST_BOOLEAN hexget (ST_INT *in)
  {
ST_INT x;
ST_INT i,len;
ST_CHAR tmp[50];
ST_INT xtoi (ST_CHAR *);

  SAVESCR;

  entry1(50,tmp);
  len = (ST_INT) strlen (tmp);			    /* check for numeric string  */
  for (i = 0; i <= len && len && (isxdigit (tmp[i])); ++i)
    ;
  if (i && (i == len))
    {
    x = (ST_INT) xtoi (tmp);
    *in = x;
    return (SD_TRUE);
    }
  else
    return (SD_FALSE);
  }


/************************************************************************/
/*				longhexget 				*/
/* Function to get a positive long hex integer value from the user.     */
/************************************************************************/

ST_BOOLEAN longhexget (ST_LONG *in)
  {
ST_INT i,len;
ST_LONG x;
ST_CHAR tmp[50];
ST_LONG xtol (ST_CHAR *);

  SAVESCR;

  entry1(50,tmp);
  len = (ST_INT) strlen (tmp);			    /* check for numeric string  */
  for (i = 0; i <= len && len && (isxdigit (tmp[i])); ++i)
    ;
  if (i && (i == len))
    {
    x = xtol (tmp);
    *in = x;
    return (SD_TRUE);
    }
  else
    return (SD_FALSE);
  }


/************************************************************************/
/*                        xtoi                                          */
/* Function to convert an ASCII hex number to an long integer           */
/************************************************************************/

ST_INT xtoi (ST_CHAR *buf)
  {
ST_INT res;
ST_INT digit;

  res = 0;
  if (strlen (buf) > 4)
    return (0);

  while (isxdigit (*buf))
    {
    digit = toupper (*buf);
    if (digit >= '0' && digit <= '9')
      digit = digit - '0';
    else
      digit = digit - 'A' + 10;

    res = 16 * res + digit;
    buf++;
    }
  
  return (res);
  }

/************************************************************************/
/*                        xtol                                          */
/* Function to convert an ASCII hex number to an long integer           */
/************************************************************************/

ST_LONG xtol (ST_CHAR *buf)
  {
ST_LONG res;
ST_INT digit;

  res = 0;
  if (strlen (buf) > 8)
    return (0);

  while (isxdigit (*buf))
    {
    digit = toupper (*buf);
    if (digit >= '0' && digit <= '9')
      digit = digit - '0';
    else
      digit = digit - 'A' + 10;

    res = 16 * res + digit;
    buf++;
    }
  
  return (res);
  }

/************************************************************************/
/*			  floatget					*/
/* Function to get a single precision float value from the user.	*/
/************************************************************************/

ST_BOOLEAN floatget (ST_FLOAT *out)
  {
ST_INT num;
ST_CHAR buf[50];
ST_FLOAT fl;

  SAVESCR;
  entry1(50,buf);
  num = sscanf (buf,"%f",&fl);
  if (num == 1)
    {
    *out = fl;
    return (SD_TRUE);
    }

  return (SD_FALSE);
  }


/************************************************************************/
/*			  doubleget					*/
/* Function to get a double precision float value from the user.	*/
/************************************************************************/

ST_BOOLEAN doubleget (ST_DOUBLE *out)
  {
ST_INT num;
ST_CHAR buf[50];
ST_DOUBLE dbl;

  SAVESCR;
  entry1(50,buf);
  num = sscanf (buf,"%lf",&dbl);
  if (num == 1)
    {
    *out = dbl;
    return (SD_TRUE);
    }

  return (SD_FALSE);
  }


/************************************************************************/
/*			  entry1					*/
/* This function allows a character string  or a fixed length to be	*/
/* read in.  It checks for backspacing (erasing of previous data, an	*/
/* escape function (abort input), and scrolling of the inputted string. */
/* This function requires the following information be passed to it	*/
/* in the  following format:						*/
/*									*/
/* entry1(row, column, maxwidth, *str_ptr)				*/
/*									*/
/*   where:								*/
/*	   row	& column -indicate the corrdinates ot the data		*/
/*			  string to be read in				*/
/*	   maxwidth	 -indicates the maximum length of the		*/
/*			  data string					*/
/*	   str_ptr	 -is the name of the character array		*/
/*			  the data string is to be placed in		*/
/************************************************************************/
/************************************************************************/

ST_VOID entry1 (ST_INT maxlen, ST_CHAR *str_ptr)
  {
ST_INT i;
ST_INT i1;
ST_BOOLEAN exit_flag;

/**************************/
#if defined(__VMS)
  maxlen -= 1;	   /* eliminates overwrite of buffers */
  i1 = 0;
  i = 0;
  exit_flag = 0;
  str_ptr[0] = '\000';
  while (exit_flag == 0)
    {
    if (suspend_till_key (1) == 1)
      {
      str_ptr[i] = (ST_CHAR) getkch ();
      str_ptr[i+1] = '\000';
      switch (str_ptr[i])
	{
	case '\000' :                    /* check for function key      */
	  getkch ();			 /* discard if found		*/
	  break;

	case '\010'  :                   /* check for a backspace       */
	case '\177'  :
	  if (i > 0)
	    {
	    printf ("%c%c%c", '\010', ' ', '\010');
	    str_ptr[--i] = '\040';
	    str_ptr[i+1] = '\000';
	    str_ptr[i] = '\000';
	    }
	    break;

	case CR      :                  /* check for a carriage return  */
	  exit_flag = 1;
	  str_ptr[i] = '\000';
	  ADD_LF;
	  break;

	case '\033'  :                  /* check for an escape sequence */
	  exit_flag = 1;			/* discard information and exit */
	  for (0; i < maxlen; i++)
	    str_ptr[i] = '\040';
	  str_ptr[maxlen] = '\000';
	  str_ptr[0] = '\000';
	  break;

	default      :			/* addition data		*/
	  printf ("%c", str_ptr[ i ]);
	  if (i >= maxlen)		/* data greater than buffer	*/
	    {				/* scroll buffer		*/
	    for (i1=0; i1 < maxlen; ++i1)
	      str_ptr[i1] = str_ptr[i1+1];
	      str_ptr[maxlen] = '\000';
	    }
	  else
	    ++i;			/* data less than buffer length */
	}				/* add in data to end of string */
      }
    (*servefun)();			/* perform function while waiting */
    }
#endif

/**************************/
#if defined(_AIX) || defined(sun) || defined(__hpux) || defined(linux) \
    || (defined(__alpha) && !defined(__VMS)) \
    || defined(VXWORKS) || defined(__QNX__) \
    || defined(_WIN32) || defined(__OS2__)
  maxlen -= 1;	   /* eliminates overwrite of buffers */
  i1 = 0;
  i = 0;
  exit_flag = 0;
  str_ptr[0] = '\000';
  while (exit_flag == 0)
    {
#if defined(__OS2__) || defined (_WIN32)
    do_key_wait_service ();		/* Wait for key or other event	*/
#endif
    if (kbhit ())			/* If a key was hit		*/
      {
      str_ptr[i] = (ST_CHAR) get_a_char ();
      str_ptr[i+1] = '\000';
      switch (str_ptr[i])
	{
	case '\000' :                    /* check for function key      */
	  getkch ();			 /* discard if found		*/
	  break;

	case '\010'  :                   /* check for a backspace       */
	case '\177'  :
	  if (i > 0)
	    {
	    printf ("%c%c%c", '\010', ' ', '\010');
	    str_ptr[--i] = '\040';
	    str_ptr[i+1] = '\000';
	    str_ptr[i] = '\000';
	    }
	    break;

	case CR      :                  /* check for a carriage return  */
	  exit_flag = 1;
	  str_ptr[i] = '\000';
	  ADD_LF;
	  break;

	case '\033'  :                  /* check for an escape sequence */
	  exit_flag = 1;			/* discard information and exit */
	  for (i = 0; i < maxlen; i++)
	    str_ptr[i] = '\040';
	  str_ptr[maxlen] = '\000';
	  str_ptr[0] = '\000';
	  break;

	default      :			/* addition data		*/
	  printf ("%c", str_ptr[ i ]);
	  if (i >= maxlen)		/* data greater than buffer	*/
	    {				/* scroll buffer		*/
	    for (i1=0; i1 < maxlen; ++i1)
	      str_ptr[i1] = str_ptr[i1+1];
	      str_ptr[maxlen] = '\000';
	    }
	  else
	    ++i;			/* data less than buffer length */
	}				/* add in data to end of string */
      }
#if !defined(__OS2__) && !defined (_WIN32)
    (*servefun)();			/* perform function while waiting */
#endif
    }
#endif


/**************************/
#if defined(MSDOS) || defined(__MSDOS__)
  maxlen -= 1;	   /* eliminates overwrite of buffers */
  i1 = 0;
  i = 0;
  exit_flag = 0;
  str_ptr[0] = '\x0';
  while (exit_flag == 0)
    {
    if (kbhit ())
      {
      str_ptr[i] = (ST_CHAR) getch ();
      str_ptr[i+1] = '\x0';
      switch (str_ptr[i])
	{
	case '\x0' :                     /* check for function key      */
	  getch ();			 /* discard if found		*/
	  break;

	case '\x08'  :                   /* check for a backspace       */
	  if (i > 0)
	    {
	    str_ptr[--i] = '\x20';
	    str_ptr[i+1] = '\x0';
	    str_ptr[i] = '\x0';
	    }
	break;

	case CR  :			/* check for a carriage return	*/
	  exit_flag = 1;
	  str_ptr[i] = '\x0';
	  ADD_LF;
	  break;

	case '\x1b'  :                  /* check for an escape sequence */
	  exit_flag = 1;			/* discard information and exit */
	  for (i = 0; i < maxlen; i++)
	    str_ptr[i] = '\x20';
	  str_ptr[maxlen] = '\x0';
	  str_ptr[0] = '\x0';
	  break;

	default      :			/* addition data		*/

	  if (i >= maxlen)		/* data greater than buffer	*/
	    {				/* scroll buffer		*/
	    for (i1=0; i1 < maxlen; ++i1)
	      str_ptr[i1] = str_ptr[i1+1];
	      str_ptr[maxlen] = '\x0';
	    }
	  else
	    ++i;			/* data less than buffer length */
	}				/* add in data to end of string */

/* print data string on console 					*/

      if (!exit_flag)
	{
	RESTORESCR;
	PRINTSTR;
	}
      }
    (*servefun)();			/* perform function while waiting */
    }
#endif
  }

/************************************************************************/
/*			input_hex					*/
/*  Function to accept sequence of bytes (in hex) from user.		*/
/************************************************************************/

ST_INT input_hex (ST_UCHAR *dest_str, ST_INT maxbytes)
  {
ST_BOOLEAN done;
ST_INT dest_len;
ST_CHAR	input[2] = {0};
ST_UINT16 temp_word;

  dest_len   = 0;
  done	     = SD_FALSE;
  while (!done && (dest_len < maxbytes))
    {
    printf ("\n Byte %d : ", dest_len+1);
    flush_keys ();

#if defined(_AIX) || defined(sun) || defined(__hpux) || defined(linux) \
    || (defined(__alpha) && !defined(__VMS)) \
    || defined(__VMS)
    input [0] = (ST_CHAR) get_a_char ();
#endif

#if defined(VXWORKS) || defined(__QNX__)
    input [0] = (ST_CHAR) getchar ();
#endif

#if defined(MSDOS) || defined(__MSDOS__)
    input [0] = (ST_CHAR) getch ();
#endif

#if defined(__OS2__) || defined (_WIN32)
    while (!kbhit ())			/* Wait for key or other event	*/
      do_key_wait_service ();		
    input [0] = getkch ();
#endif

    printf ("%c", input [0]);
    if ((input [0] < '0' || input [0] > '9') &&
	(input [0] < 'A' || input [0] > 'F') &&
	(input [0] < 'a' || input [0] > 'f'))
      done = SD_TRUE;
    else
      {
      flush_keys ();
#if defined(_AIX) || defined(sun) || defined(__hpux) || defined(linux) \
    || (defined(__alpha) && !defined(__VMS)) \
    || defined(__VMS)
      input [1] = (ST_CHAR) get_a_char ();
#endif

#if defined(VXWORKS) || defined(__QNX__)
      input [1] = (ST_CHAR) getchar ();
#endif

#if defined(MSDOS) || defined(__MSDOS__)
      input [1] = (ST_CHAR) getch ();
#endif

#if defined(__OS2__) || defined (_WIN32)
      while (!kbhit ())			/* Wait for key or other event	*/
        do_key_wait_service ();		
      input [1] = getkch ();
#endif

      printf ("%c", input [1]);
      if ((input [1] < '0' || input [1] > '9') &&
	  (input [1] < 'A' || input [1] > 'F') &&
	  (input [1] < 'a' || input [1] > 'f'))
	done = SD_TRUE;
      else
	{				/* both characters are valid	*/
	sscanf (input, "%2hx\n",&temp_word);
	*(dest_str+(dest_len)) = (ST_UCHAR) temp_word;
	dest_len++;
	}
      }
    }

  if ((dest_len != maxbytes) && (input [0] != CR)) /* check for CR	*/
    dest_len = 0;

  return (dest_len);
  }

