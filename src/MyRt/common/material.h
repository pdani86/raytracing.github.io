#ifndef __MYRT_MATERIAL_H__
#define __MYRT_MATERIAL_H__

#include "vec3.h"

namespace myrt {

struct Material
{
    color ambient;
    color diffuse;
    color specular;
    double specK = 1.0;
    color emissive;
};

}

#endif

