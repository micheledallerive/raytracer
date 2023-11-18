//
// Created by michele on 17.10.23.
//

#pragma once

#include "glm/glm.hpp"
#include "material.h"
#include "objects/mesh.h"
#include <fstream>
#include <string>

class MeshLoader
{
public:
    MeshLoader() = default;
    virtual ~MeshLoader() = default;

    virtual Mesh* load(const std::string &file_name, const Material &material) = 0;
};