/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2009, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : asn1r.h						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Contains definitions and global variables useful to the ASN1DE	*/
/*	user.								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   ASN1_DEC_CTXT: add mmsdec_rslt.		*/
/* 11/17/09  JRB     32    Add asn1r_chk_tag, asn1r_*_contents.		*/
/* 07/21/09  JRB     31    Add asn1r_bitstr_truncated in ASN1_DEC_CTXT.	*/
/*			   Del GET_CONSTRUCTED_*STRINGS defines.	*/
/* 05/28/09  JRB     30    Add asn1r_decode_init, asn1r_get_tag functs.	*/
/*			   Add asn1r_msg_start in ASN1_DEC_CTXT struct.	*/
/*			   Add define ASN1E_INVALID_DATA.		*/
/*			   Add asn1r_strt_constr_indef.			*/
/* 01/15/09  MDE     29    Added asn1_peek, removed asn1r_head_decode	*/
/* 11/12/08  MDE     28    Added asn1r_last_byte, asn1r_head_decode rc 	*/
/* 07/02/07  JRB     26    Chg ..fixlen, ..varlen arg to (ST_UCHAR *).	*/
/* 12/20/06  JRB     25    Add bstrcmp.					*/
/* 03/17/06  JRB     24    Add asn1r_anytag_fun, ASN1_ANYTAG_METHOD,	*/
/*			   ASN1MASK_*.					*/
/*			   Chg BSTR_BIT_GET macro to return only 0 or 1.*/
/* 12/05/05  JRB     23    Add asn1r_skip_elmnt & more asn1r_get* protos*/
/* 08/01/05  JRB     22    Add const on asn1r_wr_vstr, asn1_wr_octstr args.*/
/* 03/23/05  JRB     21    Move APP_REF from suicacse to here so almost	*/
/*			   any SISCO app can use it.			*/
/* 07/08/04  JRB     20    Add define TIME_T_1984_JAN_1 & other defines.*/
/*			   Add asn1_convert_timet_to_btime6.		*/
/* 10/13/03  JRB     19    Add ASN1E_DECODE_OTHER, ASN1E_ENCODE_OTHER.	*/
/* 03/31/03  JRB     18    Add UTF8string support.			*/
/*			   asn1r_get_bitstr: add max_bits arg.		*/
/* 12/14/02  JRB     17    Add BSTR_*, MMS_BVSTRING, bstrcpy, bvstrcpy	*/
/* 11/11/02  EJV     16    asn1r_tag_add: added type cast		*/
/* 11/08/02  JRB     15    Add ASN1_DEC_PTR, ASN1_DEC_LEN macros.	*/
/* 10/29/02  JRB     14    Add asn1r_magic to ASN1_ENC_CTXT.		*/
/* 10/25/02  JRB     13    Add ASN1_ENC_PTR, ASN1_ENC_LEN macros.	*/
/*			   Add MMS_BTIME6 struct.			*/
/* 07/03/02  EJV     12    MMS_UTC_TIME: chg name usec to fraction.	*/
/* 01/22/02  JRB     11    Add "asn1r_buf_end" to ASN1_ENC_CTXT.	*/
/*			   Chg asn1r_end_of_buffer to asn1r_buf_start	*/
/*			   & asn1r_field_start to asn1r_field_end	*/
/*			   & _asn1_constr_start to asn1r_constr_end	*/
/*			   in ENC_CTX (start means start, end means end)*/
/* 01/07/02  EJV     09    Added asn1_ prefix to convert_... functions.	*/
/* 01/04/02  EJV     08    Add convert_btod_to_utc, convert_utc_to_btod	*/
/* 12/20/01  JRB     07    Split ASN1_CTXT into ASN1_DEC_CTXT, ASN1_ENC_CTXT*/
/*			   Del unused calendar stuff.			*/
/*			   Del backward compatibility defines.		*/
/*			   Chg asn1r_objidcmp to asn1_objidcmp.		*/
/*			   Del unused tzhour_diff, tconst, default_dcomp*/
/*			   Del unused strt_asn1,asn1_tz_calc,asn1_is_dst*/
/* 11/09/01  EJV     06    Added MMS_UTC_TIME typedef.			*/
/*			   Added asn1r_wr_utc_time, asn1r_get_utc_time	*/
/* 			   ASN1_CTXT: del _save_asn1_len, _save_asn1_ptr*/
/* 08/15/00  JRB     05    Add asn1_ctxt "forward reference".		*/
/* 06/19/00  JRB     04    Add #if INT64_SUPP...			*/
/* 05/25/00  RKR     03    Deleted a an extra declaration of asn1_tzone	*/
/* 07/29/99  MDE     02    Added TABLE method				*/
/* 07/26/99  MDE     01    New, derived from asn1defs.h			*/
/************************************************************************/

#if !defined(ASN1DEFS_INCLUDED) && !defined(ASN1R_INCLUDED)
#define ASN1R_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/

#include "slog.h"

