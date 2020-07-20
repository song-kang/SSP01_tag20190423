/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsintr2.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Functions and variables below this point are typically not	*/
/*	used by the application, and need not be exposed.		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del globals _mmsdec_rslt, _mms_dec_info,	*/
/*			   and _mms_dec_info_pres.			*/
/* 03/04/02  JRB     11    Use SD_CONST on ALL (RUNTIME_TYPE *) vars.	*/
/* 12/20/01  JRB     10	   Convert to use ASN1R.			*/
/* 04/19/00  JRB     09    Fix SD_CONST for decode funct ptr arrays.	*/
/* 09/13/99  MDE     08    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     07    Changes to decode buffer allocation scheme   */
/* 03/19/98  JRB     06    Move ASN.1 error codes to "asn1defs.h".	*/
/* 03/12/98  MDE     05    '_ms_m_get_rt_info' use change		*/
/* 02/10/98  MDE     04    Removed unused 'm_cl_max_event_attributes'	*/
/* 07/16/97  MDE     03    Added m_param, m_service_resp for MMS_LITE	*/
/* 07/03/97  MDE     02    Removed CALLOC_OS				*/
/* 06/09/97  MDE     01    Added decode buffer size control variables	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMSINTR2_INCLUDED
#define MMSINTR2_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/************************************************************************/
/* Defines for Modifier and Companion Standard support.			*/
/*   MMSEASE defaults to supporting both, MMSEASE-LITE defaults to 	*/
/*   supporting neither.  						*/
/************************************************************************/

#ifndef MMS_LITE       
#define MOD_SUPPORT 	/* MMSEASE - support modifiers			*/
#define CS_SUPPORT      /* MMSEASE - support companion standards  	*/
#endif

/************************************************************************/
/************************************************************************/
/* Operation Specific Decode Buffer Management				*/

extern ST_VOID *(*m_calloc_os_fun)(ST_UINT num, ST_UINT size);
extern ST_VOID *(*m_realloc_os_fun)(ST_VOID *old, ST_UINT new_size);
extern ST_VOID  (*m_free_os_fun)(ST_VOID *buf);

ST_VOID *_m_get_dec_buf (ASN1_DEC_CTXT *aCtx, ST_UINT size);

/************************************************************************/
/************************************************************************/
/* Defines for the fourteen types of MMS PDUs				*/
/************************************************************************/

#define MMSREQ	    0  /* MMS request PDU for normal confirmed services */
#define MMSRESP     1  /* MMS response PDU for normal confirmed services*/
#define MMSERROR    2  /* MMS error PDU for normal confirmed services	*/
#define MMSUNREQ    3  /* MMS response PDU for unconfirmed services	*/
#define MMSREJECT   4  /* MMS reject PDU				*/
#define MMSCANREQ   5  /* MMS request PDU for Cancel service		*/
#define MMSCANRESP  6  /* MMS response PDU for Cancel service		*/
#define MMSCANERR   7  /* MMS error PDU for Cancel service		*/
#define MMSINITREQ  8  /* MMS request PDU for Initiate service		*/
#define MMSINITRESP 9  /* MMS response PDU for Initiate service 	*/
#define MMSINITERR  10 /* MMS error PDU for Initiate service		*/
#define MMSCNCLREQ  11 /* MMS request PDU for Conclude service		*/
#define MMSCNCLRESP 12 /* MMS response PDU for Conclude service 	*/
#define MMSCNCLERR  13 /* MMS error PDU for Conclude service		*/
#define UNKNOWNTYPE 14 /* not known - used in reject info structure	*/

/************************************************************************/
/************************************************************************/
/* Define base reserved invoke IDs for the Initiate and Conclude	*/
/* services.  Allows for up to 4096 associations with reserved		*/
/* invoke IDs.								*/
/************************************************************************/

#define INIT_INVOKE_ID	0x8000	/* base invoke ID for Initiate requests */
#define CONCL_INVOKE_ID 0x9000	/* base invoke ID for Conclude requests */

/************************************************************************/
/* Define the reserved invoke ID for unconfirmed requests.		*/
/************************************************************************/

#define UNREQ_INVOKE	0xFFFF	/* reserved invoke ID for unconf req's  */

/************************************************************************/
/************************************************************************/
/* The following function pointer arrays are used for the request and	*/
/* response decode functions. The operation opcode is used as the index	*/
/* into the table.							*/
/************************************************************************/

#define MAX_REQ_DFUN	85	/* highest request decode array entry	*/
#define MAX_RSP_DFUN	85	/* highest response decode array entry	*/


/************************************************************************/
/************************************************************************/
/************************************************************************/
/* This structure is used to return the results of a MMS PDU decode.	*/
/* The user passes a pointer to a struct of this type to decode_mms,	*/
/* and when the decode is complete, the struct info will be filled in.	*/
/*									*/
/* dec_level :								*/
/*   0 - Nothing valid							*/
/*   1 - type valid							*/
/*   2 - type, invoke ID valid						*/
/*   3 - type, invoke ID, op valid					*/
/************************************************************************/

