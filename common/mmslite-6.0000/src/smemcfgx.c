/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	2000 - 2001, All Rights Reserved		        */
/*									*/
/* MODULE NAME : smemcfgx.c						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : This routine parses the smemcfg.xml             */
/*                      configuration file for Memory Management        */
/*                      context configurations.                         */
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 06/09/10  JRB	   Fix log messages.				*/
/* 01/30/06  GLB    13     Integrated porting changes for VMS           */
/* 08/13/04  JRB    12     Use thread-safe sx_parseExx_mt.		*/
/*			   Del unneeded system includes.		*/
/* 04/31/04  MDE    11     Now use sx_parseExx				*/
/* 04/14/03  JRB    10     Eliminate compiler warnings.			*/
/* 02/28/02  GLB    09     Changed ascii_t_.. to asciiTo..              */
/* 10/12/01  JRB    08     Chg AutoClone choices from Yes/No to Y/N.	*/
/* 10/09/01  JRB    07     Del _getFileSize (never used).		*/
/* 10/02/01  JRB    06     Call u_smem_get_pool_params if bufSize==0 OR	*/
/*			   numBuf==0 (was using AND).			*/
/* 06/25/01  JRB    05     Allow config at any time: if context already	*/
/*			   contains pool, just log warning.		*/
/*			   Del smemInitialized (not needed).		*/
/*			   Chg smem_add_pool calls to m_add_pool (adds	*/
/*			   overhead to buffers).			*/
/*			   Add smemSetAllCtxtNames.			*/
/*			   Del smem_find_ctxt_name (not needed when	*/
/*			   smemSetAllCtxtNames used).			*/
/*			   Chk for poolName==NULL (i.e. not configured)	*/
/*			   Clear temp_pool_info on SX_ELEMENT_START.	*/
/*			   Fix conversion of bufSize, highLimit, and	*/
/*			   lowLimit (ST_UINT) & usrId (ST_INT).		*/
/* 06/25/01  GLB    04     rearranged order of include for smem.h	*/
/* 06/24/01  GLB    03     Chg to use sx_parseEx			*/
/* 06/24/01  GLB    02     M_SMEM_MAX_CONTEXT-1 to M_SMEM_MAX_CONTEXT	*/
/* 01/18/01  GLB    01     Module created				*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#define  SMEMCFGX   /* use list of valid contexts in 'smem.h' */
#include "mem_chk.h"
#include "str_util.h"
#include "sx_defs.h"
#include "sx_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*  subroutines used during the parse                                   */
/************************************************************************/

static ST_VOID _smemCfg_SEFun (SX_DEC_CTRL *sxDecCtrl);


static ST_VOID _smemContext_SERFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _contextName_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _newContextIndex_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _userId_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _smemPools_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _smemRangeTracking_SEFun (SX_DEC_CTRL *sxDecCtrl);


static ST_VOID _pool_SERFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _poolName_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _bufferSize_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _numBuffers_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _autoClone_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _maxClones_EFun (SX_DEC_CTRL *sxDecCtrl);


static ST_VOID _range_SERFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _highLimit_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _lowLimit_EFun (SX_DEC_CTRL *sxDecCtrl);


/************************************************************************/
static ST_VOID smemSetAllCtxtNames (ST_VOID);
static ST_VOID printContextEntries (ST_VOID);
static ST_INT   smem_find_ctxt_idx (ST_CHAR *contextName);


/************************************************************************/
/*  structures used to store data during parse   			*/
/************************************************************************/
typedef struct
  {
  ST_CHAR *poolName;
  ST_UINT bufSize;
  ST_UINT8 numBuf;
  ST_BOOLEAN autoClone;
  ST_INT maxClones;
  } SMEM_POOL_CFG;
static SMEM_POOL_CFG temp_pool_info;

static SMEM_RT_CTRL *smemRangeTrack;

/************************************************************************/
/*  variables used during the parse                                     */
/************************************************************************/

static ST_LONG contextIdx;
static ST_LONG numRangesCnt;

static ST_INT noneSpecified = -1;

/************************************************************************/
/* elements referenced during the parse                                 */
/************************************************************************/

SX_ELEMENT smemCfgStartElements[] = 
{
  {"SmemCfg", SX_ELF_CSTART | SX_ELF_CEND, _smemCfg_SEFun}
};
  
SX_ELEMENT smemCfgElements[] = 
{
  {"SmemContext",    		    SX_ELF_CSTART | SX_ELF_CEND | SX_ELF_RPT,  _smemContext_SERFun}
};

