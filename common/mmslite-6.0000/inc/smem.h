/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*                 1999, All Rights Reserved             		*/
/*									*/
/* MODULE NAME : smem.h							*/
/* PRODUCT(S)  : Utilities						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/03/12  JRB           Chg args to size_t like system functs.	*/
/* 04/14/03  JRB     10    Eliminate compiler warnings.			*/
/* 10/09/01  JRB     09    Add non-debug log macros.			*/
/* 10/02/01  JRB     08    Del DBL_LNK from SMEM_CONTEXT (never used).	*/
/*			   Add forward declarations to reduce warnings.	*/
/* 09/21/01  JRB     07    Add args to smem_add_pool.			*/
/*			   Chg args to u_smem_*.			*/
/*			   Del unused prototypes.			*/
/*			   Add u_smem_get_buf_detail.			*/
/*			   Pass old & new size to smem_realloc.		*/
/*			   Add m_add_pool proto.			*/
/*			   Del #ifdef ACSE && LEANT.			*/
/*			   Del smemInitialized,smemDefCtxt (not needed).*/
/*			   Add more SLOG macros.			*/
/*			   Chg poolName, contextName to (ST_CHAR *).	*/
/* 05/21/01  MDE     06    Mode SMEM context management work		*/
/* 03/19/01  JEB     05    Moved all SMEM context defs to this file.	*/
/* 03/10/01  GLB     04	   Added u_smem_get_pool_params                 */
/* 03/10/01  GLB     03	   Added smemcfgx for parsing memory cfg. file  */
/* 02/26/01  MDE     02	   Added clone control to SMEM_POOL_CTRL	*/
/* 12/09/99  MDE     01	   New						*/
/************************************************************************/
/************************************************************************/

#ifndef SMEM_INCLUDED
#define SMEM_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "gen_list.h"

/************************************************************************/

#define SMEM_MAX_RANGE_TRACK	10

typedef struct _smem_range_track
  {
  ST_UINT lowLimit;
  ST_UINT highLimit;
  ST_LONG currNum;
  ST_LONG maxNum;
  } SMEM_RANGE_TRACK;

typedef struct _smem_rt_ctrl
  {
  ST_INT numRanges;
  SMEM_RANGE_TRACK rt[SMEM_MAX_RANGE_TRACK];
  } SMEM_RT_CTRL;

/************************************************************************/
struct _smem_pool_ctrl;			/* forward declaration		*/
struct _smem_context;			/* forward declaration		*/

typedef struct _smem_pool_ctrl
  {
  struct _smem_pool_ctrl *next;
  ST_CHAR *poolName;
  size_t bufSize;
  ST_UINT8 numBuf;
  ST_UINT8 nextAvailBuf;
  ST_UINT8 *availIndexStack;	/* Index table, size [numBuf] */
  ST_CHAR *firstBuf;
  ST_CHAR *lastBuf;	
  struct _smem_context *smemContext;

  ST_BOOLEAN autoClone;
  ST_INT maxClones;
  ST_INT cloneCount;

#ifdef DEBUG_SISCO
  ST_UINT8 maxNumUsed;
  size_t *usedSize;		/* usedSize [numBuf] */
#endif

  ST_VOID *usr;			/* SMEM user can use this ...	*/
  } SMEM_POOL_CTRL;


typedef struct _smem_context
  {
  ST_CHAR *contextName;		
  ST_INT   usrId;  	/* User */
  ST_VOID *usr;		/* User */

/* Must be arranged from smallest to largest buffer size */
  SMEM_POOL_CTRL *smemPoolCtrlList;

/* Optional range tracking */
  SMEM_RT_CTRL *smemRangeTrack;
  } SMEM_CONTEXT;

/************************************************************************/
/************************************************************************/

ST_RET smemcfgx (ST_CHAR *xml_filename);

ST_VOID  u_smem_get_pool_params ( SMEM_CONTEXT *contextName,
                                  ST_UINT8 *numBuf, ST_UINT *bufSize,
			          ST_BOOLEAN *autoClone,
			          ST_INT *maxClones,
			          ST_CHAR **poolName);

SMEM_POOL_CTRL *u_smem_need_buffers (SMEM_CONTEXT *smemContext, 
				     ST_UINT8 numBuf, size_t bufSize);
ST_CHAR *u_smem_get_buf_detail (ST_VOID *bufptr, size_t bufSize);

