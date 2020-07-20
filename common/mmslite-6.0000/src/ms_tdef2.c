/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_tdef2.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*   Functions to assist in using NESTED RUNTIME TYPES			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/01/05  JRB     13    Avoid risky cast to remove const.		*/
/* 04/02/03  JRB     12    Add UTF8string support (see RT_UTF8_STRING).	*/
/* 12/20/01  JRB     11    Converted to use ASN1R (re-entrant ASN1)	*/
/* 11/14/01  EJV     10    Added support for new MMS type UtcTime:	*/
/*			   ms_runtime_to_asn1: add case for RT_UTC_TIME	*/
/* 10/18/01  JRB     09    Eliminate warning.				*/
/* 07/28/00  JRB     08    Use ms_comp_name_find for everyone.		*/
/* 07/13/00  JRB     07    Use new ms_comp_name_find to get comp names.	*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 04/14/99  MDE     05    Removed unnecessary include files		*/
/* 09/10/98  MDE     04    Removed 'mvlRtNames' declaration		*/
/* 08/13/98  MDE     03    Added asn1 encode overrun test to loop	*/
/* 03/11/98  MDE     02    Removed NEST_RT_TYPES			*/
/* 04/08/97  MDE     01    Fixed 'RT Table too small prob w/named types	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/*                           ms_runtime_to_tdl                          */
/************************************************************************/
/* Converts RUNTIME TYPE to ASN.1. ASN.1 build must be started		*/

