/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pfil.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS file services at the primitive	*/
/*	level.								*/
/*									*/
/*	Parameters : In general, the first parameter is the channel	*/
/*		     that the request or response is to be sent over.	*/
/*		     For Responses, the second parameter is the Invoke	*/
/*		     ID to be used. The last parameter is used to pass	*/
/*		     operation specific information by pointer. 	*/
/*									*/
/*	Return								*/
/*	    Values : Request functions return a pointer to the pending	*/
/*		     request tracking structure, of type MMSREQ_PEND.	*/
/*		     In case of error, the pointer is returned == 0	*/
/*		     and mms_op_err is written with the err code.	*/
/*		     Response functions return 0 if OK, else an error	*/
/*		     code.						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/14/97  RKR     01    Format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PFIL_INCLUDED
#define MMS_PFIL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_mp.h"

/************************************************************************/
/************************************************************************/
/* FILE MANAGEMENT SERVICES						*/
/************************************************************************/
/* The structures below are used to pass FILE MANAGEMENT information	*/
/* to/from the MMS primitives.						*/
/************************************************************************/

#define MFSTAT_NO_ACT     0
#define MFSTAT_PEND_OPEN  1
#define MFSTAT_OPENED	  2
#define MFSTAT_PEND_CLOSE 3

/************************************************************************/
/* OBTAIN FILE, Conformance: FIL1					*/
/************************************************************************/

/* REQUEST :								*/

struct obtfile_req_info
  {
  ST_BOOLEAN ar_title_pres; 		/* application title present	*/
  ST_INT     ar_len; 			/* length of app proc title	*/
  ST_UCHAR   *ar_title;			/* application process title	*/
  ST_INT     num_of_src_fname;		/* num of source fname elements	*/
  ST_INT     num_of_dest_fname; 	/* num of dest. fname elements	*/
/*					   list of source file names 	*/
/*  FILE_NAME src_fname_list[ num_of_src_fname ];			*/
/*					   list of dest. file names	*/
/*  FILE_NAME dest_fname_list[ num_of_dest_fname ];			*/
  SD_END_STRUCT
  };
typedef struct obtfile_req_info	OBTFILE_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_obtfile (ST_INT chan, OBTFILE_REQ_INFO *info);
ST_RET mp_obtfile_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_obtfile (OBTFILE_REQ_INFO *info);
ST_RET mpl_obtfile_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* FILE OPEN, Conformance: FIL2 					*/
/************************************************************************/

/* REQUEST :								*/

struct fopen_req_info
  {
  ST_INT    num_of_fname; 		/* num of fname elements	*/
  ST_UINT32 init_pos;			/* initial position		*/
/*					   list of file names		*/
/*  FILE_NAME fname_list[ num_of_fname ];				*/
  SD_END_STRUCT
  };
typedef struct fopen_req_info FOPEN_REQ_INFO;

/* RESPONSE :								*/

struct file_attr
  {
  ST_UINT32  fsize;			/* file size (# bytes)		*/
  ST_BOOLEAN mtimpres;			/* last modified time present	*/
  time_t     mtime; 			/* last modified time		*/
  };
typedef struct file_attr FILE_ATTR;

struct fopen_resp_info
  {
  ST_INT32  frsmid; 			/* file read state machine ID	*/
  FILE_ATTR ent; 		/* file attributes		*/
  };
typedef struct fopen_resp_info FOPEN_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_fopen (ST_INT chan, FOPEN_REQ_INFO *info);
ST_RET mp_fopen_resp (MMSREQ_IND *ind, FOPEN_RESP_INFO *info);
#else
ST_RET mpl_fopen (FOPEN_REQ_INFO *info);
ST_RET mpl_fopen_resp (ST_UINT32 invoke, FOPEN_RESP_INFO *info);
#endif

/************************************************************************/
/* FILE READ, Conformance: FIL2 					*/
/************************************************************************/

/* REQUEST :								*/

struct fread_req_info
  {
  ST_INT32   frsmid; 			/* file read state maching ID	*/
  };
typedef struct fread_req_info FREAD_REQ_INFO;

/* RESPONSE :								*/

struct fread_resp_info
  {
  ST_INT     fd_len; 			/* length of file data octets	*/
  ST_UCHAR   *filedata;			/* pointer to file data octets	*/
  ST_BOOLEAN more_follows; 		/* additional file content	*/
					/*   default: SD_TRUE		*/
  SD_END_STRUCT
  };
