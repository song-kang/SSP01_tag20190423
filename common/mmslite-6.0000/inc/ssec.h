/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      	 2003 - 2012, All Rights Reserved                       */
/*                                                                      */
/* MODULE NAME : ssec.h		                                        */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION : This module provides user defines for SISCO's	*/
/*			Security Extensions for MMS-EASE, MMS-EASE Lite,*/
/*			ICCP, AXS4-MMS, AXS4-ICCP products.		*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev     Comments                                     */
/* --------  ---  ------   -------------------------------------------  */
/* 02/05/12  EJV	   S_SEC_CONFIG: added certExpiredDropExisting.	*/
/* 12/29/11  EJV           ssleSecuritySupported: removed dllexport to	*/
/*			     elim warning.				*/
/* 03/22/11  EJV     	   S_SEC_VERSION_STR: changed to V3.1000 for	*/
/*			     MMS-SECURE-14x-415 V3.0000.		*/
/* 02/21/11  EJV     	   ssleCertFileToXml, ssleCertFileToText: elim	*/
/*			     warning in 5th arg on Sun.			*/
/* 01/14/11  EJV     	   S_SEC_VERSION_STR: changed to V3.0200 for	*/
/*			     ICCP-LITE V5.4000 B5 & up.			*/
/* 10/23/10  EJV     	   S_SEC_VERSION_STR: changed to V3.0100 for	*/
/*			     ICCP-LITE V5.4000. Made S_SEC_VERSION_STR	*/
/*			     and S_SEC_LITESECURE_VERSION_STR the same.	*/
/* 09/04/10  EJV	   secManCfgChange: now used also for MMS_LITE.	*/
/* 06/22/10  EJV           Added sSecLog proto.				*/
/* 11/17/09  JRB    02     Def S_SEC_MAGIC_..., etc for ALL systems.	*/
/*			   Def ssleSecuritySupported for ALL systems.	*/
/*			   Move S_SEC_EVENT struct before prototypes.	*/
/*			   startSSLEngine: fix log funct ptr arg.	*/	
/* 10/07/09  EJV    01     New implementation. Works with SNAP-Lite.	*/
/************************************************************************/

#ifndef S_SEC_INCLUDED
#define S_SEC_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "glbsem.h"
#include "acseauth.h"
#include "gen_list.h"
#include "asn1r.h"
#include "dibmatch.h"
#include "mms_def2.h"	/* for MAX_AR_LEN  (including lean_a.h produces errors)	*/

/*--------------------------------------------------------------*/
/* Security product version					*/
/*--------------------------------------------------------------*/

/* for MMS-EASE Lite products over LEAN-T Stack with static security libs */
#define S_SEC_LITESECURE_NAME		"MMS-LITE-SECURE"
#define S_SEC_LITESECURE_VERSION_STR	S_SEC_VERSION_STR
/* Define Security Product version majorVer.minorVer				*/
#define S_SEC_MAGIC_NUM			((ST_UINT64) 0x53534543)   /* 0xSSEC	*/
#define S_SEC_VERSION_MAJOR             3	  /* number for easy ver cmp	*/
#define S_SEC_VERSION_MINOR             1000	  /* number for easy ver cmp	*/	
#define S_SEC_VERSION_STR	        "3.1000"  /* Security Product version	*/

#if defined(_WIN32)
  /* this is new SSL Engine DLL name with SISCO prefix */
  #if _DEBUG
    #define SSL_ENGINE_NAME		"SISCO.Security.SslEngine_ld.dll"
  #else
    #define SSL_ENGINE_NAME		"SISCO.Security.SslEngine_l.dll"
  #endif
#else  /* !defined(_WIN32) */
  /* UNIX, Linux,... */
  /* this is new SSL Engine shared lib name with SISCO prefix	*/
  #define SSL_ENGINE_NAME		"SISCO.Security.SslEngine_l.so"
#endif /* !defined(_WIN32) */


/************************************************************************/
/************************************************************************/
/* NOTES ON CERTIFICATE NAME MATCHING 					*/
/* This sSec API has a number of functions that must match certificates	*/
/* based on a S_CERT_ID data structure. In all cases the matching 	*/
/* process works as follows:						*/
/*									*/
/*   The matching can be based on certificate number, subject,		*/
/*   issuer, or any combination of these as specified in the certId.	*/
/*   For instance, the following specifications are valid:		*/
/*	1. Serial Number only						*/
/*	2. Subject only							*/
/*	3. Issuer only							*/
/*	4. Subject and Issuer						*/
/*	5. Subject and Serial Number					*/
/*	6. Issuer and Serial Number					*/
/*	7. Subject, Issuer, Serial Number				*/

/* Note that the all elements of the input 'certId' must be present and	*/
/* match exactly in the local certificate. That is, all elements of the	*/
/* certificate name must be present and in the specified order.		*/

/************************************************************************/
/************************************************************************/
#define SEC_LOG_ERR		0x00000001
#define SEC_LOG_NERR		0x00000002
#define SEC_LOG_FLOW		0x00000004
#define SEC_LOG_DATA  		0x00000008
#define SEC_LOG_DEBUG 		0x00000010

extern ST_UINT sec_debug_sel;
extern ST_UINT ssle_debug_sel;


/************************************************************************/
/* SNAP Startup Event Names */

#if defined(_WIN32)
#define SNAP_EXIT_EVENT_NAME		"snap.exit"
#endif /* defined(_WIN32) */

/************************************************************************/
/* Allowed authType values */

#define S_SEC_AUTHTYPE_NONE		0
#define S_SEC_AUTHTYPE_CERTIFICATE	1
#define S_SEC_AUTHTYPE_SYMMETRIC	2

/************************************************************************/
/* General Defines */

/* Maximum signed time data size */
#define S_SEC_MAX_SIGNED_TIME_LEN 128

/* Help in sizing the asn1 buffer (really more like 25 ...) */
#define S_SEC_ASN1_BUF_OH 50

/* Maximum size of the certificate serial number (in bytes) */
#define S_SEC_MAX_SERIAL_LEN 64

/************************************************************************/
/* MACE decode error codes */

#define MACE_ASN1_INCOMPLETE			0x9102
#define MACE_ASN1_SYMMETRIC_NOT_SUPPORTED	0x9103

/************************************************************************/
/* Default Ports							*/

/* SSL Port */
#define S_SEC_DEF_SSL_PORT		3782

/* IPC Ports */
#define S_SEC_DEF_PROXY_CALLED_PORT	10042
#define S_SEC_DEF_PROXY_CALLING_PORT	10043
#define S_SEC_DEF_SNAP_CONTROL_PORT	10044
#define S_SEC_DEF_SNAP_MONITOR_PORT	10045


/************************************************************************/
/* Cipher Suite Selection Defines */

#define S_MAX_ALLOWED_CIPHER_SUITES     49

/* Masks for Cipher Suite elements */
#define S_KEYX_MASK           0xff0000
#define S_CRYPT_MASK          0x00ff00
#define S_HASH_MASK           0x0000ff


/* Key Exchange Algorithms */
#define S_KEYX_NONE           0x000000
#define S_KEYX_NULL           0x010000
#define S_KEYX_RSA            0x020000
#define S_KEYX_RSA_EX         0x030000
#define S_KEYX_DH_DSS         0x040000
#define S_KEYX_DH_DSS_EX      0x050000
#define S_KEYX_DH_RSA         0x060000
#define S_KEYX_DH_RSA_EX      0x070000
#define S_KEYX_DHE_DSS        0x080000
#define S_KEYX_DHE_DSS_EX     0x090000
#define S_KEYX_DHE_RSA        0x0A0000
#define S_KEYX_DHE_RSA_EX     0x0B0000
#define S_KEYX_DH_ANON        0x0C0000
#define S_KEYX_DH_ANON_EX     0x0D0000
#define S_KEYX_FORTEZZA_DMS   0x0E0000

