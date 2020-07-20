/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1986, 1987, 1988, All Rights Reserved                   */
/*									*/
/* MODULE NAME : mem_chk.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains enhanced memory allocation functions	*/
/*	for performing various integrity checks				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	ST_VOID	init_mem_chk () 					*/
/*	ST_CHAR	*fat_err ()						*/
/*	ST_CHAR	*chk_malloc (size)					*/
/*	ST_CHAR	*chk_calloc (num,size)					*/
/*	ST_CHAR	*chk_realloc (ptr)					*/
/*	ST_CHAR	*chk_free (ptr)						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/20/01  JRB     04    If !DEBUG_SISCO, chk_* macros call nd_chk_*.	*/
/*			   If DEBUG_SISCO,  chk_* macros call x_chk_*.	*/
/*			   Del (x_)chk_free_wipe functions.		*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 07/25/96  MDE     02    Added x_ series functions, other 		*/
/* 02/21/95  MDE     01    Created from mem_chk.h			*/
/************************************************************************/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#define MEM_CHK					/* used in mem_chk.h 	*/
#include "mem_chk.h"

/************************************************************************/
/* These variables are not used, but are included here to satisfy 	*/
/* linking to modules that need them					*/

ST_BOOLEAN m_fill_en;
ST_BOOLEAN m_heap_check_enable;
ST_BOOLEAN m_check_list_enable;

/************************************************************************/
/* These functions (with leading '_') are EXACTLY the same as the	*/
/* "nd_chk_*" functions, but they are static so they can only be	*/
/* called by the "x_chk_*" functions within this module, and		*/
/* only if DEBUG_SISCO is defined.					*/
/* This prevents the user from mixing debug and non-debug calling code.	*/
/*   If this module is compiled with DEBUG_SISCO defined, then ALL user	*/
/* code must be compiled with DEBUG_SISCO defined, and the user code	*/
/* will call "x_chk_*".							*/
/*   If this module is compiled with DEBUG_SISCO !defined, then ALL user*/
/* code must be compiled with DEBUG_SISCO !defined, and the user code	*/
/* will call "nd_chk_*".							*/
/************************************************************************/
#ifdef DEBUG_SISCO
static ST_VOID *_nd_chk_malloc (size_t size);
static ST_VOID *_nd_chk_calloc (size_t num, size_t size);
static ST_VOID *_nd_chk_realloc (ST_VOID *ptr, size_t size);
static ST_VOID  _nd_chk_free (ST_VOID *del_mem_ptr);
#endif

/************************************************************************/
/*			nd_chk_malloc					*/
/************************************************************************/

#ifdef DEBUG_SISCO
static ST_VOID *_nd_chk_malloc (size_t size)
#else
ST_VOID *nd_chk_malloc (size_t size)
#endif
  {
ST_VOID *p;

  p = malloc (size);
  if (p == NULL)   	/* these functions cannot return if they fail 	*/
    exit (2);
  return (p);
  }

/************************************************************************/
/*			nd_chk_calloc					*/
/************************************************************************/

#ifdef DEBUG_SISCO
static ST_VOID *_nd_chk_calloc (size_t num, size_t size)
#else
ST_VOID *nd_chk_calloc (size_t num, size_t size)
#endif
  {
ST_VOID *p;

  p = calloc (num,size);
  if (p == NULL)   	/* these functions cannot return if they fail 	*/
    exit (3);
  return (p);
  }

/************************************************************************/
/*			nd_chk_realloc					*/
/************************************************************************/

#ifdef DEBUG_SISCO
static ST_VOID *_nd_chk_realloc (ST_VOID *old, size_t size)
#else
ST_VOID *nd_chk_realloc (ST_VOID *old, size_t size)
#endif
  {
ST_VOID *p;

  p = realloc (old,size);
  if (p == NULL)   	/* these functions cannot return if they fail 	*/
    exit (4);
  return (p);
  }

/************************************************************************/
/*			nd_chk_free					*/
/************************************************************************/

#ifdef DEBUG_SISCO
static ST_VOID _nd_chk_free (ST_VOID *p)
#else
ST_VOID nd_chk_free (ST_VOID *p)
#endif
  {
  free (p);
  }

#ifdef DEBUG_SISCO
/************************************************************************/
/*                     x_chk_malloc					*/
/*                     x_chk_calloc					*/
/*                     x_chk_realloc					*/
/*                     x_chk_free					*/
/************************************************************************/
/* These functions are usually called via macro (see mem_chk.h) and	*/
/* pass the usual allocation parameters plus the file name & line num	*/


ST_VOID *x_chk_malloc (size_t size, SD_CONST ST_CHAR *SD_CONST file, 
		       SD_CONST ST_INT line)
  {
  return (_nd_chk_malloc (size));	/* get the pointer			*/
  }

ST_VOID *x_chk_calloc (size_t num, size_t size, 
		       SD_CONST ST_CHAR *SD_CONST file, SD_CONST ST_INT line)
  {
  return (_nd_chk_calloc (num,size));
  }

ST_VOID *x_chk_realloc (ST_VOID *ptr, size_t size, 
			SD_CONST ST_CHAR *SD_CONST file, SD_CONST ST_INT line)
  {
  return (_nd_chk_realloc (ptr,size)); /* get the pointer			*/
  }

ST_VOID x_chk_free (ST_VOID *ptr, SD_CONST ST_CHAR *SD_CONST file, 
				  SD_CONST ST_INT line)
  {
  _nd_chk_free (ptr);		/* free the pointer			*/
  }

#endif	/* DEBUG_SISCO	*/

