#include "material.h"

#include <utility>

MaterialFactory &MaterialFactory::ambient(const glm::vec3 &ambient)
{
    this->material.ambient = ambient;
    return *this;
}

MaterialFactory &MaterialFactory::diffuse(const glm::vec3 &diffuse)
{
    this->material.diffuse = diffuse;
    return *this;
}

MaterialFactory &MaterialFactory::specular(const glm::vec3 &specular)
{
    this->material.specular = specular;
    return *this;
}

MaterialFactory &MaterialFactory::shininess(float shininess)
{
    this->material.shininess = shininess;
    return *this;
}

MaterialFactory &MaterialFactory::texture(std::function<glm::vec3(glm::vec2)> texture)
{
    this->material.texture = std::move(texture);
    return *this;
}

MaterialFactory &MaterialFactory::reflection(float reflection)
{
    this->material.reflection = reflection;
    return *this;
}

MaterialFactory &MaterialFactory::refractiveIndex(float refractive_index)
{
    this->material.refractive_index = refractive_index;
    return *this;
}

MaterialFactory &MaterialFactory::transparency(float transparency)
{
    this->material.transparency = transparency;
    return *this;
}

Material MaterialFactory::build()
{
    return this->material;
}
