//
// Created by michele on 17.10.23.
//

#pragma once

#include "object.h"
#include "plane.h"

class Triangle : public Object
{
 private:
	inline float _subarea(const glm::vec3& point, const glm::vec3& next, const glm::vec3& prev)
	{
		const glm::vec3 n = glm::cross(next - point, prev - point);
		const float sign = glm::sign(glm::dot(n, this->normal));
		return glm::length(n) * (sign >= 0.0f ? 1.0f : -1.0f) * 0.5f;
	}
 public:
	const std::array<glm::vec3, 3> points;

	const float area{ glm::length(glm::cross(points[1] - points[0], points[2] - points[0])) / 2 };
	const std::optional<std::array<glm::vec3, 3>> face_normals{ std::nullopt };
	const glm::vec3 normal{ glm::normalize(glm::cross(points[1] - points[0], points[2] - points[0])) };
	const glm::vec3 centroid{ (points[0] + points[1] + points[2]) / 3.0f };

	explicit Triangle(std::array<glm::vec3, 3> points, const Material& material) : points(points), Object(material)
	{
	}
	explicit Triangle(std::array<glm::vec3, 3> points) : points(points), Object()
	{
	}

	Triangle(std::array<glm::vec3, 3> points, const std::array<glm::vec3, 3> fnormals, const Material& material)
		: points(points), face_normals(fnormals), Object(material)
	{
	}

	Triangle(std::array<glm::vec3, 3> points, const std::array<glm::vec3, 3> fnormals)
		: points(points), face_normals(fnormals), Object()
	{
	}

	inline bool is_inside(const glm::vec3& point)
	{
		return this->_subarea(point, this->points[1], this->points[2]) >= 0
			   && this->_subarea(point, this->points[2], this->points[0]) >= 0
			   && this->_subarea(point, this->points[0], this->points[1]) >= 0;
	}

	glm::vec3 compute_hit_normal(const glm::vec3& point)
	{
		if (this->face_normals)
		{
			const float a1 = this->_subarea(point, this->points[1], this->points[2]);
			const float a2 = this->_subarea(point, this->points[2], this->points[0]);
			const float a3 = this->_subarea(point, this->points[0], this->points[1]);
			const float total_area = a1 + a2 + a3;
			return glm::normalize(
				(a1 / total_area) * this->face_normals->at(0) + (a2 / total_area) * this->face_normals->at(1)
				+ (a3 / total_area) * this->face_normals->at(2));
		}
		return this->normal;
	}

	std::optional<Hit> intersect(const Ray& ray) override
	{
		const std::optional<Hit> plane_hit = Plane(this->points[0], this->normal).intersect(ray);
		if (!plane_hit)
			return plane_hit;

		if (!is_inside(plane_hit->intersection))
			return std::nullopt;

		return Hit{
			.normal = compute_hit_normal(plane_hit->intersection),
			.intersection = plane_hit->intersection,
			.distance = plane_hit->distance,
			.object = this
		};
	}
};
