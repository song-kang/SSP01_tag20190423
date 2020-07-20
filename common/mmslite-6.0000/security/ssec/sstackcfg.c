/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*     		2003 - 2012, All Rights Reserved		        */
/*									*/
/* MODULE NAME : sStackCfg.c						*/
/* PRODUCT(S)  : XXX-142-XXX						*/
/*									*/
/* MODULE DESCRIPTION : SISCO Stack Configuration (SSC)			*/
/*									*/
/*	This file provides user application interface to configure	*/
/*	the SISCO Stack	and Security parameters.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments    			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/15/12  EJV           Added KEY_WOW64_64KEY to allow 32-bit app	*/
/*			     read 64-bit registry keys in HKLM\SOFTWARE.*/
/* 01/19/12  EJV	   _WIN64: added typecast to elim warnings.	*/
/* 03/08/11  EJV	   sscVerifyAppNetworkEx: fix to previous chg.	*/
/* 02/16/11  EJV	   sscVerifyAppNetworkEx: elim warning.		*/
/* 02/08/11  EJV	   sscUpdateAppNetwork: fix dibMatch update.	*/
/* 01/17/11  EJV	   Add sscVerifyAppNetworkEx to fix VALIDATE_CFG*/
/*			     for APP_NAME=ALL option (#1800).		*/
/* 12/21/10  EJV	   Closed comment. Del comment.			*/
/* 10/28/10  EJV	   Added sscVerifyArMatchingProfile.		*/
/* 08/10/10  EJV	   sscUpdateAppArName: chg slogs.		*/
/*			   sscUpdateAppArNameList: don't update if role	*/
/*			     changed; corr updated Ar Name.		*/
/*			   Added sscFindNextAppArName.			*/
/* 08/10/10  EJV           find_dib_entry: ret NULL if name=NULL.	*/
/* 07/27/10  EJV           Eliminated warnings.				*/
/* 04/27/10  EJV           sscGetCfgFilePath: access registry ConfigPath*/
/* 12/16/09  EJV    09     SSC_APP_NETWORK: added MMS params (max*).	*/
/* 11/17/09  JRB    08     Add find_dib_entry.				*/
/*			   Add #include files.				*/
/*			   Put ifdef S_SEC_ENABLED around secure code	*/
/*			   (don't need to link ssec_l.lib or DLL).	*/
/* 08/21/09  EJV    07     Added sscGetNetworkDataPath for Vista UAC.	*/
/*			   sscGetCfgFilePath: chg to use ...DataPath.	*/
/*			   Added sscFreeArName.				*/
/*                         Finished sscUpdate* functions.		*/
/* 04/23/09  EJV    06     sscConfigAppNetwork:don't ret cfg ptr (safer)*/
/* 08/21/08  EJV    05     Chg sscFindCaCertCfgId to sscFindCertCfgId.	*/
/* 09/26/08		   Mv lookup code to seccfg_comn.c.		*/
/* 03/06/08  EJV    04     sscCheckStackCfg: disabled code until	*/
/*                           reconfiguration implemented.		*/
/* 02/07/08  EJV    03     sscParseObjId: elim unsafe strtok call.	*/
/* 12/14/07  EJV    02     Lock usr code before calling most SSLEngine	*/
/*			   funs to avoid deadlocks.			*/
/* 07/31/07  EJV    01     Merged osill2.cfg and secManCfg.xml for V2.00*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "str_util.h"

#include "tp0_sock.h"
#include "tp0_sock_snap.h"
#include "sstackcfg.h"
#include "snap_l_inf.h"
#include "snap_l_ipc.h"
#include "ssec_int.h"
#if defined(S_SEC_ENABLED)
#include "sslengine.h"		/* needed for sslePrintSubject		*/
#endif

/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* macro to get the application name into logging */
#define SSC_APP_NAME(sscStackCfg) ((sscStackCfg->appNetwork && sscStackCfg->appNetwork->appName) ? sscStackCfg->appNetwork->appName : "?")

	/* =====================================================*/
	/*	SISCO's Stack Configuration logging		*/
	/* =====================================================*/

ST_UINT ssc_debug_sel = SSC_LOG_ERR;

/* Log type strings */
SD_CONST ST_CHAR *SD_CONST ssc_err_logstr   = "SSC_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST ssc_nerr_logstr  = "SSC_LOG_NERR";
SD_CONST ST_CHAR *SD_CONST ssc_flow_logstr  = "SSC_LOG_FLOW";

LOGCFGX_VALUE_MAP sscLogMaskMaps[] =
  {
    {"SSC_LOG_ERR",	SSC_LOG_ERR,	&ssc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"SSC_LOG_NERR",	SSC_LOG_NERR,	&ssc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"SSC_LOG_FLOW",	SSC_LOG_FLOW,	&ssc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Flow"}
  };

LOGCFG_VALUE_GROUP sscLogMaskMapCtrl =
  {
  {NULL,NULL},
  "SscLogMasks",	/* Parent Tag	*/
  sizeof(sscLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  sscLogMaskMaps
  };


/* global SISCO Stack Configuration, used to configure lower layers	*/
/* (at this point we do have the function intarface to pass this	*/
/* pointer to various initialization functions)				*/
static SSC_STACK_CFG *_sscStackCfgGlb = NULL;

#if !defined(_WIN32)
/* UNIX, Linux systems....						*/
/* default installation path for SISCO product(s)			*/
static ST_CHAR  sscSiscoInstallPath [S_MAX_PATH] = SISCO_PATH_INSTALL;
/* default path for Sisco Stack Configuration file SSC_FILE_NAME	*/
static ST_CHAR  sscCfgFilePath [S_MAX_PATH]      = SSC_FILE_PATH;
#endif /* !defined(_WIN32) */

	/* =====================================================*/
	/* 	Functions internal to this module		*/
	/* =====================================================*/

ST_RET sscVerifyAppNetworkEx (SSC_STACK_CFG *sscStackCfg, ST_CHAR *appName, ST_BOOLEAN bVerifyAll);


/************************************************************************/
/*			sscAccessStackCfg				*/
/*----------------------------------------------------------------------*/
/* Returns SISCO Stack Cfg internal pointer. LOCK mutex to protect the	*/
/* SISCO Stack Configuratin integrity since run-time updates may change	*/
/* the structure. Call sscReleaseStackCfg when pointer is no longer needed.*/
/************************************************************************/
SSC_STACK_CFG *sscAccessStackCfg (ST_VOID)
{
  S_LOCK_UTIL_RESOURCES();

  return (_sscStackCfgGlb);  /* maybe NULL ptr */
}

/************************************************************************/
/*			sscReleaseStackCfg				*/
/*----------------------------------------------------------------------*/
/* Releases access to the SISCO Stack Cfg internal pointer.		*/
/* UNLOCK mutex is called.						*/
/************************************************************************/
ST_RET sscReleaseStackCfg (ST_VOID)
{
  S_UNLOCK_UTIL_RESOURCES();

  return (SD_SUCCESS);
}

	/* =====================================================*/
	/*	SISCO Stack Read functions			*/
	/* =====================================================*/

/************************************************************************/
/*			sscReadStackCfg 				*/
/*----------------------------------------------------------------------*/
/* Reads the SISCO Stack configuration from XML file or DB.		*/
/* Return:								*/
/*	SD_SUCCESS			if function successful,		*/
/*	SX_xxx error or SD_FAILURE	otherwise			*/
/************************************************************************/
ST_RET sscReadStackCfg (SSC_STACK_CFG **sscStackCfgOut)
{
ST_RET         rtn = SD_FAILURE;
ST_INT         useCfgFile = 1;	/* default on UNIX and Linux		*/
ST_CHAR        sscFileName [S_MAX_PATH+1];

  SSC_LOG_FLOW0 ("SSC INFO: reading SISCO Stack Configuration.");

#if defined(_WIN32)
  rtn = sscGetUseCfgFileFlag (&useCfgFile);
#endif /* defined(_WIN32) */

  if (useCfgFile)
    {
    /* read SISCO Stack Cfg XML file */
    rtn = sscGetCfgFilePath (sscFileName, (ST_INT) (sizeof(sscFileName) - strlen(SSC_FILE_NAME) - 1));
    if (rtn == SD_SUCCESS)
      {
      strcat (sscFileName, SSC_FILE_NAME);
      rtn = sscReadStackCfgFromXmlFile (sscFileName, sscStackCfgOut);
      if (rtn == SD_SUCCESS)
        SSC_LOG_FLOW1 ("SSC INFO: SISCO Stack Configuration read from file '%s'", sscFileName);
      }
    }
  else
    {
    /*  configuration from other sources (database) not implemented	*/
    SLOGALWAYS0 ("SSC ERROR: Failed to configure SISCO Stack from other sources (useCfgFile=0).");
    rtn = SD_FAILURE;
    }

  return (rtn);
}


	/* =====================================================*/
	/*	SISCO Stack Install Path Get/Set functions	*/
	/* =====================================================*/

#if defined(_WIN32)
/************************************************************************/
/*			  sscGetNetworkInstallPath			*/
/*----------------------------------------------------------------------*/
/* Returns the path to the Network subdirectory (lookup in registry).	*/
/* The pathBuf will have the '\\' appended at the end.			*/
/************************************************************************/
ST_RET sscGetNetworkInstallPath (ST_CHAR *pathBuf, ST_INT pathBufSize)
  {
ST_RET  rtn = SD_FAILURE;
LONG    winRet;
HKEY    hKey;
ST_INT  len = pathBufSize - 2; 	/* -1 for NULL terminator, -1 for '\\'	*/

  winRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
    	                 SISCO_REGISTRY_NETWORK, 
    	                 0, 
                         KEY_READ | KEY_WOW64_64KEY, 	/* allow 32-bit app read 64-bit registry key */
    	                 &hKey); 
  if (winRet == ERROR_SUCCESS)
    {
    winRet = RegQueryValueEx (hKey, "Path", NULL, NULL, (LPBYTE) pathBuf, &len);
    if (winRet == ERROR_SUCCESS)
      {
      pathBuf [pathBufSize-2] = 0;  /* make sure the string is NULL terminated	*/ 
      /* we will append file names to this path, add the saparator to the end */
      if (pathBuf[strlen(pathBuf)-1] != '\\' )
        strcat (pathBuf, "\\");
      rtn = SD_SUCCESS;
      }
    RegCloseKey (hKey);
    }
  if (winRet != ERROR_SUCCESS)
    SSC_LOG_ERR2("SSC ERROR: Unable to read registry key, winRet=%ld\n"
                "  'HKEY_LOCAL_MACHINE\\%s\\Path'", winRet, SISCO_REGISTRY_NETWORK);
  return (rtn);
  }

/************************************************************************/
/*			  sscGetNetworkDataPath				*/
/*----------------------------------------------------------------------*/
/* Returns path to the Network data subdirectory (lookup in registry).	*/
/* The pathBuf will have the '\\' appended at the end.			*/
/************************************************************************/
ST_RET sscGetNetworkDataPath (ST_CHAR *pathBuf, ST_INT pathBufSize)
  {
ST_RET  rtn = SD_FAILURE;
LONG    winRet;
HKEY    hKey;
ST_INT  len = pathBufSize - 2; 	/* -1 for NULL terminator, -1 for '\\'	*/

  winRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
    	                 SISCO_REGISTRY_NETWORK, 
    	                 0, 
                         KEY_READ | KEY_WOW64_64KEY, 	/* allow 32-bit app read 64-bit registry key */
    	                 &hKey); 
  if (winRet == ERROR_SUCCESS)
    {
    winRet = RegQueryValueEx (hKey, "DataPath", NULL, NULL, (LPBYTE) pathBuf, &len);
    if (winRet == ERROR_SUCCESS)
      {
      pathBuf [pathBufSize-2] = 0;  /* make sure the string is NULL terminated	*/ 
      /* we will append file names to this path, add the saparator to the end */
      if (pathBuf[strlen(pathBuf)-1] != '\\' )
        strcat (pathBuf, "\\");
      rtn = SD_SUCCESS;
      }
    RegCloseKey (hKey);
    }
  if (winRet != ERROR_SUCCESS)
    SSC_LOG_ERR2("SSC ERROR: Unable to read registry key, winRet=%ld\n"
                "  'HKEY_LOCAL_MACHINE\\%s\\DataPath'", winRet, SISCO_REGISTRY_NETWORK);
  return (rtn);
  }

/************************************************************************/
/*			  sscGetCfgFilePath				*/
/*----------------------------------------------------------------------*/
/* Returns the Network configuration path.				*/
/* The pathBuf will have the '\\' appended at the end.			*/
/************************************************************************/
ST_RET sscGetCfgFilePath (ST_CHAR *pathBuf, ST_INT pathBufSize)
  {
ST_RET  rtn = SD_FAILURE;
LONG    winRet;
HKEY    hKey;
ST_INT  len = pathBufSize - 2; 	/* -1 for NULL terminator, -1 for '\\'	*/

  winRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
    	                 SISCO_REGISTRY_NETWORK, 
    	                 0, 
                         KEY_READ | KEY_WOW64_64KEY, 	/* allow 32-bit app read 64-bit registry key */
    	                 &hKey); 
  if (winRet == ERROR_SUCCESS)
    {
    winRet = RegQueryValueEx (hKey, "ConfigPath", NULL, NULL, (LPBYTE) pathBuf, &len);
    if (winRet == ERROR_SUCCESS)
      {
      pathBuf [pathBufSize-2] = 0;  /* make sure the string is NULL terminated	*/ 
      /* we will append file names to this path, add the saparator to the end */
      if (pathBuf[strlen(pathBuf)-1] != '\\' )
        strcat (pathBuf, "\\");
      rtn = SD_SUCCESS;
      }
    RegCloseKey (hKey);
    }
  if (winRet != ERROR_SUCCESS)
    SSC_LOG_ERR2("SSC ERROR: Unable to read registry key, winRet=%ld\n"
                "  'HKEY_LOCAL_MACHINE\\%s\\ConfigPath'", winRet, SISCO_REGISTRY_NETWORK);
  return (rtn);
  }

/************************************************************************/
/*			  sscGetUseCfgFileFlag				*/
/*----------------------------------------------------------------------*/
/* Returns the value of registry parameter "UseCfgFile".		*/
/* If it is set to 1 then the SSC_FILE_NAME will be used to access the 	*/
/* SISCO Stack Configuration, otherwise other source will be used.	*/
/************************************************************************/
ST_RET sscGetUseCfgFileFlag (ST_INT *useCfgFile)
  {
ST_RET  rtn = SD_FAILURE;
LONG    winRet;
HKEY    hKey;
ST_INT  val;
DWORD   valSize = sizeof (ST_INT);
ST_CHAR keyBuf[256];

  sprintf (keyBuf, "%s\\Parameters", SISCO_REGISTRY_NETWORK);
  winRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
    	                 keyBuf, 
    	                 0, 
                         KEY_READ | KEY_WOW64_64KEY, 	/* allow 32-bit app read 64-bit registry key */
    	                 &hKey); 
  if (winRet == ERROR_SUCCESS)
    {
    winRet = RegQueryValueEx (hKey, "UseCfgFile", NULL, NULL, (LPBYTE) &val, &valSize);
    if (winRet == ERROR_SUCCESS)
      {
      *useCfgFile = val;
      rtn = SD_SUCCESS;
      }
    RegCloseKey (hKey);
    }
  if (winRet != ERROR_SUCCESS)
    SSC_LOG_ERR2("SSC ERROR: Unable to read registry key, winRet=%ld\n"
                "  'HKEY_LOCAL_MACHINE\\%s\\UseCfgFile'", winRet, keyBuf);
  return (rtn);
  }

#else  /* !defined(_WIN32)	*/

/* UNIX, Linux systems....						*/

