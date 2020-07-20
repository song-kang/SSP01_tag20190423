/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2003-2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : seccfg_int.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			secCfgIntToStr					*/
/*			secCfgStrToInt					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    01     Created.					*/
/*			   Moved simple stuff from seccfg_comn.c to here*/
/*                         Stack cfg with !defined(S_SEC_ENABLED) needs	*/
/*			   only this.					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "str_util.h"

#include "ssec.h"
#include "ssec_int.h"

	/* =============================================================*/
	/* 	Define strings/value & tables for lookup functions	*/
	/* =============================================================*/


SECCFG_INT_TO_STRING cNameTypeStrings[] =
  {
    {S_CERT_NAME_TYPE_ANY,                 S_CERT_CNAME_TYPE_STR_ANY},
    {S_CERT_NAME_TYPE_COMMON_NAME,         S_CERT_CNAME_TYPE_STR_COMMON_NAME},
    {S_CERT_NAME_TYPE_COUNTRY,             S_CERT_CNAME_TYPE_STR_COUNTRY_NAME},
    {S_CERT_NAME_TYPE_STATE_OR_PROVINCE,   S_CERT_CNAME_TYPE_STR_STATE_OR_PROVINCE_NAME},
    {S_CERT_NAME_TYPE_LOCALITY,            S_CERT_CNAME_TYPE_STR_LOCALITY},
    {S_CERT_NAME_TYPE_ORGANIZATION,        S_CERT_CNAME_TYPE_STR_ORGANIZATION},
    {S_CERT_NAME_TYPE_EMAIL, 		   S_CERT_CNAME_TYPE_STR_EMAIL_ADDRESS},
    {S_CERT_NAME_TYPE_ORGANIZATIONAL_UNIT, S_CERT_CNAME_TYPE_STR_ORGANIZATION_UNIT_NAME},
    {S_CERT_NAME_TYPE_ANY,                 "Unknown"}, /* For backward compatibility */
    {-1,  			           NULL}
  };

SECCFG_INT_TO_STRING glbAuthTypeStrings[] =
  {
    {S_APP_AUTHREQ_AR_SEC,               "AR Security"},
    {S_APP_AUTHREQ_MACE,                 "MACE"},
    {S_APP_AUTHREQ_MACE_ENCRYPTED,       "MACE Encrypted"},
    {S_APP_AUTHREQ_ENCRYPTED,            "Encrypted"},
    {S_APP_AUTHREQ_NONE,                 "None"},     
    {S_APP_AUTHREQ_NONE,                 "Any"},      /* For backward compatibility */
    {-1,  			          NULL}
  };

SECCFG_INT_TO_STRING arAuthModeTypeStrings[] =
  {
    {S_AR_AUTH_MODE_MACE,                 "MACE"},
    {S_AR_AUTH_MODE_SSL,                  "SSL"},
    {S_AR_AUTH_MODE_AETITLE,              "AE Title"}, 
    {S_AR_AUTH_MODE_AETITLE,              "None"}, /* For backward compatibility */
    {-1,  			          NULL}
  };

SECCFG_INT_TO_STRING arEncryptionTypeStrings[] =
  {
    {S_SEC_ENCRYPT_SSL,                  "SSL"},
    {S_SEC_ENCRYPT_NONE,                 "None"},
    {-1,  			          NULL}
  };

SECCFG_INT_TO_STRING keyexTypeStrings[] =
  {
    {S_KEYX_NONE,		"NONE"},	
    {S_KEYX_NULL,		"NULL"},
    {S_KEYX_RSA,		"RSA"},
    {S_KEYX_RSA_EX,		"RSA_EX"},
    {S_KEYX_DH_DSS,		"DH_DSS"},
    {S_KEYX_DH_DSS_EX,		"DH_DSS_EX"},
    {S_KEYX_DH_RSA,		"DH_RSA"},
    {S_KEYX_DH_RSA_EX,		"DH_RSA_EX"},
    {S_KEYX_DHE_DSS,		"DHE_DSS"},
    {S_KEYX_DHE_DSS_EX,		"DHE_DSS_EX"},
    {S_KEYX_DHE_RSA,		"DHE_RSA"},
    {S_KEYX_DHE_RSA_EX,		"DHE_RSA_EX"},
    {S_KEYX_DH_ANON,		"DH_ANON"},
    {S_KEYX_DH_ANON_EX,		"DH_ANON_EX"},
    {S_KEYX_FORTEZZA_DMS,	"FORTEZZA_DMS"},
    {-1,  	        	NULL}
  };

SECCFG_INT_TO_STRING cryptTypeStrings[] =
  {
    {S_CRYPT_NONE,		"NONE"},
    {S_CRYPT_NULL,		"NULL"},
    {S_CRYPT_RC4_40,		"RC4_40"},
    {S_CRYPT_RC4_56,		"RC4_56"},
    {S_CRYPT_RC4_128,		"RC4_128"},
    {S_CRYPT_RC2_CBC_40,	"RC2_CBC_40"},
    {S_CRYPT_IDEA_CBC,		"IDEA_CBC"},
    {S_CRYPT_DES40_CBC,		"DES40_CBC"},
    {S_CRYPT_DES_CBC,		"DES_CBC"},
    {S_CRYPT_3DES_EDE_CBC,	"3DES_EDE_CBC"},
    {S_CRYPT_FORTEZZA_CBC,	"FORTEZZA_CBC"},
    {S_CRYPT_AES_128_CBC,	"AES_128_CBC"},
    {S_CRYPT_AES_256_CBC,	"AES_256_CBC"},
    {-1,  	      		NULL}
  };

SECCFG_INT_TO_STRING hashTypeStrings[] =
  {
    {S_HASH_NONE,	"NONE"},
    {S_HASH_NULL,	"NULL"},
    {S_HASH_MD5,	"MD5"},
    {S_HASH_SHA,	"SHA"},
    {-1,  	      	NULL}
  };


/************************************************************************/
/*			secCfgIntToStr					*/
/************************************************************************/
ST_CHAR *secCfgIntToStr (SECCFG_INT_TO_STRING *strMap, ST_INT val)
{
ST_INT i;

  for (i = 0;; ++i, ++strMap)
    {
    if (strMap->str == NULL)
      break;
    if (strMap->val == val)
      return (strMap->str);
    }
  return ("Invalid");
}

/************************************************************************/
/*			secCfgStrToInt					*/
/************************************************************************/
ST_RET secCfgStrToInt (SECCFG_INT_TO_STRING *strMap, ST_CHAR *str, ST_INT *valOut)
{
ST_RET rtn = SD_FAILURE;
ST_INT i;

  for (i = 0;; ++i, ++strMap)
    {
    if (strMap->str == NULL)
      break;

    if (stricmp (strMap->str, str) == 0)
      {
      *valOut = strMap->val;
      rtn = SD_SUCCESS;
      break;
      }
    }
  return (rtn);
}


