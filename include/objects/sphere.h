//
// Created by michele on 17.10.23.
//

#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object.h"

/**
 Implementation of the class Object for sphere shape.
 */
class Sphere : public Object
{
 private:
	float radius{ 0 };     ///< Radius of the sphere
	glm::vec3 center{ 0 }; ///< Center of the sphere

 public:
	/**
	 The constructor of the sphere
	 @param radius Radius of the sphere
	 @param center Center of the sphere
	 @param color Color of the sphere
	 */
	Sphere(const float radius, const glm::vec3& center, const glm::vec3& color)
		: radius(radius), center(center), Object(color)
	{
	}

	Sphere(const float radius, const glm::vec3& center, const Material& material)
		: radius(radius), center(center), Object(material)
	{
	}

	/** Implementation of the intersection function */
	std::optional<Hit> intersect(const Ray& ray) override
	{
		const glm::vec3 centerToOrigin = center - ray.origin;
		const float c2 = glm::dot(centerToOrigin, centerToOrigin);

		const float a = glm::dot(centerToOrigin, ray.direction);
		const float D2 = c2 - a * a;

		if (D2 > radius * radius)
			return std::nullopt;

		const float b = std::sqrt(radius * radius - D2);

		const float t1 = std::min(a - b, a + b);
		const float t2 = std::max(a - b, a + b);

		if (t1 < 0 && t2 < 0)
			return std::nullopt;

		const float t = t1 < 0 ? t2 : t1;

		const glm::vec3 intersection = ray.origin + t * ray.direction;
		const glm::vec3 normal = glm::normalize(intersection - center);

		const auto u = (float)(0.5 + atan2(normal.z, normal.x) / (2 * M_PI));
		const auto v = (float)(0.5 + asin(normal.y) / M_PI);

		return Hit{ normal, intersection, t, this, { u, v }};
	}
};
#endif //_SPHERE_H_
