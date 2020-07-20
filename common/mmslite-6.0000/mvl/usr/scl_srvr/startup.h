#ifndef STARTUP_INCLUDED
#define STARTUP_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2004 All Rights Reserved					*/
/*									*/
/* MODULE NAME : startup.h						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Structures and functions to read "startup.cfg".			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/07/12  JRB	   Add BRCBBufferSize,LogScanRateMs,LogMaxEntries*/
/* 07/06/04  JRB    02     Chg rpt_scan_rate to ST_UINT num of millisec.*/
/* 06/08/04  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "mms_def2.h"		/* need MAX_IDENT_LEN	*/

typedef struct
  {
  ST_CHAR scl_filename [MAX_IDENT_LEN+1];
  ST_CHAR ied_name [MAX_IDENT_LEN+1];
  ST_CHAR access_point_name [MAX_IDENT_LEN+1];
  ST_UINT report_scan_rate;	/* Report scan rate in milliseconds	*/
  ST_UINT BRCBBufferSize;	/* Buffered report buffer size.		*/
  ST_UINT LogScanRateMs;	/* Log scan rate in milliseconds	*/
  ST_UINT LogMaxEntries;	/* Maximim number of entries in Log	*/
  } STARTUP_CFG;

/************************************************************************/
/*			startup_cfg_read				*/
/* Reads "startup.cfg" input file & fills in STARTUP_CFG struct.	*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
ST_RET startup_cfg_read (
	ST_CHAR *in_filename,	/* usually "startup.cfg"	*/
	STARTUP_CFG *startup_cfg);

#endif	/* !STARTUP_INCLUDED	*/
