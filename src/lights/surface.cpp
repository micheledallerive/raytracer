//
// Created by michele on 15.12.23.
//

#include "lights/surface.h"

SurfaceLight::SurfaceLight(const std::shared_ptr<Object> &object)
    : SurfaceLight(glm::vec3(1), object)
{
}

SurfaceLight::SurfaceLight(glm::vec3 color, const std::shared_ptr<Object> &object)
    : Light(color, object->getSamples(SurfaceLight::SAMPLES)), object(object)
{
}
std::shared_ptr<Object> SurfaceLight::getLightObject() const
{
    return object;
}
