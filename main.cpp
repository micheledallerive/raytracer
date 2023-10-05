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
#include <variant>
#include <memory>

#include "image.h"
#include "material.h"
#include "textures.h"

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
	glm::vec2 uv;            ///< Coordinates for computing the texture (texture coordinates)
};

/**
 General class for the object
 */
class Object
{
 private:
	const variant<glm::vec3, Material> surface; ///< Surface of the object: either a color (i.e. vec3) or a material
 public:

	Object() : surface(glm::vec3(1.0))
	{
	}

	explicit Object(const glm::vec3& color) : surface(color)
	{
	}

	explicit Object(const Material& material) : surface(material)
	{
	}

	/** A function computing an intersection, which returns the structure Hit */
	virtual optional<Hit> intersect(const Ray& ray) = 0;

	template<typename T> optional<T> getSurfaceSafe() const
	{
		if (holds_alternative<T>(surface))
		{
			return get<T>(surface);
		}
		return nullopt;
	}

	/**
	 * @brief Get the Surface object without checking the type
	 * @tparam T the type of the surface to get
	 * @return the surface value for the type
	 * @throws bad_variant_access if the type is not the one expected
	 */
	template<typename T> T getSurface() const
	{
		return get<T>(surface);
	}

};

class Plane : public Object
{

 private:
	glm::vec3 normal;
	glm::vec3 point;

 public:
	Plane(const glm::vec3& point, const glm::vec3& normal) : point(point), normal(normal), Object()
	{
	}
	Plane(const glm::vec3& point, const glm::vec3& normal, const Material& material)
		: point(point), normal(normal), Object(material)
	{
	}
	optional<Hit> intersect(const Ray& ray) override
	{
		const float normal_dot = glm::dot(normal, ray.direction);
		if (normal_dot == 0)
			return nullopt;

		const float t = glm::dot(normal, point - ray.origin) / normal_dot;
		if (t < 0)
			return nullopt;

		const glm::vec3 intersection = ray.origin + t * ray.direction;
		return Hit{ normal, intersection, t, this };
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
	Sphere(const float radius, const glm::vec3& center, const glm::vec3& color)
		: radius(radius), center(center), Object(color)
	{
	}

	Sphere(const float radius, const glm::vec3& center, const Material& material)
		: radius(radius), center(center), Object(material)
	{
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

		const auto u = (float)(0.5 + atan2(normal.z, normal.x) / (2 * M_PI));
		const auto v = (float)(0.5 + asin(normal.y) / M_PI);

		return Hit{ normal, intersection, t, this, { u, v }};
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
	explicit Light(glm::vec3 position) : position(position), color(glm::vec3(1.0))
	{
	}
	Light(glm::vec3 position, glm::vec3 color) : position(position), color(color)
	{
	}
};

glm::vec3 ambient_light(1.0, 1.0, 1.0);

vector<shared_ptr<Light>> lights; ///< A list of lights in the scene
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

		color += diffuse + specular;
	}
	return glm::clamp(color, 0.0f, 1.0f);
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
			return phong_model(closest_hit->intersection, closest_hit->normal, closest_hit->uv,
				glm::normalize(-ray.direction), *material);
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

	glm::vec3 tone_mapped = intensity; //tonemapped intensity
	return glm::clamp(tone_mapped, glm::vec3(0.0), glm::vec3(1.0));
}

/**
 Function defining the scene
 */
void sceneDefinition()
{
	objects.emplace_back(
		new Sphere(6, glm::vec3(-5.5, 3, 20),
			Material{ glm::vec3(0.07, 0.09, 0.07),
					  glm::vec3(0.7, 0.9, 0.7),
					  glm::vec3(0), 0, rainbowTexture }));

	objects.emplace_back(
		new Sphere(1.0, glm::vec3(1, -2, 8),
			Material{ glm::vec3(0.07, 0.07, 0.1),
					  glm::vec3(0.7, 0.7, 1),
					  glm::vec3(0.6), 100 }));
	objects.emplace_back(
		new Sphere(0.5, glm::vec3(-1, -2.5, 6),
			Material{ glm::vec3(0.01, 0.03, 0.03),
					  glm::vec3(1, 0.3, 0.3),
					  glm::vec3(0.5), 10, }));
	objects.emplace_back(
		new Sphere(1.0, glm::vec3(3, -2, 6),
			Material{ glm::vec3(0.07, 0.09, 0.07),
					  glm::vec3(0.7, 0.9, 0.7),
					  glm::vec3(0), 0 }));

	const Material wall_material = Material{
		glm::vec3(0.07, 0.07, 0.1),
		glm::vec3(0.2, 0.7, 1),
		glm::vec3(0.2),
		10,
	};

	// create a box from -15 to 15 in x coordinate, from -3 to 27 in y and from -0.01 to 30 in z
	objects.emplace_back(new Plane(glm::vec3(-15, 0, 0), glm::vec3(1, 0, 0), wall_material));
	objects.emplace_back(new Plane(glm::vec3(15, 0, 0), glm::vec3(-1, 0, 0), wall_material));
	objects.emplace_back(new Plane(glm::vec3(0, -3, 0), glm::vec3(0, 1, 0), wall_material));
	objects.emplace_back(new Plane(glm::vec3(0, 27, 0), glm::vec3(0, -1, 0), wall_material));
	objects.emplace_back(new Plane(glm::vec3(0, 0, -0.01), glm::vec3(0, 0, 1), wall_material));
	objects.emplace_back(new Plane(glm::vec3(0, 0, 30), glm::vec3(0, 0, -1), wall_material));

	lights.emplace_back(new Light(glm::vec3(0, 26, 5), glm::vec3(0.4, 0.4, 0.4)));
	lights.emplace_back(new Light(glm::vec3(0, 1, 12), glm::vec3(0.4, 0.4, 0.4)));
	lights.emplace_back(new Light(glm::vec3(0, 5, 1), glm::vec3(0.4, 0.4, 0.4)));
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
