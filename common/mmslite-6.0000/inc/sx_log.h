/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  	 1999 - 2005, All Rights Reserved.		*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : sx_log.h						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/02/05  EJV     07    Add sx_debug_sel_cfg to save SX masks while	*/
/*			    logging configuration is parsed.		*/
/* 05/06/04  DWL     06    Use new "Base" macros SLOG_*, SLOGC_*, SLOGH.*/
/* 06/06/01  GLB     05    Added SXLOG_CFLOW2                           */
/* 04/26/01  EJV     04    Added/rearranged slog macros for !DEBUG_SISCO*/
/* 01/25/01  JRB     03    Added SXLOG_ERR2,3				*/
/* 10/30/99  MDE     02    Added SXLOG_NERR3				*/
/* 09/17/99  MDE     01    New						*/
/************************************************************************/

#ifndef SX_LOG_INCLUDED
#define SX_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/

#include "slog.h"

/************************************************************************/
/* sx_debug_sel bit assignments						*/

#define SX_LOG_DEC  	0x0001   
#define SX_LOG_ENC  	0x0002   
#define SX_LOG_FLOW  	0x0004   
#define SX_LOG_DEBUG 	0x0008   
#define SX_LOG_ERR  	0x0010   
#define SX_LOG_NERR 	0x0020   

extern ST_UINT sx_debug_sel;
extern ST_UINT sx_debug_sel_cfg;	/* saves SX masks while logging configuration is parsed */

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST _sx_log_dec_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sx_log_enc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sx_log_flow_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sx_log_debug_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sx_log_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sx_log_nerr_logstr;
 
/************************************************************************/
/* Decode log macros 			*/
#define SXLOG_DEC0(a) \
    SLOG_0 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a)
#define SXLOG_DEC1(a,b) \
    SLOG_1 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a,b)
#define SXLOG_DEC2(a,b,c) \
    SLOG_2 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a,b,c)
#define SXLOG_DEC3(a,b,c,d) \
    SLOG_3 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a,b,c,d)
#define SXLOG_DEC4(a,b,c,d,e) \
    SLOG_4 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a,b,c,d,e)
#define SXLOG_DEC5(a,b,c,d,e,f) \
    SLOG_5 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a,b,c,d,e,f)
#define SXLOG_DEC6(a,b,c,d,e,f,g) \
    SLOG_6 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a,b,c,d,e,f,g)
#define SXLOG_DEC7(a,b,c,d,e,f,g,h) \
    SLOG_7 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a,b,c,d,e,f,g,h)
#define SXLOG_DEC8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (sx_debug_sel & SX_LOG_DEC,_sx_log_dec_logstr,a,b,c,d,e,f,g,h,i)

/* Decode continuation log macros	*/
#define SXLOG_CDEC0(a) \
    SLOGC_0 (sx_debug_sel & SX_LOG_DEC,a)
#define SXLOG_CDEC1(a,b) \
    SLOGC_1 (sx_debug_sel & SX_LOG_DEC,a,b)
#define SXLOG_CDEC2(a,b,c) \
    SLOGC_2 (sx_debug_sel & SX_LOG_DEC,a,b,c)
#define SXLOG_CDEC3(a,b,c,d) \
    SLOGC_3 (sx_debug_sel & SX_LOG_DEC,a,b,c,d)
#define SXLOG_CDEC4(a,b,c,d,e) \
    SLOGC_4 (sx_debug_sel & SX_LOG_DEC,a,b,c,d,e)
#define SXLOG_CDEC5(a,b,c,d,e,f) \
    SLOGC_5 (sx_debug_sel & SX_LOG_DEC,a,b,c,d,e,f)
#define SXLOG_CDEC6(a,b,c,d,e,f,g) \
    SLOGC_6 (sx_debug_sel & SX_LOG_DEC,a,b,c,d,e,f,g)
#define SXLOG_CDEC7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (sx_debug_sel & SX_LOG_DEC,a,b,c,d,e,f,g,h)
#define SXLOG_CDEC8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (sx_debug_sel & SX_LOG_DEC,a,b,c,d,e,f,g,h,i)

/* Decode Hex log macro    		*/
#define SXLOG_DECH(a,b) \
    SLOGH (sx_debug_sel & SX_LOG_DEC,a,b) 

/************************************************************************/
/* Encode log macros 			*/
#define SXLOG_ENC0(a) \
    SLOG_0 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a)
#define SXLOG_ENC1(a,b) \
    SLOG_1 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a,b)
#define SXLOG_ENC2(a,b,c) \
    SLOG_2 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a,b,c)
