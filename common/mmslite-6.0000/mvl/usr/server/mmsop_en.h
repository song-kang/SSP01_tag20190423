/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mmsop_en.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the operation enable switches used to	*/
/*	configure the decode system, the user service system, & the	*/
/*	supported services bit strings.					*/
/*									*/
/*	Note that by enabling (or disabling) only the desired MMS	*/
/*	operations, a limited subset may be created (the subset 	*/
/*	creation module is mmsop_en.c), thus allowing the creation of 	*/
/*	a smaller executable because:					*/
/*	  1) references to the various MMS-EASE functions are removed	*/
/*		and therefore are not included from the library, and	*/
/*	  2) operation specific user application code is eliminated.	*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/16/00  JRB     04    Enabled response for GetCapabiliyList	*/
/* 05/12/00  RKR     03    Enabled reception of InfoReports		*/
/* 10/06/99  GLB     02    Added file Rename and Obtain File            */
/* 08/18/97  MDE     01    Added parameter support defines		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMSOP_EN_INCLUDED
#define MMSOP_EN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_def2.h"		/* for REQ_RESP_EN, etc.		*/

/************************************************************************/
/* define the opcode enable switches					*/
/************************************************************************/

#define MMS_INIT_EN		RESP_EN
#define MMS_CONCLUDE_EN 	RESP_EN
#define MMS_CANCEL_EN		RESP_EN

#define MMS_STATUS_EN		RESP_EN
#define MMS_USTATUS_EN		REQ_RESP_DIS
#define MMS_GETNAMES_EN 	RESP_EN
#define MMS_IDENT_EN		RESP_EN
#define MMS_RENAME_EN 		REQ_RESP_DIS
#define MMS_GETCL_EN 		RESP_EN
#define MMS_VMD_EN		0
				 	   
#define MMS_NAMED_DOM_EN	0
#define MMS_MV_DWN_EN		REQ_RESP_DIS	/* VM Upload		*/
#define MMS_INIT_DWN_EN 	REQ_RESP_DIS
#define MMS_DWN_LOAD_EN 	REQ_RESP_DIS
#define MMS_TERM_DWN_EN 	REQ_RESP_DIS
#define MMS_MV_UPL_EN		REQ_RESP_DIS	/* VM Download		*/
#define MMS_INIT_UPL_EN 	REQ_RESP_DIS
#define MMS_UP_LOAD_EN		REQ_RESP_DIS
#define MMS_TERM_UPL_EN 	REQ_RESP_DIS
#define MMS_RDDWN_EN 		REQ_RESP_DIS
#define MMS_RDUPL_EN 		REQ_RESP_DIS
#define MMS_LOAD_DOM_EN 	REQ_RESP_DIS
#define MMS_STR_DOM_EN		REQ_RESP_DIS
#define MMS_DEL_DOM_EN		REQ_RESP_DIS
#define MMS_GET_DOM_EN		RESP_EN

#define MMS_CRE_PI_EN		REQ_RESP_DIS
#define MMS_DEL_PI_EN		REQ_RESP_DIS
#define MMS_START_EN		REQ_RESP_DIS
#define MMS_STOP_EN		REQ_RESP_DIS
#define MMS_RESUME_EN		REQ_RESP_DIS
#define MMS_RESET_EN		REQ_RESP_DIS
#define MMS_KILL_EN		REQ_RESP_DIS
#define MMS_GET_PI_EN		REQ_RESP_DIS

#define MMS_VA_EN		0
#define MMS_MV_READ_EN		REQ_RESP_DIS	/* VM Read variable(s)	*/
#define MMS_MV_RDVARS_EN 	REQ_RESP_DIS	/* General VM Rd var's	*/
#define MMS_READ_EN		RESP_EN
#define MMS_MV_WRITE_EN		REQ_RESP_DIS	/* VM Write variable(s)	*/
#define MMS_MV_WRVARS_EN 	REQ_RESP_DIS	/* General VM WR var's	*/
#define MMS_WRITE_EN		RESP_EN
#define MMS_INFO_EN		RESP_EN
#define MMS_GETVAR_EN		RESP_EN
#define MMS_DEFVAR_EN		REQ_RESP_DIS
#define MMS_DEFSCAT_EN		REQ_RESP_DIS
#define MMS_GETSCAT_EN		REQ_RESP_DIS
#define MMS_DELVAR_EN		REQ_RESP_DIS
#define MMS_DEFVLIST_EN		RESP_EN
#define MMS_GETVLIST_EN		RESP_EN
#define MMS_DELVLIST_EN		RESP_EN
#define MMS_MV_DEFTYPE_EN	REQ_RESP_DIS	/* VM DefineType	*/
#define MMS_DEFTYPE_EN		REQ_RESP_DIS
#define MMS_GETTYPE_EN		REQ_RESP_DIS
#define MMS_DELTYPE_EN		REQ_RESP_DIS

