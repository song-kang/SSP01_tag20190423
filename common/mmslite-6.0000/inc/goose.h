/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1999-2002, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : goose.h 						*/
/* PRODUCT(S)  : 	 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 07/23/08  JRB     10    Fix len of gcRef, dataSetRef, appID.		*/
/* 09/11/07  JRB     09    Add gse_iec_enc_part1, gse_iec_enc_part2.	*/
/*			   Chg gcRef, dataSetRef, appID data type from	*/
/*			   char ptr to char array (reduces allocation).	*/
/*			   Fix type of numDataEntries, confRev.		*/
/* 01/08/03  JRB     08    Del obsolete dataRef, elementId from structs	*/
/*			   & from gse_iec_data_init args.		*/
/* 12/03/02  ASK     07    Chg Goose structs for new ASN.1 encoding. Chg*/
/*			   gse_iec_encode and gse_iec_hdr_decode protos	*/
/*			   for Ethertype. include "ethertyp.h" 		*/
/* 09/30/02  NAV     06	   Add ifdef __cplusplus                        */
/* 02/26/02  JRB     05    Chg IEC GOOSE asn1Data args to UCHAR.	*/
/*			   Chg some IEC GOOSE decode params to INT32.	*/
/* 01/02/02  JRB     04    Add IEC GOOSE definitions.			*/
/* 07/25/00  RKR     03    changed VisibleString SendingIED to ST_CHAR	*/
/* 09/10/99  JRB     02    Chg SqNum, StNum, HoldTim, & BackTim		*/
/*			   to UINT32 to match GOMSFE 0.9.		*/
/* 06/25/99  MDE     01    Created					*/
/************************************************************************/
#ifndef GOOSE_HDR_INCLUDED
#define GOOSE_HDR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "asn1r.h"			/* need MMS_UTC_TIME	*/
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "acse2usr.h"
#include "ethertyp.h"
#include "mvl_defs.h"			/* need MVL61850_MAX_* defines.	*/
/* NOTE: need runtime_type from "mms_vvar.h" but requires bunch of	*/
/*       other includes, so just use forward declaration.		*/ 
struct runtime_type;			/* forward declaration		*/

/************************************************************************/


#define	GOOSE_NAME_MISMATCH	0x112
#define	GOOSE_DONE_TOO_SOON	0x113

/************************************************************************/
#define GOOSE_MAX_NUM_DNA_BITS	128
#define GOOSE_MAX_NUM_USR_BITS	512

#define GOOSE_DEF_NUM_DNA_BITS	64
#define GOOSE_DEF_NUM_USR_BITS	128

typedef struct 
  {
/* GOOSE Stack addressing information */
  AUDT_APDU audtApdu;

/* GOOSE MMS values */
  ST_CHAR  SendingIED[66];
  MMS_BTOD  t;
  ST_UINT32 SqNum;
  ST_UINT32 StNum;
  ST_UINT32 HoldTim;
  ST_UINT32 BackTim;
  ST_UINT16 PhsID;

  ST_INT num_dna_bits; 
  ST_UCHAR  DNA[GOOSE_MAX_NUM_DNA_BITS/8];

  ST_INT num_usr_bits; 
  ST_UCHAR  UserSt[GOOSE_MAX_NUM_USR_BITS/8];
  } GOOSE_INFO;

/************************************************************************/
/* The user makes use of these functions to send and receive GOOSE	*/
/* messages.								*/

ST_RET mmsl_send_goose (GOOSE_INFO *gi);
ST_VOID u_mmsl_goose_received (GOOSE_INFO *goose_info);

/************************************************************************/
/* Internal 	*/

#define GOOSE_MAX_PDU_SIZE 	200

/* Define macro so new function just calls old function.	*/
#define gse_uca_write(gi)	mmsl_send_goose (gi)

ST_RET gse_uca_decode (SN_UNITDATA *sn_udt,	/* input subnet packet	*/
		GOOSE_INFO *goose_info);	/* output GOOSE data	*/

/************************************************************************/
/* BEGIN IEC GOOSE DEFINITIONS						*/
/************************************************************************/
typedef struct
  {
  struct runtime_type *runtimeTypeHead;	/* Array of Runtime Types	*/
  ST_INT numRuntimeTypes;		/* # of Runtime Types in array	*/
  ST_CHAR *dataBuf;			/* ptr to local data		*/
  ST_VOID *userInfo;			/* To store anything user wants.*/
					/* GSE code does not use it.	*/
  } GSE_IEC_DATA_ENTRY;

typedef struct
  {
  ST_UINT8 *asn1Ptr;
  ST_INT asn1Len;
  } GSE_IEC_DATA_ENTRY_RX;