SMEM_POOL_CTRL *smem_add_pool (SMEM_CONTEXT *smemContext, 
			       ST_UINT8 numBuf, size_t bufSize,
			       ST_BOOLEAN autoClone,
			       ST_INT maxClones,
			       ST_CHAR *poolName);

ST_VOID *smem_malloc (SMEM_CONTEXT *smemContext, size_t size);
ST_VOID *smem_calloc (SMEM_CONTEXT *smemContext, size_t num, size_t size);
ST_VOID *smem_realloc (SMEM_CONTEXT *smemContext, ST_VOID *oldptr,
                       size_t oldsize, size_t newsize);
ST_VOID smem_free (SMEM_CONTEXT *smemContext, ST_VOID *ptr);
ST_VOID smem_log_state (SMEM_CONTEXT *smemContext);

ST_VOID smem_range_check (SMEM_CONTEXT *smemContext, 
			  size_t size, ST_BOOLEAN add);


/************************************************************************/
/* mem_smem.c function prototypes					*/
/************************************************************************/
SMEM_POOL_CTRL *m_add_pool (SMEM_CONTEXT *smemContext, 
			       ST_UINT8 numBuf, ST_UINT bufSize,
			       ST_BOOLEAN autoClone,
			       ST_INT maxClones,
			       ST_CHAR *poolName);

/************************************************************************/
/************************************************************************/
/************************************************************************/

#define SMEM_LOG_ERR		0x0001 
#define SMEM_LOG_NERR		0x0002 
#define SMEM_LOG_FLOW		0x0004  
#define SMEM_LOG_DEBUG		0x0008  

extern ST_UINT smem_debug_sel;

/************************************************************************/
/************************************************************************/

extern SD_CONST ST_CHAR *SD_CONST _smem_log_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _smem_log_flow_logstr;
extern SD_CONST ST_CHAR *SD_CONST _smem_log_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST _smem_log_debug_logstr;
extern SD_CONST ST_CHAR *SD_CONST _smem_log_dump_logstr;

#ifdef DEBUG_SISCO

#define SMEMLOG_ERR0(a) {\
                       _slog (sLogCtrl,_smem_log_err_logstr,\
		             thisFileName,__LINE__,a);\
                    }
#define SMEMLOG_ERR1(a,b) {\
                       _slog (sLogCtrl,_smem_log_err_logstr,\
		             thisFileName,__LINE__,a,b);\
                    }

#define SMEMLOG_ERR2(a,b,c) {\
                       _slog (sLogCtrl,_smem_log_err_logstr,\
		             thisFileName,__LINE__,a,b,c);\
                    }

#define SMEMLOG_CERR0(a) {\
                       _slogc (sLogCtrl,a);\
                    }
#define SMEMLOG_CERR1(a,b) {\
                       _slogc (sLogCtrl,a,b);\
                    }

#define SMEMLOG_CERR2(a,b,c) {\
                       _slogc (sLogCtrl,a,b,c);\
                    }

#define SMEMLOG_NERR0(a) {\
                     if (smem_debug_sel & SMEM_LOG_NERR)\
                       _slog (sLogCtrl,_smem_log_nerr_logstr,\
		             thisFileName,__LINE__,a);\
                    }
#define SMEMLOG_NERR1(a,b) {\
                     if (smem_debug_sel & SMEM_LOG_NERR)\
                       _slog (sLogCtrl,_smem_log_nerr_logstr,\
		             thisFileName,__LINE__,a,b);\
                    }

#define SMEMLOG_FLOW1(a,b) {\
                     if (smem_debug_sel & SMEM_LOG_FLOW)\
                       _slog (sLogCtrl,_smem_log_flow_logstr,\
		             thisFileName,__LINE__,a,b);\
                    }
#define SMEMLOG_CFLOW1(a,b) {\
                     if (smem_debug_sel & SMEM_LOG_FLOW)\
                       _slogc (sLogCtrl,a,b);\
                    }
#define SMEMLOG_CFLOW2(a,b,c) {\
                     if (smem_debug_sel & SMEM_LOG_FLOW)\
                       _slogc (sLogCtrl,a,b,c);\
                    }

#define SMEMLOG_DUMP0(a) {\
                       _slog (sLogCtrl,_smem_log_dump_logstr,\
		             thisFileName,__LINE__,a);\
                    }
