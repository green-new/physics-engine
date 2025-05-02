#pragma once
#include <array>
#include <vector>
#include "glm/glm.hpp"
#include <string>

struct BoundingBox {
	/**
	 * The lowest coordinate of this bounding box.
	 */
	glm::vec3 min{};
	/**
	 * The highest coordinate of this bounding box.
	 */
	glm::vec3 max{};
	/**
	 * The center coordinate of this bounding box.
	 */
	glm::vec3 center{};
	/**
	 * A interim variable that says this bounding box is currently overlapping with another object.
	 */
	bool overlapping{};

	BoundingBox() = default;
	BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max), center(0.0f), overlapping(false) {}
	BoundingBox(glm::vec3 min, glm::vec3 max, glm::vec3 center) : min(min), max(max), center(center), overlapping(false) {}
	BoundingBox(glm::vec3 min, glm::vec3 max, glm::vec3 center, glm::vec3 rotation) : min(min), max(max), center(center), overlapping(false) {}
	constexpr BoundingBox(float lenX, float lenY, float lenZ) : min(glm::vec3(-lenX / 2.0f, -lenY / 2.0f, -lenZ / 2.0f)), max(glm::vec3(lenX / 2.0f, lenY / 2.0f, lenZ / 2.0f)), center(0.0f), overlapping(false) {}
	constexpr BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : min(glm::vec3(minX, minY, minZ)), max(glm::vec3(maxX, maxY, maxZ)), center(0.0f), overlapping(false) {}
	constexpr BoundingBox(const BoundingBox& other, glm::vec3 worldPos) : min(other.min + worldPos), max(other.max + worldPos), center(worldPos), overlapping(false) {}
	BoundingBox& operator=(const BoundingBox& other);

	bool overlaps(const BoundingBox& other) const;
	glm::vec3 overlap(const BoundingBox& other) const;
	std::string toString();
};