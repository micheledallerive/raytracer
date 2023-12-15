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

class SceneBuilder
{
public:
    std::vector<Light> lights;
    std::vector<std::unique_ptr<Object>> objects;
};

class Scene
{
private:
    std::vector<Light> lights;
    std::unique_ptr<Tracer> tracer;

    const glm::vec3 ambient_light = glm::vec3(0.001f);

public:
    Scene() : lights(), tracer() {}
    ~Scene() = default;

    /**
     * Setup the scene using the lambda provided.
     *
     * @param func a lambda that takes a SceneBuilder as argument and any other custom optional argument
     */
    template<typename T>
    void setup(const std::function<void(SceneBuilder &)> &func)
    {
        SceneBuilder builder;
        func(builder);
        lights = std::move(builder.lights);
        tracer = std::make_unique<T>(builder.objects);
    }

    [[nodiscard]] std::optional<Hit> intersect(const Ray &ray) const
    {
        return tracer->trace(ray);
    }

    [[nodiscard]] const std::vector<Light> &getLights() const { return lights; }

    [[nodiscard]] const glm::vec3 &getAmbientLight() const
    {
        return ambient_light;
    }
};
