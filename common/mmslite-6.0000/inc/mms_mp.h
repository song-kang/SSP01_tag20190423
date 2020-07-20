/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_mp.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the common data structures and general function	*/
/*	declarations required to interface with MMS at the primitive	*/
/*	level.								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     05	   Convert to use ASN1R.			*/
/* 08/15/97  MDE     04    BTOD handling changes			*/
/* 08/14/97  RKR     03    Format changes				*/
/* 06/11/97  RKR     02    Added constants for EXTERNAL encoding tag	*/
/* 06/09/97  MDE     01    MMS-LITE define changes			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_MP_INCLUDED
#define MMS_MP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "asn1defs.h"
#ifndef MMS_LITE
#include "mms_defs.h"
#endif

/************************************************************************/
/* MMS-LITE specific defines						*/

#if defined(MMS_LITE)
/* For MMS-LITE, we will use 'lite' versions of some data structures	*/
#define USE_COMPACT_MMS_STRUCTS

/* For MMS-LITE, we will supply a single static decode info buffer	*/
#define M_USR_DEC_BUF
#endif

/************************************************************************/
/* Common default values for priority and severity			*/
/************************************************************************/

#define MMS_NORM_PRIORITY		 64
#define MMS_NORM_SEVERITY		 64

/************************************************************************/
/* Object Name - This structure is used to reference MMS objects	*/
/************************************************************************/

/* These defines may be used for the object_tag element selector	*/
#define VMD_SPEC	0
#define DOM_SPEC	1
#define AA_SPEC		2

#if !defined (USE_COMPACT_MMS_STRUCTS)

/* Standard form of object name */
struct object_name
  {
  ST_INT  object_tag;			/* See above for legal values	*/
  union
    {
    ST_CHAR vmd_spec [MAX_IDENT_LEN+1];	/* vmd specific identifier	*/
    ST_CHAR item_id [MAX_IDENT_LEN+1];	/* item identifier		*/
    ST_CHAR aa_spec [MAX_IDENT_LEN+1];	/* aa specific identifier	*/
    } obj_name;
  ST_CHAR domain_id [MAX_IDENT_LEN+1];	/* domain identifier		*/
  SD_END_STRUCT
  };

#else		/* Use compact form */

/* Compact form of object name */
struct object_name
  {
  ST_INT  object_tag;			/* See above for legal values	*/
  union
    {
    ST_CHAR *vmd_spec;			/* vmd specific identifier	*/
    ST_CHAR *item_id;			/* item identifier		*/
    ST_CHAR *aa_spec;			/* aa specific identifier	*/
    } obj_name;
  ST_CHAR *domain_id;			/* domain identifier		*/
  SD_END_STRUCT
  };				   
#endif

typedef struct object_name OBJECT_NAME;


/************************************************************************/
/* timeofday, event_time, attach_to_evcon, attach_to_semaphore and 	*/
/* modifier structure definitions.					*/
/************************************************************************/

struct event_time
  {
  ST_INT16      evtime_tag;		/* event time tag		*/
					/*  0: time of day		*/
					/*  1: time sequence identifier */
					/*  2: undefined		*/
  union
    {
    MMS_BTOD    time_of_day;		/* time of day			*/
    ST_UINT32   time_seq_id; 		/* time sequence identifier	*/
    } evtime;
  };
typedef struct event_time EVENT_TIME;

struct attach_to_evcon
  {
  OBJECT_NAME evenroll_name;		/* event enrollment name       	*/
  OBJECT_NAME evcon_name;		/* event condition name       	*/
  ST_UCHAR    causing_transitions;	/* causing transitions:		*/
  					/* bitstring			*/
					/*  0 : idle to disabled	*/
					/*  1 : active to disabled	*/
					/*  2 : disabled to idle	*/
					/*  3 : active to idle		*/
					/*  4 : disabled to active	*/
					/*  5 : idle to active		*/
					/*  6 : any to deleted		*/
  ST_BOOLEAN  acc_delay_pres;		/* acceptable delay present	*/
  ST_UINT32   acc_delay;		/* acceptable delay		*/
  SD_END_STRUCT
  };
