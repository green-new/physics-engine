#pragma once
#include <glm/glm.hpp>

namespace Components {
	struct RigidBody {
		Geometry::Geometry3D Shape;
		bool Anchored;
		float Mass;
		glm::vec3 Velocity;
		glm::vec3 Acceleration;
		glm::vec3 NetForce;
	};
}