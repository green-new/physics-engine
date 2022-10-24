#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

struct Sunlight {
	vec3 direction;
	vec3 color;
	float ambience_strength;
	float specular_strength;
	float diffuse_strength;
};