SX_ELEMENT smemContextElements[] = 
{
  {"ContextName",    		    SX_ELF_CEND, 	_contextName_EFun},
  {"ContextIndex",    		    SX_ELF_CEND, 	_newContextIndex_EFun},
  {"UserId",    		    SX_ELF_CEND, 	_userId_EFun},
  {"SmemPools",    		    SX_ELF_CSTART, 	_smemPools_SFun},
  {"SmemRangeTracking",    	    SX_ELF_CSTART | SX_ELF_CEND, 	_smemRangeTracking_SEFun}
};

SX_ELEMENT smemPoolsElements[] = 
{
  {"Pool",    		             SX_ELF_CSTART | SX_ELF_CEND | SX_ELF_RPT, 	_pool_SERFun}
};

SX_ELEMENT poolElements[] = 
{
  {"PoolName",    		    SX_ELF_CEND|SX_ELF_OPT, _poolName_EFun},
  {"BufferSize",    		    SX_ELF_CEND, 	_bufferSize_EFun},
  {"NumBuffers",    		    SX_ELF_CEND, 	_numBuffers_EFun},
  {"AutoClone",    		    SX_ELF_CEND, 	_autoClone_EFun},
  {"MaxClones",    		    SX_ELF_CEND, 	_maxClones_EFun}
};

SX_ELEMENT smemRangeTrackElements[] = 
{
  {"Range",    		            SX_ELF_CSTART | SX_ELF_CEND | SX_ELF_RPT, 	_range_SERFun}
};

SX_ELEMENT rangeElements[] = 
{
  {"HighLimit",    		    SX_ELF_CEND, 	_highLimit_EFun},
  {"LowLimit",    		    SX_ELF_CEND, 	_lowLimit_EFun}
};


/************************************************************************/
/************************************************************************/
/*  subroutines used during the parse                                   */
/************************************************************************/


/************************************************************************/
/*			smemcfgx				        */
/************************************************************************/

ST_RET smemcfgx (ST_CHAR *logFileName)
  {
ST_RET rc;
ST_UINT sx_debug_sel_save;

  smemSetAllCtxtNames ();	/* Set all contextName in m_smem_ctxt array*/

  rc = sx_parseExx_mt (logFileName, 
	   	  sizeof (smemCfgStartElements)/sizeof(SX_ELEMENT), smemCfgStartElements,
		  NULL, NULL, NULL);

  if (rc != SD_SUCCESS)
    {
    /* Config failed:  Turn on cfg logging and parse file again.	*/
    sx_debug_sel_save = sx_debug_sel;

    sx_debug_sel |= SX_LOG_DEC | SX_LOG_ENC | SX_LOG_FLOW | SX_LOG_DEBUG;
    SXLOG_ERR1 ("Error parsing '%s'. Begin reparsing with all logging enabled.",
                 logFileName);

    rc = sx_parseExx_mt (logFileName, 
	   	  sizeof (smemCfgStartElements)/sizeof(SX_ELEMENT), smemCfgStartElements,
		  NULL, NULL, NULL);
    sx_debug_sel = sx_debug_sel_save;
    }

  return (rc);
  }

/************************************************************************/
/*			_smemCfg_SEFun 					*/
/************************************************************************/

static ST_VOID _smemCfg_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SXLOG_CFLOW0 ("SX DEC: _smemCfg_SEFun");
  SX_PUSH (smemCfgElements);

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Do nothing.	*/
    }

  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    /* output values configured in parsing smemcfg.xml */
    printContextEntries (); 
    }
  }

/************************************************************************/
/*			_smemContext_SERFun 				                                    */
/************************************************************************/

static ST_VOID _smemContext_SERFun (SX_DEC_CTRL *sxDecCtrl)
  {


  SXLOG_CFLOW0 ("SX DEC: _smemContext_SERFun");
  SX_PUSH (smemContextElements);

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* set flag to indicate no context index or   */
    /* context name has been entered for this     */
    /* context so far                             */
    if (contextIdx > M_SMEM_MAX_CONTEXT-1)
      {
      SXLOG_ERR2 ("ERROR: %ld contexts exceeds M_SMEM_MAX_CONTEXT of %d: ",
                  contextIdx,M_SMEM_MAX_CONTEXT);
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    contextIdx = noneSpecified; 
    }
    
  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    }

  }

/************************************************************************/
/*		     _newContextIndex_EFun 					                        */
/************************************************************************/

