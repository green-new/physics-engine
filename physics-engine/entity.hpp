#pragma once
#include <cstdint>
#include <queue>
#include <cassert>
#include <array>
#include "signatures.hpp"

using Entity = uint32_t;
const Entity MAX_ENTITIES = 10000;

class EntityManager {
public:
	EntityManager() {
		// Generate all unused entity ids.
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
			mUnusedIDs.push(entity);
		}
	}
	Entity createEntity() {
		assert(mEntityCount < MAX_ENTITIES && "[ECS] Error creating entity: out of space");

		Entity new_entity = mUnusedIDs.front();

		mUnusedIDs.pop();

		++mEntityCount;

		return new_entity;
	}
	// Maybe have other classes subscribe to this event function.
	void deleteEntity(Entity entity) {
		assert(mEntityCount < MAX_ENTITIES && "[ECS] Error creating entity: out of space");
		assert(entity < MAX_ENTITIES && "[ECS] Error creating entity: invalid entity ID");

		// Clear the signature of this entity
		mEntitySignatures[entity].reset();
		mUnusedIDs.push(entity);

		// Reduce the active number of entities in the system.
		--mEntityCount;
	}
	// Sets the component signature of this entity.
	// The signature is responsible for telling the managers
	// what components a particular entity owns.
	// The entity ID is the index for the signature array, 
	// which references its signature.
	void setSignature(Entity entity, Signature signature) {
		assert(mEntityCount < MAX_ENTITIES && "[ECS] Error setting signature: out of space");
		assert(entity < MAX_ENTITIES && "[ECS] Error setting signature: invalid entity ID");

		mEntitySignatures[entity] = signature;
	}
	// Get the signature of this entity id.
	Signature getSignature(Entity entity) {
		assert(entity < MAX_ENTITIES && "[ECS] Error getting signature: invalid entity ID");

		return mEntitySignatures[entity];
	}
private:
	// The queue of unused entity ids.
	std::queue<Entity> mUnusedIDs{};

	// The signatures of each entity.
	std::array<Signature, MAX_ENTITIES> mEntitySignatures;

	// The number of living entities.
	unsigned int mEntityCount{};
};

