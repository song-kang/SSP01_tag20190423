#ifndef TP4_ENCD_INCLUDED
#define TP4_ENCD_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996 - 2005, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : tp4_encd.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file defines structures and functions for	*/
/*		encoding/decoding of class 4 TPDUs.			*/
/*									*/
/*	Note:	The following TP options are NOT supported:		*/
/*			Transport Expedited Data			*/
/*			Non-use of checksum				*/
/*			Use of extended format.				*/
/*		This is reflected in structures and defines below.	*/
/*									*/
/*  For information see the:						*/
/*	ISO 8073 "Information processing systems - Open Systems		*/
/*	Interconnections - Connection oriented transport protocol	*/
/*	specification.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/22/08  JRB    08     tp_decode_ak(_dc,_er): change args.		*/
/*			   Delete tp_decode_rj (not used in TP0 or TP4).*/
/* 11/01/07  EJV    07     Added comments.				*/
/* 05/25/05  EJV    06     Added include tp4.h				*/
/* 01/28/99  JRB    05     Move cltp_decode_nsdu proto to ACSE file.	*/
/* 12/01/97  JRB    04     CRITICAL: Don't use "sizeof" on structures.	*/
/*			   Caused padding on VXWORKS.			*/
/* 08/14/97  JRB    03     Add TP0/RFC1006 support defines & protos.	*/
/*			   Add tp_decode_* protos (were static before).	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 11/22/96  JRB    02     Add TP_PDU_TYPE_UD & cltp_decode_nsdu for CLTP.*/
/* 06/20/96  EJV    01     Created					*/
/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "tp4.h"

	/* encoding for Transport Protocol class and options */

#define TP_CLASS_0		(ST_UCHAR) 0x00
#define TP_CLASS_1		(ST_UCHAR) 0x10
#define TP_CLASS_2		(ST_UCHAR) 0x20
#define TP_CLASS_3		(ST_UCHAR) 0x30
#define TP_CLASS_4		(ST_UCHAR) 0x40

#define TP_OPT_FORMAT_NORMAL	(ST_UCHAR) 0x00
#define TP_OPT_FORMAT_EXTENDED	(ST_UCHAR) 0x02

#define TP_CLASS_4_NORMAL	(ST_UCHAR) (TP_CLASS_4 | TP_OPT_FORMAT_NORMAL)
#define TP_CLASS_4_EXT		(ST_UCHAR) (TP_CLASS_4 | TP_OPT_FORMAT_EXTENDED)

#define TP_CLASS_MASK_OPT	(ST_UCHAR) 0xF0	/* for masking opt in ASN1_CLASS_METHOD-OPT in CR */

	/* encoding for TPDU type code */

#define TP_PDU_TYPE_CR	(ST_UCHAR) 0xE0	/* 1110 xxxx */
#define TP_PDU_TYPE_CC	(ST_UCHAR) 0xD0	/* 1101 xxxx */
#define TP_PDU_TYPE_DR	(ST_UCHAR) 0x80	/* 1000 0000 */
#define TP_PDU_TYPE_DC	(ST_UCHAR) 0xC0	/* 1100 0000 */
#define TP_PDU_TYPE_DT	(ST_UCHAR) 0xF0	/* 1111 0000 */
#define TP_PDU_TYPE_ED	(ST_UCHAR) 0x10	/* 0001 0000 */
#define TP_PDU_TYPE_AK	(ST_UCHAR) 0x60	/* 0110 zzzz */
#define TP_PDU_TYPE_EA	(ST_UCHAR) 0x20	/* 0010 0000 */
#define TP_PDU_TYPE_RJ	(ST_UCHAR) 0x50	/* 0101 zzzz */
#define TP_PDU_TYPE_ER	(ST_UCHAR) 0x70	/* 0111 0000 */
#define TP_PDU_TYPE_UD	(ST_UCHAR) 0x40	/* 0100 0000 for CLTP	*/

#define TP_PDU_MASK_TYPE (ST_UCHAR) 0x0F	/* for masking type to get CDT	*/
#define TP_PDU_MASK_CDT	 (ST_UCHAR) 0xF0	/* for masking CTD to get type	*/

/* encoding for max size of TPDU (parameter in the CR) */

#define TP_PDU_MAX_SIZE_65531	(ST_UCHAR) 0x0E	/* kluge for TP0/RFC1006 only*/
#define TP_PDU_MAX_SIZE_8192	(ST_UCHAR) 0x0D
#define TP_PDU_MAX_SIZE_4096	(ST_UCHAR) 0x0C
#define TP_PDU_MAX_SIZE_2048	(ST_UCHAR) 0x0B
#define TP_PDU_MAX_SIZE_1024	(ST_UCHAR) 0x0A
#define TP_PDU_MAX_SIZE_512	(ST_UCHAR) 0x09
#define TP_PDU_MAX_SIZE_256	(ST_UCHAR) 0x08
#define TP_PDU_MAX_SIZE_128	(ST_UCHAR) 0x07

