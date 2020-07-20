/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2010, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : cospmain.c						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements functions common to CALLING and	*/
/* 		CALLED side of the COSP protocol.			*/
/*									*/
/*  For information see the:						*/
/*	ISO 8326 "Information processing systems - Open Systems		*/
/*	  Interconnection - Basic connection oriented session service	*/
/*	  definition.							*/
/*	ISO 8327 "Information processing systems - Open Systems		*/
/*	  Interconnection - Basic connection oriented session protocol	*/
/*	  specification.						*/
/*	ISO 8327/ADD.2 (Draft for Version2).				*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			cosp_bind					*/
/*			cosp_unbind					*/
/*									*/
/*			cosp_con_req					*/
/*			cosp_con_rsp_pos				*/
/*			cosp_con_rsp_neg				*/
/*									*/
/*			cosp_rel_req					*/
/*			cosp_rel_rsp_pos				*/
/*									*/
/*			cosp_u_abort_req				*/
/*									*/
/*			cosp_data_req					*/
/*									*/
/*			tp4_bind_cnf					*/
/*			tp4_unbind_cnf					*/
/*									*/
/*			tp4_connect_ind					*/
/*			tp4_connect_cnf					*/
/*			tp4_disconnect_ind				*/
/*			tp4_data_ind 					*/
/*			tp4_expdata_ind 				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/09/11  JRB	   Move session_cfg global to lean_var.c.	*/
/* 09/07/10  EJV	   acse_free_con: free local auth_info.		*/
/* 11/17/09  JRB    22     Chg "*_bind_id" & "*_conn_id" to (ST_VOID *).*/
/*			   cosp_con_req: pass loc_addr instead of bind_id.*/
/*			   tp4_connect: pass loc_addr instead of bind_id.*/
/*			   Use new separate cosp_enc_cn & cosp_enc_ac.	*/
/*			   Add if !SNAP_LITE around cosp_process_connect*/
/*			   Chk Local SSEL in cosp_process_connect	*/
/*			   (chk was removed from cosp_validate_cn_ac).	*/
/* 07/30/08  JRB    21     Chg log msg from ERR to ENC.			*/
/* 04/28/08  GLB    20     Removed CALLED_ONLY and CALLING_ONLY         */
/* 04/09/08  JRB    19     Fix log message for unsupported SPDU.	*/
/* 05/05/05  EJV    18     cosp_con_req: copy new port param.		*/
/* 09/17/04  JRB    17     cosp_con_req: save tp4_conn_id returned from	*/
/*			   modified tp4_connect (need for abort fix).	*/
/*			   cosp_u_abort_req: if waiting for T-CONNECT.cnf,*/
/*			   call tp4_disconnect (correct ISO 8327 action).*/
/* 01/20/04  EJV    16     acse_free_con: add a_free_part_security_info.*/
/* 11/14/03  JRB    15     Save encrypt_ctrl ONLY for TP0 (not for TP4).*/
/* 08/25/03  EJV    14     Added param to tp4_connect().		*/
/*			   tp4_connect_ind, cnf: save encrypt_ctrl	*/
/* 06/20/02  JRB    13     Chg addr args to tp4_connect_ind &		*/
/*			   copy them to ACSE_CONN.			*/
/* 06/05/02  JRB    12     Allow Called SSEL=NULL (0 length) on conn req*/
/* 09/10/01  JRB    11     Remove last change but fix cosp_cn_ptr free.	*/
/* 08/01/01  JRB    10     Del rem_addr from cosp_enc_cn_ac calls.	*/
/*			   Del use of cosp_cn_ptr, cosp_cn_len.		*/
/* 05/09/01  JRB    09     On S-CONNECT.ind, save SSELs in ACSE_CONN.	*/
/* 03/14/01  JRB    08     Use new SMEM allocation functions.		*/
/*			   Add global variable "session_cfg".		*/
/* 09/13/99  MDE    07     Added SD_CONST modifiers			*/
/* 08/13/98  JRB    06     Lint cleanup.				*/
/* 06/17/98  JRB    05     Allow second call to cosp_bind if SSEL same.	*/
/* 12/30/97  JRB    04     cosp_bind return same as tp4_bind if it fails*/
/* 07/31/97  JRB    03     cosp_bind & cosp_con_req: set "tp_type" in	*/
/*			   tp4_addr.					*/ 
/*			   Don't set NSAP on tp4_bind call (ignored).	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/20/97  EJV    02     Enhanced logging and collision handling.	*/
/* 02/07/97  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;	/* Define for SLOG mechanism	*/
#endif

#include "mem_chk.h"
#include "tp4api.h"
#include "tp4.h"	/* need MIN_TP0_CONN_ID	definition	*/
#include "acse2.h"

#include "cosp_log.h"
#include "cosp.h"
#include "cosp_usr.h"


	/*======================================================*/
	/*							*/
	/* 		L O C A L  V A R I A B L E S		*/
	/*			A N D				*/
	/*	F U N C T I O N S   P R O T O T Y P E S		*/
	/*							*/
	/*======================================================*/

/* define actions to perform after return from one of decode functions */
#define COSP_S_P_ABORT_IND		1
#define COSP_SEND_ABORT_SPDU		2
#define COSP_T_DISCONNECT		4


static ST_RET cosp_send_abort (ACSE_CONN *con, ST_INT err_code);

static ST_RET cosp_process_finish  (ACSE_CONN *con, ST_UINT spdu_len, char *spdu_buf);
static ST_RET cosp_process_accept  (ACSE_CONN *con, ST_UINT spdu_len, char *spdu_buf);
static ST_RET cosp_process_refuse  (ACSE_CONN *con, ST_UINT spdu_len, char *spdu_buf);
static ST_RET cosp_process_discon  (ACSE_CONN *con, ST_UINT spdu_len, char *spdu_buf);
static ST_RET cosp_process_abort   (ACSE_CONN *con, ST_UINT spdu_len, char *spdu_buf);
static ST_RET cosp_process_data    (ACSE_CONN *con, ST_UINT spdu_len, char *spdu_buf);


/************************************************************************/
/* Global and static variables.						*/
/************************************************************************/
/* flag below prevents calling acse_free_con twice (if we are in func	*/
/* tp4_connect and tp4_disconnect_ind is called)			*/

static ST_BOOLEAN cosp_in_tp4_func;


ST_UCHAR   cosp_only_ssel [1+MAX_SSEL_LEN];

#if !defined(SNAP_LITE)		/* SNAP_LITE code in other files	*/
static ST_BOOLEAN cosp_bind_called;	/* only 1 binding allowed 	*/
	/*======================================================*/
	/*							*/
	/*		B I N D I N G   F U N C T I O N S	*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			cosp_bind					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to bind to local Session Address.		*/
/* The SS-user should implement the u_cosp_bind_cnf function.		*/
/*									*/
/* Parameters:								*/
/*  ST_VOID *	cosp_user_bind_id	COSP User's id for this binding.*/
/*  PRES_ADDR  *loc_addr		Local Address to bind to	*/
/*  ST_INT	sharable		Ignored, (for compatibility with*/
/*  ST_INT	max_conns		Ignored, previous versions)	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if bind successful			*/
/*	error code 		otherwise				*/
/************************************************************************/
ST_RET cosp_bind (ST_VOID *cosp_user_bind_id, PRES_ADDR *loc_addr,
		   ST_INT sharable, ST_INT max_conns)
{
ST_INT		ret;
TP4_ADDR	tp4_addr;
ST_VOID *	user_bind_id;	/* TP4 user bind id */

  COSP_LOG_ENC1 ("S-BIND.req:    cosp_user_bind_id =" S_FMT_PTR, cosp_user_bind_id);

  ret = SD_SUCCESS;

  /* Check SSEL.  If OK, save it.		*/	
  if (!cosp_bind_called)
    {
    /* If local SSEL len is legal, save SSEL.	*/
    if (loc_addr->ssel_len <= MAX_SSEL_LEN)
      {
      if ((cosp_only_ssel [0] = (ST_UCHAR) loc_addr->ssel_len) > 0)
        memcpy (&cosp_only_ssel [1], loc_addr->ssel, loc_addr->ssel_len);
      }
    else
      {
      ret = COSP_ERR_INV_SSEL;			/* invalid loc SSEL	*/
      COSP_LOG_ERR1 ("COSP-ERROR: S-BIND.req: Invalid length=%d of SSEL.",
        loc_addr->ssel_len);
      }
    }
  else
    {    /* Second Bind. Only allow it if the SSEL is the same.		*/
    if (cosp_only_ssel [0] != (ST_UCHAR) loc_addr->ssel_len  ||
         (loc_addr->ssel_len != 0  &&
          memcmp (&cosp_only_ssel [1], loc_addr->ssel, loc_addr->ssel_len)))
      {  
      COSP_LOG_ERR0 ("COSP-ERROR: Cannot BIND to different SSEL");
      ret = COSP_ERR_INV_SSEL;
      }  
    }

  /* Bind to Transport.		*/
  if (ret == SD_SUCCESS)
    {
    /* Copy tsel from pres_addr to tp4_addr struct. Don't copy nsap	*/
    /* because tp4_bind ignores it anyway.				*/
    if (loc_addr->tsel_len <= sizeof (tp4_addr.tp4_sel))
      {
      tp4_addr.tp4_sel_len = loc_addr->tsel_len;
      memcpy (tp4_addr.tp4_sel, loc_addr->tsel, loc_addr->tsel_len);
      tp4_addr.tp_type = loc_addr->tp_type;	/* TP4 or TCP?		*/

      user_bind_id = cosp_user_bind_id;	/* pass bind id	down */

      if ((ret = tp4_bind (user_bind_id, &tp4_addr, sharable, max_conns))
          != SD_SUCCESS)
        {
        COSP_LOG_ERR0 ("COSP-ERROR: S-BIND.req: T-BIND.req failed.");
        }
      }
    else
      {
      ret = COSP_ERR_INV_TP4_ADDR;		/* TP4 bind failed	*/
      COSP_LOG_ERR2 ("COSP-ERROR: S-BIND.req: Invalid TSEL length=%d or NSAP length=%d.",
        loc_addr->tsel_len, loc_addr->nsap_len);
      }
    }	/* end Transport binding section	*/

  /* If successful, set called flag. Otherwise, treat next bind like first.*/
  if (ret == SD_SUCCESS)
    cosp_bind_called = SD_TRUE;

  return (ret);
}


