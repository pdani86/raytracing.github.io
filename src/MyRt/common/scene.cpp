#include "scene.h"

#include <cmath>

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

    auto reflectDir = reflect(r.dir, rec.normal);
    Material* pMaterial = nullptr;
    if(rec.materialId >= 0 && rec.materialId <= materials.size()-1) {
        pMaterial = &materials[rec.materialId];
    }

    color diffuseAndSpecularSum;
    for(auto& curLight : lights) {
        vec3 toLightDir = curLight->pos - rec.p;
        double toLightDist = toLightDir.length();
        vec3 toLightDirUnit = toLightDir / toLightDist;
        ray toLight(rec.p, toLightDirUnit);
        hit_record light_hit;
        if(world->hit(toLight, EPSILON, toLightDist - EPSILON, light_hit)) continue;
        double cosLight = dot(rec.normal, toLightDirUnit);
        //if(cosLight < 0) continue;
        if(cosLight < 0) cosLight *= -1.0;

        color diffuseColor;
        if(pMaterial)
            diffuseColor = pMaterial->diffuse;

        color lightColor = curLight->getColor(toLightDir);
        diffuseAndSpecularSum += cosLight * lightColor * diffuseColor;

        /* // TODO: too bright
        if(pMaterial && pMaterial->isSpecular) {
            if(cosLight > EPSILON) {
                double cosReflOut = dot(toLightDirUnit, reflectDir);
                if(cosReflOut > 0.0) {
                    color specularColor = pMaterial->specular * lightColor * (std::pow(cosReflOut, pMaterial->specK));
                    diffuseAndSpecularSum += specularColor;
                }
            }
        }*/

    }


    color reflected;
    //color emitted;
    //color ambient;

    if(pMaterial) {
        ray reflectRay(rec.p, reflectDir);
        color reflectInColor = ray_color(reflectRay, depth - 1);
        reflected = pMaterial->reflective * reflectInColor;

        if(pMaterial->isRefractive) {
            // TODO
            //vec3 refractDir = refract()
        }
    }

    return /*emitted + */diffuseAndSpecularSum + reflected;
}

}
