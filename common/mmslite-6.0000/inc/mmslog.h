/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2007, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmslog.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains MMS-EASE logging macros used 		*/
/*	internally by MMS-EASE for SLOGGING				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 07/21/09  MDE     08    Added MID log masks				*/
/* 09/05/07  EJV     07    Removed obsolete LLC30_LLP code.		*/
/* 11/01/04  JRB     06    Use new "Base" macros SLOG_*, SLOGC_*, SLOGH.*/
/*			   Change MLOG_ALWAYS* like SLOGALWAYS*.	*/
/* 03/13/02  JRB     05    MLOG_PAUSE* macros do nothing.		*/
/* 10/05/01  EJV     04    Corrected comment.				*/
/* 03/15/01  JRB     03    Added USER log macros.			*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 10/08/98  MDE     01    Migrated to updated SLOG interface		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMSLOG_INCLUDED
#define MMSLOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern SD_CONST ST_CHAR *SD_CONST _mms_log_midd_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_mide_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_dec_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_enc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_acse_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_ique_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_rque_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_ind_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_conf_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_vm_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_pdu_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_config_logstr;
extern SD_CONST ST_CHAR *SD_CONST _mms_log_always_logstr;

extern SD_CONST ST_CHAR *SD_CONST _user_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _user_client_logstr;
extern SD_CONST ST_CHAR *SD_CONST _user_server_logstr;

/*********************** PAUSE Macros ***********************************/
#define MLOG_PAUSEDEC(msg)
#define MLOG_PAUSEENC(msg)
#define MLOG_PAUSEIQUE(msg)
#define MLOG_PAUSERQUE(msg)
#define MLOG_PAUSEPDU(msg)

/*********************** MIDD Macros **********************************/
#define MLOG_MIDD0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a)
#define MLOG_MIDD1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a,b)
#define MLOG_MIDD2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a,b,c)
#define MLOG_MIDD3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a,b,c,d)
#define MLOG_MIDD4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a,b,c,d,e)
#define MLOG_MIDD5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a,b,c,d,e,f)
#define MLOG_MIDD6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a,b,c,d,e,f,g)
#define MLOG_MIDD7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a,b,c,d,e,f,g,h)
#define MLOG_MIDD8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_MIDD,_mms_log_midd_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CMIDD0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_MIDD,a)
#define MLOG_CMIDD1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_MIDD,a,b)
#define MLOG_CMIDD2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_MIDD,a,b,c)
#define MLOG_CMIDD3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_MIDD,a,b,c,d)
#define MLOG_CMIDD4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_MIDD,a,b,c,d,e)
#define MLOG_CMIDD5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_MIDD,a,b,c,d,e,f)
#define MLOG_CMIDD6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_MIDD,a,b,c,d,e,f,g)
#define MLOG_CMIDD7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_MIDD,a,b,c,d,e,f,g,h)
#define MLOG_CMIDD8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_MIDD,a,b,c,d,e,f,g,h,i)

/*********************** MIDE Macros **********************************/
#define MLOG_MIDE0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a)
#define MLOG_MIDE1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a,b)
#define MLOG_MIDE2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a,b,c)
#define MLOG_MIDE3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a,b,c,d)
#define MLOG_MIDE4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a,b,c,d,e)
#define MLOG_MIDE5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a,b,c,d,e,f)
#define MLOG_MIDE6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a,b,c,d,e,f,g)
#define MLOG_MIDE7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a,b,c,d,e,f,g,h)
#define MLOG_MIDE8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_MIDE,_mms_log_mide_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CMIDE0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_MIDE,a)
#define MLOG_CMIDE1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_MIDE,a,b)
#define MLOG_CMIDE2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_MIDE,a,b,c)
#define MLOG_CMIDE3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_MIDE,a,b,c,d)
#define MLOG_CMIDE4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_MIDE,a,b,c,d,e)
#define MLOG_CMIDE5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_MIDE,a,b,c,d,e,f)
#define MLOG_CMIDE6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_MIDE,a,b,c,d,e,f,g)
#define MLOG_CMIDE7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_MIDE,a,b,c,d,e,f,g,h)
#define MLOG_CMIDE8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_MIDE,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_DECH(a,b) \