typedef struct fread_resp_info FREAD_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_fread (ST_INT chan,FREAD_REQ_INFO *info);
ST_RET mp_fread_resp (MMSREQ_IND *ind, FREAD_RESP_INFO *info);
#else
ST_RET mpl_fread (FREAD_REQ_INFO *info);
ST_RET mpl_fread_resp (ST_UINT32 invoke, FREAD_RESP_INFO *info);
#endif

/************************************************************************/
/* FILE CLOSE, Conformance: FIL2					*/
/************************************************************************/

/* REQUEST  :								*/

struct fclose_req_info
  {
  ST_INT32   frsmid; 			/* file read state maching ID	*/
  };
typedef struct fclose_req_info FCLOSE_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_fclose (ST_INT chan, FCLOSE_REQ_INFO *info);
ST_RET mp_fclose_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_fclose (FCLOSE_REQ_INFO *info);
ST_RET mpl_fclose_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* FILE RENAME, Conformance: FIL3					*/
/************************************************************************/

/* REQUEST :								*/

struct frename_req_info
  {
  ST_INT num_of_cur_fname;		/* num of current fname elements*/
  ST_INT num_of_new_fname; 	   	/* num of new fname elements	*/
/*					   list of current file names 	*/
/*  FILE_NAME cur_fname_list[ num_of_cur_fname ];			*/
/*					   list of new file names	*/
/*  FILE_NAME new_fname_list[ num_of_new_fname ];			*/
  SD_END_STRUCT
  };
typedef struct frename_req_info	FRENAME_REQ_INFO;

/* RESPONSE :  NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_frename (ST_INT chan, FRENAME_REQ_INFO *info);
ST_RET mp_frename_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_frename (FRENAME_REQ_INFO *info);
ST_RET mpl_frename_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* FILE DELETE, Conformance: FIL3					*/
/************************************************************************/

/* REQUEST  :								*/

struct fdelete_req_info
  {
  ST_INT num_of_fname; 			/* num of fname elements	*/
/*					   list of file names		*/
/*  FILE_NAME fname_list[ num_of_fname ];				*/
  SD_END_STRUCT
  };
typedef struct fdelete_req_info	FDELETE_REQ_INFO;

/* RESPONSE :  NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_fdelete (ST_INT chan, FDELETE_REQ_INFO *info);
ST_RET mp_fdelete_resp (MMSREQ_IND *info);
#else
ST_RET mpl_fdelete (FDELETE_REQ_INFO *info);
ST_RET mpl_fdelete_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* FILE DIRECTORY, Conformance: FIL3					*/
/************************************************************************/

/* REQUEST :								*/

struct fdir_req_info
  {
  ST_BOOLEAN filespec_pres; 		/* file specification present	*/
  ST_BOOLEAN cont_after_pres;		/* continue after name present	*/
  ST_INT     num_of_fs_fname;		/* num of fname elements        */
  ST_INT     num_of_ca_fname; 	   	/* num of cont after elements	*/
/*					   list of current file names 	*/
/*  FILE_NAME fs_fname_list[ num_of_fs_fname ];				*/
/*					   list of new file names	*/
/*  FILE_NAME ca_fname_list[ num_of_ca_fname ];				*/
  SD_END_STRUCT
  };
typedef struct fdir_req_info FDIR_REQ_INFO;

/* RESPONSE :								*/

struct fdir_dir_ent
  {
  ST_UINT32  fsize; 			/* file size (# bytes)		*/
  ST_BOOLEAN mtimpres;			/* last modified time present	*/
  time_t     mtime; 			/* last modified time		*/
  ST_INT     num_of_fname; 		/* num of fname elements	*/
/*					   list of file names		*/
/*  FILE_NAME fname_list[ num_of_fname ];				*/
  SD_END_STRUCT
  };
typedef struct fdir_dir_ent FDIR_DIR_ENT;

struct fdir_resp_info
  {
  ST_INT     num_dir_ent;		/* number of directory entries	*/
  ST_BOOLEAN more_follows;		/* more dir entries follow	*/
					/*   default: SD_FALSE		*/
/*					   list of directory entries	*/
/*  FDIR_DIR_ENT  dir_ent_list [num_dir_ent]				*/
  SD_END_STRUCT
  };
typedef struct fdir_resp_info FDIR_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_fdir (ST_INT chan,FDIR_REQ_INFO *info);
ST_RET mp_fdir_resp (MMSREQ_IND *ind, FDIR_RESP_INFO *info);
#else
ST_RET mpl_fdir (FDIR_REQ_INFO *info);
ST_RET mpl_fdir_resp (ST_UINT32 invoke, FDIR_RESP_INFO *info);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_PFIL_INCLUDED */


