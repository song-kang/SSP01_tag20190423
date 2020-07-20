/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-1996, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_log.h						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/* All the logging macros are defined in following order for easy 	*/
/* editing of this file. Please keep the same order if logging macros	*/
/* need to be changed							*/
/*									*/
/*   ADLC_MASK_LOG_ERR     	for critical errors                 	*/
/*   ADLC_MASK_LOG_NERR    	for normal errors               	*/
/*   ADLC_MASK_LOG_FLOW    	for program flow logging            	*/
/*   ADLC_MASK_LOG_IO      	for control frames              	*/
/*   ADLC_MASK_LOG_DEBUG   	for debugging purposes          	*/
/*   ADLC_MASK_LOG_ALWAYS  	for always logging              	*/
/*   ADLC_MASK_LOG_SM      	for serial manager logging          	*/                      
/*   ADLC_MASK_LOG_INFO_FRAME  for data coming to or from stack  	*/
/*   ADLC_MASK_LOG_USER  	from or to user (stack) logging       	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/13/99  MDE    19    Added SD_CONST modifiers			*/
/* 02/01/99  JRB    18     Fix ADLC_LOG_DEV_FILT macros for new API.	*/
/*			   Put back ADLC_MAX_LOG_MASK. ADLC_LM needs it.*/
/* 10/08/98  MDE    17     Migrated to updated SLOG interface		*/
/* 09/01/98  IKE    16     Removed the if statement for ALWAYS macros	*/
/* 09/18/97  IKE    15     Added telephone support			*/
/* 08/27/97  IKE    14     Fixed number of parameters in 		*/
/*			   ADLC_LOG_DEV_FILT macros for non debug ver	*/
/* 07/16/97  IKE    13     Added ADLC_LOG_DEV_FILT			*/
/* 06/13/97  EJV    12     Added ADLC_LOG_ERR5				*/
/* 05/12/97  IKE    11     Changed to MMSEASE 7.0 Data Types		*/
/* 04/30/97  IKE    10     Updated ADLC_MAX_LOG_MASK			*/
/* 02/24/97  IKE    09     Changed logMask6 to use adlc_debug_sel	*/
/*			   moved log masks to adlc_usr.h, changed	*/
/*			   the logging macros to use slog functions	*/
/* 			   deleted macros specific to OS2		*/
/* 12/03/96  IKE    08     WIN32 Support, header files cleanup		*/ 
/* 11/06/96  IKE    07     Changed C++ style comments to C style 	*/
/*			   comments to make it more platform independent*/
/* 10/28/96  KCR    06     Changed LT of HEX logging macros to _CONT	*/
/* 10/22/96  IKE    05     Added LOG_ALWAYS5				*/
/* 10/04/96  IKE    04     Changed DEBUG_MMS to DEBUG_SISCO		*/
/* 06/21/96  IKE    03     Added ADLC_SM, ADLC_INFO_FRAME and ADLC_USER */
/*			   Changed from slog_remote to SLOG6_REM_xxx	*/
/* 04/04/96  rkr    02	   Cleanup, removed slog_ipc.h			*/
/* 01/20/96  IKE    01	   Merged NAV's OS2 code			*/
/* 11/25/95  IKE   1.00    Created					*/
/************************************************************************/

#ifndef ADLC_LOG_INCLUDED
#define ADLC_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "slog.h"

/************************************************************************/
/* The Logging Control structure					*/
/* Note that more than one may be used, and there is a 1-to-1 mapping	*/
/* of the control structure to associated file && memory buffer.	*/
/************************************************************************/

#define ADLC_MAX_LOG_MASK (ADLC_MASK_LOG_ERR | ADLC_MASK_LOG_NERR | \
		ADLC_MASK_LOG_FLOW | \
		ADLC_MASK_LOG_IO | ADLC_MASK_LOG_HEXIO | \
		ADLC_MASK_LOG_DEBUG | ADLC_MASK_LOG_HEXDEBUG | \
		ADLC_MASK_LOG_SM | ADLC_MASK_LOG_HEXSM | \
		ADLC_MASK_LOG_INFO_FRAME | ADLC_MASK_LOG_HEXINFO_FRAME | \
		ADLC_MASK_LOG_USER | ADLC_MASK_LOG_HEXUSER)
      
#define ADLC_LOG_CLOSE_FILE (p) {slogCloseFile (p);}

