//
// Created by michele on 17.11.23.
//

#include "tracers/tracer.h"
Tracer::Tracer() : objects() {}
template<typename T, typename>
void Tracer::addObject(T &&object)
{
    objects.emplace_back(object);
}
void Tracer::addObject(std::unique_ptr<Object> &&object)
{
    objects.emplace_back(std::move(object));
}
void Tracer::addObject(std::unique_ptr<Object> &object)
{
    objects.emplace_back(std::move(object));
}
void Tracer::addObject(Object *object)
{
    objects.emplace_back(object);
}