#define SMEMLOG_DUMP1(a,b) {\
                       _slog (sLogCtrl,_smem_log_dump_logstr,\
		             thisFileName,__LINE__,a,b);\
                    }

#define SMEMLOG_CDUMP0(a) {\
                       _slogc (sLogCtrl,a);\
                    }

#define SMEMLOG_CDUMP1(a,b) {\
                       _slogc (sLogCtrl,a,b);\
                    }

#define SMEMLOG_CDUMP2(a,b,c) {\
                       _slogc (sLogCtrl,a,b,c);\
                    }

#define SMEMLOG_CDUMP3(a,b,c,d) {\
                       _slogc (sLogCtrl,a,b,c,d);\
                    }

#define SMEMLOG_CDUMP4(a,b,c,d,e) {\
                       _slogc (sLogCtrl,a,b,c,d,e);\
                    }

#define SMEMLOG_CDUMP5(a,b,c,d,e,f) {\
                       _slogc (sLogCtrl,a,b,c,d,e,f);\
                    }

#define SMEMLOG_DEBUG0(a) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slog (sLogCtrl,_smem_log_debug_logstr,\
		             thisFileName,__LINE__,a);\
                    }

#define SMEMLOG_DEBUG1(a,b) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slog (sLogCtrl,_smem_log_debug_logstr,\
		             thisFileName,__LINE__,a,b);\
                    }

#define SMEMLOG_DEBUG2(a,b,c) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slog (sLogCtrl,_smem_log_debug_logstr,\
		             thisFileName,__LINE__,a,b,c);\
                    }
#define SMEMLOG_DEBUG3(a,b,c,d) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slog (sLogCtrl,_smem_log_debug_logstr,\
		             thisFileName,__LINE__,a,b,c,d);\
                    }
#define SMEMLOG_DEBUG4(a,b,c,d,e) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slog (sLogCtrl,_smem_log_debug_logstr,\
		             thisFileName,__LINE__,a,b,c,d,e);\
                    }

#define SMEMLOG_CDEBUG0(a) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slogc (sLogCtrl,a);\
                    }

#define SMEMLOG_CDEBUG1(a,b) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slogc (sLogCtrl,a,b);\
                    }

#define SMEMLOG_CDEBUG2(a,b,c) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slogc (sLogCtrl,a,b,c);\
                    }
#define SMEMLOG_CDEBUG3(a,b,c,d) {\
                     if (smem_debug_sel & SMEM_LOG_DEBUG)\
                       _slogc (sLogCtrl,a,b,c,d);\
                    }

#else /* #ifdef DEBUG_SISCO */

#define SMEMLOG_ERR0(a)
#define SMEMLOG_ERR1(a,b)
#define SMEMLOG_ERR2(a,b,c)
#define SMEMLOG_CERR0(a)
#define SMEMLOG_CERR1(a,b)
#define SMEMLOG_CERR2(a,b,c)
#define SMEMLOG_NERR0(a)
#define SMEMLOG_NERR1(a,b)
#define SMEMLOG_FLOW1(a,b)
#define SMEMLOG_CFLOW1(a,b)
#define SMEMLOG_CFLOW2(a,b,c)
#define SMEMLOG_DUMP0(a)
#define SMEMLOG_DUMP1(a,b)
#define SMEMLOG_CDUMP0(a,b)
#define SMEMLOG_CDUMP1(a,b)
#define SMEMLOG_CDUMP2(a,b,c)
#define SMEMLOG_CDUMP3(a,b,c,d)
#define SMEMLOG_CDUMP4(a,b,c,d,e)
#define SMEMLOG_CDUMP5(a,b,c,d,e,f)
#define SMEMLOG_DEBUG0(a,b,c)
#define SMEMLOG_DEBUG2(a,b,c)
#define SMEMLOG_DEBUG3(a,b,c,d)
#define SMEMLOG_DEBUG4(a,b,c,d,e)
#define SMEMLOG_CDEBUG0(a)
#define SMEMLOG_CDEBUG1(a,b)
#define SMEMLOG_CDEBUG2(a,b,c)
#define SMEMLOG_CDEBUG3(a,b,c,d)

#endif /* #ifdef DEBUG_SISCO */

/************************************************************************/
/* Here we define a context control table and the associated indices	*/

