#include "compression.h"

#include <stdio.h>

#include <memory.h>
#include <string.h>
#include <malloc.h>

#include <zlib.h>

#define XNE_BUFFER_SIZE 256

int xne_inflate(char** v0, size_t* size){
    z_stream zstream;
    memset(&zstream, 0, sizeof(z_stream));
    zstream.zalloc = NULL;
    zstream.zfree = NULL;
    zstream.opaque = NULL;

    if(inflateInit(&zstream) != Z_OK){
        fprintf(stderr, "failed to initialize zlib");
        return 0;
    }

    int ret;
    char* buffer = (char*) malloc(XNE_BUFFER_SIZE);
    size_t bufsize = 0;
    
    do
    {
        if(bufsize % XNE_BUFFER_SIZE == 0){
            buffer = (char*) realloc(buffer, bufsize + XNE_BUFFER_SIZE);
            if(!buffer){
                fprintf(stderr, "failed to realloc buffer");
                inflateEnd(&zstream);
                free(buffer);
                return 0;
            }
        }        

        zstream.avail_in = *size - zstream.total_in;
        zstream.next_in = *v0 + zstream.total_in;
        zstream.avail_out = XNE_BUFFER_SIZE;
        zstream.next_out = buffer + bufsize;

        ret = inflate(&zstream, Z_NO_FLUSH);
        bufsize += XNE_BUFFER_SIZE - zstream.avail_out;

    } while (ret != Z_STREAM_END);
    
    inflateEnd(&zstream);

    *v0 = (char*) realloc(*v0, bufsize + 1);
    strncpy(*v0, buffer, bufsize);
    *size = bufsize;
    
    free(buffer);
}