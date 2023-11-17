#pragma once

class Box
{
private:
    glm::vec3 min;
    glm::vec3 max;

public:
    Box() = default;

    Box(const glm::vec3 &min, const glm::vec3 &max) : min(min), max(max)
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
};
