/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1996 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_jread.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_jread_ind						*/
/*	mvlas_jread_resp						*/
/*	mplas_jread_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/11/12  JRB           Do not reduce _bufCtrl.num* after calling	*/
/*			   u_mvl_start_jread (caused memory corruption).*/
/* 04/09/07  MDE     21    Enhanced filtered logging 			*/
/* 10/30/06  JRB     20    Use new mvl_vmd_* object handling functions.	*/
/* 01/30/06  GLB     19    Integrated porting changes for VMS           */
/* 03/28/05  JRB     18    Del suicacse header.				*/
/* 02/25/02  MDE     17    Now get max PDU size from mvl_cfg_info	*/
/* 09/21/01  JRB     16    Alloc global bufs only once at startup.	*/
/* 03/30/01  MDE     15    Changed MSMEM_ENC_INFO to MSMEM_ENC_OS_INFO	*/
/* 01/21/00  MDE     14    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     13    Added SD_CONST modifiers			*/
/* 08/30/99  JRB     12    DON'T log after mplas_err_resp (causes GPF).	*/
/* 01/15/99  JRB     11    Call mvl_find_jou to find journal.		*/
/* 11/16/98  MDE     10    Renamed internal functions (prefix '_')	*/
/* 10/09/98  JRB     09    Use modified MVL_JOURNAL_ENTRY.		*/
/* 09/21/98  MDE     08    Minor lint cleanup				*/
/* 09/10/98  JRB     07    Make sure moreFollows set correctly.		*/
/* 07/16/98  MDE     06    More journal work				*/
/* 07/15/98  MDE     05    Changed Journal Read user functions		*/
/* 06/15/98  MDE     04    Changes to allow compile under C++		*/
/* 05/14/98  JRB     03    Use MVL_IND_PEND. Add mvlas resp function.	*/
/* 05/14/98  JRB     02    Fixed uninitialized pointers, etc.		*/
/* 11/04/97  MDE     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mvl_acse.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/* Local variables and functions					*/

static MVL_JREAD_BUF_CTRL _bufCtrl;
static ST_UCHAR *_asn1BufStart;
static ST_INT _asn1BufLeft; 

static ST_RET mvlJe_to_je (JOURNAL_ENTRY *je, MVL_JOURNAL_ENTRY *mvlJe);

/************************************************************************/
/************************************************************************/
/*			_mvl_process_jread_ind				*/
/* A Read Journal indication has been rcved, process and respond	*/
/************************************************************************/

ST_VOID _mvl_process_jread_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.jread.req_info =
        (JREAD_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_jread_ind (indCtrl);
  }

/************************************************************************/
/*			mvlas_jread_resp				*/
/* This is the Virtual Machine response function for ReadJournal.	*/
/* It calls the following user functions to manage the Journal data:	*/
/*	u_mvl_start_jread						*/
/*	u_mvl_get_next_jread_entry					*/
/*	u_mvl_free_jread_entry						*/
/*	u_mvl_end_jread							*/
/************************************************************************/
ST_VOID mvlas_jread_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;
JREAD_REQ_INFO *reqInfo;
JREAD_RESP_INFO *respInfo;
JOURNAL_ENTRY *je;
ST_BOOLEAN moreFollows = SD_FALSE;	/* Would never get set if 0 entries*/
MVL_JOURNAL_CTRL *jCtrl;
MVL_JOURNAL_ENTRY mvlJe;
MVL_JOURNAL_ENTRY *mvlJep;
ST_VOID *usrHandle;
ST_UCHAR *asn1_buf;
ST_INT i;

  reqInfo = indCtrl->u.jread.req_info;

/* First find our Journal Control element */
  jCtrl = mvl_vmd_find_jou (&mvl_vmd, &reqInfo->jou_name, indCtrl->event->net_info);
  if (jCtrl == NULL)
    {
    MVL_LOG_NERR1 ("ReadJournal - could not find journal '%s'", 
				reqInfo->jou_name.obj_name.vmd_spec);
    _mplas_err_resp (indCtrl, 2, 1); 	/* object undefined */
    return;
    }  

/* Give the user the JREAD request information */
  memset (&_bufCtrl, 0, sizeof (MVL_JREAD_BUF_CTRL));
  rc = u_mvl_start_jread (jCtrl, reqInfo, &_bufCtrl, &usrHandle);
  if (rc != SD_SUCCESS)
    {
    _mplas_err_resp (indCtrl, 2, 1); 	/* object undefined */
    return;
    }  

