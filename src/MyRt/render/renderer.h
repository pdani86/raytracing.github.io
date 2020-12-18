#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

//#include "rtweekend.h"

#include "mem.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "light.h"

#include "scene.h"

using myrt::color;
using myrt::Camera;
using myrt::vec3;
using myrt::point3;
using myrt::ray;
using myrt::hittable;
using myrt::hittable_list;
using myrt::hit_record;
using myrt::infinity;
using myrt::Light;
using myrt::dot;
using myrt::cross;
using myrt::Scene;

class AtomicCounterGuard
{
public:
    AtomicCounterGuard(std::atomic_int& c) : counter(c) {++counter;}
    ~AtomicCounterGuard() {--counter;}
private:
    std::atomic_int& counter;
};

class Renderer
{
public:
    Renderer(unsigned int w, unsigned int h, Camera& cam): cam(cam), image_width(w), image_height(h) {}

    void render(int lineFrom, int lineTo, std::function<bool(int,int,double)> progressFunc);
    void render(int lineFrom = 0, int lineTo = -1) {render(lineFrom, lineTo, [](int, int, double) -> bool {return true;});}
    std::vector<std::thread> renderMultiThreaded(unsigned char N = 4, bool async = false);
    void requestRenderStop() { bRenderCancelled = true; }

    color ray_color(const ray& r, int depth);

    std::vector<double>& getImage() {return image;}

    bool isRenderInProgress() const { return nRenderInProgress > 0; }

private:


public:
    Camera cam;
    unsigned int image_width = 100;
    unsigned int image_height = 100;
    int samples_per_pixel = 1;
    int max_depth = 20;

    std::shared_ptr<Scene> scene;

    std::unique_lock<std::mutex> lockImage() { return std::unique_lock<std::mutex>(imageMutex); }
private:
    std::vector<double> image;
    std::mutex imageMutex;
    std::atomic_int nRenderInProgress{0};
    std::atomic_bool bRenderCancelled{false};
};


#endif // RENDERER_H
