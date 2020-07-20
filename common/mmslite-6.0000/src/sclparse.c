/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	2004-2011, All Rights Reserved					*/
/*                                                                      */
/* MODULE NAME : sclparse.c                                             */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION : This routine parses XML files conforming to the */
/*                      SCL object model.			        */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev          Comments                                */
/* --------  ---  ------   -------------------------------------------  */
/* 01/11/12  JRB	   Many Ed 2 changes (see V6.0000 release notes).*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 08/12/11  JRB	   Add scl_parse_scd, etc.			*/
/* 03/25/11  JRB	   Chg rptID in ReportControl to optional for Ed 2*/
/* 02/23/11  JRB           Save "Val" elem w/ sGroup attr to linked list*/
/*			   Allow "Val" to be repeated if sGroup present.*/
/* 12/02/10  JRB	   Chg EnumVal to ptr & add EnumValBuf.		*/
/* 10/11/10  JRB	   Add convert_hex_str_to_ulong and use it.	*/
/* 09/28/10  JRB	   Add missing "if" in _SMV_Address_P_SEFun.	*/
/* 09/17/10  JRB	   Allow $ in IED name, LDevice inst, DataSet name.*/
/* 07/28/10  JRB	   Stop parse if any component name is illegal.	*/
/* 05/27/10  JRB	   Stop parse when optional attr conversion fails.*/
/*			   Improve error logging.			*/
/*			   Del extra scl_stop_parsing calls.		*/
/* 03/17/10  JRB	   In "*_P_SEFun": chk scl_get_attr_ptr return	*/
/*			   AND make "type" attr required.		*/
/*			   Read APPID, VLAN-* as hex (6 places).	*/
/*			   Add "gi" to "TrgOps" & default to "true".	*/
/* 11/06/08  RKR    26     Removed unused variable			*/
/* 10/31/08  JRB    25     Don't use tp0_convert_ip, not always available*/
/* 10/01/08  JRB    24     Parse & save "Address" info.			*/
/* 05/06/08  JRB    23     Chk ReportControl indexed attr (must be true)*/
/* 04/23/08  JRB    22     Parse & save "SettingControl" info (SGCB).	*/
/*			   Save dchg, qchg, dupd values in SCL_DA struct*/
/* 01/18/08  JRB    21     Allow max LN prefix+inst=11. IEC 61850 max=7.*/
/*			   Must also reduce max suffix in mvlu_rt.c to	*/
/*			   avoid overflow.				*/
/* 01/18/08  JRB    20     Chg datSet in GSEControl & LogControl to	*/
/*			   optional to match Schema (see tControl def).	*/
/* 07/10/07  JRB    19     Check length of lnClass, prefix, & inst.	*/
/* 06/12/07  JRB    18     Save VLANID in SCL_GSE & SCL_SMV.		*/
/* 09/08/06  JRB    17     Fix dstlen type in convert_mac.		*/
/* 07/26/06 LWP/JRB 16     Parse & save GOOSE & SMV addressing info from*/
/*			   "Communication" section.			*/
/* 04/19/06  JRB    15     Make datSet in ReportControl "optional".	*/
/* 04/19/06  JRB    14     Parse & save "RptEnabled" info.		*/
/* 04/04/06  JRB    13     Parse & save "SampledValueControl" info.	*/
/* 03/25/06  JRB    12     Change order of operations: call create/add	*/
/*			   functions first, then copy data directly to	*/
/*			   SCL structs (eliminates intermediate copy).	*/
/*			   Repl scl_get_attr (could overflow caller buf)*/
/*			   with scl_get_attr_ptr & scl_get_attr_copy.	*/
/*			   Replace CONTROL_BLOCK with SCL_RCB & SCL_LCB,*/
/*			   Fix _DA_SEFun setting of dchg, qchg, & dupd.	*/
/*			   Fix sx_get_string calls to prevent overflow.	*/
/*			   Fix "*_Val_*" functions.			*/
/*			   Chg desc to ptr (allocated during parse).	*/
/* 07/25/05  JRB    11     Make nameStructure optional, default=IEDName.*/
/* 06/24/05  JRB    10     Call sx_parseExx_mt (faster).		*/
/*			   Chg "ord" from unsigned to signed value.	*/
/* 05/17/05  DWL    09     Use sx_push instead of SX_PUSH, and force	*/
/*			   user code to call sx_pop.  Eliminated some	*/
/*			   unnecessary debug logging.			*/
/* 05/17/05  JRB    08     Avoid allocating struct in scl_parse.	*/
/* 02/15/05  JRB    07     Save iedName in scl_info.			*/
/*			   Parse Header & save in scl_info.		*/
/*			   Fix some log messages.			*/
/* 12/07/04  JRB    06     Del unneeded memsets after callocs.		*/
/* 08/23/04  JRB    05     Ret err if iedName or accessPoint.. not found*/
/* 08/06/04  JRB    04     Del "sclparse.h", use "scl.h".		*/
/* 07/23/04  JRB    03     Del unused static functions.			*/
/* 07/15/04  DWL    02     Added LogControl & GSEControl elements	*/
/*			   Handle LN and LN0 differently.		*/
/* 06/10/04  DWL    01     Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "sx_defs.h"
#include "sx_log.h"
#include "str_util.h"
#include "slog.h"
#include "scl.h"	/* SCL file processing structs & functions	*/
#include "mvl_uca.h"
#include "smpval.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of      */
/* __FILE__ strings.                                                    */

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
static ST_VOID _SCL_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _Header_SFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _Communication_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SubNetwork_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ConnectedAP_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _Address_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _Address_P_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _GSE_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _GSE_Address_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _GSE_Address_P_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SMV_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SMV_Address_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SMV_Address_P_SEFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _IED_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _AccessPoint_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _Server_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _LDevice_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _LN_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DataSet_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _FCDA_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ReportControl_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _LogControl_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _GSEControl_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SettingControl_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _TrgOps_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _OptFlds_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _RptEnabled_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DOI_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SDI_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DAI_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DAI_Val_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DataTypeTemplates_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _LNodeType_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DO_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DOType_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DA_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SDO_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DAType_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _DA_Val_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _BDA_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _BDA_Val_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _EnumType_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _EnumVal_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_RET _scl_unknown_el_start (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag);
static ST_RET _scl_unknown_el_end (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag);
static ST_VOID _SampledValueControl_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SampledValueControl_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SampledValueControl_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _SmvOpts_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _Inputs_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ExtRef_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _GSE_MinTime_SEFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _GSE_MaxTime_SEFun (SX_DEC_CTRL *sxDecCtrl);

/************************************************************************/
/************************************************************************/
/* Only the elements we need to extract are listed here.		*/
/* The rest are handled by "unknown" element handler.			*/
SX_ELEMENT sclStartElements[] = 
{
  {"SCL", 		SX_ELF_CSTARTEND,		_SCL_SEFun, NULL, 0}
};

SX_ELEMENT SCLElements[] = 
{
  {"Header",           	SX_ELF_CSTART|SX_ELF_OPT, 	_Header_SFun, NULL, 0},
  {"Communication",    	SX_ELF_CSTARTEND|SX_ELF_OPT, 	_Communication_SEFun, NULL, 0},
  {"IED",            	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_IED_SEFun, NULL, 0},
  {"DataTypeTemplates", SX_ELF_CSTARTEND|SX_ELF_OPT, 	_DataTypeTemplates_SEFun, NULL, 0}
};

/************************************************************************/
/* Tables for mapping "Communication" elements.				*/
/************************************************************************/
SX_ELEMENT CommunicationElements[] = 
{
  {"SubNetwork",      	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_SubNetwork_SEFun, NULL, 0}
};

SX_ELEMENT SubNetworkElements[] = 
{
  /* NOTE: "bitRate" and "Text" elements ignored.	*/
  {"ConnectedAP",      	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_ConnectedAP_SEFun, NULL, 0}
};

SX_ELEMENT ConnectedAPElements[] = 
{
  /* DEBUG: Ignore "PhyConn".	*/
  {"Address",		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_Address_SEFun, NULL, 0},
  {"GSE",	      	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_GSE_SEFun, NULL, 0},
  {"SMV",	      	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_SMV_SEFun, NULL, 0}
};

/* This is for "Address" element inside "ConnectedAP".	*/
SX_ELEMENT AddressElements[] = 
{
  {"P",			SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_Address_P_SEFun, NULL, 0}
};

SX_ELEMENT GSEElements[] = 
{
  {"Address",      	SX_ELF_CSTARTEND|SX_ELF_OPT, 	_GSE_Address_SEFun, NULL, 0},
  {"MinTime",      	SX_ELF_CSTARTEND|SX_ELF_OPT, 	_GSE_MinTime_SEFun, NULL, 0},
  {"MaxTime",      	SX_ELF_CSTARTEND|SX_ELF_OPT, 	_GSE_MaxTime_SEFun, NULL, 0}
};

SX_ELEMENT GSEAddressElements[] = 
{
  {"P",      		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_GSE_Address_P_SEFun, NULL, 0}
};

SX_ELEMENT SMVElements[] = 
{
  {"Address",      	SX_ELF_CSTARTEND|SX_ELF_OPT, 	_SMV_Address_SEFun, NULL, 0}
};

SX_ELEMENT SMVAddressElements[] = 
{
  {"P",      		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_SMV_Address_P_SEFun, NULL, 0}
};
/************************************************************************/
/* Tables for mapping "IED" elements.					*/
/************************************************************************/
SX_ELEMENT IEDElements[] = 
{
  {"Services",		SX_ELF_CSTARTEND,		Services_SEFun, NULL, 0},
  {"AccessPoint",      	SX_ELF_CSTARTEND|SX_ELF_RPT, 	_AccessPoint_SEFun, NULL, 0}
};

SX_ELEMENT AccessPointElements[] = 
{
  {"Server",      	SX_ELF_CSTARTEND, 		_Server_SEFun, NULL, 0}
};

SX_ELEMENT ServerElements[] = 
{
  {"LDevice",      	SX_ELF_CSTARTEND|SX_ELF_RPT,	_LDevice_SEFun, NULL, 0}
};

SX_ELEMENT LDeviceElements[] = 
{
  {"LN0",      		SX_ELF_CSTARTEND,		_LN_SEFun, NULL, 0},
  {"LN",      		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_LN_SEFun, NULL, 0}
};

SX_ELEMENT LN0Elements[] = 
{
  {"DataSet",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DataSet_SEFun, NULL, 0},
  {"ReportControl",	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_ReportControl_SEFun, NULL, 0},
  {"DOI",		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DOI_SEFun, NULL, 0}, 
  {"SampledValueControl",	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_SampledValueControl_SEFun, NULL, 0},
  {"LogControl",	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_LogControl_SEFun, NULL, 0},
  {"SettingControl",	SX_ELF_CSTART|SX_ELF_OPTRPT,	_SettingControl_SFun, NULL, 0},
  {"GSEControl",	SX_ELF_CSTART|SX_ELF_OPTRPT,	_GSEControl_SFun, NULL, 0},
  {"Inputs",		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_Inputs_SEFun, NULL, 0}
};

SX_ELEMENT LNElements[] = 
{
  {"DataSet",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DataSet_SEFun, NULL, 0},
  {"ReportControl",	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_ReportControl_SEFun, NULL, 0},
  {"DOI",		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DOI_SEFun, NULL, 0}, 
  {"SampledValueControl",	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_SampledValueControl_SEFun, NULL, 0},
  {"LogControl",	SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_LogControl_SEFun, NULL, 0},
  {"Inputs",		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_Inputs_SEFun, NULL, 0}
};

SX_ELEMENT DataSetElements[] = 
{
  {"FCDA",  		SX_ELF_CSTART|SX_ELF_RPT,	_FCDA_SFun, NULL, 0}
};

SX_ELEMENT ReportControlElements[] = 
{
  {"TrgOps",  		SX_ELF_CSTART|SX_ELF_OPT,	_TrgOps_SFun, NULL, 0},
  {"OptFields",		SX_ELF_CSTART,			_OptFlds_SFun, NULL, 0},
  {"RptEnabled",	SX_ELF_CSTART|SX_ELF_OPT,	_RptEnabled_SFun, NULL, 0}
};

SX_ELEMENT LogControlElements[] = 
{
  {"TrgOps",  		SX_ELF_CSTART|SX_ELF_OPT,	_TrgOps_SFun, NULL, 0}
};

SX_ELEMENT SampledValueControlElements[] = 
{
  {"SmvOpts",  		SX_ELF_CSTART,			_SmvOpts_SFun, NULL, 0}
};

SX_ELEMENT InputsElements[] = 
{
/* DEBUG: Text and Private elements ignored??	*/
  {"ExtRef",  		SX_ELF_CSTART|SX_ELF_OPTRPT,	_ExtRef_SFun, NULL, 0}
};

SX_ELEMENT DOIElements[] = 
{
  {"SDI",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_SDI_SEFun, NULL, 0},
  {"DAI",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DAI_SEFun, NULL, 0}
};

/* SDI can be nested under itself indefinitely */
SX_ELEMENT SDIElements[] = 
{
  {"SDI",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_SDI_SEFun, NULL, 0},
  {"DAI",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DAI_SEFun, NULL, 0}
};

SX_ELEMENT DAIElements[] = 
{
  {"Val",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DAI_Val_SEFun, NULL, 0}
};

/************************************************************************/
/* Tables for mapping "DataTypeTemplates" elements.			*/
/************************************************************************/
SX_ELEMENT DataTypeTemplatesElements[] = 
{
  {"LNodeType",  	SX_ELF_CSTARTEND|SX_ELF_RPT,	_LNodeType_SEFun, NULL, 0},
  {"DOType",  		SX_ELF_CSTARTEND|SX_ELF_RPT,	_DOType_SEFun, NULL, 0},
  {"DAType",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DAType_SEFun, NULL, 0},
  {"EnumType", 		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_EnumType_SEFun, NULL, 0}
};

SX_ELEMENT LNodeTypeElements[] = 
{
  {"DO",  		SX_ELF_CSTART|SX_ELF_RPT,	_DO_SFun, NULL, 0}
};

SX_ELEMENT DOTypeElements[] = 
{
  {"DA",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DA_SEFun, NULL, 0},
  {"SDO",  		SX_ELF_CSTART|SX_ELF_OPTRPT,	_SDO_SFun, NULL, 0}
};

SX_ELEMENT DAElements[] = 
{
  {"Val",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_DA_Val_SEFun, NULL, 0}
};

SX_ELEMENT DATypeElements[] = 
{
  {"BDA",  		SX_ELF_CSTARTEND|SX_ELF_RPT,	_BDA_SEFun, NULL, 0}
};

SX_ELEMENT BDAElements[] = 
{
  {"Val",  		SX_ELF_CSTARTEND|SX_ELF_OPTRPT,	_BDA_Val_SEFun, NULL, 0}
};

SX_ELEMENT EnumTypeElements[] = 
{
  {"EnumVal",  		SX_ELF_CSTARTEND|SX_ELF_RPT,	_EnumVal_SEFun, NULL, 0}
};

