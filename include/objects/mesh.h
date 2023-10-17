//
// Created by michele on 17.10.23.
//

#ifndef _MESH_H_
#define _MESH_H_

#include <string>
#include <memory>
#include <vector>
#include <array>
#include "object.h"
#include "triangle.h"
#include "sphere.h"

#define USE_BOUNDING_SPHERE 1

class Mesh : public Object
{
 private:
	glm::vec3 _position;
	std::string _name;
	std::vector<std::unique_ptr<Triangle>> _triangles;
#if USE_BOUNDING_SPHERE
	glm::vec3 _centroid{ 0.0f };
	std::unique_ptr<Sphere> _bounding_sphere;
#endif

	template<const int N>
	std::array<std::string, N> parse_tokens(const std::string& s, const std::string& delim = " ")
	{
		std::array<std::string, N> tokens{};
		int last_idx = 0;
		for (int i = 0; i < N - 1; ++i)
		{
			const auto idx = s.find(delim, last_idx);
			tokens[i] = s.substr(last_idx, idx - last_idx);
			last_idx = idx + delim.size();
		}
		tokens[N - 1] = s.substr(last_idx);
		return tokens;
	}
 public:
	virtual ~Mesh() = default;
	Mesh(const std::string& file_name, const glm::vec3& position, const Material& material) : _position(position)
	{
		std::ifstream in(file_name, std::ios::in);
		/*
		 * OBJ files are composed of:
		 * - Comments (starting with #)
		 * - Name of the object (starting with o)
		 * - Vertices (in the form of "v <x> <y> <z>")
		 * - Faces (in the form of "f <idx 1> <idx 2> <idx 3>")
		 */
		std::vector<glm::vec3> vertices; // could be optimized by reserving based on the file size!
		std::vector<glm::vec3> vertex_normals;

		std::string line;

		bool smooth = false;
		while (getline(in, line))
		{
			switch (line[0])
			{
			case 'o':
			{
				this->_name = move(parse_tokens<2>(line))[1];
				break;
			}
			case 'v':
			{
				std::array<std::string, 4> s_vertex = move(parse_tokens<4>(line));

				std::vector<glm::vec3>& target = line[1] == 'n' ? vertex_normals : vertices;
				target.emplace_back(stof(s_vertex[1]), stof(s_vertex[2]), stof(s_vertex[3]));
				break;
			}
			case 'f':
			{
				std::array<std::string, 4> s_face = move(parse_tokens<4>(line));
				std::array<glm::vec3, 3> face_vxs{};
				std::array<glm::vec3, 3> face_normals{};
				for (int i = 0; i < 3; ++i)
				{
					// Assuming there is no texture indes after the vector index
					if (!smooth)
					{
						int idx = stoi(s_face[i + 1]) - 1;
						glm::vec3 vertex = vertices[idx];
						face_vxs[i] = vertex + position;
					}
					else
					{
						// face is in the form of "f <idx 1>//<normal idx 1> ..."
						const auto tokens = parse_tokens<2>(s_face[i + 1], "//");
						int vidx = stoi(tokens[0]) - 1;
						int nidx = stoi(tokens[1]) - 1;
						face_vxs[i] = vertices[vidx] + position;
						face_normals[i] = vertex_normals[nidx];
					}
				}
				if (smooth)
				{
					this->_triangles.push_back(std::make_unique<Triangle>(face_vxs, face_normals, material));
				}
				else
				{
					this->_triangles.push_back(std::make_unique<Triangle>(face_vxs, material));
				}
				break;
			}
			case 's':
			{
				smooth = (line == "s 1" || line == "s on");
			}
			default:
				break;
			}
		}

#if USE_BOUNDING_SPHERE
		// compute centroid
		float triangles_area = 0.0f;
		for (auto& triangle : this->_triangles)
		{
			this->_centroid += triangle->centroid * triangle->area;
			triangles_area += triangle->area;
		}
		this->_centroid /= triangles_area;

		// compute bounding sphere
		float max_distance = 0.0f;
		for (auto& triangle : this->_triangles)
		{
			for (auto& point : triangle->points)
			{
				const float distance = glm::distance(point, this->_centroid);
				if (distance > max_distance)
					max_distance = distance;
			}
		}
		this->_bounding_sphere = std::make_unique<Sphere>(max_distance, this->_centroid, Material{ glm::vec3(0.0f),
																								   glm::vec3(0.0f),
																								   glm::vec3(0.0f),
																								   0 });
#endif
	}

	std::optional<Hit> intersect(const Ray& ray) override
	{
#if USE_BOUNDING_SPHERE
		if (!this->_bounding_sphere->intersect(ray))
			return std::nullopt;
#endif
		std::optional<Hit> closest_hit = std::nullopt;
		for (auto& triangle : this->_triangles)
		{
			std::optional<Hit> current_hit = triangle->intersect(ray);
			if (current_hit && (!closest_hit || current_hit->distance < closest_hit->distance))
				closest_hit = current_hit;
		}
		return closest_hit;
	}
};
#endif //_MESH_H_
