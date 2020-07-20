/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2003, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_log.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains MMS-EASE logging definitions		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 07/21/09  MDE     05    Added MID log masks				*/
/* 09/04/03  EJV     04    Chg mms_debug_sel to ST_UINT, and deleted	*/
/*			     L suffix from MMS_LOG_... masks.		*/
/* 03/15/01  JRB     03    Added user_debug_sel to control USER logging.*/
/* 10/08/98  MDE     02    Migrated to updated SLOG interface		*/
/* 06/09/97  MDE     01    Added 2 new Log macros			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_LOG_INCLUDED
#define MMS_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/

#include "slog.h"

/************************************************************************/
/* mms_debug_sel bit assignments					*/
#define MMS_LOG_DEC      0x00000001
#define MMS_LOG_ENC      0x00000002
#define MMS_LOG_ACSE     0x00000004
#define MMS_LOG_LLC      0x00000008
#define MMS_LOG_IQUE     0x00000010
#define MMS_LOG_RQUE     0x00000020

/* Keep the next two reserved for the user (not used internally)	*/
#define MMS_LOG_USR_IND  0x00000040
#define MMS_LOG_USR_CONF 0x00000080

#define MMS_LOG_VM       0x00000100
#define MMS_LOG_DATA     0x00000200
#define MMS_LOG_MIDD     0x00000400
#define MMS_LOG_MIDE     0x00000800

/* Defines used for selecting MMSEASE que servicing debug levels.	*/
#define  MMSREQQUE_HOLD  0x00004000
#define  MMSINDQUE_HOLD  0x00008000

#define MMS_LOG_ERR      0x00010000
#define MMS_LOG_NERR     0x00020000
#define MMS_LOG_PDU      0x00040000
#define MMS_LOG_CONFIG   0x00080000
								
#define MMS_LOG_RT       0x00100000
#define MMS_LOG_RTAA     0x00200000
#define MMS_LOG_AA       0x00400000

#define MMS_LOG_REQ      0x01000000
#define MMS_LOG_RESP     0x02000000
#define MMS_LOG_IND      0x04000000
#define MMS_LOG_CONF     0x08000000

/************************************************************************/
/* These are S_LOG control items for MMS-EASE logging			*/

extern ST_UINT mms_debug_sel;

/************************************************************************/
/* user_debug_sel bit assignments					*/
#define USER_LOG_ERR     0x00000001
#define USER_LOG_CLIENT  0x00000002
#define USER_LOG_SERVER  0x00000004

extern ST_UINT user_debug_sel;	/* global var to control USER logging	*/

/************************************************************************/
/* User selectable function to log MMS actions (MLOG, for instance)	*/

#ifndef MMS_LITE
extern ST_VOID (*m_log_ureq_info_fun) (ST_INT chan, ST_INT op, ST_VOID *info);
extern ST_VOID (*m_log_req_info_fun)  (MMSREQ_PEND *req, ST_VOID *info);
extern ST_VOID (*m_log_resp_info_fun) (MMSREQ_IND *ind, ST_VOID *info);
extern ST_VOID (*m_log_ind_info_fun)  (MMSREQ_IND *ind);
extern ST_VOID (*m_log_conf_info_fun) (MMSREQ_PEND *conf);
extern ST_VOID (*m_log_error_resp_fun) (MMSREQ_IND *ind, ST_INT16 err_class, ST_INT16 code);
#endif

/************************************************************************/
/* Some SLOG Logging control macros					*/

#define MLOG_CLOSE_FILE()            {slogCloseFile (sLogCtrl);}
#define MLOG_DUMP_MEM_LOG()          {slogDumpMem (sLogCtrl);}
#define MLOG_STOP_FILE_LOGGING()     {sLogCtrl->logCtrl &= ~LOG_FILE_EN;}
#define MLOG_START_FILE_LOGGING()    {sLogCtrl->logCtrl |=  LOG_FILE_EN;}

#ifdef __cplusplus
}
#endif

/************************************************************************/
#endif		/* End of MMS_LOG_INCLUDED				*/
/************************************************************************/
