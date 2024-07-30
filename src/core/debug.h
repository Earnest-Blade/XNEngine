#pragma once

#ifndef XNE_DEBUG_IMPL
#define XNE_DEBUG_IMPL

#include <stdio.h>
#include <string.h>
#include <signal.h>

#define XNE_OK 0
#define XNE_FAILURE 1

#define XNE_INVALID_VALUE -1

#if _WIN32
    #define XNE_FILE (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #define XNE_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

void xne__wmessage(FILE* stream, const int line, const char* file, const char* __message, ...);

void xne__wassert(const char* __message, const char* __file, unsigned long long __line);
int xne__werror(const char* __message, int code);

#define xne_printf(message) (void)(xne__wmessage(stdout, __LINE__, XNE_FILE, message))
#define xne_vprintf(message, ...) (void)(xne__wmessage(stdout, __LINE__, XNE_FILE, message, __VA_ARGS__))

#define xne_assert(expression) (void) (                                         \
    (((expression) == 0) ? xne__wassert(#expression, __FILE__, __LINE__) : 0)  \
)
#define xne_error(message, code) (void) (xne__werror(#message, code))

#define xne_break() (void)(raise(SIGABRT))

#endif