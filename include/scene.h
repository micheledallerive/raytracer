//
// Created by michele on 18.11.23.
//

#pragma once

#include "lights/light.h"
#include "lights/surface.h"
#include "objects/object.h"
#include "tracers/naive.h"
#include "tracers/tracer.h"
#include <memory>
#include <vector>

class SceneBuilder
{
private:
    std::vector<std::shared_ptr<Light>> lights;
    std::vector<std::shared_ptr<Object>> objects;

    friend class Scene;

public:
    SceneBuilder() : lights(), objects() {}
    ~SceneBuilder() = default;

    template<typename L>
    void addLight(const L light)
    {
        lights.emplace_back(light);
    }

    template<typename O>
    void addObject(const O object)
    {
        objects.emplace_back(object);
    }

    template<typename O>
    void addLightObject(const O object, const glm::vec3 color)
    {
        objects.emplace_back(object);
        const auto obj_ptr = objects.back();
        obj_ptr->setSurface(color);
        lights.emplace_back(std::make_shared<SurfaceLight>(color, obj_ptr));
    }
};

class Scene
{
private:
    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<Tracer> tracer;

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

    [[nodiscard]] const std::vector<std::shared_ptr<Light>> &getLights() const { return lights; }

    [[nodiscard]] const glm::vec3 &getAmbientLight() const
    {
        return ambient_light;
    }
};
