#include "light.h"

#include <memory.h>

void xne_create_light(xne_Light_t* light, xne_LightType_t type){
    xne_assert(light);

    memset(light, 0, sizeof(xne_Light_t));
}