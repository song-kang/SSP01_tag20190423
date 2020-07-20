/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2006, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_vvar.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains data and function definitions associated	*/
/*	with the MMS virtual machine variable access operations.	*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/30/12  JRB	   Del unneeded ms_rt_compute_prim_count.	*/
/* 01/11/12  JRB	   Chg prim_count to UINT16 to support big vars.*/
/* 03/16/11  JRB	   ms_rt_bld_finish: chg return to ST_RET.	*/
/*			   Add ms_rt_compute_prim_count.		*/
/* 05/29/09  JRB     59    Add ms_asn1_to_local_2.			*/
/* 08/02/07  JRB     58    Add elements to RUNTIME_BUILD_CTXT to allow	*/
/*			   2 pass build process. Add more ms_rt_bld*.	*/
/* 05/10/07  JRB     57    Chg el_size, etc. to ST_INT to avoid warnings*/
/* 04/30/07  JRB     56    Chg el_size, el_len, etc. in RUNTIME_TYPE to	*/
/*			   ST_INT32 to allow larger primitives, arrays.	*/
/* 04/17/07  JRB     55    Add prim_count to MVLU_TYPE_INFO.		*/
/* 10/30/06  JRB     54    Add reserved_1 to RUNTIME_CTRL.		*/
/*			   Add "ms_rt_bld_*" prototypes.		*/
/* 08/09/06  JRB     53    Add ms_local_to_asn1_2.			*/
/*			   Add ms_adl_to_asn1_2, ms_aa_to_asn1_2.	*/
/* 03/22/06  EJV     52	   Added GENERAL_TIME_ALGN (time_t is 64-bits in VS 2005)*/
/* 07/29/05  MDE     51	   Select USE_RT_TYPE_3 for USPS_IOS		*/
/* 05/09/05  JRB     50    Default to USE_RT_TYPE_2 (was USE_RT_TYPE_3).*/
/* 03/10/05  JRB     49    Add ms_rt_el_tag_text proto.			*/
/* 02/22/05  JRB     48    ms_is_rt_prim: add SD_CONST to arg.		*/
/* 01/27/05  JRB     47    Add m_lite_data_algn_tbl extern.		*/
/* 07/22/04  JRB     46    Add rt_index_q, rt_index_t, data_offset to	*/
/*			   RUNTIME_TYPE if !MMS_LITE.			*/
/* 07/12/04  JRB     45    Chg RT_UTF8_STRING tag from 18 to 16 to	*/
/*			   match latest 61850-8-1.			*/ 
/* 01/22/04  JRB     44    Add ms_local_to_text.			*/
/* 10/03/03  JRB     43    Chg offset_to_last & offSet to ST_INT32 in	*/
/*			   RUNTIME_TYPE to handle BIG IEC-61850 types.	*/
/* 03/31/03  JRB     42    Add UTF8string support (see RT_UTF8_STRING).	*/
/*			   Add "utf8" to M_ARB_DATA_CTRL.		*/
/* 03/13/03  JRB     41    u_ml_get_rt_type: Chg from "func ptr" to func.*/
/* 01/02/03  JRB     40    Add arg to u_ml_get_rt_type.			*/
/*			   Add ASN1_TO_RT_DEC_INFO struct.		*/
/* 12/27/02  MDE     39	   Allow use of ms_asn1_to_runtime		*/
/* 12/17/02  JRB     38    Del SD_CONST from numMvlRtNames.		*/
/* 12/16/02  JRB     37    Chg ms_is_rt_prim from macro to function.	*/
/* 11/27/02  MDE     36    Added ms_is_rt_prim				*/
/* 11/11/02  JRB     35    Add RUNTIME_CTRL, chg ms_runtime_create &	*/
/*			   ms_runtime_destroy to use it.		*/
/*			   Del ms_asn1_to_runtime (use ms_runtime_create)*/
/*			   Add ms_tdl_to_runtime proto.			*/
/* 03/01/02  JRB     34    Add SD_CONST to (RUNTIME_TYPE *) in RT_AA_CTRL.*/
/* 02/06/02  JRB     33    Deleted unused MMS_ALTA_DATA			*/
/* 12/20/01  JRB     32    Convert to use ASN1R.			*/
/*			   Add ASN1_ENC_CTXT arg to ms_*_to_asn1*.	*/
/*			   Add new function ms_local_to_asn1_easy.	*/
/* 11/13/01  EJV     31    Added support for new MMS type UtcTime:	*/
/*			   Added RT_UTC_TIME, chg RT_STR_END, RT_ARR_END*/
/*			   M_ARB_DATA_CTRL: added utc fun proto.	*/
/* 06/20/01  EJV     30    Added () around rt in ms_comp_name_xxx macros*/
/*			   to handle rt+1, rt-1 arguments.		*/
/* 04/11/01  MDE     29    Added ms_comp_name_pres macros		*/
/* 01/21/01  EJV     28    M_ARB_DATA_CTRL: changed bool to booln,	*/
/*			   to avoid conflict w/ bool type in C++ files.	*/
/* 11/02/00  JRB     27    Del #if MVL_UCA. Use #if MMS_LITE instead.	*/
/* 10/25/00  JRB     26    Del MVL_UCA define (define in makefile).	*/
/*			   Move ST_RTREF typedef so always defined.	*/
/* 08/29/00  JRB     25    Chg ms_comp_name_find from funct to macro.	*/
/* 07/28/00  JRB     24    Simplify RT_TYPE ifdefs.			*/
/*			   Fix SD_CONST in some prototypes.		*/
/*			   Del m_type_nam* externs, only used in 1 file.*/
/* 07/13/00  JRB     23    Add "#ifdef USE_RT_TYPE_2".			*/
/*			   Add ms_runtime_create, ms_runtime_destroy,	*/
/*			     ms_comp_name_find.				*/
/* 06/22/00  MDE     22    Added numMvlRtNames declaration		*/
/* 06/16/00  JRB     21    Add #if INT64_SUPP...			*/
/* 04/20/00  JRB     20    Del SD_CONST from (RUNTIME_TYPE **) args.	*/
/* 09/13/99  MDE     19    Added SD_CONST modifiers			*/
/* 09/10/98  MDE     18    Added 'mvlRtNames' declaration		*/
/* 08/11/98  MDE     17    Minor changes for Foundry			*/
/* 06/30/98  MDE     16    Changed ST_REF to ST_VOID *			*/
/* 04/07/98  MDE     15    Added struct/arr start/end alignment mode	*/
/* 03/23/98  MDE     14    Now let user supply AA derived type (RD/WR)	*/
/* 03/12/98  MDE     13    MMS-Lite related runtime type changes	*/
/* 03/11/98  MDE     12    Removed NEST_RT_TYPES			*/
/* 02/10/98  MDE     11    Changed runtime type				*/
/* 12/29/97  MDE     10    Added 'typeCtrl' to RUNTIME_TYPE for use	*/
/* 			   with 'mktypes.exe' only.			*/
/* 09/11/97  MDE     09    MMS ARB user funs now return ST_RET		*/
/* 09/04/97  NAV     08    Split ARB Btime functions & add 		*/
/*			     have them return a ST_BOOLEAN		*/
/* 08/15/97  MDE     07    BTOD handling comments			*/
/* 08/14/97  RKR     06    Format changes				*/
/* 07/16/97  RKR     05    Added more 64 bit integer support		*/
/* 06/23/97  RKR     04    Added arbitrary data handling		*/
/* 06/20/97  MDE     03    Added 64 bit integer support			*/
/* 06/09/97  MDE     02    Changed Runtime Type and it's use		*/
/* 06/05/97  RKR     01    Removed AA_OUTER_NEST rearranged AA constants*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_VVAR_INCLUDED
#define MMS_VVAR_INCLUDED

#ifndef MMS_LITE
#include "mms_mv.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************/
/************************************************************************/
/* BINARY TIME OF DAY NOTES						*/
/* There are two forms of TimeOfDay, 4 byte and 6 byte.			*/
/* The 6 byte form contains the number of milliseconds since midnight	*/
/* and the number of days since Jan 1, 1984. The 4 byte form contains	*/
/* only the millisecond number. 					*/
/*									*/
/* In memory, the Btime6 data looks like a pair of 32 bit integers:	*/
/*   ST_INT32 ms;	      Number of milliseconds since midnight	*/
/*   ST_INT32 day;	      Number of days since Jan 1, 1984		*/
/*									*/
/* and the Btime4 data looks like a single 32 bit integer:		*/
/*   ST_INT32 ms;	      Number of milliseconds since midnight	*/
/*									*/
/* Data alignment for either form is that of a ST_INT32.		*/
/* Note that MMS-EASE simply encodes/decodes the values to/from the 	*/
/* MMS PDU and does not perform any local time localization.		*/
/************************************************************************/

