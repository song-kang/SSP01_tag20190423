/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*                  2003-2204, All Rights Reserved                   	*/
/*									*/
/* MODULE NAME : ExceptionHandler.cpp					*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : Unhandled Exception Handler			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/29/11  DSF           Minor tweaks					*/
/* 11/02/11  DSF           Support for process dump, exception filters	*/
/* 03/10/11  DSF           Allow for non-global instances via compile	*/
/*			   option					*/
/* 07/22/09  DSF     10    Added ExpSetLogFileName and			*/
/*			   ExpSetDumpFileName				*/
/* 01/19/09  NAV     09    Add SetDumpFileName				*/
/* 12/11/08  DSF     08    Migrated to VS2008				*/
/* 03/29/07  DSF     07	   Fixed a print bug				*/
/* 02/13/06  DSF     06    Migrate to VS.NET 2005			*/
/* 06/30/05  DSF     05    Generate a mini dump and terminate		*/
/* 02/18/05  DSF     04    Trap _purecall () and force an access	*/
/*			   violation					*/
/* 01/08/05  DSF     03    Check for null hMod in GetLogicalAddress ()	*/
/* 12/20/04  DSF     02    Minor usability changes			*/
/*			   Stack State recorder				*/
/* 11/16/04  DSF     01    Added ExpSetLogCtrl () to keep linker happy	*/
/************************************************************************/

#define thisFileName	__FILE__

#define WIN32_LEAN_AND_MEAN
#if (_MSC_VER < 1500)
#define __out_xcount(x)
#endif
#include "ExceptionReport.h"

#pragma warning(disable : 4311 4312)

#pragma comment(linker, "/defaultlib:dbghelp.lib")

#define	EXCEPTION_DIAGNOSTIC_DEBUG 	0xE0000001

//============================== Global Variables =============================

//
// Declare the static variables of the WheatyExceptionReport class
//
TCHAR WheatyExceptionReport::m_szLogFileName[MAX_PATH];
TCHAR WheatyExceptionReport::m_szDumpFileName[MAX_PATH];
LOG_CTRL WheatyExceptionReport::m_logCtrl = {LOG_FILE_EN | LOG_TIMEDATE_EN | LOG_TIME_EN,
			    {1000000, "ExceptionReport.log", 
    			     FIL_CTRL_WIPE_EN | 
                   	     FIL_CTRL_WRAP_EN | 
                   	     FIL_CTRL_MSG_HDR_EN,
			     0, NULL}};
LPTOP_LEVEL_EXCEPTION_FILTER WheatyExceptionReport::m_previousFilter;
HANDLE WheatyExceptionReport::m_hReportFile;
HANDLE WheatyExceptionReport::m_hProcess;
bool WheatyExceptionReport::m_bTerminate = false;

// Declare global instance of class
#if !defined (NON_GLOBAL_EXP_REPORT)
WheatyExceptionReport g_WheatyExceptionReport;
#else
WheatyExceptionReport *g_pWheatyExceptionReport;
#endif

#define USE_FILE	1

//=========================================================================
// _purecall: traps R6025
//=========================================================================
void ExpPureCallHandler(void)
  {
  int *p = (int *) 0xdeadbeef;
  *p = 0;
  }

//============================== Class Methods =============================

WheatyExceptionReport::WheatyExceptionReport( )   // Constructor
{
    // Install the unhandled exception filter function
    m_previousFilter =
        SetUnhandledExceptionFilter(WheatyUnhandledExceptionFilter);

    // Figure out what the report file will be named, and store it away
    GetModuleFileName( 0, m_szLogFileName, MAX_PATH );
    _tcscpy (m_szDumpFileName, m_szLogFileName);

    // Look for the '.' before the "EXE" extension.  Replace the extension
    // with "RPT"
    PTSTR pszDot = _tcsrchr( m_szLogFileName, _T('.') );
    if ( pszDot )
    {
        pszDot++;   // Advance past the '.'
        if ( _tcslen(pszDot) >= 3 )
            _tcscpy( pszDot, _T("RPT") );   // "RPT" -> "Report"
    }
    pszDot = _tcsrchr( m_szDumpFileName, _T('.') );
    if ( pszDot )
    {
        pszDot++;   // Advance past the '.'
        if ( _tcslen(pszDot) >= 3 )
            _tcscpy( pszDot, _T("DMP") );   // "DMP" -> "Dump"
    }

  m_logCtrl.fc.fileName = new char[sizeof (m_szLogFileName) + 1];
  strcpy (m_logCtrl.fc.fileName, (const char *) m_szLogFileName);

  m_hProcess = GetCurrentProcess();

  _set_purecall_handler (ExpPureCallHandler);
    
#if defined (NON_GLOBAL_EXP_REPORT)    
    g_pWheatyExceptionReport = this;
#endif    
}

