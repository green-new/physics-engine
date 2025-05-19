#include "render_system.hpp"
#include "coordinator.hpp"
#include "components.hpp"
#include "shader.hpp"
#include "resource.hpp"
#include "window.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Systems;

std::unordered_map<int, int> player_keys;

void camera_movement(const int key, const int scancode, const int action, const int mods) {
	player_keys[key] = action;
}

void update_camera_movement(
	Components::Transform& transform,
	const Components::Orientation& orient,
	Components::Camera& 
) {
	static float speed = 0.1f;

	if (player_keys[GLFW_KEY_W] != GLFW_RELEASE) {
		transform.Position += speed * orient.Front;
	}
	if (player_keys[GLFW_KEY_S] != GLFW_RELEASE) {
		transform.Position -= speed * orient.Front;
	}
	if (player_keys[GLFW_KEY_A] != GLFW_RELEASE) {
		transform.Position -= speed * orient.Right;
	}
	if (player_keys[GLFW_KEY_D] != GLFW_RELEASE) {
		transform.Position += speed * orient.Right;
	}
}

void camera_look(
	Components::Orientation& orient, 
	Components::Camera& cam, 
	const Input::MouseState& mouse
) {
	static double sensitivity = 0.05f;
	double yawOff = mouse.deltaX() * sensitivity;
	double pitchOff = -mouse.deltaY() * sensitivity;
	cam.Yaw += yawOff;
	cam.Pitch += pitchOff;
	if (cam.Pitch > 89.0f) {
		cam.Pitch = 89.0f;
	}
	if (cam.Pitch < -89.0f) {
		cam.Pitch = -89.0f;
	}

	cam.update(orient);
}

void RenderSystem::init() {
	m_camera = m_coordinator.createEntity();
	m_coordinator.addComponent(m_camera,
		Components::Transform{
			.Position = glm::vec3(0.0f),
			.Rotation = glm::vec3(0.0f),
			.Scale = glm::vec3(0.0f)
		});
	m_coordinator.addComponent(m_camera,
		Components::Orientation{
			.Front = glm::vec3(0.0f, 0.0f, -1.0f),
			.Up = glm::vec3(0.0f, 1.0f, 0.0f),
			.Right = glm::vec3(1.0f, 0.0f, 0.0f)
		});
	m_coordinator.addComponent(m_camera,
		Components::Camera{
			.Projection = Components::Camera::createProjection(45.0f, 0.1f, 1000.0f, 1920.0f, 1080.0f),
			.Pitch = 0.0f,
			.Yaw = -90.0f
		});
	

	m_keyboardManager.subscribe(&camera_movement, { GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D });
	m_mouseManager.subscribe([this](const Input::MouseState& mouse) {
		auto& orient = this->m_coordinator.getComponent<Components::Orientation>(this->m_camera);
		auto& cam = this->m_coordinator.getComponent<Components::Camera>(this->m_camera);
		return camera_look(orient, cam, mouse);
	});

	auto shader = m_resourceManager.getShader("transform");
	shader.use();
	shader.set_int("texture1", 0);
	shader.set_vec3("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

	m_plane.init();
}

void RenderSystem::update(float deltaTime) {
	auto& camera = m_coordinator.getComponent<Components::Camera>(m_camera);
	auto& camera_transform = m_coordinator.getComponent<Components::Transform>(m_camera);
	auto & camera_orientation = m_coordinator.getComponent<Components::Orientation>(m_camera);
	auto const& view = camera.getView(camera_orientation, camera_transform);
	camera.update(camera_orientation);

	update_camera_movement(camera_transform, camera_orientation, camera);

	auto shader = m_resourceManager.getShader("transform");
	shader.use();
	shader.set_mat4("view", view);
	shader.set_mat4("projection", camera.Projection);
	shader.set_vec3("viewPos", camera_transform.Position);

	// draw the plane
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(100.0f, 1.0f, 100.0f));
	shader.set_mat4("model", model);

	m_plane.draw();

	for (Entity entity : m_entities) {
		auto const& appearance = m_coordinator.getComponent<Components::Appearence>(entity);
		auto const& transform = m_coordinator.getComponent<Components::Transform>(entity);
		auto const& shape = m_coordinator.getComponent<Components::RenderShape>(entity);
		auto const& body = m_coordinator.getComponent<Components::RigidBody>(entity);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform.Position);
		// model = glm::scale(model, transform.Scale);
		// model = glm::rotate(model, transform.RotationAngle, transform.Rotation);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, appearance.Texture);
		
		shader.use();
		shader.set_mat4("model", model);
		shader.set_vec3("baseColor", appearance.BaseColor);
		shader.set_float("opacity", appearance.Opacity);

		shape.Shape->draw();
		
		// draw bounding boxes
		const glm::vec3 bbColor = body.Box.overlapping ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(1.0f);
		shader.set_vec3("baseColor", bbColor);
		model = glm::scale(model, glm::vec3{ body.Mass });
		shader.set_mat4("model", model);
		if (m_render_bounding_boxes && shape.BoxShape) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_resourceManager.getTexture("white"));
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

glm::mat4 RenderSystem::getView() {
	auto& camera = m_coordinator.getComponent<Components::Camera>(m_camera);
	auto& camera_transform = m_coordinator.getComponent<Components::Transform>(m_camera);
	auto& camera_orientation = m_coordinator.getComponent<Components::Orientation>(m_camera);
	auto const& view = camera.getView(camera_orientation, camera_transform);
	return view;
}


glm::mat4 RenderSystem::getProjection() {
	auto& camera = m_coordinator.getComponent<Components::Camera>(m_camera);
	return camera.Projection;
}
