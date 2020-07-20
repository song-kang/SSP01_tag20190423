/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2003 - 2010, All Rights Reserved     	                */
/*                                                                      */
/* MODULE NAME : sslLog.h                                          	*/
/* PRODUCT(S)  : Sisco MMS Security Toolkit				*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* SSL Toolkit logging macros						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/21/10  EJV           Added _ssleLogErrorQueue.			*/
/* 05/28/08  EJV    06     Merged with non-Marben code.			*/
/* 04/08/08  JRB    05     Chg () to (ST_VOID) in prototype.		*/
/* 10/20/03  ASK    04     Added ssleGetDebugSel proto. Modlog cleanup.	*/
/* 09/03/03  EJV    03     Made ssle_debug_sel ST_UINT			*/
/*			   Moved SSLE_LOG_* masks from sslEngine.h	*/
/* 08/21/03  EJV    02     Made ssle_debug_sel extern			*/
/* 08/21/02  ASK    01     Created					*/
/************************************************************************/

#ifndef SSLE_LOG_INCLUDED
#define SSLE_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "slog.h"

/* Log control masks */
#define SSLE_LOG_ERR		0x00000001
#define SSLE_LOG_NERR		0x00000002
#define SSLE_LOG_FLOW		0x00000004 
#define SSLE_LOG_DATA		0x00000008 
#define SSLE_LOG_DEBUG		0x00000010

extern ST_UINT ssle_debug_sel;

/* Function so user can set log masks */
ST_UINT    *ssleGetDebugSel(ST_VOID);
ST_UINT    *ssleGetASN1DebugSel(ST_VOID);
/* Function so user can set memory debug flag */
ST_BOOLEAN *ssleGetMemDebug (ST_VOID);

/* Internal SSL Toolkit Utility Functions */
ST_VOID _ssleLogError(ST_ULONG err);
ST_VOID _ssleLogErrorQueue (ST_CHAR *errBuf, ST_UINT errBufLen);

/* The log macros */
#define SSLE_LOG_ERR0(a) \
	  SLOG_0 (ssle_debug_sel & SSLE_LOG_ERR, "SSLE_LOG_ERR", a)
#define SSLE_LOG_ERR1(a,b) \
	  SLOG_1 (ssle_debug_sel & SSLE_LOG_ERR, "SSLE_LOG_ERR", a,b)
#define SSLE_LOG_ERR2(a,b,c) \
	  SLOG_2 (ssle_debug_sel & SSLE_LOG_ERR, "SSLE_LOG_ERR", a,b,c)
#define SSLE_LOG_ERR3(a,b,c,d) \
	  SLOG_3 (ssle_debug_sel & SSLE_LOG_ERR, "SSLE_LOG_ERR", a,b,c,d)
#define SSLE_LOG_ERR4(a,b,c,d,e) \
	  SLOG_4 (ssle_debug_sel & SSLE_LOG_ERR, "SSLE_LOG_ERR", a,b,c,d,e)

#define SSLE_LOG_NERR0(a) \
	  SLOG_0 (ssle_debug_sel & SSLE_LOG_NERR, "SSLE_LOG_NERR", a)
#define SSLE_LOG_NERR1(a,b) \
	  SLOG_1 (ssle_debug_sel & SSLE_LOG_NERR, "SSLE_LOG_NERR", a,b)
#define SSLE_LOG_NERR2(a,b,c) \
	  SLOG_2 (ssle_debug_sel & SSLE_LOG_NERR, "SSLE_LOG_NERR", a,b,c)
#define SSLE_LOG_NERR3(a,b,c,d) \
	  SLOG_3 (ssle_debug_sel & SSLE_LOG_NERR, "SSLE_LOG_NERR", a,b,c,d)
#define SSLE_LOG_NERR4(a,b,c,d,e) \
	  SLOG_4 (ssle_debug_sel & SSLE_LOG_NERR, "SSLE_LOG_NERR", a,b,c,d,e)

#define SSLE_LOG_FLOW0(a) \
	  SLOG_0 (ssle_debug_sel & SSLE_LOG_FLOW, "SSLE_LOG_FLOW", a)
#define SSLE_LOG_FLOW1(a,b) \
	  SLOG_1 (ssle_debug_sel & SSLE_LOG_FLOW, "SSLE_LOG_FLOW", a,b)
#define SSLE_LOG_FLOW2(a,b,c) \
	  SLOG_2 (ssle_debug_sel & SSLE_LOG_FLOW, "SSLE_LOG_FLOW", a,b,c)
#define SSLE_LOG_FLOW3(a,b,c,d) \
	  SLOG_3 (ssle_debug_sel & SSLE_LOG_FLOW, "SSLE_LOG_FLOW", a,b,c,d)