/************************************************************************/
/*			tp4_bind_cnf					*/
/*----------------------------------------------------------------------*/
/* This function is called by the TP-provider to indicate the result of	*/
/* the bind operation (see cosp_bind func).				*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID *	user_bind_id	User's id (COSP id) for this binding.	*/
/*  ST_VOID *	tp4_bind_id	TP4 assigned ID for this binding.	*/
/*  ST_INT	result  	Indicates if the bind was successful:	*/
/*					=  0	Success			*/
/*					<> 0	Error code		*/
/*									*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID tp4_bind_cnf (ST_VOID *user_bind_id, ST_VOID *tp4_bind_id, ST_RET result)
{
ST_VOID *cosp_user_bind_id;
ST_VOID *cosp_bind_id;

  cosp_user_bind_id = user_bind_id;	/* we passing same bind id up */
  cosp_bind_id = tp4_bind_id;		/* COSP bind id same as TP4	*/

  COSP_LOG_DEC3 ("S-BIND.cnf:    cosp_user_bind_id =" S_FMT_PTR "  cosp_bind_id =" S_FMT_PTR "  result =%d",
    cosp_user_bind_id, cosp_bind_id, result);

  u_cosp_bind_cnf (cosp_user_bind_id, cosp_bind_id, result);
}


/************************************************************************/
/*			cosp_unbind					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to unbind from local Session Address.	*/
/* The SS-user should implement the u_cosp_unbind_cnf function.		*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID *	cosp_bind_id	COSP's id to unbind.			*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if unbind successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_unbind (ST_VOID *cosp_bind_id)
{
ST_VOID *tp4_bind_id;

  COSP_LOG_ENC1 ("S-UNBIND.req:                                 cosp_bind_id =" S_FMT_PTR,
                 cosp_bind_id);

  tp4_bind_id = cosp_bind_id;

  /* unbind the transport */
  if (tp4_unbind (tp4_bind_id) != SD_SUCCESS)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: S-UNBIND.req: T-UNBIND.req failed.");
    return (COSP_ERR_TP4_RET);
    }
  return (SD_SUCCESS);
}


/************************************************************************/
/*			tp4_unbind_cnf					*/
/*----------------------------------------------------------------------*/
/* This function is called by the TP4-provider to indicate that the	*/
/* unbind operation completed.						*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID *	user_bind_id	User's id for this binding.		*/
/*									*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID tp4_unbind_cnf (ST_VOID *user_bind_id)
{
ST_VOID *cosp_user_bind_id;

  cosp_user_bind_id = user_bind_id;

  COSP_LOG_DEC1 ("S-UNBIND.cnf:  cosp_user_bind_id =" S_FMT_PTR, cosp_user_bind_id);

  u_cosp_unbind_cnf (cosp_user_bind_id);
}
#endif	/* !defined(SNAP_LITE)	*/


/************************************************************************/
/*                      acse_free_con					*/
/*----------------------------------------------------------------------*/
/* This function will free the pointer to connection info.		*/
/* In most cases the COSP engine is responsible for releasing the con	*/
/* pointer.								*/
/*	  								*/
/* Parameters:								*/
/*  ACSE_CONN *con		Pointer to connection info struct	*/
/*	  								*/
/* Return:								*/
/*  ST_VOID		none						*/
/************************************************************************/
ST_VOID acse_free_con (ACSE_CONN *con)
{
  if (con)
    {
#if defined(MMS_LITE)	/* MMS-EASE keeps the auth_info in the CHAN_INFO */
    /* free Authentication Info */
    a_free_security_info (&con->auth_info);
    a_free_part_security_info (con);
#endif

    if (con->cosp_cn_ptr)
      M_FREE (MSMEM_COSP_CN, con->cosp_cn_ptr);	/* free saved encoded CN SPDU	*/

    M_FREE (MSMEM_ACSE_CONN, con);
    }
}

	/*======================================================*/
	/*							*/
	/*   C O N N E C T  (CALLED SIDE)  F U N C T I O N S	*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			tp4_connect_ind					*/
/*----------------------------------------------------------------------*/
/* This function is called by the TP4-provider to indicate that a	*/
/* remote node wishes to establish a connection.			*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID      *user_bind_id	User's id for this binding.		*/
/*  ST_LONG	  tp4_conn_id	COSP connection id for this connect.	*/
/*  ST_UCHAR     *loc_tsel      Local (called) TSEL.			*/
/*  ST_UCHAR     *rem_tsel      Remote (calling) TSEL.			*/
/*  ST_UCHAR     *rem_nsap      Remote (calling) NSAP (TP4) or		*/
/*  ST_ULONG      rem_ip	Remote (calling) IP addr(TP0)		*/
/*  ST_INT	  conndata_len	Length of opt connect data <= 32 bytes	*/
/*  char	 *conndata	Pointer to opt connect data.		*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID tp4_connect_ind (ST_VOID *user_bind_id, ST_LONG tp4_conn_id,
                      ST_UCHAR *loc_tsel, ST_UCHAR *rem_tsel,
                      ST_UCHAR *rem_nsap, ST_ULONG rem_ip,
                      ST_INT conndata_len, char *conndata)
{
ACSE_CONN       *con;

  tp4_reldata (conndata);		/* !!! we ignoring conndata	*/

  con = (ACSE_CONN *) M_CALLOC (MSMEM_ACSE_CONN, 1, sizeof (ACSE_CONN));

  /* accept incomming transport connection */
  if (tp4_accept (tp4_conn_id, con, 0, NULL) == SD_SUCCESS)
    {
    con->cosp_state   = COSP_CSTATE_IDLE_TCON;
    con->cosp_bind_id = user_bind_id;	/* save the id to pass to COPP */
    con->cosp_vtca    = SD_TRUE;           /* transport conn acceptor	*/
    con->tp4_conn_id  = tp4_conn_id;
    /* Save TSELs, NSAP in ACSE_CONN struct.				*/
    memcpy (con->loc_tsel, loc_tsel, loc_tsel [0]+1);
    memcpy (con->rem_tsel, rem_tsel, rem_tsel [0]+1);
    if (tp4_conn_id >= MIN_TP0_CONN_ID)	/* This is TP0 conn. Save IP addr*/
      {
      con->remNetAddr.ip = rem_ip;
#if defined (TP0_ENABLED)
      /* If connection is TP0, save encrypt_ctrl.	*/
      if (tp4_conn_id >= MIN_TP0_CONN_ID)
        {	/* this is TP0 connection	*/
        TP0_CONN *tp0_conn = &tp0_conn_arr [tp4_conn_id-MIN_TP0_CONN_ID];
        /* save the encryption info in ACSE_CONN */
        con->encrypt_ctrl = tp0_conn->sock_info->encrypt_ctrl;
        }
#endif
      }
    else				/* This is TP4 conn. Save NSAP	*/
      memcpy (con->remNetAddr.nsap, rem_nsap, rem_nsap [0]+1);
    }
  else
    {
    COSP_LOG_ERR0 ("COSP-ERROR: T-CONNECT.ind: T-ACCEPT.req connection failed.");

    M_FREE (MSMEM_STARTUP, con);

    /* reject incomming transport connection, accept failed */
    tp4_disconnect (tp4_conn_id, 0, NULL);
    }
}


