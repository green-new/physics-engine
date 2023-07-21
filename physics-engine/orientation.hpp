#pragma once
#include <glm/glm.hpp>

namespace Components {
	struct Orientation {
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
	};
}