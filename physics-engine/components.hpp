#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "materials.hpp"
#include "geometry.hpp"
#include "mesh.hpp"
#include "camera.hpp"

using ComponentType = uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using namespace glm;
using namespace geolib;

namespace Components {

	struct Transform {
		vec3 Position;
		vec3 Rotation;
		vec3 Scale;
		float Angle;
	};

	struct Orientation {
		vec3 Front;
		vec3 Up;
		vec3 Right;
	};

	struct Appearence {
		float Opacity;
		float Reflectance;
		vec3 BaseColor;
		Material Material;
	};

	struct Physics {
		// True for no gravity, false for gravity.
		bool Anchored;

		float Mass;
		vec3 Velocity;
		vec3 Acceleration;
		vec3 NetForce;
	};

	struct RenderShape {
		Mesh3D Shape;
	};

	struct Camera {
		float FOV;
		mat4 ProjectionMatrix;
		mat4 ViewMatrix;
		vec3 WorldUp;
	};
}

// The interface for component arrays, so we
// can refer to themin the component manager class
// as the component arrays with use generics.
class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void onEntityDestroyed(Entity entity) = 0;
};

// The component array.
// Uses generics for each type of component.
// Creates continguous memory by keeping track of indexes to entities
// and entities to indices.
template<class T>
class ComponentArray {
public:
	// Insert the specific component data for the entity.
	void insertData(Entity entity, T component) {
		assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "[ECS] Error inserting component data: component added to same entity more than once");

		unsigned int newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[entity] = component;
		++mSize;
	}
	void removeData(Entity entity) {
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "[ECS] Error deleting component data: entity doesn't have this component");

		unsigned int removedElementIndex = mEntityToIndexMap[entity];
		unsigned int lastElementIndex = mSize - 1;

		mComponentArray[removedElementIndex] = mComponentArray[lastElementIndex];
	
		Entity lastEntity = mIndexToEntityMap[lastElementIndex];

		mIndexToEntityMap[removedElementIndex] = mIndexToEntityMap[lastElementIndex];
		mEntityToIndexMap[entity] = mEntityToIndexMap[lastEntity];

		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(lastElementIndex);
		--mSize;
	}
	T& getData(Entity entity) {
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "[ECS] Error detecting entity: does not exist");

		return mComponentArray[mEntityToIndexMap[entity]];
	}

	void onEntityDestroyed(Entity entity) override {
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
			removeData(entity);
		}
	}
private:
	// Our component array for each entity of component type T.
	std::array<T, MAX_ENTITIES> mComponentArray{};

	// The index (mComponentArray) to entity map.
	std::unordered_map<Entity, unsigned int> mIndexToEntityMap{};

	// The entity to index (mComponentArray) map.
	std::unordered_map<unsigned int, Entity> mEntityToIndexMap{};

	// The current size of the component array.
	unsigned int mSize;
};

// Now we need to manage all the component arrays in a
// manager class.
// This will responsible for registering components, adding them to entities, getting types, adding, 
// removing, and getting.
class ComponentManager {
public:
	template<class T>
	void registerComponent() {
		const char* tn = typeid(T).name();

		assert(mComponentTypes.find(tn) == mComponentTypes.end() && "[ECS] Error registering component: already exists");

		mComponentTypes.insert({ tn, mNextComponentType });
		mComponentArrays.insert({ tn, std::make_shared<ComponentArray<T>>()});
	}
	template<class T>
	ComponentType getComponentType() {
		const char tn = typeid(T).name();

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
	// Map from type string pointer to a component type.
	std::unordered_map<const char*, ComponentType> mComponentTypes{};

	// Map from string pointer to a component array.
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

	// The component type to be assigned to the next registered component that starts at 0.
	ComponentType mNextComponentType{};

	template<class T>
	std::shared_ptr<ComponentArray<T>> getComponentArray() {
		const char* tn = typeid(T).name();

		assert(mComponentTypes.find(tn) != mComponentTypes.end() && "[ECS] Error getting component array: does not exist");

		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[tn]);
	}
};