#pragma once
#include "component_manager.hpp"
#include "system_manager.hpp"
#include "entity_manager.hpp"

class Coordinator {
public:
	void init();
	Entity createEntity();
	void destroyEntity(Entity entity);

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