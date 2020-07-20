/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc., 	*/
/*      	  1986 - 2001, All Rights Reserved		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_arb.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	This module contains an MMS-EASE support function which		*/
/*	processes arbitrary data based on a derived MMS type		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/02/03  JRB     12    Add UTF8string support (see RT_UTF8_STRING)	*/
/*			   - use new "utf8" added to M_ARB_DATA_CTRL.	*/
/* 03/04/02  JRB     10    Use SD_CONST on ALL "RUNTIME_TYPE *" vars.	*/
/* 11/15/01  EJV     09    Added support for new MMS type UtcTime:	*/
/*			   ms_process_arb_data: add case for RT_UTC_TIME*/
/* 01/21/01  EJV     08    M_ARB_DATA_CTRL: changed bool to booln,	*/
/*			   to avoid conflict w/ bool type in C++ files.	*/
/* 07/13/00  JRB     07    Added #ifdef around INT64 code.		*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 08/21/98  MDE     05    Reversed 1-st arg casting for gt and bt4 fun.*/
/* 12/10/97  MDE     04    Fixed call to int8 fun to take ST_INT8	*/
/* 09/11/97  MDE     03    MMS ARB now returns ST_RET, as do user funs	*/
/* 09/02/97  NAV     02    Support Btime6 and Btime4			*/
/* 07/16/97  RKR     01    added 64 bit support				*/
/* 06/23/97  RKR   7.00    created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"

/************************************************************************/
/* Use a static pointer to avoid duplication of __FILE__ strings.	*/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;

/************************************************************************/
/*			  ms_process_arb_data							*/
/************************************************************************/

