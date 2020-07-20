/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1999-2007 All Rights Reserved                   	*/
/*									*/
/* MODULE NAME : mem_smem.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains enhanced memory allocation functions	*/
/*	for performing various integrity checks				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/03/12  JRB           Chg args to size_t like system functs.	*/
/* 06/09/10  JRB	   Fix more log messages.			*/
/* 04/13/10  JRB	   Fix log messages & printf calls.		*/
/* 01/15/07  EJV     12    Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 01/30/06  GLB     11    Integrated porting changes for VMS           */
/* 06/06/03  JRB     10    Use S_LOCK_RESOURCES (new util mutex).	*/
/* 04/14/03  JRB     09    Eliminate compiler warnings.			*/
/* 10/09/01  JRB     08    Fix for !DEBUG_SISCO.			*/
/* 09/19/01  JRB     07    Add NULL ptr check in dyn_mem_ptr_status.	*/
/* 09/10/01  JRB     06    If !DEBUG_SISCO, macros call "nd_m_*".	*/
/*			   If DEBUG_SISCO,  macros call "x_m_*".	*/
/*			   nd_* functions use CHK_DYN_MEM_NODE_NDEBUG	*/
/*			   Globals calling_file/line deleted (see args).*/
/*			   Del "*_free_wipe" functions.			*/
/* 			   Del m_pad_string_len (use m_num_pad_bytes)	*/
/*			   Del smemInitialized (not needed).		*/
/*			   Use "assert (mem_chk_initialized)".		*/
/*			   Add S_LOCK in init_mem_chk.			*/
/*			   Use m_mem_overhead everywhere.		*/
/*			   Never call *_memerr_fun (user can't recover).*/
/*			   Add m_add_pool function.			*/
/*			   Don't allow SMEM_CONTEXT *smem_ctx == NULL.	*/
/* 09/10/01  JRB     05    Default m_find_node_enable=FALSE (faster).	*/
/* 03/16/01  GLB     04    Added S_SMEM_MAX_CONTEXT                     */
/* 03/16/01  GLB     03    Added assert for smemInitialized             */
/* 11/01/00  MDE     02    Added m_smem_ctxt				*/
/* 12/28/99  MDE     01    New, derived from mem_chks.c			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mem_chk.h"
#include "memlog.h"
#include "gen_list.h"
#include <ctype.h>

#include <assert.h>

/************************************************************************/
/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* These functions (with leading '_') are EXACTLY the same as the	*/
/* "nd_m_*" functions, but they are static so they can only be		*/
/* called by the "x_m_*" functions within this module, and		*/
/* only if DEBUG_SISCO is defined.					*/
/* This prevents the user from mixing debug and non-debug calling code.	*/
/*   If this module is compiled with DEBUG_SISCO defined, then ALL user	*/
/* code must be compiled with DEBUG_SISCO defined, and the user code	*/
/* will call "x_m_*".							*/
/*   If this module is compiled with DEBUG_SISCO !defined, then ALL user*/
/* code must be compiled with DEBUG_SISCO !defined, and the user code	*/
/* will call "nd_m_*".							*/
/************************************************************************/
#ifdef DEBUG_SISCO
static ST_VOID *_nd_m_malloc (SMEM_CONTEXT *smem_ctx, size_t size);
static ST_VOID *_nd_m_calloc (SMEM_CONTEXT *smem_ctx, size_t num, size_t size);
static ST_VOID *_nd_m_realloc (SMEM_CONTEXT *smem_ctx, ST_VOID *ptr, size_t size);
static ST_VOID  _nd_m_free (SMEM_CONTEXT *smem_ctx, ST_VOID *del_mem_ptr);
#endif

/************************************************************************/

SMEM_CONTEXT m_smem_ctxt[M_SMEM_MAX_CONTEXT];

/************************************************************************/
/* DEBUG related defines & variables (leave declared for both versions)	*/

ST_BOOLEAN m_mem_debug;
static ST_BOOLEAN m_mem_debug_saved;	/* changed ONLY in init_mem_chk	*/
ST_VOID *m_bad_ptr_val;

/* Use 4 bytes (default) at the end of each buffer for overwrite detect	*/
/* Default value used will be 0xDEADBEEF			 	*/
ST_CHAR *m_pad_string;
ST_INT m_num_pad_bytes;

/* Use this to fill malloc'ed and freed memory with garbage */
ST_UCHAR m_fill_byte;
ST_BOOLEAN m_fill_en;

/* This variable used to enable MSOFT heap check calls on every alloc	*/
/* and free call.							*/
ST_BOOLEAN m_heap_check_enable;

/* This variable used to enable list validation and overwrite checking	*/
/* on every alloc  and free call. 					*/
ST_BOOLEAN m_check_list_enable;

/* Set this variable SD_FALSE to speed up the debug version. When SD_TRUE, it */
/* enables searching the memory list for the element before accessing	*/
/* the memory during chk_realloc and chk_free calls			*/
ST_BOOLEAN m_find_node_enable;

/* This variable will cause chk_realloc to not realloc when the new	*/
/* size is smaller than the old size					*/
ST_BOOLEAN m_no_realloc_smaller;

/* Dump memory table whenever high water mark is advanced */
ST_BOOLEAN m_auto_hw_log;

/* If this variable is SD_TRUE, the x_chk_free function will record the	*/
/* free'er of a memory block. Can cause protect violations if a truly	*/
/* bogus pointer is free'd						*/
ST_BOOLEAN m_track_prev_free;

/* Bit masked logging control variable				 	*/
ST_UINT chk_debug_en;

