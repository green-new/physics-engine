#include "mesh.hpp"
Mesh3D::Mesh3D(GLData glData) {
	mGLData = glData;
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(1, &mVertexData);
	glGenBuffers(1, &mNormalData);
	glGenBuffers(1, &mTextureData);

	glBindVertexArray(mVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexData);
	glBufferData(GL_ARRAY_BUFFER, mGLData.vertices.size() * sizeof(mGLData.vertices.at(0)), mGLData.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalData);
	glBufferData(GL_ARRAY_BUFFER, mGLData.normals.size() * sizeof(mGLData.normals.at(0)), mGLData.normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mTextureData);
	glBufferData(GL_ARRAY_BUFFER, mGLData.textures.size() * sizeof(mGLData.textures.at(0)), mGLData.textures.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
}
Mesh3D::~Mesh3D() {
	glDeleteBuffers(1, &mVertexData); 
	glDeleteBuffers(1, &mNormalData);
	glDeleteBuffers(1, &mTextureData);
	glDeleteVertexArrays(1, &mVertexArray);
}
GLsizei Mesh3D::vertexCount() const {
	return (GLsizei)mGLData.vertices.size();
}
void Mesh3D::draw() const {
	glBindVertexArray(mVertexArray);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount());
}