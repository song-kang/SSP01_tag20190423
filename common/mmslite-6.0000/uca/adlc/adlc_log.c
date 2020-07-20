/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_log.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* 	This module contains functions for logging ADLC Statistics	*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    10     Cleaned up general PClint errors             */
/* 09/23/02  KCR    09     Removed extra include files                  */
/* 09/23/02  KCR    08     Moved thisFileName before includes           */
/* 09/13/99  MDE    07     Added SD_CONST modifiers         		*/
/* 07/16/97  IKE    06     Added device statistics and device filt log	*/
/* 05/12/97  IKE    05     Changed to MMSEASE 7.0 Data Types		*/
/* 05/22/96  IKE    04     Header file cleanup. Added some more stats 	*/
/*			   vars.					*/
/* 04/05/96  rkr    03     Header file and SAVE_CALLING cleanup 	*/
/* 04/04/96  rkr    02	   Cleanup					*/
/* 01/10/96  IKE   1.00    Initial release                             	*/
/************************************************************************/
static char *thisFileName = __FILE__;
/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

/* Some other SISCO files						*/
#include "cfg_util.h"
#include "memlog.h"

/* ADLC files								*/
#include "adlc.h"

/************************************************************************/
/*		              logAdlcStats				*/
/************************************************************************/
ST_VOID logAdlcStats(ST_VOID)
  {
  ADLC_LOG_ALWAYS4("txFrames = %lu rxFrames = %lu goodFrames = %lu badFrames = %lu",
  		    adlcStats.txFrames,adlcStats.rxFrames,adlcStats.goodFrames,adlcStats.badFrames);
  ADLC_LOG_CALWAYS4("discardedBytes = %lu mCastFrames = %lu txPackets = %lu rxPackets = %lu",
  		    adlcStats.discardedBytes,adlcStats.mCastFrames,adlcStats.txPackets,adlcStats.rxPackets);	
  ADLC_LOG_CALWAYS4("rxInfo = %lu rxRr = %lu rxRnr = %lu rxRej = %lu",
		    adlcStats.rxInfo,adlcStats.rxRr,adlcStats.rxRnr,adlcStats.rxRej);	
  ADLC_LOG_CALWAYS4("rxUi = %lu txUFrames = %lu   txSFrames = %lu txMFrames = %lu",
  		    adlcStats.rxUi,adlcStats.txUFrames,adlcStats.txSFrames,adlcStats.txMFrames);
  ADLC_LOG_CALWAYS3("txIFrames = %lu timeout = %lu retransmits = %lu",
  		    adlcStats.txIFrames,adlcStats.timeout,adlcStats.retransmits);
#if defined(ADLC_MASTER)
  ADLC_LOG_CALWAYS4("rxUa = %lu rxRd = %lu rxDm = %lu rxFrmr = %lu",
		     adlcStats.rxUa,adlcStats.rxRd,adlcStats.rxDm,adlcStats.rxFrmr);	
  ADLC_LOG_CALWAYS1("txRFrames = %lu",adlcStats.txRFrames);
#endif /* ADLC_MASTER */
#if defined(ADLC_SLAVE)
  ADLC_LOG_CALWAYS2("rxSnrm = %lu rxDisc = %lu",adlcStats.rxSnrm,adlcStats.rxDisc);
#endif /* ADLC_SLAVE */
  }
