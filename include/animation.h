#pragma once

float jumping_ball_position(float base_height, float max_height, float t, float sphere_radius);

glm::vec3 jumping_ball_scale(float base_height, float max_height, float t, float sphere_radius, float step_size);
