#pragma once
#include "component_array.hpp"

class ComponentManager {
public:

	template<class T>
	void registerComponent() {
		const char* tn = typeid(T).name();

		assert(mComponentTypes.find(tn) == mComponentTypes.end() && "[ECS] Error registering component: already exists");

		mComponentTypes.insert({ tn, mNextComponentType });
		mComponentArrays.insert({ tn, std::make_shared<ComponentArray<T>>() });

		++mNextComponentType;
	}

	template<class T>
	ComponentType getComponentType() {
		const char* tn = typeid(T).name();

		assert(mComponentTypes.find(tn) != mComponentTypes.end() && "[ECS] Error getting component type: doesn't exist");

		return mComponentTypes[tn];
	}

	template<class T>
	void addComponent(Entity entity, T component) {
		getComponentArray<T>()->insertData(entity, component);
	}

	template<class T>
	void removeComponent(Entity entity) {
		getComponentArray<T>()->removeData(entity);
	}

	template<class T>
	T& getComponent(Entity entity) {
		return getComponentArray<T>()->getData(entity);
	}

	void onEntityDestroyed(Entity entity) {
		for (auto const& pair : mComponentArrays) {
			auto const& component = pair.second;

			component->onEntityDestroyed(entity);
		}
	}
private:
	std::unordered_map<const char*, ComponentType> mComponentTypes{};
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};
	ComponentType mNextComponentType{};

	template<class T>
	std::shared_ptr<ComponentArray<T>> getComponentArray() {
		const char* tn = typeid(T).name();

		assert(mComponentTypes.find(tn) != mComponentTypes.end() && "[ECS] Error getting component array: does not exist");

		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[tn]);
	}
};