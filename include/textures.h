//
//  Textures.h
//  Raytracer
//
//  Created by Piotr Didyk on 19.08.21.
//

#pragma once

#include <iostream>
#include <functional>
#include "glm/glm.hpp"

glm::vec3 checkerboardTexture(glm::vec2 uv);

glm::vec3 rainbowTexture(glm::vec2 uv);

/**
 * Parse a .PPM file and return a texture function
 * @param filename the PPM file
 * @return a function that maps a 2D vector to a 3D color
 */
std::function<glm::vec3(glm::vec2)> textureFromFile(const std::string& filename);