//
// Created by michele on 17.10.23.
//

#pragma once

#include "glm/glm.hpp"

/**
 Class representing a single ray.
 */
class Ray
{
 public:
	glm::vec3 origin;    ///< Origin of the ray
	glm::vec3 direction; ///< Direction of the ray
	/**
	 Contructor of the ray
	 @param origin Origin of the ray
	 @param direction Direction of the ray
	 */
	Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction)
	{
	}
};

class Object;
/**
 Structure representing the even of hitting an object
 */
struct Hit
{
	glm::vec3 normal;       ///< Normal vector of the intersected object at the intersection point
	glm::vec3 intersection; ///< Point of Intersection
	float distance;         ///< Distance from the origin of the ray to the intersection point
	Object* object;         ///< A pointer to the intersected object
	glm::vec2 uv;            ///< Coordinates for computing the texture (texture coordinates)
};
