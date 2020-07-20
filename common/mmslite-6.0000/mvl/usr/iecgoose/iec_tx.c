/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2002-2005, All Rights Reserved					*/
/*									*/
/* MODULE NAME : iec_tx.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Module contains sample code for sending  IEC GOOSE		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/						
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/11/12  JRB	   iecGoosePubCreate: add retrans_curve arg &	*/
/*			   save it in struct.				*/
/* 07/08/08  JRB     17    Make sure StNum is 1-4294967295 (never 0).	*/
/*			   Skip retrans checks if 0 time elapsed.	*/
/*			   Chg some arg names & comments for clarity.	*/
/* 07/02/08  JRB     16    iecGoosePubCreate: chg ConfRev to ST_UINT32	*/
/*			   & init gptr->needsCommissioning.		*/
/* 07/02/07  JRB     15    Fix goose_pdu, goose_enc_buf types.		*/
/* 10/30/06  JRB     14    Use new mvl_vmd_* object handling functions.	*/
/* 08/09/06  JRB     13    Don't use u_mvl_get_nvl, u_mvl_free_nvl	*/
/*			    (not needed). Use only mvl_find_nvl.	*/
/*			   Simplify loop calling gse_iec_data_init.	*/
/* 03/20/06  JRB     12    Use new clnp_snet_write_raw instead of	*/
/*			   gse_iec_send (now obsolete).			*/
/* 02/08/05  JRB     11    retrans_goose: chg to loop thru pub_list	*/
/*			    (body of loop same but fixed curve_offset calc).*/
/*			   start_trans_goose: pass IEC_GOOSE_SEND_USER_INFO*/
/*			    arg (avoids need to loop though list).	*/
/*			   iecGoosePubDestroy: fix memory corruption.	*/
/* 08/31/04  JRB     10    Reset sqNum to 0 on each state change.	*/
/* 05/19/04  JRB     09    Use gs_mutex.. only if USE_FRAMEWORK.. defined*/ 
/* 01/09/03  JRB     08    Check return from gse_iec_encode.		*/
/* 11/06/03  JRB     07    Chg retrans_goose return value.		*/
/*			   start_trans_goose signal retrans_event_sem.	*/
/* 10/09/03  JRB     06    Use new shorter log macro names.		*/
/* 04/17/03  JRB     05    Fix GOOSE scan code so multiple		*/
/*			   concurrent scans don't corrupt one another.	*/
/*			   Del u_mvlu_goose_scan_done_fun global	*/
/*			   Use mvlu_setup_scan_read, don't need		*/
/*			    mvlu_rpt_create_scan_ctrl or MVLU_RPT_SCAN_CTRL*/
/* 01/10/03  JRB     04    Del all refs to dataRef, elementId struct	*/
/*			   members & related code.			*/
/*			   Del unneeded args from gse_iec_data_init.	*/
/*			   Make function & flag names more consistent.	*/
/* 12/10/02  ASK     03    Ethertype additions				*/
/* 02/20/01  JRB     02    Fix format.					*/
/* 02/14/02  HSF     01    Created					*/
/************************************************************************/
#include "iec_demo.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__; 
#endif


static ST_RET u_mvlu_goose_scan_done (MVL_IND_PEND *indCtrl);