/* Control bit set/clear macros                                         */

#define ADLC_SET_CTRL(a)      {sLogCtrl->logCtrl |=  (a);}
#define ADLC_CLR_CTRL(a)      {sLogCtrl->logCtrl &= ~(a);}

#define ADLC_SET_FILE_CTRL(a) {sLogCtrl->fc.ctrl |=  (a);}
#define ADLC_CLR_FILE_CTRL(a) {sLogCtrl->fc.ctrl &= ~(a);}

#define ADLC_SET_LOG_MASK(a) {adlc_debug_sel |=  (a);}
#define ADLC_CLR_LOG_MASK(a) {adlc_debug_sel &= ~(a);}


#ifdef DEBUG_SISCO	/* if DEBUG_SISCO is defined, expand macros */

/************************************************************************/
/* ADLC LOGGING CONTROL							*/
/************************************************************************/

/* Log Type Strings */
extern SD_CONST ST_CHAR *_adlc_err_logstr;
extern SD_CONST ST_CHAR *_adlc_nerr_logstr;
extern SD_CONST ST_CHAR *_adlc_flow_logstr;
extern SD_CONST ST_CHAR *_adlc_io_logstr;
extern SD_CONST ST_CHAR *_adlc_debug_logstr;
extern SD_CONST ST_CHAR *_adlc_always_logstr;
extern SD_CONST ST_CHAR *_adlc_sm_logstr;
extern SD_CONST ST_CHAR *_adlc_info_frame_logstr;
extern SD_CONST ST_CHAR *_adlc_user_logstr;

/* ---------------------------------------------------------------	*/
/*  ------------------ Error Logging Macros ------------------		*/
#define ADLC_LOG_ERR0(a)           {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slog (sLogCtrl, _adlc_err_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_ERR1(a,b)         {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slog (sLogCtrl, _adlc_err_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_ERR2(a,b,c)       {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slog (sLogCtrl, _adlc_err_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_ERR3(a,b,c,d)     {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slog (sLogCtrl, _adlc_err_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_ERR4(a,b,c,d,e)   {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slog (sLogCtrl, _adlc_err_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

#define ADLC_LOG_ERR5(a,b,c,d,e,f) {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slog (sLogCtrl, _adlc_err_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e,f);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CERR0(a)          {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CERR1(a,b)        {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CERR2(a,b,c)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CERR3(a,b,c,d)    {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CERR4(a,b,c,d,e)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			

#define ADLC_LOG_ERR_HEX(a,b)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_ERR)\
				     _slogHex (sLogCtrl,a,b);\
			           }			


/* ---------------------------------------------------------------	*/
/*  ------------------ Normal Error Logging Macros ------------------	*/

#define ADLC_LOG_NERR0(a)           {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slog (sLogCtrl, _adlc_nerr_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_NERR1(a,b)         {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slog (sLogCtrl, _adlc_nerr_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_NERR2(a,b,c)       {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slog (sLogCtrl, _adlc_nerr_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_NERR3(a,b,c,d)     {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slog (sLogCtrl, _adlc_nerr_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_NERR4(a,b,c,d,e)   {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slog (sLogCtrl, _adlc_nerr_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CNERR0(a)          {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CNERR1(a,b)        {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CNERR2(a,b,c)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CNERR3(a,b,c,d)    {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CNERR4(a,b,c,d,e)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			

#define ADLC_LOG_NERR_HEX(a,b)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_NERR)\
				     _slogHex (sLogCtrl,a,b);\
			           }			

/* ---------------------------------------------------------------	*/
/* ------------------ Flow Logging Macros ------------------		*/

#define ADLC_LOG_FLOW0(a)           {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slog (sLogCtrl, _adlc_flow_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_FLOW1(a,b)         {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slog (sLogCtrl, _adlc_flow_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_FLOW2(a,b,c)       {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slog (sLogCtrl, _adlc_flow_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_FLOW3(a,b,c,d)     {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slog (sLogCtrl, _adlc_flow_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_FLOW4(a,b,c,d,e)   {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slog (sLogCtrl, _adlc_flow_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CFLOW0(a)          {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CFLOW1(a,b)        {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CFLOW2(a,b,c)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CFLOW3(a,b,c,d)    {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CFLOW4(a,b,c,d,e)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			

