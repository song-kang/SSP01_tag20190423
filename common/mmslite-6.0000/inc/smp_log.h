/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1997, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smp_log.h						*/
/* PRODUCT(S)  : UCA Station Management Protocol (UCA SMP).		*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file contains logging macros for SMP.	*/
/*	Note:	To compile logging functions use the DEBUG_SISCO define	*/
/*		in the project make file.				*/
/*									*/
/* MODIFICATION LOG :							*/
/*									*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/13/99  MDE    06     Added SD_CONST modifiers			*/
/* 10/08/98  MDE    05     Migrated to updated SLOG interface		*/
/* 02/20/98  EJV    04     Corrected non-debug macro def		*/
/* 11/17/97  EJV    03     Added check for HEX logging mask		*/
/* 10/15/97  MDE    02     Added missing no debug macros		*/
/* 09/09/97  EJV    01     Created					*/
/************************************************************************/

#ifndef SMP_LOG_INCLUDED
#define SMP_LOG_INCLUDED

#include "slog.h"

#ifdef __cplusplus
extern "C" {
#endif


/* NOTE: SMP uses log masks defined in the smp_usr.h set in the smp_debug_sel */


#if defined(DEBUG_SISCO)

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST _smp_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _smp_req_logstr;
extern SD_CONST ST_CHAR *SD_CONST _smp_ind_logstr;
extern SD_CONST ST_CHAR *SD_CONST _smp_deb_logstr;
extern SD_CONST ST_CHAR *SD_CONST _smp_hex_logstr;

  /*--------------------------------------------*/
  /* hard errors logging			*/
  /*--------------------------------------------*/

  #define SMP_LOG_ERR0(a) {\
			if (smp_debug_sel & SMP_LOG_ERR)\
			  _slog (sLogCtrl,_smp_err_logstr,thisFileName,__LINE__,a);\
			}
  #define SMP_LOG_ERR1(a,b) {\
			if (smp_debug_sel & SMP_LOG_ERR)\
			  _slog (sLogCtrl,_smp_err_logstr,thisFileName,__LINE__,a,b);\
			}
  #define SMP_LOG_ERR2(a,b,c) {\
			if (smp_debug_sel & SMP_LOG_ERR)\
			  _slog (sLogCtrl,_smp_err_logstr,thisFileName,__LINE__,a,b,c);\
			}
  #define SMP_LOG_ERR3(a,b,c,d) {\
			if (smp_debug_sel & SMP_LOG_ERR)\
			  _slog (sLogCtrl,_smp_err_logstr,thisFileName,__LINE__,a,b,c,d);\
			}

  /*--------------------------------------------*/
  /* SMP-user request logging			*/
  /*--------------------------------------------*/

  #define SMP_LOG_REQ0(a) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slog (sLogCtrl,_smp_req_logstr,thisFileName,__LINE__,a);\
			}
  #define SMP_LOG_REQ1(a,b) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slog (sLogCtrl,_smp_req_logstr,thisFileName,__LINE__,a,b);\
			}
  #define SMP_LOG_REQ2(a,b,c) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slog (sLogCtrl,_smp_req_logstr,thisFileName,__LINE__,a,b,c);\
			}
  #define SMP_LOG_REQ3(a,b,c,d) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slog (sLogCtrl,_smp_req_logstr,thisFileName,__LINE__,a,b,c,d);\
			}
  #define SMP_LOG_REQ6(a,b,c,d,e,f,g) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slog (sLogCtrl,_smp_req_logstr,thisFileName,__LINE__,a,b,c,d,e,f,g);\
			}

  /* continuation log */
  #define SMP_LOGC_REQ0(a) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slogc (sLogCtrl,a);\
			}
  #define SMP_LOGC_REQ1(a,b) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slogc (sLogCtrl,a,b);\
			}
  #define SMP_LOGC_REQ2(a,b,c) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slogc (sLogCtrl,a,b,c);\
			}
  #define SMP_LOGC_REQ6(a,b,c,d,e,f,g) {\
			if (smp_debug_sel & SMP_LOG_REQ)\
			  _slogc (sLogCtrl,a,b,c,d,e,f,g);\
			}

  /* hex logging */
  #define SMP_LOGH_REQ(a,b) {\
			if (smp_debug_sel & SMP_LOG_REQ && \
			    smp_debug_sel & SMP_LOG_HEX)\
 			    _slogHex (sLogCtrl,a,b);\
			}

  /* hex continuation logging */
  #define SMP_LOGHC_REQ(a,b) {\
			if (smp_debug_sel & SMP_LOG_REQ && \
			    smp_debug_sel & SMP_LOG_HEX)\
 			    _slogHex (sLogCtrl,a,b);\
			}
  /*--------------------------------------------*/
  /* SMP_user indication logging		*/
  /*--------------------------------------------*/

  #define SMP_LOG_IND0(a) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slog (sLogCtrl,_smp_ind_logstr,thisFileName,__LINE__,a);\
			}
  #define SMP_LOG_IND1(a,b) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slog (sLogCtrl,_smp_ind_logstr,thisFileName,__LINE__,a,b);\
			}
  #define SMP_LOG_IND2(a,b,c) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slog (sLogCtrl,_smp_ind_logstr,thisFileName,__LINE__,a,b,c);\
			}
  #define SMP_LOG_IND3(a,b,c,d) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slog (sLogCtrl,_smp_ind_logstr,thisFileName,__LINE__,a,b,c,d);\
			}
  #define SMP_LOG_IND6(a,b,c,d,e,f,g) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slog (sLogCtrl,_smp_ind_logstr,thisFileName,__LINE__,a,b,c,d,e,f,g);\
			}
  /* continuation log */
  #define SMP_LOGC_IND0(a) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slogc (sLogCtrl,a);\
			}
  #define SMP_LOGC_IND1(a,b) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slogc (sLogCtrl,a,b);\
			}
  #define SMP_LOGC_IND2(a,b,c) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slogc (sLogCtrl,a,b,c);\
                        }
  #define SMP_LOGC_IND6(a,b,c,d,e,f,g) {\
			if (smp_debug_sel & SMP_LOG_IND)\
			  _slogc (sLogCtrl,a,b,c,d,e,f,g);\
			}

  /* hex logging */
  #define SMP_LOGH_IND(a,b) {\
			if (smp_debug_sel & SMP_LOG_IND && \
			    smp_debug_sel & SMP_LOG_HEX)\
 			    _slogHex (sLogCtrl,a,b);\
			}

  /* hex continuation logging */
  #define SMP_LOGHC_IND(a,b) {\
			if (smp_debug_sel & SMP_LOG_IND && \
			    smp_debug_sel & SMP_LOG_HEX)\
 			    _slogHex (sLogCtrl,a,b);\
			}

  /*--------------------------------------------*/
  /* debugging app logging			*/
  /*--------------------------------------------*/

  #define SMP_LOG_DEB0(a) {\
			if (smp_debug_sel & SMP_LOG_DEB)\
			  _slog (sLogCtrl,_smp_deb_logstr,thisFileName,__LINE__,a);\
			}
  #define SMP_LOG_DEB1(a,b) {\
			if (smp_debug_sel & SMP_LOG_DEB)\
			  _slog (sLogCtrl,_smp_deb_logstr,thisFileName,__LINE__,a,b);\
			}
  #define SMP_LOG_DEB2(a,b,c) {\
			if (smp_debug_sel & SMP_LOG_DEB)\
			  _slog (sLogCtrl,_smp_deb_logstr,thisFileName,__LINE__,a,b,c);\
			}
  #define SMP_LOG_DEB3(a,b,c,d) {\
			if (smp_debug_sel & SMP_LOG_DEB)\
			  _slog (sLogCtrl,_smp_deb_logstr,thisFileName,__LINE__,a,b,c,d);\
			}

  #define SMP_LOGC_DEB0(a) {\
			if (smp_debug_sel & SMP_LOG_DEB)\
			  _slogc (sLogCtrl,a);\
			}
  #define SMP_LOGC_DEB1(a,b) {\
			if (smp_debug_sel & SMP_LOG_DEB)\
			  _slogc (sLogCtrl,a,b);\
			}
  #define SMP_LOGC_DEB2(a,b,c) {\
			if (smp_debug_sel & SMP_LOG_DEB)\
			  _slogc (sLogCtrl,a,b,c);\
			}
  #define SMP_LOGC_DEB3(a,b,c,d) {\
			if (smp_debug_sel & SMP_LOG_DEB)\
			  _slogc (sLogCtrl,a,b,c,d);\
			}

