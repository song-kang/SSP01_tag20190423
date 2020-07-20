/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1997, All Rights Reserved 				*/
/*									*/
/* MODULE NAME : stime.h  						*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 06/14/05  EJV     03    Del sGetMsStartTime, sSetMsStartTime		*/
/* 11/04/03  JRB     02    Add sMsSleep (replaces gs_sleep).		*/
/* 11/05/97  DSF     01    Added sGetSysMsTime ()			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

ST_RET     stimeInit (ST_VOID);
ST_DOUBLE  sGetMsTime (ST_VOID);
ST_DOUBLE  sGetSysMsTime (ST_VOID);
ST_VOID    sResetMsTime (ST_VOID);
ST_VOID    stimeExit (ST_VOID);
ST_VOID    sMsSleep (ST_LONG ms);

#ifdef __cplusplus
}
#endif

