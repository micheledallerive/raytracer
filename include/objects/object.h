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
 private:
	const std::variant<glm::vec3,
					   Material> surface; ///< Surface of the object: either a color (i.e. vec3) or a material
 public:

	Object() : surface(glm::vec3(1.0))
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

};