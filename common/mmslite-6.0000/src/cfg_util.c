/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1991-2006, All Rights Reserved       			*/
/*									*/
/* MODULE NAME : cfg_util.c						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/07/06  EJV     18    cfg_special_char_str: added '$'.		*/
/* 02/20/03  JRB     17    Del PSOS code.				*/
/* 11/29/01  EJV     16    Del code for old LATT, XENIX, ultrix, RMX86.	*/
/*			   Removed call to strcasecmp (sun)		*/
/* 10/18/01  JRB     15    Add str_util.h (for strcmpi, etc. protos)	*/
/* 01/19/01  EJV     14    Moved strcmpi,stricmp,strnicmp to str_util.c	*/
/* 11/07/00  JRB     13    Del QNX from ifdef. It supports stricmp, etc.*/
/* 04/28/00  JRB     12    Lint cleanup					*/
/* 09/13/99  MDE     11    Added SD_CONST modifiers			*/
/* 04/14/99  MDE     10    Replaced SYSTEM_SEL with compiler defines	*/
/* 10/08/98  MDE     09    Migrated to updated SLOG interface		*/
/* 08/24/98  EJV     08    Added __hpux to str* cmp functions		*/
/* 08/06/98  JRB     07    Added cfg_get_octet_string function.		*/
/* 06/15/98  MDE     06    Changes to allow compile under C++		*/
/* 05/22/98  EJV     05    added _AIX, sun, and __alpha to		*/
/*			     stricmp, strcmpi, strnicmp functions	*/
/* 04/03/98  RKR     04    added stricmp, strcmpi, strnicmp		*/
/* 12/22/97  JRB     03    Clean up PSOS code.				*/
/* 12/04/97  KCR     02    Added cfg_goto_keyword function        	*/
/* 11/05/97  MDE     01    Added VXWORKS support            		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "cfg_util.h"
#include "cfglog.h"
#include "str_util.h"
#include <ctype.h>

#include "slog.h"

/************************************************************************/

/* #define DEBUG */

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/

/* Externally visable variables                                         */
ST_CHAR *cfg_line_buf;             	/* File line read buffer        */
ST_INT cfg_curr_line;    		/* Current line number          */
ST_INT cfg_line_offset;	        	/* Offset into cfg_line_buf     */

ST_CHAR *cfg_special_char_str;
ST_CHAR *cfg_end_of_rval_str;

ST_BOOLEAN config_stop;
ST_RET config_err;
ST_BOOLEAN config_eof;

ST_UINT cfg_log_mask;
#ifdef DEBUG_SISCO 
SD_CONST ST_CHAR *SD_CONST _cfg_err_logstr = "CFG_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _cfg_flow_logstr = "CFG_LOG_FLOW";
#endif

/************************************************************************/

/* Internal definitions                                                 */
#define MAX_STRING_LEN 		256
#define LINE_BUF_SIZE		256

ST_INT cfg_max_string_len;
ST_INT cfg_line_buf_size;
ST_INT cfg_max_rval_len;

#define MAX_CONFIG_NEST		 10

/* Internal variables                                                   */
static ST_CHAR *cfg_string_buf;
static struct cfg_kw_el *curr_table;
static ST_BOOLEAN cfg_need_new_line;	
static FILE *config_fptr;		/* Configuration file handle            */

/* Keyword table stack control                                          */
static ST_INT num_on_stack;
static struct cfg_kw_el **cfg_kw_stack;


/* Internal funtions                                                    */
static struct cfg_kw_el *cfg_lookup_keyword (struct cfg_kw_el *, ST_CHAR *);
static ST_RET cfg_get_next_keyword (FILE *);
static ST_CHAR *cfg_get_rval (ST_VOID);
static ST_RET cfg_end_of_rval (ST_CHAR c);
static ST_RET cfg_special_char (ST_CHAR c);

/************************************************************************/
/************************************************************************/
/************************************************************************/
/*                       cfg_process_file				*/
/************************************************************************/

