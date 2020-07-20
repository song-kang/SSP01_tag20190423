/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2000 - 2009, All Rights Reserved                            */
/*                                                                      */
/* MODULE NAME : logcfgx.c                                              */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :    This module processes each value parsed in   */
/*                         the logcfg.xml file .  The logcfg.xml file   */
/*                         assigns Log File Attributes and              */
/*                         Miscellaneous Control Flags as well as       */
/*                         Memory Use Logging and various Log Masks     */
/*                         to enable specific logging required.         */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev          Comments                                */
/* --------  ---  ------  --------------------------------------------  */
/* 03/09/12  EJV           Revised code to eliminate Klocwork warnings. */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 11/03/11  MDE	  Added PROCESS_DUMP support			*/
/* 07/14/11  MDE	  SLOG command handler default tweaks		*/
/* 04/22/11  JRB	  If fileName not configured, alloc default "mms.log"*/
/*			  logcfgx_unconfigure: close file, clear state.	*/
/* 03/30/11  JRB	  Added logcfgx_unconfigure.			*/
/* 01/14/10  MDE    54    Added SLOGIPC_MSG_TYPE_LOG_MEM command	*/
/* 11/18/10  MDE    53    Added heartbeat				*/
/* 11/10/10  MDE    52    Added return to slog_ipc_std_cmd_service_ex 	*/
/* 11/06/10  MDE    51    Added SEV_SUPPORT				*/
/* 08/25/10  MDE    50    Fixed leak in logCfgFindTagVal		*/
/* 06/07/10  MDE    49    Fixed crash when saving tag values	 	*/
/* 05/19/10  MDE    48    Tweaks to tag save/find for Logger use	*/
/* 02/19/10  MDE    47    Added lineCountLimit				*/
/* 08/20/09  MDE    46    Calling backoff is not set from XML file	*/
/* 06/10/09  EJV    45    Added LOGCFG_CFG to log settings after parsing*/
/*                        logCfgLogSettings: chg SLOGALWAYS to LOGCFG_CFG*/
/* 06/01/09  MDE    45    Aligned with SLOGIPC client changes		*/
/* 05/28/09  MDE    44    Fixed leak on reload masks 			*/
/* 04/06/09  EJV    43    Moved cfg params logging to logCfgLogSettings,*/
/*			   changed names as they appear in logcfg.xml.	*/
/* 01/03/09  MDE    42    Added SSEAL parameters    			*/
/* 11/07/08  MDE    41    Added 'Get Log Masks' feature support		*/
/* 09/09/08  MDE    40    Fixed problems with NULL destLogCtrl		*/
/* 08/11/08  MDE    39    Added set 'initialized' 			*/
/* 08/04/08  MDE    38    Now call slogIpcEventEx			*/
/* 07/25/08  MDE    37    Moved slogIpcCtx into LOG_CTRL		*/
/* 04/24/08  MDE    36    In fopen, removed 't' (not ANSI)		*/
/* 05/12/08  RKR    35    Fixed slog_ipc_std_cmd_service arg list,      */
/* 			  Added clone, reset, remote log message 	*/
/* 04/24/08  MDE    34    In slog_ipc_std_cmd_service, use text mode	*/
/* 04/07/08  MDE    33    Changed slog_ipc_std_cmd_service to reconnect	*/
/* 03/10/08  MDE    32    Added slog_ipc_std_cmd_service		*/
/* 03/05/08  JRB    31    Put #ifdef DEBUG_SISCO around entire file.	*/
/* 02/25/08  MDE    30    Rearranged table entries 			*/
/* 12/19/07  MDE    29    Fixed duplicate LOGCFGX_TAG_VAL on reload	*/
/* 10/29/07  JRB    28    strncat_safe obsolete, use strncat_maxstrlen.	*/
/* 10/23/07  MDE    27    Added ElapsedTime option 			*/
/* 10/10/07  MDE    26    Turn on timestamps by default, add tag	*/
/* 10/04/07  MDE    25    Fixed up preferred tag handling, etc.. 	*/
/* 05/30/07  MDE    24    Save tag values better			*/
/* 03/12/07  MDE    23    Added 'LogIpcEditLogCfg' 			*/
/* 07/28/06  MDE    22    Fixed logCfgClearTagVals			*/
/* 07/17/06  EJV    21    Add logCfgCallbackFunTYPE to avoid warnings.	*/
/* 04/03/06  RKR    20    Declare "tagVal" specific to DEBUT_SISCO      */
/* 03/21/06  GLB    19    Modify LOGCFGX_STATE_.. values         	*/
/* 03/03/06  EJV    18    AVOID using constants (use sizeof instead).	*/
/* 03/02/06  MDE    17    Fixed case where logCtrl == NULL		*/
/* 02/06/06  MDE    16    Added logcfg_exx, tagPath, etc.		*/
/* 02/02/06  EJV    15    _logcfg_data_end: corr _LOGCFG_DATATYPE_DOUBLE*/
/*			    Separated handling for signed and unsigned.	*/
/* 10/31/05  EJV    14    sx_get_... can fail, check for return.	*/
/*			  Use sx_parseExx_mt instead of sx_parseEx_mt.	*/
/*			  Chg sx_debug_sel_save.Revised _logcfg_data_end*/
/* 08/10/05  MDE    13    Added logCfgRemoveAllMaskGroups		*/
/* 06/21/05  EJV    12    Deleted obsolete logcfgx function.		*/
/* 05/23/05  EJV    11    Moved xxxLogMaskMapCtrl to respective C files.*/
/*			  Del m_track_prev_free (not used anymore).	*/
/*			  Force user to use logcfgx_ex insted of logcfgx*/
/* 04/25/05  MDE    10    Added MMS_LOG_CLIENT & MMS_LOG_SERVER		*/
/* 03/31/05  MDE    09    Put suicacse.h back, for MAP30_ACSE		*/
/* 03/28/05  JRB    08    Del suicacse header.              		*/
/* 02/10/05  MDE    07    Changed MILOG_ to MI_LOG_			*/
/* 02/10/05  MDE    06    Added new DATATYPE's				*/
/* 01/27/05  MDE    05    DEBUG_SISCO #ifdefs, LINUX warning cleanup	*/
/* 01/20/05  MDE    04    Lot's more work ...				*/
/* 12/10/04  ASK    03    Added Slog IPC params, added 			*/
/*		          case _LOGCFG_DATATYPE_UINT32_MASK		*/
/*			  Modify params to use 'R'everse data type	*/
/*                        Update copyright year                         */
/* 11/16/04  MDE    02    Fixed to compile with MAP30_ACSE		*/
/* 10/12/04  MDE    01    Complete rewrite to work with SX changes,     */
/*                        make extendable				*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "gen_list.h"
#include "str_util.h"
#include "glbsem.h"
#include "slog.h"

#include "sx_defs.h"
#include "sx_log.h"
#ifdef SEV_SUPPORT
#include "sev.h"
#include "ExceptionReport.h"
#endif

#if defined(DEBUG_SISCO)  /*code in this file needed only if this defined*/

/************************************************************************/

/* For debug version, use a static pointer to avoid duplication of      */
/* __FILE__ strings.                                                    */

#if defined(DEBUG_SISCO)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/


LOGCFG_VALUE_GROUP *logCfgLogCtrlGroupList;
LOGCFG_VALUE_GROUP *logCfgLogMaskGroupList;
static LOGCFGX_TAG_VAL *_tagValList;

/************************************************************************/
#if defined(DEBUG_SISCO)
static LOG_CTRL tmp_sLogCtrl;

ST_UINT logcfg_debug_sel;

/* Log type strings */
SD_CONST ST_CHAR *SD_CONST _logcfg_log_err_logstr 	= "LOGCFG_ERR";
SD_CONST ST_CHAR *SD_CONST _logcfg_log_nerr_logstr	= "LOGCFG_NERR";
SD_CONST ST_CHAR *SD_CONST _logcfg_log_flow_logstr	= "LOGCFG_FLOW";
SD_CONST ST_CHAR *SD_CONST _logcfg_log_cfg_logstr	= "LOGCFG_CFG";

