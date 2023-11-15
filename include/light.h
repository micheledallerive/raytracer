//
// Created by michele on 17.10.23.
//

#pragma once

#include "glm/glm.hpp"

/**
 Light class
 */
class Light
{
public:
    glm::vec3 position;///< Position of the light source
    glm::vec3 color;   ///< Color/intentisty of the light source

    explicit Light(glm::vec3 position);
    Light(glm::vec3 position, glm::vec3 color);
};
