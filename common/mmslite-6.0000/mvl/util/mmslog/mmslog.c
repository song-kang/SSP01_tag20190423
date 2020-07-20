/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 1993, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mmslog.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 07/23/08  GLB     25    Update Copyright.                            */
/* 02/14/07  GLB     24    Added welcome() to print Copyright info      */
/* 01/30/06  GLB     23    Integrated porting changes for VMS           */
/* 12/19/05  JRB     22    Chg cr to cr_mmslog, del static to fix warning.*/
/* 02/23/05  JRB     21    Add m_lite_data_algn_tbl, m_struct_* globals	*/
/*			   (removed from mms_tdef).			*/
/*			   Set m_data_algn_tbl (no longer set in mms_tdef)*/
/* 12/09/02  MDE     20    Upped the runtime limit to 5000, again	*/
/* 04/14/03  JRB     19    Eliminate compiler warnings.			*/
/* 04/03/03  JRB     18    Add dummy u_ml_get_rt_type function.		*/
/* 12/09/02  MDE     17    Upped the runtime limit to 5000		*/
/* 11/11/02  JRB     16    Replace ms_asn1_to.. w/ ms_runtime_create.	*/
/* 07/24/02  MDE     15    Added MMSLOG_DONE event			*/
/* 04/30/02  MDE     14    Removed limits on # vars, etc..		*/
/* 04/05/02  MDE     13    Minor changes to work with latest Lite 	*/
/* 01/25/01  MDE     12    Added -a flag to decode data			*/
/* 07/31/01  MDE     11    Added -b flag to write binary		*/
/* 10/06/00  MDE     10    Added -l flag to see all SLOG lines		*/
/* 05/01/00  RKR     09    Updated static copyright string		*/
/* 04/12/00  MDE     08    Allow decoding typespecs (optional)		*/
/* 01/21/00  MDE     07    Increased capacities				*/
/* 09/07/99  MDE     06    Added 'findPdu' option			*/
/* 04/01/99  MDE     05    Changes to decode buffer allocation scheme   */
/* 03/03/99  MDE     04    Changes to work with new SLOG		*/
/* 08/14/98  MDE     03    Set m_rt_type_limit = 500			*/
/* 07/16/98  MDE     02    Now preserve original SLOG file header line	*/
/* 06/06/97  MDE     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "arg.h"
#include "mloguser.h"
#include "readhex.h"
#if defined (_WIN32)
#include <windows.h>
#include <process.h>
#endif

/************************************************************************/
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
/************************************************************************/

static ST_VOID welcome (ST_VOID);
ST_VOID m_set_log_cfg (ST_VOID);
ST_RET log_mms_pdu   (ST_UCHAR *pdu_buf, ST_INT pdu_len);
ST_VOID setSlogHex (ST_INT argNum, ST_CHAR *arg);
ST_VOID setNgHex (ST_INT argNum, ST_CHAR *arg);
ST_VOID setNsHex (ST_INT argNum, ST_CHAR *arg);
ST_VOID setPlainHex (ST_INT argNum, ST_CHAR *arg);
ST_VOID logAllSlogHeader (ST_INT argNum, ST_CHAR *arg);
ST_VOID writeBinary (ST_INT argNum, ST_CHAR *arg);

ST_VOID *_calloc_os (ST_UINT num, ST_UINT size);
ST_VOID *_realloc_os (ST_VOID *old, ST_UINT new_size);
ST_VOID  _free_os (ST_VOID *buf);

/************************************************************************/

#define MMS_BUFFER_SIZE  0xFFFF
ST_CHAR  mms_dec_buffer[MMS_BUFFER_SIZE];
MMSDEC_INFO rslt;

#define MAX_PDU_LEN 0xFFFF
char pduBuf[MAX_PDU_LEN];

ST_CHAR srcFileName[200];
ST_CHAR destFileName[200] = "mmslog.log";

ST_INT fileType = SLOG_FILE;
ST_BOOLEAN logAllSLogHeader;
ST_BOOLEAN findPdu = SD_FALSE;

#define _MAX_RT		5000
static ST_BOOLEAN tryTypes = SD_FALSE;
static ST_BOOLEAN tryData = SD_FALSE;
static ST_BOOLEAN binaryOut = SD_FALSE;
static FILE *binFp;