#ifdef DEBUG_SISCO
SD_CONST ST_CHAR *SD_CONST _mem_log_err_logstr = "MEM_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _mem_log_calloc_logstr = "MEM_LOG_CALLOC";
SD_CONST ST_CHAR *SD_CONST _mem_log_malloc_logstr = "MEM_LOG_MALLOC";
SD_CONST ST_CHAR *SD_CONST _mem_log_realloc_logstr = "MEM_LOG_REALLOC";
SD_CONST ST_CHAR *SD_CONST _mem_log_free_logstr = "MEM_LOG_FREE";
SD_CONST ST_CHAR *SD_CONST _mem_log_nerr_logstr = "MEM_LOG_NERR";
SD_CONST ST_CHAR *SD_CONST _mem_log_dump_logstr = "MEM_LOG_DUMP";
#endif

/* Declarations for functions and array of structures used to keep track*/
/* of allocated memory.							*/
ST_UINT32 chk_curr_mem_count;
ST_UINT32 chk_max_dyn_mem_ptrs;
size_t chk_max_mem_allocated;
size_t chk_curr_mem_size;

/* This function pointer can be set to select a function to be called 	*/
/* in case of memory error.						*/
ST_VOID (*mem_chk_err)(ST_VOID);

/* For BACKWARD COMPATIBILITY only. Never called.			*/
ST_VOID *(*m_memerr_fun)(size_t size);
ST_VOID *(*c_memerr_fun)(size_t num, size_t size);
ST_VOID *(*r_memerr_fun)(ST_VOID *ptr, size_t size);

/************************************************************************/
/************************************************************************/
/* Internal variables							*/

/* Memory block header, used for DEBUG to track all allocated memory	*/

#if defined(DEBUG_SISCO)
typedef struct chk_dyn_mem_node
  {
  DBL_LNK chk_mem_node_link;
  size_t chk_alloc_mem_size;
  ST_VOID *chk_alloc_mem_ptr;
  ST_CHAR *file; 	/* File && line that allocated the memory	*/
  ST_INT line;
  ST_UINT16 overwrite_prev_detected;  /* already detected flag 		*/
  SD_END_STRUCT
  } CHK_DYN_MEM_NODE;
static CHK_DYN_MEM_NODE *head_dyn_mem_list;
#endif

/* NO DEBUG structure. Used if !DEBUG_SISCO || !m_mem_debug_saved.	*/

typedef struct chk_dyn_mem_node_ndebug
  {
  size_t chk_alloc_mem_size;
  SD_END_STRUCT
  } CHK_DYN_MEM_NODE_NDEBUG;

/************************************************************************/
/* DEBUG internal variables						*/

/* padding variables							*/
ST_UINT m_mem_overhead; 

/* The 'mem_chk' tools must be initialized before being used, esp.	*/
/* in DEBUG mode, or we may write or flush a NULL stream		*/
static ST_INT mem_chk_initialized;

/************************************************************************/
/* Internal functions							*/

#ifdef DEBUG_SISCO
static ST_RET check_overwrite (CHK_DYN_MEM_NODE *ptr);
static ST_VOID set_overwrite_value (CHK_DYN_MEM_NODE *ptr);
#endif

/************************************************************************/
/*                       init_mem_chk					*/
/************************************************************************/

