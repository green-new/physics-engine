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
	glBufferData(GL_ARRAY_BUFFER, mGLData.flat_vertices.size() * sizeof(mGLData.flat_vertices.at(0)), mGLData.flat_vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalData[FLAT_SHADING]);
	glBufferData(GL_ARRAY_BUFFER, mGLData.flat_normals.size() * sizeof(mGLData.flat_normals.at(0)), mGLData.flat_normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

/*	glBindBuffer(GL_ARRAY_BUFFER, mTextureData);
	glBufferData(GL_ARRAY_BUFFER, mGLData.textureData.size() * sizeof(mGLData.textureData.at(0)), mGLData.textureData.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, (GLint)mGLData.textureData.size(), GL_FLOAT, GL_FALSE, 0, 0)*/;

	glBindVertexArray(mVertexArray[SMOOTH_SHADING]);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexData[SMOOTH_SHADING]);
	glBufferData(GL_ARRAY_BUFFER, mGLData.smooth_vertices.size() * sizeof(mGLData.smooth_vertices.at(0)), mGLData.smooth_vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalData[SMOOTH_SHADING]);
	glBufferData(GL_ARRAY_BUFFER, mGLData.smooth_normals.size() * sizeof(mGLData.smooth_normals.at(0)), mGLData.smooth_normals.data(), GL_STATIC_DRAW);
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
		return (GLsizei)mGLData.smooth_vertices.size();
	}
	else if (mCurrentRenderMethod == FLAT_SHADING) {
		return (GLsizei)mGLData.flat_vertices.size();
	}
	else {
		return (GLsizei)mGLData.smooth_vertices.size();
	}
}
void Mesh3D::draw() const {
	glBindVertexArray(mVertexArray[mCurrentRenderMethod]);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount());
}