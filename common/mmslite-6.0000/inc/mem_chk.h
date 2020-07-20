/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1986-2009 All Rights Reserved       			*/
/*									*/
/* MODULE NAME : mem_chk.h						*/
/* PRODUCT(S)  : General Use						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the declarations of the dynamic memory 	*/
/*	handling functions.						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/12/12  JRB           Chg args to size_t like system functs.	*/
/* 03/30/11  DSF           Added sCheckHeap				*/
/* 03/22/11  MDE     19    Added sLogheap defines			*/
/* 01/14/11  MDE     18    Added sLogheap				*/
/* 07/23/09  EJV     17    Added set_mem_report_path for Vista UAC.	*/
/* 02/06/04  EJV     16    Added dyn_mem_ptr_status2.			*/
/* 04/14/03  DSF     15    Cleanup					*/
/* 04/08/03  DSF     14    added m_mem_crt_debug			*/
/* 04/04/03  DSF     13    new/delete checks for MFC			*/
/* 08/20/01  JRB     12    chk_* functions chged to nd_chk_*.		*/
/*			   chk_* names are now ALWAYS macros.		*/
/*			   Make 4 sets of macros depending on whether	*/
/*			   DEBUG_SISCO and/or SMEM_ENABLE defined.	*/
/*			   Don't define SMEM_ENABLE, do from makefiles.	*/
/*			   Remove logging backward compatibility stuff.	*/
/* 08/06/01  RKR     11    S_THISFILE was removed, need a thisFileName	*/
/* 03/19/01  JRB     10    Move SMEM context defs to "smem.h".		*/
/* 01/25/01  DSF     09    new/delete checks				*/
/* 11/01/00  MDE     08    Additional SMEM work				*/
/* 01/21/00  MDE     07    Added SMEM support				*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 01/26/98  MDE     05    Added 'chk_strdup'				*/
/* 12/08/98  MDE     04    Added 'ST_BOOLEAN m_auto_hw_log'		*/
/* 10/08/98  MDE     03    Migrated to updated SLOG interface		*/
/* 10/14/97  DSF     02    m_bad_ptr_val is now a pointer		*/
/* 09/16/97  DSF     01    chk_debug_en is UINT				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MEM_CHK_INCLUDED
#define MEM_CHK_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifdef SMEM_ENABLE
#include "smem.h"
#endif

/************************************************************************/
/* MEM_CHK MACROS and FUNCTION PROTOTYPES				*/
/* Memory allocation macros. There are 4 sets of macros depending on	*/
/* DEBUG_SISCO and SMEM_ENABLE. Each set of macros calls a unique set	*/
/* of functions.							*/
/*   The DEBUG macros use thisFileName (instead of __FILE__) to reduce	*/
/* memory usage. Any file using these macros MUST contain the following	*/
/* statement:								*/
/*   static char *thisFileName = __FILE__;				*/
/************************************************************************/

