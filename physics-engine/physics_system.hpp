#pragma once
#include "systems.hpp"

namespace Systems {
	class PhysicsSystem : public System {
	public:
		void update(float deltaTime) override;
	};
}