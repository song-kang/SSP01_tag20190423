/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_init.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     06    Fix log messages.				*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 10/14/03  JRB     04    Del unnecessary casts.			*/
/* 09/24/99  JRB     03    (*upbInfo++) changed to (upbInfo++)          */
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/09/97  MDE     01    Created from existing MLOG source		*/
/************************************************************************/


#include "glbtypes.h"
#include "sysincs.h"

#ifndef MMS_LITE
#include "mms_usr.h"		/* to access MMS fucntions, variables   */
#else
#include "mmsdefs.h"
#endif
#include "mloguser.h"

/************************************************************************/
static ST_VOID logServiceSupport (ST_UCHAR *ubpInfo);
static ST_VOID logParameterSupport (ST_BOOLEAN bISFlag, ST_UCHAR *ubpInfo);

/************************************************************************/
/*                      m_log_init_info                                 */
/* This function is a general initiate information log function, and    */
/* is used by both u_indic and u_confrm initiate functions.             */
/************************************************************************/


ST_VOID m_log_init_info (INIT_INFO *info)
  {
CS_INIT_INFO *cs_info;
ST_INT i;
ST_BOOLEAN bISFlag;

  if (info->mms_p_context_pres)
    {   
    MLOG_CALWAYS0 ("MMS CORE P-CONTEXT INITIATE");
    if (info->max_segsize_pres)
      {
      MLOG_CALWAYS1 ("Max Segment Size = %ld.",(ST_LONG)info->max_segsize);
      }
    else
      {
      MLOG_CALWAYS0 ("Max Segment Size Not Present.");
      }
    MLOG_CALWAYS1 ("Max Outstanding Requests Calling = %d.",
                                              info->maxreq_calling);
    MLOG_CALWAYS1 ("Max Outstanding Requests Called = %d.",
                                              info->maxreq_called);
    if (info->max_nest_pres)
      {
      MLOG_CALWAYS1 ("Max Data Nesting = %d.",info->max_nest);
      }
    else
      {
      MLOG_CALWAYS0 ("Max Data Nesting Not Present.");
      }

    if (info->mms_detail_pres)
      {
      MLOG_CALWAYS0 ("MMS initiate Detail : ");
      MLOG_CALWAYS1 ("Version : %d",info->version);
      MLOG_CALWAYS0 ("Parameter Support : ");
      if (info->version == 0)
	bISFlag = SD_FALSE;
      else
	bISFlag = SD_TRUE;
      logParameterSupport (bISFlag, info->param_supp);

      MLOG_CALWAYS0 ("Service Support:");
      logServiceSupport (info->serv_supp);
      }
    else
      {
      MLOG_CALWAYS0 ("MMS initiate Detail : Not Present.");
      }
    MLOG_CALWAYS1 ("Core Context Position : %d",info->core_position);
    }
  else
    {
    MLOG_CALWAYS0 ("MMS CORE P-CONTEXT Not Present.");
    }

/* Now look at each CS initiate                                         */
  cs_info = (CS_INIT_INFO *) (info+1);
  for (i = 0; i < info->num_cs_init; ++i, ++cs_info)
    {
    MLOG_CALWAYS1 (" MMS CS P-CONTEXT INITIATE, CONTEXT = %04x",
                                        cs_info->p_context);
    if (cs_info->local_detail_pres)
      {
      MLOG_CALWAYS1 (" Local Detail = %ld",(ST_LONG)cs_info->local_detail);
      }
    else
      {
      MLOG_CALWAYS0 (" Local Detail Not Present ");
      }
    MLOG_CALWAYS1 (" max outst. requests calling = %d",cs_info->maxreq_calling);
    MLOG_CALWAYS1 (" max outst. requests called  = %d",cs_info->maxreq_called);
    if (cs_info->max_nest_pres)
      {
      MLOG_CALWAYS1 ("Max Data Nesting = %d.",cs_info->max_nest);
      }
    else
      {
      MLOG_CALWAYS0 ("Max Data Nesting Not Present.");
      }
    MLOG_CALWAYS0 ("CS Initiate Information Follows in HEX:");
    MLOG_ALWAYSH (cs_info->init_detail_len, cs_info->init_detail);
    }
  }

/* Indication Paramter Support Table Follows				*/
ST_CHAR *caIndParamTable [] =
/* actual BIT orders would be reversed from order in comments below	*/
  {	"STR1",		/* BIT 0	*/
   	"STR2",		/* BIT 1	*/
	"VNAM",		/* BIT 2	*/
	"VALT",		/* BIT 3	*/
	"VADR",		/* BIT 4	*/
	"VSCA",		/* BIT 5	*/
	"TYP", 		/* BIT 6	*/
	"VLIS",		/* BIT 7	*/
   	"REAL",		/* BIT 0	*/
	"AKEC",		/* BIT 1	*/
	"CEI", 		/* BIT 2	*/
	"", 		/* BIT 3	*/
	"",		/* BIT 4	*/
	"", 		/* BIT 5	*/
	"", 		/* BIT 6	*/
	"" 		/* BIT 7	*/
	};

/*************************************************************************/
/*  logServiceSupport							 */
/*************************************************************************/

static ST_VOID logServiceSupport (ST_UCHAR *ubpInfo)
  {
ST_UCHAR ucBitMask;
ST_INT nBit;
ST_INT nByte;
ST_INT nTotalBits;

  nTotalBits = 0;
  for (nByte = 0; nByte < 11; nByte++)
    {
    ucBitMask = 0x80;		/* gives '1000000' as byte to start	*/
    for (nBit=0; (nBit < 8) && (nTotalBits < 86); nBit++, nTotalBits++)
      {
      if ((*ubpInfo) & ucBitMask) 
        {
        MLOG_CALWAYS1 ("  %s", mms_op_string[nTotalBits]);
        }
      ucBitMask >>= 1;		/* shift bit mask to right one bit	*/
      }
    ubpInfo++;		/* increment pointer by one byte	*/
    }
  }

/************************************************************************/
/*  logParameterSupport							*/
/************************************************************************/

static ST_VOID logParameterSupport (ST_BOOLEAN bISFlag, ST_UCHAR *ubpInfo)
  {
ST_UCHAR ucBitMask;
ST_INT nBit;
ST_INT nByte;
ST_INT nTotalBits;

  if (bISFlag)			/* it is for IS		*/
    {
    nTotalBits = 0;
    for (nByte = 0; nByte < 2; nByte++)
      {
      ucBitMask = 0x80;		/* gives '1000000' as byte to start	*/
      for (nBit=0; (nBit < 8) && (nTotalBits < 11); nBit++, nTotalBits++)
        {
        if ((*ubpInfo) & ucBitMask) 
          {
          MLOG_CALWAYS1 ("  %s", caIndParamTable[nTotalBits]);
          }
        ucBitMask >>= 1;	/* shift bit mask to right one bit	*/
        }
      ubpInfo++;		/* increment pointer by one byte	*/
      }
    }
  else				/* it is for DIS		*/
    {
    ucBitMask = 0x80;		/* gives '1000000' as byte to start	*/
    for (nBit=0; nBit < 7; nBit++)
      {
      if ((*ubpInfo) & ucBitMask) 
        {
        MLOG_CALWAYS1 ("  %s", caIndParamTable[nBit]);
        }
      ucBitMask >>= 1;	/* shift bit mask to right one bit	*/
      }
    ubpInfo++;		/* increment pointer by one byte	*/
    }
  }

