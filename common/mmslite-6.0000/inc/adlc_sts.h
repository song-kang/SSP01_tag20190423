/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-1996, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_sts.h						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*									*/
/* MODULE DESCRIPTION :	This header file defines data structures	*/
/*			and types for rli adlc link manager.		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 07/16/97  IKE    05     Added device statistics and device filt log	*/
/* 05/12/97  IKE    04     Changed to MMSEASE 7.0 Data Types		*/
/* 04/10/97  EJV    03     Deletes defines ADLC_MASTER, ADLC_SLAVE	*/
/* 10/23/96  IKE    02     Added statsResetTime				*/
/* 09/30/96  IKE    01     Created					*/
/************************************************************************/

#ifndef ADLC_STS_H
#define ADLC_STS_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/  

/************************************************************************/  
typedef struct tagAdlcStats
  {
  ST_LONG statsResetTime; /* time stats last reset 			*/
  ST_ULONG txFrames;   	/* Total transmitted Frames			*/
  ST_ULONG rxFrames;  	/* Total received Frames			*/
  ST_ULONG goodFrames;	/* Total good frames received			*/
  ST_ULONG badFrames;   /* Total bad frames when sof was ok		*/
  ST_ULONG discardedBytes;	/* Total discarded bytes		*/
  ST_ULONG mCastFrames;	/* Total Multicast Frames			*/
  ST_ULONG txPackets;	/* Total Transmitted MMS LSDU			*/
  ST_ULONG rxPackets;	/* Total Received MMS LSDU			*/
  ST_ULONG rxInfo;		/* Total Received INFO frames			*/
  ST_ULONG rxRr;		/* Total Received RR frames			*/
  ST_ULONG rxRnr;		/* Total Received RNR frames			*/
  ST_ULONG rxRej;		/* Total Received REJ frames			*/
  ST_ULONG rxUi;		/* Total Received UI frames			*/
  ST_ULONG txUFrames;	/* Total Transmitted Unumbered frames		*/
  ST_ULONG txSFrames;	/* Total Transmitted Supervisory frames		*/
  ST_ULONG txMFrames;	/* Total Transmitted Multicast frames		*/
  ST_ULONG txIFrames;	/* Total Transmitted INFO frames		*/
  ST_ULONG txUiFrames;		/* Total Received UI frames			*/
  ST_ULONG timeout;	/* Total Timeouts				*/
  ST_ULONG retransmits;		/* Total retransmit INFO frames 	*/
  ST_ULONG rxUa;		/* Total Received UA frames		*/
  ST_ULONG rxRd;		/* Total Received RD frames		*/
  ST_ULONG rxDm;		/* Total Received DM frames		*/
  ST_ULONG rxFrmr;		/* Total Received FRMR frames		*/
  ST_ULONG txRFrames;	/* Total Received Redirect frames		*/
  ST_ULONG rxSnrm;		/* Total Received SNRM frames			*/
  ST_ULONG rxDisc;		/* Total Received DISC frames			*/
  } ADLC_STATS;

#if defined(__OS2__) || defined(_WIN32)
typedef struct tagAdlcDevStats
  {
  ST_ULONG txFrames;   	/* Total transmitted Frames			*/
  ST_ULONG rxFrames;  	/* Total received Frames			*/
  ST_ULONG timeout;	/* Total Timeouts				*/
  ST_ULONG retransmits;	/* Total retransmitted INFO frames 		*/
  } ADLC_DEV_STATS;
#endif

#ifdef __cplusplus
}
#endif

#endif /* ADLC_STS.H already included */

