//
// Created by michele on 17.11.23.
//

#include "tracers/tracer.h"
Tracer::Tracer() : objects() {}

Tracer::Tracer(std::vector<std::shared_ptr<Object>> &objects) : objects(std::move(objects))
{
}
std::vector<std::shared_ptr<Object>> &Tracer::getObjects()
{
    return objects;
}
