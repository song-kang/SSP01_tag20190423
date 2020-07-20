/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 2011, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_acse.h    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/23/11  EJV           Added mvla_initiate_req_exx with locMaceCert.*/
/*			   Added macro for mvla_initiate_req_ex.	*/
/* 03/30/11  JRB	   Added osicfgx_unconfigure.			*/
/* 11/17/09  JRB     27    mvla_initiate_req_ex: add locAr arg.		*/
/*			   Del mvl_initiate_req, mvla_initiate_req.	*/
/*			   MVL_CFG_INFO: del calling_used,called_used;	*/
/*			     del #ifdef around use_bind_ctrl.		*/
/*			   NET_BIND_CTRL changes:			*/
/*			     del num_calling,num_called,de,clpp_bind_id;*/
/*			     del copp_bind_id_*, use only copp_bind_id;	*/
/*			     add cmd_done, cmd_rslt.			*/
/*			   Del MAP30_ACSE, MMSEASE_MOSI stuff.		*/
/*			   Del unused _mvla_initiate_locDe,ma_clbind_ctrl.*/
/*			   Del mvl_match*, mvl_cmp* (use dib_* directly)*/
/*			   Del mvla_set_init_ar (no longer needed).	*/
/* 05/10/05  EJV     26    Moved up osicfgx proto. 			*/
/* 04/06/05  EJV     25    Added MMSEASE_MOSI.				*/
/* 03/28/05  JRB     24    Del suicacse header.				*/
/* 08/11/04  JRB     23    Del global funct ptr u_mvl_connect_ind_fun.	*/
/*			   Fix mvl_abort_req_ex return (ST_RET).	*/
/* 07/08/04  JRB     22    Del unused global var mvl_local_cl_ar_name.	*/
/* 07/23/03  MDE     21    Removed ACSE_AUTH_ENABLED			*/
/* 06/16/03  EJV     20    Removed m_match.h, s_match.h includes	*/
/*			   Chg M_MATCH_CTRL to DIB_MATCH_CTRL.		*/
/* 06/10/03  MDE     19    Added address matching			*/
/* 10/11/02  ASK     18    Added mvla_initiate_req_ex and 		*/
/*			   u_mvl_connect_*_ex				*/
/* 05/13/02  MDE     17    Added structure element comments		*/
/* 05/03/02  MDE     16    Added mvl_add_bind_ctrl, associated changes	*/
/* 02/25/02  MDE     15    More changes to MVL_CFG_INFO			*/
/* 02/20/02  MDE     14    Changes to new MVL_CFG_INFO, for mult. bind	*/
/* 02/07/02  JRB     13    Define MVL_CFG_INFO & pass to mvl_start_acse.*/
/*			   Move osicfgx proto here, add MVL_CFG_INFO arg*/
/*			   Del global var mvl_local_ar_name.		*/
/* 01/29/02  JRB     12    Replace mvl_init_cl_netinfo funct with	*/
/*			   mvl_init_audt_addr.				*/
/* 11/28/01  MDE     11    mvl_set_num_connections,mvl_set_max_msg_size */
/* 01/05/01  MDE     10    Changes for use over Marben			*/
/* 06/26/00  JRB     09    Del "#define MVL_ACSE". Not used.		*/
/*			   Move "#define COACSE" to mvl_defs.h (avoids	*/
/*			     mvl_defs.h dependence on mvl_acse.h).	*/
/* 03/09/00  MDE     08    Added _mvl_init_conn_ctrl			*/
/* 07/30/99  JRB     07    Chg ma_clbind_ctrl from static to global.	*/
/* 04/13/99  MDE     06    Added '_mvla_initiate_locDe'			*/
/* 03/18/99  MDE     05    Made connection limits runtime settable	*/
/* 11/16/98  MDE     04    Removed mvl_release, mvla_release (obsolete)	*/
/* 07/16/98  MDE     03    Addded 'mvl_dib_entry_to_app_ref'		*/
/* 06/19/98  JRB     02    Add tp_type, etc. to NET_BIND_CTRL.		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#ifndef MVL_ACSE_INCLUDED
#define MVL_ACSE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "acse2usr.h"
#include "mvl_defs.h"
#include "lean_a.h"	/* for DIB_ENTRY	*/

/************************************************************************/
/* Some error return codes */
#define REQ_INCOMPLETE  	0x100
#define LOCAL_NAME_NOT_FOUND 	0x101
#define REMOTE_NAME_NOT_FOUND 	0x102

#define MVLE_LOCAL_NAME 	0x101
#define MVLE_REMOTE_NAME 	0x102
#define MVLE_NUM_CONNECTIONS	0x103