WheatyExceptionReport::WheatyExceptionReport( bool terminate) // Constructor
{
    WheatyExceptionReport::m_bTerminate = terminate;
    WheatyExceptionReport ();
}

//============
// Destructor 
//============
WheatyExceptionReport::~WheatyExceptionReport( )
{
    SetUnhandledExceptionFilter( m_previousFilter );
}

//==============================================================
// Lets user change the name of the report file to be generated 
//==============================================================
void WheatyExceptionReport::SetLogFileName( PTSTR pszLogFileName )
{
    _tcscpy( m_szLogFileName, pszLogFileName );
    m_logCtrl.fc.fileName = new char[sizeof (m_szLogFileName) + 1];
    strcpy (m_logCtrl.fc.fileName, (const char *) m_szLogFileName);
}

//==============================================================
// Lets user change the name of the dump file to be generated 
//==============================================================
void WheatyExceptionReport::SetDumpFileName( PTSTR pszDumpFileName )
{
    _tcscpy( m_szDumpFileName, pszDumpFileName );
}

void WheatyExceptionReport::GenerateProcessDump ()
{
    GenerateProcessDump (m_szDumpFileName);
}

void WheatyExceptionReport::GenerateProcessDump (PTSTR pszDumpFileName)
{
    time_t currTime = time (NULL);
    struct tm *pLocalTime = localtime (&currTime);
    char temp[1000];
    sprintf (temp,"%s_%02d%02d%02d%02d%02d.dmp", pszDumpFileName,
        pLocalTime->tm_mon + 1,
        pLocalTime->tm_mday,
        pLocalTime->tm_hour,
        pLocalTime->tm_min,
        pLocalTime->tm_sec);
    HANDLE file = CreateFile( temp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
							       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if ( file != INVALID_HANDLE_VALUE ) 
    {
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
				  file, 
				  (MINIDUMP_TYPE)
				  (MiniDumpWithDataSegs | MiniDumpWithFullMemory | 
				   MiniDumpWithHandleData | MiniDumpWithIndirectlyReferencedMemory | 
				   MiniDumpWithProcessThreadData | MiniDumpWithPrivateReadWriteMemory | 
				   MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo), 
				  NULL, NULL, NULL);
	CloseHandle(file);
    }
}

//===========================================================
// Entry point where control comes on an unhandled exception 
//===========================================================
LONG WINAPI WheatyExceptionReport::WheatyUnhandledExceptionFilter(
                                    PEXCEPTION_POINTERS pExceptionInfo )
{
   GenerateExceptionReport( pExceptionInfo );
   GenerateDumpFile( pExceptionInfo );

   if (m_bTerminate)
     exit (1);
     
   if ( m_previousFilter )
     return m_previousFilter( pExceptionInfo );
   else
     return EXCEPTION_EXECUTE_HANDLER;
}

