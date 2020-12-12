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
};

inline bool triangle::hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const {
    return false;
}

inline bool triangle::bounding_box(double time0, double time1, aabb& output_box) const {
    return false;
}

inline double triangle::pdf_value(const point3& o, const vec3& v) const {
    return 0.0;
}

inline vec3 triangle::random(const point3& o) const {
    return vec3(0.0, 0.0, 0.0);
}

#endif // TRIANGLE_H