/************************************************************************/
/* Bind and Connection control structures				*/

typedef struct net_bind_ctrl
  {
/* These parameters must be set before using in mvl_start_acse		*/
  ST_CHAR ar_name[MAX_AR_LEN+1];	

/* Internal */
  ST_VOID *user_info;
  ST_INT tp_type;

  ST_VOID *copp_bind_id;	/* bind id returned from COPP layer	*/

  ST_BOOLEAN cmd_done;		/* Is bind/unbind command done?		*/
  ST_RET cmd_rslt;		/* result of bind/unbind command	*/
  } NET_BIND_CTRL;

/************************************************************************/
/* MVL configuration information. Filled in by user or configuration	*/
/* and passed to mvl_start_acse.					*/
/************************************************************************/

typedef struct
  {
  ST_INT max_msg_size;	     	/* Max MMS message size			*/
  ST_INT num_calling;	     	/* total number of calling connections	*/
  ST_INT num_called;	     	/* total number of called connections	*/

/* Used for single local address binding (LEAN-T)			*/ 
  ST_CHAR local_ar_name[MAX_AR_LEN+1];

/* Used for multiple local address binding only				*/ 
  ST_BOOLEAN use_bind_ctrl;	/* Set to 0 for single local		*/

/* User sets these only when use_bind_ctrl is SD_TRUE			*/
  ST_INT         max_bind_ctrl;	/* Max allowed				*/
  ST_INT         num_bind_ctrl;	/* Active bindings			*/
  NET_BIND_CTRL *bind_ctrl;	/* Array of NET_BIND_CTRL		*/
				/* MUST BE allocated with chk_?alloc.	*/
  } MVL_CFG_INFO;

/* Persistant config information; set by MVL during mvl_start_acse 	*/
extern MVL_CFG_INFO *mvl_cfg_info;

ST_RET osicfgx (ST_CHAR *xml_filename, MVL_CFG_INFO *mvlCfg);
ST_VOID osicfgx_unconfigure (ST_VOID);

/************************************************************************/
/* Global configuration variables					*/

extern INIT_INFO *mvl_init_resp_info;

/************************************************************************/
/* ACSE interface related MVL ACSE functions				*/

ST_VOID mvl_set_max_msg_size (ST_INT max_msg_size);

ST_RET mvl_start_acse (MVL_CFG_INFO *cfgInfo);
ST_RET mvl_end_acse (ST_VOID);

ST_RET mvla_initiate_req_exx (ST_CHAR *locAr,
			     ST_CHAR *remAr,
			     INIT_INFO *req_info, 
			     INIT_INFO *resp_info,
			     MVL_NET_INFO **net_info_out,
			     MVL_REQ_PEND **req_out,
			     ACSE_AUTH_INFO *auth_info,
			     S_SEC_ENCRYPT_CTRL *encrypt_info,
			     S_CERT_CTRL *locMaceCert);
#define mvla_initiate_req_ex(locAr,remAr,req_info,resp_info,net_info_out,req_out,auth_info,encrypt_info)\
       mvla_initiate_req_exx(locAr,remAr,req_info,resp_info,net_info_out,req_out,auth_info,encrypt_info,NULL)

ST_RET mvl_abort_req (MVL_NET_INFO *cc);
ST_RET mvl_abort_req_ex (MVL_NET_INFO *cc, 
			 ST_BOOLEAN diagnostic_pres, 
			 ST_ACSE_AUTH diagnostic);

ST_RET mvl_init_audt_addr (AUDT_APDU *audt, ST_CHAR *locArName, ST_CHAR *remArName);
ST_VOID mvl_dib_entry_to_app_ref (DIB_ENTRY *de, APP_REF *appRef);
ST_RET mvl_add_bind_ctrl (NET_BIND_CTRL *bc);

/************************************************************************/
/* User defined function pointer to handle disconnect indications	*/

#define MVL_ACSE_RELEASE_IND	1
#define MVL_ACSE_ABORT_IND	2
extern ST_VOID (*u_mvl_disc_ind_fun) (MVL_NET_INFO *cc, ST_INT discType);

/************************************************************************/
/* User defined functions to handle connect indications & confirms.	*/
/* User must supply these functions.					*/

extern ST_ACSE_AUTH u_mvl_connect_ind_ex (MVL_NET_INFO *cc, INIT_INFO *init_info, ACSE_AUTH_INFO *req_auth_info, ACSE_AUTH_INFO *rsp_auth_info);
extern ST_ACSE_AUTH u_mvl_connect_cnf_ex (MVL_NET_INFO *cc, AARE_APDU *ass_rsp_info);

/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif 	/* MVL_ACSE_INCLUDED */
/************************************************************************/

