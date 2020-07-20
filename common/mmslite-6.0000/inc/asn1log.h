/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 1994, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : asn1log.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains ASN.1 logging definitions			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/16/03  JRB     06    Add more ERR/NERR macros.			*/
/* 03/31/03  JRB     05    Use new "Base" macros SLOG_*, SLOGC_*, SLOGH.*/
/*			   Add more macros.				*/
/* 03/13/02  JRB     04    ALOG_PAUSEDEC, ALOG_DECLF macros do nothing.	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 10/08/98  MDE     02    Migrated to updated SLOG interface		*/
/* 09/11/97  DSF     01    Adde ALOG_NERRH				*/
/* 04/02/97	   7.00    MMSEASE 7.0 release. See ASN1ML70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef ASN1_LOG_INCLUDED
#define ASN1_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST _asn1_log_dec_logstr;
extern SD_CONST ST_CHAR *SD_CONST _asn1_log_enc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _asn1_log_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _asn1_log_nerr_logstr;
       

#define ALOG_DECLF()
#define ALOG_PAUSEDEC(msg)

#define ALOG_DEC0(a) \
	SLOG_0 (asn1_debug_sel & ASN1_LOG_DEC,_asn1_log_dec_logstr,a)
#define ALOG_DEC1(a,b) \
	SLOG_1 (asn1_debug_sel & ASN1_LOG_DEC,_asn1_log_dec_logstr,a,b)

#define ALOG_CDEC0(a) \
	SLOGC_0 (asn1_debug_sel & ASN1_LOG_DEC,a)
#define ALOG_CDEC1(a,b) \
	SLOGC_1 (asn1_debug_sel & ASN1_LOG_DEC,a,b)
#define ALOG_CDEC2(a,b,c) \
	SLOGC_2 (asn1_debug_sel & ASN1_LOG_DEC,a,b,c)
#define ALOG_CDEC3(a,b,c,d) \
	SLOGC_3 (asn1_debug_sel & ASN1_LOG_DEC,a,b,c,d)
#define ALOG_CDEC4(a,b,c,d,e) \
	SLOGC_4 (asn1_debug_sel & ASN1_LOG_DEC,a,b,c,d,e)

#define ALOG_DECH(a,b) \
	SLOGH (asn1_debug_sel & ASN1_LOG_DEC,a,b)

#define ALOG_ENC0(a) \
	SLOG_0 (asn1_debug_sel & ASN1_LOG_ENC,_asn1_log_enc_logstr,a)
#define ALOG_ENC1(a,b) \
	SLOG_1 (asn1_debug_sel & ASN1_LOG_ENC,_asn1_log_enc_logstr,a,b)

#define ALOG_CENC0(a) \
	SLOGC_0 (asn1_debug_sel & ASN1_LOG_ENC,a)
#define ALOG_CENC1(a,b) \
	SLOGC_1 (asn1_debug_sel & ASN1_LOG_ENC,a,b)

#define ALOG_ENCH(a,b) \
	SLOGH (asn1_debug_sel & ASN1_LOG_ENC,a,b)

#define ALOG_ERR0(a) \
	SLOG_0 (asn1_debug_sel & ASN1_LOG_ERR,_asn1_log_err_logstr,a)
#define ALOG_ERR1(a,b) \
	SLOG_1 (asn1_debug_sel & ASN1_LOG_ERR,_asn1_log_err_logstr,a,b)
#define ALOG_ERR2(a,b,c) \
	SLOG_2 (asn1_debug_sel & ASN1_LOG_ERR,_asn1_log_err_logstr,a,b,c)
#define ALOG_ERR3(a,b,c,d) \
	SLOG_3 (asn1_debug_sel & ASN1_LOG_ERR,_asn1_log_err_logstr,a,b,c,d)

#define ALOG_NERR0(a) \
	SLOG_0 (asn1_debug_sel & ASN1_LOG_NERR,_asn1_log_nerr_logstr,a)
#define ALOG_NERR1(a,b) \
	SLOG_1 (asn1_debug_sel & ASN1_LOG_NERR,_asn1_log_nerr_logstr,a,b)
#define ALOG_NERR2(a,b,c) \
	SLOG_2 (asn1_debug_sel & ASN1_LOG_NERR,_asn1_log_nerr_logstr,a,b,c)
#define ALOG_NERR3(a,b,c,d) \
	SLOG_3 (asn1_debug_sel & ASN1_LOG_NERR,_asn1_log_nerr_logstr,a,b,c,d)

#define ALOG_NERRH(a,b) \
	SLOGH (asn1_debug_sel & ASN1_LOG_NERR,a,b)

#ifdef __cplusplus
}
#endif


/************************************************************************/
#endif		/* End of ASN1_LOG_INCLUDED				*/
/************************************************************************/
