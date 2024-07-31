#include "debug.h"

#include <stdlib.h>
#include <stdarg.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <signal.h>
#endif

void xne__wmessage(FILE* stream, const int line, const char* file, const char* __message, ...){
    va_list arg_list;
    int finised;

    // print header
    if(line > 0 || file){
        fprintf(stream, "%s:%i ", file, line);
    }

    va_start(arg_list, __message);
    finised = vfprintf(stream, __message, arg_list);
    va_end(arg_list);

    // print footer
    fprintf(stream, "\n");
}

void xne__wassert(const char* __message, const char* __file, unsigned long long __line){
    xne__wmessage(stderr, -1, 0, "assertion failed: %s, %s, line %i\n", __message, __file, __line);
    
    exit(XNE_FAILURE);
}

int xne__werror(const char* __message, int code){
    xne__wmessage(stderr, -1, 0, "exited with code (%i) : %s\n", code, __message);
    
    exit(code);
}

void xne__break(){
#ifdef _MSC_VER
    __debugbreak();
#elif _WIN32
    DebugBreak();
#else
    raise(SIGABRT);
#endif
}