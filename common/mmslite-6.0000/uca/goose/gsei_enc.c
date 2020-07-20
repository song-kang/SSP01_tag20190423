/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	2002-2002, All Rights Reserved.					*/
/*									*/
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*									*/
/* MODULE NAME : gsei_enc.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION :  IEC GOOSE encode functions.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*									*/
/* NOTE: The "data" and a few other parameters only need to be encoded	*/
/* once when a new GOOSE message is generated (see gse_iec_enc_part1),	*/
/* because they do not change with each retransmission. However, the	*/
/* last part of the header must be re-encoded each time because "sqNum"	*/
/* and other parameters change with each retransmission			*/
/* (see gse_iec_enc_part2).						*/
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 07/24/08  JRB     11    Fix len checks for gcRef, dataSetRef, appID.	*/
/* 09/11/07  JRB     10    gcRef, dataSetRef, appID changed from	*/
/*			   char ptr to char array. Don't alloc/free.	*/
/*			   Add gse_iec_enc_part1, gse_iec_enc_part2.	*/
/* 03/30/06  JRB     09    gse_iec_control_create alloc & copy strings	*/
/*			   so OK if caller frees strings (MUCH SAFER).	*/
/*			   gsi_iec_control_destroy free the strings.	*/
/* 03/20/06  JRB     08    Del gse_iec_send (use clnp_snet_write_raw).	*/
/* 01/04/05  JRB     07    Fix index check in gse_iec_data_init.	*/
/* 06/20/03  JRB     06    Fix first ASN1 constructor: APP 1 IMPLICIT SEQ.*/
/* 01/08/03  JRB     05    Del obsolete dataRef, elementId from structs	*/
/*			   & from gse_iec_data_init args.		*/
/*			   Use ASN1_ENC_* macros.			*/
/* 12/11/02  JRB     04    Del mvl_acse.h include.			*/
/* 12/03/02  ASK     03    Changes for new IEC Goose encodings. Changed	*/
/*			   gse_iec_encode() for Ethertype frames.	*/
/* 02/26/02  JRB     02    Chg asn1Data args to UCHAR. Fix // comments.	*/
/* 02/11/02  JRB     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "mem_chk.h"
#include "mmsdefs.h"		/* many basic MMS definitions	*/
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "clnp_llc.h"
#include "clnp.h"	/* for clnpl_decode	*/
#include "goose.h"
#include "acse2log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#include "ethertyp.h"

/************************************************************************/
/*			gse_iec_control_create				*/
/* NOTE: the "strlen" checks in this function should work even if	*/
/*       buffer sizes change.						*/
/* NOTE: The first 3 arguments correspond to GoCBRef, DatSet, and GoID	*/
/*       in the IEC 61850 standard (Edition 2).				*/
/************************************************************************/
GSE_IEC_CTRL *gse_iec_control_create (ST_CHAR *gcRef, ST_CHAR *dataSetRef, ST_CHAR *appID,
				ST_INT numDataEntry)
  {
GSE_IEC_CTRL *ctrl;

  if (strlen (gcRef) >= sizeof (ctrl->gcRef))
    {
    SLOGALWAYS0 ("Error creating GOOSE control: GoCBRef too long.");
    return (NULL);
    }
  if (strlen (dataSetRef) >= sizeof (ctrl->dataSetRef))
    {
    SLOGALWAYS0 ("Error creating GOOSE control: DatSet too long.");
    return (NULL);
    }
  if (appID != NULL && strlen (appID) >= sizeof (ctrl->appID))
    {
    SLOGALWAYS0 ("Error creating GOOSE control: GoID too long.");
    return (NULL);
    }

  if (numDataEntry <= 0)
    {
    SLOGALWAYS1 ("Error creating GOOSE control: numDataEntry=%d not allowed.", numDataEntry);
    return (NULL);
    }

  ctrl = (GSE_IEC_CTRL *) chk_calloc (1, sizeof (GSE_IEC_CTRL));

  strcpy (ctrl->gcRef, gcRef);
  strcpy (ctrl->dataSetRef, dataSetRef);

  if(appID != NULL)
    {
    ctrl->appID_pres = SD_TRUE;
    strcpy (ctrl->appID, appID);
    }
  else
    ctrl->appID_pres = SD_FALSE;

  ctrl->numDataEntries = numDataEntry;

  ctrl->dataEntries = (GSE_IEC_DATA_ENTRY *) chk_calloc (numDataEntry,sizeof (GSE_IEC_DATA_ENTRY));
  return (ctrl);
  }

