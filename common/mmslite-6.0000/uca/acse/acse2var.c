/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995 All Rights Reserved					*/
/*									*/
/* MODULE NAME : acse2var.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Global variables for ACSE component.  Used by both connection-	*/
/*	oriented ACSE and connectionless ACSE.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 09/13/99  MDE    08     Added SD_CONST modifiers			*/
/* 07/14/99  JRB    07     Add ACSE Password Authentication obj id.	*/
/* 07/10/98  JRB    06     Add *_obj_id so CO and CL ACSE can share.	*/
/* 11/15/96  JRB    05     Move acse_debug_sel to acse2enc.c		*/
/* 05/02/96  KCR    04     Initialized acse_debug_sel to make sure it   */
/*			     is pulled into OS/2 library		*/
/* 04/17/96  JRB    03     Del acse_sLogCtrl, use sLogCtrl (slog.c) now	*/
/* 03/26/96  MDE    02     Chg define to PLAIN_DOS.			*/
/* 10/30/95  JRB    01     Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1defs.h"

/* NOTE: we only support ISO MMS Abstract Syntax. No more MAP MMS.	*/
SD_CONST MMS_OBJ_ID mms_obj_id = {5,{1,0,9506,2,1}}; /* ISO MMS Abstract Syntax*/
SD_CONST MMS_OBJ_ID acse_obj_id = {5,{2,2,1,0,1}};   /* ACSE    Abstract Syntax*/
SD_CONST MMS_OBJ_ID asn1_obj_id = {3,{2,1,1}};	     /* ASN.1   Transfer Syntax*/

SD_CONST MMS_OBJ_ID a_auth_password_obj_id = {4,{2,2,3,1}};	/* ACSE Authent. - Password*/