/************************************************************************/
/*			cosp_con_rsp_pos				*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to Accept an incomming connection.	*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_con_rsp_pos (ACSE_CONN *con)
{
ST_INT		ret;
ST_UINT	spdu_len;
char           *spdu_ptr;

  COSP_LOG_ENC3 ("S-CONNECT.rsp+:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
  COSP_LOG_ENCH (con->ppdu_len, con->ppdu_ptr);

  ret = SD_SUCCESS;

  if (con->cosp_state == COSP_CSTATE_WAIT_CON_RSP)
    {
    /* encode ACCEPT SPDU */
    /* Calling or Responding SSEL are NOT encoded. These should	never	*/
    /* be needed and it reduces the bytes on the wire.			*/
    ret = cosp_enc_ac (con, &spdu_ptr, &spdu_len);
    if (ret == SD_SUCCESS)
      {
      if (tp4_data (con->tp4_conn_id, SD_TRUE, spdu_len, spdu_ptr) == SD_SUCCESS)
        {
        con->cosp_state = COSP_CSTATE_DATA_XFER;
        con->cosp_vcoll = SD_FALSE;
        con->cosp_vdnr  = SD_FALSE;
        }
      else
        {
        ret = COSP_ERR_TP4_RET;
        COSP_LOG_ERR1 ("COSP-ERROR: S-CONNECT.rsp+ acse_conn_id=" S_FMT_PTR ": T-DATA.req failed.",
          con);
        }
      }
    else
      {
      COSP_LOG_ERR1 ("COSP-ERROR: S-CONNECT.rsp+ acse_conn_id=" S_FMT_PTR ": Encoding ACCEPT SPDU failed.",
        con);
      }
    }
  else
    {
    ret = COSP_ERR_INV_CON_STATE;
    COSP_LOG_ERR2 ("COSP-ERROR: S-CONNECT.rsp+ acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
      con, con->cosp_state);
    }

  return (ret);
}


/************************************************************************/
/*			cosp_con_rsp_neg				*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to Reject an incomming connection.	*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*  ST_INT	reason		Reason for reject COSP_CON_RSP_U_...	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_con_rsp_neg (ACSE_CONN *con, ST_INT reason)
{
ST_INT		ret;
ST_UINT	spdu_len;
char           *spdu_ptr;
ST_UCHAR		reason_code;

  COSP_LOG_ENC4 ("S-CONNECT.rsp-:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "  reason =%d \n"
                 "  User data length: %u", con, con->user_conn_id, reason, con->ppdu_len);
  COSP_LOG_ENCH (con->ppdu_len, con->ppdu_ptr);

  ret = SD_SUCCESS;

  if (con->cosp_state == COSP_CSTATE_WAIT_CON_RSP)
    {
    /* encode REFUSE SPDU */
    switch (reason)
      {				   
      case COSP_CON_RSP_U_CONGESTION:
        reason_code = COSP_RF_REASON_U_CONGESTION;
      break;

      case COSP_CON_RSP_U_IN_UDATA:
        reason_code = COSP_RF_REASON_U_REJECT;
        /* SS-user data should be present in this case */
      break;

      case COSP_CON_RSP_U_NOT_SPECIFIED:
      default:
        reason_code = COSP_RF_REASON_U_NOT_SPECIFIED;
      }

    /* check if user data present */
    if (reason == COSP_CON_RSP_U_IN_UDATA)
      {
      /* SS-user data should be present in this case */
      if (con->ppdu_len == 0)
        ret = COSP_ERR_INV_UDATA_LEN;
      }
    else
      {
      /* SS-user data should NOT be present in this case */
      if (con->ppdu_len != 0)
        ret = COSP_ERR_INV_UDATA_LEN;
      }

    if (ret == SD_SUCCESS)
      {
      ret = cosp_enc_rf (con, &spdu_ptr, &spdu_len, reason_code);
      if (ret == SD_SUCCESS)
        {
        if (tp4_data (con->tp4_conn_id, SD_TRUE, spdu_len, spdu_ptr) == SD_SUCCESS)
          {
          con->cosp_state = COSP_CSTATE_WAIT_TDISCON_IND;
          tp4_session_timer (con->tp4_conn_id, SD_TRUE);	/* start timer	*/
          }
        else
          {
          ret = COSP_ERR_TP4_RET;
          COSP_LOG_ERR1 ("COSP-ERROR: S-CONNECT.rsp- acse_conn_id=" S_FMT_PTR ": T-DATA.req failed.",
            con);
          }
        }
      else
        {
        COSP_LOG_ERR1 ("COSP-ERROR: S-CONNECT.rsp- acse_conn_id=" S_FMT_PTR ": Encoding REFUSE SPDU failed.",
          con);
        }
      }
    else
      {
      COSP_LOG_ERR3 ("COSP-ERROR: S-CONNECT.rsp- acse_conn_id=" S_FMT_PTR ": Invalid SS-UserData length=%u for Reason=%d.",
        con, con->ppdu_len, reason);
      }
    }
  else
    {
    ret = COSP_ERR_INV_CON_STATE;
    COSP_LOG_ERR2 ("COSP-ERROR: S-CONNECT.rsp- acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
      con, con->cosp_state);
    }

  return (ret);
}

	/*======================================================*/
	/*							*/
	/*   C O N N E C T  (CALLING SIDE)  F U N C T I O N S	*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			cosp_con_req					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to establish Session Layer connection.	*/
/* SS-user should implement the u_cosp_con_cnf_pos & u_cosp_con_cnf_neg	*/
/* functions to receive confirm	for this request.			*/
/* If this function returns value other then SD_SUCCESS then the function	*/
/* u_cosp_con_cnf_xxx will not be called.				*/
/* 									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_con_req (
	ACSE_CONN *con,		/* Connection info			*/
	PRES_ADDR *loc_addr,	/* Local Presentation Address		*/
	PRES_ADDR *rem_addr)	/* Remote Presentation Address		*/
{
ST_RET		ret;
TP4_ADDR        loc_tp4_addr;	/* Local Transport Address		*/
TP4_ADDR        tp4_addr;	/* Remote Transport Address		*/
ST_UINT	spdu_len;
char           *spdu_ptr;

  COSP_LOG_ENC3 ("S-CONNECT.req:   acse_conn_id =" S_FMT_PTR 
                 "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
  COSP_LOG_ENCH (con->ppdu_len, con->ppdu_ptr);

  if (con->cosp_state != COSP_CSTATE_IDLE)
    {
    ret = COSP_ERR_INV_CON_STATE;               /* invalid connect state*/
    COSP_LOG_ERR2 ("COSP-ERROR: S-CONNECT.req acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
      con, con->cosp_state);
    return (ret);
    }

  /* Check "Called" SSEL. May be NULL (i.e. length=0) */
  if (rem_addr->ssel_len <= MAX_SSEL_LEN && loc_addr->ssel_len <= MAX_SSEL_LEN)
    {
    /* Save local and remote SSEL in ACSE_CONN struct.			*/
    /* Used in "cosp_enc_cn". May also be needed later.			*/
    con->loc_ssel[0] = loc_addr->ssel_len;	/* first byte is len	*/
    memcpy (&con->loc_ssel[1], loc_addr->ssel, loc_addr->ssel_len);

    con->rem_ssel[0] = rem_addr->ssel_len;	/* first byte is len	*/
    memcpy (&con->rem_ssel[1], rem_addr->ssel, rem_addr->ssel_len);

    /* Encode CN request.	*/
    ret = cosp_enc_cn (con, &spdu_ptr, &spdu_len);
    if (ret == SD_SUCCESS)
      {
      /* we always will initiate new transport connection for every new	*/
      /* session connection, so find empty spot for con info		*/

      /* Copy remote TSEL and NSAP (or IP Addr) to "tp4_addr".	*/
      tp4_addr.tp4_sel_len = rem_addr->tsel_len;
      memcpy (tp4_addr.tp4_sel, rem_addr->tsel, rem_addr->tsel_len);
      tp4_addr.tp_type = rem_addr->tp_type;
      if (rem_addr->tp_type == TP_TYPE_TP4)
        {
        tp4_addr.net_addr_len = rem_addr->nsap_len;
        memcpy (tp4_addr.netAddr.nsap, rem_addr->netAddr.nsap, rem_addr->nsap_len);
        }
      else
        {
        tp4_addr.netAddr.ip = rem_addr->netAddr.ip;	/* IP Addr	*/
        tp4_addr.port = rem_addr->port;
	}

      /* Copy local TSEL to "loc_tp4_addr".				*/
      /* NOTE: local NSAP or IP addr not used so don't copy them.	*/
      loc_tp4_addr.tp4_sel_len = loc_addr->tsel_len;
      memcpy (loc_tp4_addr.tp4_sel, loc_addr->tsel, loc_addr->tsel_len);
      loc_tp4_addr.tp_type = loc_addr->tp_type;

      /* Note: !!! the tp4_disconnect_ind may be called before the	*/
      /* tp4_connect function returns (SD_FAILURE), we want to log	the err	*/
      /* only in one place (here)					*/
      cosp_in_tp4_func = SD_TRUE;			/* prevents logging err */

      /* request transport connection (no connect data)	*/
      if ((con->tp4_conn_id = tp4_connect (con, &loc_tp4_addr, &tp4_addr, 0, NULL,
                    &con->encrypt_ctrl)) >= 0)
        {
        /* save the encoded CN until T-CONNECT.cnf received */
        con->cosp_state  = COSP_CSTATE_WAIT_TCON_CNF;
        con->cosp_cn_len = spdu_len;
        con->cosp_cn_ptr = (ST_CHAR *) M_MALLOC (MSMEM_COSP_CN, spdu_len);
        memcpy (con->cosp_cn_ptr, spdu_ptr, spdu_len);
        con->cosp_vtca   = SD_FALSE;      /* transport conn initiator	*/
        }
      else
        {
        ret = COSP_ERR_TP4_RET;
        COSP_LOG_ENC1 ("COSP-ERROR: S-CONNECT.req acse_conn_id=" S_FMT_PTR ": T-CONNECT.req failed.",
          con);
        }

      cosp_in_tp4_func = SD_FALSE;	/* turn off the flag */
      }
    else
      {
      COSP_LOG_ERR1 ("COSP-ERROR: S-CONNECT.req acse_conn_id=" S_FMT_PTR ": Encoding CONNECT SPDU failed.",
        con);
      }
    }
  else
    {
    ret = COSP_ERR_INV_SSEL;			/* invalid SSEL (length) */
    COSP_LOG_ERR3 ("COSP-ERROR: S-CONNECT.req acse_conn_id=" S_FMT_PTR ": SSEL too long: local=%d, remote=%d.",
      con, loc_addr->ssel_len, rem_addr->ssel_len);
    }

  return (ret);
}


/************************************************************************/
/*			tp4_connect_cnf					*/
/*----------------------------------------------------------------------*/
/* This function is called by the TP4-provider to indicate that	the	*/
/* transport connection has been established.				*/
/* NOTE: cosp_con_req saved the TP4 connection ID in con->tp4_conn_id.	*/
/*       It should match the tp4_conn_id passed to this function.	*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID      *user_conn_id	User's connection id for this connect.	*/
/*  ST_LONG	  tp4_conn_id	TP4 connection id for this connect.	*/
/*  ST_INT	 conndata_len	Length of optional connect data.	*/
/*  char	*conndata	Pointer to optional connect data.	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID tp4_connect_cnf (ST_VOID *user_conn_id, ST_LONG tp4_conn_id, 
  		      ST_INT conndata_len, char *conndata)
{
ACSE_CONN       *con;

  tp4_reldata (conndata);		/* !!! we ignoring conndata	*/

  con = (ACSE_CONN *) user_conn_id;

  if (con->cosp_state == COSP_CSTATE_WAIT_TCON_CNF)
    {
#if defined (TP0_ENABLED)
    /* If connection is TP0, save encrypt_ctrl.	*/
    if (tp4_conn_id >= MIN_TP0_CONN_ID)
      {	/* this is TP0 connection	*/
      TP0_CONN *tp0_conn = &tp0_conn_arr [tp4_conn_id-MIN_TP0_CONN_ID];
      /* save the encryption info in ACSE_CONN */
      con->encrypt_ctrl = tp0_conn->sock_info->encrypt_ctrl;
      }
#endif

    /* send the CN (already encoded) on the established transport connection */
    if (tp4_data (tp4_conn_id, SD_TRUE, con->cosp_cn_len, con->cosp_cn_ptr)
      == SD_SUCCESS)
      {
      con->cosp_state = COSP_CSTATE_WAIT_AC;		/* wait AC or RF */

      /* we are done with the encoding buffer for CN */
      if (con->cosp_cn_ptr)
        {
        M_FREE (MSMEM_COSP_CN, con->cosp_cn_ptr);	
        con->cosp_cn_len = 0;
        con->cosp_cn_ptr = NULL;
        }
      }
    else
      {
      COSP_LOG_ERR1 ("COSP-ERROR: T-CONNECT.cnf acse_conn_id=" S_FMT_PTR ": T-DATA.req failed",
        con);
      }
    }
  else
    {
    COSP_LOG_ERR2 ("COSP-ERROR: T-CONNECT.cnf acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
      con, con->cosp_state);
    }
}

	/*======================================================*/
	/*							*/
	/*  C O N N E C T I O N   R E L E A S E   (CALLED SIDE)	*/
	/*							*/
	/*======================================================*/

