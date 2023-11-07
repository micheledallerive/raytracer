#ifndef ANIMATE
#define ANIMATE 0
#endif

#include "../include/glm/glm.hpp"
#include "../include/animation.h"

#if ANIMATE
float jumping_ball_position(float base_height, float max_height, float t, float sphere_radius)
{
	// sphere_radius optional for the case of a ball elastic collision
	if (sphere_radius > 0)
		base_height -= sphere_radius / 3;
	// simulate a ball bouncing, starting from base_height and reaching max_height, and keeping bouncing up and down
	// return the value of the y position of the ball at time t while bouncing
	// t is in seconds
	const float g = 9.81f;
	const float period = 2 * std::sqrt((max_height - base_height) / g);
	const float t_mod = std::fmod(t, period);
	const float t_mod_ = t_mod < period / 2 ? t_mod : period - t_mod;
	// start from base_height and reach max_height, then go back to base_height
	const float y = base_height + (max_height - base_height) * (1 - t_mod_ * t_mod_ / (period * period / 4));
	return y;
}
#else
float jumping_ball_position(float base_height, float max_height, float t, float sphere_radius)
{
	return base_height;
}
#endif

#if ANIMATE
glm::vec3 jumping_ball_scale(float base_height, float max_height, float t, float sphere_radius, float step_size)
{
	const float y_position = jumping_ball_position(base_height, max_height, t, sphere_radius);
	float distance_from_ground = y_position - (base_height);
	if (distance_from_ground > 0)
		return glm::vec3(1.0f);

	const float next_y = jumping_ball_position(base_height, max_height, t + step_size, sphere_radius);
	const float next_distance_from_ground = next_y - (base_height);
	if (next_distance_from_ground > 0.25f)
		return glm::vec3(1.0f);

	const float y_scale = 1.0f + distance_from_ground / sphere_radius;
	const float xz_scale = 1.0f - sqrt(2) / 2 * distance_from_ground / sphere_radius;
	return glm::vec3(xz_scale, y_scale, xz_scale);
}
#else
glm::vec3 jumping_ball_scale(float base_height, float max_height, float t, float sphere_radius, float step_size)
{
	return glm::vec3(1.0f);
}
#endif

#ifndef ANIMATE
#define ANIMATE 0
#endif