#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>

#include "rtweekend.h"

#include "mem.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "color.h"
#include "material.h"

class Renderer
{
public:
    Renderer(unsigned int w, unsigned int h, camera& cam): cam(cam), image_width(w), image_height(h) {}

    void render(int lineFrom, int lineTo, std::function<void(int,int,double)> progressFunc);
    void render(int lineFrom = 0, int lineTo = -1) {render(lineFrom, lineTo, [](int, int, double) {});}
    void renderMultiThreaded(unsigned char N = 4);

    color ray_color(const ray& r, int depth) {return ray_color(r, depth, make_shared<hittable_pdf>(nullptr, point3(0.0, 0.0, 0.0)));}
    color ray_color(const ray& r, int depth, const shared_ptr<hittable_pdf>& _lights_pdf);

    std::vector<double>& getImage() {return image;}

private:


public:
    camera cam;
    unsigned int image_width = 100;
    unsigned int image_height = 100;
    int samples_per_pixel = 1;
    color background;
    shared_ptr<hittable> world;
    int max_depth = 20;
    shared_ptr<hittable> lights;

private:
    std::vector<double> image;
    std::mutex imageMutex;
};

void Renderer::render(int lineFrom, int lineTo, std::function<void(int,int,double)> progressFunc)  {
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    auto imageSize = 3*image_width*image_height;

    {
        std::lock_guard<std::mutex> lock(imageMutex);
        if(imageSize != image.size()) {
            image.resize(imageSize);
            memset(image.data(), 0, image.size()*sizeof(double));
        }
    }

    auto _lights_pdf = make_shared<hittable_pdf>(nullptr, point3(0.0, 0.0, 0.0));
    if(lineTo<0) lineTo = image_height -1;
    for (int j = lineTo; j >= lineFrom; --j) {
        //std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                double ru = (samples_per_pixel == 1)?0.0:random_double();
                double rv = (samples_per_pixel == 1)?0.0:random_double();
                auto u = (i + ru) / (image_width-1);
                auto v = (j + rv) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, max_depth, _lights_pdf);
            }
            auto pixel_ix = image_width * 3 * j + 3 * i;
            image[pixel_ix+2] = pixel_color.x();
            image[pixel_ix+1] = pixel_color.y();
            image[pixel_ix+0] = pixel_color.z();
        }
        int linesReady = abs(j - lineTo) + 1;
        int linesAll = abs(lineTo-lineFrom) + 1;
        double progress = linesReady/static_cast<double>(linesAll);
        progressFunc(linesReady, linesAll, progress);
    }
}

color Renderer::ray_color(
    const ray& r,
    int depth,
    const shared_ptr<hittable_pdf>& _lights_pdf
) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world->hit(r, 0.001, infinity, rec))
        return background;

    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;

    _lights_pdf->ptr = lights;
    _lights_pdf->o = rec.p;

    if (srec.is_specular) {
        return srec.attenuation
             * ray_color(srec.specular_ray, depth-1, _lights_pdf);
    }

    mixture_pdf p(_lights_pdf, srec.pdf_ptr);
    ray scattered = ray(rec.p, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());

    return emitted
         + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
                            * ray_color(scattered, depth-1, _lights_pdf)
                            / pdf_val;
}


void Renderer::renderMultiThreaded(unsigned char N) {
    std::vector<std::thread> threads;
    int lastLineEnd = -1;

    for(int i=0; i<N; ++i) {
        int lineFrom = lastLineEnd + 1;
        int step = image_height/N;
        int lineTo = lineFrom + step - 1;
        if(i==N-1) lineTo += image_height % N;
        lastLineEnd = lineTo;
        threads.emplace_back([=]() {
            auto progressFunc = [=](int linesReady, int linesAll, double progress) {
                std::cerr << "render thread[" << std::to_string(i) << "] progress: " << std::to_string(progress) << std::endl;
            };
            render(lineFrom, lineTo, progressFunc);
        });
    }

    for(std::size_t i = 0;i<threads.size(); ++i) {
        threads[i].join();
    }
}

#endif // RENDERER_H
