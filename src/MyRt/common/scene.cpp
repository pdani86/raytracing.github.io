#include "scene.h"

namespace myrt
{

color Scene::ray_color(const ray& r, int depth) {
    constexpr double EPSILON = 0.001;
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world->hit(r, EPSILON, infinity, rec))
        return background;

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
        directLightSum += cosLight * curLight->getColor(dir);
    }

    auto reflectDir = reflect(r.dir, rec.normal);
    ray reflectRay(rec.p, reflectDir);
    //color reflectInColor = ray_color(reflectRay, depth - 1);

    color emitted;
    color ambient;
    color diffuse;
    color reflected;
    //color refracted;
    color specular;

    return emitted + directLightSum;
}

}
