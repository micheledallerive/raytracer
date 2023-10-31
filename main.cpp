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

#include "include/mesh-loader.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <optional>
#include <vector>
#include <memory>
#include "include/glm/glm.hpp"
#include "include/objects/cone.h"
#include "include/glm/ext/matrix_transform.hpp"

#define VOID_COLOR_RGB 0, 0, 0
#define SCENE_Z 1.0f

using namespace std;

const glm::vec3 ambient_light(0.001f);

vector<unique_ptr<Light>> lights; ///< A list of lights in the scene
vector<shared_ptr<Object>> objects; ///< A list of all objects in the scene

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

		const float distance = max(glm::distance(light->position, point), 0.1f);
		const float attenuation = 1.0f / (distance * distance);

		color += attenuation * (diffuse + specular);
	}
	return color;
}

/**
 Functions that computes a color along the ray
 @param ray Ray that should be traced through the scene
 @return Color at the intersection point
 */
glm::vec3 trace_ray(const Ray& ray)
{

	// hit structure representing the closest intersection
	optional<Hit> closest_hit;

	// Loop over all objects to find the closest intersection
	for (auto& object : objects)
	{
		optional<Hit> hit = object->intersect(ray);
		if (hit && (!closest_hit || hit->distance < closest_hit->distance))
			closest_hit = hit;
	}

	if (closest_hit)
	{
		const auto material = closest_hit->object->getSurfaceSafe<Material>();
		if (material)
		{
			return phong_model(closest_hit->intersection, closest_hit->normal, closest_hit->uv, glm::normalize(-ray
				.direction), *material);
		}
		else
		{
			return closest_hit->object->getSurface<glm::vec3>();
		}
	}
	return { VOID_COLOR_RGB };
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
void sceneDefinition()
{
	// objects.emplace_back(new Sphere(7, glm::vec3(-6, 4, 23), Material{ glm::vec3(0.07, 0.07, 0.07),
//																	   glm::vec3(0.7, 0.7, 0.7), glm::vec3(0), 0,
//																	   rainbowTexture }));

	const Material red_material = Material{ glm::vec3(0.01, 0.03, 0.03), glm::vec3(1, 0.3, 0.3), glm::vec3(0.5), 10, };
	const Material blue_material = Material{ glm::vec3(0.07, 0.07, 0.1), glm::vec3(0.25, 0.25, 1), glm::vec3(0.6),
											 100 };
	const Material green_material = Material{ glm::vec3(0.07, 0.09, 0.07), glm::vec3(0.4, 0.9, 0.4), glm::vec3(0), 0 };

//	objects.emplace_back(new Sphere(1.0, glm::vec3(1, -2, 8), blue_material));
//	objects.emplace_back(new Sphere(0.5, glm::vec3(-1, -2.5, 6), red_material));
//	objects.emplace_back(new Sphere(1.0, glm::vec3(3, -2, 6), green_material));

	const Material white_material = Material{ glm::vec3(0.07, 0.07, 0.07), glm::vec3(0.7, 0.7, 0.7), glm::vec3(0.5),
											  10 };

//	objects
//		.emplace_back(MeshLoader::load(MeshType::OBJ, "./meshes/bunny.obj", { 0, -3, 7 }, white_material));
//	objects.emplace_back(MeshLoader::load(MeshType::OBJ, "./meshes/armadillo.obj", { -4, -3,
//																								  10 }, white_material));
//	objects
//		.emplace_back(MeshLoader::load(MeshType::OBJ, "./meshes/lucy.obj", { 4, -3, 10 }, white_material));

//	objects.emplace_back(new Triangle({ 3.6, 2.945824, 8.535236 }, { 3.9, 2.957204, 8.467452 }, { 4.7,
//																								  2.345807,
//																								  8.464094 }, blue_material));
	// create a box from -15 to 15 in x coordinate, from -3 to 27 in y and from -0.01 to 30 in z

	const Material yellow_material = Material{ glm::vec3(0.03, 0.03, 0.03), glm::vec3(0.35, 0.35, 0.0f),
											   glm::vec3(1.0f),
											   100 };
	auto cone1 = make_shared<Cone>(yellow_material);
	cone1->transform(glm::translate(glm::mat4(1.0f), glm::vec3(5, 9, 14)));
	cone1->transform(glm::scale(glm::mat4(1.0f), glm::vec3(3, -12, 3)));
	objects.push_back(cone1);

	auto cone2 = make_shared<Cone>(green_material);
	cone2->transform(glm::translate(glm::mat4(1.0f), glm::vec3(6, -3, 7)));

	const float rotation = std::acos(1.0f / std::sqrt(10.0f));
	cone2->transform(glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1)));
	cone2->transform(glm::scale(glm::mat4(1.0f), glm::vec3(1, 3, 1)));
	objects.push_back(cone2);

	objects.emplace_back(new Plane(glm::vec3(-15, 0, 0), glm::vec3(1, 0, 0), red_material));
	objects.emplace_back(new Plane(glm::vec3(15, 0, 0), glm::vec3(-1, 0, 0), blue_material));
	objects.emplace_back(new Plane(glm::vec3(0, -3, 0), glm::vec3(0, 1, 0), white_material));
	objects.emplace_back(new Plane(glm::vec3(0, 27, 0), glm::vec3(0, -1, 0), blue_material));
	objects.emplace_back(new Plane(glm::vec3(0, 0, -0.01), glm::vec3(0, 0, 1), green_material));
	objects.emplace_back(new Plane(glm::vec3(0, 0, 30), glm::vec3(0, 0, -1), green_material));

	lights.emplace_back(new Light(glm::vec3(0, 26, 5), glm::vec3(1.0f)));
	lights.emplace_back(new Light(glm::vec3(0, 1, 12), glm::vec3(0.1f)));
	lights.emplace_back(new Light(glm::vec3(0, 5, 1), glm::vec3(0.4f)));
}

int main(int argc, const char* argv[])
{

	clock_t t = clock(); // variable for keeping the time of the rendering

	int width = 1024; // width of the image
	int height = 768; // height of the image
	float fov = 90;   // field of view

	// Compute the size of each pixel given the FOV
	const float pixelSize = 2.0f * tan(glm::radians(fov / 2.0f)) / width;

	sceneDefinition(); // Let's define the scene

	Image image(width, height); // Create an image where we will store the result

	const float sceneLeft = (float)-width * pixelSize / 2.0f;
	const float sceneTop = (float)height * pixelSize / 2.0f;

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			glm::vec3 origin(0, 0, 0);

			const float x = sceneLeft + (float)i * pixelSize + pixelSize / 2.0f;
			const float y = sceneTop - (float)j * pixelSize - pixelSize / 2.0f;
			const float z = SCENE_Z;
			glm::vec3 direction(x, y, z);
			direction = glm::normalize(direction);

			Ray ray(origin, direction); // ray traversal

			image.setPixel(i, j, tone_mapping(trace_ray(ray)));
		}

	t = clock() - t;
	cout << "It took " << ((float)t) / CLOCKS_PER_SEC << " seconds to render the image." << endl;
	cout << "I could render at " << (float)CLOCKS_PER_SEC / ((float)t) << " frames per second." << endl;

	// Writing the final results of the rendering
	if (argc == 2)
	{
		image.writeImage(argv[1]);
	}
	else
	{
		image.writeImage("result.ppm");
	}

	return 0;
}
