/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1986-2010, All Rights Reserved				*/
/*									*/
/* MODULE NAME : memlog.h						*/
/* PRODUCT(S)  : General Use						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the declarations of the dynamic memory 	*/
/*	handling functions.						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/21/10  JRB     09    Use "Base" macros SLOG_*, SLOGC_*, SLOGH.	*/
/*			   Use SLOGCALWAYS* for MEMLOG_CDUMP*.		*/
/* 03/13/02  JRB     08    MEMLOG_PAUSE*, MEMLOG_LF macros do nothing.	*/
/* 09/13/99  MDE     07    Added SD_CONST modifiers			*/
/* 10/08/98  MDE     06    Migrated to updated SLOG interface		*/
/* 04/17/96  JRB     05    Del #define THISFILE thisFileName.		*/
/* 04/17/96  JRB     04    Del mem_sLogCtrl. Use new sLogCtrl in slog.c	*/
/* 04/01/94  MDE     03    Removed THISFILE defines, changed THISFILE 	*/
/*			   to thisFileName				*/
/* 03/29/94  DSF     02    Corrected definition for THISFILE		*/
/* 12/10/93  DSF     01    Added MEMLOG_CHERR macro			*/
/************************************************************************/

#ifndef MEMLOG_INCLUDED
#define MEMLOG_INCLUDED

#ifdef __cplusplus		
extern "C" {
#endif

#include "slog.h"

/************************************************************************/
/************************************************************************/

extern SD_CONST ST_CHAR *SD_CONST _mem_log_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mem_log_calloc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mem_log_malloc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mem_log_realloc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mem_log_free_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mem_log_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mem_log_dump_logstr;


#define MEMLOG_LF()

#define MEMLOG_CALLSTACK(a) slogCallStack (sLogCtrl,a);

#define MEMLOG_PAUSE(msg)
#define MEMLOG_PAUSEERR(msg)

#define MEMLOG_MALLOC2(a,b,c) \
    SLOG_2 (chk_debug_en & MEM_LOG_MALLOC,_mem_log_malloc_logstr,a,b,c)

#define MEMLOG_CMALLOC1(a,b) \
    SLOGC_1 (chk_debug_en & MEM_LOG_MALLOC,a,b)
#define MEMLOG_CMALLOC2(a,b,c) \
    SLOGC_2 (chk_debug_en & MEM_LOG_MALLOC,a,b,c)
#define MEMLOG_CMALLOC3(a,b,c,d) \
    SLOGC_3 (chk_debug_en & MEM_LOG_MALLOC,a,b,c,d)

#define MEMLOG_CALLOC3(a,b,c,d) \
    SLOG_3 (chk_debug_en & MEM_LOG_CALLOC,_mem_log_calloc_logstr,a,b,c,d)

#define MEMLOG_CCALLOC1(a,b) \
    SLOGC_1 (chk_debug_en & MEM_LOG_CALLOC,a,b)
#define MEMLOG_CCALLOC2(a,b,c) \
    SLOGC_2 (chk_debug_en & MEM_LOG_CALLOC,a,b,c)
#define MEMLOG_CCALLOC3(a,b,c,d) \
    SLOGC_3 (chk_debug_en & MEM_LOG_CALLOC,a,b,c,d)

#define MEMLOG_REALLOC3(a,b,c,d) \
    SLOG_3 (chk_debug_en & MEM_LOG_REALLOC,_mem_log_realloc_logstr,a,b,c,d)

#define MEMLOG_CREALLOC1(a,b) \
    SLOGC_1 (chk_debug_en & MEM_LOG_REALLOC,a,b)
#define MEMLOG_CREALLOC2(a,b,c) \
    SLOGC_2 (chk_debug_en & MEM_LOG_REALLOC,a,b,c)
#define MEMLOG_CREALLOC3(a,b,c,d) \
    SLOGC_3 (chk_debug_en & MEM_LOG_REALLOC,a,b,c,d)

#define MEMLOG_FREE1(a,b) \
    SLOG_1 (chk_debug_en & MEM_LOG_FREE,_mem_log_free_logstr,a,b)

#define MEMLOG_CFREE2(a,b,c) \
    SLOGC_2 (chk_debug_en & MEM_LOG_FREE,a,b,c)
#define MEMLOG_CFREE3(a,b,c,d) \
    SLOGC_3 (chk_debug_en & MEM_LOG_FREE,a,b,c,d)

#define MEMLOG_ERR0(a) \
    SLOG_0 (chk_debug_en & MEM_LOG_ERR,_mem_log_err_logstr,a)
#define MEMLOG_ERR1(a,b) \
    SLOG_1 (chk_debug_en & MEM_LOG_ERR,_mem_log_err_logstr,a,b)
#define MEMLOG_ERR2(a,b,c) \
    SLOG_2 (chk_debug_en & MEM_LOG_ERR,_mem_log_err_logstr,a,b,c)

#define MEMLOG_CERR0(a) \
    SLOGC_0 (chk_debug_en & MEM_LOG_ERR,a)
#define MEMLOG_CERR1(a,b) \
    SLOGC_1 (chk_debug_en & MEM_LOG_ERR,a,b)
#define MEMLOG_CERR2(a,b,c) \
    SLOGC_2 (chk_debug_en & MEM_LOG_ERR,a,b,c)
#define MEMLOG_CERR3(a,b,c,d) \
    SLOGC_3 (chk_debug_en & MEM_LOG_ERR,a,b,c,d)
#define MEMLOG_CERR4(a,b,c,d,e) \
    SLOGC_4 (chk_debug_en & MEM_LOG_ERR,a,b,c,d,e)
#define MEMLOG_CERR5(a,b,c,d,e,f) \
    SLOGC_5 (chk_debug_en & MEM_LOG_ERR,a,b,c,d,e,f)
#define MEMLOG_CERR6(a,b,c,d,e,f,g) \
    SLOGC_6 (chk_debug_en & MEM_LOG_ERR,a,b,c,d,e,f,g)

#define MEMLOG_CHERR(a,b) \
    SLOGH (chk_debug_en & MEM_LOG_ERR,a,b)

#define MEMLOG_DUMP0(a) {\
                       _slog (sLogCtrl,_mem_log_dump_logstr,\
		             thisFileName,__LINE__,a);\
                    }
#define MEMLOG_DUMP1(a,b) {\
                       _slog (sLogCtrl,_mem_log_dump_logstr,\
		             thisFileName,__LINE__,a,b);\
                    }

/* Continuation macros. Do same as "SLOGCALWAYS*".	*/
#define MEMLOG_CDUMP0	SLOGCALWAYS0
#define MEMLOG_CDUMP1	SLOGCALWAYS1
#define MEMLOG_CDUMP2	SLOGCALWAYS2
#define MEMLOG_CDUMP3	SLOGCALWAYS3
#define MEMLOG_CDUMP4	SLOGCALWAYS4
#define MEMLOG_CDUMP5	SLOGCALWAYS5

#ifdef __cplusplus
}
#endif


#endif /* memlog.h already included */

