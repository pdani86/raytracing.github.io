#ifndef __MYRT_SCENE__
#define __MYRT_SCENE__

#include <memory>
#include <vector>

#include "hittable.h"
#include "light.h"
#include "material.h"

namespace myrt
{

class Scene
{
public:
    color ray_color(const ray& r, int depth);
    unsigned int addMaterial(const Material& newMat);
public:
    color background;
    std::shared_ptr<hittable> world;
    std::vector<std::shared_ptr<Light>> lights;
    std::vector<Material> materials;
};

inline unsigned int Scene::addMaterial(const Material& newMat) {
    materials.emplace_back(newMat);
    return materials.size() - 1;
}



}

#endif
