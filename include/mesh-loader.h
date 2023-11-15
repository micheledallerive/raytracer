//
// Created by michele on 17.10.23.
//

#pragma once

#include "glm/glm.hpp"
#include "material.h"
#include "objects/mesh.h"
#include <fstream>
#include <string>

class OBJMeshLoader
{
private:
    template<const int N>
    static std::array<std::string, N> parse_tokens(const std::string &s, const std::string &delim = " ");

public:
    static std::unique_ptr<Mesh>
    load(const std::string &file_name, const glm::vec3 &position, const Material &material);
};

class MeshLoader
{
public:
    static std::unique_ptr<Mesh>
    load(const MeshType &type, const std::string &file_name, const glm::vec3 &position, const Material &material);
};