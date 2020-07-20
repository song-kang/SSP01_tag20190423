/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*     		2003 - 2012, All Rights Reserved		        */
/*									*/
/* MODULE NAME : sStackCfg.h						*/
/* PRODUCT(S)  : XXX-142-xxx						*/
/*									*/
/* MODULE DESCRIPTION : SISCO's Stack Configuration (SSC)		*/
/*									*/
/*	This file provides user application interface to configure	*/
/*	the SISCO Stack	and Security parameters from the		*/
/*      siscoStackCfg.xml file.						*/
/*	SNAP-Lite is also using this interface.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments    			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/05/12  EJV	   SSC_CERT_PARAM: add certExpiredDropExistingConns.*/
/* 10/28/10  EJV	   Added sscVerifyArMatchingProfile.		*/
/* 10/25/10  EJV	   Added sscFindNextAppArName.			*/
/* 08/05/10  EJV	   Added sscUpdate* function protos.		*/
/* 08/04/10  EJV           Moved most SISCO_REGISTRY_* to glbtypes.h.	*/
/*			   Added sscFreeArName.	Added comment.		*/
/* 05/17/10  JRB	   Add sscGetNetworkDataPath proto for WIN32.	*/
/* 02/05/10  JRB    07     Add parsing function prototypes.		*/
/* 12/16/09  EJV    06     SSC_APP_NETWORK: added MMS params (max*).	*/
/* 11/17/09  JRB    05     Add sscConfigMvl for MMS_LITE only.		*/
/* 04/23/09  EJV    04     sscConfigAppNetwork:don't ret cfg ptr (safer)*/
/*			   SSC_SSL_PARAM: chg to arr of local SSL certs.*/
/* 09/26/08  EJV    03	   Mv lookup code to seccfg_comn.c.		*/
/* 12/06/07  EJV    02     SSC_LEANT_PROFILE: added tcpMaxIdleTime.	*/
/* 07/25/07  EJV    01     Merged osill2.cfg and secManCfg.xml for V2.00*/
/************************************************************************/

#ifndef SSTACKCFG_INCLUDED
#define SSTACKCFG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "slog.h"
#include "mms_def2.h"		/* for MAX_AR_LEN	*/
#include "lean_a.h"		/* for DIB_ENTRY	*/
#include "ssec.h"
#include "sx_defs.h"		/* for SX_DEC_CTRL	*/

#if defined(MMS_LITE)
#include "clnp_usr.h"		/* for CLNP_* defines 	*/
#include "mvl_acse.h"		/* for MVL_CFG_INFO	*/
#endif

	/* =============================================================*/
	/*  SISCO's Stack Configuration (SSC) defines and structures.	*/
	/* =============================================================*/


/* SSC Version define, make a new version define when there is a change	*/
/*                     impacting backward compatibility.		*/
#define	SSC_VERSION_1			1
#define	SSC_VERSION_TIME		"2009-01-01"	/* purpose ???	*/

					
/* SNAP-Lite app name in SISCO Stack Configuration	*/
#define   SNAPL_CFG_APP_NAME  		"SNAP-Lite"

#if defined(_WIN32)
  /* default directories, to assemble other paths */
  #define SISCO_PATH_INSTALL		"C:\\Program Files\\SISCO"
  #define SISCO_PATH_LICENSE		"\\License\\"
  #define SISCO_PATH_NETWORK		"\\Network\\"
  #define SISCO_PATH_SECURITY		"\\Security\\"

  #define SSC_FILE_PATH			SISCO_PATH_INSTALL SISCO_PATH_NETWORK
  #define SSC_FILE_NAME			"siscoStackCfg.xml"

#else  /* !defined(_WIN32) */

  /* UNIX, Linux,... */
  /* default install path (fixed) on UNIX, Linux (diff on each system)	*/
  #if defined(_AIX)
    #define SISCO_PATH_INSTALL		"/usr/lpp/sisco"
  #elif defined(__hpux)
    #define SISCO_PATH_INSTALL		"/opt/sisco"
  #else  /* other UNIX systems, Linux */
    #define SISCO_PATH_INSTALL		"/usr/sisco"
  #endif
  #define SISCO_PATH_LICENSE		"/license/"
  #define SISCO_PATH_NETWORK		"/network/"
  #define SISCO_PATH_SECURITY		"/security/"
  /* to detect if ICCP Toolkit is installed */
  #define SISCO_PATH_ICCP		"/iccp"	/* DEBUG: should be mmsease/iccp ??? */

  /* use only lower case on UNIX / Linux */
  #define SSC_FILE_PATH			SISCO_PATH_INSTALL SISCO_PATH_NETWORK
  #define SSC_FILE_NAME			"siscostackcfg.xml"

