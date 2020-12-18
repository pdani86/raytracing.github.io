#include "scene.h"

namespace myrt
{

color Scene::ray_color(const ray& r, int depth) {
    constexpr double EPSILON = 0.001;
    hit_record rec;
    //const color oneColor{1.0, 1.0, 1.0};

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world->hit(r, EPSILON, infinity, rec))
        return background;

    auto reflectDir = reflect(r.dir, rec.normal);
    Material* pMaterial = nullptr;
    if(rec.materialId >= 0 && rec.materialId <= materials.size()-1) {
        pMaterial = &materials[rec.materialId];
    }

    color directLightSum;
    for(auto& curLight : lights) {
        vec3 dir = curLight->pos - rec.p;
        double dist = dir.length();
        ray toLight(rec.p, dir/dist);
        hit_record light_hit;
        if(world->hit(toLight, EPSILON, dist - EPSILON, light_hit)) continue;
        double cosLight = dot(rec.normal, dir);
        cosLight /= dist;
        //if(cosLight < 0) continue;
        if(cosLight < 0) cosLight *= -1.0;

        color diffuseColor;// = oneColor;
        if(pMaterial)
            diffuseColor = pMaterial->diffuse;

        directLightSum += cosLight * curLight->getColor(dir) * diffuseColor;
    }


    color reflected;
    color emitted;
    color ambient;
    color diffuse;
    //color refracted;
    color specular;

    ray reflectRay(rec.p, reflectDir);
    if(pMaterial) {
        color reflectInColor = ray_color(reflectRay, depth - 1);
        reflected = pMaterial->reflective * reflectInColor;
    }

    return emitted + directLightSum + reflected;
}

}
