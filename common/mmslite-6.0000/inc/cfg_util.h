/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1991-2005 All Rights Reserved       			*/
/*									*/
/* MODULE NAME : cfg_util.h						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/31/05  EJV     05    Chg MAX_NUM_KEYWORDS from 100 to 200.	*/
/* 10/08/98  MDE     04    Migrated to updated SLOG interface		*/
/* 08/06/98  JRB     03    Added cfg_get_octet_string function.		*/
/* 12/04/97  KCR     02    Added cfg_goto_keyword function        	*/
/* 08/14/97  RKR     01    Format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef CFG_UTIL_INCLUDED
#define CFG_UTIL_INCLUDED

#include "slog.h"

#ifdef __cplusplus		
extern "C" {
#endif

/************************************************************************/

#define MAX_RVAL_LEN 	       256
#define MAX_NUM_KEYWORDS       200	/* just to prevent runaway      */

/* Keyword table element                                                */
struct cfg_kw_el
  {
  ST_CHAR *name; 			/* string to match on			*/
  ST_VOID (*extract)(ST_VOID);    	/* function to call to extract  	*/
  ST_RET  (*valid)(ST_VOID);     	/* call to check if ok to call 		*/
  };
typedef struct cfg_kw_el CFG_KW_EL;

/************************************************************************/
/* Externally visible variables                                        */

extern ST_BOOLEAN config_stop;     /* Set SD_TRUE to terminate        	*/
extern ST_RET     config_err;      /* Set to non-zero for error    	*/
extern ST_BOOLEAN config_eof;      /* Set to SD_SUCCESS when eof is ok	*/

extern ST_CHAR *cfg_line_buf;	       /* file line read buffer        	*/
extern ST_INT cfg_curr_line;           /* Current line number          	*/
extern ST_INT cfg_line_offset;	       /* Offset into cfg_line_buf     	*/
extern ST_CHAR *cfg_special_char_str;  /* Special ravl chars  	*/
extern ST_CHAR *cfg_end_of_rval_str;   /* 'End of rval' chars  */

/************************************************************************/
/* Internal CFG_UTIL error codes                                        */

#define CFG_CONVERT_ERR	   0x8000
#define CFG_GET_RVAL_ERR   0x8001
#define CFG_UTIL_ERR       0x8002

/************************************************************************/
/* Externally visible functions                                         */

ST_RET cfg_process_file (ST_CHAR *fileName, CFG_KW_EL *root_kw_tbl);
ST_RET cfg_tbl_push (CFG_KW_EL *);
ST_RET cfg_tbl_pop (ST_INT);
ST_CHAR *cfg_get_alloc_string (ST_VOID);
ST_CHAR *cfg_get_string_ptr (ST_VOID);
ST_RET cfg_get_short (ST_INT16 *int_ptr);
ST_RET cfg_get_ushort (ST_UINT16 *int_ptr);
ST_RET cfg_get_int (ST_INT *int_ptr);
ST_RET cfg_get_uint (ST_UINT *int_ptr);
ST_RET cfg_get_long (ST_LONG *int_ptr);
ST_RET cfg_get_ulong (ST_ULONG *int_ptr);
ST_RET cfg_get_hex_ushort (ST_UINT16 *int_ptr);
ST_RET cfg_get_hex_uint (ST_UINT *int_ptr);
ST_RET cfg_get_hex_ulong (ST_ULONG *out_ptr);
ST_RET cfg_get_value (ST_CHAR *format_string, ST_VOID *out_ptr);
ST_VOID  cfg_set_config_err(ST_VOID);
ST_VOID  cfg_set_endfile_ok(ST_VOID);
ST_RET cfg_get_double (ST_DOUBLE *out_ptr);
ST_RET cfg_goto_keyword (ST_CHAR *keyword);
ST_RET cfg_get_octet_string (ST_UCHAR *ostr,	/* ptr to user's ostr	*/
                ST_UINT *len_out_ptr,	/* addr of len var to be set	*/
                ST_UINT len_max);	/* maximum len to allow.	*/

/************************************************************************/
/* CFG_UTIL logging control                                             */

#define CFG_LOG_ERR     0x0001
#define CFG_LOG_FLOW    0x0002
extern ST_UINT cfg_log_mask;

/* For compatibility with older code only				*/
#define cfg_sLogCtrl sLogCtrl

/************************************************************************/

#ifdef __cplusplus
}

#endif

#endif