/* Encryption Algorithms */
#define S_CRYPT_NONE          0x000000
#define S_CRYPT_NULL          0x000100
#define S_CRYPT_RC4_40        0x000200
#define S_CRYPT_RC4_56        0x000300
#define S_CRYPT_RC4_128       0x000400
#define S_CRYPT_RC2_CBC_40    0x000500
#define S_CRYPT_IDEA_CBC      0x000600
#define S_CRYPT_DES40_CBC     0x000700
#define S_CRYPT_DES_CBC       0x000800
#define S_CRYPT_3DES_EDE_CBC  0x000900
#define S_CRYPT_FORTEZZA_CBC  0x000A00
#define S_CRYPT_AES_128_CBC   0x000B00
#define S_CRYPT_AES_256_CBC   0x000C00

/* Hash Algorithms */
#define S_HASH_NONE           0x000000
#define S_HASH_NULL           0x000001
#define S_HASH_MD5            0x000002
#define S_HASH_SHA            0x000003

/* Cipher Suite Seletion */
#define S_CIPHER_NOSSL	      0x000000
#define S_CIPHER_ANY          0xFFFFFF

/* 49 Cipher Suites are defined by SSL 3.0 and TLS 1.0 */
/* Bit fields:		  Key Exchange		Encryption		Hash	*/
#define S_CIPHER_NONE    (S_KEYX_NONE         | S_CRYPT_NONE          | S_HASH_NONE)
#define S_CIPHER_010103  (S_KEYX_NULL         | S_CRYPT_NULL          | S_HASH_SHA)
#define S_CIPHER_020102  (S_KEYX_RSA          | S_CRYPT_NULL          | S_HASH_MD5)
#define S_CIPHER_020103  (S_KEYX_RSA          | S_CRYPT_NULL          | S_HASH_SHA)
#define S_CIPHER_020402  (S_KEYX_RSA          | S_CRYPT_RC4_128       | S_HASH_MD5)
#define S_CIPHER_020403  (S_KEYX_RSA          | S_CRYPT_RC4_128       | S_HASH_SHA)
#define S_CIPHER_020603  (S_KEYX_RSA          | S_CRYPT_IDEA_CBC      | S_HASH_SHA)
#define S_CIPHER_020803  (S_KEYX_RSA          | S_CRYPT_DES_CBC       | S_HASH_SHA)
#define S_CIPHER_020903  (S_KEYX_RSA          | S_CRYPT_3DES_EDE_CBC  | S_HASH_SHA)
#define S_CIPHER_020B03  (S_KEYX_RSA          | S_CRYPT_AES_128_CBC   | S_HASH_SHA)
#define S_CIPHER_020C03  (S_KEYX_RSA          | S_CRYPT_AES_256_CBC   | S_HASH_SHA)
#define S_CIPHER_030202  (S_KEYX_RSA_EX       | S_CRYPT_RC4_40        | S_HASH_MD5)
#define S_CIPHER_030303  (S_KEYX_RSA_EX       | S_CRYPT_RC4_56        | S_HASH_SHA)
#define S_CIPHER_030502  (S_KEYX_RSA_EX       | S_CRYPT_RC2_CBC_40    | S_HASH_MD5)
#define S_CIPHER_030703  (S_KEYX_RSA_EX       | S_CRYPT_DES40_CBC     | S_HASH_SHA)
#define S_CIPHER_030803  (S_KEYX_RSA_EX       | S_CRYPT_DES_CBC       | S_HASH_SHA)
#define S_CIPHER_040803  (S_KEYX_DH_DSS       | S_CRYPT_DES_CBC       | S_HASH_SHA)
#define S_CIPHER_040903  (S_KEYX_DH_DSS       | S_CRYPT_3DES_EDE_CBC  | S_HASH_SHA)
#define S_CIPHER_040B03  (S_KEYX_DH_DSS       | S_CRYPT_AES_128_CBC   | S_HASH_SHA)
#define S_CIPHER_040C03  (S_KEYX_DH_DSS       | S_CRYPT_AES_256_CBC   | S_HASH_SHA)
#define S_CIPHER_050703  (S_KEYX_DH_DSS_EX    | S_CRYPT_DES40_CBC     | S_HASH_SHA)
#define S_CIPHER_060803  (S_KEYX_DH_RSA       | S_CRYPT_DES_CBC       | S_HASH_SHA)
#define S_CIPHER_060903  (S_KEYX_DH_RSA       | S_CRYPT_3DES_EDE_CBC  | S_HASH_SHA)
#define S_CIPHER_060B03  (S_KEYX_DH_RSA       | S_CRYPT_AES_128_CBC   | S_HASH_SHA)
#define S_CIPHER_060C03  (S_KEYX_DH_RSA       | S_CRYPT_AES_256_CBC   | S_HASH_SHA)
#define S_CIPHER_070703  (S_KEYX_DH_RSA_EX    | S_CRYPT_DES40_CBC     | S_HASH_SHA)
#define S_CIPHER_080403  (S_KEYX_DHE_DSS      | S_CRYPT_RC4_128       | S_HASH_SHA)
#define S_CIPHER_080803  (S_KEYX_DHE_DSS      | S_CRYPT_DES_CBC       | S_HASH_SHA)
#define S_CIPHER_080903  (S_KEYX_DHE_DSS      | S_CRYPT_3DES_EDE_CBC  | S_HASH_SHA)
#define S_CIPHER_090303  (S_KEYX_DHE_DSS_EX   | S_CRYPT_RC4_56        | S_HASH_SHA)
#define S_CIPHER_090703  (S_KEYX_DHE_DSS_EX   | S_CRYPT_DES40_CBC     | S_HASH_SHA)
#define S_CIPHER_090803  (S_KEYX_DHE_DSS_EX   | S_CRYPT_DES_CBC       | S_HASH_SHA)
#define S_CIPHER_080B03  (S_KEYX_DHE_DSS      | S_CRYPT_AES_128_CBC   | S_HASH_SHA)
#define S_CIPHER_080C03  (S_KEYX_DHE_DSS      | S_CRYPT_AES_256_CBC   | S_HASH_SHA)
#define S_CIPHER_0A0803  (S_KEYX_DHE_RSA      | S_CRYPT_DES_CBC       | S_HASH_SHA)
#define S_CIPHER_0A0903  (S_KEYX_DHE_RSA      | S_CRYPT_3DES_EDE_CBC  | S_HASH_SHA)
#define S_CIPHER_0A0B03  (S_KEYX_DHE_RSA      | S_CRYPT_AES_128_CBC   | S_HASH_SHA)
#define S_CIPHER_0A0C03  (S_KEYX_DHE_RSA      | S_CRYPT_AES_256_CBC   | S_HASH_SHA)
#define S_CIPHER_0B0703  (S_KEYX_DHE_RSA_EX   | S_CRYPT_DES40_CBC     | S_HASH_SHA)
#define S_CIPHER_0C0402  (S_KEYX_DH_ANON      | S_CRYPT_RC4_128       | S_HASH_MD5)
#define S_CIPHER_0C0803  (S_KEYX_DH_ANON      | S_CRYPT_DES_CBC       | S_HASH_SHA)
#define S_CIPHER_0C0903  (S_KEYX_DH_ANON      | S_CRYPT_3DES_EDE_CBC  | S_HASH_SHA)
#define S_CIPHER_0C0B03  (S_KEYX_DH_ANON      | S_CRYPT_AES_128_CBC   | S_HASH_SHA)
#define S_CIPHER_0C0C03  (S_KEYX_DH_ANON      | S_CRYPT_AES_256_CBC   | S_HASH_SHA)
#define S_CIPHER_0D0202  (S_KEYX_DH_ANON_EX   | S_CRYPT_RC4_40        | S_HASH_MD5)
#define S_CIPHER_0D0703  (S_KEYX_DH_ANON_EX   | S_CRYPT_DES40_CBC     | S_HASH_SHA)
#define S_CIPHER_0E0103  (S_KEYX_FORTEZZA_DMS | S_CRYPT_NULL          | S_HASH_SHA)
#define S_CIPHER_0E0403  (S_KEYX_FORTEZZA_DMS | S_CRYPT_RC4_128       | S_HASH_SHA)
#define S_CIPHER_0E0A03  (S_KEYX_FORTEZZA_DMS | S_CRYPT_FORTEZZA_CBC  | S_HASH_SHA)

