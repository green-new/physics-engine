#pragma once
#include <glm/glm.hpp>

namespace Components {
	struct Transform {
		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;
		float RotationAngle;
	};
}