/************************************************************************/
/*			cosp_rel_rsp_pos				*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to Accept a release of connection.	*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_rel_rsp_pos (ACSE_CONN *con)
{
ST_INT		ret;
ST_UINT	spdu_len;
char           *spdu_ptr;

  COSP_LOG_ENC3 ("S-RELEASE.rsp+:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
  COSP_LOG_ENCH (con->ppdu_len, con->ppdu_ptr);

  if (con->cosp_state == COSP_CSTATE_WAIT_REL_RSP)
    {
    /* some additional predicates must be also true */
    if (!con->cosp_vcoll ||
        (con->cosp_vcoll && con->cosp_vdnr) ||	/* collision but DN received */
        (con->cosp_vcoll && !con->cosp_vtca))   /* collision and we are con initiator */
      {
      /* connection released confirmed by COPP, encode and send DISCONNECT SPDU */
      /* Transport connections are not reused.				*/
      ret = cosp_enc_fn_dn (con, &spdu_ptr, &spdu_len, COSP_SI_DISCON);
      if (ret == SD_SUCCESS)
        {
        if (tp4_data (con->tp4_conn_id, SD_TRUE, spdu_len, spdu_ptr) == SD_SUCCESS)
          {
          if (!con->cosp_vcoll ||
              (con->cosp_vcoll && con->cosp_vdnr)) /* collision but DN received */
            {
            con->cosp_state = COSP_CSTATE_WAIT_TDISCON_IND;
            tp4_session_timer (con->tp4_conn_id, SD_TRUE);	/* start timer	*/
            }
          else
            {
            /* collision and we are the con initiator */
            con->cosp_state = COSP_CSTATE_WAIT_DN;
            }
          }
        else
          {
          ret = COSP_ERR_TP4_RET;
          COSP_LOG_ERR1 ("COSP-ERROR: S-RELEASE.rsp+ acse_conn_id=" S_FMT_PTR ": T-DATA.req failed.",
            con);
          }
        }
      else
        {
        COSP_LOG_ERR1 ("COSP-ERROR: S-RELEASE.rsp+ acse_conn_id=" S_FMT_PTR ": Encoding DISCONNECT SPDU failed.",
          con);
        }
      }
    else
      {
      ret = COSP_ERR_INV_CON_STATE;
      COSP_LOG_ERR1 ("COSP-ERROR: S-RELEASE.rsp+ acse_conn_id=" S_FMT_PTR ": Connection Release Collision\n"
                     "            (DN not received or not a connection initiator).",
        con);
      }
    }
  else
    {
    ret = COSP_ERR_INV_CON_STATE;
    COSP_LOG_ERR2 ("COSP-ERROR: S-RELEASE.rsp+ acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
      con, con->cosp_state);
    }

  return (ret);
}


	/*======================================================*/
	/*							*/
	/*  C O N N E C T I O N   R E L E A S E   (CALLING SIDE)*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			cosp_rel_req					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to release a Session connection.		*/
/* SS-user should implement the u_cosp_rel_cnf_pos function to receive	*/
/* confirm for this request.  Negative confirm is not implemented.	*/
/* If this function returns value other then SD_SUCCESS then the function	*/
/* u_cosp_rel_cnf_pos will not be called.				*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_rel_req (ACSE_CONN *con)
{
ST_INT		ret;
ST_UINT	spdu_len;
char           *spdu_ptr;

  COSP_LOG_ENC3 ("S-RELEASE.req:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
  COSP_LOG_ENCH (con->ppdu_len, con->ppdu_ptr);

  ret = SD_SUCCESS;

  if (con->cosp_state == COSP_CSTATE_WAIT_REL_RSP ||
      con->cosp_state == COSP_CSTATE_DATA_XFER)
    {
    /* connection is beeing released, encode and send FINISH SPDU */
    ret = cosp_enc_fn_dn (con, &spdu_ptr, &spdu_len, COSP_SI_FINISH);
    if (ret == SD_SUCCESS)
      {
      if (tp4_data (con->tp4_conn_id, SD_TRUE, spdu_len, spdu_ptr) == SD_SUCCESS)
        {
        if (con->cosp_state == COSP_CSTATE_WAIT_REL_RSP)
          {
          /* collision occurred, request to release con while waiting	*/
          /* for resp to S-RELEASE.ind					*/
          con->cosp_vcoll = SD_TRUE;
          /* remain in the same state	*/
          }
        else
          con->cosp_state = COSP_CSTATE_WAIT_DN;
        }
      else
        {
        ret = COSP_ERR_TP4_RET;
        COSP_LOG_ERR1 ("COSP-ERROR: S-RELEASE.req acse_conn_id=" S_FMT_PTR ": T-DATA.req failed.",
          con);
        }
      }
    else
      {
      COSP_LOG_ERR1 ("COSP-ERROR: S-RELEASE.req acse_conn_id=" S_FMT_PTR ": Encoding FINISH SPDU failed.",
        con);
      }
    }
  else
    {
    ret = COSP_ERR_INV_CON_STATE;
    COSP_LOG_ERR2 ("COSP-ERROR: S-RELEASE.req acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
      con, con->cosp_state);
    }

  return (ret);
}