#define SXLOG_ENC3(a,b,c,d) \
    SLOG_3 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a,b,c,d)
#define SXLOG_ENC4(a,b,c,d,e) \
    SLOG_4 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a,b,c,d,e)
#define SXLOG_ENC5(a,b,c,d,e,f) \
    SLOG_5 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a,b,c,d,e,f)
#define SXLOG_ENC6(a,b,c,d,e,f,g) \
    SLOG_6 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a,b,c,d,e,f,g)
#define SXLOG_ENC7(a,b,c,d,e,f,g,h) \
    SLOG_7 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a,b,c,d,e,f,g,h)
#define SXLOG_ENC8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (sx_debug_sel & SX_LOG_ENC,_sx_log_enc_logstr,a,b,c,d,e,f,g,h,i)

/* Encode continuation log macros	*/
#define SXLOG_CENC0(a) \
    SLOGC_0 (sx_debug_sel & SX_LOG_ENC,a)
#define SXLOG_CENC1(a,b) \
    SLOGC_1 (sx_debug_sel & SX_LOG_ENC,a,b)
#define SXLOG_CENC2(a,b,c) \
    SLOGC_2 (sx_debug_sel & SX_LOG_ENC,a,b,c)
#define SXLOG_CENC3(a,b,c,d) \
    SLOGC_3 (sx_debug_sel & SX_LOG_ENC,a,b,c,d)
#define SXLOG_CENC4(a,b,c,d,e) \
    SLOGC_4 (sx_debug_sel & SX_LOG_ENC,a,b,c,d,e)
#define SXLOG_CENC5(a,b,c,d,e,f) \
    SLOGC_5 (sx_debug_sel & SX_LOG_ENC,a,b,c,d,e,f)
#define SXLOG_CENC6(a,b,c,d,e,f,g) \
    SLOGC_6 (sx_debug_sel & SX_LOG_ENC,a,b,c,d,e,f,g)
#define SXLOG_CENC7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (sx_debug_sel & SX_LOG_ENC,a,b,c,d,e,f,g,h)
#define SXLOG_CENC8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (sx_debug_sel & SX_LOG_ENC,a,b,c,d,e,f,g,h,i)

/* Encode Hex log macro    		*/
#define SXLOG_ENCH(a,b) \
    SLOGH (sx_debug_sel & SX_LOG_ENC,a,b) 

/************************************************************************/
/* Flow log macros 			*/
#define SXLOG_FLOW0(a) \
    SLOG_0 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a)
#define SXLOG_FLOW1(a,b) \
    SLOG_1 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a,b)
#define SXLOG_FLOW2(a,b,c) \
    SLOG_2 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a,b,c)
#define SXLOG_FLOW3(a,b,c,d) \
    SLOG_3 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a,b,c,d)
#define SXLOG_FLOW4(a,b,c,d,e) \
    SLOG_4 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a,b,c,d,e)
#define SXLOG_FLOW5(a,b,c,d,e,f) \
    SLOG_5 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a,b,c,d,e,f)
#define SXLOG_FLOW6(a,b,c,d,e,f,g) \
    SLOG_6 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a,b,c,d,e,f,g)
#define SXLOG_FLOW7(a,b,c,d,e,f,g,h) \
    SLOG_7 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a,b,c,d,e,f,g,h)
#define SXLOG_FLOW8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (sx_debug_sel & SX_LOG_FLOW,_sx_log_flow_logstr,a,b,c,d,e,f,g,h,i)

/* Flow continuation log macros		*/
#define SXLOG_CFLOW0(a) \
    SLOGC_0 (sx_debug_sel & SX_LOG_FLOW,a)
#define SXLOG_CFLOW1(a,b) \
    SLOGC_1 (sx_debug_sel & SX_LOG_FLOW,a,b)
#define SXLOG_CFLOW2(a,b,c) \
    SLOGC_2 (sx_debug_sel & SX_LOG_FLOW,a,b,c)
#define SXLOG_CFLOW3(a,b,c,d) \
    SLOGC_3 (sx_debug_sel & SX_LOG_FLOW,a,b,c,d)
#define SXLOG_CFLOW4(a,b,c,d,e) \
    SLOGC_4 (sx_debug_sel & SX_LOG_FLOW,a,b,c,d,e)
#define SXLOG_CFLOW5(a,b,c,d,e,f) \
    SLOGC_5 (sx_debug_sel & SX_LOG_FLOW,a,b,c,d,e,f)
