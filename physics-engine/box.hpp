#pragma once
#include "glm/glm.hpp"

struct BoundingBox {
	glm::vec3 min;
	glm::vec3 max;

	BoundingBox() : min(glm::vec3(-1.0f)), max(glm::vec3(1.0f)) {}
	BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}
	BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : min(glm::vec3(minX, minY, minZ)), max(glm::vec3(maxX, maxY, maxZ)) {}

	BoundingBox& operator=(const BoundingBox& other) {
		min = other.min;
		max = other.max;
	}
	bool overlaps(const BoundingBox& other) {
		return (max.x >= other.min.x && other.max.x >= min.x) ||
			(max.y >= other.min.y && other.max.y >= min.y) ||
			(max.z >= other.min.z && other.max.z >= min.z);
	}
};