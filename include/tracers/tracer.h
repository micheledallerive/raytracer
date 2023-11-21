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
    explicit Tracer(std::vector<std::unique_ptr<Object>> &objects);
    virtual ~Tracer() = default;

    [[nodiscard]] std::vector<std::unique_ptr<Object>> &getObjects();

    [[nodiscard]] virtual std::optional<Hit> trace(const Ray &ray) const = 0;
};