/************************************************************************/
/*			tp4_disconnect_ind				*/
/*----------------------------------------------------------------------*/
/* This function is called by the TP4-provider to indicate that a	*/
/* transport connection is beeing released.				*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID	 *user_conn_id	User's connection id for this connect.	*/
/*  ST_INT	  reason	Reason for disconnect			*/
/*  ST_INT	  conndata_len	Length of opt connect data <= 32 bytes	*/
/*  char	 *conndata	Pointer to opt connect data.		*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID tp4_disconnect_ind (ST_VOID *user_conn_id, ST_INT reason,
                         ST_INT conndata_len, char *conndata)
{
ACSE_CONN       *con;

  tp4_reldata (conndata);		/* !!! we ignoring conndata	*/

  con = (ACSE_CONN *) user_conn_id;

  switch (con->cosp_state)
    {
    case COSP_CSTATE_IDLE:
      /* ignore this tp4_disconnect_ind if in tp4_connect (returns SD_FAILURE) */
      if (!cosp_in_tp4_func)
        {
        COSP_LOG_ERR2 ("COSP-ERROR: T-DISCONNECT.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
          con, con->cosp_state);
        }
    break;

    case COSP_CSTATE_IDLE_TCON:
      /* disconnected before a CN has been received (COPP has no	*/
      /* knowledge about this connection at this point).		*/
      acse_free_con (con);
    break;

    case COSP_CSTATE_WAIT_TCON_CNF:
    case COSP_CSTATE_WAIT_AC:
    case COSP_CSTATE_WAIT_DN:
    case COSP_CSTATE_WAIT_CON_RSP:
    case COSP_CSTATE_WAIT_REL_RSP:
    case COSP_CSTATE_DATA_XFER:

      /* issue S-P-ABORT.ind to local SS-user (use code from cosp_usr.h)*/
      COSP_LOG_DEC4 ("S-P-ABORT.ind:   acse_conn_id =" S_FMT_PTR 
                     "  user_conn_id =" S_FMT_PTR "  reason =%d (TP4 reason=0x%02x)",
        con, con->user_conn_id, (ST_INT) COSP_P_AB_IND_TP_DISCON, reason);

      u_cosp_p_abort_ind (con, COSP_P_AB_IND_TP_DISCON);
      acse_free_con (con);
    break;

    case COSP_CSTATE_WAIT_TDISCON_IND:
      tp4_session_timer (con->tp4_conn_id, SD_FALSE);	/* stop timer	*/
      acse_free_con (con);
    break;

    default:
      COSP_LOG_ERR2 ("COSP-ERROR: T-DISCONNECT.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
        con, con->cosp_state);
    }
}


/************************************************************************/
/*                      tp4_session_timer_expired			*/
/*----------------------------------------------------------------------*/
/* This function is called by TP4 when the session_timer expire.	*/
/*	  								*/
/* Parameters:								*/
/*  ST_VOID	*user_conn_id	COSP connection id for this connect.	*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID tp4_session_timer_expired (ST_VOID *user_conn_id)
{
ACSE_CONN       *con;

  con = (ACSE_CONN *) user_conn_id;

  if (con->cosp_state == COSP_CSTATE_WAIT_TDISCON_IND)
    {
    tp4_disconnect (con->tp4_conn_id, 0, NULL);
    acse_free_con (con);
    }
  else
    {
    COSP_LOG_ERR2 ("COSP-ERROR: S-TIMER.expired acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
      con, con->cosp_state);
    }
}


	/*======================================================*/
	/*							*/
	/*	A B O R T    F U N C T I O N S			*/
	/*							*/
	/*======================================================*/


/************************************************************************/
/*			cosp_u_abort_req				*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to Abort a connection (S-U-ABORT)		*/
/*	  								*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/* CRITICAL: this function may free the ACSE_CONN struct so the struct	*/
/*	MUST NOT be accessed after calling this function.		*/
/************************************************************************/
ST_RET cosp_u_abort_req (ACSE_CONN *con)
{
ST_INT	ret;

  COSP_LOG_ENC3 ("S-U-ABORT.req:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %d", con, con->user_conn_id, con->ppdu_len);
  COSP_LOG_ENCH (con->ppdu_len, con->ppdu_ptr);

  ret = SD_SUCCESS;

  switch (con->cosp_state)
    {
    case COSP_CSTATE_WAIT_TCON_CNF:
      tp4_disconnect (con->tp4_conn_id, 0, NULL);
      acse_free_con (con);		/* done with this connection	*/
    break;

    case COSP_CSTATE_WAIT_AC:
    case COSP_CSTATE_WAIT_DN:
    case COSP_CSTATE_WAIT_CON_RSP:
    case COSP_CSTATE_WAIT_REL_RSP:
    case COSP_CSTATE_DATA_XFER:
      /* send ABORT SPDU and wait for T-DISCONNECT.ind	*/
      if ((ret = cosp_send_abort (con, 0)) == SD_SUCCESS)
        {
        con->cosp_state = COSP_CSTATE_WAIT_TDISCON_IND;
        tp4_session_timer (con->tp4_conn_id, SD_TRUE);	/* start timer	*/
	}
    break;

    default:
      ret = COSP_ERR_INV_CON_STATE;
      COSP_LOG_ERR2 ("COSP-ERROR: S-U-ABORT.req acse_conn_id=" S_FMT_PTR ": Invalid state=%d.",
        con, con->cosp_state);
     }

  if (ret != SD_SUCCESS)
    {
    /* disconnect if we couldn't send abort */
    tp4_disconnect (con->tp4_conn_id, 0, NULL);
    acse_free_con (con);
    }

  /* we always will return success */
  return (SD_SUCCESS);
}


/************************************************************************/
/*			cosp_send_abort					*/
/*----------------------------------------------------------------------*/
/* Function to sends ABORT SPDU to remote SS-user.			*/
/*	  								*/
/* Parameters:								*/
/*  ACSE_CONN       	*con		Pointer to conn info struct	*/
/*  ST_INT		err_code	Error code (defined by COSP)	*/
/*					If  =0 then S-U-ABORT sent	*/
/*					If !=0 then S-P-ABORT sent	*/
/* Return:								*/
/*	SD_SUCCESS (0)		if abort successful			*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET cosp_send_abort (ACSE_CONN *con, ST_INT err_code)
{
ST_RET   ret;
char      spdu_buf [COSP_P_AB_SPDU_LEN];
ST_UINT  spdu_len;
char	 *spdu_ptr;

  if (err_code != 0)
    {
    /* encode S-P-ABORT (error code will be encoded and we supply encoding buffer) */
    spdu_len = sizeof (spdu_buf);
    spdu_ptr = spdu_buf;
    if ((ret = cosp_enc_p_ab (spdu_ptr, err_code)) == SD_SUCCESS)
      {
      if (tp4_data (con->tp4_conn_id, SD_TRUE, spdu_len, spdu_ptr) != SD_SUCCESS)
        {
        ret = COSP_ERR_TP4_RET;
        COSP_LOG_ERR1 ("COSP-ERROR: S-P-ABORT acse_conn_id=" S_FMT_PTR ": T-DATA.req failed.",
          con);
        }
      }
    else
      {
      COSP_LOG_ERR1 ("COSP-ERROR: S-P-ABORT acse_conn_id=" S_FMT_PTR ": Encoding of P-ABORT SPDU failed.",
        con);
      }
    }
  else
    {
    /* encode S-U-ABORT (user data should be already encoded) */
    if ((ret = cosp_enc_u_ab (con, &spdu_ptr, &spdu_len)) == SD_SUCCESS)
      {
      if (tp4_data (con->tp4_conn_id, SD_TRUE, spdu_len, spdu_ptr) != SD_SUCCESS)
        {
        ret = COSP_ERR_TP4_RET;
        COSP_LOG_ERR1 ("COSP-ERROR: S-U-ABORT.req acse_conn_id=" S_FMT_PTR ": T-DATA.req failed.",
          con);
        }
      }
    else
      {
      COSP_LOG_ERR1 ("COSP-ERROR: S-U-ABORT.req acse_conn_id=" S_FMT_PTR ": Encoding of U-ABORT SPDU failed.",
        con);
      }
    }

  return (ret);
}


	/*======================================================*/
	/*							*/
	/*	D A T A    F U N C T I O N S			*/
	/*							*/
	/*======================================================*/

