//
// Created by michele on 17.11.23.
//

#pragma once

#include "tracer.h"

class NaiveTracer: public Tracer
{
public:
    NaiveTracer() = default;
    explicit NaiveTracer(std::vector<std::unique_ptr<Object>> &objects) : Tracer(objects) {}

    [[nodiscard]] std::optional<Hit> trace(const Ray &ray) const override;
};
