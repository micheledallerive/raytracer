//
// Created by michele on 17.10.23.
//

#pragma once

#include "object.h"

/**
 Implementation of the class Object for sphere shape.
 */
class Sphere: public Object
{
public:
    /**
	 The constructor of the sphere
	 @param color Color of the sphere
	 */
    Sphere(const glm::vec3 &color)
        : Object(color)
    {
    }

    Sphere(const Material &material)
        : Object(material)
    {
    }

    /** Implementation of the intersection function */
    std::optional<Hit> intersect(const Ray &ray) override
    {
        const Ray local_ray = this->transformRayToLocal(ray);
        const float c2 = glm::dot(local_ray.origin, local_ray.origin);

        const float a = glm::dot(-local_ray.origin, local_ray.direction);
        const float D2 = c2 - a * a;

        if (D2 > 1)
            return std::nullopt;

        const float b = std::sqrt(1 - D2);

        const float t1 = std::min(a - b, a + b);
        const float t2 = std::max(a - b, a + b);

        if (t1 < 0 && t2 < 0)
            return std::nullopt;

        const float t = t1 < 0 ? t2 : t1;

        const glm::vec3 intersection = local_ray.origin + t * local_ray.direction;
        const glm::vec3 normal = glm::normalize(intersection);


        const auto u = normal.x != 0 && normal.z != 0 ? (float) (0.5 + atan2(normal.z, normal.x) / (2 * M_PI)) : 0;
        const auto v = (float) (0.5 + asin(normal.y) / M_PI);

        return this->transformHitToGlobal(Hit{normal, intersection, t, this, {u, v}}, ray);
    }

    [[nodiscard]] std::vector<glm::vec3> getSamples(int n) const override
    {
        std::vector<glm::vec3> samples;
        samples.reserve(n);
        for (int i = 0; i < n; i++) {
            const float theta = 2 * M_PI * (i + 0.5f) / n;
            const float phi = acos(1 - 2 * (i + 0.5f) / n);
            const auto point = glm::vec3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
            samples.push_back(coordsToGlobal(point, 1));
        }
        return samples;
    }

protected:
    Box computeBoundingBox() override
    {
        return {coordsToGlobal(glm::vec3(-1, -1, -1), 1), coordsToGlobal(glm::vec3(1, 1, 1), 1)};
    }
};