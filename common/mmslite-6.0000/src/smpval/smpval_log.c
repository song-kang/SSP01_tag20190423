/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2011-2011, All Rights Reserved.					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smpval_log.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION : IEC 61850-9-2 SMPVAL logging functions.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			smpval_msg_log					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 12/14/11  JRB           Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "smpval.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			smpval_msg_log					*/
/************************************************************************/
ST_VOID smpval_msg_log (SMPVAL_MSG *smpvalMsg,
	ST_CHAR *hdr)	/* text to log on first line	*/
  {
ST_UINT j;
  SLOGALWAYS0 (hdr);
  SLOGCALWAYS1 ("edition = %d", smpvalMsg->edition);
  SLOGCALWAYS1 ("numASDU = %u", smpvalMsg->numASDU);
  SLOGCALWAYS1 ("securityLen = %d", smpvalMsg->securityLen);
  for (j = 0; j < smpvalMsg->numASDU; j++) 
    {
    SMPVAL_ASDU *asdu = &smpvalMsg->asduArray[j];
    SLOGCALWAYS1 ("ASDU # = %u", j);
    SLOGCALWAYS1 ("  SampleLen = %d", asdu->SampleLen);
    SLOGCALWAYS1 ("  svID = %s", asdu->svID);

    if (asdu->DatSetPres)
      SLOGCALWAYS1 ("  DatSet = %s", asdu->DatSet);
    else
      SLOGCALWAYS0 ("  DatSet (not present)");

    SLOGCALWAYS1 ("  SmpCnt = %u", asdu->SmpCnt);
    SLOGCALWAYS1 ("  ConfRev = %lu", (ST_ULONG) asdu->ConfRev);

    if (smpvalMsg->edition == 2)
      SLOGCALWAYS1 ("  SmpSynch (INT8U) = %u", asdu->SmpSynch_ed2);
    else
      SLOGCALWAYS1 ("  SmpSynch (Bool) = %u", asdu->SmpSynch);

    if (asdu->RefrTmPres)
      SLOGCALWAYS1 ("  RefrTm.secs = %lu", (ST_ULONG) asdu->RefrTm.secs);
    else
      SLOGCALWAYS0 ("  RefrTm (not present)");

    if (asdu->SmpRatePres)
      SLOGCALWAYS1 ("  SmpRate = %u", asdu->SmpRate);
    else
      SLOGCALWAYS0 ("  SmpRate (not present)");

    if (smpvalMsg->edition == 2)
      {		/* SmpMod is only in Edition 2	*/
      if (asdu->SmpModPres)
        SLOGCALWAYS1 ("  SmpMod = %u", asdu->SmpMod);
      else
        SLOGCALWAYS0 ("  SmpMod (not present)");
      }
    }
  }
  


