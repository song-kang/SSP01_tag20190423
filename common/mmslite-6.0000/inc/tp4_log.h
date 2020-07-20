#ifndef TP4_LOG_INCLUDED
#define TP4_LOG_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996 - 2008, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : tp4_log.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file contains logging macros for TP.	*/
/*	Note:	To compile logging functions use the DEBUG_SISCO define	*/
/*		in the project make file.				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/30/08  EJV     05    Converted to new SLOG macros.		*/
/* 05/10/05  EJV     04    Added TP_LOG_ERR3.				*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 10/08/98  MDE     02    Migrated to updated SLOG interface		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 06/24/96  EJV     01    Created					*/
/************************************************************************/
#include "slog.h"
#include "tp4api.h"	/* NOTE the masks are defined in tp4api.h,	*/
			/*	should we include it here? */

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************/
/* Log Control types						*/
/****************************************************************/

/* defined for TP_LOG.._RIn macros */
#define	TP_REQ	(ST_UCHAR) 0
#define	TP_IND	(ST_UCHAR) 1

#if defined(DEBUG_SISCO)

extern SD_CONST ST_CHAR *SD_CONST _tp4_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _tp4_flowup_logstr;
extern SD_CONST ST_CHAR *SD_CONST _tp4_flowdown_logstr;

  /*--------------------------------------------*/
  /* errors logging				*/
  /*--------------------------------------------*/

  #define TP_LOG_ERR0(a)            SLOG_0 (tp4_debug_sel & TP4_LOG_ERR,_tp4_err_logstr,a)
  #define TP_LOG_ERR1(a,b)          SLOG_1 (tp4_debug_sel & TP4_LOG_ERR,_tp4_err_logstr,a,b)
  #define TP_LOG_ERR2(a,b,c)        SLOG_2 (tp4_debug_sel & TP4_LOG_ERR,_tp4_err_logstr,a,b,c)
  #define TP_LOG_ERR3(a,b,c,d)      SLOG_3 (tp4_debug_sel & TP4_LOG_ERR,_tp4_err_logstr,a,b,c,d)

  /* continuation log */

  #define TP_LOGC_ERR0(a)           SLOGC_0 (tp4_debug_sel & TP4_LOG_ERR,a)
  #define TP_LOGC_ERR1(a,b)         SLOGC_1 (tp4_debug_sel & TP4_LOG_ERR,a,b)
  #define TP_LOGC_ERR2(a,b,c)       SLOGC_2 (tp4_debug_sel & TP4_LOG_ERR,a,b,c)
  #define TP_LOGC_ERR3(a,b,c,d)     SLOGC_2 (tp4_debug_sel & TP4_LOG_ERR,a,b,c)

  /* hex logging */

  #define TP_LOGH_ERR(a,b)          SLOGH (tp4_debug_sel & TP4_LOG_ERR,a,b)

  /*--------------------------------------------*/
  /* Indication logging				*/
  /*--------------------------------------------*/

  #define TP_LOG_IND0(a)             SLOG_0 (tp4_debug_sel & TP4_LOG_FLOWUP,_tp4_flowup_logstr,a)
  #define TP_LOG_IND1(a,b)           SLOG_1 (tp4_debug_sel & TP4_LOG_FLOWUP,_tp4_flowup_logstr,a,b)
  #define TP_LOG_IND2(a,b,c)         SLOG_2 (tp4_debug_sel & TP4_LOG_FLOWUP,_tp4_flowup_logstr,a,b,c)
  #define TP_LOG_IND3(a,b,c,d)       SLOG_3 (tp4_debug_sel & TP4_LOG_FLOWUP,_tp4_flowup_logstr,a,b,c,d)
  #define TP_LOG_IND4(a,b,c,d,e)     SLOG_4 (tp4_debug_sel & TP4_LOG_FLOWUP,_tp4_flowup_logstr,a,b,c,d,e)
  #define TP_LOG_IND5(a,b,c,d,e,f)   SLOG_5 (tp4_debug_sel & TP4_LOG_FLOWUP,_tp4_flowup_logstr,a,b,c,d,e,f)
  #define TP_LOG_IND6(a,b,c,d,e,f,g) SLOG_6 (tp4_debug_sel & TP4_LOG_FLOWUP,_tp4_flowup_logstr,a,b,c,d,e,f,g)
		      
  /* continuation log */

  #define TP_LOGC_IND0(a)           SLOGC_0 (tp4_debug_sel & TP4_LOG_FLOWUP,a)
  #define TP_LOGC_IND1(a,b)         SLOGC_1 (tp4_debug_sel & TP4_LOG_FLOWUP,a,b)
  #define TP_LOGC_IND2(a,b,c)       SLOGC_2 (tp4_debug_sel & TP4_LOG_FLOWUP,a,b,c)

  /* hex logging */

  #define TP_LOGH_IND(a,b)          SLOGH (tp4_debug_sel & TP4_LOG_FLOWUP,a,b)

  /* continuation hex logging */

  #define TP_LOGHC_IND(a,b)         SLOGH (tp4_debug_sel & TP4_LOG_FLOWUP,a,b)

  /*--------------------------------------------*/
  /* Request logging				*/
  /*--------------------------------------------*/

  #define TP_LOG_REQ0(a)             SLOG_0 (tp4_debug_sel & TP4_LOG_FLOWDOWN,_tp4_flowdown_logstr,a)
  #define TP_LOG_REQ1(a,b)           SLOG_1 (tp4_debug_sel & TP4_LOG_FLOWDOWN,_tp4_flowdown_logstr,a,b)
  #define TP_LOG_REQ2(a,b,c)         SLOG_2 (tp4_debug_sel & TP4_LOG_FLOWDOWN,_tp4_flowdown_logstr,a,b,c)
  #define TP_LOG_REQ3(a,b,c,d)       SLOG_3 (tp4_debug_sel & TP4_LOG_FLOWDOWN,_tp4_flowdown_logstr,a,b,c,d)
  #define TP_LOG_REQ4(a,b,c,d,e)     SLOG_4 (tp4_debug_sel & TP4_LOG_FLOWDOWN,_tp4_flowdown_logstr,a,b,c,d,e)
  #define TP_LOG_REQ5(a,b,c,d,e,f)   SLOG_5 (tp4_debug_sel & TP4_LOG_FLOWDOWN,_tp4_flowdown_logstr,a,b,c,d,e,f)
  #define TP_LOG_REQ6(a,b,c,d,e,f,g) SLOG_6 (tp4_debug_sel & TP4_LOG_FLOWDOWN,_tp4_flowdown_logstr,a,b,c,d,e,f,g)
	
  /* continuation log */

  #define TP_LOGC_REQ0(a)           SLOGC_0 (tp4_debug_sel & TP4_LOG_FLOWDOWN,a)
  #define TP_LOGC_REQ1(a,b)         SLOGC_1 (tp4_debug_sel & TP4_LOG_FLOWDOWN,a,b)
  #define TP_LOGC_REQ2(a,b,c)       SLOGC_2 (tp4_debug_sel & TP4_LOG_FLOWDOWN,a,b,c)

  /* hex logging */

  #define TP_LOGH_REQ(a,b)          SLOGH (tp4_debug_sel & TP4_LOG_FLOWDOWN,a,b)

  /* continuation hex logging */

  #define TP_LOGHC_REQ(a,b)         SLOGH (tp4_debug_sel & TP4_LOG_FLOWDOWN,a,b)

  /*--------------------------------------------------------------------*/
  /* macros below are used to log REQ or IND by passing mask to select 	*/
  /* the logging macro (this is handy in log function so the macros	*/
  /* do not have to repeated in function for REQ and then for IND)	*/
  /*--------------------------------------------------------------------*/

  #define TP_LOG_RI0(m,a)             {if (m == TP_REQ) {TP_LOG_REQ0(a);}             else {TP_LOG_IND0(a)};}
  #define TP_LOG_RI1(m,a,b)           {if (m == TP_REQ) {TP_LOG_REQ1(a,b);}           else {TP_LOG_IND1(a,b);}}
  #define TP_LOG_RI2(m,a,b,c)         {if (m == TP_REQ) {TP_LOG_REQ2(a,b,c);}         else {TP_LOG_IND2(a,b,c);}}
  #define TP_LOG_RI3(m,a,b,c,d)       {if (m == TP_REQ) {TP_LOG_REQ3(a,b,c,d);}       else {TP_LOG_IND3(a,b,c,d);}}
  #define TP_LOG_RI4(m,a,b,c,d,e)     {if (m == TP_REQ) {TP_LOG_REQ4(a,b,c,d,e);}     else {TP_LOG_IND4(a,b,c,d,e);}}
  #define TP_LOG_RI5(m,a,b,c,d,e,f)   {if (m == TP_REQ) {TP_LOG_REQ5(a,b,c,d,e,f);}   else {TP_LOG_IND5(a,b,c,d,e,f);}}
  #define TP_LOG_RI6(m,a,b,c,d,e,f,g) {if (m == TP_REQ) {TP_LOG_REQ6(a,b,c,d,e,f,g);} else {TP_LOG_IND6(a,b,c,d,e,f,g);}}

  #define TP_LOGC_RI0(m,a)     {if (m == TP_REQ) {TP_LOGC_REQ0(a);}     else {TP_LOGC_IND0(a);}}
  #define TP_LOGC_RI1(m,a,b)   {if (m == TP_REQ) {TP_LOGC_REQ1(a,b);}   else {TP_LOGC_IND1(a,b);}}
  #define TP_LOGC_RI2(m,a,b,c) {if (m == TP_REQ) {TP_LOGC_REQ2(a,b,c);} else {TP_LOGC_IND2(a,b,c);}}

  #define TP_LOGH_RI(m,a,b)    {if (m == TP_REQ) {TP_LOGH_REQ(a,b);}    else {TP_LOGH_IND(a,b);}}
  #define TP_LOGHC_RI(m,a,b)   {if (m == TP_REQ) {TP_LOGHC_REQ(a,b);}   else {TP_LOGHC_IND(a,b);}}


