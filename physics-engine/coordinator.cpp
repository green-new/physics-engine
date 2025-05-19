#include "coordinator.hpp"

Entity Coordinator::createEntity() {
	return mEntityManager->createEntity();
}
void Coordinator::destroyEntity(Entity entity) {
	mEntityManager->deleteEntity(entity);
	mSystemManager->onEntityDestroyed(entity);
	mComponentManager->onEntityDestroyed(entity);
}
