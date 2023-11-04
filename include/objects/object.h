//
// Created by michele on 17.10.23.
//

#pragma once

#include <variant>
#include <optional>

#include "../glm/glm.hpp"
#include "../material.h"
#include "../ray.h"

/**
 General class for the object
 */
class Object
{
 protected:
	const std::variant<glm::vec3,
					   Material> surface; ///< Surface of the object: either a color (i.e. vec3) or a material

	glm::mat4 transformationMatrix = glm::mat4(1.0f); ///< Matrix representing the transformation from the local to the global coordinate system
	glm::mat4 inverseTransformationMatrix = glm::mat4(1.0f); ///< Matrix representing the transformation from the global to the local coordinate system
	glm::mat4 normalMatrix = glm::mat4(1.0f); ///< Matrix for transforming normal vectors from the local to the global coordinate system

	[[nodiscard]] std::optional<Hit> transformHitToGlobal(const std::optional<Hit>&& hit, const Ray& ray) const
	{
		if (hit)
		{
			const glm::vec3 globalIntersection = glm::vec3(transformationMatrix * glm::vec4(hit->intersection, 1));
			return Hit{
				.normal = glm::normalize(glm::vec3(normalMatrix * glm::vec4(hit->normal, 0))),
				.intersection = globalIntersection,
				.distance = glm::distance(globalIntersection, ray.origin),
				.object = hit->object,
				.uv = hit->uv
			};
		}
		return std::nullopt;
	}
	[[nodiscard]] Ray transformRayToLocal(const Ray& ray) const
	{
		return { glm::vec3(inverseTransformationMatrix * glm::vec4(ray.origin, 1)),
				 glm::normalize(glm::vec3(inverseTransformationMatrix * glm::vec4(ray.direction, 0))) };
	}
 public:
	Object() : surface(Material())
	{
	}

	explicit Object(const glm::vec3& color) : surface(color)
	{
	}

	explicit Object(const Material& material) : surface(material)
	{
	}

	/** A function computing an intersection, which returns the structure Hit */
	virtual std::optional<Hit> intersect(const Ray& ray) = 0;

	template<typename T> std::optional<T> getSurfaceSafe() const
	{
		if (std::holds_alternative<T>(surface))
		{
			return std::get<T>(surface);
		}
		return std::nullopt;
	}

	/**
	 * @brief Get the Surface object without checking the type
	 * @tparam T the type of the surface to get
	 * @return the surface value for the type
	 * @throws bad_variant_access if the type is not the one expected
	 */
	template<typename T> T getSurface() const
	{
		return std::get<T>(surface);
	}

	void transform(const glm::mat4& transformation)
	{
		transformationMatrix *= transformation;
		inverseTransformationMatrix = glm::inverse(transformationMatrix);
		normalMatrix = glm::transpose(inverseTransformationMatrix);
	}
};