/************************************************************************/
/* Certificate Names                                                    */

/* nameType defines */
#define S_CERT_NAME_TYPE_ANY                	0
#define S_CERT_NAME_TYPE_COMMON_NAME            1
#define S_CERT_NAME_TYPE_COUNTRY                2
#define S_CERT_NAME_TYPE_STATE_OR_PROVINCE      3
#define S_CERT_NAME_TYPE_LOCALITY               4
#define S_CERT_NAME_TYPE_ORGANIZATION           5
#define S_CERT_NAME_TYPE_EMAIL                  6
#define S_CERT_NAME_TYPE_ORGANIZATIONAL_UNIT    7

#define S_CERT_CNAME_TYPE_STR_ANY                    "ANY"
#define S_CERT_CNAME_TYPE_STR_COMMON_NAME            "CN"
#define S_CERT_CNAME_TYPE_STR_COUNTRY_NAME           "C"
#define S_CERT_CNAME_TYPE_STR_STATE_OR_PROVINCE_NAME "S"
#define S_CERT_CNAME_TYPE_STR_LOCALITY               "L"
#define S_CERT_CNAME_TYPE_STR_ORGANIZATION           "O"
#define S_CERT_CNAME_TYPE_STR_EMAIL_ADDRESS          "E"
#define S_CERT_CNAME_TYPE_STR_ORGANIZATION_UNIT_NAME "OU"

#define S_CERT_NAME_MAX_LEN			1024

/* A name in a certificate is a list of names and name types            */
typedef struct  S_CERT_NAME_tag
  {
  DBL_LNK l;
  ST_INT nameType;
  ST_CHAR *nameText;
  } S_CERT_NAME;


/************************************************************************/
/* Identifying a Certificate */

#define S_SERIAL_NUM_NONE -1

/* defines used to set privateKey and privateKeyExportable fields in S_CERT_ID			*/
#define S_CERT_PRIVATE_KEY_PRESENT_UNKNOWN	0	/* unable to tell if private key is present	*/
#define S_CERT_PRIVATE_KEY_PRESENT_NO		1
#define S_CERT_PRIVATE_KEY_PRESENT_YES		2

#define S_CERT_PRIVATE_KEY_EXPORTABLE_UNKNOWN	0	/* unable to tell if private key is exportable	*/
#define S_CERT_PRIVATE_KEY_EXPORTABLE_NO	1
#define S_CERT_PRIVATE_KEY_EXPORTABLE_YES	2

#define S_CERT_TYPE_UNKNOWN			0
#define S_CERT_TYPE_LOCAL			1
#define S_CERT_TYPE_REMOTE			2
#define S_CERT_TYPE_CA				4

#define S_CERT_TYPE_STR_UNKNOWN			"Unknown"
#define S_CERT_TYPE_STR_LOCAL			"Local"
#define S_CERT_TYPE_STR_REMOTE			"Remote"
#define S_CERT_TYPE_STR_CA			 "CA"

#define S_CERT_OK				0	/* SD_SUCCESS			*/

typedef struct  S_CERT_VALIDITY_tag
  {
  time_t        validNotBefore;   /* valid from				*/
  time_t        validNotAfter;    /* valid to				*/
  ST_CHAR       validFrom[64];    /* cert validity time string in format:*/
  ST_CHAR       validTo[64];      /* "yyyy-mm-dd hh:mm:ssZ"		*/
  } S_CERT_VALIDITY;

typedef struct  S_CERT_ID_tag
  {
  /* Note: adding/removing fields to/from this struct will make older	*/
  /* !!!   applications incompatible with new security DLLs.		*/
  ST_UINT      certType;	/* one of the defined S_CERT_TYPE_*	*/
  ST_UINT      certStatus;	/* S_CERT_OK - good or unverified cert,	*/
                                /* otherwise S_SEC_CHECK_ERR_CERT_...	*/
  ST_CHAR     *certName;	/* NULL if no associated name, alloc max S_CERT_NAME_MAX_LEN */
  ST_INT       serialLen;	 /* 0 == wildcard  			*/
  ST_UCHAR     serialNumber[S_SEC_MAX_SERIAL_LEN]; /* reverse from mmc	*/
  S_CERT_NAME *issuerCName;      /* NULL  == wildcard  			*/
  S_CERT_NAME *subjectCName;	 /* NULL  == wildcard  			*/

  S_CERT_VALIDITY validity;	 /* extracted from certificate		*/

  /* informational (optional) fields read from security cfg file,	*/
  /* not used in runtime processing					*/
  ST_INT       privateKey;	      /* S_CERT_PRIVATE_KEY_PRESENT_x	*/
  ST_INT       privateKeyExportable;  /* S_CERT_PRIVATE_KEY_EXPORTABLE_x*/
  } S_CERT_ID;


/************************************************************************/
/* Certificate BLOB, X.509 format                                       */

typedef struct  S_CERT_BLOB_tag
  {
  /* Note: adding/removing fields to/from this struct will make older	*/
  /* !!!   applications incompatible with new security DLLs.		*/
  ST_INT    certDataLen;
  ST_UCHAR *certData;
  } S_CERT_BLOB;

/************************************************************************/
/* Signed Time								*/

typedef struct  S_SIGNED_TIME_tag
  {
  /* Note: adding/removing fields to/from this struct will make older	*/
  /* !!!   applications incompatible with new security DLLs.		*/
  ST_INT    signedTimeLen;
  ST_UCHAR *signedTimeData;
  } S_SIGNED_TIME;

/************************************************************************/
/* MACE authentication information */

typedef struct  S_MACE_INFO_tag
  {
  ST_INT authType;
  union
    {
    struct
      {
      S_CERT_BLOB certBlob; 
      time_t signedTimeT;
      
      /* UTC NULL-terminated time string to be verified */
      ST_UCHAR clearTime[S_SEC_MAX_SIGNED_TIME_LEN];
      /* UTC time string signed by the sender */
      S_SIGNED_TIME signedTime;
      
      } certBased;
    struct
      {
      ST_INT placeHolder;
      } symmetric;
    } u; 
  } S_MACE_INFO;
  

