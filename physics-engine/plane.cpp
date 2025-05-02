#include "plane.hpp"

bool Plane::intersects(glm::vec3 pointA, glm::vec3 pointB) {
	// plane: 3x - 2y + z = 10 (example)
	// line: y = 1 + t
	return true;
}

bool Plane::intersects(const BoundingBox& box) {
	return true;
}
