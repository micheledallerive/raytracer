#pragma once

#include "glm/glm.hpp"
#include "ray.h"
#include <optional>
class Box
{
public:
    glm::vec3 min{};
    glm::vec3 max{};

    Box() = default;

    Box(const glm::vec3 &min, const glm::vec3 &max) : min(glm::min(min, max)), max(glm::max(min, max))
    {
    }

    [[nodiscard]] glm::vec3 getMin() const
    {
        return min;
    }

    [[nodiscard]] glm::vec3 getMax() const
    {
        return max;
    }

    [[nodiscard]] glm::vec3 getCenter() const
    {
        return (min + max) * 0.5f;
    }

    [[nodiscard]] std::optional<Hit> intersect(const Ray &ray) const
    {
        const glm::vec3 invDir = 1.0f / ray.direction;
        const glm::vec3 t0 = (min - ray.origin) * invDir;
        const glm::vec3 t1 = (max - ray.origin) * invDir;
        const glm::vec3 tmin = glm::min(t0, t1);
        const glm::vec3 tmax = glm::max(t0, t1);
        const float tNear = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
        const float tFar = glm::min(glm::min(tmax.x, tmax.y), tmax.z);
        if (tNear > tFar || tFar < 0) {
            return std::nullopt;
        }
        return Hit{glm::vec3(0), glm::vec3(0), tNear, nullptr, glm::vec2(0)};
    }

    void merge(const Box &box)
    {
        min.x = glm::min(min.x, box.min.x);
        min.y = glm::min(min.y, box.min.y);
        min.z = glm::min(min.z, box.min.z);

        max.x = glm::max(max.x, box.max.x);
        max.y = glm::max(max.y, box.max.y);
        max.z = glm::max(max.z, box.max.z);
    }

    Box operator+(const Box &box) const
    {
        Box newBox;
        newBox.merge(box);
        return newBox;
    }
};
