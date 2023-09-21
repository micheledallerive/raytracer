/**
@file main.cpp
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include "glm/glm.hpp"
#include <optional>

#include "image.h"

#define VOID_COLOR_RGB 0,0,0

using namespace std;

/**
 Class representing a single ray.
 */
class Ray
{
 public:
	glm::vec3 origin; ///< Origin of the ray
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
	glm::vec3 normal; ///< Normal vector of the intersected object at the intersection point
	glm::vec3 intersection; ///< Point of Intersection
	float distance; ///< Distance from the origin of the ray to the intersection point
	Object* object; ///< A pointer to the intersected object
};

/**
 General class for the object
 */
class Object
{
 public:
	glm::vec3 color; ///< Color of the object
	/** A function computing an intersection, which returns the structure optional<Hit> */
	virtual optional<Hit> intersect(Ray ray) = 0;
};

/**
 Implementation of the class Object for sphere shape.
 */
class Sphere : public Object
{
 private:
	float radius; ///< Radius of the sphere
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
	/** Implementation of the intersection function*/
	optional<Hit> intersect(Ray ray) override
	{

		optional<Hit> hit;

		/* -------------------------------------------------
		 
		Place for your code: ray-sphere intersection. Remember to set all the fields of the hit structure:

		 hit.intersection =
		 hit.normal =
		 hit.distance =
		 hit.object = this;
		 
		------------------------------------------------- */

		return hit;
	}
};

vector<Object*> objects; ///< A list of all objects in the scene

/**
 Functions that computes a color along the ray
 @param ray Ray that should be traced through the scene
 @return Color at the intersection point
 */
glm::vec3 trace_ray(Ray ray)
{

	// hit structure representing the closest intersection
	optional<Hit> closest_hit;

	//Loop over all objects to find the closest intersection
	for (auto& object : objects)
	{
		optional<Hit> hit = object->intersect(ray);
		if (!closest_hit || (hit && hit->distance < closest_hit->distance))
			closest_hit = hit;
	}

	if (closest_hit)
		return closest_hit->object->color;
	return { VOID_COLOR_RGB };
}
/**
 Function defining the scene
 */
void sceneDefinition()
{
	objects.push_back(new Sphere(1.0, glm::vec3(-0, -2, 8), glm::vec3(0.6, 0.9, 0.6)));
}

int main(int argc, const char* argv[])
{

	clock_t t = clock(); // variable for keeping the time of the rendering

	int width = 1024; //width of the image
	int height = 768; // height of the image
	float fov = 90; // field of view

	sceneDefinition(); // Let's define the scene

	Image image(width, height); // Create an image where we will store the result

	/* -------------------------------------------------
	 
	Place for your code: Loop over pixels to form and traverse the rays through the scene
	 
	------------------------------------------------- */

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{

			/*
			 
			Place for your code: ray definition for pixel (i,j), ray traversal
			 
			*/

			//Definition of the ray
			//glm::vec3 origin(0, 0, 0);
			//glm::vec3 direction(?, ?, ?);               // fill in the correct values
			//direction = glm::normalize(direction);

			//Ray ray(origin, direction);  // ray traversal

			//image.setPixel(i, j, trace_ray(ray));
		}

	t = clock() - t;
	cout << "It took " << ((float)t) / CLOCKS_PER_SEC << " seconds to render the image." << endl;
	cout << "I could render at " << (float)CLOCKS_PER_SEC / ((float)t) << " frames per second." << endl;

	// Writing the final results of the rendering
	if (argc == 2)
	{
		image.writeImage(argv[2]);
	}
	else
	{
		image.writeImage("./result.ppm");
	}

	return 0;
}
