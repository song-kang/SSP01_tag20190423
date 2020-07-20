/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2000 - 2000, All Rights Reserved				*/
/*									*/
/* MODULE NAME : c_fget.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_fget							*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/13/10  JRB	   send_file_*: on error, free req & ret NULL.	*/
/* 01/20/10  JRB     10    Fix log messages.				*/
/* 03/03/08  JRB     09    Add static on "u_f*" functions.		*/
/*			   Chk fget_cnf_ptr before using.		*/
/* 11/12/07  MDE     08    Use _mvl_getpath for temp file path		*/
/* 01/30/06  GLB     07    Integrated porting changes for VMS           */
/* 03/01/04  MDE     06    Free request for fget (file open) error	*/
/* 06/17/03  MDE     05    Now use _mvl_tmpnam				*/
/* 02/17/03  JRB     04    Del unused includes.				*/
/* 07/18/02  JRB     03    Del unused var.				*/
/* 03/25/02  MDE     02	   Moved error/reject elements outside union	*/
/* 06/22/00  GLB     01    Created to do an asynchronous File Get.	*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static MVL_REQ_PEND *send_file_open (MVL_NET_INFO *NetInfo, ST_CHAR *src);
static MVL_REQ_PEND *send_file_read (MVL_NET_INFO *NetInfo, 
			     MVL_FGET_REQ_INFO *u_file_info);
static MVL_REQ_PEND *send_file_close (MVL_NET_INFO *NetInfo, 
			     MVL_FGET_REQ_INFO *u_file_info);

static ST_VOID u_fopen_done (MVL_REQ_PEND *req);
static ST_VOID u_fread_done (MVL_REQ_PEND *req);
static ST_VOID u_fclose_done (MVL_REQ_PEND *req);

/************************************************************************/
/*			mvla_fget					*/
/* Perform an asynchronous file copy                                    */
/************************************************************************/
ST_RET mvla_fget (
	MVL_NET_INFO  *net_info,
	ST_CHAR	*srcfilename,
  	ST_CHAR	*destfilename,
	MVL_FGET_REQ_INFO *fget_req_info)
  {
MVL_REQ_PEND *req_out;
ST_CHAR tmpfile_path[MVL_MAX_TMPFILE_PATH];
   
  /* save destination name for rename */
  if (strlen (destfilename) <= MAX_FILE_NAME)
    {
    strcpy (fget_req_info->destfilename, destfilename);
    }
  else
    {
    MVL_LOG_NERR0 ("ERROR: mvla_fget: destination filename save failed ");
    return (SD_FAILURE);
    }

  /* save source name for returning copy status */
  if (strlen (srcfilename) <= MAX_FILE_NAME)
    {
    strcpy (fget_req_info->srcfilename, srcfilename);
    }
  else
    {
    MVL_LOG_NERR0 ("ERROR: mvla_fget: source filename save failed ");
    return (SD_FAILURE);
    }

  /* Create a temporary file name to copy into. This prevents partially	*/
  /* transferred files from hanging around and being accidently used	*/
  /* Open the created temporary file. Send a file open request for the  */
  /* source file specified.                                             */

  _mvl_getpath (destfilename, tmpfile_path);

  if (_mvl_tmpnam (fget_req_info->tempfilename, tmpfile_path) != SD_SUCCESS  ||
      (fget_req_info->fp = fopen(fget_req_info->tempfilename, "wb")) == NULL ||
      (req_out = send_file_open (net_info, srcfilename)) == NULL)
    {
    if (fget_req_info->fp )
      {
      MVL_LOG_NERR1 ("ERROR: mvla_fget: fopen (%s) failed ",
                     fget_req_info->srcfilename);

      if (fclose (fget_req_info->fp)!=0)  /* close temporary file */
        {
        MVL_LOG_ERR1 ("ERROR: mvla_fget: fclose of temp file (%s) failed ",
                       fget_req_info->tempfilename);
        }

      if (remove (fget_req_info->tempfilename)!=0)  /* get rid of temporary file */
        {
        MVL_LOG_ERR1 ("ERROR: mvla_fget: remove of temp file (%s) failed ",
                       fget_req_info->tempfilename);
        }
      }
    else
      {	/* no temp file created */
      MVL_LOG_NERR0 ("ERROR: mvla_fget: temp file creation failed ");
      }
    return(SD_FAILURE);
    }
  else
    {
    req_out->v = fget_req_info;
    return (SD_SUCCESS);
    }
    								 
  }  /* mvla_fget */                                 

