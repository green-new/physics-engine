#pragma once
#include "systems.hpp"

namespace Systems {
	class PhysicsSystem : public System {
	public:
		void init();
		void update(float deltaTime) override;
		void collision(Entity base, float deltaTime);
		void switchGravity();
	private:
		bool gravity;
	};
}