#endif /* !defined(_WIN32) */

#define	SSC_MAX_LEN_NAME		(ST_UINT) 255
#define	SSC_MAX_LEN_DESC		(ST_UINT) 255
#define	SSC_MAX_LEN_AR_NAME		(ST_UINT) MAX_AR_LEN
#define SSC_MAX_LEN_AP_TITLE		(ST_UINT) 127	/* sequence of up to 16 16-bit integers and some spaces */

#if defined(MMS_LITE)
#define SSC_MAX_LEN_MAC			CLNP_MAX_LEN_MAC 
#define SSC_MAX_LEN_NSAP		CLNP_MAX_LEN_NSAP
#else  /* default setting */
#define SSC_MAX_LEN_MAC			6		/* Max len of MAC addr  () 	*/
#define SSC_MAX_LEN_NSAP		20		/* Max len of NSAP addr	()	*/
#endif /* default setting */

#define SSC_SESSION_DISCONNECT_TIMEOUT	60		/* no define in MMS-LITE	*/


typedef struct SSC_NETWORK_ADAPTER_tag
  {
  DBL_LNK l;
  /* network adapter parameters */
  ST_CHAR        name [SSC_MAX_LEN_NAME+1];	/* adapter name		*/
  ST_CHAR        description [SSC_MAX_LEN_DESC+1];
  ST_UCHAR       mac [SSC_MAX_LEN_MAC];
  } SSC_NETWORK_ADAPTER;


typedef struct SSC_LEANT_PROFILE_tag
  {
  DBL_LNK l;
  /* general LeanT profile parameters */
  ST_CHAR        name [SSC_MAX_LEN_NAME+1];	/* profile name		*/

  /* session parameters */
  ST_UINT16      sessionDisconnectTimeout;

  /* TCP transport parameters */
  ST_BOOLEAN	 tcpEnable;
  ST_UINT16      tcpMaxTpduLen;
  ST_UINT        tcpMaxSpduOutstanding;
  ST_UINT16      tcpListenPort;			/* RFC1006 port is 102 */
  ST_UINT        tcpMaxIdleTime;		/* secs, if > 0 check for idle cons,   	*/
  /* UNIX, Linux start */
  ST_UINT16      tcpEventWakeupPortBase;	/* starting port for wakeup socket to		*/
  ST_UINT        tcpEventWakeupPortRange;	/* indicate socket activity to application	*/
  /* UNIX, Linux end   */
  ST_UINT16      gsWakeupPortBase;		/* starting port for wakeup socket to		*/
  ST_UINT        gsWakeupPortRange;		/* indicate socket activity to gensock2 threads	*/

  /* TP4 transport parameters */
  ST_BOOLEAN	 tp4Enable;
  ST_UINT16      tp4MaxTpduLen;
  ST_UINT        tp4MaxSpduOutstanding;
  ST_UINT8       tp4LocalCredits;
  ST_UINT8       tp4MaxRemoteCredits;
  ST_UINT16      tp4WindowTime;
  ST_UINT16      tp4InactivityTime;
  ST_UINT16      tp4RetransmissionTime;
  ST_UINT8       tp4MaxTransmissions;
  ST_UINT8       tp4AckDelay;

  /* network parameters (TP4) */
  ST_UINT8       clnpLifetime;
  ST_UINT8       clnpLifetimeDecrement;
  ST_UINT16      clnpCfgTimer;
  ST_UINT16      clnpEshDelay;
  ST_CHAR        adapterName [SSC_MAX_LEN_NAME+1];
  } SSC_LEANT_PROFILE;


