/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1999, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_obtfi.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_obtfile_ind					*/
/*	mplas_obtfile_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/13/10  JRB	   send_file_*: on error, free req & ret NULL.	*/
/*			   mvlas_obtfile_*: on error, free obtfile_state*/
/* 01/20/10  JRB     11    Fix log messages.				*/
/* 11/12/07  MDE     10    Use _mvl_getpath for temp file path		*/
/* 04/09/07  MDE     09    Enhanced filtered logging 			*/
/* 10/17/06  MDE     08    Added mvlas_obtfile_resp_ex			*/
/* 01/30/06  GLB     07    Integrated porting changes for VMS           */
/* 02/04/03  MDE     06    Now use _mvl_tmpnam				*/
/* 09/21/01  JRB     05    Alloc global bufs only once at startup.	*/
/* 04/14/00  EJV     04    Added check for more error conditions in	*/
/*			   u_fopen_done and u_fread_done		*/
/* 03/21/00  JRB     03    Send err resp if tmpnam or fopen fails.	*/
/* 01/21/00  MDE     02    Now use MEM_SMEM for dynamic memory		*/
/* 10/15/99  GLB     01    Created to add Obtain File function   	*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mvl_defs.h"
#include "mvl_log.h"

#include <stdio.h>
/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

typedef struct filecopyinfo
  {
  ST_CHAR  	tempfilename[MVL_MAX_TMPNAM_LEN+1];
  ST_CHAR	destfilename[MAX_FILE_NAME+1];
  MVL_IND_PEND  *indCtrl;
  FILE		*fp;
  ST_INT32	frsmid;
  ST_UINT32	fsize;
  ST_UINT32	fileSizeLimit;
  } MVL_OBTFILE_STATE;

MVL_REQ_PEND *send_file_open (MVL_NET_INFO *NetInfo, ST_CHAR *src);
MVL_REQ_PEND *send_file_read (MVL_NET_INFO *NetInfo, 
			      MVL_OBTFILE_STATE *u_file_info);
MVL_REQ_PEND *send_file_close (MVL_NET_INFO *NetInfo, 
			      MVL_OBTFILE_STATE *u_file_info);
ST_VOID u_fopen_done (MVL_REQ_PEND *req);
ST_VOID u_fread_done( MVL_REQ_PEND *req);
ST_VOID u_fclose_done( MVL_REQ_PEND *req);


/************************************************************************/
/************************************************************************/
/*			_mvl_process_obtfile_ind		        */
/* An Obtain File request has been sent and                             */
/* An Obtain File indication was received, so process it and respond	*/
/************************************************************************/

