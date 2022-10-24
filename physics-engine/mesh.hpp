#pragma once
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include "shader.hpp"
#include "texture.hpp"
#include "geometry.hpp"
#include "buffer_objects.hpp"

class Mesh3D {
public:
	Mesh3D(geolib::Geometry3D* g);
	~Mesh3D();
	void draw(Shader& prog);
private:
	geolib::adapter_GLdata m_data;
	vbo* m_positionBuffer;
	vbo* m_normalBuffer;
	vao* m_vertexArray;

	GLsizei vertex_count();
};