//
// Created by michele on 17.10.23.
//

#pragma once

#include <optional>
#include <variant>

#include "../glm/glm.hpp"
#include "../material.h"
#include "../ray.h"
#include "box.h"

/**
 General class for the object
 */
class Object
{
protected:
    const std::variant<glm::vec3, Material> surface;///< Surface of the object: either a color (i.e. vec3) or a material
    std::optional<Box> boundingBox;           ///< Bounding box of the object

    glm::mat4 transformationMatrix = glm::mat4(
        1.0f);///< Matrix representing the transformation from the local to the global coordinate system
    glm::mat4 inverseTransformationMatrix = glm::mat4(
        1.0f);///< Matrix representing the transformation from the global to the local coordinate system
    glm::mat4 normalMatrix = glm::mat4(
        1.0f);///< Matrix for transforming normal vectors from the local to the global coordinate system

    [[nodiscard]] std::optional<Hit> transformHitToGlobal(const std::optional<Hit> &&hit, const Ray &ray) const;
    [[nodiscard]] Ray transformRayToLocal(const Ray &ray) const;
    [[nodiscard]] glm::vec3 coordsToGlobal(const glm::vec3 &point, float w) const;

    [[nodiscard]] virtual Box computeBoundingBox() = 0;

public:
    virtual ~Object() = default;

    Object() : surface(Material())
    {
    }

    explicit Object(const glm::vec3 &color) : surface(color)
    {
    }

    explicit Object(const Material &material) : surface(material)
    {
    }

    /** A function computing an intersection, which returns the structure Hit */
    virtual std::optional<Hit> intersect(const Ray &ray) = 0;
    Box &getBoundingBox();

    template<typename T>
    std::optional<T> getSurfaceSafe() const
    {
        if (std::holds_alternative<T>(surface)) {
            return std::get<T>(surface);
        }
        return std::nullopt;
    }

    /**
     * @brief Get the Surface object without checking the type
     * @tparam T the type of the surface to get
     * @return the surface value for the type
     * @throws bad_variant_access if the type is not the one expected
     */
    template<typename T>
    T getSurface() const
    {
        return std::get<T>(surface);
    }

    void transform(const glm::mat4 &transformation)
    {
        transformationMatrix *= transformation;
        inverseTransformationMatrix = glm::inverse(transformationMatrix);
        normalMatrix = glm::transpose(inverseTransformationMatrix);
    }
};