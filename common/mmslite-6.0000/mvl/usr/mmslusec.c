/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	      2003 - 2011, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mmslusec.c						*/
/* PRODUCT(S)  : MMS-EASE Lite with LITESECURE				*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/* 		This file contains Security related user functions.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/18/11  EJV           ulCheckSecureAssocInd: fix fallback mode.	*/
/* 01/31/11  EJV           ulUpdateCertRevocationList:add ICCP_LITE code*/
/* 10/03/10  EJV           ulCheckSecureAssocInd:dibMatch now mandatory.*/
/* 07/12/10  EJV           Merged with mms_usec.c (enhanced security).	*/
/* 09/06/05  EJV     06    Rpl suicacse.h with lean_a.h.		*/
/* 08/02/05  EJV     05    Added suicacse.h (not in MMS H file anymore)	*/
/* 02/25/05  MDE     04    Now match remote AE Title on confirm		*/
/* 01/20/04  EJV     03    ulFreeAssocSecurity: del cc param; reworked.	*/
/* 01/07/04  EJV     02    Merged with  MMS-EASE mms_usec.c.		*/
/* 07/08/03  EJV     01    Adopted from MMS-EASE mms_usec.c.		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "mvl_defs.h"
#include "mvl_acse.h"
#include "mmslusec.h"
#include "lean_a.h"
#include "acse2.h"
#include "sstackcfg.h"			/* for SISCO Stack Configuration*/

#ifdef ICCP_LITE
#include "mi.h"
#endif

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#if defined(S_SEC_ENABLED)

/************************************************************************/
/*			ulInitSecurity					*/
/*----------------------------------------------------------------------*/
/* This function initializes the security subsystem.			*/
/* It is to be be called just before strt_MMS is called.		*/
/* Parameters:								*/
/*    none								*/
/* Return:								*/
/*    SD_SUCCESS		if security initialization successful	*/
/*    SD_FAILURE		otherwise				*/
/************************************************************************/

