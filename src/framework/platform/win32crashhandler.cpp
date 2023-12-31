/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if defined(WIN32) && defined(CRASH_HANDLER)

#include "crashhandler.h"
#include <framework/global.h>
#include <framework/core/application.h>
#include <framework/core/resourcemanager.h>

#include <winsock2.h>
#include <windows.h>
#include <process.h>

#ifdef _MSC_VER

#pragma warning (push)
#pragma warning (disable:4091) // warning C4091: 'typedef ': ignored on left of '' when no variable is declared
#include <imagehlp.h>
#pragma warning (pop)

#else

#include <imagehlp.h>

#endif

bool quiet_crash = false;

const char *getExceptionName(DWORD exceptionCode)
{
    switch (exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:         return "Access violation";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Datatype misalignment";
        case EXCEPTION_BREAKPOINT:               return "Breakpoint";
        case EXCEPTION_SINGLE_STEP:              return "Single step";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array bounds exceeded";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Float denormal operand";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Float divide by zero";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "Float inexact result";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "Float invalid operation";
        case EXCEPTION_FLT_OVERFLOW:             return "Float overflow";
        case EXCEPTION_FLT_STACK_CHECK:          return "Float stack check";
        case EXCEPTION_FLT_UNDERFLOW:            return "Float underflow";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer divide by zero";
        case EXCEPTION_INT_OVERFLOW:             return "Integer overflow";
        case EXCEPTION_PRIV_INSTRUCTION:         return "Privileged instruction";
        case EXCEPTION_IN_PAGE_ERROR:            return "In page error";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal instruction";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable exception";
        case EXCEPTION_STACK_OVERFLOW:           return "Stack overflow";
        case EXCEPTION_INVALID_DISPOSITION:      return "Invalid disposition";
        case EXCEPTION_GUARD_PAGE:               return "Guard page";
        case EXCEPTION_INVALID_HANDLE:           return "Invalid handle";
    }
    return "Unknown exception";
}

void Stacktrace(LPEXCEPTION_POINTERS e, std::stringstream& ss)
{
    PIMAGEHLP_SYMBOL pSym;
    STACKFRAME sf;
    HANDLE process, thread;
    ULONG_PTR dwModBase, Disp;
    BOOL more = FALSE;
    DWORD machineType;
    int count = 0;
    char modname[MAX_PATH];
    char symBuffer[sizeof(IMAGEHLP_SYMBOL) + 255];

    pSym = (PIMAGEHLP_SYMBOL)symBuffer;

    ZeroMemory(&sf, sizeof(sf));
#ifdef _WIN64
    sf.AddrPC.Offset = e->ContextRecord->Rip;
    sf.AddrStack.Offset = e->ContextRecord->Rsp;
    sf.AddrFrame.Offset = e->ContextRecord->Rbp;
    machineType = IMAGE_FILE_MACHINE_AMD64;
#else
    sf.AddrPC.Offset = e->ContextRecord->Eip;
    sf.AddrStack.Offset = e->ContextRecord->Esp;
    sf.AddrFrame.Offset = e->ContextRecord->Ebp;
    machineType = IMAGE_FILE_MACHINE_I386;
#endif

    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Mode = AddrModeFlat;

    process = GetCurrentProcess();
    thread = GetCurrentThread();

    while(1) {
        more = StackWalk(machineType,  process, thread, &sf, e->ContextRecord, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL);
        if(!more || sf.AddrFrame.Offset == 0)
            break;

        dwModBase = SymGetModuleBase(process, sf.AddrPC.Offset);
        if(dwModBase)
            GetModuleFileNameA((HINSTANCE)dwModBase, modname, MAX_PATH);
        else
            strcpy(modname, "Unknown");

        Disp = 0;
        pSym->SizeOfStruct = sizeof(symBuffer);
        pSym->MaxNameLength = 254;

        if(SymGetSymFromAddr(process, sf.AddrPC.Offset, &Disp, pSym))
            ss << stdext::format("    %d: %s(%s+%#0lx) [0x%016lX]\n", count, modname, pSym->Name, Disp, sf.AddrPC.Offset);
        else
            ss << stdext::format("    %d: %s [0x%016lX]\n", count, modname, sf.AddrPC.Offset);
        ++count;
    }
    GlobalFree(pSym);
}

LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS e)
{
    // generate crash report
    SymInitialize(GetCurrentProcess(), 0, TRUE);
    std::stringstream ss;
    ss << "== application crashed\n";
    ss << stdext::format("app name: %s\n", g_app.getName());
    ss << stdext::format("app version: %s\n", g_app.getVersion());
    ss << stdext::format("build compiler: %s\n", BUILD_COMPILER);
    ss << stdext::format("build date: %s\n", __DATE__);
    ss << stdext::format("build type: %s\n", BUILD_TYPE);
    ss << stdext::format("build revision: %s (%s)\n", BUILD_COMMIT);
    ss << stdext::format("crash date: %s\n", stdext::date_time_string());
    ss << stdext::format("exception: %s (0x%08lx)\n", getExceptionName(e->ExceptionRecord->ExceptionCode), e->ExceptionRecord->ExceptionCode);
    ss << stdext::format("exception address: 0x%08lx\n", (size_t)e->ExceptionRecord->ExceptionAddress);
    ss << stdext::format("  backtrace:\n");
    Stacktrace(e, ss);
    ss << "\n";
    SymCleanup(GetCurrentProcess());

    // print in stdout
    g_logger.info(ss.str());

    // write stacktrace to crashreport.log
    auto dumpFilePath = std::filesystem::path(g_resources.getWriteDir());
    dumpFilePath /= "crashreport.log";
    std::ofstream fout(dumpFilePath, std::ios::out | std::ios::app);
    if(fout.is_open() && fout.good()) {
        fout << ss.str();
        fout.close();
        g_logger.info(stdext::format("Crash report saved to file %s", dumpFilePath.string()));
    } else
        g_logger.error("Failed to save crash report!");

    // inform the user
    std::string msg = std::string(
        "The application has crashed.\n\n"
        "A crash report has been written to:\n") + dumpFilePath.string();
    if(!g_app.isStopping() && !quiet_crash) {
        MessageBoxA(NULL, msg.c_str(), "Application crashed", 0);
    }

    // this seems to silently close the application
    //return EXCEPTION_EXECUTE_HANDLER;

    // this triggers the microsoft "application has crashed" error dialog
    return EXCEPTION_CONTINUE_SEARCH;
}


#define TRACE_MAX_FUNCTION_NAME_LENGTH 1024
#define TRACE_LOG_ERRORS FALSE
#define TRACE_DUMP_NAME "exception.dmp"
#define TRACE_DUMP_NAME_QUIET "exception2.dmp"
#define TRACE_DUMP_NAME_FULL "exception_full.dmp"

LONG WINAPI UnhandledExceptionFilter2(PEXCEPTION_POINTERS exception)
{
    CONTEXT context = *(exception->ContextRecord);
    HANDLE thread = GetCurrentThread();
    HANDLE process = GetCurrentProcess();
    STACKFRAME frame;
    memset(&frame, 0, sizeof(STACKFRAME));
    DWORD image;

#ifdef _M_IX86
    image = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset = context.Eip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Ebp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Esp;
    frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    image = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset = context.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
    image = IMAGE_FILE_MACHINE_IA64;
    frame.AddrPC.Offset = context.StIIP;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.IntSp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrBStore.Offset = context.RsBSP;
    frame.AddrBStore.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.IntSp;
    frame.AddrStack.Mode = AddrModeFlat;
#else
#error "This platform is not supported."
#endif

    auto dumpFilePath = std::filesystem::path(g_resources.getWriteDir());
    if (quiet_crash) {
        dumpFilePath /= TRACE_DUMP_NAME_QUIET;
    } else {
        dumpFilePath /= TRACE_DUMP_NAME;
    }
    {
        HANDLE dumpFile = CreateFileA(dumpFilePath.string().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
        exceptionInformation.ThreadId = GetCurrentThreadId();
        exceptionInformation.ExceptionPointers = exception;
        exceptionInformation.ClientPointers = FALSE;
        int flags = MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory;
        MiniDumpWriteDump(process, GetProcessId(process), dumpFile, (MINIDUMP_TYPE)flags, exception ? &exceptionInformation : NULL, NULL, NULL);
    }
    {
        dumpFilePath = std::filesystem::path(g_resources.getWriteDir());
        dumpFilePath /= TRACE_DUMP_NAME_FULL;
        HANDLE dumpFile = CreateFileA(dumpFilePath.string().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
        exceptionInformation.ThreadId = GetCurrentThreadId();
        exceptionInformation.ExceptionPointers = exception;
        exceptionInformation.ClientPointers = FALSE;
        int flags = MiniDumpWithPrivateReadWriteMemory |
            MiniDumpWithDataSegs |
            MiniDumpWithHandleData |
            MiniDumpWithFullMemoryInfo |
            MiniDumpWithThreadInfo |
            MiniDumpWithUnloadedModules;
        MiniDumpWriteDump(process, GetProcessId(process), dumpFile, (MINIDUMP_TYPE)flags, exception ? &exceptionInformation : NULL, NULL, NULL);
    }

    if (quiet_crash) {
#ifdef _MSC_VER
        quick_exit(0);
#else
        exit(0);
#endif
    }

    Sleep(1000);
    ExceptionHandler(exception);

    return EXCEPTION_CONTINUE_SEARCH;
}

void installCrashHandler()
{
    SetUnhandledExceptionFilter(UnhandledExceptionFilter2);
}

void uninstallCrashHandler()
{
    quiet_crash = true;
}

#endif
