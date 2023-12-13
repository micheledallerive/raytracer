#include "image.h"
#include "glm/glm.hpp"
#include <iostream>
Image::Image(const int width, const int height)
    : width(width), height(height), data(width, std::vector<glm::vec3>(height))
{
}
void Image::writeImage(const std::string &path) const
{
    std::ofstream file(path);
    file << "P3" << std::endl;
    file << width << " " << height << std::endl;
    file << 255 << std::endl;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            file << (int) data[w][h].r << " " << (int) data[w][h].g << " " << (int) data[w][h].b << " ";
        }
        file << std::endl;
    }
    file.close();
}

void Image::setPixel(const int x, const int y, const int r, const int g, const int b)
{
    data[x][y] = glm::vec3(r, g, b);
}

void Image::setPixel(const int x, const int y, const float r, const float g, const float b)
{
    setPixel(x, y, (int) (255 * r), (int) (255 * g), (int) (255 * b));
}

void Image::setPixel(const int x, const int y, const glm::vec3 color)
{
    setPixel(x, y, (int) (255 * color.r), (int) (255 * color.g), (int) (255 * color.b));
}

Image Image::downsample(int factor) const
{
    if (factor == 1)
        return *this;

    Image downsampled(width / factor, height / factor);

    for (int i = 0; i < downsampled.width; i++) {
        for (int j = 0; j < downsampled.height; j++) {
            glm::vec3 color(0);
            for (int k = 0; k < factor; k++) {
                for (int l = 0; l < factor; l++) {
                    color += data[i * factor + k][j * factor + l]; // data contains colors 0-255
                }
            }
            color /= (float) (255 * factor * factor);
            downsampled.setPixel(i, j, color);

        }
    }

    return downsampled;
}
