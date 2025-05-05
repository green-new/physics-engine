#include "plane.hpp"

bool Plane::intersects(glm::vec3 a, glm::vec3 b) {
	float dot = glm::dot(b - a, m_normal);
	return dot != 0.0f;
}

bool Plane::intersects(const BoundingBox& _box, const glm::vec3 position) {
	const BoundingBox box = BoundingBox(_box, position);
	return intersects(glm::vec3(box.min), glm::vec3(box.min.x, box.min.y, box.max.x))
		|| intersects(glm::vec3(box.min), glm::vec3(box.min.x, box.max.y, box.min.z))
		|| intersects(glm::vec3(box.min), glm::vec3(box.max.x, box.min.y, box.min.z))
		|| intersects(glm::vec3(box.max), glm::vec3(box.min.x, box.max.y, box.max.z))
		|| intersects(glm::vec3(box.max), glm::vec3(box.max.x, box.min.y, box.max.z))
		|| intersects(glm::vec3(box.max), glm::vec3(box.max.x, box.min.y, box.min.z));
}
