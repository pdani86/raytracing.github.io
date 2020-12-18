#ifndef __MYRT_MATERIAL_H__
#define __MYRT_MATERIAL_H__

#include "vec3.h"

namespace myrt {

struct Material
{
    color ambient;
    color diffuse;
    color specular;
    color reflective;
    double specK = 1.0;
    color emissive;
    color refractive;

    bool isEmissive = false;
    bool isSpecular = true;
    bool isRefractive = false;
};

}

#endif