/************************************************************************/
/*			chk_mms_ident_legal				*/
/* Check that all characters in "name" are legal for an MMS Identifier.	*/
/* Important if "name" is used to construct an MMS Identifier.		*/
/* Allow only alphanumeric or '_' or '$'.				*/
/************************************************************************/
static ST_RET chk_mms_ident_legal (ST_CHAR *name)
  {
ST_CHAR *pChar;
  for (pChar = name; *pChar != '\0'; pChar++)
    {
    if ((!(isalnum(*pChar))) && (*pChar != '_')  && (*pChar != '$') && (*pChar != '-'))//MEMO: ÈÝ´í£¬Ìí¼Ó¡®-¡¯×Ö·ûÔÊÐí [2016-8-18 18:10 ÉÛ¿­Ìï]
      {
      return (SD_FAILURE);
      }
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			chk_comp_name_legal				*/
/* Check for legal characters in a component name.			*/
/* All characters must be legal MMS Identifier characters but not '$'.	*/
/* Allow only alphanumeric or '_'.					*/
/************************************************************************/
static ST_RET chk_comp_name_legal (ST_CHAR *name)
  {
ST_CHAR *pChar;
  for (pChar = name; *pChar != '\0'; pChar++)
    {
    if ((!(isalnum(*pChar))) && (*pChar != '_') && (*pChar != '-'))//MEMO: ÈÝ´í£¬Ìí¼Ó¡®-¡¯×Ö·ûÔÊÐí [2016-8-18 18:12 ÉÛ¿­Ìï]
      {
      return (SD_FAILURE);
      }
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			convert_mac					*/
/* Converts MAC string read from SCL file (like 01-02-03-04-05-06)	*/
/* to 6 byte hex MAC address.						*/
/************************************************************************/
#define MAX_MAC_STRING_LEN	60
static ST_RET convert_mac (ST_UCHAR *dst, ST_CHAR *src)
  {
ST_RET retcode;
ST_CHAR tmpbuf [MAX_MAC_STRING_LEN+1];
ST_CHAR *tmpptr;
ST_UINT dstlen;

  /* Input string may include extra blanks, so allow for fairly long string.*/
  if (strlen (src) > MAX_MAC_STRING_LEN)
    retcode = SD_FAILURE;
  else
    {
    /* Just replace each '-' with ' '. Then use ascii_to_hex_str to convert.*/
    tmpptr = tmpbuf;
    /* Copy until NULL terminator but ignore '-' and spaces.	*/
    for ( ;  *src;  src++)
      {
      if (*src != '-' && (!isspace(*src)))
        *tmpptr++ = *src;
      }
    *tmpptr = '\0';	/* NULL terminate temp buffer	*/
    retcode = ascii_to_hex_str (dst, &dstlen, 6, tmpbuf);
    if (retcode == SD_SUCCESS && dstlen != 6)
      retcode = SD_FAILURE; 
    }
  return (retcode);
  }
    
/************************************************************************/
/*			convert_hex_str_to_ulong				*/
/* Convert a hexidecimal string to an unsigned long.			*/
/************************************************************************/
static ST_RET convert_hex_str_to_ulong (ST_CHAR *str, ST_ULONG *ulong_ptr)
  {
ST_ULONG ulong;	/* converted value	*/
size_t j, len;
  len = strlen (str);
  for (j = 0; j<len; j++)
    {
    if (!isxdigit (str[j]))
      {
      SXLOG_ERR1 ("Illegal Hex string '%s'", str);
      return (SD_FAILURE);	/* stop on first invalid char	*/
      }
    }
  /* All digits are valid. Convert to unsigned long.	*/
  errno = 0;	/* chk after strtoul	*/
  ulong = strtoul (str, NULL, 16);	/* base=16(hex)	*/
  if (errno)
    {
    SXLOG_ERR1 ("Error converting hex string '%s' to unsigned long", str);
    return (SD_FAILURE);
    }

  *ulong_ptr = ulong;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			scl_stop_parsing				*/
/* NOTE: Errors should be logged before calling this function,		*/
/*       except SX_ERR_ALLOC.						*/
/************************************************************************/

ST_VOID scl_stop_parsing (SX_DEC_CTRL *sxDecCtrl, ST_RET errCode)
  {
  /* Log only SX_ERR_ALLOC here since it may occur in many places.	*/
  /* Other errors logged where they occur.				*/
  if (errCode == SX_ERR_ALLOC)
    SXLOG_ERR0 ("SCL PARSE: Allocation error. Should never occur.");
    
  sxDecCtrl->errCode = errCode;
  sxDecCtrl->termFlag = SD_TRUE;
  }

/************************************************************************/
/*			scl_get_attr_ptr				*/
/* Get a pointer to an attr string stored in SX_DEC_CTRL.		*/
/* If attr found, SD_SUCCESS returned & "*value" points to string.	*/
/* NOTE: The pointer returned in "*value" might not be valid later	*/
/*	when parsing continues.						*/
/* RETURNS: SD_SUCCESS or SD_FAILURE (but also calls scl_stop_parsing	*/
/*          to stop parser on any critical error).			*/
/*          This greatly simplifies calling code.			*/
/************************************************************************/

ST_RET scl_get_attr_ptr (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *name,
			        ST_CHAR **value, ST_BOOLEAN required)
  {
ST_RET ret;

  ret = sx_get_attr_ptr (sxDecCtrl, value, name);
  if (ret != SD_SUCCESS)
    {
    *value = NULL;	/* make sure ptr is NULL on error (better than garbage)*/
    if (required)
      {
      SXLOG_ERR2 ("SCL PARSE: In element '%s', required attribute '%s' not found",
                  sxDecCtrl->sxDecElInfo.tag, name);
      scl_stop_parsing (sxDecCtrl, SX_ERR_REQUIRED_ATTR_NOT_FOUND);
      }
    }
  else
    SXLOG_DEC2 ("SCL PARSE: Found attribute '%s', value is '%s'", name, *value);

  return (ret);
  }

/************************************************************************/
/*			scl_get_attr_copy				*/
/* Get a pointer to an attr string stored in SX_DEC_CTRL.		*/
/* If strlen <= maxValueLen, copy string, else return error.		*/
/* RETURNS: SD_SUCCESS or SD_FAILURE (but also calls scl_stop_parsing	*/
/*          to stop parser on any critical error).			*/
/*          This greatly simplifies calling code.			*/
/************************************************************************/

ST_RET scl_get_attr_copy (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *name,
			        ST_CHAR *value, ST_UINT maxValueLen, ST_BOOLEAN required)
  {
ST_RET ret;
ST_CHAR *pValue;

  ret = scl_get_attr_ptr (sxDecCtrl, name, &pValue, required);
  if (ret == SD_SUCCESS)
    {
    if (strlen (pValue) <= maxValueLen)
      strcpy (value, pValue);	/* copy string to caller's buffer	*/
    else
      {
      SXLOG_ERR3 ("Attribute Value='%s' exceeds max len '%d' for attribute '%s'", pValue, maxValueLen, name);
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      ret = SD_FAILURE;
      }
    }

  return (ret);
  }

/************************************************************************/
/*			scl_get_int_attr				*/
/* RETURNS: SD_SUCCESS or SD_FAILURE (but also calls scl_stop_parsing	*/
/*          to stop parser on any critical error).			*/
/*          This greatly simplifies calling code.			*/
/************************************************************************/

ST_RET scl_get_int_attr (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *name,
			        ST_INT *value, ST_BOOLEAN required)
  {
ST_RET ret;
ST_CHAR *str;
  ret = scl_get_attr_ptr (sxDecCtrl, name, &str, required);
  if (ret == SD_SUCCESS)
    {
    ret = asciiToSint (str, value);
    if (ret)
      {
      SXLOG_ERR3 ("SCL PARSE: In element '%s', attr '%s', can't convert string '%s' to INT",
                  sxDecCtrl->sxDecElInfo.tag, name, str);
      scl_stop_parsing (sxDecCtrl, SX_ERR_CONVERT);
      }
    }
  return (ret);
  }

/************************************************************************/
/*			scl_get_uint_attr				*/
/* RETURNS: SD_SUCCESS or SD_FAILURE (but also calls scl_stop_parsing	*/
/*          to stop parser on any critical error).			*/
/*          This greatly simplifies calling code.			*/
/************************************************************************/

ST_RET scl_get_uint_attr (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *name,
			        ST_UINT *value, ST_BOOLEAN required)
  {
ST_RET ret;
ST_CHAR *str;
  ret = scl_get_attr_ptr (sxDecCtrl, name, &str, required);
  if (ret == SD_SUCCESS)
    {
    ret = asciiToUint (str, value);
    if (ret)
      {
      SXLOG_ERR3 ("SCL PARSE: In element '%s', attr '%s', can't convert string '%s' to UINT",
                  sxDecCtrl->sxDecElInfo.tag, name, str);
      scl_stop_parsing (sxDecCtrl, SX_ERR_CONVERT);
      }
    }
  return (ret);
  }

/************************************************************************/
/*			 construct_flattened				*/
/* Construct a flattened variable name from DOI, SDI, DAI names.	*/
/************************************************************************/
ST_RET construct_flattened (ST_CHAR *flattened, size_t maxlen, ST_CHAR *name, ST_CHAR *ix)
  {
size_t ixlen;
ST_RET retCode;

  /* Calc space needed for optional [ix]	*/
  if (ix)
    ixlen = strlen(ix)+2;	/* string plus brackets	*/
  else
    ixlen = 0;
  /* Make sure there is room for [ix] and "$"	*/
  if (strlen (flattened) + strlen(name) + ixlen + 1 <= maxlen)
    {
    /* If flattened is now empty, just copy name, else add "$" then name.*/
    if (strlen(flattened) == 0)
      strcpy (flattened, name);
    else
      {
      strcat (flattened, "$");
      strcat (flattened, name);
      }
    if (ix)
      {      /* Add 'ix' to flattened if necessary.	*/
      strcat (flattened, "[");
      strcat (flattened, ix);
      strcat (flattened, "]");
      }
    retCode = SD_SUCCESS;
    }
  else
    {	/* flattened is big, so this error should never occur with normal SCL.*/
    SXLOG_ERR2 ("ERROR: not enough space to add name '%s' to flattened name '%s'", name, flattened);
    retCode = SD_FAILURE;
    }
  return (retCode);
  }

/************************************************************************/
/*			scl_serv_cfg_find_1				*/
/* Used only for parseMode == SCL_PARSE_MODE_SCD_FILTERED.		*/
/* Find pointer to first in scl_serv_cfg_arr array where iedName	*/
/* matches (apName may NOT match).					*/
/************************************************************************/
SCL_SERV_CFG *scl_serv_cfg_find_1 (SCL_INFO *scl_info, ST_CHAR *iedName)
  {
ST_UINT j;
  for (j=0; j<scl_info->scl_serv_cfg_num; j++)
    {
    if (strcmp (scl_info->scl_serv_cfg_arr[j].iedName, iedName) == 0)
      {
      return (&scl_info->scl_serv_cfg_arr[j]);	/* found match	*/
      }
    }
  return (NULL);	/* no match found	*/
  }

/************************************************************************/
/*			scl_serv_cfg_find_2				*/
/* Used only for parseMode == SCL_PARSE_MODE_SCD_FILTERED.		*/
/* Find pointer to first in scl_serv_cfg_arr array where iedName/apName	*/
/* "pair" matches.							*/
/************************************************************************/
SCL_SERV_CFG *scl_serv_cfg_find_2 (SCL_INFO *scl_info,
	ST_CHAR *iedName,
	ST_CHAR *apName)
  {
ST_UINT j;
  for (j = 0;
       j < scl_info->scl_serv_cfg_num;
       j++)
    {
    if (strcmp (scl_info->scl_serv_cfg_arr[j].iedName, iedName) == 0 &&
        strcmp (scl_info->scl_serv_cfg_arr[j].apName, apName) == 0)
      {
      return (&scl_info->scl_serv_cfg_arr[j]);	/* match found	*/
      }
    }
  return (NULL);	/* match NOT found	*/
  }

/************************************************************************/
/*			scl_iedtype_match				*/
/* Used only for parseMode == SCL_PARSE_MODE_SCD_FILTERED.		*/
/* Loop through iedType array passed to "scl_parse_scd_filtered".	*/
/* See if this iedType matches one in the array.			*/
/* RETURNS: SD_TRUE if iedType matches one passed to parser.		*/
/*          SD_FALSE otherwise.						*/
/************************************************************************/
ST_BOOLEAN scl_iedtype_match (SCL_INFO *scl_info, ST_CHAR *iedType)
  {
ST_UINT j;
  if (scl_info->scl_iedtype_cfg_arr == NULL)
    return (SD_TRUE);	/* This is allowed. Treat as if we found a match*/
			/* In this case, ANY iedType is allowed.	*/
  for (j=0; j<scl_info->scl_iedtype_cfg_num; j++)
    {
    if (strcmp (scl_info->scl_iedtype_cfg_arr[j], iedType) == 0)
      return (SD_TRUE);	/* found match	*/
    }
  return (SD_FALSE);	/* no match found	*/
  }

/************************************************************************/
/*			scl_parse					*/
/* DEPRECATED: Use scl_parse_cid or scl_parse_scd_all.			*/
/************************************************************************/

ST_RET scl_parse (ST_CHAR *xmlFileName, ST_CHAR *iedName, 
		  ST_CHAR *accessPointName, SCL_INFO *sclInfo)
  {
  ST_RET ret;
  /* Just pass NULL for (SCL_OPTIONS *) arg.	*/
  ret = scl_parse_cid (xmlFileName, iedName, accessPointName, NULL, sclInfo);
  return (ret);
  }

/************************************************************************/
/*			scl_parse_cid					*/
/* Parse a CID file (look for matching IED and AccessPoint name).	*/
/************************************************************************/
ST_RET scl_parse_cid (ST_CHAR *xmlFileName,
	ST_CHAR *iedName, 
	ST_CHAR *accessPointName,
	SCL_OPTIONS *options,	/* miscellaneous parser options		*/
				/* may be NULL if no options needed	*/
	SCL_INFO *sclInfo)	/* main struct where all SCL info stored*/
  {
ST_RET ret;
SCL_DEC_CTRL sclDecCtrl = {0};	/* start with clean struct.	*/

  /* If "iedName" contains illegal characters, don't even parse.	*/
  /* It must match an "IED name" in the file which also must be legal.	*/
  ret = chk_mms_ident_legal (iedName);
  if (ret)
    {
    SXLOG_ERR1 ("Illegal character in IED name '%s' passed to SCL parser. Cannot parse.", iedName);
    return (ret);
    }

  memset (sclInfo, 0, sizeof (SCL_INFO));    /* CRITICAL: start with clean struct*/

  sclDecCtrl.iedName = iedName;
  sclDecCtrl.accessPointName = accessPointName;
  sclDecCtrl.accessPointFound = SD_FALSE;
  sclDecCtrl.sclInfo = sclInfo;

  /* Set "parseMode" to control parsing later.	*/
  sclDecCtrl.parseMode = SCL_PARSE_MODE_CID;

  /* save parser options to use later. Used for any parse mode	*/
  if (options != NULL)
    memcpy (&sclInfo->options, options, sizeof (SCL_OPTIONS));

  ret = sx_parseExx_mt (xmlFileName, 
          sizeof (sclStartElements)/sizeof(SX_ELEMENT), sclStartElements,
          &sclDecCtrl, _scl_unknown_el_start, _scl_unknown_el_end);

  /* NOTE: sx_parseEx_mt doesn't log error if file open fails, so log here*/
  /* It may not log some other errors, so log any other error here too.	*/
  if (ret == SX_FILE_NOT_FOUND)
    SXLOG_ERR1 ("XML File (%s) Open Error",xmlFileName);
  else if (ret != SD_SUCCESS)
    SXLOG_ERR2 ("Error 0x%X parsing SCL file (%s)", ret, xmlFileName);

  /* If parsing successful, check if requested AccessPoint was found.	*/
  if (ret == SD_SUCCESS  &&  sclDecCtrl.accessPointFound == SD_FALSE)
    {
    SXLOG_ERR3 ("IED='%s' or AccessPoint='%s' not found in input file (%s)",
                iedName, accessPointName, xmlFileName);
    ret = SX_REQUIRED_TAG_NOT_FOUND;	/* new error code could be clearer*/
    }

  return (ret);
  }

/************************************************************************/
/************************************************************************/
/*			SCL_SEFun					*/
/************************************************************************/

static ST_VOID _SCL_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_CHAR *version;
ST_CHAR *revision;
SCL_DEC_CTRL *sclDecCtrl;
SCL_INFO *sclInfo;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclInfo = sclDecCtrl->sclInfo;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    if (sclInfo->options.forceEdition != 0)
      {
      if (sclInfo->options.forceEdition == 1 || sclInfo->options.forceEdition == 2)
	{
	sclInfo->edition = sclInfo->options.forceEdition;
	SXLOG_FLOW1 ("'forceEdition' option used. Assuming 61850 Edition = %d.", sclInfo->edition);
	}
      else
	{
	sclInfo->edition = 1;
	SXLOG_ERR1 ("Option 'forceEdition = %d' not supported. Assuming 61850 Edition 1", sclInfo->options.forceEdition);
	}
      }
    else
      {
      /* Look for attributes "version" & "revision", required in Ed 2, not present in Ed 1.*/
      if (scl_get_attr_ptr (sxDecCtrl, "version", &version, SCL_ATTR_OPTIONAL) == SD_SUCCESS &&
	  scl_get_attr_ptr (sxDecCtrl, "revision", &revision, SCL_ATTR_OPTIONAL) == SD_SUCCESS)
	{
	sclInfo->edition = 2; /* use 61850 Edition 2 for this parse  */
	if (strcmp (version, "2007") != 0)
	  SXLOG_ERR1 ("Invalid SCL version = '%s'. Should be '2007' Assuming 61850 Edition 2 anyway.", version);
	if (strcmp (revision, "A") != 0)
	  SXLOG_ERR1 ("Invalid SCL revision = '%s'. Should be 'A'. Assuming 61850 Edition 2 anyway.", revision);
	}
      else
	{	/* assume default (Edition 1)	*/
	sclInfo->edition = 1;
	SXLOG_FLOW0 ("SCL version or revision not present. Assuming 61850 Edition 1.");
	}
      }
    sx_push (sxDecCtrl, sizeof(SCLElements)/sizeof(SX_ELEMENT), SCLElements, SD_FALSE);
    }
  else
    {
    while (sxDecCtrl->itemStackLevel > 0)
      sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_Header_SFun					*/
/************************************************************************/

static ST_VOID _Header_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *nameStructure;
ST_RET ret;
SCL_INFO *sclInfo;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  sclInfo = sclDecCtrl->sclInfo;

  /* Get required attributes	*/
  ret = scl_get_attr_copy (sxDecCtrl, "id", sclInfo->Header.id,
            (sizeof(sclInfo->Header.id)-1), SCL_ATTR_REQUIRED);
  if (ret != SD_SUCCESS)
    return;	/* At least one required attr not found. Stop now.	*/

  /* Handle optional "nameStructure".	*/
  if (scl_get_attr_ptr (sxDecCtrl, "nameStructure", &nameStructure, SCL_ATTR_OPTIONAL) == SD_SUCCESS)
    {
    if (strcmp (nameStructure, "IEDName") != 0)
      SXLOG_ERR1 ("Header attribute nameStructure='%s' not allowed. Assuming nameStructure='IEDName' (i.e. 'Product Naming')", nameStructure);
    }
  /* Always assume nameStructure="IEDName" (i.e. "Product Naming")	*/
  sclInfo->Header.nameStructure = SCL_NAMESTRUCTURE_IEDNAME;
  }

/************************************************************************/
/*			_Communication_SEFun				*/
/************************************************************************/
static ST_VOID _Communication_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sx_push (sxDecCtrl, sizeof(CommunicationElements)/sizeof(SX_ELEMENT), 
             CommunicationElements, SD_FALSE);
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }
/************************************************************************/
/*			_SubNetwork_SEFun				*/
/************************************************************************/
static ST_VOID _SubNetwork_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;
ST_CHAR *desc;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SCL_SUBNET *scl_subnet;
    scl_subnet = scl_subnet_add (sclDecCtrl->sclInfo);
    if (scl_subnet == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }
    /* Get required attributes.	*/
    ret = scl_get_attr_copy (sxDecCtrl, "name", scl_subnet->name, (sizeof(scl_subnet->name)-1), SCL_ATTR_REQUIRED);
    if (ret)
      {
      return;
      }
    /* Get optional attributes.	*/
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, SCL_ATTR_OPTIONAL);
    if (ret == SD_SUCCESS)
      scl_subnet->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
    ret = scl_get_attr_copy (sxDecCtrl, "type", scl_subnet->type, (sizeof(scl_subnet->type)-1), SCL_ATTR_OPTIONAL);

    sx_push (sxDecCtrl, sizeof(SubNetworkElements)/sizeof(SX_ELEMENT), 
             SubNetworkElements, SD_FALSE);
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }
/************************************************************************/
/*			_ConnectedAP_SEFun				*/
/************************************************************************/
static ST_VOID _ConnectedAP_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;
ST_CHAR *desc;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SCL_CAP *scl_cap;
    scl_cap = scl_cap_add (sclDecCtrl->sclInfo);
    if (scl_cap == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }
    /* Get required attributes	*/
    ret = scl_get_attr_copy (sxDecCtrl, "iedName", scl_cap->iedName, (sizeof(scl_cap->iedName)-1), SCL_ATTR_REQUIRED);
    ret |= scl_get_attr_copy (sxDecCtrl, "apName", scl_cap->apName, (sizeof(scl_cap->apName)-1), SCL_ATTR_REQUIRED);
    if (ret)
      {
      return;
      }
    /* Get optional attributes.	*/
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, SCL_ATTR_OPTIONAL);
    if (ret == SD_SUCCESS)
      scl_cap->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/

    sx_push (sxDecCtrl, sizeof(ConnectedAPElements)/sizeof(SX_ELEMENT), 
             ConnectedAPElements, SD_FALSE);
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_Address_SEFun					*/
/* Process "Address" element.						*/
/************************************************************************/
static ST_VOID _Address_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Only one Address allowed, so no need to alloc struct & add to list.*/
    /* Functions will save address info directly to this struct:	*/
    /*   "sclDecCtrl->sclInfo->subnetHead->capHead->address".		*/
    sx_push (sxDecCtrl, sizeof(AddressElements)/sizeof(SX_ELEMENT), 
             AddressElements, SD_FALSE);
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_Address_P_SEFun				*/
/* Process "P" element inside "Address" element.			*/
/* Save this Address element to appropriate member of this structure:	*/
/*   "sclDecCtrl->sclInfo->subnetHead->capHead->address".		*/
/* NOTE: this handles OSI-PSEL, OSI-SSEL, OSI-TSEL, IP, etc.		*/
/* NOTE: should use case sensitive strcmp but this is more forgiving.	*/
/************************************************************************/
static ST_VOID _Address_P_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_CHAR *str;
ST_RET ret;
ST_CHAR *strOut;
ST_INT strLen;
ST_UINT hexStrLen;
ST_INT32 int32_val;
SCL_ADDRESS *address;	/* store all address info here	*/

  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    /* Save this Address element to appropriate member of this structure.*/
    address = &sclDecCtrl->sclInfo->subnetHead->capHead->address;

    ret = scl_get_attr_ptr (sxDecCtrl, "type", &str, SCL_ATTR_REQUIRED);
    if (ret != SD_SUCCESS)
      return;	/* error getting "required" attr, already logged.	*/

    if (!strcmpi(str,"OSI-PSEL"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        /* Set "psel" and "psel_len".	*/
        if (ascii_to_hex_str (address->psel,&hexStrLen,MAX_PSEL_LEN,strOut)
            == SD_SUCCESS)
          {
          address->psel_len = hexStrLen;
          }
        else
          {
          SXLOG_ERR1 ("Illegal OSI-PSEL '%s'", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        }
      }
    else if (!strcmpi(str,"OSI-SSEL"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        /* Set "ssel" and "ssel_len".	*/
        if (ascii_to_hex_str (address->ssel,&hexStrLen,MAX_SSEL_LEN,strOut)
            == SD_SUCCESS)
          {
          address->ssel_len = hexStrLen;
          }
        else
          {
          SXLOG_ERR1 ("Illegal OSI-SSEL '%s'", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        }
      }
    else if (!strcmpi(str,"OSI-TSEL"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        /* Set "tsel" and "tsel_len".	*/
        if (ascii_to_hex_str (address->tsel,&hexStrLen,MAX_TSEL_LEN,strOut)
            == SD_SUCCESS)
          {
          address->tsel_len = hexStrLen;
          }
        else
          {
          SXLOG_ERR1 ("Illegal OSI-TSEL '%s'", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        }
      }
    else if (!strcmpi(str,"IP"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if ((address->ip = inet_addr (strOut)) == INADDR_NONE)
          {
          SXLOG_ERR1 ("Illegal IP Address '%s'", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        }
      }
    else if (!strcmpi(str,"OSI-AP-Title"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (asciiToObjId (strOut, &address->ae_title.AP_title))
          {
          SXLOG_ERR1 ("Invalid AP Title '%s'", strOut);
          //scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          address->ae_title.AP_title_pres = SD_TRUE;    /* defaults to FALSE    */
        }
      }
    else if (!strcmpi(str,"OSI-AE-Qualifier"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (strLen == 0 || asciiToSint32 (strOut, &int32_val) != SD_SUCCESS)
          {
          SXLOG_ERR1 ("Invalid AE-Qual '%s'", strOut);
          //scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          {
          address->ae_title.AE_qual = int32_val;
          address->ae_title.AE_qual_pres = SD_TRUE;   /* defaults to FALSE    */
          }
        }
      }
    else if (!strcmpi(str,"OSI-AP-Invoke"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (strLen == 0 || asciiToSint32 (strOut, &int32_val) != SD_SUCCESS)
          {
          SXLOG_ERR1 ("Invalid AP-Invoke '%s', ignore it!", strOut);
          //del by skt 20190103 ÈÝ´í scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          {
          address->ae_title.AP_inv_id = int32_val;
          address->ae_title.AP_inv_id_pres = SD_TRUE;   /* defaults to FALSE    */
          }
        }
      }
    else if (!strcmpi(str,"OSI-AE-Invoke"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (strLen == 0 || asciiToSint32 (strOut, &int32_val) != SD_SUCCESS)
          {
          SXLOG_ERR1 ("Invalid AE-Invoke '%s', ignore it!", strOut);
          //del by skt 20190103 ÈÝ´í  scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          {
          address->ae_title.AE_inv_id = int32_val;
          address->ae_title.AE_inv_id_pres = SD_TRUE;   /* defaults to FALSE    */
          }
        }
      }
    }
  }

/************************************************************************/
/*			_GSE_SEFun					*/
/************************************************************************/
static ST_VOID _GSE_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* NOTE: save ptr in sclDecCtrl->scl_gse to use later in parsing.	*/
    sclDecCtrl->scl_gse = scl_gse_add (sclDecCtrl->sclInfo);
    if (sclDecCtrl->scl_gse == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }
    ret = scl_get_attr_copy (sxDecCtrl, "ldInst", sclDecCtrl->scl_gse->ldInst, (sizeof(sclDecCtrl->scl_gse->ldInst)-1), SCL_ATTR_REQUIRED);    
    ret |= scl_get_attr_copy (sxDecCtrl, "cbName", sclDecCtrl->scl_gse->cbName, (sizeof(sclDecCtrl->scl_gse->cbName)-1), SCL_ATTR_REQUIRED);    
    if (ret)
      {
      return;
      }
    else
      sx_push (sxDecCtrl, sizeof(GSEElements)/sizeof(SX_ELEMENT), 
             GSEElements, SD_FALSE);
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }
/************************************************************************/
/*			_GSE_Address_SEFun				*/
/************************************************************************/
static ST_VOID _GSE_Address_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sx_push (sxDecCtrl, sizeof(GSEAddressElements)/sizeof(SX_ELEMENT), 
             GSEAddressElements, SD_FALSE);
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }
/************************************************************************/
/*			_GSE_Address_P_SEFun				*/
/************************************************************************/
static ST_VOID _GSE_Address_P_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_CHAR *str;
ST_RET ret;
ST_CHAR *strOut;
ST_INT strLen;
ST_ULONG ulong;	/* converted value	*/

  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    ret = scl_get_attr_ptr (sxDecCtrl, "type", &str, SCL_ATTR_REQUIRED);
    if (ret != SD_SUCCESS)
      return;	/* error getting "required" attr, already logged.	*/

    if (!strcmpi(str,"MAC-Address"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (convert_mac (sclDecCtrl->scl_gse->MAC,strOut))
          {
          SXLOG_ERR1 ("Illegal MAC Address '%s'", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        }
      }
    else if (!strcmpi(str,"APPID"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (convert_hex_str_to_ulong (strOut, &ulong) || ulong > 0xFFFF)
          {
          SXLOG_ERR1 ("Illegal APPID '%s' (must be 0-FFFF)", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          sclDecCtrl->scl_gse->APPID = (ST_UINT) ulong;
        }
      }
    else if (!strcmpi(str,"VLAN-PRIORITY"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (convert_hex_str_to_ulong (strOut, &ulong) || ulong > 7)
          {
          SXLOG_ERR1 ("Illegal VLAN-PRIORITY '%s' (must be 0-7)", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          sclDecCtrl->scl_gse->VLANPRI = (ST_UINT) ulong;
        }
      }
    else if (!strcmpi(str,"VLAN-ID"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (convert_hex_str_to_ulong (strOut, &ulong) || ulong > 0xFFF)
          {
          SXLOG_ERR1 ("Illegal VLAN-ID '%s' (must be 0-FFF)", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          sclDecCtrl->scl_gse->VLANID = (ST_UINT) ulong;
        }
      }
    }
  }
/************************************************************************/
/*			_SMV_SEFun					*/
/************************************************************************/
static ST_VOID _SMV_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* NOTE: save ptr in sclDecCtrl->scl_smv to use later in parsing.	*/
    sclDecCtrl->scl_smv = scl_smv_add (sclDecCtrl->sclInfo);
    if (sclDecCtrl->scl_smv == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }
    ret = scl_get_attr_copy (sxDecCtrl, "ldInst", sclDecCtrl->scl_smv->ldInst, (sizeof(sclDecCtrl->scl_smv->ldInst)-1), SCL_ATTR_REQUIRED);    
    ret |= scl_get_attr_copy (sxDecCtrl, "cbName", sclDecCtrl->scl_smv->cbName, (sizeof(sclDecCtrl->scl_smv->cbName)-1), SCL_ATTR_REQUIRED);    
    if (ret)
      {
      return;
      }
    else
      sx_push (sxDecCtrl, sizeof(SMVElements)/sizeof(SX_ELEMENT), 
             SMVElements, SD_FALSE);
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }
/************************************************************************/
/*			_SMV_Address_SEFun				*/
/************************************************************************/
static ST_VOID _SMV_Address_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sx_push (sxDecCtrl, sizeof(SMVAddressElements)/sizeof(SX_ELEMENT), 
             SMVAddressElements, SD_FALSE);
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }
/************************************************************************/
/*			_SMV_Address_P_SEFun				*/
/************************************************************************/
static ST_VOID _SMV_Address_P_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_CHAR *str;
ST_RET ret;
ST_CHAR *strOut;
ST_INT strLen;
ST_ULONG ulong;	/* converted value	*/

  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    ret = scl_get_attr_ptr (sxDecCtrl, "type", &str, SCL_ATTR_REQUIRED);
    if (ret != SD_SUCCESS)
      return;	/* error getting "required" attr, already logged.	*/

    if (!strcmpi(str,"MAC-Address"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (convert_mac (sclDecCtrl->scl_smv->MAC,strOut))
          {
          SXLOG_ERR1 ("Illegal MAC Address '%s'", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        }
      }
    else if (!strcmpi(str,"APPID"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (convert_hex_str_to_ulong (strOut, &ulong) || ulong > 0xFFFF)
          {
          SXLOG_ERR1 ("Illegal APPID '%s' (must be 0-FFFF)", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          sclDecCtrl->scl_smv->APPID = (ST_UINT) ulong;
        }
      }
    else if (!strcmpi(str,"VLAN-PRIORITY"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (convert_hex_str_to_ulong (strOut, &ulong) || ulong > 7)
          {
          SXLOG_ERR1 ("Illegal VLAN-PRIORITY '%s' (must be 0-7)", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          sclDecCtrl->scl_smv->VLANPRI = (ST_UINT) ulong;
        }
      }
    else if (!strcmpi(str,"VLAN-ID"))
      {
      ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
      if (ret == SD_SUCCESS)
        {
        if (convert_hex_str_to_ulong (strOut, &ulong) || ulong > 0xFFF)
          {
          SXLOG_ERR1 ("Illegal VLAN-ID '%s' (must be 0-FFF)", strOut);
          scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
          }
        else
          sclDecCtrl->scl_smv->VLANID = (ST_UINT) ulong;
        }
      }
    }
  }
/************************************************************************/
/*			IED_SEFun					*/
/************************************************************************/
static ST_VOID _IED_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *str;	/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_INFO *scl_info;
ST_BOOLEAN match_found = SD_FALSE;	/* set if IED name matches expected*/

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_info = sclDecCtrl->sclInfo;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_ptr (sxDecCtrl, "name", &str, required);
    /* If required attributes parsed OK, make sure "name" is legal.*/
    if (ret == SD_SUCCESS)
      {
      ret = chk_mms_ident_legal (str);
      if (ret)
        SXLOG_ERR1 ("Illegal character in IED name '%s'", str);
      }
    if (ret != SD_SUCCESS)
      {
      /* Error on required attribute, so stop parsing.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }
    /* Save to sclDecCtrl->iedNameProc to use while processing this IED.*/
    strcpy (sclDecCtrl->iedNameProc, str);
    /* Behavior depends on "parseMode".	*/
    if (sclDecCtrl->parseMode == SCL_PARSE_MODE_SCD_ALL)
      match_found = SD_TRUE;
    else if (sclDecCtrl->parseMode == SCL_PARSE_MODE_SCD_FILTERED)
      {
      /* Find a Server with this "iedName".	*/
      if (scl_serv_cfg_find_1 (scl_info, str) != NULL)
        match_found = SD_TRUE;	/* iedName matched one on our list*/
      }
    else
      {	/* SCL_PARSE_MODE_CID (default parse mode)	*/
      if (strcmp(str, sclDecCtrl->iedName) == 0)
        match_found = SD_TRUE;
      }

    if (match_found)
      {
      /* Initialize all default values in sclDecCtrl->scl_services.*/
      /* NOTE: Parsed values will be saved there. Later when SCL_SERVER	*/
      /* is allocated (see scl_server_add), this struct is copied there.*/
      scl_services_init (&sclDecCtrl->scl_services);

      SXLOG_CDEC1 ("SCL PARSE: IED 'name' match found: %s", str);
      sclDecCtrl->iedNameMatched = SD_TRUE;
      sx_push (sxDecCtrl, sizeof(IEDElements)/sizeof(SX_ELEMENT), IEDElements, SD_FALSE);
      }
    else
      {
      SXLOG_CDEC1 ("SCL PARSE: IED 'name' found (%s), not a match", str);
      }
    /* end required attributes */
    }
  else
    {
    sclDecCtrl->iedNameProc[0] = '\0';	/* clear iedName. Done with this IED.*/
    if (sclDecCtrl->iedNameMatched == SD_TRUE)
      {
      sclDecCtrl->iedNameMatched = SD_FALSE;
      sx_pop (sxDecCtrl);
      }
    }
  }
/************************************************************************/
/*			_AccessPoint_SEFun				*/
/************************************************************************/
static ST_VOID _AccessPoint_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *str;	/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_INFO *scl_info;
ST_BOOLEAN match_found = SD_FALSE;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_info = sclDecCtrl->sclInfo;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_ptr (sxDecCtrl, "name", &str, required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* Behavior depends on "parseMode".	*/
    if (sclDecCtrl->parseMode == SCL_PARSE_MODE_SCD_ALL)
      match_found = SD_TRUE;
    else if (sclDecCtrl->parseMode == SCL_PARSE_MODE_SCD_FILTERED)
      {
      /* If Server matches one we expect, continue normal parse.*/
      SCL_SERV_CFG *scl_serv_cfg;	/* ptr to caller structure	*/
      /* NOTE: sclDecCtrl->iedNameProc was set by _IED_SEFun on IED start.*/
      scl_serv_cfg = scl_serv_cfg_find_2 (scl_info, sclDecCtrl->iedNameProc, str);
      if (scl_serv_cfg)
        match_found = SD_TRUE;
      }
    else
      {	/* SCL_PARSE_MODE_CID (default parse mode)	*/
      if (strcmp(str, sclDecCtrl->accessPointName) == 0)
        match_found = SD_TRUE;
      }

    if (match_found)
      {	/* found caller structure with matching iedName/apName	*/
      SCL_SERVER *scl_server;
      /* Add server now.	*/
      scl_server = scl_server_add (scl_info);
      strcpy (scl_server->iedName, sclDecCtrl->iedNameProc);
      strncpy_safe (scl_server->apName, str, MAX_IDENT_LEN);
      /* CRITICAL: copy info from "Services" section saved in sclDecCtrl.	*/
      /* This should copy entire structure.	*/
      scl_server->scl_services = sclDecCtrl->scl_services;
      SXLOG_CDEC1 ("SCL PARSE: AccessPoint 'name' match found: %s", str);
      sclDecCtrl->accessPointFound = SD_TRUE;	/*NOTE: only get here if IED also found*/
      sclDecCtrl->accessPointMatched = SD_TRUE;
      sx_push (sxDecCtrl, sizeof(AccessPointElements)/sizeof(SX_ELEMENT), AccessPointElements, SD_FALSE);
      }
    else
      {
      SXLOG_CDEC1 ("SCL PARSE: AccessPoint 'name' found (%s), not a match", str);
      }
    /* end required attributes */
    }
  else
    {	/* SX_ELEMENT_END	*/
    if (sclDecCtrl->accessPointMatched == SD_TRUE)
      {
      sclDecCtrl->accessPointMatched = SD_FALSE;
      sx_pop (sxDecCtrl);
      }
    }
  }

/************************************************************************/
/*			_Server_SEFun					*/
/************************************************************************/

static ST_VOID _Server_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sx_push (sxDecCtrl, sizeof(ServerElements)/sizeof(SX_ELEMENT), ServerElements, SD_FALSE);
    }
  else
    sx_pop (sxDecCtrl);
  }

/************************************************************************/
/*			_LDevice_SEFun					*/
/************************************************************************/
static ST_VOID _LDevice_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_INFO *scl_info;
SCL_LD *scl_ld;
ST_CHAR *desc;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

    scl_ld = sclDecCtrl->scl_ld = scl_ld_create (sclDecCtrl->sclInfo);
    if (scl_ld == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start optional attributes */
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
    if (ret == SD_SUCCESS)
      scl_ld->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
    /* end optional attributes */

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "inst", scl_ld->inst, (sizeof(scl_ld->inst)-1), required);
    /* If required attributes parsed OK, make sure "inst" is legal.*/
    if (ret == SD_SUCCESS)
      {
      ret = chk_mms_ident_legal (scl_ld->inst);
      if (ret)
        SXLOG_ERR1 ("Illegal character in LDevice inst '%s'", scl_ld->inst);
      }
    if (ret != SD_SUCCESS)
      {
      /* Error on required attribute, so stop parsing.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }
    /* end required attributes */

    sx_push (sxDecCtrl, sizeof(LDeviceElements)/sizeof(SX_ELEMENT), LDeviceElements, SD_FALSE);
    }
  else
    {		/* reason == SX_ELEMENT_END	*/
    sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
    scl_info = sclDecCtrl->sclInfo;
    scl_ld = sclDecCtrl->scl_ld;
    /* Construct MMS Domain name from scl info.	*/
    /* ASSUME nameStructure="IEDName" (domain name = IED name + LDevice inst)*/
    /* nameStructure="FuncName" is OBSOLETE.				*/
    if (strlen(scl_info->serverHead->iedName) + strlen(scl_ld->inst) <= MAX_IDENT_LEN)
      {
      strcpy (scl_ld->domName, scl_info->serverHead->iedName);
      strncat_maxstrlen (scl_ld->domName, scl_ld->inst, MAX_IDENT_LEN);
      }
    else
      {
      SXLOG_ERR0 ("Cannot create LD: constructed domain name too long");
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_LN_SEFun					*/
/************************************************************************/

static ST_VOID _LN_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_LN *scl_ln;
ST_CHAR *desc;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

    scl_ln = sclDecCtrl->scl_ln = scl_ln_add (sclDecCtrl->sclInfo);
    if (scl_ln == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start optional attributes */
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
    if (ret == SD_SUCCESS)
      scl_ln->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
    ret = scl_get_attr_copy (sxDecCtrl, "prefix", scl_ln->prefix, (sizeof(scl_ln->prefix)-1), required);
    /* end optional attributes */

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "lnType", scl_ln->lnType, (sizeof(scl_ln->lnType)-1), required);
    ret |= scl_get_attr_copy (sxDecCtrl, "inst", scl_ln->inst, (sizeof(scl_ln->inst)-1), required);
    ret |= scl_get_attr_copy (sxDecCtrl, "lnClass", scl_ln->lnClass, (sizeof(scl_ln->lnClass)-1), required);

    /* If required attributes parsed OK, make sure "lnClass" and "inst" contain all legal characters.*/
    if (ret == SD_SUCCESS)
      {
      ret = chk_comp_name_legal (scl_ln->lnClass);
      if (ret)
        SXLOG_ERR1 ("Illegal character in LN lnClass '%s'", scl_ln->lnClass);
      }
    if (ret == SD_SUCCESS)
      {
      ret = chk_comp_name_legal (scl_ln->inst);
      if (ret)
        SXLOG_ERR1 ("Illegal character in LN inst '%s'", scl_ln->inst);
      }
    if (ret != SD_SUCCESS)
      {
      /* Error on required attribute, so stop parsing.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }

    if (stricmp(sxDecCtrl->sxDecElInfo.tag, "LN0") == 0 && 
	stricmp(scl_ln->lnClass, "LLN0") != 0)
      {
      sxDecCtrl->errCode = SX_USER_ERROR;
      sxDecCtrl->termFlag = SD_TRUE;
      SXLOG_ERR0 ("SCL PARSE: Attribute 'lnClass' of element 'LN0' has a value other then 'LLN0' (schema violation).");
      return;
      }
    /* end required attributes */

    if (stricmp(sxDecCtrl->sxDecElInfo.tag, "LN0") == 0)
      sx_push (sxDecCtrl, sizeof(LN0Elements)/sizeof(SX_ELEMENT), LN0Elements, SD_FALSE);
    else
      sx_push (sxDecCtrl, sizeof(LNElements)/sizeof(SX_ELEMENT), LNElements, SD_FALSE);
    }
  else
    {		/* reason == SX_ELEMENT_END	*/
    sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
    scl_ln = sclDecCtrl->scl_ln;
    /* Construct MMS Variable name from scl info.	*/
    //MEMO:  [2016-8-29 9:51 ÉÛ¿­Ìï]if (strlen (scl_ln->lnClass) != 4)
	if (strlen (scl_ln->lnClass) < 1)
      {
      SXLOG_ERR1 ("Illegal lnClass='%s'. Must be exactly 4 char",
                scl_ln->lnClass);
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    else if (strlen (scl_ln->prefix) + strlen (scl_ln->inst) > 11)
      {
      /* NOTE: standard only allows max=7, but we want to be more forgiving.*/
      SXLOG_ERR3 ("Illegal definition for lnClass='%s': prefix (%s) plus inst (%s) > 11 char.",
                scl_ln->lnClass, scl_ln->prefix, scl_ln->inst);
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    else
      {
      strcpy (scl_ln->varName, scl_ln->prefix);
      strncat_maxstrlen (scl_ln->varName, scl_ln->lnClass, MAX_IDENT_LEN);
      strncat_maxstrlen (scl_ln->varName, scl_ln->inst, MAX_IDENT_LEN);
      }
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_DataSet_SEFun					*/
/************************************************************************/

static ST_VOID _DataSet_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_DATASET *scl_dataset;
ST_CHAR *desc;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

    scl_dataset = scl_dataset_add (sclDecCtrl->sclInfo);
    if (scl_dataset == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start optional attributes */
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
    if (ret == SD_SUCCESS)
      scl_dataset->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
    /* end optional attributes */

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "name", scl_dataset->name, (sizeof(scl_dataset->name)-1), required);
    /* If required attributes parsed OK, make sure "name" is legal.*/
    if (ret == SD_SUCCESS)
      {
      ret = chk_mms_ident_legal (scl_dataset->name);
      if (ret)
        SXLOG_ERR1 ("Illegal character in DataSet name '%s'", scl_dataset->name);
      }
    if (ret != SD_SUCCESS)
      {
      /* Error on required attribute, so stop parsing.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }
    /* end required attributes */

    sx_push (sxDecCtrl, sizeof(DataSetElements)/sizeof(SX_ELEMENT), DataSetElements, SD_FALSE);
    }
  else
    sx_pop (sxDecCtrl);
  }

/************************************************************************/
/*			_FCDA_SFun					*/
/************************************************************************/

static ST_VOID _FCDA_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_INFO *scl_info;
SCL_FCDA *scl_fcda;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_info = sclDecCtrl->sclInfo;

  scl_fcda = scl_fcda_add (scl_info);
  if (scl_fcda == NULL)
    {
    scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
    return;
    }

  /* start optional attributes */
  ret = scl_get_attr_copy (sxDecCtrl, "ldInst",  scl_fcda->ldInst,  (sizeof(scl_fcda->ldInst)-1), required);
  ret = scl_get_attr_copy (sxDecCtrl, "prefix",  scl_fcda->prefix,  (sizeof(scl_fcda->prefix)-1), required);
  ret = scl_get_attr_copy (sxDecCtrl, "lnInst",  scl_fcda->lnInst,  (sizeof(scl_fcda->lnInst)-1), required);
  ret = scl_get_attr_copy (sxDecCtrl, "lnClass", scl_fcda->lnClass, (sizeof(scl_fcda->lnClass)-1), required);
  ret = scl_get_attr_copy (sxDecCtrl, "doName",  scl_fcda->doName,  (sizeof(scl_fcda->doName)-1), required);
  ret = scl_get_attr_copy (sxDecCtrl, "daName",  scl_fcda->daName,  (sizeof(scl_fcda->daName)-1), required);
  /* NOTE: "ix" should be present only in Edition 2 SCL files.	*/
  ret = scl_get_attr_copy (sxDecCtrl, "ix",      scl_fcda->ix,      (sizeof(scl_fcda->ix)-1), required);
  /* end optional attributes */

  /* start required attributes */
  required = SD_TRUE;
  ret = scl_get_attr_copy (sxDecCtrl, "fc", scl_fcda->fc, (sizeof(scl_fcda->fc)-1), required);
  if (ret != SD_SUCCESS)
    return;
  /* end required attributes */

  /* Construct domain name from SCL info	*/
  /* ASSUME nameStructure="IEDName" (domain name = IED name + LDevice inst)*/
  /* nameStructure="FuncName" is OBSOLETE.				*/
  if (strlen(scl_info->serverHead->iedName) + strlen(scl_fcda->ldInst) <= MAX_IDENT_LEN)
    {
    strcpy (scl_fcda->domName, scl_info->serverHead->iedName);
    strncat_maxstrlen (scl_fcda->domName, scl_fcda->ldInst, MAX_IDENT_LEN);
    }
  else
    {
    SXLOG_ERR0 ("Cannot add FCDA: constructed domain name too long");
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    }
  }

/************************************************************************/
/*			_ReportControl_SEFun				*/
/************************************************************************/

static ST_VOID _ReportControl_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *str;	/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
ST_CHAR *desc;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Alloc struct, save ptr in sclDecCtrl, & set local ptr to it.	*/
    SCL_RCB *scl_rcb = sclDecCtrl->scl_rcb = scl_rcb_add (sclDecCtrl->sclInfo);
    if (scl_rcb == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }
    
    /* start optional attributes */
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
    if (ret == SD_SUCCESS)
      scl_rcb->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
    ret = scl_get_attr_copy (sxDecCtrl, "datSet", scl_rcb->datSet, (sizeof(scl_rcb->datSet)-1), required);
    ret = scl_get_uint_attr (sxDecCtrl, "intgPd", &scl_rcb->intgPd, required);
    ret = scl_get_uint_attr (sxDecCtrl, "bufTime", &scl_rcb->bufTime, required);
    ret = scl_get_attr_ptr (sxDecCtrl, "buffered", &str, required);

    scl_rcb->buffered = SD_FALSE;  /* default */
    if (ret == SD_SUCCESS)
      {
      if (stricmp(str, "true") == 0)
        scl_rcb->buffered = SD_TRUE;
      }
    /* NOTE: we only accept default value of indexed="true".	*/
    ret = scl_get_attr_ptr (sxDecCtrl, "indexed", &str, required);
    if (ret == SD_SUCCESS && stricmp(str, "false") == 0)
      {
      SXLOG_ERR0 ("ReportControl attribute indexed='false' not supported");
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }
    ret = scl_get_attr_copy (sxDecCtrl, "rptID", scl_rcb->rptID, (sizeof(scl_rcb->rptID)-1), required);
    /* end optional attributes */
    
    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "name", scl_rcb->name, (sizeof(scl_rcb->name)-1), required);
    ret |= scl_get_uint_attr (sxDecCtrl, "confRev", &scl_rcb->confRev, required);
    /* If required attributes parsed OK, make sure "name" is legal.*/
    if (ret == SD_SUCCESS)
      {
      ret = chk_comp_name_legal (scl_rcb->name);
      if (ret)
        SXLOG_ERR1 ("Illegal character in ReportControl name '%s'", scl_rcb->name);
      }
    if (ret != SD_SUCCESS)
      {
      /* Error on required attribute, so stop parsing.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }
    /* end required attributes */

    sx_push (sxDecCtrl, sizeof(ReportControlElements)/sizeof(SX_ELEMENT), ReportControlElements, SD_FALSE);
    }
  else /* reason = SX_ELEMENT_END */
    {
    /* CRITICAL: Copy TrgOps to scl_rcb.	*/
    sclDecCtrl->scl_rcb->TrgOps[0] = sclDecCtrl->TrgOps[0];
    /* If "RptEnabled max" not configured, set default value*/
    if (sclDecCtrl->scl_rcb->maxClient == 0)
      sclDecCtrl->scl_rcb->maxClient = 1;	/* default	*/
    /* NOTE: scl_rcb is all filled in now	*/
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_LogControl_SEFun				*/
/************************************************************************/

static ST_VOID _LogControl_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *str;	/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
ST_CHAR *desc;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Alloc struct, save ptr in sclDecCtrl, & set local ptr to it.	*/
    SCL_LCB *scl_lcb = sclDecCtrl->scl_lcb = scl_lcb_add (sclDecCtrl->sclInfo);
    if (scl_lcb == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }
    
    /* start optional attributes */
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
    if (ret == SD_SUCCESS)
      scl_lcb->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
    ret = scl_get_uint_attr (sxDecCtrl, "intgPd", &scl_lcb->intgPd, required);
    ret = scl_get_attr_copy (sxDecCtrl, "datSet", scl_lcb->datSet, (sizeof(scl_lcb->datSet)-1), required);
    ret = scl_get_attr_ptr (sxDecCtrl, "logEna", &str, required);
    scl_lcb->logEna = SD_FALSE;  /* default */
    if (ret == SD_SUCCESS)
      {
      if (stricmp(str, "true") == 0)
        scl_lcb->logEna = SD_TRUE;
      }

    ret = scl_get_attr_ptr (sxDecCtrl, "reasonCode", &str, required);
    scl_lcb->reasonCode = SD_FALSE;  /* default */
    if (ret == SD_SUCCESS)
      {
      if (stricmp(str, "true") == 0)
        scl_lcb->reasonCode = SD_TRUE;
      }
    /* end optional attributes */
    
    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "name", scl_lcb->name, (sizeof(scl_lcb->name)-1), required);
    ret |= scl_get_attr_copy (sxDecCtrl, "logName", scl_lcb->logName, (sizeof(scl_lcb->logName)-1), required);
    /* If required attributes parsed OK, make sure "name" is legal.*/
    if (ret == SD_SUCCESS)
      {
      ret = chk_comp_name_legal (scl_lcb->name);
      if (ret)
        SXLOG_ERR1 ("Illegal character in LogControl name '%s'", scl_lcb->name);
      }
    if (ret != SD_SUCCESS)
      {
      /* Error on required attribute, so stop parsing.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }
    /* end required attributes */

    sx_push (sxDecCtrl, sizeof(LogControlElements)/sizeof(SX_ELEMENT), LogControlElements, SD_FALSE);
    }
  else /* reason = SX_ELEMENT_END */
    {
    /* CRITICAL: Copy TrgOps to scl_lcb.	*/
    sclDecCtrl->scl_lcb->TrgOps[0] = sclDecCtrl->TrgOps[0];
    /* NOTE: scl_lcb is all filled in now	*/
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_GSEControl_SFun				*/
/************************************************************************/

static ST_VOID _GSEControl_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
ST_CHAR *type;	/* ptr set by scl_get_attr_ptr	*/
SCL_GCB *scl_gcb;
ST_CHAR *desc;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  scl_gcb = scl_gcb_add (sclDecCtrl->sclInfo);
  if (scl_gcb == NULL)
    {
    scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
    return;
    }

  /* start optional attributes */
  ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
  if (ret == SD_SUCCESS)
    scl_gcb->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
  ret = scl_get_uint_attr (sxDecCtrl, "confRev", &scl_gcb->confRev, required);
  ret = scl_get_attr_copy (sxDecCtrl, "datSet", scl_gcb->datSet, (sizeof(scl_gcb->datSet)-1), required);
  ret = scl_get_attr_ptr (sxDecCtrl, "type", &type, required);
  if (ret == SD_SUCCESS && strcmp(type, "GSSE") == 0)
    scl_gcb->isGoose = SD_FALSE;
  else
    scl_gcb->isGoose = SD_TRUE;
  /* end optional attributes */
    
  /* start required attributes */
  required = SD_TRUE;
  ret = scl_get_attr_copy  (sxDecCtrl, "name",   scl_gcb->name,   (sizeof(scl_gcb->name)-1), required);
  ret |= scl_get_attr_copy (sxDecCtrl, "appID",  scl_gcb->appID,  (sizeof(scl_gcb->appID)-1), required);
  /* If required attributes parsed OK, make sure "name" is legal.*/
  if (ret == SD_SUCCESS)
    {
    ret = chk_comp_name_legal (scl_gcb->name);
    if (ret)
      SXLOG_ERR1 ("Illegal character in GSEControl name '%s'", scl_gcb->name);
    }
  if (ret != SD_SUCCESS)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  /* end required attributes */
  }

/************************************************************************/
/*			_SettingControl_SFun				*/
/************************************************************************/

static ST_VOID _SettingControl_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required;
SCL_SGCB *scl_sgcb;
ST_CHAR *desc;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  scl_sgcb = scl_sgcb_add (sclDecCtrl->sclInfo);
  if (scl_sgcb == NULL)
    {
    scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
    return;
    }

  /* start optional attributes */
  required = SD_FALSE;
  ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
  if (ret == SD_SUCCESS)
    scl_sgcb->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
  ret = scl_get_uint_attr (sxDecCtrl, "actSG", &scl_sgcb->actSG, required);
  if (ret)
    scl_sgcb->actSG = 1;  /* default value */
  /* end optional attributes */
    
  /* start required attributes */
  required = SD_TRUE;
  ret = scl_get_uint_attr (sxDecCtrl, "numOfSGs", &scl_sgcb->numOfSGs, required);
  /* end required attributes */
  }

/************************************************************************/
/*			_TrgOps_SFun					*/
/* Save all TrgOps bits in sclDecCtrl->TrgOps.				*/
/************************************************************************/

static ST_VOID _TrgOps_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *str;	/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  
  sclDecCtrl->TrgOps[0] = 0;	/* Start with all bits=0	*/

  /* start optional attributes */
  ret = scl_get_attr_ptr (sxDecCtrl, "dchg", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(sclDecCtrl->TrgOps, TRGOPS_BITNUM_DATA_CHANGE);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "qchg", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(sclDecCtrl->TrgOps, TRGOPS_BITNUM_QUALITY_CHANGE);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "dupd", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(sclDecCtrl->TrgOps, TRGOPS_BITNUM_DATA_UPDATE);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "period", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(sclDecCtrl->TrgOps, TRGOPS_BITNUM_INTEGRITY);
      }
    }

  /* NOTE: "gi" defaults to "true".	*/
  ret = scl_get_attr_ptr (sxDecCtrl, "gi", &str, required);
  if (ret != SD_SUCCESS  ||  stricmp(str, "true") == 0)
    {
    /* "gi" not found (default is true), or "gi" found and value is true.*/
    BSTR_BIT_SET_ON(sclDecCtrl->TrgOps, TRGOPS_BITNUM_GENERAL_INTERROGATION);
    }
  /* end optional attributes */
  }

/************************************************************************/
/*			_OptFlds_SFun					*/
/* Save all OptFlds bits in sclDecCtrl->scl_rcb->OptFlds.		*/
/************************************************************************/

static ST_VOID _OptFlds_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *str;	/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;
SCL_RCB *scl_rcb;
ST_BOOLEAN required = SD_FALSE;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_rcb = sclDecCtrl->scl_rcb;

  /* start optional attributes */
  ret = scl_get_attr_ptr (sxDecCtrl, "seqNum", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_rcb->OptFlds, OPTFLD_BITNUM_SQNUM);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "timeStamp", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_rcb->OptFlds, OPTFLD_BITNUM_TIMESTAMP);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "dataSet", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_rcb->OptFlds, OPTFLD_BITNUM_DATSETNAME);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "reasonCode", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_rcb->OptFlds, OPTFLD_BITNUM_REASON);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "dataRef", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_rcb->OptFlds, OPTFLD_BITNUM_DATAREF);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "bufOvfl", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_rcb->OptFlds, OPTFLD_BITNUM_BUFOVFL);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "entryID", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_rcb->OptFlds, OPTFLD_BITNUM_ENTRYID);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "configRef", &str, required);
  if (ret == SD_SUCCESS)
    {
    if (stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_rcb->OptFlds, OPTFLD_BITNUM_CONFREV);
      }
    }
  /* end optional attributes */
  }