#if defined(SMEM_ENABLE)
 #if defined(DEBUG_SISCO)
  #define M_MALLOC(ctx,x)	x_m_malloc  (ctx,x,  thisFileName,__LINE__)
  #define M_CALLOC(ctx,x,y)	x_m_calloc  (ctx,x,y,thisFileName,__LINE__)
  #define M_REALLOC(ctx,x,y)	x_m_realloc (ctx,x,y,thisFileName,__LINE__)
  #define M_STRDUP(ctx,x)	x_m_strdup  (ctx,x,  thisFileName,__LINE__)
  #define M_FREE(ctx,x)		x_m_free    (ctx,x,  thisFileName,__LINE__)

  #define chk_malloc(x)		x_m_malloc  (MSMEM_GEN,x,  thisFileName,__LINE__)
  #define chk_calloc(x,y)	x_m_calloc  (MSMEM_GEN,x,y,thisFileName,__LINE__)
  #define chk_realloc(x,y)	x_m_realloc (MSMEM_GEN,x,y,thisFileName,__LINE__)
  #define chk_strdup(x)		x_m_strdup  (MSMEM_GEN,x,  thisFileName,__LINE__)
  #define chk_free(x)		x_m_free    (MSMEM_GEN,x,  thisFileName,__LINE__)

  ST_VOID *x_m_malloc  (SMEM_CONTEXT *smem_ctx, size_t size,  
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);

  ST_VOID *x_m_calloc  (SMEM_CONTEXT *smem_ctx, size_t num, 
			size_t size,  
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);

  ST_VOID *x_m_realloc (SMEM_CONTEXT *smem_ctx, ST_VOID *old, 
			size_t size, 
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);
  ST_VOID  x_m_free    (SMEM_CONTEXT *smem_ctx, ST_VOID *ptr, 
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);
  ST_CHAR *x_m_strdup  (SMEM_CONTEXT *smem_ctx, ST_CHAR *str, 
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);

 #else	/* !DEBUG_SISCO	*/

  #define M_MALLOC(ctx,x)	nd_m_malloc  (ctx,x)
  #define M_CALLOC(ctx,x,y)	nd_m_calloc  (ctx,x,y)
  #define M_REALLOC(ctx,x,y)	nd_m_realloc (ctx,x,y)
  #define M_STRDUP(ctx,x)	nd_m_strdup  (ctx,x)
  #define M_FREE(ctx,x)		nd_m_free    (ctx,x)
  
  #define chk_malloc(x)		nd_m_malloc  (MSMEM_GEN,x)
  #define chk_calloc(x,y)	nd_m_calloc  (MSMEM_GEN,x,y)
  #define chk_realloc(x,y)	nd_m_realloc (MSMEM_GEN,x,y)
  #define chk_strdup(x)		nd_m_strdup  (MSMEM_GEN,x)
  #define chk_free(x)		nd_m_free    (MSMEM_GEN,x)

  ST_VOID *nd_m_malloc  (SMEM_CONTEXT *smem_ctx, size_t size);
  ST_VOID *nd_m_calloc  (SMEM_CONTEXT *smem_ctx, size_t num, size_t size);
  ST_VOID *nd_m_realloc (SMEM_CONTEXT *smem_ctx, ST_VOID *old, size_t size);
  ST_VOID  nd_m_free    (SMEM_CONTEXT *smem_ctx, ST_VOID *ptr);
  ST_CHAR *nd_m_strdup  (SMEM_CONTEXT *smem_ctx, ST_CHAR *str);

 #endif	/* !DEBUG_SISCO	*/
