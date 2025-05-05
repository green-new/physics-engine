#include "glm/glm.hpp"
#include "box.hpp"

struct Plane {
	
	float dist;

	glm::vec3 m_normal{};

	Plane(float dist = 0.0f, glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f)) : dist(dist), m_normal(normal) {}

	Plane(const Plane& plane) : dist(plane.dist), m_normal(plane.m_normal) {}

	~Plane() = default;

	bool intersects(glm::vec3 pointA, glm::vec3 pointB);
	bool intersects(const BoundingBox& box, const glm::vec3 position);
};
