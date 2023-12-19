//
// Created by michele on 17.10.23.
//

#pragma once

#include "glm/glm.hpp"
#include "objects/object.h"
#include <memory>
#include <vector>
/**
 Light class
 */
class Light
{
protected:
    const glm::vec3 color;///< Color/intentisty of the light source
    const std::vector<glm::vec3> samples;

public:
    explicit Light(std::vector<glm::vec3> samples);
    Light(glm::vec3 color, std::vector<glm::vec3> samples);
    virtual ~Light() = default;

    /**
     * @return the samples of the light
     */
    [[nodiscard]] virtual const std::vector<glm::vec3> &getSamples() const;

    /**
     * @return the color of the light
     */
    [[nodiscard]] glm::vec3 getColor() const;

    [[nodiscard]] virtual std::shared_ptr<Object> getLightObject() const;
};
