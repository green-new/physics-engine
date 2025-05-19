#pragma once
#include "component_manager.hpp"
#include "system_manager.hpp"
#include "entity_manager.hpp"

class Coordinator {
public:
	Coordinator()
		:
		mComponentManager(std::make_unique<ComponentManager>()),
		mEntityManager(std::make_unique<EntityManager>()),
		mSystemManager(std::make_unique<SystemManager>()) {}
public:
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
	template<class T, typename... CtorArgs>
	std::shared_ptr<T> registerSystem(CtorArgs&&... args) {
		return mSystemManager->registerSystem<T>(std::forward<CtorArgs>(args)...);
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