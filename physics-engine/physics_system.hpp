#pragma once
#include "systems.hpp"
#include "octree.hpp"

namespace Systems {
	class PhysicsSystem : public System {
	public:
		PhysicsSystem(Coordinator& c) :
			System(c),
			m_gravity(true),
			m_entitiesScheduledToRemove() { }
	public:
		void init();
		void update(float deltaTime) override;
		void collision(Entity base, float deltaTime);
		void switchGravity();
		void removeEntity(Entity entity);
		void future(float futureTime);
	private:
		bool m_gravity{true};
		std::set<Entity> m_entitiesScheduledToRemove{};
	};
}