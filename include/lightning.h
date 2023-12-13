//
// Created by michele on 13.12.23.
//
#pragma once

#include "glm/glm.hpp"
#include "material.h"
#include "ray.h"
#include "scene.h"

const constexpr int MAX_RAY_DEPTH = 10;

/**
 Functions that computes a color along the ray
 @param ray Ray that should be traced through the scene
 @return Color at the intersection point
 */
glm::vec3 trace_ray(const Scene &scene, const Ray &ray, int depth = 0, float refl_cumulative = 1.0f, float refr_cumulative = 1.0f);

/**
 Function performing tonemapping of the intensities computed using the raytracer
 @param intensity Input intensity
 @return Tonemapped intensity in range (0,1)
 */
glm::vec3 tone_mapping(glm::vec3 intensity);
