#ifndef __MYRT_SCENE__
#define __MYRT_SCENE__

#include <memory>
#include <vector>

#include "hittable.h";
#include "light.h";

namespace myrt
{

class Scene
{
public:
    color ray_color(const ray& r, int depth);
public:
    color background;
    std::shared_ptr<hittable> world;
    std::vector<std::shared_ptr<Light>> lights;
};



}

#endif
