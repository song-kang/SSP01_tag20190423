/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2001-2001, All Rights Reserved					*/
/*									*/
/* MODULE NAME : c_ustat.c   						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Receive "Unsolicited Status" request.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_ustatus_ind						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/01/01  JRB     01    Created.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/*			_mvl_process_ustatus_ind			*/
/* Process Unsolicited Status indication.				*/
/************************************************************************/

ST_VOID _mvl_process_ustatus_ind (MVL_COMM_EVENT *event)
  {
  /* IMPORTANT: When user funct u_mvl_ustatus_ind returns, event is freed.*/
  /*            User funct must save event data if it is needed later.	*/
  u_mvl_ustatus_ind (event);		/* pass up to user.	*/
  _mvl_free_comm_event (event);		/* free resources.	*/
  }
