/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*                  1986 - 1997 All Rights Reserved                     */
/*                                                                      */
/* MODULE NAME : mlogafil.c                                             */
/* PRODUCT(S)  : MMS-EASE 250-001                                       */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      This module contains user defined logging functions for         */
/*      mmsafil.c functions that require logging.                       */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 01/20/10  JRB     07    Fix log messages.				*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 04/14/99  MDE     04    Removed unnecessary include files		*/
/* 03/20/98  JRB     03    Don't need mmsop_en.h anymore.		*/
/* 08/04/97  MDE     02    Removed DEBUG_SISCO code			*/
/* 06/09/97  MDE     01    Added MMS-LITE ifdefs			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#ifndef MMS_LITE
#include "mms_usr.h"		/* to access MMS fucntions, variables   */
#else
#include "mmsdefs.h"
#endif
#include "mms_pfil.h"

#include "mloguser.h"

/************************************************************************/
/*			m_log_asn1_app_ref					*/
/* Function to convert ApplicationReference ASN.1 encoded data into an	*/
/* intermediately printable form.					*/
/************************************************************************/

ST_VOID m_log_asn1_app_ref (ST_UCHAR *buf_ptr, ST_INT buf_len)
  {
APP_REF ar_ref;

  ms_asn1_to_appref ( &ar_ref, buf_ptr, buf_len );
  m_log_app_ref ( &ar_ref );
  }

/************************************************************************/
/*			m_log_app_ref					*/
/* Function to convert ApplicationReference ASN.1 encoded data into an	*/
/* intermediately printable form.					*/
/************************************************************************/

ST_VOID m_log_app_ref (APP_REF *info)
  {
ST_INT i;

  MLOG_CALWAYS1 ("Number of AP-title components: %d",
                                info->ap_title.form_2.num_comps );

  for ( i=0; i < info->ap_title.form_2.num_comps; i++ )
    {
    MLOG_CALWAYS2 ("AP-title comp[ %d ] is: %d",i,
                                        info->ap_title.form_2.comps[i]);
    }

  if ( info->ap_invoke_pres )
    {
    MLOG_CALWAYS1 ("AP-invocation-id: %ld",(ST_LONG)info->ap_invoke);
    }
  else
    {
    MLOG_CALWAYS0 ("AP-invocation-id: Not Present");
    }

  if ( info->ae_qual_pres )
    {
    MLOG_CALWAYS1 ("AE-qualifier : %ld",(ST_LONG)info->ae_qual.form_2);
    }
  else
    {
    MLOG_CALWAYS0 ("AE-qualifier : Not Present");
    }

  if ( info->ae_invoke_pres )
    {
    MLOG_CALWAYS1 ("AE-invoke-id : %ld",(ST_LONG)info->ae_invoke);
    }
  else
    {
    MLOG_CALWAYS0 ("AE-invoke-id : Not present");
    }
  }

