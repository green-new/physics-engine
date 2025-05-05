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

std::unordered_map<int, int> player_keys;

void camera_movement(const int key, const int scancode, const int action, const int mods) {
	player_keys[key] = action;
}

void update_camera_movement() {
	auto& transform = gCoordinator.getComponent<Components::Transform>(mCamera);
	const auto& orientation = gCoordinator.getComponent<Components::Orientation>(mCamera);
	auto& camera = gCoordinator.getComponent<Components::Camera>(mCamera);
	static float speed = 0.1f;

	if (player_keys[GLFW_KEY_W] != GLFW_RELEASE) {
		transform.Position += speed * orientation.Front;
	}
	if (player_keys[GLFW_KEY_S] != GLFW_RELEASE) {
		transform.Position -= speed * orientation.Front;
	}
	if (player_keys[GLFW_KEY_A] != GLFW_RELEASE) {
		transform.Position -= speed * orientation.Right;
	}
	if (player_keys[GLFW_KEY_D] != GLFW_RELEASE) {
		transform.Position += speed * orientation.Right;
	}
}

void camera_look(const Input::MouseState& mouse) {
	static double sensitivity = 0.05f;
	auto& orientation = gCoordinator.getComponent<Components::Orientation>(mCamera);
	auto& camera = gCoordinator.getComponent<Components::Camera>(mCamera);
	double yawOff = mouse.deltaX() * sensitivity;
	double pitchOff = -mouse.deltaY() * sensitivity;
	camera.Yaw += yawOff;
	camera.Pitch += pitchOff;
	if (camera.Pitch > 89.0f) {
		camera.Pitch = 89.0f;
	}
	if (camera.Pitch < -89.0f) {
		camera.Pitch = -89.0f;
	}

	camera.update(orientation);
}

RenderSystem::RenderSystem() : m_shader(resourceManager->getShader("transform")), m_skybox(), m_plane(), m_render_bounding_boxes(false) {}

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

	gameWindow->getKeyboardManager().subscribe(&camera_movement, {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D });
	gameWindow->getMouseManager().subscribe(&camera_look);

	m_shader.use();
	m_shader.set_int("texture1", 0);
	m_shader.set_vec3("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));
	m_shader.set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
}

void RenderSystem::update(float deltaTime) {

	/* Draw the world plane */
	

	auto const& camera = gCoordinator.getComponent<Components::Camera>(mCamera);
	auto const& camera_transform = gCoordinator.getComponent<Components::Transform>(mCamera);
	auto & camera_orientation = gCoordinator.getComponent<Components::Orientation>(mCamera);
	auto const& view = camera.getView(camera_orientation, camera_transform);

	m_skybox.onProjectionUpdate(camera.Projection);
	m_skybox.draw(glm::mat3(view));
	camera.update(camera_orientation);

	update_camera_movement();

	m_shader.use();
	m_shader.set_mat4("view", view);
	m_shader.set_mat4("projection", camera.Projection);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
	m_shader.set_mat4("model", model);
	m_plane.draw();

	for (Entity entity : m_entities) {
		auto const& appearance = gCoordinator.getComponent<Components::Appearence>(entity);
		auto const& transform = gCoordinator.getComponent<Components::Transform>(entity);
		auto const& shape = gCoordinator.getComponent<Components::RenderShape>(entity);
		auto const& body = gCoordinator.getComponent<Components::RigidBody>(entity);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform.Position);
		// model = glm::scale(model, transform.Scale);
		// model = glm::rotate(model, transform.RotationAngle, transform.Rotation);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, appearance.Texture);
		
		m_shader.use();
		m_shader.set_mat4("model", model);
		m_shader.set_vec3("viewPos", camera_transform.Position);
		m_shader.set_vec3("baseColor", appearance.BaseColor);
		m_shader.set_float("opacity", appearance.Opacity);

		shape.Shape->draw();
		
		// draw bounding boxes
		const glm::vec3 bbColor = body.Box.overlapping ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(1.0f);
		m_shader.set_vec3("baseColor", bbColor);
		model = glm::scale(model, glm::vec3{ body.Mass });
		m_shader.set_mat4("model", model);
		if (m_render_bounding_boxes && shape.BoxShape) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("white"));
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(2);
			shape.BoxShape->draw();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glLineWidth(1);
		}
	}
}

void RenderSystem::toggleBoxRendering() {
	m_render_bounding_boxes = !m_render_bounding_boxes;
	std::cout << (m_render_bounding_boxes ? "Render boxes: ON" : "Render boxes: OFF") << '\n';
}