/************************************************************************/
/************************************************************************/
/* Certificate Content Information */

typedef struct  S_CERT_INFO_tag
  {
  /* Note: adding/removing fields to/from this struct will make older	*/
  /*       applications incompatible with new security DLLs.		*/
  ST_INT version;	/* X509 version */
  S_CERT_ID certId;
#if 0
  struct
    {
    ST_INT publicKeyAlgorithm;
    ST_INT algorithmIdentifier;
    } algorithms;
#endif
  
  ST_UCHAR *publicKey;
  ST_INT publicKeyLen;
  } S_CERT_INFO;


/************************************************************************/
/************************************************************************/
/* Security Configuration 						*/
/************************************************************************/

/* List of Certificate ID's */
typedef struct  S_CERT_CTRL_tag
  {
  DBL_LNK l;
  S_CERT_ID  certId;

/* Following 3 fields are currently not used (only the global rekey	*/
/* parameters are used). 						*/
/* Rekey Control. These override the global rekey parameters if != 0 	*/
/* They apply only to remote certificates				*/
  ST_UINT32  maxTransactionsPerKey;
  ST_UINT32  maxTimePerKey;			/* in seconds */
  ST_INT   rekeyTimeout;			/* in seconds */

/* Internal use */
  ST_UINT32  cfgId;		/* computed 32-bit CRC on certId	*/
  				/* enabled fields			*/
  } S_CERT_CTRL;

/************************************************************************/
/* AR Security Mapping Defines */

/* appAuthRequired defines */
/* These defines come into play only if 'Remote AR Security' is not	*/
/* found for an incoming connection					*/

/* 'Remote AR Security' is required; reject all other indications 	*/
#define  S_APP_AUTHREQ_AR_SEC			0
/* A configured MACE certificate is required				*/
#define  S_APP_AUTHREQ_MACE			1
/* A configured MACE certificate is required and encryption is required	*/
#define  S_APP_AUTHREQ_MACE_ENCRYPTED		2
/* Encryption is required						*/
#define  S_APP_AUTHREQ_ENCRYPTED		3
/* There are no requirements; all connections are acceptable		*/
#define  S_APP_AUTHREQ_NONE			4



#define S_AR_AUTH_MODE_AETITLE		0	/* AE TITLE matching only	*/
#define S_AR_AUTH_MODE_MACE		1	/* remote MACE certificate	*/
#define S_AR_AUTH_MODE_SSL		2	/* remote SSL  certificate	*/
/* For backward compatibility only */
#define S_AR_AUTH_MODE_NONE		S_AR_AUTH_MODE_AETITLE

#define S_MAX_LOCAL_SSL_CERTS		2	/* max local  SSL  certificates */
/* #define S_MAX_LOCAL_MACE_CERTS	S_AR_AUTH_MAX_CERTS	in 142xxx	*/
#define S_AR_AUTH_MAX_CERTS		8	/* max remote authentication certs (MACE or SSL) */

/* Local & Remote AR Security Configuration 				*/
/* Note that the Local is used to select the local MACE certificate for	*/
/* connections to a remote with arAuthMode == S_AR_AUTH_MODE_MACE	*/
/* Note that the Remote is used to determine the ACSE authentication	*/
/* and encryption to be used for both calling and called connections,	*/
/* and is used in identifying the remote for called connections		*/
typedef struct  S_SEC_LOC_REM_AR_tag
  {
  DBL_LNK l;
  ST_CHAR    arName[MAX_AR_LEN + 1];	/* Local or Remote AR Name	*/

  /* Authentication, see  S_AR_AUTH_MODE_... defines */
  ST_INT     arAuthMode;

/* Authentication Certificate:	*/
					/* cfg preferred MACE cert first*/
  S_CERT_CTRL *authCertArr [S_AR_AUTH_MAX_CERTS];
  ST_INT       authCertArrCnt;
  	/* Changed to array of pointers (can not be a list of certCtrl	*/
  	/* because these pointers are already on the locRemCertList).	*/

/* Encryption */
  ST_INT     encryptMode;
  ST_UINT16  sslPort;		 /* For S_SEC_ENCRYPT_SSL 		*/

/* User */
  ST_VOID *usr;
  } S_SEC_LOC_REM_AR;

/************************************************************************/
/* code to return security check errors					*/

#define S_SEC_CHECK_ERR_NONE				0	/* SD_SUCCESS						*/
#define S_SEC_CHECK_ERR_PROCESSING_ERROR		1	/* SD_FAILURE - processing error			*/
#define S_SEC_CHECK_ERR_NOT_ENCRYPTED			2	/* Encryption expected but not present			*/
#define S_SEC_CHECK_ERR_MACE_AUTH_NOT_PRESENT		3	/* MACE cert expected but not present in Assoc-Ind/Cnf	*/
#define S_SEC_CHECK_ERR_MACE_CERT_TIME_SEAL		4       /* MACE cert Time Seal out of configured range		*/
#define S_SEC_CHECK_ERR_MACE_CERT_TIME_INVALID		5	/* MACE cert expired or not yet valid			*/
#define S_SEC_CHECK_ERR_MACE_CERT_UNKNOWN_ISSUER	6	/* MACE cert signed by unknown CA (not configured)	*/
#define S_SEC_CHECK_ERR_MACE_CERT_DEC_FAILED		7	/* MACE cert decoding/verification failed		*/
#define S_SEC_CHECK_ERR_MACE_CERT_SIG_TIME_NOT_VERIFIED	8	/* MACE cert Signed Time verification failed		*/
#define S_SEC_CHECK_ERR_MACE_CERT_NOT_CONFIG		9	/* MACE cert not found in Remote Certs List 		*/
#define S_SEC_CHECK_ERR_MACE_CERT_NOT_CONFIG_IN_AR_SEC	10	/* MACE cert not found in Remote Certs List (in ARSec)	*/
#define S_SEC_CHECK_ERR_SSL_CERT_NOT_CONFIG		11	/* SSL cert not found in Remote Certs List		*/
#define S_SEC_CHECK_ERR_SSL_CERT_NOT_CONFIG_IN_AR_SEC	12	/* SSL cert not found in Remote Certs List (in ARSec)	*/
#define S_SEC_CHECK_ERR_REM_AR_SEC_NOT_CONFIG		13	/* Remote AR Security not configured for AR Name 	*/
#define S_SEC_CHECK_ERR_LOC_AR_SEC_NOT_CONFIG		14	/* Local  AR Security not configured for AR Name 	*/
#define S_SEC_CHECK_ERR_LOC_AR_SEC_MACE_NOT_CONFIG	15	/* MACE cert not configured in Loc AR Security		*/
#define S_SEC_CHECK_ERR_LOC_AR_SEC_MACE_FAILED_TO_LOAD	16	/* Failed to retrieve MACE cert BLOB for Loc AR Security*/
#define S_SEC_CHECK_ERR_REM_AE_TITLE_NOT_MATCHED	17	/* Rem AE Title does not match config Remote AR Security */
#define S_SEC_CHECK_ERR_LICENSE				18	/* License check failed					*/
#define S_SEC_CHECK_ERR_MISC				19	/* placeholder for an error				*/
/* certificate verification errors */
#define S_SEC_CHECK_ERR_CERT_EXPIRED			20	/* Certificate expired					*/
#define S_SEC_CHECK_ERR_CERT_EXPIRING			21	/* Certificate will expire within period limit		*/
#define S_SEC_CHECK_ERR_CERT_NOT_YET_VALID		22	/* Certificate not yet valid				*/
#define S_SEC_CHECK_ERR_CERT_REVOKED			23	/* Certificate revoked					*/
#define S_SEC_CHECK_ERR_CERT_VERIFY_FAILED		24	/* Certificate verification error			*/
#define S_SEC_CHECK_ERR_CERT_KEY_SIZE			25	/* Certificate Key size is > 1024			*/