/************************************************************************/
/*			_RptEnabled_SFun				*/
/* Save RptEnabled info in sclDecCtrl->scl_rcb.				*/
/************************************************************************/
static ST_VOID _RptEnabled_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;
SCL_RCB *scl_rcb;

  assert (sxDecCtrl->reason == SX_ELEMENT_START);
  scl_rcb = sclDecCtrl->scl_rcb;

  /* start optional attributes */
  ret = scl_get_uint_attr (sxDecCtrl, "max", &scl_rcb->maxClient, SCL_ATTR_OPTIONAL);
  /* If configured, check for legal value.	*/
  if (ret == SD_SUCCESS)
    {
    if (scl_rcb->maxClient <= 0 || scl_rcb->maxClient > 99)
      {
      SXLOG_ERR1 ("RptEnabled max=%d is not valid. Must be value between 1 and 99", scl_rcb->maxClient);
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      scl_rcb->maxClient = 1;	/* set to default just in case user ignores error*/
      }
    }
  }

/************************************************************************/
/*			_DOI_SEFun					*/
/************************************************************************/

static ST_VOID _DOI_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *ix;
ST_CHAR *name;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* start optional attributes */
    //ret = scl_get_attr_ptr (sxDecCtrl, "ix", &ix, required);
    //if (ret)
      ix = NULL;
    /* end optional attributes */

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_ptr (sxDecCtrl, "name", &name, required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* end required attributes */

    /* Start creation of flattened name */
    sclDecCtrl->flattened[0] = '\0';	/* CRITICAL: start with empty flatname*/
    if (construct_flattened (sclDecCtrl->flattened, sizeof(sclDecCtrl->flattened), name, ix)
        != SD_SUCCESS)
      {	/* error already logged.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }


    SXLOG_CDEC1 ("SCL PARSE: Created flattened variable: '%s'", sclDecCtrl->flattened);

    sx_push (sxDecCtrl, sizeof(DOIElements)/sizeof(SX_ELEMENT), DOIElements, SD_FALSE);    
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_SDI_SEFun					*/
/************************************************************************/

static ST_VOID _SDI_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *ix;
ST_CHAR *name;
ST_RET ret;
ST_CHAR *p;
ST_BOOLEAN required = SD_FALSE;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* start optional attributes */
    //ret = scl_get_attr_ptr (sxDecCtrl, "ix", &ix, required);
    //if (ret)
      ix = NULL;
    /* end optional attributes */

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_ptr (sxDecCtrl, "name", &name, required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* end required attributes */

    /* Continue creation of flattened name */
    if (construct_flattened (sclDecCtrl->flattened, sizeof(sclDecCtrl->flattened), name, ix)
        != SD_SUCCESS)
      {	/* error already logged.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }

    SXLOG_CDEC1 ("SCL PARSE: Appended to flattened variable: '%s'", sclDecCtrl->flattened);

    sx_push (sxDecCtrl, sizeof(SDIElements)/sizeof(SX_ELEMENT), SDIElements, SD_FALSE);    
    }
  else /* reason = SX_ELEMENT_END */
    {
    /* Remove the last item from the flattened string */
    p = strrchr(sclDecCtrl->flattened, '$');
    if (p != NULL)
      *p = 0;
    SXLOG_CDEC1 ("SCL PARSE: Removed last item from flattened variable: '%s'", sclDecCtrl->flattened);
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_DAI_SEFun					*/
/************************************************************************/

static ST_VOID _DAI_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *ix;
ST_CHAR *name;
ST_RET ret;
ST_CHAR *p;
ST_BOOLEAN required = SD_FALSE;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SCL_DAI *scl_dai;
    if ((scl_dai = sclDecCtrl->scl_dai = scl_dai_add (sclDecCtrl->sclInfo)) == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start optional attributes */
    //ret = scl_get_attr_ptr (sxDecCtrl, "ix", &ix, required);
    //if (ret)
      ix = NULL;
    ret = scl_get_attr_copy (sxDecCtrl, "sAddr", scl_dai->sAddr, (sizeof(scl_dai->sAddr)-1), required);
    ret = scl_get_attr_copy (sxDecCtrl, "valKind", scl_dai->valKind, (sizeof(scl_dai->valKind)-1), required);
    if (ret)
      strcpy (scl_dai->valKind, "Set"); /* default */
    /* end optional attributes */

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_ptr (sxDecCtrl, "name", &name, required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* end required attributes */

    /* Continue creation of flattened name */
    if (construct_flattened (sclDecCtrl->flattened, sizeof(sclDecCtrl->flattened), name, ix)
        != SD_SUCCESS)
      {	/* error already logged.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }

    SXLOG_CDEC1 ("SCL PARSE: Appended to flattened variable: '%s'", sclDecCtrl->flattened);
    strcpy (scl_dai->flattened, sclDecCtrl->flattened);
    sx_push (sxDecCtrl, sizeof(DAIElements)/sizeof(SX_ELEMENT), DAIElements, SD_FALSE);    
    }
  else /* reason = SX_ELEMENT_END */
    {
    /* Remove the last item from the flattened string */
    p = strrchr(sclDecCtrl->flattened, '$');
    if (p != NULL)
      *p = 0;
    SXLOG_CDEC1 ("SCL PARSE: Removed last item from flattened variable: '%s'", sclDecCtrl->flattened);
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_DAI_Val_SEFun					*/
/* Sets "sclDecCtrl->scl_dai->Val" OR adds entry to the linked list	*/
/* "sclDecCtrl->scl_dai->sgValHead".					*/
/* NOTE: sclDecCtrl->sGroupTmp is set when reason == SX_ELEMENT_START	*/
/*       and used when reason == SX_ELEMENT_END.			*/
/************************************************************************/

static ST_VOID _DAI_Val_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;
SCL_SG_VAL *scl_sg_val;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sclDecCtrl->sGroupTmp = 0;	/* Default: sGroup attr NOT present	*/
    /* start optional attributes (don't care about return)	*/
    scl_get_uint_attr (sxDecCtrl, "sGroup", &sclDecCtrl->sGroupTmp, SCL_ATTR_OPTIONAL);
    /* end optional attributes */
    }
  else /* reason = SX_ELEMENT_END */
    {
    ST_INT strLen;
    ST_CHAR *Val;
    ret = sx_get_string_ptr (sxDecCtrl, &Val, &strLen);
    if (ret==SD_SUCCESS)
      {
      if (sclDecCtrl->sGroupTmp > 0)
        {	/* sGroup attr is present.	*/
        /* Add entry to linked list	*/
        scl_sg_val = scl_dai_sg_val_add (sclDecCtrl->scl_dai);
        scl_sg_val->sGroup = sclDecCtrl->sGroupTmp;
        scl_sg_val->Val = chk_strdup (Val);	/* alloc & store Val*/
        }
      else if (sclDecCtrl->scl_dai->Val)
        {
        /* DO NOT allow multiple "Val" without "sGroup": pointer	*/
        /* sclDecCtrl->scl_dai->Val would get overwritten, and never freed.*/
        SXLOG_ERR0 ("Multiple 'Val' elements without 'sGroup' not allowed in DAI");
        //MEMO: del by skt ÈÝ´í [2016-8-18 15:10 ÉÛ¿­Ìï] scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
        }
      else
        sclDecCtrl->scl_dai->Val = chk_strdup (Val);	/* alloc & store Val*/
      }
    else
      {
      SXLOG_ERR0 ("Error parsing element 'Val' of DAI");
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    }
  }

/************************************************************************/
/*			_DataTypeTemplates_SEFun			*/
/************************************************************************/

static ST_VOID _DataTypeTemplates_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    sx_push (sxDecCtrl, sizeof(DataTypeTemplatesElements)/sizeof(SX_ELEMENT), DataTypeTemplatesElements, SD_FALSE);    
  else
    sx_pop (sxDecCtrl);
  }

/************************************************************************/
/*			_LNodeType_SEFun				*/
/************************************************************************/

static ST_VOID _LNodeType_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_LNTYPE *scl_lntype;
SCL_INFO *scl_info;
ST_CHAR iedType[MAX_IDENT_LEN+1];	/* optional iedType attr, if found*/

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_info = sclDecCtrl->sclInfo;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Assume iedType matched. Clear this below if match not found.	*/
    sclDecCtrl->iedTypeMatched = SD_TRUE;

    /* IMPORTANT: For "SCD" parse mode, check the optional iedType,	*/
    /*            if present, BEFORE saving anything.			*/
    ret = scl_get_attr_copy (sxDecCtrl, "iedType", iedType, (sizeof(iedType)-1), required);
    if (ret == SD_SUCCESS && iedType[0] != '\0')	/* iedType is not empty string	*/
      {
      if (sclDecCtrl->parseMode == SCL_PARSE_MODE_SCD_FILTERED)
        {
        /* "SCD" parse mode AND "iedType" is present. Check for match.*/
        if (!scl_iedtype_match (scl_info, iedType))
          {	/* iedType DOES NOT match one requested	*/
          SXLOG_DEC1 ("LNodeType ignored: iedType='%s' is not in list passed to parser", iedType);
          sclDecCtrl->iedTypeMatched = SD_FALSE;	/* IGNORE THIS TYPE.	*/
          return;
          }
        }
      }

    scl_lntype = scl_lntype_create (sclDecCtrl->sclInfo);
    if (scl_lntype == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "id", scl_lntype->id, (sizeof(scl_lntype->id)-1), required);
    ret |= scl_get_attr_copy (sxDecCtrl, "lnClass", scl_lntype->lnClass, (sizeof(scl_lntype->lnClass)-1), required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* end required attributes */

    sx_push (sxDecCtrl, sizeof(LNodeTypeElements)/sizeof(SX_ELEMENT), LNodeTypeElements, SD_FALSE);    
    }
  else
    {
    if (sclDecCtrl->iedTypeMatched)
      sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_DO_SFun					*/
/************************************************************************/

static ST_VOID _DO_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_DO *scl_do;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  scl_do = scl_lntype_add_do (sclDecCtrl->sclInfo);
  if (scl_do == NULL)
    {
    scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
    return;
    }

  /* start required attributes */
  required = SD_TRUE;
  ret = scl_get_attr_copy (sxDecCtrl, "name", scl_do->name, (sizeof(scl_do->name)-1), required);
  ret |= scl_get_attr_copy (sxDecCtrl, "type", scl_do->type, (sizeof(scl_do->type)-1), required);
  /* If required attributes parsed OK, make sure "name" is legal.*/
  if (ret == SD_SUCCESS)
    {
    ret = chk_comp_name_legal (scl_do->name);
    if (ret)
      SXLOG_ERR1 ("Illegal character in DO name '%s'", scl_do->name);
    }
  if (ret != SD_SUCCESS)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  /* end required attributes */
  }

/************************************************************************/
/*			_DOType_SEFun					*/
/************************************************************************/

static ST_VOID _DOType_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_DOTYPE *scl_dotype;
SCL_INFO *scl_info;
ST_CHAR iedType[MAX_IDENT_LEN+1];	/* optional iedType attr, if found*/

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_info = sclDecCtrl->sclInfo;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Assume iedType matched. Clear this below if match not found.	*/
    sclDecCtrl->iedTypeMatched = SD_TRUE;

    /* IMPORTANT: For "SCD" parse mode, check the optional iedType,	*/
    /*            if present, BEFORE saving anything.			*/
    ret = scl_get_attr_copy (sxDecCtrl, "iedType", iedType, (sizeof(iedType)-1), required);
    if (ret == SD_SUCCESS && iedType[0] != '\0')	/* iedType is not empty string	*/
      {
      if (sclDecCtrl->parseMode == SCL_PARSE_MODE_SCD_FILTERED)
        {
        /* "SCD" parse mode AND "iedType" is present. Check for match.*/
        if (!scl_iedtype_match (scl_info, iedType))
          {	/* iedType DOES NOT match one requested	*/
          SXLOG_DEC1 ("DOType ignored: iedType='%s' is not in list passed to parser", iedType);
          sclDecCtrl->iedTypeMatched = SD_FALSE;	/* IGNORE THIS TYPE.	*/
          return;
          }
        }
      }

    scl_dotype = scl_dotype_create (sclDecCtrl->sclInfo);
    if (scl_dotype == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "id", scl_dotype->id, (sizeof(scl_dotype->id)-1), required);
    ret |= scl_get_attr_copy (sxDecCtrl, "cdc", scl_dotype->cdc, (sizeof(scl_dotype->cdc)-1), required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* end required attributes */

    sx_push (sxDecCtrl, sizeof(DOTypeElements)/sizeof(SX_ELEMENT), DOTypeElements, SD_FALSE);    
    }
  else
    {
    if (sclDecCtrl->iedTypeMatched)
      sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_DA_SEFun					*/
/************************************************************************/

static ST_VOID _DA_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SCL_DEC_CTRL *sclDecCtrl;
    ST_CHAR *str;	/* use for dchg, qchg, dupd	*/
    ST_RET ret;
    ST_BOOLEAN required = SD_FALSE;
    SCL_DA *scl_da;
    ST_CHAR *desc;

    sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

    scl_da = sclDecCtrl->scl_da = scl_dotype_add_da (sclDecCtrl->sclInfo);
    if (scl_da == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }
  
    /* start optional attributes */
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
    if (ret == SD_SUCCESS)
      scl_da->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
    ret = scl_get_attr_copy (sxDecCtrl, "sAddr", scl_da->sAddr, (sizeof(scl_da->sAddr)-1), required);
    ret = scl_get_attr_copy (sxDecCtrl, "valKind", scl_da->valKind, (sizeof(scl_da->valKind)-1), required);
    if (ret)
      strcpy (scl_da->valKind, "Set"); /* default */
    ret = scl_get_attr_copy (sxDecCtrl, "type", scl_da->type, (sizeof(scl_da->type)-1), required);
    ret = scl_get_uint_attr (sxDecCtrl, "count", &scl_da->count, required);
    ret = scl_get_attr_ptr (sxDecCtrl, "dchg", &str, required);
    if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
      scl_da->dchg = SD_TRUE;
  
    ret = scl_get_attr_ptr (sxDecCtrl, "qchg", &str, required);
    if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
      scl_da->qchg = SD_TRUE;
  
    ret = scl_get_attr_ptr (sxDecCtrl, "dupd", &str, required);
    if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
      scl_da->dupd = SD_TRUE;
    /* end optional attributes */
  
    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "name", scl_da->name, (sizeof(scl_da->name)-1), required);
    ret |= scl_get_attr_copy (sxDecCtrl, "bType", scl_da->bType, (sizeof(scl_da->bType)-1), required);
    ret |= scl_get_attr_copy (sxDecCtrl, "fc", scl_da->fc, (sizeof(scl_da->fc)-1), required);
    /* If required attributes parsed OK, make sure "name" is legal.*/
    if (ret == SD_SUCCESS)
      {
      ret = chk_comp_name_legal (scl_da->name);
      if (ret)
        SXLOG_ERR1 ("Illegal character in DA name '%s'", scl_da->name);
      }
    if (ret != SD_SUCCESS)
      {
      /* Error on required attribute, so stop parsing.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }
    /* end required attributes */
    sx_push (sxDecCtrl, sizeof(DAElements)/sizeof(SX_ELEMENT), DAElements, SD_FALSE);
    }
  else
    sx_pop (sxDecCtrl);
  }

/************************************************************************/
/*			_SDO_SFun					*/
/************************************************************************/

static ST_VOID _SDO_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_DA *scl_da;
ST_CHAR *desc;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  scl_da = scl_dotype_add_sdo (sclDecCtrl->sclInfo);
  if (scl_da == NULL)
    {
    scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
    return;
    }

  /* start optional attributes */
  ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
  if (ret == SD_SUCCESS)
    scl_da->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
  /* NOTE: "count" should be present only in Edition 2 SCL files.	*/
  scl_get_uint_attr (sxDecCtrl, "count", &scl_da->count, required);
  /* end optional attributes */

  /* start required attributes */
  required = SD_TRUE;
  ret = scl_get_attr_copy (sxDecCtrl, "name", scl_da->name, (sizeof(scl_da->name)-1), required);
  ret |= scl_get_attr_copy (sxDecCtrl, "type", scl_da->type, (sizeof(scl_da->type)-1), required);
  /* If required attributes parsed OK, make sure "name" is legal.*/
  if (ret == SD_SUCCESS)
    {
    ret = chk_comp_name_legal (scl_da->name);
    if (ret)
      SXLOG_ERR1 ("Illegal character in SDO name '%s'", scl_da->name);
    }
  if (ret != SD_SUCCESS)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  /* end required attributes */
  }

/************************************************************************/
/*			_DA_Val_SEFun					*/
/* Sets "sclDecCtrl->scl_da->Val" OR adds entry to the linked list	*/
/* "sclDecCtrl->scl_da->sgValHead".					*/
/* NOTE: sclDecCtrl->sGroupTmp is set when reason == SX_ELEMENT_START	*/
/*       and used when reason == SX_ELEMENT_END.			*/
/************************************************************************/

static ST_VOID _DA_Val_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;
SCL_SG_VAL *scl_sg_val;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sclDecCtrl->sGroupTmp = 0;	/* Default: sGroup attr NOT present	*/
    /* start optional attributes (don't care about return)	*/
    scl_get_uint_attr (sxDecCtrl, "sGroup", &sclDecCtrl->sGroupTmp, SCL_ATTR_OPTIONAL);
    /* end optional attributes */
    }
  else /* reason = SX_ELEMENT_END */
    {
    ST_INT strLen;
    ST_CHAR *Val;
    ret = sx_get_string_ptr (sxDecCtrl, &Val, &strLen);
    if (ret==SD_SUCCESS)
      {
      if (sclDecCtrl->sGroupTmp > 0)
        {	/* sGroup attr is present.	*/
        /* Add entry to linked list	*/
        scl_sg_val = scl_da_sg_val_add (sclDecCtrl->scl_da);
        scl_sg_val->sGroup = sclDecCtrl->sGroupTmp;
        scl_sg_val->Val = chk_strdup (Val);	/* alloc & store Val*/
        }
      else
        sclDecCtrl->scl_da->Val = chk_strdup (Val);	/* alloc & store Val*/
      }
    else
      {
      SXLOG_ERR0 ("Error parsing element 'Val' of DA");
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    }
  }

/************************************************************************/
/*			_DAType_SEFun					*/
/************************************************************************/

static ST_VOID _DAType_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_DATYPE *scl_datype;
SCL_INFO *scl_info;
ST_CHAR iedType[MAX_IDENT_LEN+1];	/* optional iedType attr, if found*/

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_info = sclDecCtrl->sclInfo;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Assume iedType matched. Clear this below if match not found.	*/
    sclDecCtrl->iedTypeMatched = SD_TRUE;

    /* IMPORTANT: For "SCD" parse mode, check the optional iedType,	*/
    /*            if present, BEFORE saving anything.			*/
    ret = scl_get_attr_copy (sxDecCtrl, "iedType", iedType, (sizeof(iedType)-1), required);
    if (ret == SD_SUCCESS && iedType[0] != '\0')	/* iedType is not empty string	*/
      {
      if (sclDecCtrl->parseMode == SCL_PARSE_MODE_SCD_FILTERED)
        {
        /* "SCD" parse mode AND "iedType" is present. Check for match.*/
        if (!scl_iedtype_match (scl_info, iedType))
          {	/* iedType DOES NOT match one requested	*/
          SXLOG_DEC1 ("DAType ignored: iedType='%s' is not in list passed to parser", iedType);
          sclDecCtrl->iedTypeMatched = SD_FALSE;	/* IGNORE THIS TYPE.	*/
          return;
          }
        }
      }

    scl_datype = scl_datype_create (sclDecCtrl->sclInfo);
    if (scl_datype == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "id", scl_datype->id, (sizeof(scl_datype->id)-1), required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* end required attributes */

    sx_push (sxDecCtrl, sizeof(DATypeElements)/sizeof(SX_ELEMENT), DATypeElements, SD_FALSE);    
    }
  else
    {
    if (sclDecCtrl->iedTypeMatched)
      sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_BDA_SEFun					*/
/************************************************************************/

static ST_VOID _BDA_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SCL_DEC_CTRL *sclDecCtrl;
    ST_RET ret;
    ST_BOOLEAN required = SD_FALSE;
    SCL_BDA *scl_bda;
    ST_CHAR *desc;

    sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

    scl_bda = sclDecCtrl->scl_bda = scl_datype_add_bda (sclDecCtrl->sclInfo);
    if (scl_bda == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start optional attributes */
    ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, required);
    if (ret == SD_SUCCESS)
      scl_bda->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
    ret = scl_get_attr_copy (sxDecCtrl, "sAddr", scl_bda->sAddr, (sizeof(scl_bda->sAddr)-1), required);
    ret = scl_get_attr_copy (sxDecCtrl, "valKind", scl_bda->valKind, (sizeof(scl_bda->valKind)-1), required);
    if (ret)
      strcpy (scl_bda->valKind, "Set"); /* default */
    ret = scl_get_attr_copy (sxDecCtrl, "type", scl_bda->type, (sizeof(scl_bda->type)-1), required);
    ret = scl_get_uint_attr (sxDecCtrl, "count", &scl_bda->count, required);
    /* end optional attributes */

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "name", scl_bda->name, (sizeof(scl_bda->name)-1), required);
    ret |= scl_get_attr_copy (sxDecCtrl, "bType", scl_bda->bType, (sizeof(scl_bda->bType)-1), required);
  /* If required attributes parsed OK, make sure "name" is legal.*/
    if (ret == SD_SUCCESS)
      {
      ret = chk_comp_name_legal (scl_bda->name);
      if (ret)
        SXLOG_ERR1 ("Illegal character in BDA name '%s'", scl_bda->name);
      }
    if (ret != SD_SUCCESS)
      {
      /* Error on required attribute, so stop parsing.	*/
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      return;
      }
    /* end required attributes */
    sx_push (sxDecCtrl, sizeof(BDAElements)/sizeof(SX_ELEMENT), BDAElements, SD_FALSE);
    }
  else
    sx_pop (sxDecCtrl);
  }

/************************************************************************/
/*			_BDA_Val_SEFun					*/
/* Sets "sclDecCtrl->scl_bda->Val" OR adds entry to the linked list	*/
/* "sclDecCtrl->scl_bda->sgValHead".					*/
/* NOTE: sclDecCtrl->sGroupTmp is set when reason == SX_ELEMENT_START	*/
/*       and used when reason == SX_ELEMENT_END.			*/
/************************************************************************/

static ST_VOID _BDA_Val_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;
SCL_SG_VAL *scl_sg_val;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sclDecCtrl->sGroupTmp = 0;	/* Default: sGroup attr NOT present	*/
    /* start optional attributes (don't care about return)	*/
    scl_get_uint_attr (sxDecCtrl, "sGroup", &sclDecCtrl->sGroupTmp, SCL_ATTR_OPTIONAL);
    /* end optional attributes */
    }
  else /* reason = SX_ELEMENT_END */
    {
    ST_INT strLen;
    ST_CHAR *Val;
    ret = sx_get_string_ptr (sxDecCtrl, &Val, &strLen);
    if (ret==SD_SUCCESS)
      {
      if (sclDecCtrl->sGroupTmp > 0)
        {	/* sGroup attr is present.	*/
        /* Add entry to linked list	*/
        scl_sg_val = scl_bda_sg_val_add (sclDecCtrl->scl_bda);
        scl_sg_val->sGroup = sclDecCtrl->sGroupTmp;
        scl_sg_val->Val = chk_strdup (Val);	/* alloc & store Val*/
        }
      else
        sclDecCtrl->scl_bda->Val = chk_strdup (Val);	/* alloc & store Val*/
      }
    else
      {
      SXLOG_ERR0 ("Error parsing element 'Val' of BDA");
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    }
  }

/************************************************************************/
/*			_EnumType_SEFun					*/
/************************************************************************/

static ST_VOID _EnumType_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_BOOLEAN required = SD_FALSE;
SCL_ENUMTYPE *scl_enumtype;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

    if ((scl_enumtype = scl_enumtype_create (sclDecCtrl->sclInfo)) == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_attr_copy (sxDecCtrl, "id", scl_enumtype->id, (sizeof(scl_enumtype->id)-1), required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* end required attributes */

    sx_push (sxDecCtrl, sizeof(EnumTypeElements)/sizeof(SX_ELEMENT), EnumTypeElements, SD_FALSE);    
    }
  else
    sx_pop (sxDecCtrl);
  }

/************************************************************************/
/*			_EnumVal_SEFun					*/
/************************************************************************/

static ST_VOID _EnumVal_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
ST_CHAR *strOut;	/* string pointer set by sx_get_string_ptr	*/
ST_INT strLen;		/* string length  set by sx_get_string_ptr	*/
ST_BOOLEAN required = SD_FALSE;
SCL_ENUMVAL *scl_enumval;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    if ((scl_enumval = sclDecCtrl->scl_enumval = scl_enumtype_add_enumval (sclDecCtrl->sclInfo)) == NULL)
      {
      scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
      return;
      }

    /* start required attributes */
    required = SD_TRUE;
    ret = scl_get_int_attr (sxDecCtrl, "ord", &scl_enumval->ord, required);
    if (ret != SD_SUCCESS)
      {
      return;
      }
    /* end required attributes */
    }
  else /* reason = SX_ELEMENT_END */
    {
    scl_enumval = sclDecCtrl->scl_enumval;
    ret = sx_get_string_ptr (sxDecCtrl, &strOut, &strLen);
    if (ret == SD_SUCCESS)
      {
      /* If string fits in EnumValBuf, it is copied there & EnumVal is set*/
      /* to point to it. Else, EnumVal is allocated & string is copied to it.*/
      if (strLen < sizeof (scl_enumval->EnumValBuf))
        {	/* fits in buf, so copy to buf & point to buf		*/
        strcpy (scl_enumval->EnumValBuf, strOut);
        scl_enumval->EnumVal = scl_enumval->EnumValBuf;
        }
      else
        {	/* DOES NOT fit in buf, so alloc EnumVal & copy string	*/
        scl_enumval->EnumVal = chk_strdup (strOut);
        /* NOTE: On exit, if EnumVal DOES NOT point to EnumValBuf,	*/
        /*       we know it was allocated here, so it must be freed.	*/
        }
      }
    else
      {
      SXLOG_ERR0 ("Error parsing element 'EnumVal'");
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    }
  }

