#include "box.hpp"
#include <string>
#include <format>

BoundingBox& BoundingBox::operator=(const BoundingBox& other) {
	min = other.min;
	max = other.max;
	return *this;
}
bool BoundingBox::overlaps(const BoundingBox& other) const {
	return (max.x >= other.min.x && other.max.x >= min.x) &&
		(max.y >= other.min.y && other.max.y >= min.y) &&
		(max.z >= other.min.z && other.max.z >= min.z);
}

glm::vec3 BoundingBox::overlap(const BoundingBox& other) const {
	return glm::vec3(
		glm::max(0.0f, glm::min(min.x, other.min.x) - glm::max(max.x, other.max.x)),
		glm::max(0.0f, glm::min(min.y, other.min.y) - glm::max(max.y, other.max.y)),
		glm::max(0.0f, glm::min(min.z, other.min.z) - glm::max(max.z, other.max.z))
	);
}

std::string BoundingBox::toString() {
	return std::format("[BoundingBox] min: [{}, {}, {}] | max: [{}, {}, {}]", min.x, min.y, min.z, max.x, max.y, max.z);
}
