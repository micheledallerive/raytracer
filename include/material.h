//
//  Material.h
//  Raytracer
//
//  Created by Piotr Didyk on 14.07.21.
//

#pragma once

#include <functional>
#include "glm/glm.hpp"
/**
 Structure describing a material of an object
 */
struct Material
{
	glm::vec3 ambient = glm::vec3(0.0); ///< Ambient coefficient
	glm::vec3 diffuse = glm::vec3(1.0); ///< Diffuse coefficient
	glm::vec3 specular = glm::vec3(0.0); ///< Specular coefficient
	float shininess = 0.0; ///< Exponent for Phong model
	std::function<glm::vec3(glm::vec2)> texture; ///< Texture function (glm::vec3) -> glm::vec2
};