typedef struct attach_to_evcon ATTACH_TO_EVCON;

struct attach_to_semaphore
  {	  
  OBJECT_NAME sem_name;			/* semaphore name       	*/
  ST_BOOLEAN  named_token_pres;		/* named token present ind	*/
  ST_CHAR     named_token[MAX_IDENT_LEN+1];/* named token		*/
  ST_UCHAR    priority;			/* priority			*/
					/*   0 - highest		*/
					/*  64 - normal (default)	*/
					/* 127 - lowest 		*/
  ST_BOOLEAN  acc_delay_pres;		/* acceptable delay present	*/
  ST_UINT32   acc_delay;		/* acceptable delay		*/
  ST_BOOLEAN  ctrl_timeout_pres;	/* control timeout present	*/
  ST_UINT32   ctrl_timeout;	  	/* control timeout		*/
  ST_BOOLEAN  abrt_on_timeout_pres;	/* Abort On Timeout present	*/
  ST_BOOLEAN  abrt_on_timeout;		/* Abort On Timeout		*/
  ST_BOOLEAN  rel_conn_lost;		/* relinquish if connection	*/
  SD_END_STRUCT
  };					/* lost (default = true)	*/
typedef struct attach_to_semaphore ATTACH_TO_SEMAPHORE;

struct modifier
  {
  ST_INT modifier_tag;			/* modifier tag			*/
					/*  0:attach to event condition	*/
					/*  1:attach to semaphore	*/
  union
    {
    ATTACH_TO_EVCON     atec;		/* attach to event condition	*/
    ATTACH_TO_SEMAPHORE atsem;		/* attach to semaphore		*/
    } mod;
  SD_END_STRUCT
  };
typedef struct modifier	MODIFIER;

/************************************************************************/
/* File Name								*/
/************************************************************************/

struct file_name
  {
  ST_INT  fn_len; 		/* The length of the string		*/
  ST_CHAR *fname;  		/* pointer to actual filename  string	*/
  SD_END_STRUCT
  };
typedef struct file_name FILE_NAME;

/************************************************************************/
/* Values for the encoding_tag of an EXTERN_INFO			*/
/************************************************************************/

#define M_EXT_TAG_ANY			0
#define M_EXT_TAG_OCT_ALIGNED		1
#define M_EXT_TAG_ARB			2

/************************************************************************/
/* EXTERNAL								*/
/*   The structure below is used to store ASN.1 type EXTERNAL in "C"	*/
/*   format.								*/
/************************************************************************/

struct extern_info
  {
  ST_BOOLEAN dir_ref_pres;		/* direct reference present	*/
  MMS_OBJ_ID dir_ref;

  ST_BOOLEAN indir_ref_pres;		/* indirect reference present	*/
  ST_INT32   indir_ref;			/* indirect reference value	*/

  ST_BOOLEAN dv_descr_pres;		/* data value descript pres	*/
  ST_INT     dv_descr_len;   		/* data value descr. len	*/
  ST_UCHAR   *dv_descr;			/* data value descr. ptr	*/

  ST_CHAR    encoding_tag;		/* type of encoding		*/
  ST_INT     num_bits;			/* for arbitrary only		*/
  ST_INT     data_len;			/* # bytes of data		*/
  ST_UCHAR   *data_ptr;			/* data pointer			*/
  SD_END_STRUCT
  };
typedef struct extern_info EXTERN_INFO;


ST_RET ms_encode_extern (EXTERN_INFO *src, 
			 ST_UCHAR *asn1dest, ST_INT destlen,
			 ST_INT *len_out, ST_UCHAR **asn1_start_out);

ST_RET ms_decode_extern (ST_UCHAR *asn1, ST_INT asn1len, EXTERN_INFO *dest);

ST_VOID wr_external (ASN1_ENC_CTXT *aCtx, EXTERN_INFO *ext);


#ifdef __cplusplus
}
#endif

#endif  /* MMS_MP_INCLUDED  */