ST_VOID init_mem_chk ()
  {
  S_LOCK_UTIL_RESOURCES ();

  if (mem_chk_initialized)
    {					/* do not allow reinitialize	*/
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

  m_mem_debug_saved = m_mem_debug;	/* Save user var to private var	*/

  chk_debug_en |= MEM_LOG_ERR;

  if (m_bad_ptr_val == NULL)
    m_bad_ptr_val = (ST_VOID *) -1;
  if (m_pad_string == NULL)
    m_pad_string = "\336\255\276\357"; /* 0xDEADBEEF */
  if (m_num_pad_bytes == 0)
    m_num_pad_bytes = 4;
  if (m_fill_byte == 0)
    m_fill_byte = (ST_UCHAR) 0xCC;

/* Calculate the debug memory overhead					*/
#ifdef DEBUG_SISCO
  if (m_mem_debug_saved)
    m_mem_overhead = sizeof (CHK_DYN_MEM_NODE) + m_num_pad_bytes;
  else
    m_mem_overhead = sizeof (CHK_DYN_MEM_NODE_NDEBUG);
#else
  m_mem_overhead = sizeof (CHK_DYN_MEM_NODE_NDEBUG);
#endif

  mem_chk_initialized = SD_TRUE;
  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*			m_add_pool					*/
/************************************************************************/
SMEM_POOL_CTRL *m_add_pool (SMEM_CONTEXT *smemContext, 
			       ST_UINT8 numBuf, ST_UINT bufSize,
			       ST_BOOLEAN autoClone,
			       ST_INT maxClones,
			       ST_CHAR *poolName)
  {
  assert (mem_chk_initialized);	/* to be sure m_mem_overhead is set	*/
  return (smem_add_pool (smemContext, numBuf, bufSize + m_mem_overhead,
                         autoClone, maxClones, poolName));
  }

#if defined(DEBUG_SISCO)	/* x_m_* functions only for DEBUG mode	*/
/************************************************************************/
/*			x_m_malloc					*/
/************************************************************************/

ST_VOID *x_m_malloc  (SMEM_CONTEXT *smem_ctx, size_t size,  
			SD_CONST ST_CHAR *SD_CONST calling_file, 
			SD_CONST ST_INT calling_line)
  {		      
CHK_DYN_MEM_NODE *new_mem_node;

  assert (mem_chk_initialized);		/* init_mem_chk MUST be called first*/

  if (m_mem_debug_saved)
    {
    S_LOCK_UTIL_RESOURCES (); 

/* Do linked list && overwrite checks, if enabled			*/
    if (m_check_list_enable)
      check_mem_list ();	

/* It is not OK to malloc a buffer of size 0 - check for this		*/
    if (!size)
      {
      MEMLOG_ERR0 ("Attempting to malloc with size == 0");
      if (mem_chk_err)
        (*mem_chk_err)();
      size++;
      }

/* Allocate the buffer		*/
    new_mem_node = (CHK_DYN_MEM_NODE *) 
			smem_malloc (smem_ctx, size + m_mem_overhead);
    if (new_mem_node == NULL)
      {
      MEMLOG_ERR0 ("WARNING : Returning NULL Pointer");
      if (mem_chk_err)
        (*mem_chk_err)();

      S_UNLOCK_UTIL_RESOURCES ();
      return (NULL);
      }

    smem_range_check (smem_ctx, size, SD_TRUE);

/* Set up the memory control block header 				*/
    new_mem_node->chk_alloc_mem_size = size;
    new_mem_node->chk_alloc_mem_ptr = (ST_VOID *)(new_mem_node+1);

/* Fill memory with garbage */
    if (m_fill_en)
      memset (new_mem_node->chk_alloc_mem_ptr, m_fill_byte, size);

/* Save file and line numbers						*/
    new_mem_node->file = (ST_CHAR *)calling_file;
    new_mem_node->line = calling_line;

/* Set the overwrite detect value at the end of the buffer		*/
    set_overwrite_value (new_mem_node);

/* Now add this element to the allocation control list 			*/
    list_add_first ((ST_VOID **)&head_dyn_mem_list, new_mem_node);     

/* Update memory allocation statistics parameters			*/
    chk_curr_mem_count++;
    chk_curr_mem_size += size;
    if (chk_curr_mem_count > chk_max_dyn_mem_ptrs)
      chk_max_dyn_mem_ptrs = chk_curr_mem_count;
    if (chk_curr_mem_size > chk_max_mem_allocated)
      {
      chk_max_mem_allocated = chk_curr_mem_size;
      if (m_auto_hw_log)
        dyn_mem_ptr_status ();
      }
/* log allocation info, if enabled					*/
    MEMLOG_MALLOC2 ("m_malloc : size = %05u, ptr = " S_FMT_PTR
                                       ,size, (ST_VOID *)(new_mem_node+1));
    if (calling_line != -1)
      {
      MEMLOG_CMALLOC2 ("File : %s, Line %d",calling_file, calling_line);
      }

/* return the user pointer						*/
    S_UNLOCK_UTIL_RESOURCES ();
    return ((ST_VOID *)(new_mem_node+1));
    }
  else
    return (_nd_m_malloc (smem_ctx, size));
  }

/************************************************************************/
/*			x_m_calloc					*/
/************************************************************************/

ST_VOID *x_m_calloc  (SMEM_CONTEXT *smem_ctx, size_t num, 
			size_t size,  
			SD_CONST ST_CHAR *SD_CONST calling_file, 
			SD_CONST ST_INT calling_line)
  {
CHK_DYN_MEM_NODE *new_mem_node;
size_t	buf_size;

  assert (mem_chk_initialized);		/* init_mem_chk MUST be called first*/

  if (m_mem_debug_saved)
    {
    S_LOCK_UTIL_RESOURCES (); 

/* It is not OK to calloc a buffer of size 0 - check for this		*/
    if (!size)
      {
      MEMLOG_ERR0 ("Attempting to calloc with size == 0");
      if (mem_chk_err)
        (*mem_chk_err)();
      size++;
      }
    if (!num)
      {
      MEMLOG_ERR0 ("Attempting to calloc with num == 0");
      if (mem_chk_err)
        (*mem_chk_err)();
      num++;
      }

/* Calculate the size required ... Save a multiply? 		*/
    if (num == 1)
      buf_size = size;
    else if (size == 1)
      buf_size = num;
    else
      buf_size = num * size; 

/* get the buffer							*/
    smem_range_check (smem_ctx, buf_size, SD_TRUE);

    new_mem_node = (CHK_DYN_MEM_NODE *) 
			smem_calloc (smem_ctx, 1, buf_size + m_mem_overhead);

/* verify NULL not returned						*/
    if (!new_mem_node)
      {
      MEMLOG_ERR0 ("WARNING : Returning NULL Pointer");
      if (mem_chk_err)
        (*mem_chk_err)();
      S_UNLOCK_UTIL_RESOURCES (); 
      return (NULL);
      }

/* Set up the memory control block header 				*/
    new_mem_node->chk_alloc_mem_size = buf_size;
    new_mem_node->chk_alloc_mem_ptr = (ST_VOID *)(new_mem_node+1);

/* Save file and line numbers						*/
    new_mem_node->file = (ST_CHAR *)calling_file;
    new_mem_node->line = calling_line;

/* Set the overwrite detect value at the end of the buffer		*/
    set_overwrite_value (new_mem_node);

/* Now add this element to the allocation control list 			*/
    list_add_first ((ST_VOID **)&head_dyn_mem_list, new_mem_node);     

/* Update memory allocation statistics parameters			*/
    chk_curr_mem_count++;
    chk_curr_mem_size += buf_size;
    if (chk_curr_mem_count > chk_max_dyn_mem_ptrs)
      chk_max_dyn_mem_ptrs = chk_curr_mem_count;
    if (chk_curr_mem_size > chk_max_mem_allocated)
      {
      chk_max_mem_allocated = chk_curr_mem_size;
      if (m_auto_hw_log)
        dyn_mem_ptr_status ();
      }

/* log allocation info, if enabled					*/
    MEMLOG_CALLOC3 ("m_calloc : num = %05u, size = %05u Ptr = " S_FMT_PTR,num,size,new_mem_node+1);
    if (calling_line != -1)
      {
      MEMLOG_CCALLOC2 ("File : %s, Line %d",calling_file, calling_line);
      }

/* return the user pointer						*/
    S_UNLOCK_UTIL_RESOURCES (); 
    return ((ST_VOID *)(new_mem_node+1));
    }
  else
    return (_nd_m_calloc (smem_ctx, num, size));
  }

/************************************************************************/
/*			x_m_realloc					*/
/************************************************************************/

ST_VOID *x_m_realloc (SMEM_CONTEXT *smem_ctx, ST_VOID *ptr, 
			size_t size, 
			SD_CONST ST_CHAR *SD_CONST calling_file, 
			SD_CONST ST_INT calling_line)
  {
CHK_DYN_MEM_NODE *old_mem_node, *new_mem_node;
ST_VOID *old_mem_ptr;
size_t old_mem_size;

  assert (mem_chk_initialized);		/* init_mem_chk MUST be called first*/

  if (m_mem_debug_saved)
    {
    S_LOCK_UTIL_RESOURCES (); 

/* Do linked list && overwrite checks, if enabled			*/
    if (m_check_list_enable)
      check_mem_list ();	

/* It is not OK to pass in a NULL pointer, check for this		*/
    if (!ptr)
      {
      MEMLOG_ERR2 ("File : %s, Line %d",
                                      calling_file, calling_line);
      MEMLOG_CERR0 ("Attempting to realloc NULL pointer");
      if (mem_chk_err)
        (*mem_chk_err)();
      S_UNLOCK_UTIL_RESOURCES (); 
      return (NULL);
      }

/* It is not OK to re-alloc a buffer of size 0 - check for this		*/
    if (!size)
      {
      MEMLOG_ERR0 ( "Attempting to realloc with size = 0");
      if (mem_chk_err)
        (*mem_chk_err)();
      size++;
      }

/* Get the pointer to the control information				*/
    old_mem_node = ((CHK_DYN_MEM_NODE *)ptr)-1;

/* Verify that this node is on the list before accessing it		*/
    if (m_find_node_enable == SD_TRUE)
      {
      if (list_find_node (head_dyn_mem_list, old_mem_node) != SD_SUCCESS)
        {
        goto MEM_REALLOC_ERROR;
        }
      }

/* extract information from memory block header				*/
    old_mem_ptr = old_mem_node->chk_alloc_mem_ptr;
    old_mem_size = old_mem_node->chk_alloc_mem_size;

    if (old_mem_ptr != ptr) /* verify that the ptr passed in matches 	*/
      {
MEM_REALLOC_ERROR :
      MEMLOG_ERR2 ("\n File : %s, Line %d",
                                      calling_file, calling_line);
      MEMLOG_CERR1 (
                 "Invalid realloc (" S_FMT_PTR ")",ptr);

      if (mem_chk_err)
        (*mem_chk_err)();
      S_UNLOCK_UTIL_RESOURCES (); 
      return (ptr);	/* just hope realloc was for smaller		*/
      }

/* Update memory allocation statistics parameters			*/
    chk_curr_mem_count--;				/* update the node count*/
    chk_curr_mem_size -= old_mem_size;		/* update the mem size	*/


/* Check for 'no realloc smaller' option				*/
/* This is controlled either by compile time define or by variable	*/
/* If new size is less than old size, just don't use the system realloc	*/

    if (m_no_realloc_smaller &&
        (size <= old_mem_node->chk_alloc_mem_size))
      {
      new_mem_node = old_mem_node;
      }  
    else
      {
/* invalidate the old block						*/
      old_mem_node->chk_alloc_mem_ptr = NULL;    

/* remove the old memory block from the mem_chk system	 		*/
      list_unlink ((ST_VOID **)&head_dyn_mem_list, old_mem_node); /* unlink the old node	*/

/* Use the system realloc to get the new pointer			*/
      smem_range_check (smem_ctx, size, SD_TRUE);
      smem_range_check (smem_ctx, old_mem_size, SD_FALSE);


      new_mem_node = (CHK_DYN_MEM_NODE *) 
		smem_realloc (smem_ctx, (ST_VOID *) old_mem_node,
                              old_mem_node->chk_alloc_mem_size + m_mem_overhead,
			      size + m_mem_overhead);
      if (new_mem_node == NULL)
        {
        MEMLOG_ERR0 ("WARNING : Returning NULL Pointer");
        if (mem_chk_err)
          (*mem_chk_err)();
        S_UNLOCK_UTIL_RESOURCES (); 
        return( NULL );	/* if we can't get any back we return a NULL	*/
        }

/* Now add this element to the allocation control list 			*/
      list_add_first ((ST_VOID **)&head_dyn_mem_list, new_mem_node);
      }

/* Now we have a new memory block, initialize the mem_chk header and	*/
/* put on the active list						*/
    new_mem_node->chk_alloc_mem_size = size;/* update the header	*/
    new_mem_node->chk_alloc_mem_ptr = (ST_VOID *)(new_mem_node+1);

    new_mem_node->file = (ST_CHAR *)calling_file;
    new_mem_node->line = calling_line;

/* Initialize the overwrite value					*/
    set_overwrite_value (new_mem_node);

/* Update memory allocation statistics parameters			*/
    chk_curr_mem_size += size;		/* add the new size	*/
    chk_curr_mem_count++;			/* update the node count*/
    if (chk_curr_mem_size > chk_max_mem_allocated)
      {
      chk_max_mem_allocated = chk_curr_mem_size;
      if (m_auto_hw_log)
        dyn_mem_ptr_status ();
      }
/* log allocation info, if enabled					*/
    MEMLOG_REALLOC3 ("m_realloc : ptr = " S_FMT_PTR ", size = %05u, ptr = " S_FMT_PTR,
					  ptr,size, (ST_VOID *)(new_mem_node+1));
    if (calling_line != -1)
      {
      MEMLOG_CREALLOC2 ("File : %s. Line %d",
			      calling_file,
			      calling_line);
      }

    S_UNLOCK_UTIL_RESOURCES (); 
    return ((ST_VOID *)(new_mem_node+1));
    }
  else
    return (_nd_m_realloc (smem_ctx, ptr, size));
  }

/************************************************************************/
/*			x_m_strdup					*/
/************************************************************************/

ST_CHAR *x_m_strdup  (SMEM_CONTEXT *smem_ctx, ST_CHAR *str, 
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line)
  {
ST_CHAR *new_str;

  new_str = (ST_CHAR *) x_m_malloc (smem_ctx, strlen (str) + 1, file, line);
  strcpy (new_str, str);
  return (new_str);
  }

/************************************************************************/
/*			  x_m_free					*/
/************************************************************************/

ST_VOID  x_m_free    (SMEM_CONTEXT *smem_ctx, ST_VOID *del_mem_ptr, 
			SD_CONST ST_CHAR *SD_CONST calling_file, 
			SD_CONST ST_INT calling_line)
  {
CHK_DYN_MEM_NODE *chk_mem_node;

  assert (mem_chk_initialized);		/* init_mem_chk MUST be called first*/

  if (m_mem_debug_saved)
    {
    S_LOCK_UTIL_RESOURCES (); 

/* Do linked list && overwrite checks, if enabled			*/
    if (m_check_list_enable)
      check_mem_list ();	/* check linked list. Works for all systems.	*/

/* log allocation info, if enabled					*/
    MEMLOG_FREE1 ("m_free   : ptr = " S_FMT_PTR,del_mem_ptr);

    if (calling_line != -1)
      {
      MEMLOG_CFREE2 ("File : %s. Line %d",
			      calling_file,
			      calling_line);
      }

/* verify that a non-NULL pointer was passed in				*/
    if (!del_mem_ptr)
      {
      goto MEM_FREE_ERROR; 
      }

/* Get the pointer to the control information				*/
    chk_mem_node = ((CHK_DYN_MEM_NODE *)del_mem_ptr)-1;

/* Verify that this node is on the list before accessing it		*/
    if (m_find_node_enable == SD_TRUE)
      {
      if (list_find_node (head_dyn_mem_list, chk_mem_node) != SD_SUCCESS)
        {
        goto MEM_FREE_ERROR;
        }
      }

/* verify that the pointer matches the expected pointer			*/
    if (chk_mem_node->chk_alloc_mem_ptr == del_mem_ptr)
      {
      if (check_overwrite (chk_mem_node))
        {
        MEMLOG_ERR0 ("Memory overwrite at end of buffer detected");
        MEMLOG_CERR5 (
          "Node = " S_FMT_PTR ", Prev = " S_FMT_PTR ", Next = " S_FMT_PTR ", Len = %u, Curr = " S_FMT_PTR,
                 chk_mem_node, 
                 chk_mem_node->chk_mem_node_link.prev, 
                 chk_mem_node->chk_mem_node_link.next,
    	   chk_mem_node->chk_alloc_mem_size, 
    	   chk_mem_node->chk_alloc_mem_ptr);
        MEMLOG_CERR2 ("Pointer was allocated at file: %s, line %d ",
	   		  chk_mem_node->file,chk_mem_node->line);
        MEMLOG_CERR2 ("Free call made from file : %s, Line %d ",
                                        calling_file,
                                        calling_line);
        MEMLOG_CHERR ((ST_INT)(chk_mem_node->chk_alloc_mem_size + 
		      sizeof (CHK_DYN_MEM_NODE) +
		      m_num_pad_bytes), chk_mem_node);

/* Take care of this memory block					*/
        chk_mem_node->chk_alloc_mem_ptr = NULL;    /* invalidate the block */
        chk_curr_mem_size -= chk_mem_node->chk_alloc_mem_size;
        chk_curr_mem_count--;
        list_unlink ((ST_VOID **)&head_dyn_mem_list, chk_mem_node);
        if (mem_chk_err)
          (*mem_chk_err)();
        }
      else	/* This is a good free, do it!				*/
        {
        chk_mem_node->chk_alloc_mem_ptr = NULL;    /* invalidate the block	*/
        chk_curr_mem_size -= chk_mem_node->chk_alloc_mem_size;
        chk_curr_mem_count--;
        list_unlink ((ST_VOID **)&head_dyn_mem_list, chk_mem_node);

/* Fill memory with garbage before freeing it */
#ifdef MEM_FILL_CONTROL
        if (m_fill_en)
          memset (chk_mem_node, m_fill_byte, 
	          chk_mem_node->chk_alloc_mem_size + 
		  sizeof (CHK_DYN_MEM_NODE));
#else
        if (m_fill_en)
          memset ((ST_CHAR *) chk_mem_node + sizeof (CHK_DYN_MEM_NODE), 
	          m_fill_byte, 
	          chk_mem_node->chk_alloc_mem_size);
#endif

        smem_range_check (smem_ctx, chk_mem_node->chk_alloc_mem_size, SD_FALSE);

        smem_free (smem_ctx, (ST_VOID *)chk_mem_node);
        }
      }  /* end if pointer matched expected */
    else		/* Whoops, free error					*/
      {
MEM_FREE_ERROR :

      MEMLOG_ERR1 (
              "Invalid Free (" S_FMT_PTR ")",del_mem_ptr);

      MEMLOG_CERR2 ("Free call made from file : %s, Line %d ",
                                        calling_file,
                                        calling_line);
      if (mem_chk_err)
        (*mem_chk_err)();
      }

/* Do linked list && overwrite checks, if enabled			*/
    if (m_check_list_enable)
      check_mem_list ();	/* check linked list. Works for all systems.	*/
    S_UNLOCK_UTIL_RESOURCES (); 
    return;
    }
  else
    _nd_m_free (smem_ctx, del_mem_ptr);
  }

#endif	/* defined(DEBUG_SISCO)	 x_m_* functions only for DEBUG mode	*/

/************************************************************************/
/************************************************************************/
/* "nd_m_*" functions below called if NOT debug mode			*/
/* (i.e. either m_mem_debug_saved == 0 OR compiled w/o DEBUG_SISCO	*/
/************************************************************************/

/************************************************************************/
/*			nd_m_malloc					*/
/************************************************************************/
#ifdef DEBUG_SISCO	/* Use "static" for DEBUG, so user can't call.	*/
static ST_VOID *_nd_m_malloc (SMEM_CONTEXT *smem_ctx, size_t size)
#else
ST_VOID *nd_m_malloc (SMEM_CONTEXT *smem_ctx, size_t size)
#endif
  {		      
CHK_DYN_MEM_NODE_NDEBUG *new_mem_node;

  assert (mem_chk_initialized);		/* init_mem_chk MUST be called first*/
  S_LOCK_UTIL_RESOURCES (); 

/* Update memory allocation statistics parameters			*/
  chk_curr_mem_count++;
  if (chk_curr_mem_count > chk_max_dyn_mem_ptrs)
    chk_max_dyn_mem_ptrs = chk_curr_mem_count;

  new_mem_node = (CHK_DYN_MEM_NODE_NDEBUG *) smem_malloc (smem_ctx, 
					     size + m_mem_overhead);
  if (new_mem_node == NULL)
    {
    if (mem_chk_err)
      (*mem_chk_err)();
    S_UNLOCK_UTIL_RESOURCES (); 
    return (NULL);
    }

  new_mem_node->chk_alloc_mem_size = size;
  S_UNLOCK_UTIL_RESOURCES (); 
  return ((ST_VOID *)(new_mem_node+1));
  }

/************************************************************************/
/*			nd_m_calloc					*/
/************************************************************************/
#ifdef DEBUG_SISCO	/* Use "static" for DEBUG, so user can't call.	*/
static ST_VOID *_nd_m_calloc (SMEM_CONTEXT *smem_ctx, size_t num, size_t size)
#else
ST_VOID *nd_m_calloc (SMEM_CONTEXT *smem_ctx, size_t num, size_t size)
#endif
  {		      
CHK_DYN_MEM_NODE_NDEBUG *new_mem_node;
size_t buf_size;

  assert (mem_chk_initialized);		/* init_mem_chk MUST be called first*/
  S_LOCK_UTIL_RESOURCES (); 

/* Update memory allocation statistics parameters			*/
  chk_curr_mem_count++;
  if (chk_curr_mem_count > chk_max_dyn_mem_ptrs)
    chk_max_dyn_mem_ptrs = chk_curr_mem_count;

  buf_size = num * size;
  new_mem_node = (CHK_DYN_MEM_NODE_NDEBUG *) 
  		smem_calloc (smem_ctx, 1, buf_size + m_mem_overhead);
  if (new_mem_node == NULL)
    {
    if (mem_chk_err)
      (*mem_chk_err)();
    S_UNLOCK_UTIL_RESOURCES (); 
    return (NULL);
    }

  new_mem_node->chk_alloc_mem_size = buf_size;
  S_UNLOCK_UTIL_RESOURCES (); 
  return ((ST_VOID *)(new_mem_node+1));
  }

/************************************************************************/
/*			nd_m_realloc					*/
/************************************************************************/
#ifdef DEBUG_SISCO	/* Use "static" for DEBUG, so user can't call.	*/
static ST_VOID *_nd_m_realloc (SMEM_CONTEXT *smem_ctx, ST_VOID *ptr, size_t size)
#else
ST_VOID *nd_m_realloc (SMEM_CONTEXT *smem_ctx, ST_VOID *ptr, size_t size)
#endif
  {		      
CHK_DYN_MEM_NODE_NDEBUG *old_mem_node, *new_mem_node;

  assert (mem_chk_initialized);		/* init_mem_chk MUST be called first*/
  S_LOCK_UTIL_RESOURCES (); 

/* Get the pointer to the control information				*/
  old_mem_node = ((CHK_DYN_MEM_NODE_NDEBUG *)ptr)-1;

/* Check for 'no realloc smaller' option				*/
/* If new size is less than old size, just don't use the system realloc	*/

  if (m_no_realloc_smaller &&
      (size <= old_mem_node->chk_alloc_mem_size))
    {
    new_mem_node = old_mem_node;
    }  
  else
    {
    new_mem_node = (CHK_DYN_MEM_NODE_NDEBUG *) 
    	smem_realloc (smem_ctx, (ST_VOID *) old_mem_node, 
                      old_mem_node->chk_alloc_mem_size + m_mem_overhead,
    		      size + m_mem_overhead);
    if (new_mem_node == NULL)
      {
      if (mem_chk_err)
        (*mem_chk_err)();
      S_UNLOCK_UTIL_RESOURCES (); 
      return (NULL);
      }
    }

  new_mem_node->chk_alloc_mem_size = size; /* update the header	*/
  S_UNLOCK_UTIL_RESOURCES (); 
  return ((ST_VOID *)(new_mem_node+1));
  }

/************************************************************************/
/*			nd_m_strdup					*/
/* No "static" version because only called by user.			*/
/************************************************************************/
#ifndef DEBUG_SISCO
ST_CHAR *nd_m_strdup (SMEM_CONTEXT *smem_ctx, ST_CHAR *str)
  {
ST_CHAR *new_str;

  new_str = (ST_CHAR *) nd_m_malloc (smem_ctx, strlen (str) + 1);
  strcpy (new_str, str);
  return (new_str);
  }
#endif

/************************************************************************/
/*			  nd_m_free					*/
/************************************************************************/
#ifdef DEBUG_SISCO	/* Use "static" for DEBUG, so user can't call.	*/
static ST_VOID _nd_m_free (SMEM_CONTEXT *smem_ctx, ST_VOID *del_mem_ptr)
#else
ST_VOID nd_m_free (SMEM_CONTEXT *smem_ctx, ST_VOID *del_mem_ptr)
#endif
  {		      
CHK_DYN_MEM_NODE_NDEBUG *chk_mem_node;

  assert (mem_chk_initialized);		/* init_mem_chk MUST be called first*/
  S_LOCK_UTIL_RESOURCES (); 

/* Update memory allocation statistics parameters			*/
  chk_curr_mem_count--;


  chk_mem_node = (CHK_DYN_MEM_NODE_NDEBUG *) del_mem_ptr;
  --chk_mem_node;
  smem_free (smem_ctx, (ST_VOID *)chk_mem_node);

  S_UNLOCK_UTIL_RESOURCES (); 
  }

/***************************************************************************/
/***************************************************************************/
/*                        dyn_mem_ptr_status				   */
/***************************************************************************/

ST_VOID dyn_mem_ptr_status ()
  {
#ifdef DEBUG_SISCO
  CHK_DYN_MEM_NODE *chk_mem_node;
  ST_ULONG count;
  ST_INT line;

  if (m_mem_debug_saved)
    {
    S_LOCK_UTIL_RESOURCES (); 

    count = list_get_sizeof (head_dyn_mem_list);
    if (count != chk_curr_mem_count)
      {
      MEMLOG_ERR2 ("List size (%lu) vs. count (%lu) mismatch", 
				count, (ST_ULONG) chk_curr_mem_count);
      }

    chk_mem_node = head_dyn_mem_list;
    MEMLOG_LF ();
    MEMLOG_DUMP0 ("DYNAMIC MEMORY POINTER TABLE ");
    MEMLOG_CDUMP0 ("-----------------------------  ");
    count = 1;
    while (count <= chk_curr_mem_count)
      {
      if (chk_mem_node == NULL)
        {
        MEMLOG_CDUMP0 ("Dump halted due to NULL pointer. Table corrupted.");
        break;		/* Break out of loop.		*/
        }
      MEMLOG_CDUMP4 ("Ptr " S_FMT_PTR "  Size %5u  File : %-12s #%04d",
		              chk_mem_node->chk_alloc_mem_ptr,
		              chk_mem_node->chk_alloc_mem_size,
			      chk_mem_node->file,
    			      chk_mem_node->line);

      chk_mem_node = (CHK_DYN_MEM_NODE *)
                      chk_mem_node->chk_mem_node_link.next;
      count++;

      line = 1;
      line++;
      if (line > 10)
        {
        line = 1;
        }
      }
 
    MEMLOG_CDUMP1 ("Total number of allocated pointers:   %lu",
		  (ST_ULONG) chk_curr_mem_count);
    MEMLOG_CDUMP1 ("Total amount of memory allocated:     %lu bytes ",
		  (ST_ULONG) chk_curr_mem_size);
    MEMLOG_CDUMP1 ("Maximum number of pointers allocated: %lu",
		  (ST_ULONG) chk_max_dyn_mem_ptrs);
    MEMLOG_CDUMP1 ("Maximum amount of memory allocated:   %lu bytes",
		  (ST_ULONG) chk_max_mem_allocated);

    S_UNLOCK_UTIL_RESOURCES (); 
    return;
    }
#endif			/* end of DEBUG_SISCO					   */
  }

/***************************************************************************/
/*                        dyn_mem_ptr_statistics			   */
/***************************************************************************/

ST_VOID dyn_mem_ptr_statistics (ST_BOOLEAN log_to_screen)
  {
#ifdef DEBUG_SISCO
  if (m_mem_debug_saved)
    {
    S_LOCK_UTIL_RESOURCES (); 
    if ( log_to_screen == SD_TRUE )
      {
#ifndef _WINDOWS
      printf("\nDYNAMIC MEMORY POINTER TABLE ");
      printf("\n-----------------------------  ");
#endif
      }
    else
      {
      MEMLOG_LF ();
      MEMLOG_DUMP0 ("DYNAMIC MEMORY POINTER TABLE ");
      MEMLOG_CDUMP0 ("-----------------------------  ");
      }
 
    if ( log_to_screen == SD_TRUE )
      {
#if !defined(_WINDOWS) || defined(_WIN32)
      printf ("\nTotal number of allocated pointers:   %lu",
             (ST_ULONG) chk_curr_mem_count);
      printf ("\nTotal amount of memory allocated:     %lu bytes ",
	     (ST_ULONG) chk_curr_mem_size);
      printf ("\nMaximum number of pointers allocated: %lu",
	     (ST_ULONG) chk_max_dyn_mem_ptrs);
      printf ("\nMaximum amount of memory allocated:   %lu bytes",
	     (ST_ULONG) chk_max_mem_allocated);
#endif
      }
    else
      {
      MEMLOG_CDUMP1 ("Total number of allocated pointers:   %lu",
		    (ST_ULONG) chk_curr_mem_count);
      MEMLOG_CDUMP1 ("Total amount of memory allocated:     %lu bytes ",
		    (ST_ULONG) chk_curr_mem_size);
      MEMLOG_CDUMP1 ("Maximum number of pointers allocated: %lu",
		    (ST_ULONG) chk_max_dyn_mem_ptrs);
      MEMLOG_CDUMP1 ("Maximum amount of memory allocated:   %lu bytes",
		    (ST_ULONG) chk_max_mem_allocated);
      }
    S_UNLOCK_UTIL_RESOURCES (); 
    return;
    }
#endif			/* end of DEBUG_SISCO					   */
  }

/************************************************************************/
/* 			check_mem_list					*/
/************************************************************************/

ST_VOID check_mem_list ()
  {
#ifdef DEBUG_SISCO
CHK_DYN_MEM_NODE *node;
CHK_DYN_MEM_NODE *prev;
CHK_DYN_MEM_NODE *next;
size_t len;
ST_VOID *curr;
ST_ULONG j;

  if (m_mem_debug_saved)
    {
    S_LOCK_UTIL_RESOURCES (); 
    if ((node = head_dyn_mem_list) == NULL)
      {
      S_UNLOCK_UTIL_RESOURCES (); 
      return;				/* list empty. Nothing to check	*/
      }
    for (j = 0;  j < chk_max_dyn_mem_ptrs;  j++)
      {
      /* should break out when j = chk_curr_mem_count	*/
      prev = (CHK_DYN_MEM_NODE *) node->chk_mem_node_link.prev;
      next = (CHK_DYN_MEM_NODE *) node->chk_mem_node_link.next;
      len = node->chk_alloc_mem_size;
      curr = node->chk_alloc_mem_ptr;

      /* check ptr to this node	*/
      if (curr != (ST_VOID *) (node + 1))
        {
        MEMLOG_ERR1 ("Current node pointer corrupted on node #%lu", j);
        MEMLOG_CERR5 (
          "Node = " S_FMT_PTR ", Prev = " S_FMT_PTR ", Next = " S_FMT_PTR ", Len = %u, Curr = " S_FMT_PTR,
          node, prev, next, len, curr);
        if (mem_chk_err)
          (*mem_chk_err)();
        }

      /* check link list ptrs	*/
      if (node != (CHK_DYN_MEM_NODE *) next->chk_mem_node_link.prev)
        {
        MEMLOG_ERR1 ("\nLinked list corrupted at node #%lu", j);
        MEMLOG_CERR6 (
            "Node=" S_FMT_PTR ", Prev=" S_FMT_PTR ", Next=" S_FMT_PTR ", Len=%5u, Curr=" S_FMT_PTR ", Next node's prev=" S_FMT_PTR,
            node, prev, next, len, curr, next->chk_mem_node_link.prev);
        if (mem_chk_err)
          (*mem_chk_err)();
        }

      /* Check for buffer overwrite					*/
      if (check_overwrite (node))
        {
        if(node->overwrite_prev_detected==SD_FALSE)
          {
          MEMLOG_ERR0 ("Memory overwrite at end of buffer detected");
          MEMLOG_CERR5 (
            "Node = " S_FMT_PTR ", Prev = " S_FMT_PTR ", Next = " S_FMT_PTR ", Len = %u, Curr = " S_FMT_PTR,
            node, prev, next, len, curr);
          MEMLOG_CERR2 ("Pointer was allocated at file: %s, line %d ",
     				  node->file,node->line);
          MEMLOG_CHERR ((ST_INT)(node->chk_alloc_mem_size + 
        		sizeof (CHK_DYN_MEM_NODE) +
		        m_num_pad_bytes), node);
          if (mem_chk_err)
            (*mem_chk_err)();
          node->overwrite_prev_detected=SD_TRUE;
          }
        }

      if (next == head_dyn_mem_list)	/* is this last node on list?	*/
        break;

      node = next;		/* point to next node in linked list	*/
      }	/* end "for"	*/

    if (j >= chk_max_dyn_mem_ptrs)
      {
      MEMLOG_ERR1 (
          "Last node (Node #%lu) does not point back to first.", j - 1);
      if (mem_chk_err)
        (*mem_chk_err)();
      }
    else if (j + 1 != chk_curr_mem_count)
      {
      MEMLOG_CERR0 ("Linked list current count corrupted.");
      MEMLOG_CERR2 ("Count = %lu,  Should be = %lu.",
          (ST_ULONG) chk_curr_mem_count, j + 1);
      if (mem_chk_err)
        (*mem_chk_err)();
      }
    S_UNLOCK_UTIL_RESOURCES (); 
    return;
    }
#endif	/* DEBUG_SISCO	*/
  }

/************************************************************************/
/************************************************************************/
/*			check_overwrite 				*/
/************************************************************************/

#ifdef DEBUG_SISCO
static ST_RET check_overwrite (CHK_DYN_MEM_NODE *node)
  {
ST_CHAR *bptr;
ST_CHAR *eptr;
ST_INT i;

/* Get a pointer to the byte after the user's data			*/
  bptr = (ST_CHAR *) (node +1);
  bptr += node->chk_alloc_mem_size;
  eptr = bptr + m_num_pad_bytes;

/* Verify the signature							*/
  i = 0;
  while (bptr < eptr)
    {
    if (*(bptr++) != m_pad_string[i])
      return (SD_TRUE);	/* Overwrite detected, return the error		*/

    if (++i >= m_num_pad_bytes) /* if we have wrapped the string  	*/
      i = 0;
    }  

/* All is OK with the end of this buffer				*/
  return (SD_FALSE);
  }
#endif

/************************************************************************/
/*			set_overwrite_value				*/
/************************************************************************/

#ifdef DEBUG_SISCO
static ST_VOID set_overwrite_value (CHK_DYN_MEM_NODE *node)
  {
ST_CHAR *bptr;
ST_CHAR *eptr;
ST_INT i;

/* Get a pointer to the byte after the user's data			*/
  bptr = (ST_CHAR *)(node+1);
  bptr += node->chk_alloc_mem_size;
  eptr = bptr + m_num_pad_bytes;

/* Set the signature							*/
  i = 0;
  while (bptr < eptr)
    {
    *(bptr++) = m_pad_string[i];
    if (++i >= m_num_pad_bytes) /* if we have wrapped the string	*/
      i = 0;
    }

  node->overwrite_prev_detected=SD_FALSE;
  }
#endif

/************************************************************************/
/*			u_smem_get_buf_detail				*/
/* SMEM calls this function to get additional info about a buffer.	*/
/* This functions fills in a string with the "usable" size (without	*/
/* overhead) and the file and line where the buffer was allocated.	*/
/*   bufDetail: ptr to caller string where buffer info written.		*/
/************************************************************************/
#ifdef DEBUG_SISCO
ST_CHAR *u_smem_get_buf_detail (ST_VOID *bufPtr, size_t bufSize)
  {
static ST_CHAR bufDetail [200];	/* static so caller can use after return*/
CHK_DYN_MEM_NODE *mem_node;	/* Need filename, line # from here	*/

  if (m_mem_debug_saved)
    {
    mem_node = (CHK_DYN_MEM_NODE *) bufPtr;
    sprintf (bufDetail, "Usable Size %5u  File : %-12s #%04d",
           bufSize - m_mem_overhead, mem_node->file, mem_node->line);
    }
  else				/* Calling file and line never saved	*/
    sprintf (bufDetail, "Usable Size %5u", bufSize - m_mem_overhead);

  return (bufDetail);
  }
#endif	/* DEBUG_SISCO	*/