#define ADLC_LOG_FLOW_HEX(a,b)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_FLOW)\
				     _slogHex (sLogCtrl,a,b);\
			           }			

/* ---------------------------------------------------------------	*/
/* ------------------ I/O Logging Macros ------------------		*/

#define ADLC_LOG_IO0(a)           {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_IO1(a,b)         {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_IO2(a,b,c)       {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_IO3(a,b,c,d)     {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_IO4(a,b,c,d,e)   {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CIO0(a)          {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CIO1(a,b)        {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CIO2(a,b,c)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CIO3(a,b,c,d)    {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CIO4(a,b,c,d,e)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_IO)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			

#define ADLC_LOG_IO_HEX(a,b)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_HEXIO)\
				     _slogHex (sLogCtrl,a,b);\
			           }			

/* ---------------------------------------------------------------	*/
/* ------------------ Debug Logging Macros ------------------		*/

#define ADLC_LOG_DEBUG0(a)           {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slog (sLogCtrl, _adlc_debug_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_DEBUG1(a,b)         {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slog (sLogCtrl, _adlc_debug_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_DEBUG2(a,b,c)       {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slog (sLogCtrl, _adlc_debug_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_DEBUG3(a,b,c,d)     {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slog (sLogCtrl, _adlc_debug_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_DEBUG4(a,b,c,d,e)   {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slog (sLogCtrl, _adlc_debug_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CDEBUG0(a)          {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CDEBUG1(a,b)        {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CDEBUG2(a,b,c)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CDEBUG3(a,b,c,d)    {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CDEBUG4(a,b,c,d,e)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_DEBUG)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			

#define ADLC_LOG_DEBUG_HEX(a,b)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_HEXDEBUG)\
				     _slogHex (sLogCtrl,a,b);\
			           }			

/* ---------------------------------------------------------------	*/
/* ------------------ Always Logging Macros ------------------		*/

#define ADLC_LOG_ALWAYS0(a)           {\
				     _slog (sLogCtrl, _adlc_always_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_ALWAYS1(a,b)         {\
				     _slog (sLogCtrl, _adlc_always_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_ALWAYS2(a,b,c)       {\
				     _slog (sLogCtrl, _adlc_always_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_ALWAYS3(a,b,c,d)     {\
				     _slog (sLogCtrl, _adlc_always_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_ALWAYS4(a,b,c,d,e)   {\
				     _slog (sLogCtrl, _adlc_always_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

#define ADLC_LOG_ALWAYS5(a,b,c,d,e,f)   {\
				     _slog (sLogCtrl, _adlc_always_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e,f);\
			           }			
/* 'Continue' versions							*/
#define ADLC_LOG_CALWAYS0(a)          {\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CALWAYS1(a,b)        {\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CALWAYS2(a,b,c)      {\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CALWAYS3(a,b,c,d)    {\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CALWAYS4(a,b,c,d,e)  {\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			
#define ADLC_LOG_CALWAYS5(a,b,c,d,e,f)  {\
				     _slogc (sLogCtrl,a,b,c,d,e,f);\
			           }			

#define ADLC_LOG_ALWAYS_HEX(a,b)      {\
				     _slogHex (sLogCtrl,a,b);\
			           }			

/* ---------------------------------------------------------------	*/
/* ------------------ SM Logging Macros ------------------		*/

#define ADLC_LOG_SM0(a)           {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slog (sLogCtrl, _adlc_sm_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_SM1(a,b)         {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slog (sLogCtrl, _adlc_sm_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_SM2(a,b,c)       {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slog (sLogCtrl, _adlc_sm_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_SM3(a,b,c,d)     {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slog (sLogCtrl, _adlc_sm_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_SM4(a,b,c,d,e)   {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slog (sLogCtrl, _adlc_sm_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CSM0(a)          {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CSM1(a,b)        {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CSM2(a,b,c)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CSM3(a,b,c,d)    {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CSM4(a,b,c,d,e)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_SM)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			

#define ADLC_LOG_SM_HEX(a,b)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_HEXSM)\
				     _slogHex (sLogCtrl,a,b);\
			           }			

/* ---------------------------------------------------------------	*/
/* ------------------ INFO_FRAME Logging Macros ------------------	*/

#define ADLC_LOG_INFO_FRAME0(a)           {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slog (sLogCtrl, _adlc_info_frame_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_INFO_FRAME1(a,b)         {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slog (sLogCtrl, _adlc_info_frame_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_INFO_FRAME2(a,b,c)       {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slog (sLogCtrl, _adlc_info_frame_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_INFO_FRAME3(a,b,c,d)     {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slog (sLogCtrl, _adlc_info_frame_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_INFO_FRAME4(a,b,c,d,e)   {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slog (sLogCtrl, _adlc_info_frame_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CINFO_FRAME0(a)          {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CINFO_FRAME1(a,b)        {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CINFO_FRAME2(a,b,c)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CINFO_FRAME3(a,b,c,d)    {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CINFO_FRAME4(a,b,c,d,e)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_INFO_FRAME)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			

#define ADLC_LOG_INFO_FRAME_HEX(a,b)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_HEXINFO_FRAME)\
				     _slogHex (sLogCtrl,a,b);\
			           }			

/* ---------------------------------------------------------------	*/
/* ------------------ USER Logging Macros ------------------		*/

#define ADLC_LOG_USER0(a)           {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slog (sLogCtrl, _adlc_user_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_USER1(a,b)         {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slog (sLogCtrl, _adlc_user_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_USER2(a,b,c)       {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slog (sLogCtrl, _adlc_user_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_USER3(a,b,c,d)     {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slog (sLogCtrl, _adlc_user_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_USER4(a,b,c,d,e)   {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slog (sLogCtrl, _adlc_user_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			
#define ADLC_LOG_USER5(a,b,c,d,e,f) {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slog (sLogCtrl, _adlc_user_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e,f);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CUSER0(a)          {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CUSER1(a,b)        {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CUSER2(a,b,c)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CUSER3(a,b,c,d)    {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CUSER4(a,b,c,d,e)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			
#define ADLC_LOG_CUSER5(a,b,c,d,e,f)  {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_USER)\
				     _slogc (sLogCtrl,a,b,c,d,e,f);\
			           }	
#define ADLC_LOG_USER_HEX(a,b)      {\
				   if (adlc_debug_sel & ADLC_MASK_LOG_HEXUSER)\
				     _slogHex (sLogCtrl,a,b);\
			           }			

/* ---------------------------------------------------------------	*/
/* ------------------ DEV_FILT Logging Macros ------------------		*/
#if defined(ADLC_LM)
#define ADLC_LOG_DEV_FILT0(x,a)           {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a);\
			           }			
#define ADLC_LOG_DEV_FILT1(x,a,b)         {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a,b);\
			           }			
