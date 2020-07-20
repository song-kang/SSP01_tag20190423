/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996-2002, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mvl_log.h 						*/
/* PRODUCT(S)  : Tool							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/22/08  JRB     14    Del unused MVLULOG_TIMING macros.		*/
/* 04/09/07  MDE     13    Enhanced filtered logging 			*/
/* 09/12/05  JRB     12    Added more ERR, NERR macros			*/
/* 07/13/05  MDE     11    Added more CNERR macros			*/
/* 08/12/04  JRB     10    Fix MVLU_LOG_CTIMING.. macros.		*/
/* 12/12/02  JRB     09    Use new "Base" macros SLOG_*, SLOGC_*, SLOGH.*/
/* 07/31/01  MDE     08    Added mvl_conn_filtered_logging		*/
/* 03/28/01  JRB     07    Add ..ACSEDATA3 macro.			*/
/* 03/22/01  GLB     06    Added: MVLULOG_DEBUG, MVLULOG_TIMING         */
/* 12/13/99  JRB     05    Delete ";" from non-debug macros. Get extra	*/
/*			   ";" when expanded (doesn't always compile).	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     03    Added FLOW3, CFLOW3 macros			*/
/* 10/08/98  MDE     02    Migrated to updated SLOG interface		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#ifndef MVL_LOG_INCLUDED
#define MVL_LOG_INCLUDED

#include "slog.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* MMS-EASE Lite logging control 					*/

#define MVLLOG_ERR		0x00000001
#define MVLLOG_NERR		0x00000002
#define MVLLOG_ACSE		0x00000040
#define MVLLOG_ACSEDATA		0x00000080
#define MVLULOG_FLOW		0x00000200
#define MVLULOG_DEBUG		0x00000400


/* For backward compatability only */
#define mvl_log_mask mvl_debug_sel
extern ST_UINT mvl_debug_sel;

/* Allows filtering of ACSE data logging by connection */
extern ST_BOOLEAN mvl_conn_filtered_logging;

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST _mvllog_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mvllog_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mvllog_acse_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mvllog_acsedata_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mvlulog_flow_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mvlulog_debug_logstr;


/*********************** ERR Macros *************************************/
#define MVL_LOG_ERR0(a) \
	SLOG_0 (mvl_debug_sel & MVLLOG_ERR,_mvllog_err_logstr,a)
#define MVL_LOG_ERR1(a,b) \
	SLOG_1 (mvl_debug_sel & MVLLOG_ERR,_mvllog_err_logstr,a,b)
#define MVL_LOG_ERR2(a,b,c) \
	SLOG_2 (mvl_debug_sel & MVLLOG_ERR,_mvllog_err_logstr,a,b,c)
#define MVL_LOG_ERR3(a,b,c,d) \
	SLOG_3 (mvl_debug_sel & MVLLOG_ERR,_mvllog_err_logstr,a,b,c,d)
#define MVL_LOG_ERR4(a,b,c,d,e) \
	SLOG_4 (mvl_debug_sel & MVLLOG_ERR,_mvllog_err_logstr,a,b,c,d,e)
#define MVL_LOG_ERR5(a,b,c,d,e,f) \
	SLOG_5 (mvl_debug_sel & MVLLOG_ERR,_mvllog_err_logstr,a,b,c,d,e,f)

#define MVL_LOG_CERR0(a) \
	SLOGC_0 (mvl_debug_sel & MVLLOG_ERR,a)
#define MVL_LOG_CERR1(a,b) \
	SLOGC_1 (mvl_debug_sel & MVLLOG_ERR,a,b)
#define MVL_LOG_CERR2(a,b,c) \
	SLOGC_2 (mvl_debug_sel & MVLLOG_ERR,a,b,c)
#define MVL_LOG_CERR3(a,b,c,d) \
	SLOGC_3 (mvl_debug_sel & MVLLOG_ERR,a,b,c,d)
#define MVL_LOG_CERR4(a,b,c,d,e) \
	SLOGC_4 (mvl_debug_sel & MVLLOG_ERR,a,b,c,d,e)
#define MVL_LOG_CERR5(a,b,c,d,e,f) \
	SLOGC_5 (mvl_debug_sel & MVLLOG_ERR,a,b,c,d,e,f)
                    

/*********************** NERR Macros *************************************/

#define MVL_LOG_NERR0(a) \
	SLOG_0 (mvl_debug_sel & MVLLOG_NERR,_mvllog_nerr_logstr,a)
#define MVL_LOG_NERR1(a,b) \
	SLOG_1 (mvl_debug_sel & MVLLOG_NERR,_mvllog_nerr_logstr,a,b)
#define MVL_LOG_NERR2(a,b,c) \
	SLOG_2 (mvl_debug_sel & MVLLOG_NERR,_mvllog_nerr_logstr,a,b,c)
#define MVL_LOG_NERR3(a,b,c,d) \
	SLOG_3 (mvl_debug_sel & MVLLOG_NERR,_mvllog_nerr_logstr,a,b,c,d)
#define MVL_LOG_NERR4(a,b,c,d,e) \
	SLOG_4 (mvl_debug_sel & MVLLOG_NERR,_mvllog_nerr_logstr,a,b,c,d,e)
#define MVL_LOG_NERR5(a,b,c,d,e,f) \
	SLOG_5 (mvl_debug_sel & MVLLOG_NERR,_mvllog_nerr_logstr,a,b,c,d,e,f)
                    
