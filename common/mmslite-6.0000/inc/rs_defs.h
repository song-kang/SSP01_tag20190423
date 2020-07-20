/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*              1994 All Rights Reserved                                */
/*                                                                      */
/* MODULE NAME : rs_defs.h                                             	*/
/* PRODUCT(S)  : TP4/RS API internal include file                       */
/*                                                                      */
/* MODULE DESCRIPTION : 						*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 09/13/99  MDE    13     Added SD_CONST modifiers			*/
/* 02/05/99  JRB    12     Del get_rs_event proto. It is now "static".	*/
/* 10/08/98  MDE    11     Migrated to updated SLOG interface		*/
/* 05/27/97  JRB  7.00     MMSEASE 7.0 release.				*/
/* 01/02/96  EJV    10     Added RS_DEFS_INCLUDED			*/
/* 11/12/96  MDE    09     Changed to DEBUG_SISCO			*/
/* 07/22/96  MDE    08     Changes towards A-Unit Data support		*/
/* 03/26/96  MDE    07     Added REDUCED_STACK support			*/
/* 02/16/96  JRB    06     Use thisFileName.				*/
/* 01/02/96  JRB    05     Removed ipcuptyp for __OS2__			*/
/* 12/28/95  KCW    04     Removed ipcuptyp for AIX                     */
/* 12/14/95  DSF    03     Changes for CONN list management		*/
/* 10/06/95  JRB    02     For _DOS16M, add hVipcTp4 global		*/
/*			   so that multiple Stack users allowed.	*/
/* 04/17/95  DSF    01     Created                                      */
/*                                                                      */
/************************************************************************/

#ifndef RS_DEFS_INCLUDED
#define RS_DEFS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "gen_list.h"
#include "slog.h"

/****************************************************************/

#undef local

/****************************************************************/
/****************************************************************/
/* Local ADLC Address control variables */
extern ST_UINT16 rsBaseSrcAdlcAddr;
extern ST_UINT16 rsNumSrcAdlcAddr;

/****************************************************************/
/* Internal structure definitions				*/
/****************************************************************/

typedef struct
  {
  DBL_LNK *l;
  TP4_ADDR localTp4Addr;
  ST_LONG user_bind_id;		/* User-supplied Bind ID */
  } REGINFO;

typedef struct
  {
  DBL_LNK lnk;			/* linkage */
  REGINFO *reginfo;		/* pointer to Bind Control Info */
  ST_LONG user_conn_id;		/* User-supplied Connection ID */
  ST_LONG osi_conn_id;		/* Connection ID returned by OSIAM */
  ST_UINT16 localAddr;		/* ADLC addresses, which define the	*/
  ST_UINT16 remAddr;		/* ADLC connection.			*/
  ST_BOOLEAN connFlag;
  } CONNINFO;

extern CONNINFO *conn_list;	/* pointer to list of conn structs 	*/

/****************************************************************/
extern REGINFO *reg_list;  /* pointer to list of REG structs 	*/

/****************************************************************/

extern SD_CONST ST_UCHAR rs_co_dest;
extern SD_CONST ST_UCHAR rs_co_src;
extern SD_CONST ST_UCHAR rs_co_qos;

extern SD_CONST ST_UCHAR rs_cl_dest;
extern SD_CONST ST_UCHAR rs_cl_src;
extern SD_CONST ST_UCHAR rs_cl_qos;

/****************************************************************/

extern ST_VOID   *co_rs_event;
extern ST_INT co_rs_op;
extern ST_RET co_rs_rslt;

extern ST_VOID   *cl_rs_event;
extern ST_INT cl_rs_op;
extern ST_RET cl_rs_rslt;

/****************************************************************/
/* Log Control macros						*/
/****************************************************************/

extern SD_CONST ST_CHAR *SD_CONST _tp4_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _tp4_flowup_logstr;
extern SD_CONST ST_CHAR *SD_CONST _tp4_flowdown_logstr;

/****************************************************************/
#ifdef DEBUG_SISCO

#define SFLOWUP0(a) {\
                     if ((tp4_debug_sel & TP4_LOG_FLOWUP) && sLogCtrl)\
                       _slog (sLogCtrl,_tp4_flowup_logstr,\
		             thisFileName,__LINE__,a);\
                    }
#define SFLOWUP0C(a) {\
                     if ((tp4_debug_sel & TP4_LOG_FLOWUP) && sLogCtrl)\
                       _slogc (sLogCtrl,a);\
                    }
#define SFLOWUP1(a,b) {\
                       if ((tp4_debug_sel & TP4_LOG_FLOWUP) && sLogCtrl)\
                         _slog (sLogCtrl,_tp4_flowup_logstr,\
		               thisFileName,__LINE__,a,b);\
                      }
#define SFLOWUP1C(a,b) {\
                       if ((tp4_debug_sel & TP4_LOG_FLOWUP) && sLogCtrl)\
                         _slogc (sLogCtrl,a,b);\
                      }
