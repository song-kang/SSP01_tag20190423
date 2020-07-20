/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1991-1997 All Rights Reserved       			*/
/*									*/
/* MODULE NAME : cfglog.h						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 10/08/98  MDE     01    Migrated to updated SLOG interface		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef CFGLOG_INCLUDED
#define CFGLOG_INCLUDED

#ifdef __cplusplus		
extern "C" {
#endif

/************************************************************************/
/************************************************************************/

extern SD_CONST ST_CHAR *SD_CONST _cfg_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _cfg_flow_logstr;

/************************************************************************/

#ifdef DEBUG_SISCO

#define CFG_LOG_ERR0(a) {\
                     if (cfg_log_mask & CFG_LOG_ERR)\
                       _slog (sLogCtrl,_cfg_err_logstr,\
		             thisFileName,__LINE__,a);\
                    }
#define CFG_LOG_ERR1(a,b) {\
                     if (cfg_log_mask & CFG_LOG_ERR)\
                       _slog (sLogCtrl,_cfg_err_logstr,\
		             thisFileName,__LINE__,a,b);\
                    }
#define CFG_LOG_ERR2(a,b,c) {\
                     if (cfg_log_mask & CFG_LOG_ERR)\
                       _slog (sLogCtrl,_cfg_err_logstr,\
		             thisFileName,__LINE__,a,b,c);\
                    }
#define CFG_LOG_ERR3(a,b,c,d) {\
                     if (cfg_log_mask & CFG_LOG_ERR)\
                       _slog (sLogCtrl,_cfg_err_logstr,\
		             thisFileName,__LINE__,a,b,c,d);\
                    }


#define CFG_LOG_FLOW0(a) {\
                     if (cfg_log_mask & CFG_LOG_FLOW)\
                       _slog (sLogCtrl,_cfg_flow_logstr,\
		             thisFileName,__LINE__,a);\
                    }
#define CFG_LOG_FLOW1(a,b) {\
                     if (cfg_log_mask & CFG_LOG_FLOW)\
                       _slog (sLogCtrl,_cfg_flow_logstr,\
		             thisFileName,__LINE__,a,b);\
                    }
#define CFG_LOG_CFLOW0(a) {\
                     if (cfg_log_mask & CFG_LOG_FLOW)\
                       _slogc (sLogCtrl,a);\
                    }
#define CFG_LOG_CFLOW1(a,b) {\
                     if (cfg_log_mask & CFG_LOG_FLOW)\
                       _slogc (sLogCtrl,a,b);\
                    }

#else   /* no DEBUG */

#define CFG_LOG_ERR0(a)
#define CFG_LOG_ERR1(a,b)
#define CFG_LOG_ERR2(a,b,c)
#define CFG_LOG_ERR3(a,b,c,d)
#define CFG_LOG_FLOW0(a)
#define CFG_LOG_FLOW1(a,b)
#define CFG_LOG_CFLOW0(a)
#define CFG_LOG_CFLOW1(a,b)

#endif



#ifdef __cplusplus
}
#endif


#endif