/*********************** DECODE Macros **********************************/
#define MLOG_DEC0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a)
#define MLOG_DEC1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a,b)
#define MLOG_DEC2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a,b,c)
#define MLOG_DEC3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a,b,c,d)
#define MLOG_DEC4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a,b,c,d,e)
#define MLOG_DEC5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a,b,c,d,e,f)
#define MLOG_DEC6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a,b,c,d,e,f,g)
#define MLOG_DEC7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a,b,c,d,e,f,g,h)
#define MLOG_DEC8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_DEC,_mms_log_dec_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CDEC0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_DEC,a)
#define MLOG_CDEC1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_DEC,a,b)
#define MLOG_CDEC2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_DEC,a,b,c)
#define MLOG_CDEC3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_DEC,a,b,c,d)
#define MLOG_CDEC4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_DEC,a,b,c,d,e)
#define MLOG_CDEC5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_DEC,a,b,c,d,e,f)
#define MLOG_CDEC6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_DEC,a,b,c,d,e,f,g)
#define MLOG_CDEC7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_DEC,a,b,c,d,e,f,g,h)
#define MLOG_CDEC8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_DEC,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_DECH(a,b) \

/*********************** ENCODE Macros **********************************/
#define MLOG_ENC0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a)
#define MLOG_ENC1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a,b)
#define MLOG_ENC2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a,b,c)
#define MLOG_ENC3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a,b,c,d)
#define MLOG_ENC4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a,b,c,d,e)
#define MLOG_ENC5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a,b,c,d,e,f)
#define MLOG_ENC6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a,b,c,d,e,f,g)
#define MLOG_ENC7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a,b,c,d,e,f,g,h)
#define MLOG_ENC8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_ENC,_mms_log_enc_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CENC0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_ENC,a)
#define MLOG_CENC1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_ENC,a,b)
#define MLOG_CENC2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_ENC,a,b,c)
#define MLOG_CENC3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_ENC,a,b,c,d)
#define MLOG_CENC4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_ENC,a,b,c,d,e)
#define MLOG_CENC5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_ENC,a,b,c,d,e,f)
#define MLOG_CENC6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_ENC,a,b,c,d,e,f,g)
#define MLOG_CENC7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_ENC,a,b,c,d,e,f,g,h)
#define MLOG_CENC8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_ENC,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_ENCH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_ENC,a,b) 

/*********************** ACSE Macros ***********************************/
#define MLOG_ACSE0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a)
#define MLOG_ACSE1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a,b)
#define MLOG_ACSE2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a,b,c)
#define MLOG_ACSE3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a,b,c,d)
#define MLOG_ACSE4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a,b,c,d,e)
#define MLOG_ACSE5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a,b,c,d,e,f)
#define MLOG_ACSE6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a,b,c,d,e,f,g)
#define MLOG_ACSE7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a,b,c,d,e,f,g,h)
#define MLOG_ACSE8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_ACSE,_mms_log_acse_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CACSE0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_ACSE,a)
#define MLOG_CACSE1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_ACSE,a,b)
#define MLOG_CACSE2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_ACSE,a,b,c)
#define MLOG_CACSE3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_ACSE,a,b,c,d)
#define MLOG_CACSE4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_ACSE,a,b,c,d,e)
#define MLOG_CACSE5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_ACSE,a,b,c,d,e,f)
#define MLOG_CACSE6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_ACSE,a,b,c,d,e,f,g)
#define MLOG_CACSE7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_ACSE,a,b,c,d,e,f,g,h)
#define MLOG_CACSE8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_ACSE,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_ACSEH(a,b) \

/*********************** IQUE Macros **************************************/
#define MLOG_IQUE0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a)
#define MLOG_IQUE1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a,b)
#define MLOG_IQUE2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a,b,c)
#define MLOG_IQUE3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a,b,c,d)
#define MLOG_IQUE4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a,b,c,d,e)
#define MLOG_IQUE5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a,b,c,d,e,f)
#define MLOG_IQUE6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a,b,c,d,e,f,g)
#define MLOG_IQUE7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a,b,c,d,e,f,g,h)
#define MLOG_IQUE8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_IQUE,_mms_log_ique_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CIQUE0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_IQUE,a)
#define MLOG_CIQUE1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_IQUE,a,b)
#define MLOG_CIQUE2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_IQUE,a,b,c)
#define MLOG_CIQUE3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_IQUE,a,b,c,d)
#define MLOG_CIQUE4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_IQUE,a,b,c,d,e)
#define MLOG_CIQUE5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_IQUE,a,b,c,d,e,f)
#define MLOG_CIQUE6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_IQUE,a,b,c,d,e,f,g)
#define MLOG_CIQUE7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_IQUE,a,b,c,d,e,f,g,h)
#define MLOG_CIQUE8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_IQUE,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_IQUEH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_IQUE,a,b) 

