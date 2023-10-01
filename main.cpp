/**
@file main.cpp
*/

#include "glm/glm.hpp"
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#include "Image.h"
#include "Material.h"

#define VOID_COLOR_RGB 0, 0, 0
#define SCENE_Z 1.0f

using namespace std;

/**
 Class representing a single ray.
 */
class Ray
{
 public:
	glm::vec3 origin;    ///< Origin of the ray
	glm::vec3 direction; ///< Direction of the ray
	/**
	 Contructor of the ray
	 @param origin Origin of the ray
	 @param direction Direction of the ray
	 */
	Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction)
	{
	}
};

class Object;

/**
 Structure representing the even of hitting an object
 */
struct Hit
{
	glm::vec3 normal;       ///< Normal vector of the intersected object at the intersection point
	glm::vec3 intersection; ///< Point of Intersection
	float distance;         ///< Distance from the origin of the ray to the intersection point
	Object* object;         ///< A pointer to the intersected object
};

/**
 General class for the object
 */
class Object
{
 public:
	glm::vec3 color; ///< Color of the object
	Material material; ///< Structure describing the material of the object
	/** A function computing an intersection, which returns the structure Hit */
	virtual optional<Hit> intersect(const Ray& ray) = 0;

	/** Function that returns the material struct of the object*/
	Material getMaterial()
	{
		return material;
	}
	/** Function that set the material
	 @param material A structure desribing the material of the object
	*/
	void setMaterial(Material material)
	{
		this->material = material;
	}
};

/**
 Implementation of the class Object for sphere shape.
 */
class Sphere : public Object
{
 private:
	float radius;     ///< Radius of the sphere
	glm::vec3 center; ///< Center of the sphere

 public:
	/**
	 The constructor of the sphere
	 @param radius Radius of the sphere
	 @param center Center of the sphere
	 @param color Color of the sphere
	 */
	Sphere(float radius, glm::vec3 center, glm::vec3 color) : radius(radius), center(center)
	{
		this->color = color;
	}
	Sphere(float radius, glm::vec3 center, Material material) : radius(radius), center(center)
	{
		this->material = material;
	}

	/** Implementation of the intersection function */
	optional<Hit> intersect(const Ray& ray) override
	{
		const glm::vec3 centerToOrigin = center - ray.origin;
		const float c2 = glm::dot(centerToOrigin, centerToOrigin);

		const float a = glm::dot(centerToOrigin, ray.direction);
		const float D2 = c2 - a * a;

		if (D2 > radius * radius)
			return nullopt;

		const float b = sqrt(radius * radius - D2);

		const float t1 = min(a - b, a + b);
		const float t2 = max(a - b, a + b);

		if (t1 < 0 && t2 < 0)
			return nullopt;

		const float t = t1 < 0 ? t2 : t1;

		const glm::vec3 intersection = ray.origin + t * ray.direction;
		const glm::vec3 normal = glm::normalize(intersection - center);
		return Hit{ normal, intersection, t, this };
	}
};

/**
 Light class
 */
class Light
{
 public:
	glm::vec3 position; ///< Position of the light source
	glm::vec3 color; ///< Color/intentisty of the light source
	Light(glm::vec3 position) : position(position)
	{
		color = glm::vec3(1.0);
	}
	Light(glm::vec3 position, glm::vec3 color) : position(position), color(color)
	{
	}
};

vector<Light*> lights; ///< A list of lights in the scene
glm::vec3 ambient_light(1.0, 1.0, 1.0);

vector<Object*> objects; ///< A list of all objects in the scene

/** Function for computing color of an object according to the Phong Model
 @param point A point belonging to the object for which the color is computed
 @param normal A normal vector the the point
 @param view_direction A normalized direction from the point to the viewer/camera
 @param material A material structure representing the material of the object
*/
glm::vec3 PhongModel(glm::vec3 point, glm::vec3 normal, glm::vec3 view_direction, Material material)
{

	glm::vec3 color(0.0);

	/*

	 Assignment 2

	 Phong model.
	 Your code should implement a loop over all the lightsourses in the array lights and agredate the contribution of each of them to the final color of the object.
	 Outside of the loop add also the ambient component from ambient_light.

	*/

	// The final color has to be clamped so the values do not go beyond 0 and 1.
	color = glm::clamp(color, glm::vec3(0.0), glm::vec3(1.0));
	return color;
}

/**
 Functions that computes a color along the ray
 @param ray Ray that should be traced through the scene
 @return Color at the intersection point
 */
glm::vec3 trace_ray(Ray ray)
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
		return PhongModel(closest_hit->intersection,
			closest_hit->normal,
			glm::normalize(-ray.direction),
			closest_hit->object->getMaterial());

	return { VOID_COLOR_RGB };
}

/**
 Function defining the scene
 */
void sceneDefinition()
{
	objects.push_back(new Sphere(1.0, glm::vec3(-0, -2, 8), glm::vec3(0.6, 0.9, 0.6)));
	objects.push_back(new Sphere(1.0, glm::vec3(1, -2, 8), glm::vec3(0.6, 0.6, 0.9)));
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

			image.setPixel(i, j, trace_ray(ray));
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
