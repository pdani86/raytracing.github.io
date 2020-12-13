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

#include "renderer.h"

#include "bmp.h"

class point_light
{
public:

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
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
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



int main() {
    // Image

    const auto aspect_ratio = 1.0 / 1.0;
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1;
    const int max_depth = 1;

    // World

    auto lights = make_shared<hittable_list>();
    //lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));
    //lights->add(make_shared<sphere>(point3(190, 500, 190), 90, shared_ptr<material>()));
    //lights->add(make_shared<sphere>(point3(0, 10, 190), 90, shared_ptr<material>()));

    auto world = cornell_box();
    int heightMapX = 190;
    int heightMapY = 30;
    int heightMapZ = 190;
    int blockSquareSize = 100;
    //auto heightmap0 = make_shared<heightmap>(50, 50, make_shared<metal>(color(0.95, 0.95, 0.95), 0.0));
    auto heightmap0 = make_shared<heightmap>(50,50,make_shared<lambertian>(color(1.0, 1.0, 1.0)));
    world.add(make_shared<xz_rect>(
                  heightMapX - blockSquareSize/2, heightMapX + blockSquareSize/2,
                  heightMapZ - blockSquareSize/2, heightMapZ + blockSquareSize/2,
                  heightMapY + 70,
                  make_shared<lambertian>(color(1.0, 1.0, 1.0)))
              );
    world.add(make_shared<flip_face>(make_shared<xz_rect>(
                  heightMapX - blockSquareSize/2, heightMapX + blockSquareSize/2,
                  heightMapZ - blockSquareSize/2, heightMapZ + blockSquareSize/2,
                  heightMapY + 70.5,
                  make_shared<lambertian>(color(1.0, 1.0, 1.0))))
              );

    world.add(make_shared<translate>(heightmap0,vec3(heightMapX, heightMapY,heightMapZ)));

    //color background(0,0,0);
    color background(0.05,0.05,0.05);

    // Camera

    //point3 lookfrom(278, 278, -800);
    point3 lookfrom(278, 600, -800);
    //point3 lookfrom(278, 1400, -800);
    //point3 lookfrom(278, 50, -800);
    point3 lookat(278, 278, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;
    auto time0 = 0.0;
    auto time1 = 0.0;

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


    //writeToImage(image_width, image_height, renderer.getImage(), samples_per_pixel);
    BMP::saveBmpRGB("img.bmp", renderer.getImage(), image_width, image_height, 255.0/samples_per_pixel);

    std::cerr << "\nDone.\n";
}
