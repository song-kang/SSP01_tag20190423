/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_sta.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file defines the statistics for the		*/
/*		Connectionless-mode Network Protocol (CLNP).		*/
/*									*/
/*	Note:	Compile the whole project with CLNP_STAT define to	*/
/*		keep statistics of CLNP operation.			*/
/*									*/
/* GLOBAL STRUCTURES DEFINED IN THIS MODULE :				*/
/*									*/
/*			CLNP_STATS struct				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/23/96  EJV    01     Created					*/
/************************************************************************/


#ifndef CLNP_STA_INCLUDED
#define CLNP_STA_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

/* struct below describes the statistics recorded by the CLNP		*/

/* Statistics for PDUs */
typedef struct
  {
  ST_UINT cnt_sent;			/* Number of sent PDUs		*/
  ST_UINT cnt_rcvd;			/* Number of received PDUs	*/
  ST_UINT cnt_expired; 		/* Number of expired PDUs	*/
  ST_UINT cnt_discarded;		/* Number of discarded PDUs	*/
  }CLNP_PDU_STAT;
  
typedef struct
  {
  CLNP_PDU_STAT all_pdu;		/* stats for all PDUs	*/
  CLNP_PDU_STAT dt_pdu; 		/* stats for DT PDUs	*/
  CLNP_PDU_STAT er_pdu; 		/* stats for ER PDUs	*/
  }CLNP_STATS;
  
#define CLNP_STATS_LEN	sizeof (CLNP_STATS)


ST_RET     clnp_stats_reset (ST_VOID);
CLNP_STATS *clnp_stats_get (ST_VOID);


#if defined(CLNP_STAT)

  extern CLNP_STATS	clnp_stats;
  
  /* macros to increment all PDUs statistics */

  #define CLNP_STAT_INC_PDU_SENT      {clnp_stats.all_pdu.cnt_sent += 1;}
  #define CLNP_STAT_INC_PDU_RECEIVED  {clnp_stats.all_pdu.cnt_rcvd += 1;}
  #define CLNP_STAT_INC_PDU_EXPIRED   {clnp_stats.all_pdu.cnt_expired += 1;}
  #define CLNP_STAT_INC_PDU_DISCARDED {clnp_stats.all_pdu.cnt_discarded += 1;}

  /* macros to increment DT PDU statistics */

  #define CLNP_STAT_INC_DT_SENT      {clnp_stats.dt_pdu.cnt_sent += 1;}
  #define CLNP_STAT_INC_DT_RECEIVED  {clnp_stats.dt_pdu.cnt_rcvd += 1;}
  #define CLNP_STAT_INC_DT_EXPIRED   {clnp_stats.dt_pdu.cnt_expired += 1;}
  #define CLNP_STAT_INC_DT_DISCARDED {clnp_stats.dt_pdu.cnt_discarded += 1;}
  
  /* macros to increment ER PDU statistics */

  #define CLNP_STAT_INC_ER_SENT	     {clnp_stats.er_pdu.cnt_sent += 1;}
  #define CLNP_STAT_INC_ER_RECEIVED  {clnp_stats.er_pdu.cnt_rcvd += 1;}
  #define CLNP_STAT_INC_ER_EXPIRED   {clnp_stats.er_pdu.cnt_expired += 1;}
  #define CLNP_STAT_INC_ER_DISCARDED {clnp_stats.er_pdu.cnt_discarded += 1;}
  
#else

  /* macros if CLNP_STAT is not defined */

  /* macros to increment all PDU statistics */

  #define CLNP_STAT_INC_PDU_SENT
  #define CLNP_STAT_INC_PDU_RECEIVED
  #define CLNP_STAT_INC_PDU_EXPIRED
  #define CLNP_STAT_INC_PDU_DISCARDED

  /* macros to increment DT PDU statistics */

  #define CLNP_STAT_INC_DT_SENT
  #define CLNP_STAT_INC_DT_RECEIVED
  #define CLNP_STAT_INC_DT_EXPIRED
  #define CLNP_STAT_INC_DT_DISCARDED
  
  /* macros to increment ER PDU statistics */

  #define CLNP_STAT_INC_ER_SENT
  #define CLNP_STAT_INC_ER_RECEIVED
  #define CLNP_STAT_INC_ER_EXPIRED
  #define CLNP_STAT_INC_ER_DISCARDED
  
#endif /* end of CLNP_STAT defined */


#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/

