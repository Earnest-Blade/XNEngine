#pragma once

#ifdef __cplusplus
extern "C" {    
#endif

#include "types.h"

#ifdef XNE_CORE_FILE
    #include "file.h"
#endif

#ifdef XNE_CORE_BUFFER
    #include "buffer.h"
#endif

#ifdef XNE_CORE_STRING
    #include "str.h"
#endif

#ifdef XNE_CORE_COMPRESSION
    #include "compression.h"
#endif

#ifdef __cplusplus
}

#endif
