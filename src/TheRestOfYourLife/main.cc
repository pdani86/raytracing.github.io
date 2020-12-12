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

#include "myutils.h"
#include "heightmap.h"


color ray_color(
    const ray& r,
    const color& background,
    const hittable& world,
    shared_ptr<hittable> lights,
    int depth
) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;

    if (srec.is_specular) {
        return srec.attenuation
             * ray_color(srec.specular_ray, background, world, lights, depth-1);
    }

    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr);
    ray scattered = ray(rec.p, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());

    return emitted
         + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
                            * ray_color(scattered, background, world, std::move(lights), depth-1)
                            / pdf_val;
}


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

class Renderer
{
public:
    Renderer(int w, int h, camera& cam): cam(cam), image_width(w), image_height(h) {}

    void render(int lineFrom = 0, int lineTo = -1) {
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        int imageSize = 3*image_width*image_height;

        {
            std::lock_guard<std::mutex> lock(imageMutex);
            if(imageSize != image.size()) {
                image.resize(imageSize);
                memset(image.data(), 0, image.size()*sizeof(double));
            }
        }

        if(lineTo<0) lineTo = image_height -1;
        for (int j = lineTo; j >= lineFrom; --j) {
            std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    double ru = (samples_per_pixel == 1)?0.0:random_double();
                    double rv = (samples_per_pixel == 1)?0.0:random_double();
                    auto u = (i + ru) / (image_width-1);
                    auto v = (j + rv) / (image_height-1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, background, *world, lights, max_depth);
                }
                auto pixel_ix = image_width * 3 * j + 3 * i;
                image[pixel_ix+2] = pixel_color.x();
                image[pixel_ix+1] = pixel_color.y();
                image[pixel_ix+0] = pixel_color.z();
                //write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }
    }

    std::vector<double>& getImage() {return image;}

public:
    int image_width = 100;
    int image_height = 100;
    int samples_per_pixel = 1;
    color background;
    shared_ptr<hittable> world;
    int max_depth;
    shared_ptr<hittable> lights;
    camera cam;
private:
    std::vector<double> image;
    std::mutex imageMutex;
};

int main() {
    // Image

    const auto aspect_ratio = 1.0 / 1.0;
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1;
    const int max_depth = 5;

    // World

    auto lights = make_shared<hittable_list>();
    lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));

    auto world = cornell_box();
    //auto heightmap0 = make_shared<heightmap>(50,50,make_shared<metal>(color(0.8, 0.85, 0.88), 0.0));
    auto heightmap0 = make_shared<heightmap>(50,50,make_shared<lambertian>(color(1.0, 1.0, 1.0)));
    world.add(make_shared<translate>(heightmap0,vec3(190,90,190)));
    //world.add(make_shared<translate>(heightmap0,vec3(278,278,-200)));
    //world.add(heightmap0);

    //color background(0,0,0);
    color background(0.5,0.5,0.5);

    // Camera

    point3 lookfrom(278, 278, -800);
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
    std::vector<std::thread> threads;
    int lastLineStart = 0;
    int lastLineEnd = -1;
    const int N = 4;
    for(int i=0; i<N; ++i) {
        int lineFrom = lastLineEnd + 1;
        int step = image_height/4;
        int lineTo = lineFrom + step - 1;
        if(i==N-1) lineTo += image_height % N;
        lastLineStart = lineFrom;
        lastLineEnd = lineTo;
        threads.emplace_back(std::thread([=, &renderer]() {
            renderer.render(lineFrom, lineTo);
        }));

    }


    using clock = std::chrono::steady_clock;
    auto startTime = clock::now();

    for(int i = 0;i<threads.size(); ++i) {
        threads[i].join();
    }

    auto endTime = clock::now();
        auto dtUs = std::chrono::duration_cast<std::chrono::microseconds>(endTime-startTime).count();
        std::cerr << "timeUs: " << std::to_string(dtUs) << std::endl;

    writeToImage(image_width, image_height, renderer.getImage(), samples_per_pixel);

    std::cerr << "\nDone.\n";
}