/************************************************************************/
/*			_scl_unknown_el_start				*/
/************************************************************************/

static ST_RET _scl_unknown_el_start (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag)
  {
  SXLOG_DEC1 ("SCL PARSE: Unneeded or unknown element '%s'", tag);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_scl_unknown_el_end				*/
/************************************************************************/

static ST_RET _scl_unknown_el_end (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag)
  {
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_SampledValueControl_SEFun			*/
/* DEBUG: if parser called separate start and end functions, the lower	*/
/*   functs could be called directly & this funct would not be needed.	*/
/************************************************************************/
static ST_VOID _SampledValueControl_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    _SampledValueControl_SFun (sxDecCtrl);
  else
    _SampledValueControl_EFun (sxDecCtrl);
  }
/************************************************************************/
/*			_SampledValueControl_SFun			*/
/* Handle Start tag							*/
/************************************************************************/
static ST_VOID _SampledValueControl_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *str;		/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;
SCL_SVCB *scl_svcb;
ST_CHAR *desc;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  /* Alloc struct, save in sclDecCtrl, & set local ptr to it.	*/
  scl_svcb = sclDecCtrl->scl_svcb = scl_svcb_add (sclDecCtrl->sclInfo);
  if (scl_svcb == NULL)
    {
    scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
    return;
    }
  
  /* start required attributes */
  ret = scl_get_attr_copy (sxDecCtrl, "name", scl_svcb->name, (sizeof(scl_svcb->name)-1), SCL_ATTR_REQUIRED);
  if (sclDecCtrl->sclInfo->edition == 2)
    ret |= scl_get_attr_copy (sxDecCtrl, "smvID", scl_svcb->smvID, (sizeof(scl_svcb->smvID)-1), SCL_ATTR_REQUIRED);
  else
    /* For Edition 1, do not allow more than MVL61850_MAX_RPTID_LEN (65) characters.*/
    ret |= scl_get_attr_copy (sxDecCtrl, "smvID", scl_svcb->smvID, MVL61850_MAX_RPTID_LEN, SCL_ATTR_REQUIRED);
  ret |= scl_get_uint_attr (sxDecCtrl, "smpRate", &scl_svcb->smpRate, SCL_ATTR_REQUIRED);
  ret |= scl_get_uint_attr (sxDecCtrl, "nofASDU", &scl_svcb->nofASDU, SCL_ATTR_REQUIRED);
  /* If required attributes parsed OK, make sure "name" is legal.*/
  if (ret == SD_SUCCESS)
    {
    ret = chk_comp_name_legal (scl_svcb->name);
    if (ret)
      SXLOG_ERR1 ("Illegal character in SampledValueControl name '%s'", scl_svcb->name);
    }
  if (ret != SD_SUCCESS)
    {
    /* Error on required attribute, so stop parsing.	*/
    scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
    return;
    }
  /* end required attributes */

  /* start optional attributes */
  ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS)
    scl_svcb->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
  ret = scl_get_attr_copy (sxDecCtrl, "datSet", scl_svcb->datSet, (sizeof(scl_svcb->datSet)-1), SCL_ATTR_OPTIONAL);
  ret = scl_get_uint_attr (sxDecCtrl, "confRev", &scl_svcb->confRev, SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_ptr (sxDecCtrl, "multicast", &str, SCL_ATTR_OPTIONAL);	/* chk "str" below*/
  if (ret == SD_SUCCESS  &&  stricmp(str, "false") == 0)
    scl_svcb->multicast = SD_FALSE;
  else
    scl_svcb->multicast = SD_TRUE;  /* default value */

  /* "smpMod" is for Edition 2 only. Should never be found in Edition 1 SCL file.*/
  ret = scl_get_attr_ptr (sxDecCtrl, "smpMod", &str, SCL_ATTR_OPTIONAL);	/* chk "str" below*/
  if (ret == SD_SUCCESS)
    {	/* Convert string to Enumerated value.	*/
    if (stricmp(str, "SmpPerPeriod") == 0)
      scl_svcb->smpMod = 0;
    else if (stricmp(str, "SmpPerSec") == 0)
      scl_svcb->smpMod = 1;
    else if (stricmp(str, "SecPerSmp") == 0)
      scl_svcb->smpMod = 2;
    else
      {
      SXLOG_ERR1 ("smpMod='%s' is not allowed. Using default value 'SmpPerPeriod' (0)", str);
      /* scl_svcb calloced so smpMod is already set to default value (0).*/
      }
    }
  /* end optional attributes */
  
  sx_push (sxDecCtrl, sizeof(SampledValueControlElements)/sizeof(SX_ELEMENT), SampledValueControlElements, SD_FALSE);
  }
