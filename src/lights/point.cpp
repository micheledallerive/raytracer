//
// Created by michele on 15.12.23.
//

#include "lights/point.h"

PointLight::PointLight(glm::vec3 position) : Light({position}), position(position)
{
}

PointLight::PointLight(glm::vec3 position, glm::vec3 color) : Light(color, {position}), position(position)
{
}

glm::vec3 PointLight::getPosition() const
{
    return position;
}