#define MVL_LOG_CNERR0(a) \
	SLOGC_0 (mvl_debug_sel & MVLLOG_NERR,a)
#define MVL_LOG_CNERR1(a,b) \
	SLOGC_1 (mvl_debug_sel & MVLLOG_NERR,a,b)
#define MVL_LOG_CNERR2(a,b,c) \
	SLOGC_2 (mvl_debug_sel & MVLLOG_NERR,a,b,c)
#define MVL_LOG_CNERR3(a,b,c,d) \
	SLOGC_3 (mvl_debug_sel & MVLLOG_NERR,a,b,c,d)
#define MVL_LOG_CNERR4(a,b,c,d,e) \
	SLOGC_4 (mvl_debug_sel & MVLLOG_NERR,a,b,c,d,e)
#define MVL_LOG_CNERR5(a,b,c,d,e,f) \
	SLOGC_5 (mvl_debug_sel & MVLLOG_NERR,a,b,c,d,e,f)

/*********************** ACSE Macros *************************************/

#define MVL_LOG_ACSE0(a) \
	SLOG_0 (mvl_debug_sel & MVLLOG_ACSE,_mvllog_acse_logstr,a)
#define MVL_LOG_ACSE1(a,b) \
	SLOG_1 (mvl_debug_sel & MVLLOG_ACSE,_mvllog_acse_logstr,a,b)
                    

#define MVL_LOG_CACSE0(a) \
	SLOGC_0 (mvl_debug_sel & MVLLOG_ACSE,a)
#define MVL_LOG_CACSE1(a,b) \
	SLOGC_1 (mvl_debug_sel & MVLLOG_ACSE,a,b)
                    

#define MVL_LOG_ACSEDATA1(a,b) \
	SLOG_1 (mvl_debug_sel & MVLLOG_ACSEDATA,_mvllog_acsedata_logstr,a,b)
#define MVL_LOG_ACSEDATA2(a,b,c) \
	SLOG_2 (mvl_debug_sel & MVLLOG_ACSEDATA,_mvllog_acsedata_logstr,a,b,c)
#define MVL_LOG_ACSEDATA3(a,b,c,d) \
	SLOG_3 (mvl_debug_sel & MVLLOG_ACSEDATA,_mvllog_acsedata_logstr,a,b,c,d)
                    

#define MVL_LOG_ACSEDATAH(a,b) \
	SLOGH (mvl_debug_sel & MVLLOG_ACSEDATA,a,b)
                    

/*********************** MVLU FLOW Macros *******************************/

#define MVLU_LOG_FLOW0(a) \
	SLOG_0 (mvl_debug_sel & MVLULOG_FLOW,_mvlulog_flow_logstr,a)
#define MVLU_LOG_FLOW1(a,b) \
	SLOG_1 (mvl_debug_sel & MVLULOG_FLOW,_mvlulog_flow_logstr,a,b)
#define MVLU_LOG_FLOW2(a,b,c) \
	SLOG_2 (mvl_debug_sel & MVLULOG_FLOW,_mvlulog_flow_logstr,a,b,c)
#define MVLU_LOG_FLOW3(a,b,c,d) \
	SLOG_3 (mvl_debug_sel & MVLULOG_FLOW,_mvlulog_flow_logstr,a,b,c,d)
                    

#define MVLU_LOG_CFLOW0(a) \
	SLOGC_0 (mvl_debug_sel & MVLULOG_FLOW,a)
#define MVLU_LOG_CFLOW1(a,b) \
	SLOGC_1 (mvl_debug_sel & MVLULOG_FLOW,a,b)
#define MVLU_LOG_CFLOW2(a,b,c) \
	SLOGC_2 (mvl_debug_sel & MVLULOG_FLOW,a,b,c)
#define MVLU_LOG_CFLOW3(a,b,c,d) \
	SLOGC_3 (mvl_debug_sel & MVLULOG_FLOW,a,b,c,d)
                    

/*********************** MVLU DEBUG Macros *******************************/

#define MVLU_LOG_DEBUG0(a) \
	SLOG_0 (mvl_debug_sel & MVLULOG_DEBUG,_mvlulog_debug_logstr,a)
#define MVLU_LOG_DEBUG1(a,b) \
	SLOG_1 (mvl_debug_sel & MVLULOG_DEBUG,_mvlulog_debug_logstr,a,b)
#define MVLU_LOG_DEBUG2(a,b,c) \
	SLOG_2 (mvl_debug_sel & MVLULOG_DEBUG,_mvlulog_debug_logstr,a,b,c)
#define MVLU_LOG_DEBUG3(a,b,c,d) \
	SLOG_3 (mvl_debug_sel & MVLULOG_DEBUG,_mvlulog_debug_logstr,a,b,c,d)
                    

#define MVLU_LOG_CDEBUG0(a) \
	SLOGC_0 (mvl_debug_sel & MVLULOG_DEBUG,a)
#define MVLU_LOG_CDEBUG1(a,b) \
	SLOGC_1 (mvl_debug_sel & MVLULOG_DEBUG,a,b)
#define MVLU_LOG_CDEBUG2(a,b,c) \
	SLOGC_2 (mvl_debug_sel & MVLULOG_DEBUG,a,b,c)
#define MVLU_LOG_CDEBUG3(a,b,c,d) \
	SLOGC_3 (mvl_debug_sel & MVLULOG_DEBUG,a,b,c,d)
                    
#ifdef __cplusplus
}
#endif

#endif

