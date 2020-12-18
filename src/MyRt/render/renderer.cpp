
#include "renderer.h"


void Renderer::render(int lineFrom, int lineTo, std::function<bool(int,int,double)> progressFunc)  {
    AtomicCounterGuard renderThreadCounter(nRenderInProgress);
    if(!world) {
        std::cerr << "Renderer::render: No World\n";
        return;
    }
    if(lights.empty()) {
        std::cerr << "Renderer::render: No Lights\n";
        return;
    }

    auto imageSize = 3*image_width*image_height;

    {
        std::lock_guard<std::mutex> lock(imageMutex);
        if(imageSize != image.size()) {
            image.resize(imageSize);
            memset(image.data(), 0, image.size()*sizeof(double));
        }
    }

    //auto _lights_pdf = make_shared<hittable_pdf>(nullptr, point3(0.0, 0.0, 0.0));
    if(lineTo<0) lineTo = image_height -1;
    for (int j = lineTo; j >= lineFrom; --j) {
        //std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                //double ru = (samples_per_pixel == 1)?0.0:random_double();
                //double rv = (samples_per_pixel == 1)?0.0:random_double();
                double ru = 0.0;
                double rv = 0.0;
                auto u = (i + ru) / (image_width-1);
                auto v = (j + rv) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, max_depth);
            }
            auto pixel_ix = image_width * 3 * j + 3 * i;
            image[pixel_ix+2] = pixel_color.x();
            image[pixel_ix+1] = pixel_color.y();
            image[pixel_ix+0] = pixel_color.z();
        }
        int linesReady = abs(j - lineTo) + 1;
        int linesAll = abs(lineTo-lineFrom) + 1;
        double progress = linesReady/static_cast<double>(linesAll);
        if(!progressFunc(linesReady, linesAll, progress)) {
            return;
        }
    }
}

color Renderer::ray_color(const ray& r, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world->hit(r, 0.001, infinity, rec))
        return background;

    color directLightSum;
    for(auto& curLight : lights) {
        vec3 dir = curLight->pos - rec.p;
        double dist = dir.length();
        ray toLight(rec.p, dir/dist);
        hit_record light_hit;
        if(world->hit(toLight, 0.001, dist - 0.001, light_hit)) continue;
        double cosLight = dot(rec.normal, dir);
        cosLight /= dist;
        if(cosLight < 0) continue;
        directLightSum += cosLight * curLight->getColor(dir);
    }

    color emitted;
    color ambient;
    color diffuse;
    color reflected;
    //color refracted;
    color specular;

    return emitted + directLightSum;
}


std::vector<std::thread> Renderer::renderMultiThreaded(unsigned char N, bool async) {
    bRenderCancelled = false;
    std::vector<std::thread> threads;
    int lastLineEnd = -1;

    for(int i=0; i<N; ++i) {
        int lineFrom = lastLineEnd + 1;
        int step = image_height/N;
        int lineTo = lineFrom + step - 1;
        if(i==N-1) lineTo += image_height % N;
        lastLineEnd = lineTo;
        threads.emplace_back([=]() {
            auto progressFunc = [=](int linesReady, int linesAll, double progress) -> bool {
                //std::cerr << "render thread[" << std::to_string(i) << "] progress: " << std::to_string(progress) << std::endl;
                return !bRenderCancelled;
            };
            render(lineFrom, lineTo, progressFunc);
        });
    }

    if(!async) {
        for(std::size_t i = 0;i<threads.size(); ++i) {
            threads[i].join();
        }
        return {};
    }
    return threads;
}
