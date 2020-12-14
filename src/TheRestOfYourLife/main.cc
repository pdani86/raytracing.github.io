//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

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

class spot_light
{
public:
    ray getRay(double u, double v) {
        ray r;
        //r.
        return r;
    }

    double getIntensity(double u, double v) {
        return 0.0;
    }

public:
    vec3 up;
    vec3 dir;
    point3 pos;
};

hittable_list cornell_box() {
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

shared_ptr<hittable> createHeightMap(const point3 lightPos) {
    auto heightmap0 = make_shared<heightmap>(50, 50, make_shared<metal>(color(0.95, 0.95, 0.95), 0.0));
    //heightmap0->material = make_shared<lambertian>(color(1.0, 1.0, 1.0));

    const auto ySize = heightmap0->map_data.size();
    const auto xSize = heightmap0->map_data.at(0).size();
    double centerX = xSize/2.0;
    double centerY = ySize/2.0;

    heightmap0->generateData([=](double x, double y) {
        double dx = (x - centerX);
        double dy = (y - centerY);

        return (dx*dx + dy*dy) * 0.15 + std::cos(2*M_PI*(dx+dy)/40.0) * 15.0;
    });
    heightmap0->generateGeometry();
    return make_shared<translate>(heightmap0, lightPos + vec3(0.0, -30.0, 0.0));
}

int main() {
    // Image

    const auto aspect_ratio = 1.0 / 1.0;
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1;
    const int max_depth = 1;

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
    auto triangle0 = make_shared<triangle>(point3(50, 50, 50), point3(150, 50, 50), point3(150, 150, 50));
    triangle0->mat = make_shared<lambertian>(color(1.0, 0.4, 0.3));

    auto world = cornell_box();
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

    /*std::cerr << "film corner: " << std::to_string(film0->corner.x()) << ", " << std::to_string(film0->corner.y()) << ", " << std::to_string(film0->corner.z()) << std::endl;
    std::cerr << "film side1: " << std::to_string(film0->side1.x()) << ", " << std::to_string(film0->side1.y()) << ", " << std::to_string(film0->side1.z()) << std::endl;
    std::cerr << "film side2: " << std::to_string(film0->side2.x()) << ", " << std::to_string(film0->side2.y()) << ", " << std::to_string(film0->side2.z()) << std::endl;
    std::cerr << "film side1res: " << std::to_string(film0->side1res) << std::endl;*/

    //world.add(dbgMarkerSphere);
    film0->mat = make_shared<lambertian>(color(1.0, 0.4, 0.3));
    world.add(film0);
    auto blockRect = make_shared<xz_rect>(
                      lightPos.x() - blockSquareSize/2, lightPos.x() + blockSquareSize/2,
                      lightPos.z() - blockSquareSize/2, lightPos.z() + blockSquareSize/2,
                      lightPos.y() + 70,
                      make_shared<lambertian>(color(1.0, 1.0, 1.0)));

    //world.add(blockRect);
    world.add(heightmap0);
    world.add(triangle0);

    color background(0,0,0);
    //color background(0.05,0.05,0.05);

    // Camera

    //point3 lookfrom(278, 278, -800);
    point3 lookfrom(278, 200, -800);
    //point3 lookfrom(555/2, 555/2, -800);
    //point3 lookfrom(555/2, 1400, -200);
    point3 lookat(555/2, 555/2, 552/2);

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 50.0;
    auto time0 = 0.0;
    auto time1 = 0.0;

    // ----
    /*lookfrom = lightPos;
    vup = vec3(0.0, 0.0, 1.0);
    lookat = lookfrom + vec3(0.0, -1.0, 0.0);
    vfov = 70.0;*/
    // ----


    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, time0, time1);
    Renderer renderer(image_width, image_height, cam);
    renderer.background = background;
    renderer.max_depth = max_depth;
    renderer.samples_per_pixel = samples_per_pixel;
    renderer.world = make_shared<hittable_list>(world);
    renderer.lights = lights;

    // Render
    using clock = std::chrono::steady_clock;
    auto startTime = clock::now();

    const int threadNum = 4;
    renderer.renderMultiThreaded(threadNum);

    auto endTime = clock::now();
    auto dtUs = std::chrono::duration_cast<std::chrono::microseconds>(endTime-startTime).count();
    std::cerr << "timeUs: " << std::to_string(dtUs) << std::endl;

    std::cerr << "film hit count: " << std::to_string(film0->hitCount) << std::endl;

    auto normFilmData = BMP::mapToBytePerChannelNormalize(film0->pixelData);
    auto normFilmDataRGB = BMP::grayToRGB(normFilmData);
    //BMP::saveBmpGray("film.bmp", normFilmData.data(), film0->side1res, film0->side2res);
    BMP::saveBmpRGB("film.bmp", normFilmDataRGB.data(), film0->side1res, film0->side2res);

    //writeToImage(image_width, image_height, renderer.getImage(), samples_per_pixel);
    BMP::saveBmpRGB("img.bmp", renderer.getImage(), image_width, image_height, 255.0/samples_per_pixel);

    std::cerr << "\nDone.\n";
}
