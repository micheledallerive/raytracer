//
// Created by michele on 15.12.23.
//

#include "lights/surface.h"

SurfaceLight::SurfaceLight(const Object &object) : Light(object.getSamples(SurfaceLight::SAMPLES))
{
}

SurfaceLight::SurfaceLight(glm::vec3 color, const Object &object) : Light(color, object.getSamples(SurfaceLight::SAMPLES))
{
}