/* encoding EOT in DT TPDU - bit 8 */

#define TP_PDU_EOF	(ST_UCHAR) 0x80

#define TP_PDU_MASK_EOF	(ST_UCHAR) 0x7F	/* for masking EOF to get SN (DT)*/
#define TP_PDU_MASK_SN	(ST_UCHAR) 0x80	/* for masking SN to get EOF (DT)*/

#define TP_PDU_MASK_BIT8 (ST_UCHAR) 0x7F	/* for masking bit 8 to get SN (AK)*/

/* encoding for DR reason */
/* for class 1 - 4	*/
#define TP_DR_NORMAL			(ST_UCHAR) 0x80
#define TP_DR_CONGESTION 		(ST_UCHAR) 0x81
#define TP_DR_CLASS_UNSUPPORTED		(ST_UCHAR) 0x82
#define TP_DR_DUPLICATE_SRC_REF		(ST_UCHAR) 0x83
#define TP_DR_INVALID_REF		(ST_UCHAR) 0x84
#define TP_DR_PROT_ERROR		(ST_UCHAR) 0x85
#define TP_DR_REF_OVERFLOW		(ST_UCHAR) 0x87
#define TP_DR_CR_REFUSED		(ST_UCHAR) 0x88
#define TP_DR_HDR_INVALID		(ST_UCHAR) 0x8A

/* for all classes (TP0) */
#define TP_DR_NO_REASON			(ST_UCHAR) 0x00
#define TP_DR_CONGESTION_AT_TSAP	(ST_UCHAR) 0x01
#define TP_DR_NO_SESSION		(ST_UCHAR) 0x02
#define TP_DR_ADDR_UNKNOWN		(ST_UCHAR) 0x03


/* misc defines */

#define	TP_MAX_CDT	(ST_UCHAR) 0x0F



	/*----------------------------------------------*/
	/* Encoding and structures for TP parameters	*/
	/*----------------------------------------------*/

	/* encoding for TP version */

#define TP_VERSION			(ST_UCHAR) 0x01

	/* encoding for the parameter code in CR or CC TPDU variable part */

#define TP_PAR_CODE_TSAP_CALLING	(ST_UCHAR) 0xC1
#define TP_PAR_CODE_TSAP_CALLED		(ST_UCHAR) 0xC2
#define TP_PAR_CODE_TPDU_SIZE		(ST_UCHAR) 0xC0
#define TP_PAR_CODE_VERSION		(ST_UCHAR) 0xC4
#define TP_PAR_CODE_ADD_OPTIONS		(ST_UCHAR) 0xC6

	/* encoding for the parameter code in AK TPDU variable part */

#define TP_PAR_CODE_AK_SUB_SN		(ST_UCHAR) 0x8A
#define TP_PAR_CODE_AK_FLOW_CTRL	(ST_UCHAR) 0x8C

	/* encoding for the parameter code in ER TPDU variable part	*/

#define TP_PAR_CODE_ER_INVAL_TPDU	(ST_UCHAR) 0xC1

	/* encoding for the parameter code in variable part (common to	*/
	/* different TPDU types)					*/

#define TP_PAR_CODE_CHECKSUM		(ST_UCHAR) 0xC3


	/* encoding for CR TPDU additional options */

#define TP_OPT_CHECKSUM_USE		(ST_UCHAR) 0x00
#define TP_OPT_CHECKSUM_NON_USE		(ST_UCHAR) 0x02

#define TP_OPT_EXPEDITED_NON_USE	(ST_UCHAR) 0x00
#define TP_OPT_EXPEDITED_USE		(ST_UCHAR) 0x01


typedef struct
  {
  ST_UCHAR		code;		/* parameter code			*/
  ST_UCHAR		len;		/* Parameter length			*/
  ST_UCHAR		value [2];	/* parameter value has variable length	*/
				/* (defined as 2 bytes buffer, the ptr	*/
				/* will allow us to access any len param*/
  }TP_PAR;

#define TP_PAR_FIX_LEN	2
			/* define above specifies only the fixed part	*/
			/* of parameter (code and len)			*/

	/* some parameters are fixed in size so we can define them here	*/

	/* parameters length in CR or CC				*/

#define TP_PAR_LEN_TPDU_SIZE	(TP_PAR_FIX_LEN + 1)
#define TP_PAR_LEN_VERSION	(TP_PAR_FIX_LEN + 1)
#define TP_PAR_LEN_ADD_OPTIONS	(TP_PAR_FIX_LEN + 1)

	/* parameters length in AK					*/

