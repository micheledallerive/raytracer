//
// Created by michele on 15.12.23.
//

#ifndef POINT_H
#define POINT_H


#include "light.h"
#include <vector>

class PointLight: public Light
{
private:
    glm::vec3 position;

public:
    explicit PointLight(glm::vec3 position);
    PointLight(glm::vec3 color, glm::vec3 position);
    ~PointLight() override = default;

    [[nodiscard]] glm::vec3 getPosition() const;
};


#endif//POINT_H