/************************************************************************/
/************************************************************************/
/* MMS-LITE 								*/
/************************************************************************/

/* Default to USE_RT_TYPE_2 for MMS Lite (allows dynamic type creation).*/
/* NOTE: define USE_RT_TYPE_3 only if you need to store RUNTIME_TYPES	*/
/*   in ROM, and you DO NOT need dynamic type creation.			*/

#if defined(MMS_LITE) && (!defined(USE_RT_TYPE_3))
#define USE_RT_TYPE_2
#endif

/* The IOS Toolkit requires RT_TYPE_3 */
#ifdef USPS_IOS
#undef USE_RT_TYPE_2
#define USE_RT_TYPE_3
#endif

#if defined(FOUNDRY)	/* Overwride defines when compiling Foundry	*/
#undef USE_RT_TYPE_2
#undef USE_RT_TYPE_3
#endif

/* RUNTIME TYPE INTEGER FORM						*/
/* In MMS-LITE the size of the RUNTIME_TYPE must be minimized, so we	*/
/* will use 16 bit integers for the compoments. This puts a 32K limit	*/
/* on the size of a variable. 						*/

#if !defined(MMS_LITE)
#define ST_RTINT  ST_INT
#else
#define ST_RTINT  ST_INT16
#endif

/************************************************************************/
/* MMS-LITE UCA SPECIFIC SECTION					*/
/************************************************************************/
/* MMS-LITE uses an extended RUNTIME_TYPE for use with the MVL UCA 	*/
/* object handling extensions.						*/
/* To use the UCA extensions, MVL_UCA must be defined.			*/
/* NOTE: MVL_UCA is NOT defined by default.				*/

/* This typedef is for the application use only and can be changed as 	*/
/* required. Normally it is used to allow the applcation to consolidate	*/
/* MVL_UCA read and write indication handler functions.			*/
typedef ST_VOID *  ST_RTREF;

/* These elements are designed to be used to assist in implementing the */
/* UCA device models in MMS-EASE Lite. Note that 'MVL_UCA' must be 	*/
/* defined when creating the MMS-EASE libraries if these features are	*/
/* to be used.								*/

/* You may comment out the defines for those components that will not	*/
/* bs used in the target application to reduce the size of the 		*/
/* RUNTIME_TYPE table.							*/
#define MVLU_USE_REF

typedef struct mvluTypeStruct 
  {
  ST_UINT16 prim_count;	/* number of prims in this ARR, STRUCT, or prim	*/
  ST_RTINT sortedNum;	/* For use by MVLU GNL code 			*/
  ST_INT32 offSet;	/* Offset from start of mother structure 	*/
  ST_RTINT rdIndFunIndex;	
  ST_RTINT wrIndFunIndex;	

/* These elements can be used by the application as needed, and are	*/
/* copied into the derived variable association. MVLU does not use them	*/
/* directly.								*/
#if defined(MVLU_USE_REF)
  ST_RTREF ref;		/* set to 'XXX$YYY_REF' by foundry 		*/
#endif
  } MVLU_TYPE_INFO;

/************************************************************************/
/* RUNTIME TYPE DEFINITION STRUCTURE					*/
/************************************************************************/
/* The following defines specify the various data parameters		*/
/************************************************************************/

#define RT_ARR_START	        1
#define RT_STR_START	        2
#define	RT_BOOL		        3
#define	RT_BIT_STRING	        4
#define	RT_INTEGER		5
#define	RT_UNSIGNED	        6
#define	RT_FLOATING_POINT	7
#define	RT_OCTET_STRING	        9
#define	RT_VISIBLE_STRING	10
#define	RT_GENERAL_TIME	        11
#define	RT_BINARY_TIME	        12
#define	RT_BCD		        13
#define	RT_BOOLEANARRAY	        14
#define	RT_UTC_TIME	        17
#define	RT_UTF8_STRING		16	/* specified in IEC-61850-8-1	*/
/* ADD new tags here and bump up RT_MAX_PRIM_TAG accordingly.	*/
#define RT_MAX_PRIM_TAG		18	/* use when looking for max tag val*/
#define RT_STR_END 	        (RT_MAX_PRIM_TAG+1)
#define RT_ARR_END 	        (RT_MAX_PRIM_TAG+2)