struct mmsdec_info
  {
  ST_RET err_code;		/* decode error code (NO_DECODE_ERR) 	*/
  ST_INT dec_level;		/* indicates valid members		*/
  ST_INT type;			/* MMS pdu type (req,resp,err,rej)	*/
  ST_INT  op;			/* operation opcode			*/
  ST_UINT32 id;			/* invoke ID				*/
  struct list_of_mods mods;	/* tracking of modifiers		*/
  struct csi cs;		/* Companion Standard Info		*/
  ST_BOOLEAN data_pres;		/* flag to indicate data buffer present */
  ST_VOID *data_ptr;		/* pointer to data if present		*/
  };
typedef struct mmsdec_info MMSDEC_INFO;

extern ST_VOID (* SD_CONST mms_req_decode_fun [MAX_REQ_DFUN+1]) (ASN1_DEC_CTXT *aCtx);
extern ST_VOID (* SD_CONST mms_rsp_decode_fun [MAX_RSP_DFUN+1]) (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/* _ms_mms_decode is used at MMS decode level, not generally used by	*/
/* application directly.						*/
/************************************************************************/

ST_RET _ms_mms_decode (ST_UCHAR *ptr, ST_INT len, MMSDEC_INFO *rslt);

extern ST_UINT      _mmsdec_ctxt;	/* default to core context	*/
extern ST_INT       _mmsdec_msglen;	/* MMS decode message length	*/
extern ST_UCHAR    *_mmsdec_msgptr;	/* MMS decode message ptr	*/


/************************************************************************/
/************************************************************************/
/* These variables are used help control decode operation specific data */
/* structure allocation. For MMS-LITE (M_USR_DEC_BUF), these variables 	*/
/* are used to tell the decode system how many objects the allocated 	*/
/* buffer (_mms_dec_info). For MMS-EASE, these are normally not used;	*/
/* if they are set non-zero, they will be used in calculating the size	*/
/* of the dec info buffer to be allocated.				*/

/* Client Decode Limits. 						*/
extern ST_INT m_cl_read_spec_in_result;
extern ST_INT m_cl_max_read_var_spec;
extern ST_INT m_cl_max_read_acc_rslt;
extern ST_INT m_cl_max_info_var_spec;
extern ST_INT m_cl_max_info_acc_rslt;
extern ST_INT m_cl_max_getvla_vars;
extern ST_INT m_cl_max_write_rslts;
extern ST_INT m_cl_max_namel_names;
extern ST_INT m_cl_max_getcl_cap;
extern ST_INT m_cl_max_getpi_doms;
extern ST_INT m_cl_max_getdom_cap;
extern ST_INT m_cl_max_getdom_pi;
extern ST_INT m_cl_max_initupl_cap;
extern ST_INT m_cl_max_alarm_summary;
extern ST_INT m_cl_max_journal_entries;
extern ST_INT m_cl_max_entry_content;
extern ST_INT m_cl_max_semaphore_entries;
extern ST_INT m_cl_max_named_tokens;
extern ST_INT m_cl_max_file_directory;
extern ST_INT m_cl_max_event_enrollment;
extern ST_INT m_cl_max_enrollment_summary;

/* Server Decode Limits. 						*/
extern ST_INT m_sv_max_loadd_cap;
extern ST_INT m_sv_max_read_var_spec;
extern ST_INT m_sv_max_write_var_spec;
extern ST_INT m_sv_max_write_data;
extern ST_INT m_sv_max_rqdlnl_cap;
extern ST_INT m_sv_max_initdnld_cap;
extern ST_INT m_sv_max_vstr;
extern ST_INT m_sv_max_file_names;  
extern ST_INT m_sv_max_obj_name;
extern ST_INT m_sv_max_prompt_count;
extern ST_INT m_sv_max_num_vars;
extern ST_INT m_sv_max_data_count;
extern ST_INT m_sv_max_entry_content;
extern ST_INT m_sv_max_data_vars;
extern ST_INT m_sv_max_jread_vars;

/************************************************************************/
/* These bitstrings are located in mmsop_en.c and indicate which 	*/
/* services and CBB's are supported by this application.		*/
#ifdef MMS_LITE
extern SD_CONST ST_UCHAR m_param[2];	/* parameter support			*/
			        /* services supported as responder	*/
extern SD_CONST ST_UCHAR m_service_resp[11];
#endif

/************************************************************************/
/************************************************************************/
/* This variable can be set to force use of the DIS MMS floating point 	*/
/* type protocol (used in any over the wire and internal ASN.1 type 	*/
/* definitions).							*/

extern ST_BOOLEAN m_use_dis_float;  /* = SD_FALSE; */

/************************************************************************/
/************************************************************************/
/* ALTERNATE ACCESS							*/

/* This structure is used to control the creation of RTAA types		*/
/* and contains state information for the various functions to use	*/
/* during the process.							*/

struct aa_apply_ctrl
  {
/* Alternate Access spec                                                */
  ST_INT num_aa;                       	/* # elements in AA table       */
  struct alt_acc_el *aa_head;           /* Head of AA table             */
  struct alt_acc_el *aa_end;            /* Last element in AA table     */
  struct alt_acc_el *curr_aa;           /* Current location in table    */

/* 'All'runtime aa 							*/
  ST_INT num_all_rtaa;                   
  struct rt_aa_ctrl *all_rtaa_head;
  struct rt_aa_ctrl *all_rtaa_end;

/* New (output) runtime aa                                              */
  ST_INT num_rtaa;
  struct rt_aa_ctrl *rtaa_head;
  struct rt_aa_ctrl *curr_rtaa;

/* Nest Level	*/
  ST_INT nest;
  };
typedef struct aa_apply_ctrl AA_APPLY_CTRL;

/************************************************************************/
/* Floating point type ASN.1 definition values 				*/

#define SNGL_WHOLE	32
#define SNGL_FRACT	23
#define SNGL_EXP	 8

#define DOUBL_WHOLE	64
#define DOUBL_FRACT	52
#define DOUBL_EXP	11

/* Used to determine number of bytes required to hold a number of bits	*/
#define CALC_BIT_LEN(x) (((x)+7)/8)

/************************************************************************/
ST_RET _ms_m_get_rt_info (SD_CONST RUNTIME_TYPE *rt,
			  ST_INT *algn_out,
			  ST_INT *ellen_out);

/************************************************************************/
/* Function pointers for Variable manipulation functions.		*/
/* User exposed functions (i.e. ms_add_named_var, etc.) call these	*/
/* function pointers which should point to "linked list" functions	*/
/* by default (see "_ms_ll_*" functions below).  If the "ms_bs_init"	*/
/* function is called, these will point to "binary search" functions	*/
/* (see "_ms_bs_*" functions below).					*/
/************************************************************************/
#ifndef MMS_LITE

extern NAMED_VAR *(*_ms_fp_add_named_var) (DOMAIN_OBJS *dom, ST_CHAR *name,
	     OBJECT_NAME *type, VAR_ACC_ADDR *addr, ST_INT chan);
extern NAMED_VAR *(*_ms_fp_add_var) (DOMAIN_OBJS *dom, ST_CHAR *name,
	     NAMED_TYPE *type_ptr, VAR_ACC_ADDR *addr, ST_INT chan);
extern NAMED_VAR *(*_ms_fp_find_named_var)
                                (DOMAIN_OBJS *dom, ST_CHAR *name);
extern ST_RET (*_ms_fp_del_named_var) (DOMAIN_OBJS *dom, ST_CHAR *name);
extern ST_VOID (*_ms_fp_del_all_named_vars) (DOMAIN_OBJS *dom);
extern ST_VOID (*_ms_fp_del_deletable_named_vars) (DOMAIN_OBJS *, DELVAR_RESP_INFO *);
extern NAMED_VAR_LIST *(*_ms_fp_add_nvlist) (DOMAIN_OBJS *dom,
	ST_CHAR *name, ST_INT num_vars, VARIABLE_LIST *var_list);
extern NAMED_VAR_LIST *(*_ms_fp_find_nvlist) (DOMAIN_OBJS *dom, ST_CHAR *name);
extern ST_RET (*_ms_fp_del_nvlist) (DOMAIN_OBJS *dom, ST_CHAR *name);
extern ST_VOID (*_ms_fp_del_all_nvlists) (DOMAIN_OBJS *dom);
extern ST_VOID (*_ms_fp_del_deletable_nvlists) (DOMAIN_OBJS *, DELVLIST_RESP_INFO *);

extern NAMED_DOM_CTRL *(*_ms_fp_add_named_domain) (INITDOWN_REQ_INFO *dom_info, 
					ST_UCHAR pro);
extern NAMED_DOM_CTRL *(*_ms_fp_find_named_dom)(ST_CHAR dname[MAX_IDENT_LEN+1]);
extern ST_RET (*_ms_fp_del_named_dom) (ST_CHAR *dname);
extern ST_VOID (*_ms_fp_del_all_named_doms)(NAMED_DOM_CTRL *head_of_dom_list);

extern ST_VOID (*_ms_fp_write_object_names) (ST_VOID *dom_or_vmd,
	NAMELIST_REQ_INFO *reqinfo, NAMELIST_RESP_INFO *respinfo,
	ST_INT max_names);
#endif


#ifdef __cplusplus
}
#endif

#endif  /* MMSINTR2_INCLUDED */
