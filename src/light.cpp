#include "light.h"
#include "glm/glm.hpp"

Light::Light(glm::vec3 position) : position(position), color(glm::vec3(1.0))
{
}

Light::Light(glm::vec3 position, glm::vec3 color) : position(position), color(color)
{
}