#define SXLOG_CFLOW6(a,b,c,d,e,f,g) \
    SLOGC_6 (sx_debug_sel & SX_LOG_FLOW,a,b,c,d,e,f,g)
#define SXLOG_CFLOW7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (sx_debug_sel & SX_LOG_FLOW,a,b,c,d,e,f,g,h)
#define SXLOG_CFLOW8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (sx_debug_sel & SX_LOG_FLOW,a,b,c,d,e,f,g,h,i)

/* Flow Hex log macro    		*/
#define SXLOG_FLOWH(a,b) \
    SLOGH (sx_debug_sel & SX_LOG_FLOW,a,b) 

/************************************************************************/
/* Debug log macros 			*/
#define SXLOG_DEBUG0(a) \
    SLOG_0 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a)
#define SXLOG_DEBUG1(a,b) \
    SLOG_1 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a,b)
#define SXLOG_DEBUG2(a,b,c) \
    SLOG_2 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a,b,c)
#define SXLOG_DEBUG3(a,b,c,d) \
    SLOG_3 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a,b,c,d)
#define SXLOG_DEBUG4(a,b,c,d,e) \
    SLOG_4 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a,b,c,d,e)
#define SXLOG_DEBUG5(a,b,c,d,e,f) \
    SLOG_5 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a,b,c,d,e,f)
#define SXLOG_DEBUG6(a,b,c,d,e,f,g) \
    SLOG_6 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a,b,c,d,e,f,g)
#define SXLOG_DEBUG7(a,b,c,d,e,f,g,h) \
    SLOG_7 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a,b,c,d,e,f,g,h)
#define SXLOG_DEBUG8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (sx_debug_sel & SX_LOG_DEBUG,_sx_log_debug_logstr,a,b,c,d,e,f,g,h,i)

/* Debug continuation log macros	*/
#define SXLOG_CDEBUG0(a) \
    SLOGC_0 (sx_debug_sel & SX_LOG_DEBUG,a)
#define SXLOG_CDEBUG1(a,b) \
    SLOGC_1 (sx_debug_sel & SX_LOG_DEBUG,a,b)
#define SXLOG_CDEBUG2(a,b,c) \
    SLOGC_2 (sx_debug_sel & SX_LOG_DEBUG,a,b,c)
#define SXLOG_CDEBUG3(a,b,c,d) \
    SLOGC_3 (sx_debug_sel & SX_LOG_DEBUG,a,b,c,d)
#define SXLOG_CDEBUG4(a,b,c,d,e) \
    SLOGC_4 (sx_debug_sel & SX_LOG_DEBUG,a,b,c,d,e)
#define SXLOG_CDEBUG5(a,b,c,d,e,f) \
    SLOGC_5 (sx_debug_sel & SX_LOG_DEBUG,a,b,c,d,e,f)
#define SXLOG_CDEBUG6(a,b,c,d,e,f,g) \
    SLOGC_6 (sx_debug_sel & SX_LOG_DEBUG,a,b,c,d,e,f,g)
#define SXLOG_CDEBUG7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (sx_debug_sel & SX_LOG_DEBUG,a,b,c,d,e,f,g,h)
#define SXLOG_CDEBUG8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (sx_debug_sel & SX_LOG_DEBUG,a,b,c,d,e,f,g,h,i)

/* Debug Hex log macro    		*/
#define SXLOG_DEBUGH(a,b) \
    SLOGH (sx_debug_sel & SX_LOG_DEBUG,a,b) 

/************************************************************************/
/* Err log macros 			*/
#define SXLOG_ERR0(a) \
    SLOG_0 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a)
#define SXLOG_ERR1(a,b) \
    SLOG_1 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a,b)
#define SXLOG_ERR2(a,b,c) \
    SLOG_2 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a,b,c)
#define SXLOG_ERR3(a,b,c,d) \
    SLOG_3 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a,b,c,d)
#define SXLOG_ERR4(a,b,c,d,e) \
    SLOG_4 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a,b,c,d,e)
#define SXLOG_ERR5(a,b,c,d,e,f) \
    SLOG_5 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a,b,c,d,e,f)
#define SXLOG_ERR6(a,b,c,d,e,f,g) \
    SLOG_6 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a,b,c,d,e,f,g)
#define SXLOG_ERR7(a,b,c,d,e,f,g,h) \
    SLOG_7 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a,b,c,d,e,f,g,h)
#define SXLOG_ERR8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (sx_debug_sel & SX_LOG_ERR,_sx_log_err_logstr,a,b,c,d,e,f,g,h,i)