extern ST_CHAR *sSecCheckErrCodeStr[];
extern ST_UINT  sSecCheckErrCodeStrCnt;

/* macro to get the error code string */
#define  S_SEC_CHECK_ERR_CODE_STR(code) (((ST_UINT) code < sSecCheckErrCodeStrCnt) ? sSecCheckErrCodeStr[code] : "UNKNOWN CODE")


/************************************************************************/
/**** Main Configuration Structure ****/

#define S_SSL_MAX_LISTEN_PORTS          10
#define S_SSL_PROXY_CON_TIMEOUT         (10*1000)	/* ms, default	*/

typedef struct  S_SEC_CONFIG_tag
  {
  /* Note: adding/removing fields to/from this struct will make older	*/
  /* !!!   applications incompatible with new security DLLs.		*/

  /* to detect if compatible struct is used by application and DLL	*/
  ST_UINT64  sSecMagicNum;		/* S_SEC_MAGIC_NUM		*/
  ST_UINT32  sSecMajorVer;		/* S_SEC_VERSION_MAJOR		*/
  ST_UINT32  sSecMinorVer;		/* S_SEC_VERSION_MINOR		*/

  /****** General Configuration ******/

  ST_BOOLEAN secureModeEnabled;

  ST_BOOLEAN nonSecureFallbackEnabled;	/* allow non-secure connection	*/
					/* for remAR cfg with SSL/MACE	*/

  /****** Certificates general ******/

  /* to check certificates expiration */  
  ST_UINT32 certExpirationEvalInterval;		/* in hours, how often to check for cert expiration	*/
						/* if 0 do not check for expiration			*/
  ST_UINT32 certExpirationAlarmLimit;		/* in hours, how far ahead to alarm about cert expiration*/
						/* max 10,000 hours (1 year=8784 hours)			*/
  ST_DOUBLE certExpirationLastCheckTime;	/* time when certificates expiration was last checked	*/
  ST_BOOLEAN certExpiredDropExisting;

  /* CRL Control */
  ST_INT     crlCheckTime;			/* In minutes */
  ST_BOOLEAN crlDropExisting;
 
  /****** Certificate Lists ******/

  /* Acceptable CA Certificates */
  S_CERT_CTRL *sslTrustedCaCertList;

  /* Local & Remote Certificates for SSL and MACE use */
  S_CERT_CTRL *locRemCertList;

  /****** SSL Configuration ******/

  /* Local Certificate to be used for SSL connection */
  S_CERT_CTRL *sslLocalCertCtrlArr[S_MAX_LOCAL_SSL_CERTS]; /* SSL certs arr	*/
  ST_UINT      sslLocalCertCtrlArrCnt;
  S_CERT_CTRL *sslLocalCertCtrl;  /* currently loaded SSL cert into OpenSSL	*/

  /* Listen Port Numbers (only one used) */
  ST_INT numSslListenPorts;
  ST_UINT16 sslListenPorts[S_SSL_MAX_LISTEN_PORTS];

  /* Allowed Ciphers, ordered by preference */
  ST_INT numAllowedCipherSuites;
  ST_INT allowedCipherSuites[S_MAX_ALLOWED_CIPHER_SUITES];

  /* Flags */
  ST_BOOLEAN certAuthCalling;	/* verify that peer's SSL certificate is configured */
  ST_BOOLEAN certAuthCalled;	/* verify that peer's SSL certificate is configured */
  ST_BOOLEAN encryptReqCalling;	/* encryption required on calling connection	*/
  ST_BOOLEAN encryptReqCalled;	/* encryption required on called  connection	*/

  /* Rekey Control */
  ST_UINT32   maxTransactionsPerKey;
  ST_UINT32   maxTimePerKey;			/* in seconds */
  ST_INT      rekeyTimeout;			/* In seconds */

 
  /****** MACE Level Configuration ******/

  /* Time Signature Window */
  ST_UINT32 timeSealWindow;

  /****** Application Security Configuation ******/

  ST_INT appAuthRequired;

  /* Well Known AR Names */
  S_SEC_LOC_REM_AR *secLocRemArList;

  DIB_MATCH_CTRL *dibMatch;		/* RemArAddrMatching - allocated ptr	*/

  /****** SNAP Configuation *******/

  ST_UINT16 defCallingSslPort;		/* Default SSL Calling Port Number	*/

  /* Misc */
  ST_UINT sslMaxCalling;		/* max SSL calling connections		*/
  ST_UINT sslMaxCalled;			/* max SSL called  connections		*/

  ST_INT cpuAffinity;

  /* SSL connection timeout, used by SNAP-Lite. Default 10000ms			*/
  ST_UINT sslProxyConnectTimeout;	/* In milliseconds			*/

  /****** SNAP-Lite Ports & IP Addresses */
  ST_UINT16 sslProxyCalledPort;		/* Called connections proxy port	*/
  ST_UINT16 sslProxyCallingPort;	/* Calling connections proxy port	*/
  ST_UINT16 snapControlListenPort;	/* IPC port 				*/
  ST_UINT16 snapMonitorListenPort;	/* not implemented			*/

  /****** Security Logging ******/

  /* UNIX, Linux: Security Event logging, if not config then app log file is used */
  ST_CHAR  *secEventLogFileName;
  ST_ULONG  secEventLogFileSize;

  ST_INT    ipcTimeout;			/* In milliseconds */

  ST_CHAR  *instPath;			/* UNIX: ptr to buff holding InstallPath*/
					/* needed by SSL Engine to find license	*/
  } S_SEC_CONFIG;


/************************************************************************/
/************************************************************************/

#define S_SEC_ENCRYPT_NONE	0
#define S_SEC_ENCRYPT_SSL	1
typedef struct  S_SEC_ENCRYPT_CTRL_tag
  {
  ST_INT   encryptMode;
  union
    {
    struct 
      {
      S_CERT_CTRL *localSslCert;      /* used this local SSL cert	*/
      ST_UINT16    port;	      /* for outgoing connection	*/
      ST_BOOLEAN   sslCertMatched;    /* The Subject-to-CertId succeded */
      S_CERT_CTRL *sslCert;	      /* partner's matched SSL cert ctrl*/
      ST_INT       cipherSuite;	      /* Cipher suite in use		*/
      ST_CHAR      cipherSuiteStr[80];/* Cipher suite in use as OpenSSL string	*/
      } ssl;
    } u;
  } S_SEC_ENCRYPT_CTRL;

/* Struct passed back and forth to SUIC */
typedef struct  S_SEC_SPARAMS_tag
  {
  ACSE_AUTH_INFO *authInfo;
  S_CERT_CTRL    *maceCert;
  ACSE_AUTH_INFO *partAuthInfo;
  S_CERT_CTRL    *partMaceCert;
  S_SEC_ENCRYPT_CTRL *encryptCtrl;
  ST_DOUBLE          *certExpirationLastCheckTime;
  } S_SEC_SPARAMS;

/************************************************************************/

