/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2007 - 2007 All Rights Reserved					*/
/*									*/
/* MODULE NAME : crc_util.h						*/
/* PRODUCT(S)  : General Use						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	General purpose CRC computation functions.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    02     Add crc32table extern.			*/
/* 08/23/07  EJV    01     Extracted from other code for general use.	*/
/************************************************************************/
#ifndef CRC_UTIL_H

#define CRC_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

extern const ST_UINT crc32table[];

ST_UINT32 sCrc32OnString (ST_CHAR *str, ST_UINT32 crc);
ST_UINT32 sCrc32OnBuffer (ST_CHAR *buf, ST_UINT len, ST_UINT32 crc);


#ifdef __cplusplus
}
#endif
	
#endif  /* CRC_UTIL_H */
