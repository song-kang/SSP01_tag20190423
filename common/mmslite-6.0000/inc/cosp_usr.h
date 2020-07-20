/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : cosp_usr.h						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file defines the interface between the	*/
/*		Connection-oriented Session Protocol (COSP) and the	*/
/*		Connection-oriented Presentation Layer (COPP) in order	*/
/*		to exchange Session Service Data Units (SSDU).		*/
/*									*/
/*  For information see the:						*/
/*	ISO 8326 "Information processing systems - Open Systems		*/
/*	  Interconnection - Basic connection oriented session service	*/
/*	  definition.							*/
/*	ISO 8327 "Information processing systems - Open Systems		*/
/*	  Interconnection - Basic connection oriented session protocol	*/
/*	  specification.						*/
/*	ISO 8327/ADD.2 (Draft for Version2).				*/
/*									*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			cosp_bind					*/
/*		      u_cosp_bind_cnf					*/
/*			cosp_unbind					*/
/*		      u_cosp_unbind_cnf					*/
/*									*/
/*			cosp_con_req					*/
/*		      u_cosp_con_cnf_pos				*/
/*		      u_cosp_con_cnf_neg				*/
/*									*/
/*		      u_cosp_con_ind					*/
/*			cosp_con_rsp_pos				*/
/*			cosp_con_rsp_neg				*/
/*									*/
/*			cosp_rel_req					*/
/*		      u_cosp_rel_cnf_pos				*/
/*									*/
/*		      u_cosp_rel_ind					*/
/*			cosp_rel_rsp_pos				*/
/*									*/
/*			cosp_u_abort_req				*/
/*		      u_cosp_abort_ind 					*/
/*									*/
/*			cosp_data_req					*/
/*		      u_cosp_data_ind 					*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/17/09  JRB    04     Chg *_bind_id to (ST_VOID *).		*/
/*			   Chg all cosp_con_req args.			*/
/*			   Add acse2.h					*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 04/10/97  JRB    02     Chg error defs to use base from "glberror.h".*/
/* 01/10/97  EJV    01     Created					*/
/************************************************************************/
#ifndef COSP_USR_INCLUDED
#define COSP_USR_INCLUDED

#include "acse2.h"	/* Need ACSE_CONN	*/

