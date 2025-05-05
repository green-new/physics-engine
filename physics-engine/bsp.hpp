#pragma once

#include "texture.hpp"

#include <glm/glm.hpp>
#include <vector>

struct LevelVertex {
	glm::vec3 position{};
	glm::vec3 texture{};
	glm::vec3 color{};
	glm::vec3 normal{};

	texture_t tex;

	LevelVertex() = default;
	LevelVertex(glm::vec3 position, glm::vec3 texture, glm::vec3 color, glm::vec3 normal, texture_t tex)
		: position(position), texture(texture), color(color), normal(normal), tex(tex) { }

	~LevelVertex() = default;
};

struct LevelGeometry {
	std::vector<LevelVertex> vertices;
};

struct BSPnode {
	BSPnode* front;
	BSPnode* back;
	int plane; // Which hyperplane was used for partitioning
	LevelGeometry geometry; // level geometry struct
};