/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      	 2003 - 2009, All Rights Reserved                       */
/*                                                                      */
/* MODULE NAME : ssec_int.h	                                        */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*	This module contains internal security functions, defines,...	*/
/*	that are not exposed to user.					*/
/*      Extracted common secCfg* internal functions (code) for old and 	*/
/*	new (non-Marben) implementation.				*/
/*	It also contains function that are passing GEN_SOCK parameter.	*/
/*	Having these protos in ssec.h caused compile conflicts in	*/
/*	project using old gensock.h					*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev     Comments                                     */
/* --------  ---  ------   -------------------------------------------  */
/* 04/24/09  EJV    04     sslGetCipherSuite: added params, chg return.	*/
/*			   Rem sSecGetCipherSuite, use sslGetCipherSuite*/
/* 02/06/09  EJV    03     Chg secLogSSLMaceParams to secLogSSLParams.	*/
/*			   Added secLogCertParams proto.		*/
/*			   Mv SECCFG_INT_TO_STRING here and fun protos.	*/
/* 11/06/07  EJV    02     Chg sSecGetCertId param to ST_UINT32.	*/
/*			   Added few prototypes.			*/
/* 08/25/03  EJV    01     Moved protos with GEN_SOCK from ssec.h	*/
/************************************************************************/

#ifndef SSEC_INT_INCLUDED
#define SSEC_INT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "gensock2.h"
#include "dibmatch.h"


/* internal functions for secure socket interface */
ST_RET setGenSockSSL (GEN_SOCK_CONFIG *sockCfg);
ST_RET sSecGetCertId (GEN_SOCK *pSock, ST_UINT32 *cfgIdOut);
ST_RET sSecGetCertCtrl (GEN_SOCK *pSock, S_CERT_CTRL **certCtrlOut);

ST_RET       sslGetCipherSuite (GEN_SOCK *pSock, ST_INT *cipherSuiteOut, ST_INT *cipherSuiteIdxOut);
S_CERT_INFO *sslGetPeerCert    (GEN_SOCK *pSock);
ST_VOID      sslFreePeerCert   (S_CERT_INFO *remCert);


/* lookup cfg strings */
typedef struct
  {
  ST_INT val;
  ST_CHAR *str;
  } SECCFG_INT_TO_STRING;

extern SECCFG_INT_TO_STRING	cNameTypeStrings [];
extern SECCFG_INT_TO_STRING	glbAuthTypeStrings[];
extern SECCFG_INT_TO_STRING	arAuthModeTypeStrings[];
extern SECCFG_INT_TO_STRING	arEncryptionTypeStrings[];
extern SECCFG_INT_TO_STRING	keyexTypeStrings[];
extern SECCFG_INT_TO_STRING	cryptTypeStrings[];
extern SECCFG_INT_TO_STRING	hashTypeStrings[];


/* funcs in seccfg_comn.c							*/
/* extracted common secCfg* funcs for old and new (non-Marben) implementation.	*/
ST_CHAR     *secCfgIntToStr (SECCFG_INT_TO_STRING *strMap, ST_INT val);
ST_RET       secCfgStrToInt (SECCFG_INT_TO_STRING *strMap, ST_CHAR *str, ST_INT *valOut);

ST_UINT32    secCfgComputeCertCrc32 (S_CERT_CTRL *certCtrl);
S_CERT_CTRL *secCfgFindCertCtrl     (S_CERT_CTRL *certList, ST_CHAR *certName);
S_CERT_CTRL *secCfgFindCertCfgId    (S_CERT_CTRL *certList, ST_UINT32 cfgId);
ST_RET       secCfgParseSerialNumberStr (ST_CHAR *serialNumStr, S_CERT_ID *certId);

/**** Create/Free CNames ****/
ST_RET       secCfgAddToCName       (S_CERT_NAME **cNameHead, ST_INT nameType, ST_CHAR *nameText);
ST_RET       secCfgFreeCName        (S_CERT_NAME **cNameHead); 
ST_RET       secCfgFreeCertCtrl     (S_CERT_CTRL **certList);

/* cfg logging */ 
ST_VOID      secLogCertParams     (S_SEC_CONFIG *secCfg);
ST_VOID      secLogCertCtrl       (S_CERT_CTRL *certCtrl, ST_BOOLEAN caCert);
ST_VOID      secLogSSLParams      (S_SEC_CONFIG *secCfg);
ST_VOID      secLogDibMatchParams (DIB_MATCH_CTRL *dibMatch);



/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* SSEC_INT_INCLUDED */
/************************************************************************/