static ST_VOID _newContextIndex_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
ST_INT validContext;
ST_UINT16 uShortInt;

  SXLOG_CFLOW0 ("SX DEC: _newContextIndex_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;			     

  if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
    {
    SXLOG_ERR0 ("ERROR: NewContextIndex Data Conversion Error");
    sxDecCtrl->errCode = SX_ERR_CONVERT;
    }
  else
    { /* valid short integer value */
    if (uShortInt > M_SMEM_MAX_CONTEXT-1)        
      {	
      SXLOG_ERR1 ("ERROR: Invalid Context Array subscript: %s",str);
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else /* valid context array subscript */
      {
      if (m_smem_ctxt[uShortInt].smemPoolCtrlList != NULL)  /* context already in use? */
        {
        SXLOG_ERR1 ("WARNING: New Context being configured already contains pools. Context Index: %d",uShortInt);
        }
      validContext = SD_TRUE;                     
      contextIdx = uShortInt;
      SXLOG_CFLOW1 ("NEWContextIndex: %d",uShortInt);
      }
    }

  }
  
/************************************************************************/
/*		     _contextName_EFun 					                        */
/************************************************************************/

static ST_VOID _contextName_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  SXLOG_CFLOW0 ("SX DEC: _contextName_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;			     

  if (contextIdx == noneSpecified) 
    {
    contextIdx = smem_find_ctxt_idx (str);
    if (contextIdx < 0)
      {
      SXLOG_ERR1 ("ERROR: Context Name: %s not found in smemCtxtNames array",str);
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      return;
      }

    /* found context name, it is valid               */
    /* now see if this context has already been used */
    if (m_smem_ctxt[contextIdx].smemPoolCtrlList != NULL)  /* context already in use? */
      {
      SXLOG_ERR1 ("WARNING: New Context being configured already contains pools. Context Name: %s",str);
      }
    } /* if (contextIdx == noneSpecified) */
  else
    {
    SXLOG_ERR0 ("ERROR: Configured both index and name for specified context");
    sxDecCtrl->errCode = SX_ERR_CONVERT;
    }
  }
  
/************************************************************************/
/*			_userId_EFun					*/
/* usrId is ST_INT, so use sscanf with "%d".				*/
/************************************************************************/

static ST_VOID _userId_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  SXLOG_CFLOW0 ("SX DEC: _userId_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;			     

  if (sscanf (str, "%d", &m_smem_ctxt[contextIdx].usrId) != 1)
    {
    SXLOG_ERR0 ("ERROR: UserId Data Conversion Error");
    sxDecCtrl->errCode = SX_ERR_CONVERT;
    }
  }
  
/************************************************************************/
/************************************************************************/
/*			_smemPools_SFun 				                                    */
/************************************************************************/

static ST_VOID _smemPools_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SXLOG_CFLOW0 ("SX DEC: _smemPools_SFun");
  SX_PUSH (smemPoolsElements);
  }

/************************************************************************/
/*			_pool_SERFun 				                                    */
/************************************************************************/

static ST_VOID _pool_SERFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SMEM_POOL_CTRL *smem_pool_ctrl;

  SXLOG_CFLOW0 ("SX DEC: _pool_SERFun");
  SX_PUSH (poolElements);

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* reinitialize temporary storage structure for pool info    */
    memset (&temp_pool_info, 0, sizeof (temp_pool_info));    
    }

  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    if (temp_pool_info.bufSize == 0  ||  temp_pool_info.numBuf == 0)   
      {
      /* if buffer size and number of buffers is not entered in    */
      /* the configuration file for a specified context name and   */
      /* pool name then                                            */
      /* buffer size and number of buffers needed to create a pool */
      /* is calculated from existing data and a pool is created    */
      /* from those calculated sizes                               */ 

      u_smem_get_pool_params ( &m_smem_ctxt[contextIdx],
                               &temp_pool_info.numBuf, &temp_pool_info.bufSize,
                               &temp_pool_info.autoClone,
                               &temp_pool_info.maxClones,
                               &temp_pool_info.poolName); 

      /* allocate pool with parameters calculated from existing info */
      smem_pool_ctrl = m_add_pool (&m_smem_ctxt[contextIdx],
                         temp_pool_info.numBuf,temp_pool_info.bufSize,
                         temp_pool_info.autoClone,temp_pool_info.maxClones,
                         temp_pool_info.poolName); 
      if (smem_pool_ctrl == NULL)
        {
        SXLOG_ERR0 ("ERROR: m_add_pool failed");
        sxDecCtrl->errCode = SX_ERR_CONVERT;
 	}
      }
    else
      {
      /* allocate pool with parameters input from configuration file */
      smem_pool_ctrl = m_add_pool (&m_smem_ctxt[contextIdx],
                         temp_pool_info.numBuf,temp_pool_info.bufSize,
                         temp_pool_info.autoClone,temp_pool_info.maxClones,
                         temp_pool_info.poolName); 
      if (smem_pool_ctrl == NULL)
        {
        SXLOG_ERR0 ("ERROR: m_add_pool failed");
        sxDecCtrl->errCode = SX_ERR_CONVERT;
 	}
      }
    }
  }