ST_VOID *_mvl_curr_net_info; /* MVL_NET_INFO *_mvl_curr_net_info; */
ST_BOOLEAN mvl_conn_filtered_logging;

/************************************************************************/

ST_VOID printHelp (ST_INT argNum, ST_CHAR *arg);
ST_VOID setDecodeLogging (ST_INT argNum, ST_CHAR *arg);
ST_VOID setFindPdu (ST_INT argNum, ST_CHAR *arg);
ST_VOID setDecTypes (ST_INT argNum, ST_CHAR *arg);
ST_VOID setDecData (ST_INT argNum, ST_CHAR *arg);
ST_VOID setOutFile (ST_INT argNum, ST_CHAR *arg);
ST_VOID getFileName (ST_INT argNum, ST_CHAR *arg);
ST_VOID setOverwrite (ST_INT argNum, ST_CHAR *arg);
ST_VOID init_mem (ST_VOID);

struct arg_ctrl argCtrl[] =
  {
    {"-?", "List Valid Arguments", printHelp},
    {"-o", "Overwrite log file", setOverwrite},
    {"-w", "{fileName} Output file", setOutFile},
    {"-l", "Log all SLOG header lines", logAllSlogHeader},
    {"-d", "Enable Decode Logging", setDecodeLogging},
    {"-f", "Find MMS PDU in hex buffers", setFindPdu},
    {"-t", "Try decoding as MMS Typedef", setDecTypes},
    {"-a", "Try decoding as MMS Data", setDecData},
    {"-b", "Write as binary data", writeBinary},
    {"-hex", "Input file is simple hex", setPlainHex},
    {"-slog", "Input file is from SLOG (default)", setSlogHex},
    {"-ns", "Input file is from Netsight", setNsHex},
    {"-ng", "Input file is from Network General", setNgHex},
    {"*", "Input File Name", getFileName}
  };
ST_INT numArgStrings = sizeof(argCtrl)/sizeof(struct arg_ctrl);
ST_LONG pduCount;
ST_LONG okCount;
ST_LONG byteCount;
ST_LONG typeCount;

/* Normally Foundry creates these. This app doesn't use Foundry,	*/
/* so they must be here. The values don't matter much because they are	*/
/* only used for creating temporary types for logging, so we just use	*/
/* the _WIN32 values.							*/
ST_INT m_struct_start_algn_mode=M_STRSTART_MODE_LARGEST;
ST_INT m_struct_end_algn_mode  =M_STREND_MODE_LARGEST;
ST_INT m_lite_data_algn_tbl[NUM_ALGN_TYPES] =
  {
  0x0000,	/* ARRSTRT_ALGN	00  */
  0x0000,	/* ARREND_ALGN	01  */
  0x0000,       /* STRSTRT_ALGN 02  */
  0x0000,	/* STREND_ALGN	03  */
  0x0000,	/* INT8_ALGN	04  */
  0x0001,	/* INT16_ALGN	05  */
  0x0003,	/* INT32_ALGN	06  */
  0x0007,	/* INT64_ALGN	07  */
  0x0003,	/* FLOAT_ALGN	08  */
  0x0007,	/* DOUBLE_ALGN	09  */
  0x0000,	/* OCT_ALGN	10  */
  0x0000,	/* BOOL_ALGN	11  */
  0x0000,	/* BCD1_ALGN	12  */
  0x0001,	/* BCD2_ALGN	13  */
  0x0003,	/* BCD4_ALGN	14  */
  0x0000,	/* BIT_ALGN	15  */
  0x0000	/* VIS_ALGN	16  */
  };   

/************************************************************************/
/************************************************************************/

ST_VOID setDoneSem (ST_VOID);
#ifdef _WIN32
static HANDLE doneEventSem;
#endif