/* Defines for UNICODE local data format.	*/
#define UNICODE_UTF8	0
#define UNICODE_UTF16	1
#if !defined(UNICODE_LOCAL_FORMAT)
#if defined(_WIN32)
#define UNICODE_LOCAL_FORMAT	UNICODE_UTF16
#else
#define UNICODE_LOCAL_FORMAT	UNICODE_UTF8	/*default format	*/
#endif
#endif	/* !defined(UNICODE_LOCAL_FORMAT)	*/

/************************************************************************/
/* asn1_debug_sel bit assignments					*/

#define ASN1_LOG_DEC  0x0001   
#define ASN1_LOG_ENC  0x0002   
#define ASN1_LOG_ERR  0x0004   
#define ASN1_LOG_NERR 0x0008   

/************************************************************************/
/* These are S_LOG control items for ASN1 logging			*/

extern ST_UINT asn1_debug_sel;

/************************************************************************/
/* Support for development time debugging can be enabled here		*/
/* #define DEBUG_ASN1_DECODE */

/************************************************************************/
/* Support for seldom needed decode features can be disabled here	*/

#if !defined(MMS_LITE)
#define ASN1_ARB_FLOAT
#endif

/************************************************************************/
/* Helpful Time-related defines						*/

#define SECONDS_PER_DAY		(60*60*24)
#define MILLISECONDS_PER_DAY	(1000 * SECONDS_PER_DAY) 
#define DAYS_FROM_1970_TO_1984	((11*365) + (3*366))	/*includes 3 leap years 1972, 1976, 1980*/
#define TIME_T_1984_JAN_1	(DAYS_FROM_1970_TO_1984 * SECONDS_PER_DAY)

/************************************************************************/
/* Binary Time Of Day							*/

#define MMS_BTOD4	4
#define MMS_BTOD6	6

typedef struct btod_data
  {
  ST_INT form;	 	/* MMS_BTOD6, MMS_BTOD4				*/
  ST_INT32 ms;	      	/* Number of milliseconds since midnight	*/
  ST_INT32 day;	      	/* Number of days since Jan 1, 1984		*/
  } MMS_BTOD;

/* MMS_BTIME6: this structure is the format used to pass "Btime6" data	*/
/* (i.e. ISO/IEC 9506-2 TimeOfDay with size=6) between the user and	*/
/* the ASN.1 encoder/decoder (i.e. ms_asn1_to_local & ms_local_to_asn1).*/
typedef struct
  {
  ST_INT32 ms;		/* Number of milliseconds since midnight	*/
  ST_INT32 day;		/* Number of days since Jan 1, 1984		*/
  } MMS_BTIME6;

/************************************************************************/
/* UTC Time								*/

typedef struct mms_utc_time_tag
  {
  ST_UINT32 secs;      	/* Number of seconds since January 1, 1970	*/
  ST_UINT32 fraction;  	/* Fraction of a second				*/
  ST_UINT32 qflags;	/* Quality flags, 8 least-significant bits only	*/
  } MMS_UTC_TIME;

ST_RET asn1_convert_btod_to_utc (MMS_BTOD *btod, MMS_UTC_TIME *utc);
ST_RET asn1_convert_utc_to_btod (MMS_UTC_TIME *utc, MMS_BTOD *btod);
ST_RET asn1_convert_timet_to_btime6 (time_t tThis, MMS_BTIME6 *bTime6);

/************************************************************************/
/* Variable Length Bitstring						*/
/* This structure defines the format used to pass "variable length	*/
/* bitstring" data between the user and					*/
/* the ASN.1 encoder/decoder (i.e. ms_asn1_to_local & ms_local_to_asn1).*/
/* NOTE: For bitstrings longer than 8 bits, the actual data extends	*/
/*       beyond the end of this struct.					*/

typedef struct
  {
  ST_INT16 len;
  ST_UCHAR data[1];
  } MMS_BVSTRING;

/************************************************************************/
/* VMS Float Format select						*/

#define 	A_D_FLOAT         	1
#define		A_G_FLOAT		2
extern ST_INT	asn1_vax_double_type;

/************************************************************************/
/* ASN1-DE limitiations							*/

#define ASN1_MAX_PDU   0x7FFF  /* maximum decodable message length	*/
#define ASN1_MAX_FIELD 0x7FFF  /* maximum field (data element) length	*/
#define ASN1_MAX_LEVEL 50      /* maximum supported nesting level	*/

/************************************************************************/
/* ASN.1 Definitions to support identifier construction by bit OR.   	*/
/* Used to make up and test 'tag' bits.                                 */
			
/* These used in ASN1_ANYTAG_METHOD decode method.	*/
#define ASN1MASK_UNI   	0x0000 	/* universal class			*/
#define ASN1MASK_APP   	0x4000 	/* application wide			*/
#define ASN1MASK_CTX   	0x8000 	/* context specific			*/
#define ASN1MASK_PRV   	0xC000 	/* private use				*/
#define ASN1MASK_CONSTR	0x2000	/* constructor				*/

/* These used in other decode methods and in encoder.	*/
#define UNI	   	0x00 	/* universal class			*/
#define APP	   	0x40 	/* application wide			*/
#define CTX	   	0x80 	/* context specific			*/
#define PRV	   	0xC0 	/* private use				*/
#define CONSTR	   	0x20 	/* constructor				*/

