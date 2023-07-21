#include "skybox.hpp"

extern std::unique_ptr<Resources::ResourceManager> resourceManager;

Skybox::Skybox() : mSkyboxShader(resourceManager->getShader("skybox")) {
	mProjectionMatrix = glm::mat4(1.0f);
	mSkyboxTexture = resourceManager->getTexture("skybox");
	mVao = 0;
	mVbo = 0;
	init();
}
void Skybox::init() {
	mSkyboxShader.set_int("skybox", 0);
	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);
	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
}
void Skybox::onProjectionUpdate(glm::mat4 newProjection) {
	mProjectionMatrix = newProjection;
}
void Skybox::draw(glm::mat3 view) {
	glDepthMask(GL_FALSE);

	mSkyboxShader.use();
	mSkyboxShader.set_mat4("projection", mProjectionMatrix);
	// We only use the mat3 of the view. Remove the homogenous coordinate
	mSkyboxShader.set_mat4("view", view);
	glBindVertexArray(mVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}