/************************************************************************/
/*			_SampledValueControl_EFun			*/
/* Handle End tag							*/
/************************************************************************/
static ST_VOID _SampledValueControl_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
  sx_pop (sxDecCtrl);
  }

/************************************************************************/
/*			_SmvOpts_SFun					*/
/************************************************************************/
static ST_VOID _SmvOpts_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_CHAR *str;		/* ptr set by scl_get_attr_ptr	*/
ST_RET ret;
SCL_SVCB *scl_svcb;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
  scl_svcb = sclDecCtrl->scl_svcb;
  
  /* start optional attributes */
  ret = scl_get_attr_ptr (sxDecCtrl, "sampleRate", &str, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
    {
    BSTR_BIT_SET_ON(scl_svcb->OptFlds, SVOPT_BITNUM_SMPRATE);
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "refreshTime", &str, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
    {
    BSTR_BIT_SET_ON(scl_svcb->OptFlds, SVOPT_BITNUM_REFRTM);
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "sampleSynchronized", &str, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
    {
    BSTR_BIT_SET_ON(scl_svcb->OptFlds, SVOPT_BITNUM_SMPSYNCH);
    }
  /* "sampleSynchronized" must be "true" for Edition 2.	*/
  if (sclDecCtrl->sclInfo->edition == 2 &&
      BSTR_BIT_GET(scl_svcb->OptFlds, SVOPT_BITNUM_SMPSYNCH) == 0)
    {
    SXLOG_ERR0 ("sampleSynchronized='false' not allowed for Edition 2. Automatically setting it to 'true'.");
    BSTR_BIT_SET_ON(scl_svcb->OptFlds, SVOPT_BITNUM_SMPSYNCH);
    }

  /* "dataSet" is for Edition 2 only.	*/
  if (sclDecCtrl->sclInfo->edition == 2)
    {
    ret = scl_get_attr_ptr (sxDecCtrl, "dataSet", &str, SCL_ATTR_OPTIONAL);
    if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
      {
      BSTR_BIT_SET_ON(scl_svcb->OptFlds, SVOPT_BITNUM_DATSET);
      }
    }

  ret = scl_get_attr_ptr (sxDecCtrl, "security", &str, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
    {
    scl_svcb->securityPres = SD_TRUE;	/* scl_svcb calloced so init val is FALSE*/
    if (sclDecCtrl->sclInfo->edition == 2)
      BSTR_BIT_SET_ON(scl_svcb->OptFlds, SVOPT_BITNUM_SECURITY);	/* Ed 2 only*/
    }

  /* NOTE: SCL calls this "dataRef", but 7-2 & 9-2 call it "DatSet".	*/
  /* For Edition 2, "dataRef" is not allowed. Should never be present.	*/
  /* Edition 2 uses SVOPT_BITNUM_DATSET instead (see "dataSet" attr above).*/
  ret = scl_get_attr_ptr (sxDecCtrl, "dataRef", &str, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS  &&  stricmp(str, "true") == 0)
    scl_svcb->dataRefPres = SD_TRUE;	/* scl_svcb calloced so init val is FALSE*/
  /* end optional attributes */
  }

/************************************************************************/
/*			_Inputs_SEFun					*/
/************************************************************************/
static ST_VOID _Inputs_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    sx_push (sxDecCtrl, sizeof(InputsElements)/sizeof(SX_ELEMENT), InputsElements, SD_FALSE);    
    }
  else
    {
    sx_pop (sxDecCtrl);
    }
  }

/************************************************************************/
/*			_ExtRef_SFun					*/
/************************************************************************/
static ST_VOID _ExtRef_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl;
ST_RET ret;
SCL_EXTREF *scl_extref;
ST_CHAR *desc;
ST_CHAR *intAddr;

  sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;

  if ((scl_extref = scl_extref_add (sclDecCtrl->sclInfo)) == NULL)
    {
    scl_stop_parsing (sxDecCtrl, SX_ERR_ALLOC);
    return;
    }

  /* start optional attributes */
  /* "desc" and "intAddr" are allocated pointers. The rest are fixed buffers.*/
  ret = scl_get_attr_ptr (sxDecCtrl, "desc", &desc, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS)
    scl_extref->desc = chk_strdup (desc);	/* Alloc & copy desc string	*/
  ret = scl_get_attr_ptr (sxDecCtrl, "intAddr", &intAddr, SCL_ATTR_OPTIONAL);
  if (ret == SD_SUCCESS)
    scl_extref->intAddr = chk_strdup (intAddr);	/* Alloc & copy intAddr string	*/

  ret = scl_get_attr_copy (sxDecCtrl, "iedName", scl_extref->iedName, (sizeof(scl_extref->iedName)-1), SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "ldInst",  scl_extref->ldInst,  (sizeof(scl_extref->ldInst)-1),  SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "prefix", scl_extref->prefix, (sizeof(scl_extref->prefix)-1), SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "lnClass", scl_extref->lnClass, (sizeof(scl_extref->lnClass)-1), SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "lnInst",  scl_extref->lnInst,  (sizeof(scl_extref->lnInst)-1),  SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "doName",  scl_extref->doName,  (sizeof(scl_extref->doName)-1),  SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "daName", scl_extref->daName, (sizeof(scl_extref->daName)-1), SCL_ATTR_OPTIONAL);

  ret = scl_get_attr_copy (sxDecCtrl, "serviceType", scl_extref->serviceType, (sizeof(scl_extref->serviceType)-1), SCL_ATTR_OPTIONAL);

  ret = scl_get_attr_copy (sxDecCtrl, "srcLDInst",  scl_extref->srcLDInst,  (sizeof(scl_extref->srcLDInst)-1), SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "srcPrefix",  scl_extref->srcPrefix,  (sizeof(scl_extref->srcPrefix)-1), SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "srcLNClass", scl_extref->srcLNClass, (sizeof(scl_extref->srcLNClass)-1), SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "srcLNInst",  scl_extref->srcLNInst,  (sizeof(scl_extref->srcLNInst)-1), SCL_ATTR_OPTIONAL);
  ret = scl_get_attr_copy (sxDecCtrl, "srcCBName",  scl_extref->srcCBName,  (sizeof(scl_extref->srcCBName)-1), SCL_ATTR_OPTIONAL);
  /* end optional attributes */
  }

