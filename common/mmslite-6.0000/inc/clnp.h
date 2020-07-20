/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp.h							*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file specifies internal defines and function*/
/*		prototypes for implementation of Connectionless-mode	*/
/*		Network Protocol (CLNP).				*/
/*		Note that the CLNP is implemented as non-segmenting	*/
/*		subset of the full protocol. 				*/
/*									*/
/*  For information see the:						*/
/*	ISO 8473 "Information processing systems - Data communication -	*/
/*	  Protocol providing the connectionless-mode  network service";	*/
/*	ISO 8348 "Information processing systems - Data communication -	*/
/*	  Network service definition. Addendum 1: Connectionless-mode	*/
/*	  transmission".						*/
/*	ISO 9542 "Information processing systems - Telecommunications	*/
/*	  and information exchange beetween systems - End system to	*/
/*	  Intermediate system routing exchange protocol for use with	*/
/*	  ISO 8473.							*/
/*									*/
/* GLOBAL STRUCTURES DEFINED IN THIS MODULE :				*/
/*									*/
/*			CLNP_8473_FHDR struct				*/
/*			CLNP_9542_FHDR struct				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 07/19/96  EJV    02     Moved CLNP_DEF_LEN_HDR from clnp_usr.h and	*/
/*			   deleted clnpl_all_es_mac			*/
/* 05/24/96  EJV    01     Created					*/
/************************************************************************/

#ifndef CLNP_INCLUDED
#define CLNP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************/
/* Defines and structures below are specific to the ISO 8473 standard	*/
/* (DT and ER PDUs for CLNP).						*/
/************************************************************************/
			

/* ISO 8473 protocol id and version */
#define CLNP_8473_PROT_ID	(ST_UCHAR) 0x81
#define CLNP_8473_PROT_VER	(ST_UCHAR) 0x01

/* PDU type and flags (they are already in proper bit position) */
#define CLNP_8473_TYPE_ER	(ST_UCHAR) 0x01
#define CLNP_8473_TYPE_DT	(ST_UCHAR) 0x1C

#define CLNP_SEG_NOT_PERMITTED	(ST_UCHAR) 0x00
#define CLNP_SEG_PERMITTED	(ST_UCHAR) 0x80
						/* bit 8 ON	*/

#define CLNP_LAST_SEG		(ST_UCHAR) 0x00
#define CLNP_MORE_SEG		(ST_UCHAR) 0x40
						/* bit 7 ON	*/	

#define CLNP_ERR_REP_NOT_GEN	(ST_UCHAR) 0x00
#define CLNP_ERR_REP_GENERATE	(ST_UCHAR) 0x20
						/* bit 6 ON	*/

#define CLNP_MASK_FLAGS	(ST_UCHAR) 0x1F	/* mask bit 6,7,8 */



/*----------------------------------------------------------------------*/
/*			CLNP_8473_FHDR struct				*/
/*----------------------------------------------------------------------*/
/* Structure below define the fixed part in the ISO 8473 PDU header.	*/
/* The rest of the header has variable length.				*/

/* NOTE: ! only ST_UCHAR type should be in protocol structs to avoid	*/
/*	 ! packing or problems with sizeof operator.			*/

typedef struct
  {
  ST_UCHAR		protocol_id;	/* Network Layer Protocol Identifier	*/
  ST_UCHAR		hdr_len;	/* Header length			*/
  ST_UCHAR		protocol_ver;	/* Version of the protocol		*/
  ST_UCHAR		pdu_lifetime;	/* Remaining lifetime for PDU		*/
  ST_UCHAR		type_flags;	/* PDU type & flags			*/
  ST_UCHAR		seg_len[2];	/* Segment length (hdr+data len)swapped	*/
  ST_UCHAR		checksum [2];	/* Checksum of the PDU header		*/
  }CLNP_8473_FHDR;

#define CLNP_8473_FHDR_LEN	sizeof (CLNP_8473_FHDR)


/* structure below defines the Segmenting part in a DT PDU header	*/
typedef struct
  {
  ST_UCHAR		pdu_id [2]; 	/* Data Unit Identiefier		*/
  ST_UCHAR		pdu_offset [2];	/* Offset of the segment in original PDU*/
  ST_UCHAR		pdu_total_len [2];	/* Total len of original PDU	*/
  }CLNP_8473_HDR_SEG;

#define CLNP_8473_HDR_SEG_LEN	sizeof (CLNP_8473_HDR_SEG)


#define CLNP_DEF_LEN_HDR   (ST_UCHAR) (CLNP_8473_FHDR_LEN+2*(CLNP_MAX_LEN_NSAP+1))
					/* this implementaion assumes	*/
					/* following parts to be present*/
					/* in header:			*/
					/* fixedPart +destNSAP +srcNSAP */
					/* (no seg, no options parts);	*/
					/* 51 bytes or less		*/


/************************************************************************/
/* Defines and structures below are specific to the ISO 9542 standard	*/
/* (ES-IS HELLOs for CLNP).						*/
/************************************************************************/
			

/* ISO 8473 protocol id and version */
#define CLNP_9542_PROT_ID	(ST_UCHAR) 0x82
#define CLNP_9542_PROT_VER	(ST_UCHAR) 0x01

