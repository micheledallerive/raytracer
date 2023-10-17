//
// Created by michele on 17.10.23.
//

#pragma once


#include "object.h"
#include "../ray.h"
#include "../material.h"
#include "../glm/glm.hpp"

class Plane : public Object
{

 private:
	glm::vec3 normal;
	glm::vec3 point;

 public:
	Plane(const glm::vec3& point, const glm::vec3& normal) : point(point), normal(normal), Object()
	{
	}
	Plane(const glm::vec3& point, const glm::vec3& normal, const Material& material)
		: point(point), normal(normal), Object(material)
	{
	}
	std::optional<Hit> intersect(const Ray& ray) override
	{
		const float normal_dot = glm::dot(normal, ray.direction);
		if (normal_dot == 0)
			return std::nullopt;

		const float t = glm::dot(normal, point - ray.origin) / normal_dot;
		if (t < 0)
			return std::nullopt;

		const glm::vec3 intersection = ray.origin + t * ray.direction;
		return Hit{ normal, intersection, t, this };
	}
};