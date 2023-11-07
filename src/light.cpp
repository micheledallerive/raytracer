#include "glm/glm.hpp"
#include "light.h"

Light::Light(glm::vec3 position) : position(position), color(glm::vec3(1.0))
{
}

Light::Light(glm::vec3 position, glm::vec3 color) : position(position), color(color)
{
}
