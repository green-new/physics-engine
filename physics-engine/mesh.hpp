#pragma once
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include "shader.hpp"
#include "texture.hpp"
#include "geometry.hpp"
#include "types.hpp"

using namespace Geometry;

class Mesh3D {
public:
	Mesh3D(GLData glData);
	~Mesh3D();

	GLsizei vertexCount() const;
	void draw() const;
private:
	GLData mGLData;

	VBO mVertexData;
	VBO mNormalData;
	VBO mTextureData;
	VAO mVertexArray;
};