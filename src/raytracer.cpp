//
// Created by michele on 13.12.23.
//
#include "raytracer.h"
#include "lightning.h"
#include "ray.h"
#include <iostream>

Raytracer::Raytracer(int width, int height, int fov, std::string outputFile)
    : _width(width), _height(height), _fov(fov), _outputFile(std::move(outputFile))
{
}

Raytracer::Raytracer(int width, int height, int fov)
    : Raytracer(width, height, fov, std::string(Raytracer::DEFAULT_OUTPUT_FILE))
{
}

Raytracer &Raytracer::setAntiAliasing(int SSAAFactor)
{
    this->_SSAA = SSAAFactor;
    return *this;
}

Raytracer &Raytracer::setOutputFile(std::string outputFile)
{
    this->_outputFile = std::move(outputFile);
    return *this;
}

int Raytracer::getWidth() const
{
    return _width;
}

int Raytracer::getHeight() const
{
    return _height;
}

int Raytracer::getFov() const
{
    return _fov;
}

std::string Raytracer::getOutputFile() const
{
    return _outputFile;
}

int Raytracer::getAntiAliasingFactor() const
{

    return _SSAA;
}

void Raytracer::render(const Scene &scene)
{
    clock_t t = clock();// variable for keeping the time of the rendering

    const int width = _width * _SSAA;
    const int height = _height * _SSAA;

    Image image(width, height);

    const float pixelSize = (2.0f * tan(glm::radians(_fov / 2.0f))) / width;

    const float sceneLeft = (float) -width * pixelSize / 2.0f;
    const float sceneTop = (float) height * pixelSize / 2.0f;

    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++) {
            glm::vec3 origin(0, 0, 0);

            const float x = sceneLeft + (float) i * pixelSize + pixelSize / 2.0f;
            const float y = sceneTop - (float) j * pixelSize - pixelSize / 2.0f;
            const float z = SCENE_Z;
            glm::vec3 direction(x, y, z);
            direction = glm::normalize(direction);

            Ray ray(origin, direction);// ray traversal

            image.setPixel(i, j, tone_mapping(trace_ray(scene, ray)));
        }

    t = clock() - t;
    std::cout << "It took " << ((float) t) / CLOCKS_PER_SEC << " seconds to render the image." << std::endl;
    std::cout << "I could render at " << (float) CLOCKS_PER_SEC / ((float) t) << " frames per second." << std::endl;

    std::move(image.downsample(_SSAA)).writeImage("result.ppm");
}