//===========================================================================
// Open the report file, and write the desired information to it.  Called by 
// WheatyUnhandledExceptionFilter                                               
//===========================================================================
void WheatyExceptionReport::GenerateExceptionReport(
    PEXCEPTION_POINTERS pExceptionInfo )
{

#if USE_FILE
    m_hReportFile = CreateFile( m_szLogFileName,
                                GENERIC_WRITE,
                                0,
                                0,
                                OPEN_ALWAYS,
                                FILE_FLAG_WRITE_THROUGH,
                                0 );

    if ( !m_hReportFile )
	return;
	
    SetFilePointer( m_hReportFile, 0, 0, FILE_END );

#else
    LOG_CTRL *pTempLogCtrl = sLogCtrl;
    sLogCtrl = &m_logCtrl;
#endif    

    // Start out with a banner
    _tprintf(_T("//=====================================================\r\n"));
#if USE_FILE    
    _tprintf(_T("//=====================================================\r\n"));  
    time_t now;
    time (&now);
    struct tm *pLocalTime = localtime (&now);
    _tprintf(_T("Exception detected on %s\r\n"), asctime (pLocalTime));        
#endif    

    PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

    // First print information about the type of fault
    _tprintf(   _T("Exception code: %08X %s\r\n"),
                pExceptionRecord->ExceptionCode,
                GetExceptionString(pExceptionRecord->ExceptionCode) );

    // Now print information about where the fault occured
    TCHAR szFaultingModule[MAX_PATH];
    DWORD section, offset;
    GetLogicalAddress(  pExceptionRecord->ExceptionAddress,
                        szFaultingModule,
                        sizeof( szFaultingModule ),
                        section, offset );

    _tprintf( _T("Fault address:  %08X %02X:%08X %s\r\n"),
                (LONG) pExceptionRecord->ExceptionAddress,
                section, offset, szFaultingModule );

    PCONTEXT pCtx = pExceptionInfo->ContextRecord;

    // Show the registers
    #ifdef _M_IX86  // X86 Only!
    _tprintf( _T("\n  Registers:\r\n") );

    _tprintf(_T("EAX:%08X\n  EBX:%08X\n  ECX:%08X\n  EDX:%08X\n  ESI:%08X\n  EDI:%08X\r\n")
            ,pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx,
            pCtx->Esi, pCtx->Edi );

    _tprintf( _T("CS:EIP:%04X:%08X\r\n"), pCtx->SegCs, pCtx->Eip );
    _tprintf( _T("SS:ESP:%04X:%08X  EBP:%08X\r\n"),
                pCtx->SegSs, pCtx->Esp, pCtx->Ebp );
    _tprintf( _T("DS:%04X  ES:%04X  FS:%04X  GS:%04X\r\n"),
                pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs );
    _tprintf( _T("Flags:%08X\r\n"), pCtx->EFlags );

    #endif

    SymSetOptions( SYMOPT_DEFERRED_LOADS );

    // Initialize DbgHelp
    SymInitialize( GetCurrentProcess(), 0, TRUE );

    CONTEXT trashableContext = *pCtx;

    WriteStackDetails( &trashableContext, false );

#if 0
    #ifdef _M_IX86  // X86 Only!

    _tprintf( _T("\n========================\r\n") );
    _tprintf( _T("Local Variables And Parameters\r\n") );

    trashableContext = *pCtx;
    WriteStackDetails( &trashableContext, true );

    _tprintf( _T("========================\r\n") );
    _tprintf( _T("Global Variables\r\n") );

    SymEnumSymbols( GetCurrentProcess(),
                    (DWORD64)GetModuleHandle(szFaultingModule),
                    0, EnumerateSymbolsCallback, 0 );
    
    #endif      // X86 Only!
#endif

    SymCleanup( GetCurrentProcess() );
    
#if USE_FILE
    CloseHandle( m_hReportFile );
    m_hReportFile = 0;
#else
    sLogCtrl = pTempLogCtrl;
#endif    
    
}

//===========================================================================
// Generate minidump file.  Called by 
// WheatyUnhandledExceptionFilter                                               
//===========================================================================
void WheatyExceptionReport::GenerateDumpFile(
    PEXCEPTION_POINTERS pExceptionInfo )
{
    MINIDUMP_EXCEPTION_INFORMATION mdException;

    ZeroMemory( &mdException, sizeof(MINIDUMP_EXCEPTION_INFORMATION) );	

    mdException.ThreadId = GetCurrentThreadId();
    mdException.ClientPointers = TRUE;
    mdException.ExceptionPointers = pExceptionInfo;

    time_t currTime = time (NULL);
    struct tm *pLocalTime = localtime (&currTime);
    char temp[1000];
    sprintf (temp,"%s_%02d%02d%02d%02d%02d.dmp", m_szDumpFileName,
        pLocalTime->tm_mon + 1,
        pLocalTime->tm_mday,
        pLocalTime->tm_hour,
        pLocalTime->tm_min,
        pLocalTime->tm_sec);

    HANDLE file = CreateFile( temp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
							       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if ( file != INVALID_HANDLE_VALUE ) 
    {
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
				  file, 
				  (MINIDUMP_TYPE)
				  (MiniDumpWithDataSegs | MiniDumpWithFullMemory | 
				   MiniDumpWithHandleData | MiniDumpWithIndirectlyReferencedMemory | 
				   MiniDumpWithProcessThreadData | MiniDumpWithPrivateReadWriteMemory | 
				   MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo), 
				  &mdException, NULL, NULL);
	CloseHandle(file);
    }
	
}

