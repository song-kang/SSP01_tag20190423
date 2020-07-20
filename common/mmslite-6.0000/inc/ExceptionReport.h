/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*                  2003-2204, All Rights Reserved                   	*/
/*									*/
/* MODULE NAME : ExceptionReport.h					*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : Unhandled Exception Handler			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/02/11  DSF           Support for process dump, exception filters	*/
/* 03/10/11  DSF           Allow for non-global instances via compile	*/
/*			   option					*/
/* 07/22/09  DSF     05    Added ExpSetLogFileName and			*/
/*			   ExpSetDumpFileName				*/
/* 01/19/09  NAV     04    Add SetDumpFileName				*/
/* 02/13/06  DSF     03    Migrate to VS.NET 2005			*/
/* 06/30/05  DSF     02    Generate a mini dump				*/
/* 12/20/04  DSF     01    Minor usability changes			*/
/*			   Stack State recorder				*/
/************************************************************************/

#pragma once

#include "glbtypes.h"
#include "sysincs.h"
#include "slog.h"
#include "mem_chk.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <dbghelp.h>
#include <excpt.h>

#ifdef __cplusplus

#include <vector>
#include <string>
using namespace std;

enum BasicType  // Stolen from CVCONST.H in the DIA 2.0 SDK
{
    btNoType = 0,
    btVoid = 1,
    btChar = 2,
    btWChar = 3,
    btInt = 6,
    btUInt = 7,
    btFloat = 8,
    btBCD = 9,
    btBool = 10,
    btLong = 13,
    btULong = 14,
    btCurrency = 25,
    btDate = 26,
    btVariant = 27,
    btComplex = 28,
    btBit = 29,
    btBSTR = 30,
    btHresult = 31
};

class WheatyExceptionReport
{
    public:
    
    WheatyExceptionReport( );
    WheatyExceptionReport( bool terminate );
    ~WheatyExceptionReport( );
    
    void SetLogFileName( PTSTR pszLogFileName );
    void SetDumpFileName (PTSTR pszDumpFileName);

    // where report info is extracted and generated 
    static void GenerateExceptionReport( PEXCEPTION_POINTERS pExceptionInfo );

    // where mini dump is generated 
    static void GenerateDumpFile( PEXCEPTION_POINTERS pExceptionInfo );

    static void GenerateProcessDump ();
    static void GenerateProcessDump (PTSTR pszDumpFileName);

    // entry point where control comes on an unhandled exception
    static LONG WINAPI WheatyUnhandledExceptionFilter(
                                PEXCEPTION_POINTERS pExceptionInfo );

    static BOOL GetLogicalAddress(  PVOID addr, PTSTR szModule, DWORD len,
                                    DWORD& section, DWORD& offset );

    private:

    // Helper functions
    static LPTSTR GetExceptionString( DWORD dwCode );
    static void WriteStackDetails( PCONTEXT pContext, bool bWriteVariables );

    static BOOL CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO,ULONG, PVOID);

    static bool FormatSymbolValue( PSYMBOL_INFO, STACKFRAME *, char * pszBuffer, unsigned cbBuffer );

    static char * DumpTypeIndex( char *, DWORD64, DWORD, unsigned, DWORD_PTR, bool & );

    static char * FormatOutputValue( char * pszCurrBuffer, BasicType basicType, DWORD64 length, PVOID pAddress );
    
    static BasicType GetBasicType( DWORD typeIndex, DWORD64 modBase );

    static int __cdecl _tprintf(const TCHAR * format, ...);

    // Variables used by the class
    static TCHAR m_szLogFileName[MAX_PATH];
    static TCHAR m_szDumpFileName[MAX_PATH];
    static LOG_CTRL m_logCtrl;
    static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
    static HANDLE m_hReportFile;
    static HANDLE m_hProcess;
    static bool m_bTerminate;
};


extern WheatyExceptionReport g_WheatyExceptionReport; //  global instance of class

struct SourceInfo
  {
  string filename;
  long linenum;
  };

struct StackInfo
  {
  DWORD threadID;
  SYSTEMTIME timestamp;
  vector<SourceInfo>	stack;
  
  void Log ();
  };
  
void GetStackInfo (StackInfo& stackInfo);

#endif

#ifdef __cplusplus
extern "C" {
#endif

void ExpInit ();
void ExpRaiseDebugException ();
void ExpSetLogCtrl (LOG_CTRL *pLogCtrl);
void ExpSetLogFileName( PTSTR pszLogFileName );
void ExpSetDumpFileName (PTSTR pszDumpFileName);
void ExpClearExceptionFilter ();
void ExpGenerateProcessDump (PTSTR pszDumpFileName);

int ExpFilter (unsigned int code, struct _EXCEPTION_POINTERS *ep);

#ifdef __cplusplus
}
#endif


