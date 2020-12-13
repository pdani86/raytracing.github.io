#ifndef FILM_H
#define FILM_H

#include "rtweekend.h"
#include "hittable.h"

#include <vector>
#include <mem.h>
#include <atomic>


class film : public hittable {
public:
    /*using pixel_intensitiy = double;
    using pixel_dir = vec3;
    using pixel = std::pair<pixel_intensitiy, pixel_dir>;*/
    using pixel = double;

    film():side1res(1000), side2res(1000) {
        pixelData.resize(side1res*side2res);
        clear();
    }

    film(point3 corner_, vec3 uSide, vec3 vSide, int uRes = 1000, int vRes = 1000)
            : corner(corner_), side1(uSide), side2(vSide), side1res(uRes), side2res(vRes) {
        pixelData.resize(side1res*side2res);
        clear();
    }

    void clear() {
        //memset(pixelData.data(), 0, pixelData.size() * sizeof(decltype(pixelData)::value_type));
        memset((char*)pixelData.data(), 0, pixelData.size() * sizeof(pixel));
    }

    bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    bool bounding_box(double time0, double time1, aabb& output_box) const override;
    double pdf_value(const point3& o, const vec3& v) const override;
    vec3 random(const point3& o) const override;

    shared_ptr<material> mat;
    point3 corner;
    vec3 side1{1.0, 0.0, 0.0};
    vec3 side2{0.0, 0.0, 1.0};
    mutable std::vector<pixel> pixelData; // TODO: not threadsafe
    int side1res = 1000;
    int side2res = 1000;

    mutable std::atomic_uint64_t hitCount{0};
};

bool film::hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 normal = cross(side1, side2);
    auto normalLen = normal.length();
    if(normalLen < 0.0001) return false;
    normal = normal / normalLen; // TODO: can be precalculated
    double num = dot(normal, corner - r.orig);
    double den = dot(normal, r.dir);
    if(den<0.0001) return false;
    double t = num / den;
    if(t<t_min || t>t_max) return false;
    point3 hitPoint = r.orig + t * r.dir;
    double side1Len = side1.length();
    double side2Len = side2.length();
    vec3 cornerToHit = hitPoint - corner;
    vec3 uVec = side1 / side1Len;
    vec3 vVec = side2 / side2Len;
    double u = dot(cornerToHit, uVec) / side1res;
    double v = dot(cornerToHit, vVec) / side2res;
    if(u < 0) return false;
    if(v < 0) return false;
    int uCoord = u * side1res;
    int vCoord = v * side2res;
    if(uCoord>=side1res) return false;
    if(vCoord>=side2res) return false;
    int ix = side1res * vCoord + uCoord;
    if(ix>= pixelData.size()) {
        return false;
    }
    pixelData[ix] += 1.0;
    rec.normal = normal;
    rec.t = t;
    rec.p = hitPoint;
    rec.u = u;
    rec.v = v;
    rec.mat_ptr = mat;
    rec.front_face = true;
    ++hitCount;
    return true;
}

bool film::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box.maximum = vec3(
                std::max(corner.x() + side1.x(), corner.x() + side2.x()),
                std::max(corner.y() + side1.y(), corner.y() + side2.y()),
                std::max(corner.z() + side1.z(), corner.z() + side2.z())
                );
    output_box.minimum = vec3(
                std::min(corner.x() + side1.x(), corner.x() + side2.x()),
                std::min(corner.y() + side1.y(), corner.y() + side2.y()),
                std::min(corner.z() + side1.z(), corner.z() + side2.z())
                );
    return true;
}


double film::pdf_value(const point3& o, const vec3& v) const {
    return 0.0;
}

vec3 film::random(const point3& o) const {
    return random_unit_vector();
}

#endif // FILM_H
