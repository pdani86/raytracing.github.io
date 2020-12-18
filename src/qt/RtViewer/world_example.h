#ifndef WORLD_EXAMPLE_H
#define WORLD_EXAMPLE_H

#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "heightmap.h"
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
using myrt::point3;

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

inline std::pair<shared_ptr<hittable_list>, std::vector<point3>> createExampleWorld() {
    // World
    const point3 lightPos(555/2, 50, 555/2);
    auto lights = std::make_shared<hittable_list>();
    //auto dbgMarkerSphere = make_shared<sphere>(vec3(lightPos.x(), lightPos.y()+350, lightPos.z()), 10.0, make_shared<lambertian>(color(1.0, 0.4, 0.3)));
    //auto dbgMarkerSphere2 = make_shared<sphere>(vec3(50,50,50), 10.0, make_shared<lambertian>(color(1.0, 0.4, 0.3)));
    //auto dbgMarkerSphere3 = make_shared<sphere>(vec3(150,50,50), 10.0, make_shared<lambertian>(color(1.0, 0.4, 0.3)));
    //auto dbgMarkerSphere4 = make_shared<sphere>(vec3(150,350,50), 10.0, make_shared<lambertian>(color(1.0, 0.4, 0.3)));
    auto triangle0 = std::make_shared<triangle>(point3(50, 50, 50), point3(150, 50, 50), point3(150, 350, 50));
    //triangle0->mat = std::make_shared<lambertian>(color(1.0, 0.4, 0.3));
    auto world = std::make_shared<hittable_list>(/*cornell_box()*/);
    /*world->add(dbgMarkerSphere2);
    world->add(dbgMarkerSphere3);
    world->add(dbgMarkerSphere4);*/
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

    //world.add(dbgMarkerSphere);
    //film0->mat = make_shared<lambertian>(color(1.0, 0.4, 0.3));
    world->add(film0);
    auto blockRect = make_shared<xz_rect>(
                      lightPos.x() - blockSquareSize/2, lightPos.x() + blockSquareSize/2,
                      lightPos.z() - blockSquareSize/2, lightPos.z() + blockSquareSize/2,
                      lightPos.y() + 70,
                      make_shared<lambertian>(color(1.0, 1.0, 1.0)));

    //world.add(blockRect);*/
    world->add(heightmap0);
    //world->add(triangle0);

    std::pair<std::shared_ptr<hittable_list>, std::vector<point3>> result;
    result.first = world;
    result.second = std::vector<point3>();
    result.second.push_back(lightPos);
    return result;
}


#endif
