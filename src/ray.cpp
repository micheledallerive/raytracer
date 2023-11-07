#include "../include/ray.h"
#include "../include/glm/glm.hpp"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(
	origin + 0.0001f * direction), direction(direction)
{
}
