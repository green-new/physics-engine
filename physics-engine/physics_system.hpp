#pragma once
#include "systems.hpp"

namespace Systems {
	class Collision {
		virtual void resolve() const = 0;
	};

	class EntityToEntityCollision : Collision {

	};

	class EntityToWorldCollision : Collision {

	};

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