ST_RET ulInitSecurity (ST_VOID)
  {
ST_INT        ret;
S_SEC_CONFIG *pSecCfg;			/* pointer to security cfg	*/

  ret = sSecUsrStart(&pSecCfg);
  if (ret != SD_SUCCESS)
    {
    SECLOG_ERR1 ("Could not start the security subsystem (ret=%d)", ret);
    return (SD_FAILURE);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			ulEndSecurity					*/
/*----------------------------------------------------------------------*/
/* This function cleans up the security subsystem.			*/
/* It is to be be called after end_MMS is called.			*/
/* Parameters:								*/
/*    none								*/
/* Return:								*/
/*    SD_SUCCESS		if security cleanup successful		*/
/*    SD_FAILURE		otherwise				*/
/************************************************************************/

ST_RET ulEndSecurity (ST_VOID)
  {
ST_INT        ret;

  ret = sSecUsrEnd();
  if (ret != SD_SUCCESS)
    {
    SECLOG_ERR1 ("Could not cleanup the security subsystem (ret=%d)", ret);
    return (SD_FAILURE);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			ulSetSecurityCalling				*/
/*----------------------------------------------------------------------*/
/* This function prepares the calling connection security info based on	*/
/* information in the Security Configuration (secManCfg.xml).		*/
/* It is to be be called just before the initiate request is called.	*/
/* Parameters:								*/
/*  ST_CHAR    *locArName	ptr to local AR Name for the connection	*/
/*  ST_CHAR    *remArName	ptr to remote AR Name for the connection*/
/*  S_SEC_LOC_AR **locArSecOut	addr where local AR Security ptr is ret	*/
/*  S_SEC_REM_AR **remArSecOut	addr where remote AR Security ptr is ret*/
/*  ACSE_AUTH_INFO     *locAuthInfo	ptr to Authenication Info to set*/
/*  S_CERT_CTRL       **maceCertOut,   local MACE cert used in this con	*/
/*  S_SEC_ENCRYPT_CTRL *encryptCtrl    ptr to Encryption Ctrl to set	*/
/* Returns:								*/
/*    SD_SUCCESS		if setting security info for calling	*/
/*				connection successful,			*/
/*    S_SEC_CHECK_ERR_*    	otherwise.				*/
/************************************************************************/
ST_RET ulSetSecurityCalling (ST_CHAR            *locArName,
			     ST_CHAR            *remArName,
			     S_SEC_LOC_REM_AR  **locArSecOut,
			     S_SEC_LOC_REM_AR  **remArSecOut,
                             ACSE_AUTH_INFO     *locAuthInfo,
                             S_CERT_CTRL       **maceCertOut,
                             S_SEC_ENCRYPT_CTRL *encryptCtrl)
{
ST_RET             ret;
S_SEC_CONFIG      *pSecCfg;			/* pointer to security cfg	*/
S_SEC_LOC_REM_AR  *locArSec = NULL;		/* Local AR Security  		*/
S_SEC_LOC_REM_AR  *remArSec = NULL;		/* Remote AR Security 		*/
S_CERT_CTRL       *maceCert = NULL;
ST_CHAR            errDesc [MAX_LOG_SIZE];

  /* access the security configuration */
  if (secManAccessCfg (&pSecCfg) != SD_SUCCESS)
    return (SD_FAILURE);

  /* init to no authentication and no encryption */
  memset (locAuthInfo, 0, sizeof (ACSE_AUTH_INFO));
  locAuthInfo->auth_pres = SD_FALSE;
  memset (encryptCtrl, 0, sizeof (S_SEC_ENCRYPT_CTRL));
  encryptCtrl->encryptMode = S_SEC_ENCRYPT_NONE;

  if (pSecCfg->secureModeEnabled)
    {
    /* Retrieve the AR security configuration for local AR Name 	*/
    ret = sSecArNameToArSec (locArName, &locArSec);
    if (ret == SD_SUCCESS)
      {
      /* Retrieve the AR security configuration for remote AR Name 	*/
      ret = sSecArNameToArSec (remArName, &remArSec);
      if (ret == SD_SUCCESS)
        {
        /* See if we are to use MACE with this remote node */
        if (remArSec->arAuthMode == S_AR_AUTH_MODE_MACE)
          {
          /* Create MACE Authentication information	*/
          /* Note that the MACE ASN.1 Buffer will be free'd by MVL as appropriate	*/
          ret = sSecMaceAuthInfoCreate (locArSec, locAuthInfo, &maceCert);
          if (ret != SD_SUCCESS)
            {
	    /* ret = S_SEC_CHECK_ERR_* */
            SECLOG_NERR2 ("Could not create MACE Authentication Information (ret=%d) for Local AR Name='%s' ",
                          ret, locArName);
            }
          }
        /* else	not using MACE */

        /* Now select encryption for this connection				*/
        encryptCtrl->encryptMode = remArSec->encryptMode;
        encryptCtrl->u.ssl.port = remArSec->sslPort;
        encryptCtrl->u.ssl.cipherSuite = S_CIPHER_ANY;
        }
      else
        {
        SECLOG_NERR1 ("Remote AR Security for AR Name='%s' not found", remArName);
        ret = S_SEC_CHECK_ERR_REM_AR_SEC_NOT_CONFIG;  /* don't have better error code */
        }
      }
    else
      {
      SECLOG_NERR1 ("Local AR Security for AR Name='%s' not found", locArName);
      ret = S_SEC_CHECK_ERR_LOC_AR_SEC_NOT_CONFIG;  /* don't have better error code */
      }

    if (ret != SD_SUCCESS)
      {
      /* problem setting security for this connection, check if fallback to	*/
      /* non-secure mode enabled 						*/
      sprintf (errDesc, "Failed to set security info for connection to '%s', \n"
                        "  %s.",
                        remArName, S_SEC_CHECK_ERR_CODE_STR(ret));
      if (pSecCfg->nonSecureFallbackEnabled)
        {
        /* try a non-secure connection */
        locAuthInfo->auth_pres = SD_FALSE;
        encryptCtrl->encryptMode = S_SEC_ENCRYPT_NONE;
        strcat (errDesc, "\n  Connection allowed because Fallback to non-Secure mode is enabled.");
        secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_WARNING, NULL);
	ret = SD_SUCCESS; /* overwrite the error ret */
        }
      else
        {
	/* log error and return */
        secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_ERROR, NULL);
        secManReleaseCfg ();
	return (ret);
	}
      }

    *locArSecOut = locArSec;
    *remArSecOut = remArSec;
    *maceCertOut = maceCert;
    }
  else
    {
    /* non-secured connection */
    *locArSecOut = NULL;
    *remArSecOut = NULL;
    *maceCertOut = NULL;
    }

  secManReleaseCfg ();
  return (SD_SUCCESS);
}

/************************************************************************/
/*			ulCheckSecureAssocConf				*/
/*----------------------------------------------------------------------*/
/* This function is to be called when a successful associate confirm is	*/
/* received. It verifies that the appropriate security settings are in	*/
/* effect.								*/
/* It should be called from 'u_mvl_connect_cnf_ex', and the connection 	*/
/* should only be accepted for a return value of ACSE_AUTH_SUCCESS	*/
/* Parameters:								*/
/*   MVL_NET_INFO   *cc		ptr to connection info			*/
/* S_SEC_AUTHCHK_RSLT *secChk	ptr where to return security info	*/
/* Return:								*/
/*   ACSE_AUTH_SUCCESS		if security check successful		*/
/*   ACSE_DIAG_...		otherwise diagnostics code as defined	*/
/*				in acseauth.h				*/
/************************************************************************/
ST_RET ulCheckSecureAssocConf (MVL_NET_INFO *cc, S_SEC_AUTHCHK_RSLT *secChk)
{
ST_RET              ret;
S_SEC_CONFIG       *pSecCfg;		/* pointer to security configuration	*/
ST_CHAR             errDesc [MAX_LOG_SIZE];

  /* access the security configuration */
  if (secManAccessCfg (&pSecCfg) != SD_SUCCESS)
    return (ACSE_DIAG_NO_REASON);

  memset (secChk, 0, sizeof (S_SEC_AUTHCHK_RSLT));

  if (pSecCfg->secureModeEnabled)
    {
    /* Check to make sure appropriate security settings are in place */

    ret = sSecAssocConfChkEx (cc, NULL, cc->rem_ar_sec, secChk);	/* matchCtrl not used */
    if (ret != SD_SUCCESS)
      {
      S_CERT_ID *certId = (secChk->u.certBased.maceCert ? &secChk->u.certBased.maceCert->certId : NULL);

      sprintf (errDesc, "Associate-Cnf ERROR: security checks failed, remote AR Name='%s'.\n"
                        "  %s.",
                        cc->rem_ar_sec->arName, S_SEC_CHECK_ERR_CODE_STR(ret));
      if (pSecCfg->nonSecureFallbackEnabled)
        {
	/* some security error conditions can be acceptable if nonSecureFallbackEnabled */
        switch (ret)
          {
          case S_SEC_CHECK_ERR_NOT_ENCRYPTED:
          case S_SEC_CHECK_ERR_MACE_AUTH_NOT_PRESENT:
          case S_SEC_CHECK_ERR_MACE_CERT_TIME_SEAL:
          case S_SEC_CHECK_ERR_MACE_CERT_TIME_INVALID:
          case S_SEC_CHECK_ERR_MACE_CERT_NOT_CONFIG:
          case S_SEC_CHECK_ERR_MACE_CERT_NOT_CONFIG_IN_AR_SEC:
          case S_SEC_CHECK_ERR_SSL_CERT_NOT_CONFIG:
          case S_SEC_CHECK_ERR_SSL_CERT_NOT_CONFIG_IN_AR_SEC:
          case S_SEC_CHECK_ERR_CERT_EXPIRED:
          case S_SEC_CHECK_ERR_CERT_NOT_YET_VALID:
          case S_SEC_CHECK_ERR_CERT_REVOKED:
          case S_SEC_CHECK_ERR_CERT_VERIFY_FAILED:
            strcat (errDesc, "  Connection allowed because Fallback to non-Secure mode is enabled.");
            SECLOG_NERR1 ("%s", errDesc);	/* also used in secEvent    */
            /* output msg to SISCO Security Log */
	    if (certId)
              secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_WARNING_CERT, certId);
	    else
              secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_WARNING, NULL);
	    ret = SD_SUCCESS;  /* ovwerwite ret to allow connection */
          break;
          }
        }
      if (ret != SD_SUCCESS)
        {
        SECLOG_NERR1 ("%s", errDesc);	/* also used in secEvent    */
        /* output msg to SISCO Security Log */
	if (certId)
          secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_ERROR_CERT, certId);
	else
          secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_ERROR, NULL);
        secManReleaseCfg ();
        return (ACSE_DIAG_AUTH_REQUIRED);  	    /* Reject the connection Cnf	*/
        }
      }

    SECLOG_FLOW1 ("Associate-Cnf: Security checks passed for connection with remote AR Name='%s'", cc->rem_ar_sec->arName);
    }

  secManReleaseCfg ();
  return (ACSE_AUTH_SUCCESS);
}