struct runtime_type
  {
  ST_UCHAR el_tag; 	/* ID tag expected for this element, or special */
			/* values for end of array, end of struct	*/
			/* or nested runtime types			*/

  ST_INT   el_size;	/* number of bytes occupied by this element	*/
  ST_INT32   offset_to_last; /* offset in bytes from the start of the 	*/
                         /* data to the last element 		 	*/
  union
    {
    /* CRITICAL: element types in each structure in this union must	*/
    /* match. If not, the Foundry would misalign data in this union.	*/
    struct		 /* primitive element				*/
      {
      ST_INT el_len;	 /* precision, in bytes except bitstr, time	*/
      ST_RTINT pad;	 /* included to allow aggregate initialization	*/
      } p;
    struct		 /* structure (top or bottom)			*/
      {
      ST_INT num_rt_blks; /* # runtime blks to get to strt/end elmnt	*/
      ST_RTINT pad;	 /* included to allow aggregate initialization	*/
#if !defined(USE_RT_TYPE_2) && !defined(USE_RT_TYPE_3)
      ST_BOOLEAN packd;	 /* Indicates if packed or not.			*/
#endif
      } str;
    struct		 /* Array (top or bottom)			*/
      {
      ST_INT num_elmnts;  /* # array elements (index limit)		*/
      ST_RTINT num_rt_blks; /* # runtime blks to get to strt/end elmnt	*/
#if !defined(USE_RT_TYPE_2) && !defined(USE_RT_TYPE_3)
      ST_BOOLEAN packd;	 /* Indicates if packed or not.			*/
#endif
      } arr;
    } u;

#if defined(USE_RT_TYPE_2)
  ST_CHAR *comp_name_ptr;		/* ptr to comp name string	*/
#elif defined(USE_RT_TYPE_3)
  ST_RTINT name_index;	 		/* Index into mvlRtNames table	*/
#else					/* Default			*/
  ST_CHAR name[MAX_IDENT_LEN+1];
#endif

#if defined(MMS_LITE)
  MVLU_TYPE_INFO mvluTypeInfo;	    	/* MMS-LITE UCA handler use	*/
#endif
#if defined(FOUNDRY)
  ST_CHAR *refBuf;
  ST_CHAR *rdIndexBuf;
  ST_CHAR *wrIndexBuf;
#endif
#if !defined (MMS_LITE)
  ST_RTINT rt_index_q;		/* index into RUNTIME_TYPE array for 'q'*/
				/* attribute of IEC-61850 type		*/
  ST_RTINT rt_index_t;		/* index into RUNTIME_TYPE array for 't'*/
				/* attribute of IEC-61850 type		*/
  ST_INT32 data_offset;		/* offset in bytes from start of data	*/
				/* NOTE: should be same as offSet in	*/
				/*      mvluTypeInfo above for MMS_LITE	*/
#endif
  };
typedef struct runtime_type RUNTIME_TYPE;

/* Runtime type HEAD struct contains info about RUNTIME_TYPE array.	*/
typedef struct
  {
  RUNTIME_TYPE *rt_first;	/* ptr to first in array of structs	*/
  ST_INT rt_num;		/* num entries in RUNTIME_TYPE array	*/
  ST_VOID *reserved_1;		/* ptr to any optional info for this type*/
  } RUNTIME_CTRL;

/************************************************************************/
/* ASN1_TO_RT_DEC_INFO - structure to store temporary decode info.	*/
/************************************************************************/
typedef struct
  {
#if defined(USE_RT_TYPE_2)
/* These are used for saving component names.				*/
  ST_CHAR *comp_name_buf;	/* Alloc'd buf to store ALL component names.*/
  ST_INT   comp_name_size;	/* Buffer size (in bytes)		*/
  ST_INT   comp_name_used;	/* Running count of bytes needed OR used.*/

  ST_CHAR *comp_name_ptr;	/* current position in buffer.		*/
#endif
  RUNTIME_TYPE  *rt_start;	/* ptr to runtime table start	*/
  ST_BOOLEAN calc_only;		/* flag to calculate num rt blocks	*/
  /* DEBUG: add rt_ptr, rt_count, rt_limit, etc. & delete global vars.*/
  } ASN1_TO_RT_DEC_INFO;

/************************************************************************/
/* RUNTIME_BUILD_CTXT: info for dynamic building of runtime type defs.	*/	
/************************************************************************/
typedef struct
  {
  RUNTIME_CTRL *rt_ctrl;	/* type definition being built		*/
  RUNTIME_TYPE *rt_type;	/* ptr to current type			*/
  RUNTIME_TYPE *max_rt_type;	/* ptr after last RUNTIME_TYPE struct	*/
  ST_INT nest_level;		/* index into "nest_start_ptr" array	*/
  RUNTIME_TYPE *nest_start_ptr [ASN1_MAX_LEVEL];	/* array of ptrs to*/
				/* STR_START or ARR_START		*/
  ST_CHAR *comp_name_buf;	/* allocated buf to store comp names	*/
				/* [MAX_IDENT_LEN+1] for each		*/
  ST_RET errcode;		/* Set when first error detected	*/
				/* Initialized to SD_SUCCESS.		*/

  ST_BOOLEAN do_count;		/* If SD_TRUE, bld functions just count	*/
				/* num of RUNTIME_TYPE elements	needed	*/
				/* (nothing allocated or initialized)	*/
  ST_INT curr_count;		/* current count of RUNTIME_TYPE elems	*/
				/* (used only when do_count=SD_TRUE)	*/
  ST_INT max_count;		/* max count of RUNTIME_TYPE elems	*/
				/* (used only when do_count=SD_TRUE)	*/
  } RUNTIME_BUILD_CTXT;

#if defined(MMS_LITE)
ST_RET u_ml_get_rt_type (ASN1_DEC_CTXT *aCtx, OBJECT_NAME *type_name, 
      			   RUNTIME_TYPE **rt_out, ST_INT *num_rt_out);

extern ST_INT maxMvlRtNames;
extern ST_INT numMvlRtNames;
extern ST_CHAR **mvlRtNames;
#endif

ST_BOOLEAN ms_is_rt_prim (SD_CONST RUNTIME_TYPE *rt);
ST_CHAR *ms_rt_el_tag_text (SD_CONST RUNTIME_TYPE *rt_type);

/************************************************************************/
/* ALTERNATE ACCESS DEFINITION STRUCTURE  				*/
/************************************************************************/

/* Possible values for sel_type                                         */
#define AA_COMP		        0
#define AA_INDEX 	        1
#define AA_INDEX_RANGE	        2
#define AA_ALL	                3
#define AA_COMP_NEST		4
#define AA_INDEX_NEST 	        5
#define AA_INDEX_RANGE_NEST	6
#define AA_ALL_NEST	        7
#define AA_END_NEST             8

#define MAX_NUM_AA_EL		100

struct alt_acc_el
  {
  ST_BOOLEAN comp_name_pres;
  ST_CHAR    comp_name[MAX_IDENT_LEN+1];/* named component for derived 	*/
					/* type				*/
  ST_INT sel_type;			/* type of access selection	*/
  union
    {
    ST_CHAR component[MAX_IDENT_LEN+1];	/* Used for AA_COMP		*/ 
    ST_UINT32 index;			/* Used for AA_INDEX		*/
    struct		  		/* Used for AA_INDEX_RANGE	*/
      {
      ST_UINT32 low_index;
      ST_UINT32 num_elmnts;
      } ir;
    } u;
  };
typedef struct alt_acc_el ALT_ACC_EL;

struct alt_access
  {
  ST_INT     num_aa;
  ALT_ACC_EL *aa;
  };
typedef struct alt_access ALT_ACCESS;


