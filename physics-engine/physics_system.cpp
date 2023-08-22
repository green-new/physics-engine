#include "physics_system.hpp"
#include "coordinator.hpp"
#include "components.hpp"
#include "gjk.hpp"

using namespace Systems;

extern Coordinator gCoordinator;

void PhysicsSystem::init() {
	gravity = true;
}

void PhysicsSystem::switchGravity() {
	gravity = !gravity;
}

void PhysicsSystem::collision(Entity base, float deltaTime) {
	/* Naiive implementation */
	for (Entity entity : mEntities) {
		if (entity != base) {
			auto& colliderP = gCoordinator.getComponent<Components::RigidBody>(base);
			auto& colliderQ = gCoordinator.getComponent<Components::RigidBody>(entity);
			//if (Physics::gjk(base, entity)) {
			//	/* Do collision response */
			//}
		}
	}
}

void PhysicsSystem::update(float deltaTime) {
	const glm::vec3 gravityDirection = glm::vec3(0.0f, -9.8f, 0.0f);
	for (Entity entity : mEntities) {
		auto& transform = gCoordinator.getComponent<Components::Transform>(entity);
		auto& rigidBody = gCoordinator.getComponent<Components::RigidBody>(entity);

		// Perform movement stuff and make collision checks
		if (!rigidBody.Anchored) {

			// Gravity stuff
			if (gravity) {
				/* Move our objects downward */
				rigidBody.Acceleration += glm::vec3(0.0f, -9.8f, 0.0f) * rigidBody.Mass * 1.0f;
			}

			// Update accel.
			rigidBody.Velocity += rigidBody.Acceleration * deltaTime;

			// Update pos./vel.
			transform.Position += rigidBody.Velocity * deltaTime;

			//if (rigidBody.canCollide) {
			//	// Do collision checks
			//  // Make a check system for possibly collidable entities
			//}

			rigidBody.Acceleration = glm::zero<glm::vec3>();
		}
	}
}