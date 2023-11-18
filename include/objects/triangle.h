//
// Created by michele on 17.10.23.
//

#pragma once

#include "object.h"
#include "plane.h"

class Triangle: public Object
{
private:
    inline float _subarea(const glm::vec3 &point, const glm::vec3 &next, const glm::vec3 &prev)
    {
        const glm::vec3 n = glm::cross(next - point, prev - point);
        const float sign = glm::sign(glm::dot(n, this->normal));
        return glm::length(n) * (sign >= 0.0f ? 1.0f : -1.0f) * 0.5f;
    }

public:
    const std::array<glm::vec3, 3> points = {glm::vec3(0.0f)};

    const float area{glm::length(glm::cross(points[1] - points[0], points[2] - points[0])) / 2};
    const std::optional<std::array<glm::vec3, 3>> face_normals{std::nullopt};
    const glm::vec3 normal{glm::normalize(glm::cross(points[1] - points[0], points[2] - points[0]))};
    const glm::vec3 centroid{(points[0] + points[1] + points[2]) / 3.0f};

    ~Triangle() override = default;

    explicit Triangle(std::array<glm::vec3, 3> points, const Material &material) : Object(material), points(points)
    {
    }

    explicit Triangle(std::array<glm::vec3, 3> points) : Object(), points(points)
    {
    }

    Triangle(std::array<glm::vec3, 3> points, const std::array<glm::vec3, 3> fnormals, const Material &material)
        : Object(material), points(points), face_normals(fnormals)
    {
    }

    Triangle(std::array<glm::vec3, 3> points, const std::array<glm::vec3, 3> fnormals)
        : Object(), points(points), face_normals(fnormals)
    {
    }

    inline bool is_inside(const glm::vec3 &point)
    {
        return this->_subarea(point, this->points[1], this->points[2]) >= 0
            && this->_subarea(point, this->points[2], this->points[0]) >= 0
            && this->_subarea(point, this->points[0], this->points[1]) >= 0;
    }

    glm::vec3 compute_hit_normal(const glm::vec3 &point)
    {
        if (this->face_normals) {
            const float a1 = this->_subarea(point, this->points[1], this->points[2]);
            const float a2 = this->_subarea(point, this->points[2], this->points[0]);
            const float a3 = this->_subarea(point, this->points[0], this->points[1]);
            const float total_area = a1 + a2 + a3;
            return glm::normalize(
                (a1 / total_area) * this->face_normals->at(0) + (a2 / total_area) * this->face_normals->at(1)
                + (a3 / total_area) * this->face_normals->at(2));
        }
        return this->normal;
    }

    std::optional<Hit> intersect(const Ray &ray) override
    {
        const std::optional<Hit> plane_hit = Plane(coordsToGlobal(this->points[0], 1), coordsToGlobal(this->normal, 0)).intersect(ray);
        if (!plane_hit)
            return plane_hit;

        if (!is_inside(coordsToLocal(plane_hit->intersection, 1)))
            return std::nullopt;

        return Hit{
            coordsToGlobal(compute_hit_normal(coordsToLocal(plane_hit->intersection, 1)), 0),
            plane_hit->intersection,
            plane_hit->distance,
            this};
    }

protected:
    Box computeBoundingBox() override
    {
        glm::vec3 min = this->points[0];
        glm::vec3 max = this->points[0];
        for (auto &point : this->points) {
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
        return {min, max};
    }
};
