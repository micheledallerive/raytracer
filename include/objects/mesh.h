//
// Created by michele on 17.10.23.
//

#pragma once

#include "../glm/ext/matrix_transform.hpp"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

class Mesh: public Object
{
private:
    std::string _name;
    std::vector<std::unique_ptr<Triangle>> _triangles;

public:
    ~Mesh() override = default;

    Mesh(std::string name, std::vector<std::unique_ptr<Triangle>> triangles)
        : _name(std::move(name)), _triangles(std::move(triangles))
    {
    }

    std::optional<Hit> intersect(const Ray &ray) override
    {
        std::optional<Hit> closest_hit = std::nullopt;
        for (auto &triangle : this->_triangles) {
            std::optional<Hit> current_hit = triangle->intersect(ray);
            if (current_hit && (!closest_hit || current_hit->distance < closest_hit->distance))
                closest_hit = current_hit;
        }
        return closest_hit;
    }

    void transform(const glm::mat4 &transformation) override
    {
        for (auto &triangle : this->_triangles) {
            triangle->transform(transformation);
        }
    }

protected:
    Box computeBoundingBox() override
    {
        auto min = glm::vec3(std::numeric_limits<float>::max());
        auto max = glm::vec3(std::numeric_limits<float>::min());
        for (auto &triangle : this->_triangles) {
            for (auto &point : triangle->points) {
                if (point.x < min.x)
                    min.x = point.x;
                if (point.y < min.y)
                    min.y = point.y;
                if (point.z < min.z)
                    min.z = point.z;
                if (point.x > max.x)
                    max.x = point.x;
                if (point.y > max.y)
                    max.y = point.y;
                if (point.z > max.z)
                    max.z = point.z;
            }
        }
        return {this->coordsToGlobal(min, 1), this->coordsToGlobal(max, 1)};
    }
};