#ifdef __cplusplus
extern "C" {
#endif


	/*======================================================*/
	/*							*/
	/*	C O S P   S P E C I F I C    D E F I N E S	*/
	/*							*/
	/*======================================================*/

/* Defines below specify limits on certain parameters for the COSP MOSI	*/
/* Version2 implementation:						*/

#define COSP_MAX_UDATA_CON	(ST_UINT) 10240
#define COSP_MAX_ENVELOPE	(ST_UINT) 56
#define COSP_MAX_UDATA	       ((ST_UINT) 65535 - COSP_MAX_ENVELOPE)

	/*======================================================*/
	/*							*/
	/*	Result and Reason Codes in S-CONNECT.cnf-	*/
	/*							*/
	/*======================================================*/

/* Result codes */
#define COSP_CON_CNF_U_REJECT	1
#define COSP_CON_CNF_P_REJECT	2

/* Reason codes in S-CONNECT.cnf- when result=COSP_CON_CNF_U_REJECT */
#define COSP_CON_CNF_U_NOT_SPECIFIED	0
#define COSP_CON_CNF_U_CONGESTION 	1
#define COSP_CON_CNF_U_IN_UDATA		2	/* udata = PPDU	*/

/* Valid reason codes in S-CONNECT.cnf- when result=COSP_CON_CNF_P_REJECT */
#define COSP_CON_CNF_P_NOT_SPECIFIED	3	/* or version not supported,		*/
						/* or implementation restrictions	*/
#define COSP_CON_CNF_P_CONGESTION 	4
#define COSP_CON_CNF_P_INV_SSEL		5	/* called Session Address unknown	*/
#define COSP_CON_CNF_P_NOT_ATTACHED	6	/* called SS-user not attached to SSAP	*/


	/*======================================================*/
	/*							*/
	/*	Reason Codes in S-CONNECT.rsp-			*/
	/*							*/
	/*======================================================*/

/* Reason codes in S-CONNECT.rsp- (if connection rejected)		*/
#define COSP_CON_RSP_U_NOT_SPECIFIED	0
#define COSP_CON_RSP_U_CONGESTION 	1
#define COSP_CON_RSP_U_IN_UDATA		2		/* udata = PPDU	*/


	/*======================================================*/
	/*							*/
	/*	Reason Codes in S-P-ABORT.ind			*/
	/*							*/
	/*======================================================*/

#define COSP_P_AB_IND_TP_DISCON		0
#define COSP_P_AB_IND_PROT_ERR		1
#define COSP_P_AB_IND_UNDEFINED		2


	/*======================================================*/
	/*							*/
	/*		C O S  P   E R R O R   C O D E S	*/
	/*							*/
	/*======================================================*/

/* NOTE: Values begin from E_COSP (found in glberror.h).		*/

/* COSP general errors */
#define	COSP_ERR_BIND_STATE	   0x3201  /* Invalid Bind state->already called*/
#define	COSP_ERR_TP4_RET	   0x3202  /* Transport layer returned error	*/
#define	COSP_ERR_INV_TP4_ADDR	   0x3203  /* Transport address invalid		*/
#define	COSP_ERR_INV_CON_STATE     0x3204  /* Invalid connect state		*/
#define	COSP_ERR_INV_SSEL	   0x3205  /* Invalid local SSEL length		*/
#define	COSP_ERR_INV_UDATA_LEN     0x3206  /* Invalid User Data length		*/
#define	COSP_ERR_INV_POINTER	   0x3207  /* Invalid pointer to encode buffer	*/

/* COSP PDU decoding errors */
#define	COSP_ERR_DEC_INV_SPDU	   0x3210  /* Invalid/not-supp. SPDU SI rcvd	*/
#define	COSP_ERR_DEC_INV_LEN	   0x3211  /* Invalid SPDU len (dec vs fun len)	*/
#define	COSP_ERR_DEC_INV_PI_CODE   0x3212  /* Invalid/out-of-place PGI/PI code	*/
#define	COSP_ERR_DEC_INV_LOC_SSEL  0x3213  /* SPDU not addressed to local SSEL	*/
#define	COSP_ERR_DEC_INV_PROT_OPT  0x3214  /* Extended SPDU concat not supp.	*/
#define	COSP_ERR_DEC_INV_SEG       0x3215  /* Invalid/not-supp. SSDU Segmenting	*/
#define	COSP_ERR_DEC_INV_PROT_VER  0x3216  /* Invalid/not-supp. prot version	*/
#define	COSP_ERR_DEC_INV_FUN_UNITS 0x3217  /* Invalid/not-supp. FU in Ses Req. 	*/
#define	COSP_ERR_DEC_INV_RF_UDATA  0x3218  /* Invalid udata len,len>0 reason!=2	*/
#define	COSP_ERR_DEC_INV_AB_RP     0x3219  /* Inv len of Reflect Par in AB SPDU	*/


	/*======================================================*/
	/*							*/
	/*		B I N D I N G   F U N C T I O N S	*/
	/*							*/
	/*======================================================*/

/************************************************************************/
/*			cosp_bind					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to bind to local Session Address.		*/
/* The SS-user should implement the u_cosp_bind_cnf function.		*/
/*									*/
/* Parameters:								*/
/*  ST_VOID    *cosp_user_bind_id	COSP User's id for this binding.*/
/*  PRES_ADDR  *loc_addr		Local Address to bind to	*/
/*  ST_INT	sharable		Ignored, (for compatibility with*/
/*  ST_INT	max_conns		Ignored, previous versions)	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if bind successful			*/
/*	error code 		otherwise				*/
/************************************************************************/
ST_RET cosp_bind (ST_VOID *cosp_user_bind_id, PRES_ADDR *loc_addr,
		   ST_INT sharable, ST_INT max_conns);


/************************************************************************/
/*			u_cosp_bind_cnf					*/
/*----------------------------------------------------------------------*/
/* This function is called by the SS-provider to indicate the result of	*/
/* the bind operation (see cosp_bind func).				*/
/* 									*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_bind_cnf (
	ST_VOID *cosp_user_bind_id,	/* COSP User's bind id		*/
	ST_VOID *cosp_bind_id,		/* COSP bind id			*/
	ST_RET result);			/* SD_SUCCESS if bind succeeded,*/
					/* else error code.		*/


/************************************************************************/
/*			cosp_unbind					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to unbind from local Session Address.	*/
/* The SS-user should implement the u_cosp_unbind_cnf function.		*/
/* 									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if unbind successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_unbind (
	ST_VOID *cosp_bind_id);		/* COSP	bind id			*/


/************************************************************************/
/*			u_cosp_unbind_cnf				*/
/*----------------------------------------------------------------------*/
/* This function is called by the SS-provider to indicate that the	*/
/* unbind operation finished (see cosp_unbind func).			*/
/* 									*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_unbind_cnf (
	ST_VOID *cosp_user_bind_id);	/* COSP User's bind id		*/


	/*======================================================*/
	/*							*/
	/*   C O N N E C T  F U N C T I O N S   (CALLED SIDE)  	*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			u_cosp_con_ind					*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* remote node wishes to establish a connection.			*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID	*cosp_user_bind_id	COSP user id bind		*/