/************************************************************************/
/*			  sscSetSiscoInstallPath			*/
/*----------------------------------------------------------------------*/
/* UNIX, Linux: in case the customer has problem with using the default	*/
/* installation	path we will allow the path be set by the customer code.*/
/************************************************************************/
ST_RET sscSetSiscoInstallPath (ST_CHAR *pathBuf)
  {
ST_RET rtn = SD_FAILURE;

/* NOTE: no SLOGs, logging is not initialized when this fun is called.	*/
  if (strlen (pathBuf) > 0)
    {
    strncpy_safe (sscSiscoInstallPath, pathBuf, sizeof(sscSiscoInstallPath)-2);

    /* update also the sscCfgFilePath (only if user did not changed it already) */
    if (strcmp(sscCfgFilePath, SSC_FILE_PATH) == 0)
      sprintf(sscCfgFilePath, "%s%s", pathBuf, SISCO_PATH_NETWORK);
    rtn = SD_SUCCESS;
    }

  return (rtn);
  }

/************************************************************************/
/*			  sscGetSiscoInstallPath			*/
/*----------------------------------------------------------------------*/
/* Returns the path to the SISCO's installation directory.		*/
/************************************************************************/
ST_RET sscGetSiscoInstallPath (ST_CHAR *pathBuf, ST_INT pathBufSize)
  {
  if (!pathBuf)
    return SD_FAILURE;
  if (pathBufSize-1 <= 0)
    return SD_FAILURE;  /* buffer too small */

  if (strlen(sscSiscoInstallPath) > 0)
    {
    strncpy_safe (pathBuf, sscSiscoInstallPath, pathBufSize-1);
    return SD_SUCCESS;
    }
  else
    return SD_FAILURE;    /* path not set */
  }

/************************************************************************/
/*			  sscGetSiscoInstallPathPtr			*/
/*----------------------------------------------------------------------*/
/* Returns the ptr to the path to the SISCO's installation directory.	*/
/************************************************************************/
ST_CHAR *sscGetSiscoInstallPathPtr (ST_VOID)
  {
  return (sscSiscoInstallPath);
  }

/************************************************************************/
/*			  sscSetCfgFilePath				*/
/*----------------------------------------------------------------------*/
/* UNIX, Linux: in case the customer has problem with using the default	*/
/* cfg file path we will allow the path be set by the customer code.	*/
/************************************************************************/
ST_RET sscSetCfgFilePath (ST_CHAR *pathBuf)
  {
/* NOTE: no SLOGs, logging is not initialized when this fun is called.	*/
  if (strlen(pathBuf) > 0)
    {
    strncpy_safe (sscCfgFilePath, pathBuf, sizeof(sscCfgFilePath)-2);
    if (sscCfgFilePath[strlen(sscCfgFilePath)-1] != '/')
      /* we will append file names to this path, add the saparator to the end */
       strcat (sscCfgFilePath, "/");
    return SD_SUCCESS;
    }
  else
    return SD_FAILURE;    /* path not set */
  }


/************************************************************************/
/*			  sscGetNetworkInstallPath			*/
/*----------------------------------------------------------------------*/
/* Returns the path to the SISCO's Network subdirectory.		*/
/************************************************************************/
ST_RET sscGetNetworkInstallPath (ST_CHAR *pathBuf, ST_INT pathBufSize)
  {
  if (!pathBuf)
    return SD_FAILURE;
  if (pathBufSize-strlen(SISCO_PATH_NETWORK)-1 <= 0)
    return SD_FAILURE;  /* buffer too small */

  if (strlen(sscSiscoInstallPath) > 0)
    {
    strncpy_safe (pathBuf, sscSiscoInstallPath, pathBufSize-strlen(SISCO_PATH_NETWORK)-1);
    strcat (pathBuf, SISCO_PATH_NETWORK);
    return SD_SUCCESS;
    }
  else
    return SD_FAILURE;    /* path not set */
  }

/************************************************************************/
/*			  sscGetCfgFilePath				*/
/*----------------------------------------------------------------------*/
/* Returns the path to the SISCO Stack Configuration file SSC_FILE_NAME.*/
/************************************************************************/
ST_RET sscGetCfgFilePath (ST_CHAR *pathBuf, ST_INT pathBufSize)
  {
  if (!pathBuf)
    return SD_FAILURE;
  if (pathBufSize-1 <= 0)
    return SD_FAILURE;  /* buffer too small */

  if (strlen(sscCfgFilePath) > 0)
    {
    strncpy_safe (pathBuf, sscCfgFilePath, pathBufSize-1);
    return SD_SUCCESS;
    }
  else
    return SD_FAILURE;    /* path not set */
  }

#endif /* !defined(_WIN32) */


	/* =====================================================*/
	/* 	Helper/lookup/Conversion functions		*/
	/* =====================================================*/

/************************************************************************/
/*			sscParseObjId					*/
/*----------------------------------------------------------------------*/
/* Parses components of the 'ApTitle'.                                  */
/************************************************************************/

ST_RET sscParseObjId (MMS_OBJ_ID *objId, ST_CHAR *buff)
{
char    *numStr;
ST_CHAR *seps = " \t";	/* delimiters to search for	*/

  /* Found keyword 'ApTitle', read the components                       */
  objId->num_comps = 0;
  numStr = get_next_string (&buff, seps);	/* "buff" ptr changes	*/
  while (numStr && strlen (numStr) > 0)
    {
    if (objId->num_comps >= MAX_OBJID_COMPONENTS)
      {
      return (SD_FAILURE);
      }
    objId->comps[objId->num_comps] = (ST_INT16) atoi (numStr);
    numStr = get_next_string (&buff, seps);	/* "buff" ptr changes	*/
    ++objId->num_comps;
    }

  if (objId->num_comps == 0)    /* couldn't parse anything  */
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.AP_Title' components not found.");
    return (SD_FAILURE);
    }
  return (SD_SUCCESS);
}

	/* =============================================================*/
	/* 	SISCO Stack Configuration Find functions		*/
	/* =============================================================*/

/************************************************************************/
/*		sscFindNetworkAdapter					*/
/************************************************************************/
SSC_NETWORK_ADAPTER *sscFindNetworkAdapter (SSC_STACK_CFG *sscStackCfg, ST_CHAR *adapterName)
{
SSC_NETWORK_ADAPTER *networkAdapter;
SSC_NETWORK_ADAPTER *networkAdapterOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  if (!adapterName)
    return (NULL);

  /* find the Network Adapter in the networkAdapterList */
  networkAdapter = sscStackCfg->networkAdapterList;
  while (networkAdapter)
    {
    if (strcmp (adapterName, networkAdapter->name) == 0)
      {
      networkAdapterOut = networkAdapter;
      break;
      }
    networkAdapter = list_get_next (sscStackCfg->networkAdapterList, networkAdapter);
    }

  return (networkAdapterOut);
}

/************************************************************************/
/*		sscFindLeantProfile					*/
/************************************************************************/
SSC_LEANT_PROFILE *sscFindLeantProfile (SSC_STACK_CFG *sscStackCfg, ST_CHAR *profileName)
{
SSC_LEANT_PROFILE  *leantProfile;
SSC_LEANT_PROFILE  *leantProfileOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  if (!profileName)
    return (NULL);

  /* find the leantProfile name in the leantProfileList */
  leantProfile = sscStackCfg->leantProfileList;
  while (leantProfile)
    {
    if (strcmp (profileName, leantProfile->name) == 0)
      {
      leantProfileOut = leantProfile;
      break;
      }
    leantProfile = list_get_next (sscStackCfg->leantProfileList, leantProfile);
    }

  return (leantProfileOut);
}

/************************************************************************/
/*		sscFindArName						*/
/************************************************************************/
SSC_AR_NAME *sscFindArName (SSC_STACK_CFG *sscStackCfg, ST_CHAR *arName)
{
SSC_AR_NAME *sscArName;
SSC_AR_NAME *sscArNameOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  if (!arName)
    return (NULL);

  /* find the certificate name in the certificateList */
  sscArName = sscStackCfg->arNameList;
  while (sscArName)
    {
    if (sscArName->arDib)
      {
      if (strcmp  (arName, sscArName->arDib->name) == 0)
        {
        sscArNameOut = sscArName;
        break;
        }
      }
    sscArName = list_get_next (sscStackCfg->arNameList, sscArName);
    }

  return (sscArNameOut);
}

/************************************************************************/
/*		sscFindAppArName					*/
/************************************************************************/
SSC_APP_AR_NAME *sscFindAppArName (SSC_APP_NETWORK *appNetwork, ST_CHAR *arName)
{
SSC_APP_AR_NAME *sscAppArName;
SSC_APP_AR_NAME *sscAppArNameOut = NULL;

  if (!appNetwork)
    return (NULL);

  if (!arName)
    return (NULL);

  /* find the ArName in the appArNameList */
  sscAppArName = appNetwork->appArNameList;
  while (sscAppArName)
    {
    if (strcmp (arName, sscAppArName->arName) == 0)
        {
        sscAppArNameOut = sscAppArName;
        break;
        }
    sscAppArName = list_get_next (appNetwork->appArNameList, sscAppArName);
    }

  return (sscAppArNameOut);
}

/************************************************************************/
/*		sscFindNextAppArName					*/
/************************************************************************/
SSC_APP_AR_NAME *sscFindNextAppArName (SSC_APP_NETWORK *appNetwork, ST_INT role,
                                       SSC_APP_AR_NAME *sscAppArNameAfter)
{
SSC_APP_AR_NAME *sscAppArName = NULL;
SSC_APP_AR_NAME *sscAppArNameOut = NULL;

  if (!appNetwork)
    return (NULL);

  if (sscAppArNameAfter)
    {
    if (appNetwork->appArNameList)
      /* start after this link list node */
      sscAppArName = list_get_next (appNetwork->appArNameList, sscAppArNameAfter); 
    }
  else
    sscAppArName = appNetwork->appArNameList;

  /* find the appArName in the appArNameList */
  while (sscAppArName)
    {
    if (sscAppArName->role == role)
      {
      sscAppArNameOut = sscAppArName;
      break;
      }
    sscAppArName = list_get_next (appNetwork->appArNameList, sscAppArName);
    }

  return (sscAppArNameOut);
}

/************************************************************************/
/*		sscFindSecurityProfile					*/
/************************************************************************/
SSC_SECURITY_PROFILE *sscFindSecurityProfile (SSC_STACK_CFG *sscStackCfg, ST_CHAR *profileName)
{
SSC_SECURITY_PROFILE  *securityProfile;
SSC_SECURITY_PROFILE  *securityProfileOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  if (!profileName)
    return (NULL);

  /* find the securityProfile name in the securityProfileList */
  securityProfile = sscStackCfg->securityProfileList;
  while (securityProfile)
    {
    if (strcmp (profileName, securityProfile->name) == 0)
      {
      securityProfileOut = securityProfile;
      break;
      }
    securityProfile = list_get_next (sscStackCfg->securityProfileList, securityProfile);
    }

  return (securityProfileOut);
}

/************************************************************************/
/*		sscFindArMatchingProfile				*/
/************************************************************************/
SSC_AR_MATCHING_PROFILE *sscFindArMatchingProfile (SSC_STACK_CFG *sscStackCfg, ST_CHAR *profileName)
{
SSC_AR_MATCHING_PROFILE  *arMatchingProfile;
SSC_AR_MATCHING_PROFILE  *arMatchingProfileOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  if (!profileName)
    return (NULL);

  /* find the arMatchingProfile name in the arMatchingProfileList */
  arMatchingProfile = sscStackCfg->arMatchingProfileList;
  while (arMatchingProfile)
    {
    if (strcmp (profileName, arMatchingProfile->name) == 0)
      {
      arMatchingProfileOut = arMatchingProfile;
      break;
      }
    arMatchingProfile = list_get_next (sscStackCfg->arMatchingProfileList, arMatchingProfile);
    }

  return (arMatchingProfileOut);
}

/************************************************************************/
/*		sscFindAppNetwork					*/
/************************************************************************/
SSC_APP_NETWORK *sscFindAppNetwork (SSC_STACK_CFG *sscStackCfg, ST_CHAR *appName)
{
SSC_APP_NETWORK *appNetwork;
SSC_APP_NETWORK *appNetworkOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  if (!appName)
    return (NULL);

  /* find the appNetwork in the appNetworklist */
  appNetwork = sscStackCfg->appNetworkList;
  while (appNetwork)
    {
    if (strcmp (appName, appNetwork->appName) == 0)
      {
      appNetworkOut = appNetwork;
      break;
      }
    appNetwork = list_get_next (sscStackCfg->appNetworkList, appNetwork);
    }

  return (appNetworkOut);
}


	/* =============================================================*/
	/* 	SISCO Stack Configuration Add functions			*/
	/* =============================================================*/
	/* Functions to add subcomponents to the SSC_STACK_CFG.	These	*/
	/* functions allocate all needed pointers so they can be used	*/
	/* during Stack Configuration without recopying the content.	*/
	/* Where applicable the structures are initialized with default	*/
	/* parameters. 							*/
	/* =============================================================*/


/************************************************************************/
/*		sscAddNetworkAdapter					*/
/************************************************************************/
SSC_NETWORK_ADAPTER *sscAddNetworkAdapter (SSC_STACK_CFG *sscStackCfg)
{
SSC_NETWORK_ADAPTER  *networkAdapterOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  if (sscStackCfg)
    {
    networkAdapterOut = (SSC_NETWORK_ADAPTER *) chk_calloc (1, sizeof (SSC_NETWORK_ADAPTER));
    list_add_last (&sscStackCfg->networkAdapterList, networkAdapterOut);

    /* set default parameters */

    strcpy (networkAdapterOut->name, "UNKNOWN");
    }

  return (networkAdapterOut);
}

/************************************************************************/
/*		sscAddLeantProfile					*/
/************************************************************************/
SSC_LEANT_PROFILE *sscAddLeantProfile (SSC_STACK_CFG *sscStackCfg)
{
SSC_LEANT_PROFILE *leantProfileOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  leantProfileOut = (SSC_LEANT_PROFILE *) chk_calloc (1, sizeof (SSC_LEANT_PROFILE));
  list_add_last (&sscStackCfg->leantProfileList, leantProfileOut);

  /* set default parameters */

  strcpy (leantProfileOut->name, "UNKNOWN");
  leantProfileOut->sessionDisconnectTimeout = SSC_SESSION_DISCONNECT_TIMEOUT;

  /* TCP transport parameters */
  leantProfileOut->tcpEnable             = SD_FALSE;	/* let user set		*/
  leantProfileOut->tcpMaxTpduLen         = 1024;
  leantProfileOut->tcpMaxSpduOutstanding = 50;
  leantProfileOut->tcpListenPort         = RFC1006_PORT;/* default is 102	*/
  leantProfileOut->tcpMaxIdleTime        = 0;		/* not checking for idle conns	*/
  leantProfileOut->gsWakeupPortBase      = GEN_SOCK_WAKEUP_PORT_BASE;
  leantProfileOut->gsWakeupPortRange     = GEN_SOCK_WAKEUP_PORT_RANGE;
  leantProfileOut->tcpEventWakeupPortBase  = TCP_EVENT_WAKEUP_PORT_BASE;
  leantProfileOut->tcpEventWakeupPortRange = TCP_EVENT_WAKEUP_PORT_RANGE;
  /* TP4 transport parameters */
  leantProfileOut->tp4Enable             = SD_FALSE;	/* let user set		*/
  leantProfileOut->tp4MaxTpduLen         = 1024;
  leantProfileOut->tp4MaxSpduOutstanding = 4;
  leantProfileOut->tp4LocalCredits       = 4;
  leantProfileOut->tp4MaxRemoteCredits   = 4;
  leantProfileOut->tp4WindowTime         = 10;
  leantProfileOut->tp4InactivityTime     = 120;
  leantProfileOut->tp4RetransmissionTime = 10;
  leantProfileOut->tp4MaxTransmissions   = 2;
  leantProfileOut->tp4AckDelay           = 2;
  /* network parameters (TP4) */
  leantProfileOut->clnpLifetime          = 50;
  leantProfileOut->clnpLifetimeDecrement = 1;
  leantProfileOut->clnpCfgTimer          = 120;
  leantProfileOut->clnpEshDelay          = 5;
  leantProfileOut->adapterName [0]       = 0;

  return (leantProfileOut);
}