#define	M_SMEM_MAX_CONTEXT	        30  /* number of specific context array elements  	       */
                                            /* + extra context array elements for user applications    */
                                            /* = MAX context array size                                */
extern SMEM_CONTEXT m_smem_ctxt[M_SMEM_MAX_CONTEXT]; 

/* All users of SMEM that want a a context are to specify it here as an	*/
/* index into the m_smem_ctxt table.					*/

/* Valid context indices */
#define MSMEM_GEN_IDX			0	/* chk_* macros use this ctxt*/

/* MMS-EASE Lite indices */
#define MSMEM_DEC_OS_INFO_IDX		1
#define MSMEM_ENC_OS_INFO_IDX		2
#define MSMEM_WR_DATA_DEC_BUF_IDX       3
#define MSMEM_ASN1_DATA_ENC_IDX	        4
#define MSMEM_PDU_ENC_IDX	        5
#define MSMEM_COM_EVENT_IDX	        6
#define MSMEM_RXPDU_IDX		        7
#define MSMEM_NETINFO_IDX               8
#define MSMEM_DYN_RT_IDX		9
#define MSMEM_AA_ENCODE_IDX 	       10
#define MSMEM_REQ_CTRL_IDX	       11
#define MSMEM_IND_CTRL_IDX             12
#define MSMEM_MVLU_VA_IDX	       13
#define MSMEM_MVLU_VA_CTRL_IDX	       14
#define MSMEM_MVLU_VA_DATA_IDX	       15
#define MSMEM_MVLU_GNL_IDX	       16
#define MSMEM_MVLU_AA_IDX 	       17

/* For STACK (ACSE and LEANT) */ 
#define MSMEM_ACSE_CONN_IDX            18 
#define MSMEM_ACSE_DATA_IDX            19 
#define MSMEM_COSP_CN_IDX              20 
#define MSMEM_N_UNITDATA_IDX           21 
#define MSMEM_SOCK_INFO_IDX            22 
#define MSMEM_SPDU_TX_IDX              23 
#define MSMEM_STARTUP_IDX              24 
#define MSMEM_TPKT_IDX                 25 

/* The following are valid Contexts                        */
#define MSMEM_GEN	         &m_smem_ctxt[MSMEM_GEN_IDX]

/* MMS-EASE Lite indices */
#define MSMEM_DEC_OS_INFO	 &m_smem_ctxt[MSMEM_DEC_OS_INFO_IDX]  
#define MSMEM_ENC_OS_INFO	 &m_smem_ctxt[MSMEM_ENC_OS_INFO_IDX]  
#define MSMEM_WR_DATA_DEC_BUF	 &m_smem_ctxt[MSMEM_WR_DATA_DEC_BUF_IDX]
#define MSMEM_ASN1_DATA_ENC	 &m_smem_ctxt[MSMEM_ASN1_DATA_ENC_IDX]
#define MSMEM_PDU_ENC		 &m_smem_ctxt[MSMEM_PDU_ENC_IDX]
#define MSMEM_COM_EVENT		 &m_smem_ctxt[MSMEM_COM_EVENT_IDX]
#define MSMEM_RXPDU		 &m_smem_ctxt[MSMEM_RXPDU_IDX]
#define MSMEM_NETINFO		 &m_smem_ctxt[MSMEM_NETINFO_IDX]     
#define MSMEM_REQ_CTRL   	 &m_smem_ctxt[MSMEM_REQ_CTRL_IDX]
#define MSMEM_IND_CTRL		 &m_smem_ctxt[MSMEM_IND_CTRL_IDX]      
#define MSMEM_MVLU_VA  		 &m_smem_ctxt[MSMEM_MVLU_VA_IDX]
#define MSMEM_MVLU_VA_CTRL	 &m_smem_ctxt[MSMEM_MVLU_VA_CTRL_IDX]
#define MSMEM_MVLU_VA_DATA  	 &m_smem_ctxt[MSMEM_MVLU_VA_DATA_IDX]
#define MSMEM_MVLU_AA		 &m_smem_ctxt[MSMEM_MVLU_AA_IDX]
#define MSMEM_MVLU_GNL		 &m_smem_ctxt[MSMEM_MVLU_GNL_IDX]
#define MSMEM_DYN_RT		 &m_smem_ctxt[MSMEM_DYN_RT_IDX]	     
#define MSMEM_AA_ENCODE		 &m_smem_ctxt[MSMEM_AA_ENCODE_IDX]    