/* PDU type */
#define CLNP_9542_TYPE_ESH	(ST_UCHAR) 0x02
#define CLNP_9542_TYPE_ISH	(ST_UCHAR) 0x04
#define CLNP_9542_TYPE_RD	(ST_UCHAR) 0x06



/*----------------------------------------------------------------------*/
/*			CLNP_9542_FHDR struct				*/
/*----------------------------------------------------------------------*/
/* Structure below defines the fixed part in the ISO 9542 PDU header.	*/
/* The rest of the header has variable length.				*/
/* Note that the ISO 9542 (ES HELLO and IS HELLO PDUs) do not contain	*/
/* PDU data part.							*/

/* NOTE: ! only ST_UCHAR type should be in protocol structs to avoid	*/
/*	 ! packing or problems with sizeof operator.			*/

typedef struct
  {
  ST_UCHAR		protocol_id;	/* Network Layer Protocol Identifier	*/
  ST_UCHAR		hdr_len;	/* Header length			*/
  ST_UCHAR		protocol_ver;	/* Version of the protocol		*/
  ST_UCHAR		rfu;		/* Reserved for future use		*/
  ST_UCHAR		type;		/* PDU type, bits 6,7,8 are always 0	*/
  ST_UCHAR		holding_time[2];/* max time to retain received info	*/
  ST_UCHAR		checksum [2];	/* Checksum of the PDU header		*/
  }CLNP_9542_FHDR;

#define CLNP_9542_FHDR_LEN	sizeof (CLNP_9542_FHDR)



/************************************************************************/
/* Internal CLNP function prototypes					*/
/************************************************************************/

/* clnp_enc.c module */

ST_RET clnpl_encode_dt  (SN_UNITDATA *sn_req, N_UNITDATA *req);
ST_RET clnpl_encode_esh (ST_UCHAR *enc_buf, ST_UINT16 *enc_len);


/* clnp_dec.c module */

ST_RET clnpl_decode   (SN_UNITDATA *sn_req, N_UNITDATA **req);
ST_VOID    clnpl_log_npdu (ST_UCHAR *pdu_buf, ST_UCHAR npdu_type, ST_CHAR *enc_dec_str);

/* clnp_esh.c module */

ST_RET clnpl_init_esh        (ST_UINT16 delay_time);
ST_VOID    clnpl_check_cfg_timer (ST_VOID);
ST_RET clnpl_send_esh        (ST_UCHAR *rem_mac);


#if 0
/* struct below may be needed to implement in the future the ISO 8473	*/
/* header options							*/
/*----------------------------------------------------------------------*/
/*			CLNP_OPTIONS struct				*/
/*----------------------------------------------------------------------*/
/* Structure below specifies the options to be used for the PDU.	*/
/* The options by default are all turned off at the start-up		*/
/* time. In future some functions may be provided to set up the options	*/
/* or the structure will be exposed to the CLNP-user for setting the	*/
/* parameters.								*/
/* NOTE: currently none of the option are implemented.			*/

/* Options for padding_flag */
#define CLNP_OPT_PAD_NONE	0	/* no header padding		*/
#define CLNP_OPT_PAD_TO_SIZE	1	/* pad header to specified size	*/
#define CLNP_OPT_PAD_TO_BOUDARY	2	/* pad header so that data	*/
					/* starts on specified boundary */
/* Options for security_flag */
#define CLNP_OPT_SEC_NONE	0	/* no security level specified	*/
#define CLNP_OPT_SEC_SRC	1	/* sec. Source Address specific	*/
#define CLNP_OPT_SEC_DEST	2	/* sec. Dest Address specific	*/
#define CLNP_OPT_SEC_UNIQUE	3	/* security Globally Unique	*/

/* Options for routeing_flag */
#define CLNP_OPT_ROUTE_NONE	0	/* no routeing specified	*/
#define CLNP_OPT_ROUTE_PART	1	/* partial routeing requested	*/
#define CLNP_OPT_ROUTE_COMP	2	/* complete routeing requested	*/

/* Options for record_route_flag */
#define CLNP_OPT_REC_ROUTE_NONE	0	/* no route recording		*/
#define CLNP_OPT_REC_ROUTE_PART	1	/* partial route recording	*/
#define CLNP_OPT_RCE_ROUTE_COMP	2	/* complete route recording	*/

/* Options for qos_flag (Quality of Service - QoS) */
#define CLNP_OPT_QoS_NONE	0	/* no QoS specified		*/
#define CLNP_OPT_QoS_SRC	1	/* QoS Source Address specific	*/
#define CLNP_OPT_QoS_DEST	2	/* QoS Dest Address specific	*/
#define CLNP_OPT_QoS_UNIQUE	3	/* QoS Globally Unique	*/

/* Options for priority_flag (use values from 1 to 14 to specified	*/
/* priority parameter for the DT PDU.					*/
#define CLNP_OPT_PRIORITY_DEF	0	/* default priority		*/
#define CLNP_OPT_PRIORITY_MAX	14	/* max priority			*/

typedef struct
  {
  ST_UCHAR  padding_flag;		
  ST_UCHAR  security_flag;
  ST_UCHAR  routeing_flag;
  ST_UCHAR  record_route_flag;
  ST_UCHAR  qos_flag;
  ST_UCHAR  priority_flag;
  }CLNP_OPTIONS;

extern	CLNP_OPTIONS	clnpl_options;
#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/