/************************************************************************/
/*		            send_file_open				*/
/* Send a file open request to open source file on the other side       */
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
  }  /* send_file_open */

/************************************************************************/
/*		            u_fopen_done				*/
/* It was indicated that a file open request was made                   */
/* Confirm file open response that was received as a result of a request*/                                           
/************************************************************************/

static ST_VOID u_fopen_done( MVL_REQ_PEND *req)
{
  ST_RET ret;
  MVL_REQ_PEND  *freadreq= NULL;
  MVL_FGET_REQ_INFO  *fget_req_info;
  FOPEN_RESP_INFO  *resp_info;
  ERR_INFO 		*err_ptr;
  REJECT_RESP_INFO 	*rej_ptr;

  fget_req_info = (MVL_FGET_REQ_INFO *) req->v;
  ret = req->result;
  if (ret == SD_SUCCESS)  /* open of source file succeeded */
    {
    resp_info = req->u.fopen.resp_info;
    fget_req_info ->frsmid = resp_info->frsmid;
    fget_req_info ->fsize = resp_info->ent.fsize;
    if ((freadreq = send_file_read (req->net_info, fget_req_info)) != NULL)
      {
      freadreq->v = fget_req_info;
      }
    else
      {
      MVL_LOG_NERR1 ("ERROR: u_fopen_done: send_file_read (%s) failed ",
                     fget_req_info->srcfilename);
      ret = SD_FAILURE;

      /* stop copy file process and report error to user */
      fget_req_info->fget_done = SD_TRUE;
      fget_req_info->fget_error = ret;
      if (fget_req_info->fget_cnf_ptr)
        (*fget_req_info->fget_cnf_ptr)(fget_req_info);  

      }
    } /* file open succeeded */
  else
    { /* file open failed */
    MVL_LOG_NERR1 ("ERROR: u_fopen_done: mvla_fopen (%s) failed ",
                   fget_req_info->srcfilename);

    if (req->result==MVL_ERR_CNF_REJ_ERR)
      {
      rej_ptr = (REJECT_RESP_INFO *) req->event->u.mms.dec_rslt.data_ptr;
      MVLU_LOG_FLOW3 ("MMS REJECT \n  detected_here = %d \n  invoke_known = %d  invoke = %ld",
                     req->reject_info->detected_here,
                     req->reject_info->invoke_known,
                     (ST_LONG) req->reject_info->invoke);

      MVLU_LOG_CFLOW3 ("pdu_type = %d \n  rej_class = %d \n  rej_code = %d",
                     req->reject_info->pdu_type,
                     req->reject_info->rej_class,
                     req->reject_info->rej_code);
      }

    if (req->result==MVL_ERR_CNF_ERR_OK)
      {
      err_ptr = (ERR_INFO *) req->event->u.mms.dec_rslt.data_ptr;
      MVLU_LOG_FLOW2 ("MMS ERROR RESPONSE \n  class = %d, code = %d", 
                      req->error_info->eclass, 
                      req->error_info->code);
      }

    if (fclose (fget_req_info->fp)!=0)  /* close temporary file */
      {
      MVL_LOG_ERR1 ("ERROR: u_fopen_done: fclose of temp file (%s) failed ",
                     fget_req_info->tempfilename);
      }
     
    if (remove (fget_req_info->tempfilename)!=0)  /* get rid of temporary file */
      MVL_LOG_ERR1 ("ERROR: u_fopen_done: remove of temp file (%s) failed ",
                     fget_req_info->tempfilename);

    /* stop copy file process and report error to user */
    fget_req_info->fget_done = SD_TRUE;
    fget_req_info->fget_error = ret;
    if (fget_req_info->fget_cnf_ptr)
      (*fget_req_info->fget_cnf_ptr)(fget_req_info);  
    }  /* file open failed */
 
  mvl_free_req_ctrl (req);
  }  /* u_fopen_done */

