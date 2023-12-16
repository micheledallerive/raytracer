//
// Created by michele on 15.12.23.
//

#pragma once

#include "light.h"
#include "objects/object.h"

class SurfaceLight: public Light
{
private:
    constexpr static float SAMPLES = 50;
public:
    explicit SurfaceLight(const Object &object);
    SurfaceLight(glm::vec3 color, const Object &object);
};