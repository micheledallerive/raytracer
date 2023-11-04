//
//  Textures.h
//  Raytracer
//
//  Created by Piotr Didyk on 19.08.21.
//

#pragma once

#include <iostream>
#include "glm/glm.hpp"

glm::vec3 checkerboardTexture(glm::vec2 uv)
{
	const int squares = 16;
	int x = (int)(uv.x * squares * 2);
	int y = (int)(uv.y * squares);
	if ((x + y) % 2 == 0)
	{
		return glm::vec3(0.0);
	}
	return glm::vec3(1.0);
}

glm::vec3 rainbowTexture(glm::vec2 uv)
{
	const int n = 40;
	double pos = uv.x + uv.y / 2;
	// we want pos between 0 and 2, not -1 and 1, because we want to compute the modulo
	pos += 1.0;
	int times = (int)(pos * n);
	times %= 3;

	switch (times)
	{
	case 0:
		return { 0.0, 0.0, 1.0 };
	case 1:
		return { 1.0, 0.0, 0.0 };
	default:
		return { 0.0, 1.0, 0.0 };
	}
}

/**
 * Parse a .PPM file and return a texture function
 * @param filename the PPM file
 * @return a function that maps a 2D vector to a 3D color
 */
std::function<glm::vec3(glm::vec2)> textureFromFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Could not open file " << filename << std::endl;
		exit(1);
	}

	int width, height;
	float max_value;

	std::string token;
	file >> token;
	if (token != "P3")
	{
		std::cerr << "Invalid PPM file " << filename << std::endl;
		exit(1);
	}

	file >> width >> height >> max_value;

	std::vector<std::vector<glm::vec3>> pixels(height, std::vector<glm::vec3>(width));
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width && !file.eof(); j++)
		{
			float r, g, b;
			file >> r >> g >> b;
			pixels[i][j] = glm::vec3(r, g, b) / max_value;
		}
	}

	return [pixels, width, height](glm::vec2 uv) -> glm::vec3
	{
	  int x = (int)(uv.x * (float)width);
	  int y = (int)(uv.y * (float)height);
	  return pixels[y][x];
	};
}