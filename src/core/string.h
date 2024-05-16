#pragma once
#ifndef XNE_STR_IMPL
#define XNE_STR_IMPL

#include <stddef.h>

/*
    Return a new allocated array of string. Thoses are from an str and then separated by a char.
*/
const char** xne_str_split(char* __str, const char __delim, unsigned int* __count);

/*
    Insert a string into an other.
*/
const char* xne_str_insert(const char* __v0, const char* __v1, const size_t __offset);

/*
    Extract a double from a string.
*/
double xne_str_to_digit(const char* __v0, size_t __size);

/*
    Insert values into a formatted string.
*/
const char* xne_str_format(const char* v0, ...);

/*
    Merge two strings into a single one.
*/
void xne_str_merge(const char* __v0, const char* __v1, char* __dest);

#endif