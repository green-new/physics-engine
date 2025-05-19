#pragma once

#include "systems.hpp"

class Render {
public:
	Render(
		std::shared_ptr<Systems::RenderSystem> s,
		std::shared_ptr<double> dt,
		Resources::ResourceManager& rm
	) : 
		m_system(s),
		m_dt(dt),
		m_resourceManager(rm),
		m_skybox(rm) {}
	~Render() = default;
public:
	void init();
	void render();
private:
	void renderSkybox(glm::mat3 view, glm::mat4 projection);
private:
	std::shared_ptr<Systems::RenderSystem> m_system;
	std::shared_ptr<double> m_dt;
	Resources::ResourceManager& m_resourceManager;
	Skybox m_skybox;
};