#define MMS_TAKECTRL_EN 	REQ_RESP_DIS
#define MMS_RELCTRL_EN		REQ_RESP_DIS
#define MMS_DEFINE_SEM_EN	REQ_RESP_DIS
#define MMS_DELETE_SEM_EN	REQ_RESP_DIS
#define MMS_REP_SEMSTAT_EN	REQ_RESP_DIS
#define MMS_REP_SEMPOOL_EN	REQ_RESP_DIS
#define MMS_REP_SEMENTRY_EN	REQ_RESP_DIS

#define MMS_JREAD_EN		RESP_EN
#define MMS_JWRITE_EN		REQ_RESP_DIS
#define MMS_JINIT_EN		RESP_EN
#define MMS_JSTAT_EN		RESP_EN
#define MMS_JCREATE_EN		REQ_RESP_DIS
#define MMS_JDELETE_EN		REQ_RESP_DIS
	
#define MMS_DEFEC_EN		REQ_RESP_DIS
#define MMS_DELEC_EN		REQ_RESP_DIS	
#define MMS_GETECA_EN		REQ_RESP_DIS
#define MMS_REPECS_EN		REQ_RESP_DIS
#define MMS_ALTECM_EN		REQ_RESP_DIS	
#define MMS_TRIGE_EN		REQ_RESP_DIS	
#define MMS_DEFEA_EN		REQ_RESP_DIS	
#define MMS_DELEA_EN		REQ_RESP_DIS	
#define MMS_GETEAA_EN		REQ_RESP_DIS	
#define MMS_REPEAS_EN		REQ_RESP_DIS	
#define MMS_DEFEE_EN		REQ_RESP_DIS	
#define MMS_DELEE_EN		REQ_RESP_DIS	
#define MMS_GETEEA_EN		REQ_RESP_DIS	
#define MMS_REPEES_EN		REQ_RESP_DIS	
#define MMS_ALTEE_EN		REQ_RESP_DIS	
#define MMS_EVNOT_EN		REQ_RESP_DIS
#define MMS_ACKEVNOT_EN 	REQ_RESP_DIS
#define MMS_GETAS_EN		REQ_RESP_DIS
#define MMS_GETAES_EN		REQ_RESP_DIS

#define MMS_INPUT_EN		REQ_RESP_DIS
#define MMS_OUTPUT_EN		REQ_RESP_DIS
				     
#define MMS_MV_FOPEN_EN		REQ_RESP_DIS	/* VM FileOpen		*/
#define	MMS_MV_FREAD_EN		REQ_RESP_DIS	/* VM FileRead		*/
#define MMS_MV_FCLOSE_EN	REQ_RESP_DIS	/* VM FileClose		*/
#define	MMS_MV_FCOPY_EN		REQ_RESP_DIS	/* VM FileCopy		*/
#define MMS_OBTAINFILE_EN	RESP_EN
#define MMS_FOPEN_EN		REQ_RESP_EN
#define MMS_FREAD_EN		REQ_RESP_EN
#define MMS_FCLOSE_EN		REQ_RESP_EN
#define MMS_FRENAME_EN		RESP_EN
#define MMS_FDELETE_EN		RESP_EN
#define MMS_FDIR_EN		RESP_EN

/************************************************************************/
/************************************************************************/
/* PARAMETER SUPPORTED BITSTRING VALUES					*/
/************************************************************************/

#define MPARAM_STR1  0x80    /* 0x80 structures 			*/
#define MPARAM_STR2  0x40    /* 0x40 arrays 				*/
#define MPARAM_VNAM  0x20    /* 0x20 named variables 			*/
#define MPARAM_VALT  0x10    /* 0x10 alternate access 			*/
#define MPARAM_VADR  0x00    /* 0x08 address formed variables 		*/
#define MPARAM_VSCA  0x00    /* 0x04 scattered access 			*/
#define MPARAM_TPY   0x00    /* 0x02 third party 			*/
#define MPARAM_VLIS  0x01    /* 0x01 variable list         		*/
#define MPARAM_REAL  0x00    /* 0x80 real data type        		*/
#define MPARAM_AKEC  0x00    /* 0x40 ack event cond.       		*/
#define MPARAM_CEI   0x00    /* 0x20 cond. eval. interval  		*/

#define MPARAM0	 MPARAM_STR1 | MPARAM_STR2 | MPARAM_VNAM | MPARAM_VALT |\
                 MPARAM_VADR | MPARAM_VSCA | MPARAM_TPY  | MPARAM_VLIS
#define MPARAM1  MPARAM_REAL | MPARAM_AKEC | MPARAM_CEI

#ifdef __cplusplus
}
#endif

#endif  /* MMSOP_EN_INCLUDED */