/*  ACSE_CONN	*con			Pointer to connection info	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_con_ind (ST_VOID *cosp_user_bind_id, ACSE_CONN *con);


/************************************************************************/
/*			cosp_con_rsp_pos				*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to Accept an incomming connection.	*/
/* If SS-user data are sent make sure the reason=COSP_CON_RSP_U_IN_UDATA*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_con_rsp_pos (ACSE_CONN *con);


/************************************************************************/
/*			cosp_con_rsp_neg				*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to Reject an incomming connection.	*/
/* If SS-user data are sent make sure the reason=COSP_CON_RSP_U_IN_UDATA*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to con info struct		*/
/*  ST_INT	reason		Reason for reject COSP_CON_RSP_U_...	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_con_rsp_neg (ACSE_CONN *con, ST_INT reason);


	/*======================================================*/
	/*							*/
	/*   C O N N E C T  F U N C T I O N S   (CALLING SIDE)  */
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			cosp_con_req					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to establish a Session connection.	*/
/* SS-user should implement following functions to confirm to this	*/
/* request:  u_cosp_con_cnf_pos and u_cosp_con_cnf_neg.			*/
/* If this function returns value other then SD_SUCCESS then the function	*/
/* u_cosp_con_cnf_xxx will not be called.				*/
/* 									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_con_req (
	ACSE_CONN *con,		/* Connection info			*/
	PRES_ADDR *loc_addr,	/* Local Presentation Address		*/
	PRES_ADDR *rem_addr);	/* Remote Presentation Address		*/

/************************************************************************/
/*			u_cosp_con_cnf_pos				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that	*/
/* the connection has been established.					*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_con_cnf_pos (ACSE_CONN *con);


/************************************************************************/
/*			u_cosp_con_cnf_neg				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate the	*/
/* connection has been rejected by remote.				*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*  ST_INT	result		see def results COSP_CON_CNF_... above	*/
/*  ST_INT	reason		see def reasons COSP_CON_CNF_... above	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_con_cnf_neg (ACSE_CONN *con, ST_INT result, ST_INT reason);


	/*======================================================*/
	/*							*/
	/*  C O N N E C T I O N   R E L E A S E   (CALLED SIDE)	*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			u_cosp_rel_ind					*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* remote node wishes to release a connection. The SS-user should call	*/
/* cosp_rel_rsp_pos to release the connection. Negative cnf not allowed.*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con			Pointer to connection info	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_rel_ind (ACSE_CONN *con);


/************************************************************************/
/*			cosp_rel_rsp_pos				*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to respond positively to release of	*/
/* connection.								*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_rel_rsp_pos (ACSE_CONN *con);


	/*======================================================*/
	/*							*/
	/*  C O N N E C T I O N   R E L E A S E   (CALLING SIDE)*/
	/*							*/
	/*======================================================*/

/************************************************************************/
/*			cosp_rel_req					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to release a Session connection.		*/
/* SS-user should implement the u_cosp_rel_cnf_pos function to receive	*/
/* confirm for this request.  Negative confirm is not implemented.	*/
/* If this function returns value other then SD_SUCCESS then the function	*/
/* u_cosp_rel_cnf_pos will not be called.				*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_rel_req (ACSE_CONN *con);


/************************************************************************/
/*			u_cosp_rel_cnf_pos				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate positive	*/
/* result of the release operation.					*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_rel_cnf_pos (ACSE_CONN *con);


	/*======================================================*/
	/*							*/
	/*	A B O R T    F U N C T I O N S			*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			cosp_u_abort_req				*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to Abort a connection (S-U-ABORT)		*/
/*									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_u_abort_req (ACSE_CONN *con);


/************************************************************************/
/*			u_cosp_p_abort_ind 				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* connection has been aborted by SS-provider (local or remote).	*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*  ST_INT	reason		see def reasons COSP_P_AB_IND_... above	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_p_abort_ind (ACSE_CONN *con, ST_INT reason);


/************************************************************************/
/*			u_cosp_u_abort_ind 				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* connection has been aborted by remote SS-user.			*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_u_abort_ind (ACSE_CONN *con);


	/*======================================================*/
	/*							*/
	/*	D A T A   T R A N S F E R   F U N C T I O N S	*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			cosp_data_req					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to transfer normal data on a previously	*/
/* established connection.						*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_data_req (ACSE_CONN *con);


/************************************************************************/
/*			u_cosp_data_ind 				*/
/*----------------------------------------------------------------------*/
/* This USER function is called by the SS-Provider to indicate that a	*/
/* data have been received.						*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID u_cosp_data_ind (ACSE_CONN *con);

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/