#else	/* !SMEM_ENABLE	*/
 #if defined(DEBUG_SISCO)

  #define M_MALLOC(ctx,x)	x_chk_malloc  (x,  thisFileName,__LINE__)
  #define M_CALLOC(ctx,x,y)	x_chk_calloc  (x,y,thisFileName,__LINE__)
  #define M_REALLOC(ctx,x,y)	x_chk_realloc (x,y,thisFileName,__LINE__)
  #define M_STRDUP(ctx,x)	x_chk_strdup  (x,  thisFileName,__LINE__)
  #define M_FREE(ctx,x)		x_chk_free    (x,  thisFileName,__LINE__)

  #define chk_malloc(x)		x_chk_malloc  (x,  thisFileName,__LINE__)
  #define chk_calloc(x,y)	x_chk_calloc  (x,y,thisFileName,__LINE__)
  #define chk_realloc(x,y)	x_chk_realloc (x,y,thisFileName,__LINE__)
  #define chk_strdup(x)		x_chk_strdup  (x,  thisFileName,__LINE__)
  #define chk_free(x)		x_chk_free    (x,  thisFileName,__LINE__)

  ST_VOID *x_chk_realloc (ST_VOID *old, 
			size_t size, 
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);
  ST_VOID *x_chk_malloc  (size_t size,  
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);
  ST_VOID *x_chk_calloc  (size_t num,
			size_t size,  
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);
  ST_CHAR *x_chk_strdup  (ST_CHAR *str, 
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);
  ST_VOID  x_chk_free    (ST_VOID *old, 
			SD_CONST ST_CHAR *SD_CONST file, 
			SD_CONST ST_INT line);

 #else	/* !DEBUG_SISCO	*/

  #define M_MALLOC(ctx,x)	nd_chk_malloc  (x)
  #define M_CALLOC(ctx,x,y)	nd_chk_calloc  (x,y)
  #define M_REALLOC(ctx,x,y)	nd_chk_realloc (x,y)
  #define M_STRDUP(ctx,x)	nd_chk_strdup  (x)
  #define M_FREE(ctx,x)		nd_chk_free    (x)

  #define chk_malloc(x)		nd_chk_malloc  (x)
  #define chk_calloc(x,y)	nd_chk_calloc  (x,y)
  #define chk_realloc(x,y)	nd_chk_realloc (x,y)
  #define chk_strdup(x)		nd_chk_strdup  (x)
  #define chk_free(x)		nd_chk_free    (x)

  ST_VOID *nd_chk_malloc (size_t size);
  ST_VOID *nd_chk_calloc (size_t num, size_t size);
  ST_VOID *nd_chk_realloc (ST_VOID *old, size_t size);
  ST_CHAR *nd_chk_strdup (ST_CHAR *str);
  ST_VOID nd_chk_free (ST_VOID *ptr);

 #endif	/* !DEBUG_SISCO	*/
#endif	/* !SMEM_ENABLE	*/


/************************************************************************/
/* chk_debug_en bit assignments						*/
/* These are the defines to be used by all new applications		*/
#define MEM_LOG_ERR	0x0001 
#define MEM_LOG_CALLOC	0x0002 
#define MEM_LOG_MALLOC	0x0004 
#define MEM_LOG_REALLOC	0x0008 
#define MEM_LOG_FREE	0x0010 

extern ST_UINT chk_debug_en;

/************************************************************************/
/* Misc Memory functions.						*/
/************************************************************************/

ST_VOID init_mem_chk (ST_VOID);
ST_VOID dyn_mem_ptr_status (ST_VOID);
ST_UINT dyn_mem_ptr_status2 (ST_VOID *marker_ptr);
ST_VOID dyn_mem_ptr_statistics (ST_BOOLEAN log_to_screen);
ST_VOID check_mem_list (ST_VOID);
ST_RET chk_alloc_ptr (ST_VOID *ptr);

/* when defined(_WIN32) && defined(_DEBUG) && !defined(MMS_LITE) 	*/
/* this function sets path to MemReport.log file for Vista UAC		*/
ST_RET set_mem_report_path (ST_UCHAR *path);

/************************************************************************/
/* fatal error service function pointers, used for unrecoverable errors */
/************************************************************************/
#define MEM_ERR_EXIT_CODE 	2001

extern ST_VOID *(*m_memerr_fun)(size_t size);               /* for malloc */
extern ST_VOID *(*c_memerr_fun)(size_t num, size_t size); /* for calloc */
extern ST_VOID *(*r_memerr_fun)(ST_VOID *old, size_t size);    /* for realloc*/

/************************************************************************/
/* statistics								*/
/************************************************************************/
extern ST_UINT32 chk_curr_mem_count;
extern ST_UINT32 chk_max_dyn_mem_ptrs;
extern size_t chk_max_mem_allocated;
extern size_t chk_curr_mem_size;


/************************************************************************/
/************************************************************************/
/* HEAP checking functions and macros					*/

/* These defines are used to control the level or print in chk_hp	*/
#define DEBUG_HEAP_SUM		0x0001
#define DEBUG_HEAP_ENTRY	0x0002
#define DEBUG_HEAP_ALL		0x0003

#ifdef HEAP_CHECK_ENABLE
#define HEAP_CHECK() chk_hp (0, (ST_CHAR *) __FILE__, __LINE__)
#else
#define HEAP_CHECK() 
#endif

