#include "physics_system.hpp"
#include "coordinator.hpp"
#include "components.hpp"
#include "gjk.hpp"

#include <format>

using namespace Systems;

extern Coordinator gCoordinator;

void PhysicsSystem::init() {
	m_gravity = true;

}

void PhysicsSystem::switchGravity() {
	m_gravity = !m_gravity;
}

void PhysicsSystem::collision(Entity me, float deltaTime) {
	/* Naiive implementation */
	for (Entity other : m_entities) {
		if (other == me) {
			continue;
		}
		auto& p = gCoordinator.getComponent<Components::Transform>(me);
		auto& pBody = gCoordinator.getComponent<Components::RigidBody>(me);
		auto& q = gCoordinator.getComponent<Components::Transform>(other);
		auto& qBody = gCoordinator.getComponent<Components::RigidBody>(other);

		BoundingBox qWorldBox = BoundingBox(qBody.Box, q.Position);
		BoundingBox pWorldBox = BoundingBox(pBody.Box, p.Position);

		if (qWorldBox.overlaps(pWorldBox)) {
			// std::cout << "Overlapping\n";
			pBody.Box.overlapping = true;
			qBody.Box.overlapping = true;
			const glm::vec3 overlap = qWorldBox.overlap(pWorldBox);

			// correct q's position
			q.Position -= overlap;

			// swap the positions (assuming mass is constant, which its not, but we avoid the magic math here)
			const glm::vec3 tempVelocity = qBody.Velocity;
			qBody.Velocity = pBody.Velocity;
			pBody.Velocity = tempVelocity;
		} else {
			pBody.Box.overlapping = false;
			qBody.Box.overlapping = false;
		}
	}
}

void PhysicsSystem::future(float futureTime) {

}

void PhysicsSystem::removeEntity(Entity entity) {
	m_entitiesScheduledToRemove.insert(entity);
}

void PhysicsSystem::update(float deltaTime) {

	/* if there are no entities left, exit early */
	if (m_entities.empty()) {
		return;
	}
	const glm::vec3 gravityDirection = glm::vec3(0.0f, -9.8f, 0.0f);

	// handle removing entities before iteration
	for (Entity entity : m_entitiesScheduledToRemove) {
		gCoordinator.destroyEntity(entity);
	}

	// remove entities from schedule
	m_entitiesScheduledToRemove.clear();

	for (Entity entity : m_entities) {
		auto& transform = gCoordinator.getComponent<Components::Transform>(entity);
		auto& rigidBody = gCoordinator.getComponent<Components::RigidBody>(entity);

		// Perform movement stuff and make collision checks
		if (!rigidBody.Anchored) {

			// run future checks
			future(deltaTime);

			if (!rigidBody.Anchored) {
				collision(entity, deltaTime);
			}

			// Gravity stuff
			if (m_gravity && !rigidBody.onGround) {
				/* Move our objects downward iff they are not grounded */
				// rigidBody.Force += glm::vec3(0.0f, -9.8f, 0.0f);
			}

			// Update accel.
			rigidBody.Velocity += rigidBody.Force / rigidBody.Mass * deltaTime;

			// Update pos./vel.
			transform.Position += rigidBody.Velocity * deltaTime;

			// Zero out the force
			rigidBody.Force = glm::zero<glm::vec3>();

			// Delete entity if fall out of world
			if (transform.Position.y < -100.0f) {
				// removeEntity(entity);
			}
		}
	}
}