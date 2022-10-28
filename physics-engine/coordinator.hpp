#pragma once
#include "signatures.hpp"
#include "system.hpp"
#include "components.hpp"

class Coordinator {
public:
	void init() {
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mSystemManager = std::make_unique<SystemManager>();
	}
	Entity createEntity() {
		return mEntityManager->createEntity();
	}
	void destroyEntity(Entity entity) {
		mEntityManager->deleteEntity(entity);

		mSystemManager->onEntityDestroyed(entity);
		mComponentManager->onEntityDestroyed(entity);
	}
	template<class T>
	void registerComponent() {
		mComponentManager->registerComponent<T>();
	}
	template<class T>
	void addComponent(Entity entity, T component) {
		mComponentManager->addComponent<T>(entity, component);

		auto signature = mEntityManager->getSignature(entity);
		signature.set(mComponentManager->getComponentType<T>(), true);
		mEntityManager->setSignature(entity, signature);
		mSystemManager->onEntitySignatureChange(entity, signature);
	}
	template<class T>
	T& getComponent(Entity entity) {
		return mComponentManager->getComponent<T>(entity);
	}
	template<class T>
	ComponentType getComponentType() {
		return mComponentManager->getComponentType<T>();
	}
	template<class T>
	std::shared_ptr<T> registerSystem() {
		return mSystemManager->registerSystem<T>();
	}
	template<class T>
	void setSystemSignature(Signature signature) {
		mSystemManager->setSignature<T>(signature);
	}
private:
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<SystemManager> mSystemManager;
};