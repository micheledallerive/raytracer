/**
@file main.cpp
*/

#define USE_BOUNDING_SPHERE 1

#include "include/image.h"
#include "include/material.h"
#include "include/ray.h"
#include "include/objects/object.h"
#include "include/objects/plane.h"
#include "include/objects/mesh.h"
#include "include/light.h"
#include "include/textures.h"
#include "include/mesh-loader.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <optional>
#include <vector>
#include <memory>
#include <map>
#include "include/glm/glm.hpp"
#include "include/objects/cone.h"
#include "include/glm/ext/matrix_transform.hpp"
#include "include/animation.h"

#define VOID_COLOR_RGB 1, 0, 0
#define SCENE_Z 1.0f
#define MAX_RAY_DEPTH 10

using namespace std;

const glm::vec3 ambient_light(0.001f);

vector<unique_ptr<Light>> lights; ///< A list of lights in the scene
vector<shared_ptr<Object>> objects; ///< A list of all objects in the scene

float shadow(const glm::vec3& point, const glm::vec3& direction, const float light_distance)
{
	const optional<Hit> closest_hit = Ray(point, direction).closest_hit(objects.begin(), objects.end());
	if (!closest_hit)
		return 1;

	const optional<Material> material = closest_hit->object->getSurfaceSafe<Material>();
	if (!material)
		return closest_hit->distance < light_distance ? 0 : 1;

	const bool transparent = material->transparency > 0;
	if (!transparent)
		return closest_hit->distance < light_distance ? 0 : 1;

	const float n1 = glm::dot(direction, closest_hit->normal) > 0 ? material->refractive_index : 1.0f;
	const float n2 = glm::dot(direction, closest_hit->normal) > 0 ? 1.0f : material->refractive_index;
	const float n = n1 / n2;
	const glm::vec3 refracted_direction = glm::refract(direction, closest_hit->normal, n);
	const Ray refraction_ray = Ray(closest_hit->intersection, refracted_direction);

	const float refracted_color = shadow(closest_hit->intersection, refracted_direction, light_distance);
	return material->transparency * refracted_color;
}

/** Function for computing color of an object according to the Phong Model
 @param point A point belonging to the object for which the color is computed
 @param normal A normal vector the the point
 @param view_direction A normalized direction from the point to the viewer/camera
 @param material A material structure representing the material of the object
*/
glm::vec3
phong_model(const glm::vec3& point, const glm::vec3& normal, const glm::vec2& uv, const glm::vec3& view_direction, const Material& material)
{
	glm::vec3 color = material.ambient * ambient_light; // diffusion
	for (const auto& light : lights)
	{
		const glm::vec3 light_direction = glm::normalize(light->position - point);
		glm::vec3 diffuse(0);
		const float light_angle = glm::dot(normal, light_direction);
		if (light_angle > 0)
		{
			diffuse = material.diffuse * light_angle * light->color;
			if (material.texture)
				diffuse *= material.texture(uv);
		}

		glm::vec3 reflection_direction = glm::normalize(glm::reflect(-light_direction, normal));
		glm::vec3 specular(0);
		const float reflection_angle = glm::dot(reflection_direction, view_direction);
		if (reflection_angle > 0)
		{
			specular = material.specular * pow(reflection_angle, material.shininess) * light->color;
		}

		const float light_distance = max(glm::distance(light->position, point), 0.1f);
		const float attenuation = 1.0f / (light_distance * light_distance);

		const float shadow_factor = shadow(point, light_direction, light_distance);

		color += attenuation * (diffuse + specular) * shadow_factor;
	}
	return color;
}

/**
 * Function for computing the fresnel factor
 */
float fresnel(const glm::vec3& normal, const glm::vec3& view_direction, const float n1, const float n2)
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