/************************************************************************/
/*		            send_file_read				*/
/* Send a file read request to copy a block of the source file data     */
/************************************************************************/

MVL_REQ_PEND *send_file_read (MVL_NET_INFO *NetInfo, 
			      MVL_FGET_REQ_INFO *fget_req_info)
  {
  MVL_REQ_PEND *req= NULL;
  FREAD_REQ_INFO fread_req;

  fread_req.frsmid = fget_req_info->frsmid;  
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
  }  /* send_file_read */


/************************************************************************/
/*		            u_fread_done				*/
/* Confirm the file read response sent to indicate a read request       */
/************************************************************************/

static ST_VOID u_fread_done( MVL_REQ_PEND *req)
  {
  ST_RET ret;
  MVL_REQ_PEND *freadreq= NULL;
  MVL_REQ_PEND *fclosereq= NULL;
  MVL_FGET_REQ_INFO *fget_req_info;
  FREAD_RESP_INFO	*resp_info;

  fget_req_info = (MVL_FGET_REQ_INFO *) req->v;

  ret = req->result;
  if (ret == SD_SUCCESS)  /* read succeeded */
    {
    resp_info = req->u.fread.resp_info;
   
     if ( fwrite (resp_info->filedata, sizeof(ST_CHAR), resp_info->fd_len, fget_req_info->fp) 
          == (unsigned) resp_info->fd_len )
      {
      if ( resp_info->more_follows == SD_TRUE)
        {
        if ((freadreq = send_file_read (req->net_info, fget_req_info)) != NULL)
          {
          freadreq->v = fget_req_info;  /* if more file data request to read another block */
          }
        else
          { /* read failed */
          MVL_LOG_NERR1 ("ERROR: u_fread_done: send_file_read (%s) failed ",
                         fget_req_info->srcfilename);
          ret = SD_FAILURE;

          /* stop copy file process and report error to user */
          fget_req_info->fget_done = SD_TRUE;
          fget_req_info->fget_error = ret;
          if (fget_req_info->fget_cnf_ptr)
            (*fget_req_info->fget_cnf_ptr)(fget_req_info);  
 
          }
        }  /* more file data to be copied */
      else
        {  /* no more file data to be copied */
        if ((fclosereq = send_file_close (req->net_info, fget_req_info)) != NULL)
          {
          fclosereq->v = fget_req_info;  /* no more file data so send a file close request */
          }
        else
          {
          MVL_LOG_NERR1 ("ERROR: u_fread_done: send_file_close (%s) failed ",
                         fget_req_info->srcfilename);
          ret = SD_FAILURE;

          /* stop copy file process and report error to user */
          fget_req_info->fget_done = SD_TRUE;
          fget_req_info->fget_error = ret;
          if (fget_req_info->fget_cnf_ptr)
            (*fget_req_info->fget_cnf_ptr)(fget_req_info);  
 
          }
        }
      }	 /* writing data to temp file succeeded */
    else
      {	 /* write failed */
      MVL_LOG_NERR0 ("ERROR: u_fread_done: fwrite failed ");
      ret = SD_FAILURE;

      /* stop copy file process and report error to user */
      fget_req_info->fget_done = SD_TRUE;
      fget_req_info->fget_error = ret;
      if (fget_req_info->fget_cnf_ptr)
        (*fget_req_info->fget_cnf_ptr)(fget_req_info);  
 
      }	 /* write failed */
    } /* read of data in source file succeeded */
  else
    { /* read failed so close and remove temp file */
    MVL_LOG_NERR1 ("ERROR: u_fread_done: mvla_fread (%s) failed ",
                   fget_req_info->srcfilename);

    if (fclose (fget_req_info->fp)!=0)  /* close temporary file      */
      {
      MVL_LOG_ERR1 ("ERROR: mvla_fget: fclose of temp file (%s) failed ",
                     fget_req_info->tempfilename);
      }

    if (remove (fget_req_info->tempfilename)!=0)  /* get rid of temporary file */
      {
      MVL_LOG_ERR1 ("ERROR: mvla_fget: remove of temp file (%s) failed ",
                     fget_req_info->tempfilename);
      }

    /* stop copy file process and report error to user */
    fget_req_info->fget_done = SD_TRUE;
    fget_req_info->fget_error = ret;
    if (fget_req_info->fget_cnf_ptr)
      (*fget_req_info->fget_cnf_ptr)(fget_req_info);  
 
    }  /* read failed */

  mvl_free_req_ctrl (req);
  }  /* u_fread_done */