#define ADLC_LOG_DEV_FILT2(x,a,b,c)       {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a,b,c);\
			           }			
#define ADLC_LOG_DEV_FILT3(x,a,b,c,d)     {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a,b,c,d);\
			           }			
#define ADLC_LOG_DEV_FILT4(x,a,b,c,d,e)   {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slog (sLogCtrl, _adlc_io_logstr,\
				  		thisFileName,__LINE__,a,b,c,d,e);\
			           }			

/* 'Continue' versions							*/
#define ADLC_LOG_CDEV_FILT0(x,a)          {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slogc (sLogCtrl,a);\
			           }			
#define ADLC_LOG_CDEV_FILT1(x,a,b)        {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slogc (sLogCtrl,a,b);\
			           }			
#define ADLC_LOG_CDEV_FILT2(x,a,b,c)      {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slogc (sLogCtrl,a,b,c);\
			           }			
#define ADLC_LOG_CDEV_FILT3(x,a,b,c,d)    {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slogc (sLogCtrl,a,b,c,d);\
			           }			
#define ADLC_LOG_CDEV_FILT4(x,a,b,c,d,e)  {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slogc (sLogCtrl,a,b,c,d,e);\
			           }			

#define ADLC_LOG_DEV_FILT_HEX(x,a,b)      {\
				   if (stFiltDevLog.bFilter && stFiltDevLog.address == x)\
				     _slogHex (sLogCtrl,a,b);\
			           }			
#endif /* ADLC_LM */
#else  /* DEBUG_SISCO not defined */

/*-----------------------------------------------------------------------
 * Error Logging Macros for Hard( serious ) Errors
 *----------------------------------------------------------------------*/
 
#define ADLC_LOG_ERR0(a)
#define ADLC_LOG_ERR1(a,b)
#define ADLC_LOG_ERR2(a,b,c)
#define ADLC_LOG_ERR3(a,b,c,d)
#define ADLC_LOG_ERR4(a,b,c,d,e)
#define ADLC_LOG_ERR5(a,b,c,d,e,f)
	     
