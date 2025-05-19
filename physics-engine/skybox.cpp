#include "skybox.hpp"
#include "geometry.hpp"

void Skybox::init() {

	auto shader = m_resourceManager.getShader("skybox");
	auto shape = m_resourceManager.getGeometry("skybox");

	shader.set_int("skybox", 0);
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(skyboxVertices),
		&skyboxVertices,
		GL_STATIC_DRAW
	);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
}

void Skybox::draw(glm::mat3 view, glm::mat4 projection) {
	glDepthMask(GL_FALSE);

	auto shader = m_resourceManager.getShader("skybox");
	auto tex = m_resourceManager.getTexture("skybox");

	shader.use();
	shader.set_mat4("projection", projection);

	// We only use the mat3 of the view. Remove the homogenous coordinate
	shader.set_mat4("view", view);

	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}