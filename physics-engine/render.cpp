#include "render.hpp"

void Render::init() {
	
	m_system->init();
	m_skybox.init();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
}

void Render::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderSkybox(m_system->getView(), m_system->getProjection());

	m_system->update(*m_dt);

}

void Render::renderSkybox(glm::mat3 view, glm::mat4 projection) {
	m_skybox.draw(view, projection);
}