ST_VOID _mvl_process_obtfile_ind (MVL_IND_PEND *indCtrl)
  {
  OBTFILE_REQ_INFO *req_info;
  MVLAS_OBTFILE_CTRL *obtfile;
  FILE_NAME *fname_idx;


  req_info = (OBTFILE_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;
  fname_idx = (FILE_NAME *)(req_info + 1);

  obtfile = &indCtrl->u.obtfile;

  if (_mvl_concat_filename (obtfile->srcfilename, req_info->num_of_src_fname, 
                            fname_idx, sizeof (obtfile->srcfilename)))
    {
    MVL_LOG_ERR0 ("ObtFile error: _mvl_process_obtfile_ind failed (_mvl_concat_filename(srcfilename) failed)");
    _mplas_err_resp (indCtrl,11,3);	/* 3 = File name syntax error */
    return;
    }

  if (_mvl_concat_filename (obtfile->destfilename, req_info->num_of_dest_fname, 
                    (FILE_NAME *)(fname_idx + req_info->num_of_src_fname), 
                    sizeof (obtfile->destfilename)))
    {
    MVL_LOG_ERR0 ("ObtFile error: _mvl_process_obtfile_ind failed (_mvl_concat_filename(destfilename) failed)");
    _mplas_err_resp (indCtrl,11,3);	/* 3 = File name syntax error */
    return;
    }
  
  u_mvl_obtfile_ind (indCtrl);
  }

/************************************************************************/
/*			mvlas_obtfile_resp				*/
/* Open a temp file to copy into and save the destination file name     */
/************************************************************************/

ST_VOID mvlas_obtfile_resp (MVL_IND_PEND *indCtrl)
  {  
  mvlas_obtfile_resp_ex (indCtrl, 0);
  }


ST_VOID mvlas_obtfile_resp_ex (MVL_IND_PEND *indCtrl, ST_UINT32 fileSizeLimit)
  {
  MVL_OBTFILE_STATE *obtfile_state;
  MVL_REQ_PEND *req;
  MVLAS_OBTFILE_CTRL *obtfile;
  ST_CHAR tmpfile_path[MVL_MAX_TMPFILE_PATH];

  obtfile = &indCtrl->u.obtfile;

  obtfile_state = (MVL_OBTFILE_STATE *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_OBTFILE_STATE));

  /* save destination name for rename */
  strncpy (obtfile_state->destfilename, obtfile->destfilename, MAX_FILE_NAME+1); 

  obtfile_state->indCtrl = indCtrl;

  /* Create a temporary file name to copy into. This prevents partially	*/
  /* transferred files from hanging around and being accidently used	*/
  /* Open the created temporary file. Send the file open request.	*/
   
  _mvl_getpath (obtfile_state->destfilename, tmpfile_path);

  if (_mvl_tmpnam (obtfile_state->tempfilename, tmpfile_path) != SD_SUCCESS  ||
      (obtfile_state->fp = fopen(obtfile_state->tempfilename, "wb")) == NULL ||
      (req = send_file_open (indCtrl->event->net_info,obtfile->srcfilename)) == NULL)
    {
    if ( obtfile_state->fp )
      {
      MVL_LOG_ERR0 ("ObtFile error: mvlas_obtfile_resp failed (fopen failed)");
      fclose (obtfile_state->fp);              /* close temporary file      */
      remove (obtfile_state->tempfilename);    /* get rid of temporary file */
      }
    else
      {
      MVL_LOG_ERR1 ("ObtFile: mvlas_obtfile_resp failed (invalid tmpnam (%s) or send_file_open failed)", 
      					obtfile_state->tempfilename);
      }
    _mplas_err_resp (indCtrl,11,0);	/* 0 = other */
    M_FREE (MSMEM_GEN, obtfile_state);
    }
  else
    {
    req->v = obtfile_state;
    obtfile_state->fileSizeLimit = fileSizeLimit;
    }
  }

/************************************************************************/
/*		            send_file_open				*/
/* Send a file open request to open file on the other side              */
/************************************************************************/

MVL_REQ_PEND *send_file_open (MVL_NET_INFO *NetInfo, ST_CHAR *src)
  {
MVL_REQ_PEND *req= NULL;

  if ((mvla_fopen(NetInfo, src, 0, &req)) == SD_SUCCESS)
    {
    req->u_req_done = u_fopen_done;
    }
  else
    {
    /* error, clean up and return NULL	*/
    /* NOTE: "req" may be NULL but "mvl_free_req_ctrl" safely handles that.*/
    mvl_free_req_ctrl(req);
    req = NULL;
    }
  return (req);
  }

/************************************************************************/
/*		            u_fopen_done				*/
/* It was indicated that a file open request was made                   */
/* Confirm file open response that was received as a result of a request*/                                           
/************************************************************************/

