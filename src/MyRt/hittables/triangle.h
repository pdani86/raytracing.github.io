#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

namespace myrt {


class Triangle : public hittable
{
public:
    Triangle(point3 a, point3 b, point3 c) : a(a), b(b), c(c)  {}

    bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    bool bounding_box(double time0, double time1, aabb& output_box) const override;

    point3 a;
    point3 b;
    point3 c;

    //std::shared_ptr<material> mat;
};

inline bool Triangle::hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 ab = b - a;
    vec3 bc = c - b;
    vec3 ca = a - c;

    vec3 normal = cross(ab, ca);
    auto normalLen = normal.length();
    if(normalLen < 0.0001) return false;

    normal = normal / normalLen; // TODO: can be precalculated
    double num = dot(normal, a - r.orig);
    double den = dot(normal, r.dir);
    if(fabs(den)<0.0001) return false;
    //if(den > 0) return false; // TODO which side to allow (front face/back face)

    double t = num / den;
    if(t<t_min || t>t_max) return false;

    point3 hitPoint = r.orig + t * r.dir;
    vec3 aToHit = hitPoint - a;

    vec3 nAB = cross(normal, ab);
    vec3 nBC = cross(normal, bc);
    vec3 nCA = cross(normal, ca);
    vec3 cToHit = hitPoint - c;
    double dAB = dot(nAB, aToHit);
    double dBC = dot(nBC, cToHit);
    double dCA = dot(nCA, aToHit);
    bool sideAB = dAB >= 0;
    bool sideBC = dBC >= 0;
    bool sideCA = dCA >= 0;
    bool isInside = (sideAB == sideBC) && (sideAB == sideCA);
    if(!isInside) return false;


    double abLen = ab.length();
    double acLen = ca.length();
    vec3 uVec = ab / abLen;
    vec3 vVec = ca / (acLen*-1);
    double u = dot(aToHit, uVec) / abLen;
    double v = dot(aToHit, vVec) / acLen;

    rec.normal = normal;
    rec.t = t;
    rec.p = hitPoint;
    rec.u = u;
    rec.v = v;
    //rec.mat_ptr = mat;
    rec.front_face = true;
    return true;
}

inline bool Triangle::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box.maximum = a.maxCoords(b.maxCoords(c));
    output_box.minimum = a.minCoords(b.minCoords(c));
    return true;
}

}

#endif // TRIANGLE_H
