/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1986 - 2005, All Rights Reserved             		*/
/*									*/
/* MODULE NAME : scrndefs.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : This include file contains definitions needed	*/
/*  for screen handling 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/06/05  EJV     20    linux: no fancy screen.                      */
/* 03/31/03  EJV     19    Removed _WINDOWS code.			*/
/* 03/18/03  JRB     18    Fix QNX #ifdefs.				*/
/* 02/17/03  CRM     17    Added "defined(linux)" code.			*/
/* 02/17/03  JRB     16	   Stop using SYSTEM_SEL define.		*/
/*			   Del QNX_C86 support.				*/
/* 08/24/98  EJV     15    Eliminated semicoln in some macros.		*/
/* 04/29/98  EJV     14    CLEARSCR on sparc beeps.			*/
/* 01/08/98  EJV     13    Deleted CLEARSCR specific to sun sparc	*/
/* 05/16/97  RKR     12    Changes for HP-UX				*/
/* 12/07/95  DSF     11    Minor corrections				*/
/* 11/08/95  MDE     10    Tweaked WINDOWS defines a bit		*/
/* 05/04/95  JRB     09    Added SYS_QNX4 for QNX 4.x port.		*/
/* 05/04/95  JRB     08    Simplified macros:  removed LINE?COL?,	*/
/*			     added GOTOLINE(line) an CLEARLINE(line).	*/
/* 04/20/95  KCW     07    Changes for SUN Solaris                      */
/* 01/05/95  JRB     06    Change BSD to SYS_BSD.			*/
/* 12/09/94  JRB     05    Disable ANSI escape sequences for VXWORKS.	*/
/* 10/12/93  DSF     04    Changed MMSWIN to _WINDOWS			*/
/* 04/08/93  WEO     03    Added MMSWIN defines for screen formatting   */
/* 12/03/92  JRB     02    Added LINE9COL0 for QNX.                     */
/* 04/16/92  WEO     01    Added CLEARSCR for SUN			*/
/* 06/25/91	   5.00    MMSEASE 5.0 release. See UTILML50.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef SCRNDEFS_INCLUDED
#define SCRNDEFS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__QNX__)

#define CLEARSCR  printf( "\33H\33J" )
#define SAVESCR 	printf( "\33S" )
#define BOTTOMSCR	printf( "\33=7 " )	/* "7" means row 23, " " means col 0 */
#define CLEAREOL	printf( "\33K" )
#define GOTOLINE(line)	printf("\33=%c ",line+0x1f)	/* line=1 thru 24*/
#define CLEARLINE(line)	printf("\33=%c \33K",line+0x1f)	/* line=1 thru 24*/

#else	/* non QNX system */

#if defined (_WIN32) || defined (VXWORKS) || \
    defined(__hpux) || defined(sparc) || defined(linux)
#define CLEARSCR        printf("\n")
#define RESTORESCR
#define SAVESCR   
#define BOTTOMSCR 
#define CLEAREOL  
#define GOTOLINE(line)  printf("\n")
#define CLEARLINE(line)
#else
#define CLEARSCR        printf( "\33[2J\33[0;0H" )
#define RESTORESCR	printf( "\33[u" )
#define SAVESCR 	printf( "\33[s" )
#define BOTTOMSCR	printf( "\33[23;0H" )
#define CLEAREOL	printf( "\33[K" )
#define GOTOLINE(line)	printf( "\33[%d;1H", line)	/* line= 1 thru 24*/
#define CLEARLINE(line)	printf( "\33[%d;1H\33[K", line)	/* line= 1 thru 24*/
#endif
#endif

#define PRINTSTR	printf( "%s", str_ptr )
#define ADD_LF		printf( "\n" )

#if defined(_AIX) || defined(sun) || defined(__hpux) ||	defined(linux) \
    || (defined(__alpha) && !defined(__VMS)) \
    || defined(VXWORKS) || defined(__QNX__)
#define CR		'\012'
#else
#define CR		'\015'
#endif

#ifdef __cplusplus
}
#endif

#endif