/************************************************************************/
/*			tp4_data_ind					*/
/*----------------------------------------------------------------------*/
/* Function to process data indication.					*/
/* Note that the tp4_data_ind is called when complete SPDU has been	*/
/* received, so the eot flag is always SD_TRUE.				*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID	*user_conn_id	User's connection id for this connect.	*/
/*  ST_INT	 eot            Indicates if last part of SPDU received.*/
/*  ST_UINT	 data_len	Length of data.				*/
/*  char	*data		Pointer to data buffer.			*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID tp4_data_ind (ST_VOID *user_conn_id, ST_INT eot,
		   ST_UINT data_len, char *data)	
{
ST_INT		ret;
ST_UINT 	action;
ACSE_CONN       *con;

  ret    = SD_SUCCESS;
  action = 0;
  con    = (ACSE_CONN *) user_conn_id;

  /* Note: the protocol requires following steps to be taken for invalid*/
  /*	   incomming-event/state intersection (default: in switch) or	*/
  /*	   if the incomming SPDU can't be correctly process:		*/
  /*		- issue S-P-ABORT					*/
  /*		- send an ABORT SPDU					*/
  /*		- start timer						*/
  /*		- wait for T-DISCONNECT or ABORT ACCEPT SPDU		*/

  switch (data [0])	/* First byte is SPDU Session Identifier	*/
    {

    case COSP_SI_CONNECT:
      switch (con->cosp_state)
        {
        case COSP_CSTATE_IDLE_TCON:
          /* if we are the transport connection acceptor, process CN	*/
          if (con->cosp_vtca)
            ret = cosp_process_connect (con, data_len, data);
          else
            action = COSP_T_DISCONNECT;
        break;

        case COSP_CSTATE_WAIT_TDISCON_IND:
          tp4_session_timer (con->tp4_conn_id, SD_FALSE);	/* stop timer	*/
          action = COSP_T_DISCONNECT;
        break;

        default:				/* in invalid state	*/
          ret = COSP_ERR_INV_CON_STATE;
          COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for CONNECT SPDU).",
            con, con->cosp_state);
        }
    break;

    case COSP_SI_FINISH:
      switch (con->cosp_state)
        {
        case COSP_CSTATE_IDLE_TCON:
          action = COSP_T_DISCONNECT;
        break;

        case COSP_CSTATE_WAIT_DN:	/* collision of conn releases	*/
          con->cosp_vcoll = SD_TRUE;
          /* go to next case */
        case COSP_CSTATE_DATA_XFER:
          ret = cosp_process_finish (con, data_len, data);
        break;

        case COSP_CSTATE_WAIT_TDISCON_IND:
          /* we ignore the FINISH SPDU and remain in the same state */
        break;

        default:				/* in invalid state	*/
          ret = COSP_ERR_INV_CON_STATE;
          COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for FINISH SPDU).",
            con, con->cosp_state);
        }
    break;

    case COSP_SI_ACCEPT:
      switch (con->cosp_state)
        {
        case COSP_CSTATE_IDLE_TCON:
          action = COSP_T_DISCONNECT;
        break;

        case COSP_CSTATE_WAIT_AC:
          con->cosp_vcoll = SD_FALSE;
          con->cosp_vdnr  = SD_FALSE;
          ret = cosp_process_accept (con, data_len, data);
        break;

        case COSP_CSTATE_WAIT_TDISCON_IND:
          /* we ignore the ACCEPT SPDU and remain in the same state */
        break;

        default:				/* in invalid state	*/
          ret = COSP_ERR_INV_CON_STATE;
          COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for ACCEPT SPDU).",
            con, con->cosp_state);
        }
    break;

    case COSP_SI_REFUSE:
      switch (con->cosp_state)
        {
        case COSP_CSTATE_IDLE_TCON:
          action = COSP_T_DISCONNECT;
        break;

        case COSP_CSTATE_WAIT_AC:
          if ((ret = cosp_process_refuse (con, data_len, data)) == SD_SUCCESS)
            action = COSP_T_DISCONNECT;
        break;

        case COSP_CSTATE_WAIT_TDISCON_IND:
          /* we ignore the REFUSE SPDU and remain in the same state */
        break;

        default:				/* in invalid state	*/
          ret = COSP_ERR_INV_CON_STATE;
          COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for REFUSE SPDU).",
            con, con->cosp_state);
        }
    break;

    case COSP_SI_DISCON:
      switch (con->cosp_state)
        {
        case COSP_CSTATE_IDLE_TCON:
          action = COSP_T_DISCONNECT;
        break;

        case COSP_CSTATE_WAIT_DN:
          if ((ret = cosp_process_discon (con, data_len, data)) == SD_SUCCESS)
            action = COSP_T_DISCONNECT;
        break;

        case COSP_CSTATE_WAIT_REL_RSP:		/* collision */
          if (con->cosp_vcoll && con->cosp_vtca)
            {
            con->cosp_vdnr = SD_TRUE;
            ret = cosp_process_discon (con, data_len, data);
            /* remain in the same state */
            }
          else
            {
            ret = COSP_ERR_INV_CON_STATE;
            COSP_LOG_ERR1 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Collision flag not set or not a connection acceptor.", 
              con);
            }
        break;

        case COSP_CSTATE_WAIT_TDISCON_IND:
          /* we ignore the DN SPDU and remain in the same state */
        break;

        default:				/* in invalid state	*/
          ret = COSP_ERR_INV_CON_STATE;
          COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for DISCONNECT SPDU).",
            con, con->cosp_state);
        }
    break;

    case COSP_SI_ABORT:		/* !always! disconnect transport	*/
    				/* otherwise we may loop sending ABORTs	*/
      action = COSP_T_DISCONNECT;

      switch (con->cosp_state)
        {
        case COSP_CSTATE_IDLE_TCON:
        break;

        case COSP_CSTATE_WAIT_AC:
        case COSP_CSTATE_WAIT_DN:
        case COSP_CSTATE_WAIT_CON_RSP:
        case COSP_CSTATE_WAIT_REL_RSP:
        case COSP_CSTATE_DATA_XFER:
          if ((ret = cosp_process_abort (con, data_len, data)) != SD_SUCCESS)
            {
            /* even if we failed to decode abort we still will generate S-P-ABORT */
            action |= COSP_S_P_ABORT_IND;
            }
        break;

        case COSP_CSTATE_WAIT_TDISCON_IND:
          tp4_session_timer (con->tp4_conn_id, SD_FALSE);	/* stop timer	*/
        break;

        default:				/* in invalid state	*/
          ret = COSP_ERR_INV_CON_STATE;
          COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for ABORT SPDU).",
            con, con->cosp_state);
          action |= COSP_S_P_ABORT_IND;
        }
    break;

    case COSP_SI_AB_ACCEPT:
      /* this SPDU may be sent by remote COSP engine even if the	*/
      /* transport connection is not to be kept				*/

      action = COSP_T_DISCONNECT;  /* !always! disconnect transport 	*/

      switch (con->cosp_state)
        {
        case COSP_CSTATE_IDLE_TCON:
        break;

        case COSP_CSTATE_WAIT_TDISCON_IND:
          tp4_session_timer (con->tp4_conn_id, SD_FALSE);	/* stop timer	*/
        break;

        default:				/* in invalid state	*/
          ret = COSP_ERR_INV_CON_STATE;
          COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for ABORT ACCEPT SPDU).",
            con, con->cosp_state);
	  /* ABORT ACCEPT can be received only in response to already	*/
	  /* sent ABORT, so we will not send another one here		*/
        }
    break;

/*  case COSP_SI_GIVE_TOKEN:	!!! GIVE-TOKEN is concatinated with DATA*/
    case COSP_SI_DATA:		/*  SPDU and both have the same code=1	*/
      switch (con->cosp_state)
        {
        case COSP_CSTATE_IDLE_TCON:
          action = COSP_T_DISCONNECT;
        break;

        case COSP_CSTATE_WAIT_DN:
          if (con->cosp_vcoll)
            {
            ret = COSP_ERR_INV_CON_STATE;
            COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for DATA SPDU).",
              con, con->cosp_state);
            break;
            }
          /* process the received DATA SPDU and remain in the same state */
          /* go to next case						*/
        case COSP_CSTATE_DATA_XFER:
          ret = cosp_process_data (con, data_len, data);
        break;

        case COSP_CSTATE_WAIT_TDISCON_IND:
          /* we ignore the DATA SPDU and remain in the same state */
        break;

        default:				/* in invalid state	*/
          ret = COSP_ERR_INV_CON_STATE;
          COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Invalid state=%d (for DATA SPDU).",
            con, con->cosp_state);
        }
    break;

    default:
      ret = COSP_ERR_DEC_INV_SPDU;
      COSP_LOG_ERR2 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Unsupported SPDU SI code=%d received.",
        con, (ST_INT)(data [0]));
      action = COSP_S_P_ABORT_IND | COSP_SEND_ABORT_SPDU;
    } /* end of switch */

  tp4_reldata (data);		/* we are done with the TP4 data buffer	*/

  if (ret != SD_SUCCESS)
    {
    /* if error occurred and SPDU other then an ABORT or ABORT ACCEPT	*/
    /* received then issue S-P-ABORT and SEND an ABORT SPDU.		*/

    if ((data [0] != COSP_SI_ABORT) && (data [0] != COSP_SI_AB_ACCEPT))
      action = COSP_S_P_ABORT_IND | COSP_SEND_ABORT_SPDU;
    }

  /*--------------------------------------------------------------------*/
  /* abort/disconnect if processing of received SPDU failed or this is	*/
  /* the protocol specification						*/
  /*--------------------------------------------------------------------*/

  if (action & COSP_SEND_ABORT_SPDU)
    {
    /* encode and send ABORT SPDU to remote SS-user, ret is the error code */
    if (cosp_send_abort (con, ret) == SD_SUCCESS)
      {
      con->cosp_state = COSP_CSTATE_WAIT_TDISCON_IND;
      tp4_session_timer (con->tp4_conn_id, SD_TRUE);	/* start timer	*/
      }
    else
      {
      con->cosp_state = COSP_CSTATE_IDLE;
      action |= COSP_T_DISCONNECT;	/* disconnect if we can't abort */
      }
    }

  if (action & COSP_S_P_ABORT_IND)
    {
    /* issue S-P-ABORT.ind to local SS-user (use code from cosp_usr.h)	*/

    COSP_LOG_DEC3 ("S-P-ABORT.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "  reason =%d",
      con, con->user_conn_id, (ST_INT) COSP_P_AB_IND_PROT_ERR);

    u_cosp_p_abort_ind (con, COSP_P_AB_IND_PROT_ERR);
    }

  if (action & COSP_T_DISCONNECT)
    {
    tp4_disconnect (con->tp4_conn_id, 0, NULL);
    acse_free_con (con);	/* we are disconnected, free con struct */
    }
}


