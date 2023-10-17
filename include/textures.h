//
//  Textures.h
//  Raytracer
//
//  Created by Piotr Didyk on 19.08.21.
//

#ifndef Textures_h
#define Textures_h

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

#endif /* Textures_h */
