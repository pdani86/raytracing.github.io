#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "rtweekend.h"

#include "hittable.h"

class triangle : public hittable
{
public:
    triangle(point3 a, point3 b, point3 c) : a(a), b(b), c(c)  {}

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
    virtual double pdf_value(const point3& o, const vec3& v) const override;
    virtual vec3 random(const point3& o) const override;

    point3 a;
    point3 b;
    point3 c;

    shared_ptr<material> mat;
};

inline bool triangle::hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 normal = cross(ab, ac);
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
    double abLen = ab.length();
    double acLen = ac.length();
    vec3 aToHit = hitPoint - a;
    vec3 uVec = ab / abLen;
    vec3 vVec = ac / acLen;
    double u = dot(aToHit, uVec) / abLen;
    double v = dot(aToHit, vVec) / acLen;
    if(u < 0) return false;
    if(v < 0) return false;
    if(u>1.0) return false;
    if(v>1.0) return false;
    if(u+v>1.0) return false;
    rec.normal = normal;
    rec.t = t;
    rec.p = hitPoint;
    rec.u = u;
    rec.v = v;
    rec.mat_ptr = mat;
    rec.front_face = true;
    return true;
}

inline bool triangle::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box.maximum = a.maxCoords(b.maxCoords(c));
    output_box.minimum = a.minCoords(b.minCoords(c));
    return true;
}

inline double triangle::pdf_value(const point3& o, const vec3& v) const {
    return 0.0;
}

inline vec3 triangle::random(const point3& o) const {
    return vec3(0.0, 0.0, 0.0);
}

#endif // TRIANGLE_H
