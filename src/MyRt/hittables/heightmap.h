#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

//#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"

#include "triangle.h"
#include "material.h"

#include <memory>
#include <vector>
#include <functional>

using std::shared_ptr;

namespace myrt {

class Heightmap : public hittable {

public:

    Heightmap(int x, int y) {
        map_data.resize(y);
        for(auto& line : map_data) {
            line.resize(x);
        }
        geometry = std::make_shared<hittable_list>();
        generateParabolaData();
        generateGeometry();
    }

    void generateGeometry(vec3 translate = vec3());
    void generateData(std::function<double(double,double)> func);
    void generateParabolaData();

    bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    bool bounding_box(double time0, double time1, aabb& output_box) const override;
    //double pdf_value(const point3& o, const vec3& v) const override;
    //vec3 random(const point3& o) const override;

public:
    shared_ptr<hittable_list> geometry;
    //shared_ptr<material> material;

    std::vector<std::vector<double>> map_data;

    double step = 1.0;
    double heightScale = 1.0;
};

inline void Heightmap::generateGeometry(vec3 translate) {
    geometry->clear();
    if(map_data.size()==0) return;
    if(map_data.at(0).size() == 0) return;
    const auto ySize = map_data.size();
    const auto xSize = map_data.at(0).size();

    double centerX = xSize/2.0;
    double centerY = ySize/2.0;

    for(std::size_t y = 0; y < ySize-1; ++y) {
        for(std::size_t x = 0; x < xSize-1; ++x) {
            point3 p1((x - centerX) * step, map_data[y][x] * heightScale, (y - centerY) * step);
            point3 p2((x + 1 - centerX) * step, map_data[y][x+1] * heightScale, (y - centerY) * step);
            point3 p3((x + 1 - centerX) * step, map_data[y+1][x+1] * heightScale, (y + 1 - centerY) * step);
            point3 p4((x - centerX) * step, map_data[y+1][x] * heightScale, (y + 1 - centerY) * step);
            p1 += translate;
            p2 += translate;
            p3 += translate;
            p4 += translate;

            auto newTriangle1 = std::make_shared<Triangle>(p1,p2,p3);
            auto newTriangle2 = std::make_shared<Triangle>(p1,p3,p4);
            //newTriangle1->mat = std::make_shared<metal>(color(0.95, 0.92, 0.9), 0.0);
            //newTriangle2->mat = std::make_shared<metal>(color(0.95, 0.92, 0.9), 0.0);
            geometry->add(newTriangle1);
            geometry->add(newTriangle2);
        }
    }
}

inline void Heightmap::generateData(std::function<double(double,double)> func) {
    if(map_data.size()==0) return;
    if(map_data.at(0).size() == 0) return;
    const auto ySize = map_data.size();
    const auto xSize = map_data.at(0).size();
    for(std::size_t y = 0; y < ySize; ++y) {
        for(std::size_t x = 0; x < xSize; ++x) {
            map_data[y][x] = func(x, y);
        }
    }
}

inline void Heightmap::generateParabolaData() {
    if(map_data.size()==0) return;
    if(map_data.at(0).size() == 0) return;
    const auto ySize = map_data.size();
    const auto xSize = map_data.at(0).size();
    double centerX = xSize/2.0;
    double centerY = ySize/2.0;

    generateData([=](double x, double y) {
        double dx = (x - centerX);
        double dy = (y - centerY);
        return (dx*dx + dy*dy);
    });
}

inline bool Heightmap::hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const {
    return geometry->hit(r, t_min, t_max, rec);
}

inline bool Heightmap::bounding_box(double time0, double time1, aabb& output_box) const {
    return geometry->bounding_box(time0, time1, output_box);
}
/*
inline double heightmap::pdf_value(const point3& o, const vec3& v) const {
    return geometry->pdf_value(o, v);
}

inline vec3 heightmap::random(const point3& o) const {
    return geometry->random(o);
}
*/

}

#endif // HEIGHTMAP_H