#define ADLC_LOG_CERR0(a)
#define ADLC_LOG_CERR1(a,b)
#define ADLC_LOG_CERR2(a,b,c)
#define ADLC_LOG_CERR3(a,b,c,d)
#define ADLC_LOG_CERR4(a,b,c,d,e)
	
#define ADLC_LOG_ERR_HEX(a,b)

/*-----------------------------------------------------------------------
 * Logging Macros for on Errors
 *----------------------------------------------------------------------*/

#define ADLC_LOG_NERR0(a)
#define ADLC_LOG_NERR1(a,b)
#define ADLC_LOG_NERR2(a,b,c)
#define ADLC_LOG_NERR3(a,b,c,d)
#define ADLC_LOG_NERR4(a,b,c,d,e) 

#define ADLC_LOG_CNERR0(a)
#define ADLC_LOG_CNERR1(a,b)
#define ADLC_LOG_CNERR2(a,b,c)
#define ADLC_LOG_CNERR3(a,b,c,d)
#define ADLC_LOG_CNERR4(a,b,c,d,e) 

#define ADLC_LOG_NERR_HEX(a,b)    
/*-----------------------------------------------------------------------
 * Flow Logging Macros
 *----------------------------------------------------------------------*/

#define ADLC_LOG_FLOW0(a)
#define ADLC_LOG_FLOW1(a,b)
#define ADLC_LOG_FLOW2(a,b,c)
#define ADLC_LOG_FLOW3(a,b,c,d)
#define ADLC_LOG_FLOW4(a,b,c,d,e)
#define ADLC_LOG_FLOW5(a,b,c,d,e,f)

#define ADLC_LOG_CFLOW0(a)
#define ADLC_LOG_CFLOW1(a,b)
#define ADLC_LOG_CFLOW2(a,b,c)
#define ADLC_LOG_CFLOW3(a,b,c,d)
#define ADLC_LOG_CFLOW4(a,b,c,d,e)
#define ADLC_LOG_CFLOW5(a,b,c,d,e,f)

#define ADLC_LOG_FLOW_HEX(a,b)    
/*-----------------------------------------------------------------------
 * I/O Logging Macros
 *----------------------------------------------------------------------*/

#define ADLC_LOG_IO0(a)          
#define ADLC_LOG_IO1(a,b)        
#define ADLC_LOG_IO2(a,b,c)      
#define ADLC_LOG_IO3(a,b,c,d)    
#define ADLC_LOG_IO4(a,b,c,d,e)  

#define ADLC_LOG_CIO0(a)          
#define ADLC_LOG_CIO1(a,b)        
#define ADLC_LOG_CIO2(a,b,c)      
#define ADLC_LOG_CIO3(a,b,c,d)    
#define ADLC_LOG_CIO4(a,b,c,d,e)  

#define ADLC_LOG_IO_HEX(a,b)     

/*-----------------------------------------------------------------------
 * Debug Logging Macros
 *----------------------------------------------------------------------*/

#define ADLC_LOG_DEBUG0(a)          
#define ADLC_LOG_DEBUG1(a,b)        
#define ADLC_LOG_DEBUG2(a,b,c)      
#define ADLC_LOG_DEBUG3(a,b,c,d)    
#define ADLC_LOG_DEBUG4(a,b,c,d,e)  

#define ADLC_LOG_CDEBUG0(a)          
#define ADLC_LOG_CDEBUG1(a,b)        
#define ADLC_LOG_CDEBUG2(a,b,c)      
#define ADLC_LOG_CDEBUG3(a,b,c,d)    
#define ADLC_LOG_CDEBUG4(a,b,c,d,e)  

#define ADLC_LOG_DEBUG_HEX(a,b)     

/*-----------------------------------------------------------------------
 * Always Logging Macros
 *----------------------------------------------------------------------*/

#define ADLC_LOG_ALWAYS0(a)	    
#define ADLC_LOG_ALWAYS1(a,b)	    
#define ADLC_LOG_ALWAYS2(a,b,c)	    
#define ADLC_LOG_ALWAYS3(a,b,c,d)   
#define ADLC_LOG_ALWAYS4(a,b,c,d,e) 
#define ADLC_LOG_ALWAYS5(a,b,c,d,e,f) 