/* This data structure is used in table form to implement alternate	*/
/* access from the server perspective (NEWDATA <-> ASN1DATA)		*/
/* It allows re-ordering of elements as well as limited selection	*/

struct rt_aa_ctrl
  {
  SD_CONST RUNTIME_TYPE *rt;
  ST_UINT      offset_to_data;	/* offset to data element		*/
  ST_INT       el_size;		/* Size of the element in memory	*/
  };
typedef struct rt_aa_ctrl RT_AA_CTRL;

ST_RET ms_adl_to_aa (ST_CHAR *adl_str,ALT_ACCESS *alt_acc);
ST_RET ms_adl_to_asn1_2 (ST_CHAR *adl_str,
	ST_UCHAR *buf_ptr,
	ST_INT buf_len,
	ST_UCHAR **asn1_ptr_out,
	ST_INT *asn1_len_out);

ST_RET ms_aa_to_adl (ALT_ACCESS *alt_acc, 
                     ST_CHAR *adl_str, 
		     ST_INT max_adl_len);

ST_RET ms_local_to_asn1_aa (ASN1_ENC_CTXT *aCtx, SD_CONST RUNTIME_TYPE *rt_head,
			    ST_INT rt_num,
                            ALT_ACCESS *alt_acc,
                            ST_CHAR *dptr);

ST_RET ms_asn1_to_local_aa (SD_CONST RUNTIME_TYPE *rt_head,
                            ST_INT rt_num,
                            ALT_ACCESS *alt_acc,
                            ST_UCHAR *asn1ptr,
                            ST_INT asn1len,
                            ST_CHAR *dptr);


ST_VOID ms_log_alt_access (ALT_ACCESS *alt_acc);


ST_RET ms_aa_to_asn1 (ASN1_ENC_CTXT *aCtx, ALT_ACCESS *alt_acc);
/* ms_aa_to_asn1_2 usually easier to use than ms_aa_to_asn1.		*/
ST_RET ms_aa_to_asn1_2 (ALT_ACCESS *alt_acc,
	ST_UCHAR *buf_ptr,
	ST_INT buf_len,
	ST_UCHAR **asn1_ptr_out,
	ST_INT *asn1_len_out);
ST_RET ms_asn1_to_aa (ST_UCHAR *asn1ptr, ST_INT asn1len, 
                      ALT_ACCESS *alt_acc_out);


/************************************************************************/
#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/* Type definition structures and manipulation functions		*/
/************************************************************************/

struct named_type		/* type definition table structure	*/
  {
  DBL_LNK      link;		/* generic list links			*/
  ST_CHAR type_name[MAX_IDENT_LEN+1]; 	/* name of data type		*/
  ST_BOOLEAN   deletable;	/* set != 0 if deletable		*/
  ST_UCHAR     protection;	/* protection afforded for type		*/
  ST_BOOLEAN   erased; 		/* flags if deleted, but nref !=0	*/
  ST_INT       nref;		/* number of var def'ns referencing this*/
  RUNTIME_CTRL *rt_ctrl;	/* May be NULL				*/
  RUNTIME_TYPE *rt_head; 	/* ptr to runtime type def head 	*/
  ST_INT       rt_num; 		/* # of rt_blocks in runtime type def	*/
  ST_INT       asn1len;		/* # bytes in ASN.1 definition		*/
  ST_UCHAR     *asn1ptr;	/* pointer to ASN.1 definition		*/
  ST_INT       blocked_len;	/* length of blocked (packed) data type */
  ST_CHAR *(*read_ind_fun)(ST_CHAR *src, ST_INT len);  /* std read ind function ptr */
  ST_INT (*write_ind_fun)(ST_CHAR *src, ST_CHAR *dest, ST_INT len); /* std write ind	*/
  };
typedef struct named_type NAMED_TYPE;

ST_RET ms_locl_to_asn1_aa (ASN1_ENC_CTXT *aCtx, NAMED_TYPE *tptr, 
                          ALT_ACCESS *alt_acc,
			  ST_CHAR *dptr);

ST_RET ms_asn1_to_locl_aa (NAMED_TYPE *tptr,
                          ALT_ACCESS *alt_acc,
                          ST_UCHAR *asn1ptr,
                          ST_INT asn1len,
                          ST_CHAR *dptr);

extern ST_INT max_mmsease_types;	   /* max number allowed	*/
extern ST_INT mms_type_count;		   /* number currently defined	*/

NAMED_TYPE *ms_add_named_type (DOMAIN_OBJS *dom, ST_CHAR *name,
				ST_UCHAR *asn1, ST_INT asn1len);
NAMED_TYPE *ms_find_named_type_obj (OBJECT_NAME *obj, 
					ST_INT chan);
NAMED_TYPE *ms_find_named_type (DOMAIN_OBJS *dom,ST_CHAR *name);
ST_RET ms_del_named_type (DOMAIN_OBJS *dom,ST_CHAR *name);
ST_RET ms_del_type (DOMAIN_OBJS *dom, NAMED_TYPE *name);
ST_VOID  ms_del_all_named_types (DOMAIN_OBJS *dom_objs);
ST_RET ms_del_named_type_obj (OBJECT_NAME *obj, ST_INT chan);
ST_RET ms_add_std_types (DOMAIN_OBJS *dom);

ST_CHAR *rdind_fun (ST_CHAR *addr, ST_INT len);
ST_RET wrind_fun (ST_CHAR *data, ST_CHAR *addr, ST_INT len);

ST_RET mv_gettype_resp (MMSREQ_IND *ind);
ST_RET mv_gettype_resp (MMSREQ_IND *ind);
ST_RET mv_deltype_resp (MMSREQ_IND *ind);
ST_RET mv_deftype_resp (MMSREQ_IND *ind);


/************************************************************************/
/* Variable definition structures and manipulation functions		*/
/************************************************************************/

struct	named_var			/* name table structure 	*/
  {
  DBL_LNK      link;			/* generic list links		*/
  ST_CHAR      varname[MAX_IDENT_LEN+1];/* variable name		*/
  ST_BOOLEAN   invalid;			/* variable invalidated due	*/
					/* to type or addr deleted	*/
  NAMED_TYPE   *type;		   	/* type name/domain		*/
  ST_BOOLEAN   deletable;		/* != 0 if deletable 		*/
  ST_UCHAR     rd_pro;			/* read protection		*/
  ST_UCHAR     wr_pro;			/* write protection		*/
  VAR_ACC_ADDR addr;		   	/* address			*/
  ST_CHAR *(*read_ind_fun)(ST_CHAR *src, ST_INT len);  /* std read ind function ptr */
  ST_INT (*write_ind_fun)(ST_CHAR *src, ST_CHAR *dest, ST_INT len); /* std write ind	*/
/* Followed by the address, for symbolic or unconstrained address      */
/* types. The length of this address is determined when the var  added */
  SD_END_STRUCT
  };
typedef struct named_var NAMED_VAR;