#define DEF	   	0x00 	/* definite length constructor		*/
#define INDEF	   	0xFF 	/* indefinite length constructor	*/

/* Universal class ASN.1 ID code definitions   				*/
#define EOC_CODE	0x00
#define BOOL_CODE	0x01
#define INT_CODE	0x02
#define BITS_CODE	0x03
#define OCT_CODE	0x04
#define NULL_CODE	0x05
#define OBJ_ID_CODE	0x06
#define OBJDSCR_CODE	0x07
#define EXTERN_CODE	0x08
#define SEQ_CODE	0x10
#define SET_CODE	0x11
#define NUMSTR_CODE	0x12
#define PRTSTR_CODE	0x13
#define S61STR_CODE	0x14
#define S100STR_CODE	0x15
#define IA5STR_CODE	0x16
#define UTCTIM_CODE	0x17
#define GENTIM_CODE	0x18
#define GRAFSTR_CODE	0x19
#define VISTR_CODE	0x1A
#define GENSTR_CODE	0x1B


/************************************************************************/
/* ASN.1 decode error definitions					*/
/* Used as the argument to "asn1_set_dec_err" function which sets the	*/
/* global variable "asn1_pdu_dec_err".					*/
/************************************************************************/

#define NO_DECODE_ERR		0	/* no decode error		*/
#define ASN1E_NEST_TOO_DEEP	0x7001
#define ASN1E_UNEXPECTED_CLASS	0x7002
#define ASN1E_UNEXPECTED_TAG	0x7003
#define ASN1E_ID_TOO_BIG	0x7004
#define ASN1E_CSTR_INVALID_LEN	0x7005
#define ASN1E_INVALID_LENGTH	0x7006
#define ASN1E_UNEXPECTED_FORM	0x7007
#define ASN1E_PDU_TOO_BIG	0x7008
#define ASN1E_END_O_BUFFER	0x7009
#define ASN1E_BITCSTR_TOO_BIG	0x700a
#define ASN1E_INVALID_BITSTR	0x700b
#define ASN1E_OCTSTR_TOO_BIG	0x700c
#define ASN1E_INVALID_BOOLEAN	0x700d
#define ASN1E_END_OF_MESSAGE	0x700e
#define ASN1E_END_OF_CSTR	0x700f

/* CRITICAL: The rest of the ASN.1 decode error definitions must not	*/
/* must not be changed.  The lower 2 nibbles are used to set the	*/
/* REJECT class and code (mmsdec_map_err saves value to rslt.err_code	*/
/* which is used in "mmspdu.c" to generate REJECT class and code).	*/

/************************************************************************/
/* CONFIRMED REQUEST REJECT DEFINITIONS					*/
/************************************************************************/
#define REQ_UNSPECIFIED	 	0x7010
#define REQ_UNREC_SERV		0x7011
#define REQ_UNREC_MOD		0x7012
#define REQ_INVAL_INVOKE 	0x7013
#define REQ_INVAL_ARG		0x7014
#define REQ_INVAL_MOD		0x7015
#define REQ_OUTSTD_EXCEED	0x7016
#define REQ_EXCSV_LENGTH	0x7017
#define REQ_EXCSV_NEST		0x7018
#define REQ_BAD_VALUE		0x7019

/************************************************************************/
/* CONFIRMED RESPONSE REJECT DEFINITIONS				*/
/************************************************************************/
#define RESP_UNSPECIFIED	0x7020
#define RESP_UNREC_SERV 	0x7021
#define RESP_INVAL_INVOKE	0x7022
#define RESP_INVAL_RSLT	 	0x7023
#define RESP_EXCSV_LENGTH	0x7024
#define RESP_EXCSV_NEST		0x7025
#define RESP_BAD_VALUE		0x7026

/************************************************************************/
/* CONFIRMED ERROR REJECT DEFINITIONS					*/
/************************************************************************/
#define ERR_UNSPECIFIED 	0x7030
#define ERR_UNREC_SERV		0x7031
#define ERR_INVAL_INVOKE	0x7032
#define ERR_INVAL_SERV		0x7033
#define ERR_BAD_VALUE		0x7034

/************************************************************************/
/* UNCONFIRMED PDU REJECT DEFINITIONS					*/
/************************************************************************/
#define UNCONF_UNSPECIFIED 	0x7040
#define UNCONF_UNREC_SERV	0x7041
#define UNCONF_INVAL_ARG	0x7042
#define UNCONF_EXCSV_NEST	0x7043
#define UNCONF_BAD_VALUE	0x7044

/************************************************************************/
/* PDU REJECT DEFINITIONS						*/
/************************************************************************/
#define PDU_UNKNOWN_TYPE	0x7050
#define PDU_INVALID		0x7051

/************************************************************************/
/* CANCEL REQUEST REJECT DEFINITIONS					*/
/************************************************************************/
#define CANREQ_UNSPECIFIED 	0x7060
#define CANREQ_INVAL_INVOKE	0x7061

/************************************************************************/
/* CANCEL RESPONSE REJECT DEFINITIONS					*/
/************************************************************************/
#define CANRESP_UNSPECIFIED 	0x7070
#define CANRESP_INVAL_INVOKE	0x7071

