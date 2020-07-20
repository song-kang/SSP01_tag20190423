/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2010, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : cosp_log.h						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file contains logging macros for COSP.	*/
/*	Note:	To compile logging functions use the DEBUG_SISCO define	*/
/*		in the project make file.				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/21/10  JRB    06     Use "Base" macros SLOG_*, SLOGC_*, SLOGH.	*/
/* 09/13/99  MDE    05     Added SD_CONST modifiers			*/
/* 10/08/98  MDE    04     Migrated to updated SLOG interface		*/
/* 05/27/97  JRB  7.00     MMSEASE 7.0 release.				*/
/* 04/24/97  JRB    03     Added NO-DEBUG version of COSP_LOG_ERRH.	*/
/* 03/20/97  EJV    02     Enhanced logging.				*/
/* 02/13/97  EJV    01     Created					*/
/************************************************************************/
#ifndef COSP_LOG_INCLUDED
#define COSP_LOG_INCLUDED

#include "slog.h"

#ifdef __cplusplus
extern "C" {
#endif


/* NOTE: COSP log masks and acse_debug_sel are defined in acse2usr.h */


#if defined(DEBUG_SISCO)
extern SD_CONST ST_CHAR *SD_CONST _cosp_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _cosp_dec_logstr;
extern SD_CONST ST_CHAR *SD_CONST _cosp_dec_hex_logstr;
extern SD_CONST ST_CHAR *SD_CONST _cosp_enc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _cosp_enc_hex_logstr;
#endif	/* DEBUG_SISCO	*/

  /*--------------------------------------------*/
  /* hard errors logging			*/
  /*--------------------------------------------*/

#define COSP_LOG_ERR0(a) \
	SLOG_0 (acse_debug_sel & COSP_LOG_ERR,_cosp_err_logstr,a)

#define COSP_LOG_ERR1(a,b) \
	SLOG_1 (acse_debug_sel & COSP_LOG_ERR,_cosp_err_logstr,a,b)

#define COSP_LOG_ERR2(a,b,c) \
	SLOG_2 (acse_debug_sel & COSP_LOG_ERR,_cosp_err_logstr,a,b,c)

#define COSP_LOG_ERR3(a,b,c,d) \
	SLOG_3 (acse_debug_sel & COSP_LOG_ERR,_cosp_err_logstr,a,b,c,d)

  /* continuation macros */
#define COSP_LOG_ERRC0(a) \
	SLOGC_0 (acse_debug_sel & COSP_LOG_ERR,a)

#define COSP_LOG_ERRC1(a,b) \
	SLOGC_1 (acse_debug_sel & COSP_LOG_ERR,a,b)

#define COSP_LOG_ERRC2(a,b,c) \
	SLOGC_2 (acse_debug_sel & COSP_LOG_ERR,a,b,c)

  /* hex logging */
#define COSP_LOG_ERRH(a,b) \
	SLOGH (acse_debug_sel & COSP_LOG_ERR,a,b)


  /*--------------------------------------------*/
  /* SS-user req and rsp logging		*/
  /*--------------------------------------------*/

#define COSP_LOG_ENC0(a) \
	SLOG_0 (acse_debug_sel & COSP_LOG_ENC,_cosp_enc_logstr,a)

#define COSP_LOG_ENC1(a,b) \
	SLOG_1 (acse_debug_sel & COSP_LOG_ENC,_cosp_enc_logstr,a,b)

#define COSP_LOG_ENC2(a,b,c) \
	SLOG_2 (acse_debug_sel & COSP_LOG_ENC,_cosp_enc_logstr,a,b,c)

#define COSP_LOG_ENC3(a,b,c,d) \
	SLOG_3 (acse_debug_sel & COSP_LOG_ENC,_cosp_enc_logstr,a,b,c,d)

#define COSP_LOG_ENC4(a,b,c,d,e) \
	SLOG_4 (acse_debug_sel & COSP_LOG_ENC,_cosp_enc_logstr,a,b,c,d,e)

  /* continuation macros */
#define COSP_LOG_ENCC0(a) \
	SLOGC_0 (acse_debug_sel & COSP_LOG_ENC,a)

#define COSP_LOG_ENCC1(a,b) \
	SLOGC_1 (acse_debug_sel & COSP_LOG_ENC,a,b)

  /* hex logging */
#define COSP_LOG_ENCH(a,b) \
	SLOGH (acse_debug_sel & COSP_LOG_ENC_HEX,a,b)


  /*--------------------------------------------*/
  /* SS-user ind and cnf logging		*/
  /*--------------------------------------------*/

#define COSP_LOG_DEC0(a) \
	SLOG_0 (acse_debug_sel & COSP_LOG_DEC,_cosp_dec_logstr,a)

#define COSP_LOG_DEC1(a,b) \
	SLOG_1 (acse_debug_sel & COSP_LOG_DEC,_cosp_dec_logstr,a,b)

#define COSP_LOG_DEC2(a,b,c) \
	SLOG_2 (acse_debug_sel & COSP_LOG_DEC,_cosp_dec_logstr,a,b,c)

#define COSP_LOG_DEC3(a,b,c,d) \
	SLOG_3 (acse_debug_sel & COSP_LOG_DEC,_cosp_dec_logstr,a,b,c,d)

#define COSP_LOG_DEC4(a,b,c,d,e) \
	SLOG_4 (acse_debug_sel & COSP_LOG_DEC,_cosp_dec_logstr,a,b,c,d,e)

  /* continuation macros */
#define COSP_LOG_DECC0(a) \
	SLOGC_0 (acse_debug_sel & COSP_LOG_DEC,a)

  #define COSP_LOG_DECC1(a,b) \
	SLOGC_1 (acse_debug_sel & COSP_LOG_DEC,a,b)

  /* hex logging */
#define COSP_LOG_DECH(a,b) \
	SLOGH (acse_debug_sel & COSP_LOG_DEC_HEX,a,b)

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/