//======================================================================
// Given an exception code, returns a pointer to a static string with a 
// description of the exception                                         
//======================================================================
LPTSTR WheatyExceptionReport::GetExceptionString( DWORD dwCode )
{
    #define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x);

    switch ( dwCode )
    {
        EXCEPTION( ACCESS_VIOLATION )
        EXCEPTION( DATATYPE_MISALIGNMENT )
        EXCEPTION( BREAKPOINT )
        EXCEPTION( SINGLE_STEP )
        EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
        EXCEPTION( FLT_DENORMAL_OPERAND )
        EXCEPTION( FLT_DIVIDE_BY_ZERO )
        EXCEPTION( FLT_INEXACT_RESULT )
        EXCEPTION( FLT_INVALID_OPERATION )
        EXCEPTION( FLT_OVERFLOW )
        EXCEPTION( FLT_STACK_CHECK )
        EXCEPTION( FLT_UNDERFLOW )
        EXCEPTION( INT_DIVIDE_BY_ZERO )
        EXCEPTION( INT_OVERFLOW )
        EXCEPTION( PRIV_INSTRUCTION )
        EXCEPTION( IN_PAGE_ERROR )
        EXCEPTION( ILLEGAL_INSTRUCTION )
        EXCEPTION( NONCONTINUABLE_EXCEPTION )
        EXCEPTION( STACK_OVERFLOW )
        EXCEPTION( INVALID_DISPOSITION )
        EXCEPTION( GUARD_PAGE )
        EXCEPTION( INVALID_HANDLE )
        EXCEPTION( DIAGNOSTIC_DEBUG )
    }

    // If not one of the "known" exceptions, try to get the string
    // from NTDLL.DLL's message table.

    static TCHAR szBuffer[512] = { 0 };

    FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
                   GetModuleHandle( _T("NTDLL.DLL") ),
                   dwCode, 0, szBuffer, sizeof( szBuffer ), 0 );

    return szBuffer;
}

//=============================================================================
// Given a linear address, locates the module, section, and offset containing  
// that address.                                                               
//                                                                             
// Note: the szModule paramater buffer is an output buffer of length specified 
// by the len parameter (in characters!)                                       
//=============================================================================
BOOL WheatyExceptionReport::GetLogicalAddress(
        PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset )
{
    MEMORY_BASIC_INFORMATION mbi;

    if ( !VirtualQuery( addr, &mbi, sizeof(mbi) ) )
        return FALSE;

    DWORD hMod = (DWORD)mbi.AllocationBase;

    if ( !GetModuleFileName( (HMODULE)hMod, szModule, len ) )
        return FALSE;

    if (hMod == 0)
        return FALSE;
	
    // Point to the DOS header in memory
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;

    // From the DOS header, find the NT (PE) header
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);

    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

    DWORD rva = (DWORD)addr - hMod; // RVA is offset from module load address

    // Iterate through the section table, looking for the one that encompasses
    // the linear address.
    for (   unsigned i = 0;
            i < pNtHdr->FileHeader.NumberOfSections;
            i++, pSection++ )
    {
        DWORD sectionStart = pSection->VirtualAddress;
        DWORD sectionEnd = sectionStart
                    + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

        // Is the address in this section???
        if ( (rva >= sectionStart) && (rva <= sectionEnd) )
        {
            // Yes, address is in the section.  Calculate section and offset,
            // and store in the "section" & "offset" params, which were
            // passed by reference.
            section = i+1;
            offset = rva - sectionStart;
            return TRUE;
        }
    }

    return FALSE;   // Should never get here!
}