typedef struct SSC_CERT_PARAM_tag
  {
  /* to check certificates expiration */  
  ST_UINT32      certExpirationAlarmLimit;	/* in hours, how far ahead to alarm about cert expiration*/
						/* if 0 no checking done, max 10,000 hours (1 year=8784 hours)*/
  ST_UINT32      certExpirationEvalInterval;	/* in hours, how often to check for cert expiration	*/
  ST_BOOLEAN     certExpiredDropExistingConns;

  /* SSL Certificate Revocation parameters */
  ST_INT         crlCheckTime;			/* in minutes				*/
  ST_BOOLEAN     crlDropExistingConns;
  } SSC_CERT_PARAM;


typedef struct SSC_SSL_PARAM_tag
  {
  /* SSL connection parameters */
  S_CERT_CTRL   *sslLocalCertCtrlArr[S_MAX_LOCAL_SSL_CERTS]; /* SSL certs arr	*/
  ST_UINT        sslLocalCertCtrlArrCnt;
  ST_UINT16      sslCalledPort;			/* listening SSL port			*/
  ST_UINT16      sslCallingPort;		/* default calling port			*/
  ST_BOOLEAN     sslCalledPeerCertKnown;	/* verify that peer cert is configured	*/
  ST_BOOLEAN     sslCallingPeerCertKnown;	/* verify that peer cert is configured	*/

  /* SSL session renegotiation (rekeying) parameters */
  ST_UINT32      maxTranPerKey;
  ST_UINT32      maxTimePerKey;			/* in seconds				*/
  ST_INT         rekeyTimeout;			/* in seconds				*/

  ST_INT         cipherArr [S_MAX_ALLOWED_CIPHER_SUITES];
  ST_INT         cipherArrCnt;
  } SSC_SSL_PARAM;


typedef struct SSC_SNAP_PARAM_tag
  {

  /* SNAP-Lite secure connections parameters */
  ST_UINT        sslMaxCalled;				/* if 0 no limit on secured called  cons */
  ST_UINT        sslMaxCalling;				/* if 0 no limit on secured calling cons */
  ST_UINT16      sslProxyCalledPort;
  ST_UINT16      sslProxyCallingPort;
  ST_UINT        sslProxyConnectTimeout;		/* in milliseconds	*/

  /* SNAP-Lite IPC parameters */
  ST_UINT        maxUsers;
  ST_UINT        maxBindsPerUser;
  ST_UINT16      ipcPort;				/* for IPC on Windows	*/
  ST_CHAR        ipcDir [S_MAX_PATH];			/* for IPC on UNIX/Linux*/
  ST_INT         ipcTimeout;				/* in milliseconds	*/

  /* SNAP-Lite misc parameters */
  ST_UINT        cpuAffinity;				/* for future use	*/
  } SSC_SNAP_PARAM;


typedef struct SSC_AR_NAME_tag
  {
  DBL_LNK l;
  /* AR Name parameters */
  DIB_ENTRY      *arDib;	/* allocated to be added to app dib table	*/ 
  S_SEC_LOC_REM_AR *arSec;	/* allocated to be added to secLocRemArList	*/ 
  } SSC_AR_NAME;


#define  SSC_AR_NAME_ROLE_NONE		0	/* not assigned			*/
#define  SSC_AR_NAME_ROLE_LOCAL		1
#define  SSC_AR_NAME_ROLE_REMOTE	2

#define  SSC_APP_AR_NAME_ALL		"_ALL_"	/* a special remote AR Name - all remaining AR Names */

typedef struct SSC_APP_AR_NAME_tag
  {
  DBL_LNK l;
  /* AR Name parameters */
  ST_CHAR           arName [SSC_MAX_LEN_AR_NAME+1];
  ST_UINT           role;			/* one of SSC_AR_NAME_ROLE_...	*/
  } SSC_APP_AR_NAME;


typedef struct SSC_AR_MATCHING_PROFILE_tag
  {
  DBL_LNK l;
  /* AR Name matching parameters */
  ST_CHAR           name [SSC_MAX_LEN_NAME+1];		/* profile name		*/
  DIB_MATCH_CTRL   *dibMatch;
  } SSC_AR_MATCHING_PROFILE;