#define TP_PAR_LEN_AK_SUB_SN	(TP_PAR_FIX_LEN + 2)
#define TP_PAR_LEN_AK_FLOW_CTRL	(TP_PAR_FIX_LEN + 8)

	/* parameters length (common to different types of TPDU types)	*/

#define TP_PAR_LEN_CHECKSUM	(TP_PAR_FIX_LEN + 2)

/*	 !								*/
/* NOTE: ! only ST_UCHAR type should be in protocol structs to avoid	*/
/*	 ! packing or problems with sizeof operator.			*/
/*	 !								*/


	/*----------------------------------------------*/
	/* Struct and defines for CR and CC TPDU 	*/
	/*----------------------------------------------*/

typedef struct
  {
  /* fixed part of header */
  ST_UCHAR		hdr_len;  	/* hdr length (don't count the len byte)*/
  ST_UCHAR		type_cdt;	/* TPDU type and CDT			*/
  ST_UCHAR		dest_ref [2];	/* Remote connection id (set to 0)	*/
  ST_UCHAR		src_ref [2];	/* Local connection id			*/
  ST_UCHAR		class_opt;	/* Class and options			*/
  }TP_FHDR_CX;

#define TP_FHDR_CX_LEN	7

	/* for alloc of n_unitdata purposes the max len will be used */
					/* fixed part of header	*/
					/* local & remote TSAP	*/
					/* TPDU size parameter	*/
					/* TP version parameter	*/
					/* checksum parameter	*/
					/* additional opt param	*/
					/* user data (max allowed)*/
						
#define TP_MAX_LEN_CX	(TP_FHDR_CX_LEN + \
			 2*(TP_PAR_FIX_LEN + MAX_TSEL_LEN) + \
			 TP_PAR_LEN_TPDU_SIZE + \
			 TP_PAR_LEN_VERSION + \
			 TP_PAR_LEN_CHECKSUM + \
			 TP_PAR_LEN_ADD_OPTIONS + \
			 TP_MAX_CONN_UDATA)

	/*--------------------------------------*/
	/* Struct and defines for DR TPDU 	*/
	/*--------------------------------------*/

typedef struct
  {
  /* fixed part of header */
  ST_UCHAR		hdr_len;  	/* hdr length (don't count the len byte)*/
  ST_UCHAR		type;		/* TPDU type				*/
  ST_UCHAR		dest_ref [2];	/* Remote connection id (set to 0)	*/
  ST_UCHAR		src_ref [2];	/* Local connection id			*/
  ST_UCHAR		reason;		/* Reason code for disconnect		*/
  }TP_FHDR_DR;

#define TP_FHDR_DR_LEN	7

	/* for alloc of n_unitdata the max len will be used.	*/

#define TP_MAX_LEN_DR	(TP_FHDR_DR_LEN + TP_PAR_LEN_CHECKSUM)

	/* no additional info parameter or user data in DR */

	/*--------------------------------------*/
	/* Struct and defines for DC TPDU 	*/
	/*--------------------------------------*/

typedef struct
  {
  /* fixed part of header */
  ST_UCHAR		hdr_len;  	/* hdr length (don't count the len byte)*/
  ST_UCHAR		type;		/* TPDU type				*/
  ST_UCHAR		dest_ref [2];	/* Remote connection id (set to 0)	*/
  ST_UCHAR		src_ref [2];	/* Local connection id			*/
  }TP_FHDR_DC;

#define TP_FHDR_DC_LEN	6

	/* for alloc of n_unitdata the max len will be used.	*/

#define TP_MAX_LEN_DC	(TP_FHDR_DC_LEN + TP_PAR_LEN_CHECKSUM)


	/*--------------------------------------*/
	/* Struct and defines for DT TPDU 	*/
	/*--------------------------------------*/

typedef struct
  {
  ST_UCHAR		hdr_len;  	/* hdr length (don't count the len byte)*/
  ST_UCHAR		type;		/* TPDU type				*/
  ST_UCHAR		dest_ref [2];	/* Remote connection id			*/
  ST_UCHAR		eot_sn;		/* EOT flag and sequence number		*/
  }TP_FHDR_DT;

#define TP_FHDR_DT_LEN	5

	/* for alloc of n_unitdata use TP_HEAD_LEN_DT + udata_len	*/

#define TP_HEAD_LEN_DT	(ST_UINT16)(TP_FHDR_DT_LEN + TP_PAR_LEN_CHECKSUM)

/* TP0 DT same as TP4 except doesn't use DST-REF.	*/
typedef struct
  {
  ST_UCHAR		hdr_len;  	/* hdr length (don't count the len byte)*/
  ST_UCHAR		type;		/* TPDU type			*/
  ST_UCHAR		eot_sn;		/* EOT flag and sequence number	*/
  }TP0_FHDR_DT;

