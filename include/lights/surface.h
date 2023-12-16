//
// Created by michele on 15.12.23.
//

#pragma once

#include "light.h"
#include "objects/object.h"

class SurfaceLight: public Light
{
private:
    constexpr static int SAMPLES = 50;

    const std::shared_ptr<Object> object;

public:
    explicit SurfaceLight(const std::shared_ptr<Object> &object);
    SurfaceLight(glm::vec3 color, const std::shared_ptr<Object> &object);

    [[nodiscard]] std::shared_ptr<Object> getLightObject() const override;
};