/************************************************************************/
/*			ulCheckSecureAssocInd				*/
/*----------------------------------------------------------------------*/
/* This function checks if the received association indication can	*/
/* be accepted. 							*/
/* It should be called from 'u_mvl_connect_ind_ex', and the connection 	*/
/* should only be accepted for a return value of ACSE_AUTH_SUCCESS	*/
/*   MVL_NET_INFO    *cc	  ptr to connection info		*/
/*   ACSE_AUTH_INFO  *rspAuthInfo ptr to athorization info for response	*/
/*   S_SEC_AUTHCHK_RSLT *secChk   ptr where to return security info.	*/
/* Returns:								*/
/*  ACSE_AUTH_SUCCESS		if security check successful		*/
/*   ACSE_DIAG_...		otherwise diagnostics code as defined	*/
/*				in acseauth.h				*/
/************************************************************************/
ST_RET ulCheckSecureAssocInd (MVL_NET_INFO *cc, ACSE_AUTH_INFO *rspAuthInfo, 
                              S_SEC_AUTHCHK_RSLT *secChk)
{
ST_RET              ret;
S_SEC_CONFIG       *pSecCfg;		/* ptr to security configuration*/
S_SEC_LOC_REM_AR   *remArSec = NULL;	/* Local AR Security  		*/
S_SEC_LOC_REM_AR   *locArSec = NULL;	/* Remote AR Security 		*/
ST_CHAR             errDesc [MAX_LOG_SIZE];
S_CERT_CTRL        *maceCert = NULL;
ST_BOOLEAN          allowConn = SD_FALSE;

  /* access the security configuration */
  if (secManAccessCfg (&pSecCfg) != SD_SUCCESS)
    return (ACSE_DIAG_NO_REASON);

  memset (secChk, 0, sizeof (S_SEC_AUTHCHK_RSLT));
  rspAuthInfo->auth_pres = SD_FALSE;	/* assume no response authentication	*/

  if (pSecCfg->secureModeEnabled)
    {
    /* Find out who the remote node is based on configured Remote AR Security	*/

    ret = sSecAssocIndChk (cc, pSecCfg->dibMatch, &remArSec, secChk);
    if (ret != SD_SUCCESS)
      {
      S_CERT_ID *certId = (secChk->u.certBased.maceCert ? &secChk->u.certBased.maceCert->certId : NULL);

      sprintf (errDesc, "Associate-Ind ERROR: security checks failed, remote AR Name='%s'.\n"
                        "  %s.",
                        (remArSec ? remArSec->arName : " "), S_SEC_CHECK_ERR_CODE_STR(ret));
      if (pSecCfg->nonSecureFallbackEnabled)
        {
	/* some security error conditions can be acceptable if nonSecureFallbackEnabled */
        switch (ret)
          {
          case S_SEC_CHECK_ERR_NOT_ENCRYPTED:
          case S_SEC_CHECK_ERR_MACE_AUTH_NOT_PRESENT:
          case S_SEC_CHECK_ERR_MACE_CERT_TIME_SEAL:
          case S_SEC_CHECK_ERR_MACE_CERT_TIME_INVALID:
          case S_SEC_CHECK_ERR_MACE_CERT_NOT_CONFIG:
          case S_SEC_CHECK_ERR_MACE_CERT_NOT_CONFIG_IN_AR_SEC:
          case S_SEC_CHECK_ERR_SSL_CERT_NOT_CONFIG:
          case S_SEC_CHECK_ERR_SSL_CERT_NOT_CONFIG_IN_AR_SEC:
          case S_SEC_CHECK_ERR_CERT_EXPIRED:
          case S_SEC_CHECK_ERR_CERT_NOT_YET_VALID:
          case S_SEC_CHECK_ERR_CERT_REVOKED:
          case S_SEC_CHECK_ERR_CERT_VERIFY_FAILED:
            /* NOTE: remArSec != NULL for all these error codes */
            strcat (errDesc, "  Connection allowed because Fallback to non-Secure mode is enabled.");
            SECLOG_NERR1 ("%s", errDesc);	/* also used in secEvent    */
            /* output msg to SISCO Security Log */
	    if (certId)
              secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_WARNING_CERT, certId);
            else
              secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_WARNING, NULL);
	    ret = SD_SUCCESS;  /* ovwerwite ret to allow connection */
            allowConn = SD_TRUE;
          break;
	  /* all other return codes are considered errors	*/
          }
        }
      if (ret != SD_SUCCESS)
        {
        SECLOG_NERR1 ("%s", errDesc);	/* also used in secEvent    */
        /* output msg to SISCO Security Log */
        if (certId)
          secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_ERROR_CERT, certId);
        else
          secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_ERROR, NULL);
        secManReleaseCfg ();
        return (ACSE_DIAG_AUTH_REQUIRED);  	    /* Reject the connection Ind	*/
        }
      }
    cc->rem_ar_sec = remArSec;		/* save the remote AR security		*/
    SECLOG_FLOW1 ("Associate-Ind: received connection from remote AR Name='%s'.", remArSec->arName);

    /* We know who the remote is						*/
    /* If we are using MACE for this remote, get it ready to send		*/
    if (remArSec->arAuthMode == S_AR_AUTH_MODE_MACE)
      {
      /* if remote didn't sent us MACE (and nonSecureFallbackEnabled=SD_TRUE)	*/
      /* then we probably should not send MACE in Associate-Cnf for the		*/
      /* connection to succeed							*/
      if (pSecCfg->nonSecureFallbackEnabled && allowConn)
        {
        /* since we did not received MACE certificate from remote do not send	*/
        /* our MACE either							*/
        rspAuthInfo->auth_pres = SD_FALSE;
	}
      else  /* this is the normal handling for Associate-Cnf */
        {
        /* Retrieve the AR security configuration for the Local AR Name		*/
        ret = sSecLocArNameToArSec (cc->bind_ctrl->ar_name, &locArSec);
        if (ret != SD_SUCCESS)
          {
          /* NOTE: we require the locArSec to be configured for every Ar Name 	*/
          SECLOG_NERR1 ("Associate-Ind ERROR: AR Security not configured for local AR Name='%s'.",
                        (cc->bind_ctrl ? cc->bind_ctrl->ar_name : "?"));
          secManReleaseCfg ();
          return (ACSE_DIAG_AUTH_FAILURE); 	    /* Reject the connection 	*/
          }
        cc->loc_ar_sec = locArSec;	/* save the local AR security		*/

        /* Now create the MACE auth_info structure for response.			*/
        ret = sSecMaceAuthInfoCreate (locArSec, rspAuthInfo, &maceCert);
        if (ret != SD_SUCCESS)
          {
          SECLOG_NERR2 ("Could not create MACE Authentication Information (ret=%d) for Local AR Name='%s' ", 
                        ret, (cc->bind_ctrl ? cc->bind_ctrl->ar_name : "?"));
          secManReleaseCfg ();
          return (ACSE_DIAG_AUTH_FAILURE);	/* Reject the connection 	*/
          }
        }
      }
    }
  else	/* Secure Mode not enabled */
    {
    DIB_ENTRY          *remDe;
    /* check if we can find calling node's AR Name in Stack's Configuration	*/
    ret = dib_match_remote_ar_ll (pSecCfg->dibMatch, &cc->ass_ind_info, &remDe);
    if (ret == SD_SUCCESS)
      {
      /* We know the remote AR Name, but in this sample do not take action 	*/
      /* based on that information.						*/
/* DEBUG TO-DO: can we set some field in cc ???*/
      SECLOG_FLOW1 ("Associate-Ind: received connection from remote AR Name='%s'.", remDe->name);
      }
    else
      {
      /* we like to know who connected to our system */
      SECLOG_NERR0 ("Associate-Ind: Remote AE Title does not match any Remote AR Names configured locally");
      secManReleaseCfg ();
      return (ACSE_DIAG_AUTH_REQUIRED);	/* Reject the connection 	*/
      }
    }

  secManReleaseCfg ();
  return (ACSE_AUTH_SUCCESS);		/* Accept the connection 	*/
}