struct	named_var_list		/* name variable list structure		*/
  {					
  DBL_LNK    link;			/* generic list links		*/
  ST_CHAR    name[MAX_IDENT_LEN+1];	/* variable list name		*/
  ST_BOOLEAN deletable;			/* != 0 if deletable 		*/
  ST_INT     num_vars;		/* # of vars in Named Variable List.	*/
  /* VARIABLE_LIST var_list [num_of_variables];				*/ 
  SD_END_STRUCT
  };
typedef struct named_var_list NAMED_VAR_LIST;

extern ST_INT  max_mmsease_vars;	/* max number allowed		*/
extern ST_INT  mms_var_count;		/* number currently defined	*/
extern ST_INT  max_mmsease_nvlists;
extern ST_INT  mms_nvlist_count;

/************************************************************************/
/* This function selects the binary search mechansisms (default)	*/
ST_VOID ms_bs_init (ST_VOID);

/* This function selects the linked list search mechansisms		*/
ST_VOID ms_ll_init (ST_VOID);

NAMED_VAR *ms_add_named_var (DOMAIN_OBJS *dom, ST_CHAR *name,
	                     OBJECT_NAME *type, VAR_ACC_ADDR *addr, 
			     ST_INT chan);
NAMED_VAR *ms_add_var (DOMAIN_OBJS *dom, ST_CHAR *name,
	               NAMED_TYPE *type, VAR_ACC_ADDR *addr, ST_INT chan);

NAMED_VAR *ms_find_named_var (DOMAIN_OBJS *dom, ST_CHAR *name);
NAMED_VAR *ms_find_named_var_obj (OBJECT_NAME *obj, ST_INT chan);

ST_RET ms_del_named_var (DOMAIN_OBJS *dom, ST_CHAR *name);
ST_RET ms_del_named_var_obj (OBJECT_NAME *obj, ST_INT chan);

ST_VOID  ms_del_all_named_vars    (DOMAIN_OBJS *dom);

NAMED_VAR_LIST *ms_add_nvlist (DOMAIN_OBJS *dom, ST_CHAR *name, 
                               ST_INT num_vars, VARIABLE_LIST *var_list);
NAMED_VAR_LIST *ms_find_nvlist (DOMAIN_OBJS *dom, ST_CHAR *name);
NAMED_VAR_LIST *ms_find_nvlist_obj (OBJECT_NAME *obj, ST_INT chan);

ST_RET ms_del_nvlist (DOMAIN_OBJS *dom, ST_CHAR *name);
ST_RET ms_del_nvlist_obj (OBJECT_NAME *obj, ST_INT chan);

ST_VOID  ms_del_all_nvlists (DOMAIN_OBJS *dom);

ST_RET mv_getvar_resp (MMSREQ_IND *ind);
ST_RET mv_delvar_resp (MMSREQ_IND *ind);
ST_RET mv_defvar_resp (MMSREQ_IND *ind);
ST_RET mv_namelist_resp	(MMSREQ_IND *ind);

ST_RET mv_getvlist_resp (MMSREQ_IND *ind);
ST_RET mv_delvlist_resp (MMSREQ_IND *ind);
ST_RET mv_defvlist_resp (MMSREQ_IND *ind);

/************************************************************************/
/* VIRTUAL MACHINE READ, WRITE, INFORMATION REPORT			*/
/* The structure below are used for most variable access VM req funs,	*/
/* and allows multiple variables per transaction.			*/
/************************************************************************/
/* VM defines to be used for DataAccessError codes		      	*/
#define ERROR_RESPONSE		20	/* for error response only 	*/
#define NUM_VAR_MISMATCH	21	


struct mv_vardesc 		/* virtual machine variable description	*/
  {
  OBJECT_NAME name;			/* name of variable		*/
  OBJECT_NAME type;			/* type of variable		*/
  ST_CHAR     *data;			/* destination/source pointer	*/
  };
typedef struct mv_vardesc MV_VARDESC;

/* This structure is provided as request information for a MV read req.	*/
/* If desired, this info may be examined in the user confirm function.	*/

struct readvars_req_info
  {				/* virtual machine read req structure	*/
  ST_INT  num_of_vars;			/* number of variables		*/
/*					   list of variables		*/
/* MV_VARDESC vardesc_list [num_of_vars];				*/
  SD_END_STRUCT
  };
typedef struct readvars_req_info READVARS_REQ_INFO;


/***** READ REQUEST ****						*/

/* a pointer to an array of these structures are passed into the 	*/
/* 'mv_read_variables' function						*/

struct mv_read_req_info
  {
  struct			/* input (request) information		*/
    {
    ST_CHAR    *data_ptr;	/* Dest data buffer (for success)	*/
    NAMED_TYPE *type;	 	/* pointer to associated type		*/
    ST_BOOLEAN alt_acc_pres;	/* set SD_TRUE if Alt Access is present	*/
    ST_BOOLEAN alt_acc_type_aa;	/* set SD_TRUE if type is Alt Acc ready	*/
    ST_BOOLEAN alt_acc_data_packed; /* set SD_TRUE if local data is packed	*/
    ALT_ACCESS alt_acc; 	/* Alternate Access specification	*/
    }i;
  struct			/* output (result) information		*/
    {
    ST_RET   result;     	/* !=0 for success, else SD_FAILURE	*/
    ST_INT16 err_code;		/* DataAccessError (for failure)	*/

/* If a named type is not supplied, and the read was a success, this	*/
/* is the derived runtime type. It was allocated using 'chk_calloc'	*/
/* and must be free'd using 'chk_free'					*/

    ST_INT       num_rt;		
    RUNTIME_TYPE *rt_out;	

/* If a data_ptr is not supplied, and the read was a success, this	*/
/* is the buffer allocated for the data. It was allocated using 	*/
/* 'chk_calloc'	and must be free'd using 'chk_free'			*/

    ST_CHAR      *data_ptr_out;	 /* Dest data buffer (for success)	*/
    }o;
  };
typedef struct mv_read_req_info	MV_READ_REQ_INFO;

MMSREQ_PEND *mv_read (ST_INT chan, OBJECT_NAME *name, 
		      OBJECT_NAME *type, ST_CHAR *dest);
MMSREQ_PEND *mv_readvars (ST_INT chan, ST_INT num_var, MV_VARDESC *info);
MMSREQ_PEND *mv_read_variables (ST_INT chan, ST_INT num_var, 
				READ_REQ_INFO *prim, 
				MV_READ_REQ_INFO *vminfo);

/***** READ RESPONSE  *****						*/

/* a pointer to an array of these structures are passed into the 	*/
/* 'mv_read_response' function						*/

struct mv_read_resp_info
  {
  ST_INT16   result; 		/* ACC_RSLT_SUCCESS, ACC_RSLT_FAILURE	*/
  ST_INT16   err_code;		/* DataAccessError (for failure)	*/
  ST_CHAR    *data_ptr;		/* Source data buffer (for success)	*/
  NAMED_TYPE *type;		/* pointer to associated type		*/
  ST_BOOLEAN alt_acc_pres;	/* set SD_TRUE if Alt Access is present	*/
  ST_BOOLEAN alt_acc_data_packed;/* set SD_TRUE if local data is packed	*/
  ALT_ACCESS alt_acc; 		/* Alternate Access specification	*/
  };