ST_VOID u_fopen_done( MVL_REQ_PEND *req)
  {
  ST_RET ret;
  MVL_REQ_PEND  *freadreq= NULL;
  MVL_OBTFILE_STATE  *obtfile_state;
  FOPEN_RESP_INFO  *resp_info;
#if 0
  ERR_INFO 		*err_ptr;
  REJECT_RESP_INFO 	*rej_ptr;
#endif

  obtfile_state = (MVL_OBTFILE_STATE *) req->v;
  ret = req->result;
  if (ret == SD_SUCCESS)
    {
    resp_info = req->u.fopen.resp_info;
    if (obtfile_state->fileSizeLimit > 0 && 
        (resp_info->ent.fsize > obtfile_state->fileSizeLimit))
      {
      MVL_LOG_NERR2 ("ObtFile error: file size %lu exceeds limit %lu", 
          (ST_ULONG)resp_info->ent.fsize, (ST_ULONG)obtfile_state->fileSizeLimit);
      ret = SD_FAILURE;
      }  
    else
      {  

      obtfile_state ->frsmid = resp_info->frsmid;
      obtfile_state ->fsize = resp_info->ent.fsize;
      if ((freadreq = send_file_read (req->net_info, obtfile_state)) != NULL)
        {
        freadreq->v = obtfile_state;
        }
      else
        {
        MVL_LOG_ERR0 ("ObtFile error: u_fopen_done failed (send_file_read failed)");
        ret = SD_FAILURE;
        }
      }
    }
  else
    {
    MVL_LOG_ERR0 ("ObtFile error: u_fopen_done failed (mvla_fopen failed)");
    }
  if ( ret != SD_SUCCESS )
    { /* file open failed */
#if 0
    if (req->result==MVL_ERR_CNF_REJ_ERR)
      {
      rej_ptr = (REJECT_RESP_INFO *) req->event->u.mms.dec_rslt.data_ptr;
      printf ("\n MMS REJECT");
      printf ("\n detected_here = %d",req->u.reject.resp_info->detected_here);
      printf ("\n invoke_known = %d",req->u.reject.resp_info->invoke_known);
      printf ("\n invoke = %d",req->u.reject.resp_info->invoke);
      printf ("\n pdu_type = %d",req->u.reject.resp_info->pdu_type);
      printf ("\n rej_class = %d",req->u.reject.resp_info->rej_class);
      printf ("\n rej_code = %d",req->u.reject.resp_info->rej_code);
      }
    if (req->result==MVL_ERR_CNF_ERR_OK)
      {
      err_ptr = (ERR_INFO *) req->event->u.mms.dec_rslt.data_ptr;
      printf ("\n MMS ERROR RESPONSE");
      printf ("\n class = %d, code = %d", req->u.error.resp_info->eclass, 
                                          req->u.error.resp_info->code);

      }
#endif
    fclose (obtfile_state->fp);	       /* close temporary file      */
    remove (obtfile_state->tempfilename);    /* get rid of temporary file */
    if (req->net_info->conn_active == SD_TRUE)
      _mplas_err_resp (obtfile_state->indCtrl,11,0);	/* 0 = other */
    M_FREE (MSMEM_GEN, obtfile_state);
    } 
  mvl_free_req_ctrl (req);
  }

/************************************************************************/
/*		            send_file_read				*/
/* Send a file read request to obtain a block of the file data          */
/************************************************************************/

MVL_REQ_PEND *send_file_read (MVL_NET_INFO *NetInfo, 
			      MVL_OBTFILE_STATE *obtfile_state)
  {
  MVL_REQ_PEND *req= NULL;

  FREAD_REQ_INFO fread_req;

  fread_req.frsmid = obtfile_state->frsmid;  
  if ((mvla_fread (NetInfo, &fread_req, &req)) == SD_SUCCESS)
    {
    req->u_req_done = u_fread_done;
    }
  else
    {
    /* error, clean up and return NULL	*/
    /* NOTE: "req" may be NULL but "mvl_free_req_ctrl" safely handles that.*/
    mvl_free_req_ctrl(req);
    req = NULL;
    }
  return (req);
  }


/************************************************************************/
/*		            u_fread_done				*/
/* Confirm the file read response sent to indicate a read request       */
/************************************************************************/

ST_VOID u_fread_done( MVL_REQ_PEND *req)
  {
  ST_RET ret;
  MVL_REQ_PEND *freadreq= NULL;
  MVL_REQ_PEND *fclosereq= NULL;
  MVL_OBTFILE_STATE *obtfile_state;
  FREAD_RESP_INFO	*resp_info;

  obtfile_state = (MVL_OBTFILE_STATE *) req->v;

  ret = req->result;
  if (ret == SD_SUCCESS)
    {
    resp_info = req->u.fread.resp_info;
   
    if ( fwrite (resp_info->filedata, sizeof(ST_CHAR), resp_info->fd_len, obtfile_state->fp) 
          == (unsigned) resp_info->fd_len )
      {
      if ( resp_info->more_follows == SD_TRUE)
        {
        if ((freadreq = send_file_read (req->net_info, obtfile_state)) != NULL)
          {
          freadreq->v = obtfile_state;  /* if more file data request to read another block */
          }
        else
          {
          MVL_LOG_ERR0 ("ObtFile error: u_fread_done failed (send_file_read failed)");
          ret = SD_FAILURE;
          }
        }
      else
        {
        if ((fclosereq = send_file_close (req->net_info, obtfile_state)) != NULL)
          {
          fclosereq->v = obtfile_state;  /* no more file data so send a file close request */
          }
        else
          {
          MVL_LOG_ERR0 ("ObtFile error: u_fread_done failed (send_file_close failed)");
          ret = SD_FAILURE;
          }
        }
      }
    else
      {
      /* write failed */
      MVL_LOG_ERR0 ("ObtFile error: u_fread_done failed (fwrite failed)");
      ret = SD_FAILURE;
      }
    }
  else
    {
    MVL_LOG_ERR0 ("ObtFile error: u_fread_done failed (mvla_fread failed)");
    }

  if (ret != SD_SUCCESS )
    { /* read failed so close and remove temp file */
    fclose (obtfile_state->fp);
    remove (obtfile_state->tempfilename);  /* may fail if file nonexistent */
    if (req->net_info->conn_active == SD_TRUE)
      _mplas_err_resp (obtfile_state->indCtrl,11,0);	   /* 0 = Other */
    M_FREE (MSMEM_GEN, obtfile_state);
    }

  mvl_free_req_ctrl (req);
  }