#define SLOG_HEAP_HEAPWALK		0x0001
#define SLOG_HEAP_HEAPWALK_DETAIL	0x0002
#define SLOG_HEAP_GLOBAL_STATS		0x0004
#define SLOG_HEAP_PROCESS_HEAPS		0x0008
#define SLOG_HEAP_PROCESS_HEAPS_DETAIL	0x0010
#define SLOG_HEAP_MEM_CHK		0x0020
extern ST_INT _slogHeapDumpDetail;
int sLogHeap (ST_VOID);
int sCheckHeap (ST_VOID);

ST_RET msoft_heap_check (ST_INT16);
ST_RET chk_hp (ST_INT16 prt_mask,ST_CHAR *file,ST_INT line);

/************************************************************************/

extern ST_VOID *m_bad_ptr_val;

/* This variable used to enable MSOFT heap check calls on every alloc	*/
/* and free call. Also sets the unused heap memory to 0x55's	       	*/
extern ST_BOOLEAN m_heap_check_enable;

/* This variable used to enable list validation and overwrite checking	*/
/* on every alloc  and free call. 					*/
extern ST_BOOLEAN m_check_list_enable;

/* Set this = SD_FALSE to speed up the debug version. When SD_TRUE, it  */
/* enables searching the memory list for the element before accessing	*/
/* the memory during chk_realloc and chk_free calls			*/
extern ST_BOOLEAN m_find_node_enable;

/* This variable will cause chk_realloc to not realloc when the new	*/
/* size is smaller than the old size					*/
extern ST_BOOLEAN m_no_realloc_smaller;

/* Dump memory table whenever high water mark is advanced */
extern ST_BOOLEAN m_auto_hw_log;

extern ST_CHAR *m_pad_string;
extern ST_INT m_num_pad_bytes;

extern ST_BOOLEAN m_fill_en;
extern ST_UCHAR m_fill_byte;

extern ST_BOOLEAN m_mem_debug;

extern ST_BOOLEAN m_mem_crt_debug;

/* This function pointer can be set to select a function to be called 	*/
/* in case of memory error.						*/

extern ST_VOID (*mem_chk_err)(ST_VOID);

/* Read only */
extern ST_UINT m_mem_overhead; 


/************************************************************************/
/************************************************************************/
#ifdef __cplusplus
}
#endif



#ifdef __cplusplus
/************************************************************************/
/* deal with new and delete						*/
/************************************************************************/
#if defined (DEBUG_SISCO) && defined (DEBUG_NEW_SISCO) 

#include <new>

void * operator new(size_t nSize) throw (std::bad_alloc);
void * operator new(size_t nSize, const char *fileName, const int lineNum) throw (std::bad_alloc);
void * operator new(size_t nSize, const std::nothrow_t&) throw ();
void * operator new[](size_t nSize) throw (std::bad_alloc);
void * operator new[](size_t nSize, const char *fileName, const int lineNum) throw (std::bad_alloc);
void * operator new[](size_t nSize, const std::nothrow_t&) throw ();
void operator delete(void *p) throw ();
void operator delete(void *p, const char *fileName, const int lineNum) throw ();
void operator delete[](void *p) throw ();
void operator delete[](void *p, const char *fileName, const int lineNum) throw ();

#ifndef MEM_CHK
#define new new(thisFileName, __LINE__)
#endif

#endif

#if defined (DEBUG_SISCO) && defined (DEBUG_NEW_MFC) 

#include <new>

#if !defined (__AFX_H__)

#include <windows.h>

#define THIS_FILE          __FILE__

void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
void __cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine);

void * __cdecl operator new[](size_t);
void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete[](void* p, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete[](void *);

#endif /* AFX */

#endif /* DEBUG_NEW_MFC */

#endif /* __cplusplus */


#endif /* mem_chk.h already included */