typedef struct
  {
  ST_CHAR gcRef [MVL61850_MAX_OBJREF_LEN+1];	/* GoCBRef in IEC 61850	*/
  ST_UINT32 timeToLive;
  ST_CHAR dataSetRef [MVL61850_MAX_OBJREF_LEN+1];/* DatSet in IEC 61850	*/
  ST_BOOLEAN appID_pres;
  ST_CHAR appID [MVL61850_MAX_RPTID_LEN+1];	/* GoID in IEC 61850	*/
						/* (was AppID in Ed 1)	*/
  MMS_UTC_TIME utcTime;
  ST_UINT32 stNum;
  ST_UINT32 sqNum;
  ST_BOOLEAN test;
  ST_UINT32 confRev;
  ST_BOOLEAN needsCommissioning;
  ST_INT32 numDataEntries;
  GSE_IEC_DATA_ENTRY *dataEntries;	/* array of data entry structs	*/
  }GSE_IEC_CTRL;

typedef struct
  {
  ST_CHAR *gcRef;
  ST_UINT32 timeToLive;
  ST_CHAR *dataSetRef;
  ST_BOOLEAN appID_pres;
  ST_CHAR *appID;
  MMS_UTC_TIME utcTime;
  ST_UINT32 stNum;
  ST_UINT32 sqNum;
  ST_BOOLEAN test;
  ST_UINT32 confRev;
  ST_BOOLEAN needsCommissioning;
  ST_INT32 numDataEntries;
  ST_INT tmpIndex;	/* index to current entry in "dataEntries" array.*/
			/* Used during decode when filling in "dataEntries".*/
  GSE_IEC_DATA_ENTRY_RX *dataEntries;	/* array of data entry structs	*/
  }GSE_IEC_HDR;

/* Subnet functions (clnp_snet_*) must be used to set multicast filters.*/
/* The following "gse_*" macros may be used to access these functions.	*/
#define gse_set_multicast_filter	clnp_snet_set_multicast_filter
#define gse_discovery_start		clnp_snet_rx_all_multicast_start
#define gse_discovery_stop		clnp_snet_rx_all_multicast_stop 

GSE_IEC_HDR *gse_iec_hdr_decode (ST_UCHAR *apdu, ST_INT apdu_len);
/* After calling "gse_iec_hdr_decode", user can examine "dataEntries"	*/
/* array in GSE_IEC_HDR, and call "ms_asn1_to_local" to decode any or	*/
/* all dataEntries.							*/

/* User must call this function when they are done with the decoded	*/
/* GOOSE info to free up the resources.					*/
ST_RET gse_iec_decode_done (GSE_IEC_HDR *hdr);
 

GSE_IEC_CTRL *gse_iec_control_create (ST_CHAR *gcRef, ST_CHAR *dataSetRefRef, ST_CHAR *appId,
				      ST_INT numDataEntry);
ST_RET gse_iec_control_destroy (GSE_IEC_CTRL *ctrl);

ST_RET gse_iec_data_init (GSE_IEC_CTRL *ctrl,
			ST_INT index,
			struct runtime_type *runtimeTypeHead,
			ST_INT numRuntimeTypes
			);

ST_RET gse_iec_data_update (GSE_IEC_CTRL *ctrl,
			ST_INT index,
			ST_VOID *dataPtr);

ST_UCHAR *gse_iec_encode (GSE_IEC_CTRL *ctrl,
			ST_UCHAR *asn1DataBuf,	/* buffer to encode in	*/
			ST_INT asn1DataBufLen,	/* len of buffer	*/
			ST_INT *asn1DataLenOut,	/* ptr to len encoded   */
			ETYPE_INFO *etype_info);/* ptr to Ethertype info*/

ST_RET gse_iec_send (GSE_IEC_CTRL *ctrl,
			ST_UCHAR *dstMac,	/* Destination MAC addr	*/
			ST_UCHAR *asn1Data,	/* ptr to ASN.1 encoded data*/
			ST_INT asn1DataLen);	/* len of ASN.1 encoded data*/

/* Functions to use for 2-part encode. Reduces re-encoding of data.	*/
ST_UCHAR *gse_iec_enc_part1 (GSE_IEC_CTRL *ctrl,
	ST_UCHAR *buf_ptr,		/* buffer in which to encode	*/
	ST_INT buf_len,			/* buffer len			*/
	ST_INT *asn1_len_out);		/* len of ASN.1 encoded data	*/
ST_UCHAR *gse_iec_enc_part2 (GSE_IEC_CTRL *ctrl,
	ETYPE_INFO *etype_info,		/* ptr to Ethertype info	*/
	ST_UCHAR *buf_ptr,		/* buffer in which to encode	*/
	ST_INT buf_len,			/* buffer len			*/
	ST_INT enc_part1_len,		/* len of data encoded by	*/
					/* gse_iec_enc_part1		*/
					/* CRITICAL: must be < buf_len.	*/
	ST_INT *asn1_len_out);		/* len of ASN.1 encoded data	*/

/************************************************************************/
/* END IEC GOOSE DEFINITIONS						*/
/************************************************************************/
#ifdef __cplusplus
}
#endif
#endif 		/* included	*/