/* Application Level Authentication Check Result */
typedef struct  S_SEC_AUTHCHK_RSLT_tag
  {
/* Encryption in use */
  S_SEC_ENCRYPT_CTRL encryptInfo;

/* Authentication in use */
  ST_INT authPres;
  ST_INT mechType;
  union
    {
    struct
      {
      ST_BOOLEAN   timeSealOk;          /* Timeseal not reused, in window */
      ST_BOOLEAN   maceCertTimeValid;   /* Certificate validity times OK  */
      ST_BOOLEAN   maceCertIssuerOk;    /* The issuer is in the OK list   */
      ST_BOOLEAN   maceCertMatched;     /* The Subject-to-CertId succeded */
      S_CERT_CTRL *maceCert;	        /* The matched MACE cert ctrl	  */
      } certBased;
    struct
      {
      ST_INT placeHolder;
      } symmetric;
    struct
      {
      ST_INT placeHolder2;
      } passsword;
    } u; 
  } S_SEC_AUTHCHK_RSLT;

/* Logging eventType defines */
#define S_SEC_EVENT_LOGGING_STARTED	   1	/* for debugging			*/
#define S_SEC_EVENT_UNKNOWN_REMOTE	   2	/* peer's SSL certificate not configured*/
#define S_SEC_EVENT_UNKNOWN_CA		   3	/* CA cert not installed on local system*/
#define S_SEC_EVENT_SSL_CERT_FAILURE	   4	/* Rem SSL cert failed OpenSSL verify during connection */
#define S_SEC_EVENT_SSL_FAILURE		   5	/* TLS protocol error			*/
#define S_SEC_EVENT_REKEY_FAILURE	   6	/* failed to rekey TLS session		*/
#define S_SEC_EVENT_NO_CIPHERS		   7	/* not set 				*/
#define S_SEC_EVENT_CERT_NOT_YET_VALID     8	/* Loc/Rem certificate not yet valid	*/
#define S_SEC_EVENT_CERT_EXPIRING          9	/* Loc/Rem certificate is about to expire*/
#define S_SEC_EVENT_CERT_EXPIRED          10	/* Loc/Rem certificate expired		*/
#define S_SEC_EVENT_CERT_REVOKED          11	/* Loc/Rem certificate revoked		*/
#define S_SEC_EVENT_CA_CERT_EXPIRING	  12	/* CA certificate is about to expire	*/
#define S_SEC_EVENT_CA_CERT_EXPIRED	  13	/* CA certificate expired		*/
#define S_SEC_EVENT_SSL_CERT_FAILED_CHK	  14	/* Loc/Rem SSL  cert failed OpenSSL verify*/
#define S_SEC_EVENT_MACE_CERT_FAILURE	  15	/* Loc/Rem MACE cert decode/verify failed */
						/* few spare numbers for security lib 	*/
/* these events are set by user application */
#define S_SEC_EVENT_APP_SEC_ERROR	  20	/* app error security event message	*/
#define S_SEC_EVENT_APP_SEC_ERROR_CERT	  21	/* app error security event message,	*/
						/* problematic certificate info provided*/
#define S_SEC_EVENT_APP_SEC_WARNING	  22	/* app warning security event message	*/
#define S_SEC_EVENT_APP_SEC_WARNING_CERT  23	/* app warning security event message,	*/
						/* problematic certificate info provided*/

typedef struct  S_SEC_EVENT_tag
  {
  ST_BOOLEAN logSystemEvent;	/* Log to System event system		*/
  ST_INT eventType;	
  ST_INT8 numArgs;
  ST_CHAR *eventArgs[32];

  ST_BOOLEAN logToSlog;		/* Log to SLOG file 			*/
  ST_INT logType; 		/* Standard SLOG information		*/
  ST_CHAR *SD_CONST logTypeStr; 
  ST_CHAR *SD_CONST sourceFile; 
  ST_INT lineNum;
  ST_INT bufLen; 
  ST_CHAR *buf;
  } S_SEC_EVENT;

ST_VOID sSecLog (S_SEC_EVENT *secEvent);

/************************************************************************/
/* Look up configured security information by AR Name */
ST_RET  sSecArNameToArSec (ST_CHAR *arName, S_SEC_LOC_REM_AR **arSecOut);
/* defines for backward compatability */
#define sSecRemArNameToArSec(arName,arSecOut)	sSecArNameToArSec(arName,arSecOut)
#define sSecLocArNameToArSec(arName,arSecOut)	sSecArNameToArSec(arName,arSecOut)

/************************************************************************/
/* These functions can be used to select and examine SSL level info	*/
/* for the selected connection						*/
#if defined(MMS_LITE)
/* ssecusr.c functions */
struct mvl_net_info;					/* forward reference	*/
ST_RET sSecAssocIndChk (struct mvl_net_info *cc, DIB_MATCH_CTRL *matchCtrl, 
			S_SEC_LOC_REM_AR **arSecOut, S_SEC_AUTHCHK_RSLT *srcChkDest);
ST_RET sSecAssocConfChkEx (struct mvl_net_info *cc, DIB_MATCH_CTRL *matchCtrl,
                           S_SEC_LOC_REM_AR *remArSec, S_SEC_AUTHCHK_RSLT *srcChkDest);
ST_RET sSecAuthChk (struct mvl_net_info *cc, S_SEC_AUTHCHK_RSLT *srcChkDest);
ST_RET sSecGetSecParam (struct mvl_net_info *cc, S_SEC_SPARAMS *secParams);
ST_RET sSecFreeSecParam (ACSE_AUTH_INFO *authInfo);
ST_RET sSecFreePartSecParam (struct mvl_net_info *cc);
ST_RET sSecVerifyMaceCerts (struct mvl_net_info *cc, ST_INT certType);

/* For backward compatability */
#define sSecAssocConfChk(cc,remArSec,secChkDest)   sSecAssocConfChkEx(cc,NULL,remArSec,secChkDest)
#else  /* !defined(MMS_LITE) */
/* ssecusr.c functions */
ST_RET sSecAssocIndChk (ST_INT chan, DIB_MATCH_CTRL *matchCtrl, 
			S_SEC_LOC_REM_AR **arSecOut, S_SEC_AUTHCHK_RSLT *srcChkDest);
ST_RET sSecAssocConfChkEx (ST_INT chan, DIB_MATCH_CTRL *matchCtrl,
			   S_SEC_LOC_REM_AR *remArSec, S_SEC_AUTHCHK_RSLT *srcChkDest);
ST_INT sSecAuthChk (ST_INT chan, S_SEC_AUTHCHK_RSLT *srcChkDest);
ST_RET sSecSetSecParam (ST_INT chan, S_SEC_SPARAMS *secParams);
ST_RET sSecGetSecParam (ST_INT chan, S_SEC_SPARAMS *secParams);
ST_RET sSecVerifyMaceCerts (ST_INT chan, ST_INT certType);

/* For backward compatability */
#define sSecAssocConfChk(chan,remArSec,secChkDest) sSecAssocConfChkEx(chan,NULL,remArSec,secChkDest)
#endif /* !defined(MMS_LITE) */



/************************************************************************/
/************************************************************************/
/************************************************************************/
/* Initialize/Terminate 						*/

ST_RET sSecUsrStart (S_SEC_CONFIG **secCfgOut);
ST_RET sSecUsrEnd (ST_VOID);
ST_RET sSecStart (S_SEC_CONFIG **secCfg);
ST_RET sSecEnd (ST_VOID);

/************************************************************************/
/* Configuration */