/************************************************************************/
/*		            send_file_close				*/
/* No more data to read from file so send a request to close the file   */
/************************************************************************/

MVL_REQ_PEND *send_file_close (MVL_NET_INFO *NetInfo, 
			       MVL_OBTFILE_STATE *obtfile_state)
  {
  MVL_REQ_PEND *req= NULL;
  FCLOSE_REQ_INFO fclose_req;

  fclose_req.frsmid = obtfile_state->frsmid;
  if ((mvla_fclose (NetInfo, &fclose_req, &req)) == SD_SUCCESS)
    {
    req->u_req_done = u_fclose_done;
    }
  else
    {
    /* error, clean up and return NULL	*/
    /* NOTE: "req" may be NULL but "mvl_free_req_ctrl" safely handles that.*/
    mvl_free_req_ctrl(req);
    req = NULL;
    }
  return (req);
  }

/************************************************************************/
/*		            u_fclose_done				*/
/* Confirm file closed response                                         */
/************************************************************************/

ST_VOID u_fclose_done( MVL_REQ_PEND *req)
  {

  MVL_OBTFILE_STATE *obtfile_state;

  obtfile_state = (MVL_OBTFILE_STATE *) req->v;

  if (req->result == SD_SUCCESS)    /* close succeeded */
  {

    /* always close transferred file, rename and remove temp file */
    fclose (obtfile_state->fp);	    /* close temp file */
    remove (obtfile_state->destfilename);  /* may fail if file nonexistent */

    /* rename the temporary file to become the requested destination file  */ 
    /* use the ANSI rename function if available on your OS  */
    /* otherwise use other function appropriate for your system */
    if (rename (obtfile_state->tempfilename, obtfile_state->destfilename))
      {
       MVL_LOG_ERR0 ("ObtFile error: u_fclose_done failed (rename failed)");
       remove (obtfile_state->tempfilename);  /* cleanup useless file */
       _mplas_err_resp (obtfile_state->indCtrl, 11, 0);   /* 0 = other file error */
      }
    else	/* send an obtainfile response */
      mplas_obtfile_resp (obtfile_state->indCtrl);  

  }
  else  /* close failed */
  {
    MVL_LOG_ERR0 ("ObtFile error: u_fclose_done failed (mvla_fclose failed)");

    /* always close transferred file, rename and remove temp file */
    fclose (obtfile_state->fp);	    /* close temp file */

    remove (obtfile_state->tempfilename);  /* cleanup useless file */
    /* but don't send an obtainfile response */

   if (req->net_info->conn_active == SD_TRUE)
      _mplas_err_resp (obtfile_state->indCtrl,11,0);	/* 0 = other */
  }

  M_FREE (MSMEM_GEN, obtfile_state);
  mvl_free_req_ctrl (req);
  }


/************************************************************************/
/*			mplas_obtfile_resp				*/
/* Send a response to confirm the obtain file request that prompted the */
/* reading of file data                                                 */
/************************************************************************/
ST_VOID mplas_obtfile_resp (MVL_IND_PEND *indCtrl)
  {
  ST_RET rc;
   
  /* Build an Obtain File Response with Null to confirm */
  rc = mpl_obtfile_resp (indCtrl->event->u.mms.dec_rslt.id);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, NULL);

  _mvl_send_resp_i (indCtrl, rc);


  }