/* Allocate the response information data buffer 			*/
  respInfo = 
    (JREAD_RESP_INFO *) M_CALLOC (MSMEM_ENC_OS_INFO, 1, sizeof (JREAD_RESP_INFO) + 
		 	(_bufCtrl.numUsrEntries * sizeof (JOURNAL_ENTRY)) +
                 	(_bufCtrl.numVarInfo * sizeof (VAR_INFO)) );

/* Set the storage buffer start locations				*/
#if defined (MVL_JREAD_ENTRYID_DYNAMIC)
  _bufCtrl.nextEntryId = _bufCtrl.entryIdBuf;
#endif
#if defined (MVL_JREAD_VARTAG_DYNAMIC)
  _bufCtrl.nextVarTag = _bufCtrl.varTagBuf;
#endif
#if defined (MVL_JREAD_ANNOTATION_DYNAMIC)
  _bufCtrl.nextAnnotation = _bufCtrl.annotationBuf;
#endif

/* Allocate the ASN.1 data encode buffer */
  asn1_buf = (ST_UCHAR *) M_MALLOC (MSMEM_ASN1_DATA_ENC, mvl_cfg_info->max_msg_size);
  _asn1BufStart = asn1_buf;
  _asn1BufLeft = mvl_cfg_info->max_msg_size;

/* Now get the journals from the user, one at a time */
  respInfo->num_of_jou_entry = 0;
  je = (JOURNAL_ENTRY *) (respInfo + 1);
  for (i = 0; i < _bufCtrl.numUsrEntries; ++i)
    {
  /* get the next from the user */
    mvlJep = &mvlJe;
    rc = u_mvl_get_next_jread_entry (jCtrl, usrHandle, &moreFollows, &mvlJep);
    if (rc != SD_SUCCESS)
      {
      moreFollows = SD_FALSE;	/* Didn't get entry, so can't be more.	*/
      break;
      }

  /* Map it onto the MMS-EASE JOURNAL_ENTRY */
    rc = mvlJe_to_je (je, mvlJep);

  /* Let the user free resources */
    u_mvl_free_jread_entry (jCtrl, usrHandle, mvlJep);

  /* Verify that the mapping was OK */
    if (rc != SD_SUCCESS)
      {
      M_FREE (MSMEM_ASN1_DATA_ENC, asn1_buf);
      M_FREE (MSMEM_ENC_OS_INFO, respInfo);
      _mplas_err_resp (indCtrl, 1, 3); 	/* application reference invalid */
      return;
      }  

    ++respInfo->num_of_jou_entry;
    if (moreFollows == SD_FALSE)
      break;

  /* We need to adjust the MMS-EASE JE pointer to the next position	*/
  /* Each JE is possibly followed by a array of VAR_INFO		*/
    if (je->ent_content.entry_form_tag == 2  &&
        je->ent_content.ef.data.list_of_var_pres)
      {
      je = (JOURNAL_ENTRY *) ((ST_CHAR *)je + sizeof (JOURNAL_ENTRY) +
           je->ent_content.ef.data.num_of_var * sizeof (VAR_INFO));
      }
    else
      ++je;	/* CRITICAL: don't try to access je after this	*/
    }
  respInfo->more_follows = moreFollows;

/* OK, the MMS-EASE data structure is ready to roll */

  indCtrl->u.jread.resp_info = respInfo;

  mplas_jread_resp (indCtrl);

  M_FREE (MSMEM_ASN1_DATA_ENC, asn1_buf);
  M_FREE (MSMEM_ENC_OS_INFO, respInfo);

/* Tell the user we are done */
  u_mvl_end_jread (jCtrl, usrHandle, &_bufCtrl, reqInfo);
  }

/************************************************************************/
/*			mplas_jread_resp				*/
/************************************************************************/
ST_VOID mplas_jread_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  /* Build a jread response 						*/
  rc = mpl_jread_resp (indCtrl->event->u.mms.dec_rslt.id, 
                       indCtrl->u.jread.resp_info);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, indCtrl->u.jread.resp_info);

  _mvl_send_resp_i (indCtrl, rc);
  }


