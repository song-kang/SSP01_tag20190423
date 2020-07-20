/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995-2003, All Rights Reserved					*/
/*									*/
/* MODULE NAME : acse2log.h						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	ACSE logging header file.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 12/02/05  JRB    07     Added acse2usr.h				*/
/* 06/17/03  EJV    06     Converted to new log macros.			*/
/*			   Deleted _audt_*_logstr. Added DIB logging.	*/
/* 12/11/00  EJV    05     Added more log macros			*/
/* 09/13/99  MDE    05     Added SD_CONST modifiers			*/
/* 12/16/96  JRB    04     Del "AUDTLOG_*" macros. Use "ACSELOG_*" for	*/
/*			   CL ACSE as well as CO ACSE.			*/
/* 10/08/98  MDE    08     Migrated to updated SLOG interface		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/20/97  JRB    07     Stop checking for sLogCtrl==NULL.		*/
/* 02/20/97  JRB    06     Move *_debug_sel & logmasks from acse2log.h	*/
/*			   to acse2usr.h.				*/
/* 01/02/97  EJV    05     Added ACSE2LOG_INCLUDED			*/
/* 12/16/96  JRB    04     Add "AUDTLOG_*" macros for CLACSE.		*/
/* 07/19/96  JRB    03     Change DEBUG_MMS to DEBUG_SISCO.		*/
/* 04/17/96  JRB    02     Del acse_sLogCtrl, use sLogCtrl (slog.c) now	*/
/* 10/30/95  JRB    01     Created					*/
/************************************************************************/
#ifndef ACSE2LOG_INCLUDED
#define ACSE2LOG_INCLUDED

#include "acse2usr.h"	/* need acse_debug_sel & log mask defines	*/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef DEBUG_SISCO
/****************************************************************/
/* Log Control macros						*/
/****************************************************************/

extern SD_CONST ST_CHAR *SD_CONST _acse_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _acse_enc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _acse_dec_logstr;
extern SD_CONST ST_CHAR *SD_CONST _acse_dib_logstr;
#endif /* DEBUG_SISCO */

/*--------------------------------------*/
/* ACSELOG_DEC macros			*/
/*--------------------------------------*/
#define ACSELOG_DEC0(a) \
		SLOG_0 (acse_debug_sel & ACSE_LOG_DEC,_acse_dec_logstr,a)
#define ACSELOG_DEC1(a,b) \
		SLOG_1 (acse_debug_sel & ACSE_LOG_DEC,_acse_dec_logstr,a,b)
#define ACSELOG_DEC2(a,b,c) \
		SLOG_2 (acse_debug_sel & ACSE_LOG_DEC,_acse_dec_logstr,a,b,c)
#define ACSELOG_DEC3(a,b,c,d) \
		SLOG_3 (acse_debug_sel & ACSE_LOG_DEC,_acse_dec_logstr,a,b,c,d)

#define ACSELOG_DEC0C(a)	SLOGC_0 (acse_debug_sel & ACSE_LOG_DEC,a)
#define ACSELOG_DEC1C(a,b)	SLOGC_1 (acse_debug_sel & ACSE_LOG_DEC,a,b)
#define ACSELOG_DEC2C(a,b,c)	SLOGC_2 (acse_debug_sel & ACSE_LOG_DEC,a,b,c)
#define ACSELOG_DEC3C(a,b,c,d)	SLOGC_3 (acse_debug_sel & ACSE_LOG_DEC,a,b,c,d)

#define ACSELOG_DECH(a,b)	SLOGH (acse_debug_sel & ACSE_LOG_DEC,a,b)

/*--------------------------------------*/
/* ACSELOG_ENC macros			*/
/*--------------------------------------*/
#define ACSELOG_ENC0(a) \
		SLOG_0 (acse_debug_sel & ACSE_LOG_ENC,_acse_enc_logstr,a)
#define ACSELOG_ENC1(a,b) \
		SLOG_1 (acse_debug_sel & ACSE_LOG_ENC,_acse_enc_logstr,a,b)
#define ACSELOG_ENC2(a,b,c) \
		SLOG_2 (acse_debug_sel & ACSE_LOG_ENC,_acse_enc_logstr,a,b,c)
#define ACSELOG_ENC3(a,b,c,d) \
		SLOG_3 (acse_debug_sel & ACSE_LOG_ENC,_acse_enc_logstr,a,b,c,d)

#define ACSELOG_ENC0C(a)	SLOGC_0 (acse_debug_sel & ACSE_LOG_ENC,a)
#define ACSELOG_ENC1C(a,b)	SLOGC_1 (acse_debug_sel & ACSE_LOG_ENC,a,b)
#define ACSELOG_ENC2C(a,b,c)	SLOGC_2 (acse_debug_sel & ACSE_LOG_ENC,a,b,c)
#define ACSELOG_ENC3C(a,b,c,d)	SLOGC_3 (acse_debug_sel & ACSE_LOG_ENC,a,b,c,d)

#define ACSELOG_ENCH(a,b)	SLOGH (acse_debug_sel & ACSE_LOG_ENC,a,b)

/*--------------------------------------*/
/* ACSELOG_ERR macros			*/
/*--------------------------------------*/
#define ACSELOG_ERR0(a) \
		SLOG_0 (acse_debug_sel & ACSE_LOG_ERR,_acse_err_logstr,a)
#define ACSELOG_ERR1(a,b) \
		SLOG_1 (acse_debug_sel & ACSE_LOG_ERR,_acse_err_logstr,a,b)
#define ACSELOG_ERR2(a,b,c) \
		SLOG_2 (acse_debug_sel & ACSE_LOG_ERR,_acse_err_logstr,a,b,c)
#define ACSELOG_ERR3(a,b,c,d) \
		SLOG_3 (acse_debug_sel & ACSE_LOG_ERR,_acse_err_logstr,a,b,c,d)

#define ACSELOG_ERR0C(a)	SLOGC_0 (acse_debug_sel & ACSE_LOG_ERR,a)
#define ACSELOG_ERR1C(a,b)	SLOGC_1 (acse_debug_sel & ACSE_LOG_ERR,a,b)
#define ACSELOG_ERR2C(a,b,c)	SLOGC_2 (acse_debug_sel & ACSE_LOG_ERR,a,b,c)
#define ACSELOG_ERR3C(a,b,c,d)	SLOGC_3 (acse_debug_sel & ACSE_LOG_ERR,a,b,c,d)

#define ACSELOG_ERRH(a,b)	SLOGH (acse_debug_sel & ACSE_LOG_ERR,a,b)

/*--------------------------------------*/
/* ACSELOG_DIB macros			*/
/*--------------------------------------*/
#define ACSELOG_DIB0(a) \
		SLOG_0 (acse_debug_sel & ACSE_LOG_DIB,_acse_dib_logstr,a)
#define ACSELOG_DIB1(a,b) \
		SLOG_1 (acse_debug_sel & ACSE_LOG_DIB,_acse_dib_logstr,a,b)
#define ACSELOG_DIB2(a,b,c) \
		SLOG_2 (acse_debug_sel & ACSE_LOG_DIB,_acse_dib_logstr,a,b,c)
#define ACSELOG_DIB3(a,b,c,d) \
		SLOG_3 (acse_debug_sel & ACSE_LOG_DIB,_acse_dib_logstr,a,b,c,d)

#define ACSELOG_DIB0C(a)	SLOGC_0 (acse_debug_sel & ACSE_LOG_DIB,a)
#define ACSELOG_DIB1C(a,b)	SLOGC_1 (acse_debug_sel & ACSE_LOG_DIB,a,b)
#define ACSELOG_DIB2C(a,b,c)	SLOGC_2 (acse_debug_sel & ACSE_LOG_DIB,a,b,c)
#define ACSELOG_DIB3C(a,b,c,d)	SLOGC_3 (acse_debug_sel & ACSE_LOG_DIB,a,b,c,d)

#define ACSELOG_DIBH(a,b)	SLOGH (acse_debug_sel & ACSE_LOG_DIB,a,b)


/****************************************************************/
/* Log Control prototypes.					*/
/****************************************************************/
ST_VOID acse_default_log (ST_VOID);	/* Set default log configuration	*/

#ifdef __cplusplus
}
#endif

#endif	/* ACSE2LOG_INCLUDED */

