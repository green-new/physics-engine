#include "entity_manager.hpp"
#include <assert.h>
#include <iostream>

EntityManager::EntityManager() {
	this->mEntityCount = 0;
	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
		mUnusedIDs.push(entity);
	}
}
Entity EntityManager::createEntity() {
	assert(mEntityCount < MAX_ENTITIES && "[ECS] Error creating entity: out of space");

	Entity new_entity = mUnusedIDs.front();

	mUnusedIDs.pop();

	++mEntityCount;

	std::cout << "Created entity: " << new_entity << '\n';

	return new_entity;
}
void EntityManager::deleteEntity(Entity entity) {
	assert(mEntityCount < MAX_ENTITIES && "[ECS] Error deleting entity: out of space");
	assert(entity < MAX_ENTITIES && "[ECS] Error deleting entity: invalid entity ID");

	mEntitySignatures[entity].reset();
	mUnusedIDs.push(entity);

	--mEntityCount;

	std::cout << "Entities left: " << mEntityCount << '\n';
}
void EntityManager::setSignature(Entity entity, Signature signature) {
	assert(mEntityCount < MAX_ENTITIES && "[ECS] Error setting signature: out of space");
	assert(entity < MAX_ENTITIES && "[ECS] Error setting signature: invalid entity ID");

	mEntitySignatures[entity] = signature;
}
Signature EntityManager::getSignature(Entity entity) {
	assert(entity < MAX_ENTITIES && "[ECS] Error getting signature: invalid entity ID");

	return mEntitySignatures[entity];
}