/************************************************************************/
/*			_GSE_MinTime_SEFun				*/
/* Units must be "ms" but check "unit" and "multiplier" attributes.	*/
/************************************************************************/
static ST_VOID _GSE_MinTime_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;
ST_INT strLen;
ST_CHAR *strPtr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Check required attribute.	*/
    ret = scl_get_attr_ptr (sxDecCtrl, "unit", &strPtr, SCL_ATTR_REQUIRED);
    if (ret == SD_SUCCESS)
      {
      if (strcmp (strPtr, "s") != 0)
        SXLOG_ERR1 ("unit='%s' not allowed. Assuming unit='s'.", strPtr);
      }
    else	/* "unit" not found	*/
      return;	/* parse will stop now.	*/

    /* Check optional attribute.	*/
    ret = scl_get_attr_ptr (sxDecCtrl, "multiplier", &strPtr, SCL_ATTR_OPTIONAL);
    if (ret == SD_SUCCESS && strcmp (strPtr, "m") != 0)
      {
      SXLOG_ERR1 ("multiplier='%s' not allowed. Assuming multiplier='m'.", strPtr);
      }
    }
  else /* reason = SX_ELEMENT_END */
    {
    ret = sx_get_string_ptr (sxDecCtrl, &strPtr, &strLen);
    if (ret==SD_SUCCESS)
      {
      /* Convert string to UINT and save.	*/
      ret = asciiToUint (strPtr, &sclDecCtrl->scl_gse->MinTime);
      }
    if (ret!=SD_SUCCESS)
      {
      SXLOG_ERR0 ("Error parsing element 'MinTime' of GSE");
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    }
  }
