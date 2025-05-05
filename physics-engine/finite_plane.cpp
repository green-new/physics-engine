#include "finite_plane.hpp"
#include <glad/glad.h>

FinitePlane::FinitePlane() : m_points({
		-1, 0.0f, -1.0f,
		1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 1.0f
	}),
	m_indices({
		0, 1, 2, 3
	}) {

	// initialize vao
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// vbo
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_points), m_points.data(), GL_STATIC_DRAW);

	// ebo
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void FinitePlane::draw() {
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	glLineWidth(2);
	glDrawElements(GL_FILL, m_indices.size(), GL_UNSIGNED_INT, 0);
	glLineWidth(1);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
