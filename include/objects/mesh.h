//
// Created by michele on 17.10.23.
//

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <array>
#include "object.h"
#include "triangle.h"
#include "sphere.h"

#define USE_BOUNDING_SPHERE 1

enum MeshType
{
	OBJ
};

class Mesh : public Object
{
 private:
	glm::vec3 _position;
	std::string _name;
	std::vector<std::unique_ptr<Triangle>> _triangles;
#if USE_BOUNDING_SPHERE
	glm::vec3 _centroid{ 0.0f };
	std::unique_ptr<Sphere> _bounding_sphere;
#endif
 public:
	virtual ~Mesh() = default;

	Mesh(std::string name, glm::vec3 position, std::vector<std::unique_ptr<Triangle>> triangles)
		: _name(std::move(name)), _position(position), _triangles(std::move(triangles))
	{
#if USE_BOUNDING_SPHERE
		// compute centroid
		float triangles_area = 0.0f;
		for (auto& triangle : this->_triangles)
		{
			this->_centroid += triangle->centroid * triangle->area;
			triangles_area += triangle->area;
		}
		this->_centroid /= triangles_area;

		// compute bounding sphere
		float max_distance = 0.0f;
		for (auto& triangle : this->_triangles)
		{
			for (auto& point : triangle->points)
			{
				const float distance = glm::distance(point, this->_centroid);
				if (distance > max_distance)
					max_distance = distance;
			}
		}
		this->_bounding_sphere = std::make_unique<Sphere>(max_distance, this->_centroid, Material{ glm::vec3(0.0f),
																								   glm::vec3(0.0f),
																								   glm::vec3(0.0f),
																								   0 });
#endif
	}

	std::optional<Hit> intersect(const Ray& ray) override
	{
#if USE_BOUNDING_SPHERE
		if (!this->_bounding_sphere->intersect(ray))
			return std::nullopt;
#endif
		std::optional<Hit> closest_hit = std::nullopt;
		for (auto& triangle : this->_triangles)
		{
			std::optional<Hit> current_hit = triangle->intersect(ray);
			if (current_hit && (!closest_hit || current_hit->distance < closest_hit->distance))
				closest_hit = current_hit;
		}
		return closest_hit;
	}
};
