#include "render_system.hpp"
#include "coordinator.hpp"
#include "components.hpp"
#include "shader.hpp"
#include "resource.hpp"
#include "window.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern Coordinator gCoordinator;
extern std::unique_ptr<Resources::ResourceManager> resourceManager;
extern std::unique_ptr<Window> gameWindow;

using namespace Systems;

Entity mCamera;

void camera_movement(const Input::GLFWKey key, const bool state) {
	using namespace Input;
	auto& transform = gCoordinator.getComponent<Components::Transform>(mCamera);
	const auto& orientation = gCoordinator.getComponent<Components::Orientation>(mCamera);
	auto& camera = gCoordinator.getComponent<Components::Camera>(mCamera);
	static float speed = 0.1f;
	if (key == GLFWKey::W && state == GLFW_PRESS) {
		transform.Position += speed * orientation.Front;
	} else if (key == GLFWKey::A && state == GLFW_PRESS) {
		transform.Position -= speed * orientation.Right;
	} else if (key == GLFWKey::S && state == GLFW_PRESS) {
		transform.Position -= speed * orientation.Front;
	} else if (key == GLFWKey::D && state == GLFW_PRESS) {
		transform.Position += speed * orientation.Right;
	}
}

void camera_look(const Input::MouseState& mouse) {
	static double sensitivity = 0.05f;
	auto& orientation = gCoordinator.getComponent<Components::Orientation>(mCamera);
	auto& camera = gCoordinator.getComponent<Components::Camera>(mCamera);
	double yawOff = mouse.deltaX * sensitivity;
	double pitchOff = mouse.deltaY * sensitivity;
	camera.Yaw += yawOff;
	camera.Pitch += pitchOff;
	if (camera.Pitch > 89.0f)
		camera.Pitch = 89.0f;
	if (camera.Pitch < -89.0f)
		camera.Pitch = -89.0f;

	camera.update(orientation);
}

RenderSystem::RenderSystem() : mShader(resourceManager->getShader("transform")), mSkybox() {

}

void RenderSystem::init() {
	mCamera = gCoordinator.createEntity();
	gCoordinator.addComponent(mCamera,
		Components::Transform{
			.Position = glm::vec3(0.0f),
			.Rotation = glm::vec3(0.0f),
			.Scale = glm::vec3(0.0f)
		});
	gCoordinator.addComponent(mCamera,
		Components::Orientation{
			.Front = glm::vec3(0.0f, 0.0f, -1.0f),
			.Up = glm::vec3(0.0f, 1.0f, 0.0f),
			.Right = glm::vec3(1.0f, 0.0f, 0.0f)
		});
	gCoordinator.addComponent(mCamera,
		Components::Camera{
			.Projection = Components::Camera::createProjection(45.0f, 0.1f, 1000.0f, 1920.0f, 1080.0f),
			.Pitch = 0.0f,
			.Yaw = -90.0f
		});

	gameWindow->getKeyboardManager().subscribe(&camera_movement, {Input::GLFWKey::W, Input::GLFWKey::A, Input::GLFWKey::S, Input::GLFWKey::D }, 4);
	gameWindow->getMouseManager().subscribe(&camera_look);

	mShader.use();
	mShader.set_int("texture1", 0);
	mShader.set_vec3("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));
	mShader.set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
}
void RenderSystem::update(float deltaTime) {
	auto const& camera = gCoordinator.getComponent<Components::Camera>(mCamera);
	auto const& camera_transform = gCoordinator.getComponent<Components::Transform>(mCamera);
	auto & camera_orientation = gCoordinator.getComponent<Components::Orientation>(mCamera);
	auto const& view = camera.getView(camera_orientation, camera_transform);

	mSkybox.onProjectionUpdate(camera.Projection);
	mSkybox.draw(glm::mat3(view));
	camera.update(camera_orientation);

	mShader.use();
	mShader.set_mat4("view", view);
	mShader.set_mat4("projection", camera.Projection);
	for (Entity entity : mEntities) {
		auto const& appearance = gCoordinator.getComponent<Components::Appearence>(entity);
		auto const& transform = gCoordinator.getComponent<Components::Transform>(entity);
		auto const& shape = gCoordinator.getComponent<Components::RenderShape>(entity);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform.Position);
		model = glm::scale(model, transform.Scale);
		// model = glm::rotate(model, transform.RotationAngle, transform.Rotation);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, appearance.Texture);
		
		mShader.use();
		mShader.set_mat4("model", model);
		mShader.set_vec3("viewPos", camera_transform.Position);
		mShader.set_vec3("baseColor", appearance.BaseColor);
		mShader.set_float("opacity", appearance.Opacity);

		shape.Shape->draw();
	}
}