#else

  /* this are empty macros for compilation without the DEBUG_SISCO define */

  /*--------------------------------------------*/
  /* hard errors logging			*/
  /*--------------------------------------------*/
  #define SMP_LOG_ERR0(a)
  #define SMP_LOG_ERR1(a,b)
  #define SMP_LOG_ERR2(a,b,c)
  #define SMP_LOG_ERR3(a,b,c,d)

  /*--------------------------------------------*/
  /* SMP-user request (write) logging 		*/
  /*--------------------------------------------*/

  #define SMP_LOG_REQ0(a)
  #define SMP_LOG_REQ1(a,b)
  #define SMP_LOG_REQ2(a,b,c)
  #define SMP_LOG_REQ3(a,b,c,d)
  #define SMP_LOG_REQ6(a,b,c,d,e,f,g)
  #define SMP_LOGC_REQ0(a)
  #define SMP_LOGC_REQ1(a,b)
  #define SMP_LOGC_REQ2(a,b,c)
  #define SMP_LOGC_REQ6(a,b,c,d,e,f,g)
  #define SMP_LOGH_REQ(a,b)
  #define SMP_LOGHC_REQ(a,b)

  /*--------------------------------------------*/
  /* SMP-user indication (read) logging	*/
  /*--------------------------------------------*/

  #define SMP_LOG_IND0(a)
  #define SMP_LOG_IND1(a,b)
  #define SMP_LOG_IND2(a,b,c)
  #define SMP_LOG_IND3(a,b,c,d)
  #define SMP_LOG_IND6(a,b,c,d,e,f,g)
  #define SMP_LOGC_IND0(a)
  #define SMP_LOGC_IND1(a,b)
  #define SMP_LOGC_IND2(a,b,c)
  #define SMP_LOGC_IND6(a,b,c,d,e,f,g)
  #define SMP_LOGH_IND(a,b)
  #define SMP_LOGHC_IND(a,b)

  /*--------------------------------------------*/
  /* debugging app logging			*/
  /*--------------------------------------------*/

  #define SMP_LOG_DEB0(a)
  #define SMP_LOG_DEB1(a,b)
  #define SMP_LOG_DEB2(a,b,c)
  #define SMP_LOG_DEB3(a,b,c,d)
  #define SMP_LOGC_DEB0(a)
  #define SMP_LOGC_DEB1(a,b)
  #define SMP_LOGC_DEB2(a,b,c)
  #define SMP_LOGC_DEB3(a,b,c,d)

#endif


#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/