LOGCFGX_VALUE_MAP logcfgMaskMaps[] =
  {
    {"LOGCFG_ERR",	LOGCFG_ERR,	&logcfg_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"LOGCFG_NERR",	LOGCFG_NERR,	&logcfg_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"LOGCFG_FLOW",	LOGCFG_FLOW,	&logcfg_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Flow"},
    {"LOGCFG_CFG",	LOGCFG_CFG,	&logcfg_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Settings"}
  };

LOGCFG_VALUE_GROUP logcfgMaskMapCtrl =
  {
  {NULL,NULL},
  "LOGCFG",
  sizeof(logcfgMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  logcfgMaskMaps
  };

/************************************************************************/
/************************************************************************/

LOGCFGX_VALUE_MAP logCfgLogMaskMaps[] =
  {
/* Common Log Message Formmatting Related */
    {"LogElapsedTime", 		LOG_ELAPSEDTIME_EN, 	&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_UINT32_MASK},
  /* Not frequently used */
    {"HeaderLogTime", 		LOG_TIME_EN, 		&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_RUINT32_MASK},
    {"HeaderCr", 		LOG_NO_HEADER_CR, 	&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_RUINT32_MASK},
    {"HeaderFilename", 		LOG_FILENAME_SUPPRESS, 	&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_RUINT32_MASK},
    {"HeaderLogType", 		LOG_LOGTYPE_SUPPRESS, 	&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_RUINT32_MASK},

/* File Logging Related */
    {"LogFileEnable", 		LOG_FILE_EN, 		&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_UINT32_MASK},
    {"LogFileSize",    		0, 			&tmp_sLogCtrl.fc.maxSize, 	_LOGCFG_DATATYPE_ULONG},
    {"LogFileName",    		0, 			&tmp_sLogCtrl.fc.fileName, 	_LOGCFG_DATATYPE_FILENAME},
    {"DestroyOldFile", 		FIL_CTRL_NO_APPEND, 	&tmp_sLogCtrl.fc.ctrl, 		_LOGCFG_DATATYPE_UINT_MASK},
    {"HardFlush", 		FIL_CTRL_HARD_FLUSH, 	&tmp_sLogCtrl.fc.ctrl, 		_LOGCFG_DATATYPE_UINT_MASK},
  /* Not frequently used */
    {"Setbuf", 			FIL_CTRL_SETBUF_EN, 	&tmp_sLogCtrl.fc.ctrl, 		_LOGCFG_DATATYPE_UINT_MASK},
    {"MsgHeader", 		FIL_CTRL_MSG_HDR_EN, 	&tmp_sLogCtrl.fc.ctrl, 		_LOGCFG_DATATYPE_UINT_MASK},
    {"Wipe", 			FIL_CTRL_WIPE_EN, 	&tmp_sLogCtrl.fc.ctrl, 		_LOGCFG_DATATYPE_UINT_MASK},
    {"Wrap", 			FIL_CTRL_WRAP_EN, 	&tmp_sLogCtrl.fc.ctrl, 		_LOGCFG_DATATYPE_UINT_MASK},

/* IPC Logging Related */
    {"LogIpcAppId", 	        0, 			&tmp_sLogCtrl.ipc.appId, 	_LOGCFG_DATATYPE_STRING},
    {"LogIpcSmartMode", 	LOG_IPC_SMART, 	        &tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_UINT32_MASK},
    {"LogIpcEditLogCfg", 	LOG_IPC_EDIT_LOGCFG,    &tmp_sLogCtrl.ipc.options,	_LOGCFG_DATATYPE_UINT32_MASK},
    {"LogIpcMaxQueCount",       0, 			&tmp_sLogCtrl.ipc.maxQueCnt, 	_LOGCFG_DATATYPE_UINT},
    {"LogIpcSealMode",          0, 			&tmp_sLogCtrl.ipc.sealMode, 	_LOGCFG_DATATYPE_INT},
    {"LogIpcSealTimeWindow",    0, 			&tmp_sLogCtrl.ipc.sealTimeWindow, _LOGCFG_DATATYPE_INT},
    {"LogIpcLineCountLimit",    0, 			&tmp_sLogCtrl.ipc.lineCountLimit, _LOGCFG_DATATYPE_INT},
    {"LogIpcHeartbeatTxPeriod", 0, 			&tmp_sLogCtrl.ipc.heartbeatTxPeriod, _LOGCFG_DATATYPE_DOUBLE},

  /* Listen */
    {"LogIpcListenEnable", 	LOG_IPC_LISTEN_EN, 	&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_UINT32_MASK},
    {"LogIpcListenPort",        0, 			&tmp_sLogCtrl.ipc.port, 	_LOGCFG_DATATYPE_UINT16},
    {"LogIpcNumListenPorts",    0, 			&tmp_sLogCtrl.ipc.portCnt, 	_LOGCFG_DATATYPE_UINT16},
    {"LogIpcMaxListenConn",     0, 			&tmp_sLogCtrl.ipc.maxConns, 	_LOGCFG_DATATYPE_UINT},

  /* Calling */
    {"LogIpcCallEnable", 	LOG_IPC_CALL_EN, 	&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_UINT32_MASK},
    {"LogIpcCallingPort",       0, 			&tmp_sLogCtrl.ipc.callingPort, 	_LOGCFG_DATATYPE_UINT16},
    {"LogIpcCallingIp",         0, 			&tmp_sLogCtrl.ipc.callingIp, 	_LOGCFG_DATATYPE_STRING},
    {"LogIpcCallingBackoff",    0, 			&tmp_sLogCtrl.ipc.callingBackoff,_LOGCFG_DATATYPE_ULONG},

/* Memory Logging Related */
    {"LogMemoryEnable", 	LOG_MEM_EN, 		&tmp_sLogCtrl.logCtrl, 		_LOGCFG_DATATYPE_UINT32_MASK},
    {"MemoryFileDump", 		MEM_CTRL_AUTODUMP_EN, 	&tmp_sLogCtrl.mc.ctrl, 		_LOGCFG_DATATYPE_UINT_MASK},
    {"LogMemoryItems", 		0, 			&tmp_sLogCtrl.mc.maxItems, 	_LOGCFG_DATATYPE_ULONG},
    {"MemoryFileName", 		0, 			&tmp_sLogCtrl.mc.dumpFileName,	_LOGCFG_DATATYPE_FILENAME},
  };

LOGCFG_VALUE_GROUP logCfgLogCtrlMapCtrl =
  {
  {NULL,NULL},
  "",
  sizeof(logCfgLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  logCfgLogMaskMaps
  };
#endif /* defined(DEBUG_SISCO) */


/************************************************************************/
/************************************************************************/

static ST_RET _logcfg_data_start (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag);
static ST_RET _logcfg_data_end (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag);

static ST_VOID _logcfg_proc_value_map (SX_DEC_CTRL *sxDecCtrl, 
				       LOGCFG_VALUE_GROUP *slogMaskGroup,
			               LOGCFGX_VALUE_MAP *slogValueMap, 
			               LOGCFGX_TAG_VAL *tagVal);

/************************************************************************/
/*			logCfgAddMaskGroup 				*/
/************************************************************************/

ST_VOID logCfgAddMaskGroup (LOGCFG_VALUE_GROUP *logMaskGroup)
  {
  if (list_find_node (logCfgLogMaskGroupList, logMaskGroup) != SD_SUCCESS)
    list_add_last (&logCfgLogMaskGroupList, logMaskGroup);
  }

/************************************************************************/
/*			logCfgRemoveMaskGroup 				*/
/************************************************************************/

ST_VOID logCfgRemoveMaskGroup (LOGCFG_VALUE_GROUP *logMaskGroup)
  {
  if (list_find_node (logCfgLogMaskGroupList, logMaskGroup) == SD_SUCCESS)
    list_unlink (&logCfgLogMaskGroupList, logMaskGroup);
  }

/************************************************************************/
/*			logCfgRemoveAllMaskGroups			*/
/************************************************************************/

ST_VOID logCfgRemoveAllMaskGroups ()
  {
  logCfgLogMaskGroupList = NULL;
  }

/************************************************************************/
/* 		_logcfg_save_logmasks					*/
/************************************************************************/

ST_RET _logcfg_save_logmasks (ST_CHAR *destFile)
  {
FILE *fp;
ST_INT xmlSize;
ST_CHAR *xmlBuf;
ST_RET rc;
ST_INT bytesWritten;

  xmlSize = _logcfg_get_logmasks_size ();
  xmlSize += SX_ENC_BUFFER_OH;
  xmlBuf = (ST_CHAR *)chk_malloc (xmlSize);

  rc = _logcfg_get_logmasks (xmlBuf, &xmlSize);
  if (rc == SD_SUCCESS)
    {
    fp = fopen (destFile, "w");
    if (fp != NULL)
      {
      bytesWritten = (int) fwrite (xmlBuf, 1, xmlSize, fp);
      fclose (fp);
      }
    else
      rc = SD_FAILURE;
    }

  chk_free (xmlBuf);
  return (rc);
  }

/************************************************************************/
/*			_logcfg_get_logmasks				*/
/************************************************************************/

ST_RET _logcfg_get_logmasks (ST_CHAR *xmlBuf, ST_INT *xmlSizeIo)
  {
LOGCFG_VALUE_GROUP *slogMaskGroup;
LOGCFGX_VALUE_MAP  *slogValueMap;
SX_ENC_CTRL         sxEncCtrl;
SXE_ATTR_PAIR       attr[1];
ST_INT              i;
ST_CHAR *v;

  sx_init_encode (&sxEncCtrl, xmlBuf, *xmlSizeIo - SX_ENC_BUFFER_OH);
  sxEncCtrl.bUseFormatting = SD_TRUE;
  sx_start_element (&sxEncCtrl, "LogMasks", 0, NULL);

  slogMaskGroup = logCfgLogMaskGroupList;
  while (slogMaskGroup)
    {
    if ((slogMaskGroup != &memDebugMapCtrl) && 
        (slogMaskGroup != &logcfgMaskMapCtrl))
      {
      sx_start_element (&sxEncCtrl, slogMaskGroup->name, 0, NULL);
      for (i = 0; i < slogMaskGroup->numMaskMap; ++i)
        {
        slogValueMap = &slogMaskGroup->maskMapTbl[i];

      /* Masks with "_ERR" or "_NERR" in them will be On, others Off */       
        v = "Off";
	if ((strstr (slogValueMap->tag, "_ERR") != NULL) ||
	    (strstr (slogValueMap->tag, "_NERR") != NULL))
	  {
          v = "On";
	  }

        if (slogValueMap->preferredTag)
	  {
          attr[0].name  = "desc";
          attr[0].value = slogValueMap->preferredTag;
          sx_wrx_string_el (&sxEncCtrl, slogValueMap->tag, v, 1, attr);
	  }
        else
          sx_wr_string_el (&sxEncCtrl, slogValueMap->tag, v);
        }
      sx_end_element (&sxEncCtrl);
      }
    slogMaskGroup = (LOGCFG_VALUE_GROUP *)list_get_next (logCfgLogMaskGroupList, slogMaskGroup);
    }

  sx_end_element (&sxEncCtrl);
  *xmlSizeIo = (int) SX_ENC_LEN (&sxEncCtrl);
  return (sxEncCtrl.errCode);
  }

/************************************************************************/
/*
<LogMasks>n</LogMasks>n
*/
#define _LOGCFG_XML_OH	23

/*
t<>nt</>n
*/
#define _LOGCFG_GROUP_OH 	9

/*
tt<>ntt</>n
*/
#define _LOGCFG_TAG_OH		11

/*
 desc=""
*/
#define _LOGCFG_DESC_OH		8


ST_INT _logcfg_get_logmasks_size ()
  {
ST_INT xmlSize;
LOGCFG_VALUE_GROUP *slogMaskGroup;
LOGCFGX_VALUE_MAP *slogValueMap;
ST_INT i;

  xmlSize = _LOGCFG_XML_OH;
  slogMaskGroup = logCfgLogMaskGroupList;
  while (slogMaskGroup)
    {
    if ((slogMaskGroup != &memDebugMapCtrl) && 
        (slogMaskGroup != &logcfgMaskMapCtrl))
      {
      xmlSize += _LOGCFG_GROUP_OH;
      xmlSize += (int) (2 * strlen (slogMaskGroup->name));
      for (i = 0; i < slogMaskGroup->numMaskMap; ++i)
        {
        slogValueMap = &slogMaskGroup->maskMapTbl[i];
        xmlSize += (_LOGCFG_TAG_OH + (int) (2 * strlen (slogValueMap->tag)));

        if (slogValueMap->preferredTag)
          xmlSize += (_LOGCFG_DESC_OH + (int) strlen (slogValueMap->preferredTag));
        }
      }
    slogMaskGroup = (LOGCFG_VALUE_GROUP*) list_get_next (logCfgLogMaskGroupList, slogMaskGroup);
    }
  return (xmlSize);
  }

/************************************************************************/
/************************************************************************/
/*			logCfgFindTagVal				*/
/************************************************************************/

LOGCFGX_TAG_VAL *logCfgFindTagValPath (ST_CHAR *tag)
  {
LOGCFGX_TAG_VAL *tagVal;

  tagVal = _tagValList;
  while (tagVal != NULL)
    {
    if (!strcmp (tag, tagVal->tagPath))
      return (tagVal);

    tagVal = (LOGCFGX_TAG_VAL *)list_get_next (_tagValList, tagVal);
    }
  return (NULL);
  }

/************************************************************************/
/*			logCfgChkTagValPath				*/
/************************************************************************/

/*
Not OK:
                *
    Old: XXX/YYY/ABC
    New: XXX/YYY

            *
    Old: XXX/YYY/ABC
    New: XXX

                    *
    Old: XXX/YYY/ABC
    New: XXX/YYY/ABC

OK:
                  *
    Old: XXX/YYY/ABC
    New: XXX/YYY/A

                 *
    Old: XXX/YYY/ABC
    New: XXX/YYY/B
*/

static ST_RET logCfgChkTagValPath (ST_CHAR *ntp)
  {
LOGCFGX_TAG_VAL *tagVal;
ST_CHAR *newTagPath;
ST_CHAR *oldTagPath;

  tagVal = _tagValList;
  while (tagVal != NULL)
    {
  /* Skip all the matching lead characters */
    oldTagPath = tagVal->tagPath;
    newTagPath = ntp;
    while ((*newTagPath == *oldTagPath) && *newTagPath && *oldTagPath)
      {
      ++newTagPath;
      ++oldTagPath;
      }

    if ((*oldTagPath == '/') && (*newTagPath == 0))
      return SD_FAILURE;

    if ((*oldTagPath == 0) && (*newTagPath == 0))
      return SD_FAILURE;
    

    tagVal = (LOGCFGX_TAG_VAL *)list_get_next (_tagValList, tagVal);
    }
  return SD_SUCCESS;
  }

/************************************************************************/
/*			logCfgFindTagVal				*/
/************************************************************************/
/* Tags can be saved in three forms:  					*/
/* 	component/group/tagVal 	: C# Logger for multiple components	*/
/* 	group/tagVal 	       	: XSLOG with single component		*/
/* 	tagVal 		       	: XSLOG with single component		*/

/* Find string can be:							*/
/* 	component/group/tagVal 	: Fully qualified			*/
/* 	component/tagVal 	: Legacy				*/
/* 	group/tagVal 	       	: XSLOG with single component		*/
/* 	tagVal 		       	: XSLOG with single component		*/

LOGCFGX_TAG_VAL *logCfgFindTagVal (ST_CHAR *tag)
  {
LOGCFGX_TAG_VAL *tagVal;
ST_CHAR *s;
ST_CHAR *p1;
ST_CHAR *p2;

/* First try using the full tag path */
  tagVal = logCfgFindTagValPath (tag);
  if (tagVal)
    return (tagVal);

/* No luck, now try looking for component/tagVal (caller left out group) */
  tagVal = _tagValList;
  while (tagVal != NULL)
    {
    s = chk_strdup (tagVal->tagPath);
    p1 = strstr (s, "/");
    if (p1)
      {
      p2 = strstr (p1+1, "/");
      if (p2)
        {
	/* OK, the logcfg.xml tag format is OK */
        while (*p2 != 0)
	  *(p1++) = *(p2++);
	*p1 = 0;

        /* We have removed the group, check again ... */
        if (!strcmp (tag, s))
	  {
	  chk_free (s);
          return (tagVal);
	  }
	}
      }
    chk_free (s);
    tagVal = (LOGCFGX_TAG_VAL *)list_get_next (_tagValList, tagVal);
    }

/* No luck, now just look for tag (caller left out group) */
  tagVal = _tagValList;
  while (tagVal != NULL)
    {
    if (!strcmp (tag, tagVal->tag))
      return (tagVal);

    tagVal = (LOGCFGX_TAG_VAL *)list_get_next (_tagValList, tagVal);
    }
  return (NULL);
  }


/************************************************************************/
/*			logCfgClearTagVals				*/
/************************************************************************/

ST_VOID logCfgClearTagVals ()
  {
LOGCFGX_TAG_VAL *tagVal;

  while (_tagValList)
    {
    tagVal = _tagValList;
    list_unlink (&_tagValList, _tagValList);
    if (tagVal->dataType == _LOGCFG_DATATYPE_STRING || 
        tagVal->dataType == _LOGCFG_DATATYPE_FILENAME)
      {
      chk_free (tagVal->u.str);
      }
    chk_free (tagVal);
    }
  }

/************************************************************************/
/*			logCfgGetTagValList				*/
/************************************************************************/

LOGCFGX_TAG_VAL *logCfgGetTagValList ()
  {
  return (_tagValList);
  }

/************************************************************************/
/************************************************************************/
/*                                logcfgx_exx                           */
/* User must call logCfgAddMaskGroup for each group of log masks to be	*/
/* configured BEFORE calling this function to parse the file.		*/
/************************************************************************/

#define _LOGCFG_MAX_NAME_LEVEL	10

#define LOGCFGX_STATE_NONE      	0
#define LOGCFGX_STATE_UNKNOWN		1
#define LOGCFGX_STATE_LOGCTRL		2
#define LOGCFGX_STATE_LOOK_COMPNAME	3
#define LOGCFGX_STATE_LOGMASKS		4

typedef struct
  {
  LOG_CTRL *destLogCtrl;
  ST_INT     state;
  ST_CHAR   *logFileName; 
  ST_CHAR   *fileNamePrefix;
  ST_BOOLEAN masksOnly;
  ST_BOOLEAN saveTagVals;
  ST_CHAR   *compName;

  ST_INT    logMaskNestLevel;
  ST_CHAR   logMaskNameStack[_LOGCFG_MAX_NAME_LEVEL][SX_MAX_TAG_LEN+1];
  } LOGCFGX_CTRL;


ST_RET logcfgx_ex (LOG_CTRL *destLogCtrl, ST_CHAR *logFileName, ST_CHAR *fileNamePrefix, 
		   ST_BOOLEAN masksOnly, ST_BOOLEAN saveTagVals)
  {
  return (logcfgx_exx (destLogCtrl, logFileName, fileNamePrefix, 
		       masksOnly, saveTagVals, NULL));
  }		     


ST_RET logcfgx_exx (LOG_CTRL *destLogCtrl, ST_CHAR *logFileName, ST_CHAR *fileNamePrefix, 
		    ST_BOOLEAN masksOnly, ST_BOOLEAN saveTagVals, ST_CHAR *compName)

  {
LOGCFGX_CTRL logCfgxCtrl;
ST_RET rc;
#if defined(DEBUG_SISCO)
ST_CHAR *savedFileName;
#endif

  logCfgxCtrl.destLogCtrl     = destLogCtrl;
  logCfgxCtrl.logFileName     =	logFileName; 
  if (fileNamePrefix != NULL) 
    logCfgxCtrl.fileNamePrefix  = fileNamePrefix; 
  else
    logCfgxCtrl.fileNamePrefix  =  ""; 

  logCfgxCtrl.masksOnly       =	masksOnly;
  logCfgxCtrl.saveTagVals     =	saveTagVals;
  logCfgxCtrl.compName        =	compName;
  logCfgxCtrl.logMaskNestLevel = 0;
  logCfgxCtrl.state = LOGCFGX_STATE_NONE;   

#if defined(DEBUG_SISCO)
  logcfg_debug_sel |= LOGCFG_ERR;
  logcfg_debug_sel |= LOGCFG_NERR;
  sx_debug_sel     |= SX_LOG_ERR;
  sx_debug_sel     |= SX_LOG_NERR;

  if (logCfgLogCtrlGroupList == NULL)
    list_add_last (&logCfgLogCtrlGroupList, &logCfgLogCtrlMapCtrl);

/* Always allow LOGCFG & SX logging ... */
  logCfgAddMaskGroup (&logcfgMaskMapCtrl);
  logCfgAddMaskGroup (&sxLogMaskMapCtrl);

  if (destLogCtrl != NULL)
    {
    savedFileName = destLogCtrl->fc.fileName;

  /* Copy original "destLogCtrl" to temporary struct which may be modified */
  /* by the configuration.                                              */
  /* This prevents logging control changes while there may be logging.  */

    memcpy (&tmp_sLogCtrl, destLogCtrl, sizeof (LOG_CTRL));
    /* Init fileName=NULL, so we can see later if it was allocated.	*/
    tmp_sLogCtrl.fc.fileName = NULL;
    }

#endif  /* defined(DEBUG_SISCO) */

  if (destLogCtrl != NULL)
    {
  /* Set reasonable default values */
    tmp_sLogCtrl.logCtrl |= LOG_TIME_EN;
    tmp_sLogCtrl.logCtrl |= LOG_IPC_SMART;
    tmp_sLogCtrl.fc.ctrl |= FIL_CTRL_MSG_HDR_EN;
    tmp_sLogCtrl.fc.ctrl |= FIL_CTRL_WIPE_EN;
    tmp_sLogCtrl.fc.ctrl |= FIL_CTRL_WRAP_EN;
    tmp_sLogCtrl.ipc.heartbeatTxPeriod = 0.0;
    }

  rc = sx_parseExx_mt (logFileName, 0, NULL, &logCfgxCtrl, _logcfg_data_start, _logcfg_data_end);
#if defined(DEBUG_SISCO)
  if (rc != SD_SUCCESS)
    {
    /* Config failed: Do not modify "destLogCtrl".                         */
    /* Turn on cfg logging and parse file again.                        */
    if (destLogCtrl != NULL)
      destLogCtrl->logCtrl |= LOG_FILE_EN;

    sx_debug_sel |= SX_LOG_DEC | SX_LOG_DEBUG | SX_LOG_FLOW;
    logcfg_debug_sel |= LOGCFG_ERR | LOGCFG_NERR | LOGCFG_FLOW;
    LOGCFG_ERR0 ("ERROR Parsing Logging Configuration File: Trying again w/debug on ... ");
    rc = sx_parseExx_mt (logFileName, 0,NULL,&logCfgxCtrl, _logcfg_data_start, _logcfg_data_end);
    }
  else    /* rc == SD_SUCCESS */
    {
    if (destLogCtrl != NULL)
      destLogCtrl->initialized = SD_TRUE;

    logCfgLogSettings (destLogCtrl, SD_TRUE);
    }
#endif  /* defined(DEBUG_SISCO) */

  /* store the SX masks now  (saved during parsing) */
  sx_debug_sel = sx_debug_sel_cfg;
  return (rc);
  }

/************************************************************************/
/*			logCfgLogSettings				*/
/*----------------------------------------------------------------------*/
/* Log the settings for logging.					*/
/* The LOG_CTRL parameters will be logged only if non-NULL lc is passed	*/
/* (and lc must be initialized). Logging log masks is optional.		*/
/* If lc=NULL and bLogMasks=SD_TRUE only log masks will be logged.	*/
/************************************************************************/
ST_VOID logCfgLogSettings (LOG_CTRL *lc, ST_BOOLEAN bLogMasks)
  {
LOGCFGX_TAG_VAL *tagVal;

#if defined(DEBUG_SISCO)
  if (!(logcfg_debug_sel & LOGCFG_CFG))
    return;	/* do not continue if this mask is not set */

  /* lock to log the whole cfg in one chunk (in multhithreaded apps)	*/
  S_LOCK_UTIL_RESOURCES ();

  LOGCFG_CFG0 ("Logging configuration:");

  /* NOTE: parameters present in the Template Log Configuration XML are	*/
  /*	   logged here, other (optional) params are logged conditionally*/
  if (lc != NULL && lc->initialized)
    {
    LOGCFG_CCFG0 ("");
    LOGCFG_CCFG0 ("Log Control");
    LOGCFG_CCFG0 ("  Log Common");
    /* slog only if not turned ON by default */
    if (!(lc->logCtrl & LOG_TIME_EN))
      LOGCFG_CCFG1 ("    Timestamps           : %s",  lc->logCtrl & LOG_TIME_EN        ? "On" : "Off");
    LOGCFG_CCFG1 ("    LogElapsedTime       : %s",  lc->logCtrl & LOG_ELAPSEDTIME_EN ? "On" : "Off");

    LOGCFG_CCFG0 ("  LogFileAttributes");
    LOGCFG_CCFG1 ("    LogFileEnable        : %s",  lc->logCtrl & LOG_FILE_EN ? "On" : "Off");
    if (lc->logCtrl & LOG_FILE_EN)
      {
      LOGCFG_CCFG1 ("    LogFileName          :'%s'", lc->fc.fileName);
      LOGCFG_CCFG1 ("    LogFileSize          : %lu", lc->fc.maxSize);
      LOGCFG_CCFG1 ("    DestroyOldFile       : %s",  lc->fc.ctrl & FIL_CTRL_NO_APPEND  ? "On" : "Off");
      LOGCFG_CCFG1 ("    HardFlush            : %s",  lc->fc.ctrl & FIL_CTRL_HARD_FLUSH ? "On" : "Off");

      /* slog only if not turned ON by default */
      if (!(lc->fc.ctrl & FIL_CTRL_WIPE_EN))
        LOGCFG_CCFG1 ("    Wipe                 : %s",  lc->fc.ctrl & FIL_CTRL_WIPE_EN    ? "On" : "Off");
      if (!(lc->fc.ctrl & FIL_CTRL_WRAP_EN))
        LOGCFG_CCFG1 ("    Wrap                 : %s",  lc->fc.ctrl & FIL_CTRL_WRAP_EN    ? "On" : "Off");
      if (!(lc->fc.ctrl & FIL_CTRL_MSG_HDR_EN))
        LOGCFG_CCFG1 ("    Header               : %s",  lc->fc.ctrl & FIL_CTRL_MSG_HDR_EN ? "On" : "Off");
      if (!(lc->fc.ctrl & FIL_CTRL_SETBUF_EN))
        LOGCFG_CCFG1 ("    Setbuf               : %s",  lc->fc.ctrl & FIL_CTRL_SETBUF_EN  ? "On" : "Off");
      }
    					    
    if (lc->logCtrl & LOG_MEM_EN)
      {
      LOGCFG_CCFG0 ("  LogMemoryAttributes");
      LOGCFG_CCFG1 ("    LogMemoryEnable      : %s",  lc->logCtrl & LOG_MEM_EN           ? "On" : "Off");
      LOGCFG_CCFG1 ("    MemoryFileDump       : %s",  lc->mc.ctrl & MEM_CTRL_AUTODUMP_EN ? "On" : "Off");
      LOGCFG_CCFG1 ("    MemoryFileName       :'%s'",  lc->mc.dumpFileName);
      LOGCFG_CCFG1 ("    LogMemoryItems       : %d",  lc->mc.maxItems);
      }

    LOGCFG_CCFG0 ("  LogIpcAttributes");
    LOGCFG_CCFG1 ("    LogIpcAppId          :'%s'", lc->ipc.appId);
    LOGCFG_CCFG1 ("    LogIpcMaxQueCount    : %u",  lc->ipc.maxQueCnt);
    LOGCFG_CCFG0 ("");
    LOGCFG_CCFG1 ("    LogIpcListenEnable   : %s",  lc->logCtrl & LOG_IPC_LISTEN_EN ? "On" : "Off");
    LOGCFG_CCFG1 ("    LogIpcListenPort     : %u",  (unsigned int) lc->ipc.port);
    LOGCFG_CCFG1 ("    LogIpcNumListenPorts : %d",  lc->ipc.portCnt);
    LOGCFG_CCFG1 ("    LogIpcMaxListenConn  : %d",  lc->ipc.maxConns);
    LOGCFG_CCFG0 ("");
    LOGCFG_CCFG1 ("    LogIpcCallEnable     : %s",  lc->logCtrl & LOG_IPC_CALL_EN   ? "On" : "Off");
    LOGCFG_CCFG1 ("    LogIpcCallingPort    : %u",  (unsigned int ) lc->ipc.callingPort);
    LOGCFG_CCFG1 ("    LogIpcCallingIp      : %s",  lc->ipc.callingIp);
    LOGCFG_CCFG1 ("    LogIpcCallingBackoff : %lu", lc->ipc.callingBackoff);
    LOGCFG_CCFG0 ("");
    LOGCFG_CCFG1 ("    LogIpcSmartMode      : %s",  lc->logCtrl & LOG_IPC_SMART           ? "On" : "Off");
    LOGCFG_CCFG1 ("    LogIpcEditLogCfg     : %s",  lc->ipc.options & LOG_IPC_EDIT_LOGCFG ? "On" : "Off");
    LOGCFG_CCFG1 ("    LogIpcSealMode       : %d",  lc->ipc.sealMode);
    LOGCFG_CCFG1 ("    LogIpcTimeWindow     : %d secs",  lc->ipc.sealTimeWindow);
    }

  if (bLogMasks)
    {
    LOGCFG_CCFG0 ("");
    LOGCFG_CCFG0 ("LogMasks");
    tagVal = _tagValList;
    if (tagVal != NULL)
      {
      while (tagVal != NULL)
        {
        if (tagVal->dataType == _LOGCFG_DATATYPE_STRING)
          LOGCFG_CCFG0 ("");  /* this is end of logmask tag */
	else
          LOGCFG_CCFG2 ("  %-20.20s\t%s",  tagVal->tag, (tagVal->u.b ? "On" : "Off" ));
        tagVal = (LOGCFGX_TAG_VAL *)list_get_next (_tagValList, tagVal);
        }
      }
    }

  LOGCFG_CCFG0 ("Logging configuration complete");

  S_UNLOCK_UTIL_RESOURCES ();
#endif  /* defined(DEBUG_SISCO) */
  }


/************************************************************************/
/************************************************************************/
/*			_logcfg_data_start				*/
/************************************************************************/

static ST_RET _logcfg_data_start (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag)
  {
LOGCFGX_CTRL *logCfgxCtrl;

  logCfgxCtrl = (LOGCFGX_CTRL *)sxDecCtrl->usr;

/* See if we are starting the LogControl section */
  if (!strcmp (tag, "LogControl"))
    {
    logCfgxCtrl->state = LOGCFGX_STATE_LOGCTRL;
    return (SD_SUCCESS);
    }

/* See if we are starting the LogMasks section */
  if (!strcmp (tag, "LogMasks"))
    {
    if (logCfgxCtrl->compName)
      logCfgxCtrl->state = LOGCFGX_STATE_LOOK_COMPNAME;
    else
      {
      logCfgxCtrl->state = LOGCFGX_STATE_LOGMASKS;
      logCfgxCtrl->logMaskNestLevel = 0;
      }
    return (SD_SUCCESS);
    }

/* See if we are looking for our component tag */
  if (logCfgxCtrl->state == LOGCFGX_STATE_LOOK_COMPNAME)
    {
    if (!strcmp (tag, logCfgxCtrl->compName))
      {
      logCfgxCtrl->state = LOGCFGX_STATE_LOGMASKS;
      logCfgxCtrl->logMaskNestLevel = 0;
      }
    return (SD_SUCCESS);
    }


/* If we are in the logmasks state, record the tag stack for use below */
  if (logCfgxCtrl->state == LOGCFGX_STATE_LOGMASKS)
    {
    strcpy (logCfgxCtrl->logMaskNameStack[logCfgxCtrl->logMaskNestLevel], tag);
    ++logCfgxCtrl->logMaskNestLevel;
    return (SD_SUCCESS);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_logcfg_data_end				*/
/************************************************************************/

#define _MAX_TAGPATH	200

static ST_RET _logcfg_data_end (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag)
  {
LOGCFGX_CTRL *logCfgxCtrl;
LOGCFG_VALUE_GROUP *slogMaskGroup;
LOGCFGX_VALUE_MAP *slogValueMap;
LOG_CTRL *destLogCtrl;
LOGCFGX_TAG_VAL *tagVal;
SX_DEC_ELEMENT_INFO *sxDecElInfo; 
ST_CHAR *str;
ST_INT i;
ST_INT strLen;
#if defined(DEBUG_SISCO)
ST_CHAR *oldFileName;
ST_BOOLEAN logFileChanged;
#endif
ST_BOOLEAN found;
ST_CHAR *p;
ST_CHAR *parentTag;
ST_CHAR tagPath[_MAX_TAGPATH+1];

  logCfgxCtrl = (LOGCFGX_CTRL *)sxDecCtrl->usr;

/* See if this is the End of the LogMasks section */
  if (!strcmp (tag, "LogMasks"))
    {
    logCfgxCtrl->state = LOGCFGX_STATE_NONE;
    return (SD_SUCCESS);
    }

/* See if this is the End of the file */
  if (!strcmp (tag, "LOG_CFG"))
    return (SD_SUCCESS);


  destLogCtrl = logCfgxCtrl->destLogCtrl;
  sxDecElInfo = &sxDecCtrl->sxDecElInfo; 

/* See if this is the End of the LogControl section */
  if (!strcmp (tag, "LogControl"))
    {
#if defined(DEBUG_SISCO)
    logFileChanged = SD_FALSE;
    if (logCfgxCtrl->masksOnly == SD_FALSE)
      {
      /* If fileName is still NULL, allocate default name.	*/
      if (tmp_sLogCtrl.fc.fileName == NULL)
        tmp_sLogCtrl.fc.fileName = chk_strdup ("mms.log");

      /* If the log file name has changed and the log file is open, 	*/
      /* close the log file and clear state.				*/
      
      if (destLogCtrl->fc.state & FIL_STATE_OPEN)
        {                 
        oldFileName = destLogCtrl->fc.fileName;
#ifdef _WIN32   /* Filenames NOT case sensitive on Windows  */
        if (stricmp (tmp_sLogCtrl.fc.fileName, oldFileName) != 0)
#else
        if (strcmp (tmp_sLogCtrl.fc.fileName, oldFileName) != 0)
#endif
          {   /* NEW log file name different from OLD.    */
          fclose (destLogCtrl->fc.fp);           /* close it */
          destLogCtrl->fc.state &= ~(FIL_STATE_OPEN);    /* clear state  */
          logFileChanged = SD_TRUE;
          }
        }
      if (tmp_sLogCtrl.fc.ctrl & FIL_CTRL_MSG_HDR_EN)
        tmp_sLogCtrl.mc.ctrl |= MEM_CTRL_MSG_HDR_EN;
      else
        tmp_sLogCtrl.mc.ctrl &= ~MEM_CTRL_MSG_HDR_EN;
  
      destLogCtrl->logCtrl         = tmp_sLogCtrl.logCtrl;
      destLogCtrl->fc.ctrl         = tmp_sLogCtrl.fc.ctrl;
      destLogCtrl->fc.fileName     = tmp_sLogCtrl.fc.fileName;
      destLogCtrl->fc.maxSize      = tmp_sLogCtrl.fc.maxSize;
      destLogCtrl->mc.ctrl         = tmp_sLogCtrl.mc.ctrl;
      destLogCtrl->mc.dumpFileName = tmp_sLogCtrl.mc.dumpFileName;
      destLogCtrl->mc.maxItems     = tmp_sLogCtrl.mc.maxItems;
      destLogCtrl->ipc.appId 	   = tmp_sLogCtrl.ipc.appId; 	
      destLogCtrl->ipc.options	   = tmp_sLogCtrl.ipc.options; 	
      destLogCtrl->ipc.port 	   = tmp_sLogCtrl.ipc.port; 	
      destLogCtrl->ipc.portCnt     = tmp_sLogCtrl.ipc.portCnt; 	
      destLogCtrl->ipc.maxConns    = tmp_sLogCtrl.ipc.maxConns; 	
      destLogCtrl->ipc.callingIp   = tmp_sLogCtrl.ipc.callingIp; 
      destLogCtrl->ipc.callingPort = tmp_sLogCtrl.ipc.callingPort;
      destLogCtrl->ipc.callingBackoff = tmp_sLogCtrl.ipc.callingBackoff;
      destLogCtrl->ipc.maxQueCnt      = tmp_sLogCtrl.ipc.maxQueCnt; 
      destLogCtrl->ipc.sealMode       = tmp_sLogCtrl.ipc.sealMode; 
      destLogCtrl->ipc.sealTimeWindow = tmp_sLogCtrl.ipc.sealTimeWindow; 
      destLogCtrl->ipc.lineCountLimit = tmp_sLogCtrl.ipc.lineCountLimit; 
      destLogCtrl->ipc.heartbeatTxPeriod = tmp_sLogCtrl.ipc.heartbeatTxPeriod; 
      }
    else /* Masks only, free up allocated strings */
      {
      if (tmp_sLogCtrl.fc.fileName != NULL)
        chk_free (tmp_sLogCtrl.fc.fileName);

      if (tmp_sLogCtrl.mc.dumpFileName != NULL)
        chk_free (tmp_sLogCtrl.mc.dumpFileName);

      if (tmp_sLogCtrl.ipc.callingIp != NULL)
        chk_free (tmp_sLogCtrl.ipc.callingIp);

      if (tmp_sLogCtrl.ipc.appId != NULL)
        chk_free (tmp_sLogCtrl.ipc.appId);
      }
    LOGCFG_FLOW2 ("LogCfg loading file %s, prefix %s", 
		      logCfgxCtrl->logFileName, logCfgxCtrl->fileNamePrefix);

    if (logFileChanged)
      {
      _slog (destLogCtrl,_logcfg_log_flow_logstr, thisFileName,__LINE__, 
        	"See the file '%s' for messages logged before or during Logging config",
                  oldFileName);
      }
#endif  /* defined(DEBUG_SISCO) */
    logCfgxCtrl->state = LOGCFGX_STATE_NONE;
    return (SD_SUCCESS);
    } /* end of <LogCtrl> handling */


/* See if we are in the LogCtrl section */
  if (logCfgxCtrl->state == LOGCFGX_STATE_LOGCTRL)
    {
  /* See if we can find it in the selected table set ... */
  /* First try to use the group name/preferredTag to find the value map */
    found = SD_FALSE;
    if (logCfgxCtrl->logMaskNestLevel > 1)  
      {
      parentTag = logCfgxCtrl->logMaskNameStack[logCfgxCtrl->logMaskNestLevel-2];
      slogMaskGroup = logCfgLogCtrlGroupList;
      while (slogMaskGroup)
        {
        if (!strcmp (slogMaskGroup->name, parentTag))
          {
        /* OK, found the right group */
          for (i = 0; !found && (i < slogMaskGroup->numMaskMap); ++i)
            {
	    slogValueMap = &slogMaskGroup->maskMapTbl[i];
          /* Accept either the preferred tag or the tag */
            if (slogValueMap->preferredTag && !strcmp (tag, slogValueMap->preferredTag))
              found = SD_TRUE;
            else if (!strcmp (tag, slogValueMap->tag))
              found = SD_TRUE;
	    }
	  }
        if (found)
          break;

        slogMaskGroup = (LOGCFG_VALUE_GROUP *)list_get_next (logCfgLogCtrlGroupList, slogMaskGroup);
        }
      }

  /* If that did not work, look for the unqualified tag */
    if (!found)
      {
      slogMaskGroup = logCfgLogCtrlGroupList;
      while (slogMaskGroup)
        {
        for (i = 0; !found && (i < slogMaskGroup->numMaskMap); ++i)
          {
	  slogValueMap = &slogMaskGroup->maskMapTbl[i];
          if (!strcmp (tag, slogValueMap->tag))
            found = SD_TRUE;
	  }
        if (found)
          break;

        slogMaskGroup = (LOGCFG_VALUE_GROUP *)list_get_next (logCfgLogCtrlGroupList, slogMaskGroup);
        }
      }

  /* If we found the tag, process it appropriately */
    if (found)
      _logcfg_proc_value_map (sxDecCtrl, slogMaskGroup, slogValueMap, NULL);

    return (SD_SUCCESS);
    }

/* logCfgxCtrl->state != LOGCFGX_STATE_LOGCTRL		   */
/* This might be an Log Mask or other value of interest */
  if (logCfgxCtrl->state == LOGCFGX_STATE_LOGMASKS || 
      logCfgxCtrl->state == LOGCFGX_STATE_NONE)
    {
  /* If we are to save tag values for LogMasks, do it now */
    if (logCfgxCtrl->saveTagVals && logCfgxCtrl->logMaskNestLevel > 0)
      {
    /* Assemble the path */
      tagPath[0] = 0;
      for (i = 0; i < logCfgxCtrl->logMaskNestLevel; ++i)
        {
        if (i > 0)
          strncat_maxstrlen (tagPath, "/", _MAX_TAGPATH);

        strncat_maxstrlen (tagPath, logCfgxCtrl->logMaskNameStack[i], _MAX_TAGPATH);
        }

    /* See if the TagVal already is in our list, and free it if so */
      tagVal = logCfgFindTagValPath (tagPath);
      if (tagVal)
        {
        list_unlink (&_tagValList, tagVal);
        if (tagVal->dataType == _LOGCFG_DATATYPE_STRING || 
            tagVal->dataType == _LOGCFG_DATATYPE_FILENAME)
          {
          chk_free (tagVal->u.str);
          }
        chk_free (tagVal);
        }

      /* See of it is OK to add this one */
      if (logCfgChkTagValPath(tagPath) == SD_SUCCESS)
        {
      /* OK, allocate the new one */
        tagVal = (LOGCFGX_TAG_VAL *)chk_calloc (1, (unsigned int) (sizeof (LOGCFGX_TAG_VAL) + 
        			      (strlen (tag) + 1) + 
        			      (strlen (tagPath) + 1))) ;

        p = (ST_CHAR *) (tagVal + 1);
        tagVal->tag = p;
        strcpy (p, tag);
        p = (ST_CHAR *) (p + (strlen (tag) + 1));
        tagVal->tagPath = p;
        strcpy (p, tagPath);
        list_add_last (&_tagValList, tagVal);
        }
      else	/* This is not a leaf, don't add it */
        tagVal = NULL;
      }
    else
      tagVal = NULL;

/* Now see if we can find it in the selected tag set ... */
/* First try to use the GroupName/preferredTag or GroupName/Tag to find the value map */
    found = SD_FALSE;
    if (logCfgxCtrl->logMaskNestLevel > 1)  
      {
      parentTag = logCfgxCtrl->logMaskNameStack[logCfgxCtrl->logMaskNestLevel-2];
      slogMaskGroup = logCfgLogMaskGroupList;
      while (slogMaskGroup)
        {
        if (!strcmp (slogMaskGroup->name, parentTag))
          {
        /* OK, found the right group */
          for (i = 0; !found && (i < slogMaskGroup->numMaskMap); ++i)
            {
          /* Accept either the preferred tag or the tag */
	    slogValueMap = &slogMaskGroup->maskMapTbl[i];
            if (slogValueMap->preferredTag && !strcmp (tag, slogValueMap->preferredTag))
              found = SD_TRUE;
	    else if (!strcmp (tag, slogValueMap->tag))
              found = SD_TRUE;
  	    }
          }
        if (found)
          break;

        slogMaskGroup = (LOGCFG_VALUE_GROUP *)list_get_next (logCfgLogMaskGroupList, slogMaskGroup);
        }
      }

  /* If that did not work, settle for the unqualified tag */
    if (!found)
      {
      slogMaskGroup = logCfgLogMaskGroupList;
      while (slogMaskGroup)
        {
        for (i = 0; !found && (i < slogMaskGroup->numMaskMap); ++i)
          {
	  slogValueMap = &slogMaskGroup->maskMapTbl[i];
          if (!strcmp (tag, slogValueMap->tag))
            found = SD_TRUE;
	  }
        if (found)
          break;

        slogMaskGroup = (LOGCFG_VALUE_GROUP *)list_get_next (logCfgLogMaskGroupList, slogMaskGroup);
        }
      }

  /* If we found the tag, process it appropriately */
    if (found)
      {
      if (tagVal != NULL)
        tagVal->dataType = slogValueMap->dataType;

      _logcfg_proc_value_map (sxDecCtrl, slogMaskGroup, slogValueMap, tagVal);
      }
    else 
      {
    /* We did not find this tag */
      if (tagVal != NULL)
        {
        strLen = 0;
        if (sx_get_string_ptr (sxDecCtrl, &str, &strLen) == SD_SUCCESS)
          {
          if (!stricmp (str, "On"))
            {
            LOGCFG_CFLOW1 ("Saving  log mask %-20s  ON  (ignored)", tag);
            tagVal->u.b = SD_TRUE;
            tagVal->dataType = _LOGCFG_DATATYPE_BOOLEAN;
            }
          else if (!stricmp (str, "Off"))
            {
            LOGCFG_CFLOW1 ("Saving  log mask %-20s  OFF (ignored)", tag);
            tagVal->u.b = SD_FALSE;
            tagVal->dataType = _LOGCFG_DATATYPE_BOOLEAN;
            }
          else
            {
            /* DEBUG: currently str may be unusable if the tag is an end of a XML subsection */
            /* LOGCFG_CFLOW2 ("Saving           %-20s  '%s'", tag, str); */
            tagVal->u.str = chk_strdup (str);
            tagVal->dataType = _LOGCFG_DATATYPE_STRING;
            }
          }
        }
      else /* We are not saving log mask values */
        {
        /* log only tags that could be log masks */
        if (strstr (tag, "LOG_"))
          LOGCFG_CFLOW1 ("        log mask %-20s  ignored", tag);
        }
      }

  /* If this was the end of the selected component, switch states */
    if (logCfgxCtrl->compName && !strcmp (tag, logCfgxCtrl->compName))
      logCfgxCtrl->state = LOGCFGX_STATE_LOOK_COMPNAME;

    --logCfgxCtrl->logMaskNestLevel;
    } /* End of 'if logCfgxCtrl->state == LOGCFGX_STATE_LOGMASKS */

  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			_logcfg_proc_value_map				*/
/************************************************************************/

static ST_VOID _logcfg_proc_value_map (SX_DEC_CTRL *sxDecCtrl, 
				       LOGCFG_VALUE_GROUP *slogMaskGroup,
			               LOGCFGX_VALUE_MAP *slogValueMap, 
			               LOGCFGX_TAG_VAL *tagVal)
  {
LOGCFGX_CTRL *logCfgxCtrl;
typedef ST_VOID (*logCfgCallbackFunTYPE) (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag,
					  LOGCFGX_VALUE_MAP *valMap);
logCfgCallbackFunTYPE logCfgCallbackFun;
ST_CHAR *str;
ST_BOOLEAN b;
ST_INT strLen;
ST_CHAR *p;
ST_CHAR paramName[200];

  sprintf (paramName, "%s/%s (%s)", 
  	slogMaskGroup->name != NULL ? slogMaskGroup->name: "",
	slogValueMap->tag,
	slogValueMap->preferredTag ? slogValueMap->preferredTag : "");

  logCfgxCtrl = (LOGCFGX_CTRL *)sxDecCtrl->usr;
  switch (slogValueMap->dataType)
    {
    case _LOGCFG_DATATYPE_UINT_MASK:
      {
      ST_UINT *up;
      up = (ST_UINT *)slogValueMap->addr;
      if (sx_get_string_OnOff_bool (sxDecCtrl, &b, SX_ERR_CONVERT) != SD_SUCCESS)
        break;
      if (b)
        *up |= slogValueMap->mask; /* Set the bit */
      else
        *up &= ~slogValueMap->mask; /* Clear the bit */
  
      LOGCFG_CFLOW2 ("Setting log mask %-50.50s  %s", paramName, b ? "ON" : "OFF");

      if (tagVal != NULL)
        tagVal->u.b = b;
      }
    break;

    case _LOGCFG_DATATYPE_UINT32_MASK:
      {
      ST_UINT32 *u32p;
      u32p = (ST_UINT32 *)slogValueMap->addr;
      if (sx_get_string_OnOff_bool (sxDecCtrl, &b, SX_ERR_CONVERT) != SD_SUCCESS)
        break;
      if (b)
        *u32p |= slogValueMap->mask; /* Set the bit */
      else
        *u32p &= ~slogValueMap->mask; /* Clear the bit */
    
      LOGCFG_CFLOW2 ("Setting log mask %-50.50s  %s", paramName, b ? "ON" : "OFF");
      if (tagVal != NULL)
        tagVal->u.b = b;
      }
    break;
    
    /* 'R'everse mask settings */
    case _LOGCFG_DATATYPE_RUINT32_MASK:
      {
      ST_UINT *up;
      up = (ST_UINT *)slogValueMap->addr;
      if (sx_get_string_OnOff_bool (sxDecCtrl, &b, SX_ERR_CONVERT) != SD_SUCCESS)
        break;
      if (b)
        *up &= ~slogValueMap->mask; /* Clear the bit */
      else
        *up |= slogValueMap->mask; /* Set the bit */
    
      LOGCFG_CFLOW2 ("Setting log mask %-50.50s  %s", paramName, b ? "ON" : "OFF");
      if (tagVal != NULL)
        tagVal->u.b = b;
      }
    break;
    
    case _LOGCFG_DATATYPE_BOOLEAN:
      {
      ST_BOOLEAN *bp;
      bp = (ST_BOOLEAN *) slogValueMap->addr;
      if (sx_get_string_OnOff_bool (sxDecCtrl, bp, SX_ERR_CONVERT) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting boolean  %-50.50s  %s", paramName, *bp ? "ON" : "OFF");
      if (tagVal != NULL)
        tagVal->u.b = *bp;
      }
    break;
    
    case _LOGCFG_DATATYPE_INT:
      {
      ST_INT *sp;
      sp = (ST_INT *)slogValueMap->addr;
      if (sx_get_int (sxDecCtrl, sp) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %d", paramName, *sp);
      if (tagVal != NULL)
        tagVal->u.sInt = *sp;
      }
    break;
    
    case _LOGCFG_DATATYPE_UINT:
      {
      ST_UINT *up;
      up = (ST_UINT *)slogValueMap->addr;
      if (sx_get_uint (sxDecCtrl, up) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %u", paramName, *up);
      if (tagVal != NULL)
        tagVal->u.uInt = *up;
      }
    break;
    
    case _LOGCFG_DATATYPE_LONG:
      {
      ST_LONG *slp;
      slp = (ST_LONG *)slogValueMap->addr;
      if (sx_get_long (sxDecCtrl, slp) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %ld", paramName, *slp);
      if (tagVal != NULL)
        tagVal->u.sLong = *slp;
      }
    break;
    
    case _LOGCFG_DATATYPE_ULONG:
      {
      ST_ULONG *ulp;
      ulp = (ST_ULONG *)slogValueMap->addr;
      if (sx_get_ulong (sxDecCtrl, ulp) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %lu", paramName, *ulp);
      if (tagVal != NULL)
        tagVal->u.uLong = *ulp;
      }
    break;
    
    case _LOGCFG_DATATYPE_INT16:
      {
      ST_INT16 *s16p;
      s16p = (ST_INT16 *)slogValueMap->addr;
      if (sx_get_int16 (sxDecCtrl, s16p) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %d", paramName, (ST_INT) *s16p);
      if (tagVal != NULL)
        tagVal->u.sInt16 = *s16p;
      }
    break;
    
    case _LOGCFG_DATATYPE_UINT16:
      {
      ST_UINT16 *u16p;
      u16p = (ST_UINT16 *)slogValueMap->addr;
      if (sx_get_uint16 (sxDecCtrl, u16p) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %u", paramName, (ST_UINT) *u16p);
      if (tagVal != NULL)
        tagVal->u.uInt16 = *u16p;
      }
    break;
    
    case _LOGCFG_DATATYPE_INT32:
      {
      ST_INT32 *s32p;
      s32p = (ST_INT32 *)slogValueMap->addr;
      if (sx_get_int32 (sxDecCtrl, s32p) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %ld", paramName, (ST_LONG) *s32p);
      if (tagVal != NULL)
        tagVal->u.sInt32 = *s32p;
      }
    break;
    
    case _LOGCFG_DATATYPE_UINT32:
      {
      ST_UINT32 *u32p;
      u32p = (ST_UINT32 *)slogValueMap->addr;
      if (sx_get_uint32 (sxDecCtrl, u32p) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %lu", paramName, (ST_ULONG) *u32p);
      if (tagVal != NULL)
        tagVal->u.uInt32 = *u32p;
      }
    break;
    
    case _LOGCFG_DATATYPE_DOUBLE:
      {
      ST_DOUBLE *dp;
      dp = (ST_DOUBLE *)slogValueMap->addr;
      if (sx_get_double (sxDecCtrl, dp) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  %.4f", paramName, *dp);
      if (tagVal != NULL)
        tagVal->u.dbl = *dp;
      }
    break;
    
    case _LOGCFG_DATATYPE_STRING:
      {
      ST_CHAR **cpp;
      cpp = (ST_CHAR **)slogValueMap->addr;
      if (sx_get_alloc_string (sxDecCtrl, cpp) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  '%s'", paramName, *cpp);
      if (tagVal != NULL)
        tagVal->u.str = *cpp;
      }
    break;
    
    case _LOGCFG_DATATYPE_STRINGBUF:
      {
      ST_CHAR *cp;
      cp = (ST_CHAR *)slogValueMap->addr;
      strLen = 0;
      if (sx_get_string_ptr (sxDecCtrl, &str, &strLen) != SD_SUCCESS)
        break;
      LOGCFG_CFLOW2 ("Setting          %-50.50s  '%s'", paramName, str);
      cp = (ST_CHAR *)slogValueMap->addr;
      strncpy_safe (cp, str, slogValueMap->mask);
    
      if (tagVal != NULL)
        tagVal->u.str = cp;
      }
    break;
    
    case _LOGCFG_DATATYPE_FILENAME:
      {
      ST_CHAR **cpp;
      cpp = (ST_CHAR **)slogValueMap->addr;
    
    /* The user might want a prefix for this file name */
      strLen = 0;
      if (sx_get_string_ptr (sxDecCtrl, &str, &strLen) != SD_SUCCESS)
        break;
      if (logCfgxCtrl->fileNamePrefix)
        strLen += (int) strlen (logCfgxCtrl->fileNamePrefix);
      p = (char*) chk_malloc (strLen+1);
      *cpp = p;
      if (logCfgxCtrl->fileNamePrefix)
        strcpy (p, logCfgxCtrl->fileNamePrefix);
      else
        *p = 0;
      strcat (p, str);

      LOGCFG_CFLOW2 ("Setting          %-50.50s  '%s'", paramName, p);
      if (tagVal != NULL)
        tagVal->u.str = p;
      }
    break;

    case _LOGCFG_DATATYPE_CALLBACK:
      logCfgCallbackFun = (logCfgCallbackFunTYPE) slogValueMap->addr;
      if (logCfgCallbackFun != NULL)
        (*logCfgCallbackFun) (sxDecCtrl, sxDecCtrl->sxDecElInfo.tag, slogValueMap);
    break;
    }
  }

/************************************************************************/
/*			slog_ipc_std_cmd_service			*/
/*			slog_ipc_std_cmd_service_ex			*/
/************************************************************************/

ST_INT slog_ipc_std_cmd_service (ST_CHAR *logCfgFile, 
				  ST_CHAR *fileNamePrefix,
                                  ST_CHAR *cName,
                                  ST_BOOLEAN callingEnable,
				  ST_RET (*logReloadHandler) (ST_VOID),
				  ST_RET (*extendedCmdHandler) (SLOGIPC_CMD *slogIpcCmd))
  {
ST_INT retVal;

  retVal = slog_ipc_std_cmd_service_ex (sLogCtrl, logCfgFile, fileNamePrefix,
                                  cName, callingEnable,logReloadHandler, extendedCmdHandler);
  return retVal;
  }


ST_INT slog_ipc_std_cmd_service_ex (LOG_CTRL *lc, ST_CHAR *logCfgFile, 
				  ST_CHAR *fileNamePrefix,
                                  ST_CHAR *cName,
                                  ST_BOOLEAN callingEnable,
				  ST_RET (*logReloadHandler) (ST_VOID),
				  ST_RET (*extendedCmdHandler) (SLOGIPC_CMD *slogIpcCmd))
  {
SLOGIPC_CMD *slogIpcCmd;
SLOGIPC_RX_IND *slogIpcRxInd;
ST_ULONG msgRespType;
ST_LONG fileSize;
ST_LONG bytesRead;
ST_LONG bytesWritten;
ST_CHAR *cfgData;
ST_RET  rc;
struct stat buf;
int result;
FILE *fp;
ST_INT xmlSize;
ST_CHAR *xmlBuf;
ST_INT retVal;

  slogIpcEventEx (lc);	/* required for IPC Logging if gensock2.c is	*/
    			/* not compiled with GENSOCK_THREAD_SUPPORT	*/

  slogIpcCallingEnableEx (lc, callingEnable);
  slogIpcHeartbeatService(lc);

/* Any pending SLOG IPC Commands ? */
  if (slogIpcCmdList == NULL)
    return SLOGIPC_CMD_NONE_OK;

  S_LOCK_UTIL_RESOURCES ();
  slogIpcCmd = slogIpcCmdList;
  list_unlink (&slogIpcCmdList, slogIpcCmd);
  S_UNLOCK_UTIL_RESOURCES ();

  slogIpcRxInd = &slogIpcCmd->slogIpcRxInd;

/* Assume the worst ... */
  msgRespType = SLOGIPC_MSG_TYPE_RESP_FAILURE;
  retVal = SLOGIPC_CMD_UNKNOWN_FAILED;

  switch (slogIpcRxInd->msgType)
    {
    case SLOGIPC_MSG_TYPE_READ_LOGCFG:
      retVal = SLOGIPC_CMD_READ_LOGCFG_FAILED;

    /* Get the size of the file */
      bytesRead = 0;
      cfgData = NULL;
      fp = fopen (logCfgFile, "r");
      if (fp != NULL)
        {
        result = fstat (fileno (fp), &buf);
        if (result == 0)
          {  
        /* Allocate a buffer and read all into memory */
          fileSize = buf.st_size;
          cfgData = (ST_CHAR *) chk_malloc (fileSize);
          bytesRead = (long) fread (cfgData, 1, fileSize, fp);
          if (bytesRead > 0)
	    {
            msgRespType = SLOGIPC_MSG_TYPE_RESP_SUCCESS;
            retVal = SLOGIPC_CMD_READ_LOGCFG_OK;
	    }
          }
        fclose (fp);
        }
      slogIpcSendData (slogIpcRxInd->pSock, msgRespType, bytesRead, (ST_UCHAR *)cfgData);
      if (cfgData != NULL)
        chk_free (cfgData);
    break;

    case SLOGIPC_MSG_TYPE_WRITE_LOGCFG:
      retVal = SLOGIPC_CMD_WRITE_LOGCFG_FAILED;
      fp = fopen (logCfgFile, "w");
      if (fp != NULL)
        {
        bytesWritten = (long) fwrite (slogIpcRxInd->msgData, 1, slogIpcRxInd->msgDataLen, fp);
        fclose (fp);
        if (bytesWritten == slogIpcRxInd->msgDataLen)
          {  
          if (logReloadHandler != NULL)
	    rc = (*logReloadHandler)();
          else
            rc = logcfgx_exx (lc, logCfgFile, fileNamePrefix, SD_TRUE, SD_TRUE, cName);

          if (rc == SD_SUCCESS)
	    {
            retVal = SLOGIPC_CMD_WRITE_LOGCFG_OK;
            msgRespType = SLOGIPC_MSG_TYPE_RESP_SUCCESS;
	    }
	  }
        }
      slogIpcSendData (slogIpcRxInd->pSock, msgRespType, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_CLONE_LOG:
      retVal = SLOGIPC_CMD_CLONE_OK;
      slogIpcRxInd->msgData[slogIpcRxInd->msgDataLen] = 0;
      slogCloneFile (lc, slogIpcRxInd->msgData);
      slogIpcSendData (slogIpcRxInd->pSock, SLOGIPC_MSG_TYPE_RESP_SUCCESS, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_RESET_LOG:
      retVal = SLOGIPC_CMD_RESET_OK;
      slogDeleteFile (lc);
      slogIpcSendData (slogIpcRxInd->pSock, SLOGIPC_MSG_TYPE_RESP_SUCCESS, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_LOG_MEM:
      retVal = SLOGIPC_CMD_WRITE_MSGDATA_OK;
      SLOGALWAYS0 ("MEMORY LOG COMMAND");
      dyn_mem_ptr_statistics (SD_FALSE);
      sLogHeap ();
      slogIpcSendData (slogIpcRxInd->pSock, SLOGIPC_MSG_TYPE_RESP_SUCCESS, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_WRITE_MSGDATA:
      retVal = SLOGIPC_CMD_WRITE_MSGDATA_OK;
      SLOGALWAYS0 (slogIpcRxInd->msgData);
      slogIpcSendData (slogIpcRxInd->pSock, SLOGIPC_MSG_TYPE_RESP_SUCCESS, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_GET_LOGMASKS:
      retVal = SLOGIPC_CMD_GET_LOGMASKS_FAILED;
      xmlSize = _logcfg_get_logmasks_size ();
      xmlSize += SX_ENC_BUFFER_OH;
      xmlBuf = (ST_CHAR *)chk_malloc (xmlSize);

      rc = _logcfg_get_logmasks (xmlBuf, &xmlSize);
      if (rc == SD_SUCCESS)
        {
        retVal = SLOGIPC_CMD_GET_LOGMASKS_OK;
        msgRespType = SLOGIPC_MSG_TYPE_RESP_SUCCESS;
	}
      else
        xmlSize = 0;
      slogIpcSendData (slogIpcRxInd->pSock, msgRespType, xmlSize, (ST_UCHAR *) xmlBuf);
      chk_free (xmlBuf);
    break;

#ifdef SEV_SUPPORT
    case SLOGIPC_MSG_TYPE_SEV_LOG_STATE:
      retVal = SLOGIPC_CMD_SEV_LOG_FAILED;
      rc = sevLogState ();
      if (rc == SD_SUCCESS)
        {
        retVal = SLOGIPC_CMD_SEV_LOG_OK;
        msgRespType = SLOGIPC_MSG_TYPE_RESP_SUCCESS;
        }
      slogIpcSendData (slogIpcRxInd->pSock, msgRespType, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_SEV_TRACE_PAUSE:
      retVal = SLOGIPC_CMD_SEV_PAUSE_OK;
      sevSetPause (SD_TRUE);
      slogIpcSendData (slogIpcRxInd->pSock, SLOGIPC_MSG_TYPE_RESP_SUCCESS, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_SEV_TRACE_RESUME:
      retVal = SLOGIPC_CMD_SEV_RESUME_OK;
      sevSetPause (SD_FALSE);
      slogIpcSendData (slogIpcRxInd->pSock, SLOGIPC_MSG_TYPE_RESP_SUCCESS, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_SEV_STOP:
      retVal = SLOGIPC_CMD_SEV_STOP_FAILED;
      rc = sevStop (SD_FALSE);
      if (rc == SD_SUCCESS)
        {
        retVal = SLOGIPC_CMD_SEV_STOP_OK;
	msgRespType = SLOGIPC_MSG_TYPE_RESP_SUCCESS;
	}
      slogIpcSendData (slogIpcRxInd->pSock, msgRespType, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_SEV_CLONE:
      retVal = SLOGIPC_CMD_SEV_CLONE_FAILED;
      rc = sevCloneTrace ();
      if (rc == SD_SUCCESS)
        {
        retVal = SLOGIPC_CMD_SEV_CLONE_OK;
	msgRespType = SLOGIPC_MSG_TYPE_RESP_SUCCESS;
	}
      slogIpcSendData (slogIpcRxInd->pSock, msgRespType, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_SEV_LOG_STATE_AND_STOP:
      retVal = SLOGIPC_CMD_SEV_LOG_AND_STOP_FAILED;
      rc = sevLogStateStop (SD_FALSE);
      if (rc == SD_SUCCESS)
        {
        retVal = SLOGIPC_CMD_SEV_LOG_AND_STOP_OK;
	msgRespType = SLOGIPC_MSG_TYPE_RESP_SUCCESS;
	}
      slogIpcSendData (slogIpcRxInd->pSock, msgRespType, 0, NULL);
    break;

    case SLOGIPC_MSG_TYPE_PROCESS_DUMP:
      retVal = SLOGIPC_CMD_PROCESS_DUMP_OK;
      msgRespType = SLOGIPC_MSG_TYPE_RESP_SUCCESS;

      if (slogIpcRxInd->msgDataLen)
        ExpGenerateProcessDump (slogIpcRxInd->msgData);
      else
        ExpGenerateProcessDump (NULL);

      slogIpcSendData (slogIpcRxInd->pSock, msgRespType, 0, NULL);
    break;

#endif

    default:
      if (extendedCmdHandler != NULL)
        retVal = (*extendedCmdHandler) (slogIpcCmd);
      else
        {
        if ((slogIpcRxInd->msgType != SLOGIPC_MSG_TYPE_RESP_SUCCESS) && 
	    (slogIpcRxInd->msgType != SLOGIPC_MSG_TYPE_RESP_FAILURE))
	  {
          slogIpcSendData (slogIpcRxInd->pSock, msgRespType, 0, NULL);
	  }
        }
    break;
    }

/* Normally would use 'slogIpc_uDataBufFree' for sockData, but it is static ... */
  chk_free (slogIpcRxInd->sockData);
  chk_free (slogIpcCmd);
  return retVal;
  }

/************************************************************************/
/*			logcfgx_unconfigure				*/
/* Free all buffers allocated by logcfgx.				*/
/************************************************************************/
ST_VOID logcfgx_unconfigure (LOG_CTRL *lc) 
  {
  S_LOCK_UTIL_RESOURCES ();

  /* Disable file, mem, ipc logging first so we won't use buffers freed below.*/
  lc->logCtrl &= ~(LOG_FILE_EN | LOG_MEM_EN | LOG_IPC_EN);

  /* If log file is open, close it now.	*/
  if (lc->fc.state & FIL_STATE_OPEN)
    {                 
    fclose (lc->fc.fp);
    lc->fc.state &= ~(FIL_STATE_OPEN);    /* clear state  */
    }

  if (lc->fc.fileName)
    {
    chk_free (lc->fc.fileName);
    lc->fc.fileName = NULL;	/* prevent access to freed mem	*/
    }
  if (lc->mc.dumpFileName)
    {
    chk_free (lc->mc.dumpFileName);
    lc->mc.dumpFileName = NULL;	/* prevent access to freed mem	*/
    }
  if (lc->ipc.callingIp)
    {
    chk_free (lc->ipc.callingIp);
    lc->ipc.callingIp = NULL;	/* prevent access to freed mem	*/
    }
  if (lc->ipc.appId)
    {
    chk_free (lc->ipc.appId);
    lc->ipc.appId = NULL;	/* prevent access to freed mem	*/
    }

  S_UNLOCK_UTIL_RESOURCES ();
  }
#endif	/* defined(DEBUG_SISCO)	*/