/************************************************************************/
/*			tp4_expdata_ind					*/
/*----------------------------------------------------------------------*/
/* Function to process a expedited S-DATA.ind.				*/
/* 									*/
/* Parameters:								*/
/*  ST_VOID     *user_conn_id	User's connection id for this connect.	*/
/*  ST_UINT	 data_len	Length of data.				*/
/*  char	*data		Pointer to data buffer.			*/
/*	  								*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID tp4_expdata_ind (ST_VOID *user_conn_id, ST_UINT data_len, char *data)	
{
  /* Treat like normal data.  Assume "eot" = 1 */
  tp4_data_ind (user_conn_id, 1, data_len, data);
}


/************************************************************************/
/*			cosp_data_req					*/
/*----------------------------------------------------------------------*/
/* Function called by SS-user to transfer normal data on a previously	*/
/* established connection.						*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Pointer to connection info		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_data_req (ACSE_CONN *con)
{
ST_INT		ret;
ST_UINT	spdu_len;
char           *spdu_ptr;

  COSP_LOG_ENC3 ("S-DATA.req:      acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                 "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
  COSP_LOG_ENCH (con->ppdu_len, con->ppdu_ptr);

  ret = SD_SUCCESS;

  if ((con->cosp_state == COSP_CSTATE_DATA_XFER) ||
      (con->cosp_state == COSP_CSTATE_WAIT_REL_RSP && !con->cosp_vcoll))
    {
    if ((ret = cosp_enc_dt (con, &spdu_ptr, &spdu_len)) == SD_SUCCESS)
      {
      if (tp4_data (con->tp4_conn_id, SD_TRUE, spdu_len, spdu_ptr) != SD_SUCCESS)
        {
        ret = COSP_ERR_TP4_RET;
        COSP_LOG_ERR1 ("COSP-ERROR: S-DATA.req acse_conn_id=" S_FMT_PTR ": T-DATA.req failed.",
          con);
        }
      }
    else
      {
      COSP_LOG_ERR1 ("COSP-ERROR: S-DATA.req acse_conn_id=" S_FMT_PTR ": Encoding of DATA SPDU failed.",
        con);
      }
    }
  else
    {
    ret = COSP_ERR_INV_CON_STATE;
    COSP_LOG_ERR2 ("COSP-ERROR: S-DATA.req acse_conn_id=" S_FMT_PTR ": Invalid state=%d (or collision flag set).",
      con, con->cosp_state);
    }

  return (ret);
}


	/*======================================================*/
	/*							*/
	/*  L O C A L   S P D U   P R O C E S S I N G   F U N C	*/
	/*							*/
	/*======================================================*/


#if !defined(SNAP_LITE)		/* cosp_process_connect for SNAP_LITE	*/
				/* is in another file.			*/
/************************************************************************/
/*			cosp_process_connect				*/
/*----------------------------------------------------------------------*/
/* This function will decode the CONNECT SPDU and call SS-user's funct	*/
/* u_cosp_con_ind to indicate to the SS-user incomming connection req.	*/
/*									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*  ST_UINT	 spdu_len	Length of SPDU				*/
/*  char	*spdu_buf	Pointer to SPDU buffer			*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_process_connect (ACSE_CONN *con, ST_UINT spdu_len,
				     char *spdu_buf)
{
ST_INT		ret;
COSP_CN_AC	dec_par;		/* decoded connection params	*/

  /*----------------------------*/
  /* decode the CONNECT SPDU	*/
  /*----------------------------*/

  ret = cosp_dec_cn_ac (&dec_par, spdu_buf, spdu_len, COSP_SI_CONNECT);
  if (ret == SD_SUCCESS)
    {
    /* check if CONNECT SPDU is addressed to us */
    if (memcmp (dec_par.loc_ssel, cosp_only_ssel, cosp_only_ssel [0]+1)!=0)
      {
      COSP_LOG_ERR1 ("COSP-ERROR: CN.ind: Invalid Called SSEL (len=%d).",
        (ST_INT) dec_par.loc_ssel [0]);
      COSP_LOG_ERRH((ST_INT) dec_par.loc_ssel [0], &dec_par.loc_ssel [1]);
      return (COSP_ERR_DEC_INV_LOC_SSEL);
      }

    /* Save SSELs in ACSE_CONN struct.					*/
    memcpy (con->loc_ssel, dec_par.loc_ssel, dec_par.loc_ssel [0]+1);
    memcpy (con->rem_ssel, dec_par.rem_ssel, dec_par.rem_ssel [0]+1);

    /* generate S-CONNECT.ind to the SS-user */

    con->cosp_state = COSP_CSTATE_WAIT_CON_RSP;
    con->ppdu_len = dec_par.udata_len;
    con->ppdu_ptr = dec_par.udata_ptr;

    /* con->user_conn_id == NULL (invalid) now. Set later by 	*/
    /* "a_associate_rsp". OK because only used after that.	*/

    COSP_LOG_DEC1 ("S-CONNECT.ind:   acse_conn_id =" S_FMT_PTR, con);
    COSP_LOG_DECC1 ("User data length: %u", con->ppdu_len);
    COSP_LOG_DECH (con->ppdu_len, con->ppdu_ptr);

    /* Pass saved bind_id up to COSP user (i.e. COPP).	*/
    u_cosp_con_ind (con->cosp_bind_id, con);
    }
  else
    {
    COSP_LOG_ERR1 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Decoding of CONNECT SPDU failed.",
      con);
    }

  return (ret);
}
#endif	/* !defined(SNAP_LITE)	*/


/************************************************************************/
/*			cosp_process_finish				*/
/*----------------------------------------------------------------------*/
/* This function will decode the FINISH SPDU and call SS-user's function*/
/* u_cosp_rel_ind to indicate to the SS-user that the remote wants to	*/
/* release connection.							*/
/*									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*  ST_UINT	 spdu_len	Length of SPDU				*/
/*  char	*spdu_buf	Pointer to SPDU buffer			*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET cosp_process_finish (ACSE_CONN *con, ST_UINT spdu_len,
     				    char *spdu_buf)
{
ST_INT		ret;
COSP_FN_DN	dec_par;		/* decoded con rel params	*/

  /*------------------------------------*/
  /* decode and check the FINISH SPDU	*/
  /*------------------------------------*/

  ret = cosp_dec_fn_dn (&dec_par, spdu_buf, spdu_len, COSP_SI_FINISH);
  if (ret == SD_SUCCESS)
    {
    /* generate S-RELEASE.ind */

    con->cosp_state = COSP_CSTATE_WAIT_REL_RSP;
    con->ppdu_len = dec_par.udata_len;
    con->ppdu_ptr = dec_par.udata_ptr;

    COSP_LOG_DEC3 ("S-RELEASE.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                   "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
    COSP_LOG_DECH (con->ppdu_len, con->ppdu_ptr);

    u_cosp_rel_ind (con);
    }
  else
    {
    COSP_LOG_ERR1 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Decoding of FINISH SPDU failed.",
      con);
    }

  return (ret);
}


/************************************************************************/
/*			cosp_process_accept				*/
/*----------------------------------------------------------------------*/
/* This function will decode the ACCEPT SPDU and call SS-user's funct	*/
/* u_cosp_con_cnf_pos to indicate to the SS-user that the connection has*/
/* been established.							*/
/*									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*  ST_UINT	 spdu_len	Length of SPDU				*/
/*  char	*spdu_buf	Pointer to SPDU buffer			*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET cosp_process_accept (ACSE_CONN *con, ST_UINT spdu_len,
				    char *spdu_buf)
{
ST_INT		ret;
COSP_CN_AC	dec_par;		/* decoded accept params	*/

  /*----------------------------------*/
  /* decode and check the ACCEPT SPDU */
  /*----------------------------------*/

  ret = cosp_dec_cn_ac (&dec_par, spdu_buf, spdu_len, COSP_SI_ACCEPT);
  if (ret == SD_SUCCESS)
    {
    /* generate S-CONNECT.cnf+ */

    con->cosp_state = COSP_CSTATE_DATA_XFER;
    con->ppdu_len = dec_par.udata_len;
    con->ppdu_ptr = dec_par.udata_ptr;

    COSP_LOG_DEC3 ("S-CONNECT.cnf+:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                   "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
    COSP_LOG_DECH (con->ppdu_len, con->ppdu_ptr);

    u_cosp_con_cnf_pos (con);
    }
  else
    {
    COSP_LOG_ERR1 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Decoding of ACCEPT SPDU failed.",
      con);
    }

  return (ret);
}


