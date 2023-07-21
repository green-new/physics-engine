#pragma once
#include <queue>
#include <array>
#include "types.hpp"


class EntityManager {
public:
	EntityManager();
	Entity createEntity();
	void deleteEntity(Entity entity);
	void setSignature(Entity entity, Signature signature);
	Signature getSignature(Entity entity);
private:
	std::queue<Entity> mUnusedIDs{};
	std::array<Signature, MAX_ENTITIES> mEntitySignatures;
	uint32_t mEntityCount{};
};