/* Used by app to reload security components */
ST_RET sSecUpdate(S_SEC_CONFIG *currCfg, S_SEC_CONFIG *newCfg);

/************************************************************************/
/* MACE Authentication 							*/

/* Create/Free MACE Authentication Information for a local CertId	*/
ST_RET sSecMaceAuthInfoCreate (S_SEC_LOC_REM_AR *locArSec, ACSE_AUTH_INFO *authInfo,
                               S_CERT_CTRL **maceCertOut);

ST_VOID sSecMaceAuthInfoFree (ACSE_AUTH_INFO *auth_info);

/* Decode MACE Authentication Information */
ST_RET sSecMaceAuthInfoDec (ACSE_AUTH_INFO *authInfo, ST_UINT certType,
			    S_MACE_INFO **maceInfoOut, 
			    S_CERT_INFO **certInfoOut);
ST_VOID sSecFreeCertInfo (S_CERT_INFO *certInfo);

/************************************************************************/
/************************************************************************/
/* Finding things in the Security Configuration				*/

/**** Find configured CertCtrl for given certName ****/
ST_RET sSecCertNameToCertCtrl (ST_CHAR *certName, S_CERT_CTRL **certCtrlOut);
#define sSecCertNameToLocCertCtrl(certName,certCtrlOut)	 sSecCertNameToCertCtrl(certName,certCtrlOut)
#define sSecCertNameToRemCertCtrl(certName,certCtrlOut)	 sSecCertNameToCertCtrl(certName,certCtrlOut)

/**** Find configured CertCtrl for given CertId ****/
ST_RET sSecCertIdToCertCtrl (S_CERT_ID *certId, S_CERT_CTRL **certCtrlOut);
#define  sSecCertIdToRemCertCtrl(certId,certCtrlOut)	 sSecCertIdToCertCtrl(certId,certCtrlOut)
#define  sSecCertIdToLocCertCtrl(certId,certCtrlOut)	 sSecCertIdToCertCtrl(certId,certCtrlOut)

/**** Find configured CertCtrl for given cfgId ****/
ST_RET sSecCertCfgIdToCertCtrl (ST_UINT32 cfgId, S_CERT_CTRL **CertCtrlOut);
#define sSecCertCfgIdToRemCertCtrl(cfgId,certCtrlOut)	 sSecCertCfgIdToCertCtrl(cfgId,certCtrlOut)
#define sSecCertCfgIdToLocCertCtrl(cfgId,certCtrlOut)	 sSecCertCfgIdToCertCtrl(cfgId,certCtrlOut)

/************************************************************************/
/************************************************************************/
/************************************************************************/
/**** Certificate Blob Handling ****/

/* Retrieve local Certificate Blob from Certifcate Store */
ST_RET sSecGetLocalCert (S_CERT_ID *certId, S_CERT_BLOB *certDataOut);

/* Decoding a Certificate blob */
ST_RET sSecCertDec (S_CERT_BLOB *certBlob, ST_UINT certType, S_CERT_INFO **certInfoOut);

/************************************************************************/
/**** Signed Time Handling ****/

/* Verify a Signed Time	signature				*/
ST_RET sSecSignedTimeVerify (S_CERT_INFO *remCert, S_SIGNED_TIME *signedTime, ST_UCHAR *clearTime);

/* Sign a time_t 						*/
ST_RET sSecSignTimeT (S_CERT_ID *localCert, S_SIGNED_TIME *signedTime, time_t timeToSign);

/************************************************************************/
/**** MACE ASN.1 Encode/Decode ****/

/* Encode MACE ACSE Authentication */
ST_RET sSecMaceAsn1Enc (S_MACE_INFO *maceInfo, 
                        ST_CHAR *asn1Buf, ST_INT asn1BufLen,
                        ST_CHAR **maceAsn1Out, ST_INT *maceAsn1LenOut);

/* Decode Certificate based ACSE Authentication */
ST_RET sSecMaceAsn1Dec (ST_CHAR *maceAsn1, ST_INT maceAsn1Len, 
					    S_MACE_INFO **maceInfoOut);

/************************************************************************/
/**** Remote Certificate Validity Checking ****/

/* Make sure the certificate issuer is OK */
ST_RET sSecChkIssuer (S_CERT_INFO *certInfo);

/* Make sure the certificate validity times are OK */
ST_RET sSecChkValidityTimes (S_CERT_INFO *certInfo);

/* Validate a MACE time seal */
ST_RET sSecChkTimeSeal (S_CERT_INFO *remCert, 
			time_t signedTimeT,
			ST_UCHAR *clearTime,
			S_SIGNED_TIME *signedTime);


/************************************************************************/
/************************************************************************/
/************************************************************************/
/* Common functions declarations					*/

/**** Compare remote cert issuer names with ones that we trust ****/
ST_BOOLEAN sSecCmpIssuer (S_CERT_ID *remCert, S_CERT_ID *trustedCert);
/**** Compare two CertIds ****/
ST_BOOLEAN sSecCmpCertId (S_CERT_ID *certId1, S_CERT_ID *certId2);

/* XML header for temporary file where certificates are writen		*/
#define S_SEC_XML_HEADER_STR	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"

#if !defined(_WIN32)
ST_RET ssleSetCertStorePath (ST_CHAR *pathBuf);
/* Export a certificate to XML */
ST_INT ssleCertFileToXml (ST_CHAR *fileName, ST_CHAR *buf, ST_INT bufLen, ST_CHAR *pass, ST_VOID (*secLog)(S_SEC_EVENT *evt));
/* Export a certificate to Text */
ST_INT ssleCertFileToText (ST_CHAR *fileName, ST_CHAR *buf, ST_INT bufLen, ST_CHAR *pass, ST_VOID (*secLog)(S_SEC_EVENT *evt));
#endif /* !defined(_WIN32) */

ST_UINT ssleSecuritySupported (ST_VOID);

ST_RET  ssleLoadCRLists(ST_VOID *sslCtx, ST_BOOLEAN *newCrlOut);

/************************************************************************/
/************************************************************************/
/************************************************************************/
/* secMan Declarations							*/
/************************************************************************/

/* Start/Stop */
ST_RET secManStart (S_SEC_CONFIG **secCfg);
ST_RET secManEnd (ST_VOID);

/* Start the SSL Engine in ssec.lib (genssl.c) */
ST_RET startSSLEngine(S_SEC_CONFIG *sSecCfg,
	ST_VOID (*secLog)(S_SEC_EVENT *evt));	/* log funct ptr	*/
/* Stop the SSL Engine in ssec.lib (genssl.c) */
ST_RET stopSSLEngine(ST_VOID);
/* Called by SNAP-Lite to reload the security configuration (genssl.c) */
ST_RET sslUpdate (ST_BOOLEAN bUpdateAll);
/* Called by SNAP-Lite to reload the CRLs (genssl.c) */
ST_RET updateCRLists (ST_VOID);

/* Used by SNAP and user apps to get the configuration. 
 * The release function MUST be called when the app is done using the 
 * configuration.
 */
ST_RET secManAccessCfg (S_SEC_CONFIG **secCfgOut);
ST_RET secManReleaseCfg (ST_VOID);