//============================================================
// Walks the stack, and writes the results to the report file 
//============================================================
void WheatyExceptionReport::WriteStackDetails(
        PCONTEXT pContext,
        bool bWriteVariables )  // true if local/params should be output
{
    _tprintf( _T("\n  Call stack:\r\n") );

    _tprintf( _T("Address   Frame     Function            SourceFile\r\n") );

    DWORD dwMachineType = 0;
    // Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag

    char buffer[1024];
    STACKFRAME sf;
    memset( &sf, 0, sizeof(sf) );

    #ifdef _M_IX86
    // Initialize the STACKFRAME structure for the first call.  This is only
    // necessary for Intel CPUs, and isn't mentioned in the documentation.
    sf.AddrPC.Offset       = pContext->Eip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = pContext->Esp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = pContext->Ebp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_I386;
    #endif

    while ( 1 )
    {
        // Get the next stack frame
        if ( ! StackWalk(  dwMachineType,
                            m_hProcess,
                            GetCurrentThread(),
                            &sf,
                            pContext,
                            0,
                            SymFunctionTableAccess,
                            SymGetModuleBase,
                            0 ) )
            break;

        if ( 0 == sf.AddrFrame.Offset ) // Basic sanity check to make sure
            break;                      // the frame is OK.  Bail if not.

        TCHAR szModule[MAX_PATH] = _T("");
        DWORD section = 0, offset = 0;
        GetLogicalAddress(  (PVOID)sf.AddrPC.Offset, szModule, sizeof(szModule), section, offset );
        
        // Get the name of the function for this stack frame entry
        BYTE symbolBuffer [sizeof(SYMBOL_INFO) + MAX_SYM_NAME + 1];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;
                        
        DWORD64 symDisplacement = 0;    // Displacement of the input address,
                                        // relative to the start of the symbol

        // Get the source line for this stack frame entry
        IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
        DWORD dwLineDisplacement;
	
        if ( SymFromAddr(m_hProcess,sf.AddrPC.Offset,&symDisplacement,pSymbol))
        {
            if ( SymGetLineFromAddr( m_hProcess, sf.AddrPC.Offset,
                                &dwLineDisplacement, &lineInfo ) )
               sprintf(buffer, _T("%08X  %08X  %hs+%I64X  %s line %u"),
			sf.AddrPC.Offset, sf.AddrFrame.Offset, pSymbol->Name, symDisplacement, lineInfo.FileName,lineInfo.LineNumber); 
	    else
               sprintf(buffer, _T("%08X  %08X  %hs+%I64X"), 
			sf.AddrPC.Offset, sf.AddrFrame.Offset, pSymbol->Name, symDisplacement );
        }
        else    // No symbol found.  Print out the logical address instead.
        {

            if ( SymGetLineFromAddr( m_hProcess, sf.AddrPC.Offset,
                                &dwLineDisplacement, &lineInfo ) )
               sprintf(buffer, _T("%04X:%08X %s  %s line %u"),section, offset, szModule, lineInfo.FileName,lineInfo.LineNumber); 
	    else
               sprintf(buffer, _T("%04X:%08X %s"), section, offset, szModule );
        }
	_tprintf ("%s\r\n", buffer);

        // Write out the variables, if desired
        if ( bWriteVariables )
        {
            // Use SymSetContext to get just the locals/params for this frame
            IMAGEHLP_STACK_FRAME imagehlpStackFrame;
            imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;
            SymSetContext( m_hProcess, &imagehlpStackFrame, 0 );

            // Enumerate the locals/parameters
            SymEnumSymbols( m_hProcess, 0, 0, EnumerateSymbolsCallback, &sf );
        }
    }

}

//////////////////////////////////////////////////////////////////////////////
// The function invoked by SymEnumSymbols
//////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK
WheatyExceptionReport::EnumerateSymbolsCallback(
    PSYMBOL_INFO  pSymInfo,
    ULONG         SymbolSize,
    PVOID         UserContext )
{

    char szBuffer[2048];

    __try
    {
        if ( FormatSymbolValue( pSymInfo, (STACKFRAME*)UserContext,
                                szBuffer, sizeof(szBuffer) ) )  
            _tprintf( _T("\t%s\r\n"), szBuffer );
    }
    __except( 1 )
    {
        _tprintf( _T("punting on symbol %s\r\n"), pSymInfo->Name );
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Given a SYMBOL_INFO representing a particular variable, displays its
// contents.  If it's a user defined type, display the members and their
// values.
//////////////////////////////////////////////////////////////////////////////
bool WheatyExceptionReport::FormatSymbolValue(
            PSYMBOL_INFO pSym,
            STACKFRAME * sf,
            char * pszBuffer,
            unsigned cbBuffer )
{
    char * pszCurrBuffer = pszBuffer;

    // Indicate if the variable is a local or parameter
    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
        pszCurrBuffer += sprintf( pszCurrBuffer, "Parameter " );
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
        pszCurrBuffer += sprintf( pszCurrBuffer, "Local " );

    // If it's a function, don't do anything.
    if ( pSym->Tag == 5 )   // SymTagFunction from CVCONST.H from the DIA SDK
        return false;

    // Emit the variable name
    pszCurrBuffer += sprintf( pszCurrBuffer, "\'%s\'", pSym->Name );

    DWORD_PTR pVariable = 0;    // Will point to the variable's data in memory

    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE )
    {
        // if ( pSym->Register == 8 )   // EBP is the value 8 (in DBGHELP 5.1)
        {                               //  This may change!!!
            pVariable = sf->AddrFrame.Offset;
            pVariable += (DWORD_PTR)pSym->Address;
        }
        // else
        //  return false;
    }
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER )
    {
        return false;   // Don't try to report register variable
    }
    else
    {
        pVariable = (DWORD_PTR)pSym->Address;   // It must be a global variable
    }

    // Determine if the variable is a user defined type (UDT).  IF so, bHandled
    // will return true.
    bool bHandled;
    pszCurrBuffer = DumpTypeIndex(pszCurrBuffer,pSym->ModBase, pSym->TypeIndex,
                                    0, pVariable, bHandled );

    if ( !bHandled )
    {
        // The symbol wasn't a UDT, so do basic, stupid formatting of the
        // variable.  Based on the size, we're assuming it's a char, WORD, or
        // DWORD.
        BasicType basicType = GetBasicType( pSym->TypeIndex, pSym->ModBase );
        
        pszCurrBuffer = FormatOutputValue(pszCurrBuffer, basicType, pSym->Size,
                                            (PVOID)pVariable ); 
    }


    return true;
}

