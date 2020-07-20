/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	1999-2002, All Rights Reserved.					*/
/*                                                                      */
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*                                                                      */
/* MODULE NAME : goose.c                                                */
/* PRODUCT(S)  : ASN1DE                                                 */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 12/12/03  JRB     09    u_a_unit_data_ind: memset GOOSE_INFO struct	*/
/*			   to all 0 at start of decode.			*/
/* 03/31/03  JRB     08    asn1r_get_bitstr: add max_bits arg.		*/
/* 12/11/02  JRB     07    Del mvl_acse.h include.			*/
/* 03/05/02  JRB     06    Del unused proto. Add braces when initializing*/
/*			   ASN1R_TAG_CTRL_1 structs.			*/
/* 01/10/02  JRB     05    No longer need bind_id.			*/
/*			   Chg args to _goose_decode_mms.		*/
/*                         Add gse_uca_decode				*/
/* 07/25/00  RKR     04    Reconciled changes from BSD			*/
/* 12/10/99  JRB     03    Fix mmsl_send_goose & _goose_tx_audt_apdu	*/
/*			   to properly check return codes.		*/
/* 09/10/99  JRB     02    Use asn1r_get_u32 for decoding of SqNum,     */
/*                         StNum, HoldTim, & BackTim (encode is OK).    */
/* 06/25/99  MDE     01    Created                                      */
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "clnp.h"	/* for clnpl_decode	*/
#include "goose.h"

/* For loopback testing use this define */
/* #define GOOSE_TEST  */

/************************************************************************/
/************************************************************************/
/* Static functions */

static ST_RET _goose_tx_audt_apdu (GOOSE_INFO *gi, ST_UCHAR *goose_mms_pdu,
                                      ST_INT goose_mms_pdu_len);

static ST_RET _goose_encode_mms (ST_UCHAR *asn1_buf, ST_INT asn1_buf_size,
                   ST_UCHAR **goose_asn1_out, ST_INT *goose_asn1_len_out,
                   GOOSE_INFO *gi);

static ST_RET _goose_decode_mms (AUDT_APDU *audt_apdu, GOOSE_INFO *gi);