/*********************** RQUE Macros **************************************/
#define MLOG_RQUE0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a)
#define MLOG_RQUE1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a,b)
#define MLOG_RQUE2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a,b,c)
#define MLOG_RQUE3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a,b,c,d)
#define MLOG_RQUE4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a,b,c,d,e)
#define MLOG_RQUE5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a,b,c,d,e,f)
#define MLOG_RQUE6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a,b,c,d,e,f,g)
#define MLOG_RQUE7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a,b,c,d,e,f,g,h)
#define MLOG_RQUE8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_RQUE,_mms_log_rque_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CRQUE0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_RQUE,a)
#define MLOG_CRQUE1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_RQUE,a,b)
#define MLOG_CRQUE2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_RQUE,a,b,c)
#define MLOG_CRQUE3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_RQUE,a,b,c,d)
#define MLOG_CRQUE4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_RQUE,a,b,c,d,e)
#define MLOG_CRQUE5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_RQUE,a,b,c,d,e,f)
#define MLOG_CRQUE6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_RQUE,a,b,c,d,e,f,g)
#define MLOG_CRQUE7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_RQUE,a,b,c,d,e,f,g,h)
#define MLOG_CRQUE8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_RQUE,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_RQUEH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_RQUE,a,b) 

/*********************** REQ Macros **************************************/
#define MLOG_REQ0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a)
#define MLOG_REQ1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a,b)
#define MLOG_REQ2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a,b,c)
#define MLOG_REQ3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a,b,c,d)
#define MLOG_REQ4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a,b,c,d,e)
#define MLOG_REQ5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a,b,c,d,e,f)
#define MLOG_REQ6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a,b,c,d,e,f,g)
#define MLOG_REQ7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a,b,c,d,e,f,g,h)
#define MLOG_REQ8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_REQ,_mms_log_req_logstr,a,b,c,d,e,f,g,h,i)

/* DEBUG: Add Continuation log macros?    */

/* Hex log macro    */
#define MLOG_REQH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_REQ,a,b) 

/*********************** IND Macros **************************************/
#define MLOG_IND0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a)
#define MLOG_IND1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a,b)
#define MLOG_IND2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a,b,c)
#define MLOG_IND3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a,b,c,d)
#define MLOG_IND4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a,b,c,d,e)
#define MLOG_IND5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a,b,c,d,e,f)
#define MLOG_IND6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a,b,c,d,e,f,g)
#define MLOG_IND7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a,b,c,d,e,f,g,h)
#define MLOG_IND8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_IND,_mms_log_ind_logstr,a,b,c,d,e,f,g,h,i)

/* DEBUG: Add Continuation log macros?    */

/* Hex log macro    */
#define MLOG_INDH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_IND,a,b) 

/*********************** RESP Macros **************************************/
#define MLOG_RESP0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a)
#define MLOG_RESP1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a,b)
#define MLOG_RESP2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a,b,c)
#define MLOG_RESP3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a,b,c,d)
#define MLOG_RESP4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a,b,c,d,e)
#define MLOG_RESP5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a,b,c,d,e,f)
#define MLOG_RESP6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a,b,c,d,e,f,g)
#define MLOG_RESP7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a,b,c,d,e,f,g,h)
#define MLOG_RESP8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_RESP,_mms_log_resp_logstr,a,b,c,d,e,f,g,h,i)

/* DEBUG: Add Continuation log macros?    */

/* Hex log macro    */
#define MLOG_RESPH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_RESP,a,b) 