/************************************************************************/
/*			_GSE_MaxTime_SEFun				*/
/* Units must be "ms" but check "unit" and "multiplier" attributes.	*/
/************************************************************************/
static ST_VOID _GSE_MaxTime_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
SCL_DEC_CTRL *sclDecCtrl = (SCL_DEC_CTRL *) sxDecCtrl->usr;
ST_RET ret;
ST_INT strLen;
ST_CHAR *strPtr;


  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* Check required attribute.	*/
    ret = scl_get_attr_ptr (sxDecCtrl, "unit", &strPtr, SCL_ATTR_REQUIRED);
    if (ret == SD_SUCCESS)
      {
      if (strcmp (strPtr, "s") != 0)
        SXLOG_ERR1 ("unit='%s' not allowed. Assuming unit='s'.", strPtr);
      }
    else	/* "unit" not found	*/
      return;	/* parse will stop now.	*/

    /* Check optional attribute.	*/
    ret = scl_get_attr_ptr (sxDecCtrl, "multiplier", &strPtr, SCL_ATTR_OPTIONAL);
    if (ret == SD_SUCCESS && strcmp (strPtr, "m") != 0)
      {
      SXLOG_ERR1 ("multiplier='%s' not allowed. Assuming multiplier='m'.", strPtr);
      }
    }
  else /* reason = SX_ELEMENT_END */
    {
    ret = sx_get_string_ptr (sxDecCtrl, &strPtr, &strLen);
    if (ret==SD_SUCCESS)
      {
      /* Convert string to UINT and save.	*/
      ret = asciiToUint (strPtr, &sclDecCtrl->scl_gse->MaxTime);
      }
    if (ret!=SD_SUCCESS)
      {
      SXLOG_ERR0 ("Error parsing element 'MaxTime' of GSE");
      scl_stop_parsing (sxDecCtrl, SX_USER_ERROR);
      }
    }
  }

