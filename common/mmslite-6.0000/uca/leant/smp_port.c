#if defined(UCA_SMP)
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1997, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smp_port.c						*/
/* PRODUCT(S)  : UCA Station Management Protocol (UCA SMP).		*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module provides user functions for SMP.			*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/27/00  MDE    04     Lint cleanup					*/
/* 09/13/99  MDE    03     Added SD_CONST modifiers			*/
/* 10/15/97  MDE    02     Changed constants to longs, fixed pragma	*/
/* 09/23/97  EJV    01     Created					*/
/************************************************************************/
#if defined (_WIN32)
#include <windows.h>
#include <mmsystem.h>
#endif

#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "smp_usr.h"
#include "smp_log.h"

#define SMP_SEC_IN_DAY		(24L*60L*60L)
#if defined(_WIN32)
#define SMP_MSEC_IN_DAY		(24L*60L*60L*1000L)
#define SMP_USEC_IN_DAY		(24L*60L*60L*1000L*1000L)
#endif  /* _WIN32 */


	/*======================================================*/
	/*  SMP SLAVE functions implemented by user		*/
	/*======================================================*/

/************************************************************************/
/*			u_smp_time_sync_ind				*/
/*----------------------------------------------------------------------*/
/* SLAVE function implemented by user to synchronize time.		*/
/*									*/
/* Parameters:								*/
/*			SLAVE side:					*/
/*	SMP_TIME *a1	recorded time of Measure-RequestPDU  reception	*/
/*	SMP_TIME *a2	recorded time of Measure-ResponsePDU sending	*/
/*			MASTER side:					*/
/*	SMP_TIME *b1	recorded time of Measure-RequestPDU  sending	*/
/*	SMP_TIME *b2	recorded time of Measure-ResponsePDU arrival	*/
/*	SMP_TIME *delta_time return the local_adjustement.		*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET u_smp_time_sync_ind (SMP_TIME *a1, SMP_TIME *a2,
			    SMP_TIME *b1, SMP_TIME *b2,
			    SMP_TIME *delta_time)
{
#if defined(MSDOS) || defined(__MSDOS__)

ST_INT32 path_latency;
ST_INT32 local_adjust;
ST_INT32 A1, A2, B1, B2;	/* times in sec */

  /* check if time is corrupted */
  if ( a2->days <  a1->days ||
      (a2->days == a1->days && a2->msec < a1->msec))
    return (SD_FAILURE);

  if ( b2->days < b1->days ||
      (b2->days == b1->days && b2->msec < b1->msec))
    return (SD_FAILURE);

  A1 = a1->days * SMP_SEC_IN_DAY + a1->msec / 1000;
  A2 = a2->days * SMP_SEC_IN_DAY + a2->msec / 1000;
  B1 = b1->days * SMP_SEC_IN_DAY + b1->msec / 1000;
  B2 = b2->days * SMP_SEC_IN_DAY + b1->msec / 1000;

  path_latency = ((A2-A1) - (B2-B1)) / 2;
  local_adjust =  B1 - path_latency - A1; /* in seconds */

  /* set the return value */
  delta_time->days = local_adjust / SMP_SEC_IN_DAY;
  delta_time->msec = (local_adjust - delta_time->days * SMP_SEC_IN_DAY) * 1000;
  delta_time->usec = 0;				/* not implemented	*/


	/*======================================================*/
	/* USER should implement the Time Synchronization below	*/
	/*======================================================*/

#pragma message("Warning: THE SMP TIME SYNCHRONIZATION FUNCTION NEEDS TO BE IMPLEMENTED")

  return (SD_SUCCESS);
#endif  /* MSDOS || __MSDOS__ */

	/*=====================================================*/

#if defined(_WIN32)

ST_INT64 path_latency;
ST_INT64 local_adjust;
ST_INT64 A1, A2, B1, B2;	/* times in msec */

  /* check if time is corrupted */
  if ( a2->days <  a1->days ||
      (a2->days == a1->days && a2->msec < a1->msec))
    return (SD_FAILURE);

  if ( b2->days < b1->days ||
      (b2->days == b1->days && b2->msec < b1->msec))
    return (SD_FAILURE);

  A1 = (ST_INT64) a1->days * SMP_MSEC_IN_DAY + (ST_INT64) a1->msec;
  A2 = (ST_INT64) a2->days * SMP_MSEC_IN_DAY + (ST_INT64) a2->msec;
  B1 = (ST_INT64) b1->days * SMP_MSEC_IN_DAY + (ST_INT64) b1->msec;
  B2 = (ST_INT64) b2->days * SMP_MSEC_IN_DAY + (ST_INT64) b2->msec;

  SMP_LOGC_DEB2 ("A2:           %16I64X (%I64u) ", A2, A2);
  SMP_LOGC_DEB2 ("A1:           %16I64X (%I64u) ", A1, A1);
  SMP_LOGC_DEB2 ("B2:           %16I64X (%I64u) ", B2, B2);
  SMP_LOGC_DEB2 ("B1:           %16I64X (%I64u) ", B1, B1);

  path_latency = ((ST_INT64)(A2-A1) - (ST_INT64)(B2-B1)) / 2;
  local_adjust =  B1 - path_latency - A1;
					/* in milliseconds */
  SMP_LOGC_DEB2 ("Latency:      %16I64X (%I64d)", path_latency, path_latency);
  SMP_LOGC_DEB2 ("Adjust:       %16I64X (%I64d)", local_adjust, local_adjust);

  /* set the return value */
  delta_time->days = (ST_INT32) (local_adjust / SMP_MSEC_IN_DAY);
  delta_time->msec = (ST_INT32) (local_adjust - (ST_INT64) delta_time->days * SMP_MSEC_IN_DAY);
  delta_time->usec = 0;				/* not implemented	*/

  SMP_LOGC_DEB2 ("delta->days:  %08X (%ld) ", delta_time->days, delta_time->days);
  SMP_LOGC_DEB2 ("delta->msec:  %08X (%ld) \n", delta_time->msec, delta_time->msec);

	/*======================================================*/
	/* USER should implement the Time Synchronization below	*/
	/*======================================================*/

#pragma message("Warning: THE SMP TIME SYNCHRONIZATION FUNCTION NEEDS TO BE IMPLEMENTED")

  return (SD_SUCCESS);
#endif  /* _WIN32 */

  /* other systems not implemented */
  return (SD_FAILURE);
}



#endif  /* UCA_SMP */

