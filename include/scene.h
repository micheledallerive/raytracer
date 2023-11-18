//
// Created by michele on 18.11.23.
//

#pragma once

#include "light.h"
#include "objects/object.h"
#include "tracers/naive.h"
#include "tracers/tracer.h"
#include <memory>
#include <vector>

template<typename T = NaiveTracer, typename = std::enable_if_t<std::is_base_of_v<Tracer, T>>>
class Scene
{
private:
    std::vector<Light> lights;
    T tracer;

public:
    Scene() : lights(), tracer() {}
    ~Scene() = default;

    template<typename O>
    void addObject(O object)
    {
        tracer.addObject(object);
    }

    template<typename L>
    void addLight(L light)
    {
        lights.push_back(std::move(light));
    }

    [[nodiscard]] std::optional<Hit> intersect(const Ray &ray) const
    {
        return tracer.trace(ray);
    }

    std::vector<Light> &getLights() { return lights; }
};