/************************************************************************/
/*			ulFreeAssocSecurity				*/
/*----------------------------------------------------------------------*/
/* This function releases authentication info.				*/
/* Parameters:								*/
/*   ACSE_AUTH_INFO	*authInfo	ptr to authtentication info	*/
/* Returns:								*/
/*	SD_SUCCESS		if function successful			*/
/*	SD_FAILURE		otherwise.				*/
/************************************************************************/
ST_RET ulFreeAssocSecurity (ACSE_AUTH_INFO *authInfo)
{
ST_RET ret = SD_SUCCESS;

  /* Clean authentication information */
  if (authInfo && authInfo->auth_pres)
    {
    ret = sSecFreeSecParam (authInfo);
    if (ret != SD_SUCCESS)
      SECLOG_NERR1 ("Could not free Authentication Info (ret=%d)", ret);
    }

  return (ret);
}

/************************************************************************/
/*			ulCheckSecurityConfiguration			*/
/*----------------------------------------------------------------------*/
/* This function can be called to check to see if the security 		*/
/* configuration has changed, and to reload it if so.			*/
/* Parameters:								*/
/*    none								*/
/* Return:								*/
/*    SD_SUCCESS		if checked/reloaded security config	*/
/*    SD_FAILURE or error code 	otherwise				*/
/************************************************************************/