/************************************************************************/
/*			gse_iec_control_destroy				*/
/************************************************************************/
ST_RET gse_iec_control_destroy (GSE_IEC_CTRL *ctrl)
  {
ST_INT index;
  if (ctrl == NULL)
    return (SD_FAILURE);	/* Invalid Control. Can't destroy it.	*/
  /* If dataBufs were allocated, free them.	*/
  for (index = 0; index < ctrl->numDataEntries; index++)
    {
    if (ctrl->dataEntries[index].dataBuf)
      chk_free (ctrl->dataEntries[index].dataBuf);
    }
  chk_free (ctrl->dataEntries);
  chk_free (ctrl);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			gse_iec_data_init				*/
/* WARNING: dataRef and runtimeTypeHead args must point to persistent	*/
/*	data. They are saved as pointers for later use.			*/
/************************************************************************/
ST_RET gse_iec_data_init (GSE_IEC_CTRL *ctrl,
			ST_INT index,
			struct runtime_type *runtimeTypeHead,
			ST_INT numRuntimeTypes)
  {
ST_RET rc;
  if (index < 0  || index >= ctrl->numDataEntries)
    rc = SD_FAILURE;
  else
    {
    ctrl->dataEntries[index].runtimeTypeHead = runtimeTypeHead;
    ctrl->dataEntries[index].numRuntimeTypes = numRuntimeTypes;
    /* Allocate buffer to store data.	*/
    ctrl->dataEntries[index].dataBuf = chk_malloc (runtimeTypeHead->offset_to_last);
    rc = SD_SUCCESS;
    }
  return (rc);
  }

/************************************************************************/
/*			gse_iec_data_update				*/
/************************************************************************/
ST_RET gse_iec_data_update (GSE_IEC_CTRL *ctrl,
			ST_INT index,
			ST_VOID *dataPtr)
  {
GSE_IEC_DATA_ENTRY *dataEntry;	/* ptr to info for this data entry.	*/
ST_RET rc;

  rc = SD_FAILURE;				/* assume failure	*/

  if (index >= 0  &&  index < ctrl->numDataEntries)
    {
    dataEntry = &ctrl->dataEntries[index];	/* point to this entry	*/
    memcpy (dataEntry->dataBuf, dataPtr, dataEntry->runtimeTypeHead->offset_to_last);
    rc = SD_SUCCESS;
    }

  return (rc);
  }

/************************************************************************/
/*			gse_iec_encode					*/
/* Encodes IEC GOOSE message including Ethertype header (14 bytes).  	*/
/************************************************************************/
ST_UCHAR *gse_iec_encode (GSE_IEC_CTRL *ctrl,
			  ST_UCHAR *asn1Buf,	 /* buffer to encode in	*/
			  ST_INT asn1BufLen,	 /* len of buffer	*/
			  ST_INT *asn1LenOut,	 /* ptr to len encoded	*/
			  ETYPE_INFO *etype_info)/* ptr to Ethertype info*/
  {
ST_RET retVal=SD_SUCCESS;	/* assume SUCCESS if numDataEntries=0	*/
ST_INT index;
GSE_IEC_DATA_ENTRY *dataEntry;	/* ptr to info for this data entry.	*/
ASN1_ENC_CTXT ac;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	*/
				/* init to NULL (i.e. failure)		*/

  asn1r_strt_asn1_bld (&ac, asn1Buf, asn1BufLen);

  asn1r_strt_constr (&ac);	/* Start APPLICATION 1 IMPLICIT SEQUENCE*/
  asn1r_strt_constr (&ac); 	/* Start allData	*/
  for (index = ctrl->numDataEntries -1; index >= 0; index--)
    {
    dataEntry = &ctrl->dataEntries[index];	/* point to this entry	*/
    /* Call encode function. Returns pointer to ASN.1 encoded data.	*/
    retVal = ms_local_to_asn1 (&ac,
             dataEntry->runtimeTypeHead,
             dataEntry->numRuntimeTypes,
	     dataEntry->dataBuf);
    /* NOTE: if successful, asn1Data points somewhere inside asn1DataBuf.*/
    if (retVal)
      {
      ACSELOG_ERR2 ("IEC GOOSE encode error 0x%X on data entry number %d",
                    retVal, index);
      break;	/* stop encoding	*/
      }
    }	/* end loop	*/

  if (retVal == SD_SUCCESS)	/* encoding OK so far?	*/
    {
    /* do rest of encoding	*/
    asn1r_fin_constr (&ac,11,CTX,DEF);	/* finish IMPLICIT SEQ	*/

    asn1r_wr_i32 (&ac, ctrl->numDataEntries);
    asn1r_fin_prim (&ac,10,CTX);

    asn1r_wr_bool (&ac, ctrl->needsCommissioning);
    asn1r_fin_prim (&ac,9,CTX);

    asn1r_wr_u32 (&ac, ctrl->confRev);
    asn1r_fin_prim (&ac,8,CTX);

    asn1r_wr_bool (&ac, ctrl->test);
    asn1r_fin_prim (&ac,7,CTX);

    asn1r_wr_u32 (&ac, ctrl->sqNum);
    asn1r_fin_prim (&ac,6,CTX);

    asn1r_wr_u32 (&ac, ctrl->stNum);
    asn1r_fin_prim (&ac,5,CTX);

    asn1r_wr_utc_time (&ac, &ctrl->utcTime);
    asn1r_fin_prim (&ac,4,CTX);

    if(ctrl->appID_pres)
      {
      asn1r_wr_vstr (&ac, ctrl->appID);
      asn1r_fin_prim (&ac,3,CTX);
      }

    asn1r_wr_vstr (&ac, ctrl->dataSetRef);
    asn1r_fin_prim (&ac,2,CTX);

    asn1r_wr_u32 (&ac, ctrl->timeToLive);
    asn1r_fin_prim (&ac,1,CTX);

    asn1r_wr_vstr (&ac, ctrl->gcRef);
    asn1r_fin_prim (&ac,0,CTX);

    asn1r_fin_constr (&ac,1,APP,DEF);	/* finish APPLICATION 1 IMPLICIT SEQ*/

    /* ASN.1 encoding done. Chk for overrun. */
    if (ac.asn1r_encode_overrun)
      {
      ACSELOG_ERR0 ("IEC GOOSE encode overrun");
      }
    else
      {
      *asn1LenOut = (int) ASN1_ENC_LEN(&ac);	/* len encoded	*/
      
      /* Finally, add the Ethertype information */
      /* This is the only place we set return to GOOD value.		*/
      asn1Start = etype_hdr_encode(ASN1_ENC_PTR(&ac),  /* buffer to encode into */
	       		           asn1BufLen,         /* len of buffer 	*/
	       		           asn1LenOut,         /* ptr to len encoded    */
	       		           etype_info);        /* ptr etype struct      */
      }
    }
  
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }

/************************************************************************/
/*			gse_iec_enc_part1				*/
/* Encode the data part of IEC GOOSE message and some constant elements,*/
/* up to the "sqNum". The "sqNum" and other elements change with each 	*/
/* retransmission, so they are encoded in "gse_iec_enc_part2".		*/
/* NOTE: The encode buffer is "ctrl->enc_buf". The len encoded by this	*/
/*       function is stored in "ctrl->enc_part1_len".			*/
/************************************************************************/
ST_UCHAR *gse_iec_enc_part1 (GSE_IEC_CTRL *ctrl,
	ST_UCHAR *buf_ptr,		/* buffer in which to encode	*/
	ST_INT buf_len,			/* buffer len			*/
	ST_INT *asn1_len_out)		/* len of ASN.1 encoded data	*/
  {
ST_RET retVal=SD_SUCCESS;	/* assume SUCCESS if numDataEntries=0	*/
ST_INT index;
GSE_IEC_DATA_ENTRY *dataEntry;	/* ptr to info for this data entry.	*/
ASN1_ENC_CTXT ac;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	*/
				/* init to NULL (i.e. failure)		*/

  asn1r_strt_asn1_bld (&ac, buf_ptr, buf_len);

  asn1r_strt_constr (&ac); 	/* Start allData	*/
  for (index = ctrl->numDataEntries -1; index >= 0; index--)
    {
    dataEntry = &ctrl->dataEntries[index];	/* point to this entry	*/
    /* Call encode function. Returns pointer to ASN.1 encoded data.	*/
    retVal = ms_local_to_asn1 (&ac,
             dataEntry->runtimeTypeHead,
             dataEntry->numRuntimeTypes,
             dataEntry->dataBuf);
    /* NOTE: if successful, asn1Data points somewhere inside asn1DataBuf.*/
    if (retVal)
      {
      ACSELOG_ERR2 ("IEC GOOSE encode error 0x%X on data entry number %d",
                    retVal, index);
      break;	/* stop encoding	*/
      }
    }	/* end loop	*/
  if (retVal == SD_SUCCESS)	/* encoding OK so far?	*/
    {
    /* Finish the constructor for the data.	*/
    asn1r_fin_constr (&ac,11,CTX,DEF);	/* finish allData	*/

    asn1r_wr_i32 (&ac, ctrl->numDataEntries);
    asn1r_fin_prim (&ac,10,CTX);

    asn1r_wr_bool (&ac, ctrl->needsCommissioning);
    asn1r_fin_prim (&ac,9,CTX);

    asn1r_wr_u32 (&ac, ctrl->confRev);
    asn1r_fin_prim (&ac,8,CTX);

    asn1r_wr_bool (&ac, ctrl->test);
    asn1r_fin_prim (&ac,7,CTX);

    /* The next element would be "sqNum". It changes with each retrans,	*/
    /* so "sqNum" and rest of PDU encoded in "gse_iec_enc_part2".	*/

    /* ASN.1 encoding done. Chk for overrun. */
    if (ac.asn1r_encode_overrun)
      {
      ACSELOG_ERR0 ("IEC GOOSE encode overrun while encoding data");
      }
    else
      {
      asn1Start = ASN1_ENC_PTR(&ac);
      *asn1_len_out = (int) ASN1_ENC_LEN(&ac);	/* len encoded	*/
      }
    }
  
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }

/************************************************************************/
/*			gse_iec_enc_part2				*/
/* Encode the second part of IEC GOOSE message including Ethertype	*/
/* header. This part of the encoding includes elements that change 	*/
/* with each retransmission. 						*/
/* This function is similar to "gse_iec_encode" but assumes data is	*/
/* already encoded in the ASN.1 buffer (see gse_iec_enc_part1).		*/
/* NOTE: The encode buffer is "ctrl->enc_buf". The len encoded by	*/
/*       "gse_iec_enc_part1" is stored in "ctrl->enc_part1_len".	*/
/************************************************************************/
ST_UCHAR *gse_iec_enc_part2 (GSE_IEC_CTRL *ctrl,
	ETYPE_INFO *etype_info,		/* ptr to Ethertype info	*/
	ST_UCHAR *buf_ptr,		/* buffer in which to encode	*/
	ST_INT buf_len,			/* buffer len			*/
	ST_INT enc_part1_len,		/* len of data encoded by	*/
					/* gse_iec_enc_part1		*/
					/* CRITICAL: must be < buf_len.	*/
	ST_INT *asn1_len_out)		/* len of ASN.1 encoded data	*/
  {
ST_RET retVal=SD_SUCCESS;	/* assume SUCCESS if numDataEntries=0	*/
ASN1_ENC_CTXT ac;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	*/
				/* init to NULL (i.e. failure)		*/

  asn1r_strt_asn1_bld (&ac, buf_ptr, buf_len);

  asn1r_strt_constr (&ac);	/* Start APPLICATION 1 IMPLICIT SEQUENCE*/
  /* WARNING: this is not typical ASN.1 encoding.			*/
  /* Data was encoded earlier by calling "gse_iec_enc_part1".		*/
  /* Adjust "asn1r_field_ptr" to point just before pre-encoded data.	*/
  /* This replaces encoding the "allData" part of GOOSE.		*/
  assert (enc_part1_len < buf_len);
  ac.asn1r_field_ptr -= enc_part1_len;
  ac.asn1r_field_end = ac.asn1r_field_ptr;	/* CRITICAL	*/

  if (retVal == SD_SUCCESS)	/* encoding OK so far?	*/
    {
    /* do rest of encoding	*/
    asn1r_wr_u32 (&ac, ctrl->sqNum);
    asn1r_fin_prim (&ac,6,CTX);

    asn1r_wr_u32 (&ac, ctrl->stNum);
    asn1r_fin_prim (&ac,5,CTX);

    asn1r_wr_utc_time (&ac, &ctrl->utcTime);
    asn1r_fin_prim (&ac,4,CTX);

    if(ctrl->appID_pres)
      {
      asn1r_wr_vstr (&ac, ctrl->appID);
      asn1r_fin_prim (&ac,3,CTX);
      }

    asn1r_wr_vstr (&ac, ctrl->dataSetRef);
    asn1r_fin_prim (&ac,2,CTX);

    asn1r_wr_u32 (&ac, ctrl->timeToLive);
    asn1r_fin_prim (&ac,1,CTX);

    asn1r_wr_vstr (&ac, ctrl->gcRef);
    asn1r_fin_prim (&ac,0,CTX);

    asn1r_fin_constr (&ac,1,APP,DEF);	/* finish APPLICATION 1 IMPLICIT SEQ*/

    /* ASN.1 encoding done. Chk for overrun. */
    if (ac.asn1r_encode_overrun)
      {
      ACSELOG_ERR0 ("IEC GOOSE encode overrun");
      }
    else
      {
      *asn1_len_out = (int) ASN1_ENC_LEN(&ac);	/* len encoded	*/
      
      /* Finally, add the Ethertype information */
      /* This is the only place we set return to GOOD value.		*/
      asn1Start = etype_hdr_encode(
                       ASN1_ENC_PTR(&ac),	/* encode hdr BEFORE this*/
                       buf_len,			/* len of encode buffer	*/
                       asn1_len_out,		/* ptr to len encoded	*/
                       etype_info);		/* ptr etype struct	*/
      }
    }
  
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }

