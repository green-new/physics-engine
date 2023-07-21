#pragma once
#include "types.hpp"
#include <unordered_map>
#include <memory>
#include <assert.h>

class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void onEntityDestroyed(Entity entity) = 0;
};

template<class T>
class ComponentArray : public IComponentArray {
public:

	void insertData(Entity entity, T component) {
		assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "[ECS] Error inserting component data: component added to same entity more than once");

		unsigned int newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[newIndex] = component;
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
	std::array<T, MAX_ENTITIES> mComponentArray{};
	std::unordered_map<Entity, unsigned int> mIndexToEntityMap{};
	std::unordered_map<unsigned int, Entity> mEntityToIndexMap{};
	unsigned int mSize{};
};