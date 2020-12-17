#ifndef WORLD_EXAMPLE_H
#define WORLD_EXAMPLE_H


#include "rtweekend.h"

#include "aarect.h"
#include "box.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>

#include "heightmap.h"
#include "film.h"

#include "renderer.h"

#include "bmp.h"

inline hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    //objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    //objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    /*shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);*/

    auto glass = make_shared<dielectric>(1.5);
    //objects.add(make_shared<sphere>(point3(190,90,190), 90 , glass));

    return objects;
}

inline shared_ptr<hittable> createHeightMap(const point3 lightPos) {
    auto heightmap0 = make_shared<heightmap>(20, 20, make_shared<metal>(color(0.95, 0.95, 0.95), 0.0));
    //heightmap0->material = make_shared<lambertian>(color(1.0, 1.0, 1.0));
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
    heightmap0->generateGeometry();
    return make_shared<translate>(heightmap0, lightPos + vec3(0.0, -30.0, 0.0));
}

inline std::pair<shared_ptr<hittable_list>, shared_ptr<hittable_list>> createExampleWorld() {
    // World

    const point3 lightPos(555/2, 50, 555/2);

    auto lights = make_shared<hittable_list>();
    //lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    lights->add(make_shared<sphere>(lightPos, 1, shared_ptr<material>()));
    //lights->add(make_shared<sphere>(lightPos + vec3(0.0, 600.0, 0.0), 1, shared_ptr<material>()));
    lights->add(make_shared<sphere>(vec3(200.0, 100.0, -50.0), 1, shared_ptr<material>()));
    //lights->add(make_shared<sphere>(point3(190, 500, 190), 90, shared_ptr<material>()));
    //lights->add(make_shared<sphere>(point3(0, 10, 190), 90, shared_ptr<material>()));
    //auto dbgMarkerSphere = make_shared<sphere>(vec3(lightPos.x(), lightPos.y()+350, lightPos.z()), 10.0, make_shared<lambertian>(color(1.0, 0.4, 0.3)));
    //auto dbgMarkerSphere2 = make_shared<sphere>(vec3(50,50,50), 10.0, make_shared<lambertian>(color(1.0, 0.4, 0.3)));
    //auto dbgMarkerSphere3 = make_shared<sphere>(vec3(150,50,50), 10.0, make_shared<lambertian>(color(1.0, 0.4, 0.3)));
    //auto dbgMarkerSphere4 = make_shared<sphere>(vec3(150,350,50), 10.0, make_shared<lambertian>(color(1.0, 0.4, 0.3)));
    auto triangle0 = make_shared<triangle>(point3(50, 50, 50), point3(150, 50, 50), point3(150, 350, 50));
    triangle0->mat = make_shared<lambertian>(color(1.0, 0.4, 0.3));

    auto world = make_shared<hittable_list>(cornell_box());
    /*world->add(dbgMarkerSphere2);
    world->add(dbgMarkerSphere3);
    world->add(dbgMarkerSphere4);*/
    int blockSquareSize = 80;
    int filmSize = 555/2;

    auto heightmap0 = createHeightMap(lightPos);

    auto film0 = make_shared<film>(
                point3(lightPos.x() - filmSize/2, lightPos.y() + 400, lightPos.z() - filmSize/2),
                vec3(1.0 * filmSize, 0.0, 0.0),
                vec3(0.0, 0.0, 1.0 * filmSize),
                1000,
                1000
          );

    //world.add(dbgMarkerSphere);
    film0->mat = make_shared<lambertian>(color(1.0, 0.4, 0.3));
    world->add(film0);
    auto blockRect = make_shared<xz_rect>(
                      lightPos.x() - blockSquareSize/2, lightPos.x() + blockSquareSize/2,
                      lightPos.z() - blockSquareSize/2, lightPos.z() + blockSquareSize/2,
                      lightPos.y() + 70,
                      make_shared<lambertian>(color(1.0, 1.0, 1.0)));

    //world.add(blockRect);
    world->add(heightmap0);
    //world->add(triangle0);

    std::pair<shared_ptr<hittable_list>, shared_ptr<hittable_list>> result;
    result.first = world;
    result.second = lights;
    return result;
}

#endif