int main (int argc,char *argv[])
  {
ST_RET ret;

  welcome ();
  m_data_algn_tbl = m_lite_data_algn_tbl;	/* set data alignment rules*/

#ifdef _WIN32
  doneEventSem = CreateEvent(NULL,         // no security attributes
      			    FALSE,	   // manual-reset event
      			    FALSE,         // initial state is not signaled
			    "MMSLOG_DONE"); 
#endif

  if (process_args (argc, argv) != SD_SUCCESS)
    {
    setDoneSem ();
    exit (1);
    }

  if (srcFileName[0] == 0)
    {
    printHelp (1,NULL);
    setDoneSem ();
    exit(2);
    }

  init_mem ();
  m_set_log_cfg ();
  m_calloc_os_fun = _calloc_os;
  m_realloc_os_fun = _realloc_os;
  m_free_os_fun = _free_os;

  if (binaryOut == SD_TRUE)
    {
    binFp = fopen ("mmslog.bin", "wb");
    if (binFp == NULL)
      {
      printf ("\nError, could not open binary output file");
      setDoneSem ();
      exit (5);
      }
    }

#if 0
  m_cl_read_spec_in_result = 1000;
  m_cl_max_read_var_spec = 1000;
  m_cl_max_read_acc_rslt = 4000;
  m_cl_max_write_rslts = 1000;
  m_cl_max_info_acc_rslt = 1000;
  m_cl_max_info_var_spec = 1000;
  m_cl_max_namel_names = 1000;
  m_cl_max_getcl_cap = 1000;
  m_cl_max_getpi_doms = 1000;
  m_cl_max_getvla_vars = 1000;
  m_cl_max_getdom_cap = 1000;
  m_cl_max_getdom_pi = 1000;
  m_cl_max_initupl_cap = 1000;
  m_cl_max_event_enrollment = 1000;
  m_cl_max_file_directory = 1000;
  m_cl_max_alarm_summary = 1000;
  m_cl_max_journal_entries = 1000;
  m_cl_max_entry_content = 1000;
  m_cl_max_enrollment_summary = 1000;
  m_cl_max_semaphore_entries = 1000;
  m_cl_max_named_tokens = 1000;
  m_sv_max_read_var_spec = 1000;
  m_sv_max_write_var_spec = 1000;
  m_sv_max_write_data = 1000;
  m_sv_max_vstr = 1000;
  m_sv_max_loadd_cap = 1000;
  m_sv_max_file_names = 1000;
  m_sv_max_rqdlnl_cap = 1000;
  m_sv_max_file_names = 1000;
  m_sv_max_file_names = 1000;
  m_sv_max_obj_name = 1000;
  m_sv_max_file_names = 1000;
  m_sv_max_file_names = 1000;
  m_sv_max_initdnld_cap = 1000;
  m_sv_max_prompt_count = 1000;
  m_sv_max_num_vars = 1000;
  m_sv_max_file_names = 1000;
  m_sv_max_file_names = 1000;
  m_sv_max_data_count = 1000;
  m_sv_max_obj_name = 1000;
  m_sv_max_obj_name = 1000;
  m_sv_max_obj_name = 1000;
  m_sv_max_entry_content = 1000;
  m_sv_max_data_vars = 1000;
  m_sv_max_jread_vars = 1000;
#endif
  m_rt_type_limit = 5000;


  pduCount = 0;
  printf ("\n");
  ret = readHex (srcFileName, pduBuf, MAX_PDU_LEN, fileType);
  if (ret != SD_SUCCESS)
    printf ("\n\n Error reading PDU file %s, Logged %ld MMS PDU's\n", 
					argv[1], pduCount);
  else
    {
    if (binaryOut == SD_FALSE)
      {
      printf ("\n\n Success - Extracted %ld MMS PDU's, %ld decoded OK\n", 
				pduCount, okCount);
      }
    else
      {
      printf ("\n\n Success - Extracted %ld PDU's, wrote %ld bytes\n", 
				pduCount, byteCount);
      }

    }

  if (binaryOut == SD_TRUE)
    fclose (binFp);


  setDoneSem ();
  return (0);
  }

/************************************************************************/
/************************************************************************/

ST_VOID setDoneSem ()
  {
#ifdef _WIN32
  if (doneEventSem != NULL)
    {
    SetEvent (doneEventSem);
    CloseHandle (doneEventSem);
    }
#endif
  }

/************************************************************************/
/*			is_mms_pdu_start_char 				*/
/************************************************************************/

