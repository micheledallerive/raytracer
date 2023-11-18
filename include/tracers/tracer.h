//
// Created by michele on 17.11.23.
//

#pragma once

#include "objects/object.h"
#include <memory>
#include <optional>
#include <vector>

class Tracer
{
protected:
    std::vector<std::unique_ptr<Object>> objects;

public:
    Tracer();
    virtual ~Tracer() = default;

    template<typename T, typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
    void addObject(T &&object);
    void addObject(std::unique_ptr<Object> &&object);
    void addObject(std::unique_ptr<Object> &object);
    void addObject(Object *object);

    [[nodiscard]] virtual std::optional<Hit> trace(const Ray &ray) const = 0;
};
