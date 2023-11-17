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

enum MeshType
{
    OBJ
};

class Mesh: public Object
{
private:
    glm::vec3 _position;
    std::string _name;
    std::vector<std::unique_ptr<Triangle>> _triangles;
#if USE_BOUNDING_SPHERE
    glm::vec3 _centroid{0.0f};
    std::unique_ptr<Sphere> _bounding_sphere;
#endif
public:
    ~Mesh() override = default;

    Mesh(std::string name, glm::vec3 position, std::vector<std::unique_ptr<Triangle>> triangles)
        : _position(position), _name(std::move(name)), _triangles(std::move(triangles))
    {
#if USE_BOUNDING_SPHERE
        // compute centroid
        float triangles_area = 0.0f;
        for (auto &triangle : this->_triangles) {
            this->_centroid += triangle->centroid * triangle->area;
            triangles_area += triangle->area;
        }
        this->_centroid /= triangles_area;

        // compute bounding sphere
        float max_distance = 0.0f;
        for (auto &triangle : this->_triangles) {
            for (auto &point : triangle->points) {
                const float distance = glm::distance(point, this->_centroid);
                if (distance > max_distance)
                    max_distance = distance;
            }
        }
        this->_bounding_sphere = std::make_unique<Sphere>(glm::vec3(0));
        this->_bounding_sphere->transform(glm::scale(glm::mat4(1.0f), glm::vec3(max_distance)));
        this->_bounding_sphere->transform(glm::translate(glm::mat4(1.0f), this->_centroid));
#endif
    }

    std::optional<Hit> intersect(const Ray &ray) override
    {
#if USE_BOUNDING_SPHERE
        if (!this->_bounding_sphere->intersect(ray))
            return std::nullopt;
#endif
        std::optional<Hit> closest_hit = std::nullopt;
        for (auto &triangle : this->_triangles) {
            std::optional<Hit> current_hit = triangle->intersect(ray);
            if (current_hit && (!closest_hit || current_hit->distance < closest_hit->distance))
                closest_hit = current_hit;
        }
        return closest_hit;
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
