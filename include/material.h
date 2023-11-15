//
//  Material.h
//  Raytracer
//
//  Created by Piotr Didyk on 14.07.21.
//

#pragma once

#include "glm/glm.hpp"
#include <functional>

/**
 Structure describing a material of an object
 */
struct Material {
    glm::vec3 ambient = glm::vec3(0.0);         ///< Ambient coefficient
    glm::vec3 diffuse = glm::vec3(1.0);         ///< Diffuse coefficient
    glm::vec3 specular = glm::vec3(0.0);        ///< Specular coefficient
    float shininess = 0.0;                      ///< Exponent for Phong model
    std::function<glm::vec3(glm::vec2)> texture;///< Texture function (glm::vec3) -> glm::vec2
    float reflection = 0.0;                     ///< Reflection coefficient [0, 1]
    float refractive_index = 1.0;               ///< Refractive index
    float transparency = 0.0;                   ///< Transparency coefficient [0, 1]
};

class MaterialFactory
{
private:
    Material material;

public:
    MaterialFactory() = default;

    MaterialFactory &ambient(const glm::vec3 &ambient);

    MaterialFactory &diffuse(const glm::vec3 &diffuse);

    MaterialFactory &specular(const glm::vec3 &specular);

    MaterialFactory &shininess(float shininess);

    MaterialFactory &texture(std::function<glm::vec3(glm::vec2)> texture);

    MaterialFactory &reflection(float reflection);

    MaterialFactory &refractiveIndex(float refractive_index);

    MaterialFactory &transparency(float transparency);

    Material build();
};