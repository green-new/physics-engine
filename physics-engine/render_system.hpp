#pragma once
#include "systems.hpp"
#include "shader.hpp"
#include "input_manager.hpp"
#include <memory>
#include "skybox.hpp"

namespace Systems {
	class RenderSystem : public System {
	public:
		RenderSystem();
		void init();
		void update(float deltaTime) override;
		void renderBoundingBoxes(float dt);
		void toggleBoxRendering();
	private:
		Shader& m_shader;
		Skybox m_skybox;
		bool m_render_bounding_boxes;
	};
}