typedef struct SSC_SECURITY_PROFILE_tag
  {
  DBL_LNK l;
  /* Application security parameters */
  ST_CHAR           name [SSC_MAX_LEN_NAME+1];		/* profile name		*/
  ST_BOOLEAN        secure;
  ST_BOOLEAN        nonSecureFallbackEnabled;
  ST_UINT32         maceTimeSealWindow;
  ST_BOOLEAN        encryptReqCalled;
  ST_BOOLEAN        encryptReqCalling;
  } SSC_SECURITY_PROFILE;


typedef struct SSC_APP_NETWORK_tag
  {
  DBL_LNK l;
  /* Application Network parameters */
  ST_CHAR                  appName [SSC_MAX_LEN_NAME+1];
  SSC_LEANT_PROFILE       *leantProfile;	/* mandatory for all apps	*/
  SSC_SECURITY_PROFILE    *securityProfile;	/* mandatory for all apps	*/
  SSC_AR_MATCHING_PROFILE *arMatchingProfile;	/* should be present for all app except SNAP-Lite	*/
  SSC_APP_AR_NAME         *appArNameList;	/* should be present for all app except SNAP-Lite	*/
  /* lists below are created in sscConfigAppNetwork or sscUpdateAppNetwork	*/
  DIB_ENTRY               *dibEntryList;	/* list of ArNames used by app	*/ 
  S_SEC_LOC_REM_AR        *arLocRemSecList;	/* list of ArSec   used by app	*/ 

  ST_UINT                  maxMmsPduLength;
  ST_UINT                  maxConnections;	/* used by MMS-EASE		*/
  ST_UINT                  maxCallingConns;	/* used by MMS-LITE		*/
  ST_UINT                  maxCalledConns;	/* used by MMS-LITE		*/

  /* UNIX, Linux: optional Security Events Log (if not configured then application log file is used)	*/
  ST_CHAR                  secEventLogFileName[S_MAX_PATH] ;  /* every app has its own sec events file	*/
  ST_ULONG                 secEventLogFileSize;	/* size of circular log file	*/
  } SSC_APP_NETWORK;


typedef struct SSC_STACK_CFG_tag
  {
  ST_UINT                  cfgVersion;		/* initial defined as 1, change when cfg changes*/
  ST_INT64                 cfgVersionTime;	/* DEBUG: what is the time representation ?	*/
  ST_BOOLEAN               secureDllLib;	/* 0 if NULL Security DLL (NULL shared library)	*/

  SSC_NETWORK_ADAPTER     *networkAdapterList;
  SSC_LEANT_PROFILE       *leantProfileList;	/* mandatory for all applications and SNAP-Lite	*/

  SSC_CERT_PARAM          *certParam;		/* params related to checking certificates	*/
  S_CERT_CTRL             *trustedCaCertList;	/* Trusted CA list,   mandatory if secured 	*/
  S_CERT_CTRL             *certificateList;	/* Certificate list,  mandatory if secured 	*/
  
  SSC_SSL_PARAM           *sslParam;		/* SSL connections params, mandatory if secured */
  SSC_SNAP_PARAM          *snapParam;		/* SNAP-Lite parameters,   mandatory if SNAP-Lite*/
  SSC_AR_NAME             *arNameList;		/* mandatory for applications (except SNAP-Lite)*/
  SSC_AR_MATCHING_PROFILE *arMatchingProfileList;
  SSC_SECURITY_PROFILE    *securityProfileList;	/* mandatory for all applications and SNAP-Lite	*/
  SSC_APP_NETWORK         *appNetworkList;	/* mandatory for all applications and SNAP-Lite	*/

  SSC_APP_NETWORK         *appNetwork;		/* this is the application being configured	*/

  /* fields below are tmp during XML parsing	*/
  S_CERT_CTRL            **currCertList;
  S_CERT_NAME            **currCNameList;
  } SSC_STACK_CFG;


	/* =============================================================*/
	/* -------------------------------------------------------------*/
	/* 	SISCO's Stack Configuration USER interface functions	*/
	/* -------------------------------------------------------------*/
	/* =============================================================*/


/* user functions implemented in sStackCfg.c */
ST_RET         sscConfigAppNetwork (ST_CHAR *appName);
SSC_STACK_CFG *sscAccessStackCfg   (ST_VOID);
ST_RET         sscReleaseStackCfg  (ST_VOID);
ST_RET         sscCheckStackCfg    (ST_CHAR *appName);
ST_RET         sscFreeStackCfg     (ST_VOID);