/************************************************************************/
/* CANCEL ERROR REJECT DEFINITIONS					*/
/************************************************************************/
#define CANERR_UNSPECIFIED 	0x7080
#define CANERR_INVAL_INVOKE	0x7081
#define CANERR_INVAL_SERV 	0x7082
#define CANERR_BAD_VALUE	0x7083

/************************************************************************/
/* CONCLUDE REQUEST REJECT DEFINITIONS					*/
/************************************************************************/
#define CONREQ_UNSPECIFIED 	0x7090
#define CONREQ_INVAL_ARG 	0x7091

/************************************************************************/
/* CONCLUDE RESPONSE REJECT DEFINITIONS					*/
/************************************************************************/
#define CONRESP_UNSPECIFIED 	0x70A0
#define CONRESP_INVAL_RSLT 	0x70A1

/************************************************************************/
/* CONCLUDE ERROR REJECT DEFINITIONS					*/
/************************************************************************/
#define CONERR_UNSPECIFIED 	0x70B0
#define CONERR_INVAL_SERV	0x70B1
#define CONERR_BAD_VALUE	0x70B2

/************************************************************************/
/* OTHER ASN.1 ENCODE/DECODE ERROR DEFINITIONS				*/
/************************************************************************/
#define ASN1E_DECODE_OTHER	0x70C0
#define ASN1E_ENCODE_OTHER	0x70C1
#define ASN1E_INVALID_DATA	0x70C2

/************************************************************************/
/* General variables and pointers used by the ASN1 system.		*/
/************************************************************************/

#define ASN1_TAG_METHOD   0 	/* decode by tag method 		*/
#define ASN1_CLASS_METHOD 1 	/* decode by class method		*/
#define ASN1_TABLE_METHOD 2 	/* decode by table method		*/
#define ASN1_ANYTAG_METHOD 3	/* decode by "anytag" method		*/
				/* same callback for any tag of any class*/
#define CSTR_DONE_FUN	   ac->asn1r_c_done_fun[ac->asn1r_msg_level]

/************************************************************************/
/* This define is the maximum tag + length for a MMS-EASE encoded data	*/
/* element (actually probably is 9), and is used in fin_constr and 	*/
/* fin_prim to prevent encode buffer overrun				*/

#define ASN1_MAX_ELEMENT_OVERHEAD 	10

struct asn1_dec_ctxt;	/* Forward reference	*/
/************************************************************************/
/************************************************************************/
/* TAG METHOD STRUCTURES AND DEFINES					*/

#define ASN1R_TAG_VAL(class,code) (((ST_UINT16)(class) << 8) | (code))

typedef struct
  {
  ST_UINT16 _asn1r_tag;
  ST_VOID (*_asn1r_tag_fun)(struct asn1_dec_ctxt *ac); 
  } ASN1R_TAG_PAIR;

/* Decode users: Select the structure that has sufficient tag capacity */
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[1]; } ASN1R_TAG_CTRL_1;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[2]; } ASN1R_TAG_CTRL_2;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[3]; } ASN1R_TAG_CTRL_3;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[4]; } ASN1R_TAG_CTRL_4;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[5]; } ASN1R_TAG_CTRL_5;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[6]; } ASN1R_TAG_CTRL_6;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[7]; } ASN1R_TAG_CTRL_7;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[8]; } ASN1R_TAG_CTRL_8;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[9]; } ASN1R_TAG_CTRL_9;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[10];} ASN1R_TAG_CTRL_10;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[11];} ASN1R_TAG_CTRL_11;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[12];} ASN1R_TAG_CTRL_12;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[13];} ASN1R_TAG_CTRL_13;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[14];} ASN1R_TAG_CTRL_14;
typedef struct {ST_INT num_tags; ASN1R_TAG_PAIR tags[15];} ASN1R_TAG_CTRL_15;


/************************************************************************/
/* ASN.1 Context							*/
/* This data structure is used to store all ASN.1 encode/decode context	*/
/* information. If an application provides thread safe storage for this */
/* data structure the ASN.1 tools become thread safe.			*/

#define ASN1_MAX_USR_INFO	5
#define MAX_TAG_FUN		16

