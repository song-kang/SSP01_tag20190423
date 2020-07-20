/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2002-2005, All Rights Reserved					*/
/*								       	*/
/* MODULE NAME : iec_comn.c					       	*/
/* PRODUCT(S)  : MMSEASE-LITE					       	*/
/*								       	*/
/* MODULE DESCRIPTION : 					       	*/
/*	  Module contains common queue code for sending and receiving   */
/*		IEC GOOSE						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/						
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/08/05  JRB     05    add_rxd_goose/delete_rxd..: move to iec_rx.c	*/
/*			   Move IEC_GOOSE_LOG_*_TEXT here so other	*/
/*			    applications can use them.			*/
/*			   Use pub_ctrl, sub_ctrl & del goose_send_info,*/
/*			    send_info_pool, initgoose_pool,		*/
/*			    destroy_goose_pool (confusing, buggy).	*/
/*			   delete_sending_goose: chg arg & simplify.	*/
/* 05/19/04  JRB     04    Use gs_mutex.. only if USE_FRAMEWORK.. defined*/ 
/* 11/04/03  JRB     03    Del sisco_move. Use strncpy.			*/
/* 02/20/01  JRB     02    Fix format.					*/
/* 02/14/02  HSF     01    Created					*/
/************************************************************************/
#include "iec_demo.h"
#include "glbsem.h"


#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__; 
#endif

SD_CONST ST_CHAR *SD_CONST  IEC_GOOSE_LOG_FLOW_TEXT    = "IEC_GOOSE_LOG_FLOW";
SD_CONST ST_CHAR *SD_CONST  IEC_GOOSE_LOG_RX_TEXT      = "IEC_GOOSE_LOG_RX";
SD_CONST ST_CHAR *SD_CONST  IEC_GOOSE_LOG_TX_TEXT      = "IEC_GOOSE_LOG_TX";
SD_CONST ST_CHAR *SD_CONST  IEC_GOOSE_LOG_RETRANS_TEXT = "IEC_GOOSE_LOG_RETRANS";


PUBLISHER_CTRL pub_ctrl;	/* ctrl struct for all publishers.	*/
				/* Used by multiple threads, so access	*/
				/* must be protected by mutex semaphore.*/
SUBSCRIBER_CTRL sub_ctrl;	/* ctrl struct for all subscribers.	*/
				/* Used by multiple threads, so access	*/
				/* must be protected by mutex semaphore.*/

ST_ULONG iec_goose_usr_debug;

/**********************************************************/
/*		add_sending_goose()					*/
/**********************************************************/

IEC_GOOSE_SEND_USER_INFO *add_sending_goose(GSE_IEC_CTRL *gptr)
  {
  IEC_GOOSE_SEND_USER_INFO *user_info= NULL;

  user_info = chk_calloc (1, sizeof (IEC_GOOSE_SEND_USER_INFO));
  user_info->goose_state = SD_GOOSE_CONFIGURED;
  user_info->gptr = gptr;
#if defined(USE_FRAMEWORK_THREADS)
  gs_mutex_get (&pub_ctrl.pub_mutex);
#endif
  list_add_last (&pub_ctrl.pub_list, user_info);
#if defined(USE_FRAMEWORK_THREADS)
  gs_mutex_free (&pub_ctrl.pub_mutex);
#endif
  return(user_info);
  }


/**********************************************************/
/*		delete_sending_goose()					*/
/**********************************************************/
ST_RET delete_sending_goose(IEC_GOOSE_SEND_USER_INFO *user_info)
  {
  ST_RET ret = SD_FAILURE;

#if defined(USE_FRAMEWORK_THREADS)
  gs_mutex_get (&pub_ctrl.pub_mutex);
#endif
  list_unlink (&pub_ctrl.pub_list, user_info);
  chk_free (user_info);
  ret = SD_SUCCESS;
#if defined(USE_FRAMEWORK_THREADS)
  gs_mutex_free (&pub_ctrl.pub_mutex);
#endif
  return (ret);
  }


/**********************************************************/
/*		convert_reference_to_mms_obj()				*/
/**********************************************************/
OBJECT_NAME *convert_reference_to_mms_obj(ST_CHAR *reference, OBJECT_NAME *obj)
  {
  OBJECT_NAME *ret=NULL;
  ST_INT  len;
  ST_CHAR *token_ptr;
  ST_CHAR temp_ref[MAX_IEC_REFERENCE_SIZE+1];
  ST_CHAR *dest_ptr;
  
  if( (reference==NULL) || ((len = strlen(reference))>MAX_IEC_REFERENCE_SIZE))	/*check for a NULL pointer just to be careful	*/

    return(ret);

  strcpy(temp_ref,reference);				/*replace "." with "$"	*/

  while( (token_ptr = strstr(temp_ref,"."))!=NULL)
    *token_ptr = 0x36;   /*change to $	*/

  /*prepare to obtain the domain	*/

  obj->domain_id[0] = 0x0;
  obj->obj_name.item_id[0] = 0x0;
  obj->object_tag = VMD_SPEC;

  if( (token_ptr = strstr(temp_ref,"@"))!=NULL)
    obj->object_tag = AA_SPEC;			/*then we have AA_SPEC	*/
  else if ( (token_ptr = strstr(temp_ref,"/"))!=NULL)
    {
    /* May be either VMD or Domain SCOPE (for VMD, first char is '/').	*/

    if(temp_ref[0] != '/')
      {		/* '/' is NOT first char, so it's Domain scope	*/
      ST_INT name_len = token_ptr - temp_ref;
      obj->object_tag = DOM_SPEC;
      dest_ptr = obj->domain_id;
      strncpy (dest_ptr, temp_ref, name_len);
      dest_ptr [name_len] = '\0';	/* NULL terminate	*/ 
      }
    }
  
  ++token_ptr;	/* point to first char after '/' or '@'	*/
  if(strlen(token_ptr)<=MAX_IDENT_LEN)	
    {			/*can fill in the ITEM_ID	*/
    strcpy(obj->obj_name.item_id,token_ptr);
    ret=obj;
    }

  return(ret);
  }