/* Err continuation log macros		*/
#define SXLOG_CERR0(a) \
    SLOGC_0 (sx_debug_sel & SX_LOG_ERR,a)
#define SXLOG_CERR1(a,b) \
    SLOGC_1 (sx_debug_sel & SX_LOG_ERR,a,b)
#define SXLOG_CERR2(a,b,c) \
    SLOGC_2 (sx_debug_sel & SX_LOG_ERR,a,b,c)
#define SXLOG_CERR3(a,b,c,d) \
    SLOGC_3 (sx_debug_sel & SX_LOG_ERR,a,b,c,d)
#define SXLOG_CERR4(a,b,c,d,e) \
    SLOGC_4 (sx_debug_sel & SX_LOG_ERR,a,b,c,d,e)
#define SXLOG_CERR5(a,b,c,d,e,f) \
    SLOGC_5 (sx_debug_sel & SX_LOG_ERR,a,b,c,d,e,f)
#define SXLOG_CERR6(a,b,c,d,e,f,g) \
    SLOGC_6 (sx_debug_sel & SX_LOG_ERR,a,b,c,d,e,f,g)
#define SXLOG_CERR7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (sx_debug_sel & SX_LOG_ERR,a,b,c,d,e,f,g,h)
#define SXLOG_CERR8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (sx_debug_sel & SX_LOG_ERR,a,b,c,d,e,f,g,h,i)

/* Err Hex log macro    		*/
#define SXLOG_ERRH(a,b) \
    SLOGH (sx_debug_sel & SX_LOG_ERR,a,b) 

/************************************************************************/
/* NErr log macros 			*/
#define SXLOG_NERR0(a) \
    SLOG_0 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a)
#define SXLOG_NERR1(a,b) \
    SLOG_1 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a,b)
#define SXLOG_NERR2(a,b,c) \
    SLOG_2 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a,b,c)
#define SXLOG_NERR3(a,b,c,d) \
    SLOG_3 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a,b,c,d)
#define SXLOG_NERR4(a,b,c,d,e) \
    SLOG_4 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a,b,c,d,e)
#define SXLOG_NERR5(a,b,c,d,e,f) \
    SLOG_5 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a,b,c,d,e,f)
#define SXLOG_NERR6(a,b,c,d,e,f,g) \
    SLOG_6 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a,b,c,d,e,f,g)
#define SXLOG_NERR7(a,b,c,d,e,f,g,h) \
    SLOG_7 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a,b,c,d,e,f,g,h)
#define SXLOG_NERR8(a,b,c,d,e,f,g,h,i) \
    SLOG_8 (sx_debug_sel & SX_LOG_NERR,_sx_log_nerr_logstr,a,b,c,d,e,f,g,h,i)

/* NErr continuation log macros		*/
#define SXLOG_CNERR0(a) \
    SLOGC_0 (sx_debug_sel & SX_LOG_NERR,a)
#define SXLOG_CNERR1(a,b) \
    SLOGC_1 (sx_debug_sel & SX_LOG_NERR,a,b)
#define SXLOG_CNERR2(a,b,c) \
    SLOGC_2 (sx_debug_sel & SX_LOG_NERR,a,b,c)
#define SXLOG_CNERR3(a,b,c,d) \
    SLOGC_3 (sx_debug_sel & SX_LOG_NERR,a,b,c,d)
#define SXLOG_CNERR4(a,b,c,d,e) \
    SLOGC_4 (sx_debug_sel & SX_LOG_NERR,a,b,c,d,e)
#define SXLOG_CNERR5(a,b,c,d,e,f) \
    SLOGC_5 (sx_debug_sel & SX_LOG_NERR,a,b,c,d,e,f)
#define SXLOG_CNERR6(a,b,c,d,e,f,g) \
    SLOGC_6 (sx_debug_sel & SX_LOG_NERR,a,b,c,d,e,f,g)
#define SXLOG_CNERR7(a,b,c,d,e,f,g,h) \
    SLOGC_7 (sx_debug_sel & SX_LOG_NERR,a,b,c,d,e,f,g,h)
#define SXLOG_CNERR8(a,b,c,d,e,f,g,h,i) \
    SLOGC_8 (sx_debug_sel & SX_LOG_NERR,a,b,c,d,e,f,g,h,i)

/* NErr Hex log macro    		*/
#define SXLOG_NERRH(a,b) \
    SLOGH (sx_debug_sel & SX_LOG_NERR,a,b) 
/************************************************************************/


#ifdef __cplusplus
}
#endif

/************************************************************************/
#endif		/* End of SX_LOG_INCLUDED				*/
/************************************************************************/
