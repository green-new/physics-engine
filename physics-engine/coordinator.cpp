#include "coordinator.hpp"

void Coordinator::init() {
	mComponentManager = std::make_unique<ComponentManager>();
	mEntityManager = std::make_unique<EntityManager>();
	mSystemManager = std::make_unique<SystemManager>();
}
Entity Coordinator::createEntity() {
	return mEntityManager->createEntity();
}
void Coordinator::destroyEntity(Entity entity) {
	mEntityManager->deleteEntity(entity);
	mSystemManager->onEntityDestroyed(entity);
	mComponentManager->onEntityDestroyed(entity);
}
