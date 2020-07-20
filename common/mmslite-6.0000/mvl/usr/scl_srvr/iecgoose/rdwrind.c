/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 1997, All Rights Reserved		        */
/*									*/
/* MODULE NAME : rdwrind.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/26/07  JRB     05    Simplify include files.			*/
/* 08/09/06  JRB     04    Del several unused leaf functions.		*/
/* 08/13/04  JRB     03    Del unneeded includes.			*/
/* 01/06/04  JRB     02    Add mvlu_null_rd(wr)_ind (foundry no longer	*/
/*			   creates them). mvlu_null_rd_ind return	*/
/*			   non-zero dummy data for some data types.	*/
/* 04/28/03  JRB     01    Created based on file from uca_srvr sample.	*/
/*			   Deleted unused u_arr_type_*, u_srcconfig_*.	*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"

#include "mvl_acse.h"
#include "mvl_log.h"
#include "mvl_uca.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/************************************************************************/
/* SBO HANDLING								*/
/************************************************************************/
/*			u_mvl_sbo_operate				*/
/************************************************************************/

ST_VOID  u_mvl_sbo_operate (MVL_SBO_CTRL *sboSelect, 
			    MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
MVL_VAR_ASSOC *va;
ST_CHAR *src;

  /* The select is in place, go ahead and operate ... 			*/
  va = mvluWrVaCtrl->wrVaCtrl->va;
  src = mvluWrVaCtrl->primData;

  mvlu_wr_prim_done (mvluWrVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			mvlu_null_rd_ind				*/
/* Default read indication function. Just return some kind of dummy data*/
/* that is appropriate for the element type (mvluRdVaCtrl->rt->el_tag).	*/ 
/************************************************************************/

ST_VOID mvlu_null_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
  switch (mvluRdVaCtrl->rt->el_tag)
    {
    case RT_BOOL :
      *(ST_BOOLEAN *) mvluRdVaCtrl->primData = 0;
    break;

    case RT_BIT_STRING :
      /* Set all bits to 0.					*/
      /* For variable-length bitstring, this also sets len = 0	*/
      memset (mvluRdVaCtrl->primData, 0, mvluRdVaCtrl->rt->el_size);
    break;

    case RT_INTEGER :
      switch (mvluRdVaCtrl->rt->u.p.el_len)	/* determine length		*/
        {
        case 1 :
          *(ST_INT8 *) mvluRdVaCtrl->primData = -1;
        break;

        case 2 :
          *(ST_INT16 *) mvluRdVaCtrl->primData = -2;
        break;

        case 4 :
          *(ST_INT32 *) mvluRdVaCtrl->primData = -4;
        break;

#ifdef INT64_SUPPORT
        case 8 :
          *(ST_INT64 *) mvluRdVaCtrl->primData = -8;
        break;
#endif
        }
    break;

    case RT_UNSIGNED :
      switch (mvluRdVaCtrl->rt->u.p.el_len)	/* determine length		*/
        {
        case 1 :
          *(ST_UINT8 *) mvluRdVaCtrl->primData = 1;
        break;

        case 2 :
          *(ST_UINT16 *) mvluRdVaCtrl->primData = 2;
        break;

        case 4 :
          *(ST_UINT32 *) mvluRdVaCtrl->primData = 4;
        break;

#ifdef INT64_SUPPORT
        case 8 :
          *(ST_UINT64 *) mvluRdVaCtrl->primData = 8;
        break;
#endif
        }
    break;

    case RT_FLOATING_POINT :
      if (mvluRdVaCtrl->rt->u.p.el_len != sizeof (ST_FLOAT))
        {
        *(ST_DOUBLE *) mvluRdVaCtrl->primData = 2.2;
        }
      else
        {
        *(ST_FLOAT *) mvluRdVaCtrl->primData = (ST_FLOAT) 1.1;
        }
    break;

    case RT_OCTET_STRING :
      memset (mvluRdVaCtrl->primData, 0, mvluRdVaCtrl->rt->el_size);
    break;

    case RT_VISIBLE_STRING :
      /* copy as much of dummy string as possible	*/
      strncpy (mvluRdVaCtrl->primData, "dummy_vstring_33bytes_34567890123", mvluRdVaCtrl->rt->el_size);
      mvluRdVaCtrl->primData[mvluRdVaCtrl->rt->el_size-1] = '\0';	/* make sure NULL-terminated*/
    break;

    case RT_GENERAL_TIME :
    case RT_BINARY_TIME :
    case RT_BCD :
    case RT_UTC_TIME :
      memset (mvluRdVaCtrl->primData, 0, mvluRdVaCtrl->rt->el_size);
    break;

    case RT_UTF8_STRING :
      /* Just return empty (0 length) string.	*/
      memset (mvluRdVaCtrl->primData, 0, mvluRdVaCtrl->rt->el_size);
    break;

    default :			/* should not be any other tag	*/
      MVL_LOG_ERR1 ("Invalid tag: %d", (int) mvluRdVaCtrl->rt->el_tag);
    break;
    }

  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			mvlu_null_wr_ind				*/
/************************************************************************/

ST_VOID mvlu_null_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
  mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);
  }
