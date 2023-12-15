//
// Created by michele on 13.12.23.
//

#include "lightning.h"
#include "ray.h"
#include "scene.h"

float shadow(const Scene &scene, const glm::vec3 &point, const glm::vec3 &direction, const float light_distance, int depth=0)
{
    const std::optional<Hit> closest_hit = scene.intersect(Ray(point, direction));
    if (!closest_hit)
        return 1;
    if (depth >= MAX_RAY_DEPTH)
        return 1;

    const std::optional<Material> material = closest_hit->object->getSurfaceSafe<Material>();
    if (!material)
        return closest_hit->distance < light_distance ? 0 : 1;

    const bool transparent = material->transparency > 0;
    if (!transparent)
        return closest_hit->distance < light_distance ? 0 : 1;

    const float n1 = glm::dot(direction, closest_hit->normal) > 0 ? material->refractive_index : 1.0f;
    const float n2 = glm::dot(direction, closest_hit->normal) > 0 ? 1.0f : material->refractive_index;
    const float n = n1 / n2;
    const glm::vec3 refracted_direction = glm::refract(direction, closest_hit->normal, n);

    const float refracted_color = shadow(scene, closest_hit->intersection, refracted_direction, light_distance, depth + 1);
    return material->transparency * refracted_color;
}

glm::vec3 phong_model(const Scene &scene, const glm::vec3 &point, const glm::vec3 &normal, const glm::vec2 &uv, const glm::vec3 &view_direction, const Material &material)
{
    glm::vec3 color = material.ambient * scene.getAmbientLight();// diffusion
    for (const auto &light : scene.getLights()) {
        const glm::vec3 light_direction = glm::normalize(light.position - point);
        glm::vec3 diffuse(0);
        const float light_angle = glm::dot(normal, light_direction);
        if (light_angle > 0) {
            diffuse = material.diffuse * light_angle * light.color;
            if (material.texture)
                diffuse *= material.texture(uv);
        }

        glm::vec3 reflection_direction = glm::normalize(glm::reflect(-light_direction, normal));
        glm::vec3 specular(0);
        const float reflection_angle = glm::dot(reflection_direction, view_direction);
        if (reflection_angle > 0) {
            specular = material.specular * glm::pow(reflection_angle, material.shininess) * light.color;
        }

        const float light_distance = glm::max(glm::distance(light.position, point), 0.1f);
        const float attenuation = 1.0f / (light_distance * light_distance);

        const float shadow_factor = shadow(scene, point, light_direction, light_distance);

        color += attenuation * (diffuse + specular) * shadow_factor;
    }
    return color;
}

/**
 * Function for computing the fresnel factor
 */
float fresnel(const glm::vec3 &normal, const glm::vec3 &view_direction, const float n1, const float n2)
{
    const float n = n1 / n2;
    const float cos_theta = glm::dot(normal, view_direction);
    const float sin_theta = sqrt(1 - cos_theta * cos_theta);
    const float sin_theta_t = n * sin_theta;
    if (sin_theta_t >= 1)
        // total internal reflection
        return 1;

    const float cos_theta_t = sqrt(1 - sin_theta_t * sin_theta_t);
    const float r_parallel = (n1 * cos_theta - n2 * cos_theta_t) / (n1 * cos_theta + n2 * cos_theta_t);
    const float r_perpendicular = (n2 * cos_theta - n1 * cos_theta_t) / (n2 * cos_theta + n1 * cos_theta_t);
    return (r_parallel * r_parallel + r_perpendicular * r_perpendicular) / 2;
}

bool is_total_internal_reflection(const glm::vec3 &normal, const glm::vec3 &view_direction, const float n1, const float n2)
{
    const float n = n1 / n2;
    const float cos_theta = glm::dot(normal, view_direction);
    const float sin_theta_2 = 1 - cos_theta * cos_theta;
    const float sin_theta_t_2 = n * n * sin_theta_2;
    return sin_theta_t_2 > 1;
}

/**
 Functions that computes a color along the ray
 @param ray Ray that should be traced through the scene
 @return Color at the intersection point
 */
glm::vec3 trace_ray(const Scene &scene, const Ray &ray, int depth, float refl_cumulative, float refr_cumulative)
{
    const std::optional<Hit> closest_hit = scene.intersect(ray);
    if (!closest_hit)
        return {0, 0, 0};

    const std::optional<Material> material = closest_hit->object->getSurfaceSafe<Material>();
    if (!material)
        return closest_hit->object->getSurface<glm::vec3>();

    const bool inside_object = glm::dot(ray.direction, closest_hit->normal) > 0;
    const glm::vec3 normal = inside_object ? -closest_hit->normal : closest_hit->normal;

    const glm::vec3 phong = phong_model(scene, closest_hit->intersection, normal, closest_hit->uv, glm::normalize(-ray.direction), *material);

    if (depth >= MAX_RAY_DEPTH)
        return phong;

    const float n1 = inside_object ? material->refractive_index : 1.0f;
    const float n2 = inside_object ? 1.0f : material->refractive_index;

    // If the reflection/refraction coefficients get too small during the recursion, their contribution is negligible.
    // This check avoids recursion if the material is not transparent/reflective, but also reduces the recursion in the
    // case of a transparent/reflective material with a low coefficient.
    const float COEFFICIENT_THRESH = 1e-4f;

    float reflection_factor = material->reflection;
    float refraction_factor = material->transparency;
    if (is_total_internal_reflection(normal, -ray.direction, n1, n2)) {
        reflection_factor = 1;
        refraction_factor = 0;
    }
    else if (refraction_factor > COEFFICIENT_THRESH) {
        float fresnel_factor = fresnel(normal, -ray.direction, n1, n2);

        if (reflection_factor > COEFFICIENT_THRESH)
            reflection_factor *= fresnel_factor;
        else
            reflection_factor = fresnel_factor;
        refraction_factor *= 1.0f - fresnel_factor;
    }

    // reflection
    const Ray reflection_ray = Ray(closest_hit->intersection, glm::reflect(ray.direction, normal));
    glm::vec3 reflected_color(0);
    if (reflection_factor * refl_cumulative > COEFFICIENT_THRESH)
        reflected_color = trace_ray(scene, reflection_ray, depth + 1, reflection_factor * refl_cumulative, refr_cumulative);

    // refraction
    const float n = n1 / n2;
    const glm::vec3 refracted_direction = glm::refract(ray.direction, normal, n);
    const Ray refraction_ray = Ray(closest_hit->intersection, refracted_direction);

    glm::vec3 refracted_color(0);
    if (refraction_factor * refr_cumulative > COEFFICIENT_THRESH)
        refracted_color = trace_ray(scene, refraction_ray, depth + 1, refl_cumulative, refraction_factor * refr_cumulative);

    return phong + reflection_factor * reflected_color + refraction_factor * refracted_color;
}

/**
 Function performing tonemapping of the intensities computed using the raytracer
 @param intensity Input intensity
 @return Tonemapped intensity in range (0,1)
 */
glm::vec3 tone_mapping(const glm::vec3 intensity)
{
    const float a = 12.0f;
    const float gamma = 2.0f;

    const glm::vec3 mapped_intensity = a * glm::pow(intensity, glm::vec3(1.0f / gamma));
    return glm::clamp(mapped_intensity, 0.0f, 1.0f);
}
