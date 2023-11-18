//
// Created by michele on 18.11.23.
//

#pragma once

#include "loader.h"
#include <array>
#include <string>

class OBJMeshLoader: public MeshLoader
{
private:
    template<const int N>
    std::array<std::string, N> parse_tokens(const std::string &s, const std::string &delim = " ");

public:
    OBJMeshLoader() = default;
    ~OBJMeshLoader() override = default;

    Mesh *load(const std::string &file_name, const Material &material) override;
};
