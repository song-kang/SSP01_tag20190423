/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2007 - 2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : bindsels.h						*/
/* PRODUCT(S)  : MMS-EASE-142-xxx, MMS-LITE				*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Macros & functions for bind / unbind operations.		*/
/*	Used by applications and by SNAP-Lite.				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    04     Add bind_ctrl_find. Add arg to add_bind_sels.*/
/*			   Def different BIND_CONTROL for User & SNAP.	*/
/*			   Make MAX_SELS_BUF_LEN same for all apps.	*/
/* 01/05/09  EJV    03     find_user_bind_id,add_bind_sels,del_bind_sels:*/
/*			     chg user_bind_id to (ST_VOID *).		*/
/*			   Added find_user_bind_id_u64.			*/
/*			   BIND_CONTROL: added user_bind_id_u64.	*/
/* 01/29/08  EJV    02     Added 1 to MAX_SELS_BUF_LEN for even buf len */
/*			   Added BIND_STATE_* and state in BIND_CONTROL.*/
/* 05/17/07  EJV    01     Created					*/
/************************************************************************/
#ifndef BINDSELS_INCLUDED
#define BINDSELS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "acse2usr.h"		/* for MAX_PSEL_LEN & MAX_SSEL_LEN	*/
#include "tp4api.h"		/* for MAX_TSEL_LEN			*/

/************************************************************************/
/* Defines for operations (bind / unbind) on seclectors buffer.		*/
/* SELs buffer layout: TSELlen, TSEL, SSELlen, SSEL, PSELlen, PSEL	*/ 
/************************************************************************/
#define MAX_USER_BINDS		64	/* default value if not config	*/

/* Selector sizes different for different applications, but		*/
/* MAX_SELS_BUF_LEN, used for IPC buffer, must be same for all apps.	*/
/* TSEL, SSEL, PSEL in the same buffer (first byte is len)		*/
/* 1+32 + 1+16 + 1+16 + 1 = 68 bytes (1 byte for padding to even size)	*/
#define MAX_SELS_BUF_LEN	68

/* Make sure that selector sizes never exceed the IPC buffer.		*/
#if (MAX_SELS_BUF_LEN < 1+MAX_TSEL_LEN + 1+MAX_SSEL_LEN + 1+MAX_PSEL_LEN +1)
  #error Defined MAX_TSEL_LEN, MAX_SSEL_LEN, or MAX_PSEL_LEN too big for SNAP IPC implementation.
#endif

/* Since these error codes wiil be used by Presentation layer make	*/
/* them unique when added E_COPP base (see copp_usr.h)			*/
#define	BIND_ERR_TOO_MANY_BINDS	3	/* Too many binds		*/
#define	BIND_ERR_ALREADY_BOUND	4	/* Selectors set already bound	*/
#define	BIND_ERR_USER_BIND_ID	5	/* User bind id already used	*/
#define	BIND_ERR_NOT_BOUND	6	/* User not bound		*/

/* BIND state */
#define	BIND_STATE_NONE			0	/* not bound			*/
#define	BIND_STATE_INITIALIZED		1	/* user app called cosp_bind	*/
#define	BIND_STATE_ACTIVE		2	/* SNAP-Lite send ACK for BIND	*/
#define	BIND_STATE_DEACTIVATING		3	/* SNAP-Lite send ACK for UNBIND*/

/* BIND_CONTROL structure is almost identical for User application and	*/
/* for SNAP-Lite except the bind_id is ST_UINT64 for SNAP-Lite and 	*/
/* (ST_VOID *) for user application.					*/
/* Note: ST_UINT64 is used in SNAP-Lite and in IPC messages to allow	*/
/*       64-bit application to talk to 32-bit applications.		*/
#if defined(SNAP_LITE_PROCESS)
typedef struct BIND_CONTROL_tag
  {
  ST_UINT    used;			/* 0 - unused, 1 - used					*/
  ST_INT     state;			/* one of the BIND_STATE_*				*/
  ST_UINT64  user_bind_id_u64;		/* user bind id, copied to ST_UINT64 (used in IPC)	*/
  ST_UINT    sels_len;			/* actual length of all selectors			*/
  ST_UCHAR   sels [MAX_SELS_BUF_LEN];	/* combined TSEL, SSEL, PSEL with respective lengths	*/
  } BIND_CONTROL;

#else	/* !defined(SNAP_LITE_PROCESS)	*/
typedef struct BIND_CONTROL_tag
  {
  ST_UINT    used;			/* 0 - unused, 1 - used					*/
  ST_INT     state;			/* one of the BIND_STATE_*				*/
  ST_VOID   *user_bind_id;		/* user bind id, unique for each bind (pointer)		*/
  ST_UINT    sels_len;			/* actual length of all selectors			*/
  ST_UCHAR   sels [MAX_SELS_BUF_LEN];	/* combined TSEL, SSEL, PSEL with respective lengths	*/
  } BIND_CONTROL;
