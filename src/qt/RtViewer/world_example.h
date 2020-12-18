#ifndef WORLD_EXAMPLE_H
#define WORLD_EXAMPLE_H

#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "heightmap.h"
#include "light.h"
#include "scene.h"
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

using myrt::Heightmap;
using myrt::Triangle;
using myrt::hittable;
using myrt::hittable_list;
using myrt::point3;
using myrt::Light;
using myrt::color;
using myrt::Material;
using myrt::Scene;

inline void _add_quad(hittable_list& list, const std::array<point3, 4>& points, unsigned int matId = 0, bool reverse = false) {
    // TODO: reverse CW, CCW
    auto t1 = std::make_shared<Triangle>(points[0], points[1], points[2]);
    auto t2 = std::make_shared<Triangle>(points[0], points[2], points[3]);
    t1->materialId = matId;
    t2->materialId = matId;
    list.add(t1);
    list.add(t2);
}


inline hittable_list my_box() {
    hittable_list list;

    constexpr double size = 200.0;
    constexpr double halfSize = size/2.0;
    // points: bottom-top | front-rear | left-right
    point3 bfl(-halfSize, 0.0, -halfSize);
    point3 brl(-halfSize, 0.0, halfSize);
    point3 brr(halfSize, 0.0, halfSize);
    point3 bfr(halfSize, 0.0, -halfSize);

    point3 tfl(-halfSize, size, -halfSize);
    point3 trl(-halfSize, size, halfSize);
    point3 trr(halfSize, size, halfSize);
    point3 tfr(halfSize, size, -halfSize);
    _add_quad(list, {bfl, brl, brr, bfr}, 0); // bottom
    _add_quad(list, {tfl, trl, trr, tfr}, 0, true); // top
    _add_quad(list, {bfl, tfl, trl, brl}, 1); // left
    _add_quad(list, {bfr, tfr, trr, brr}, 2, true); // right
    _add_quad(list, {brl, trl, trr, brr}, 0, true); // back/rear

    return list;
}

inline std::shared_ptr<hittable> createHeightMap(const point3 lightPos) {
    auto heightmap0 = std::make_shared<Heightmap>(20, 20);
    heightmap0->step = 2.0;
    heightmap0->materialId = 3;

    const auto ySize = heightmap0->map_data.size();
    const auto xSize = heightmap0->map_data.at(0).size();
    double centerX = xSize/2.0;
    double centerY = ySize/2.0;

    double minVal = 0.0;
    double maxVal = 50.0;

    heightmap0->generateData([=](double x, double y) {
        double dx = (x - centerX);
        double dy = (y - centerY);

        dx *= 2.5;
        dy *= 2.5;

        double val = ((dx*dx + dy*dy) * 0.1 /*+ std::cos(2*M_PI*(dx+dy)/40.0) * 15.0*/);
        if(val < minVal) val = minVal;
        if(val > maxVal) val = maxVal;
        return val;
    });
    heightmap0->generateGeometry(lightPos + vec3(0.0, -30.0, 0.0));
    return heightmap0;
}


inline std::vector<Material> getExampleMaterials() {
    std::vector<Material> result;
    Material whiteSide;
    Material greenSide;
    Material redSide;
    Material reflective;

    whiteSide.diffuse = color(1.0, 1.0, 1.0);
    whiteSide.specular = color(1.0, 1.0, 1.0);
    whiteSide.reflective = color(0.2, 0.2, 0.2);

    greenSide.diffuse = color(0.3, 0.95, 0.05);
    greenSide.specular = color(0.3, 0.95, 0.05);

    redSide.diffuse = color(0.95, 0.3, 0.05);
    redSide.specular = color(0.95, 0.3, 0.05);

    reflective.diffuse = color(0.01, 0.01, 0.01);
    reflective.specular = color(0.05, 0.05, 0.05);
    reflective.reflective = color(0.9, 0.95, 0.75);

    result.emplace_back(whiteSide);
    result.emplace_back(greenSide);
    result.emplace_back(redSide);
    result.emplace_back(reflective);

    return result;
}

inline Scene createExampleWorld() {
    // World
    const point3 lightPos(0.0, 40.0, 0.0);
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

    std::vector<std::shared_ptr<Light>> lights = std::vector<std::shared_ptr<Light>>();
    //auto light = std::make_shared<Light>(lightPos);
    auto light2 = std::make_shared<Light>(point3(0.0, 165.0, 0.0));
    //auto light3 = std::make_shared<Light>(point3(70.0, 195.5, -30.0));

    light2->attenuation[2] = 0.001;

    //result.second.emplace_back(light);
    lights.emplace_back(light2);
    //result.second.emplace_back(light3);

    Scene resultScene;
    resultScene.world = world;
    resultScene.lights = lights;
    resultScene.materials = getExampleMaterials();

    return resultScene;
}


#endif
