
#include "ray_source.h"

namespace myrt
{

bool RaySourcePoint::shootRay(const ray& r, hittable& target, int max_depth) {
    if(max_depth<=0) return false;
    hit_record rec;
    if(target.hit(r, 0.001, infinity, rec)) {
        auto reflectDir = reflect(r.dir, rec.normal);
        ray rNew(rec.p, reflectDir);
        shootRay(rNew, target, max_depth - 1);
    }
    return true;
}

double RaySourcePoint::getIntensity(const vec3& dir) {
    return 1.0;
}

void RaySourcePoint::shootRays(hittable& target, int max_depth) {
    double angleStep = cutOff * 2.0 / (angleRes - 1);
    double u = -1.0 * cutOff;
    double v = -1.0 * cutOff;
    while(v <= cutOff) {
        u = -1.0 * cutOff;
        while(u <= cutOff) {
            vec3 dir =
                    direction * std::cos(u) * std::cos(v) +
                    right * std::cos(u) * std::sin(v) +
                    up * std::sin(u);
            //std::cerr << "shoot ray (" << std::to_string(u) << ", " << std::to_string(v) << ") (" << std::to_string(dir.x()) << ", " << std::to_string(dir.y()) << ", " << std::to_string(dir.z()) << ")" << std::endl;
            ray r(position, dir);
            shootRay(r, target, max_depth);
            u += angleStep;
        }
        v += angleStep;
    }
}

}