/************************************************************************/
/*		sscAddCertParam						*/
/************************************************************************/
SSC_CERT_PARAM *sscAddCertParam (SSC_STACK_CFG *sscStackCfg)
{
SSC_CERT_PARAM *certParamOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  sscStackCfg->certParam = certParamOut = (SSC_CERT_PARAM *) chk_calloc (1, sizeof (SSC_CERT_PARAM));

  /* set default parameters */

  /* certificates expiration checking */
  certParamOut->certExpirationAlarmLimit   = 1440;	/* in hours, if 0 no checking done	*/
  certParamOut->certExpirationEvalInterval = 24;	/* in hours	*/
  /* SSL Certificate Revocation parameters */
  certParamOut->crlCheckTime            = 720;		/* in minutes	*/
  certParamOut->crlDropExistingConns    = SD_FALSE;

  return (certParamOut);
}

/************************************************************************/
/*		sscAddTrustedCaCert					*/
/************************************************************************/
S_CERT_CTRL *sscAddTrustedCaCert (SSC_STACK_CFG *sscStackCfg)
{
S_CERT_CTRL *certCtrlOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  certCtrlOut = (S_CERT_CTRL *) chk_calloc (1, sizeof (S_CERT_CTRL));

  /* set default parameters */

  certCtrlOut->certId.validity.validNotBefore = 0;
  certCtrlOut->certId.validity.validNotAfter  = (time_t) -1;
  strcpy (certCtrlOut->certId.validity.validFrom, "unknown");
  strcpy (certCtrlOut->certId.validity.validTo,   "unknown");

  list_add_last (&sscStackCfg->trustedCaCertList, certCtrlOut);

  return (certCtrlOut);
}

/************************************************************************/
/*		sscAddCertificate					*/
/************************************************************************/
S_CERT_CTRL *sscAddCertificate (SSC_STACK_CFG *sscStackCfg)
{
S_CERT_CTRL *certCtrlOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  certCtrlOut = (S_CERT_CTRL *) chk_calloc (1, sizeof (S_CERT_CTRL));

  /* set default parameters */

  certCtrlOut->certId.validity.validNotBefore = 0;
  certCtrlOut->certId.validity.validNotAfter  = (time_t) -1;
  strcpy (certCtrlOut->certId.validity.validFrom, "unknown");
  strcpy (certCtrlOut->certId.validity.validTo,   "unknown");

  list_add_last (&sscStackCfg->certificateList, certCtrlOut);

  return (certCtrlOut);
}

/************************************************************************/
/*		sscAddCipher						*/
/************************************************************************/
ST_RET sscAddCipher (SSC_STACK_CFG *sscStackCfg, ST_INT cipherMask)
{
ST_RET         rtn = SD_FAILURE;
SSC_SSL_PARAM *sslParam = NULL;

  if (!sscStackCfg)
    return (SD_FAILURE);

  if (!sscStackCfg->sslParam)
    {
    SSC_LOG_ERR0 ("SSC ERROR: add Cipher failed (sslParam=NULL).");
    return (SD_FAILURE);
    }
  sslParam = sscStackCfg->sslParam;

  if (sslParam->cipherArrCnt < S_MAX_ALLOWED_CIPHER_SUITES)
    {
    sslParam->cipherArr[sslParam->cipherArrCnt] = cipherMask;
    sslParam->cipherArrCnt++;
    rtn = SD_SUCCESS;
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: add Cipher failed (maximum limit of Ciphers reached %d)",
                  sslParam->cipherArrCnt);

  return (rtn);  
}

/************************************************************************/
/*		sscAddSslParam						*/
/************************************************************************/
SSC_SSL_PARAM *sscAddSslParam (SSC_STACK_CFG *sscStackCfg)
{
SSC_SSL_PARAM *sslParamOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  sscStackCfg->sslParam = sslParamOut = (SSC_SSL_PARAM *) chk_calloc (1, sizeof (SSC_SSL_PARAM));

  /* set default SSL connection parameters */
  sslParamOut->sslLocalCertCtrlArrCnt  = 0;
  sslParamOut->sslCalledPort           = S_SEC_DEF_SSL_PORT; 	/* listening SSL port		*/
  sslParamOut->sslCallingPort          = S_SEC_DEF_SSL_PORT;	/* calling SSL port (default to)*/
  sslParamOut->sslCalledPeerCertKnown  = SD_TRUE;	/* verify that peer cert is configured	*/
  sslParamOut->sslCallingPeerCertKnown = SD_TRUE;	/* verify that peer cert is configured	*/
  /* SSL session renegotiation (rekeying) parameters */
  sslParamOut->maxTranPerKey           = 5000;
  sslParamOut->maxTimePerKey           = 1800;		/* in seconds */
  sslParamOut->rekeyTimeout            = 60;		/* in seconds */
  sslParamOut->cipherArrCnt            = 0;
  return (sslParamOut);
}

/************************************************************************/
/*		sscAddSnapParam						*/
/************************************************************************/
SSC_SNAP_PARAM *sscAddSnapParam (SSC_STACK_CFG *sscStackCfg)
{
SSC_SNAP_PARAM *snapParamOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  sscStackCfg->snapParam = snapParamOut = (SSC_SNAP_PARAM *) chk_calloc (1, sizeof (SSC_SNAP_PARAM));

  /* SNAP-Lite secure connections parameters */
  if (sscStackCfg->secureDllLib)
    {
    snapParamOut->sslMaxCalled           = 128;
    snapParamOut->sslMaxCalling          = 128;
    snapParamOut->sslProxyCalledPort     = S_SEC_DEF_PROXY_CALLED_PORT;
    snapParamOut->sslProxyCallingPort    = S_SEC_DEF_PROXY_CALLING_PORT;
    snapParamOut->sslProxyConnectTimeout = S_SSL_PROXY_CON_TIMEOUT;			/* ms	*/
    }

  /* SNAP-Lite IPC parameters */
  snapParamOut->maxUsers                = SNAPL_MAX_USERS;	/* 64	*/
  snapParamOut->maxBindsPerUser         = MAX_USER_BINDS;	/* 64	*/
#if defined(_WIN32)
  snapParamOut->ipcPort                 = S_SEC_DEF_SNAP_CONTROL_PORT;
#else /* UNIX,Linux */
  strcpy (snapParamOut->ipcDir, SNAPL_IPC_PATH);
#endif
  snapParamOut->ipcTimeout              = SNAPL_IPC_TIMEOUT;	/* ms	*/
  /* SNAP-Lite misc parameters */
  snapParamOut->cpuAffinity             = 3;	/* for future use	*/

  return (snapParamOut);
}

/************************************************************************/
/*		sscAddArName						*/
/************************************************************************/
SSC_AR_NAME *sscAddArName (SSC_STACK_CFG *sscStackCfg)
{
SSC_AR_NAME *sscArNameOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  sscArNameOut = (SSC_AR_NAME *) chk_calloc (1, sizeof (SSC_AR_NAME));

  /* alloc DIB_ENTRY, the arDib maybe added to appNetwork->dibEntryList */
  sscArNameOut->arDib = (DIB_ENTRY *) chk_calloc (1, sizeof (DIB_ENTRY));

  strcpy ( sscArNameOut->arDib->name, "UNKNOWN");
  /* default to TCP transport (and local IP Addr) */
  sscArNameOut->arDib->pres_addr.tp_type = TP_TYPE_TCP;	
  sscArNameOut->arDib->pres_addr.netAddr.ip = inet_addr ("127.0.0.1");
  
  /* NOTE: the S_SEC_LOC_REM_AR is optional so set to non-secure,	*/
  /* the arSec maybe added to the appNetwork->arLocRemSecList		*/
  sscArNameOut->arSec = (S_SEC_LOC_REM_AR *) chk_calloc (1, sizeof (S_SEC_LOC_REM_AR));
  sscArNameOut->arSec->arAuthMode = S_AR_AUTH_MODE_NONE;
  sscArNameOut->arSec->encryptMode = S_SEC_ENCRYPT_NONE;

  list_add_last (&sscStackCfg->arNameList, sscArNameOut);

  return (sscArNameOut);
}

/************************************************************************/
/*		sscAddAppArName						*/
/************************************************************************/
SSC_APP_AR_NAME *sscAddAppArName (SSC_APP_NETWORK *appNetwork)
{
SSC_APP_AR_NAME *sscAppArNameOut = NULL;

  if (!appNetwork)
    return (NULL);

  sscAppArNameOut = (SSC_APP_AR_NAME *)  chk_calloc (1, sizeof (SSC_APP_AR_NAME));

  /* set default parameters */

  strcpy (sscAppArNameOut->arName, "UNKNOWN");
  sscAppArNameOut->role = SSC_AR_NAME_ROLE_NONE;	/* user must select LOCAL or REMOTE role 	*/

  list_add_last (&appNetwork->appArNameList, sscAppArNameOut);

  return (sscAppArNameOut);
}

/************************************************************************/
/*		sscAddArMatchingProfile					*/
/************************************************************************/
SSC_AR_MATCHING_PROFILE *sscAddArMatchingProfile (SSC_STACK_CFG *sscStackCfg)
{
SSC_AR_MATCHING_PROFILE *arMatchingProfileOut = NULL;
DIB_MATCH_CTRL          *dibMatch;

  if (!sscStackCfg)
    return (NULL);

  arMatchingProfileOut = (SSC_AR_MATCHING_PROFILE *)  chk_calloc (1, sizeof (SSC_AR_MATCHING_PROFILE));
  arMatchingProfileOut->dibMatch = (DIB_MATCH_CTRL *) chk_calloc (1, sizeof (DIB_MATCH_CTRL));

  /* set default parameters */

  strcpy (arMatchingProfileOut->name, "UNKNOWN");

  dibMatch = arMatchingProfileOut->dibMatch;
  dibMatch->match_allow_missing_ae_elements = SD_FALSE;
  dibMatch->match_allow_extra_ae_elements   = SD_TRUE;
  dibMatch->match_ap_title     = SD_TRUE;
  dibMatch->match_ae_qualifier = SD_TRUE;
  dibMatch->match_ap_invoke    = SD_FALSE;
  dibMatch->match_ae_invoke    = SD_FALSE;
  dibMatch->match_psel         = SD_TRUE;
  dibMatch->match_ssel         = SD_TRUE;
  dibMatch->match_tsel         = SD_TRUE;
  dibMatch->match_net_addr     = SD_FALSE;
  dibMatch->use_gethostbyname  = SD_FALSE;	/* gethostbyname() call may take LONG time	*/

  list_add_last (&sscStackCfg->arMatchingProfileList, arMatchingProfileOut);

  return (arMatchingProfileOut);
}

/************************************************************************/
/*		sscAddSecurityProfile					*/
/************************************************************************/
SSC_SECURITY_PROFILE *sscAddSecurityProfile (SSC_STACK_CFG *sscStackCfg)
{
SSC_SECURITY_PROFILE *securityProfileOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  securityProfileOut = (SSC_SECURITY_PROFILE *)  chk_calloc (1, sizeof (SSC_SECURITY_PROFILE));

  /* set default parameters */

  strcpy (securityProfileOut->name, "UNKNOWN");

  securityProfileOut->secure = SD_FALSE;	/* not secured by default */
  securityProfileOut->nonSecureFallbackEnabled = SD_FALSE;
  securityProfileOut->maceTimeSealWindow = 600;	/* seconds		*/
  securityProfileOut->encryptReqCalled   = SD_FALSE;
  securityProfileOut->encryptReqCalling	 = SD_FALSE;

  list_add_last (&sscStackCfg->securityProfileList, securityProfileOut);

  return (securityProfileOut);
}

/************************************************************************/
/*		sscAddAppNetwork					*/
/************************************************************************/
SSC_APP_NETWORK *sscAddAppNetwork (SSC_STACK_CFG *sscStackCfg)
{
SSC_APP_NETWORK *appNetworkOut = NULL;

  if (!sscStackCfg)
    return (NULL);

  appNetworkOut = (SSC_APP_NETWORK *)  chk_calloc (1, sizeof (SSC_APP_NETWORK));

  /* set default parameters */

  strcpy (appNetworkOut->appName, "UNKNOWN");

  appNetworkOut->secEventLogFileSize = 2000000;		/* default size */

  list_add_last (&sscStackCfg->appNetworkList, appNetworkOut);

  return (appNetworkOut);
}

	/* =============================================================*/
	/* 	SISCO Stack Configuration Verify functions		*/
	/* =============================================================*/
	/* Functions to verify SISCO's Stack Configuration parameters.	*/
	/* Only elements that are needed for given application are	*/
	/* checked. All these verify functions are called fron the	*/
	/* user interface function sscVerifyAppNetwork.			*/
	/* =============================================================*/