ST_RET cfg_process_file (ST_CHAR *fileName, struct cfg_kw_el *root_kw_tbl)
  {
ST_RET ret;

  if (cfg_special_char_str == NULL)
    cfg_special_char_str = ":\\/-_ .{}[]<>@,()$";

  if (cfg_end_of_rval_str == NULL)
    cfg_end_of_rval_str = "|";

  if (cfg_max_string_len == 0)
    cfg_max_string_len = MAX_STRING_LEN;

  if (cfg_line_buf_size  == 0)
    cfg_line_buf_size  = LINE_BUF_SIZE;	

  if (cfg_max_rval_len == 0)
    cfg_max_rval_len = MAX_RVAL_LEN;

/* Open the configuration file                                          */
  if ((config_fptr = fopen (fileName,"r"))==NULL)
    {
    CFG_LOG_ERR1 ("Config File (%s) Open Error",fileName);
    return (SD_FAILURE);
    }

  CFG_LOG_FLOW1 ("Config File (%s) Opened OK",fileName);

/* If a previous call to the function ended in error there may be some	*/
/* old keyword table pushed on the stack.  The keywords table stack is 	*/
/* initialized to zero to process the current configuration file	*/

  num_on_stack = 0;

/* Allocate required buffers                                            */

  cfg_line_buf = (ST_CHAR *) chk_calloc (cfg_line_buf_size, 
						sizeof(ST_CHAR));
  cfg_string_buf = (ST_CHAR *) chk_calloc (cfg_max_string_len+1, 
						sizeof(ST_CHAR));
  cfg_kw_stack = (struct cfg_kw_el **) chk_calloc (MAX_CONFIG_NEST, 
						sizeof (struct cfg_kw_el *));

/* Start with the ROOT keyword table                                    */

  cfg_tbl_push (root_kw_tbl);

/* Process the configuration file                                       */

  cfg_need_new_line = SD_TRUE;	
  cfg_line_offset = 0;		
  cfg_curr_line = 0;
  config_err = SD_FALSE;
  config_stop = SD_FALSE;
  config_eof = SD_FAILURE;

/* Process keywords while not done and no error is detected		*/

  while ( ( config_stop == SD_FALSE ) && ( config_err == SD_FALSE ) )
    {
    if ( cfg_get_next_keyword( config_fptr ) )
      break;
    }

/* All done, see if it went OK                                         */

  if ( ( config_stop == SD_TRUE ) && ( config_err == SD_FALSE ) )
    {
    CFG_LOG_CFLOW0 ("Config File Read OK");
    ret = SD_SUCCESS;
    }
  else
    {
    CFG_LOG_ERR3 ("Config Error, Line %d, \"%s\", Column %d",
          	cfg_curr_line, cfg_line_buf, cfg_line_offset);

    ret = SD_FAILURE;
    }

/* Free allocated buffers                                               */
  chk_free (cfg_line_buf);
  chk_free (cfg_string_buf);
  chk_free (cfg_kw_stack);

/* Close the file so the calling program can reopen it if necessary.	*/

  fclose( config_fptr );

  return (ret);
  }

/************************************************************************/
/************************************************************************/
/*                          cfg_get_next_keyword                        */
/************************************************************************/
/* This function is used to get the next keyword			*/