/*********************** CONF Macros **************************************/
#define MLOG_CONF0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a)
#define MLOG_CONF1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a,b)
#define MLOG_CONF2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a,b,c)
#define MLOG_CONF3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a,b,c,d)
#define MLOG_CONF4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a,b,c,d,e)
#define MLOG_CONF5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a,b,c,d,e,f)
#define MLOG_CONF6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a,b,c,d,e,f,g)
#define MLOG_CONF7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a,b,c,d,e,f,g,h)
#define MLOG_CONF8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_CONF,_mms_log_conf_logstr,a,b,c,d,e,f,g,h,i)


/* DEBUG: Add Continuation log macros?    */

/* Hex log macro    */
#define MLOG_CONFH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_CONF,a,b) 

/*********************** VM Macros **************************************/
#define MLOG_VM0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a)
#define MLOG_VM1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a,b)
#define MLOG_VM2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a,b,c)
#define MLOG_VM3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a,b,c,d)
#define MLOG_VM4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a,b,c,d,e)
#define MLOG_VM5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a,b,c,d,e,f)
#define MLOG_VM6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a,b,c,d,e,f,g)
#define MLOG_VM7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a,b,c,d,e,f,g,h)
#define MLOG_VM8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_VM,_mms_log_vm_logstr,a,b,c,d,e,f,g,h,i)

/* DEBUG: Add Continuation log macros?    */

/* Hex log macro    */
#define MLOG_VMH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_VM,a,b) 

/*********************** ERR Macros **************************************/
#define MLOG_ERR0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a)
#define MLOG_ERR1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a,b)
#define MLOG_ERR2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a,b,c)
#define MLOG_ERR3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a,b,c,d)
#define MLOG_ERR4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a,b,c,d,e)
#define MLOG_ERR5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a,b,c,d,e,f)
#define MLOG_ERR6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a,b,c,d,e,f,g)
#define MLOG_ERR7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a,b,c,d,e,f,g,h)
#define MLOG_ERR8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_ERR,_mms_log_err_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CERR0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_ERR,a)
#define MLOG_CERR1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_ERR,a,b)
#define MLOG_CERR2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_ERR,a,b,c)
#define MLOG_CERR3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_ERR,a,b,c,d)
#define MLOG_CERR4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_ERR,a,b,c,d,e)
#define MLOG_CERR5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_ERR,a,b,c,d,e,f)
#define MLOG_CERR6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_ERR,a,b,c,d,e,f,g)
#define MLOG_CERR7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_ERR,a,b,c,d,e,f,g,h)
#define MLOG_CERR8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_ERR,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_ERRH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_ERR,a,b) 

/*********************** NERR Macros *************************************/
#define MLOG_NERR0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a)
#define MLOG_NERR1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a,b)
#define MLOG_NERR2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a,b,c)
#define MLOG_NERR3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a,b,c,d)
#define MLOG_NERR4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a,b,c,d,e)
#define MLOG_NERR5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a,b,c,d,e,f)
#define MLOG_NERR6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a,b,c,d,e,f,g)
#define MLOG_NERR7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a,b,c,d,e,f,g,h)
#define MLOG_NERR8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_NERR,_mms_log_nerr_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CNERR0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_NERR,a)
#define MLOG_CNERR1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_NERR,a,b)
#define MLOG_CNERR2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_NERR,a,b,c)
#define MLOG_CNERR3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_NERR,a,b,c,d)
#define MLOG_CNERR4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_NERR,a,b,c,d,e)
#define MLOG_CNERR5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_NERR,a,b,c,d,e,f)
#define MLOG_CNERR6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_NERR,a,b,c,d,e,f,g)
#define MLOG_CNERR7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_NERR,a,b,c,d,e,f,g,h)
#define MLOG_CNERR8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_NERR,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_NERRH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_NERR,a,b) 

/*********************** PDU Macros **************************************/
#define MLOG_PDU0(a) \
    SLOG_0 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a)
#define MLOG_PDU1(a,b) \
    SLOG_1 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a,b)
#define MLOG_PDU2(a,b,c) \
    SLOG_2 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a,b,c)
#define MLOG_PDU3(a,b,c,d) \
    SLOG_3 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a,b,c,d)
#define MLOG_PDU4(a,b,c,d,e) \
    SLOG_4 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a,b,c,d,e)
#define MLOG_PDU5(a,b,c,d,e,f) \
    SLOG_5 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a,b,c,d,e,f)
