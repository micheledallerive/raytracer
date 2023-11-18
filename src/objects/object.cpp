
#include "objects/object.h"
#include "objects/box.h"
Box &Object::getBoundingBox()
{
    if (boundingBox.has_value()) {
        return boundingBox.value();
    }
    boundingBox.emplace(computeBoundingBox());
    return boundingBox.value();
}


[[nodiscard]] std::optional<Hit> Object::transformHitToGlobal(const std::optional<Hit> &&hit, const Ray &ray) const
{
    if (hit) {
        const glm::vec3 globalIntersection = glm::vec3(transformationMatrix * glm::vec4(hit->intersection, 1));
        return Hit{
            glm::normalize(glm::vec3(normalMatrix * glm::vec4(hit->normal, 0))),
            globalIntersection,
            glm::distance(globalIntersection, ray.origin),
            hit->object,
            hit->uv};
    }
    return std::nullopt;
}

[[nodiscard]] Ray Object::transformRayToLocal(const Ray &ray) const
{
    return {glm::vec3(inverseTransformationMatrix * glm::vec4(ray.origin, 1)),
            glm::normalize(glm::vec3(inverseTransformationMatrix * glm::vec4(ray.direction, 0)))};
}
glm::vec3 Object::coordsToGlobal(const glm::vec3 &point, float w = 0) const
{
    return {transformationMatrix * glm::vec4(point, w)};
}

glm::vec3 Object::coordsToLocal(const glm::vec3 &point, float w = 0) const
{
    return {inverseTransformationMatrix * glm::vec4(point, w)};
}