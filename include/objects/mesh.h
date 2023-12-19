//
// Created by michele on 17.10.23.
//

#pragma once

#include "../glm/ext/matrix_transform.hpp"
#include "object.h"
#include "sphere.h"
#include "tracers/kdtree.h"
#include "tracers/naive.h"
#include "triangle.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

class Mesh: public Object
{
    using TracerClass = KDTreeTracer;

private:
    std::string _name;
    std::shared_ptr<TracerClass> _tracer;
    std::vector<std::shared_ptr<Object>> _triangles;

public:
    ~Mesh() override = default;

    Mesh(std::string name, std::vector<std::shared_ptr<Object>> &triangles)
        : _name(std::move(name)), _triangles(std::move(triangles))
    {
    }

    std::optional<Hit> intersect(const Ray &ray) override
    {
        auto hit = this->_tracer->trace(ray);
        return hit;
    }

    void transform(const glm::mat4 &transformation) override
    {
        for (auto &triangle : this->_triangles) {
            triangle->transform(transformation);
        }
    }

    void initializeTracer()
    {
        this->_tracer = std::make_unique<TracerClass>(this->_triangles);
    }

protected:
    Box computeBoundingBox() override
    {
        auto min = glm::vec3(std::numeric_limits<float>::max());
        auto max = glm::vec3(std::numeric_limits<float>::min());
        for (auto &obj : this->_tracer->getObjects()) {
            auto triangle = dynamic_cast<Triangle *>(obj.get());
            const Box triangleBox = triangle->getBoundingBox();
            min.x = std::min(min.x, triangleBox.min.x);
            min.y = std::min(min.y, triangleBox.min.y);
            min.z = std::min(min.z, triangleBox.min.z);
            max.x = std::max(max.x, triangleBox.max.x);
            max.y = std::max(max.y, triangleBox.max.y);
            max.z = std::max(max.z, triangleBox.max.z);
        }
        return Box{min, max};
    }
};
