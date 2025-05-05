#pragma once
#include <glm/glm.hpp>
#include "box.hpp"

namespace Components {
	struct RigidBody {
		BoundingBox Box;

		Geometry::Geometry3D Shape;

		bool Anchored;
		bool onGround;

		float Mass;
		float Restitution;

		glm::vec3 Velocity;
		glm::vec3 Force;
	};
}