static ST_VOID _goose_unconf_req (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_info_rpt (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_list_of_var (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_seq_of_varspec (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_named (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_vmd_spec (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_named_done (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_seq_of_varspec_done (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_list_of_var_done (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_list_acc_rslt (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_struct (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_app_id (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_sent_time (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_seqnum (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_sub_seq (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_hold_time (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_back_time (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_phsid (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_dnabits (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_get_usrbits (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_struct_done (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_list_acc_rslt_done (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_info_rpt_done (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_unconf_req_done (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_done_ok (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_done_error (ASN1_DEC_CTXT *ac);
static ST_VOID _goose_error (ASN1_DEC_CTXT *ac, ST_RET err);

/************************************************************************/
/************************************************************************/
/*                      mmsl_send_goose                                 */
/************************************************************************/

ST_RET mmsl_send_goose (GOOSE_INFO *gi)
  {
ST_UCHAR asn1_buf[GOOSE_MAX_PDU_SIZE];
ST_UCHAR *goose_mms_pdu;
ST_INT    goose_mms_pdu_len;
ST_RET rc;
#ifdef GOOSE_TEST
GOOSE_INFO goose_rx_info;
#endif

/* Encode the MMS PDU, given the user GOOSE information */
  rc = _goose_encode_mms (asn1_buf, GOOSE_MAX_PDU_SIZE,
                      &goose_mms_pdu, &goose_mms_pdu_len, gi);

/* Finish the a-unit data encoding and transmit the PDU */
  if (rc == SD_SUCCESS)
  rc = _goose_tx_audt_apdu (gi, goose_mms_pdu, goose_mms_pdu_len);

#ifdef GOOSE_TEST
/* For test, pretend we received it too! */
  memset(&goose_rx_info, 0, sizeof (GOOSE_INFO));
  memcpy(&goose_rx_info.audtApdu, &gi->audtApdu, sizeof (AUDT_APDU));

  rc = _goose_decode_mms (&gi->audtApdu, &goose_rx_info);
  if (rc != 0)
    printf ("\n Whooops!");
  rc = memcmp (gi, &goose_rx_info, sizeof (GOOSE_INFO));
  if (rc != 0)
    printf ("\n Whoa!");
  else
    u_mmsl_goose_received (&goose_rx_info);
#endif

  return (rc);
  }

/************************************************************************/
/*                      _goose_encode_mms                               */
/************************************************************************/

static ST_RET _goose_encode_mms (ST_UCHAR *asn1_buf, ST_INT asn1_buf_size,
                   ST_UCHAR **goose_mms_pdu_out, ST_INT *goose_mms_pdu_len_out,
                   GOOSE_INFO *gi)
  {
ASN1_ENC_CTXT ac;
ST_UCHAR *pdu_start;     /* 000725 BSD - Changed from ST_CHAR* to ST_UCHAR* */
ST_INT pdu_len;

  asn1r_strt_asn1_bld (&ac, asn1_buf, asn1_buf_size);
  asn1r_strt_constr (&ac);         /* unconfirmed requests               */
  asn1r_strt_constr (&ac);         /* information report body            */
  asn1r_strt_constr (&ac);         /* access result                      */
  asn1r_strt_constr (&ac);         /* GOOSE data is a structure          */

  asn1r_wr_bitstr (&ac, gi->UserSt, gi->num_usr_bits);
  asn1r_fin_prim (&ac, 4, CTX);
  asn1r_wr_bitstr (&ac, gi->DNA, gi->num_dna_bits);
  asn1r_fin_prim (&ac, 4, CTX);
  asn1r_wr_u16 (&ac, gi->PhsID);
  asn1r_fin_prim (&ac, 6, CTX);
  asn1r_wr_u32 (&ac, gi->BackTim);
  asn1r_fin_prim (&ac, 6, CTX);
  asn1r_wr_u32 (&ac, gi->HoldTim);
  asn1r_fin_prim (&ac, 6, CTX);
  asn1r_wr_u32 (&ac, gi->StNum);
  asn1r_fin_prim (&ac, 6, CTX);
  asn1r_wr_u32 (&ac, gi->SqNum);
  asn1r_fin_prim (&ac, 6, CTX);
  asn1r_wr_btod (&ac, &gi->t);
  asn1r_fin_prim (&ac, 12, CTX);
  asn1r_wr_vstr (&ac, gi->SendingIED);  
  asn1r_fin_prim (&ac, 10, CTX);
  asn1r_fin_constr (&ac, 2, CTX, DEF);  /* end of GOOSE data struct     */
  asn1r_fin_constr (&ac, 0, CTX, DEF);     /* access result being finished */

  asn1r_strt_constr (&ac);
  asn1r_strt_constr (&ac);                 /* variable access specification        */
  asn1r_strt_constr (&ac);                 /* object name start                   */
  asn1r_wr_vstr (&ac, "GOOSE");
  asn1r_fin_prim (&ac, 0, CTX);              /* VMD Specific Name                    */
  asn1r_fin_constr (&ac, 0, CTX, DEF);
  asn1r_fin_constr (&ac, SEQ_CODE, UNI, DEF);/* end variable access specification */
  asn1r_fin_constr (&ac, 0, CTX, DEF);       /* end of information report body       */
  asn1r_fin_constr (&ac, 0, CTX, DEF);
  asn1r_fin_constr (&ac, 3, CTX, DEF);       /* end of unconfirmed service           */

  if (ac.asn1r_encode_overrun)          /* Check for encode overrun     */
    return (SD_FAILURE);

  pdu_start = ac.asn1r_field_ptr + 1;
  pdu_len = (int) ((asn1_buf + asn1_buf_size) - pdu_start);

  *goose_mms_pdu_out = pdu_start;
  *goose_mms_pdu_len_out = pdu_len;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                      _goose_tx_audt_apdu                             */
/************************************************************************/

static ST_RET _goose_tx_audt_apdu (GOOSE_INFO *gi, ST_UCHAR *goose_mms_pdu,
                                      ST_INT goose_mms_pdu_len)
  {
ST_RET ret;
  gi->audtApdu.user_info.ptr = goose_mms_pdu;
  gi->audtApdu.user_info.len = goose_mms_pdu_len;
  ret = a_unit_data_req (&gi->audtApdu);
  return (ret);
  }


/************************************************************************/
/************************************************************************/
/*                      u_a_unit_data_ind                               */
/* This "user" function processes the A-UNIT-DATA.ind. It is customized */
/* to handle only GOOSE packets. It decodes the GOOSE packet, and then  */
/* passes the decoded info to the user by calling "u_mmsl_goose_received.*/
/* If the packet is not a GOOSE, or it can't be decoded, it is ignored. */
/************************************************************************/
ST_VOID u_a_unit_data_ind (AUDT_APDU *audt_apdu)
  {
ST_RET rc;
GOOSE_INFO goose_info;  /* for u_a_unit_data_ind                        */

  /* Start with clean structure.	*/
  memset (&goose_info, 0, sizeof (goose_info));

  /* Decode GOOSE PDU.		*/
  rc = _goose_decode_mms (audt_apdu, &goose_info);

/* Tell the user about the received GOOSE message */
  if (rc == SD_SUCCESS)
    u_mmsl_goose_received (&goose_info);
  else
    {
 /*   SLOGALWAYS0 ("A-UNIT-DATA.ind is not a GOOSE. Ignoring it."); */
    }
  }

/************************************************************************/
/*                      _goose_decode_mms                               */
/************************************************************************/


static ST_RET _goose_decode_mms (AUDT_APDU *audt_apdu, GOOSE_INFO *gi)
  {
ASN1_DEC_CTXT asn1_ctxt;
ASN1_DEC_CTXT *ac;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX | CONSTR, 3), _goose_unconf_req}}};

  /* Copy AUDT struct to goose_info so user can access all addressing info.*/
  memcpy (&gi->audtApdu, audt_apdu, sizeof (AUDT_APDU));

  ac = &asn1_ctxt;
  memset (ac, 0, sizeof (ASN1_DEC_CTXT));
  ac->usr_info[0] = gi;
  ac->asn1r_err_fun = _goose_error;
  ac->asn1r_decode_done_fun = _goose_done_error;
  ac->asn1r_decode_method = ASN1_TABLE_METHOD;
  ac->asn1r_tag_table = &asn1r_tags;
  asn1r_decode_asn1 (ac, audt_apdu->user_info.ptr, audt_apdu->user_info.len);
  if (ac->asn1r_pdu_dec_err != NO_DECODE_ERR)
    return (SD_FAILURE);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*                      _goose_unconf_req                               */
/************************************************************************/

static ST_VOID _goose_unconf_req (ASN1_DEC_CTXT *ac)
  {
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL(CTX | CONSTR, 0), _goose_info_rpt}}};

  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_info_rpt                                 */
/************************************************************************/

static ST_VOID _goose_info_rpt (ASN1_DEC_CTXT *ac)
  {
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX | CONSTR, 0), _goose_list_of_var}}};

  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_list_of_var                              */
/************************************************************************/

static ST_VOID _goose_list_of_var (ASN1_DEC_CTXT *ac)
  {
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (UNI | CONSTR, SEQ_CODE), _goose_seq_of_varspec}}};

  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_seq_of_varspec                           */
/************************************************************************/

static ST_VOID _goose_seq_of_varspec (ASN1_DEC_CTXT *ac)
  {
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX | CONSTR, 0), _goose_named}}};

  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_named                                    */
/************************************************************************/

static ST_VOID _goose_named (ASN1_DEC_CTXT *ac)
  {
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 0), _goose_vmd_spec}}};

  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_vmd_spec                                 */
/************************************************************************/

static ST_VOID _goose_vmd_spec (ASN1_DEC_CTXT *ac)
  {
ST_CHAR *data_ptr;

  data_ptr = (ST_CHAR *) ac->asn1r_field_ptr-1;
  asn1r_get_vstr (ac, data_ptr);
  if (strcmp (data_ptr, "GOOSE") != 0)
    asn1r_set_dec_err (ac, GOOSE_NAME_MISMATCH);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = _goose_named_done;
  }

/************************************************************************/
/*                      _goose_named_done                               */
/************************************************************************/

static ST_VOID _goose_named_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = _goose_seq_of_varspec_done;
  }

/************************************************************************/
/*                      _goose_seq_of_varspec_done                      */
/************************************************************************/

static ST_VOID _goose_seq_of_varspec_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = _goose_list_of_var_done;
  }

/************************************************************************/
/*                      _goose_list_of_var_done                         */
/************************************************************************/

static ST_VOID _goose_list_of_var_done (ASN1_DEC_CTXT *ac)
  {
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX | CONSTR, 0), _goose_list_acc_rslt}}};

  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_list_acc_rslt                            */
/************************************************************************/

static ST_VOID _goose_list_acc_rslt (ASN1_DEC_CTXT *ac)
  {
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX | CONSTR, 2), _goose_struct}}};

  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_struct                                   */
/************************************************************************/

static ST_VOID _goose_struct (ASN1_DEC_CTXT *ac)
  {
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 10), _goose_get_app_id}}};

  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_app_id                               */
