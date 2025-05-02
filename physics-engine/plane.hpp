#include "glm/glm.hpp"
#include "box.hpp"

struct Plane {
	glm::vec3 m_center{};
	glm::vec3 m_rotation{};


	Plane() = default;
	~Plane() = default;

	Plane(const Plane& plane) : m_center(plane.m_center), m_rotation(plane.m_rotation) {}

	bool intersects(glm::vec3 pointA, glm::vec3 pointB);
	bool intersects(const BoundingBox& box);
};