ST_RET ulCheckSecurityConfiguration (ST_VOID)
  {
ST_RET        ret;
S_SEC_CONFIG *pSecCfg;
S_SEC_CONFIG  secCfgNew;

/* need to check if secureModeEnabled is set before proceeding */
  ret = secManAccessCfg (&pSecCfg);
  if (ret != SD_SUCCESS)
    return (SD_FAILURE);
  secManReleaseCfg ();

  if (!pSecCfg->secureModeEnabled)
    return (SD_SUCCESS);

  /* Security Configuration has changed, load the new configutaion 	*/
  memset (&secCfgNew, 0, sizeof(S_SEC_CONFIG));
  ret = secManLoadCfg (&secCfgNew);
  if (ret != SD_SUCCESS)
    {
    SECLOG_NERR1 ("Could not load new security configuration (ret=%d)", ret);
    return (ret);
    }
  
  /* Access the current security configuration */
  ret = secManAccessCfg (&pSecCfg);
  if (ret != SD_SUCCESS)
    {
    secManFreeCfg (&secCfgNew);
    SECLOG_ERR1 ("Could not access security configuration (ret=%d)", ret);
    return (SD_FAILURE);
    }

  /* Update the new configuration, then free the new copy		*/
  ret = sSecUpdate (pSecCfg, &secCfgNew);
  if (ret != SD_SUCCESS)
    SECLOG_NERR1 ("Could not put new configuration into effect (ret=%d)", ret);

  secManFreeCfg (&secCfgNew);

  /* log updated (or partially updated) security parameters */
  if (sec_debug_sel & SEC_LOG_FLOW)
    secManLogCfg ();

  secManReleaseCfg ();
  return (ret);
  }


/************************************************************************/
/*			ulUpdateCertRevocationList			*/
/*----------------------------------------------------------------------*/
/* Periodically check for new Certificate Revocation Lists (CRL) and	*/
/* drop exising	MMS connections (if option configured) when local or 	*/
/* remote MACE certificate is revoked. 					*/
/* Note that SNAP-Lite will drop existing TLS/SSL connection if SSL	*/
/* certificate (local or remote) was revoked.				*/
/************************************************************************/
ST_RET ulUpdateCertRevocationList (ST_VOID)
{
ST_RET           ret = SD_SUCCESS;
MVL_NET_INFO    *cc;
ST_INT           i;
S_SEC_CONFIG    *pSecCfg = NULL;
ST_CHAR         *locArName = NULL;
ST_CHAR         *remArName = NULL;
ST_BOOLEAN       newCRL = SD_FALSE;
ST_CHAR          errDesc [MAX_LOG_SIZE];
ST_INT           certType;

  if ((ret = secManAccessCfg (&pSecCfg)) != SD_SUCCESS)
    return (SD_FAILURE);

  /* is checking enabled ? */
  if (pSecCfg->secureModeEnabled && pSecCfg->crlCheckTime > 0)	
    {
    /* reloading CRLs configured */
    ret = secManUpdateCRLists (NULL, pSecCfg->crlCheckTime, &newCRL);

    if (ret == SD_SUCCESS && newCRL == SD_TRUE && pSecCfg->crlDropExisting)
      {
      /* scan all mvl_..._conn_ctrl and abort connection if local or remote MACE*/
      /* certificates were revoked						*/

      /* CALLING... */
      cc = mvl_calling_conn_ctrl;
      for (i = 0; i < mvl_cfg_info->num_calling; ++i, ++cc)
        {
        if (cc->in_use && cc->conn_active)
          {
          remArName = cc->rem_ar_sec->arName;
          locArName = cc->loc_ar_sec->arName;

          /* verify local and remote MACE certificate used on this connection */
	  certType = S_CERT_TYPE_REMOTE;
          ret = sSecVerifyMaceCerts (cc, certType);
	  if (ret == SD_SUCCESS)
	    {
            certType = S_CERT_TYPE_LOCAL;
            ret = sSecVerifyMaceCerts (cc, certType);
            }

	  if (ret != SD_SUCCESS && ret == S_SEC_CHECK_ERR_CERT_REVOKED)
            {
            /* local or remote MACE certificate was revoked, abort connection 		*/
            /* Note: the certificate was logged to SISCO Security Log by func above	*/
            sprintf (errDesc, "Local AR Name='%s' is terminating connection with remote AR Name='%s'"
                              "  due to revoked %s MACE certificate.",
                                 (locArName ? locArName : " "), (remArName ? remArName : " "), 
                                 (certType == S_CERT_TYPE_LOCAL ? "local" : "remote"));
            SECLOG_NERR1 ("%s", errDesc);
            /* output Application Event msg to SISCO Security Log */
            secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_ERROR, NULL);

#ifdef ICCP_LITE
            {
            MI_CONN *mi_conn;
            MI_ASSOC_CTRL *mi_assoc;

            /* set to abort during next mi_service call */
            if (cc->mi_in_use == SD_TRUE)
              {
              mi_conn = cc->mi_conn;
              mi_assoc = mi_conn->mi_assoc;
              mi_assoc->state = MI_ASSOC_STATE_ABORT;
              }
            }
#else
            /* abort and clean application resources for this connection */
            mvl_abort_req (cc);
            if (u_mvl_disc_ind_fun)
              (*u_mvl_disc_ind_fun) (cc, MVL_ACSE_ABORT_IND);
#endif

            }
          }
        }  /* CALLING...*/

      /* CALLED...*/
      cc = mvl_called_conn_ctrl;
      for (i = 0; i < mvl_cfg_info->num_called; ++i, ++cc)
        {
        if (cc->in_use && cc->conn_active)
          {
          remArName = cc->rem_ar_sec->arName;
          locArName = cc->loc_ar_sec->arName;

          /* verify local and remote MACE certificate used on this connection */
	  certType = S_CERT_TYPE_REMOTE;
          ret = sSecVerifyMaceCerts (cc, certType);
	  if (ret == SD_SUCCESS)
	    {
            certType = S_CERT_TYPE_LOCAL;
            ret = sSecVerifyMaceCerts (cc, certType);
            }

	  if (ret != SD_SUCCESS && ret == S_SEC_CHECK_ERR_CERT_REVOKED)
            {
            /* local or remote MACE certificate was revoked, abort connection 		*/
            /* Note: the certificate was logged to SISCO Security Log by func above	*/
            sprintf (errDesc, "Local AR Name='%s' is terminating connection with remote AR Name='%s'"
                              "  due to revoked %s MACE certificate.",
                                 (locArName ? locArName : " "), (remArName ? remArName : " "), 
                                 (certType == S_CERT_TYPE_LOCAL ? "local" : "remote"));
            SECLOG_NERR1 ("%s", errDesc);
            /* output Application Event msg to SISCO Security Log */
            secManEventLogEx (errDesc, S_SEC_EVENT_APP_SEC_ERROR, NULL);

#ifdef ICCP_LITE
            {
            MI_CONN *mi_conn;
            MI_ASSOC_CTRL *mi_assoc;

            /* set to abort during next mi_service call */
            if (cc->mi_in_use == SD_TRUE)
              {
              mi_conn = cc->mi_conn;
              mi_assoc = mi_conn->mi_assoc;
              mi_assoc->state = MI_ASSOC_STATE_ABORT;
              }
            }
#else
            /* abort and clean application resources for this connection */
            mvl_abort_req (cc);
            if (u_mvl_disc_ind_fun)
              (*u_mvl_disc_ind_fun) (cc, MVL_ACSE_ABORT_IND);
#endif

            }
          }
        }  /* CALLED... */
      }  /* newCRL == SD_TRUE  */
    }

  secManReleaseCfg();
  return (SD_SUCCESS);
}