/************************************************************************/

static ST_VOID _goose_get_app_id (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 12), _goose_get_sent_time}}};

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];
                        
  asn1r_get_vstr (ac, _goose_dec_info->SendingIED);
  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_sent_time                            */
/************************************************************************/

static ST_VOID _goose_get_sent_time (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 6), _goose_get_seqnum}}};

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];
  asn1r_get_btod (ac, &_goose_dec_info->t);
  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_seqnum                               */
/************************************************************************/

static ST_VOID _goose_get_seqnum (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 6), _goose_get_sub_seq}}};

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];
  asn1r_get_u32 (ac, &_goose_dec_info->SqNum);
  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_sub_seq                              */
/************************************************************************/

static ST_VOID _goose_get_sub_seq (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 6), _goose_get_hold_time}}};

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];
  asn1r_get_u32 (ac, &_goose_dec_info->StNum);
  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_hold_time                            */
/************************************************************************/

static ST_VOID _goose_get_hold_time (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 6), _goose_get_back_time}}};

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];
  asn1r_get_u32 (ac, &_goose_dec_info->HoldTim);
  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_back_time                            */
/************************************************************************/

static ST_VOID _goose_get_back_time (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 6), _goose_get_phsid}}};

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];
  asn1r_get_u32 (ac, &_goose_dec_info->BackTim);
  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_phsid                                */
