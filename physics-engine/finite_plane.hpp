#include "glm/glm.hpp"
#include "box.hpp"
#include "types.hpp"
#include <array>

struct FinitePlane {

	std::array<GLfloat, 12> m_points{};
	std::array<GLfloat, 12> m_normals{};
	std::array<GLfloat, 8> m_textures{};
	std::array<GLuint, 6> m_indices{};

	VAO m_vao{};

	VBO m_vboPositions{};
	VBO m_vboNormals{};
	VBO m_vboTextures{};

	VBO m_ebo{};

	FinitePlane() : m_points({
			-1, 0.0f, -1.0f,
			1.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 1.0f,
			-1.0f, 0.0f, 1.0f
		}),
		m_normals({
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
		}),
		m_textures({
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		}),
		m_indices({
			0, 1, 2, 3
		}) {}

	~FinitePlane();

	void init();
	void draw();
};
