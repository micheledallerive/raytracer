//
// Created by michele on 16.12.23.
//

#include "objects/square.h"

Square::Square(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, const Material &material)
    : Object(material), triangles({Triangle({p1, p2, p3}, material), Triangle({p1, p3, p4}, material)})
{
}
std::optional<Hit> Square::intersect(const Ray &ray)
{
    std::optional<Hit> closestHit;
    for (auto &triangle : triangles) {
        auto hit = triangle.intersect(ray);
        if (hit && (!closestHit || hit->distance < closestHit->distance)) {
            closestHit = hit;
        }
    }
    closestHit->object = this;
    return closestHit;
}
std::vector<glm::vec3> Square::getSamples(int n) const
{
    std::vector<glm::vec3> samples;
    // add the vertices
    for (auto &triangle : triangles) {
        for (auto &point : triangle.points) {
            samples.push_back(point);
        }
    }

    int rem_n = n - samples.size();
    if (rem_n <= 0) {
        return samples;
    }

    const auto t1 = triangles[0].getSamples(rem_n / 2);
    const auto t2 = triangles[1].getSamples(rem_n / 2);
    samples.insert(samples.end(), t1.begin(), t1.end());
    samples.insert(samples.end(), t2.begin(), t2.end());
    return samples;
}
Box Square::computeBoundingBox()
{
    return triangles[0].getBoundingBox() + triangles[1].getBoundingBox();
}