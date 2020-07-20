/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	2002-2002, All Rights Reserved.					*/
/*									*/
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*									*/
/* MODULE NAME : gse_mgmt_enc.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION :  GSE MGMT encode functions.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 11/03/06  CRM     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "mem_chk.h"
#include "mmsdefs.h"		/* many basic MMS definitions	*/
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "clnp_llc.h"
#include "clnp.h"		/* for clnpl_decode		*/
#include "gse_mgmt.h"
#include "acse2log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#include "ethertyp.h"


/************************************************************************/
/*			  getRefReqEncode				*/
/* Encodes GSE Management request including Ethertype header (14 bytes).*/
/************************************************************************/
static ST_UCHAR *getRefReqEncode (GSE_REF_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ST_UINT16 tag_num,	/* GSE Mgmt tag num	    */
	ETYPE_INFO *etype_info,	/* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {
ST_INT index;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;		 /* ptr to start of ASN1 encoded data	  */
					 /* init to NULL (i.e. failure)		  */

  asn1r_strt_asn1_bld (&aCtx, asn1DataBuf, asn1DataBufLen);

  asn1r_strt_constr (&aCtx);		 /* Start APPLICATION 0 IMPLICIT SEQUENCE */
  asn1r_strt_constr (&aCtx); 		 /* Start GSEMgmtRequests		  */
  asn1r_strt_constr (&aCtx); 		 /* Start "...RequestPdu"		  */
  asn1r_strt_constr (&aCtx); 		 /* Start REF REQ OFFSET IMPLICIT SEQ     */

  for (index = ctrl->numIntegers - 1; index >= 0; index--)
    {
    asn1r_wr_u32 (&aCtx, ctrl->offset[index]);
    asn1r_fin_prim (&aCtx, INT_CODE, UNI);
    }	/* end loop	*/

  asn1r_fin_constr (&aCtx,1,CTX,DEF);		/* finish REF REQ OFFSET IMPLICIT SEQ */
  
  asn1r_wr_vstr (&aCtx, ctrl->ident);
  asn1r_fin_prim (&aCtx,0,CTX);
  
  asn1r_fin_constr (&aCtx,tag_num,CTX,DEF);	/* finish "...RequestPdu"	      */
  asn1r_fin_constr (&aCtx,1,CTX,DEF);		/* finish GSEMgmtRequests	      */

  asn1r_wr_u32 (&aCtx, ctrl->stateID);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,0,APP,DEF);		/* finish APPLICATION 0 IMPLICIT SEQ  */
  
  /* ASN.1 encoding done. Chk for overrun.					      */
  if (aCtx.asn1r_encode_overrun)
    {
    ACSELOG_ERR0 ("GSE MGMT encode overrun");
    }
  else
    {
    *asn1DataLenOut = ASN1_ENC_LEN(&aCtx);	/* len encoded			      */
      
    /* Finally, add the Ethertype information					      */
    /* This is the only place we set return to GOOD value.			      */
    asn1Start = etype_hdr_encode(ASN1_ENC_PTR(&aCtx),  /* buffer to encode into	      */
        		           asn1DataBufLen,     /* len of buffer 	      */
	       		           asn1DataLenOut,     /* ptr to len encoded	      */
	       		           etype_info);        /* ptr etype struct	      */
    }
  
  /* If all is OK so far, finish encoding PDU by adding MAC addresses.	*/
  if (asn1Start)
    {
    if (asn1Start > asn1DataBuf + 12)
      {	/* there's room for MAC addresses so add them now	*/
      memcpy (asn1Start-12, dstMac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
      memcpy (asn1Start-6, srcMac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
      asn1Start -= 12;		/* point to start of "complete" PDU	*/
      *asn1DataLenOut += 12;
      }
    else
      asn1Start = NULL;	/* ERROR: not enough room to finish encode	*/
    }
  return (asn1Start);				/* ptr to start (or NULL on error)    */
  }


/************************************************************************/
/*			getOffsetReqEncode				*/
/* Encodes GSE Management request including Ethertype header (14 bytes).*/
/************************************************************************/
static ST_UCHAR *getOffsetReqEncode (GSE_OFFSET_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ST_UINT16 tag_num,	/* GSE Mgmt tag num	    */    
	ETYPE_INFO *etype_info,	/* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {
ST_INT index;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	*/
				/* init to NULL (i.e. failure)		*/

  asn1r_strt_asn1_bld (&aCtx, asn1DataBuf, asn1DataBufLen);

  asn1r_strt_constr (&aCtx);	/* Start APPLICATION 0 IMPLICIT SEQUENCE*/
  asn1r_strt_constr (&aCtx); 	/* Start GSEMgmtRequests		*/
  asn1r_strt_constr (&aCtx); 	/* Start "...RequestPdu"		*/
  asn1r_strt_constr (&aCtx); 	/* Start OFFSET REQ REF IMPLICIT SEQ	*/
  
  for (index = ctrl->numVStrings - 1; index >= 0; index--)
    {
    asn1r_wr_vstr (&aCtx, ctrl->references[index]);
    asn1r_fin_prim (&aCtx, VISTR_CODE,UNI);
    }	/* end loop	*/

  asn1r_fin_constr (&aCtx,1,CTX,DEF);		/* finish OFFSET REQ REF IMPLICIT SEQ     */

  asn1r_wr_vstr (&aCtx, ctrl->ident);
  asn1r_fin_prim (&aCtx,0,CTX);
  
  asn1r_fin_constr (&aCtx,tag_num,CTX,DEF);	/* finish "...RequestPdu"	      */
  asn1r_fin_constr (&aCtx,1,CTX,DEF);		/* finish GSEMgmtRequests	      */

  asn1r_wr_u32 (&aCtx, ctrl->stateID);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,0,APP,DEF);		/* finish APPLICATION 0 IMPLICIT SEQ   */

  /* ASN.1 encoding done. Chk for overrun. */
  if (aCtx.asn1r_encode_overrun)
    {
    ACSELOG_ERR0 ("GSE MGMT encode overrun");
    }
  else
    {
    *asn1DataLenOut = ASN1_ENC_LEN(&aCtx);	/* len encoded	*/
      
    /* Finally, add the Ethertype information */
    /* This is the only place we set return to GOOD value.		*/
    asn1Start = etype_hdr_encode(ASN1_ENC_PTR(&aCtx),  /* buffer to encode into */
        		           asn1DataBufLen,     /* len of buffer 	*/
	       		           asn1DataLenOut,     /* ptr to len encoded    */
	       		           etype_info);        /* ptr etype struct      */
    }
   
  /* If all is OK so far, finish encoding PDU by adding MAC addresses.	*/
  if (asn1Start)
    {
    if (asn1Start > asn1DataBuf + 12)
      {	/* there's room for MAC addresses so add them now	*/
      memcpy (asn1Start-12, dstMac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
      memcpy (asn1Start-6, srcMac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
      asn1Start -= 12;		/* point to start of "complete" PDU	*/
      *asn1DataLenOut += 12;
      }
    else
      asn1Start = NULL;	/* ERROR: not enough room to finish encode	*/
    }
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }



/************************************************************************/
/*			getGoRefReqEncode					*/
/* Encodes GSE Management request including Ethertype header (14 bytes).*/
/************************************************************************/
ST_UCHAR *getGoRefReqEncode (GSE_REF_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info,	/* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {

  return getRefReqEncode (ctrl, 
    asn1DataBuf,
    asn1DataBufLen, 
    asn1DataLenOut, 
    GET_GO_REF_REQ_TAG, 
    etype_info, 
    dstMac, 
    srcMac); 
  }


/************************************************************************/
/*			getGOOSEEleNumReqEncode				*/
/* Encodes GSE Management request including Ethertype header (14 bytes).*/
/************************************************************************/
ST_UCHAR *getGOOSEEleNumReqEncode (GSE_OFFSET_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
 	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
 {

  return getOffsetReqEncode (ctrl, 
    asn1DataBuf, 
    asn1DataBufLen, 
    asn1DataLenOut, 
    GET_GOOSE_ELE_NUM_REQ_TAG, 
    etype_info, 
    dstMac, 
    srcMac); 
  }


/************************************************************************/
/*			getGsRefReqEncode				*/
/* Encodes GSE Management request including Ethertype header (14 bytes).*/
/************************************************************************/
ST_UCHAR *getGsRefReqEncode (GSE_REF_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {

  return getRefReqEncode (ctrl, 
    asn1DataBuf, 
    asn1DataBufLen, 
    asn1DataLenOut, 
    GET_GS_REF_REQ_TAG, 
    etype_info, 
    dstMac, 
    srcMac); 
  }


/************************************************************************/
/*			getGSSEDataOffsetReqEncode			*/
/* Encodes GSE Management request including Ethertype header (14 bytes).*/
/************************************************************************/
ST_UCHAR *getGSSEDataOffsetReqEncode (GSE_OFFSET_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {

  return getOffsetReqEncode (ctrl, 
    asn1DataBuf, 
    asn1DataBufLen, 
    asn1DataLenOut, 
    GET_GSSE_DATA_OFF_REQ_TAG, 
    etype_info, 
    dstMac, 
    srcMac); 
  }


/************************************************************************/
/*			gse_mgmt_NotSupported				*/
/* Encodes GSE Management response including Ethertype header (14 bytes)*/
/************************************************************************/
ST_UCHAR *gse_mgmt_NotSupported (ST_VOID)
  {

  return NULL; 
  }


/************************************************************************/
/*			getGlbErrorRspEncode				*/
/* Encodes GSE Management response including Ethertype header (14 bytes)*/
/************************************************************************/
ST_UCHAR *getGlbErrorRspEncode (GSE_GLB_ERR_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	 */
				/* init to NULL (i.e. failure)		 */

  asn1r_strt_asn1_bld (&aCtx, asn1DataBuf, asn1DataBufLen);

  asn1r_strt_constr (&aCtx);	/* Start APPLICATION 0 IMPLICIT SEQUENCE */
  asn1r_strt_constr (&aCtx); 	/* Start GSEMgmtReponses		 */
  asn1r_strt_constr (&aCtx); 	/* Start "...ResponsePdu"		 */

  asn1r_wr_u32 (&aCtx, ctrl->glbError);
  asn1r_fin_prim (&aCtx,3,CTX);

  asn1r_wr_u32 (&aCtx, ctrl->confRev);
  asn1r_fin_prim (&aCtx,1,CTX);		

  asn1r_wr_vstr (&aCtx, ctrl->ident);
  asn1r_fin_prim (&aCtx,0,CTX);
  
  asn1r_fin_constr (&aCtx,GET_GO_REF_RSP_TAG,CTX,DEF);	/* finish ResponsePdu	   */
  asn1r_fin_constr (&aCtx,2,CTX,DEF);			/* fininsh GSEMgmtResonses */

  asn1r_wr_u32 (&aCtx, ctrl->stateID);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,0,APP,DEF);		/* finish APPLICATION 0 IMPLICIT SEQ  */

  /* ASN.1 encoding done. Chk for overrun. */
  if (aCtx.asn1r_encode_overrun)
    {
    ACSELOG_ERR0 ("GSE MGMT encode overrun");
    }
  else
    {
    *asn1DataLenOut = ASN1_ENC_LEN(&aCtx);	/* len encoded	*/
      
    /* Finally, add the Ethertype information */
    /* This is the only place we set return to GOOD value.		*/
    asn1Start = etype_hdr_encode(ASN1_ENC_PTR(&aCtx),  /* buffer to encode into */
        		           asn1DataBufLen,         /* len of buffer 	*/
	       		           asn1DataLenOut,         /* ptr to len encoded    */
	       		           etype_info);        /* ptr etype struct      */
    }
    
  /* If all is OK so far, finish encoding PDU by adding MAC addresses.	*/
  if (asn1Start)
    {
    if (asn1Start > asn1DataBuf + 12)
      {	/* there's room for MAC addresses so add them now	*/
      memcpy (asn1Start-12, dstMac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
      memcpy (asn1Start-6, srcMac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
      asn1Start -= 12;		/* point to start of "complete" PDU	*/
      *asn1DataLenOut += 12;
      }
    else
      asn1Start = NULL;	/* ERROR: not enough room to finish encode	*/
    }
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }

/************************************************************************/
/*			getGoRefRspEncode				*/
/* Encodes GSE Management response including Ethertype header (14 bytes)*/
/************************************************************************/
ST_UCHAR *getGoRefRspEncode (GSE_REF_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {
ST_INT index;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	 */
				/* init to NULL (i.e. failure)		 */

  asn1r_strt_asn1_bld (&aCtx, asn1DataBuf, asn1DataBufLen);

  asn1r_strt_constr (&aCtx);	/* Start APPLICATION 0 IMPLICIT SEQUENCE */
  asn1r_strt_constr (&aCtx); 	/* Start GSEMgmtReponses		 */
  asn1r_strt_constr (&aCtx); 	/* Start "...ResponsePdu"		 */

  asn1r_strt_constr (&aCtx); 	/* Start POS RSP IMPLICIT SEQ		  */
  asn1r_strt_constr (&aCtx); 	/* Start POS RSP REQ RESULT IMPLICIT SEQ  */
  
  for (index = ctrl->numResults - 1; index >= 0; index--)
    {
    switch (ctrl->result[index].rsp_type)
      {
      case SD_SUCCESS:
	asn1r_wr_vstr (&aCtx, ctrl->result[index].reference);
        asn1r_fin_prim (&aCtx, 1,CTX);
	break;
      
      case SD_FAILURE:
        asn1r_wr_u32 (&aCtx, ctrl->result[index].error);
	asn1r_fin_prim (&aCtx, 2,CTX);
	break;
      }
    }	/* end loop	*/
  asn1r_fin_constr (&aCtx,1,CTX,DEF);	/* POS RSP REQ RESULT IMPLICIT SEQ	*/

  asn1r_wr_vstr (&aCtx, ctrl->datSet);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,2,CTX,DEF);	/* POS RSP IMPLICIT SEQ			*/

  asn1r_wr_u32 (&aCtx, ctrl->confRev);
  asn1r_fin_prim (&aCtx,1,CTX);		

  asn1r_wr_vstr (&aCtx, ctrl->ident);
  asn1r_fin_prim (&aCtx,0,CTX);
  
  asn1r_fin_constr (&aCtx,GET_GO_REF_RSP_TAG,CTX,DEF);	/* finish ResponsePdu	  */
  asn1r_fin_constr (&aCtx,2,CTX,DEF);			/* finish GSEMgmtResonses */

  asn1r_wr_u32 (&aCtx, ctrl->stateID);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,0,APP,DEF);		/* finish APPLICATION 0 IMPLICIT SEQ  */

  /* ASN.1 encoding done. Chk for overrun. */
  if (aCtx.asn1r_encode_overrun)
    {
    ACSELOG_ERR0 ("GSE MGMT encode overrun");
    }
  else
    {
    *asn1DataLenOut = ASN1_ENC_LEN(&aCtx);	/* len encoded	*/
      
    /* Finally, add the Ethertype information */
    /* This is the only place we set return to GOOD value.		*/
    asn1Start = etype_hdr_encode(ASN1_ENC_PTR(&aCtx),  /* buffer to encode into */
        		           asn1DataBufLen,         /* len of buffer 	*/
	       		           asn1DataLenOut,         /* ptr to len encoded    */
	       		           etype_info);        /* ptr etype struct      */
    }
    
  /* If all is OK so far, finish encoding PDU by adding MAC addresses.	*/
  if (asn1Start)
    {
    if (asn1Start > asn1DataBuf + 12)
      {	/* there's room for MAC addresses so add them now	*/
      memcpy (asn1Start-12, dstMac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
      memcpy (asn1Start-6, srcMac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
      asn1Start -= 12;		/* point to start of "complete" PDU	*/
      *asn1DataLenOut += 12;
      }
    else
      asn1Start = NULL;	/* ERROR: not enough room to finish encode	*/
    }
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }


/************************************************************************/
/*			getGOOSEEleNumRspEncode				*/
/* Encodes GSE Management response including Ethertype header (14 bytes)*/
/************************************************************************/
ST_UCHAR *getGOOSEEleNumRspEncode (GSE_OFFSET_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {
ST_INT index;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	 */
				/* init to NULL (i.e. failure)		 */

  asn1r_strt_asn1_bld (&aCtx, asn1DataBuf, asn1DataBufLen);

  asn1r_strt_constr (&aCtx);	/* Start APPLICATION 0 IMPLICIT SEQUENCE */
  asn1r_strt_constr (&aCtx); 	/* Start GSEMgmtReponses		 */
  asn1r_strt_constr (&aCtx); 	/* Start "...ResponsePdu"		 */

  asn1r_strt_constr (&aCtx); 	/* Start POS RSP IMPLICIT SEQ		  */
  asn1r_strt_constr (&aCtx); 	/* Start POS RSP REQ RESULT IMPLICIT SEQ  */
  
  for (index = ctrl->numResults - 1; index >= 0; index--)
    {
    switch (ctrl->result[index].rsp_type)
      {
      case SD_SUCCESS:
        asn1r_wr_u32 (&aCtx, ctrl->result[index].offset);
        asn1r_fin_prim (&aCtx, 0,CTX);
	break;

      case SD_FAILURE:
        asn1r_wr_u32 (&aCtx, ctrl->result[index].error);
	asn1r_fin_prim (&aCtx, 2,CTX);
	break;
      }
    }	/* end loop	*/
  asn1r_fin_constr (&aCtx,1,CTX,DEF);	/* POS RSP REQ RESULT IMPLICIT SEQ	*/

  asn1r_wr_vstr (&aCtx, ctrl->datSet);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,2,CTX,DEF);	/* POS RSP IMPLICIT SEQ */
 
  asn1r_wr_u32 (&aCtx, ctrl->confRev);
  asn1r_fin_prim (&aCtx,1,CTX);		

  asn1r_wr_vstr (&aCtx, ctrl->ident);
  asn1r_fin_prim (&aCtx,0,CTX);
  
  asn1r_fin_constr (&aCtx,GET_GOOSE_ELE_NUM_RSP_TAG,CTX,DEF);	/* finish ResponsePdu	  */
  asn1r_fin_constr (&aCtx,2,CTX,DEF);				/* finish GSEMgmtResonses */

  asn1r_wr_u32 (&aCtx, ctrl->stateID);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,0,APP,DEF);		/* finish APPLICATION 0 IMPLICIT SEQ  */

  /* ASN.1 encoding done. Chk for overrun. */
  if (aCtx.asn1r_encode_overrun)
    {
    ACSELOG_ERR0 ("GSE MGMT encode overrun");
    }
  else
    {
    *asn1DataLenOut = ASN1_ENC_LEN(&aCtx);		/* len encoded		 */
      
    /* Finally, add the Ethertype information					 */
    /* This is the only place we set return to GOOD value.			 */
    asn1Start = etype_hdr_encode (ASN1_ENC_PTR(&aCtx),	/* buffer to encode into */
        		           asn1DataBufLen,	/* len of buffer	 */
	       		           asn1DataLenOut,	/* ptr to len encoded    */
	       		           etype_info);		/* ptr etype struct      */
    }
    
  /* If all is OK so far, finish encoding PDU by adding MAC addresses.	*/
  if (asn1Start)
    {
    if (asn1Start > asn1DataBuf + 12)
      {	/* there's room for MAC addresses so add them now	*/
      memcpy (asn1Start-12, dstMac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
      memcpy (asn1Start-6, srcMac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
      asn1Start -= 12;		/* point to start of "complete" PDU	*/
      *asn1DataLenOut += 12;
      }
    else
      asn1Start = NULL;	/* ERROR: not enough room to finish encode	*/
    }
  return (asn1Start);			      /* ptr to start (or NULL on error) */
  }


/************************************************************************/
/*			getGsRefRspEncode				*/
/* Encodes GSE Management response including Ethertype header (14 bytes)*/
/************************************************************************/
ST_UCHAR *getGsRefRspEncode (GSE_REF_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
  {
ST_INT index;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	 */
				/* init to NULL (i.e. failure)		 */

  asn1r_strt_asn1_bld (&aCtx, asn1DataBuf, asn1DataBufLen);

  asn1r_strt_constr (&aCtx);	/* Start APPLICATION 0 IMPLICIT SEQUENCE */
  asn1r_strt_constr (&aCtx); 	/* Start GSEMgmtReponses		 */
  asn1r_strt_constr (&aCtx); 	/* Start "...ResponsePdu"		 */

  asn1r_strt_constr (&aCtx); 	/* Start POS RSP IMPLICIT SEQ		  */
  asn1r_strt_constr (&aCtx); 	/* Start POS RSP REQ RESULT IMPLICIT SEQ  */
  
  for (index = ctrl->numResults - 1; index >= 0; index--)
    {
    switch (ctrl->result[index].rsp_type)
      {
      case SD_SUCCESS:
	asn1r_wr_vstr (&aCtx, ctrl->result[index].reference);
        asn1r_fin_prim (&aCtx, 1,CTX);
	break;
      
      case SD_FAILURE:
        asn1r_wr_u32 (&aCtx, ctrl->result[index].error);
	asn1r_fin_prim (&aCtx, 2,CTX);
	break;
      }
    }	/* end loop	*/
  asn1r_fin_constr (&aCtx,1,CTX,DEF);	/* POS RSP REQ RESULT IMPLICIT SEQ	*/

  asn1r_wr_vstr (&aCtx, ctrl->datSet);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,2,CTX,DEF);	/* POS RSP IMPLICIT SEQ			*/

  asn1r_wr_u32 (&aCtx, ctrl->confRev);
  asn1r_fin_prim (&aCtx,1,CTX);		

  asn1r_wr_vstr (&aCtx, ctrl->ident);
  asn1r_fin_prim (&aCtx,0,CTX);
  
  asn1r_fin_constr (&aCtx,GET_GS_REF_RSP_TAG,CTX,DEF);	/* finish ResponsePdu	  */
  asn1r_fin_constr (&aCtx,2,CTX,DEF);			/* finish GSEMgmtResonses */

  asn1r_wr_u32 (&aCtx, ctrl->stateID);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,0,APP,DEF);		/* finish APPLICATION 0 IMPLICIT SEQ  */

  /* ASN.1 encoding done. Chk for overrun. */
  if (aCtx.asn1r_encode_overrun)
    {
    ACSELOG_ERR0 ("GSE MGMT encode overrun");
    }
  else
    {
    *asn1DataLenOut = ASN1_ENC_LEN(&aCtx);	/* len encoded	*/
      
    /* Finally, add the Ethertype information */
    /* This is the only place we set return to GOOD value.		*/
    asn1Start = etype_hdr_encode(ASN1_ENC_PTR(&aCtx),  /* buffer to encode into */
        		           asn1DataBufLen,         /* len of buffer 	*/
	       		           asn1DataLenOut,         /* ptr to len encoded    */
	       		           etype_info);        /* ptr etype struct      */
    }
    
  /* If all is OK so far, finish encoding PDU by adding MAC addresses.	*/
  if (asn1Start)
    {
    if (asn1Start > asn1DataBuf + 12)
      {	/* there's room for MAC addresses so add them now	*/
      memcpy (asn1Start-12, dstMac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
      memcpy (asn1Start-6, srcMac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
      asn1Start -= 12;		/* point to start of "complete" PDU	*/
      *asn1DataLenOut += 12;
      }
    else
      asn1Start = NULL;	/* ERROR: not enough room to finish encode	*/
    }
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }


/************************************************************************/
/*			getGSSEDataOffsetRspEncode			*/
/* Encodes GSE Management response including Ethertype header (14 bytes)*/
/************************************************************************/
ST_UCHAR *getGSSEDataOffsetRspEncode (GSE_OFFSET_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac)	/* source MAC address	    */
 {
ST_INT index;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	 */
				/* init to NULL (i.e. failure)		 */

  asn1r_strt_asn1_bld (&aCtx, asn1DataBuf, asn1DataBufLen);

  asn1r_strt_constr (&aCtx);	/* Start APPLICATION 0 IMPLICIT SEQUENCE */
  asn1r_strt_constr (&aCtx); 	/* Start GSEMgmtReponses		 */
  asn1r_strt_constr (&aCtx); 	/* Start "...ResponsePdu"		 */

  asn1r_strt_constr (&aCtx); 	/* Start POS RSP IMPLICIT SEQ		  */
  asn1r_strt_constr (&aCtx); 	/* Start POS RSP REQ RESULT IMPLICIT SEQ  */
  
  for (index = ctrl->numResults - 1; index >= 0; index--)
    {
    switch (ctrl->result[index].rsp_type)
      {
      case SD_SUCCESS:
        asn1r_wr_u32 (&aCtx, ctrl->result[index].offset);
        asn1r_fin_prim (&aCtx, 0,CTX);
	break;

      case SD_FAILURE:
        asn1r_wr_u32 (&aCtx, ctrl->result[index].error);
	asn1r_fin_prim (&aCtx, 2,CTX);
	break;
      }
    }	/* end loop	*/
  asn1r_fin_constr (&aCtx,1,CTX,DEF);	/* POS RSP REQ RESULT IMPLICIT SEQ	*/

  asn1r_wr_vstr (&aCtx, ctrl->datSet);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,2,CTX,DEF);	/* POS RSP IMPLICIT SEQ			*/

  asn1r_wr_u32 (&aCtx, ctrl->confRev);
  asn1r_fin_prim (&aCtx,1,CTX);		

  asn1r_wr_vstr (&aCtx, ctrl->ident);
  asn1r_fin_prim (&aCtx,0,CTX);
  
  asn1r_fin_constr (&aCtx,GET_GSSE_DATA_OFF_RSP_TAG,CTX,DEF);	/* finish ResponsePdu	  */
  asn1r_fin_constr (&aCtx,2,CTX,DEF);				/* finish GSEMgmtResonses */

  asn1r_wr_u32 (&aCtx, ctrl->stateID);
  asn1r_fin_prim (&aCtx,0,CTX);

  asn1r_fin_constr (&aCtx,0,APP,DEF);		/* finish APPLICATION 0 IMPLICIT SEQ  */

  /* ASN.1 encoding done. Chk for overrun. */
  if (aCtx.asn1r_encode_overrun)
    {
    ACSELOG_ERR0 ("GSE MGMT encode overrun");
    }
  else
    {
    *asn1DataLenOut = ASN1_ENC_LEN(&aCtx);	/* len encoded	*/
      
    /* Finally, add the Ethertype information */
    /* This is the only place we set return to GOOD value.		*/
    asn1Start = etype_hdr_encode (ASN1_ENC_PTR(&aCtx),  /* buffer to encode into */
        		           asn1DataBufLen,         /* len of buffer 	*/
	       		           asn1DataLenOut,         /* ptr to len encoded    */
	       		           etype_info);        /* ptr etype struct      */
    }
    
  /* If all is OK so far, finish encoding PDU by adding MAC addresses.	*/
  if (asn1Start)
    {
    if (asn1Start > asn1DataBuf + 12)
      {	/* there's room for MAC addresses so add them now	*/
      memcpy (asn1Start-12, dstMac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
      memcpy (asn1Start-6, srcMac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
      asn1Start -= 12;		/* point to start of "complete" PDU	*/
      *asn1DataLenOut += 12;
      }
    else
      asn1Start = NULL;	/* ERROR: not enough room to finish encode	*/
    }
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }


