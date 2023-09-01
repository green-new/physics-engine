#pragma once
#include <array>
#include "glm/glm.hpp"

struct BoundingBox {
	glm::vec3 min;
	glm::vec3 max;

	BoundingBox() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}
	BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}
	const BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : min(glm::vec3(minX, minY, minZ)), max(glm::vec3(maxX, maxY, maxZ)) {}

	BoundingBox& operator=(const BoundingBox& other) {
		min = other.min;
		max = other.max;
	}
	bool overlaps(const BoundingBox& other) const {
		return (max.x >= other.min.x && other.max.x >= min.x) ||
			(max.y >= other.min.y && other.max.y >= min.y) ||
			(max.z >= other.min.z && other.max.z >= min.z);
	}
	void offset(glm::vec3 offset) {
		min += offset;
		max += offset;
	}
	glm::vec3 center() const {
		return (max - min) / 2.0f;
	}
	std::array<BoundingBox, 8> octlets() const {
		glm::vec3 c = center();
		std::array<BoundingBox, 8> octlets{};
		for (unsigned char i = 0; i < 8; i++) {
			octlets[i].min.x = (i & 1) ? c.x : min.x;
			octlets[i].min.y = (i & 2) ? c.y : min.y;
			octlets[i].min.z = (i & 4) ? c.z : min.z;

			octlets[i].max.x = (i & 1) ? max.x : c.x;
			octlets[i].max.y = (i & 2) ? max.y : c.y;
			octlets[i].max.z = (i & 4) ? max.z : c.z;
		}

		return octlets;
	}

	float volume() const {
		return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
	}

	bool smallerOrAt(const BoundingBox& smallest) const {
		return volume() <= smallest.volume();
	}
};