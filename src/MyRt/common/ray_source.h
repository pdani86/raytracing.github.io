#ifndef _MYRT_RAYSOURCE_H_
#define _MYRT_RAYSOURCE_H_

#include "ray.h"
#include "hittable.h"

namespace myrt
{

class RaySourcePoint
{
public:
    bool shootRay(const ray& r, hittable& target, int max_depth);
    void shootRays(hittable& target, int max_depth = 8);
    double getIntensity(const vec3& dir);

public:
    point3 position;
    vec3 direction{0.0, -1.0, 0.0};
    vec3 up{0.0, 0.0, 1.0};
    vec3 right{1.0, 0.0, 0.0};
    double cutOff = M_PI_2 / 2.0;
    int angleRes = 100;
};


}

#endif
