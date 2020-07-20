/***********************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : msaltas.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/13/99  MDE     01    Added SD_CONST modifiers			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Stub functions, included to allow transparent linking		*/
/************************************************************************/

SD_CONST static ST_CHAR *nfStubMsg = "Non-functional stub function called";

/************************************************************************/
/************************************************************************/
/* The real version of these functions are found in mms_alta.c, and are */
/* packaged differently for MMS_LITE.					*/


ST_RET ms_asn1_to_aa (ST_UCHAR *asn1ptr, ST_INT asn1len, 
		     ALT_ACCESS *alt_acc_out)
  {
  MLOG_NERR0 (nfStubMsg);
  return (SD_FAILURE);
  }

ST_RET ms_aa_to_asn1 (ALT_ACCESS *alt_acc)
  {
  MLOG_NERR0 (nfStubMsg);
  return (SD_FAILURE);
  }

ST_VOID ms_log_alt_access (ALT_ACCESS *alt_acc)
  {
  MLOG_NERR0 (nfStubMsg);
  }





