/**
@file main.cpp
*/

#include "glm/glm.hpp"
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
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

class Mesh;

class Triangle : public Object
{
 private:
	array<glm::vec3, 3> _points;
	glm::vec3 _normal{ glm::normalize(glm::cross(_points[1] - _points[0], _points[2] - _points[0])) };

	inline float area(const glm::vec3& point, const glm::vec3& next, const glm::vec3& prev)
	{
		const glm::vec3 n = glm::cross(next - point, prev - point);
		const float sign = glm::sign(glm::dot(n, this->_normal));
		return glm::length(n) * (sign >= 0.0f ? 1.0f : -1.0f) * 0.5f;
	}
 public:
	friend class Mesh;
	explicit Triangle(array<glm::vec3, 3> points, const Material& material) : _points(points), Object(material)
	{
	}
	Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, const Material& material) : Triangle({ p1, p2, p3 }, material)
	{
	}

	inline bool is_inside(const glm::vec3& point)
	{
		return area(point, this->_points[1], this->_points[2]) >= 0
			   && area(point, this->_points[2], this->_points[0]) >= 0
			   && area(point, this->_points[0], this->_points[1]) >= 0;
	}

	optional<Hit> intersect(const Ray& ray) override
	{
		const optional<Hit> plane_hit = Plane(this->_points[0], this->_normal).intersect(ray);
		if (!plane_hit)
			return plane_hit;

		if (!is_inside(plane_hit->intersection))
			return nullopt;

		return Hit{
			.normal = this->_normal,
			.intersection = plane_hit->intersection,
			.distance = plane_hit->distance,
			.object = this
		};
	}
};

class Mesh : public Object
{
 private:
	glm::vec3 _position;
	string _name;
	vector<unique_ptr<Triangle>> _triangles;

	template<const int N>
	array<string, N> parse_tokens(const string& s)
	{
		istringstream iss(s);
		array<string, N> parts;
		for (int i = 0; i < N; ++i)
		{
			iss >> parts[i];
		}
		return parts;
	}
 public:
	Mesh(const string& file_name, const glm::vec3& position, const Material& material) : _position(position)
	{
		ifstream in(file_name, ios::in);
		/*
		 * OBJ files are composed of:
		 * - Comments (starting with #)
		 * - Name of the object (starting with o)
		 * - Vertices (in the form of "v <x> <y> <z>")
		 * - Faces (in the form of "f <idx 1> <idx 2> <idx 3>")
		 */
		vector<glm::vec3> vertices; // could be optimized by reserving based on the file size!
		string line;
		while (getline(in, line))
		{
			switch (line[0])
			{
			case 'o':
			{
				this->_name = std::move(parse_tokens<2>(line))[1];
				break;
			}
			case 'v':
			{
				array<string, 4> s_vertex = std::move(parse_tokens<4>(line));
				vertices.emplace_back(stof(s_vertex[1]), stof(s_vertex[2]), stof(s_vertex[3]));
				break;
			}
			case 'f':
			{
				array<string, 4> s_face = std::move(parse_tokens<4>(line));
				array<glm::vec3, 3> face_vxs{};
				for (int i = 0; i < 3; ++i)
				{
					int idx = stoi(s_face[i + 1]) - 1;
					glm::vec3 vertex = vertices[idx];
					face_vxs[i] = vertex + position;
				}
				this->_triangles.push_back(make_unique<Triangle>(face_vxs, material));
				break;
			}
			default:
				break;
			}
		}
	}

	optional<Hit> intersect(const Ray& ray) override
	{
		optional<Hit> closest_hit = nullopt;
		for (auto& triangle : this->_triangles)
		{
			optional<Hit> current_hit = triangle->intersect(ray);
			if (current_hit && (!closest_hit || current_hit->distance < closest_hit->distance))
				closest_hit = current_hit;
		}
		return closest_hit;
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

glm::vec3 ambient_light(0.3f);

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

		const float distance = glm::distance(light->position, point) + 1.0f;
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
	const float a = 1.0f;
	const float beta = 1.3f;
	const float gamma = 2.2f;

	const glm::vec3 Ib = glm::pow(intensity, glm::vec3(beta));
	const glm::vec3 mapped_intensity = glm::pow(a * Ib, glm::vec3(1.0f / gamma));
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

	const string path = "./meshes/bunny.obj";
	objects.emplace_back(new Mesh(path, { -5, -2, 8 }, white_material));

//	objects.emplace_back(new Triangle({ 3.6, 2.945824, 8.535236 }, { 3.9, 2.957204, 8.467452 }, { 4.7,
//																								  2.345807,
//																								  8.464094 }, blue_material));
	// create a box from -15 to 15 in x coordinate, from -3 to 27 in y and from -0.01 to 30 in z
	objects.emplace_back(new Plane(glm::vec3(-15, 0, 0), glm::vec3(1, 0, 0), red_material));
	objects.emplace_back(new Plane(glm::vec3(15, 0, 0), glm::vec3(-1, 0, 0), blue_material));
	objects.emplace_back(new Plane(glm::vec3(0, -3, 0), glm::vec3(0, 1, 0), white_material));
	objects.emplace_back(new Plane(glm::vec3(0, 27, 0), glm::vec3(0, -1, 0), blue_material));
	objects.emplace_back(new Plane(glm::vec3(0, 0, -0.01), glm::vec3(0, 0, 1), green_material));
	objects.emplace_back(new Plane(glm::vec3(0, 0, 30), glm::vec3(0, 0, -1), green_material));

	lights.emplace_back(new Light(glm::vec3(0, 26, 5), glm::vec3(175)));
	// lights.emplace_back(new Light(glm::vec3(0, 1, 12), glm::vec3(30)));
	lights.emplace_back(new Light(glm::vec3(0, 5, 1), glm::vec3(90)));
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