/************************************************************************/
/*			cosp_process_refuse				*/
/*----------------------------------------------------------------------*/
/* This function will decode the REFUSE SPDU and call SS-user's function*/
/* u_cosp_con_cnf_neg to indicate to the SS-user that the establishment	*/
/* of connection failed.						*/
/* Note that this implementation will not reuse the transport connection*/
/* for another session connect (transport will be disconnected).	*/
/*									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*  ST_UINT	 spdu_len	Length of SPDU				*/
/*  char	*spdu_buf	Pointer to SPDU buffer			*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET cosp_process_refuse (ACSE_CONN *con, ST_UINT spdu_len,
     				    char *spdu_buf)
{
ST_INT	ret;
COSP_RF	dec_par;		/* decoded refuse connection params	*/
ST_INT result;
ST_INT reason;

  /*----------------------------------*/
  /* decode and check the REFUSE SPDU */
  /*----------------------------------*/

  ret = cosp_dec_rf (&dec_par, spdu_buf, spdu_len);
  if (ret == SD_SUCCESS)
    {
    con->cosp_state = COSP_CSTATE_IDLE;

    /* generate S-CONNECT.cnf- */

    if (dec_par.reason <= 2)
      result = COSP_CON_CNF_U_REJECT; 		/* remote SS-user reject */
    else
      result = COSP_CON_CNF_P_REJECT;		/* remote SS-provider reject */

    /* convert the reason code from SPDU to reason codes in cosp_usr.h */
    switch ((ST_UINT) dec_par.reason)
      {
      case 0:		/* rejection by called SS-user??, reason not specified	*/
        reason = COSP_CON_CNF_U_NOT_SPECIFIED;
      break;

      case 1:		/* rejection by called SS-user, tmp congestion	*/
        reason = COSP_CON_CNF_U_CONGESTION;
      break;

      case 2:		/* rejection by called SS-user, user data may follow */
        reason = COSP_CON_CNF_U_IN_UDATA;
        con->ppdu_len = dec_par.udata_len;
        con->ppdu_ptr = dec_par.udata_ptr;
      break;

      case 128+1:	/* rejection by called SS-prov, SSEL unknown	*/
        reason = COSP_CON_CNF_P_INV_SSEL;
      break;

      case 128+2:	/* rejection by called SS-prov, SS-user not attached */
        reason = COSP_CON_CNF_P_NOT_ATTACHED;
      break;

      case 128+3:	/* rejection by called SS-prov, congestion	*/
        reason = COSP_CON_CNF_P_CONGESTION;
      break;

      case 128+4:	/* rejection by called SS-prov, prot version not supported */
      case 128+5:	/* rejection by called SS-prov, reason not specified */
      case 128+6:	/* rejection by called SS-prov, implementation restriction stated in PICS */
      default:
        reason = COSP_CON_CNF_P_NOT_SPECIFIED;
      } /* end of switch */

    COSP_LOG_DEC4 ("S-CONNECT.cnf-:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "  result =%d  reason =%d",
      con, con->user_conn_id, result, reason);
    COSP_LOG_DECC1 ("User data length: %u", con->ppdu_len);
    COSP_LOG_DECH (con->ppdu_len, con->ppdu_ptr);

    /* S-CONNECT.cnf- */
    u_cosp_con_cnf_neg (con, result, reason);
    }
  else
    {
    COSP_LOG_ERR1 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Decoding of REFUSE SPDU failed.",
      con);
    }

  return (ret);
}


/************************************************************************/
/*			cosp_process_discon				*/
/*----------------------------------------------------------------------*/
/* This function will decode the DN SPDU and call SS-user's function	*/
/* u_cosp_rel_cnf_pos to indicate to the SS-user the result of session	*/
/* release request.							*/
/*									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*  ST_UINT	 spdu_len	Length of SPDU				*/
/*  char	*spdu_buf	Pointer to SPDU buffer			*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET cosp_process_discon (ACSE_CONN *con, ST_UINT spdu_len,
     				    char *spdu_buf)
{
ST_INT		ret;
COSP_FN_DN	dec_par;		/* decoded con rel params	*/

  /*------------------------------------*/
  /* decode and check the DN SPDU	*/
  /*------------------------------------*/

  ret = cosp_dec_fn_dn (&dec_par, spdu_buf, spdu_len, COSP_SI_DISCON);
  if (ret == SD_SUCCESS)
    {
    /* do not change here the state, new state depends on previous	*/
    /* state and if collision of S-RELEASEs occurred.			*/

    /* generate S-RELEASE.cnf+ */

    con->ppdu_len = dec_par.udata_len;
    con->ppdu_ptr = dec_par.udata_ptr;

    COSP_LOG_DEC3 ("S-RELEASE.cnf+:  acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                   "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
    COSP_LOG_DECH (con->ppdu_len, con->ppdu_ptr);

    u_cosp_rel_cnf_pos (con);	/* currently only positive cnf possible	*/
    }
  else
    {
    COSP_LOG_ERR1 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Decoding of DISCONNECT SPDU failed.",
      con);
    }

  return (ret);
}


/************************************************************************/
/*			cosp_process_abort				*/
/*----------------------------------------------------------------------*/
/* This function will decode the ABORT SPDU and call SS-user's function	*/
/* u_cosp_abort_ind to indicate to the SS-user that the session		*/
/* connection has been aborted by remote.				*/
/* Note that this implementation will not reuse the transport connection*/
/* for another session connect (transport will be disconnected).	*/
/*									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*  ST_UINT	 spdu_len	Length of SPDU				*/
/*  char	*spdu_buf	Pointer to SPDU buffer			*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET cosp_process_abort (ACSE_CONN *con, ST_UINT spdu_len,
			           char *spdu_buf)
{
ST_INT	ret;
COSP_AB	dec_par;		/* decoded abort connection params	*/
ST_INT reason;
ST_INT err_code;

  /*------------------------------------*/
  /* decode and check the ABORT SPDU	*/
  /*------------------------------------*/

  ret = cosp_dec_ab (&dec_par, spdu_buf, spdu_len);
  if (ret == SD_SUCCESS)
    {
    con->cosp_state = COSP_CSTATE_IDLE;

    /* generate S-U-ABORT.ind or S-P-ABORT.ind */

    if (dec_par.reason & COSP_AB_REASON_USER_ABORT)
      {
      /* remote SS-user abort */
      con->ppdu_len = dec_par.udata_len;
      con->ppdu_ptr = dec_par.udata_ptr;

      COSP_LOG_DEC3 ("S-U-ABORT.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                     "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
      COSP_LOG_DECH (con->ppdu_len, con->ppdu_ptr);

      u_cosp_u_abort_ind (con);
      }
    else
      {
      /* SS-provider abort */
      if (dec_par.reason & COSP_AB_REASON_PROT_ERROR)
        {
        reason = COSP_P_AB_IND_PROT_ERR;
        err_code = 0;

        /* Log the ABORT error code */
        if (dec_par.reflect_par_len == 2)
          err_code = ((ST_UINT16) dec_par.reflect_par [0] << 8)
                    + (ST_UINT16) dec_par.reflect_par [1];

        COSP_LOG_DEC4 ("S-P-ABORT.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "  reason =%d  err_code =0x%04X",
          con, con->user_conn_id, reason, err_code);
        }
      else
        {
        reason = COSP_P_AB_IND_UNDEFINED;

        COSP_LOG_DEC3 ("S-P-ABORT.ind:   acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "  reason =%d",
          con, con->user_conn_id, reason);
        }

      u_cosp_p_abort_ind (con, reason);
      }
    }
  else
    {
    COSP_LOG_ERR1 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Decoding of ABORT SPDU failed.",
      con);
    }

  return (ret);
}


/************************************************************************/
/*			cosp_process_data				*/
/*----------------------------------------------------------------------*/
/* This function will decode a DATA SPDU and generate a S-DATA.ind.	*/
/*									*/
/* Parameters:								*/
/*  ACSE_CONN    *con		Pointer to con info struct		*/
/*  ST_UINT	 spdu_len	Length of SPDU				*/
/*  char	*spdu_buf	Pointer to SPDU buffer			*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET cosp_process_data (ACSE_CONN *con, ST_UINT spdu_len,
				  char *spdu_buf)
{
ST_INT	ret;
COSP_DT	dec_par;		/* decoded refuse connection params	*/

  /*------------------------------------*/
  /* decode and check the DATA SPDU	*/
  /*------------------------------------*/
  ret = cosp_dec_dt (&dec_par, spdu_buf, spdu_len);
  if (ret == SD_SUCCESS)
    {
    /* generate S-DATA.ind */
    con->ppdu_len = dec_par.udata_len;
    con->ppdu_ptr = dec_par.udata_ptr;

    COSP_LOG_DEC3 ("S-DATA.ind:      acse_conn_id =" S_FMT_PTR "  user_conn_id =" S_FMT_PTR "\n"
                   "  User data length: %u", con, con->user_conn_id, con->ppdu_len);
    COSP_LOG_DECH (con->ppdu_len, con->ppdu_ptr);

    u_cosp_data_ind (con);
    }
  else
    {
    COSP_LOG_ERR1 ("COSP-ERROR: T-DATA.ind acse_conn_id=" S_FMT_PTR ": Decoding of DATA SPDU failed.",
      con);
    }

  return (ret);
}