/************************************************************************/
/*			scl_parse_scd_all				*/
/* Alternate function to parse an SCD file and create multiple "Servers"*/
/* This function can parse Edition 1 or Edition 2 SCD files.		*/
/* When done, sclInfo->serverHead will contain a list of servers found.	*/
/************************************************************************/
ST_RET scl_parse_scd_all (ST_CHAR *xmlFileName,
	SCL_OPTIONS *options,	/* miscellaneous parser options		*/
				/* may be NULL if no options needed	*/
	SCL_INFO *sclInfo)	/* main struct where all SCL info stored*/
  {
ST_RET ret;
SCL_DEC_CTRL sclDecCtrl = {0};	/* start with clean struct.	*/

  memset (sclInfo, 0, sizeof (SCL_INFO));    /* CRITICAL: start with clean struct*/

  /* Set "parseMode" to control parsing later.	*/
  sclDecCtrl.parseMode = SCL_PARSE_MODE_SCD_ALL;

  /* save parser options to use later. Used for any parse mode	*/
  if (options != NULL)
    memcpy (&sclInfo->options, options, sizeof (SCL_OPTIONS));

  sclDecCtrl.sclInfo = sclInfo;
    
  ret = sx_parseExx_mt (xmlFileName, 
          sizeof (sclStartElements)/sizeof(SX_ELEMENT), sclStartElements,
          &sclDecCtrl, _scl_unknown_el_start, _scl_unknown_el_end);

  /* NOTE: sx_parseEx_mt doesn't log error if file open fails, so log here*/
  /* It may not log some other errors, so log any other error here too.	*/
  if (ret == SX_FILE_NOT_FOUND)
    SXLOG_ERR1 ("XML File (%s) Open Error",xmlFileName);
  else if (ret != SD_SUCCESS)
    SXLOG_ERR2 ("Error 0x%X parsing SCL file (%s)", ret, xmlFileName);

  /* NOTE: ret==SD_SUCCESS if parse succeeded, but may NOT have found	*/
  /*       any Servers (i.e. sclInfo->serverHead may be NULL).		*/
  return (ret);
  }

/************************************************************************/
/*			scl_parse_scd_filtered				*/
/* Alternate function to parse an SCD file and create multiple "Servers"*/
/* but filter on selected Servers and selected iedTypes to reduce	*/
/* memory usage.							*/
/* When done, sclInfo->serverHead will contain a list of servers found.	*/
/* CRITICAL: scl_serv_cfg_arr and scl_iedtype_cfg_arr must point to	*/
/*           info that is valid until all SCL processing is done	*/
/*           (i.e. AFTER scl_ld_create_all_scd returns).		*/
/* NOTE: These SCL_DEC_CTRL members not used in this parsing mode	*/
/*       because multiple IED/AccessPoint matches may be found:		*/
/*		iedName							*/
/*		accessPointName						*/
/*		accessPointFound					*/
/* NOTE: This function returns SD_SUCCESS if SCL file is valid, but it	*/
/*       may not have found the Servers requested.			*/
/************************************************************************/
ST_RET scl_parse_scd_filtered (ST_CHAR *xmlFileName,
	SCL_OPTIONS *options,	/* miscellaneous parser options		*/
				/* may be NULL if no options needed	*/
	SCL_INFO *sclInfo,	/* main struct where all SCL info stored*/
	SCL_SERV_CFG *scl_serv_cfg_arr,	/* array of Servers to configure*/
	ST_UINT scl_serv_cfg_num,	/* number of Servers in array	*/
	ST_CHAR **scl_iedtype_cfg_arr,	/* array of iedTypes to configure*/
	ST_UINT scl_iedtype_cfg_num)	/* number of iedTypes in array	*/
  {
ST_RET ret;
SCL_DEC_CTRL sclDecCtrl = {0};	/* start with clean struct.	*/

  memset (sclInfo, 0, sizeof (SCL_INFO));    /* CRITICAL: start with clean struct*/

  /* Set "parseMode" to control parsing later.	*/
  sclDecCtrl.parseMode = SCL_PARSE_MODE_SCD_FILTERED;

  /* Check function arguments.	*/
  if (scl_serv_cfg_arr == NULL)
    {
    SXLOG_ERR0 ("scl_parse_scd_filtered: scl_serv_cfg_arr pointer must not be NULL");
    return (SD_FAILURE);	/* cannot continue	*/
    }
  if (scl_serv_cfg_num == 0)
    {
    SXLOG_ERR0 ("scl_parse_scd_filtered: scl_serv_cfg_num must be > 0");
    return (SD_FAILURE);	/* cannot continue	*/
    }
  /* scl_iedtype_cfg_arr may be NULL, but if !NULL, num must be > 0.	*/
  if (scl_iedtype_cfg_arr != NULL && scl_iedtype_cfg_num == 0)
    {
    SXLOG_ERR0 ("scl_parse_scd_filtered: scl_iedtype_cfg_num must be > 0");
    return (SD_FAILURE);	/* cannot continue	*/
    }
  /* These used in IED & AccessPoint functions only for this parseMode. 	*/
  sclInfo->scl_serv_cfg_arr = scl_serv_cfg_arr;
  sclInfo->scl_serv_cfg_num = scl_serv_cfg_num;
  sclInfo->scl_iedtype_cfg_arr = scl_iedtype_cfg_arr;
  sclInfo->scl_iedtype_cfg_num = scl_iedtype_cfg_num;

  /* save parser options to use later. Used for any parse mode	*/
  if (options != NULL)
    memcpy (&sclInfo->options, options, sizeof (SCL_OPTIONS));

  sclDecCtrl.sclInfo = sclInfo;
    
  ret = sx_parseExx_mt (xmlFileName, 
          sizeof (sclStartElements)/sizeof(SX_ELEMENT), sclStartElements,
          &sclDecCtrl, _scl_unknown_el_start, _scl_unknown_el_end);

  /* NOTE: sx_parseEx_mt doesn't log error if file open fails, so log here*/
  /* It may not log some other errors, so log any other error here too.	*/
  if (ret == SX_FILE_NOT_FOUND)
    SXLOG_ERR1 ("XML File (%s) Open Error",xmlFileName);
  else if (ret != SD_SUCCESS)
    SXLOG_ERR2 ("Error 0x%X parsing SCL file (%s)", ret, xmlFileName);

  /* NOTE: ret==SD_SUCCESS if parse succeeded, but may NOT have found	*/
  /*       any Servers (i.e. sclInfo->serverHead may be NULL).		*/
  return (ret);
  }