ST_RET ms_process_arb_data (ST_CHAR *dataBase, 
			    SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
 			    ST_BOOLEAN altAccPres, ST_BOOLEAN altAccPacked, 
                            ALT_ACCESS *altAcc,
		            M_ARB_DATA_CTRL *ac)
  {
RT_AA_CTRL *runtime_aa;
RT_AA_CTRL *rtaa;
ST_INT num_rt_aa;
SD_CONST RUNTIME_TYPE *rt;
ST_RET ret;
ST_INT i;
ST_CHAR *dataPtr;
ST_INT lastElSize;
ST_RET uDataRet;

/* Create an RTAA that selects all elements				*/
  if (!altAccPres)
    ret = _ms_mk_rt_aa_all (rt_head, rt_num,  &runtime_aa, &num_rt_aa);
  else	
    ret = _ms_mk_rt_aa (rt_head, rt_num, altAcc, &runtime_aa, &num_rt_aa);

  if (ret)
    {
    MLOG_ERR0 ("Error creating RTAA");
    return (ret);
    }

/* OK, we now have a nice linear representation of the data type and	*/
/* offsets in memory. 							*/

  rtaa = runtime_aa;
  dataPtr = dataBase;
  lastElSize = 0;
  uDataRet = SD_SUCCESS;
  for (i = 0; uDataRet == SD_SUCCESS && (i < num_rt_aa); ++i, ++rtaa)
    {
    rt = rtaa->rt;
    if (!altAccPres || !altAccPacked)	 
      dataPtr = dataBase + rtaa->offset_to_data;
    else	/* local data is 'packed' in memory */
      {
      dataPtr += lastElSize;
      lastElSize = rtaa->el_size;
      }

    switch (rt->el_tag)
      {
      case RT_BOOL : 			/* boolean			*/
        if (ac->booln)
          uDataRet = (*ac->booln) ((ST_BOOLEAN *) dataPtr, rtaa);
        else
          {
          MLOG_ERR0 ("No ST_BOOLEAN data handling function provided");
          }
      break;
     
      case RT_BIT_STRING : 
        if (ac->bs)
          uDataRet = (*ac->bs) ((ST_UCHAR *) dataPtr, rtaa);
        else
          {
          MLOG_ERR0 ("No BIT STRING data handling function provided");
          }
      break;
     
      case RT_INTEGER :
        switch (rt -> u.p.el_len)	/* determine length		*/
       	  {
          case 1 :			/* one byte int 		*/
            if (ac->int8)
              uDataRet = (*ac->int8) ((ST_INT8 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No INTEGER 8 data handling function provided");
              }
     	  break;
     
     	  case 2 :			/* two byte int 		*/
            if (ac->int16)
              uDataRet = (*ac->int16) ((ST_INT16 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No INTEGER 16 data handling function provided");
              }
     	  break;
     	  
     	  case 4 :			/* four byte integer		*/
            if (ac->int32)
              uDataRet = (*ac->int32) ((ST_INT32 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No INTEGER 32 data handling function provided");
              }
     	  break;
     	  
#ifdef INT64_SUPPORT
     	  case 8 :			/* eight byte integer		*/
            if (ac->int64)
              uDataRet = (*ac->int64) ((ST_INT64 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No INTEGER 64 data handling function provided");
              }
     	  break;
#endif 	/* INT64_SUPPORT	*/
     	  
     	  default :
	    MLOG_ERR0 ("Invalid integer length");
     	  }
      break;

      case RT_UNSIGNED :
        switch (rt -> u.p.el_len)	/* determine length		*/
       	  {
          case 1 :			/* one byte int 		*/
            if (ac->uint8)
              uDataRet = (*ac->uint8) ((ST_UCHAR *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No UNSIGNED 8 data handling function provided");
              }
     	  break;
     
     	  case 2 :			/* two byte int 		*/
            if (ac->uint16)
              uDataRet = (*ac->uint16) ((ST_UINT16 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No UNSIGNED 16 data handling function provided");
              }
     	  break;
     	  
     	  case 4 :			/* four byte integer		*/
            if (ac->uint32)
              uDataRet = (*ac->uint32) ((ST_UINT32 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No UNSIGNED 32 data handling function provided");
              }
     	  break;
     	  
#ifdef INT64_SUPPORT
     	  case 8 :			/* eight byte integer		*/
            if (ac->uint64)
              uDataRet = (*ac->uint64) ((ST_UINT64 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No UNSIGNED 64 data handling function provided");
              }
     	  break;
#endif 	/* INT64_SUPPORT	*/

     	  default :
	    MLOG_ERR0 ("Invalid integer length");
     	  }
      break;

      case RT_BCD:
        if (rt->u.p.el_len <= 2)
          {
          if (ac->bcd1)
            uDataRet = (*ac->bcd1) ((ST_INT8 *) dataPtr, rtaa);
          else
            {
            MLOG_ERR0 ("No BCD1 data handling function provided");
            }
          }
        else if (rt->u.p.el_len <= 4)
          {
          if (ac->bcd2)
            uDataRet = (*ac->bcd2) ((ST_INT16 *) dataPtr, rtaa);
          else
            {
            MLOG_ERR0 ("No BCD2 data handling function provided");
            }
          }
        else if (rt->u.p.el_len <= 8)
          {
          if (ac->bcd4)
            uDataRet = (*ac->bcd4) ((ST_INT32 *) dataPtr, rtaa);
          else
            {
            MLOG_ERR0 ("No BCD4 data handling function provided");
            }
          }
      break;
     
      case RT_FLOATING_POINT :
        if (rt -> u.p.el_len != sizeof (ST_FLOAT))
          {
          if (ac->dbl)
            uDataRet = (*ac->dbl) ((ST_DOUBLE *) dataPtr, rtaa);
          else
            {
            MLOG_ERR0 ("No ST_DOUBLE data handling function provided");
            }
          }
        else
          {
          if (ac->flt)
            uDataRet = (*ac->flt) ((ST_FLOAT *) dataPtr, rtaa);
          else
            {
            MLOG_ERR0 ("No ST_FLOAT data handling function provided");
            }
          }
      break;

      case RT_OCTET_STRING :	
        if (ac->oct)
          uDataRet = (*ac->oct) ((ST_UCHAR *) dataPtr, rtaa);
        else
          {
          MLOG_ERR0 ("No OCTET STRING data handling function provided");
          }
      break;
     
      case RT_VISIBLE_STRING :
        if (ac->vis)
          uDataRet = (*ac->vis) ((ST_CHAR *) dataPtr, rtaa);
        else
          {
          MLOG_ERR0 ("No VISIBLE STRING data handling function provided");
          }
      break;
     
      case RT_GENERAL_TIME :
        if (ac->gt)
          uDataRet = (*ac->gt) ((time_t *) dataPtr, rtaa);
        else
          {
          MLOG_ERR0 ("No GEN TIME data handling function provided");
          }
      break;

      case RT_BINARY_TIME :
        switch (rt -> u.p.el_len)	/* determine length		*/
	  {
	  case 4:
	    if (ac->bt4)
              uDataRet = (*ac->bt4) ((ST_INT32 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No BIN TIME 4 data handling function provided");
              }
	    break;
	  case 6:
	    if (ac->bt6)
              uDataRet = (*ac->bt6) ((ST_INT32 *) dataPtr, rtaa);
            else
              {
              MLOG_ERR0 ("No BIN TIME 6 data handling function provided");
              }
	    break;
	  }
      break;

      case RT_UTC_TIME :
        if (ac->utc)
          uDataRet = (*ac->utc) ((MMS_UTC_TIME *) dataPtr, rtaa);
        else
          {
          MLOG_ERR0 ("No UTC TIME data handling function provided");
          }
      break;

      case RT_UTF8_STRING :
        if (ac->utf8)
          uDataRet = (*ac->utf8) ((ST_UCHAR *) dataPtr, rtaa);
        else
          {
          MLOG_ERR0 ("No UTF8string data handling function provided");
          }
      break;

      case RT_ARR_START     :
        if (ac->arrStart)
          uDataRet = (*ac->arrStart) (rtaa);
      break;

      case RT_STR_START	    :
        if (ac->strStart)
          uDataRet = (*ac->strStart) (rtaa);
      break;

      case RT_STR_END	    :
        if (ac->strEnd)
          uDataRet = (*ac->strEnd) (rtaa);
      break;

      case RT_ARR_END	    :
        if (ac->arrEnd)
          uDataRet = (*ac->arrEnd) (rtaa);
      break;

      case RT_BOOLEANARRAY  :
      default :
	MLOG_ERR0 ("Bad RT asn1_tag");
      }
    }

/* All done with this RTAA, time to give it back			*/
  chk_free (runtime_aa);
  return (uDataRet);
  }