//////////////////////////////////////////////////////////////////////////////
// If it's a user defined type (UDT), recurse through its members until we're
// at fundamental types.  When he hit fundamental types, return
// bHandled = false, so that FormatSymbolValue() will format them.
//////////////////////////////////////////////////////////////////////////////
char * WheatyExceptionReport::DumpTypeIndex(
        char * pszCurrBuffer,
        DWORD64 modBase,
        DWORD dwTypeIndex,
        unsigned nestingLevel,
        DWORD_PTR offset,
        bool & bHandled )
{
    bHandled = false;

    // Get the name of the symbol.  This will either be a Type name (if a UDT),
    // or the structure member name.
    WCHAR * pwszTypeName;
    if ( SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_SYMNAME,
                        &pwszTypeName ) )
    {
        pszCurrBuffer += sprintf( pszCurrBuffer, " %ls", pwszTypeName );
        LocalFree( pwszTypeName );
    }

    // Determine how many children this type has.
    DWORD dwChildrenCount = 0;
    SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT,
                    &dwChildrenCount );

    if ( !dwChildrenCount )     // If no children, we're done
        return pszCurrBuffer;

    // Prepare to get an array of "TypeIds", representing each of the children.
    // SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
    // TI_FINDCHILDREN_PARAMS struct has.  Use derivation to accomplish this.
    struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
    {
        ULONG   MoreChildIds[1024];
        FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
    } children;

    children.Count = dwChildrenCount;
    children.Start= 0;

    // Get the array of TypeIds, one for each child type
    if ( !SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_FINDCHILDREN,
                            &children ) )
    {
        return pszCurrBuffer;
    }

    // Append a line feed
    pszCurrBuffer += sprintf( pszCurrBuffer, "\r\n" );

    // Iterate through each of the children
    for ( unsigned i = 0; i < dwChildrenCount; i++ )
    {
        // Add appropriate indentation level (since this routine is recursive)
        for ( unsigned j = 0; j <= nestingLevel+1; j++ )
            pszCurrBuffer += sprintf( pszCurrBuffer, "\t" );

        // Recurse for each of the child types
        bool bHandled2;
        pszCurrBuffer = DumpTypeIndex( pszCurrBuffer, modBase,
                                        children.ChildId[i], nestingLevel+1,
                                        offset, bHandled2 );

        // If the child wasn't a UDT, format it appropriately
        if ( !bHandled2 )
        {
            // Get the offset of the child member, relative to its parent
            DWORD dwMemberOffset;
            SymGetTypeInfo( m_hProcess, modBase, children.ChildId[i],
                            TI_GET_OFFSET, &dwMemberOffset );

            // Get the real "TypeId" of the child.  We need this for the
            // SymGetTypeInfo( TI_GET_TYPEID ) call below.
            DWORD typeId;
            SymGetTypeInfo( m_hProcess, modBase, children.ChildId[i],
                            TI_GET_TYPEID, &typeId );

            // Get the size of the child member
            ULONG64 length;
            SymGetTypeInfo(m_hProcess, modBase, typeId, TI_GET_LENGTH,&length);

            // Calculate the address of the member
            DWORD_PTR dwFinalOffset = offset + dwMemberOffset;

            BasicType basicType = GetBasicType(children.ChildId[i], modBase );

            pszCurrBuffer = FormatOutputValue( pszCurrBuffer, basicType,
                                                length, (PVOID)dwFinalOffset ); 

            pszCurrBuffer += sprintf( pszCurrBuffer, "\r\n" );
        }
    }

    bHandled = true;
    return pszCurrBuffer;
}