typedef struct mv_read_resp_info MV_READ_RESP_INFO;

ST_RET mv_read_resp (MMSREQ_IND *ind);
ST_RET mv_read_response (MMSREQ_IND *ind, ST_INT num_var, 
			 MV_READ_RESP_INFO *info);

/***** WRITE REQUEST  ****						*/

/* a pointer to an array of these structures are passed into the 	*/
/* 'mv_write_variables' function					*/

struct mv_write_req_info
  {
  ST_CHAR    *data_ptr;		/* Source data buffer 			*/
  NAMED_TYPE *type;		/* pointer to associated type		*/
  ST_BOOLEAN alt_acc_pres;	/* set SD_TRUE if Alt Access is present	*/
  ST_BOOLEAN alt_acc_type_aa;	/* set SD_TRUE if type is Alt Acc ready	*/
  ST_BOOLEAN alt_acc_data_packed;/* set SD_TRUE if local data is packed	*/
  ALT_ACCESS alt_acc; 		/* Alternate Access specification	*/
  };
typedef struct mv_write_req_info MV_WRITE_REQ_INFO;

MMSREQ_PEND *mv_write (ST_INT chan, OBJECT_NAME *var, 
	               OBJECT_NAME *type, ST_CHAR *src);
MMSREQ_PEND *mv_writevars (ST_INT chan, ST_INT num_var, MV_VARDESC *info);
MMSREQ_PEND *mv_write_variables (ST_INT chan, ST_INT num_data, 
				 VAR_ACC_SPEC *reqinfo,
				 MV_WRITE_REQ_INFO *vminfo);

/***** WRITE RESPONSE ****						*/

ST_RET mv_write_resp (MMSREQ_IND *ind);
WRITE_RESP_INFO *mv_write_decode (MMSREQ_IND *indptr);
ST_RET ms_extract_write_data (MMSREQ_IND *ind, ST_INT index, 
			      NAMED_TYPE *type,
			      ST_CHAR *dest, ST_BOOLEAN alt_acc_packed);

ST_RET ms_extract_wr_data (MMSREQ_IND *indptr, ST_INT i,
			   NAMED_TYPE *type, ST_CHAR *dest, 
			   ST_BOOLEAN alt_acc_packed, ALT_ACCESS *alt_acc_ptr);

/***** INFORMATION REPORT REQUEST *****					*/

/* a pointer to an array of these structures are passed into the 	*/
/* 'mv_info_req' function						*/

struct mv_info_req_info
  {
  ST_CHAR    *data_ptr;		/* Source data buffer 			*/
  NAMED_TYPE *type;		/* pointer to associated type		*/
  ST_BOOLEAN alt_acc_pres;	/* set SD_TRUE if Alt Access is present	*/
  ST_BOOLEAN alt_acc_data_packed;/* set SD_TRUE if local data is packed	*/
  ALT_ACCESS alt_acc; 		/* Alternate Access specification	*/
  };
typedef struct mv_info_req_info	MV_INFO_REQ_INFO;

ST_RET mv_info (ST_INT chan, OBJECT_NAME *var, OBJECT_NAME *type, 
                ST_CHAR *src);
ST_RET mv_infovars (ST_INT chan, ST_INT num_vars, MV_VARDESC *vminfo);
ST_RET mv_info_report (ST_INT chan, VAR_ACC_SPEC *req,
		       ST_INT num_var, MV_INFO_REQ_INFO *vminfo);
ST_RET mv_info_nvlist (ST_INT chan, OBJECT_NAME *obj);

ST_RET ms_extract_info_data (MMSREQ_IND *ind, ST_INT index, 
	                     NAMED_TYPE *type, ST_CHAR *dest, 
			     ST_BOOLEAN alt_acc_packed);

ST_RET ms_extract_inf_data (MMSREQ_IND *indptr, ST_INT i,
			    NAMED_TYPE *type, ST_CHAR *dest, 
			    ST_BOOLEAN alt_acc_packed, 
			    ALT_ACCESS *alt_acc_ptr);

/************************************************************************/

OBJECT_NAME *ms_extract_varname (ST_CHAR *reqinfo, ST_INT op, ST_INT indx);

/************************************************************************/
/************************************************************************/
#endif /* end of MMS_LITE not defined					*/
/************************************************************************/
/************************************************************************/

#define MMS_EASE_BUILDER	0
#define UNKNOWN_BUILDER		1

extern ST_INT ms_req_bld_id;
extern ST_INT ms_resp_bld_id;

ST_RET ms_init_va_size (ST_INT chan, ST_UCHAR spec_in_rslt, 
			ST_INT16 var_acc_tag, OBJECT_NAME *vl_name, 
			ST_LONG *req_size_out, ST_LONG *resp_size_out);

#ifndef MMS_LITE
ST_RET ms_va_size (ST_INT op, ST_UCHAR spec_in_rslt, ST_INT16 var_acc_tag,
		   VARIABLE_LIST *vl, NAMED_TYPE *type,
		   ST_LONG *req_size_out, ST_LONG *resp_size_out);
#else
ST_RET ms_va_size (ST_INT op, ST_UCHAR spec_in_rslt, ST_INT16 var_acc_tag,
	VARIABLE_LIST *vl_ptr, ST_INT type_id,
	ST_LONG *req_size, ST_LONG *resp_size);
#endif

/************************************************************************/
/************************************************************************/

extern ST_INT m_rt_type_limit;      /* max num of runtime blks per def 	*/


#ifndef MMS_LITE
ST_INT ms_get_blocked_len (NAMED_TYPE *type);
#endif

ST_INT ms_get_blocked_length (SD_CONST RUNTIME_TYPE *rt, ST_INT num_rt);

ST_RET ms_runtime_to_asn1 (ASN1_ENC_CTXT *aCtx, SD_CONST RUNTIME_TYPE *rt, ST_INT num_rt);

ST_UCHAR *ms_mk_asn1_type (ST_INT *asn1_len, ST_UCHAR *asn1_buf, 
			   ST_CHAR *tdl);

ST_VOID ms_log_runtime (SD_CONST RUNTIME_TYPE *rt, ST_INT num_rt);

ST_CHAR *ms_asn1_to_tdl (ST_UCHAR *asn1_ptr, ST_INT asn1_len, 
			 ST_INT max_tdl_len);
ST_INT ms_runtime_to_tdl (SD_CONST RUNTIME_TYPE *rt, ST_INT rt_num,
			  ST_CHAR *tdl_buf, ST_INT max_tdl_len);

RUNTIME_CTRL *ms_tdl_to_runtime (ST_CHAR *tdl,	/* TDL string	*/
			ST_UCHAR *asn1_buf,	/* ASN.1 buffer	*/
			ST_INT asn1_buf_len);	/* buffer len	*/