/* For STACK (ACSE and LEANT) */ 
#define MSMEM_ACSE_CONN          &m_smem_ctxt[MSMEM_ACSE_CONN_IDX] 
#define MSMEM_ACSE_DATA          &m_smem_ctxt[MSMEM_ACSE_DATA_IDX] 
#define MSMEM_COSP_CN            &m_smem_ctxt[MSMEM_COSP_CN_IDX] 
#define MSMEM_N_UNITDATA         &m_smem_ctxt[MSMEM_N_UNITDATA_IDX] 
#define MSMEM_SOCK_INFO          &m_smem_ctxt[MSMEM_SOCK_INFO_IDX] 
#define MSMEM_SPDU_TX            &m_smem_ctxt[MSMEM_SPDU_TX_IDX] 
#define MSMEM_STARTUP            &m_smem_ctxt[MSMEM_STARTUP_IDX] 
#define MSMEM_TPKT               &m_smem_ctxt[MSMEM_TPKT_IDX] 

#ifdef SMEMCFGX
/************************************************************************/
/* This list is used to log error messages when an invalid context name */
/* is entered.  The error message for an invalid context name will list */
/* all valid context names, as listed below.                            */
/* Note:  these names should match those listed above and those in      */
/*       'smemcfg.xml'                                                  */

typedef struct 
  {
  ST_INT idx;
  ST_CHAR *name;
  } SMEM_CTXT_NAME;

static SMEM_CTXT_NAME smemCtxtNames[] = 
  {
  {MSMEM_GEN_IDX,		"MSMEM_GEN"},
  {MSMEM_DEC_OS_INFO_IDX,	"MSMEM_DEC_OS_INFO"},
  {MSMEM_ENC_OS_INFO_IDX,	"MSMEM_ENC_OS_INFO"},
  {MSMEM_WR_DATA_DEC_BUF_IDX,	"MSMEM_WR_DATA_DEC_BUF"},
  {MSMEM_ASN1_DATA_ENC_IDX,	"MSMEM_ASN1_DATA_ENC"},
  {MSMEM_PDU_ENC_IDX,		"MSMEM_PDU_ENC"},
  {MSMEM_COM_EVENT_IDX,		"MSMEM_COM_EVENT"},
  {MSMEM_RXPDU_IDX,		"MSMEM_RXPDU"},
  {MSMEM_NETINFO_IDX,		"MSMEM_NETINFO"},
  {MSMEM_REQ_CTRL_IDX,		"MSMEM_REQ_CTRL"},
  {MSMEM_IND_CTRL_IDX,		"MSMEM_IND_CTRL"},
  {MSMEM_MVLU_VA_IDX,		"MSMEM_MVLU_VA"},
  {MSMEM_MVLU_VA_CTRL_IDX,	"MSMEM_MVLU_VA_CTRL"},
  {MSMEM_MVLU_VA_DATA_IDX, 	"MSMEM_MVLU_VA_DATA"},
  {MSMEM_MVLU_AA_IDX,		"MSMEM_MVLU_AA"},
  {MSMEM_MVLU_GNL_IDX,		"MSMEM_MVLU_GNL"},
  {MSMEM_DYN_RT_IDX,		"MSMEM_DYN_RT"},
  {MSMEM_AA_ENCODE_IDX,		"MSMEM_AA_ENCODE"},
  {MSMEM_ACSE_CONN_IDX,		"MSMEM_ACSE_CONN"},
  {MSMEM_ACSE_DATA_IDX,		"MSMEM_ACSE_DATA"},
  {MSMEM_COSP_CN_IDX,		"MSMEM_COSP_CN"},
  {MSMEM_N_UNITDATA_IDX,	"MSMEM_N_UNITDATA"},
  {MSMEM_SOCK_INFO_IDX,		"MSMEM_SOCK_INFO"},
  {MSMEM_SPDU_TX_IDX,		"MSMEM_SPDU_TX"},
  {MSMEM_STARTUP_IDX,		"MSMEM_STARTUP"},
  {MSMEM_TPKT_IDX,		"MSMEM_TPKT"}
  };

static ST_INT smem_num_ctxt_names = sizeof(smemCtxtNames)/sizeof(SMEM_CTXT_NAME); 

#endif

/************************************************************************/
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* smem.h already included */

