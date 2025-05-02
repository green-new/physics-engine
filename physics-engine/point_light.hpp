#pragma once
#include "types.hpp"
#include <glm/glm.hpp>

namespace Components {
	struct PointLight {
		glm::vec3 Color;
		glm::vec3 Strength;
	};
}