/************************************************************************/

static ST_VOID _goose_get_phsid (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 4), _goose_get_dnabits}}};

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];
  asn1r_get_u16 (ac, &_goose_dec_info->PhsID);
  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_dnabits                              */
/************************************************************************/

static ST_VOID _goose_get_dnabits (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;
static ASN1R_TAG_CTRL_1 asn1r_tags =
  {1, {{ASN1R_TAG_VAL (CTX, 4), _goose_get_usrbits}}};

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];
  asn1r_get_bitstr (ac, _goose_dec_info->DNA, GOOSE_MAX_NUM_DNA_BITS);
  _goose_dec_info->num_dna_bits = ac->asn1r_bitcount;
  ac->asn1r_tag_table = &asn1r_tags;
  }

/************************************************************************/
/*                      _goose_get_usrbits                              */
/************************************************************************/

static ST_VOID _goose_get_usrbits (ASN1_DEC_CTXT *ac)
  {
GOOSE_INFO *_goose_dec_info;

  _goose_dec_info = (GOOSE_INFO *) ac->usr_info[0];

  asn1r_get_bitstr (ac, _goose_dec_info->UserSt, GOOSE_MAX_NUM_USR_BITS);
  _goose_dec_info->num_usr_bits = ac->asn1r_bitcount;
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = _goose_struct_done;
  }