#define TP0_FHDR_DT_LEN	3
#define TP0_HEAD_LEN_DT		(ST_UINT16)(TP0_FHDR_DT_LEN)	/* no chksum*/


	/*--------------------------------------*/
	/* Struct and defines for AK TPDU 	*/
	/*--------------------------------------*/


typedef struct
  {
  ST_UCHAR		hdr_len;  	/* hdr length (don't count the len byte)*/
  ST_UCHAR		type_cdt; 	/* TPDU type and CDT			*/
  ST_UCHAR		dest_ref [2];	/* Remote connection id			*/
  ST_UCHAR		sn;		/* Next expected DT TPDU sequence number*/
  }TP_FHDR_AK;

#define TP_FHDR_AK_LEN	5

	/* for alloc of n_unitdata the max len will be used.	*/

#define TP_MAX_LEN_AK	(TP_FHDR_AK_LEN + TP_PAR_LEN_CHECKSUM)


	/*--------------------------------------*/
	/* Struct and defines for RJ TPDU 	*/
	/*--------------------------------------*/

typedef struct
  {
  ST_UCHAR		hdr_len;  	/* hdr length (don't count the len byte)*/
  ST_UCHAR		type_cdt; 	/* TPDU type and CDT			*/
  ST_UCHAR		dest_ref [2];	/* Remote connection id			*/
  ST_UCHAR		sn;		/* Next expected DT TPDU sequence number*/
  }TP_FHDR_RJ;

#define TP_FHDR_RJ_LEN	5

	/* for alloc of n_unitdata the max len will be used.	*/

#define TP_MAX_LEN_RJ	TP_FHDR_RJ_LEN

	/*--------------------------------------*/
	/* Struct and defines for ER TPDU 	*/
	/*--------------------------------------*/

typedef struct
  {
  ST_UCHAR		hdr_len;  	/* hdr length (don't count the len byte)*/
  ST_UCHAR		type; 		/* TPDU type				*/
  ST_UCHAR		dest_ref [2];	/* Remote connection id			*/
  ST_UCHAR		reason;		/* Code for reject couse		*/
  }TP_FHDR_ER;

#define TP_FHDR_ER_LEN	5

	/* for alloc of n_unitdata the max len will be used.	*/

#define TP_MAX_LEN_ER	(TP_FHDR_ER_LEN + TP_PAR_LEN_CHECKSUM)


	/*----------------------------------------------*/
	/*  Function prototypes for encoding/decoding	*/
	/*----------------------------------------------*/

ST_UINT16 tp_encode_cx (ST_UCHAR *enc_buf, TPDU_CX *tpdu_cx, ST_UCHAR pdu_type);
ST_UINT16 tp_encode_dr (ST_UCHAR *enc_buf, TPDU_DR *tpdu_dr);
ST_UINT16 tp_encode_dc (ST_UCHAR *enc_buf, TPDU_DC *tpdu_dc);
ST_UINT16 tp_encode_dt (ST_UCHAR *enc_buf, TPDU_DT *tpdu_dt);
ST_UINT16 tp0_encode_dt (ST_UCHAR *enc_buf, TPDU_DT *tpdu_dt);
ST_UINT16 tp_encode_ak (ST_UCHAR *enc_buf, TPDU_AK *tpdu_ak);

ST_RET tp_decode_nsdu (N_UNITDATA *n_unitdata);

ST_UINT16 tp_decode_cx (TPDU_CX *tpdu_cx_rx, ST_UCHAR *pdu_ptr,
              ST_UINT16 pdu_len, ST_UCHAR pdu_type, ST_UCHAR max_tpdu_len_enc);
ST_UINT16 tp_decode_dr (TPDU_DR *tpdu_dr_rx, ST_UCHAR *pdu_ptr,
              ST_UINT16 pdu_len);
ST_UINT16 tp_decode_dc (TPDU_DC *tpdu_dc_rx, ST_UCHAR *pdu_ptr,
	ST_UINT16 pdu_len);
ST_UINT16 tp_decode_dt (TPDU_DT *tpdu_dt_rx, ST_UCHAR *pdu_ptr,
              ST_UINT16 pdu_len);
ST_UINT16 tp0_decode_dt (TPDU_DT *tpdu_dt_rx, ST_UCHAR *pdu_ptr,
              ST_UINT16 pdu_len);
ST_UINT16 tp_decode_ak (TPDU_AK *tpdu_ak_rx, ST_UCHAR *pdu_ptr,
	ST_UINT16 pdu_len);
ST_UINT16 tp_decode_er (TPDU_ER *tpdu_er_rx, ST_UCHAR *pdu_ptr,
	ST_UINT16 pdu_len,
	ST_INT    tp_class);	/* must be 0 or 4	*/

ST_VOID    tp_log_tsdu (ST_VOID *tsdu, ST_UCHAR type, ST_UCHAR mask);

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/


