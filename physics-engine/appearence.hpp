#pragma once
#include "types.hpp"
#include <glm/glm.hpp>

namespace Components {
	struct Appearence {
		texture_t Texture;
		float Opacity;
		float Reflectance;
		glm::vec3 BaseColor;
	};
}