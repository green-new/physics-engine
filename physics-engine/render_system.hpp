#pragma once
#include "systems.hpp"
#include "shader.hpp"
#include "input_manager.hpp"
#include <memory>
#include "skybox.hpp"
#include "finite_plane.hpp"

namespace Systems {

	class RenderSystem : public System {
	public:
		RenderSystem(
			Coordinator& c,
			Input::KeyboardManager& km,
			Input::MouseManager& mm,
			Resources::ResourceManager& rm
		) : System(c),
			m_plane(),
			m_keyboardManager(km),
			m_mouseManager(mm),
			m_resourceManager(rm),
			m_camera(),
			m_render_bounding_boxes(false) { }
	public:
		void init();
		void update(float deltaTime) override;
		void toggleBoxRendering();

		glm::mat4 getView();
		glm::mat4 getProjection();
	private:
		FinitePlane m_plane;
		bool m_render_bounding_boxes; 
		Entity m_camera;

		Input::KeyboardManager& m_keyboardManager;
		Input::MouseManager& m_mouseManager;
		Resources::ResourceManager& m_resourceManager;
	};

}