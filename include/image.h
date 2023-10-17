//
/**
@file Image.h
*/


//  Image.h
//  Raytracer
//
//  Created by Piotr Didyk on 14.07.21.
//

#pragma once

#include <fstream>
#include <vector>
#include "glm/glm.hpp"

/**
 Class allowing for creating an image and writing it to a file
 */
class Image
{

 private:
	const int width; ///< width of the image
	const int height; ///< height of the image
	std::vector<std::vector<glm::vec3>> data; ///< a pointer to the data representing the images

 public:
	/**
	 @param width with of the image
	 @param height height of the image
	 */
	Image(const int width, const int height) : width(width), height(height), data(width, std::vector<glm::vec3>(height))
	{
	}

	/**
	 Writes and image to a file in ppm format
	 @param path the path where to the target image
	 */
	void writeImage(const std::string& path)
	{
		std::ofstream file(path);
		file << "P3" << std::endl;
		file << width << " " << height << std::endl;
		file << 255 << std::endl;
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < width; w++)
			{
				file << (int)data[w][h].r << " " << (int)data[w][h].g << " " << (int)data[w][h].b << " ";
			}
			file << std::endl;
		}
		file.close();
	}

	/**
	 Set a value for one pixel
	 @param x x coordinate of the pixel - index of the column counting from left to right
	 @param y y coordinate of the pixel - index of the row counting from top to bottom
	 @param r red chanel value in range from 0 to 255
	 @param g green chanel value in range from 0 to 255
	 @param b blue chanel value in range from 0 to 255
	 */
	void setPixel(int x, int y, int r, int g, int b)
	{
		data[x][y] = glm::vec3(r, g, b);
	}

	/**
	 Set a value for one pixel
	 @param x x coordinate of the pixel - index of the column counting from left to right
	 @param y y coordinate of the pixel - index of the row counting from top to bottom
	 @param r red chanel value in range from 0 to 1
	 @param g green chanel value in range from 0 to 1
	 @param b blue chanel value in range from 0 to 1
	 */
	void setPixel(int x, int y, float r, float g, float b)
	{
		setPixel(x, y, (int)(255 * r), (int)(255 * g), (int)(255 * b));
	}

	/**
	 Set a value for one pixel
	 @param x x coordinate of the pixel - index of the column counting from left to right
	 @param y y coordinate of the pixel - index of the row counting from top to bottom
	 @param color color of the pixel expressed as vec3 of RGB values in range from 0 to 1
	 */
	void setPixel(const int x, const int y, const glm::vec3 color)
	{
		setPixel(x, y, (int)(255 * color.r), (int)(255 * color.g), (int)(255 * color.b));
	}
};

