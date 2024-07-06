#pragma once

#ifdef __cplusplus
extern "C" {    
#endif

#include "types.h"
#include "debug.h"

#ifdef XNE_CORE_FILE
    #include "file.h"
#endif

#ifdef XNE_CORE_BUFFER
    #include "buffer.h"
#endif

#ifdef XNE_CORE_STRING
    #include "string.h"
#endif

#ifdef XNE_CORE_COMPRESSION
    #include "compression.h"
#endif

#ifdef __cplusplus
}

#endif