/* configuration update functions (internal) */
ST_RET         sscFreeStackCfgPtr  (SSC_STACK_CFG *sscStackCfg);
ST_RET         sscReadStackCfg     (SSC_STACK_CFG **sscStackCfgOut);
ST_RET         sscUpdateAppNetwork (ST_CHAR *appName);

/* functions implemented in sStackCfgXml.c */
ST_RET sscReadStackCfgFromXmlFile (ST_CHAR *fileName, SSC_STACK_CFG  **sscStackCfgOut);

	/* =====================================================*/
	/*	SISCO Stack install path Set/Get functions	*/
	/* =====================================================*/

#if defined(_WIN32)
ST_RET   sscGetNetworkInstallPath  (ST_CHAR *pathBuf, ST_INT pathBufSize);
ST_RET   sscGetNetworkDataPath     (ST_CHAR *pathBuf, ST_INT pathBufSize);
ST_RET   sscGetCfgFilePath         (ST_CHAR *pathBuf, ST_INT pathBufSize);
ST_RET   sscGetUseCfgFileFlag      (ST_INT *useCfgFile);
#else  /* !defined(_WIN32) */
ST_RET   sscGetNetworkInstallPath  (ST_CHAR *pathBuf, ST_INT pathBufSize);
ST_RET   sscSetSiscoInstallPath    (ST_CHAR *pathBuf);
ST_RET   sscGetSiscoInstallPath    (ST_CHAR *pathBuf, ST_INT pathBufSize);
ST_CHAR *sscGetSiscoInstallPathPtr (ST_VOID);
ST_RET   sscGetCfgFilePath         (ST_CHAR *pathBuf, ST_INT pathBufSize);
ST_RET   sscSetCfgFilePath         (ST_CHAR *pathBuf);
#endif /* !defined(_WIN32) */

	/* =============================================================*/
	/* 	Define strings/value & tables for lookup functions	*/
	/* =============================================================*/

#define SSC_CIPHER_KEYEX	"KeyEx"
#define SSC_CIPHER_CRYPT	"Crypt"
#define SSC_CIPHER_HASH		"Hash"

	/* =====================================================*/
	/* 	Helper/lookup/Conversion functions		*/
	/* =====================================================*/

ST_RET    sscParseObjId (MMS_OBJ_ID *objId, ST_CHAR *buff);

	/* =====================================================*/
	/*	SISCO's Stack Configuration Find functions	*/
	/* =====================================================*/

SSC_NETWORK_ADAPTER     *sscFindNetworkAdapter    (SSC_STACK_CFG *sscStackCfg, ST_CHAR *adapterName);
SSC_LEANT_PROFILE       *sscFindLeantProfile      (SSC_STACK_CFG *sscStackCfg, ST_CHAR *profileName);
SSC_AR_NAME             *sscFindArName            (SSC_STACK_CFG *sscStackCfg, ST_CHAR *arName);
SSC_APP_AR_NAME         *sscFindAppArName         (SSC_APP_NETWORK *appNetwork, ST_CHAR *arName);
SSC_APP_AR_NAME         *sscFindNextAppArName     (SSC_APP_NETWORK *appNetwork, ST_INT role, SSC_APP_AR_NAME *sscAppArNameAfter);
SSC_SECURITY_PROFILE    *sscFindSecurityProfile   (SSC_STACK_CFG *sscStackCfg, ST_CHAR *profileName);
SSC_AR_MATCHING_PROFILE *sscFindArMatchingProfile (SSC_STACK_CFG *sscStackCfg, ST_CHAR *profileName);
SSC_APP_NETWORK         *sscFindAppNetwork        (SSC_STACK_CFG *sscStackCfg, ST_CHAR *appName);

	/* =============================================================*/
	/* 	SISCO Stack Configuration Add functions			*/
	/* =============================================================*/
	/* Functions to add subcomponents to the SSC_STACK_CFG.	These	*/
	/* functions allocate all needed pointers so they can be used	*/
	/* during Stack Configuration without recopying the content.	*/
	/* Where applicable the structures are initialized with default	*/
	/* parameters. 							*/
	/* =============================================================*/