typedef struct asn1_dec_ctxt
  {
/****** User and Internal Use ******/

/* User State								*/
  ST_VOID *usr_info[ASN1_MAX_USR_INFO];

/* General variables and pointers					*/
  ST_BOOLEAN asn1r_decode_done;
  ST_UCHAR *asn1r_field_ptr;
  ST_UCHAR *asn1r_field_start;
  ST_UCHAR *asn1r_done_ptr;
  ST_UCHAR asn1r_constr_elmnt;
  ST_UCHAR asn1r_elmnt_class;
  ST_UINT16 asn1r_elmnt_id;
  ST_INT asn1r_elmnt_len;
  ST_INT asn1r_msg_level;
  ST_BOOLEAN asn1r_indef_flag;	/* flag for indef cstr			*/
  ST_RET asn1r_pdu_dec_err;
  ST_INT asn1r_decode_method;
  ST_INT asn1r_save_method;
  ST_INT asn1r_max_bits;
  ST_INT asn1r_bitcount;
  ST_BOOLEAN asn1r_bitstr_truncated;	/* SD_TRUE if bitstr truncated	*/
					/* while decoding.		*/
  ST_INT asn1r_octetcount;

/* For ASN1 decode table mode */
  ST_VOID *asn1r_tag_table;

/* Function pointers used by decoder users				*/
  ST_VOID (*asn1r_decode_done_fun)(struct asn1_dec_ctxt *ac);
  ST_VOID (*asn1r_err_fun)(struct asn1_dec_ctxt *ac, ST_RET err_code);
  ST_VOID (*asn1r_c_done_fun[ASN1_MAX_LEVEL])(struct asn1_dec_ctxt *ac);
  ST_VOID (*asn1r_u_id_fun)(struct asn1_dec_ctxt *ac, ST_UINT16 id);
  ST_VOID (*asn1r_a_id_fun)(struct asn1_dec_ctxt *ac, ST_UINT16 id);
  ST_VOID (*asn1r_c_id_fun)(struct asn1_dec_ctxt *ac, ST_UINT16 id);
  ST_VOID (*asn1r_p_id_fun)(struct asn1_dec_ctxt *ac, ST_UINT16 id);
  /* asn1r_anytag_fun called for ANY tag of ANY class if		*/
  /* asn1r_decode_method==ASN1_ANYTAG_METHOD.				*/
  /* "id" argument has bits set to indicate class and/or constructor.	*/
  ST_VOID (*asn1r_anytag_fun)(struct asn1_dec_ctxt *ac, ST_UINT16 id);

/****** Internal Use Only ******/
/* Decode variables							*/
  ST_INT    _asn1_maxoctets;
  ST_UCHAR *_asn1_octetptr;

  ST_UCHAR *_ad_bitptr;

/* Function pointers used internally by the ASN.1 tools			*/
  ST_VOID (*_asn1r_cstr_done_save)(struct asn1_dec_ctxt *ac);
  ST_VOID (*_asn1r_fun_save)(struct asn1_dec_ctxt *ac);

  ST_INT    _asn1r_ntag;
  ST_UINT16 _asn1r_valid_tags[MAX_TAG_FUN];
  ST_VOID (*_asn1r_tag_fun[MAX_TAG_FUN])(struct asn1_dec_ctxt *ac); 
  ST_INT    _asn1r_old_ntag;

  ST_BOOLEAN _asn1_indef_track[ASN1_MAX_LEVEL];
  ST_UCHAR  *_asn1_constr_start[ASN1_MAX_LEVEL];
  ST_INT     _asn1_byte_count[ASN1_MAX_LEVEL];

  ST_INT    _ad_parse_method_save;
  ST_VOID (*_ad_parse_asn1r_fun_save)(struct asn1_dec_ctxt *ac);

  ST_UCHAR *_contents_done;   /* will be here when done parsing cstr contents	*/
  ST_UCHAR *_parse_begin_ptr; /* pointer to beginning of element being skipped*/

  ST_UCHAR *asn1r_msg_start;	/* ptr to beginning of ASN.1 message	*/
				/* used to calc offset within message	*/
				/* set  by asn1r_decode_init		*/
				/* used by asn1r_get_tag		*/
  struct mmsdec_info *mmsdec_rslt;	/* MMS decode result		*/
				/* used ONLY when decoding MMS PDUs	*/
  } ASN1_DEC_CTXT;

typedef struct asn1_enc_ctxt
  {
/* General variables and pointers					*/
  ST_UCHAR *asn1r_field_ptr;
  ST_UCHAR *asn1r_field_end;
  ST_INT asn1r_msg_level;
  ST_UCHAR *asn1r_buf_start;	/* ptr to first byte of buffer	*/
  ST_UCHAR *asn1r_buf_end;	/* ptr to last byte of buffer	*/
  ST_BOOLEAN asn1r_encode_overrun;
  ST_UINT16 asn1r_magic;	/* Use to verify asn1r_strt.. was called*/
  ST_UCHAR  *asn1r_constr_end[ASN1_MAX_LEVEL];
  } ASN1_ENC_CTXT;

/* Set "asn1r_magic" in ASN1_ENC_CTXT to this value in "asn1r_strt_asn1_bld".*/
/* Check later to be sure asn1r_strt_asn1_bld was called.		*/
#define ASN1_ENC_MAGIC_NUMBER	0xABCD
/************************************************************************/

ASN1_DEC_CTXT *asn1r_get_ctxt (ST_VOID);

ST_VOID asn1r_decode_asn1 (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr, ST_INT len);
ST_VOID asn1r_decode_asn1_seq (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr, ST_INT len);
ST_VOID asn1r_set_dec_err (ASN1_DEC_CTXT *ac, ST_RET err_code);
ST_VOID asn1r_done_err (ASN1_DEC_CTXT *ac);
ST_VOID asn1r_cstr_done_err (ASN1_DEC_CTXT *ac);
ST_VOID asn1r_set_all_cstr_done (ASN1_DEC_CTXT *ac);


ST_VOID asn1r_class_err (ASN1_DEC_CTXT *ac, ST_UINT16 id);
ST_VOID asn1r_chk_getcstr_done (ASN1_DEC_CTXT *ac);

