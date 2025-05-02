#include "glm/glm.hpp"

struct Line {
	Line() = default;
	~Line() = default;

	const float distance();

	glm::vec3 m_a{};
	glm::vec3 m_b{};
};