#define SFLOWUP2(a,b,c) {\
                       if ((tp4_debug_sel & TP4_LOG_FLOWUP) && sLogCtrl)\
                         _slog (sLogCtrl,_tp4_flowup_logstr,\
		               thisFileName,__LINE__,a,b,c);\
                      }
#define SFLOWUP2C(a,b,c) {\
                       if ((tp4_debug_sel & TP4_LOG_FLOWUP) && sLogCtrl)\
                         _slogc (sLogCtrl,a,b,c);\
                      }
#define SFLOWUPH(a,b) {\
                       if ((tp4_debug_sel & TP4_LOG_FLOWUP) && sLogCtrl)\
                         _slogHex (sLogCtrl,a,b);\
                      }
#define SFLOWDOWN0(a) {\
                       if ((tp4_debug_sel & TP4_LOG_FLOWDOWN) && sLogCtrl)\
                         _slog (sLogCtrl,_tp4_flowup_logstr,\
		               thisFileName,__LINE__,a);\
                      }
#define SFLOWDOWN0C(a) {\
                       if ((tp4_debug_sel & TP4_LOG_FLOWDOWN) && sLogCtrl)\
                         _slogc (sLogCtrl,a);\
                      }
#define SFLOWDOWN1(a,b) {\
                         if ((tp4_debug_sel & TP4_LOG_FLOWDOWN) && sLogCtrl)\
                           _slog (sLogCtrl,_tp4_flowdown_logstr,\
	   	                 thisFileName,__LINE__,a,b);\
                        }
#define SFLOWDOWN1C(a,b) {\
                         if ((tp4_debug_sel & TP4_LOG_FLOWDOWN) && sLogCtrl)\
                           _slogc (sLogCtrl,a,b);\
                        }
#define SFLOWDOWN2(a,b,c) {\
                         if ((tp4_debug_sel & TP4_LOG_FLOWDOWN) && sLogCtrl)\
                           _slog (sLogCtrl,_tp4_flowdown_logstr,\
	   	                 thisFileName,__LINE__,a,b,c);\
                        }
#define SFLOWDOWN2C(a,b,c) {\
                         if ((tp4_debug_sel & TP4_LOG_FLOWDOWN) && sLogCtrl)\
                           _slogc (sLogCtrl,a,b,c);\
                        }
#define SFLOWDOWNH(a,b) {\
                         if ((tp4_debug_sel & TP4_LOG_FLOWDOWN) && sLogCtrl)\
                         _slogHex (sLogCtrl,a,b);\
                        }
#define SERR0(a) {\
                  if ((tp4_debug_sel & TP4_LOG_ERR) && sLogCtrl)\
                    _slog (sLogCtrl,_tp4_err_logstr,\
	                  thisFileName,__LINE__,a);\
                  }
#define SERR0C(a) {\
                  if ((tp4_debug_sel & TP4_LOG_ERR) && sLogCtrl)\
                    _slogc (sLogCtrl,a);\
                  }
#define SERR1(a,b) {\
                    if ((tp4_debug_sel & TP4_LOG_ERR) && sLogCtrl)\
                      _slog (sLogCtrl,_tp4_err_logstr,\
	                    thisFileName,__LINE__,a,b);\
                    }
#define SERR1C(a,b) {\
                    if ((tp4_debug_sel & TP4_LOG_ERR) && sLogCtrl)\
                      _slogc (sLogCtrl,a,b);\
                    }
#define SERR2(a,b,c) {\
                    if ((tp4_debug_sel & TP4_LOG_ERR) && sLogCtrl)\
                      _slog (sLogCtrl,_tp4_err_logstr,\
	                    thisFileName,__LINE__,a,b,c);\
                    }
#define SERR2C(a,b,c) {\
                    if ((tp4_debug_sel & TP4_LOG_ERR) && sLogCtrl)\
                      _slogc (sLogCtrl,a,b,c);\
                    }
#define SERRH(a,b) {\
                    if ((tp4_debug_sel & TP4_LOG_ERR) && sLogCtrl)\
                      _slogHex (sLogCtrl,a,b);\
                    }

#else  /* Not DEBUG_SISCO */

#define SFLOWUP0(a)
#define SFLOWUP0C(a)
#define SFLOWUP1(a,b)
#define SFLOWUP1C(a,b)
#define SFLOWUP2(a,b,c)
#define SFLOWUP2C(a,b,c)
#define SFLOWUPH(a,b)
#define SFLOWDOWN0(a)
#define SFLOWDOWN0C(a)
#define SFLOWDOWN1(a,b)
#define SFLOWDOWN1C(a,b)
#define SFLOWDOWN2(a,b,c)
#define SFLOWDOWN2C(a,b,c)
#define SFLOWDOWNH(a,b)
#define SERR0(a)
#define SERR0C(a)
#define SERR1(a,b)
#define SERR1C(a,b)
#define SERR2(a,b,c)
#define SERR2C(a,b,c)
#define SERRH(a,b)
#endif


#ifdef __cplusplus
}
#endif

#endif	/* RS_DEFS_INCLUDED */