/**********************************************************/
/*		retrans_goose()						*/
/* Application code to retransmit a GOOSE				*/
/* Returns: msec to wait before next retrans.				*/
/* NOTE: if USE_FRAMEWORK_THREADS is defined, pub_ctrl access is	*/
/*   protected by mutex semaphore (pub_ctrl.pub_mutex).			*/
/**********************************************************/
ST_LONG retrans_goose (ST_LONG elapsed_msec)
{
  ST_UINT32 stNum;
  GSE_IEC_CTRL *temp_gptr;
  IEC_GOOSE_SEND_USER_INFO *user_info= NULL;
  ST_UCHAR goose_enc_buf[MAX_IEC_GOOSE_ENCODE_BUF];
  ST_INT goose_pdu_len;
  ST_UCHAR *goose_pdu;
  ST_INT32 min_retrans_timeout = 0;	/* minimum retrans timeout for all GEESE*/
					/* 0 means infinite (none to retransmit)*/

ETYPE_INFO etype; /* Ethertype Info */

  /* Don't waste time checking for retrans if 0 time has elapsed.*/
  if (elapsed_msec == 0)
    return (0);

#ifdef USE_FRAMEWORK_THREADS
  gs_mutex_get (&pub_ctrl.pub_mutex);
#endif
  /* For each publisher, check if it's time to retransmit.	*/
  for (user_info = pub_ctrl.pub_list;
       user_info != NULL;
       user_info = (IEC_GOOSE_SEND_USER_INFO *) list_get_next (pub_ctrl.pub_list, user_info))
    {
    temp_gptr = user_info->gptr;	/* gptr ALWAYS valid at this point	*/

    /*in order to prevent a thread race condition caused by	*/
    /*an old retransmit potentially being issued after a state change	*/
 
    /*save the old state number	*/
    stNum = temp_gptr->stNum;	

    if(user_info->goose_state == SD_GOOSE_IDLE)
      {
      if( (user_info->retrans_timeout -= elapsed_msec)<=0) 
        {
        if(!++temp_gptr->sqNum)				/*zero(0) is a reserved value	*/
          ++temp_gptr->sqNum;

        /*set the new transmission information here	*/

        /* If less than max, increment 'retrans_curve_offset'.	*/
        if (user_info->retrans_curve_offset + 1 < user_info->retrans_curve.num_retrans)
          user_info->retrans_curve_offset++;
        user_info->retrans_timeout = user_info->retrans_curve.retrans[user_info->retrans_curve_offset];

        temp_gptr->timeToLive = (user_info->retrans_timeout)<<1;		/*double retranstime for TAL	*/


        /*call to encode GOOSE here	*/
 
        /* Fill out a Goose Ethertype frame */
        etype.tci     = user_info->etype_tci;
        etype.etypeID = user_info->etype_id;
        etype.appID   = user_info->etype_appID;

        goose_pdu = gse_iec_encode (temp_gptr,goose_enc_buf,MAX_IEC_GOOSE_ENCODE_BUF,&goose_pdu_len, &etype);

        if(temp_gptr->stNum == stNum)		/*here is the check for a state change while	*/
						/*encoding	*/
          {
          /*send encoded GOOSE otherwise discard	*/
          /*actual send	*/

          IEC_GOOSE_LOG_RETRANS7("Retransmitting IEC Goose GcRef[%s] to DstMac[%02x:%02x:%02x:%02x:%02x:%02x]",
              temp_gptr->gcRef,user_info->dstAddress[0],user_info->dstAddress[1],
              user_info->dstAddress[2],user_info->dstAddress[3],user_info->dstAddress[4],
              user_info->dstAddress[5]);

#if 0	/* encoding almost same on retrans. Skip hex logging.	*/
          IEC_GOOSE_LOG_RETRANSH(goose_pdu_len,goose_pdu);
#endif

          /* gse_iec_encode encodes backwards from the end of goose_enc_buf.*/
          /* Make sure there is room at start of buf to put DST & SRC MACs.*/
          assert (goose_pdu_len+12 <= sizeof (goose_enc_buf));
          memcpy (goose_pdu-12, user_info->dstAddress, CLNP_MAX_LEN_MAC);/* DST MAC*/
          memcpy (goose_pdu-6, clnp_param.loc_mac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
          clnp_snet_write_raw (goose_pdu-12, goose_pdu_len+12);
          }
        }
      if (user_info->retrans_timeout > 0)
        {
        /* if min == 0 (i.e. not set yet) or this timeout < min, update min.	*/
        if (min_retrans_timeout == 0 ||
            user_info->retrans_timeout < min_retrans_timeout)
          min_retrans_timeout = user_info->retrans_timeout;
        }
      }
    }	/* end main loop	*/
#ifdef USE_FRAMEWORK_THREADS
  gs_mutex_free (&pub_ctrl.pub_mutex);
#endif
  return (min_retrans_timeout);
}


/************************************************************************/
/*		start_trans_goose					*/
/* Start transmission of a new GOOSE message.				*/
/* Use retrans_curve saved in IEC_GOOSE_SEND_USER_INFO struct.		*/
/************************************************************************/
ST_RET	start_trans_goose (IEC_GOOSE_SEND_USER_INFO *user_info) 
  {
  ST_RET ret = SD_FAILURE;
  ST_UCHAR goose_enc_buf[MAX_IEC_GOOSE_ENCODE_BUF];
  ST_INT goose_pdu_len;
  ST_UCHAR *goose_pdu;
  GSE_IEC_CTRL *gptr;

ETYPE_INFO etype; /* Ethertype Info */

  gptr = user_info->gptr;

  IEC_GOOSE_LOG_CFLOW1("State change GOOSE Transmission GcRef[%s]",gptr->gcRef);

  user_info->goose_state = SD_GOOSE_TRANSMITTING;
  /* Initialize timeout using first entry of retrans curve.*/
  user_info->retrans_timeout = user_info->retrans_curve.retrans[0];
  gptr->timeToLive = (user_info->retrans_timeout)<<1;		/*double retranstime for TAL	*/

  user_info->retrans_curve_offset = 0;
  /* 61850-8-1 says to start with StNum=1 and skip over 0. StNum was	*/
  /* initialized to 0 when gptr created, so this should work.		*/
  ++gptr->stNum;
  if (gptr->stNum==0)
    ++gptr->stNum;	/* skip over 0					*/
  gptr->sqNum = 0;	/* reset sqNum to 0 on each state change	*/

  /*issue call to encode and to send here	*/

  /* Fill out a Goose Ethertype frame */
  etype.tci     = user_info->etype_tci;
  etype.etypeID = user_info->etype_id;
  etype.appID   = user_info->etype_appID;
  
  goose_pdu = gse_iec_encode (gptr,goose_enc_buf,MAX_IEC_GOOSE_ENCODE_BUF,&goose_pdu_len, &etype);

  if (goose_pdu)
    {
    IEC_GOOSE_LOG_TX7("Sending IEC Goose GcRef[%s] to DstMac[%02x:%02x:%02x:%02x:%02x:%02x]",
      gptr->gcRef,user_info->dstAddress[0],user_info->dstAddress[1],
      user_info->dstAddress[2],user_info->dstAddress[3],user_info->dstAddress[4],
      user_info->dstAddress[5]);

    IEC_GOOSE_LOG_TXH(goose_pdu_len,goose_pdu);
  
    /*actual send	*/

    /* gse_iec_encode encodes backwards from the end of goose_enc_buf.*/
    /* Make sure there is room at start of buf to put DST & SRC MACs.*/
    assert (goose_pdu_len+12 <= sizeof (goose_enc_buf));
    memcpy (goose_pdu-12, user_info->dstAddress, CLNP_MAX_LEN_MAC);	/* DST MAC*/
    memcpy (goose_pdu-6, clnp_param.loc_mac, CLNP_MAX_LEN_MAC);		/* SRC MAC*/
    clnp_snet_write_raw (goose_pdu-12, goose_pdu_len+12);

    ret = SD_SUCCESS;
    }
  else
    SLOGALWAYS0 ("ERROR: IEC GOOSE encode failed. NOT sent.");

  user_info->goose_state = SD_GOOSE_IDLE;
#ifdef USE_FRAMEWORK_THREADS
  assert (retrans_event_sem !=0);	/* must be initialized before now	*/
  gs_signal_event_sem (retrans_event_sem);	/* Wake up retrans_thread	*/
#endif
  return(ret);  
  }


/*************************************************************************	*/

/*************************************************************************	*/
/*************************************************************************	*/
/*************************************************************************	*/
/*				iecGoosePubCreate				*/
/* Function to create a GOOSE Control block and user tracking information	*/

IEC_GOOSE_SEND_USER_INFO *iecGoosePubCreate( ST_UCHAR *DstAddress,
	ST_CHAR *GoCBRef,
	ST_CHAR *DataSetRef,
	ST_CHAR *GoID,
	ST_UINT32 ConfRev,
	ST_BOOLEAN NeedsComm,
	ST_UINT16 tci,
	ST_UINT16 etypeID,
	ST_UINT16 appID,	/* Ethertype APPID	*/
	RETRANS_CURVE *retrans_curve)
  {
  GSE_IEC_CTRL *gptr;
  ST_INT numDataSetEntries;
  ST_INT i;
  IEC_GOOSE_SEND_USER_INFO *goosehandle = NULL;
  OBJECT_NAME nvl_obj;
  MVL_NVLIST_CTRL *nvl_ptr;
  RUNTIME_TYPE *rtType;
  ST_INT num_rts;
  ST_CHAR domain[MAX_IDENT_LEN+1];
  ST_CHAR item[MAX_IDENT_LEN+1];

  nvl_obj.domain_id= domain;
  nvl_obj.obj_name.item_id = item;

  /*find the data set via the reference	*/
  /*need to convert reference first	*/
  if(convert_reference_to_mms_obj(DataSetRef, &nvl_obj)==NULL)
    return(goosehandle);

  /*now find the actual namevariable list	*/
  if ((nvl_ptr = mvl_vmd_find_nvl(&mvl_vmd, &nvl_obj, NULL)) == NULL)
    return(goosehandle);

  numDataSetEntries = nvl_ptr->num_of_entries;

  /*create the gse_control	*/

  if((gptr = gse_iec_control_create (GoCBRef, DataSetRef, GoID, numDataSetEntries))!=NULL)
    {
    gptr->confRev = ConfRev;
    gptr->needsCommissioning = NeedsComm;
    goosehandle = add_sending_goose(gptr);    /*add here so that strings may be stored here	*/

    /*have a good control block, time to add data entries	*/

    for(i=0; i< numDataSetEntries; i++)
      {
      /*find the RuntimeType for the DataEntry	*/
      mvl_get_runtime (nvl_ptr->entries[i]->type_id,&rtType,&num_rts);
   
      gse_iec_data_init(gptr, i, rtType, num_rts);
      }

    /* Save nvl_ptr to use later when ready to start data scan.	*/
    goosehandle->nvl = nvl_ptr;
    memcpy(goosehandle->dstAddress,DstAddress,MAC_ADDR_SIZE);
    
    /* finally, add the Ethertype information */
    goosehandle->etype_tci     = tci;
    goosehandle->etype_appID   = appID;
    goosehandle->etype_id = etypeID;
    /* Save retransmission curve in the structure.	*/
    memcpy (&goosehandle->retrans_curve, retrans_curve, sizeof (RETRANS_CURVE));
    }
  return(goosehandle);
  }

/****************************************************************/
/*				iecGoosePubDestroy								*/

ST_RET iecGoosePubDestroy (IEC_GOOSE_SEND_USER_INFO *goosehandle)
{
ST_RET ret = SD_SUCCESS;
GSE_IEC_CTRL *gptr;

  gptr= goosehandle->gptr;	/*save to pass to gse_iec_*	*/

/*order of resources reserved in the creation	*/
/*	GSE_IEC_CTRL (goose_control) via gse_iec_control_create()	*/
/*		returned via gse_iec_control_destroy	*/
/*  IEC_GOOSE_SEND_USER_INFO *goosehandle  via add_sending_goose	*/
/*		returned via delete_sending_goose()	*/
/*	*/
/*Return of resources must be in reverse order of resource reservation	*/
/*just to be safe, NULL before using because of multi-thread	*/

  delete_sending_goose(goosehandle);
  gse_iec_control_destroy(gptr);

  return(ret);
  
}



/***************************************************************/
/*			iecGoosePublish	*/
/*  Function to poll for changed information, encode GOOSE	*/
/*  then send the GOOOSE	*/
/*  To actually publish, the sequence is call iecGoosePublish	*/
/*     this polls the data, the done function is then called	*/
/*     the done function updates the goose control	*/
/*		then calls start_trans_goose	*/
/*			encodes then sends	*/
/****************************************************************/
ST_VOID iecGoosePublish (IEC_GOOSE_SEND_USER_INFO *goosehandle)
  {
  /*poll the NVL that was saved	*/

  IEC_GOOSE_LOG_FLOW1("Polling for data to publish GcRef[%s]",goosehandle->gptr->gcRef);

  /* mvlu_setup_scan_read creates a simulated read indication used to scan all
   * the data of the NVL (i.e. call all the leaf functions). The read
   * indication function "u_mvl_read_ind" is then called to start the
   * scan, but a read response is not sent because this is not a real read ind.
   * Setup scan with scan_va_done_fun=NULL, scan_done_fun=u_mvlu_goose_scan_done
   * so u_mvlu_goose_scan_done is called when the scan completes.
   */
  goosehandle->scan_ind = mvlu_setup_scan_read (goosehandle->nvl, NULL,
                            u_mvlu_goose_scan_done);
  assert(goosehandle->scan_ind->usr==NULL);	/* make sure not already used*/
  goosehandle->scan_ind->usr = goosehandle;	/* save to use in scan_done_fun*/
  u_mvl_read_ind (goosehandle->scan_ind);	/* start scan	*/ 

  /*the encode and publication code is found in u_mvlu_goose_scan_done.	*/
  }


/************************************************************************/

static ST_RET u_mvlu_goose_scan_done (MVL_IND_PEND *indCtrl)
  {
IEC_GOOSE_SEND_USER_INFO *goosehandle = NULL;
MVL_VAR_ASSOC   *va,**va_array;
ST_INT i, numEntries;
GSE_IEC_CTRL *temp_gptr;

  goosehandle = indCtrl->usr;				/* retrieve NVL pointer */
  temp_gptr = goosehandle->gptr;
  IEC_GOOSE_LOG_CFLOW1("Polling for data complete GcRef[%s]",temp_gptr->gcRef);
  numEntries = goosehandle->gptr->numDataEntries; 
  va_array = goosehandle->nvl->entries;
  for (i = 0; i < numEntries; ++i,va_array++)
    {
    va = *va_array;
    /*have the data, now it is time to update the data elements	*/

    /*associated with the gooseControl	*/
  
    /*goosecontrol data offset is exactly the element offset of NVL	*/
    gse_iec_data_update (temp_gptr,i,va->data);
    }
  
  /*ready to encode and send	*/
  
  start_trans_goose (goosehandle);	/* retrans_curve is in struct*/
  M_FREE (MSMEM_GEN, indCtrl);
#if 0	/* DEBUG: enable this to chk memory usage	*/
  dyn_mem_ptr_statistics (0);
#endif

  return (SD_SUCCESS);
  }