/************************************************************************/
/*			ulCheckCertExpiration				*/
/*----------------------------------------------------------------------*/
/* User's function to check local/remote MACE and/or SSL certificates	*/
/* for expiration on an active connection.				*/
/* Note that SNAP checks periodically ALL configured local, remote	*/
/* and CA certificates for expiration and logs Security Events if any	*/
/* of the certificates expires or is within expiration alarm limit.	*/
/* Parameters:								*/
/*   MVL_NET_INFO    	 *cc	  	ptr to connection info		*/
/*   ST_BOOLEAN           bCheckNow     Overwrite configured checking	*/
/*				        intervals and check now.	*/
/*   S_SEC_CERT_STATUS   *certStatus	ptr to struct where to return	*/
/*                                      certs status			*/
/* Return:								*/
/*   SD_SUCCESS   if certStatus was obtained for connection		*/
/*   SD_FAILURE or other error otherwise.				*/
/*		  These values maybe returned:				*/
/*		    if certStatus=NULL					*/
/*		    if connection state is not active			*/
/*		    if security not enabled				*/
/*		    if non-secure connection,				*/
/*		    if expiration checking not enabled and bCheckNow=0	*/
/*		    if checking interval not elapsed and bCheckNow=0	*/
/************************************************************************/
ST_RET ulCheckCertExpiration (MVL_NET_INFO *cc, ST_BOOLEAN  bCheckNow, 
                             S_SEC_CERT_STATUS *certStatus)
{
ST_RET           ret = SD_SUCCESS;
S_SEC_CONFIG    *pSecCfg = NULL;
ST_DOUBLE        checkInterval;
S_CERT_ID       *certId = NULL;
S_SEC_SPARAMS    secParams;

  if (!cc || !(cc->in_use && cc->conn_active))
    {
    SECLOG_NERR0 ("ulCheckCertExpiration failed (cc=NULL or connection not active).");
    return (SD_FAILURE);
    }
  if (!certStatus)
    {
    SECLOG_NERR1 ("ulCheckCertExpiration failed for local AR Name='%s' (certStatus=NULL).",
                  (cc->loc_ar_sec ? cc->loc_ar_sec->arName : "?"));
    return (SD_FAILURE);
    }

  if ((ret = secManAccessCfg (&pSecCfg)) != SD_SUCCESS)
    return (SD_FAILURE);

  /* don't check if secure mode not enabled */
  if (!pSecCfg->secureModeEnabled)
    {
    secManReleaseCfg();
    return (SD_FAILURE);
    } 
 
  ret = sSecGetSecParam (cc, &secParams);
  if (ret != SD_SUCCESS)
    {
    secManReleaseCfg();
    return (ret);
    }

  /* certificates expiration checking enabled ? */
  if (!bCheckNow)
    {
    if (pSecCfg->certExpirationEvalInterval <= 0)
      {
      secManReleaseCfg();
      return (SD_FAILURE);	/* wait longer */
      }
    checkInterval = (ST_DOUBLE) (pSecCfg->certExpirationEvalInterval * 3600 * 1000.0); /* change from hours to msecs */
    if (sGetMsTime() <  (*(secParams.certExpirationLastCheckTime)) + checkInterval)
      {
      secManReleaseCfg();
      return (SD_FAILURE);	/* wait longer */
      }
    }

  SECLOG_FLOW1 ("Obtaining security information for connection on local AR Name = '%s'.",
                (cc->loc_ar_sec ? cc->loc_ar_sec->arName : "?"));

  /* time to check for expiration */
  (*(secParams.certExpirationLastCheckTime)) = sGetMsTime();  /* set for next check time */

  /* check if MACE authentication was used */
  if (secParams.authInfo->auth_pres && secParams.authInfo->mech_type == ACSE_AUTH_MECH_MACE_CERT)
    {
    /* get expiration for local MACE certificate */
    certId = &secParams.maceCert->certId;

    certStatus->locMaceAuthInfoPresent    = SD_TRUE;
    certStatus->locMaceCertName           = certId->certName;
    certStatus->locMaceCertExpirationTime = certId->validity.validNotAfter;

    certStatus->locMaceCertStatus = secManCheckCertExpiration (certId, S_CERT_TYPE_LOCAL,
                                                               pSecCfg->certExpirationAlarmLimit,
                                                              &certStatus->locMaceCertExpirationHoursLeft);
    }

  /* get expiration for remote MACE certificate */
  if (secParams.partAuthInfo->auth_pres && secParams.partAuthInfo->mech_type == ACSE_AUTH_MECH_MACE_CERT)
    {
    certId = &secParams.partMaceCert->certId;

    certStatus->remMaceAuthInfoPresent    = SD_TRUE;
    certStatus->remMaceCertName           = certId->certName;
    certStatus->remMaceCertExpirationTime = certId->validity.validNotAfter;

    certStatus->remMaceCertStatus = secManCheckCertExpiration (certId, S_CERT_TYPE_REMOTE,
                                                               pSecCfg->certExpirationAlarmLimit,
                                                              &certStatus->remMaceCertExpirationHoursLeft);
    }

  /* check if SSL/TLS encryption was used */
  if (secParams.encryptCtrl->encryptMode == S_SEC_ENCRYPT_SSL)
    {
    /* get expiration for local SSL certificate */
    if (secParams.encryptCtrl->u.ssl.localSslCert)
      {
      certId = &secParams.encryptCtrl->u.ssl.localSslCert->certId;

      certStatus->locEncryptionInfoPresent = SD_TRUE;
      certStatus->locSslCertName           = certId->certName;
      certStatus->locSslCertExpirationTime = certId->validity.validNotAfter;

      certStatus->locSslCertStatus = secManCheckCertExpiration (certId, S_CERT_TYPE_LOCAL,
                                                                pSecCfg->certExpirationAlarmLimit,
                                                               &certStatus->locSslCertExpirationHoursLeft);
      }

    /* get expiration for remote SSL certificate */
    if (secParams.encryptCtrl->u.ssl.sslCert)
      {
      certId = &secParams.encryptCtrl->u.ssl.sslCert->certId;

      certStatus->remEncryptionInfoPresent = SD_TRUE;
      certStatus->remSslCertName           = certId->certName;
      certStatus->remSslCertExpirationTime = certId->validity.validNotAfter;

      certStatus->remSslCertStatus = secManCheckCertExpiration (certId, S_CERT_TYPE_REMOTE,
                                                                pSecCfg->certExpirationAlarmLimit,
                                                               &certStatus->remSslCertExpirationHoursLeft);
      }
    }

  secManReleaseCfg();
  return (SD_SUCCESS);	/* certStatus obtained */
}