ST_BOOLEAN is_mms_pdu_start_char (ST_UCHAR *pdu_buf)
  {
ST_UCHAR c;

  c = *pdu_buf;
  if (c == 0xA0 || c == 0xA1 || c == 0xA2 || c == 0xA3 || c == 0xA4 ||
      c == 0x85 || c == 0x86 || c == 0xA7 || c == 0xA8 || c == 0xA9 ||
      c == 0xAA || c == 0x8B || c == 0x8C || c == 0xAD)
    {
    return (SD_TRUE);
    }

  return (SD_FALSE);
  }

/************************************************************************/
/************************************************************************/
/* This function is called by the system executive when a MMS message	*/
/* has been received.							*/
/************************************************************************/

ST_VOID u_readhex_pdu (ST_UCHAR *pdu_buf, ST_INT pdu_len)
  {
ST_INT i;
ST_INT start_pdu_len;
ST_BOOLEAN found_pdu;
ST_BOOLEAN pduDecOk;
ST_INT bytes_written;
RUNTIME_CTRL *rt_ctrl;

  ++pduCount;

  if (binaryOut == SD_TRUE)
    {
    bytes_written = fwrite (pdu_buf, 1, pdu_len, binFp);
    byteCount += bytes_written;
    printf ("\rTotal PDU: %ld, Total Bytes: %ld", pduCount, byteCount);
    return;
    }

  if (tryData == SD_TRUE) 
    {		/* See if this is MMS data */
    SLOGALWAYS0 ("Decoding PDU as data");
    m_log_data (pdu_len, pdu_buf);
    return;
    }

/* some kind of message was received, need to decode and act on it	*/
  pduDecOk = SD_FALSE;
  if (findPdu == SD_FALSE)
    {
    if (is_mms_pdu_start_char (pdu_buf))
      {
      _ms_mms_decode (pdu_buf, pdu_len, &rslt);
      if (rslt.err_code == NO_DECODE_ERR)
        {
        pduDecOk = SD_TRUE;
        SLOGALWAYS0 ("");
        if (fileType == SLOG_FILE)
          {
          if (logAllSLogHeader == SD_TRUE)
            {
            for (i = 0; i < numSlogHeaderLines; ++i)
              SLOGALWAYS1 ("%s", slogHeaderLines[i]);
       	    SLOGALWAYS0 ("");
            }
          else
            SLOGALWAYS1 ("%s", slogHeaderLines[0]);
          }
        ml_log_dec_rslt (&rslt);
        ++okCount;
        }
      }

    if (pduDecOk == SD_FALSE && tryTypes == SD_TRUE) 
      {		/* See if this is a MMS type spec */
      rt_ctrl = ms_runtime_create (pdu_buf, pdu_len, _MAX_RT);
      if (rt_ctrl) 
        {
        SLOGALWAYS0 ("");
        SLOGALWAYS0 ("Found Type Spec:");
        if (fileType == SLOG_FILE)
          {
          if (logAllSLogHeader == SD_TRUE)
            {
            for (i = 0; i < numSlogHeaderLines; ++i)
              SLOGALWAYS1 ("%s", slogHeaderLines[i]);
       	    SLOGALWAYS0 ("");
            }
          else
            SLOGALWAYS1 ("%s", slogHeaderLines[0]);
          }
        ms_log_runtime (rt_ctrl->rt_first, rt_ctrl->rt_num);
        ms_runtime_destroy (rt_ctrl);
        ++typeCount;
        }
      }

    if (pduDecOk == SD_FALSE && tryData == SD_TRUE) 
      {		/* See if this is MMS data */
      m_log_data (pdu_len, pdu_buf);
      }
    }
  else	/* We are searching for any and all MMS PDU's in this hex pile	*/
    {
    found_pdu = SD_FALSE;
    start_pdu_len = pdu_len;
    while (pdu_len >= 0)
      {
      if (is_mms_pdu_start_char (pdu_buf))
        {
        _ms_mms_decode (pdu_buf, pdu_len, &rslt);
        if (rslt.err_code == NO_DECODE_ERR)
          {
          SLOGALWAYS0 ("");

          if (found_pdu == SD_FALSE)
            {
            if (fileType == SLOG_FILE)
              {
              if (logAllSLogHeader == SD_TRUE)
                {
                for (i = 0; i < numSlogHeaderLines; ++i)
            	  SLOGALWAYS1 ("%s", slogHeaderLines[i]);
          	SLOGALWAYS0 ("");
                }
              else
                SLOGALWAYS1 ("%s", slogHeaderLines[0]);
              }
            }

          if (start_pdu_len != pdu_len)
            {
            SLOGALWAYS2 ("Found MMS PDU at offset %d (0x%x)",
		          	start_pdu_len - pdu_len, 
		          	start_pdu_len - pdu_len); 
            SLOGALWAYS0 ("");
            }
          if (found_pdu == SD_TRUE)
            {
            SLOGALWAYS0 ("Warning: Multiple MMS PDU's found");
            }
          ml_log_dec_rslt (&rslt);
          found_pdu = SD_TRUE;
          ++okCount;
          }
        }
      --pdu_len;
      ++pdu_buf;
      }
    }
  printf ("\rTotal PDU: %ld, Decoded OK: %ld", pduCount, okCount);
  if (tryTypes == SD_TRUE) 
    printf ("\nTypes Decoded OK: %ld", typeCount);
  }