/************************************************************************/
/*			_poolName_EFun 				                    */
/************************************************************************/

static ST_VOID _poolName_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  SXLOG_CFLOW0 ("SX DEC: _poolName_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  temp_pool_info.poolName = (ST_CHAR *) calloc (1, strlen (str) + 1);
  strcpy(temp_pool_info.poolName,str);

  }

/************************************************************************/
/*			_bufferSize_EFun 				                                */
/************************************************************************/

static ST_VOID _bufferSize_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  SXLOG_CFLOW0 ("SX DEC: _bufferSize_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (sscanf (str, "%u", &temp_pool_info.bufSize) != 1)
    {
    SXLOG_ERR0 ("ERROR: Buffer Size Data Conversion Error");
    sxDecCtrl->errCode = SX_ERR_CONVERT;
    }

  }

/************************************************************************/
/*			_numBuffers_EFun 				                                */
/************************************************************************/

static ST_VOID _numBuffers_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
ST_UINT8 uInt8;

  SXLOG_CFLOW0 ("SX DEC: _numBuffers_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

    if (asciiToUint8(str,&uInt8) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: NumBuffers Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      temp_pool_info.numBuf = uInt8;
      }

  }

/************************************************************************/
/*			_autoClone_EFun 				*/
/* When a memory pool of specified size is full and no more pools of    */
/* the specified size exist ... and cloning is allowed ...              */
/* then a new pool of similar size may be created.                      */
/* When this option is not specified no cloning is assumed.             */
/************************************************************************/

static ST_VOID _autoClone_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  SXLOG_CFLOW0 ("SX DEC: _autoClone_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (str[0] == 'y'  ||  str[0] == 'Y')
    {
    temp_pool_info.autoClone = SD_TRUE;
    }
  else if (str[0] == 'n'  ||  str[0] == 'N')
    {
    temp_pool_info.autoClone = SD_FALSE;
    }
  else
    {
    SXLOG_ERR0 ("ERROR: AutoClone Data Conversion Error");
    sxDecCtrl->errCode = SX_ERR_CONVERT;
    }

  }

/************************************************************************/
/*			_maxClones_EFun 				*/
/* This is optional.  If ommitted the max number of clones allowed for  */
/* a memory pool is set to zero, indicating an infinite amount of       */
/* cloning is allowed	                                                */
/************************************************************************/

static ST_VOID _maxClones_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
ST_UINT16 uShortInt;

  SXLOG_CFLOW0 ("SX DEC: _maxClones_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: MaxClones Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      temp_pool_info.maxClones = uShortInt;  
      }

  }

/************************************************************************/
/************************************************************************/
/*			_smemRangeTracking_SEFun 			*/	                    
/*  This is optional for each context.  Configuration of these values   */
/*  allows analysis of the max number of buffers actually assigned at   */
/*  one time to each range of memory, so that more efficient assignment */
/*  of memory blocks can be made.  Note: ranges may overlap             */    
/************************************************************************/

static ST_VOID _smemRangeTracking_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SXLOG_CFLOW0 ("SX DEC: _smemRAngeTracking_SEFun");
  SX_PUSH (smemRangeTrackElements);

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    smemRangeTrack = (SMEM_RT_CTRL *) calloc (1, sizeof (SMEM_RT_CTRL));
    m_smem_ctxt[contextIdx].smemRangeTrack = smemRangeTrack;

    numRangesCnt = 0;  
    }

  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    smemRangeTrack->numRanges = numRangesCnt;
    }

  }

/************************************************************************/
/*			_range_SERFun 				                                    */
/************************************************************************/

static ST_VOID _range_SERFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SXLOG_CFLOW0 ("SX DEC: _range_SERFun");
  SX_PUSH (rangeElements);
  
  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    numRangesCnt++;  

    if (numRangesCnt > SMEM_MAX_RANGE_TRACK)
      {
      SXLOG_ERR0 ("ERROR: Number of Range Values entered exceeds SMEM_MAX_RANGE_TRACK");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    }

  }  

/************************************************************************/
/*			_highLimit_EFun 				                                */
/************************************************************************/