bool
is_total_internal_reflection(const glm::vec3& normal, const glm::vec3& view_direction, const float n1, const float n2)
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
glm::vec3 trace_ray(const Ray& ray, int depth = 0, float refl_cumulative = 1.0f, float refr_cumulative = 1.0f)
{
	const optional<Hit> closest_hit = ray.closest_hit(objects.begin(), objects.end());
	if (!closest_hit)
		return { VOID_COLOR_RGB };

	const optional<Material> material = closest_hit->object->getSurfaceSafe<Material>();
	if (!material)
		return closest_hit->object->getSurface<glm::vec3>();

	const bool inside_object = glm::dot(ray.direction, closest_hit->normal) > 0;
	const glm::vec3 normal = inside_object ? -closest_hit->normal : closest_hit->normal;

	const glm::vec3 phong = phong_model(closest_hit->intersection, normal, closest_hit
		->uv, glm::normalize(-ray.direction), *material);

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
	if (is_total_internal_reflection(normal, -ray.direction, n1, n2))
	{
		reflection_factor = 1;
		refraction_factor = 0;
	}
	else if (refraction_factor > COEFFICIENT_THRESH)
	{
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
		reflected_color = trace_ray(reflection_ray, depth + 1, reflection_factor * refl_cumulative, refr_cumulative);

	// refraction
	const float n = n1 / n2;
	const glm::vec3 refracted_direction = glm::refract(ray.direction, normal, n);
	const Ray refraction_ray = Ray(closest_hit->intersection, refracted_direction);

	glm::vec3 refracted_color(0);
	if (refraction_factor * refr_cumulative > COEFFICIENT_THRESH)
		refracted_color = trace_ray(refraction_ray, depth + 1, refl_cumulative, refraction_factor * refr_cumulative);

	return phong + reflection_factor * reflected_color + refraction_factor * refracted_color;
}

/**
 Function performing tonemapping of the intensities computed using the raytracer
 @param intensity Input intensity
 @return Tonemapped intensity in range (0,1)
 */
glm::vec3 tone_mapping(const glm::vec3& intensity)
{
	const float a = 12.0f;
	const float gamma = 2.0f;

	const glm::vec3 mapped_intensity = a * glm::pow(intensity, glm::vec3(1.0f / gamma));
	return glm::clamp(mapped_intensity, 0.0f, 1.0f);
}

/**
 Function defining the scene
 */
void sceneDefinition(int current_frame = 0, int frame_rate = 1)
{
	const float step_size = 2.5f / frame_rate;
	const float step = step_size * current_frame;

	const Material rainbow_material = Material{ glm::vec3(0.07, 0.07, 0.07), glm::vec3(0.7, 0.7, 0.7), glm::vec3(0), 0,
												rainbowTexture };
	Sphere* rainbow_sphere = new Sphere(rainbow_material);

	rainbow_sphere->transform(glm::translate(glm::mat4(1.0f), glm::vec3(-6, 4, 23)));
	rainbow_sphere->transform(glm::scale(glm::mat4(1.0f), glm::vec3(7)));
	objects.emplace_back(rainbow_sphere);

	const Material green_diffuse{ .ambient = glm::vec3(0.03f, 0.1f, 0.03f), .diffuse = glm::vec3(0.3f, 1.0f, 0.3f), };

	const Material red_specular{ .ambient = glm::vec3(0.01f, 0.02f, 0.02f), .diffuse = glm::vec3(1.0f, 0.05f, 0.05f), };

	const Material blue_specular{ .ambient = glm::vec3(0.02f, 0.02f, 0.1f), .diffuse = glm::vec3(0.2f, 0.2f, 1.0f), .specular = glm::vec3(0.6), .shininess = 100.0 };

	const Material reflective_material = Material{ glm::vec3(0.0f), glm::vec3(0, 0, 0), glm::vec3(0),
												   0, .reflection=1.0f };

	const Material refractive_material = Material{ glm::vec3(0), glm::vec3(0, 0, 0), glm::vec3(0),
												   0, .refractive_index = 2.0f, .transparency = .96f };

#if ANIMATE
	auto* blue_sphere = new Sphere(blue_specular);
#else
	auto* blue_sphere = new Sphere(reflective_material);
#endif
	blue_sphere->transform(glm::translate(glm::mat4(1.0f), glm::vec3(1, -2, 8)));
	objects.emplace_back(blue_sphere);

	auto* red_sphere = new Sphere(red_specular);
	red_sphere->transform(glm::translate(glm::mat4(1.0f), glm::vec3(-1, -2.5, 6)));
	red_sphere->transform(glm::scale(glm::mat4(1.0f), glm::vec3(0.5)));
	objects.emplace_back(red_sphere);

	auto* refractive_sphere = new Sphere(refractive_material);
	refractive_sphere
		->transform(glm::translate(glm::mat4(1.0f), glm::vec3(-3, jumping_ball_position(-1, 4, step, -1), 8)));
	refractive_sphere->transform(glm::scale(glm::mat4(1.0f), glm::vec3(2.0)));
	objects.emplace_back(refractive_sphere);

#if ANIMATE
	auto* green_sphere = new Sphere(green_diffuse);
	green_sphere->transform(glm::translate(glm::mat4(1.0f), glm::vec3(3, -2, 6)));
	objects.emplace_back(green_sphere);

	auto* reflective_sphere = new Sphere(reflective_material);
	reflective_sphere
		->transform(glm::translate(glm::mat4(1.0f), glm::vec3(5, jumping_ball_position(1, 4, step, 4), 14)));
	reflective_sphere->transform(glm::scale(glm::mat4(1.0f), 4.0f * jumping_ball_scale(1, 4, step, 4, step_size)));
	objects.emplace_back(reflective_sphere);
#else
	const Material yellow_material{ glm::vec3(0.03, 0.03, 0.03),
									glm::vec3(0.35, 0.35, 0.0f),
									glm::vec3(1.0f),
									100 };

	auto cone1 = make_shared<Cone>(yellow_material);
	cone1->transform(glm::translate(glm::mat4(1.0f), glm::vec3(5, 9, 14)));
	cone1->transform(glm::scale(glm::mat4(1.0f), glm::vec3(3, -12, 3)));
	objects.push_back(cone1);

	auto cone2 = make_shared<Cone>(green_diffuse);
	cone2->transform(glm::translate(glm::mat4(1.0f), glm::vec3(6, -3, 7)));
	const float rotation = std::acos(1.0f / std::sqrt(10.0f));
	cone2->transform(glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1)));
	cone2->transform(glm::scale(glm::mat4(1.0f), glm::vec3(1, 3, 1)));
	objects.push_back(cone2);

