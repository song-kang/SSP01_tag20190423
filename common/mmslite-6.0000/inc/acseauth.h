/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995-2010, All Rights Reserved					*/
/*									*/
/* MODULE NAME : acseauth.h 						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	ACSE Authentication header file.     				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 03/31/10  EJV    08     Chg ACSE_MAX_LEN_PASSWORD from 16 to 128.	*/
/* 02/05/04  EJV    07     Added MACE_MECH_ID and PASWORD_MECH_ID defs.	*/
/* 02/04/04  EJV    06     Removed extern maceMechId.			*/
/* 08/20/03  EJV    05     Added extern maceMechId.			*/
/* 07/08/03  EJV    04     Del old password auth (OBSOLETE_ACSE_AUTH)	*/
/* 03/19/03  MDE    03     Added ACSE_AUTH_MECH_MACE_CERT		*/
/* 03/13/03  EJV    02     Added asn1r.h.				*/
/* 09/30/02  ASK    01     Created					*/
/************************************************************************/

#ifndef ACSEAUTH_INCLUDED
#define ACSEAUTH_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "asn1r.h"				/* for MMS_OBJ_ID	*/

/************************************************************************/
/* New Authentication stuff						*/
/************************************************************************/
#define ACSE_MAX_LEN_PASSWORD		128

typedef ST_INT16     ST_ACSE_AUTH;

/* Authentication mechanisms MMS_OBJ_ID */
#define PASS_MECH_ID	{4, {2 ,2 ,3 ,1 }}

#define MACE_MECH_ID  \
		  { 8,		/* num_comps 			*/ \
		    {		/* comps			*/ \
		    1,		/* ??? 				*/ \
		    0,		/* ??? 				*/ \
		    840,	/* iso member-body usa		*/ \
		    0,		/* ansi-t1-259-1997		*/ \
		    1,		/* stase			*/ \
		    0,		/* stase-authentication value	*/ \
		    1,		/* abstractSyntax		*/ \
		    1		/* version			*/ \
		    }						   \
		  }

/* The ACSE user shall return one of the falling codes once they have   */
/* taken a look at the partner's ACSE_AUTH_INFO structure.		*/

/* These return codes map to the ABRT-diagnostic of the Abort PDU. 	*/
/* Returning any of these will result in an abort PDU being sent with 	*/
/* the specified diagnostic code.					*/
#define ACSE_AUTH_SUCCESS			0
#define ACSE_DIAG_NO_REASON			1
#define ACSE_DIAG_PROTOCOL_ERROR		2
#define ACSE_DIAG_AUTH_MECH_NAME_NOT_RECOGNIZED	3
#define ACSE_DIAG_AUTH_MECH_NAME_REQUIRED	4
#define ACSE_DIAG_AUTH_FAILURE			5
#define ACSE_DIAG_AUTH_REQUIRED			6

/* Mechanism types */
#define ACSE_AUTH_MECH_PASSWORD                 0
#define ACSE_AUTH_MECH_MACE_CERT                1
#define ACSE_AUTH_MECH_OTHER                    2

/* This structure is used for any mechanism data that must be 		*/
/* encoded/decoded by the user instead of the ACSE.			*/
typedef struct 
  {
  ST_INT len;			/* Length of encoded data.		*/
  ST_UCHAR *ptr;		/* Pointer to encoded data.		*/
  ST_UCHAR *buf;		/* User buffer (chk_free'd if != NULL)	*/
  } USR_AUTH_BUFFER;


/* User-Supplied Authentication Structure */
typedef struct 
  {
  ST_BOOLEAN auth_pres; /* If this structure is filled out */
  ST_INT     mech_type;	/* One of the mechanism types, defined above */

  union
    {
    struct
      {
      ST_CHAR password[ACSE_MAX_LEN_PASSWORD + 1];  /* graphical string - NULL terminated */
      }	pw_auth;
    struct
      {
      MMS_OBJ_ID mech_id; 	   /* User must set to desired id 	*/
      USR_AUTH_BUFFER auth_value;  /* User must encode/decode ASN.1	*/
      } other_auth;
    } u;
  
  } ACSE_AUTH_INFO;



/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* ACSEAUTH_INCLUDED */
/************************************************************************/
