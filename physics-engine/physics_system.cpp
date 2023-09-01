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
			auto& colliderP = gCoordinator.getComponent<Components::RigidBody>(base).Shape;
			auto& colliderQ = gCoordinator.getComponent<Components::RigidBody>(entity).Shape;
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
				rigidBody.Force += glm::vec3(0.0f, -9.8f, 0.0f) * rigidBody.Mass * 10.0f;
			}

			// Update accel.
			rigidBody.Velocity += rigidBody.Force / rigidBody.Mass * deltaTime;

			// Update pos./vel.
			transform.Position += rigidBody.Velocity * deltaTime;

			//if (rigidBody.canCollide) {
			//	// Do collision checks
			//  // Make a check system for possibly collidable entities
			//}

			// Zero out the force
			rigidBody.Force = glm::zero<glm::vec3>();
		}
	}
}