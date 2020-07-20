/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_log.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file contains logging macros for CLNP.	*/
/*	Note:	To compile logging functions use the DEBUG_SISCO define	*/
/*		in the project make file.				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 06/09/03  JRB    07     Use new "Base" macros SLOG_*, SLOGC_*, SLOGH.*/
/*			   Del unused macros.				*/
/* 10/02/01  JRB    06     Fix logstr in CLSNS_LOG_* macros.		*/
/* 09/13/99  MDE    05     Added SD_CONST modifiers			*/
/* 10/08/98  MDE    04     Migrated to updated SLOG interface		*/
/* 07/28/97  JRB    03     Added CLNP_LOG_REQ2 / 3 & CLNP_LOG_IND2 / 3	*/
/* 05/27/97  JRB  7.00     MMSEASE 7.0 release.				*/
/* 03/03/97  EJV    02     Corrected ALWAYS macros;			*/
/*			   Removed check if sLogCtrl not NULL		*/
/* 06/10/96  EJV    01     Created					*/
/************************************************************************/

#ifndef CLNP_LOG_INCLUDED
#define CLNP_LOG_INCLUDED

#include "slog.h"

#ifdef __cplusplus
extern "C" {
#endif


/* NOTE: CLNP log masks and clnp_debug_sel are defined in clnp_usr.h */

extern SD_CONST ST_CHAR *SD_CONST _clnp_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _clnp_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST _clnp_req_logstr;
extern SD_CONST ST_CHAR *SD_CONST _clnp_ind_logstr;
extern SD_CONST ST_CHAR *SD_CONST _clnp_enc_dec_logstr;
extern SD_CONST ST_CHAR *SD_CONST _clnp_llc_enc_dec_logstr;
extern SD_CONST ST_CHAR *SD_CONST _clsns_req_logstr;
extern SD_CONST ST_CHAR *SD_CONST _clsns_ind_logstr;

#if defined(DEBUG_SISCO)
  /*--------------------------------------------------------------------*/
  /* macros below are used to log REQ or IND by passing mask to select 	*/
  /* the logging macro (this is handy in log function so the macros	*/
  /* do not have to repeated in function for REQ and then for IND)	*/
  /*--------------------------------------------------------------------*/

  #define CLNP_LOGC_RI0(m,a)    {if (m == CLNP_LOG_REQ) {CLNP_LOGC_REQ0(a);}     else {CLNP_LOGC_IND0(a);}}
  #define CLNP_LOGC_RI1(m,a,b)  {if (m == CLNP_LOG_REQ) {CLNP_LOGC_REQ1(a,b);}   else {CLNP_LOGC_IND1(a,b);}}

  #define CLNP_LOGH_RI(m,a,b)   {if (m == CLNP_LOG_REQ) {CLNP_LOGH_REQ(a,b);}    else {CLNP_LOGH_IND(a,b);}}

  /*--------------------------------------------------------------------*/
  /* macros below are used to log REQ or IND by passing mask to select 	*/
  /* the logging macro (this is handy in log function so the macros	*/
  /* do not have to repeated in function for REQ and then for IND)	*/
  /*--------------------------------------------------------------------*/

  #define CLSNS_LOGC_RI0(m,a)    {if (m == CLSNS_LOG_REQ) {CLSNS_LOGC_REQ0(a);}     else {CLSNS_LOGC_IND0(a);}}
  #define CLSNS_LOGC_RI1(m,a,b)  {if (m == CLSNS_LOG_REQ) {CLSNS_LOGC_REQ1(a,b);}   else {CLSNS_LOGC_IND1(a,b);}}

  #define CLSNS_LOGH_RI(m,a,b)   {if (m == CLSNS_LOG_REQ) {CLSNS_LOGH_REQ(a,b);}    else {CLSNS_LOGH_IND(a,b);}}

#else	/* !defined(DEBUG_SISCO)	*/

  /*--------------------------------------------------------------------*/
  /* macros below are used to log REQ or IND by passing mask		*/
  /*--------------------------------------------------------------------*/

  #define CLNP_LOGC_RI0(m,a)
  #define CLNP_LOGC_RI1(m,a,b)
  #define CLNP_LOGH_RI(m,a,b)

  /*--------------------------------------------------------------------*/
  /* macros below are used to log REQ or IND by passing mask		*/
  /*--------------------------------------------------------------------*/

  #define CLSNS_LOGC_RI0(m,a)
  #define CLSNS_LOGC_RI1(m,a,b)

  #define CLSNS_LOGH_RI(m,a,b)

#endif	/* !defined(DEBUG_SISCO)	*/

  /*--------------------------------------------*/
  /* hard errors logging			*/
  /*--------------------------------------------*/

#define CLNP_LOG_ERR0(a) \
	SLOG_0 (clnp_debug_sel & CLNP_LOG_ERR,_clnp_err_logstr,a)
#define CLNP_LOG_ERR1(a,b) \
	SLOG_1 (clnp_debug_sel & CLNP_LOG_ERR,_clnp_err_logstr,a,b)
#define CLNP_LOG_ERR2(a,b,c) \
	SLOG_2 (clnp_debug_sel & CLNP_LOG_ERR,_clnp_err_logstr,a,b,c)
#define CLNP_LOG_ERR3(a,b,c,d) \
	SLOG_3 (clnp_debug_sel & CLNP_LOG_ERR,_clnp_err_logstr,a,b,c,d)

  /*--------------------------------------------*/
  /* normal errors logging			*/
  /*--------------------------------------------*/

#define CLNP_LOG_NERR0(a) \
	SLOG_0 (clnp_debug_sel & CLNP_LOG_NERR,_clnp_nerr_logstr,a)
#define CLNP_LOG_NERR1(a,b) \
	SLOG_1 (clnp_debug_sel & CLNP_LOG_NERR,_clnp_nerr_logstr,a,b)
#define CLNP_LOG_NERR2(a,b,c) \
	SLOG_2 (clnp_debug_sel & CLNP_LOG_NERR,_clnp_nerr_logstr,a,b,c)
#define CLNP_LOG_NERR3(a,b,c,d) \
	SLOG_3 (clnp_debug_sel & CLNP_LOG_NERR,_clnp_nerr_logstr,a,b,c,d)

  /*--------------------------------------------*/
  /* CLNP-user request logging			*/
  /*--------------------------------------------*/

#define CLNP_LOG_REQ0(a) \
	SLOG_0 (clnp_debug_sel & CLNP_LOG_REQ,_clnp_req_logstr,a)
#define CLNP_LOG_REQ1(a,b) \
	SLOG_1 (clnp_debug_sel & CLNP_LOG_REQ,_clnp_req_logstr,a,b)
#define CLNP_LOG_REQ2(a,b,c) \
	SLOG_2 (clnp_debug_sel & CLNP_LOG_REQ,_clnp_req_logstr,a,b,c)
#define CLNP_LOG_REQ3(a,b,c,d) \
	SLOG_3 (clnp_debug_sel & CLNP_LOG_REQ,_clnp_req_logstr,a,b,c,d)
  /* continuation log */
#define CLNP_LOGC_REQ0(a) \
	SLOGC_0 (clnp_debug_sel & CLNP_LOG_REQ,a)
#define CLNP_LOGC_REQ1(a,b) \
	SLOGC_1 (clnp_debug_sel & CLNP_LOG_REQ,a,b)
  /* hex logging */
#define CLNP_LOGH_REQ(a,b) \
	SLOGH (clnp_debug_sel & CLNP_LOG_REQ,a,b)

  /*--------------------------------------------*/
  /* CLNP_user indication logging		*/
  /*--------------------------------------------*/

#define CLNP_LOG_IND0(a) \
	SLOG_0 (clnp_debug_sel & CLNP_LOG_IND,_clnp_ind_logstr,a)
#define CLNP_LOG_IND1(a,b) \
	SLOG_1 (clnp_debug_sel & CLNP_LOG_IND,_clnp_ind_logstr,a,b)
#define CLNP_LOG_IND2(a,b,c) \
	SLOG_2 (clnp_debug_sel & CLNP_LOG_IND,_clnp_ind_logstr,a,b,c)
#define CLNP_LOG_IND3(a,b,c,d) \
	SLOG_3 (clnp_debug_sel & CLNP_LOG_IND,_clnp_ind_logstr,a,b,c,d)
  /* continuation log */
#define CLNP_LOGC_IND0(a) \
	SLOGC_0 (clnp_debug_sel & CLNP_LOG_IND,a)
#define CLNP_LOGC_IND1(a,b) \
	SLOGC_1 (clnp_debug_sel & CLNP_LOG_IND,a,b)
  /* hex logging */
#define CLNP_LOGH_IND(a,b) \
	SLOGH (clnp_debug_sel & CLNP_LOG_IND,a,b)

  /*--------------------------------------------*/
  /* CLNP encoding/decoding of NPDU 		*/
  /*--------------------------------------------*/

#define CLNP_LOG_ENC_DEC0(a) \
	SLOG_0 (clnp_debug_sel & CLNP_LOG_ENC_DEC,_clnp_enc_dec_logstr,a)
#define CLNP_LOG_ENC_DEC1(a,b) \
	SLOG_1 (clnp_debug_sel & CLNP_LOG_ENC_DEC,_clnp_enc_dec_logstr,a,b)
#define CLNP_LOG_ENC_DEC2(a,b,c) \
	SLOG_2 (clnp_debug_sel & CLNP_LOG_ENC_DEC,_clnp_enc_dec_logstr,a,b,c)
  /* continuation log */
#define CLNP_LOGC_ENC_DEC0(a) \
	SLOGC_0 (clnp_debug_sel & CLNP_LOG_ENC_DEC,a)
#define CLNP_LOGC_ENC_DEC1(a,b) \
	SLOGC_1 (clnp_debug_sel & CLNP_LOG_ENC_DEC,a,b)
  /* hex logging */
#define CLNP_LOGH_ENC_DEC(a,b) \
	SLOGH (clnp_debug_sel & CLNP_LOG_ENC_DEC,a,b)

  /*--------------------------------------------*/
  /* LLC encoding/decoding of LPDU		*/
  /*--------------------------------------------*/

#define CLNP_LOG_LLC_ENC_DEC3(a,b,c,d) \
	SLOG_3 (clnp_debug_sel & CLNP_LOG_LLC_ENC_DEC,_clnp_llc_enc_dec_logstr,a,b,c,d)

  /*----------------------------------------------*/
  /* sub-network services request (write) logging */
  /*----------------------------------------------*/

#define CLSNS_LOG_REQ0(a) \
	SLOG_0 (clnp_debug_sel & CLSNS_LOG_REQ,_clsns_req_logstr,a)
#define CLSNS_LOG_REQ1(a,b) \
	SLOG_1 (clnp_debug_sel & CLSNS_LOG_REQ,_clsns_req_logstr,a,b)
  /* continuation log */
#define CLSNS_LOGC_REQ0(a) \
	SLOGC_0 (clnp_debug_sel & CLSNS_LOG_REQ,a)
#define CLSNS_LOGC_REQ1(a,b) \
	SLOGC_1 (clnp_debug_sel & CLSNS_LOG_REQ,a,b)
  /* hex logging */
#define CLSNS_LOGH_REQ(a,b) \
	SLOGH (clnp_debug_sel & CLSNS_LOG_REQ,a,b)

  /*------------------------------------------------*/
  /* sub-network services indication logging (read) */
  /*------------------------------------------------*/

#define CLSNS_LOG_IND0(a) \
	SLOG_0 (clnp_debug_sel & CLSNS_LOG_IND,_clsns_ind_logstr,a)
#define CLSNS_LOG_IND1(a,b) \
	SLOG_1 (clnp_debug_sel & CLSNS_LOG_IND,_clsns_ind_logstr,a,b)

  /* continuation log */
#define CLSNS_LOGC_IND0(a) \
	SLOGC_0 (clnp_debug_sel & CLSNS_LOG_IND,a)
#define CLSNS_LOGC_IND1(a,b) \
	SLOGC_1 (clnp_debug_sel & CLSNS_LOG_IND,a,b)
  /* hex logging */
#define CLSNS_LOGH_IND(a,b) \
	SLOGH (clnp_debug_sel & CLSNS_LOG_IND,a,b)

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/