#if !defined(ASN1R_FAST_TAG_ADD)

#define ASN1R_TAG_ADD(ac, class, code, fcn_ptr)\
   asn1r_tag_add (ac, (ST_UINT16)(((ST_UINT16)(class) << 8) | (code)), (fcn_ptr))

#else

#define ASN1R_TAG_ADD(ac, class, code, fcn_ptr)\
  {\
  ac->_asn1r_valid_tags[ac->_asn1r_ntag] = \
   			(((ST_UINT16)(class) << 8) | (code));\
  ac->_asn1r_tag_fun[ac->_asn1r_ntag] = (ST_VOID (*)(ASN1_DEC_CTXT *)) fcn_ptr;\
  ac->_asn1r_ntag++;\
  }
#endif


#define ASN1R_TAG_DEL(ac, class, code)\
   asn1r_tag_del (ac, (((ST_UINT16)(class) << 8) | (code)))

ST_VOID asn1r_tag_add (ASN1_DEC_CTXT *ac, ST_UINT16 tag_class, 
			 ST_VOID (*fcn_ptr)(ASN1_DEC_CTXT *ac));

ST_VOID asn1r_tag_del (ASN1_DEC_CTXT *ac, ST_UINT16 tag);
ST_VOID asn1r_tag_restore (ASN1_DEC_CTXT *ac);
ST_RET asn1_peek (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr, ST_INT len);

ST_VOID asn1r_strt_asn1_bld (ASN1_ENC_CTXT *ac, ST_UCHAR *bufptr, ST_INT buflen);
ST_VOID asn1r_fin_prim (ASN1_ENC_CTXT *ac, ST_UINT16 id_code, ST_UINT16 tag);
ST_VOID asn1r_fin_constr (ASN1_ENC_CTXT *ac, ST_UINT16 id_code, ST_UINT16 el_tag, ST_BOOLEAN indef);
ST_VOID asn1r_strt_constr (ASN1_ENC_CTXT *ac);
ST_VOID asn1r_strt_constr_indef (ASN1_ENC_CTXT *ac);	/* start indef len constr*/

ST_VOID asn1r_wr_u8     (ASN1_ENC_CTXT *ac, ST_UINT8 data);
ST_VOID asn1r_wr_u16    (ASN1_ENC_CTXT *ac, ST_UINT16 data);
ST_VOID asn1r_wr_u32    (ASN1_ENC_CTXT *ac, ST_UINT32 data);
ST_VOID asn1r_wr_i8     (ASN1_ENC_CTXT *ac, ST_INT8 data);
ST_VOID asn1r_wr_bool   (ASN1_ENC_CTXT *ac, ST_BOOLEAN data);
ST_VOID asn1r_wr_i16    (ASN1_ENC_CTXT *ac, ST_INT16 data);
ST_VOID asn1r_wr_i32    (ASN1_ENC_CTXT *ac, ST_INT32 data);
ST_VOID asn1r_wr_vstr   (ASN1_ENC_CTXT *ac, const ST_CHAR *data);
ST_VOID asn1r_wr_delmnt (ASN1_ENC_CTXT *ac, ST_UCHAR *bufptr, ST_INT buflen);
ST_VOID asn1r_wr_btod   (ASN1_ENC_CTXT *ac, MMS_BTOD *data);
ST_VOID asn1r_wr_time   (ASN1_ENC_CTXT *ac, time_t data);
ST_VOID asn1r_wr_objid  (ASN1_ENC_CTXT *ac, ST_INT16 *data, ST_INT num_el);
ST_VOID asn1r_wr_bitstr (ASN1_ENC_CTXT *ac, ST_UCHAR *bitptr, ST_INT numbits);
ST_VOID asn1r_wr_octstr (ASN1_ENC_CTXT *ac, const ST_UCHAR *octptr, ST_INT numoctets);
ST_VOID asn1r_wr_float  (ASN1_ENC_CTXT *ac, ST_FLOAT data);
ST_VOID asn1r_wr_double (ASN1_ENC_CTXT *ac, ST_DOUBLE data);
ST_VOID asn1r_wr_utc_time (ASN1_ENC_CTXT *ac, MMS_UTC_TIME *data);
ST_RET asn1r_wr_utf8   (ASN1_ENC_CTXT *ac, ST_CHAR *data, ST_INT el_len);

ST_RET  asn1r_get_identifier (ASN1_DEC_CTXT *ac, ST_CHAR *dest);
ST_RET  asn1r_get_objid (ASN1_DEC_CTXT *ac, ST_INT16 component_list[], ST_INT *num_components);
ST_RET  asn1r_get_bool  (ASN1_DEC_CTXT *ac, ST_BOOLEAN *dest);
ST_VOID asn1r_get_strn  (ASN1_DEC_CTXT *ac, ST_CHAR *dest);
ST_RET  asn1r_get_nstr  (ASN1_DEC_CTXT *ac, ST_CHAR *dest);
ST_RET  asn1r_get_pstr  (ASN1_DEC_CTXT *ac, ST_CHAR *dest);
ST_RET  asn1r_get_vstr  (ASN1_DEC_CTXT *ac, ST_CHAR *dest);
ST_RET  asn1r_get_vstr_maxlen (ASN1_DEC_CTXT *aCtx, ST_CHAR *ptr, ST_INT max_len);
ST_RET  asn1r_get_i8    (ASN1_DEC_CTXT *ac, ST_INT8 *dest);
ST_RET  asn1r_get_i16   (ASN1_DEC_CTXT *ac, ST_INT16 *dest);
ST_RET  asn1r_get_i32   (ASN1_DEC_CTXT *ac, ST_INT32 *dest);
ST_RET  asn1r_get_u8    (ASN1_DEC_CTXT *ac, ST_UCHAR *dest);
ST_RET  asn1r_get_u16   (ASN1_DEC_CTXT *ac, ST_UINT16 *dest);
ST_RET  asn1r_get_u32   (ASN1_DEC_CTXT *ac, ST_UINT32 *dest);
ST_RET  asn1r_get_btod  (ASN1_DEC_CTXT *ac, MMS_BTOD *dest);
ST_RET  asn1r_get_time  (ASN1_DEC_CTXT *ac, time_t *dest);
ST_RET  asn1r_get_bitstr (ASN1_DEC_CTXT *ac, ST_UCHAR *dest, ST_INT max_bits);
ST_VOID asn1r_get_bitstr_cstr (ASN1_DEC_CTXT *ac, ST_INT bits, ST_UCHAR *ptr);
ST_RET  asn1r_get_octstr (ASN1_DEC_CTXT *ac, ST_UCHAR *dest);
ST_RET  asn1r_get_octstr_fixlen (ASN1_DEC_CTXT *aCtx, ST_UCHAR *ptr, ST_INT len);
ST_RET  asn1r_get_octstr_varlen (ASN1_DEC_CTXT *aCtx, ST_UCHAR *ptr, ST_INT maxlen, ST_INT *lenout);
ST_VOID asn1r_get_octstr_cstr (ASN1_DEC_CTXT *ac, ST_INT numoctets, ST_UCHAR *octptr);
ST_VOID asn1r_get_delmnt (ASN1_DEC_CTXT *ac, ST_INT buflen, ST_UCHAR *bufptr, 
			  ST_VOID (*done_fun)(ASN1_DEC_CTXT *ac));
ST_RET  asn1r_get_float  (ASN1_DEC_CTXT *ac, ST_FLOAT *dest);
ST_RET  asn1r_get_double (ASN1_DEC_CTXT *ac, ST_DOUBLE *dest);
ST_RET  asn1r_get_utc_time (ASN1_DEC_CTXT *ac, MMS_UTC_TIME *dest);
ST_RET  asn1r_get_utf8 (ASN1_DEC_CTXT *ac, ST_CHAR *ptr, ST_INT el_len);

#ifdef INT64_SUPPORT
ST_VOID asn1r_wr_i64    (ASN1_ENC_CTXT *ac, ST_INT64 data);
ST_VOID asn1r_wr_u64    (ASN1_ENC_CTXT *ac, ST_UINT64 data);
ST_RET  asn1r_get_i64   (ASN1_DEC_CTXT *ac, ST_INT64 *dest);
ST_RET  asn1r_get_u64   (ASN1_DEC_CTXT *ac, ST_UINT64 *dest);
#endif	/* INT64_SUPPORT	*/

ST_VOID asn1r_parse_next (ASN1_DEC_CTXT *ac, ST_VOID (*done_fun)(ASN1_DEC_CTXT *ac));
ST_VOID asn1r_parse_cstr_contents (ASN1_DEC_CTXT *ac, ST_VOID (*done_fun)(ASN1_DEC_CTXT *ac));