#define MLOG_PDU6(a,b,c,d,e,f,g) \
    SLOG_6 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a,b,c,d,e,f,g)
#define MLOG_PDU7(a,b,c,d,e,f,g,h) \
    SLOG_7 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a,b,c,d,e,f,g,h)
#define MLOG_PDU8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (mms_debug_sel & MMS_LOG_PDU,_mms_log_pdu_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define MLOG_CPDU0(a) \
    SLOGC_0 (mms_debug_sel & MMS_LOG_PDU,a)
#define MLOG_CPDU1(a,b) \
    SLOGC_1 (mms_debug_sel & MMS_LOG_PDU,a,b)
#define MLOG_CPDU2(a,b,c) \
    SLOGC_2 (mms_debug_sel & MMS_LOG_PDU,a,b,c)
#define MLOG_CPDU3(a,b,c,d) \
    SLOGC_3 (mms_debug_sel & MMS_LOG_PDU,a,b,c,d)
#define MLOG_CPDU4(a,b,c,d,e) \
    SLOGC_4 (mms_debug_sel & MMS_LOG_PDU,a,b,c,d,e)
#define MLOG_CPDU5(a,b,c,d,e,f) \
    SLOGC_5 (mms_debug_sel & MMS_LOG_PDU,a,b,c,d,e,f)
#define MLOG_CPDU6(a,b,c,d,e,f,g) \
    SLOGC_6 (mms_debug_sel & MMS_LOG_PDU,a,b,c,d,e,f,g)
#define MLOG_CPDU7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (mms_debug_sel & MMS_LOG_PDU,a,b,c,d,e,f,g,h)
#define MLOG_CPDU8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (mms_debug_sel & MMS_LOG_PDU,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define MLOG_PDUH(a,b) \
    SLOGH (mms_debug_sel & MMS_LOG_PDU,a,b) 


/*********************** ALWAYS  Macros *********************************/
/* DEBUG: these are same as "SLOGALWAYS*" except header txt		*/
/* "mms_log_always_logstr". If text is not important, just remap	*/
/* "MLOG_ALWAYS*" like this (#define MLOG_ALWAYS0 SLOGALWAYS0).		*/
#if defined(DEBUG_SISCO)
#define MLOG_ALWAYS0(a) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a)
#define MLOG_ALWAYS1(a,b) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a,b)
#define MLOG_ALWAYS2(a,b,c) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a,b,c)
#define MLOG_ALWAYS3(a,b,c,d) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a,b,c,d)
#define MLOG_ALWAYS4(a,b,c,d,e) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a,b,c,d,e)
#define MLOG_ALWAYS5(a,b,c,d,e,f) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a,b,c,d,e,f)
#define MLOG_ALWAYS6(a,b,c,d,e,f,g) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a,b,c,d,e,f,g)
#define MLOG_ALWAYS7(a,b,c,d,e,f,g,h) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a,b,c,d,e,f,g,h)
#define MLOG_ALWAYS8(a,b,c,d,e,f,g,h,i) \
    _slog (sLogCtrl,_mms_log_always_logstr, thisFileName,__LINE__,a,b,c,d,e,f,g,h,i)
#else	/* !DEBUG_SISCO	*/
#define MLOG_ALWAYS0(a)
#define MLOG_ALWAYS1(a,b)
#define MLOG_ALWAYS2(a,b,c)
#define MLOG_ALWAYS3(a,b,c,d)
#define MLOG_ALWAYS4(a,b,c,d,e)
#define MLOG_ALWAYS5(a,b,c,d,e,f)
#define MLOG_ALWAYS6(a,b,c,d,e,f,g)
#define MLOG_ALWAYS7(a,b,c,d,e,f,g,h)
#define MLOG_ALWAYS8(a,b,c,d,e,f,g,h,i)
#endif	/* !DEBUG_SISCO	*/

/* Continuation & Hex macros are exactly the same as "SLOG*" macros, so just remap to them.*/
#define MLOG_CALWAYS0	SLOGCALWAYS0
#define MLOG_CALWAYS1	SLOGCALWAYS1
#define MLOG_CALWAYS2	SLOGCALWAYS2
#define MLOG_CALWAYS3	SLOGCALWAYS3
#define MLOG_CALWAYS4	SLOGCALWAYS4
#define MLOG_CALWAYS5	SLOGCALWAYS5
#define MLOG_CALWAYS6	SLOGCALWAYS6
#define MLOG_CALWAYS7	SLOGCALWAYS7
#define MLOG_CALWAYS8	SLOGCALWAYS8