static ST_RET cfg_get_next_keyword (FILE *fptr)
  {
struct cfg_kw_el *ptr;
ST_INT i;
ST_CHAR c;

/* Look for 'yyy =' string                                              */

  while (SD_TRUE)			/* While looking for the next word      */
    {
    if (cfg_need_new_line)
      {
      if ( fgets( cfg_line_buf, cfg_line_buf_size - 1, fptr ) == NULL )
        {
        CFG_LOG_CFLOW0 ("No More Keywords : End Of File");

/* The following piece of logic exists to allow the state functions to	*/
/* declare when it is ok for the file to be out of keywords.  If a user */
/* state function set 'config_eof == SD_SUCCESS' then config_stop is set	*/
/* to true when ENDFILE is encountered.					*/

	if ( config_eof == SD_SUCCESS )
	  {
	  config_stop = SD_TRUE;
	  }
	return( SD_FAILURE );
        }

      /* Wack the \n                            */
      for (i = 0; i < cfg_line_buf_size; ++i)
        {
        if (cfg_line_buf[i] == '\n')
          {
          cfg_line_buf[i] = 0;
          break;
          }
        }

      cfg_line_offset = 0;		/* Offset is 0 for new line     */
      ++cfg_curr_line;
      if (cfg_line_buf[0] == '#')
	continue;			/* Discard comment lines        */

      cfg_need_new_line = SD_FALSE;	/* Got a new line               */
      }

/* We now have a line to work on, with cfg_line_offset the 1st char     */
/* Extract the keyword from the string (alpha numeric characters)       */
/*	'=' means end of rval                                          */
/*	0 is end of line                                                */

    i = 0;
    c = cfg_line_buf[cfg_line_offset++];
    while (c && c != '#' && 	/* End of line or comment start         */
           c != '=' &&       	/* end of keyword                       */
           c != '|' && 
	   i < cfg_max_string_len)
      {
      if (c != ' ' && c != '\t') /* Ignore spaces and tabs              */
        {
        cfg_string_buf[i] = c;
        ++i;
        }
      c = cfg_line_buf[cfg_line_offset++];
      }

    if (i >= cfg_max_string_len)
      {
      CFG_LOG_ERR1 ("Error : Line %d too long", cfg_curr_line);
      return (SD_FAILURE);
      }

    if (c == '#' || !c)	         /* See if we need a new line next time */
      cfg_need_new_line = SD_TRUE;

/* i is the index to the char position that terminated the scan         */
    cfg_string_buf[i]=0x0;		/* NULL terminate the keyword   */

/* if we have a keyword, then search look it up in the currently active	*/
/* keyword table and execute the selected function			*/

    if (strlen (cfg_string_buf))
      {				/* Find the keyword                     */
      if ((ptr = cfg_lookup_keyword (curr_table, cfg_string_buf)) != NULL)
	{
	CFG_LOG_CFLOW1 ("Keyword : %s",cfg_string_buf);
	(*ptr->extract)();
	break;
	}
      else		/* invalid keyword                              */
	{
	CFG_LOG_ERR1 ("Unknown Keyword : %s",cfg_string_buf);
	return (SD_FAILURE);
	}
      }
    } /* While SD_TRUE                                                     */

  return(SD_SUCCESS);
  }

/************************************************************************/
/*                       cfg_get_rval()				*/
/************************************************************************/

static ST_CHAR *cfg_get_rval(ST_VOID)
  {
ST_INT i;
ST_CHAR c;

  cfg_string_buf[0]=0x0;

/* Now get the rval (value).                                           */
/* search for 0 to note end of line	*/

  i = 0;
  c =  cfg_line_buf[cfg_line_offset++];

/* First strip leading white space (' ', '\t')                          */
  while (c == ' ' || c == '\t')
    c =  cfg_line_buf[cfg_line_offset++];

  while (c && 
         c != '#' && 
         !cfg_end_of_rval (c) && 
         i < cfg_max_rval_len-1)
    {
    if (isalnum (c) || cfg_special_char (c))
      {
      cfg_string_buf[i] = c;
      ++i;
      }
    c =  cfg_line_buf[cfg_line_offset++];
    }

  if (c == '#' || !c)	/* See if we need a new line next time          */
    cfg_need_new_line = SD_TRUE;

/* Strip trailing spaces and tabs                                       */
  while (i && (cfg_string_buf[i-1] == ' ' || 
               cfg_string_buf[i-1] == '\t'))
    i--;

/* Null terminate the string                                            */
  cfg_string_buf[i] = 0x0;

  if (!strlen (cfg_string_buf))
    {
    CFG_LOG_ERR2 ("Bad rval, line #%d \"%s\"",cfg_curr_line,cfg_line_buf);
    return (NULL);
    }

  CFG_LOG_CFLOW1 ("Rval    : '%s'",cfg_string_buf);
  return (cfg_string_buf);
  }

/************************************************************************/
/*			cfg_lookup_keyword 				*/
/************************************************************************/

static struct cfg_kw_el *cfg_lookup_keyword(struct cfg_kw_el *tbl_ptr,
	ST_CHAR *name)
  {
ST_INT i;
struct cfg_kw_el *ret = NULL;

  for (i = 0; i < MAX_NUM_KEYWORDS; ++i)
    {
    if (tbl_ptr[i].extract == NULL)		/* End of table         */
      break;
    else if (!strcmpi (name, tbl_ptr[i].name))
      {			      			/*  we have a match	*/
      if (tbl_ptr[i].valid !=NULL)		/* Want to validate?    */
        {
	if ((*tbl_ptr[i].valid)())
      	  ret = &tbl_ptr[i];
        }
      else
	ret = &tbl_ptr[i];

      break;
      }
    }

  return(ret);
  }