ST_RET ms_rt_to_typedef (ST_CHAR *typedefName, SD_CONST RUNTIME_TYPE *rt,
				ST_INT numRt, ST_CHAR *dest, ST_INT destSize);

RUNTIME_CTRL *ms_runtime_create (ST_UCHAR *asn1_data,
			ST_UINT asn1_len,
			ST_INT max_num_rt);
ST_VOID ms_runtime_destroy (RUNTIME_CTRL *rt_head);

#if !defined(MMS_LITE)
/* For backward compatibility only.					*/
/* New programs should use ms_runtime_create.				*/

ST_INT ms_asn1_to_runtime_x (ST_UCHAR *asn1_data, ST_UINT asn1_len,
   		            RUNTIME_TYPE *rt_table, ST_INT rt_len, 
   		            ST_BOOLEAN calc_only);

#define ms_asn1_to_runtime(asn1,asn1_len,rt,num_rt)\
                   ms_asn1_to_runtime_x(asn1,asn1_len,rt, num_rt,m_calc_rt_size)

/* Note: This variable is no longer used internally by MMS-EASE, but 	*/
/* can be used in the application space for the following: 		*/
/*   1. Is used in the macro 'ms_asn1_to_runtime' for the 		*/
/*	'ms_asn1_to_runtime_x' calc_only parameter. This is done to 	*/
/* 	maintain backward compatibility, and users are encouraged to 	*/
/* 	use 'ms_runtime_create' instead.				*/
extern ST_BOOLEAN m_calc_rt_size; /* flag to calculate num rt blocks 	*/
				  /* in ms_asn1_to_runtime		*/
#endif /* !MMS_LITE	*/

/************************************************************************/
/*			ms_comp_name_find				*/
/* These macros find the component name in a runtime type structure.	*/
/* There are different macros for different RUNTIME_TYPE structures.	*/
/* Macro argument = ptr to RUNTIME_TYPE struct.				*/
/* RETURN: pointer to component name string				*/
/************************************************************************/
#if defined(USE_RT_TYPE_2)

/* rt->comp_name_ptr = pointer to string	*/
/* NOTE: ptr may be NULL. If so return empty string (i.e. "") instead	*/
/*       of NULL. Caller might not handle NULL.				*/
#define ms_comp_name_find(rt)	((rt)->comp_name_ptr ? (rt)->comp_name_ptr : "")
#define ms_comp_name_pres(rt)	((rt)->comp_name_ptr ? SD_TRUE : SD_FALSE)

#elif defined(USE_RT_TYPE_3)

/* rt->name_index = index to global array of strings "mvlRtNames"	*/
#define ms_comp_name_find(rt)	(mvlRtNames[(rt)->name_index])
#define ms_comp_name_pres(rt)	((rt)->name_index > 0 ? SD_TRUE : SD_FALSE)

#else	/* Default	*/

/* rt->name = character array.	*/
#define ms_comp_name_find(rt)	((rt)->name)
#define ms_comp_name_pres(rt)	((rt)->name[0] != 0 ? SD_TRUE : SD_FALSE)

#endif	/* Default	*/


/************************************************************************/
/* VIRTUAL MACHINE ASN.1 TO?FROM LOCAL REPRESENTATION FUNCTIONS		*/
/* These functions are used to translate ASN.1 encoded data to/from	*/
/* local format								*/
/************************************************************************/

#ifndef MMS_LITE
ST_RET ms_asn1_to_locl (NAMED_TYPE *type, 
			ST_UCHAR *asn1, ST_INT asn1_len, ST_CHAR *dest);
ST_RET ms_locl_to_asn1 (ASN1_ENC_CTXT *aCtx, NAMED_TYPE *type, ST_CHAR *src);
#endif

/* These 2 are Re-entrant functions. Based on ASN1R tools.	*/
ST_RET ms_asn1_to_local (SD_CONST RUNTIME_TYPE *rt, ST_INT num_rt,
			ST_UCHAR *asn1, ST_INT asn1_len, ST_CHAR *dest);
ST_RET ms_local_to_asn1 (ASN1_ENC_CTXT *aCtx,
			SD_CONST RUNTIME_TYPE *rt_head,
			ST_INT rt_num,
			ST_CHAR *dptr);
/* "ms_local_to_asn1_2" is almost the same as "ms_local_to_asn1_easy"	*/
/* but its arguments are more consistent with other encoding functions.	*/
ST_RET ms_local_to_asn1_2 (SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
			ST_CHAR *dptr,
			ST_UCHAR *buf_ptr,
			ST_INT buf_len,
			ST_UCHAR **asn1_ptr_out,
			ST_INT *asn1_len_out);
ST_UCHAR *ms_local_to_asn1_easy (SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
			ST_CHAR *dptr,
                        ST_UCHAR *asn1Buf,
                        ST_INT asn1BufLen,
                        ST_INT *asn1LenOut);
ST_CHAR *ms_local_to_text (ST_CHAR *datptr, SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
	ST_CHAR *textBuf, ST_UINT textBufSize);

/* Alternate "MMS Data" decode function (faster, no function pointers).	*/
ST_RET ms_asn1_to_local_2 (SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
	ST_UCHAR *asn1_ptr,		/* ASN.1 message to decode	*/
	ST_INT asn1_len,		/* length of ASN.1 message	*/
	ST_CHAR *data_dest);		/* decoded data destination	*/

/* Index definitions into the alignment table				*/
#define ARRSTRT_ALGN	 0
#define ARREND_ALGN	 1
#define STRSTRT_ALGN	 2
#define STREND_ALGN	 3
#define INT8_ALGN	 4
#define INT16_ALGN	 5
#define INT32_ALGN	 6
#define INT64_ALGN	 7
#define FLOAT_ALGN	 8
#define DOUBLE_ALGN	 9
#define OCT_ALGN	10
#define BOOL_ALGN	11
#define BCD1_ALGN	12
#define BCD2_ALGN	13
#define BCD4_ALGN	14
#define BIT_ALGN	15
#define VIS_ALGN	16

#define NUM_ALGN_TYPES 17

/* define alignment for Generalized Time (in VS 2005 time_t is 64-bits)		*/
#define GENERAL_TIME_ALGN	(sizeof(time_t) == 8 ? INT64_ALGN : INT32_ALGN)


extern SD_CONST ST_INT *m_data_algn_tbl;
extern SD_CONST ST_INT m_packed_data_algn_tbl[NUM_ALGN_TYPES];
extern SD_CONST ST_INT m_def_data_algn_tbl[NUM_ALGN_TYPES];
extern ST_INT m_lite_data_algn_tbl[NUM_ALGN_TYPES];	/* created by Foundry	*/

#define M_STRSTART_MODE_NONE 	0
#define M_STRSTART_MODE_FIRST 	1
#define M_STRSTART_MODE_LARGEST	2
extern ST_INT m_struct_start_algn_mode;

