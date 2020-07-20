/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	2002-2005, All Rights Reserved.					*/
/*									*/
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*									*/
/* MODULE NAME : ethertyp.h						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION :  Ethertype frame encode functions and constants.*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 02/08/05  JRB    03     Chg first arg to etype_hdr_decode.		*/
/* 03/24/03  ASK    02     Updated ETYPE_TYPE_* constant values		*/
/* 11/20/02  ASK    01     Created					*/
/************************************************************************/

#if !defined(ETHERTYP_H_INCLUDED)
#define ETHERTYP_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "clnp_usr.h"
#include "clnp_sne.h"


#define ETYPE_VLAN_TYPE_ID 0x8100
#define ETYPE_TYPE_GOOSE   0x88B8
#define ETYPE_TYPE_GSE	   0x88B9 
#define ETYPE_TYPE_SV	   0x88BA 
#define ETYPE_TCI_GOOSE	   0x8000	
#define ETYPE_TCI_GSE	   0x2000
#define ETYPE_TCI_SV	   0x8001

#define VLAN_HEAD_LEN	   4	
#define ETYPE_HEAD_LEN	   8

/* Ethertype Frame Info to be filled out by the user */
typedef struct
  {
  ST_UINT16 tci;     /* VLAN Tag Control Info */
  ST_UINT16 etypeID; /* Ethertype ID */
  ST_UINT16 appID;   /* APP ID */
  } ETYPE_INFO;	     	             


ST_UCHAR *etype_hdr_encode(ST_UCHAR *bufPtr, /* buffer to encode into */
		           ST_INT bufLen,    /* len of buffer 	      */
		           ST_INT *asn1Len,  /* ptr to len encoded    */
		           ETYPE_INFO *info);/* ptr etype struct      */

ST_UCHAR *vlan_hdr_encode(ST_UCHAR *bufPtr,  /* buffer to encode into */
		          ST_INT *asn1Len,   /* ptr to len encoded    */
		          ETYPE_INFO *info); /* ptr etype struct      */

ST_UCHAR *etype_hdr_decode(SN_UNITDATA *sn_req,
		           ETYPE_INFO *info, /* ptr etype struct      */
		           ST_INT *bufLen);  /* ptr to len decoded    */

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' */
