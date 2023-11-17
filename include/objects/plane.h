//
// Created by michele on 17.10.23.
//

#pragma once


#include "../glm/glm.hpp"
#include "../material.h"
#include "../ray.h"
#include "object.h"

class Plane: public Object
{

private:
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec3 point = glm::vec3(0);

public:
    Plane(const glm::vec3 &point, const glm::vec3 &normal) : Object(), normal(normal), point(point)
    {
    }

    Plane(const glm::vec3 &point, const glm::vec3 &normal, const Material &material)
        : Object(material), normal(normal), point(point)
    {
    }

    std::optional<Hit> intersect(const Ray &ray) override
    {
        const float normal_dot = glm::dot(normal, ray.direction);
        if (normal_dot == 0)
            return std::nullopt;

        const float t = glm::dot(normal, point - ray.origin) / normal_dot;
        if (t < 0)
            return std::nullopt;

        const glm::vec3 intersection = ray.origin + t * ray.direction;
        return Hit{normal, intersection, t, this};
    }

protected:
    Box computeBoundingBox() override
    {
        return {glm::vec3(-INFINITY), glm::vec3(INFINITY)};
    }
};