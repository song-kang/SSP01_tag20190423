/***********************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : msdataas.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/02/02  JRB     04    Convert to use ASN1R (re-entrant ASN1).	*/
/* 04/28/00  JRB     03    Lint cleanup					*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/09/97  MDE     01    Modified Runtime Type handling		*/
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

#ifdef DEBUG_SISCO
static ST_CHAR *nfStubMsg = "Non-functional stub function called";
#endif

/************************************************************************/
/************************************************************************/
/* The real version of these functions are found in mmsdataa.c		*/

#ifndef MMS_LITE

ST_RET ms_asn1_to_locl_aa (NAMED_TYPE *tptr,
                          ALT_ACCESS *alt_acc,
                          ST_UCHAR *asn1ptr, /* pointer to ASN.1 'Data'    */
                          ST_INT asn1len,    /* length of ASN.1 'Data'     */
                          ST_CHAR *dptr)     /* local data buffer pointer  */
  {
  MLOG_NERR0 (nfStubMsg);
  return (SD_FAILURE);
  }
#endif

ST_RET ms_asn1_to_local_aa (SD_CONST RUNTIME_TYPE *rt_head,
                           ST_INT rt_num,
                           ALT_ACCESS *alt_acc,
                           ST_UCHAR *asn1ptr,
                           ST_INT asn1len,
                           ST_CHAR *dptr)
  {
  MLOG_NERR0 (nfStubMsg);
  return (SD_FAILURE);
  }

/************************************************************************/
/* The real version of these functions are found in mmsdataa.c		*/

#ifndef MMS_LITE

ST_RET ms_locl_to_asn1_aa (ASN1_ENC_CTXT *aCtx, NAMED_TYPE *tptr, 
                          ALT_ACCESS *alt_acc,
			  ST_CHAR *dptr)
  {
  MLOG_NERR0 (nfStubMsg);
  return (SD_FAILURE);
  }
#endif


ST_RET ms_local_to_asn1_aa (ASN1_ENC_CTXT *aCtx, SD_CONST RUNTIME_TYPE *rt_head,
                          ST_INT rt_num,
                          ALT_ACCESS *alt_acc,
                          ST_CHAR *dptr)
  {
  MLOG_NERR0 (nfStubMsg);
  return (SD_FAILURE);
  }

