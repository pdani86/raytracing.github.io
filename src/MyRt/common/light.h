#ifndef __MYRT_LIGHT_H__
#define __MYRT_LIGHT_H__

#include "vec3.h"

namespace myrt
{

struct Light
{
public:
    Light(vec3 position): pos(position) {}
    //virtual color getColor(const vec3& dir) = 0;
    virtual color getColor(const vec3& dir);
public:
    point3 pos;
    color col{1.0, 1.0, 1.0};
    vec3 attenuation{1.0, 0.0, 0.0}; // a + bx + cx^2
};

inline color Light::getColor(const vec3& dir) {
    double d2 = dir.length_squared();
    double d = std::sqrt(d2);
    double scale = 1.0 / (attenuation[0] + attenuation[1] * d + attenuation[2] * d2);
    return col * scale;
}

}

#endif