/************************************************************************/
/*			ulPrintSecConnInfo				*/
/*----------------------------------------------------------------------*/
/* User's function to output to screen secure connection information	*/
/* returned from ulCheckCertExpiration call.				*/
/* Parameters:								*/
/*   MVL_NET_INFO    	 *cc	  	ptr to connection info		*/
/*   S_SEC_CERT_STATUS   *certStatus	ptr to struct where to return	*/
/*                                      certs status			*/
/* Return:								*/
/*   SD_SUCCESS		       	if printed certStatus successfully	*/
/*   SD_FAILURE or other error 	otherwise.				*/
/************************************************************************/
ST_RET ulPrintSecConnInfo (MVL_NET_INFO *cc, S_SEC_CERT_STATUS *certStatus)
  {
ST_RET           ret = SD_SUCCESS;
S_SEC_CONFIG    *pSecCfg = NULL;
S_SEC_SPARAMS    secParams;
struct tm       *gmTimePtr = NULL;
ST_CHAR         *timePtr;
S_CERT_CTRL     *certCtrl;


  if (!cc || !(cc->in_use && cc->conn_active))
    return (SD_FAILURE);

  if ((ret = secManAccessCfg (&pSecCfg)) != SD_SUCCESS)
    return (ret);

  memset (&secParams, 0, sizeof(S_SEC_SPARAMS));
  ret = sSecGetSecParam (cc, &secParams);
  if (ret != SD_SUCCESS)
    {
    printf ("\n\n\t Error obtaining security information");
    secManReleaseCfg();
    return (ret);
    }

  /* Authentication info */
  printf ("\n\n Local  AR Name='%s'", cc->loc_ar_sec->arName);
  printf ("\n Remote AR Name='%s'",   cc->rem_ar_sec->arName);
  printf ("\n Authentication : ");
  if (secParams.partAuthInfo->auth_pres == SD_FALSE)
    {
    /* no ACSE (MACE) authentication, maybe it is SSL certificate authentication */
    if (cc->rem_ar_sec->arAuthMode == S_AR_AUTH_MODE_SSL)
      printf ("SSL");
    else
      printf ("None");
    }
  else
    {
    /* ACSE Authentication present */
    printf ("MACE");
    if (certStatus->locMaceAuthInfoPresent)
      {
      printf ("\n  Local   MACE certificate name :        '%s'", (certStatus->locMaceCertName ? certStatus->locMaceCertName : " "));
      printf ("\n  Local   MACE certificate status :       %s",  (certStatus->locSslCertStatus ? S_SEC_CHECK_ERR_CODE_STR(certStatus->locMaceCertStatus) : "Valid"));
      printf ("\n  Local   MACE cert expiration hours :    %u",   certStatus->locMaceCertExpirationHoursLeft);
      timePtr = NULL;
      if (certStatus->locMaceCertExpirationTime > 0)
        {
        gmTimePtr = gmtime (&certStatus->locMaceCertExpirationTime);
	if (gmTimePtr)
          timePtr = asctime (gmTimePtr);
        }
      printf ("\n  Local   MACE cert expiration date GMT : %s", (timePtr ? timePtr : " "));
      }
    else
      {
      certCtrl = secParams.maceCert;
      if (certCtrl && certCtrl->certId.certName)
        printf ("\n  Local   MACE Certificate name :        '%s'", certCtrl->certId.certName);
      else
        printf ("\n  Local   MACE certificate name :         none");
      } 
  
    if (certStatus->remMaceAuthInfoPresent)
      {
      printf ("\n  Partner MACE certificate name :        '%s'", (certStatus->remMaceCertName ? certStatus->remMaceCertName : " "));
      printf ("\n  Partner MACE certificate status :       %s",  (certStatus->remMaceCertStatus ? S_SEC_CHECK_ERR_CODE_STR(certStatus->remMaceCertStatus) : "Valid"));
      printf ("\n  Partner MACE cert expiration hours :    %u",   certStatus->remMaceCertExpirationHoursLeft);
      timePtr = NULL;
      if (certStatus->remMaceCertExpirationTime > 0)
        {
        gmTimePtr = gmtime (&certStatus->remMaceCertExpirationTime);
	if (gmTimePtr)
            timePtr = asctime (gmTimePtr);
  	}
      printf ("\n  Partner MACE cert expiration date GMT : %s", (timePtr ? timePtr : " "));
      }
    else
      {
      certCtrl = secParams.partMaceCert;
      if (certCtrl && certCtrl->certId.certName)
        printf ("\n  Partner MACE Certificate name :        '%s'", certCtrl->certId.certName);
      else
        printf ("\n  Partner MACE certificate name :         none");
      }
    }
  
  /* SSL Encryption */
  printf ("\n Encryption : ");
  if (secParams.encryptCtrl->encryptMode == S_SEC_ENCRYPT_SSL)
    {
    printf ("SSL");
    printf ("\n  Cipher Suite : %6.6X (%s)",
           secParams.encryptCtrl->u.ssl.cipherSuite, secParams.encryptCtrl->u.ssl.cipherSuiteStr);
    /* local SSL cert info */
    if (certStatus->locEncryptionInfoPresent)
      {
      printf ("\n  Local   SSL  certificate name :        '%s'", (certStatus->locSslCertName ? certStatus->locSslCertName : " "));
      printf ("\n  Local   SSL  certificate status :       %s",  (certStatus->locSslCertStatus ? S_SEC_CHECK_ERR_CODE_STR(certStatus->locSslCertStatus) : "Valid"));
      printf ("\n  Local   SSL  cert expiration hours :    %u",   certStatus->locSslCertExpirationHoursLeft);
      timePtr = NULL;
      if (certStatus->locSslCertExpirationTime > 0)
        {
        gmTimePtr = gmtime (&certStatus->locSslCertExpirationTime);
  	if (gmTimePtr)
          timePtr = asctime (gmTimePtr);
  	}
      printf ("\n  Local   SSL  cert expiration date GMT : %s", (timePtr ?  timePtr : " "));
      }
    else
      {
      certCtrl = secParams.encryptCtrl->u.ssl.localSslCert;
      if (certCtrl && certCtrl->certId.certName)
        printf ("\n  Local   SSL  certificate name :    '%s'", certCtrl->certId.certName);
      else
        printf ("\n  Local   SSL  certificate name :     none");
      }
  
    /* partner SSL cert info */
    if (secParams.encryptCtrl->u.ssl.sslCertMatched == SD_TRUE)
      printf ("\n  Partner SSL  certificate Configured :   YES");
    else
      printf ("\n  Partner SSL  certificate Configured :   NO");
    if (certStatus->remEncryptionInfoPresent)
      {
      printf ("\n  Partner SSL  certificate name :        '%s'",   (certStatus->remSslCertName ? certStatus->remSslCertName : " "));
      printf ("\n  Partner SSL  certificate status :       %s",    (certStatus->remSslCertStatus ? S_SEC_CHECK_ERR_CODE_STR(certStatus->remSslCertStatus) : "Valid"));
      printf ("\n  Partner SSL  cert expiration hours :    %u",     certStatus->remSslCertExpirationHoursLeft);
      timePtr = NULL;
      if (certStatus->remSslCertExpirationTime > 0)
        {
        gmTimePtr = gmtime (&certStatus->remSslCertExpirationTime);
	if (gmTimePtr)
          timePtr = asctime (gmTimePtr);
  	}
      printf ("\n  Partner SSL  cert expiration date GMT : %s", (timePtr ?  timePtr : " "));
      }
    else
      {
      certCtrl = secParams.encryptCtrl->u.ssl.sslCert;
      if (certCtrl && certCtrl->certId.certName)
        printf ("\n  Partner SSL  certificate name :         '%s'", certCtrl->certId.certName);
      else
        printf ("\n  Partner SSL  certificate name :          none");
      }
    }
  else
    printf("None");

  secManReleaseCfg();
  return (ret);
  }


#endif /* defined(S_SEC_ENABLED) */