#define MLOG_ALWAYSH	SLOGALWAYSH


/*********************** USER ERR Macros********************************/
#define USER_LOG_ERR0(a) \
    SLOG_0 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a)
#define USER_LOG_ERR1(a,b) \
    SLOG_1 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a,b)
#define USER_LOG_ERR2(a,b,c) \
    SLOG_2 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a,b,c)
#define USER_LOG_ERR3(a,b,c,d) \
    SLOG_3 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a,b,c,d)
#define USER_LOG_ERR4(a,b,c,d,e) \
    SLOG_4 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a,b,c,d,e)
#define USER_LOG_ERR5(a,b,c,d,e,f) \
    SLOG_5 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a,b,c,d,e,f)
#define USER_LOG_ERR6(a,b,c,d,e,f,g) \
    SLOG_6 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a,b,c,d,e,f,g)
#define USER_LOG_ERR7(a,b,c,d,e,f,g,h) \
    SLOG_7 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a,b,c,d,e,f,g,h)
#define USER_LOG_ERR8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (user_debug_sel & USER_LOG_ERR,_user_err_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define USER_LOG_ERRC0(a) \
    SLOGC_0 (user_debug_sel & USER_LOG_ERR,a)
#define USER_LOG_ERRC1(a,b) \
    SLOGC_1 (user_debug_sel & USER_LOG_ERR,a,b)
#define USER_LOG_ERRC2(a,b,c) \
    SLOGC_2 (user_debug_sel & USER_LOG_ERR,a,b,c)
#define USER_LOG_ERRC3(a,b,c,d) \
    SLOGC_3 (user_debug_sel & USER_LOG_ERR,a,b,c,d)
#define USER_LOG_ERRC4(a,b,c,d,e) \
    SLOGC_4 (user_debug_sel & USER_LOG_ERR,a,b,c,d,e)
#define USER_LOG_ERRC5(a,b,c,d,e,f) \
    SLOGC_5 (user_debug_sel & USER_LOG_ERR,a,b,c,d,e,f)
#define USER_LOG_ERRC6(a,b,c,d,e,f,g) \
    SLOGC_6 (user_debug_sel & USER_LOG_ERR,a,b,c,d,e,f,g)
#define USER_LOG_ERRC7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (user_debug_sel & USER_LOG_ERR,a,b,c,d,e,f,g,h)
#define USER_LOG_ERRC8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (user_debug_sel & USER_LOG_ERR,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define USER_LOG_ERRH(a,b) \
    SLOGH (user_debug_sel & USER_LOG_ERR,a,b) 

/*********************** USER Client Macros ****************************/
#define USER_LOG_CLIENT0(a) \
    SLOG_0 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a)
#define USER_LOG_CLIENT1(a,b) \
    SLOG_1 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a,b)
#define USER_LOG_CLIENT2(a,b,c) \
    SLOG_2 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a,b,c)
#define USER_LOG_CLIENT3(a,b,c,d) \
    SLOG_3 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a,b,c,d)
#define USER_LOG_CLIENT4(a,b,c,d,e) \
    SLOG_4 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a,b,c,d,e)
#define USER_LOG_CLIENT5(a,b,c,d,e,f) \
    SLOG_5 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a,b,c,d,e,f)
#define USER_LOG_CLIENT6(a,b,c,d,e,f,g) \
    SLOG_6 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a,b,c,d,e,f,g)
#define USER_LOG_CLIENT7(a,b,c,d,e,f,g,h) \
    SLOG_7 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a,b,c,d,e,f,g,h)
#define USER_LOG_CLIENT8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (user_debug_sel & USER_LOG_CLIENT,_user_client_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define USER_LOG_CLIENTC0(a) \
    SLOGC_0 (user_debug_sel & USER_LOG_CLIENT,a)
#define USER_LOG_CLIENTC1(a,b) \
    SLOGC_1 (user_debug_sel & USER_LOG_CLIENT,a,b)
