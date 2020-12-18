#ifndef WORLD_EXAMPLE_H
#define WORLD_EXAMPLE_H

#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "heightmap.h"
#include "light.h"
//#include "film.h"

#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
//#include "renderer.h"

//#include "bmp.h"
/*
inline hittable_list cornell_box() {
    return objects;
}*/

using myrt::heightmap;
using myrt::triangle;
using myrt::hittable;
using myrt::hittable_list;
using myrt::point3;
using myrt::Light;

inline void _add_quad(hittable_list& list, const std::array<point3, 4>& points, bool reverse = false) {
    // TODO: reverse CW, CCW
    auto t1 = std::make_shared<triangle>(points[0], points[1], points[2]);
    auto t2 = std::make_shared<triangle>(points[0], points[2], points[3]);
    list.add(t1);
    list.add(t2);
}


inline hittable_list my_box() {
    hittable_list list;
    constexpr double size = 100.0;
    constexpr double halfSize = size/2.0;
    // points: bottom-top | front-rear | left-right
    point3 bfl(-halfSize, 0.0, -halfSize);
    point3 brl(-halfSize, 0.0, halfSize);
    point3 brr(halfSize, 0.0, halfSize);
    point3 bfr(-halfSize, 0.0, -halfSize);

    point3 tfl(-halfSize, size, -halfSize);
    point3 trl(-halfSize, size, halfSize);
    point3 trr(halfSize, size, halfSize);
    point3 tfr(-halfSize, size, -halfSize);
    _add_quad(list, {bfl, brl, brr, bfr});
    _add_quad(list, {tfl, trl, trr, tfr}, true);
    _add_quad(list, {bfl, tfl, trl, brl});
    _add_quad(list, {bfr, tfr, trr, brr}, true);

    return list;
}

inline std::shared_ptr<hittable> createHeightMap(const point3 lightPos) {
    auto heightmap0 = std::make_shared<heightmap>(20, 20/*, std::make_shared<metal>(color(0.95, 0.95, 0.95), 0.0)*/);
    //heightmap0->material = std::make_shared<lambertian>(color(1.0, 1.0, 1.0));
    heightmap0->step *= 2.5;

    const auto ySize = heightmap0->map_data.size();
    const auto xSize = heightmap0->map_data.at(0).size();
    double centerX = xSize/2.0;
    double centerY = ySize/2.0;

    heightmap0->generateData([=](double x, double y) {
        double dx = (x - centerX);
        double dy = (y - centerY);

        dx *= 2.5;
        dy *= 2.5;
        return ((dx*dx + dy*dy) * 0.15 + std::cos(2*M_PI*(dx+dy)/40.0) * 15.0);
    });
    heightmap0->generateGeometry(lightPos + vec3(0.0, -30.0, 0.0));
    //return make_shared<translate>(heightmap0, lightPos + vec3(0.0, -30.0, 0.0));
    return heightmap0;
}

inline std::pair<shared_ptr<hittable_list>, std::vector<std::shared_ptr<Light>>> createExampleWorld() {
    // World
    const point3 lightPos(0.0, 10.0, 0.0);
    auto world = std::make_shared<hittable_list>(/*cornell_box()*/);
    world->add(std::make_shared<hittable_list>(my_box()));
    int blockSquareSize = 80;
    int filmSize = 555/2;

    auto heightmap0 = createHeightMap(lightPos);
/*
    auto film0 = make_shared<film>(
                point3(lightPos.x() - filmSize/2, lightPos.y() + 400, lightPos.z() - filmSize/2),
                vec3(1.0 * filmSize, 0.0, 0.0),
                vec3(0.0, 0.0, 1.0 * filmSize),
                1000,
                1000
          );

    world->add(film0);
    auto blockRect = make_shared<xz_rect>(
                      lightPos.x() - blockSquareSize/2, lightPos.x() + blockSquareSize/2,
                      lightPos.z() - blockSquareSize/2, lightPos.z() + blockSquareSize/2,
                      lightPos.y() + 70,
                      make_shared<lambertian>(color(1.0, 1.0, 1.0)));

    //world.add(blockRect);*/
    world->add(heightmap0);

    std::pair<std::shared_ptr<hittable_list>, std::vector<std::shared_ptr<Light>>> result;
    result.first = world;
    result.second = std::vector<std::shared_ptr<Light>>();
    auto light = std::make_shared<Light>(lightPos);
    auto light2 = std::make_shared<Light>(point3(0.0, 99.5, 0.0));
    result.second.emplace_back(light);
    return result;
}


#endif
