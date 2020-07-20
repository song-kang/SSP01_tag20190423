/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2007, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_err3.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the MMS error functions for decoding	*/
/*	& encoding the error response PDUs :				*/
/*		conclude-ErrorPDU 					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/01/07  EJV     06    Rem obsolete LLC30... code.			*/
/* 03/09/05  EJV     05    Fixed thisFileName for MMS-EASE              */
/* 03/17/04  RKR     04    Changed thisFileName                         */
/* 03/11/04  GLB     03    Remove "thisFileName"                    	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pcon.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && !defined(MMS_LITE)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#ifndef MMS_LITE
/************************************************************************/
/*			mp_conclude_err		 			*/
/************************************************************************/

ST_RET mp_conclude_err (MMSREQ_IND *indptr, ST_INT16 class, ST_INT16 code)
  {
ST_RET ret;

  S_LOCK_COMMON_RESOURCES ();
  mms_chan_info[indptr->chan].ctxt.chan_state &= ~M_CONCL_URESP_WAIT;
  ret = mp_err_resp (indptr,class,code);
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#else
/************************************************************************/
/*			mpl_conclude_err 				*/
/************************************************************************/

ST_RET mpl_conclude_err (ST_INT16 err_class, ST_INT16 code)
  {
  return (mpl_err_resp (MMSOP_CONCLUDE,0L,err_class,code));
  }

#endif