#define USER_LOG_CLIENTC2(a,b,c) \
    SLOGC_2 (user_debug_sel & USER_LOG_CLIENT,a,b,c)
#define USER_LOG_CLIENTC3(a,b,c,d) \
    SLOGC_3 (user_debug_sel & USER_LOG_CLIENT,a,b,c,d)
#define USER_LOG_CLIENTC4(a,b,c,d,e) \
    SLOGC_4 (user_debug_sel & USER_LOG_CLIENT,a,b,c,d,e)
#define USER_LOG_CLIENTC5(a,b,c,d,e,f) \
    SLOGC_5 (user_debug_sel & USER_LOG_CLIENT,a,b,c,d,e,f)
#define USER_LOG_CLIENTC6(a,b,c,d,e,f,g) \
    SLOGC_6 (user_debug_sel & USER_LOG_CLIENT,a,b,c,d,e,f,g)
#define USER_LOG_CLIENTC7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (user_debug_sel & USER_LOG_CLIENT,a,b,c,d,e,f,g,h)
#define USER_LOG_CLIENTC8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (user_debug_sel & USER_LOG_CLIENT,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define USER_LOG_CLIENTH(a,b) \
    SLOGH (user_debug_sel & USER_LOG_CLIENT,a,b) 

/*********************** USER Server Macros ****************************/
#define USER_LOG_SERVER0(a) \
    SLOG_0 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a)
#define USER_LOG_SERVER1(a,b) \
    SLOG_1 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a,b)
#define USER_LOG_SERVER2(a,b,c) \
    SLOG_2 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a,b,c)
#define USER_LOG_SERVER3(a,b,c,d) \
    SLOG_3 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a,b,c,d)
#define USER_LOG_SERVER4(a,b,c,d,e) \
    SLOG_4 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a,b,c,d,e)
#define USER_LOG_SERVER5(a,b,c,d,e,f) \
    SLOG_5 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a,b,c,d,e,f)
#define USER_LOG_SERVER6(a,b,c,d,e,f,g) \
    SLOG_6 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a,b,c,d,e,f,g)
#define USER_LOG_SERVER7(a,b,c,d,e,f,g,h) \
    SLOG_7 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a,b,c,d,e,f,g,h)
#define USER_LOG_SERVER8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (user_debug_sel & USER_LOG_SERVER,_user_server_logstr,a,b,c,d,e,f,g,h,i)

/* Continuation log macros    */
#define USER_LOG_SERVERC0(a) \
    SLOGC_0 (user_debug_sel & USER_LOG_SERVER,a)
#define USER_LOG_SERVERC1(a,b) \
    SLOGC_1 (user_debug_sel & USER_LOG_SERVER,a,b)
#define USER_LOG_SERVERC2(a,b,c) \
    SLOGC_2 (user_debug_sel & USER_LOG_SERVER,a,b,c)
#define USER_LOG_SERVERC3(a,b,c,d) \
    SLOGC_3 (user_debug_sel & USER_LOG_SERVER,a,b,c,d)
#define USER_LOG_SERVERC4(a,b,c,d,e) \
    SLOGC_4 (user_debug_sel & USER_LOG_SERVER,a,b,c,d,e)
#define USER_LOG_SERVERC5(a,b,c,d,e,f) \
    SLOGC_5 (user_debug_sel & USER_LOG_SERVER,a,b,c,d,e,f)
#define USER_LOG_SERVERC6(a,b,c,d,e,f,g) \
    SLOGC_6 (user_debug_sel & USER_LOG_SERVER,a,b,c,d,e,f,g)
#define USER_LOG_SERVERC7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (user_debug_sel & USER_LOG_SERVER,a,b,c,d,e,f,g,h)
#define USER_LOG_SERVERC8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (user_debug_sel & USER_LOG_SERVER,a,b,c,d,e,f,g,h,i)

/* Hex log macro    */
#define USER_LOG_SERVERH(a,b) \
    SLOGH (user_debug_sel & USER_LOG_SERVER,a,b) 

#define USER_LOG_SERVER_HEX	USER_LOG_SERVERH	/* for backward compatibility*/

#ifdef __cplusplus
}
#endif

/************************************************************************/
#endif		/* End of MMS_LOG_INCLUDED				*/
/************************************************************************/

