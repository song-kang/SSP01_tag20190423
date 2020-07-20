/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1999-2002, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : gse_mgmt.h 							*/
/* PRODUCT(S)  : 	 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/03/06  CRM     01    Created					*/
/************************************************************************/

#ifndef GSE_MGMT_INCLUDED
#define GSE_MGMT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "acse2usr.h"
#include "asn1r.h"
#include "ethertyp.h"



#define MAX_GSE_MGMT_ENCODE_BUF	      1540
#define MAX_VSTRING_BUF		      65
#define NUM_ELEMENTS		      500

#define GET_GO_REF_REQ_TAG	      1
#define GET_GOOSE_ELE_NUM_REQ_TAG     2
#define GET_GS_REF_REQ_TAG	      3
#define GET_GSSE_DATA_OFF_REQ_TAG     4

#define GET_GO_REF_RSP_TAG	      1
#define GET_GOOSE_ELE_NUM_RSP_TAG     2
#define GET_GS_REF_RSP_TAG	      3
#define GET_GSSE_DATA_OFF_RSP_TAG     4

#define RSP_OTHER		      0
#define RSP_NOT_FOUND		      1

#define GLB_ERROR		      2
#define GLB_ERR_OTHER		      0
#define GLB_ERR_UNKNOWN_CTRL_BLK      1
#define GLB_ERR_RSP_TOO_LARGE	      2
#define GLB_ERR_CTRL_BLK_CONFIG_ERR   3


#define GSE_MSG_TYPE_GO_REF_REQ			1
#define GSE_MSG_TYPE_GOOSE_ELE_REQ		2
#define GSE_MSG_TYPE_GS_REF_REQ			3
#define GSE_MSG_TYPE_GSSE_DATA_OFF_REQ		4

#define GSE_MSG_TYPE_GLOBAL_ERROR_RSP	  	10
#define GSE_MSG_TYPE_GO_REF_RSP			11
#define GSE_MSG_TYPE_GOOSE_ELE_RSP		12
#define GSE_MSG_TYPE_GS_REF_RSP			13
#define GSE_MSG_TYPE_GSSE_DATA_OFF_RSP		14



/************************************************************************/
/* BEGIN GSE MGMT DEFINITIONS						*/
/************************************************************************/
typedef struct
  {
  ST_UINT32 stateID;
  ST_CHAR   ident[MAX_VSTRING_BUF+1];
  ST_INT    numIntegers;
  ST_UINT32 *offset;
  } GSE_REF_REQ; 

typedef struct
  {
  ST_UINT32 stateID;
  ST_CHAR   ident[MAX_VSTRING_BUF+1];
  ST_INT    numVStrings;
  ST_CHAR **references;
  } GSE_OFFSET_REQ; 

/* Global error response sturcture */
typedef struct
  {
  ST_UINT32 stateID;
  ST_CHAR   ident[MAX_VSTRING_BUF+1];
  ST_UINT32 confRev;
  ST_UINT32 glbError;
  } GSE_GLB_ERR_RSP; 


/* Offset response sturctures */
typedef struct
  {
  ST_INT    rsp_type;
  ST_UINT32 offset;
  ST_UINT32 error;
  } OFFSET_REQ_RESULTS; 

typedef struct
  {
  ST_UINT32 stateID;
  ST_CHAR   ident[MAX_VSTRING_BUF+1];
  ST_UINT32 confRev;
  ST_CHAR   datSet[MAX_VSTRING_BUF+1];
  ST_INT    numResults;
  OFFSET_REQ_RESULTS *result;
  } GSE_OFFSET_RSP; 


/* Reference response sturctures */
typedef struct
  {
  ST_INT    rsp_type;
  ST_CHAR   reference[MAX_VSTRING_BUF+1];
  ST_UINT32 error;
  } REF_REQ_RESULTS; 


typedef struct
  {
  ST_UINT32 stateID;
  ST_CHAR   ident[MAX_VSTRING_BUF+1];
  ST_UINT32 confRev;
  ST_CHAR   datSet[MAX_VSTRING_BUF+1];
  ST_INT    numResults;
  REF_REQ_RESULTS *result;
  } GSE_REF_RSP; 



/************************************************************************/
/* STRUCTURE DEFINITIONS						*/
/************************************************************************/

typedef struct
  {
  ST_UINT32 stateID;
  ST_INT    msgType;
  
  union GSE_MSG
    {
    GSE_REF_REQ	    refReq;
    GSE_OFFSET_REQ  offReq;
    GSE_OFFSET_RSP  offRsp;
    GSE_REF_RSP	    refRsp;
    GSE_GLB_ERR_RSP glbErrRsp;
    } msg;
  } GSE_MGMT_MSG;




ST_UCHAR *getGoRefReqEncode (GSE_REF_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */
ST_UCHAR *getGOOSEEleNumReqEncode (GSE_OFFSET_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info,	/* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */
ST_UCHAR *getGsRefReqEncode (GSE_REF_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */
ST_UCHAR *getGSSEDataOffsetReqEncode (GSE_OFFSET_REQ *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */



ST_UCHAR *gse_mgmt_NotSupported (ST_VOID);
ST_UCHAR *getGlbErrorRspEncode (GSE_GLB_ERR_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */
ST_UCHAR *getGoRefRspEncode (GSE_REF_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */
ST_UCHAR *getGOOSEEleNumRspEncode (GSE_OFFSET_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */
ST_UCHAR *getGsRefRspEncode (GSE_REF_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */
ST_UCHAR *getGSSEDataOffsetRspEncode (GSE_OFFSET_RSP *ctrl,
	ST_UCHAR *asn1DataBuf,	/* buffer to encode in	    */
	ST_INT asn1DataBufLen,	/* len of buffer	    */
	ST_INT *asn1DataLenOut,	/* ptr to len encoded	    */
	ETYPE_INFO *etype_info, /* ptr to Ethertype info    */
	ST_UINT8 *dstMac,	/* destination MAC address  */
	ST_UINT8 *srcMac);	/* source MAC address	    */



/************************************************************************/
/* Functions for receiving GSE_MGMT messages.				*/
/* Decode GSE_MGMT message and fill in GSE_MGMT_MSG struct.		*/
/************************************************************************/
GSE_MGMT_MSG *gse_mgmt_msg_decode (
	SN_UNITDATA *sn_req,	/* message to decode			*/
	ETYPE_INFO *etypeInfo);	/* where to store decoded Ethertype info*/

/* Must be called to free structure returned from gse_mgmt_msg_decode.	*/
ST_VOID gse_mgmt_msg_free (GSE_MGMT_MSG *gseMgmtMsg);


/************************************************************************/
/* END GSE MGMT DEFINITIONS						*/
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* GSE_MGMT_INCLUDED */
