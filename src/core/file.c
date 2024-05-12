#include "file.h"

int xne_freadw8(FILE* __file){
    return getc(__file);
}

int16_t xne_fread16(FILE* __file){
    short b0, b1;
    b0 = xne_freadw8(__file);
    b1 = xne_freadw8(__file);
    return ((b1 << 8) | b0);
}

uint16_t xne_freadw16(FILE* __file){
    uint8_t b0, b1;
    b0 = xne_freadw8(__file);
    b1 = xne_freadw8(__file);
    return ((b1 << 8) | b0);
}

int32_t xne_fread32(FILE* __file){
    uint8_t b0, b1, b2, b3;
    b0 = xne_freadw8(__file);
    b1 = xne_freadw8(__file);
    b2 = xne_freadw8(__file);
    b3 = xne_freadw8(__file);
    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

uint32_t xne_freadw32(FILE* __file){
    uint8_t b0, b1, b2, b3;
    b0 = xne_freadw8(__file);
    b1 = xne_freadw8(__file);
    b2 = xne_freadw8(__file);
    b3 = xne_freadw8(__file);
    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

size_t xne_fsize(FILE* __file){
    long curr = ftell(__file);
    if(curr == -1) return -1;
    if(fseek(__file, 0, SEEK_END) != 0) return -1;
    long size = ftell(__file);
    fseek(__file, curr, SEEK_SET);
    return size;
}