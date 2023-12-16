#include "lights/light.h"
#include "glm/glm.hpp"
#include <utility>
#include <vector>

Light::Light(std::vector<glm::vec3> samples) : color(), samples(std::move(samples))
{
}

Light::Light(glm::vec3 color, std::vector<glm::vec3> samples) : color(color), samples(std::move(samples))
{
}

glm::vec3 Light::getColor() const
{
    return color;
}

const std::vector<glm::vec3> &Light::getSamples() const
{
    return samples;
}
std::shared_ptr<Object> Light::getLightObject() const
{
    return nullptr;
}
