/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsinit.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the common functions to decode and encode	*/
/*	an initiate operation.	Note that the initiate request and the	*/
/*	initiate response are the same. 				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 03/31/03  JRB     06    Support MMS Version2.			*/
/*			   asn1r_get_bitstr: add max_bits arg.		*/
/*			   If received bitstr too long, truncate.	*/
/*			   If received bitstr too short, set missing bits=0*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 12/10/97  MDE     02    Chaned max_nest to ST_INT8			*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pcon.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#define MMS_V0_PARAM_SUPP_BITS	7	/* DIS	*/
#define MMS_V1_PARAM_SUPP_BITS	11
#define MMS_V1_SERV_SUPP_BITS	85
/************************************************************************/

static  INIT_INFO	*info;

#ifdef CS_SUPPORT
static CS_INIT_INFO *cs_info;
static ST_UCHAR *init_detail_start;
#endif

static ST_BOOLEAN  request;	/* req/resp flag			*/
static ST_RET val_err;	   	/* asn1_set_dec_err code for bad value	*/


/************************************************************************/
/* static functions in this module					*/
static ST_VOID get_maxseg (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_maxreq_calling (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_maxreq_called (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_max_nest (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_detail_seq (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_version (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_param_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_param_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_param_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_services_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_services_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_services_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID mms_detail_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID init_seq_done (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			_mms_init_dec					*/
/* This function is called from mmsdec.c when an initiate (req or resp)	*/
/* is being decoded. We are in the InitiatePDU sequence.		*/
/* Allocate an operation specific structure and set up to get either	*/
/* the segment size or the max outstanding calling			*/
/************************************************************************/

ST_VOID _mms_init_dec (ASN1_DEC_CTXT *aCtx, ST_BOOLEAN req)
  {
  request = req;			/* save the request flag	*/
  if (request)
    val_err = REQ_BAD_VALUE;
  else
    val_err = RESP_BAD_VALUE;

/* Create an init_info structure data area to put the decoded info into.*/


  /* CRITICAL: "info" struct must be calloc'd, so if received bitstrings
   *           are too short, the missing bits=0.
   */
#ifdef CS_SUPPORT
  if (_mmsdec_ctxt == MMS_PCI)
    info = (INIT_INFO *) _m_get_dec_buf (aCtx, sizeof (INIT_INFO));
  else
    cs_info = (CS_INIT_INFO *) _m_get_dec_buf (aCtx, sizeof (CS_INIT_INFO));
#else
    info = (INIT_INFO *) _m_get_dec_buf (aCtx, sizeof (INIT_INFO));
#endif

  ASN1R_TAG_ADD (aCtx, CTX,0,get_maxseg);			/* max seg size (opt)	*/
  ASN1R_TAG_ADD (aCtx, CTX,1,get_maxreq_calling);		/* max outstdg calling	*/
  }


/************************************************************************/
/*			get_maxseg					*/
/* The max_msgsegsize parameter has been encountered.  Read its value.	*/
/************************************************************************/

static ST_VOID get_maxseg (ASN1_DEC_CTXT *aCtx)
  {
ST_INT32 local_detail;

  MLOG_CDEC0 ("get_maxseg");

  if (asn1r_get_i32 (aCtx, &local_detail) || local_detail < 0)
    {
    if (request)
      asn1r_set_dec_err (aCtx, REQ_INVAL_ARG);
    else
      asn1r_set_dec_err (aCtx, RESP_INVAL_RSLT);
    }
  else
    {
#ifdef CS_SUPPORT
    if (_mmsdec_ctxt == MMS_PCI)
      {
      info->max_segsize_pres = SD_TRUE;  	/* set present flag for segsize	*/
      info->max_segsize = local_detail;
      }
    else
      {
      cs_info->local_detail_pres = SD_TRUE;  	/* set present flag	*/
      cs_info->local_detail = local_detail;
      }
#else
    info->max_segsize_pres = SD_TRUE;  	/* set present flag for segsize	*/
    info->max_segsize = local_detail;
#endif
    ASN1R_TAG_ADD (aCtx, CTX,1,get_maxreq_calling);
    }   
  }

/************************************************************************/
/*			get_maxreq_calling				*/
/* proposed/negiotiated max outstanding calling encountered		*/
/************************************************************************/

static ST_VOID get_maxreq_calling (ASN1_DEC_CTXT *aCtx)
  {
ST_INT16 maxreq_calling;

  MLOG_CDEC0 ("get_maxreq_calling");

/* Get the value of the maxreq_calling parameter.			*/

  if (asn1r_get_i16 (aCtx, &maxreq_calling))
    asn1r_set_dec_err (aCtx, val_err);

#ifdef CS_SUPPORT
  if (_mmsdec_ctxt == MMS_PCI)
    info->maxreq_calling = maxreq_calling;
  else
    cs_info->maxreq_calling = maxreq_calling;
#else
  info->maxreq_calling = maxreq_calling;
#endif

/* The next data element is the maxreq_called parameter.		*/

  ASN1R_TAG_ADD (aCtx, CTX,2,get_maxreq_called);
  }

/************************************************************************/
/*			get_maxreq_called				*/
/* proposed/negiotiated max outstanding called encountered		*/
/************************************************************************/

static ST_VOID get_maxreq_called (ASN1_DEC_CTXT *aCtx)
  {
ST_INT16 maxreq_called;

  MLOG_CDEC0 ("get_maxreq_called");

/* Get the value of the maxreq_called parameter.				*/

  if (asn1r_get_i16 (aCtx, &maxreq_called))
    asn1r_set_dec_err (aCtx, val_err);

#ifdef CS_SUPPORT
  if (_mmsdec_ctxt == MMS_PCI)
    info->maxreq_called = maxreq_called;
  else
    cs_info->maxreq_called = maxreq_called;
#else
  info->maxreq_called = maxreq_called;
#endif

/* The next data element is either the max_nest parameter (optional) or */
/* the initiate detail sequence	or the CS detail.			*/
/* All are optional, so we could be done				*/

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = init_seq_done;
  ASN1R_TAG_ADD (aCtx, CTX,3,get_max_nest);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,4,get_detail_seq);
  }

/************************************************************************/
/*			get_max_nest					*/
/* The max_nest parameter has been encountered.  Read its value.	*/
/************************************************************************/

static ST_VOID get_max_nest (ASN1_DEC_CTXT *aCtx)
  {
ST_INT8 max_nest;

  MLOG_CDEC0 ("get_max_nest");

/* Get the value of the max_nest parameter.				*/

  if (asn1r_get_i8 (aCtx, &max_nest))
    asn1r_set_dec_err (aCtx, val_err);
  else
    {
#ifdef CS_SUPPORT
    if (_mmsdec_ctxt == MMS_PCI)
      {
      info->max_nest_pres = SD_TRUE;
      info->max_nest = max_nest;
      }
    else
      {
      cs_info->max_nest_pres = SD_TRUE;
      cs_info->max_nest = max_nest;
		/* Initiate detail sequence may be next, or CS detail	*/
      init_detail_start = aCtx->asn1r_field_ptr;
      }
#else
    info->max_nest_pres = SD_TRUE;
    info->max_nest = max_nest;
#endif
    }

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,4,get_detail_seq);
  }

/************************************************************************/
/*			get_detail_seq					*/
/* The mms init detail sequence is now starting, first is version #	*/
/************************************************************************/

static ST_VOID get_detail_seq (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_detail_seq");

/* Check if decoding an MMS CORE Initiate or a CS Initiate		*/

#ifdef CS_SUPPORT
  if (_mmsdec_ctxt == MMS_PCI)
    {
    info->mms_detail_pres = SD_TRUE;	/* set detail present flag	*/
    ASN1R_TAG_ADD (aCtx, CTX,0,get_version);	/* Next comes version		*/
    }
  else		/* Companion Standard Initiate, accept sequence		*/
    asn1r_parse_cstr_contents (aCtx, mms_detail_cstr_done);
#else
  info->mms_detail_pres = SD_TRUE;		/* set detail present flag	*/
  ASN1R_TAG_ADD (aCtx, CTX,0,get_version);		/* Next comes version		*/
#endif
  }

/************************************************************************/
/*			get_version					*/
/* The mms init detail sequence is now starting, this is version #	*/
/************************************************************************/

static ST_VOID get_version (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_version");

/* Get the value of the max_nest parameter.				*/

  if (asn1r_get_i16 (aCtx, &info->version) || info->version <0)
    asn1r_set_dec_err (aCtx, val_err);

/* Next is parameter support options, a 6 bit bitstring (prim or cstr)	*/

  ASN1R_TAG_ADD (aCtx, CTX,1,get_param_prim);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,get_param_cstr);
  }


/************************************************************************/
/*			get_param_prim					*/
/* The parameter support BS has been encountered as a primitive data	*/
/* element.  Read its value.						*/
/************************************************************************/

static ST_VOID get_param_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_param_prim");

  /* NOTE: decode only the bits we recognize.
   * For version0 (DIS) should get 7 bits.
   * For version1 (IS) should get 11 bits.
   * For version2 and higher, may get more than 11, but we ignore the 
   * higher bits because we don't support them.
   */
  if (!asn1r_get_bitstr (aCtx, info->param_supp, MMS_V1_PARAM_SUPP_BITS))
    get_param_done (aCtx);	  	/* set up for the next data element	*/
  else
    asn1r_set_dec_err (aCtx, val_err);
  }

/************************************************************************/
/*			get_param_cstr					*/
/* The parameter suport has been encountered as a constructor data	*/
/* element.  Read its value.						*/
/************************************************************************/

static ST_VOID get_param_cstr (ASN1_DEC_CTXT *aCtx)
  {

  MLOG_CDEC0 ("get_param_cstr");

/* Set up to call this function when done with constructor bitstring.	*/

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = get_param_done;

/* Get the value of the parameter support.  For constructor		*/
/* bitstrings, this requires returning and letting the ASN.1 tools	*/
/* parse it to any nesting level.					*/

  asn1r_get_bitstr_cstr (aCtx, MMS_V1_PARAM_SUPP_BITS,info->param_supp);
  }

/************************************************************************/
/*			get_param_done					*/
/* Function called after the supported parameter has been parsed.	*/
/************************************************************************/

static ST_VOID get_param_done (ASN1_DEC_CTXT *aCtx)
  {

  MLOG_CDEC0 ("get_param_done");

  ASN1R_TAG_ADD (aCtx, CTX,2,get_services_prim);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,get_services_cstr);
  }

/************************************************************************/
/*			get_services_prim				*/
/* The services support BS has been encountered as a primitive data	*/
/* element.  Read its value.						*/
/************************************************************************/

static ST_VOID get_services_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_services_prim");

  /* NOTE: decode only the bits we recognize.
   * For version0 (DIS) and version1 (IS) should get 85 bits.
   * For version2 and higher, may get more than 85, but we ignore the 
   * higher bits because we don't support them.
   */
  if (!asn1r_get_bitstr (aCtx, info->serv_supp, MMS_V1_SERV_SUPP_BITS))
    get_services_done (aCtx);	 /* set up for the next data element	*/
  else
    asn1r_set_dec_err (aCtx, val_err);
  }

/************************************************************************/
/*			get_services_cstr				*/
/* The service suport BS has been encountered as a constructor data	*/
/* element.  Read its value.						*/
/************************************************************************/

static ST_VOID get_services_cstr (ASN1_DEC_CTXT *aCtx)
  {

  MLOG_CDEC0 ("get_services_cstr");

/* Set up to call this function when done with constructor bitstring.	*/

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = get_services_done;

/* Get the value of the service support.  For constructor		*/
/* bitstrings, this requires returning and letting the ASN.1 tools	*/
/* parse it to any nesting level.					*/

  asn1r_get_bitstr_cstr (aCtx, MMS_V1_SERV_SUPP_BITS,info->serv_supp);
  }

/************************************************************************/
/*			get_services_done				*/
/* Function called when entire mms_detail constructor is done		*/
/************************************************************************/

static ST_VOID get_services_done (ASN1_DEC_CTXT *aCtx)
  {

  MLOG_CDEC0 ("get_services_done");

  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = mms_detail_cstr_done;
  }

/************************************************************************/
/*			mms_detail_cstr_done				*/
/* Function called when entire mms_detail constructor is done		*/
/************************************************************************/

static ST_VOID mms_detail_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("mms_detail_cstr_done");

#ifdef CS_SUPPORT
  if (_mmsdec_ctxt != MMS_PCI)
    {
    cs_info->init_detail_len = (ST_INT) (aCtx->asn1r_field_ptr - init_detail_start);
    cs_info->init_detail = init_detail_start;
    }
#endif
  }

