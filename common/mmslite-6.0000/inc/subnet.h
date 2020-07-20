/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2010-2010 All Rights Reserved					*/
/*									*/
/* MODULE NAME : subnet.h						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Subnetwork service functions for receiving and processing	*/
/*	subnetwork packets (basically IEEE 802.3 MAC frames).		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 06/22/10  JRB	   Created.					*/
/************************************************************************/
#include "clnp_sne.h"
#include "clnp_usr.h"

/* Functions to process a packet.	*/
ST_VOID goose_ind_process (SN_UNITDATA *sn_udt);
ST_VOID gse_mgmt_ind_process (SN_UNITDATA *sn_udt);
ST_VOID smpval_ind_process (SN_UNITDATA *sn_udt);
ST_VOID cltp_ind_process (N_UNITDATA *n_udt);

/* Functions to get packets from the queue (linked list).	*/
/* NOTE: SUBNET_THREADS must be defined.			*/
SN_UNITDATA *goose_ind_get ();
SN_UNITDATA *gse_mgmt_ind_get ();
SN_UNITDATA *smpval_ind_get ();
N_UNITDATA *cltp_ind_get ();

/* Function to service the Subnetwork interface.	*/
ST_BOOLEAN subnet_serve (ST_VOID);

