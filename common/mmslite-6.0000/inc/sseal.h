/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  2008 - 2011, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : sseal.h						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/23/11  EJV     	   Del comment.					*/
/* 02/22/11  EJV     	   Chg C++ comments to standard C comment.	*/
/* 01/18/09  MDE     03    Made seal time 32 bit			*/
/* 01/12/09  MDE     02    Moved SSEAL_LIB_INITIAL_CRC64 define		*/
/* 09/23/08  MDE     01    New						*/
/************************************************************************/
/************************************************************************/

#ifndef SSEAL_INCLUDED
#define SSEAL_INCLUDED

/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************/


/************************************************************************/

#include "glbtypes.h"

/************************************************************************/

/* Error codes */
#define S_SEAL_KEYSEL			1
#define S_SEAL_BUFSIZE_REMAINDER	2
#define S_SEAL_ECRC_MISMATCH		3
#define S_SEAL_CRC_MISMATCH		4
#define S_SEAL_DATA_CRC_MISMATCH 	5
#define S_SEAL_TIME_WINDOW		6

/************************************************************************/
/************************************************************************/
/* SEAL & SKIPJACK Routines */

/* SKIPJACK keys are 80 bits */

typedef ST_UCHAR SJ_KEY[10];
typedef struct 
  {
  ST_UINT32	dataCrc;	/* External (& opaque) data CRC		*/
  ST_INT32	currTime;
  ST_INT32	keySel;
  ST_UINT32	crc[2];	   	/* CRC over currTime & keySel		*/
  ST_UINT32	eCrc[2];  	/* Encrypted CRC	 		*/
  } S_SEAL;

ST_RET sMakeSeal  (ST_INT32 keySel, ST_INT numKeys, SJ_KEY *keyArray, 
		   ST_UINT32 dataCrc, S_SEAL *destSeal);

ST_RET sCheckSeal (S_SEAL *srcSeal, ST_INT numKeys, SJ_KEY *keyArray, 
		   ST_UINT32 dataCrc, ST_INT32 timeWindow);

ST_RET sSkipjack32 (ST_INT keySel, ST_INT numKeys, SJ_KEY *keyArray,
		    ST_INT *dataIn, ST_INT *dataOut, ST_BOOLEAN encrypt);

ST_RET sSkipjackBuffer (ST_INT keySel, ST_INT numKeys, SJ_KEY *keyArray, 
			ST_INT dataLen, void *inBuf, ST_VOID *outBuf, ST_BOOLEAN encrypt);

/************************************************************************/
/* CRC Routines */

/* Suggested initial value for CRC's */
#define SSEAL_LIB_INITIAL_CRC32    0xFFFFFFFF

ST_VOID sBufCrc32 (ST_VOID *vBuf, ST_INT numBytes, ST_UINT32 *crcInOut, ST_BOOLEAN backwards);

/* Some simple versions */

ST_VOID sStrCrc32 (ST_CHAR *str, ST_UINT32 *crcInOut);
ST_VOID sIntCrc32 (ST_INT val, ST_UINT32 *crcInOut);

#ifdef SSEAL_CRC64_SUPPORT
/* Suggested initial value for CRC's */
#define SSEAL_LIB_INITIAL_CRC64    0xFFFFFFFFFFFFFFFFULL

ST_VOID sBufCrc64 (ST_VOID *vBuf, ST_INT numBytes, ST_UINT64 *crcInOut, ST_BOOLEAN backwards);
#endif


/* Application configuration, not used in SSEAL but rather in protocol	*/
/* sealMode defines 		*/
#define SSEAL_MODE_NONE		0
#define SSEAL_MODE_FALLBACK	1
#define SSEAL_MODE_ALL		2

/************************************************************************/
#ifdef __cplusplus
}
#endif


#endif /* #ifndef SSEAL_INCLUDED */