#define M_STREND_MODE_NONE 	0
#define M_STREND_MODE_LARGEST	1
extern ST_INT m_struct_end_algn_mode;

/* User defined runtime type post processing function			*/
extern ST_VOID (*u_rt_type_process) (SD_CONST RUNTIME_TYPE *rt, ST_INT num_rt);


/************************************************************************/
/************************************************************************/

/* For encoding and decoding 'alternateAccess', the outer constructor	*/
/* usually has a value of 5. If a different value is required (for 	*/
/* scattered access, for instance), this value can be modified.		*/
/* Used in both encoding and decode of alternateAccess			*/
extern ST_INT16 m_alt_acc_outer_tag;

/* When decoding an ASN.1 alternateAccess, this variable determines the */
/* size of the table that is allocated for the decoded AA (and so 	*/
/* limits the complexity of the alternate access that can be handled	*/
/* The default value is 50.						*/
extern ST_INT m_max_dec_aa;

/* This variable is the 'high water' mark of how many AA were actually	*/
/* required. This can be used to tune memory usage if desired.		*/
extern ST_INT m_hw_dec_aa;

/* When performing ASN1 <-> local data format, the runtime type is 	*/
/* 'unrolled', and one element is required for each data element and	*/
/* structure/array start/end. This variable determines the size of the 	*/
/* of the table that is allocated for this operation (and so limits the */
/* size and complexity of the data types that can be handled.		*/
/* The default value is 1000.						*/
extern ST_INT m_max_rt_aa_ctrl;

/* This variable is the 'high water' mark of how many elements were	*/
/* actually required for m_max_rt_aa_ctrl. This can be used to tune 	*/
/* memory usage if desired.						*/
extern ST_INT m_hw_rt_aa_ctrl;

/* This variable is used to determine whether the local data format is 	*/
/* in 'packed type' or 'original type' form. Used in all conversions 	*/
/* to/from local data form when the AA_DATA library is used. 		*/
/* Temporarily set to SD_FALSE  for the non-AA data conversion functions.	*/
extern ST_BOOLEAN m_alt_acc_packed;

/************************************************************************/

extern ST_RET (*ms_asn1_data_to_runtime_fun) (RUNTIME_TYPE **tptr,
					      ST_INT *t_len,
					      ST_UCHAR *asn1ptr,
					      ST_INT asn1_len);

ST_RET ms_asn1_data_to_runtime (RUNTIME_TYPE **tptr,
				ST_INT *t_len,
				ST_UCHAR *asn1ptr,
				ST_INT asn1_len);

ST_RET ms_asn1_data_to_locl (ST_UCHAR *asn1_data,	   
		  	     ST_INT asn1_data_len, 
			     ST_VOID **data_dest,
			     ST_INT *data_dest_len,
			     RUNTIME_TYPE **rt,     
			     ST_INT *t_len);	   

ST_VOID ms_rt_size_calc (RUNTIME_TYPE *rt_head, ST_INT rt_num);

/************************************************************************/
/* ARBITRARY DATA HANDLING 						*/
/************************************************************************/

typedef struct m_arb_data_ctrl
  {
  ST_RET (*arrStart) (RT_AA_CTRL *rtaa);
  ST_RET (*arrEnd)   (RT_AA_CTRL *rtaa);
  ST_RET (*strStart) (RT_AA_CTRL *rtaa);
  ST_RET (*strEnd)   (RT_AA_CTRL *rtaa);
  ST_RET (*int8)     (ST_INT8    *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*int16)    (ST_INT16   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*int32)    (ST_INT32   *data_dest, RT_AA_CTRL *rtaa);
#ifdef INT64_SUPPORT
  ST_RET (*int64)    (ST_INT64   *data_dest, RT_AA_CTRL *rtaa);
#endif
  ST_RET (*uint8)    (ST_UINT8   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*uint16)   (ST_UINT16  *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*uint32)   (ST_UINT32  *data_dest, RT_AA_CTRL *rtaa);
#ifdef INT64_SUPPORT
  ST_RET (*uint64)   (ST_UINT64  *data_dest, RT_AA_CTRL *rtaa);
#endif
  ST_RET (*flt)      (ST_FLOAT   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*dbl)      (ST_DOUBLE  *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*oct)      (ST_UCHAR   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*booln)    (ST_BOOLEAN *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*bcd1)     (ST_INT8    *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*bcd2)     (ST_INT16   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*bcd4)     (ST_INT32   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*bs)       (ST_UCHAR   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*vis)      (ST_CHAR    *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*bt4)      (ST_INT32   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*bt6)      (ST_INT32   *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*gt)       (time_t     *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*utc)      (MMS_UTC_TIME *data_dest, RT_AA_CTRL *rtaa);
  ST_RET (*utf8)     (ST_UCHAR   *data_dest, RT_AA_CTRL *rtaa);
  } M_ARB_DATA_CTRL;

ST_RET ms_process_arb_data (ST_CHAR *data_base, 
			    SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
		       	    ST_BOOLEAN alt_acc_pres, ST_BOOLEAN alt_acc_packed, 
			    ALT_ACCESS *alt_acc,
		            M_ARB_DATA_CTRL *ac);

/************************************************************************/
/* Functions for building a type definition one component at a time.	*/
/************************************************************************/
ST_VOID ms_rt_bld_start (RUNTIME_BUILD_CTXT *rt_ctxt, ST_INT max_rt_num);
ST_VOID ms_rt_bld_cancel (RUNTIME_BUILD_CTXT *rt_ctxt);
ST_RET ms_rt_bld_finish (RUNTIME_BUILD_CTXT *rt_ctxt);
ST_RET ms_rt_bld_add_str_start (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name);
ST_RET ms_rt_bld_add_str_end (RUNTIME_BUILD_CTXT *ctxt);
ST_RET ms_rt_bld_add_arr_start (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT array_count);
ST_RET ms_rt_bld_add_arr_end (RUNTIME_BUILD_CTXT *ctxt);
ST_RET ms_rt_bld_add_bool (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name);
ST_RET ms_rt_bld_add_int (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_uint (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_float (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_bstring (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_bvstring (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_vstring (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_utctime (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name);
ST_RET ms_rt_bld_add_btime6 (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name);
ST_RET ms_rt_bld_add_ostring (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_ovstring (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_utf8vstring (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, ST_INT el_len);
ST_RET ms_rt_bld_add_special (RUNTIME_BUILD_CTXT *ctxt, ST_CHAR *comp_name, RUNTIME_TYPE *special);
ST_RET ms_rt_bld_remove_last (RUNTIME_BUILD_CTXT *ctxt);
ST_RET ms_rt_bld_chk_state (RUNTIME_BUILD_CTXT *ctxt);
ST_INT ms_rt_bld_get_count (RUNTIME_BUILD_CTXT *ctxt);
ST_INT ms_rt_bld_get_max_count (RUNTIME_BUILD_CTXT *ctxt);

#ifdef __cplusplus
}
#endif

#endif