char * WheatyExceptionReport::FormatOutputValue(   char * pszCurrBuffer,
                                                    BasicType basicType,
                                                    DWORD64 length,
                                                    PVOID pAddress )
{
    // Format appropriately (assuming it's a 1, 2, or 4 bytes (!!!)
    if ( length == 1 )
        pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PBYTE)pAddress );
    else if ( length == 2 )
        pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PWORD)pAddress );
    else if ( length == 4 )
    {
        if ( basicType == btFloat )
        {
            pszCurrBuffer += sprintf(pszCurrBuffer," = %f", *(PFLOAT)pAddress);
        }
        else if ( basicType == btChar )
        {
            if ( !IsBadStringPtr( *(PSTR*)pAddress, 32) )
            {
                pszCurrBuffer += sprintf( pszCurrBuffer, " = \"%.31s\"",
                                            *(PDWORD)pAddress );
            }
            else
                pszCurrBuffer += sprintf( pszCurrBuffer, " = %X",
                                            *(PDWORD)pAddress );
        }
        else
            pszCurrBuffer += sprintf(pszCurrBuffer," = %X", *(PDWORD)pAddress);
    }
    else if ( length == 8 )
    {
        if ( basicType == btFloat )
        {
            pszCurrBuffer += sprintf( pszCurrBuffer, " = %lf",
                                        *(double *)pAddress );
        }
        else
            pszCurrBuffer += sprintf( pszCurrBuffer, " = %I64X",
                                        *(DWORD64*)pAddress );
    }

    return pszCurrBuffer;
}

BasicType
WheatyExceptionReport::GetBasicType( DWORD typeIndex, DWORD64 modBase )
{
    BasicType basicType;
    if ( SymGetTypeInfo( m_hProcess, modBase, typeIndex,
                        TI_GET_BASETYPE, &basicType ) )
    {
        return basicType;
    }

    // Get the real "TypeId" of the child.  We need this for the
    // SymGetTypeInfo( TI_GET_TYPEID ) call below.
    DWORD typeId;
    if (SymGetTypeInfo(m_hProcess,modBase, typeIndex, TI_GET_TYPEID, &typeId))
    {
        if ( SymGetTypeInfo( m_hProcess, modBase, typeId, TI_GET_BASETYPE,
                            &basicType ) )
        {
            return basicType;
        }
    }

    return btNoType;
}

//============================================================================
// Helper function that writes to the report file, and allows the user to use 
// printf style formating                                                     
//============================================================================
int __cdecl WheatyExceptionReport::_tprintf(const TCHAR * format, ...)
{
    TCHAR szBuff[10240];
    int retValue;
    va_list argptr;
          
    va_start( argptr, format );
    retValue = vsprintf( szBuff, format, argptr );
    va_end( argptr );

#if USE_FILE
    DWORD cbWritten;
    WriteFile(m_hReportFile, szBuff, retValue * sizeof(TCHAR), &cbWritten, 0 );
#else
    char *pTemp = strchr (szBuff, '\r');
    if (pTemp) *pTemp = 0;
    SLOGCALWAYS1 ("%s", szBuff);
#endif

    return retValue;
}

//=========================================================================
// ExpInit: user-callable
//=========================================================================

//============================================================
// Walks the stack, and writes the results to the report file 
//============================================================
void GetStackFrames (StackInfo& stackInfo)  // true if local/params should be output
  {
  
  CONTEXT context;
  context.ContextFlags = CONTEXT_FULL;
  PCONTEXT pContext = &context;
  BOOL ret = GetThreadContext (GetCurrentThread (), pContext);
  
  DWORD dwMachineType = IMAGE_FILE_MACHINE_I386;
  // Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag

  STACKFRAME sf;
  memset( &sf, 0, sizeof(sf) );

  #ifdef _M_IX86
  // Initialize the STACKFRAME structure for the first call.  This is only
  // necessary for Intel CPUs, and isn't mentioned in the documentation.
  sf.AddrPC.Offset       = pContext->Eip;
  sf.AddrPC.Mode         = AddrModeFlat;
  sf.AddrStack.Offset    = pContext->Esp;
  sf.AddrStack.Mode      = AddrModeFlat;
  sf.AddrFrame.Offset    = pContext->Ebp;
  sf.AddrFrame.Mode      = AddrModeFlat;

  dwMachineType = IMAGE_FILE_MACHINE_I386;
  #endif

  while ( 1 )
    {
    // Get the next stack frame
    if (!StackWalk (dwMachineType,
                      GetCurrentProcess (),
                      GetCurrentThread(),
                      &sf,
                      pContext,
                      0,
                      SymFunctionTableAccess,
                      SymGetModuleBase,
                      0))
      break;

    if (0 == sf.AddrFrame.Offset) // Basic sanity check to make sure
      break;                      // the frame is OK.  Bail if not.

    // Get the name of the function for this stack frame entry
    BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
    memset (symbolBuffer, 0, sizeof (symbolBuffer));
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
    pSymbol->SizeOfStruct = sizeof(symbolBuffer);
    pSymbol->MaxNameLen = 1024;
                        
    DWORD64 symDisplacement = 0;    // Displacement of the input address,
                                    // relative to the start of the symbol

    // Get the source line for this stack frame entry
    IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
    DWORD dwLineDisplacement;
    SourceInfo si;
	
    if (SymFromAddr (GetCurrentProcess (), sf.AddrPC.Offset, &symDisplacement, pSymbol))
      {
      if (SymGetLineFromAddr(GetCurrentProcess (), sf.AddrPC.Offset,
          &dwLineDisplacement, &lineInfo))
	{
	si.filename = lineInfo.FileName;
	si.linenum = lineInfo.LineNumber;
	stackInfo.stack.push_back (si);
	}
      else
        DWORD err = GetLastError ();
      }
    else    // No symbol found.  Print out the logical address instead.
      {
      DWORD err = GetLastError ();
      
      TCHAR szModule[MAX_PATH] = _T("");
      DWORD section = 0, offset = 0;

      WheatyExceptionReport::GetLogicalAddress ((PVOID)sf.AddrPC.Offset, szModule, sizeof(szModule), section, offset );

      if (SymGetLineFromAddr (GetCurrentProcess (), sf.AddrPC.Offset,
                              &dwLineDisplacement, &lineInfo))
        {
	si.filename = lineInfo.FileName;
	si.linenum = lineInfo.LineNumber;
	stackInfo.stack.push_back (si);
	}
      }
    }
  }

