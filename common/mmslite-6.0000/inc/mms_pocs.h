/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pocs.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS operator communication services	*/
/*	at the primitive level.	     					*/
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

#ifndef MMS_POCS_INCLUDED
#define MMS_POCS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_mp.h"

/************************************************************************/
/************************************************************************/
/* OPERATOR COMMUNICATION FUNCTIONS					*/
/************************************************************************/
/* The structures below are used to pass OPERATOR COMMUNICATION 	*/
/* operation information to/from the MMS primitives.			*/
/************************************************************************/

/************************************************************************/
/* OUTPUT OPERATION, Conformance: OCS1					*/
/************************************************************************/

/* REQUEST :								*/

struct output_req_info
  {
  ST_CHAR station_name [MAX_IDENT_LEN+1]; /* operator station name	*/
  ST_INT  data_count; 			/* number of data strings	*/
/*				list of pointers to the output data	*/
/*  ST_CHAR *output_data [data_count];					*/
  SD_END_STRUCT
  };
typedef struct output_req_info OUTPUT_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_output (ST_INT chan, OUTPUT_REQ_INFO *info);
ST_RET mp_output_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_output (OUTPUT_REQ_INFO *info);
ST_RET mpl_output_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* INPUT OPERATION, Conformance: OCS2					*/
/************************************************************************/

/* REQUEST :								*/

struct input_req_info
  {
  ST_CHAR    station_name [MAX_IDENT_LEN+1]; /* operator station name	*/
  ST_BOOLEAN echo;     			/* echo, default: SD_TRUE	*/
  ST_BOOLEAN timeout_pres;		/* input timeout present ind	*/
  ST_UINT32  timeout;			/* input timeout		*/
  ST_BOOLEAN prompt_pres;		/* prompt data present ind      */
  ST_INT     prompt_count;		/* number of prompt strings	*/
/*				list of pointers to the prompt data	*/
/*  ST_CHAR *prompt_data [prompt_count];					*/
  SD_END_STRUCT
  };
typedef struct input_req_info INPUT_REQ_INFO;

/* RESPONSE :								*/

struct input_resp_info
  {
  ST_CHAR *input_resp;			/* input response		*/
  };
typedef struct input_resp_info INPUT_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_input (ST_INT chan, INPUT_REQ_INFO *info);
ST_RET mp_input_resp (MMSREQ_IND *ind, INPUT_RESP_INFO *info);
#else
ST_RET mpl_input (INPUT_REQ_INFO *info);
ST_RET mpl_input_resp (ST_UINT32 invoke, INPUT_RESP_INFO *info);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_POCS_INCLUDED */