/************************************************************************/
/*			u_ml_get_rt_type				*/
/* "ms_runtime_create" calls this function to find a referenced type.	*/
/* This app is compiled with FOUNDRY defined, so it must have its	*/
/* own customized version of this function. It is not expected that 	*/
/* this function will ever be called, so it just calls assert.		*/
/************************************************************************/

ST_RET u_ml_get_rt_type (ASN1_DEC_CTXT *aCtx, OBJECT_NAME *type_name,
		  RUNTIME_TYPE **rt_out, ST_INT *num_rt_out)
  {
  assert (0);
  return (SD_FAILURE);
  }

/************************************************************************/
/*			m_set_log_cfg					*/
/* sLogCtrl points to a pre-allocated LOG_CTRL structure so only have	*/
/* to fill it in.							*/
/************************************************************************/

ST_VOID m_set_log_cfg (ST_VOID)
  {
/* Use File logging							*/
  sLogCtrl->logCtrl = LOG_FILE_EN;
/* Use time/date time log						*/
  sLogCtrl->logCtrl |= LOG_TIME_EN;
/* File Logging Control defaults 					*/
  sLogCtrl->fc.fileName = destFileName;
  sLogCtrl->fc.maxSize = 100000000L;
  sLogCtrl->fc.ctrl |= (FIL_CTRL_WIPE_EN | FIL_CTRL_WRAP_EN);
  sLogCtrl->fc.ctrl &= ~FIL_CTRL_MSG_HDR_EN;
  }

/************************************************************************/
/************************************************************************/
/*                              init_mem                                */
/************************************************************************/

static ST_VOID mem_chk_error_detected (ST_VOID);
static ST_VOID *my_malloc_err (ST_UINT size);
static ST_VOID *my_calloc_err (ST_UINT num, ST_UINT size);
static ST_VOID *my_realloc_err (ST_VOID *old, ST_UINT size);

static ST_CHAR *spareMem;

ST_VOID init_mem ()
  {
#if defined(NO_GLB_VAR_INIT)
  mvl_init_glb_vars ();
#endif

/* Allocate spare memory to allow logging/printing memory errors        */
  spareMem = (ST_CHAR *) malloc (500);

/* trap mem_chk errors                                                  */
  mem_chk_err = mem_chk_error_detected;
  m_memerr_fun = my_malloc_err;
  c_memerr_fun = my_calloc_err;
  r_memerr_fun = my_realloc_err;

  }

/************************************************************************/
/* This function is called from the DEBUG version of the mem library    */
/* when an error of any type is detected.                               */
/************************************************************************/

static ST_INT memErrDetected;

static ST_VOID mem_chk_error_detected (ST_VOID)
  {
  if (!memErrDetected)
    {
    free (spareMem);
    memErrDetected = SD_TRUE;
    printf ("\n Memory Error Detected! Check log file\n");
    dyn_mem_ptr_status ();
    }
  }