/************************************************************************/
/*				mvlJe_to_je				*/
/************************************************************************/

static ST_RET mvlJe_to_je (JOURNAL_ENTRY *je, MVL_JOURNAL_ENTRY *mvlJe)
  {
ST_UCHAR *appRefAsn1Start;
ST_INT appRefAsn1Len;
ST_INT i;
ST_INT numVar;
ST_RET rc;
VAR_INFO *varInfo;

/* First take care of the entry ID */
#if defined (MVL_JREAD_ENTRYID_DYNAMIC)
  je->entry_id_len = mvlJe->entry_id_len;
  je->entry_id = _bufCtrl.nextEntryId;
  memcpy (_bufCtrl.nextEntryId, mvlJe->entry_id, mvlJe->entry_id_len);
  _bufCtrl.nextEntryId += mvlJe->entry_id_len;
#else
  je->entry_id_len = mvlJe->entry_id_len;
  je->entry_id = mvlJe->entry_id;
#endif

/* Now take care of the application reference ... */
  rc = ms_appref_to_asn1 (&(mvlJe->orig_app), _asn1BufStart, _asn1BufLeft, 
			  &appRefAsn1Start, &appRefAsn1Len);
  if (rc == SD_SUCCESS)
    {
    je->orig_ae_len = appRefAsn1Len;
    je->orig_ae = appRefAsn1Start;

  /* Adjust ASN.1 buffer control - this was built from the back, so	*/
  /* don't need to change the buffer start pointer.			*/
    _asn1BufLeft -= appRefAsn1Len;
    }
  else
    {
    MVL_LOG_ERR0 ("Error: ReadJournal.Resp: orig_app encode failed");
    return (rc);
    }

/* Now set up the entry content */

/* Occurance Time */
  memcpy (&(je->ent_content.occur_time), &(mvlJe->occur_time),
	  sizeof (MMS_BTOD));

/* No additional detail */
  je->ent_content.addl_detail_pres = SD_FALSE;

  je->ent_content.entry_form_tag = mvlJe->entry_form_tag;
  if (mvlJe->entry_form_tag == 2)	/* event/data */
    {
    je->ent_content.ef.data.event_pres = mvlJe->ef.data.event_pres;

    memcpy (&(je->ent_content.ef.data.evcon_name), 
	    &(mvlJe->ef.data.evcon_name), sizeof (OBJECT_NAME));
    je->ent_content.ef.data.cur_state = mvlJe->ef.data.cur_state;

    je->ent_content.ef.data.list_of_var_pres = mvlJe->ef.data.list_of_var_pres;
    if (je->ent_content.ef.data.list_of_var_pres)
      {
      numVar = mvlJe->ef.data.num_of_var;
      je->ent_content.ef.data.num_of_var = numVar;

      varInfo = (VAR_INFO *) (je + 1);
      for (i = 0; i < numVar; ++i, ++varInfo)
        {
        /* OK, now tell MMS-EASE where the ASN.1 data is		*/
        varInfo->value_spec.data = mvlJe->ef.data.list_of_var[i].value_spec.data;
        varInfo->value_spec.len = mvlJe->ef.data.list_of_var[i].value_spec.len;

        /* Now store the var_tag in our buffer and set the MMS pointer	*/
#if defined (MVL_JREAD_VARTAG_DYNAMIC)
        varInfo->var_tag = _bufCtrl.nextVarTag;
        strcpy (_bufCtrl.nextVarTag, mvlJe->ef.data.list_of_var[i].var_tag);
        _bufCtrl.nextVarTag += (strlen (_bufCtrl.nextVarTag) + 1);
#else
        varInfo->var_tag = mvlJe->ef.data.list_of_var[i].var_tag;
#endif
        }	/* end "loop"	*/
      }		/* end "if list_of_var_pres"	*/
    }
  else	/* entry form is annotation */
    {
#if defined (MVL_JREAD_ANNOTATION_DYNAMIC)
    je->ent_content.ef.annotation = _bufCtrl.nextAnnotation;
    strcpy (_bufCtrl.nextAnnotation, mvlJe->ef.annotation);
    _bufCtrl.nextAnnotation += (strlen (_bufCtrl.nextAnnotation) + 1);
#else
    je->ent_content.ef.annotation = mvlJe->ef.annotation;
#endif
    }
  return (SD_SUCCESS);
  }