ST_RET asn1r_skip_elmnt (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/* Functions for simplified decode scheme (no function pointers).	*/
/************************************************************************/
ST_RET asn1r_decode_init (ASN1_DEC_CTXT *ac,
	ST_UCHAR *ptr,		/* ASN.1 message to decode	*/
	ST_INT len);		/* length of ASN.1 message	*/
ST_RET asn1r_get_tag (ASN1_DEC_CTXT *ac,
	ST_UINT16 *tag_out);	/* out: decoded tag		*/

/* Structure used by "asn1r_chk_tag" function.	*/
#define TAG_OPTIONAL	0	/* use in TAG_INFO	*/
#define TAG_MANDATORY	1	/* use in TAG_INFO	*/
typedef struct
  {
  ST_UINT16 tag_num;	/* expected tag (compare decoded tag to this)	*/
  ST_BOOLEAN mandatory;	/* TAG_MANDATORY or TAG_OPTIONAL		*/
  } TAG_INFO;

ST_RET asn1r_chk_tag (
	ST_UINT16 tag_num,	/* tag to check				*/
				/* returned by "asn1r_get_tag"		*/
	TAG_INFO *tag_arr,	/* array of expected tags		*/
	ST_INT tag_count,	/* num elem in tag_arr			*/
	ST_INT *p_tag_idx);	/* in/out: next in tag_arr to check	*/
ST_RET asn1r_skip_contents (ASN1_DEC_CTXT *aCtx);
ST_RET asn1r_get_contents (ASN1_DEC_CTXT *aCtx,
	ST_UCHAR **contents_ptr,	/* out: ptr to contents	*/
	ST_INT *contents_len);		/* out: len of contents	*/


/************************************************************************/
/* These macros simplify access to members of the ASN1_ENC_CTXT struct.	*/
/************************************************************************/
/* This macro returns a pointer to the start of the ASN.1 encoded data.	*/
#define ASN1_ENC_PTR(aCtx)	((aCtx)->asn1r_field_ptr+1)
/* This macro returns the length (in bytes) of the ASN.1 encoded data.	*/
#define ASN1_ENC_LEN(aCtx)	((aCtx)->asn1r_buf_end - (aCtx)->asn1r_field_ptr)

/* This macro returns a pointer to the start of the ASN.1 DECODED data.	*/
#define ASN1_DEC_PTR(aCtx)	((aCtx)->asn1r_field_ptr)
/* This macro returns the length (in bytes) of the ASN.1 DECODED data.	*/
#define ASN1_DEC_LEN(aCtx)	((aCtx)->asn1r_elmnt_len)

/************************************************************************/
/* OBJECT IDENTIFIER							*/
/************************************************************************/

#define MAX_OBJID_COMPONENTS 	16

struct mms_obj_id
  {
  ST_INT num_comps;			/* number of objid components	*/
  ST_INT16 comps[MAX_OBJID_COMPONENTS];	/* identifier components	*/
  SD_END_STRUCT
  };
typedef struct mms_obj_id MMS_OBJ_ID;
ST_BOOLEAN asn1_objidcmp (MMS_OBJ_ID *obj1, MMS_OBJ_ID *obj2);

/************************************************************************/
/* Application Reference data structure					*/
/************************************************************************/

#define APP_REF_FORM1	0
#define APP_REF_FORM2	1

struct form1
  {
  ST_UCHAR   *ptr;				/* ptr to something		*/
  ST_INT16  len;				/* length of contents at ptr	*/
  };
typedef struct form1 FORM1_T;

struct app_ref
  {
  ST_INT16 	form;			/* tells if form 1 or form 2	*/
  ST_BOOLEAN    ap_title_pres;		/* tells if AP title is there	*/
  union					/* AP title differs in form	*/
    {
    FORM1_T		form_1;		/* form 1 of AP title is ASN.1	*/
    MMS_OBJ_ID		form_2;		/* form 2 of AP title is objectid*/
    } ap_title;
  ST_BOOLEAN	ap_invoke_pres;		/* tells if AP invoke is there	*/
  ST_INT32 	ap_invoke;
  ST_BOOLEAN	ae_qual_pres;		/* tells if AE qualifier is there*/
  union					/* AE qualifier differs in form	*/
    {
    FORM1_T	form_1;			/* form 1 of AE qual is ASN.1	*/
    ST_INT32	form_2;			/* form 2 if AE qual is long word*/
    } ae_qual;
  ST_BOOLEAN	ae_invoke_pres;		/* tells if AE invoke is there	*/
  ST_INT32 	ae_invoke;
  };
typedef struct app_ref APP_REF;

/************************************************************************/
/* Miscellaneous functions/macros.					*/
/************************************************************************/
/* Functions to copy bitstrings and "variable length bitstrings".	*/
ST_VOID bstrcpy (ST_UCHAR *dstptr, ST_UCHAR *srcptr, ST_INT numbits);
ST_VOID bvstrcpy (MMS_BVSTRING *dstptr, MMS_BVSTRING *srcptr);
ST_INT bstrcmp (ST_UCHAR *dstptr, ST_UCHAR *srcptr, ST_INT numbits);

/* Macros to access each individual bit of any bitstring.		*/
#define BSTR_BIT_SET_ON(ptr,bitnum) \
	( ((ST_UINT8 *)(ptr))[(bitnum)/8] |= (0x80>>((bitnum)&7)) )
#define BSTR_BIT_SET_OFF(ptr,bitnum) \
	( ((ST_UINT8 *)(ptr))[(bitnum)/8] &= ~(0x80>>((bitnum)&7)) )

/* BSTR_BIT_GET returns 0 if bit is clear, 1 if bit is set.	*/
#define BSTR_BIT_GET(ptr,bitnum) \
	(( ((ST_UINT8 *)(ptr))[(bitnum)/8] &  (0x80>>((bitnum)&7)) ) ? 1:0)

#define BSTR_BIT_SET_TRUE	BSTR_BIT_SET_ON
#define BSTR_BIT_SET_FALSE	BSTR_BIT_SET_OFF

/* Calculate number of bytes needed to contain number of bits.	*/
#define BSTR_NUMBITS_TO_NUMBYTES(numbits)	(((numbits)+7)/8)

/* Functions for conversion from ASN.1 UTF8string to local format.	*/
/* Must be ported to each platform.					*/
ST_INT asn1r_utf8_to_local (ST_CHAR *dst, ST_INT dst_len, ST_CHAR *src, ST_INT src_len);
ST_INT asn1r_local_to_utf8 (ST_CHAR *dst, ST_INT dst_len, ST_CHAR *src);

#ifdef __cplusplus
}
#endif

/************************************************************************/
#endif  /* File not already included */
/************************************************************************/