#else  /* macros if DEBUG_SISCO not defined */

  /*--------------------------------------------*/
  /* errors logging				*/
  /*--------------------------------------------*/

  #define TP_LOG_ERR0(a)
  #define TP_LOG_ERR1(a,b)
  #define TP_LOG_ERR2(a,b,c)
  #define TP_LOG_ERR3(a,b,c,d)

  #define TP_LOGC_ERR0(a)
  #define TP_LOGC_ERR1(a,b)
  #define TP_LOGC_ERR2(a,b,c)

  #define TP_LOGH_ERR(a,b)
  #define TP_LOGHC_ERR(a,b)

  /*--------------------------------------------*/
  /* Indication logging				*/
  /*--------------------------------------------*/

  #define TP_LOG_IND0(a)
  #define TP_LOG_IND1(a,b)
  #define TP_LOG_IND2(a,b,c)
  #define TP_LOG_IND3(a,b,c,d)
  #define TP_LOG_IND4(a,b,c,d,e)
  #define TP_LOG_IND5(a,b,c,d,e,f)
  #define TP_LOG_IND6(a,b,c,d,e,f,g)

  #define TP_LOGC_IND0(a)
  #define TP_LOGC_IND1(a,b)
  #define TP_LOGC_IND2(a,b,c)

  #define TP_LOGH_IND(a,b)
  #define TP_LOGHC_IND(a,b)

  /*--------------------------------------------*/
  /* Request logging				*/
  /*--------------------------------------------*/

  #define TP_LOG_REQ0(a)
  #define TP_LOG_REQ1(a,b)
  #define TP_LOG_REQ2(a,b,c)
  #define TP_LOG_REQ3(a,b,c,d)
  #define TP_LOG_REQ4(a,b,c,d,e)
  #define TP_LOG_REQ5(a,b,c,d,e,f)
  #define TP_LOG_REQ6(a,b,c,d,e,f,g)

  #define TP_LOGC_REQ0(a)
  #define TP_LOGC_REQ1(a,b)
  #define TP_LOGC_REQ2(a,b,c)

  #define TP_LOGH_REQ(a,b)
  #define TP_LOGHC_REQ(a,b)

  /*--------------------------------------------------------------------*/
  /* macros below are used to log REQ or IND by passing mask		*/
  /*--------------------------------------------------------------------*/

  #define TP_LOG_RI0(m,a)
  #define TP_LOG_RI1(m,a,b)
  #define TP_LOG_RI2(m,a,b,c)
  #define TP_LOG_RI3(m,a,b,c,d)
  #define TP_LOG_RI4(m,a,b,c,d,e)
  #define TP_LOG_RI5(m,a,b,c,d,e,f)
  #define TP_LOG_RI6(m,a,b,c,d,e,f,g)

  #define TP_LOGC_RI0(m,a)
  #define TP_LOGC_RI1(m,a,b)
  #define TP_LOGC_RI2(m,a,b,c)

  #define TP_LOGH_RI(m,a,b)
  #define TP_LOGHC_RI(m,a,b)

#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/