/************************************************************************/
/*                      _goose_struct_done                              */
/************************************************************************/

static ST_VOID _goose_struct_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = _goose_list_acc_rslt_done;
  }

/************************************************************************/
/*                      _goose_list_acc_rslt_done                       */
/************************************************************************/

static ST_VOID _goose_list_acc_rslt_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = _goose_info_rpt_done;
  }

/************************************************************************/
/*                      _goose_info_rpt_done                            */
/************************************************************************/

static ST_VOID _goose_info_rpt_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = _goose_unconf_req_done;
  }

/************************************************************************/
/*                      _goose_unconf_req_done                          */
/************************************************************************/

static ST_VOID _goose_unconf_req_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_decode_done_fun = _goose_done_ok;
  ac->asn1r_decode_done = SD_TRUE;
  }

/************************************************************************/
/*                      _goose_done_ok                                  */
/************************************************************************/

static ST_VOID _goose_done_ok (ASN1_DEC_CTXT *ac)
  {
  }

/************************************************************************/
/*                      _goose_done_error                                       */
/************************************************************************/

static ST_VOID _goose_done_error (ASN1_DEC_CTXT *ac)
  {
  asn1r_set_dec_err (ac, GOOSE_DONE_TOO_SOON);
  }

/************************************************************************/
/*                      _goose_error                                    */
/************************************************************************/

static ST_VOID _goose_error (ASN1_DEC_CTXT *ac, ST_RET err)
  {
  }

/************************************************************************/
/*                      gse_uca_decode					*/
/* This function decodes the subnet packet (sn_udt), and puts the	*/
/* results in the user structure (goose_info).				*/
/* First it decodes from SN_UNITDATA to N_UNITDATA.			*/
/* Then it decodes from N_UNITDATA to AUDT_APDU.			*/
/* Then it decodes from AUDT_APDU to GOOSE_INFO.			*/
/************************************************************************/
ST_RET gse_uca_decode (SN_UNITDATA *sn_udt,	/* input subnet packet	*/
		GOOSE_INFO *goose_info)		/* output GOOSE data	*/
  {
ST_RET rc;
N_UNITDATA *pNudt = NULL;	/* intermediate decode result	*/
AUDT_APDU audt;			/* intermediate decode result	*/

  /* Decode from SN_UNITDATA to N_UNITDATA.	*/
  /* clnpl_decode allocs a N_UNITDATA structure if this is a N-Unitdata	*/
  /* packet, and changes the value of pNudt to point to it.		*/
  if ((rc = clnpl_decode (sn_udt, &pNudt)) != SD_SUCCESS)
    return (rc);
  if (pNudt==NULL)
    return (SD_FAILURE);	/* does this ever happen?		*/

  /* Decode from N_UNITDATA to AUDT_APDU.	*/
  if ((rc = cltp_decode_nsdu_2 (pNudt, &audt)) != SD_SUCCESS)
    return (rc);
  /* CRITICAL: DON't free N_UNITDATA here, because AUDT_APDU has pointer into it.	*/

  /* Decode from AUDT_APDU to GOOSE_INFO.	*/
  rc = _goose_decode_mms (&audt, goose_info);

  /* Now it is safe to free N_UNITDATA. All data in GOOSE_INFO now.	*/
  clnp_free (pNudt);
  return (rc);
  }


