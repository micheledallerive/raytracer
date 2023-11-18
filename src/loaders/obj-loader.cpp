//
// Created by michele on 18.11.23.
//

#include "loaders/obj-loader.h"
#include <iostream>
template<const int N>
std::array<std::string, N> OBJMeshLoader::parse_tokens(const std::string &s, const std::string &delim)
{
    std::array<std::string, N> tokens{};
    int last_idx = 0;
    for (int i = 0; i < N - 1; ++i) {
        const auto idx = s.find(delim, last_idx);
        tokens[i] = s.substr(last_idx, idx - last_idx);
        last_idx = idx + delim.size();
    }
    tokens[N - 1] = s.substr(last_idx);
    return tokens;
}

Mesh *OBJMeshLoader::load(const std::string &file_name, const Material &material)
{
    std::ifstream in(file_name, std::ios::in);
    if (!in) {
        std::cerr << "Cannot open " << file_name << std::endl;
        exit(1);
    }
    /*
	 * OBJ files are composed of:
	 * - Comments (starting with #)
	 * - Name of the object (starting with o)
	 * - Vertices (in the form of "v <x> <y> <z>")
	 * - Smooth (in the form of "s <0|1|on|off>")
	 * - Faces (in the form of "f <idx 1> <idx 2> <idx 3>")
	 */
    std::vector<glm::vec3> vertices;// could be optimized by reserving based on the file size!
    std::vector<glm::vec3> vertex_normals;

    std::string name;
    std::vector<std::unique_ptr<Triangle>> triangles;

    std::string line;

    bool smooth = false;
    while (getline(in, line)) {
        switch (line[0]) {
        case 'o': {
            name = std::move(parse_tokens<2>(line))[1];
            break;
        }
        case 'v': {
            std::array<std::string, 4> s_vertex = parse_tokens<4>(line);

            std::vector<glm::vec3> &target = line[1] == 'n' ? vertex_normals : vertices;
            target.emplace_back(stof(s_vertex[1]), stof(s_vertex[2]), stof(s_vertex[3]));
            break;
        }
        case 'f': {
            std::array<std::string, 4> s_face = parse_tokens<4>(line);
            std::array<glm::vec3, 3> face_vxs{};
            std::array<glm::vec3, 3> face_normals{};
            for (int i = 0; i < 3; ++i) {
                // Assuming there is no texture indes after the vector index
                if (!smooth) {
                    int idx = stoi(s_face[i + 1]) - 1;
                    face_vxs[i] = vertices[idx];
                }
                else {
                    // face is in the form of "f <idx 1>//<normal idx 1> ..."
                    const auto tokens = parse_tokens<2>(s_face[i + 1], "//");
                    int vidx = stoi(tokens[0]) - 1;
                    int nidx = stoi(tokens[1]) - 1;
                    face_vxs[i] = vertices[vidx];
                    face_normals[i] = vertex_normals[nidx];
                }
            }
            if (smooth) {
                triangles.push_back(std::make_unique<Triangle>(face_vxs, face_normals, material));
            }
            else {
                triangles.push_back(std::make_unique<Triangle>(face_vxs, material));
            }
            break;
        }
        case 's': {
            smooth = (line == "s 1" || line == "s on");
        }
        default:
            break;
        }
    }

    std::cout << "Mesh: " << name << " loaded with " << triangles.size() << " triangles" << std::endl;
    return new Mesh(name, std::move(triangles));
}