/************************************************************************/
/* Memory Allocation Error Handling Functions.                          */
/* These functions are called from mem_chk when it is unable to         */
/* perform the requested operation. These functions must either return  */
/* a valid buffer or not return at all.                                 */
/************************************************************************/

static ST_VOID *my_malloc_err (ST_UINT size)
  {
  mem_chk_error_detected ();
  exit (1);
  return (NULL);   
  }

static ST_VOID *my_calloc_err (ST_UINT num, ST_UINT size)
  {
  mem_chk_error_detected ();
  exit (1);
  return (NULL);   
  }

static ST_VOID *my_realloc_err (ST_VOID *old, ST_UINT size)
  {
  mem_chk_error_detected ();
  exit (1);
  return (NULL);   
  }

/************************************************************************/

ST_VOID printHelp (ST_INT argNum, ST_CHAR *arg)
  {
  printf ("\n\n mmslog usage:");
  listValidArgs ();
  }

ST_VOID setDecodeLogging (ST_INT argNum, ST_CHAR *arg)
  {
  mms_debug_sel |= MMS_LOG_DEC;
  mms_debug_sel |= MMS_LOG_NERR;
  asn1_debug_sel |= ASN1_LOG_DEC;
  asn1_debug_sel |= ASN1_LOG_NERR;
  sLogCtrl->fc.ctrl |= FIL_CTRL_MSG_HDR_EN;
  }

ST_VOID setFindPdu (ST_INT argNum, ST_CHAR *arg)
  {
  findPdu = SD_TRUE;
  }

ST_VOID setOutFile (ST_INT argNum, ST_CHAR *arg)
  {
  strcpy (destFileName, &arg[2]);
  }

ST_VOID setDecTypes (ST_INT argNum, ST_CHAR *arg)
  {
  tryTypes = SD_TRUE;
  }

ST_VOID setDecData (ST_INT argNum, ST_CHAR *arg)
  {
  tryData = SD_TRUE;
  }

ST_VOID getFileName (ST_INT argNum, ST_CHAR *arg)
  {
  strcpy (srcFileName, arg);
  }

ST_VOID setOverwrite (ST_INT argNum, ST_CHAR *arg)
  {
  sLogCtrl->fc.ctrl |= FIL_CTRL_NO_APPEND;
  }


ST_VOID setSlogHex (ST_INT argNum, ST_CHAR *arg)
  {
  fileType = SLOG_FILE;
  sLogCtrl->fc.ctrl &= ~FIL_CTRL_MSG_HDR_EN;
  }

ST_VOID setNgHex (ST_INT argNum, ST_CHAR *arg)
  {
  fileType = NETWORK_ANALYZER;
  sLogCtrl->fc.ctrl |= FIL_CTRL_MSG_HDR_EN;
  }

ST_VOID setNsHex (ST_INT argNum, ST_CHAR *arg)
  {
  fileType = NETSIGHT;
  sLogCtrl->fc.ctrl |= FIL_CTRL_MSG_HDR_EN;
  }

ST_VOID setPlainHex (ST_INT argNum, ST_CHAR *arg)
  {
  fileType = PLAIN_HEX;
  sLogCtrl->fc.ctrl |= FIL_CTRL_MSG_HDR_EN;
  }

ST_VOID logAllSlogHeader (ST_INT argNum, ST_CHAR *arg)
  {
  logAllSLogHeader = SD_TRUE;
  }

ST_VOID writeBinary (ST_INT argNum, ST_CHAR *arg)
  {
  binaryOut  = SD_TRUE;
  }

/************************************************************************/
/*                       welcome                                                */
/************************************************************************/

static ST_VOID welcome ()
  {
  printf ("\n SISCO copyright (c) Copyright Systems Integration Specialists Company, Inc.,");
  printf ("\n                  1986 - 2008, All Rights Reserved");
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/

ST_VOID *_calloc_os (ST_UINT num, ST_UINT size)
  {
  return (chk_calloc (1,size));
  }


ST_VOID *_realloc_os (ST_VOID *old, ST_UINT new_size)
  {
  return (chk_realloc (old,new_size));
  }


ST_VOID  _free_os (ST_VOID *buf)
  {
  chk_free (buf);
  }



