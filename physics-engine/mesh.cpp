#include "mesh.hpp"
Mesh3D::Mesh3D(GLData glData) {
	mGLData = glData;
	mCurrentRenderMethod = FLAT_SHADING;
	glGenVertexArrays(2, mVertexArray);
	glGenBuffers(2, mVertexData);
	glGenBuffers(2, mNormalData);
	glGenBuffers(1, &mTextureData);

	glBindVertexArray(mVertexArray[FLAT_SHADING]);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexData[FLAT_SHADING]);
	glBufferData(GL_ARRAY_BUFFER, mGLData.flatVertices.size() * sizeof(mGLData.flatVertices.at(0)), mGLData.flatVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalData[FLAT_SHADING]);
	glBufferData(GL_ARRAY_BUFFER, mGLData.flatNormals.size() * sizeof(mGLData.flatNormals.at(0)), mGLData.flatNormals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

/*	glBindBuffer(GL_ARRAY_BUFFER, mTextureData);
	glBufferData(GL_ARRAY_BUFFER, mGLData.textureData.size() * sizeof(mGLData.textureData.at(0)), mGLData.textureData.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, (GLint)mGLData.textureData.size(), GL_FLOAT, GL_FALSE, 0, 0)*/;

	glBindVertexArray(mVertexArray[SMOOTH_SHADING]);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexData[SMOOTH_SHADING]);
	glBufferData(GL_ARRAY_BUFFER, mGLData.smoothVertices.size() * sizeof(mGLData.smoothVertices.at(0)), mGLData.smoothVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalData[SMOOTH_SHADING]);
	glBufferData(GL_ARRAY_BUFFER, mGLData.smoothNormals.size() * sizeof(mGLData.smoothNormals.at(0)), mGLData.smoothNormals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, mTextureData);
	//glBufferData(GL_ARRAY_BUFFER, mGLData.textureData.size() * sizeof(mGLData.textureData.at(0)), mGLData.textureData.data(), GL_STATIC_DRAW);
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, (GLint)mGLData.textureData.size(), GL_FLOAT, GL_FALSE, 0, 0);
}
Mesh3D::~Mesh3D() {
	glDeleteBuffers(2, mVertexData); 
	glDeleteBuffers(2, mNormalData);
	glDeleteBuffers(1, &mTextureData);
	glDeleteVertexArrays(2, mVertexArray);
}
void Mesh3D::swapVaos() {
	(mCurrentRenderMethod == FLAT_SHADING ? mCurrentRenderMethod = SMOOTH_SHADING : mCurrentRenderMethod = FLAT_SHADING);
}
GLsizei Mesh3D::vertexCount() const {
	if (mCurrentRenderMethod == SMOOTH_SHADING) {
		return (GLsizei)mGLData.smoothVertices.size();
	}
	else if (mCurrentRenderMethod == FLAT_SHADING) {
		return (GLsizei)mGLData.flatVertices.size();
	}
	else {
		return (GLsizei)mGLData.smoothVertices.size();
	}
}
void Mesh3D::draw() const {
	glBindVertexArray(mVertexArray[mCurrentRenderMethod]);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount());
}