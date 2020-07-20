#ifndef GLBOPT_INCLUDED
#define GLBOPT_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2002-2002 All Rights Reserved					*/
/*									*/
/* MODULE NAME : glbopt.h						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Global compiler options to be used by all SISCO source modules.	*/
/*	This file may be used to enable optional features in		*/
/*	MMSEASE-LITE. Usually this is easier than adding an		*/
/*	option to several makefiles. If this file is modified, all	*/
/*	source code should be recompiled.				*/
/*									*/
/*	This file must be included by "glbtypes.h" so that all SISCO	*/
/*	source modules will indirectly include it.			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 05/09/05  JRB     09    Delete all defines. This file should now be	*/
/*			   used only for "user" customization.		*/
/* 09/22/03  EJV     08    Deleted ACSE_AUTH_ENABLED, S_SEC_ENABLED	*/
/* 07/16/03  MDE     07    Removed ACSE_AUTH_ENABLED			*/
/* 07/15/03  EJV     06    ACSE_AUTH_ENABLED enabled (ICCP Gateway)	*/
/* 04/17/03  JRB     05    Chg MVLU_LEAF_FUN_LOOKUP_ENABLE (defined).	*/
/* 12/20/02  JRB     04    Add MVLU_LEAF_FUN_LOOKUP_ENABLE (!defined).	*/
/* 12/18/02  JRB     03    Add USE_RT_TYPE_2 (defined by default).	*/
/* 10/17/02  ASK     02    Added ACSE_AUTH_ENABLED 			*/
/* 02/27/02  JRB     01    Created					*/
/************************************************************************/

/*
 * NOTE: Insert "global" defines here. These will be used when compiling
 *       all SISCO code.
 */

#endif	/* !GLBOPT_INCLUDED	*/

