
#ifndef _MYRT_TRIANGLE_SMOOTH_H_
#define _MYRT_TRIANGLE_SMOOTH_H_

#include "triangle.h"

namespace myrt
{

class TriangleSmooth : public Triangle
{
public:
    TriangleSmooth(point3 a, point3 b, point3 c, vec3 nA, vec3 nB, vec3 nC)
        : Triangle(a, b, c), nA(nA), nB(nB), nC(nC) {}
    bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;
public:
    vec3 nA;
    vec3 nB;
    vec3 nC;

};

inline bool TriangleSmooth::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    bool didHit = Triangle::hit(r, t_min, t_max, rec);
    double kA = 1.0 - rec.u - rec.v;
    rec.normal = kA * nA + rec.u * nB + rec.v * nC;
    return didHit;
}

}

#endif
