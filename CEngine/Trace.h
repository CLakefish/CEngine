#pragma once
#include <iostream>
#include <fstream>
#include <cstdarg>

static const char* tracePath = "Trace.txt";
static FILE* traceFile;

void TraceInit()
{
    if (fopen_s(&traceFile, tracePath, "wt") != 0)
    {
        std::cout << "Invalid path for Trace." << std::endl;
        throw;
    }

    if (!traceFile) return;
    fprintf_s(traceFile, "--------TRACE--------\n");
    fflush(traceFile);
}

void TraceClose()
{
    if (!traceFile) return;
    fclose(traceFile);
    traceFile = nullptr;
}

void TraceMessage(const char* message, ...)
{
    if (!traceFile) return;

    va_list arg_ptr;
    va_start(arg_ptr, message);
    vfprintf_s(traceFile, message, arg_ptr);
    fprintf_s(traceFile, "\n");
    va_end(arg_ptr);

    std::cout << message << std::endl;

    fflush(traceFile);
}