/************************************************************************/
/*		            send_file_close				*/
/* No more data to read from the source file so send a request to close */
/* the source file                                                      */
/************************************************************************/

MVL_REQ_PEND *send_file_close (MVL_NET_INFO *NetInfo, 
			       MVL_FGET_REQ_INFO *fget_req_info)
  {
  MVL_REQ_PEND *req= NULL;
  FCLOSE_REQ_INFO fclose_req;

  fclose_req.frsmid = fget_req_info->frsmid;

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
  }  /* send_file_close */

/************************************************************************/
/*		            u_fclose_done				*/
/* Confirm source file closed response                                  */
/************************************************************************/

static ST_VOID u_fclose_done( MVL_REQ_PEND *req)
  {
  ST_RET ret;
  MVL_FGET_REQ_INFO *fget_req_info;

  fget_req_info = (MVL_FGET_REQ_INFO *) req->v;

  ret = req->result;
  if (ret == SD_SUCCESS)    /* close of source file succeeded */
    {
    if (fclose (fget_req_info->fp)!=0)  /* close the temporary file */
      {
      MVL_LOG_ERR1 ("ERROR: mvla_fget: fclose of temp file (%s) failed ",
                     fget_req_info->tempfilename);
      }

    /* Remove dest file so temp file can be renamed to it.  */
    /* NOTE: remove may fail if file did not already exist. */
    remove (fget_req_info->destfilename);

    /* rename the temporary file to become the requested destination file  */ 
    /* use the ANSI rename function if available on your OS                */
    /* otherwise use other function appropriate for your system            */
    if (rename (fget_req_info->tempfilename, fget_req_info->destfilename))
      {
      MVL_LOG_ERR1 ("ERROR: u_fclose_done: rename (%s) failed ",
	             fget_req_info->tempfilename);
      if (remove (fget_req_info->tempfilename)!=0)  /* cleanup useless file */
        {
	MVL_LOG_ERR1 ("ERROR: u_fclose_done: remove (%s) failed ",
	               fget_req_info->tempfilename);
	}
      ret = SD_FAILURE;

      }
    else	
      {
      }
     
    } /* close succeeded */
  else  
    { /* close failed */

    MVL_LOG_NERR1 ("ERROR: u_fclose_done: mvla_fclose (%s) failed ",
                   fget_req_info->srcfilename);

    if (fclose (fget_req_info->fp)!=0)  /* close temporary file      */
      {
      MVL_LOG_ERR1 ("ERROR: mvla_fget: fclose of temp file (%s) failed ",
                     fget_req_info->tempfilename);
      }

    if (remove (fget_req_info->tempfilename)!=0)  /* get rid of temporary file */
      {
      MVL_LOG_ERR1 ("ERROR: mvla_fget: remove of temp file (%s) failed ",
                     fget_req_info->tempfilename);
      }
    }

  /* copy file process complete so report done and status to user */
  fget_req_info->fget_done = SD_TRUE;
  fget_req_info->fget_error = ret;
  if (fget_req_info->fget_cnf_ptr)
    (*fget_req_info->fget_cnf_ptr)(fget_req_info);  
 
  mvl_free_req_ctrl (req);

  } /* end u_fclose_done */