/************************************************************************/
/*			init_seq_done					*/
/* The parse of the Initiate PDU is complete, either with or without	*/
/* CS detail. Just set decode done flag.				*/
/************************************************************************/

static ST_VOID init_seq_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("init_seq_done");

  aCtx->asn1r_decode_done = SD_TRUE;
  aCtx->asn1r_decode_done_fun = _mms_dec_done_ok;
  }


/************************************************************************/
/************************************************************************/
/*			_ms_mk_init 					*/
/* Function to construct an Initiate request or response PDU in the	*/
/* selected buffer.  Pass a pointer to the end of the build buffer, and */
/* a pointer to a structure of type init_info that contains the build	*/
/* control and data information.  Returns a pointer to the first byte	*/
/* of the encoded message.						*/
/************************************************************************/

ST_UCHAR *_ms_mk_init (ASN1_ENC_CTXT *aCtx, ST_UCHAR *buf_ptr, ST_INT buf_len, 
		INIT_INFO *info_ptr, ST_INT pdutype,
		ST_BOOLEAN cs_init)
  {
#ifdef DEBUG_SISCO
ST_UCHAR  *msg_start;			/* These variables used for	*/
ST_INT	msg_len;			/* debug print only.		*/
#endif
#ifdef CS_SUPPORT
CS_INIT_INFO *cs_init_ptr; 	   
#endif

  asn1r_strt_asn1_bld (aCtx, buf_ptr,buf_len);	/* Initialize the ASN.1 tools	*/

#ifdef CS_SUPPORT
  if (cs_init)		/* COMPANION STANDARD INITIATE PDU		*/
    {
    cs_init_ptr = (CS_INIT_INFO *) info_ptr;

/* write the CS init detail constructor					*/
    asn1r_strt_constr (aCtx);			/* start the CTX 5 cstr		*/  
    asn1r_wr_octstr (aCtx, cs_init_ptr->init_detail,cs_init_ptr->init_detail_len);
    asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* close the CS detail cstr	*/

/* maximum data nesting (optional)					*/
    if (cs_init_ptr->max_nest_pres)     	/* if desired to send - 	*/
      {
      asn1r_wr_i8 (aCtx, (ST_CHAR)cs_init_ptr->max_nest);
      asn1r_fin_prim (aCtx, 3,CTX);			/* context-specific, tag 2	*/
      }

/* max outstanding requests from called MMS-user			*/
    asn1r_wr_i16 (aCtx, cs_init_ptr->maxreq_called);
    asn1r_fin_prim (aCtx, 2,CTX);			/* context-specific, tag 2	*/

/* max outstanding requests from calling MMS-user			*/
    asn1r_wr_i16 (aCtx, cs_init_ptr->maxreq_calling);
    asn1r_fin_prim (aCtx, 1,CTX);			/* context-specific, tag 1	*/

/* max segment size (optional)						*/
    if (cs_init_ptr->local_detail_pres) 	/* if enabled		 	*/
      {
      asn1r_wr_i32 (aCtx, cs_init_ptr->local_detail);
      asn1r_fin_prim (aCtx, 0,CTX);			/* context-specific, tag 0	*/
      }
    }
  else		/* MMS CORE INITIATE PDU				*/
    {
#endif
/* Now begin writing primitive data elements from back of PDU to front. */
  
/* write the mms init detail constructor, if enabled			*/
    if (info_ptr->mms_detail_pres)	   /* if we have MMS detail	*/
      {
      asn1r_strt_constr (aCtx);
      asn1r_wr_bitstr (aCtx, info_ptr->serv_supp,MMS_V1_SERV_SUPP_BITS);  /* services supported BS	*/
      asn1r_fin_prim (aCtx, 2,CTX);			   /* context-specific, tag 2	*/
  
/* parameters supported BS, write either 7 (DIS) or 11 (IS) bits	*/
      if (info_ptr->version == 0)	/* If DIS is being proposed	*/
        {
        asn1r_wr_bitstr (aCtx, info_ptr->param_supp,MMS_V0_PARAM_SUPP_BITS);
        asn1r_fin_prim (aCtx, 1,CTX);			
        }
      else				/* IS is being proposed		*/
        {
        asn1r_wr_bitstr (aCtx, info_ptr->param_supp,MMS_V1_PARAM_SUPP_BITS);
        asn1r_fin_prim (aCtx, 1,CTX);		/* context-specific, tag 1	*/
        }
  
      asn1r_wr_i16 (aCtx, info_ptr->version);	/* version number		*/
      asn1r_fin_prim (aCtx, 0,CTX);			/* context-specific, tag 0	*/

      asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* close the MMS detail cstr	*/
      }    

/* Now write the core initiate parameters				*/

/* maximum data nesting (optional)					*/
    if (info_ptr->max_nest_pres)		/* if desired to send - 	*/
      {
      asn1r_wr_i8 (aCtx, (ST_CHAR)info_ptr->max_nest);
      asn1r_fin_prim (aCtx, 3,CTX);			/* context-specific, tag 2	*/
      }

/* max outstanding requests from called MMS-user			*/
    asn1r_wr_i16 (aCtx, info_ptr->maxreq_called);
    asn1r_fin_prim (aCtx, 2,CTX);			/* context-specific, tag 2	*/

/* max outstanding requests from calling MMS-user			*/
    asn1r_wr_i16 (aCtx, info_ptr->maxreq_calling);
    asn1r_fin_prim (aCtx, 1,CTX);			/* context-specific, tag 1	*/

/* max segment size (optional)						*/
    if (info_ptr->max_segsize_pres)		/* if enabled		 	*/
      {
      asn1r_wr_i32 (aCtx, info_ptr->max_segsize);
      asn1r_fin_prim (aCtx, 0,CTX);			/* context-specific, tag 0	*/
      }
#ifdef CS_SUPPORT
    }
#endif

/* Close the PDU.  Building of either the request or response message	*/
/* is complete. 							*/

#ifdef DEBUG_SISCO
  asn1r_fin_constr (aCtx, (ST_UINT16) pdutype,CTX,DEF);
  msg_start = aCtx->asn1r_field_ptr + 1;
  msg_len =  (int) ((buf_ptr + buf_len) - msg_start);

  MLOG_ENC2 ("INITIATE %s built, len = %d",
             pdutype == MMSINITREQ ? "request" : "response",
             msg_len);
  MLOG_ENCH (msg_len,msg_start);
  MLOG_PAUSEENC (NULL);

  return (msg_start);
#else
  asn1r_fin_constr (aCtx, (ST_UINT16) pdutype,CTX,DEF);
  return (aCtx->asn1r_field_ptr + 1);
#endif
  }
