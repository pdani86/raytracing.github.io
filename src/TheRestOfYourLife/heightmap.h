#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"

#include "triangle.h"
#include "sphere.h"
#include "material.h"

#include <memory>
#include <vector>
#include <functional>

using std::shared_ptr;


class heightmap : public hittable {

public:

    heightmap(int x, int y, shared_ptr<material> material) : material(material) {
        map_data.resize(y);
        for(auto& line : map_data) {
            line.resize(x);
        }
        geometry = make_shared<hittable_list>();
        generateParabolaData();
        generateGeometry();
    }

    void generateGeometry();
    void generateData(std::function<double(double,double)> func);
    void generateParabolaData();

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
    virtual double pdf_value(const point3& o, const vec3& v) const override;
    virtual vec3 random(const point3& o) const override;

public:
    shared_ptr<hittable_list> geometry;
    shared_ptr<material> material;

    std::vector<std::vector<double>> map_data;

    double step = 5.0;
    double heightScale = 1.0;
};

inline void heightmap::generateGeometry() {
    geometry->clear();
    if(map_data.size()==0) return;
    if(map_data.at(0).size() == 0) return;
    const auto ySize = map_data.size();
    const auto xSize = map_data.at(0).size();

    //vec3 offset(190.0, 90.0, 190.0);

    for(std::size_t y = 0; y < ySize; ++y) {
        for(std::size_t x = 0; x < xSize; ++x) {
            //std::cerr << "add sphere: " << std::to_string(x*step) << ", " << std::to_string(y*step) << std::endl;
            geometry->add(make_shared<sphere>(
                              point3(
                                x * step,
                                  map_data[y][x] * heightScale,
                                  y * step
                                  )/* + offset*/,
                              step/2.0,
                              material
                              ));
        }
    }
}

inline void heightmap::generateData(std::function<double(double,double)> func) {
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

inline void heightmap::generateParabolaData() {
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

inline bool heightmap::hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const {
    return geometry->hit(r, t_min, t_max, rec);
}

inline bool heightmap::bounding_box(double time0, double time1, aabb& output_box) const {
    return geometry->bounding_box(time0, time1, output_box);
}

inline double heightmap::pdf_value(const point3& o, const vec3& v) const {
    return geometry->pdf_value(o, v);
}

inline vec3 heightmap::random(const point3& o) const {
    return geometry->random(o);
}



#endif // HEIGHTMAP_H
