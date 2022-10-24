#pragma once
#include <set>
#include "entity.hpp"

class System {
public:
	virtual void update(float deltaTime) = 0;
	std::set<Entity> mEntities;
};

namespace Systems {
	class RenderSystem : public System {
	public:
		void update(float deltaTime) override {
			for (Entity entity : mEntities) {
				auto appearance = gCoordinator.getComponent<Components::Appearence>(entity);
				auto transform = gCoordinator.getComponent<Components::Transform>(entity);
				auto shape = gCoordinator.getComponent<Components::RenderShape>(entity);
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, transform.Position);
				model = glm::rotate(model, transform.Angle, transform.Rotation);
				model = glm::scale(model, transform.Scale);
				shader.set_mat4("model", model);
				shape.Shape.draw(shader);
			}
		}
	};

	class PhysicsSystem : public System {
	public:
		void update(float deltaTime) override {
			for (Entity entity : mEntities) {
				// Work on this later
			}
		}
	};
}

class SystemManager {
public:
	template<class T>
	std::shared_ptr<T> registerSystem() {
		const char* tn = typeid(T).name();

		assert(mSystems.find(tn) == mSystems.end() && "[ECS] Error registering system: already exists");

		auto system = std::make_shared<System>();
		mSystems.insert({tn, system});
		return system;
	}
	template<class T>
	void setSignature(Signature signature) {
		const char* tn = typeid(T).name();

		assert(mSystems.find(tn) != mSystems.end() && "[ECS] Error configuring system signature: system does not exist");

		mSignatures.insert({ tn, signature });
	}
	void onEntityDestroyed(Entity entity) {
		for (auto const& pair : mSystems) {
			auto const& system = pair.second;
			system->mEntities.erase(entity);
		}
	}
	void onEntitySignatureChange(Entity entity, Signature signature) {
		for (auto const& pair : mSystems) {
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = mSignatures[type];

			if ((signature & systemSignature) == systemSignature) {
				system->mEntities.insert(entity);
			}
			else {
				system->mEntities.erase(entity);
			}
		}
	}
private:
	// Our list of systems.
	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};

	// Our list of signatures.
	std::unordered_map<const char*, Signature> mSignatures{};
};