SSC_NETWORK_ADAPTER     *sscAddNetworkAdapter     (SSC_STACK_CFG *sscStackCfg);
SSC_LEANT_PROFILE       *sscAddLeantProfile       (SSC_STACK_CFG *sscStackCfg);
SSC_CERT_PARAM          *sscAddCertParam          (SSC_STACK_CFG *sscStackCfg);
S_CERT_CTRL             *sscAddTrustedCaCert      (SSC_STACK_CFG *sscStackCfg);
S_CERT_CTRL             *sscAddCertificate        (SSC_STACK_CFG *sscStackCfg);
ST_RET                   sscAddCipher             (SSC_STACK_CFG *sscStackCfg, ST_INT cipherMask);
SSC_SSL_PARAM           *sscAddSslParam           (SSC_STACK_CFG *sscStackCfg);
SSC_SNAP_PARAM          *sscAddSnapParam          (SSC_STACK_CFG *sscStackCfg);
SSC_AR_NAME             *sscAddArName             (SSC_STACK_CFG *sscStackCfg);
SSC_APP_AR_NAME         *sscAddAppArName          (SSC_APP_NETWORK *appNetwork);
SSC_AR_MATCHING_PROFILE *sscAddArMatchingProfile  (SSC_STACK_CFG *sscStackCfg);
SSC_SECURITY_PROFILE    *sscAddSecurityProfile    (SSC_STACK_CFG *sscStackCfg);
SSC_APP_NETWORK         *sscAddAppNetwork         (SSC_STACK_CFG *sscStackCfg);

	/* =============================================================*/
	/* 	SISCO Stack Configuration Verify functions		*/
	/* =============================================================*/
	/* Functions to verify SISCO's Stack Configuration parameters.	*/
	/* Only elements that are needed for given application are	*/
	/* checked. All these verify functions are called fron the	*/
	/* user interface function sscVerifyAppNetwork.			*/
	/* =============================================================*/

ST_RET    sscVerifyLeantProfile      (SSC_STACK_CFG *sscStackCfg);
ST_RET    sscVerifySecurityProfile   (SSC_STACK_CFG *sscStackCfg);
ST_RET    sscVerifyArMatchingProfile (SSC_STACK_CFG *sscStackCfg);
ST_RET    sscVerifyCertParam         (SSC_STACK_CFG *sscStackCfg);
ST_RET    sscVerifyCaCertificates    (SSC_STACK_CFG *sscStackCfg);
ST_RET    sscVerifyCertificates      (SSC_STACK_CFG *sscStackCfg);
ST_RET    sscVerifySslParam          (SSC_STACK_CFG *sscStackCfg);
ST_RET    sscVerifySnapParam         (SSC_STACK_CFG *sscStackCfg, ST_BOOLEAN secureApp);
ST_RET    sscVerifyArName            (SSC_STACK_CFG *sscStackCfg, SSC_AR_NAME *sscArName);
ST_RET    sscVerifyAppArNameList     (SSC_STACK_CFG *sscStackCfg);
ST_RET    sscVerifyAppNetwork        (SSC_STACK_CFG *sscStackCfg, ST_CHAR *appName);
ST_RET    sscVerifyAppNetworkAllApps (SSC_STACK_CFG *sscStackCfg);

	/* =============================================================*/
	/* 	SISCO Stack Configuration Update functions		*/
	/* =============================================================*/
	/* Functions to update SISCO's Stack Configuration parameters.	*/
	/* These functions are called from the sscCheckStackCfg.	*/
	/* Note that some pointers may be moved during re-configuration	*/
	/* (sscConfigAppNetwork) to the application's runtime lists.	*/
	/* =============================================================*/

