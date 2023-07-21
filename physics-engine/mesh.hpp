#pragma once
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include "shader.hpp"
#include "texture.hpp"
#include "geometry.hpp"
#include "types.hpp"

#define FLAT_SHADING 0
#define SMOOTH_SHADING 1

using namespace Geometry;

class Mesh3D {
public:
	Mesh3D(GLData glData);
	~Mesh3D();

	GLsizei vertexCount() const;
	void draw() const;
	void swapVaos();
private:
	GLData mGLData;
	size_t mCurrentRenderMethod;

	VBO mVertexData[2];
	VBO mNormalData[2];
	VBO mTextureData;
	VAO mVertexArray[2];
};