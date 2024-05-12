#pragma once

#ifndef XNE_FILE_IMPL
#define XNE_FILE_IMPL

#include <stdint.h>
#include <stdio.h>

/*
    Read 8 bits (char) from a file.
*/
int xne_freadw8(FILE* __file);

/*
    Read 16 bit (short) from a file.
*/
int16_t xne_fread16(FILE* __file);

/*
    Read 16 bits (unsigned short) from a file.
*/
uint16_t xne_freadw16(FILE* __file);

/*
    Read 32 bits (integer) from a file.
*/
int32_t xne_fread32(FILE* __file);

/*
    Read 32 bits (unsigned int) from a file.
*/
uint32_t xne_freadw32(FILE* __file);

/*
    Return the size of a file. If an error occurs it return -1.
*/
size_t xne_fsize(FILE* __file);

#endif