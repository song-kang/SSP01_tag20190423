/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	2002-2005, All Rights Reserved.					*/
/*									*/
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*									*/
/* MODULE NAME : ethertyp.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION :  Ethertype frame encode functions.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 12/09/05  JRB    05     etype_hdr_decode: return NULL on error.	*/
/* 02/08/05  JRB    04     etype_hdr_decode: chg first arg to		*/
/*			   (SN_UNITDATA *) so it can access more data.	*/
/* 08/30/04  ASK    03     Change cast from short to ST_UINT16 in   	*/
/*			   etype_hdr_decode to avoid possible incorrect	*/
/*			   sign extension on Unix.	  		*/
/* 10/24/03  JRB    02     Del ethsub.h, caused LINUX compile errs.	*/
/* 11/20/02  ASK    01     Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "ethertyp.h"

#define MAC_HEAD_LEN	14	/* DEBUG: was from ethsub.h	*/

ST_UCHAR *etype_hdr_encode(ST_UCHAR *bufPtr, /* buffer to encode into */
		           ST_INT bufLen,    /* len of buffer 	      */
		           ST_INT *asn1Len,  /* ptr to len encoded    */
		           ETYPE_INFO *info) /* ptr etype struct      */
  {
  /* Make sure we have enough buffer left to encode the Ethertype and
   * VLAN headers.
   */
  if((*asn1Len) + VLAN_HEAD_LEN + ETYPE_HEAD_LEN + MAC_HEAD_LEN + 2 > bufLen)
    {
    /* This isn't exactly nice, but we should've had more room */
    return NULL;
    }

  /* Now encode the Ethertype Header, from back to front */
  (*asn1Len) += ETYPE_HEAD_LEN;
  
  bufPtr--;
  (*bufPtr--) = 0x00; 				   /* Reserved 2 */
  (*bufPtr--) = 0x00;
  (*bufPtr--) = 0x00; 				   /* Reserved 1 */
  (*bufPtr--) = 0x00;
  (*bufPtr--) = (ST_UCHAR) ((*asn1Len) & 0xFF);
  (*bufPtr--) = (ST_UCHAR) ((*asn1Len) >> 8); 	   /* Length */ 
  (*bufPtr--) = (ST_UCHAR) (info->appID & 0xFF);
  (*bufPtr--) = (ST_UCHAR) (info->appID >> 8);     /* AppID */
  (*bufPtr--) = (ST_UCHAR) (info->etypeID & 0xFF); 
  (*bufPtr--) = (ST_UCHAR) (info->etypeID >> 8);   /* Ethertype ID */	
  
  /* +2 here because the Ethertype ID is not considered part of the 
   * Ethertype Header length.
   */
  (*asn1Len) += 2;
  
  /* bufPtr should point at the start of Ethertype data */
  bufPtr = vlan_hdr_encode(bufPtr, asn1Len, info);
  
  return bufPtr;
  }

ST_UCHAR *vlan_hdr_encode(ST_UCHAR *bufPtr, /* buffer to encode into */
		          ST_INT *asn1Len,  /* ptr to len encoded    */
		          ETYPE_INFO *info) /* ptr etype struct      */
  {
  /* Assume we have a good buffer length from etype_encode() */

  /* Encode the VLAN info, from back to front */
  (*bufPtr--) = (ST_UCHAR) (info->tci & 0xFF);
  (*bufPtr--) = (ST_UCHAR) (info->tci >> 8);   /* TCI */	
  (*bufPtr--) = (ST_UCHAR) 0x00;
  (*bufPtr)   = (ST_UCHAR) 0x81;	       /* TPID */
  
  (*asn1Len) += VLAN_HEAD_LEN;
  
  return bufPtr;
  }

/************************************************************************/
/*			etype_hdr_decode				*/
/* Decode the ethertype header of a received frame.			*/
/* Assume sn_req->lpdu_len contains the IEEE 802.3 Length/Type field.	*/
/* Assume sn_req->lpdu points at the IEEE 802.3 MAC Client Data.	*/
/* Arguments:								*/
/*   sn_req	- Subnetwork frame to be decoded			*/
/*   info	- pointer to decoded Ethertype header info		*/
/*   apduLen	- pointer to len of APDU (after Ethertype header)	*/
/* Returns:								*/
/*   (ST_UCHAR *)	- pointer to APDU (after Ethertype header)	*/
/************************************************************************/
ST_UCHAR *etype_hdr_decode(SN_UNITDATA *sn_req,
		           ETYPE_INFO *info,
		           ST_INT *apduLen)
  {
  if (sn_req->lpdu_len < ETYPE_TYPE_GOOSE)
    return (NULL);	/* This is not an Ethertype packet we recognize	*/

  /* The "tci" is not important for the user, and it is often stripped	*/
  /* off by various network hardware, so if it is present, the		*/
  /* Subnetwork does not pass it up. This code just sets it to a dummy	*/
  /* value of 0.							*/
  info->tci    	= 0;
  info->etypeID	= sn_req->lpdu_len;	/* Ethernet Type Id saved by subnet in "lpdu_len"*/

  /* First 2 bytes of "MAC Client data" hold appID.	*/
  info->appID   = (((ST_UINT16) sn_req->lpdu[0] << 8) | (ST_UINT16) sn_req->lpdu[1]); 

  /* Next 2 bytes of "MAC Client data" hold length.	*/
  /* Encoded length includes header so subtract header length.	*/
  (*apduLen)	= (((ST_UINT16) sn_req->lpdu[2] << 8) | (ST_UINT16) sn_req->lpdu[3]) - ETYPE_HEAD_LEN;

  return(sn_req->lpdu + ETYPE_HEAD_LEN);	/* return ptr to user data (after header)	*/
  }
