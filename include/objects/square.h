//
// Created by michele on 16.12.23.
//
#pragma once

#include "glm/glm.hpp"
#include "material.h"
#include "ray.h"
#include "scene.h"
#include "triangle.h"

class Square: public Object
{
private:
    std::array<Triangle, 2> triangles;

public:
    Square(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, const Material &material);

    [[nodiscard]] std::optional<Hit> intersect(const Ray &ray) override;
    [[nodiscard]] std::vector<glm::vec3> getSamples(int n) const override;

protected:
    Box computeBoundingBox() override;
};