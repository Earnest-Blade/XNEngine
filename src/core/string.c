#include "string.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>

#include <stdio.h>
#include <ctype.h>

#define XNE_STR_BUFFER 2048

static char __buffer[XNE_STR_BUFFER];

const char** xne_str_split(char* __str, const char __delim, unsigned int* __count){
    const char** str;

    size_t cnt;
    for (size_t i = 0; i < strlen(__str); i++)
        if(__str[i] == __delim) cnt++;
    
    str = (const char**) malloc(cnt * sizeof(char*));
    
    *__count = 0;
    char c;
    size_t curr = 0;
    for (size_t i = 0; i < strlen(__str); i++, curr++)
    {
        c = __str[i];
        if(c == __delim){
            str[*__count] = (char*) malloc(i - curr * sizeof(char));
            memcpy(&str[*__count], &__str[i], curr * sizeof(char));
            curr = 0;
            *__count++;
        }
    }

    return str;
}

const char* xne_str_insert(const char* __v0, const char* __v1, const size_t __offset){
    char* str = (char*) calloc(strlen(__v0) + strlen(__v1) + 1, sizeof(char));
    strncpy(str, __v0, __offset);
    str[__offset] = '\0';
    strcat(str, __v1);
    strcat(str, &__v0[__offset]);
    return str;
}

double xne_str_to_digit(const char* __v0, size_t __size){
    if(__v0){
        double value;
        
        for (size_t i = 0; i < __size; i++)
        {
            if(isgraph(*(uint8_t*)__v0)){
                char* endptr;
                value = strtod(__v0, &endptr);
                if(__v0 != endptr){
                    return value;
                }
            }
            __v0++;
        }
    }

    return 0;
}

const char* xne_str_format(const char* v0, ...){
    va_list args;
    va_start(args, v0);
    
    size_t strlen = vsnprintf(0, 0, v0, args);
    va_end(args);
    
    if(strlen >= XNE_STR_BUFFER){
        fprintf(stdout, "trying to allocate more than 2048 bytes to a formatted string!");
        return NULL;
    }

    memset(&__buffer, 0, XNE_STR_BUFFER);
    va_start(args, v0);
    vsnprintf(&__buffer[0], strlen, v0, args);
    va_end(args);

    return &__buffer[0];
}

void xne_str_merge(const char* __v0, const char* __v1, char* __dest){
    const size_t len0 = strlen(__v0);
    const size_t len1 = strlen(__v1);

    __dest = malloc(len0 + len1 + 1);
    memcpy(__dest, __v0, len0);
    memcpy(&__dest[len0], __v1, len1);
    __dest[len0 + len1] = '\0';
}