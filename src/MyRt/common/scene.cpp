#include "scene.h"

namespace myrt
{

color Scene::ray_color(const ray& r, int depth) {
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
        //if(cosLight < 0) continue;
        if(cosLight < 0) cosLight *= -1.0;
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

}
