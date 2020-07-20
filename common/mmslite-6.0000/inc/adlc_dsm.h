/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-1996, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_dsm.h						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* This module contains functions that provide for serial port communi-	*/
/* cations for the Digiboard.						*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*	ST_INT sm_init(port,baud,parity,data,stop)			*/
/*	ST_INT sm_exit(port)						*/
/*      ST_INT sm_put (port,len,buffer)                             	*/
/* 	ST_INT sm_putc (port,ch)					*/
/*	ST_INT sm_rx_cnt (port)					*/
/*	ST_INT sm_get (port,len,buffer)				*/
/*      ST_INT sm_getc (port)                                       	*/
/*      ST_INT sm_rx_flush(port)					*/
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev      Comments                                    */
/* --------  ---  ------   -------------------------------------------  */
/* 05/12/97  IKE    09     Changed to MMSEASE 7.0 Data Types		*/
/* 02/21/97  IKE    08     Changed COM_TEST to ESSENTIAL_COM to make	*/
/*			   more sense					*/
/* 12/03/96  IKE    07     Moved some stuff to adlc_sm.h		*/
/* 09/19/96  IKE    06     Rearranged sm_get and sm_set functions	*/
/* 08/22/96  IKE    05     Added sm_get_dcd for contention avoidance	*/
/* 06/11/96  IKE    04     Added more functions to check RS232 signals  */
/* 05/22/96  IKE    03     Added functions to control RS232 signals	*/
/* 04/04/96  rkr    02	   Cleanup					*/
/* 11/12/95  IKE    1      Created					*/
/************************************************************************/

#ifndef _ADLC_DSM_H_
#define _ADLC_DSM_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESSENTIAL_COM)

ST_INT xc_entr (ST_INT);
ST_INT xc_exit(ST_VOID);
ST_INT xc_init(ST_INT, ST_INT, ST_INT, ST_INT, ST_INT);
ST_INT xc_link (ST_INT, ST_INT);
ST_INT xc_unlk (ST_INT);
ST_INT xc_put (ST_INT, ST_CHAR*, ST_INT*);
ST_INT xc_putc (ST_INT, ST_CHAR);
ST_INT xc_get (ST_INT, ST_CHAR*, ST_INT*);
ST_INT xc_getc (ST_INT);
ST_INT xc_dtr (ST_INT, ST_INT);
ST_INT xc_rts (ST_INT, ST_INT);
ST_INT xc_cts(ST_INT);
ST_INT xc_dsr(ST_INT);
ST_INT xc_dcd(ST_INT);
ST_INT  xc_test (ST_INT); 
ST_INT  xc_utest (ST_INT); 
ST_INT xc_uwait(ST_INT);
#endif /* ESSENTIAL_COM */

#ifdef __cplusplus
}
#endif

#endif	/* adlc_dsm.h already included	*/