ST_RET ms_runtime_to_asn1 (ASN1_ENC_CTXT *aCtx, SD_CONST RUNTIME_TYPE *rt, ST_INT rt_num)
  {
ST_INT i;
ST_INT level;
SD_CONST RUNTIME_TYPE *rt_ptr;
ST_BOOLEAN structComp[ASN1_MAX_LEVEL];
ST_UCHAR v1;
ST_UCHAR v2;
const ST_CHAR *comp_name;

  S_LOCK_COMMON_RESOURCES ();

  level = 0;
  rt_ptr = rt + (rt_num - 1);
  for (i = 0; i < rt_num && aCtx->asn1r_encode_overrun == SD_FALSE; ++i, --rt_ptr)
    {
    switch (rt_ptr->el_tag)
      {
      case RT_ARR_START :
        --level;
      /* Finish typeSpecification */
        asn1r_fin_constr (aCtx, 2, CTX, SD_FALSE);   

      /* Write numberOfElements */
        asn1r_wr_i32 (aCtx, (ST_INT32) rt_ptr->u.arr.num_elmnts);
        asn1r_fin_prim (aCtx, 1,CTX);

      /* Finish array */
        asn1r_fin_constr (aCtx, 1, CTX, SD_FALSE);  
      break;

      case RT_ARR_END :
        ++level;
        structComp[level] = SD_FALSE;
        asn1r_strt_constr (aCtx);                 /* Start array                  */
        asn1r_strt_constr (aCtx);                 /* Start typeSpecification      */
      break;

      case RT_STR_START :
        --level;
        asn1r_fin_constr (aCtx, 1, CTX, SD_FALSE);  /* Finish components */
        asn1r_fin_constr (aCtx, 2, CTX, SD_FALSE);  /* Finish structure  */
      break;

      case RT_STR_END :
        ++level;
        structComp[level] = SD_TRUE;

        asn1r_strt_constr (aCtx);               /* Start structure                */
        asn1r_strt_constr (aCtx);               /* Start components               */
       /* prepare for first component */
        asn1r_strt_constr (aCtx);               /* Start component seq of seq     */
        asn1r_strt_constr (aCtx);               /* Start component type           */
      break;

      case RT_BOOL : 
        asn1r_fin_prim (aCtx, RT_BOOL, CTX);
      break;

      case RT_INTEGER :
        asn1r_wr_u8 (aCtx, (ST_UCHAR) (rt_ptr->u.p.el_len * 8));
        asn1r_fin_prim (aCtx, RT_INTEGER, CTX);
      break;

      case RT_UNSIGNED :
        asn1r_wr_u8 (aCtx, (ST_UCHAR) (rt_ptr->u.p.el_len * 8));
        asn1r_fin_prim (aCtx, RT_UNSIGNED, CTX);
      break;

      case RT_FLOATING_POINT :
        asn1r_strt_constr (aCtx);
        if (rt_ptr->u.p.el_len == 4)	/* single precision		*/
          {
          v1 = SNGL_EXP;
          if (!m_use_dis_float)
            v2 = SNGL_WHOLE;
          else
            v2 = SNGL_FRACT;
	  }
        else
          {
          v1 = DOUBL_EXP;
          if (!m_use_dis_float)
            v2 = DOUBL_WHOLE;
          else
            v2 = DOUBL_FRACT;
	  }
        asn1r_wr_u8 (aCtx, v1);    /* write exp  */
        asn1r_fin_prim (aCtx, INT_CODE,UNI);
        asn1r_wr_u8 (aCtx, v2);
        asn1r_fin_prim (aCtx, INT_CODE,UNI);
        asn1r_fin_constr (aCtx, RT_FLOATING_POINT, CTX, SD_FALSE);
      break;

      case RT_BIT_STRING : 
      case RT_OCTET_STRING : 
      case RT_VISIBLE_STRING :
      case RT_UTF8_STRING :	/* all same encoding w/ different tags*/
        asn1r_wr_i32 (aCtx, (ST_INT32) rt_ptr->u.p.el_len);
        asn1r_fin_prim (aCtx, rt_ptr->el_tag, CTX);
      break;

      case RT_GENERAL_TIME : 
        asn1r_fin_prim (aCtx, RT_GENERAL_TIME, CTX);
      break;

      case RT_BINARY_TIME : 
        if (rt_ptr->u.p.el_len == 4)
          asn1r_wr_u8 (aCtx, 0);
        else
          asn1r_wr_u8 (aCtx, 1);
        asn1r_fin_prim (aCtx, RT_BINARY_TIME, CTX);
      break;

      case RT_UTC_TIME : 
        asn1r_fin_prim (aCtx, RT_UTC_TIME, CTX);
      break;

      case RT_BCD : 
        asn1r_wr_u8 (aCtx, (ST_UCHAR) rt_ptr->u.p.el_len);
        asn1r_fin_prim (aCtx, RT_BCD, CTX);
      break;

      case RT_BOOLEANARRAY : 
      break;

      default :
        MLOG_ERR1 ("MS_RUNTIME_TO_ASN1: Invalid element tag (%d)",
    					rt_ptr->el_tag);
        S_UNLOCK_COMMON_RESOURCES ();
        return (SD_FAILURE);
      break;
      }

    if (level > 0 && structComp[level] == SD_TRUE && 
	rt_ptr->el_tag != RT_STR_END)
      {
      asn1r_fin_constr (aCtx, 1, CTX, SD_FALSE);        /* Finish typeSpec     */

      comp_name = ms_comp_name_find (rt_ptr);
      if (strlen (comp_name))
        {
	asn1r_wr_vstr (aCtx, comp_name);
        asn1r_fin_prim (aCtx, 0, CTX);
        }
      asn1r_fin_constr (aCtx, SEQ_CODE, UNI, SD_FALSE);

      if ((rt_ptr-1)->el_tag != RT_STR_START)
        {
      /* prepare for the next component */
        asn1r_strt_constr (aCtx);               /* Start component seq of seq     */
        asn1r_strt_constr (aCtx);               /* Start component type           */
        }
      }
    }

  S_UNLOCK_COMMON_RESOURCES ();

  if (aCtx->asn1r_encode_overrun)
    {
    MLOG_NERR0 ("ms_runtime_to_asn1 : ASN.1 encode overrun");
    return (SD_FAILURE);
    }
  return (SD_SUCCESS);
  }