/* Dynamic Configuration functions */
ST_BOOLEAN secManChkNewCfgAvail(ST_VOID);
ST_RET secManLoadCfg(S_SEC_CONFIG *secCfgOut);
ST_RET secManUpdateCfg(S_SEC_CONFIG *currCfg, S_SEC_CONFIG *newCfg);
ST_RET secManFreeCfg(S_SEC_CONFIG *secCfg);
ST_RET secManUpdateCRLists (ST_VOID *sslCtx, ST_INT crlCheckTime, ST_BOOLEAN *newCrl);
ST_RET secManCheckCertExpiration (S_CERT_ID *certId, ST_INT certType,
                                  ST_UINT32  certExpirationAlarmLimit,
                                  ST_UINT32 *certExpirationHoursLeftOut);
ST_RET secManCheckAllCertsExpiration (ST_VOID);

ST_RET secManGetCipherInUseStr (ST_INT cipherSuiteIdx, ST_INT *cipherSuiteOut,
                                ST_CHAR *cipherSuiteStrOut, ST_INT cipherSuiteStrMaxLen);

#if defined (_WIN32)
  /* DLL func to signal change in configuration file */
  __declspec(dllimport) ST_VOID WINAPI secManCfgChange(ST_VOID);
#else
  ST_VOID secManCfgChange(ST_VOID);
#endif

#if !defined(_WIN32)
/* --- For UNIX & LINUX define default path to Certificate Stores --- */
/* --- under the SISCO_PATH_INSTALL/SISCO_PATH_SECURITY directory --- */
#define CERT_STORE_CA 	  "certstore/ca"
#define CERT_STORE_LOCAL  "certstore/local"
#define CERT_STORE_REMOTE "certstore/remote"

/* If needed these paths can be changed by app before sSecStart() is called	*/
extern ST_CHAR     CACertStorePath[S_MAX_PATH];	/* defaults to CERT_STORE_CA	*/
extern ST_CHAR  LocalCertStorePath[S_MAX_PATH];	/* defaults to CERT_STORE_LOCAL	*/
extern ST_CHAR RemoteCertStorePath[S_MAX_PATH];	/* defaults to CERT_STORE_REMOTE*/

/* Exported function from libsecpass shared library (provided by the user) */
ST_RET secPassGetPassword(S_CERT_ID *certId, ST_CHAR *certPass, ST_INT certPassLen);
#endif /* !defined(_WIN32) */

#if defined(_WIN32)
/* name for SISCO's Security events folder in Windows Event Viewer 	*/
#define S_SEC_EVENT_REG_PATH	"SYSTEM\\CurrentControlSet\\Services\\EventLog\\"
#define S_SEC_EVENT_LOG_NAME	"SISCO Security Log"
#else
/* UNIX and Linux, default name for SISCO Security log, can be configured	*/
/* in siscostackcfg.xml for SNAP-Lite and for every application			*/
#define S_SEC_EVENT_LOG_NAME	"sisco_security.log"
#endif
 
/* Logging to Windows Event Viewer via SSLE Engine (or to a dedicated Security Events file on UNIX, Linux) */
ST_VOID secManEventLog (S_SEC_EVENT *secEvent, S_CERT_ID *certId);
ST_VOID secManEventLogEx (ST_CHAR *errDesc, ST_INT eventType, S_CERT_ID *certId);

/* Log configuration to diagnostic log */
ST_VOID secManLogCfg (ST_VOID);

/* Clone the log file */
ST_VOID secManCloneLog (ST_INT logType);

/************************************************************************/
/************************************************************************/
/* Log Macros								*/

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST _sec_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sec_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sec_flow_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sec_data_logstr;
extern SD_CONST ST_CHAR *SD_CONST _sec_debug_logstr;

#define SECLOG_ERR0(a) \
	SLOG_0 (sec_debug_sel & SEC_LOG_ERR, _sec_err_logstr, a)
#define SECLOG_ERR1(a,b) \
	SLOG_1 (sec_debug_sel & SEC_LOG_ERR, _sec_err_logstr, a,b)
#define SECLOG_ERR2(a,b,c) \
	SLOG_2 (sec_debug_sel & SEC_LOG_ERR, _sec_err_logstr, a,b,c)
#define SECLOG_ERR3(a,b,c,d) \
	SLOG_3 (sec_debug_sel & SEC_LOG_ERR, _sec_err_logstr, a,b,c,d)

#define SECLOG_CERR0(a) \
	SLOGC_0 (sec_debug_sel & SEC_LOG_ERR,a)
#define SECLOG_CERR1(a,b) \
	SLOGC_1 (sec_debug_sel & SEC_LOG_ERR,a,b)
#define SECLOG_CERR2(a,b,c) \
	SLOGC_2 (sec_debug_sel & SEC_LOG_ERR,a,b,c)

#define SECLOG_NERR0(a) \
	SLOG_0 (sec_debug_sel & SEC_LOG_NERR, _sec_nerr_logstr, a)
#define SECLOG_NERR1(a,b) \
	SLOG_1 (sec_debug_sel & SEC_LOG_NERR, _sec_nerr_logstr, a,b)
#define SECLOG_NERR2(a,b,c) \
	SLOG_2 (sec_debug_sel & SEC_LOG_NERR, _sec_nerr_logstr, a,b,c)
#define SECLOG_NERR3(a,b,c,d) \
	SLOG_3 (sec_debug_sel & SEC_LOG_NERR, _sec_nerr_logstr, a,b,c,d)
#define SECLOG_NERR4(a,b,c,d,e) \
	SLOG_4 (sec_debug_sel & SEC_LOG_NERR, _sec_err_logstr, a,b,c,d,e)

#define SECLOG_FLOW0(a) \
	SLOG_0 (sec_debug_sel & SEC_LOG_FLOW, _sec_flow_logstr, a)
#define SECLOG_FLOW1(a,b) \
	SLOG_1 (sec_debug_sel & SEC_LOG_FLOW, _sec_flow_logstr, a,b)
#define SECLOG_FLOW2(a,b,c) \
	SLOG_2 (sec_debug_sel & SEC_LOG_FLOW, _sec_flow_logstr, a,b,c)
#define SECLOG_FLOW3(a,b,c,d) \
	SLOG_3 (sec_debug_sel & SEC_LOG_FLOW, _sec_flow_logstr, a,b,c,d)

#define SECLOG_CFLOW0(a) \
	SLOGC_0 (sec_debug_sel & SEC_LOG_FLOW,a)
#define SECLOG_CFLOW1(a,b) \
	SLOGC_1 (sec_debug_sel & SEC_LOG_FLOW,a,b)
#define SECLOG_CFLOW2(a,b,c) \
	SLOGC_2 (sec_debug_sel & SEC_LOG_FLOW,a,b,c)

#define SECLOG_DEBUG0(a) \
	SLOG_0 (sec_debug_sel & SEC_LOG_DEBUG, _sec_debug_logstr, a)
#define SECLOG_DEBUG1(a,b) \
	SLOG_1 (sec_debug_sel & SEC_LOG_DEBUG, _sec_debug_logstr, a,b)
#define SECLOG_DEBUG2(a,b,c) \
	SLOG_2 (sec_debug_sel & SEC_LOG_DEBUG, _sec_debug_logstr, a,b,c)
#define SECLOG_DEBUG3(a,b,c,d) \
	SLOG_3 (sec_debug_sel & SEC_LOG_DEBUG, _sec_debug_logstr, a,b,c,d)

#define SECLOG_CDEBUG0(a) \
	SLOGC_0 (sec_debug_sel & SEC_LOG_DEBUG,a)
#define SECLOG_CDEBUG1(a,b) \
	SLOGC_1 (sec_debug_sel & SEC_LOG_DEBUG,a,b)

/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* S_SEC_INCLUDED */
/************************************************************************/