void GetStackInfo (StackInfo& stackInfo)
  {
  
  stackInfo.threadID = GetCurrentThreadId ();
  GetLocalTime (&stackInfo.timestamp);
  
  SymSetOptions( SYMOPT_DEFERRED_LOADS );

  if (!SymInitialize (GetCurrentProcess(), 0, TRUE))
    return;

  GetStackFrames (stackInfo);
  }


void StackInfo::Log ()
  {
  char timeText[1024];
  sprintf (timeText, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
           timestamp.wYear, timestamp.wMonth, timestamp.wDay,
           timestamp.wHour, timestamp.wMinute, timestamp.wSecond, timestamp.wMilliseconds);
  SLOGCALWAYS2 ("Call Stack for Thread %d at %s:", threadID, timeText);
  for (vector<SourceInfo>::iterator it = stack.begin (); it != stack.end (); ++it)
    {
    SourceInfo& si = *it;
    SLOGCALWAYS2 ("  %s %d", si.filename.c_str (), si.linenum);
    }
  SLOGCALWAYS0 ("");
  }

extern "C" void ExpInit ()
  {

#if defined (NON_GLOBAL_EXP_REPORT)
  if (g_pWheatyExceptionReport == NULL)
    new WheatyExceptionReport;
#endif  
  }

//=========================================================================
// ExpRaiseDebugException: user-callable
//=========================================================================
extern "C" void ExpRaiseDebugException ()
  {

  RaiseException (EXCEPTION_DIAGNOSTIC_DEBUG, 0, 0, NULL); 
  }

#ifdef DEBUG_SISCO
//=========================================================================
// ExpSetLogCtrl: user-callable
//=========================================================================
extern "C" void ExpSetLogCtrl (LOG_CTRL *pLogCtrl)
  {
  }
#endif

extern "C" void ExpSetLogFileName( PTSTR pszLogFileName )
  {
  
#if !defined (NON_GLOBAL_EXP_REPORT)  
  g_WheatyExceptionReport.SetLogFileName (pszLogFileName);
#else
  ExpInit ();
  g_pWheatyExceptionReport->SetLogFileName (pszLogFileName);
#endif  
  }
  
extern "C" void ExpSetDumpFileName (PTSTR pszDumpFileName)
  {
  
#if !defined (NON_GLOBAL_EXP_REPORT)  
  g_WheatyExceptionReport.SetDumpFileName (pszDumpFileName);  
#else
  ExpInit ();
  g_pWheatyExceptionReport->SetDumpFileName (pszDumpFileName);
#endif  
  }
  
extern "C" void ExpGenerateProcessDump (PTSTR pszDumpFileName)
  {

  pszDumpFileName ? 
	WheatyExceptionReport::GenerateProcessDump (pszDumpFileName) :
	WheatyExceptionReport::GenerateProcessDump ();  	
  }
  
extern "C" void ExpClearExceptionFilter ()
  {  
  
  SetUnhandledExceptionFilter (NULL);
  }
  
extern "C" int ExpFilter (unsigned int code, struct _EXCEPTION_POINTERS *ep)
  {
  
  if (code == EXCEPTION_ACCESS_VIOLATION)
    {
    
    WheatyExceptionReport::GenerateExceptionReport (ep);  
    return EXCEPTION_EXECUTE_HANDLER;
    }
    
  return EXCEPTION_CONTINUE_SEARCH;
  }