static ST_VOID _highLimit_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  SXLOG_CFLOW0 ("SX DEC: _highLimit_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (sscanf (str, "%u", &smemRangeTrack->rt[numRangesCnt].highLimit) != 1)
    {
    SXLOG_ERR0 ("ERROR: High Limit Data Conversion Error");
    sxDecCtrl->errCode = SX_ERR_CONVERT;
    }
  }

/************************************************************************/
/*			_lowLimit_EFun 				        */
/************************************************************************/

static ST_VOID _lowLimit_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  SXLOG_CFLOW0 ("SX DEC: _lowLimit_EFun");
  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (sscanf (str, "%u", &smemRangeTrack->rt[numRangesCnt].lowLimit) != 1)
    {
    SXLOG_ERR0 ("ERROR: Low Limit Data Conversion Error");
    sxDecCtrl->errCode = SX_ERR_CONVERT;
    }
  }

/************************************************************************/
/*			smem_find_ctxt_idx 					*/
/************************************************************************/

static ST_INT   smem_find_ctxt_idx (ST_CHAR *contextName)
  {
ST_INT i;

  for (i = 0; i < smem_num_ctxt_names; ++i)
    {
    if (!strcmp (smemCtxtNames[i].name, contextName))
      return (smemCtxtNames[i].idx);
    }
  return (-1);
  }

/************************************************************************/
/*			smemSetAllCtxtNames				*/
/* Set the contextName in every element of m_smem_ctxt array.		*/
/************************************************************************/

static ST_VOID smemSetAllCtxtNames (ST_VOID)
  {
ST_INT i;

  for (i = 0; i < smem_num_ctxt_names; ++i)
    {
    m_smem_ctxt [smemCtxtNames[i].idx].contextName = smemCtxtNames[i].name;
    }
  }

/************************************************************************/
/*		             printContextInfo				*/
/************************************************************************/

void printContextEntries (ST_VOID)
{
ST_INT i, j;
SMEM_POOL_CTRL *poolInfo;
SMEM_RT_CTRL *rangeTrackInfo;		   

  SXLOG_DEBUG0 ("\n\nSMEMCFGX RESULTS FOR PARSE OF SMEMCFG.XML");

  for (j=0;j<M_SMEM_MAX_CONTEXT;j++)  
      {
      if (m_smem_ctxt[j].smemPoolCtrlList != NULL)
        {
        SXLOG_CDEBUG1  ("\n    CONTEXT Index:   %d",j);
        if (m_smem_ctxt[j].contextName)
          {
          SXLOG_CDEBUG1  ("  CONTEXT Name:    %s",m_smem_ctxt[j].contextName);
          }
        SXLOG_CDEBUG1  ("  UserId:          %d",m_smem_ctxt[j].usrId);
 
        poolInfo = m_smem_ctxt[j].smemPoolCtrlList;
        while (poolInfo != NULL)
          {
          SXLOG_CDEBUG1  ("\n      Pool:  " S_FMT_PTR,poolInfo);
          if (poolInfo->poolName)
            {
            SXLOG_CDEBUG1  ("      PoolName:    %s",poolInfo->poolName);
            }
          SXLOG_CDEBUG1  ("      BufSize:     %u",poolInfo->bufSize);
          SXLOG_CDEBUG1  ("      NumBuf:      %u",poolInfo->numBuf);

          if (poolInfo->autoClone == SD_TRUE)
            {
            SXLOG_CDEBUG0("      AutoClone:   YES");
            }
          else if (poolInfo->autoClone == SD_FALSE)
            {
            SXLOG_CDEBUG0("      AutoClone:   NO");
            }

          SXLOG_CDEBUG1  ("      MaxClones:   %d",poolInfo->maxClones);
          poolInfo = poolInfo->next;
          }
      
        rangeTrackInfo = m_smem_ctxt[j].smemRangeTrack;
        if (m_smem_ctxt[j].smemRangeTrack != NULL)
          {
          SXLOG_CDEBUG0  ("\n      Range Track Limits ");

          SXLOG_CDEBUG1  ("\n        NumberOfRanges: %u",rangeTrackInfo->numRanges);
          for (i=0;i<rangeTrackInfo->numRanges;i++)
            {
            SXLOG_CDEBUG2  ("\n        HighLimit[%d]:   %u",i,rangeTrackInfo->rt[i].highLimit);
            SXLOG_CDEBUG2  ("      LowLimit[%d]:    %u ",i,rangeTrackInfo->rt[i].lowLimit);
            }
	  }

	} /* m_smem_ctxt[j].smemPoolCtrlList != NULL */

      }	/* for */

  } /* printContextEntries */