#define SSLE_LOG_FLOW4(a,b,c,d,e) \
	  SLOG_4 (ssle_debug_sel & SSLE_LOG_FLOW, "SSLE_LOG_FLOW", a,b,c,d,e)

#define SSLE_LOG_DEBUG0(a) \
	  SLOG_0 (ssle_debug_sel & SSLE_LOG_DEBUG, "SSLE_LOG_DEBUG", a)
#define SSLE_LOG_DEBUG1(a,b) \
	  SLOG_1 (ssle_debug_sel & SSLE_LOG_DEBUG, "SSLE_LOG_DEBUG", a,b)
#define SSLE_LOG_DEBUG2(a,b,c) \
	  SLOG_2 (ssle_debug_sel & SSLE_LOG_DEBUG, "SSLE_LOG_DEBUG", a,b,c)
#define SSLE_LOG_DEBUG3(a,b,c,d) \
	  SLOG_3 (ssle_debug_sel & SSLE_LOG_DEBUG, "SSLE_LOG_DEBUG", a,b,c,d)
#define SSLE_LOG_DEBUG4(a,b,c,d,e) \
	  SLOG_4 (ssle_debug_sel & SSLE_LOG_DEBUG, "SSLE_LOG_DEBUG", a,b,c,d,e)

#define SSLE_LOG_DATAH(a,b) \
	  SLOGH (ssle_debug_sel & SSLE_LOG_DATA,a,b)

/* 'C' macros stand for 'Continuation' */
#define SSLE_LOG_CERR0(a) \
	  SLOGC_0 (ssle_debug_sel & SSLE_LOG_ERR, a)
#define SSLE_LOG_CERR1(a,b) \
	  SLOGC_1 (ssle_debug_sel & SSLE_LOG_ERR, a,b)
#define SSLE_LOG_CERR2(a,b,c) \
	  SLOGC_2 (ssle_debug_sel & SSLE_LOG_ERR, a,b,c)
#define SSLE_LOG_CERR3(a,b,c,d) \
	  SLOGC_3 (ssle_debug_sel & SSLE_LOG_ERR, a,b,c,d)
#define SSLE_LOG_CERR4(a,b,c,d,e) \
	  SLOGC_4 (ssle_debug_sel & SSLE_LOG_ERR, a,b,c,d,e)

#define SSLE_LOG_CNERR0(a) \
	  SLOGC_0 (ssle_debug_sel & SSLE_LOG_NERR, a)
#define SSLE_LOG_CNERR1(a,b) \
	  SLOGC_1 (ssle_debug_sel & SSLE_LOG_NERR, a,b)
#define SSLE_LOG_CNERR2(a,b,c) \
	  SLOGC_2 (ssle_debug_sel & SSLE_LOG_NERR, a,b,c)
#define SSLE_LOG_CNERR3(a,b,c,d) \
	  SLOGC_3 (ssle_debug_sel & SSLE_LOG_NERR, a,b,c,d)
#define SSLE_LOG_CNERR4(a,b,c,d,e) \
	  SLOGC_4 (ssle_debug_sel & SSLE_LOG_NERR, a,b,c,d,e)

#define SSLE_LOG_CFLOW0(a) \
	  SLOGC_0 (ssle_debug_sel & SSLE_LOG_FLOW, a)
#define SSLE_LOG_CFLOW1(a,b) \
	  SLOGC_1 (ssle_debug_sel & SSLE_LOG_FLOW, a,b)
#define SSLE_LOG_CFLOW2(a,b,c) \
	  SLOGC_2 (ssle_debug_sel & SSLE_LOG_FLOW, a,b,c)
#define SSLE_LOG_CFLOW3(a,b,c,d) \
	  SLOGC_3 (ssle_debug_sel & SSLE_LOG_FLOW, a,b,c,d)
#define SSLE_LOG_CFLOW4(a,b,c,d,e) \
	  SLOGC_4 (ssle_debug_sel & SSLE_LOG_FLOW, a,b,c,d,e)

#define SSLE_LOG_CDEBUG0(a) \
	  SLOGC_0 (ssle_debug_sel & SSLE_LOG_DEBUG, a)
#define SSLE_LOG_CDEBUG1(a,b) \
	  SLOGC_1 (ssle_debug_sel & SSLE_LOG_DEBUG, a,b)
#define SSLE_LOG_CDEBUG2(a,b,c) \
	  SLOGC_2 (ssle_debug_sel & SSLE_LOG_DEBUG, a,b,c)
#define SSLE_LOG_CDEBUG3(a,b,c,d) \
	  SLOGC_3 (ssle_debug_sel & SSLE_LOG_DEBUG, a,b,c,d)
#define SSLE_LOG_CDEBUG4(a,b,c,d,e) \
	  SLOGC_4 (ssle_debug_sel & SSLE_LOG_DEBUG, a,b,c,d,e)

/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* SSLE_LOG_INCLUDED */
/************************************************************************/