#define ADLC_LOG_CALWAYS0(a)	    
#define ADLC_LOG_CALWAYS1(a,b)	    
#define ADLC_LOG_CALWAYS2(a,b,c)    
#define ADLC_LOG_CALWAYS3(a,b,c,d)  
#define ADLC_LOG_CALWAYS4(a,b,c,d,e)
#define ADLC_LOG_CALWAYS5(a,b,c,d,e,f)

#define ADLC_LOG_ALWAYS_HEX(a,b)  

/*-----------------------------------------------------------------------
 * Serial Manager Logging Macros
 *----------------------------------------------------------------------*/

#define ADLC_LOG_SM0(a)     
#define ADLC_LOG_SM1(a,b)       
#define ADLC_LOG_SM2(a,b,c)     
#define ADLC_LOG_SM3(a,b,c,d)   
#define ADLC_LOG_SM4(a,b,c,d,e) 

#define ADLC_LOG_CSM0(a)        
#define ADLC_LOG_CSM1(a,b)      
#define ADLC_LOG_CSM2(a,b,c)    
#define ADLC_LOG_CSM3(a,b,c,d)  
#define ADLC_LOG_CSM4(a,b,c,d,e)

#define ADLC_LOG_SM_HEX(a,b)  

/*-----------------------------------------------------------------------
 * INFO_FRAME Logging Macros
 *----------------------------------------------------------------------*/

#define ADLC_LOG_INFO_FRAME0(a)     
#define ADLC_LOG_INFO_FRAME1(a,b)       
#define ADLC_LOG_INFO_FRAME2(a,b,c)     
#define ADLC_LOG_INFO_FRAME3(a,b,c,d)   
#define ADLC_LOG_INFO_FRAME4(a,b,c,d,e) 

#define ADLC_LOG_CINFO_FRAME0(a)        
#define ADLC_LOG_CINFO_FRAME1(a,b)      
#define ADLC_LOG_CINFO_FRAME2(a,b,c)    
#define ADLC_LOG_CINFO_FRAME3(a,b,c,d)  
#define ADLC_LOG_CINFO_FRAME4(a,b,c,d,e)

#define ADLC_LOG_INFO_FRAME_HEX(a,b)  

/*-----------------------------------------------------------------------
 * USER Logging Macros
 *----------------------------------------------------------------------*/

#define ADLC_LOG_USER0(a)       
#define ADLC_LOG_USER1(a,b)     
#define ADLC_LOG_USER2(a,b,c)       
#define ADLC_LOG_USER3(a,b,c,d)   
#define ADLC_LOG_USER4(a,b,c,d,e) 
#define ADLC_LOG_USER5(a,b,c,d,e,f) 

#define ADLC_LOG_CUSER0(a)      
#define ADLC_LOG_CUSER1(a,b)        
#define ADLC_LOG_CUSER2(a,b,c)    
#define ADLC_LOG_CUSER3(a,b,c,d)  
#define ADLC_LOG_CUSER4(a,b,c,d,e)
#define ADLC_LOG_CUSER5(a,b,c,d,e,f)

#define ADLC_LOG_USER_HEX(a,b)  

/*-----------------------------------------------------------------------
 * DEV_FILT Logging Macros
 *----------------------------------------------------------------------*/

#define ADLC_LOG_DEV_FILT0(x,a)       
#define ADLC_LOG_DEV_FILT1(x,a,b)     
#define ADLC_LOG_DEV_FILT2(x,a,b,c)       
#define ADLC_LOG_DEV_FILT3(x,a,b,c,d)   
#define ADLC_LOG_DEV_FILT4(x,a,b,c,d,e) 

#define ADLC_LOG_CDEV_FILT0(x,a)      
#define ADLC_LOG_CDEV_FILT1(x,a,b)        
#define ADLC_LOG_CDEV_FILT2(x,a,b,c)    
#define ADLC_LOG_CDEV_FILT3(x,a,b,c,d)  
#define ADLC_LOG_CDEV_FILT4(x,a,b,c,d,e)

#define ADLC_LOG_DEV_FILT_HEX(x,a,b)  

#endif		/* for DEBUG_SISCO				*/

#ifdef __cplusplus
}
#endif

#endif /* ADLC_LOG_INCLUDED */

