#ifndef DIBMATCH_H

#define DIBMATCH_H

/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*			2003 - 2009, All Rights Reserved		*/
/*									*/
/* MODULE NAME : dibmatch.h						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/17/09  JRB     05    Del unneeded comment in DIB_MATCH_CTRL.	*/
/* 01/04/06  EJV     04    Added DLLs incompatibility comments.		*/
/* 07/19/05  EJV     03    Added gensock2.h include.			*/
/* 07/12/05  EJV     02    DIB_MATCH_CTRL: added use_gethostbyname.	*/
/* 06/11/03  MDE     01    New						*/
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "gensock2.h"
									  
/************************************************************************/
/* General address matching defines, control, functions			*/

/* Compare Results */
#define DIB_MATCH_NOT	0
#define DIB_MATCH_CLOSE	1
#define DIB_MATCH_EXACT	2

typedef struct dib_match_ctrl_tag
  {
  ST_BOOLEAN match_allow_missing_ae_elements;
  ST_BOOLEAN match_allow_extra_ae_elements;
  
  ST_BOOLEAN match_ap_title;
  ST_BOOLEAN match_ae_qualifier;
  ST_BOOLEAN match_ap_invoke;
  ST_BOOLEAN match_ae_invoke;
  ST_BOOLEAN match_psel;
  ST_BOOLEAN match_ssel;
  ST_BOOLEAN match_tsel;
  ST_BOOLEAN match_net_addr;
  ST_BOOLEAN use_gethostbyname;	/* gethostbyname() call may take LONG	*/
  				/* time when the DNS is not available.	*/
  } DIB_MATCH_CTRL;


#ifdef __cplusplus
}
#endif
									  
#endif	/* DIBMATCH_H */