ST_RET sscUpdateNetworkAdapter (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew);
ST_RET sscUpdateLeantProfile   (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew);
ST_RET sscUpdateCertParam      (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew);
ST_RET sscUpdateCaCertificates (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew);
ST_RET sscUpdateCertificates   (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew);
ST_RET sscUpdateSslParam       (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew);
ST_RET sscUpdateSnapParam      (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew);
ST_RET sscUpdateAppArNameList  (SSC_STACK_CFG *sscStackCfg, SSC_STACK_CFG *sscStackCfgNew);
ST_RET sscUpdateAppNetwork     (ST_CHAR *appName);

	/* =============================================================*/
	/* 	SISCO Stack Configuration Free functions		*/
	/* =============================================================*/
	/* Functions to free SISCO's Stack Configuration parameters.	*/
	/* Note that some pointers may be moved during configuration	*/
	/* to the app's runtime lists freed during app's termination.	*/
	/* =============================================================*/

ST_RET sscFreeNetworkAdapterList    (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeLeantProfileList      (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeCertParam             (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeTrustedCaCertList     (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeCertificateList       (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeSslParam              (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeSnapParam             (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeArName                (SSC_STACK_CFG *sscStackCfg, SSC_AR_NAME *sscArName);
ST_RET sscFreeArNameList            (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeAppArNameList         (SSC_APP_NETWORK *appNetwork);
ST_RET sscFreeArMatchingProfileList (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeSecurityProfileList   (SSC_STACK_CFG *sscStackCfg);
ST_RET sscFreeAppNetworkList        (SSC_STACK_CFG *sscStackCfg);


#if defined(MMS_LITE)
/* Read SISCO Stack Configuration and fill in MVL_CFG_INFO structure.	*/
ST_RET sscConfigMvl (
	ST_CHAR *appName,	/* must match appName in configuration.	*/
	MVL_CFG_INFO *cfg_info);/* caller structure to fill in		*/
#endif

	/* =====================================================*/
	/* 	SISCO Stack Configuration Parsing functions	*/
	/* =====================================================*/

/* NOTE: need these prototypes because function pointers set in		*/
/*       one file but functions defined in other files.			*/

ST_VOID _ssc_Cert_Parameters_SEFun			(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_Trusted_CA_Cert_List_SEFun			(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_Certificate_List_SEFun			(SX_DEC_CTRL *sxDecCtrl);

ST_VOID _ssc_App_SNAP_Local_SSL_Certificate_List_SEFun	(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_Local_SSL_Certificate_EFun	(SX_DEC_CTRL *sxDecCtrl);

ST_VOID _ssc_App_SNAP_SSL_Called_Port_EFun		(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_SSL_Calling_Port_EFun		(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_SSL_Called_Peer_Cert_Known_EFun	(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_SSL_Calling_Peer_Cert_Known_EFun	(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_Max_Tran_Per_Key_EFun		(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_Max_Time_Per_Key_EFun		(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_Rekey_Timeout_EFun		(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_Cipher_List_SEFun			(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_Cipher_EFun			(SX_DEC_CTRL *sxDecCtrl);

ST_VOID _ssc_App_SNAP_SSL_Max_Called_EFun		(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_SSL_Max_Calling_EFun		(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_SSL_Proxy_Called_Port_EFun	(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_SSL_Proxy_Calling_Port_EFun	(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_App_SNAP_SSL_Connect_Timeout_EFun		(SX_DEC_CTRL *sxDecCtrl);

ST_VOID _ssc_AR_Name_Authentication_Certificate_List_SEFun	(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_AR_Name_Authentication_Certificate_EFun	(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_AR_Name_Encryption_EFun			(SX_DEC_CTRL *sxDecCtrl);
ST_VOID _ssc_AR_Name_SSL_Port_EFun			(SX_DEC_CTRL *sxDecCtrl);

	/* =====================================================*/
	/*	SISCO's Stack Configuration logging		*/
	/* =====================================================*/

#define SSC_LOG_ERR		0x00000001
#define SSC_LOG_NERR		0x00000002
#define SSC_LOG_FLOW		0x00000004

extern ST_UINT ssc_debug_sel;

extern LOGCFG_VALUE_GROUP sscLogMaskMapCtrl;

extern SD_CONST ST_CHAR *SD_CONST ssc_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST ssc_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST ssc_flow_logstr;

#define SSC_LOG_ERR0(a) \
	SLOG_0 (ssc_debug_sel & SSC_LOG_ERR, ssc_err_logstr, a)
#define SSC_LOG_ERR1(a,b) \
	SLOG_1 (ssc_debug_sel & SSC_LOG_ERR, ssc_err_logstr, a,b)
#define SSC_LOG_ERR2(a,b,c) \
	SLOG_2 (ssc_debug_sel & SSC_LOG_ERR, ssc_err_logstr, a,b,c)
#define SSC_LOG_ERR3(a,b,c,d) \
	SLOG_3 (ssc_debug_sel & SSC_LOG_ERR, ssc_err_logstr, a,b,c,d)

#define SSC_LOG_CERR0(a) \
	SLOGC_0 (ssc_debug_sel & SSC_LOG_ERR, a)
#define SSC_LOG_CERR1(a,b) \
	SLOGC_1 (ssc_debug_sel & SSC_LOG_ERR, a,b)
#define SSC_LOG_CERR2(a,b,c) \
	SLOGC_2 (ssc_debug_sel & SSC_LOG_ERR, a,b,c)
#define SSC_LOG_CERR3(a,b,c,d) \
	SLOGC_3 (ssc_debug_sel & SSC_LOG_ERR, a,b,c,d)

#define SSC_LOG_ERRH(a,b) \
	SLOGH (ssc_debug_sel & SSC_LOG_ERR,a,b)


#define SSC_LOG_NERR0(a) \
	SLOG_0 (ssc_debug_sel & SSC_LOG_NERR, ssc_nerr_logstr, a)
#define SSC_LOG_NERR1(a,b) \
	SLOG_1 (ssc_debug_sel & SSC_LOG_NERR, ssc_nerr_logstr, a,b)
#define SSC_LOG_NERR2(a,b,c) \
	SLOG_2 (ssc_debug_sel & SSC_LOG_NERR, ssc_nerr_logstr, a,b,c)
#define SSC_LOG_NERR3(a,b,c,d) \
	SLOG_3 (ssc_debug_sel & SSC_LOG_NERR, ssc_nerr_logstr, a,b,c,d)

#define SSC_LOG_CNERR0(a) \
	SLOGC_0 (ssc_debug_sel & SSC_LOG_NERR, a)
#define SSC_LOG_CNERR1(a,b) \
	SLOGC_1 (ssc_debug_sel & SSC_LOG_NERR, a,b)
#define SSC_LOG_CNERR2(a,b,c) \
	SLOGC_2 (ssc_debug_sel & SSC_LOG_NERR, a,b,c)
#define SSC_LOG_CNERR3(a,b,c,d) \
	SLOGC_3 (ssc_debug_sel & SSC_LOG_NERR, a,b,c,d)

#define SSC_LOG_NERRH(a,b) \
	SLOGH (ssc_debug_sel & SSC_LOG_NERR,a,b)


#define SSC_LOG_FLOW0(a) \
	SLOG_0 (ssc_debug_sel & SSC_LOG_FLOW, ssc_flow_logstr, a)
#define SSC_LOG_FLOW1(a,b) \
	SLOG_1 (ssc_debug_sel & SSC_LOG_FLOW, ssc_flow_logstr, a,b)
#define SSC_LOG_FLOW2(a,b,c) \
	SLOG_2 (ssc_debug_sel & SSC_LOG_FLOW, ssc_flow_logstr, a,b,c)
#define SSC_LOG_FLOW3(a,b,c,d) \
	SLOG_3 (ssc_debug_sel & SSC_LOG_FLOW, ssc_flow_logstr, a,b,c,d)

#define SSC_LOG_CFLOW0(a) \
	SLOGC_0 (ssc_debug_sel & SSC_LOG_FLOW, a)
#define SSC_LOG_CFLOW1(a,b) \
	SLOGC_1 (ssc_debug_sel & SSC_LOG_FLOW, a,b)
#define SSC_LOG_CFLOW2(a,b,c) \
	SLOGC_2 (ssc_debug_sel & SSC_LOG_FLOW, a,b,c)
#define SSC_LOG_CFLOW3(a,b,c,d) \
	SLOGC_3 (ssc_debug_sel & SSC_LOG_FLOW, a,b,c,d)

#define SSC_LOG_FLOWH(a,b) \
	SLOGH (ssc_debug_sel & SSC_LOG_FLOW,a,b)



#ifdef __cplusplus
}
#endif

#endif /* SSTACKCFG_INCLUDED */