#endif


	// ========= PLANES =========

	const Material red_diffuse{ .ambient = glm::vec3(0.09f, 0.06f, 0.06f), .diffuse = glm::vec3(0.9f, 0.6f, 0.6f), };
	const Material blue_diffuse{ .ambient = glm::vec3(0.06f, 0.06f, 0.09f), .diffuse = glm::vec3(0.6f, 0.6f, 0.9f), };

	objects.emplace_back(new Plane(glm::vec3(0, -3, 0), glm::vec3(0, 1, 0)));
	objects.emplace_back(new Plane(glm::vec3(0, 0, 30), glm::vec3(0, 0, -1), green_diffuse));
	objects.emplace_back(new Plane(glm::vec3(-15, 0, 0), glm::vec3(1, 0, 0), red_diffuse));
	objects.emplace_back(new Plane(glm::vec3(15, 0, 0), glm::vec3(-1, 0, 0), blue_diffuse));
	objects.emplace_back(new Plane(glm::vec3(0, 27, 0), glm::vec3(0, -1, 0)));
	objects.emplace_back(new Plane(glm::vec3(0, 0, -0.01), glm::vec3(0, 0, 1), green_diffuse));

	// ========= LIGHTS =========
	lights.emplace_back(new Light(glm::vec3(0, 26, 5), glm::vec3(1.0f)));
	lights.emplace_back(new Light(glm::vec3(0, 1, 12), glm::vec3(0.1f)));
	lights.emplace_back(new Light(glm::vec3(0, 5, 1), glm::vec3(0.4f)));
}

int main(int argc, const char* argv[])
{
	int current_frame = 0;
	int tot_frames = 1;
	if (argc >= 4) // ./main <filename> <current frame> <tot frames>
	{
		current_frame = atoi(argv[2]);
		tot_frames = atoi(argv[3]);
	}
	clock_t t = clock(); // variable for keeping the time of the rendering

#if DEBUG
	int width = 256; // width of the image
	int height = 192; // height of the image
#else
	int width = 1024; // width of the image
	int height = 768; // height of the image
#endif
	float fov = 90;   // field of view

	// Compute the size of each pixel given the FOV
	const float pixelSize = 2.0f * tan(glm::radians(fov / 2.0f)) / width;

	sceneDefinition(current_frame, tot_frames); // Let's define the scene

	Image image(width, height); // Create an image where we will store the result

	const float sceneLeft = (float)-width * pixelSize / 2.0f;
	const float sceneTop = (float)height * pixelSize / 2.0f;

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
//	for (int i = 400; i < 500; ++i)
//		for (int j = 400; j < 401; ++j)
		{
			glm::vec3 origin(0, 0, 0);

			const float x = sceneLeft + (float)i * pixelSize + pixelSize / 2.0f;
			const float y = sceneTop - (float)j * pixelSize - pixelSize / 2.0f;
			const float z = SCENE_Z;
			glm::vec3 direction(x, y, z);
			direction = glm::normalize(direction);

			Ray ray(origin, direction); // ray traversal

			image.setPixel(i, j, tone_mapping(trace_ray(ray, 0)));
		}

	t = clock() - t;
	cout << "It took " << ((float)t) / CLOCKS_PER_SEC << " seconds to render the image." << endl;
	cout << "I could render at " << (float)CLOCKS_PER_SEC / ((float)t) << " frames per second." << endl;

	// Writing the final results of the rendering
	if (argc >= 2)
	{
		image.writeImage(argv[1]);
	}
	else
	{
		image.writeImage("result.ppm");
	}

	return 0;
}
