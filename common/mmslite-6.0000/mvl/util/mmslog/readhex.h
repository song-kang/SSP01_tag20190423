/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 All Rights Reserved					*/
/*									*/
/* MODULE NAME : readhex.h						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : Header file for ReadHex.c			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*                                                                      */
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/04/98  DWL     03    Changed slog header vars to extern.		*/
/* 07/15/98  DWL     02    Added header support.			*/
/* 08/27/97  DWL     01    Created					*/
/************************************************************************/

ST_VOID u_readhex_pdu (ST_UCHAR *pdu_buf, ST_INT pdu_len);
ST_VOID u_readhex_mmspdu (ST_UCHAR *pdu_buf, ST_INT pdu_len, 
			  ST_CHAR *header, ST_CHAR *info);

ST_RET readHex (ST_CHAR *fileName, ST_CHAR *dest, ST_INT destSize, 
		ST_INT ftype);

ST_INT xtoi (ST_CHAR *buf);

#define  SLOG_FILE		1
#define  NETWORK_ANALYZER	2
#define  NETSIGHT		3
#define  PLAIN_HEX		4
#define  SPACE			32
#define  LF			10

#define  NUM_OCTETS_PER_LINE		16

#define MAX_NUM_SLOG_HEADER_LINES 	10

/* Global vars								*/
extern ST_INT numSlogHeaderLines;
extern ST_CHAR *slogHeaderLines[MAX_NUM_SLOG_HEADER_LINES];
