#pragma once
#include <set>
#include "entity.hpp"
#include "skybox.hpp"
#include "coordinator.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern Coordinator gCoordinator;
extern std::unique_ptr<registry::ResourceManager> resourceManager;

class System {
public:
	virtual void update(float deltaTime) = 0;
	std::set<Entity> mEntities;
};

namespace Systems {
	using namespace glm;
	class RenderSystem : public System {
	public:
		RenderSystem() : mShaderObject(resourceManager->getShader("transform")) {
			mCamera = gCoordinator.createEntity();
			gCoordinator.addComponent(mCamera, Components::Orientation{
				.Front = vec3(0.0f, 0.0f, 1.0f),
				.Up = vec3(0.0f, 1.0f, 0.0f),
				.Right = vec3(1.0f, 0.0f, 0.0f)
			});
			gCoordinator.addComponent(mCamera, Components::Transform{
				.Position = vec3(0.0f, 0.0f, 0.0f),
				.Rotation = vec3(0.0f, 0.0f, 0.0f),
				.Scale = vec3(1.0f, 1.0f, 1.0f),
				.Angle = 0.0f
			});
			gCoordinator.addComponent(mCamera, Components::Camera{
				.
				});
		}
		void drawSkybox() {
			// mSkybox.draw();
		}
		void update(float deltaTime) override {
			for (Entity entity : mEntities) {
				auto const& appearance = gCoordinator.getComponent<Components::Appearence>(entity);
				auto const& transform = gCoordinator.getComponent<Components::Transform>(entity);
				auto const& shape = gCoordinator.getComponent<Components::RenderShape>(entity);
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, transform.Position);
				model = glm::rotate(model, transform.Angle, transform.Rotation);
				model = glm::scale(model, transform.Scale);
				shape.Shape.draw(mShaderObject);
			}
		}
	private:
		Entity mCamera;
		Skybox mSkybox;
		const Shader& mShaderObject;
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