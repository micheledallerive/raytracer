//
// Created by michele on 28.10.23.
//

#ifndef RAYTRACER_CONE_H
#define RAYTRACER_CONE_H

#include "../utils.h"
#include "object.h"
class Cone: public Object
{
private:
    std::optional<Hit> cone_body_intersect(const Ray &ray)
    {
        const Ray local_ray = this->transformRayToLocal(ray);

        const float a = local_ray.direction.x * local_ray.direction.x + local_ray.direction.z * local_ray.direction.z
            - local_ray.direction.y * local_ray.direction.y;
        const float b = 2 * local_ray.origin.x * local_ray.direction.x + 2 * local_ray.origin.z * local_ray.direction.z
            - 2 * local_ray.origin.y * local_ray.direction.y;
        const float c = local_ray.origin.x * local_ray.origin.x + local_ray.origin.z * local_ray.origin.z
            - local_ray.origin.y * local_ray.origin.y;

        const float delta = b * b - 4 * a * c;
        if (delta < 0)
            return std::nullopt;

        const float t1 = (-b - std::sqrt(delta)) / (2 * a);
        const float t2 = (-b + std::sqrt(delta)) / (2 * a);
        const float t = (t1 >= 0 && t2 >= 0) ? std::min(t1, t2) : std::max(t1, t2);
        if (t < 0)
            return std::nullopt;

        const glm::vec3 intersection = local_ray.origin + t * local_ray.direction;

        if (intersection.y < 0 || intersection.y > 1)
            return std::nullopt;

        const glm::vec3 normal = glm::normalize(glm::vec3(intersection.x, -intersection.y, intersection.z));
        return this->transformHitToGlobal(Hit{normal, intersection, t, this}, ray);
    }
    std::optional<Hit> cone_cap_intersect(const Ray &ray)
    {
        const Ray local_ray = this->transformRayToLocal(ray);

        const float t = (1 - local_ray.origin.y) / local_ray.direction.y;
        if (t < 0)
            return std::nullopt;

        const glm::vec3 intersection = local_ray.origin + t * local_ray.direction;

        if (intersection.x * intersection.x + intersection.z * intersection.z > 1)
            return std::nullopt;

        return this->transformHitToGlobal(Hit{glm::vec3(0, 1, 0), intersection, t, this}, ray);
    }

public:
    Cone() : Object()
    {
    }

    Cone(const Material &material) : Object(material)
    {
    }

    std::optional<Hit> intersect(const Ray &ray) override
    {
        return compare_non_empty(this->cone_body_intersect(ray), this->cone_cap_intersect(ray),
                                 [](const Hit &h1, const Hit &h2) {
                                     return h1.distance < h2.distance;
                                 });
    }
};

#endif//RAYTRACER_CONE_H
