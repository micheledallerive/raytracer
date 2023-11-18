//
// Created by michele on 17.11.23.
//

#include "tracers/naive.h"
std::optional<Hit> NaiveTracer::trace(const Ray &ray) const
{
    std::optional<Hit> closestHit;
    for (const auto &object : objects) {
        if (!object->getBoundingBox().intersect(ray)) {
            continue;
        }
        auto hit = object->intersect(ray);
        if (hit && (!closestHit || hit->distance < closestHit->distance)) {
            closestHit = hit;
        }
    }
    return closestHit;
}
