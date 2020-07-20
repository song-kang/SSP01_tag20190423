/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2004 All Rights Reserved					*/
/*									*/
/* MODULE NAME : startup.c						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Function to read "startup.cfg" input file.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			startup_cfg_read				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/07/12  JRB	   Add BRCBBufferSize,LogScanRateMs,LogMaxEntries*/
/*			   Make sure always a complete line is read.	*/
/* 08/03/07  JRB    04     Allow only comma or tab as delimiters.	*/
/*			   Check for empty tokens.			*/
/*			   Add extra error log messages.		*/
/* 05/03/05  JRB    03	   Replace strtok with get_next_string to handle*/
/*			   extra delimiters and quoted strings.		*/
/*			   Allow blanks, tabs in ANY string, but it	*/
/*			   MUST be enclosed in quotes.			*/
/* 08/20/04  JRB    02     Fix ReportScanRate input error checking	*/
/* 07/07/04  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "startup.h"
#include "sx_log.h"
#include "str_util.h"	/* for strn..._safe	protos	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			startup_cfg_read				*/
/* Reads "startup.cfg" input file & fills in STARTUP_CFG struct.	*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
ST_RET startup_cfg_read (
	ST_CHAR *startup_cfg_filename,	/* usually "startup.cfg"	*/
	STARTUP_CFG *startup_cfg)
  {
FILE *in_fp;
ST_CHAR in_buf[256];	/* buffer for one line read from file	*/
ST_CHAR token_buf[256];	/* copy of "in_buf". Modified by parsing code.	*/
ST_CHAR *curptr;		/* ptr to current position in token_buf	*/
char seps[] = ",\t\n";
ST_INT line_num;		/* number of lines in file	*/
ST_RET retcode = SD_SUCCESS;
ST_CHAR *parameter_name;		/* first token on line	*/
ST_CHAR *value;			/* 2nd token	*/

  memset (startup_cfg, 0, sizeof (STARTUP_CFG));	/* CRITICAL: start with clean struct*/

  in_fp = fopen (startup_cfg_filename, "r");
  if (in_fp == NULL)
    {
    SXLOG_ERR1 ("Error opening input file '%s'", startup_cfg_filename);
    return (SD_FAILURE);
    }

  /* Read one line at a time from the input file	*/
  line_num = 0;
  while (fgets (in_buf, sizeof(in_buf) - 1, in_fp) != NULL)
    {
    size_t buf_len;

    line_num++;
    /* if last char in "in_buf" is not '\n', then complete line was not read.*/
    buf_len = strlen(in_buf);
    if (in_buf[buf_len - 1] != '\n')
      {
      SXLOG_ERR2 ("Error on line %d in '%s'. Could not read complete line. Line Ignored.",
                  line_num, startup_cfg_filename);
      continue;
      }

    /* Copy the input buffer to "token_buf". This code modifies the 
     * copied buffer (token_buf). Keep input buffer (in_buf) intact.
     */
    strcpy (token_buf, in_buf);
    curptr = token_buf;	/* init "curptr"	*/
    /* First token must be "ParameterName".	*/
    parameter_name = get_next_string (&curptr, seps);

    /* If NULL, this is empty line. If first char is '#', this is comment line.*/
    if (parameter_name == NULL || parameter_name[0] == '#')
      continue;		/* Ignore empty lines & comment lines	*/
    if (parameter_name [0] == '\0')
      {
      /* First token is empty. This is probably empty line.	*/
      /* Ignore this line, but log error if more tokens found. 	*/
      if ((value = get_next_string (&curptr, seps)) != NULL)
        SXLOG_ERR3 ("Input ignored because first token is empty at line %d in '%s'. Second token='%s'",
                    line_num, startup_cfg_filename, value);
      continue;
      }

    /* Second token must be "Value".	*/
    value          = get_next_string (&curptr, seps);

    if (value && value [0] != '\0')
      {
      if (stricmp (parameter_name, "SCLFileName") == 0)
        strncpy_safe (startup_cfg->scl_filename, value, MAX_IDENT_LEN);
      else if (stricmp (parameter_name, "IEDName") == 0)
        strncpy_safe (startup_cfg->ied_name, value, MAX_IDENT_LEN);
      else if (stricmp (parameter_name, "AccessPointName") == 0)
        strncpy_safe (startup_cfg->access_point_name, value, MAX_IDENT_LEN);
      else if (stricmp (parameter_name, "ReportScanRate") == 0)
        {
        ST_FLOAT ReportScanRateSeconds;
        /* ReportScanRate entered as floating-point Seconds. Convert to (ms).	*/
        if (asciiToFloat   (value, &ReportScanRateSeconds) != SD_SUCCESS
            || ReportScanRateSeconds < 0.0)
          {
          SXLOG_ERR2 ("ReportScanRate='%s' on line %d invalid. Must be positive floating point number of seconds",
                       value, line_num);
          retcode = SD_FAILURE;
          }
        else
          startup_cfg->report_scan_rate = (ST_UINT) (ReportScanRateSeconds * 1000);
        }
      else if (stricmp (parameter_name, "LogScanRateSeconds") == 0)
        {
        ST_FLOAT LogScanRateSeconds;
        /* LogScanRate entered as floating-point Seconds. Convert to (ms).	*/
        if (asciiToFloat   (value, &LogScanRateSeconds) != SD_SUCCESS
            || LogScanRateSeconds < 0.0)
          {
          SXLOG_ERR2 ("LogScanRateSeconds='%s' on line %d invalid. Must be positive floating point number",
                       value, line_num);
          retcode = SD_FAILURE;
          }
        else
          startup_cfg->LogScanRateMs = (ST_UINT) (LogScanRateSeconds * 1000);
        }
      else if (stricmp (parameter_name, "BRCBBufferSize") == 0)
        {
        if (asciiToUint (value, &startup_cfg->BRCBBufferSize) != SD_SUCCESS)
          {
          SXLOG_ERR2 ("BRCBBufferSize='%s' on line %d invalid. Must be unsigned integer",
                       value, line_num);
          retcode = SD_FAILURE;
          }
        }
      else if (stricmp (parameter_name, "LogMaxEntries") == 0)
        {
        if (asciiToUint (value, &startup_cfg->LogMaxEntries) != SD_SUCCESS)
          {
          SXLOG_ERR2 ("LogMaxEntries='%s' on line %d invalid. Must be unsigned integer",
                       value, line_num);
          retcode = SD_FAILURE;
          }
        }
      else
        {
        SXLOG_ERR3 ("Unrecognized ParameterName '%s' at line %d in '%s'.",
                    parameter_name, line_num, startup_cfg_filename);
        retcode = SD_FAILURE;
        }
      }
    else
      {
      SXLOG_ERR2 ("Invalid input at line %d in '%s'. Must contain ParameterName and Value.", line_num, startup_cfg_filename);
      SXLOG_CERR1 ("%s", in_buf);
      retcode = SD_FAILURE;
      }

    /* If ANYTHING failed so far, stop looping.	*/
    if (retcode)
      {
      SXLOG_CERR0 ("Error may be caused by extra delimiter in input treated as empty field");
      break;	/* get out of loop	*/
      }
    }	/* end main "while" loop	*/

  fclose (in_fp);
  return (retcode);
  }
