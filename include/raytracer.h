//
// Created by michele on 13.12.23.
//

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "image.h"
#include "scene.h"
#include <memory>
#include <string>
#include <utility>


/**
 * Parameters for depth of field effect.
 */
struct DOFParams {
    int samples;
    float focalLength;
    float aperture;
};


class Raytracer
{
private:
    static constexpr const char *DEFAULT_OUTPUT_FILE = "output.ppm";
    static constexpr const float SCENE_Z = 1.0f;
    static constexpr const DOFParams DOF_PARAMS = {4, 8.0f, 0.2f};

    const int _width;
    const int _height;
    const int _fov;

    std::string _outputFile;
    int _SSAA = 1;

public:
    Raytracer(int width, int height, int fov);
    Raytracer(int width, int height, int fov, std::string outputFile);

    Raytracer &setAntiAliasing(int SSAAFactor);
    Raytracer &setOutputFile(std::string outputFile);

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] int getFov() const;
    [[nodiscard]] std::string getOutputFile() const;
    [[nodiscard]] int getAntiAliasingFactor() const;

    void render(const Scene &scene);
};

#endif//RAYTRACER_H