/************************************************************************/
/*		sscVerifyLeantProfile					*/
/*----------------------------------------------------------------------*/
/* Mandatory for all applications.					*/
/************************************************************************/
ST_RET sscVerifyLeantProfile (SSC_STACK_CFG *sscStackCfg)
{
ST_RET             rtn = SD_SUCCESS;
SSC_LEANT_PROFILE *leantProfile = NULL;
ST_BOOLEAN         appSnapLite = SD_FALSE;	/* extra checking is done for SNAP-Lite */

  if (!sscStackCfg || !sscStackCfg->appNetwork)
    return (SD_FAILURE);
  leantProfile = sscStackCfg->appNetwork->leantProfile;

  if (strcmp (sscStackCfg->appNetwork->appName, SNAPL_CFG_APP_NAME) == 0)
    appSnapLite = SD_TRUE;

  if (!leantProfile)
    {
    SSC_LOG_ERR1 ("SSC ERROR: mandatory leantProfile not configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  if (!leantProfile->tcpEnable && !leantProfile->tp4Enable)
    {
    SSC_LOG_ERR2 ("SSC ERROR: TCP or TP4 must be enabled in the leantProfile.name='%s'\n"
                  "  for appName='%s'", leantProfile->name, SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  if (appSnapLite)
    {
    if (!leantProfile->tcpEnable)
      {
      SSC_LOG_ERR2 ("SSC ERROR: TCP be enabled in the leantProfile.name='%s'\n"
                    "  for appName='%s'", leantProfile->name, SNAPL_CFG_APP_NAME);
      return (SD_FAILURE);
      }
    if (leantProfile->tp4Enable)
      {
      SSC_LOG_ERR1 ("SSC ERROR: TP4 not supported in appName='%s'", SNAPL_CFG_APP_NAME);
      return (SD_FAILURE);
      }
    }

  if (leantProfile->tcpEnable)
    {
    if (leantProfile->tcpListenPort == 0)
      {
      leantProfile->tcpListenPort = RFC1006_PORT;
      SSC_LOG_FLOW1 ("SSC INFO: defaulting TCP Listen Port to %u.", RFC1006_PORT);
      }
    }

  if (leantProfile->tp4Enable)
    {
    if (strlen (leantProfile->adapterName) == 0)
      {
      SSC_LOG_ERR2 ("SSC ERROR: Adapter_Name not specified in the leantProfile.name='%s'\n"
                    "  for appName='%s'", leantProfile->name, SSC_APP_NAME(sscStackCfg));
      return (SD_FAILURE);
      }

    if (!sscFindNetworkAdapter (sscStackCfg, leantProfile->adapterName))
      {
      SSC_LOG_ERR2 ("SSC ERROR: leantProfile.adapterName='%s' not configured for appName='%s'",
                    leantProfile->adapterName, SSC_APP_NAME(sscStackCfg));
      return (SD_FAILURE);
      }
    }

  /* TO-DO: add more checking if necessary */

  return (rtn);
}

/************************************************************************/
/*		sscVerifySecurityProfile				*/
/*----------------------------------------------------------------------*/
/* Mandatory for all applications.					*/
/************************************************************************/
ST_RET sscVerifySecurityProfile (SSC_STACK_CFG *sscStackCfg)
{
ST_RET                rtn = SD_SUCCESS;
SSC_SECURITY_PROFILE *securityProfile = NULL;

  if (!sscStackCfg || !sscStackCfg->appNetwork)
    return (SD_FAILURE);
  securityProfile = sscStackCfg->appNetwork->securityProfile;

  if (!securityProfile)
    {
    SSC_LOG_ERR1 ("SSC ERROR: mandatory securityProfile not configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  if (!sscStackCfg->secureDllLib && securityProfile->secure)
    {
    SSC_LOG_ERR2 ("SSC ERROR: NULL Security does support secure securityProfile='%s' for appName='%s'",
                  securityProfile->name, SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  if (securityProfile->secure && securityProfile->maceTimeSealWindow == 0)
    {
    SSC_LOG_ERR2 ("SSC ERROR: Invalid maceTimeSealWindow=%d for appName='%s'",
                  securityProfile->maceTimeSealWindow, SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  /* TO-DO: add more checking if necessary */

  return (rtn);
}

/************************************************************************/
/*		sscVerifyArMatchingProfile				*/
/*----------------------------------------------------------------------*/
/* Mandatory for all applications (exept SNAP-Lite).			*/
/************************************************************************/
ST_RET sscVerifyArMatchingProfile (SSC_STACK_CFG *sscStackCfg)
{
ST_RET                  rtn = SD_SUCCESS;
SSC_AR_MATCHING_PROFILE *arMatchingProfile = NULL;

  if (!sscStackCfg || !sscStackCfg->appNetwork)
    return (SD_FAILURE);
  arMatchingProfile = sscStackCfg->appNetwork->arMatchingProfile;

  if (!arMatchingProfile)
    {
    SSC_LOG_ERR1 ("SSC ERROR: mandatory arMatchingProfile not configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  /* TO-DO: add more checking if necessary */

  return (rtn);
}

#if defined(S_SEC_ENABLED)
/************************************************************************/
/*		sscVerifyCertParam					*/
/*----------------------------------------------------------------------*/
/* Mandatory for all applications (if secure).				*/
/************************************************************************/
ST_RET sscVerifyCertParam (SSC_STACK_CFG *sscStackCfg)
{
ST_RET           rtn = SD_SUCCESS;
SSC_CERT_PARAM  *certParam = NULL;

  if (!sscStackCfg)
    return (SD_FAILURE);
  certParam = sscStackCfg->certParam;

  if (!certParam)
    {
    SSC_LOG_ERR0 ("SSC ERROR: certParam not configured.");
    return (SD_FAILURE);
    }
    /* check for certificates expiration time limit (1 year = 8784 hours) */
  if (certParam->certExpirationAlarmLimit > 10000)
    {
    SSC_LOG_ERR1 ("SSC ERROR: invalid certExpirationAlarmLimit=%u (max allowed 10,000 hours).",
                  certParam->certExpirationAlarmLimit);
    return (SD_FAILURE);
    }
  if (certParam->certExpirationEvalInterval > certParam->certExpirationAlarmLimit)
    {
    SSC_LOG_ERR2 ("SSC ERROR: certExpirationEvalInterval=%u > certExpirationAlarmLimit=%u.",
                  certParam->certExpirationEvalInterval, certParam->certExpirationAlarmLimit);
    return (SD_FAILURE);
    }

  if (certParam->crlCheckTime == 0)
    SSC_LOG_NERR0 ("SSC WARNING: Certifiacte Revocation List checking not enabled (crlCheckTime=0).");

  return (rtn);
}

/************************************************************************/
/*		sscVerifyCaCertificates					*/
/*----------------------------------------------------------------------*/
/* This function will be called from secured application to verify	*/
/* CA Certificates. Computes CRC-32 cfgId on the certificates (needed	*/
/* for internal use).							*/
/************************************************************************/
ST_RET sscVerifyCaCertificates (SSC_STACK_CFG *sscStackCfg)
{
ST_RET       rtn = SD_SUCCESS;
S_CERT_CTRL *certCtrl;
S_CERT_CTRL *certCtrlTmp;
ST_UINT32    cfgId;

  if (!sscStackCfg)
    return (SD_FAILURE);

  certCtrl = sscStackCfg->trustedCaCertList;
  while (certCtrl)
    {
    if (!certCtrl->certId.certName || strlen(certCtrl->certId.certName) == 0)
      {
      ST_CHAR subjName[MAX_LOG_SIZE-20]; /* use default slog size to fit all apps */
      subjName[0] = 0;
      S_LOCK_UTIL_RESOURCES ();  /* to avoid deadlock with sslEngine's gs_util_mutex */
      sslePrintSubject (&certCtrl->certId, subjName, sizeof(subjName));
      SSC_LOG_ERR0 ("SSC ERROR: CA certificate is missing certName\n");
      SSC_LOG_CERR1 ("  Subject:\n'%s'", subjName);
      S_UNLOCK_UTIL_RESOURCES ();
      rtn = SD_FAILURE;
      break;
      }
    /* certificate that has all the SerialNumber, Issuer, and Subject set to Enable=No	*/
    /* is useless and should not be considered as properly configured.			*/
    if ( certCtrl->certId.serialLen <= 0 &&
        !certCtrl->certId.issuerCName && !certCtrl->certId.subjectCName)
      {
      SSC_LOG_ERR1 ("SSC ERROR: CA certificate can't be used (all attributes SerialNumber, \n"
                    "  Issuer, and Subject are disabled) in 'CertName'='%s'.", certCtrl->certId.certName);
      rtn = SD_FAILURE;
      break;
      }
    /* make sure cfgId is unique (used in update functions)		*/
    cfgId = secCfgComputeCertCrc32 (certCtrl);
    if (cfgId == 0)
      {
      SSC_LOG_ERR1 ("SSC ERROR: invalid CA certificate CRC-32 (cfgId=0)\n"
                    "  in 'CertName'='%s'.", certCtrl->certId.certName);
      rtn = SD_FAILURE;
      break;
      }
    /* check if there are any certificates with the same CRC-32 */
    certCtrlTmp = secCfgFindCertCfgId (sscStackCfg->trustedCaCertList, cfgId);
    if (certCtrlTmp)
      {
      /* NOTE: this can happen if user disables some fields in certificate like: 	*/
      /*       Serial Number, Issuer, or Subject 					*/
      SSC_LOG_ERR2 ("SSC ERROR: following CA certificates produce the same CRC-32 (cfgId)\n"
                    "  'CertName'='%s' and 'CertName'='%s'.",
                    certCtrl->certId.certName, certCtrlTmp->certId.certName);
      rtn = SD_FAILURE;
      break;	/* we should not continue since we like to have unique certificates */
      }
    else
      /* this cfgId is unique */
      certCtrl->cfgId = cfgId;

    /* check if given certificate's name already exists (maybe with new serial number)	*/
    certCtrlTmp = secCfgFindCertCtrl (sscStackCfg->trustedCaCertList, certCtrl->certId.certName);
    if (certCtrlTmp && (certCtrlTmp != certCtrl))
      {
      SSC_LOG_ERR1 ("SSC ERROR: two CA certificates found with the same 'CertName'='%s'.",
                    certCtrl->certId.certName);
      rtn = SD_FAILURE;
      break;	/* we should not continue since we like to have unique certificate names */
      }

    certCtrl = (S_CERT_CTRL *) list_get_next (sscStackCfg->trustedCaCertList, certCtrl);
    } /* end while loop */

  return (rtn);
}

/************************************************************************/
/*		sscVerifyCertificates					*/
/*----------------------------------------------------------------------*/
/* This function will be called from secured application to verify	*/
/* Certificates (SSL & MACE local/remote).				*/
/* Computes CRC-32 cfgId on the certificates (needed for internal use).	*/
/************************************************************************/
ST_RET sscVerifyCertificates (SSC_STACK_CFG *sscStackCfg)
{
ST_RET       rtn = SD_SUCCESS;
S_CERT_CTRL *certCtrl;
S_CERT_CTRL *certCtrlTmp;
ST_UINT32    cfgId;

  if (!sscStackCfg)
    return (SD_FAILURE);

  /* if the app is secured some certs must be configured */
  if (!sscStackCfg->certificateList)
    {
    SSC_LOG_ERR1 ("SSC ERROR: certificateList not configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  certCtrl = sscStackCfg->certificateList;
  while (certCtrl)
    {
    if (!certCtrl->certId.certName || strlen(certCtrl->certId.certName) == 0)
      {
      ST_CHAR subjName[MAX_LOG_SIZE-20]; /* use default slog size to fit all apps */
      subjName[0] = 0;
      S_LOCK_UTIL_RESOURCES ();  /* to avoid deadlock with sslEngine's gs_util_mutex */
      sslePrintSubject (&certCtrl->certId, subjName, sizeof(subjName));
      SSC_LOG_ERR0 ("SSC ERROR: Certificate is missing certName\n");
      SSC_LOG_CERR1 ("  Subject:\n'%s'", subjName);
      S_UNLOCK_UTIL_RESOURCES ();
      rtn = SD_FAILURE;
      break;
      }
    /* certificate that has all the SerialNumber, Issuer, and Subject set to Enable=No	*/
    /* is useless and should not be considered as properly configured;			*/
    /* such certificate can not be used in matching with a peer certificate.		*/
    if ( certCtrl->certId.serialLen <= 0 &&
        !certCtrl->certId.issuerCName && !certCtrl->certId.subjectCName)
      {
      SSC_LOG_ERR1 ("SSC ERROR: certificate can't be used to match peer certificates \n"
                    "  (all attributes SerialNumber, Issuer, and Subject are disabled)\n"
                    "  in 'CertName'='%s'.", certCtrl->certId.certName);
      rtn = SD_FAILURE;
      break;
      }

    /* Compute 32-bit CRC on all the enabled certificate fields.		*/
    cfgId = secCfgComputeCertCrc32 (certCtrl);
    if (cfgId == 0)
      {
      SSC_LOG_ERR1 ("SSC ERROR: invalid certificate CRC-32 (cfgId=0)\n"
                    "  in 'CertName'='%s'.", certCtrl->certId.certName);
      rtn = SD_FAILURE;
      break;
      }
    /* check if there are any certificates with the same CRC */
    certCtrlTmp = secCfgFindCertCfgId (sscStackCfg->certificateList, cfgId);
    if (certCtrlTmp && (certCtrlTmp != certCtrl))
      {
      /* NOTE: this can happen if user disables some fields in certificate like: 	*/
      /*       Serial Number, Issuer, or Subject 					*/
      SSC_LOG_ERR2 ("SSC ERROR: following certificates produce the same CRC-32 (cfgId)\n"
                    "  'CertName'='%s' and 'CertName'='%s'.",
                    certCtrl->certId.certName, certCtrlTmp->certId.certName);
      rtn = SD_FAILURE;
      break;	/* we should not continue since we like to have unique certificates */
      }
    else
      /* this cfgId is unique */
      certCtrl->cfgId = cfgId;

    /* check if given certificate's name already exists (maybe with new serial number)	*/
    certCtrlTmp = secCfgFindCertCtrl (sscStackCfg->certificateList, certCtrl->certId.certName);
    if (certCtrlTmp && (certCtrlTmp != certCtrl))
      {
      SSC_LOG_ERR1 ("SSC ERROR: two certificates found with the same 'CertName'='%s'.",
                    certCtrl->certId.certName);
      rtn = SD_FAILURE;
      break;	/* we should not continue since we like to have unique certificate names */
      }

    certCtrl = (S_CERT_CTRL *) list_get_next (sscStackCfg->certificateList, certCtrl);
    } /* end while loop */

  return (rtn);
}

/************************************************************************/
/*		sscVerifySslParam					*/
/*----------------------------------------------------------------------*/
/* Mandatory for SNAP-Lite application.					*/
/* Security maybe ON but there can be case where only non-secure cons	*/
/* are configured. We have to allow such case scenario.			*/
/************************************************************************/
ST_RET sscVerifySslParam (SSC_STACK_CFG *sscStackCfg)
{
ST_RET          rtn = SD_SUCCESS;
SSC_SSL_PARAM  *sslParam;
ST_BOOLEAN	appSnapLite = SD_FALSE;

  if (!sscStackCfg || !sscStackCfg->appNetwork)
    return (SD_FAILURE);
  sslParam = sscStackCfg->sslParam;

  if (strcmp (sscStackCfg->appNetwork->appName, SNAPL_CFG_APP_NAME) == 0)
    appSnapLite = SD_TRUE;

  if (appSnapLite)
    {
    if (!sslParam)
      {
      SSC_LOG_ERR1 ("SSC ERROR: sslParam not configured for appName='%s'",
                    SSC_APP_NAME(sscStackCfg));
      return (SD_FAILURE);
      }
  
    /* no SSL cert OK for non-secure or MACE only connections */
    if (sslParam->sslLocalCertCtrlArrCnt == 0)
      SSC_LOG_FLOW0 ("SSC WARNING: local SSL certificate not configured.");
      /* maybe user wants only non-secure connections at this time */
  
    if (sslParam->sslCalledPort == 0)
      SSC_LOG_FLOW0 ("SSC WARNING: SSL Called Port (0) - will not accept incomimg secure SSL connections.");
  
    if (sslParam->sslCallingPort == 0)
      {
      sslParam->sslCallingPort = S_SEC_DEF_SSL_PORT;
      SSC_LOG_FLOW1 ("SSC WARNING: invalid SSL Calling Port (0), defaulting to %d.", S_SEC_DEF_SSL_PORT);
      }
    /* only warnings, user may want to run with these parameters anyway	*/
    if (sslParam->maxTranPerKey == 0 && sslParam->maxTimePerKey == 0)
      SSC_LOG_NERR0 ("SSC WARNING: SSL Session Rekeying not in force (maxTranPerKey=0 and maxTimePerKey=0).");
    if (sslParam->rekeyTimeout <= 0)
      {
      SSC_LOG_NERR1 ("SSC WARNING: invalid SSL Session Rekey Timeout (%d), defaulting to 60 seconds.", sslParam->rekeyTimeout);
      sslParam->rekeyTimeout = 60;		/* in seconds */
      }
    if (sslParam->cipherArrCnt <= 0)
      {
      SSC_LOG_NERR1 ("SSC WARNING: invalid number of Ciphers (%d) needed for SSL connections.", sslParam->cipherArrCnt);
      sslParam->cipherArrCnt = 0;
      }
    }

  return (rtn);
}
#endif	/* defined(S_SEC_ENABLED)	*/

/************************************************************************/
/*		sscVerifySnapParam					*/
/*----------------------------------------------------------------------*/
/* Mandatory for SNAP-Lite.						*/
/************************************************************************/
ST_RET sscVerifySnapParam (SSC_STACK_CFG *sscStackCfg, ST_BOOLEAN secureApp)
{
ST_RET          rtn = SD_SUCCESS;
SSC_SNAP_PARAM *snapParam = NULL;

  if (!sscStackCfg)
    return (SD_FAILURE);
  snapParam = sscStackCfg->snapParam;

  if (!snapParam)
    {
    SSC_LOG_ERR1 ("SSC ERROR: snapParam not configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  if (secureApp)
    {
    if (snapParam->sslMaxCalled == 0)
      SSC_LOG_FLOW0 ("SSC WARNING: unlimited number of secure connections configured (sslMaxCalled=0)");
    if (snapParam->sslMaxCalling == 0)
      SSC_LOG_FLOW0 ("SSC WARNING: unlimited number of secure connections configured (sslMaxCalling=0)");

    if (snapParam->sslProxyCalledPort == 0)
      {
      SSC_LOG_ERR0 ("SSC ERROR: invalid SSL Proxy Called Port (0).");
      return (SD_FAILURE);
      }
    if (snapParam->sslProxyCallingPort == 0)
      {
      SSC_LOG_ERR0 ("SSC ERROR: invalid SSL Proxy Calling Port (0)");
      return (SD_FAILURE);
      }
    if (snapParam->sslProxyConnectTimeout == 0)
      {
      SSC_LOG_ERR0 ("SSC ERROR: invalid SSL Proxy Connection Timeout (0)");
      return (SD_FAILURE);
      }
    }

  if (snapParam->maxUsers == 0)
    {
    SSC_LOG_ERR0 ("SSC ERROR: invalid Max Users (0).");
    return (SD_FAILURE);
    }
  if (snapParam->maxBindsPerUser == 0)
    {
    SSC_LOG_ERR0 ("SSC ERROR: invalid Max Binds Per User (0).");
    return (SD_FAILURE);
    }
#if defined(_WIN32)
  if (snapParam->ipcPort == 0)
    {
    SSC_LOG_ERR0 ("SSC ERROR: invalid IPC Port (0)");
    return (SD_FAILURE);
    }
#else   /* UNIX/Linux */
  if (strlen (snapParam->ipcDir) == 0)
    {
    SSC_LOG_ERR0 ("SSC ERROR: invalid IPC Domain Socket Path (empty)");
    return (SD_FAILURE);
    }
#endif  /* UNIX/Linux */
  if (snapParam->ipcTimeout == 0)
    {
    SSC_LOG_ERR0 ("SSC ERROR: invalid IPC Timeout (0)");
    return (SD_FAILURE);
    }

  return (rtn);
}

/************************************************************************/
/*		sscVerifyArName						*/
/************************************************************************/
ST_RET sscVerifyArName (SSC_STACK_CFG *sscStackCfg, SSC_AR_NAME *sscArName)
{
ST_RET                rtn = SD_SUCCESS;
SSC_LEANT_PROFILE    *leantProfile;
SSC_SECURITY_PROFILE *securityProfile = NULL;
SSC_APP_NETWORK      *appNetwork = NULL;
DIB_ENTRY            *arDib;
S_SEC_LOC_REM_AR     *arSec;
PRES_ADDR            *pres_addr;

  if (!sscStackCfg || !sscStackCfg->appNetwork)
    return (SD_FAILURE);
  appNetwork      = sscStackCfg->appNetwork;
  leantProfile    = appNetwork->leantProfile;
  securityProfile = appNetwork->securityProfile;

  if (!leantProfile)
    {
    SSC_LOG_ERR1 ("SSC ERROR: mandatory leantProfile not configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }
  if (!securityProfile)
    {
    SSC_LOG_ERR1 ("SSC ERROR: mandatory securityProfile not configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  /* do basic AR Name checks */
  arDib = sscArName->arDib;
  if (!arDib)
    {
    SSC_LOG_ERR0 ("SSC ERROR: sscVerifyArName failed (arDib=NULL in an ArName).");
    return (SD_FAILURE);
    }
  if (strlen(arDib->name) == 0)
    {
    SSC_LOG_ERR0 ("SSC ERROR: sscVerifyArName failed (ArName without a name found).");
    return (SD_FAILURE);
    }
  pres_addr = &arDib->pres_addr;
  if (pres_addr->tp_type == TP_TYPE_TP4 && !leantProfile->tp4Enable)
    {
    SSC_LOG_ERR1 ("SSC ERROR: LEAN-T TP4 profile not configured for ArName='%s'.",
                  arDib->name);
    return (SD_FAILURE);
    }
  if (pres_addr->tp_type == TP_TYPE_TP4 && pres_addr->nsap_len == 0)
    {
    SSC_LOG_ERR1 ("SSC ERROR: NSAP not configured for ArName='%s'.",
                  arDib->name);
    return (SD_FAILURE);
    }

  /* check the ArName security params */
  arSec = sscArName->arSec;
  if (!arSec)
    {
    SSC_LOG_ERR1 ("SSC ERROR: sscVerifyArName failed (arSec=NULL in ArName='%s').",
                  arDib->name);
    return (SD_FAILURE);
    }
  if (strlen(arSec->arName) == 0)
    {
    SSC_LOG_ERR1 ("SSC ERROR: ArSec without a name found for ArName='%s'.",
                  arDib->name);
    return (SD_FAILURE);
    }
  if (strcmp(arDib->name, arSec->arName) != 0)
    {
    SSC_LOG_ERR2 ("SSC ERROR: arDib.name='%s' different than arSec.arName='%s'.",
                  arDib->name, arSec->arName);
    return (SD_FAILURE);
    }

  if (!securityProfile->secure &&
      (arSec->encryptMode == S_SEC_ENCRYPT_SSL   ||
        arSec->arAuthMode == S_AR_AUTH_MODE_SSL	 ||
	arSec->arAuthMode == S_AR_AUTH_MODE_MACE))
    {
    SSC_LOG_ERR2 ("SSC ERROR: SSL Encryption, or SSL/MACE authentication configured \n"
                  "  in ArName='%s' for Non-Secure Application '%s'.",
                  arSec->arName, appNetwork->appName);
    return (SD_FAILURE);
    }
  if (arSec->encryptMode == S_SEC_ENCRYPT_SSL)
    {
    /* encryption requested */
    if (!sscStackCfg->sslParam || sscStackCfg->sslParam->sslLocalCertCtrlArrCnt == 0)
      {
      SSC_LOG_ERR1 ("SSC ERROR: Local SSL Certificate not configured (required to use SSL Encryption)\n"
                    "  in ArName='%s' ", arSec->arName);
      return (SD_FAILURE);
      }
    }
  if (arSec->arAuthMode == S_AR_AUTH_MODE_SSL)
    {
    if (!sscStackCfg->sslParam || sscStackCfg->sslParam->sslLocalCertCtrlArrCnt == 0)
      {
      SSC_LOG_ERR1 ("SSC ERROR: Local SSL Certificate not configured (required to use SSL Authentication)\n"
                    "  in ArName='%s' ", arSec->arName);
      return (SD_FAILURE);
      }
    /* to use SSL authentication the connection must be encrypted */
    if (arSec->encryptMode != S_SEC_ENCRYPT_SSL)
      {
      SSC_LOG_ERR1 ("SSC ERROR: Encryption must be enabled to use SSL Authentication in ArName='%s' ", arSec->arName);
      return (SD_FAILURE);
      }
    }
  /* if the AR Name references certificates check if they are configured */
  if ((arSec->arAuthMode == S_AR_AUTH_MODE_MACE || 
       arSec->arAuthMode == S_AR_AUTH_MODE_SSL) &&
       arSec->authCertArrCnt == 0)
    {
    SSC_LOG_ERR1 ("SSC ERROR: Authentication Certificates not configured in ArName='%s'.",
                  arSec->arName);
    return (SD_FAILURE);
    }

  return (rtn);
}

/************************************************************************/
/*		sscVerifyAppArNameList					*/
/* This function verifies ArNames that are used by the curr application.*/
/************************************************************************/
ST_RET sscVerifyAppArNameList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET           rtn = SD_SUCCESS;
SSC_APP_NETWORK *appNetwork = NULL;
SSC_SSL_PARAM   *sslParam = NULL;
SSC_APP_AR_NAME *appArNameList;
SSC_APP_AR_NAME *appArName;
SSC_AR_NAME     *sscArName;
ST_BOOLEAN       localArNameFound = SD_FALSE;	/* at least one local should be configured */

  if (!sscStackCfg || !sscStackCfg->appNetwork)
    return (SD_FAILURE);
  appNetwork    = sscStackCfg->appNetwork;
  appArNameList = sscStackCfg->appNetwork->appArNameList;
  sslParam      = sscStackCfg->sslParam;

  if (!appArNameList)
    {
    SSC_LOG_ERR1 ("SSC ERROR: appArNameList not configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    return (SD_FAILURE);
    }

  /* go through the list of app AR Names and check if they have been configured */
  appArName = appArNameList;
  while (appArName)
    {
    if (appArName->role != SSC_AR_NAME_ROLE_LOCAL &&
        appArName->role != SSC_AR_NAME_ROLE_REMOTE)
      {
      SSC_LOG_ERR2 ("SSC ERROR: appArName='%s' Role (Local or Remote) not configured for appName='%s'",
                    appArName->arName, SSC_APP_NAME(sscStackCfg));
      rtn = SD_FAILURE;
      break;
      }

    sscArName = sscFindArName (sscStackCfg, appArName->arName);
    if (!sscArName)
      {
      SSC_LOG_ERR2 ("SSC ERROR: appArName='%s' not configured for appName='%s'",
                    appArName->arName, SSC_APP_NAME(sscStackCfg));
      rtn = SD_FAILURE;
      break;
      }

    if (appArName->role == SSC_AR_NAME_ROLE_LOCAL)
      localArNameFound = SD_TRUE;

    rtn = sscVerifyArName (sscStackCfg, sscArName);
    if (rtn != SD_SUCCESS)
      break;

    /* add the AR Name used by this application to the appNetwork lists,	*/
    /* note that we need to split into two diff lists for the application use	*/
    list_add_last (&appNetwork->dibEntryList, sscArName->arDib);
    list_add_last (&appNetwork->arLocRemSecList, sscArName->arSec);

    appArName = (SSC_APP_AR_NAME *) list_get_next (appArNameList, appArName);
    }

  if (rtn == SD_SUCCESS && !localArNameFound)
    {
    SSC_LOG_ERR1 ("SSC ERROR: there are no local AR Names configured for appName='%s'",
                  SSC_APP_NAME(sscStackCfg));
    rtn = SD_FAILURE;
    }

  return (rtn);
}


/************************************************************************/
/*			sscVerifyAppNetworkAllApps			*/
/*----------------------------------------------------------------------*/
/* User function to verify integrity of the whole sscStackCfg.		*/
/* Return:								*/
/*	SD_SUCCESS	if function successful,				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/
ST_RET sscVerifyAppNetworkAllApps (SSC_STACK_CFG *sscStackCfg)
{
ST_RET                rtn = SD_FAILURE;
SSC_APP_NETWORK      *appNetwork;

  if (!sscStackCfg)
    return (rtn);

  /* verify the cfg for all applications */
  appNetwork = sscStackCfg->appNetworkList;
  while (appNetwork)
    {
    rtn = sscVerifyAppNetworkEx (sscStackCfg, appNetwork->appName, SD_TRUE);
    if (rtn != SD_SUCCESS)
      break;
    appNetwork = list_get_next (sscStackCfg->appNetworkList, appNetwork);
    }
  return (rtn);
}

/************************************************************************/
/*			sscVerifyAppNetwork				*/
/*----------------------------------------------------------------------*/
/* User function to verify integrity of the sscStackCfg.		*/
/* Only elements that are needed for given application (appName) are	*/
/* checked.								*/
/* Return:								*/
/*	SD_SUCCESS	if function successful,				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/
ST_RET sscVerifyAppNetwork (SSC_STACK_CFG *sscStackCfg, ST_CHAR *appName)
{
ST_RET rtn;

  rtn = sscVerifyAppNetworkEx (sscStackCfg, appName, SD_FALSE);

  return (rtn);
}

/************************************************************************/
/*			sscVerifyAppNetworkEx				*/
/*----------------------------------------------------------------------*/
/* User function to verify integrity of the sscStackCfg.		*/
/* Only elements that are needed for given application (appName) are	*/
/* checked.								*/
/* Return:								*/
/*	SD_SUCCESS	if function successful,				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/
#if defined(S_SEC_ENABLED)
/* Note: Moved out from function below. On Sun can not ifdef out static	*/
/*       var on function stack.						*/
static ST_BOOLEAN     bVerified = SD_FALSE;	/* if bVerifyAll=SD_TRUE do not verify some	*/
						/* sections again if they are 'global' to cfg	*/
#endif
ST_RET sscVerifyAppNetworkEx (SSC_STACK_CFG *sscStackCfg, ST_CHAR *appName, ST_BOOLEAN bVerifyAll)
{
ST_RET                rtn = SD_FAILURE;
SSC_APP_NETWORK      *appNetwork;
ST_BOOLEAN            appSnapLite = SD_FALSE;	/* extra checking is done for SNAP-Lite */
ST_BOOLEAN            secureApp = SD_FALSE;

  if (!sscStackCfg)
    {
    SSC_LOG_ERR0 ("SSC ERROR: sscVerifyAppNetwork failed sscStackCfg=NULL.");
    return (SD_FAILURE);
    }

#if defined(S_SEC_ENABLED)
  /* This function is in DLL.	*/
  sscStackCfg->secureDllLib = ssleSecuritySupported ();
#else
  sscStackCfg->secureDllLib = SD_FALSE;	/* no DLL, so can't be secure	*/
#endif

  /* set the pointer to application (or SNAP-Lite) being configured */
  sscStackCfg->appNetwork = appNetwork = sscFindAppNetwork (sscStackCfg, appName);
  if (!appNetwork)
    {
    SSC_LOG_ERR1 ("SSC ERROR: appNetwork not configured for appName='%s'.", appName);
    return (rtn);
    }
  if (strcmp (appName, SNAPL_CFG_APP_NAME) == 0)
    appSnapLite = SD_TRUE;

  /* check if the leantProfile present (required) */
  rtn = sscVerifyLeantProfile (sscStackCfg);
  if (rtn != SD_SUCCESS)
    return (rtn);

  /* verify Security Profiles */
  rtn = sscVerifySecurityProfile (sscStackCfg);
  if (rtn != SD_SUCCESS)
    return (rtn);
  secureApp = appNetwork->securityProfile->secure;

#if defined(S_SEC_ENABLED)	/* if not enabled, secureApp always FALSE here*/
  /* verify security objects only if secure mode requested */
  if (secureApp && (!bVerified))
    {
    /* verify general certificates related parameters */
    rtn = sscVerifyCertParam (sscStackCfg);
    if (rtn != SD_SUCCESS)
      return (rtn);

    /* verify CA certificates */
    rtn =  sscVerifyCaCertificates (sscStackCfg);
    if (rtn != SD_SUCCESS)
      return (rtn);

    /* verify local/remote certificates */
    rtn = sscVerifyCertificates (sscStackCfg);
    if (rtn != SD_SUCCESS)
      return (rtn);

    /* verify params for SSL connections */
    rtn = sscVerifySslParam (sscStackCfg);
    if (rtn != SD_SUCCESS)
      return (SD_FAILURE);

    if (bVerifyAll)
      bVerified = SD_TRUE;
    }
#endif	/* defined(S_SEC_ENABLED)	*/

  /* verify SNAP-Lite or application parameters */
  if (appSnapLite)
    {
    rtn = sscVerifySnapParam (sscStackCfg, secureApp);
    if (rtn != SD_SUCCESS)
      return (rtn);
    }
  else
    {
    rtn = sscVerifyAppArNameList (sscStackCfg);
    if (rtn != SD_SUCCESS)
      return (rtn);
    }

  SSC_LOG_FLOW1 ("SSC INFO: SISCO Stack Configuration verified for appName='%s'.", appName);

  return (rtn);
}


	/* =============================================================*/
	/* 	SISCO Stack Configuration Update functions		*/
	/* =============================================================*/


/************************************************************************/
/*		sscUpdateNetworkAdapter					*/
/*----------------------------------------------------------------------*/
/* This function is called to update Network Adapters List.		*/
/* Add new Network Adapter names to the current list.			*/
/* Existing Network Adapters remain unchanged.				*/
/************************************************************************/
ST_RET sscUpdateNetworkAdapter (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
ST_RET                rtn = SD_SUCCESS;
SSC_NETWORK_ADAPTER  *networkAdapterNewNext;
SSC_NETWORK_ADAPTER  *networkAdapterNew;
SSC_NETWORK_ADAPTER  *networkAdapter;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  if (sscStackCfgNew->networkAdapterList)
    SSC_LOG_CFLOW0 ("SSC Update: updating Network Adapters:");

  /* find the Network Adapter in the networkAdapterList */
  networkAdapterNew = sscStackCfgNew->networkAdapterList;
  while (networkAdapterNew)	/* list maybe empty */
    {
    networkAdapterNewNext = (SSC_NETWORK_ADAPTER *) list_get_next (sscStackCfgNew->networkAdapterList, networkAdapterNew);

    networkAdapter = sscFindNetworkAdapter (sscStackCfg, networkAdapterNew->name);
    if (networkAdapter == NULL)
      {
      /* add this network adapter to the current list */
      rtn = list_unlink (&sscStackCfgNew->networkAdapterList, networkAdapterNew);
      if (rtn == SD_SUCCESS)
        {
        list_add_last (&sscStackCfg->networkAdapterList, networkAdapterNew);
        SSC_LOG_CFLOW1 ("SSC Update: added Network Adapter '%s'", networkAdapterNew->name);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC Update ERROR: unable to unlink Network Adapter '%s'", networkAdapterNew->name);
	rtn = SD_FAILURE;
	break;
	}
      }
    else
      {
      /* else the Adapter name is already on the current list, skip it.	*/
      SSC_LOG_CFLOW1 ("SSC Update: skipping existing Network Adapter '%s' (update not supported)", networkAdapterNew->name);
      /* left over Network Adapters in the sscStackCfgNew will be freed later	*/
      }

    networkAdapterNew = networkAdapterNewNext;
    }

  return (rtn);
}

/************************************************************************/
/*		sscUpdateLeantProfile					*/
/*----------------------------------------------------------------------*/
/* This function is called to update Leant Profile List.		*/
/* Add new Leant Profile to the current list.				*/
/* Existing Leant Profiles remain unchanged.				*/
/************************************************************************/
ST_RET sscUpdateLeantProfile (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
ST_RET              rtn = SD_SUCCESS;
SSC_LEANT_PROFILE  *leantProfileNewNext;
SSC_LEANT_PROFILE  *leantProfileNew;
SSC_LEANT_PROFILE  *leantProfile;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  if (sscStackCfgNew->leantProfileList)
    SSC_LOG_CFLOW0 ("SSC Update: updating Leant Profiles:");
  else
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: at least one Leant profile expected.");
    return (SD_FAILURE);
    }

  /* find the Leant Profile in the leantProfileList */
  leantProfileNew = sscStackCfgNew->leantProfileList;
  while (leantProfileNew)  /* at least one profile expected */
    {
    leantProfileNewNext = (SSC_LEANT_PROFILE *) list_get_next (sscStackCfgNew->leantProfileList, leantProfileNew);

    leantProfile = sscFindLeantProfile (sscStackCfg, leantProfileNew->name);
    if (leantProfile == NULL)
      {
      /* add this Leant Profile to the current list */
      rtn = list_unlink (&sscStackCfgNew->leantProfileList, leantProfileNew);
      if (rtn == SD_SUCCESS)
        {
        list_add_last (&sscStackCfg->leantProfileList, leantProfileNew);
        SSC_LOG_CFLOW1 ("SSC Update: added Leant Profile '%s'", leantProfileNew->name);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC Update ERROR: unable to unlink Leant Profile '%s'", leantProfileNew->name);
	rtn = SD_FAILURE;
	break;
	}
      }
    else
      {
      /* else the Leant Profile name is already on the current list, skip it.	*/
      SSC_LOG_CFLOW1 ("SSC Update: skipping existing Leant Profile '%s' (update not supported)", leantProfileNew->name);
      /* left over Leant Profiles in the sscStackCfgNew will be freed later	*/
      }

    leantProfileNew = leantProfileNewNext;
    }

  return (rtn);
}

#if defined(S_SEC_ENABLED)
/************************************************************************/
/*		sscUpdateCertParam					*/
/*----------------------------------------------------------------------*/
/* This function is called to update general Certificates Parameters.	*/
/* Existing parameters will be updated to new values.			*/
/************************************************************************/
ST_RET sscUpdateCertParam (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
SSC_CERT_PARAM *certParamNew;
SSC_CERT_PARAM *certParam;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  certParam    = sscStackCfg->certParam;
  certParamNew = sscStackCfgNew->certParam;

  SSC_LOG_CFLOW0 ("SSC Update: updating general Certificate Parameters");

  /* certificates expiration checking */
  certParam->certExpirationAlarmLimit   = certParamNew->certExpirationAlarmLimit;
  certParam->certExpirationEvalInterval = certParamNew->certExpirationEvalInterval;
  /* SSL Certificate Revocation parameters */
  certParam->crlCheckTime               = certParamNew->crlCheckTime;
  certParam->crlDropExistingConns       = certParamNew->crlDropExistingConns;

  return (SD_SUCCESS);
}

/************************************************************************/
/*		sscUpdateCaCertificates					*/
/*----------------------------------------------------------------------*/
/* This function is called to update CA Certificates List.		*/
/* Add new CA Certificates to the current list.				*/
/* Existing CA Certificates remain unchanged.				*/
/************************************************************************/
ST_RET sscUpdateCaCertificates (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
ST_RET       rtn = SD_SUCCESS;
S_CERT_CTRL *certCtrlNewNext;
S_CERT_CTRL *certCtrlNew;
S_CERT_CTRL *certCtrl;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  SSC_LOG_CFLOW0 ("SSC Update: updating CA Certificates:");

  certCtrlNew = sscStackCfgNew->trustedCaCertList;
  while (certCtrlNew && rtn == SD_SUCCESS)
    {
    certCtrlNewNext = (S_CERT_CTRL *) list_get_next (sscStackCfgNew->trustedCaCertList, certCtrlNew);

    /* check if this CA Certificte is present in the active configuration	*/
    certCtrl = secCfgFindCertCfgId (sscStackCfg->trustedCaCertList, certCtrlNew->cfgId);
    if (certCtrl == NULL)
      {
      /* add this CA Certificate to active configuration			*/
      rtn = list_unlink (&sscStackCfgNew->trustedCaCertList, certCtrlNew);
      if (rtn == SD_SUCCESS)
        {
        list_add_last (&sscStackCfg->trustedCaCertList, certCtrlNew);
        SSC_LOG_CFLOW1 ("SSC Update: added CA Certificate '%s'", certCtrlNew->certId.certName);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC Update ERROR: unable to unlink CA Certificate '%s'", certCtrlNew->certId.certName);
	rtn = SD_FAILURE;
	break;
	}
      }
    else
      {
      /* else the CA Certificate is already on the current list, skip it.	*/
      SSC_LOG_CFLOW1 ("SSC Update: skipping CA Certificate '%s' (already in the list)", certCtrlNew->certId.certName);
      /* left over CA Certificates in the sscStackCfgNew will be freed later	*/
      }

    certCtrlNew = certCtrlNewNext;
    }

  return (rtn);
}

/************************************************************************/
/*		sscUpdateCertificates					*/
/*----------------------------------------------------------------------*/
/* This function is called to update loc/rem Certificates List.		*/
/* Add new loc/rem Certificates to the current list.			*/
/* Existing loc/rem Certificates remain unchanged.			*/
/************************************************************************/
ST_RET sscUpdateCertificates (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
ST_RET       rtn = SD_SUCCESS;
S_CERT_CTRL *certCtrlNewNext;
S_CERT_CTRL *certCtrlNew;
S_CERT_CTRL *certCtrl;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  SSC_LOG_CFLOW0 ("SSC Update: updating Certificates:");

  certCtrlNew = sscStackCfgNew->certificateList;
  while (certCtrlNew && rtn == SD_SUCCESS)
    {
    certCtrlNewNext = (S_CERT_CTRL *) list_get_next (sscStackCfgNew->certificateList, certCtrlNew);

    /* check if this Certificte is present in the active configuration	*/
    certCtrl = secCfgFindCertCfgId (sscStackCfg->certificateList, certCtrlNew->cfgId);
    if (certCtrl == NULL)
      {
      /* add this Certificate to active configuration			*/
      rtn = list_unlink (&sscStackCfgNew->certificateList, certCtrlNew);
      if (rtn == SD_SUCCESS)
        {
        list_add_last (&sscStackCfg->certificateList, certCtrlNew);
        SSC_LOG_CFLOW1 ("SSC Update: added Certificate '%s'", certCtrlNew->certId.certName);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC Update ERROR: unable to unlink Certificate '%s'", certCtrlNew->certId.certName);
	rtn = SD_FAILURE;
	break;
	}
      }
    else
      {
      /* else the Certificate is already on the current list, skip it.	*/
      SSC_LOG_CFLOW1 ("SSC Update: skipping Certificate '%s' (already in the list)", certCtrlNew->certId.certName);
      /* left over Certificates in the sscStackCfgNew will be freed later	*/
      }

    certCtrlNew = certCtrlNewNext;
    }

  return (rtn);
}

/************************************************************************/
/*		sscUpdateSslParam					*/
/*----------------------------------------------------------------------*/
/* This function is called to update SSL Parameters.			*/
/* Most of existing parameters will be updated to new values.		*/
/************************************************************************/
ST_RET sscUpdateSslParam (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
S_CERT_CTRL   *certCtrlNew;
S_CERT_CTRL   *certCtrl;
SSC_SSL_PARAM *sslParamNew;
SSC_SSL_PARAM *sslParam;
ST_UINT        j;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  SSC_LOG_CFLOW0 ("SSC Update: updating SSL parameters");

  sslParamNew = sscStackCfgNew->sslParam;
  sslParam    = sscStackCfg->sslParam;

  /* wipe current sslLocalCertCtrlArr entries				*/
  /* (we are not freeing pts we just remove them from the array)	*/
  for (j=0; j<sslParam->sslLocalCertCtrlArrCnt; ++j)
    sslParam->sslLocalCertCtrlArr[j] = NULL;
  sslParam->sslLocalCertCtrlArrCnt = 0;

  /* update available local SSL certs array (overwrite with new array)		*/
  /* we need to find the currCertCtrl in active certificate list and plug it	*/
  /* into the array (because the duplicate new cert ptrs will be released)	*/
  for (j=0; j<sslParamNew->sslLocalCertCtrlArrCnt; ++j)
    {
    certCtrlNew  = sslParamNew->sslLocalCertCtrlArr[j];
    certCtrl = secCfgFindCertCfgId (sscStackCfg->certificateList, certCtrlNew->cfgId);
    if (!certCtrl)
      {
      SSC_LOG_ERR0 ("SSC Update ERROR: secCfgFindCertCfgId failed in sscUpdateSslParam.");
      return (SD_FAILURE);
      }
    sslParam->sslLocalCertCtrlArr[j] = certCtrl;
    }
  sslParam->sslLocalCertCtrlArrCnt  = sslParamNew->sslLocalCertCtrlArrCnt;

  /*  sslParam->sslCalledPort;			can't be updated	*/
  sslParam->sslCallingPort          = sslParamNew->sslCallingPort;
  sslParam->sslCalledPeerCertKnown  = sslParamNew->sslCalledPeerCertKnown;
  sslParam->sslCallingPeerCertKnown = sslParamNew->sslCallingPeerCertKnown;

  /* SSL session renegotiation (rekeying) parameters */
  sslParam->maxTranPerKey           = sslParamNew->maxTranPerKey;
  sslParam->maxTimePerKey           = sslParamNew->maxTimePerKey;
  sslParam->rekeyTimeout            = sslParamNew->rekeyTimeout;

  /*  sslParam->cipherArr			can't be updated	*/
  /*  sslParam->cipherArrCnt           		can't be updated	*/

  return (SD_SUCCESS);
}
#endif	/* defined(S_SEC_ENABLED)	*/

/************************************************************************/
/*		sscUpdateSnapParam					*/
/*----------------------------------------------------------------------*/
/* This function is called to update SNAP-Lite Parameters.		*/
/* Some existing parameters will be updated to new values.		*/
/************************************************************************/
ST_RET sscUpdateSnapParam (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
SSC_SNAP_PARAM *snapParamNew;
SSC_SNAP_PARAM *snapParam;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  SSC_LOG_CFLOW0 ("SSC Update: updating SNAP-Lite parameters");

  snapParamNew = sscStackCfgNew->snapParam;
  snapParam    = sscStackCfg->snapParam;

  /* SNAP-Lite secure connections parameters */
  /*  snapParam->sslMaxCalled;				can't be updated	*/
  /*  snapParam->sslMaxCalling;				can't be updated	*/
  /*  snapParam->sslProxyCalledPort;			can't be updated	*/
  /*  snapParam->sslProxyCallingPort;			can't be updated	*/
  snapParam->sslProxyConnectTimeout = snapParamNew->sslProxyConnectTimeout;

  /* SNAP-Lite IPC parameters */
  /*  snapParam->maxUsers;				can't be updated	*/
  /*  snapParam->maxBindsPerUser;			can't be updated	*/
  /*  snapParam->ipcPort;				can't be updated	*/
  /*  snapParam->ipcDir;				can't be updated	*/
    snapParam->ipcTimeout = snapParamNew->ipcTimeout;

  /* SNAP-Lite misc parameters */
  /*  snapParam->cpuAffinity;				can't be updated	*/

  return (SD_SUCCESS);
}


/************************************************************************/
/*			sscUpdateAppArName				*/
/*----------------------------------------------------------------------*/
/* Update application AR Name.						*/
/* Existing remote AR Names will be updated.				*/
/* Existing local  AR Names partially updated.				*/
/************************************************************************/
ST_RET sscUpdateAppArName (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew,
                           SSC_APP_AR_NAME *sscAppArName, SSC_APP_AR_NAME *sscAppArNameNew)
{
#if defined(S_SEC_ENABLED)
S_CERT_CTRL   *certCtrlNew;
S_CERT_CTRL   *certCtrl;
ST_INT           j;
#endif	/* defined(S_SEC_ENABLED)	*/
SSC_AR_NAME   *sscArName;
SSC_AR_NAME   *sscArNameNew;

  if (!sscStackCfg || !sscStackCfgNew || sscAppArNameNew == NULL)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL or sscAppArNameNew=NULL");
    return (SD_FAILURE);
    }

  if (sscAppArNameNew->role == SSC_AR_NAME_ROLE_LOCAL)
    SSC_LOG_CFLOW1 ("SSC Update: updating local  ArName='%s'", sscAppArNameNew->arName);
  else
    SSC_LOG_CFLOW1 ("SSC Update: updating remote ArName='%s'", sscAppArNameNew->arName);

  /* first find the AR Name in the new configuration			*/
  sscArNameNew = sscFindArName (sscStackCfgNew, sscAppArNameNew->arName);
  if (!sscArNameNew)
    {
    SSC_LOG_ERR1 ("SSC Update Error: can't find ArName='%s' in the arNameList in new configuration.", sscAppArNameNew->arName);
    return (SD_FAILURE);
    }
  /* next find the AR Name in the active configuration */
  sscArName = sscFindArName (sscStackCfg, sscAppArNameNew->arName);
  if (!sscArName)
    {
    /* since this AR Name was in the appArNameList then it should be	*/
    /* also present in the arNameList					*/
    SSC_LOG_ERR1 ("SSC Update Error: can't find ArName='%s' in the arNameList.", sscAppArNameNew->arName);
    return (SD_FAILURE);
    }

  /* update content of existing remote SSC_AR_NAME (preserving pointers)	*/
  if (sscAppArName->role == SSC_AR_NAME_ROLE_REMOTE)
    {
    /* update arDib */
    memcpy (&sscArName->arDib->ae_title,  &sscArNameNew->arDib->ae_title,  sizeof(AE_TITLE));
    memcpy (&sscArName->arDib->pres_addr, &sscArNameNew->arDib->pres_addr, sizeof(PRES_ADDR));

    /* update arSec */
    sscArName->arSec->arAuthMode = sscArNameNew->arSec->arAuthMode;

    sscArName->arSec->encryptMode = sscArNameNew->arSec->encryptMode;
    sscArName->arSec->sslPort =  sscArNameNew->arSec->sslPort;
    }

#if defined(S_SEC_ENABLED)
  /* for local / remote AR Name update auth cert list */

  /* wipe current authCertArr entries					*/
  /* (we are not freeing pts we just remove them from the array)	*/
  for (j=0; j<sscArName->arSec->authCertArrCnt; ++j)
    sscArName->arSec->authCertArr[j] = NULL;
  sscArName->arSec->authCertArrCnt = 0;

  /* update authCertArr (overwrite with new array of pinters)			*/
  /* we need to find the certCtrl in active certificate list and plug it	*/
  /* into the array (because the duplicate new cert ptrs will be released)	*/
  for (j=0; j<sscArNameNew->arSec->authCertArrCnt; ++j)
    {
    certCtrlNew = sscArNameNew->arSec->authCertArr[j];
    certCtrl = secCfgFindCertCfgId (sscStackCfg->certificateList, certCtrlNew->cfgId);
    if (!certCtrl)
      {
      SSC_LOG_ERR0 ("SSC Update ERROR: secCfgFindCertCfgId failed in sscUpdateAppArName.");
      return (SD_FAILURE);
      }
    sscArName->arSec->authCertArr[j] = certCtrl;
    }
  sscArName->arSec->authCertArrCnt = sscArNameNew->arSec->authCertArrCnt;
#endif	/* defined(S_SEC_ENABLED)	*/

  return (SD_SUCCESS);   
}


/************************************************************************/
/*			sscUpdateAppArNameList				*/
/* Updates AR Names that are used by this application.			*/
/************************************************************************/
ST_RET sscUpdateAppArNameList (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
ST_RET           rtn = SD_SUCCESS;
SSC_APP_NETWORK *appNetwork;
SSC_APP_NETWORK *appNetworkNew;
SSC_APP_AR_NAME *sscAppArName;
SSC_APP_AR_NAME *sscAppArNameNew;
SSC_AR_NAME     *sscArName;
SSC_AR_NAME     *sscArNameNew;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  appNetwork = sscStackCfg->appNetwork;
  appNetworkNew = sscStackCfgNew->appNetwork;

  SSC_LOG_CFLOW0 ("SSC Update: updating application AR Names List:");

  while (appNetworkNew->appArNameList != NULL)
    {
    sscAppArNameNew = appNetworkNew->appArNameList;
    /* unlink the sscAppArNameNew (so we can add it to the active cfg)	*/
    rtn = list_unlink (&appNetworkNew->appArNameList, sscAppArNameNew);
    if (rtn != SD_SUCCESS)
      break;

    /* check if App AR Name already exist in the active configuration	*/
    sscAppArName = sscFindAppArName (appNetwork, sscAppArNameNew->arName);
    if (sscAppArName)
      {
      /* found App AR Name in the active configuration, update it	*/

      /* first compare roles, update only of roles did not changed	*/
      if (sscAppArName->role == sscAppArNameNew->role)
        rtn = sscUpdateAppArName (sscStackCfg, sscStackCfgNew, sscAppArName, sscAppArNameNew);
      else
        {
        SSC_LOG_NERR3 ("SSC Update: ArName='%s' role changed from '%s' to '%s' Update not supported.",
                       sscAppArNameNew->arName, 
                       (sscAppArName->role    == SSC_AR_NAME_ROLE_LOCAL ? "Local" : "Remote"),
                       (sscAppArNameNew->role == SSC_AR_NAME_ROLE_LOCAL ? "Local" : "Remote"));
	rtn = SD_SUCCESS;  /* continue with next App AR Name */
        }
      /* done with this one */
      chk_free (sscAppArNameNew);
      if (rtn != SD_SUCCESS)
        break;
      }
    else
      {
      /* App AR Name not found in active configuration, add it */
      list_add_last (&appNetwork->appArNameList, sscAppArNameNew);
      sscAppArName = sscAppArNameNew;

      /* now add/update the AR Name in the active configuration */

      /* is the AR Name already present in active configuration ? */
      sscArName = sscFindArName (sscStackCfg, sscAppArNameNew->arName);
      if (sscArName)
        {
	/* AR Name present,						*/
	/* add arDib & arSec to active configuration			*/
        /* we need to split into two diff lists for the application use	*/
        /* (don't need to unlink from appNetwork lists)			*/
        list_add_last (&appNetwork->dibEntryList, sscArName->arDib);
        list_add_last (&appNetwork->arLocRemSecList, sscArName->arSec);
        /* call update function in case the AR Name changed */
        rtn = sscUpdateAppArName (sscStackCfg, sscStackCfgNew, sscAppArName, sscAppArNameNew);
        if (rtn != SD_SUCCESS)
          break;
	}
      else
        {
	/* AR Name not present, */
        /* find and unlink ArName element from new list and add it to the active list	*/
        sscArNameNew = sscFindArName (sscStackCfgNew, sscAppArNameNew->arName);
        if (!sscArNameNew)
          {
          SSC_LOG_ERR1 ("SSC Update Error: can't find remote ArName='%s' in the arNameList in new configuration.",
                        sscAppArNameNew->arName);
          return (SD_FAILURE);
          }
        rtn = list_unlink (&sscStackCfgNew->arNameList, sscArNameNew);
        if (rtn != SD_SUCCESS)
          break;
        list_add_last (&sscStackCfg->arNameList, sscArNameNew);
        /* we need to split into two diff lists for the application use	*/
        /* (don't need to unlink from appNetwork lists)			*/
        list_add_last (&appNetwork->dibEntryList, sscArNameNew->arDib);
        list_add_last (&appNetwork->arLocRemSecList, sscArNameNew->arSec);
	}

      if (sscAppArNameNew->role == SSC_AR_NAME_ROLE_LOCAL)
        SSC_LOG_CFLOW1 ("SSC Update: added local ArName='%s'", sscAppArNameNew->arName);
      else
        SSC_LOG_CFLOW1 ("SSC Update: added remote ArName='%s'", sscAppArNameNew->arName);
      }
    }

  return (rtn);
}

/************************************************************************/
/*		sscUpdateArMatchingProfile				*/
/*----------------------------------------------------------------------*/
/* This function is called to update AR Matching Profile List.		*/
/* Add new AR Matching Profile to the current list.			*/
/* Existing AR Matching Profiles updated.				*/
/************************************************************************/
ST_RET sscUpdateArMatchingProfile (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
ST_RET                    rtn = SD_SUCCESS;
SSC_AR_MATCHING_PROFILE  *arMatchingProfileNewNext;
SSC_AR_MATCHING_PROFILE  *arMatchingProfileNew;
SSC_AR_MATCHING_PROFILE  *arMatchingProfile;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  if (sscStackCfgNew->arMatchingProfileList)
    SSC_LOG_CFLOW0 ("SSC Update: updating AR Matching Profiles:");

  /* find the AR Matching Profile in the arMatchingProfileList */
  arMatchingProfileNew = sscStackCfgNew->arMatchingProfileList;
  while (arMatchingProfileNew)
    {
    arMatchingProfileNewNext = (SSC_AR_MATCHING_PROFILE *) list_get_next (sscStackCfgNew->arMatchingProfileList, arMatchingProfileNew);

    arMatchingProfile = sscFindArMatchingProfile (sscStackCfg, arMatchingProfileNew->name);
    if (arMatchingProfile)
      {
      /* AR Matching Profile name is already on the current list, update the content	*/
      memcpy (arMatchingProfile->dibMatch, arMatchingProfileNew->dibMatch, sizeof (DIB_MATCH_CTRL));
      SSC_LOG_CFLOW1 ("SSC Update: updated  AR Matching Profile '%s'", arMatchingProfileNew->name);
      }
    else
      {
      /* new profile name, add this arMatching Profile to the current list */
      rtn = list_unlink (&sscStackCfgNew->arMatchingProfileList, arMatchingProfileNew);
      if (rtn == SD_SUCCESS)
        {
        list_add_last (&sscStackCfg->arMatchingProfileList, arMatchingProfileNew);
        SSC_LOG_CFLOW1 ("SSC Update: added   AR Matching Profile '%s'", arMatchingProfileNew->name);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC Update ERROR: unable to unlink AR Matching Profile '%s'", arMatchingProfileNew->name);
	rtn = SD_FAILURE;
	break;
	}
      }

    arMatchingProfileNew = arMatchingProfileNewNext;
    }

  return (rtn);
}


/************************************************************************/
/*		sscUpdateSecurityProfile				*/
/*----------------------------------------------------------------------*/
/* This function is called to update Security Profile List.		*/
/* Add new Security Profile to the current list.			*/
/* Existing Security Profiles remain unchanged.				*/
/************************************************************************/
ST_RET sscUpdateSecurityProfile (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew)
{
ST_RET                 rtn = SD_SUCCESS;
SSC_SECURITY_PROFILE  *securityProfileNewNext;
SSC_SECURITY_PROFILE  *securityProfileNew;
SSC_SECURITY_PROFILE  *securityProfile;

  if (!sscStackCfg || !sscStackCfgNew)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscStackCfg=NULL or sscStackCfgNew=NULL");
    return (SD_FAILURE);
    }

  if (sscStackCfgNew->securityProfileList)
    SSC_LOG_CFLOW0 ("SSC Update: updating Security Profiles:");
  else
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: at least one Security profile expected.");
    return (SD_FAILURE);
    }

  /* find the Security Profile in the securityProfileList */
  securityProfileNew = sscStackCfgNew->securityProfileList;
  while (securityProfileNew)  /* at least one profile expected */
    {
    securityProfileNewNext = (SSC_SECURITY_PROFILE *) list_get_next (sscStackCfgNew->securityProfileList, securityProfileNew);

    securityProfile = sscFindSecurityProfile (sscStackCfg, securityProfileNew->name);
    if (securityProfile == NULL)
      {
      /* add this Security Profile to the current list */
      rtn = list_unlink (&sscStackCfgNew->securityProfileList, securityProfileNew);
      if (rtn == SD_SUCCESS)
        {
        list_add_last (&sscStackCfg->securityProfileList, securityProfileNew);
        SSC_LOG_CFLOW1 ("SSC Update: added Security Profile '%s'", securityProfileNew->name);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC Update ERROR: unable to unlink Security Profile '%s'", securityProfileNew->name);
	rtn = SD_FAILURE;
	break;
	}
      }
    else
      {
      /* else the Security Profile name is already on the current list, skip it.	*/
      SSC_LOG_CFLOW1 ("SSC Update: skipping existing Security Profile '%s' (update not supported)", securityProfileNew->name);
      /* left over Security Profiles in the sscStackCfgNew will be freed later	*/
      }

    securityProfileNew = securityProfileNewNext;
    }

  return (rtn);
}


/********************************************************************************/
/*			sscUpdateAppNetwork					*/
/*------------------------------------------------------------------------------*/
/* Update schema (to be decided):						*/
/*   Network_Adapter_List	new added, existing unchanged			*/
/*   LeanT_Profile		new added, existing unchanged			*/
/*   Trusted_CA_Cert_List	new added (with new cfgId), existing unchanged	*/
/*   Certificate_List		new added (with new cfgId), existing unchanged	*/
/*   App_SNAP			most params unchanged				*/
/*   AR_Name_List		new added, existing loc AR Names unchanged	*/
/*				                 ???  add certificate? 		*/
/*				           existing REM AR Names updated	*/
/*   AR_Matching_Profile_List	new added					*/
/*   Security_Profile_List	new added					*/
/*   App_Network:		add new, existing mostly unchanged		*/
/*     Leant_Profile 		unchanged					*/
/*     Security_Profile		unchanged					*/
/*     AR_Matching_Profile	updated to new name				*/
/*     App_AR_Name              add new names (existing loc names unchanged)	*/
/* Return:									*/
/*	SD_SUCCESS	if function successful,					*/
/*	SD_FAILURE	otherwise						*/
/********************************************************************************/
ST_RET sscUpdateAppNetwork (ST_CHAR *appName)
{
ST_RET           rtn = SD_FAILURE;
SSC_STACK_CFG   *sscStackCfg = NULL;
SSC_STACK_CFG   *sscStackCfgNew = NULL;
SSC_APP_NETWORK *appNetwork;
SSC_APP_NETWORK *appNetworkNew;
ST_BOOLEAN       appSnapLite = SD_FALSE;	/* extra checking is done for SNAP-Lite */
ST_BOOLEAN       secureApp   = SD_FALSE;

  if (!_sscStackCfgGlb)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: sscConfigAppNetwork must be called first.");
    return (SD_FAILURE);
    }
  /* read SISCO Stack Configuration from a configuration source (file or database).*/
  rtn = sscReadStackCfg (&sscStackCfgNew);
  if (rtn != SD_SUCCESS)
    {
    SSC_LOG_ERR0 ("SSC Update ERROR: read new SISCO Stack Configuration failed.");
    return (SD_FAILURE);
    }

  /* verify the sscStackCfgNew for this application */
  rtn = sscVerifyAppNetwork (sscStackCfgNew, appName);
  if (rtn != SD_SUCCESS)
    {
    sscFreeStackCfgPtr (sscStackCfgNew);
    return (SD_FAILURE);
    }
  appNetworkNew = sscStackCfgNew->appNetwork;

  /* lock active configuration */
  sscStackCfg = sscAccessStackCfg ();
  appNetwork  = sscStackCfg->appNetwork;
  secureApp   = appNetwork->securityProfile->secure;
  if (strcmp (appNetwork->appName, SNAPL_CFG_APP_NAME) == 0)
    appSnapLite = SD_TRUE;

  SSC_LOG_FLOW1 ("SSC Update: updating Sisco Stack Configuration for appName='%s'", appName);
  SSC_LOG_CFLOW0 ("SSC Update: =================================================================");

  /* update selectively the SISCO Stack Configuration (_sscStackCfgGlb) */
  while (1)  /* use 'while' instead of 'if' to make code simpler */
    {
    /* don't allow update if user changed the 'secure' mode	*/
    if (secureApp != appNetworkNew->securityProfile->secure)
      {
      SSC_LOG_ERR0 ("SSC Update ERROR: can not switch between secure and non-secure mode \n"
                    "  during run-time (restart is required)"); 
      rtn = SD_FAILURE;
      break;
      }

    /* add new Network Adapter names */
    rtn = sscUpdateNetworkAdapter (sscStackCfg, sscStackCfgNew);
    if (rtn != SD_SUCCESS)
      break;

    /* add new Leant Profiles */
    rtn = sscUpdateLeantProfile (sscStackCfg, sscStackCfgNew);
    if (rtn != SD_SUCCESS)
      break;

#if defined(S_SEC_ENABLED)
    /* update security parameters */
    if (secureApp)
      {
      /* update general certificate parameters */
      rtn = sscUpdateCertParam (sscStackCfg, sscStackCfgNew);
      if (rtn != SD_SUCCESS)
        break;

      /* Add new CA Certificates */
      rtn = sscUpdateCaCertificates (sscStackCfg, sscStackCfgNew);
      if (rtn != SD_SUCCESS)
        break;

      /* Add new Certificates */
      rtn = sscUpdateCertificates (sscStackCfg, sscStackCfgNew);
      if (rtn != SD_SUCCESS)
        break;

      /* update SSL parameters (after certificates were updated)	*/
      rtn = sscUpdateSslParam (sscStackCfg, sscStackCfgNew);
      if (rtn != SD_SUCCESS)
        break;
      }
#endif	/* defined(S_SEC_ENABLED)	*/

    /* update SNAP-Lite parameters						*/
    /* (note that app may use some of these params for IPC with SNAP-Lite) 	*/
    rtn = sscUpdateSnapParam (sscStackCfg, sscStackCfgNew);
    if (rtn != SD_SUCCESS)
      break;

    if (!appSnapLite)
      {
      /* update only the AR Names that THIS application is using */
      rtn = sscUpdateAppArNameList (sscStackCfg, sscStackCfgNew);
      if (rtn != SD_SUCCESS)
        break;

      /* update AR Matching Profiles */
      rtn = sscUpdateArMatchingProfile (sscStackCfg, sscStackCfgNew);
      if (rtn != SD_SUCCESS)
        break;
      }

    /* update security profiles */
    rtn = sscUpdateSecurityProfile (sscStackCfg, sscStackCfgNew);
    if (rtn != SD_SUCCESS)
      break;

    /* --------- now update this application's parameters ----- */
    /* appNetwork->appName			unchanged	*/
    /* appNetwork->leantProfile			unchanged	*/
    /* appNetwork->securityProfile		unchanged	*/
    /* appNetwork->arMatchingProfile    updated in func sscUpdateArMatchingProfile above*/
    /* appNetwork->appArNameList	updated in func sscUpdateAppArNameList above	*/
    /* appNetwork->dibEntryList		updated in func sscUpdateAppArNameList above	*/
    /* appNetwork->arLocRemSecList	updated in func sscUpdateAppArNameList above	*/
    /* maxMmsPduLength;				unchanged	*/
    /* maxConnections;				unchanged	*/
    /* maxCallingConns;				unchanged	*/
    /* maxCalledConns;				unchanged	*/
    /* appNetwork->secEventLogFileName		unchanged	*/
    /* appNetwork->secEventLogFileSize		unchanged	*/

    break;  /* one time pass loop */
    }

  /* unlock active configuration */
  sscReleaseStackCfg ();

  /* free elements that were not used from the New cfg during the update */
  sscFreeStackCfgPtr (sscStackCfgNew);

  return (rtn);
}



	/* =============================================================*/
	/* 	SISCO Stack Configuration Free functions		*/
	/* =============================================================*/
	/* Functions to free SISCO's Stack Configuration parameters.	*/
	/* These functions are called from the sscFreeStackCfg.		*/
	/* Note that some pointers may be moved during configuration	*/
	/* (sscConfigAppNetwork) to the application's runtime lists and	*/
	/* tables and will be freed during application's termination.	*/
	/* =============================================================*/


/************************************************************************/
/*		sscFreeNetworkAdapterList				*/
/*----------------------------------------------------------------------*/
/* Free networkAdapterList from sscStackCfg. 				*/
/************************************************************************/

ST_RET sscFreeNetworkAdapterList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET                rtn = SD_SUCCESS;
SSC_NETWORK_ADAPTER  *networkAdapter;

  if (!sscStackCfg)
    return (SD_FAILURE);

  while (sscStackCfg->networkAdapterList != NULL)
    {
    networkAdapter = sscStackCfg->networkAdapterList;
    rtn |= list_unlink (&sscStackCfg->networkAdapterList, networkAdapter);
    chk_free (networkAdapter);    
    }

  return (rtn);
}

/************************************************************************/
/*		sscFreeLeantProfileList					*/
/*----------------------------------------------------------------------*/
/* Free leantProfileList from sscStackCfg.				*/
/************************************************************************/

ST_RET sscFreeLeantProfileList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET             rtn = SD_SUCCESS;
SSC_LEANT_PROFILE *leantProfile;

  if (!sscStackCfg)
    return (SD_FAILURE);

  while (sscStackCfg->leantProfileList != NULL)
    {
    leantProfile = sscStackCfg->leantProfileList;
    rtn |= list_unlink (&sscStackCfg->leantProfileList, leantProfile);
    chk_free (leantProfile);    
    }

  return (rtn);
}

#if defined(S_SEC_ENABLED)
/************************************************************************/
/*			sscFreeCertParam				*/
/*----------------------------------------------------------------------*/
/* Free certParam from sscStackCfg.					*/
/************************************************************************/

ST_RET sscFreeCertParam (SSC_STACK_CFG *sscStackCfg)
{
ST_RET       rtn = SD_SUCCESS;

  if (!sscStackCfg)
    return (SD_FAILURE);

  if (sscStackCfg->certParam)
    {
    chk_free (sscStackCfg->certParam);
    sscStackCfg->certParam = NULL;
    }

  return (rtn);
}

/************************************************************************/
/*			sscFreeTrustedCaCertList			*/
/*----------------------------------------------------------------------*/
/* Free trustedCaCertList from sscStackCfg.				*/
/************************************************************************/

ST_RET sscFreeTrustedCaCertList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET       rtn = SD_SUCCESS;

  if (!sscStackCfg)
    return (SD_FAILURE);

  if (sscStackCfg->trustedCaCertList)
    rtn = secCfgFreeCertCtrl (&sscStackCfg->trustedCaCertList);

  return (rtn);
}

/************************************************************************/
/*			sscFreeCertificateList				*/
/*----------------------------------------------------------------------*/
/* Free certificateList from sscStackCfg.				*/
/************************************************************************/

ST_RET sscFreeCertificateList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET       rtn = SD_SUCCESS;

  if (!sscStackCfg)
    return (SD_FAILURE);

  if (sscStackCfg->certificateList)
    rtn = secCfgFreeCertCtrl (&sscStackCfg->certificateList);

  return (rtn);
}
#endif	/* defined(S_SEC_ENABLED)	*/

/************************************************************************/
/*			sscFreeSslParam					*/
/*----------------------------------------------------------------------*/
/* Free sslParam from sscStackCfg.					*/
/************************************************************************/

ST_RET sscFreeSslParam (SSC_STACK_CFG *sscStackCfg)
{
ST_RET       rtn = SD_SUCCESS;

  if (!sscStackCfg)
    return (SD_FAILURE);

  if (sscStackCfg->sslParam)
    {
    chk_free (sscStackCfg->sslParam);
    sscStackCfg->sslParam = NULL;
    }

  return (rtn);
}

/************************************************************************/
/*			sscFreeSnapParam				*/
/*----------------------------------------------------------------------*/
/* Free snapParam from sscStackCfg.					*/
/************************************************************************/

ST_RET sscFreeSnapParam (SSC_STACK_CFG *sscStackCfg)
{
ST_RET       rtn = SD_SUCCESS;

  if (!sscStackCfg)
    return (SD_FAILURE);

  if (sscStackCfg->snapParam)
    {
    chk_free (sscStackCfg->snapParam);
    sscStackCfg->snapParam = NULL;
    }

  return (rtn);
}

/************************************************************************/
/*			sscFreeArName					*/
/*----------------------------------------------------------------------*/
/* Free ArName from sscStackCfg.					*/
/************************************************************************/

ST_RET sscFreeArName (SSC_STACK_CFG *sscStackCfg, SSC_AR_NAME *sscArName)
{
ST_RET       rtn = SD_SUCCESS;

  if (!sscStackCfg || !sscArName)
    return (SD_FAILURE);

  rtn = list_unlink (&sscStackCfg->arNameList, sscArName);
  if (rtn == SD_SUCCESS)
    {
    if (sscArName->arDib)
      chk_free (sscArName->arDib);
    if (sscArName->arSec)
      chk_free (sscArName->arSec);
    chk_free (sscArName);
    }

  return (rtn);
}

/************************************************************************/
/*			sscFreeArNameList				*/
/*----------------------------------------------------------------------*/
/* Free arNameList from sscStackCfg.					*/
/************************************************************************/

ST_RET sscFreeArNameList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET       rtn = SD_SUCCESS;
SSC_AR_NAME *sscArName;

  if (!sscStackCfg)
    return (SD_FAILURE);

  while (sscStackCfg->arNameList != NULL)
    {
    sscArName = sscStackCfg->arNameList;
    rtn = sscFreeArName (sscStackCfg, sscArName);
    if (rtn != SD_SUCCESS)
      break;	/* something unexpected ? */
    }

  return (rtn);
}

/************************************************************************/
/*			sscFreeAppArNameList				*/
/*----------------------------------------------------------------------*/
/* Free appArNameList from sscStackCfg.					*/
/************************************************************************/

ST_RET sscFreeAppArNameList (SSC_APP_NETWORK *appNetwork)
{
ST_RET           rtn = SD_SUCCESS;
SSC_APP_AR_NAME *sscAppArName;

  if (!appNetwork)
    return (SD_FAILURE);

  while (appNetwork->appArNameList != NULL)
    {
    sscAppArName = appNetwork->appArNameList;
    rtn |= list_unlink (&appNetwork->appArNameList, sscAppArName);
    chk_free (sscAppArName);
    }

  return (rtn);
}

/************************************************************************/
/*			sscFreeArMatchingProfileList			*/
/*----------------------------------------------------------------------*/
/* Free arMatchingProfileList from sscStackCfg.				*/
/************************************************************************/

ST_RET sscFreeArMatchingProfileList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET                   rtn = SD_SUCCESS;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;

  if (!sscStackCfg)
    return (SD_FAILURE);

  while (sscStackCfg->arMatchingProfileList != NULL)
    {
    arMatchingProfile = sscStackCfg->arMatchingProfileList;
    rtn |= list_unlink (&sscStackCfg->arMatchingProfileList, arMatchingProfile);
    if (arMatchingProfile->dibMatch)
      chk_free (arMatchingProfile->dibMatch);
    chk_free (arMatchingProfile);
    }

  return (rtn);
}

/************************************************************************/
/*			sscFreeSecurityProfileList			*/
/*----------------------------------------------------------------------*/
/* Free securityProfileList from sscStackCfg.				*/
/************************************************************************/

ST_RET sscFreeSecurityProfileList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET                rtn = SD_SUCCESS;
SSC_SECURITY_PROFILE *securityProfile;

  if (!sscStackCfg)
    return (SD_FAILURE);

  while (sscStackCfg->securityProfileList != NULL)
    {
    securityProfile = sscStackCfg->securityProfileList;
    rtn |= list_unlink (&sscStackCfg->securityProfileList, securityProfile);
    chk_free (securityProfile);
    }

  return (rtn);
}

/************************************************************************/
/*		sscFreeAppNetworkList					*/
/*----------------------------------------------------------------------*/
/* Free appNetworkList from sscStackCfg. 				*/
/************************************************************************/

ST_RET sscFreeAppNetworkList (SSC_STACK_CFG *sscStackCfg)
{
ST_RET                rtn = SD_SUCCESS;
SSC_APP_NETWORK      *appNetwork;

  if (!sscStackCfg)
    return (SD_FAILURE);

  /* NOTE: all pointer within the appNetwork are on different lists	*/
  /*       and will be freed from those lists.				*/
  while (sscStackCfg->appNetworkList != NULL)
    {
    appNetwork = sscStackCfg->appNetworkList;
    rtn |= list_unlink (&sscStackCfg->appNetworkList, appNetwork);

    rtn |= sscFreeAppArNameList (appNetwork);
    chk_free (appNetwork);    
    }

  return (rtn);
}

	/* =============================================================*/
	/* -------------------------------------------------------------*/
	/* 	SISCO's Stack Configuration User interface functions	*/
	/* -------------------------------------------------------------*/
	/* =============================================================*/

/************************************************************************/
/*			sscConfigAppNetwork				*/
/*----------------------------------------------------------------------*/
/* User function to configure the SISCO Stack for given application.	*/
/* This function reads SISCO Stack Configuration from a configuration	*/
/* source (file, database) and calls sscVerifyAppNetwork to check the	*/
/* integrity of the SISCO Stack Configuration.				*/
/* The sscStackCfg pointer is saved in a global variable and will be 	*/
/* used to configure various stack sections during application		*/
/* initialization. This pointer should remain valid until application	*/
/* cleanup during termination.						*/
/* The glb cfg pointercan be accessed by calling sscAccessStackCfg.	*/
/* Return:								*/
/*	SD_SUCCESS	if function successful,				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/
ST_RET sscConfigAppNetwork (ST_CHAR *appName)
{
ST_RET          rtn = SD_FAILURE;
SSC_STACK_CFG *sscStackCfg = NULL;

  if (_sscStackCfgGlb)
    {
    SSC_LOG_ERR0 ("SSC ERROR: sscConfigAppNetwork failed (already configured)");
    return (SD_FAILURE);
    }

  /* read SISCO Stack Configuration from a configuration source (file, database).*/
  rtn = sscReadStackCfg (&sscStackCfg);
  if (rtn == SD_SUCCESS)
    {
    /* verify the sscStackCfg only for this application */
    rtn = sscVerifyAppNetwork (sscStackCfg, appName);
    if (rtn == SD_SUCCESS)
      {
      /* set the global pointer */
      _sscStackCfgGlb = sscStackCfg;
      }
    else
      sscFreeStackCfgPtr (sscStackCfg);
    }
  return (rtn);
}


/************************************************************************/
/*			sscCheckStackCfg				*/
/*----------------------------------------------------------------------*/
/* Return:								*/
/*	SD_SUCCESS	if function successful,				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/
ST_RET sscCheckStackCfg (ST_CHAR *appName)
{
ST_RET      rtn = SD_FAILURE;

  rtn = sscUpdateAppNetwork (appName);
  if (rtn != SD_SUCCESS)
    {
    SSC_LOG_NERR1 ("SSC ERROR: SISCO Stack Configuration update failed for appName='%s'.", appName);
    return (rtn);
    }

 return (rtn);
}


/************************************************************************/
/*			sscFreeStackCfgPtr 				*/
/*----------------------------------------------------------------------*/
/* Functions to free the SSC_STACK_CFG pointer and its subcomponents.	*/
/* Note that some pointers may be moved by the sscConfigAppNetwork to	*/
/* the application runtime global _sscStackCfgGlb and will be free	*/
/* during application's termination.					*/
/*									*/
/* !!! IMPORTANT !!! the _gSecManCfg and _sscStackCfgGlb share some	*/
/*		     pointers, so call this fun to free _sscStackCfgGlb	*/
/*                   when terminating program. If the sscStackCfg	*/
/*		     is not pointing to the global ptr, then it can be	*/
/*		     free anytime.					*/
/* Return:								*/
/*	SD_SUCCESS	if function successful,				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/

ST_RET sscFreeStackCfgPtr (SSC_STACK_CFG *sscStackCfg)
{
ST_RET         rtn = SD_SUCCESS;

  if (!sscStackCfg)
    return (SD_SUCCESS);

  /* free sscStackCfg objects in reverse order of allocations	*/

  rtn |= sscFreeAppNetworkList (sscStackCfg);

  rtn |= sscFreeSecurityProfileList (sscStackCfg);

  rtn |= sscFreeArMatchingProfileList (sscStackCfg);

  rtn |= sscFreeArNameList (sscStackCfg);

  rtn |= sscFreeSnapParam (sscStackCfg);

  rtn |= sscFreeSslParam (sscStackCfg);

#if defined(S_SEC_ENABLED)
  rtn |= sscFreeCertificateList (sscStackCfg);

  rtn |= sscFreeTrustedCaCertList (sscStackCfg);

  rtn |= sscFreeCertParam (sscStackCfg);
#endif	/* defined(S_SEC_ENABLED)	*/
  
  rtn |= sscFreeLeantProfileList (sscStackCfg);

  rtn |= sscFreeNetworkAdapterList (sscStackCfg);

  if (sscStackCfg == _sscStackCfgGlb)
    _sscStackCfgGlb = NULL;
  chk_free (sscStackCfg);

  return (rtn);
}


/************************************************************************/
/*			sscFreeStackCfg					*/
/*----------------------------------------------------------------------*/
/* Functions to free the SISCO Stack Configuration global pinter	*/
/* and its subcomponents.						*/
/* Return:								*/
/*	SD_SUCCESS	if function successful,				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/
ST_RET sscFreeStackCfg (ST_VOID)
{
ST_RET         rtn = SD_SUCCESS;
SSC_STACK_CFG *sscStackCfg;

  sscStackCfg = sscAccessStackCfg ();
  rtn = sscFreeStackCfgPtr (sscStackCfg);
  sscReleaseStackCfg ();

  return (rtn);
}

/************************************************************************/
/*                      find_dib_entry                                  */
/* This function returns a pointer to a DIB entry for selected name.	*/
/************************************************************************/

DIB_ENTRY *find_dib_entry (ST_CHAR *name)
  {
DIB_ENTRY   *de;
DIB_ENTRY   *deOut = NULL;
DIB_ENTRY   *de_list = NULL;
SSC_STACK_CFG *sscStackCfg = NULL;

  if (!name)
    return (NULL);

  /* get the pointers to SISCO Stack Configuration */
  sscStackCfg = sscAccessStackCfg ();
  if (!sscStackCfg || !sscStackCfg->appNetwork)
    {
    sscReleaseStackCfg ();
    return (NULL);
    }
  de_list = sscStackCfg->appNetwork->dibEntryList;

  /* Loop through DIB_ENTRY list looking for matching "name".	*/
  for (de = de_list; de != NULL; de = list_get_next (de_list, de))
    {
    if (strcmp (de->name, name) == 0)
      {
      deOut = de;
      break;
      }
    }
  sscReleaseStackCfg ();

  return (deOut);
  }