/************************************************************************/
/*                       cfg_end_of_rval  				*/
/************************************************************************/
/* CONFIG calls back to this function while looking the rval over      */
/* If this function returns SD_TRUE, CONFIG will assume that the           */
/* character is a keyword/rval pair seperator                          */
/* Used to allow multiple pairs per line                                */


static ST_RET cfg_end_of_rval (ST_CHAR c)
  {
ST_INT i;
ST_INT str_len;

  str_len = (ST_INT) strlen (cfg_end_of_rval_str);
  for (i = 0; i < str_len; ++i)
    {
    if (c == cfg_end_of_rval_str[i])
      return (SD_TRUE);
    }
  return (SD_FALSE);
  }

/************************************************************************/
/*                       cfg_special_char					*/
/************************************************************************/

static ST_RET cfg_special_char (ST_CHAR c)
  {
ST_INT i;
ST_INT str_len;

  str_len = (ST_INT) strlen (cfg_special_char_str);
  for (i = 0; i < str_len; ++i)
    {
    if (c == cfg_special_char_str[i])
      return (SD_TRUE);
    }

  return (SD_FALSE);
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/
/* 			cfg_tbl_push (new_table)			*/
/************************************************************************/

ST_RET cfg_tbl_push (struct cfg_kw_el *new_table)
  {
  if (num_on_stack >= MAX_CONFIG_NEST)
    {
    CFG_LOG_ERR0 ("Too many pushes - configuration nesting exceeded");
    config_stop = SD_TRUE;
    config_err = CFG_UTIL_ERR;
    return (SD_FAILURE);
    }

  cfg_kw_stack[num_on_stack] = new_table;
  ++num_on_stack;
  curr_table = new_table;
  return (SD_SUCCESS);
  }

/************************************************************************/
/* 			cfg_tbl_pop (num_to_pop)			*/
/************************************************************************/

ST_RET cfg_tbl_pop (ST_INT num_to_pop)
  {
  if ((num_on_stack-num_to_pop-1)<0)
    {
    CFG_LOG_ERR0 ("Too many pops off configuration stack");
    config_stop = SD_TRUE;
    config_err = CFG_UTIL_ERR;
    return (SD_FAILURE);
    }
  num_on_stack -= num_to_pop;
  curr_table = cfg_kw_stack[num_on_stack-1];
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                       cfg_get_short					*/
/************************************************************************/

ST_RET cfg_get_short (ST_INT16 *out_ptr)
  {
ST_INT d;
ST_INT16 ret;

  ret = cfg_get_value ("%d", &d);
  if (!ret)
    *out_ptr = (ST_INT16) d;  
  return (ret);
  }

/************************************************************************/
/*                       cfg_get_ushort					*/
/************************************************************************/

ST_RET cfg_get_ushort (ST_UINT16 *out_ptr)
  {
ST_UINT d;
ST_RET ret;

  ret = cfg_get_value ("%u", &d);
  if (!ret)
    *out_ptr = (ST_UINT16) d;  
  return (ret);
  }

/************************************************************************/
/*                       cfg_get_int					*/
/************************************************************************/

ST_RET cfg_get_int (ST_INT *out_ptr)
  {
ST_INT d;
ST_RET ret;

  ret = cfg_get_value ("%d", &d);
  if (!ret)
    *out_ptr = d;  
  return (ret);
  }

/************************************************************************/
/*                       cfg_get_uint					*/
/************************************************************************/

ST_RET cfg_get_uint (ST_UINT *out_ptr)
  {
ST_UINT d;
ST_RET ret;

  ret = cfg_get_value ("%u", &d);
  if (!ret)
    *out_ptr = d;  
  return (ret);
  }

/************************************************************************/
/*                       cfg_get_long					*/
/************************************************************************/

ST_RET cfg_get_long (ST_LONG *out_ptr)
  {
  return (cfg_get_value ("%ld", out_ptr));
  }

/************************************************************************/
/*                       cfg_get_ulong					*/
/************************************************************************/

ST_RET cfg_get_ulong (ST_ULONG *out_ptr)
  {
  return (cfg_get_value ("%lu", out_ptr));
  }


/************************************************************************/
/*                       cfg_get_double					*/
/************************************************************************/

ST_RET cfg_get_double (ST_DOUBLE *out_ptr)
  {
  return (cfg_get_value ("%lf", out_ptr));
  }


/************************************************************************/
/*                       cfg_get_hex_ushort 				*/
/************************************************************************/

ST_RET cfg_get_hex_ushort (ST_UINT16 *out_ptr)
  {
ST_UINT d;
ST_RET ret;

  ret = cfg_get_value ("%x", &d);
  if (!ret)
    *out_ptr = (ST_UINT16) d;  
  return (ret);

/*  return (cfg_get_value ("%x", out_ptr)); */
  }

/************************************************************************/
/*                       cfg_get_hex_uint 				*/
/************************************************************************/

ST_RET cfg_get_hex_uint (ST_UINT *out_ptr)
  {
ST_UINT d;
ST_RET ret;

  ret = cfg_get_value ("%x", &d);
  if (!ret)
    *out_ptr = d;  
  return (ret);

/*  return (cfg_get_value ("%x", out_ptr)); */
  }

/************************************************************************/
/*                       cfg_get_hex_ulong				*/
/************************************************************************/

ST_RET cfg_get_hex_ulong (ST_ULONG *out_ptr)
  {
  return (cfg_get_value ("%lx", out_ptr));
  }

/************************************************************************/
/*                       cfg_get_value					*/
/************************************************************************/

ST_RET cfg_get_value (ST_CHAR *format_string, ST_VOID *out_ptr)
  {
ST_CHAR *rval;

  rval = cfg_get_rval ();		/* get the rvalue string        */
  if (!rval)
    {
    config_err = CFG_GET_RVAL_ERR;
    CFG_LOG_ERR0 ("Get Rval Error");
    }
  else
    {                                   /* Convert to desired data format*/
    if (!sscanf (rval,format_string,out_ptr))
      {
      config_err = CFG_CONVERT_ERR;
      CFG_LOG_ERR0 ("Data Conversion Error");
      }
    }
  
  if (config_err)			/* if error detected ...        */
    {
    config_stop = SD_TRUE;
    return (SD_FAILURE);
    }

  return (SD_SUCCESS);
  }


/************************************************************************/
/************************************************************************/
/*                       *cfg_get_alloc_string				*/
/************************************************************************/

ST_CHAR *cfg_get_alloc_string (ST_VOID)
  {
ST_CHAR *ret_ptr;
ST_CHAR *rval;

  rval = cfg_get_rval ();		/* get the rvalue string        */
  if (!rval)
    {
    config_stop = SD_TRUE;
    config_err = CFG_GET_RVAL_ERR;
    CFG_LOG_ERR0 ("Get Rval Error");
    return (NULL);
    }

					/* allocate storage for string  */
  ret_ptr = (ST_CHAR *) chk_calloc (1, (unsigned int) (strlen (rval) +1));
  strcpy (ret_ptr,rval); 
  return (ret_ptr);
  }


/************************************************************************/
/*                       *cfg_get_string_ptr   				*/
/************************************************************************/

ST_CHAR *cfg_get_string_ptr (ST_VOID)
  {
ST_CHAR *rval;

  rval = cfg_get_rval ();		/* get the rvalue string        */
  if (!rval)
    {
    config_stop = SD_TRUE;
    config_err = CFG_GET_RVAL_ERR;
    CFG_LOG_ERR0 ("Get Rval Error");
    return (NULL);
    }

  return (rval);
  }

/************************************************************************/
/*                       cfg_get_octet_string   			*/
/************************************************************************/

ST_RET cfg_get_octet_string (ST_UCHAR *ostr,	/* ptr to user's ostr	*/
                ST_UINT *len_out_ptr,	/* addr of len var to be set	*/
                ST_UINT len_max)	/* maximum len to allow.	*/
  {
ST_UINT j;
ST_INT digit;
ST_BOOLEAN nibble;	/* SD_TRUE if nibble read, SD_FALSE if whole byte read*/
ST_CHAR *rval;

  rval = cfg_get_rval ();		/* get the rvalue string        */
  if (!rval)
    {
    config_stop = SD_TRUE;
    config_err = CFG_GET_RVAL_ERR;
    CFG_LOG_ERR0 ("Get Rval Error");
    return (SD_FAILURE);
    }

  j = 0;
  nibble = SD_FALSE;
  while (isxdigit (*rval))	/* get hex number			*/
    {
    if (j >= len_max)
      return (SD_FAILURE);	/* Selector longer than allowed	*/

    digit = *rval++;	/* separate nibbles			*/
    digit = isdigit (digit) ? digit - '0' : 10 + (toupper (digit) - 'A'); 
    if (nibble)
      {
      nibble = SD_FALSE;
      ostr[j] = (ostr[j] | (ST_UCHAR) digit);	/* set low */
      j++;

      while (isspace (*rval) && *rval != '\n')
        ++rval;
      }
    else
      {
      nibble = SD_TRUE;
      ostr[j] = (ST_UCHAR) digit << 4;		/* set high byte */
      }
    }

  if (nibble)
    {					/* Only got half of byte.	*/
    config_stop = SD_TRUE;
    config_err = CFG_CONVERT_ERR;
    CFG_LOG_ERR0 ("Incomplete byte in Octet string");
    return (SD_FAILURE);
    }

  /* set selector length						*/
  *len_out_ptr = j;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                          cfg_set_config_err                          */
/************************************************************************/

ST_VOID    cfg_set_config_err(ST_VOID)
  {
  config_err = SD_TRUE;
  }

/************************************************************************/
/*                          cfg_set_endfile_ok                          */
/************************************************************************/

ST_VOID    cfg_set_endfile_ok(ST_VOID)
  {
  config_eof = SD_SUCCESS;
  }



/************************************************************************/
/************************************************************************/
/*                          cfg_goto_keyword                            */
/************************************************************************/
/* This function is used to skip over a section to the specified keyword*/

ST_RET cfg_goto_keyword (ST_CHAR *keyword)
  {
ST_INT i, cfgStringLen, keywordLen;
ST_CHAR c;

/* Look for 'yyy =' string                                              */

  keywordLen = (ST_INT) strlen (keyword);
  while (SD_TRUE)	   /* While looking for the specified word      */
    {
    if (cfg_need_new_line)
      {
      if ( fgets( cfg_line_buf, cfg_line_buf_size - 1, config_fptr ) == NULL )
        {
        CFG_LOG_CFLOW0 ("No More Keywords : End Of File");

/* The following piece of logic exists to allow the state functions to	*/
/* declare when it is ok for the file to be out of keywords.  If a user */
/* state function set 'config_eof == SD_SUCCESS' then config_stop is set	*/
/* to true when ENDFILE is encountered.					*/

	if ( config_eof == SD_SUCCESS )
	  {
	  config_stop = SD_TRUE;
	  }
	return( SD_FAILURE );
        }

      /* Wack the \n                            */
      for (i = 0; i < cfg_line_buf_size; ++i)
        {
        if (cfg_line_buf[i] == '\n')
          {
          cfg_line_buf[i] = 0;
          break;
          }
        }

      cfg_line_offset = 0;		/* Offset is 0 for new line     */
      ++cfg_curr_line;
      if (cfg_line_buf[0] == '#')
	continue;			/* Discard comment lines        */

      cfg_need_new_line = SD_FALSE;	/* Got a new line               */
      }

/* We now have a line to work on, with cfg_line_offset the 1st char     */
/* Extract the keyword from the string (alpha numeric characters)       */
/*	'=' means end of rval                                          */
/*	0 is end of line                                                */

    i = 0;
    c = cfg_line_buf[cfg_line_offset++];
    while (c && c != '#' && 	/* End of line or comment start         */
           c != '=' &&       	/* end of keyword                       */
           c != '|' && 
	   i < cfg_max_string_len)
      {
      if (c != ' ' && c != '\t') /* Ignore spaces and tabs              */
        {
        cfg_string_buf[i] = c;
        ++i;
        }
      c = cfg_line_buf[cfg_line_offset++];
      }

    if (i >= cfg_max_string_len)
      {
      CFG_LOG_ERR1 ("Error : Line %d too long", cfg_curr_line);
      return (SD_FAILURE);
      }

    if (c == '#' || !c)	         /* See if we need a new line next time */
      cfg_need_new_line = SD_TRUE;

/* i is the index to the char position that terminated the scan         */
    cfg_string_buf[i]=0x0;		/* NULL terminate the keyword   */

/* if we have a keyword, then search look it up in the currently active	*/
/* keyword table and execute the selected function			*/

    if ((cfgStringLen = (ST_INT) strlen (cfg_string_buf)) > 0)
      {				/* Find the keyword                     */
      if ((cfgStringLen == keywordLen) && !strcmpi (keyword, cfg_string_buf))
        break;
      else		/* keyword not found */
        cfg_need_new_line = SD_TRUE;
      }
    } /* While SD_TRUE                                                     */

  return(SD_SUCCESS);
  }
















