#include "glm/glm.hpp"
#include "box.hpp"
#include "types.hpp"
#include <array>

struct FinitePlane {

	std::array<GLfloat, 12> m_points{};
	std::array<GLuint, 6> m_indices{};

	VAO m_vao{};
	VBO m_vbo{};
	VBO m_ebo{};

	FinitePlane();

	~FinitePlane() = default;

	void draw();
};
