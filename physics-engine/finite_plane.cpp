#include "finite_plane.hpp"
#include <glad/glad.h>

FinitePlane::~FinitePlane() {
	glDeleteBuffers(1, &m_vboPositions);
	glDeleteBuffers(1, &m_vboNormals);
	glDeleteVertexArrays(1, &m_vao);
}

void FinitePlane::init() {

	// initialize vao
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vboPositions);
	glGenBuffers(1, &m_vboNormals);
	glGenBuffers(1, &m_vboTextures);
	glGenBuffers(1, &m_ebo);

	// bind the vao
	glBindVertexArray(m_vao);

	// vbos
	glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_points), &m_points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_normals), &m_normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboTextures);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_textures), &m_textures, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
}

void FinitePlane::draw() {
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	glLineWidth(2);
	glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_INT, 0);
	glLineWidth(1);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