#endif	/* !defined(SNAP_LITE_PROCESS)	*/


/* This is application ONLY bind_control_arr, SNAP-Lite keeps table of binds	*/
/* for each user in its structures 						*/
extern BIND_CONTROL *bind_control_arr;	/* global table of binds */
extern ST_UINT       bind_control_arr_cnt;
ST_RET set_arr_cnt_bind_sels (ST_UINT max_binds);



ST_RET    init_bind_sels (BIND_CONTROL **bind_ctrl, ST_UINT max_binds);
ST_RET    free_bind_sels (BIND_CONTROL  *bind_ctrl);
#if defined(SNAP_LITE_PROCESS)
ST_RET     add_bind_sels (BIND_CONTROL *bind_ctrl, ST_UINT max_binds,  ST_UCHAR *sels, ST_UINT sels_len, ST_UINT64 user_bind_id_u64);
ST_RET     del_bind_sels (BIND_CONTROL  *bind_ctrl, ST_UINT max_binds, ST_UINT64 user_bind_id_u64);
#else  /* !defined(SNAP_LITE_PROCESS) */
ST_RET     add_bind_sels (BIND_CONTROL *bind_ctrl, ST_UINT max_binds,  ST_UCHAR *sels, ST_UINT sels_len, ST_VOID  *user_bind_id,
                          BIND_CONTROL **p_bind_ctrl);	/* out: new entry*/
ST_RET     del_bind_sels (BIND_CONTROL  *bind_ctrl, ST_UINT max_binds, ST_VOID *user_bind_id);
#endif /* !defined(SNAP_LITE_PROCESS) */

ST_UINT     combine_sels (ST_UCHAR *tsel, ST_UCHAR *ssel, ST_UCHAR *psel, ST_UCHAR *sels);
ST_UINT   make_bind_sels (ST_UCHAR *tsel, ST_UINT tsel_len, 
                          ST_UCHAR *ssel, ST_UINT ssel_len, 
                          ST_UCHAR *psel, ST_UINT psel_len, ST_UCHAR *sels);

BIND_CONTROL *find_bind_tsel      (BIND_CONTROL *bind_ctrl, ST_UINT max_binds, ST_UCHAR *tsel);
BIND_CONTROL *find_bind_tsel_ssel (BIND_CONTROL *bind_ctrl, ST_UINT max_binds, ST_UCHAR *tsel, ST_UCHAR *ssel);
BIND_CONTROL *find_bind_sels      (BIND_CONTROL *bind_ctrl, ST_UINT max_binds, ST_UCHAR *sels, ST_UINT sels_len);
BIND_CONTROL *find_user_bind_id     (BIND_CONTROL *bind_ctrl, ST_UINT max_binds,  ST_VOID *user_bind_id);
BIND_CONTROL *find_user_bind_id_u64 (BIND_CONTROL *bind_ctrl, ST_UINT max_binds, ST_UINT64 user_bind_id_i64);

BIND_CONTROL *bind_ctrl_find (ST_VOID *copp_bind_id);

ST_VOID    log_sels      (ST_UCHAR *sels);
ST_VOID    log_ssel_tsel (ST_UCHAR *ssel, ST_UCHAR *tsel);

/* macros to access TSEL, SSEL, PSEL in the SELS buffer (all selectors	*/
/* are preceeded with selector length). For empty selector the length	*/
/* is set to 0.								*/
#define TSEL_LEN(s)         (ST_UINT) ( s[0])
#define TSEL_LEN_PTR(s)	              (&s[0])
#define TSEL_PTR(s)	              (&s[1])

#define SSEL_LEN(s)         (ST_UINT) (s[TSEL_LEN(s) + 1])
#define SSEL_LEN_PTR(s)               (TSEL_PTR(s) + TSEL_LEN(s))
#define SSEL_PTR(s)	              (TSEL_PTR(s) + TSEL_LEN(s) + 1)

#define PSEL_LEN(s)         (ST_UINT) (s[TSEL_LEN(s) + 1 + SSEL_LEN(s) + 1])
#define PSEL_LEN_PTR(s)	              (SSEL_PTR(s) + SSEL_LEN(s))
#define PSEL_PTR(s)	              (SSEL_PTR(s) + SSEL_LEN(s) + 1)


#ifdef __cplusplus
}
#endif

#endif /* !BINDSELS_INCLUDED */
